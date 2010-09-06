version=1.0, arch=i486-linux-gnu, time=2010.9.6 13:08:43, process=../bin/alloc_test, pid=8482, 
<1> : memory (memory allocation in bytes)
: /lib/tls/i686/cmov/libc-2.11.1.so => 0x110000-0x263000
: /lib/tls/i686/cmov/libdl-2.11.1.so => 0x26a000-0x26c000
: /usr/lib/sp-rtrace/libsp-rtrace-memory.so.1.0.0 => 0x355000-0x357000
: /lib/ld-2.11.1.so => 0x51e000-0x539000
: /lib/tls/i686/cmov/libm-2.11.1.so => 0x56f000-0x593000
: /usr/lib/sp-rtrace/libsp-rtrace-main.so.1.0.0 => 0x708000-0x70d000
: /lib/tls/i686/cmov/libpthread-2.11.1.so => 0x730000-0x745000
: /lib/libgcc_s.so.1 => 0xb60000-0xb7d000
: /lib/tls/i686/cmov/librt-2.11.1.so => 0xdf9000-0xe00000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test => 0x8048000-0x8049000
# heap status information:
#   heap bottom 0x9a5e000
#   heap top 0x9a7f000
#   lowest block 0x9a5e008
#   highest block 0x9a65600
#   non-mapped space allocated from system 135168
#   number of free chunks 1
#   number of fastbin blocks 0
#   number of mapped regions 0
#   space in mapped regions 0
#   maximum total allocated space 0
#   space available in freed fastbin blocks 0
#   total allocated space 30200
#   total free space 104968
#   top-most, releasable (via malloc_trim) space 104968
# tracing module: [0] main (1.0)
# tracing module: [1] memory (1.0)
1. [11:49:17.431] malloc(1003) = 0x9a5e008
	0x8048570 (in three at alloc_test.c:62)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
13. [11:49:17.431] malloc(1003) = 0x9a5f390
	0x8048570 (in three at alloc_test.c:62)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
25. [11:49:17.431] malloc(1003) = 0x9a60350
	0x8048570 (in three at alloc_test.c:62)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
37. [11:49:17.431] malloc(1003) = 0x9a61310
	0x8048570 (in three at alloc_test.c:62)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
49. [11:49:17.432] malloc(1003) = 0x9a622d0
	0x8048570 (in three at alloc_test.c:62)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
2. [11:49:17.431] malloc(1002) = 0x9a5e7c0
	0x8048533 (in two at alloc_test.c:53)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
14. [11:49:17.431] malloc(1002) = 0x9a5f780
	0x8048533 (in two at alloc_test.c:53)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
26. [11:49:17.431] malloc(1002) = 0x9a60740
	0x8048533 (in two at alloc_test.c:53)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
38. [11:49:17.431] malloc(1002) = 0x9a61700
	0x8048533 (in two at alloc_test.c:53)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
50. [11:49:17.432] malloc(1002) = 0x9a626c0
	0x8048533 (in two at alloc_test.c:53)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
3. [11:49:17.431] malloc(1001) = 0x9a5ebb0
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
15. [11:49:17.431] malloc(1001) = 0x9a5fb70
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
27. [11:49:17.431] malloc(1001) = 0x9a60b30
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
39. [11:49:17.431] malloc(1001) = 0x9a61af0
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
51. [11:49:17.432] malloc(1001) = 0x9a62ab0
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
4. [11:49:17.431] malloc(1000) = 0x9a5efa0
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
16. [11:49:17.431] malloc(1000) = 0x9a5ff60
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
28. [11:49:17.431] malloc(1000) = 0x9a60f20
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
40. [11:49:17.432] malloc(1000) = 0x9a61ee0
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
52. [11:49:17.432] malloc(1000) = 0x9a62ea0
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x8048585 (in three at alloc_test.c:65)
	0x80485af (in start at alloc_test.c:73)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
61. [11:49:17.432] malloc(1002) = 0x9a63290
	0x8048533 (in two at alloc_test.c:53)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
70. [11:49:17.432] malloc(1002) = 0x9a63e60
	0x8048533 (in two at alloc_test.c:53)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
79. [11:49:17.432] malloc(1002) = 0x9a64a30
	0x8048533 (in two at alloc_test.c:53)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
62. [11:49:17.432] malloc(1001) = 0x9a63680
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
71. [11:49:17.432] malloc(1001) = 0x9a64250
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
80. [11:49:17.432] malloc(1001) = 0x9a64e20
	0x80484f6 (in one at alloc_test.c:44)
	0x8048548 (in two at alloc_test.c:56)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
63. [11:49:17.432] malloc(1000) = 0x9a63a70
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
72. [11:49:17.432] malloc(1000) = 0x9a64640
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
81. [11:49:17.432] malloc(1000) = 0x9a65210
	0x80484c6 (in zero at alloc_test.c:36)
	0x804850b (in one at alloc_test.c:47)
	0x8048548 (in two at alloc_test.c:56)
	0x80485cf (in start at alloc_test.c:76)
	0x80485f1 (in main at alloc_test.c:83)
	0x126bd6 (in __libc_start_main at libc-start.c:258)
	0x8048421 (in _start)
