version=1.4, arch=i686, timestamp=2011.2.21 11:20:38, process=../bin/shmseg_test, pid=16911, filter=resolve, backtrace depth=10, origin=sp-rtrace 1.4, 
<1> : segment (shared memory segment) [refcount]
<2> : address (shared memory attachments)
<4> : control (shared memory segment control operation)
: /lib/libgcc_s.so.1 => 0x110000-0x12a000
: /lib/ld-2.12.1.so => 0x29f000-0x2bb000
: /usr/lib/libsp-rtrace-main.so.1.0.4 => 0x2ec000-0x2f1000
: /lib/libc-2.12.1.so => 0x381000-0x4d8000
: /usr/lib/sp-rtrace/libsp-rtrace-shmsysv.so => 0x664000-0x667000
: /lib/librt-2.12.1.so => 0x6ff000-0x706000
: /lib/libdl-2.12.1.so => 0x955000-0x957000
: /lib/libpthread-2.12.1.so => 0x9a8000-0x9bd000
: /lib/libm-2.12.1.so => 0xc2e000-0xc52000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test => 0x8048000-0x8049000
## tracing module: [0] main (1.0)
## tracing module: [1] shmsysv (1.0)
1. [00:30:47.188] shmget<segment>(1024) = 0x16a801e
	0x8048526 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485b5 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485c2 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485d2 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x397ce7 __libc_start_main() from /lib/libc-2.12.1.so
	0x8048461 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test

2. [00:30:47.188] shmat<address>(1024) = 0xb7702000
	$shmid = 0x16a801e
	$cpid = 16911
	0x804854a zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485b5 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485c2 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485d2 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x397ce7 __libc_start_main() from /lib/libc-2.12.1.so
	0x8048461 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test

3. [00:30:47.188] shmat<address>(1024) = 0xb7701000
	$shmid = 0x16a801e
	$cpid = 16911
	0x8048568 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485b5 one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485c2 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x80485d2 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test
	0x397ce7 __libc_start_main() from /lib/libc-2.12.1.so
	0x8048461 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/shmseg_test

4. [00:30:47.188] shmctl<control>(0x16a801e)
	$cmd = IPC_RMID

5. [00:30:47.188] shmdt<address>(0xb7702000)

6. [00:30:47.189] shmdt<address>(0xb7701000)

7. [00:30:47.189] shmdt<segment>(0x16a801e)

