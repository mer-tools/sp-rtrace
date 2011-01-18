/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02r10-1301 USA
 */

#ifndef SP_RTRACE_PROTO_H
#define SP_RTRACE_PROTO_H

/**
 * @file sp_rtrace_proto.h
 *
 * Resource trace binary protocol definitions and helper functions.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "common/debug_log.h"
#include "common/sp_rtrace_defs.h"

#define SP_RTRACE_PROTO_PACKET_TYPE(b1, b2, b3, b4) (b1 | (b2 << 8) | (b3 << 16) | (b4 << 24))

/* packet types */
#define SP_RTRACE_PROTO_MODULE_INFO        SP_RTRACE_PROTO_PACKET_TYPE('M', 'I', 'N', 'F')
#define SP_RTRACE_PROTO_MEMORY_MAP         SP_RTRACE_PROTO_PACKET_TYPE('M', 'M', 'A', 'P')
#define SP_RTRACE_PROTO_CONTEXT_REGISTRY   SP_RTRACE_PROTO_PACKET_TYPE('C', 'T', 'X', 'R')
#define SP_RTRACE_PROTO_FUNCTION_CALL      SP_RTRACE_PROTO_PACKET_TYPE('C', 'A', 'L', 'L')
#define SP_RTRACE_PROTO_BACKTRACE          SP_RTRACE_PROTO_PACKET_TYPE('B', 'T', 'R', 'C')
#define SP_RTRACE_PROTO_FUNCTION_ARGS      SP_RTRACE_PROTO_PACKET_TYPE('A', 'R', 'G', 'S')
#define SP_RTRACE_PROTO_PROCESS_INFO       SP_RTRACE_PROTO_PACKET_TYPE('P', 'I', 'N', 'F')
#define SP_RTRACE_PROTO_NEW_LIBRARY        SP_RTRACE_PROTO_PACKET_TYPE('N', 'L', 'I', 'B')
#define SP_RTRACE_PROTO_HEAP_INFO          SP_RTRACE_PROTO_PACKET_TYPE('H', 'I', 'N', 'F')
#define SP_RTRACE_PROTO_OUTPUT_SETTINGS    SP_RTRACE_PROTO_PACKET_TYPE('O', 'C', 'F', 'G')
#define SP_RTRACE_PROTO_RESOURCE_REGISTRY  SP_RTRACE_PROTO_PACKET_TYPE('R', 'E', 'S', 'R')
#define SP_RTRACE_PROTO_ATTACHMENT         SP_RTRACE_PROTO_PACKET_TYPE('F', 'I', 'L', 'E')

/* protocol version */
#define SP_RTRACE_PROTO_VERSION_MAJOR     1
#define SP_RTRACE_PROTO_VERSION_MINOR     4

/* endianness flags (used in HS packet) */
#define SP_RTRACE_PROTO_HS_LITTLE_ENDIAN  0
#define SP_RTRACE_PROTO_HS_BIG_ENDIAN     1

/* The binary protocol identification magic byte. All files starting with
 * This byte is treated by post-processor as binary files. */
#define SP_RTRACE_PROTO_HS_ID			0xF0
/*
 *  Protocol parsing helpers.
 *
 *  Data reading functions.
 */

/* data alignment for binary packages */
#define SP_RTRACE_PROTO_ALIGN	4
/* adjust size to be aligned according to the binary packet alignment */
#define SP_RTRACE_PROTO_ALIGN_SIZE(size) 	size += (SP_RTRACE_PROTO_ALIGN - (size & (SP_RTRACE_PROTO_ALIGN - 1))) & (SP_RTRACE_PROTO_ALIGN - 1)
/* size of the packet type field */
#define SP_RTRACE_PROTO_TYPE_SIZE		 4
/* size of the packet length field */
#define SP_RTRACE_PROTO_LENGTH_SIZE      4

/* optional alignment check for protocol data write/read helpers */
#if defined(ALIGN_CHECK) && defined(DEBUG_INFO)
 #define SP_RTRACE_PROTO_CHECK_ALIGNMENT(value) \
		if ((unsigned long)value & (SP_RTRACE_PROTO_ALIGN - 1)) { \
			LOG("WARNING, alignment check failed on %p\n", (void*)value);\
		}
#else
 #define SP_RTRACE_PROTO_CHECK_ALIGNMENT(value)
#endif

/**
 * Reads byte from binary stream.
 *
 * @param[in] ptr     the binary stream.
 * @param[out] value  the output value.
 * @return            the number of bytes read.
 */
static inline int read_byte(const char* ptr, unsigned char* value)
{
	*value = *ptr;
	return sizeof(char);
}


/**
 * Reads double word from binary stream.
 *
 * @param[in] ptr     the binary stream.
 * @param[out] value  the output value.
 * @return            the number of bytes read.
 */
static inline int read_dword(const char* ptr, unsigned int* value)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	*value = *(int*)ptr;
	return sizeof(int);
}


/**
 * Reads word from binary stream.
 *
 * This function is used only internally, by read_string* functions.
 * @param[in] ptr     the binary stream.
 * @param[out] value  the output value.
 * @return            the number of bytes read.
 */
static inline int read_word(const char* ptr, unsigned short* value)
{
	*value = *(unsigned short*)ptr;
	return sizeof(short);
}


/**
 * Reads pointer value from binary stream.
 *
 * @param[in] ptr     the binary stream.
 * @param[out] value  the output value.
 * @return            the number of bytes read.
 */
static inline int read_pointer(const char* ptr, pointer_t* value)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	*value = *(pointer_t*)ptr;
	return sizeof(pointer_t);
}


/**
 * Reads string from binary stream.
 *
 * @param[in] ptr     the binary stream.
 * @param[out] value  the output buffer.
 * @param[in] size    the output buffer size.
 * @return            the number of bytes read or -EINVAL if the
 *                    string size exceeds the output buffer size.
 */
static inline int read_string(const char* ptr, char* value, int size)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	unsigned short len = 0;
	ptr += read_word(ptr, &len);
	if (len >= size) return -EINVAL;
	value[len] = '\0';
	if (len) {
		memcpy(value, ptr, len);
		return len + sizeof(short);
	}
	return SP_RTRACE_PROTO_TYPE_SIZE;
}

/**
 * Allocates buffer and reads string from binary stream into it.
 *
 * @param[in] ptr     the binary stream.
 * @param[out] value  the output buffer.
 * @return            the number of bytes read or -EINVAL if the
 *                    string size exceeds the output buffer size.
 */
static inline int read_stringa(const char* ptr, char** value)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	unsigned short len = 0;
	ptr += read_word(ptr, &len);
	*value = malloc(len + 1);
	if (!*value) {
		fprintf(stderr, "ERROR: read_stringa: failed to allocate %d bytes of memory\n", len + 1);
		exit (-1);
	}
	(*value)[len] = '\0';
	if (len) {
		memcpy(*value, ptr, len);
		return len + sizeof(short);
	}
	return SP_RTRACE_PROTO_TYPE_SIZE;
}


/**
 * Reads data chunk from binary stream.
 *
 * @param[in] ptr       the binary stream.
 * @param[out] buffer   the output buffer.
 * @param[in] size      the number of bytes to read.
 * @return
 */
static inline int read_chunk(const char* ptr, char* buffer, int size)
{
	memcpy(buffer, ptr, size);
	return size;
}

/*
 * Data writing functions.
 *
 * As data writing functions are used from tracing modules,
 * they should avoid using standard functions.
 */

/**
 * Writes byte value into binary stream.
 *
 * @param[out] ptr   the binary stream.
 * @param[in] value  the value to write.
 * @return           the number of bytes written.
 */
static inline size_t write_byte(char* ptr, unsigned char value)
{
	*ptr = value;
	return sizeof(char);
}


/**
 * Writes double word value into binary stream.
 *
 * @param[out] ptr   the binary stream.
 * @param[in] value  the value to write.
 * @return           the number of bytes written.
 */
static inline size_t write_dword(char* ptr, unsigned int value)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	*(int*)ptr = value;
	return sizeof(int);
}

/**
 * Writes pointer value into binary stream.
 *
 * @param[out] ptr   the binary stream.
 * @param[in] value  the value to write.
 * @return           the number of bytes written.
 */
static inline size_t write_pointer(char* ptr, pointer_t value)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	*(pointer_t*)ptr = value;
	return sizeof(pointer_t);
}


/**
 * Writes string value into binary stream.
 *
 * @param[out] ptr   the binary stream.
 * @param[in] value  the value to write.
 * @return           the number of bytes written.
 */
static size_t write_string(char* ptr, const char* str)
{
	SP_RTRACE_PROTO_CHECK_ALIGNMENT(ptr);
	if (str) {
		short* psize = (short*)ptr;
		ptr += 2;
		while (*str) {
			*ptr++ = *str++;
		}
		int size = ptr - (char*)psize;
		/* pad the string with '\0' to ensure alignment of the whole package */
		while (size & (SP_RTRACE_PROTO_ALIGN - 1)) {
			size++;
			*ptr++ = '\0';
		}
		*psize = size - 2;
		return size;
	}
	else {
		return write_dword(ptr, 0);
	}
}

/* module types */
enum {
	MODULE_TYPE_UNDEFINED,
	MODULE_TYPE_PRELOAD,
	MODULE_TYPE_AUDIT,
};

/**
 * Module information data.
 */
typedef struct {
	int type;
	int version_major;
	int version_minor;
	const char* name;
	const char* description;
} sp_rtrace_module_info_t;

/* Module information retrieval function template. This function
 * must be implemented by all tracing submodules */
typedef sp_rtrace_module_info_t* (*sp_rtrace_get_module_info_t)();



#endif
