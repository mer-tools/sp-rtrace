version=1.0, arch=i486-linux-gnu, timestamp=2010.10.18 14:34:33, process=../bin/alloc_test, pid=31459, filter=resolve, 
<1> : memory (memory allocation in bytes)
: /lib/tls/i686/cmov/libc-2.11.1.so => 0x110000-0x263000
: /lib/tls/i686/cmov/libdl-2.11.1.so => 0x317000-0x319000
: /usr/lib/sp-rtrace/libsp-rtrace-main.so.1.0.0 => 0x430000-0x435000
: /usr/lib/sp-rtrace/libsp-rtrace-memory.so.1.0.0 => 0x632000-0x634000
: /lib/tls/i686/cmov/libpthread-2.11.1.so => 0x6c3000-0x6d8000
: /lib/ld-2.11.1.so => 0x7bf000-0x7da000
: /lib/tls/i686/cmov/librt-2.11.1.so => 0x992000-0x999000
: /lib/libgcc_s.so.1 => 0xb0b000-0xb28000
: /lib/tls/i686/cmov/libm-2.11.1.so => 0xf70000-0xf94000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test => 0x8048000-0x8049000
## heap status information:
##   heap bottom 0x9334000
##   heap top 0x9355000
##   lowest block 0x9334008
##   highest block 0x933b600
##   non-mapped space allocated from system 135168
##   number of free chunks 1
##   number of fastbin blocks 0
##   number of mapped regions 0
##   space in mapped regions 0
##   maximum total allocated space 0
##   space available in freed fastbin blocks 0
##   total allocated space 30200
##   total free space 104968
##   top-most, releasable (via malloc_trim) space 104968
## tracing module: [0] main (1.0)
## tracing module: [1] memory (1.0)
1. [01:12:19.539] malloc(1003) = 0x9334008
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

2. [01:12:19.539] malloc(1002) = 0x93347c0
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

3. [01:12:19.539] malloc(1001) = 0x9334bb0
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

4. [01:12:19.539] malloc(1000) = 0x9334fa0
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

5. [01:12:19.539] malloc(2000) = 0x9335390
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

6. [01:12:19.539] free(0x9335390)

7. [01:12:19.539] malloc(2001) = 0x9335390
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

8. [01:12:19.539] free(0x9335390)

9. [01:12:19.539] malloc(2002) = 0x9335390
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

10. [01:12:19.539] free(0x9335390)

11. [01:12:19.539] malloc(2003) = 0x9335390
	0x8048599 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

12. [01:12:19.539] free(0x9335390)

13. [01:12:19.539] malloc(1003) = 0x9335390
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

14. [01:12:19.539] malloc(1002) = 0x9335780
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

15. [01:12:19.539] malloc(1001) = 0x9335b70
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

16. [01:12:19.539] malloc(1000) = 0x9335f60
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

17. [01:12:19.539] malloc(2000) = 0x9336350
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

18. [01:12:19.539] free(0x9336350)

19. [01:12:19.539] malloc(2001) = 0x9336350
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

20. [01:12:19.539] free(0x9336350)

21. [01:12:19.539] malloc(2002) = 0x9336350
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

22. [01:12:19.539] free(0x9336350)

23. [01:12:19.539] malloc(2003) = 0x9336350
	0x8048599 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

24. [01:12:19.539] free(0x9336350)

25. [01:12:19.539] malloc(1003) = 0x9336350
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

26. [01:12:19.539] malloc(1002) = 0x9336740
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

27. [01:12:19.539] malloc(1001) = 0x9336b30
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

28. [01:12:19.539] malloc(1000) = 0x9336f20
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

29. [01:12:19.539] malloc(2000) = 0x9337310
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

30. [01:12:19.539] free(0x9337310)

31. [01:12:19.539] malloc(2001) = 0x9337310
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

32. [01:12:19.539] free(0x9337310)

33. [01:12:19.539] malloc(2002) = 0x9337310
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

34. [01:12:19.539] free(0x9337310)

35. [01:12:19.539] malloc(2003) = 0x9337310
	0x8048599 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

36. [01:12:19.539] free(0x9337310)

37. [01:12:19.539] malloc(1003) = 0x9337310
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

38. [01:12:19.539] malloc(1002) = 0x9337700
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

39. [01:12:19.539] malloc(1001) = 0x9337af0
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

40. [01:12:19.539] malloc(1000) = 0x9337ee0
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

41. [01:12:19.539] malloc(2000) = 0x93382d0
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

42. [01:12:19.539] free(0x93382d0)

43. [01:12:19.539] malloc(2001) = 0x93382d0
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

44. [01:12:19.539] free(0x93382d0)

45. [01:12:19.539] malloc(2002) = 0x93382d0
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

46. [01:12:19.539] free(0x93382d0)

47. [01:12:19.539] malloc(2003) = 0x93382d0
	0x8048599 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

48. [01:12:19.539] free(0x93382d0)

49. [01:12:19.539] malloc(1003) = 0x93382d0
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

50. [01:12:19.539] malloc(1002) = 0x93386c0
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

51. [01:12:19.539] malloc(1001) = 0x9338ab0
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

52. [01:12:19.539] malloc(1000) = 0x9338ea0
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

53. [01:12:19.539] malloc(2000) = 0x9339290
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

54. [01:12:19.539] free(0x9339290)

55. [01:12:19.539] malloc(2001) = 0x9339290
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

56. [01:12:19.539] free(0x9339290)

57. [01:12:19.539] malloc(2002) = 0x9339290
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048585 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

58. [01:12:19.539] free(0x9339290)

59. [01:12:19.539] malloc(2003) = 0x9339290
	0x8048599 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

60. [01:12:19.539] free(0x9339290)

61. [01:12:19.539] malloc(1002) = 0x9339290
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

62. [01:12:19.539] malloc(1001) = 0x9339680
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

63. [01:12:19.539] malloc(1000) = 0x9339a70
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

64. [01:12:19.539] malloc(2000) = 0x9339e60
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

65. [01:12:19.539] free(0x9339e60)

66. [01:12:19.539] malloc(2001) = 0x9339e60
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

67. [01:12:19.539] free(0x9339e60)

68. [01:12:19.539] malloc(2002) = 0x9339e60
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

69. [01:12:19.539] free(0x9339e60)

70. [01:12:19.539] malloc(1002) = 0x9339e60
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

71. [01:12:19.539] malloc(1001) = 0x933a250
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

72. [01:12:19.539] malloc(1000) = 0x933a640
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

73. [01:12:19.539] malloc(2000) = 0x933aa30
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

74. [01:12:19.539] free(0x933aa30)

75. [01:12:19.539] malloc(2001) = 0x933aa30
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

76. [01:12:19.539] free(0x933aa30)

77. [01:12:19.540] malloc(2002) = 0x933aa30
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

78. [01:12:19.540] free(0x933aa30)

79. [01:12:19.540] malloc(1002) = 0x933aa30
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

80. [01:12:19.540] malloc(1001) = 0x933ae20
	0x80484f6 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

81. [01:12:19.540] malloc(1000) = 0x933b210
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

82. [01:12:19.540] malloc(2000) = 0x933b600
	0x80484e2 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

83. [01:12:19.540] free(0x933b600)

84. [01:12:19.540] malloc(2001) = 0x933b600
	0x804851f one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

85. [01:12:19.540] free(0x933b600)

86. [01:12:19.540] malloc(2002) = 0x933b600
	0x804855c two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x126bd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

87. [01:12:19.540] free(0x933b600)

