version=1.0, arch=i486-linux-gnu, time=2010.8.18 17:06:18, process=, pid=9524, 
: /lib/tls/i686/cmov/libpthread-2.11.1.so => 0x110000-0x125000
: /lib/ld-2.11.1.so => 0x185000-0x1a0000
: /lib/tls/i686/cmov/libc-2.11.1.so => 0x1a2000-0x2f5000
: /usr/lib/sp-rtrace/libsp-rtrace-main.so.1.0.0 => 0x433000-0x438000
: /usr/lib/sp-rtrace/libsp-rtrace-memory.so.1.0.0 => 0x683000-0x685000
: /lib/tls/i686/cmov/librt-2.11.1.so => 0x9a7000-0x9ae000
: /lib/tls/i686/cmov/libdl-2.11.1.so => 0xa29000-0xa2b000
: /lib/libgcc_s.so.1 => 0xf0f000-0xf2c000
: /lib/tls/i686/cmov/libm-2.11.1.so => 0xf4b000-0xf6f000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/bin/alloc_test => 0x8048000-0x8049000
# heap status information:
#   heap bottom 0x82f1000
#   heap top 0x8312000
#   lowest block 0x82f1008
#   highest block 0x82f8600
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
1. [07:02:30.228] malloc(1003) = 0x82f1008
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
2. [07:02:30.228] malloc(1002) = 0x82f17c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
3. [07:02:30.228] malloc(1001) = 0x82f1bb0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
4. [07:02:30.228] malloc(1000) = 0x82f1fa0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
5. [07:02:30.228] malloc(2000) = 0x82f2390
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
6. [07:02:30.228] free(0x82f2390)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
7. [07:02:30.228] malloc(2001) = 0x82f2390
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
8. [07:02:30.228] free(0x82f2390)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
9. [07:02:30.228] malloc(2002) = 0x82f2390
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
10. [07:02:30.228] free(0x82f2390)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
11. [07:02:30.228] malloc(2003) = 0x82f2390
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
12. [07:02:30.228] free(0x82f2390)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
13. [07:02:30.228] malloc(1003) = 0x82f2390
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
14. [07:02:30.228] malloc(1002) = 0x82f2780
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
15. [07:02:30.228] malloc(1001) = 0x82f2b70
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
16. [07:02:30.228] malloc(1000) = 0x82f2f60
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
17. [07:02:30.228] malloc(2000) = 0x82f3350
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
18. [07:02:30.228] free(0x82f3350)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
19. [07:02:30.228] malloc(2001) = 0x82f3350
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
20. [07:02:30.228] free(0x82f3350)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
21. [07:02:30.228] malloc(2002) = 0x82f3350
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
22. [07:02:30.228] free(0x82f3350)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
23. [07:02:30.228] malloc(2003) = 0x82f3350
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
24. [07:02:30.228] free(0x82f3350)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
25. [07:02:30.228] malloc(1003) = 0x82f3350
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
26. [07:02:30.228] malloc(1002) = 0x82f3740
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
27. [07:02:30.228] malloc(1001) = 0x82f3b30
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
28. [07:02:30.228] malloc(1000) = 0x82f3f20
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
29. [07:02:30.228] malloc(2000) = 0x82f4310
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
30. [07:02:30.228] free(0x82f4310)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
31. [07:02:30.228] malloc(2001) = 0x82f4310
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
32. [07:02:30.228] free(0x82f4310)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
33. [07:02:30.228] malloc(2002) = 0x82f4310
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
34. [07:02:30.228] free(0x82f4310)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
35. [07:02:30.228] malloc(2003) = 0x82f4310
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
36. [07:02:30.228] free(0x82f4310)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
37. [07:02:30.228] malloc(1003) = 0x82f4310
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
38. [07:02:30.228] malloc(1002) = 0x82f4700
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
39. [07:02:30.228] malloc(1001) = 0x82f4af0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
40. [07:02:30.228] malloc(1000) = 0x82f4ee0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
41. [07:02:30.228] malloc(2000) = 0x82f52d0
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
42. [07:02:30.228] free(0x82f52d0)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
43. [07:02:30.228] malloc(2001) = 0x82f52d0
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
44. [07:02:30.228] free(0x82f52d0)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
45. [07:02:30.228] malloc(2002) = 0x82f52d0
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
46. [07:02:30.228] free(0x82f52d0)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
47. [07:02:30.228] malloc(2003) = 0x82f52d0
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
48. [07:02:30.228] free(0x82f52d0)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
49. [07:02:30.228] malloc(1003) = 0x82f52d0
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
50. [07:02:30.228] malloc(1002) = 0x82f56c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
51. [07:02:30.228] malloc(1001) = 0x82f5ab0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
52. [07:02:30.228] malloc(1000) = 0x82f5ea0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
53. [07:02:30.228] malloc(2000) = 0x82f6290
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
54. [07:02:30.228] free(0x82f6290)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
55. [07:02:30.228] malloc(2001) = 0x82f6290
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
56. [07:02:30.228] free(0x82f6290)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
57. [07:02:30.228] malloc(2002) = 0x82f6290
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
58. [07:02:30.228] free(0x82f6290)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
59. [07:02:30.228] malloc(2003) = 0x82f6290
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
60. [07:02:30.228] free(0x82f6290)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
61. [07:02:30.228] malloc(1002) = 0x82f6290
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
62. [07:02:30.228] malloc(1001) = 0x82f6680
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
63. [07:02:30.228] malloc(1000) = 0x82f6a70
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
64. [07:02:30.228] malloc(2000) = 0x82f6e60
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
65. [07:02:30.228] free(0x82f6e60)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
66. [07:02:30.228] malloc(2001) = 0x82f6e60
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
67. [07:02:30.228] free(0x82f6e60)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
68. [07:02:30.228] malloc(2002) = 0x82f6e60
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
69. [07:02:30.228] free(0x82f6e60)
	0x80485de (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:75)
70. [07:02:30.229] malloc(1002) = 0x82f6e60
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
71. [07:02:30.229] malloc(1001) = 0x82f7250
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
72. [07:02:30.229] malloc(1000) = 0x82f7640
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
73. [07:02:30.229] malloc(2000) = 0x82f7a30
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
74. [07:02:30.229] free(0x82f7a30)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
75. [07:02:30.229] malloc(2001) = 0x82f7a30
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
76. [07:02:30.229] free(0x82f7a30)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
77. [07:02:30.229] malloc(2002) = 0x82f7a30
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
78. [07:02:30.229] free(0x82f7a30)
	0x80485de (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:75)
79. [07:02:30.229] malloc(1002) = 0x82f7a30
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
80. [07:02:30.229] malloc(1001) = 0x82f7e20
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
81. [07:02:30.229] malloc(1000) = 0x82f8210
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
82. [07:02:30.229] malloc(2000) = 0x82f8600
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
83. [07:02:30.229] free(0x82f8600)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
84. [07:02:30.229] malloc(2001) = 0x82f8600
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
85. [07:02:30.229] free(0x82f8600)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
86. [07:02:30.229] malloc(2002) = 0x82f8600
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
87. [07:02:30.229] free(0x82f8600)
	0x80485de (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:75)
