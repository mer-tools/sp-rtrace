version=1.0, arch=i486-linux-gnu, time=2010.8.23 15:30:40, process=, pid=19038, 
& 1 : memory
: /lib/tls/i686/cmov/libpthread-2.11.1.so => 0x110000-0x125000
: /lib/tls/i686/cmov/libc-2.11.1.so => 0x1eb000-0x33e000
: /usr/lib/sp-rtrace/libsp-rtrace-main.so.1.0.0 => 0x382000-0x387000
: /lib/ld-2.11.1.so => 0x515000-0x530000
: /usr/lib/sp-rtrace/libsp-rtrace-memory.so.1.0.0 => 0x9c2000-0x9c4000
: /lib/libgcc_s.so.1 => 0xb85000-0xba2000
: /lib/tls/i686/cmov/libdl-2.11.1.so => 0xbfb000-0xbfd000
: /lib/tls/i686/cmov/librt-2.11.1.so => 0xc0f000-0xc16000
: /lib/tls/i686/cmov/libm-2.11.1.so => 0xc6c000-0xc90000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/bin/alloc_test => 0x8048000-0x8049000
# heap status information:
#   heap bottom 0x8138000
#   heap top 0x8159000
#   lowest block 0x8138008
#   highest block 0x813f600
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
1. [04:21:59.903] malloc(1003) = 0x8138008
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
13. [04:21:59.904] malloc(1003) = 0x8139390
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
25. [04:21:59.905] malloc(1003) = 0x813a350
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
37. [04:21:59.905] malloc(1003) = 0x813b310
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
49. [04:21:59.905] malloc(1003) = 0x813c2d0
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
2. [04:21:59.903] malloc(1002) = 0x81387c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
14. [04:21:59.904] malloc(1002) = 0x8139780
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
26. [04:21:59.905] malloc(1002) = 0x813a740
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
38. [04:21:59.905] malloc(1002) = 0x813b700
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
50. [04:21:59.905] malloc(1002) = 0x813c6c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
3. [04:21:59.903] malloc(1001) = 0x8138bb0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
15. [04:21:59.904] malloc(1001) = 0x8139b70
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
27. [04:21:59.905] malloc(1001) = 0x813ab30
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
39. [04:21:59.905] malloc(1001) = 0x813baf0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
51. [04:21:59.905] malloc(1001) = 0x813cab0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
4. [04:21:59.903] malloc(1000) = 0x8138fa0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
16. [04:21:59.904] malloc(1000) = 0x8139f60
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
28. [04:21:59.905] malloc(1000) = 0x813af20
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
40. [04:21:59.905] malloc(1000) = 0x813bee0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
52. [04:21:59.905] malloc(1000) = 0x813cea0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485af (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
61. [04:21:59.905] malloc(1002) = 0x813d290
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
70. [04:21:59.905] malloc(1002) = 0x813de60
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
79. [04:21:59.905] malloc(1002) = 0x813ea30
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
62. [04:21:59.905] malloc(1001) = 0x813d680
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
71. [04:21:59.905] malloc(1001) = 0x813e250
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
80. [04:21:59.905] malloc(1001) = 0x813ee20
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
63. [04:21:59.905] malloc(1000) = 0x813da70
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
72. [04:21:59.905] malloc(1000) = 0x813e640
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
81. [04:21:59.905] malloc(1000) = 0x813f210
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485cf (in start at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
	0x80485f1 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:83)
