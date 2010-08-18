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
#   highest block 0x9d92600
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
2. [09:02:04.760] malloc(1002) = 0x9d8b7c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
3. [09:02:04.760] malloc(1001) = 0x9d8bbb0
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
5. [09:02:04.760] malloc(2000) = 0x9d8c390
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
6. [09:02:04.760] free(0x9d8c390)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
7. [09:02:04.760] malloc(2001) = 0x9d8c390
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
8. [09:02:04.760] free(0x9d8c390)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
9. [09:02:04.760] malloc(2002) = 0x9d8c390
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
10. [09:02:04.760] free(0x9d8c390)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
11. [09:02:04.760] malloc(2003) = 0x9d8c390
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
12. [09:02:04.760] free(0x9d8c390)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
13. [09:02:04.760] malloc(1003) = 0x9d8c390
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
14. [09:02:04.760] malloc(1002) = 0x9d8c780
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
15. [09:02:04.760] malloc(1001) = 0x9d8cb70
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
16. [09:02:04.760] malloc(1000) = 0x9d8cf60
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
17. [09:02:04.760] malloc(2000) = 0x9d8d350
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
18. [09:02:04.760] free(0x9d8d350)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
19. [09:02:04.760] malloc(2001) = 0x9d8d350
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
20. [09:02:04.760] free(0x9d8d350)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
21. [09:02:04.760] malloc(2002) = 0x9d8d350
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
22. [09:02:04.760] free(0x9d8d350)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
23. [09:02:04.760] malloc(2003) = 0x9d8d350
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
24. [09:02:04.760] free(0x9d8d350)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
25. [09:02:04.760] malloc(1003) = 0x9d8d350
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
26. [09:02:04.760] malloc(1002) = 0x9d8d740
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
27. [09:02:04.760] malloc(1001) = 0x9d8db30
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
28. [09:02:04.760] malloc(1000) = 0x9d8df20
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
29. [09:02:04.760] malloc(2000) = 0x9d8e310
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
30. [09:02:04.760] free(0x9d8e310)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
31. [09:02:04.760] malloc(2001) = 0x9d8e310
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
32. [09:02:04.760] free(0x9d8e310)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
33. [09:02:04.760] malloc(2002) = 0x9d8e310
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
34. [09:02:04.760] free(0x9d8e310)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
35. [09:02:04.760] malloc(2003) = 0x9d8e310
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
36. [09:02:04.760] free(0x9d8e310)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
37. [09:02:04.760] malloc(1003) = 0x9d8e310
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
38. [09:02:04.760] malloc(1002) = 0x9d8e700
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
39. [09:02:04.760] malloc(1001) = 0x9d8eaf0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
40. [09:02:04.760] malloc(1000) = 0x9d8eee0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
41. [09:02:04.760] malloc(2000) = 0x9d8f2d0
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
42. [09:02:04.760] free(0x9d8f2d0)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
43. [09:02:04.760] malloc(2001) = 0x9d8f2d0
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
44. [09:02:04.760] free(0x9d8f2d0)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
45. [09:02:04.760] malloc(2002) = 0x9d8f2d0
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
46. [09:02:04.760] free(0x9d8f2d0)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
47. [09:02:04.760] malloc(2003) = 0x9d8f2d0
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
48. [09:02:04.760] free(0x9d8f2d0)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
49. [09:02:04.760] malloc(1003) = 0x9d8f2d0
	0x8048570 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:62)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
50. [09:02:04.760] malloc(1002) = 0x9d8f6c0
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
51. [09:02:04.760] malloc(1001) = 0x9d8fab0
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
52. [09:02:04.761] malloc(1000) = 0x9d8fea0
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
53. [09:02:04.761] malloc(2000) = 0x9d90290
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
54. [09:02:04.761] free(0x9d90290)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
55. [09:02:04.761] malloc(2001) = 0x9d90290
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
56. [09:02:04.761] free(0x9d90290)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
57. [09:02:04.761] malloc(2002) = 0x9d90290
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x8048585 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:65)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
58. [09:02:04.761] free(0x9d90290)
	0x804858d (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:66)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
59. [09:02:04.761] malloc(2003) = 0x9d90290
	0x8048599 (in three at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:67)
	0x80485b3 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:73)
60. [09:02:04.761] free(0x9d90290)
	0x80485bb (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:72)
61. [09:02:04.761] malloc(1002) = 0x9d90290
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
62. [09:02:04.761] malloc(1001) = 0x9d90680
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
63. [09:02:04.761] malloc(1000) = 0x9d90a70
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
64. [09:02:04.761] malloc(2000) = 0x9d90e60
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
65. [09:02:04.761] free(0x9d90e60)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
66. [09:02:04.761] malloc(2001) = 0x9d90e60
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
67. [09:02:04.761] free(0x9d90e60)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
68. [09:02:04.761] malloc(2002) = 0x9d90e60
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
69. [09:02:04.761] free(0x9d90e60)
	0x80485de (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:75)
70. [09:02:04.761] malloc(1002) = 0x9d90e60
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
71. [09:02:04.761] malloc(1001) = 0x9d91250
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
72. [09:02:04.761] malloc(1000) = 0x9d91640
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
73. [09:02:04.761] malloc(2000) = 0x9d91a30
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
74. [09:02:04.761] free(0x9d91a30)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
75. [09:02:04.761] malloc(2001) = 0x9d91a30
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
76. [09:02:04.761] free(0x9d91a30)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
77. [09:02:04.761] malloc(2002) = 0x9d91a30
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
78. [09:02:04.761] free(0x9d91a30)
	0x80485de (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:75)
79. [09:02:04.761] malloc(1002) = 0x9d91a30
	0x8048533 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:53)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
80. [09:02:04.761] malloc(1001) = 0x9d91e20
	0x80484f6 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:44)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
81. [09:02:04.761] malloc(1000) = 0x9d92210
	0x80484c6 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:36)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
82. [09:02:04.761] malloc(2000) = 0x9d92600
	0x80484e2 (in zero at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:40)
	0x804850b (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:47)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
83. [09:02:04.761] free(0x9d92600)
	0x8048513 (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:48)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
84. [09:02:04.761] malloc(2001) = 0x9d92600
	0x804851f (in one at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:49)
	0x8048548 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:56)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
85. [09:02:04.761] free(0x9d92600)
	0x8048550 (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:57)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
86. [09:02:04.761] malloc(2002) = 0x9d92600
	0x804855c (in two at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:58)
	0x80485d6 (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:76)
87. [09:02:04.761] free(0x9d92600)
	0x80485de (in main at /home/wiper/bugs/rtrace/sp-rtrace-1.0/tests/sp-rtrace.postproc/alloc_test.c:75)
