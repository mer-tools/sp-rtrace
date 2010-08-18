version=1.0, arch=i486-linux-gnu, time=2010.8.13 16:14:40, process=, pid=22223, 
: /usr/lib/sp-rtrace/libsp-rtrace-memory.so.1.0.0 => 0x110000-0x112000
: /lib/tls/i686/cmov/libc-2.11.1.so => 0x214000-0x367000
: /lib/libgcc_s.so.1 => 0x415000-0x432000
: /lib/tls/i686/cmov/libdl-2.11.1.so => 0x58a000-0x58c000
: /lib/tls/i686/cmov/libpthread-2.11.1.so => 0x7db000-0x7f0000
: /lib/ld-2.11.1.so => 0xb5a000-0xb75000
: /lib/tls/i686/cmov/librt-2.11.1.so => 0xc1c000-0xc23000
: /usr/lib/sp-rtrace/libsp-rtrace-main.so.1.0.0 => 0xe6e000-0xe73000
: /lib/tls/i686/cmov/libm-2.11.1.so => 0xf55000-0xf79000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/bin/alloc_test => 0x8048000-0x8049000
# heap status information:
#   heap bottom 0x9d8b000
#   heap top 0x9dac000
#   lowest block 0x9d8b008
#   highest block 0x9d92210
#   non-mmapped space allocated from system 135168
#   number of free chunks 1
#   number of fastbin blocks 0
#   number of mmapped regions 0
#   space in mmapped regions 0
#   maximum total allocated space 0
#   space available in freed fastbin blocks 0
#   total allocated space 30200
#   total free space 104968
#   top-most, releasable (via malloc_trim) space 104968
# tracing module: main (1.0)
# tracing module: memory (1.0)
1. [09:02:04.760] malloc(1003) = 0x9d8b008
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
13. [09:02:04.760] malloc(1003) = 0x9d8c390
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
25. [09:02:04.760] malloc(1003) = 0x9d8d350
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
37. [09:02:04.760] malloc(1003) = 0x9d8e310
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
49. [09:02:04.760] malloc(1003) = 0x9d8f2d0
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
2. [09:02:04.760] malloc(1002) = 0x9d8b7c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
14. [09:02:04.760] malloc(1002) = 0x9d8c780
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
26. [09:02:04.760] malloc(1002) = 0x9d8d740
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
38. [09:02:04.760] malloc(1002) = 0x9d8e700
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
50. [09:02:04.760] malloc(1002) = 0x9d8f6c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
3. [09:02:04.760] malloc(1001) = 0x9d8bbb0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
15. [09:02:04.760] malloc(1001) = 0x9d8cb70
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
27. [09:02:04.760] malloc(1001) = 0x9d8db30
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
39. [09:02:04.760] malloc(1001) = 0x9d8eaf0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
51. [09:02:04.760] malloc(1001) = 0x9d8fab0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
4. [09:02:04.760] malloc(1000) = 0x9d8bfa0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
16. [09:02:04.760] malloc(1000) = 0x9d8cf60
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
28. [09:02:04.760] malloc(1000) = 0x9d8df20
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
40. [09:02:04.760] malloc(1000) = 0x9d8eee0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
52. [09:02:04.761] malloc(1000) = 0x9d8fea0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
61. [09:02:04.761] malloc(1002) = 0x9d90290
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
70. [09:02:04.761] malloc(1002) = 0x9d90e60
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
79. [09:02:04.761] malloc(1002) = 0x9d91a30
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
62. [09:02:04.761] malloc(1001) = 0x9d90680
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
71. [09:02:04.761] malloc(1001) = 0x9d91250
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
80. [09:02:04.761] malloc(1001) = 0x9d91e20
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
63. [09:02:04.761] malloc(1000) = 0x9d90a70
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
72. [09:02:04.761] malloc(1000) = 0x9d91640
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
81. [09:02:04.761] malloc(1000) = 0x9d92210
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
