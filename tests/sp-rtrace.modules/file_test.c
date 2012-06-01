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

static void do_exit(const char *name)
{
	perror(name);
	fputs("exiting...\n", stderr);
	exit(1);
}
static void assert_fp(const char *name, FILE *fp)
{
	fprintf(stderr, "%s -> %p\n", name, fp);
	if (!fp) {
		do_exit(name);
	}
}
static void assert_fd(const char *name, int fd)
{
	fprintf(stderr, "%s -> %d\n", name, fd);
	if (fd < 0) {
		do_exit(name);
	}
}

void test_fd(void)
{
	FILE* fp;
	int fd, fd2, fd3, fd4, fds[2];

	fd = creat(OUTPUT_FILENAME, 0666);
	assert_fd("creat", fd);
	close(fd);
	fd = open64(OUTPUT_FILENAME, O_RDONLY);
	assert_fd("open64", fd);
	close(fd);

	fd = open(OUTPUT_FILENAME, O_RDONLY);
	assert_fd("open", fd);
	fd2 = dup2(fd, 40);
	assert_fd("dup2", fd2);
	fd3 = dup3(fd, 60, O_CLOEXEC);
	assert_fd("dup3", fd3);
	fd4 = dup(fd2);
	assert_fd("dup", fd4);
	close(fd2);
	close(fd3);
	close(fd4);

	fd2 = fcntl(fd, F_DUPFD, 80);
	assert_fd("fcntl", fd2);
	close(fd);
	/* closing this stream closes also pointed FD */
	fp = fdopen(fd2, "r");
	assert_fp("fdopen", fp);
	fclose(fp);

	DIR *dir = opendir(".");
	fd = openat(dirfd(dir), OUTPUT_FILENAME, O_RDONLY);
	assert_fd("openat", fd);
	close(fd);
	closedir(dir);

	assert_fd("pipe", pipe(fds));
	close(fds[0]);
	close(fds[1]);

	assert_fd("pipe2", pipe2(fds, O_NONBLOCK));
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

	assert_fd("socketpair", socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds));
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
	assert_fd("inotify_init", fd1);
	fd2 = inotify_init1(IN_CLOEXEC);
	assert_fd("inotify_init1", fd2);
	close(fd2);
	close(fd1);

	fd1 = epoll_create(8);
	assert_fd("epoll_create", fd1);
	fd2 = epoll_create1(EPOLL_CLOEXEC);
	assert_fd("epoll_create1", fd2);
	close(fd1);
	close(fd2);

	sigemptyset(&mask);
	sigaddset(&mask, SIGCONT);
	fd1 = signalfd(-1, &mask, 0);
	assert_fd("signalfd", fd1);
	fd2 = timerfd_create(CLOCK_MONOTONIC, 0);
	assert_fd("timerfd_create", fd2);
	fd3 = eventfd(0, 0);
	assert_fd("eventfd", fd3);
	close(fd1);
	close(fd2);
	close(fd3);

	fd1 = getpt();
	assert_fd("getpt", fd1);
	fd2 = posix_openpt(O_RDWR | O_NOCTTY);
	assert_fd("posix_openpt", fd2);
	close(fd1);
	close(fd2);
}


void test_fp(void)
{
	FILE *fp1, *fp2;
	fp1 = fopen(OUTPUT_FILENAME, "r");
	assert_fp("fopen", fp1);
	fp2 = freopen(OUTPUT_FILENAME, "r+", stdin);
	assert_fp("freopen", fp2);
	fclose(fp1);
	fp1 = popen("/bin/echo", "r");
	assert_fp("popen", fp1);
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
