version=2.0, arch=i686, timestamp=2011.6.16 15:49:48, process=../bin/shmseg_test, pid=17913, filter=resolve, backtrace depth=10, origin=sp-rtrace 1.6, 
<1> : segment (shared memory segment) [refcount]
<2> : address (shared memory attachments)
<4> : control (shared memory segment control operation)
: /lib/libc-2.12.1.so => 0x136000-0x28d000
: /usr/lib/libsp-rtrace-main.so.1.0.6 => 0x619000-0x61f000
: /lib/libgcc_s.so.1 => 0x6fc000-0x716000
: /lib/libm-2.12.1.so => 0x808000-0x82c000
: /usr/lib/libsp-rtrace1.so.1.0.6 => 0x9e0000-0x9f4000
: /lib/librt-2.12.1.so => 0xa6b000-0xa72000
: /usr/lib/sp-rtrace/libsp-rtrace-shmsysv.so => 0xc8e000-0xc91000
: /lib/libpthread-2.12.1.so => 0xcde000-0xcf3000
: /lib/libdl-2.12.1.so => 0xeb6000-0xeb8000
: /lib/ld-2.12.1.so => 0xed4000-0xef0000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test => 0x8048000-0x8049000
## tracing module: [0] main (1.0)
## tracing module: [1] shmsysv (1.0)
1. [14:54:26.319] shmget<segment>(1024) = 0x1088018
	0x8048526 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485b5 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485c2 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485d2 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x14cce7 __libc_start_main() from /lib/libc-2.12.1.so
	0x8048461 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test

2. [14:54:26.319] shmat<address>(1024) = 0xb7700000
	$shmid = 0x1088018
	$cpid = 17913
	0x804854a zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485b5 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485c2 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485d2 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x14cce7 __libc_start_main() from /lib/libc-2.12.1.so
	0x8048461 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test

3. [14:54:26.319] shmat<address>(1024) = 0xb76ff000
	$shmid = 0x1088018
	$cpid = 17913
	0x8048568 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485b5 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485c2 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485d2 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x14cce7 __libc_start_main() from /lib/libc-2.12.1.so
	0x8048461 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test

4. [14:54:26.319] shmctl<control>(0x1088018)
	$cmd = IPC_RMID

5. [14:54:26.319] shmdt<address>(0xb7700000)

6. [14:54:26.320] shmdt<address>(0xb76ff000)

7. [14:54:26.320] shmdt<segment>(0x1088018)

