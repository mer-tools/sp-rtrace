/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010-2012 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <malloc.h>

#include "listener.h"
#include "rtrace_env.h"
#include "sp_rtrace.h"

#include "common/dlist.h"
#include "common/rtrace_data.h"
#include "common/utils.h"
#include "common/sp_rtrace_proto.h"
#include "common/debug_log.h"
#include "common/msg.h"

/* the read buffer size */
#define BUFFER_SIZE			4096


int fd_out = 0;
int fd_in = 0;

/* the output buffer */
static char output_buffer[BUFFER_SIZE * 2];

/* the output buffer head */
static char* output_buffer_head = output_buffer;

/* the handshake packet buffer */
static char hs_buffer[256];
static int hs_size = 0;

/**
 * Flushes the output buffer.
 *
 * @return
 */
static int flush_data(void)
{
	int size = output_buffer_head - output_buffer;
	if (fd_out > 0 && size) {
		if (write(fd_out, output_buffer, size) < 0) {
			msg_error("failed to write to file/post-processor pipe (%s)\n",
					strerror(errno));
			return -1;
		}
	}
	output_buffer_head = output_buffer;
	return size;
}

/**
 * Writes the data.
 *
 * The data is buffered internally unless the --disable-event-buffering option is
 * specified.
 * @param[in] data    the data to write.
 * @param[in] size    the number of bytes to write.
 * @return            the number of bytes written.
 */
static int write_data(const char* data, int size)
{
	/* write directly to the output stream if the event buffering is disabled */
	if (rtrace_options.disable_packet_buffering) {
		return write(fd_out, data, size);
	}
	/* write data to buffer and flush it if necessary */
	memcpy(output_buffer_head, data, size);
	output_buffer_head += size;
	if (output_buffer_head - output_buffer >= BUFFER_SIZE) {
		if (flush_data() < 0) return -1;
	}
	return size;
}
/*
 * Memory mapping scanning support
 */
/**
 * Internal memory mapping record cache.
 */
dlist_t s_mmaps;

#if 0 /* unused for now */
/**
 * Compares memory mapping cache record with module name.
 *
 * @param[in] mmap  the cache record.
 * @param[in] name  the module name.
 * @return          0 - equal.
 */
static long mmap_compare(rd_mmap_t* mmap, const char* name)
{
	return strcmp(mmap->data.module, name);
}
#endif

static long range_compare(rd_mmap_t* mmap1, sp_rtrace_mmap_t* mmap2)
{
	if (mmap1->data.to < mmap2->from) return -1;
	if (mmap1->data.from > mmap2->to) return 1;
	return 0;
}

/**
 * Parse maps file and write memory map packets into output stream.
 *
 * @param[in] pid   the target process identifier.
 * @return
 */
static int scan_mmap_data(void)
{
	char name[PATH_MAX], buffer[PATH_MAX];
	sprintf(name, "/proc/%d/maps", rtrace_options.pid);
	FILE* fp = fopen(name, "r");
	if (fp) {
		while (fgets(name, PATH_MAX, fp)) {
			pointer_t from, to;
			char rights[8];
			if (sscanf(name, "%lx-%lx %s %[^ ] %[^ ] %[^ ] %[^ ]", &from, &to, rights, buffer, buffer, buffer, buffer) == 7 && rights[2] == 'x') {
				if (*buffer) buffer[strlen(buffer) - 1] = '\0';
				/* check if the memory mapping is not already registered */
				sp_rtrace_mmap_t map = {
						.module = buffer,
						.from = from,
						.to = to,
				};
				rd_mmap_t* mmap = (rd_mmap_t*)dlist_find(&s_mmaps, (void*)&map, (op_binary_t)range_compare);
				if (mmap) {
					/* if the addresses are the same - skip */
					if (!strcmp(mmap->data.module, buffer) && mmap->data.from == from && mmap->data.to == to) continue;
					/* update mapping record */
					dlist_remove(&s_mmaps, mmap);
					rd_mmap_free(mmap);
				}
				mmap = dlist_create_node(sizeof(rd_mmap_t));
				mmap->data.from = from;
				mmap->data.to = to;
				mmap->data.module = strdup_a(buffer);
				dlist_add(&s_mmaps, mmap);

				/* assemble and write MM packet */
				char* ptr = name + write_dword(name, SP_RTRACE_PROTO_MEMORY_MAP);
				ptr += SP_RTRACE_PROTO_TYPE_SIZE;
				ptr += write_pointer(ptr, from);
				ptr += write_pointer(ptr, to);
				ptr += write_string(ptr, buffer);
				int size = ptr - name;
				write_dword(name + SP_RTRACE_PROTO_TYPE_SIZE, size - SP_RTRACE_PROTO_TYPE_SIZE - SP_RTRACE_PROTO_LENGTH_SIZE);
				/* write the assembled packet to the output stream */
				if (write_data(name, size) < 0) {
					fclose(fp);
					return -1;
				}
			}
		}
		fclose(fp);
	}
	return 0;
}

/**
 * Processes handshake packet.
 *
 * @param[in] data   the binary data stream.
 * @param[in] size   the size of binary data stream.
 * @return           the number of bytes processed.
 */
static int process_handshake(const char* data, int size)
{
	/* read and process the handshake packet */
	int len = (unsigned char)*(data + 1) + 2;

	LOG("size=%d, len=%d", size, len);

	/* A handshake packet fragmentation is a sign of error,
	 * as the handshake packet size is less than 256 bytes
	 * and it's the first packet written into pipe. So in
	 * theory it can't be fragmented.
	 */
	if (len > size) return -1;

	memcpy(hs_buffer, data, len);
	hs_size = len;

	return len;
}
#include "common/sp_rtrace_proto.h"

/**
 * Processes generic packet.
 *
 * @param[in] data   the binary data stream.
 * @param[in] size   the size of binary data stream.
 * @return           the number of bytes processed.
 */
static int process_packet(const char* data, size_t size)
{
	unsigned int len, type;
	unsigned int offset;

	if (size < SP_RTRACE_PROTO_LENGTH_SIZE + SP_RTRACE_PROTO_TYPE_SIZE) {
		return -1;
	}

	/* read the type packet */
	offset = read_dword(data, &type);
	/* check if the buffer has at least one full packet */
	offset += read_dword(data + offset, &len);
	len += offset;

	//LOG("type=%c%c, size=%d", *(char*)&type, *((char*)&type + 1), len - offset);

	if (len > size) {
		return -1;
	}

	if (type == SP_RTRACE_PROTO_OUTPUT_SETTINGS) {
		char value[PATH_MAX];
		offset += read_string(data + offset, value, PATH_MAX);
		if (*value) {
			if (rtrace_options.output_dir) free(rtrace_options.output_dir);
			rtrace_options.output_dir = strdup_a(value);
		}
		offset += read_string(data + offset, value, PATH_MAX);
		if (*value) {
			if (rtrace_options.postproc) free(rtrace_options.postproc);
			rtrace_options.postproc = strdup_a(value);
		}

		/* output settings updated, now the output stream can be initialized */
		rtrace_connect_output();
		/* write cached handshake packet after output stream has been initialized */
		if (write_data(hs_buffer, hs_size) < 0) return -1;
	}
	else if (type == SP_RTRACE_PROTO_PROCESS_INFO) {

		/* scan the timestamp value and set if it's zero */
		int secs;
		read_dword(data + offset + 4, (unsigned int*)&secs);
		if (!secs) {
			struct timeval tv;
			gettimeofday(&tv, NULL);
			write_dword((char*)data + offset + 4, tv.tv_sec);
			write_dword((char*)data + offset + 8, tv.tv_usec);
		}
		/* store target process pid. It will be needed to locate maps file */
		read_dword(data + offset, (unsigned int*)&rtrace_options.pid);
	}
	else if (type == SP_RTRACE_PROTO_NEW_LIBRARY) {
		/* NL packet is not forwarded further to  post-processor.
		 * Just scan the maps data */
		char path[PATH_MAX];
		read_string(data + offset, path, sizeof(path));
		scan_mmap_data();
		return len;
	}
	else if (type == SP_RTRACE_PROTO_ATTACHMENT) {
		/* check for zero size attachments */
		char path[PATH_MAX];
		char* out = path;
		offset += read_string(data + offset, path, sizeof(path));
		read_string(data + offset, path, sizeof(path));
		if (*path != '/' && rtrace_options.output_dir) {
			out = stpcpy(path, rtrace_options.output_dir);
			*out++ = '/';
			read_string(data + offset, out, sizeof(path) - strlen(rtrace_options.output_dir) - 1);
		}
		struct stat file_stat;
		if (stat(path, &file_stat) == -1) {
			msg_warning("failed to stat attached file %s: %s\n", path, strerror(errno));
		}
		else {
			if (file_stat.st_size == 0) {
				msg_warning("the attached file %s has zero size\n", path);
			}
		}
	}
	write_data(data, len);
	return len;
}

/*
 * Public API
 */

int process_data()
{
	char buffer[BUFFER_SIZE * 2], *ptr_in = buffer;
	int n, offset, size;

	dlist_init(&s_mmaps);
	/* read and process the handshake packet */
	n = read(fd_in, buffer, BUFFER_SIZE - 1);
	if (n < 1) {
	    msg_error("failed to read data from pipe\n");
	    return -1;
	}
	offset = process_handshake(ptr_in, n);
	if (offset <= 0) {
		msg_error("handshaking packet processing failed\n");
		return -1;
	}
	n -= offset;
	ptr_in += offset;

	/* main packet reading/processing loop */
	while (true) {
		/* read packets from the buffer */
		while (true) {
			size = process_packet(ptr_in, n);
			if (size <= 0) break;
			ptr_in += size;
			n -= size;
		}

		/* move the incomplete packet to the beginning of buffer */
		memmove(buffer, ptr_in, n);
		/* read new data chunk into buffer */
		int nbytes = read(fd_in, buffer + n, BUFFER_SIZE);
		if (nbytes == 0) {
			break;
		}
		if (rtrace_stop_requests >= REQUEST_STOP) {
			msg_warning("trace was forced to abort before all of data was retrieved.\n");
			break;
		}
		if (nbytes == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				break;
			}
		}
		n += nbytes;
		ptr_in = buffer;
	}
	flush_data();
	dlist_free(&s_mmaps, (op_unary_t)rd_mmap_free);
	return 0;
}

