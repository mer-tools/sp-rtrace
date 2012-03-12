/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
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

/**
 * @file file_test.c
 *
 * Test application for file descriptor tracking module (file) coverage.
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/inotify.h>

#define OUTPUT_FILENAME  "./file_out"

void test_fd()
{
	int fd = creat(OUTPUT_FILENAME, 0666);
	close(fd);
	fd = open64(OUTPUT_FILENAME, O_RDONLY);
	close(fd);
	fd = open(OUTPUT_FILENAME, O_RDONLY);
	int fd2 = dup(fd);
	int fd3 = fcntl(fd, F_DUPFD, 10);
	close(fd3);
	dup2(fd2, fd2);
	close(fd);
	
	FILE* fp = fdopen(fd2, "r");
	fclose(fp);

	int fds[2];
	pipe(fds);
	close(fds[0]);
	close(fds[1]);

	pipe2(fds, O_NONBLOCK);
	close(fds[0]);
	close(fds[1]);
}

void test_socket()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	close(sockfd);
	
	int sockfds[2];
	socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds);
	close(sockfds[0]);
	close(sockfds[1]);
}

void test_inotify()
{
	int fd = inotify_init();
	close(fd);
}


void test_fp()
{
	FILE* fp = fopen(OUTPUT_FILENAME, "r");
	FILE* fp2 = freopen(OUTPUT_FILENAME, "w+", fp);
	fcloseall();
}

int main()
{
	test_fd();
	test_socket();
	test_inotify();
	test_fp();
	
	sleep (1);
	return 0;
}
