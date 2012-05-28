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
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/inotify.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <dirent.h>

#define OUTPUT_FILENAME  "file_out"
#define SOCKNAME "accept4-test"
#define MAXLISTENQUEUE 1


void test_fd(void)
{
	FILE* fp;
	int fd, fd2, fd3, fd4, fds[2];

	fd = creat(OUTPUT_FILENAME, 0666);
	close(fd);
	fd = open64(OUTPUT_FILENAME, O_RDONLY);
	close(fd);

	fd = open(OUTPUT_FILENAME, O_RDONLY);
	fd2 = dup2(fd, 40);
	fd3 = dup3(fd, 60, O_CLOEXEC);
	fd4 = dup(fd2);
	close(fd2);
	close(fd3);
	close(fd4);

	fd2 = fcntl(fd, F_DUPFD, 80);
	close(fd);
	/* closing this stream closes also pointed FD */
	fp = fdopen(fd2, "r");
	fclose(fp);

	DIR *dir = opendir(".");
	fd = openat(dirfd(dir), OUTPUT_FILENAME, O_RDONLY);
	close(fd);
	closedir(dir);

	pipe(fds);
	close(fds[0]);
	close(fds[1]);

	pipe2(fds, O_NONBLOCK);
	close(fds[0]);
	close(fds[1]);
}


static int serversocket(void)
{
	int sh, conn;
	socklen_t rlen;
	struct sockaddr_un addr;
	struct sockaddr_in raddr;

	if ((sh = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("server: socket()");
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_un));
	strncpy(addr.sun_path, SOCKNAME, sizeof(addr.sun_path) - 1);
	if (access(addr.sun_path, F_OK) == 0) {
		fprintf(stderr, "server: socket '%s' already exists, removing...\n", addr.sun_path);
		if (unlink(addr.sun_path) < 0) {
			perror("server: unlink()");
			return -1;
		}
	}

	addr.sun_family = AF_UNIX;
	if (bind(sh, &addr, sizeof(struct sockaddr_un))) {
		perror("server: bind() for " SOCKNAME);
		close(sh);
		return -1;
	}

	if (listen(sh, MAXLISTENQUEUE)) {
		perror("server: listen()");
		close(sh);
		return -1;
	}

	raddr.sin_addr.s_addr = 0;
	rlen = sizeof(struct sockaddr);
	if ((conn = accept4(sh, &raddr, &rlen, SOCK_CLOEXEC)) < 0) {
		perror("server: accept()");
		return -1;
	}
	close(sh);
	if (unlink(addr.sun_path) < 0) {
		perror("server: unlink()");
	}
	return conn;
}


static int clientsocket(void)
{
	int sh;
	struct sockaddr_un addr;
	
	if ((sh = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("client: socket()");
		return -1;
	}
	memset(&addr, 0, sizeof(struct sockaddr_un));
	strncpy(addr.sun_path, SOCKNAME, sizeof(addr.sun_path) - 1);
	addr.sun_family = AF_UNIX;
	if (connect(sh, &addr, sizeof(struct sockaddr_un))) {
		perror("client: connect() for " SOCKNAME);
		return -1;
	}
	return sh;
}

void test_socket(void)
{
	int sock;
	pid_t pid;
	int sockfds[2];

	socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds);
	close(sockfds[0]);
	close(sockfds[1]);

	pid = fork();
	if (pid < 0) {
		perror("fork");
		return;
	}
	if (!pid) {
		/* child process */
		sleep(1);
		sock = clientsocket();
		if (sock < 0) {
			fprintf(stderr, "ERROR: client failed to get connection!\n");
		} else {
			printf("SUCCESS: client got connection: %d!\n", sock);
			close(sock);
		}
		exit(0);
	}
	/* parent */
	sock = serversocket();
	if (sock < 0) {
		fprintf(stderr, "ERROR: server failed to get connection!\n");
	} else {
		printf("SUCCESS: server got connection: %d!\n", sock);
		close(sock);
	}
}

void test_fd_special(void)
{
	sigset_t mask;
	int fd1, fd2, fd3;

	fd1 = inotify_init();
	fd2 = inotify_init1(IN_CLOEXEC);
	close(fd2);
	close(fd1);

	fd1 = epoll_create(8);
	fd2 = epoll_create1(EPOLL_CLOEXEC);
	close(fd1);
	close(fd2);

	sigemptyset(&mask);
	sigaddset(&mask, SIGCONT);
	fd1 = signalfd(-1, &mask, 0);
	fd2 = timerfd_create(CLOCK_MONOTONIC, 0);
	fd3 = eventfd(0, 0);
	close(fd1);
	close(fd2);
	close(fd3);

	fd1 = getpt();
	fd2 = posix_openpt(0);
	close(fd1);
	close(fd2);
}


void test_fp(void)
{
	FILE *fp1, *fp2;
	fp1 = fopen(OUTPUT_FILENAME, "r");
	fp2 = freopen(OUTPUT_FILENAME, "w+", fp1);
	fclose(fp1);
	fp1 = popen("echo", "re");
	pclose(fp1);
	fcloseall();
}

int main()
{
	test_fd();
	test_socket();
	test_fd_special();
	test_fp();
	
	sleep (1);
	unlink(OUTPUT_FILENAME);
	return 0;
}
