version=1.0, arch=i486-linux-gnu, time=2010.10.1 14:13:59, process=../bin/alloc_test, pid=31018, 
<1> : memory (memory allocation in bytes)
: /usr/lib/sp-rtrace/libsp-rtrace-memory.so.1.0.0 => 0x197000-0x199000
: /lib/ld-2.11.1.so => 0x35a000-0x375000
: /lib/libgcc_s.so.1 => 0x3fe000-0x41b000
: /lib/tls/i686/cmov/libpthread-2.11.1.so => 0x64f000-0x664000
: /lib/tls/i686/cmov/libm-2.11.1.so => 0x684000-0x6a8000
: /lib/tls/i686/cmov/libdl-2.11.1.so => 0x7b5000-0x7b7000
: /usr/lib/sp-rtrace/libsp-rtrace-main.so.1.0.0 => 0xb23000-0xb28000
: /lib/tls/i686/cmov/libc-2.11.1.so => 0xdc9000-0xf1c000
: /lib/tls/i686/cmov/librt-2.11.1.so => 0xf93000-0xf9a000
: /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test => 0x8048000-0x8049000
# heap status information:
#   heap bottom 0x8ea0000
#   heap top 0x8ec1000
#   lowest block 0x8ea0008
#   highest block 0x8ea7600
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
1. [02:34:34.217] malloc(1003) = 0x8ea0008
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0xddfbd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

13. [02:34:34.217] malloc(1003) = 0x8ea1390
	0x8048570 three() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485af start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0xddfbd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

79. [02:34:34.218] malloc(1002) = 0x8ea6a30
	0x8048533 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0xddfbd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

81. [02:34:34.218] malloc(1000) = 0x8ea7210
	0x80484c6 zero() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x804850b one() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x8048548 two() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485cf start() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0x80485f1 main() from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test
	0xddfbd6 __libc_start_main() from /lib/tls/i686/cmov/libc-2.11.1.so
	0x8048421 from /scratchbox/users/wiper/home/wiper/bugs/rtrace/sp-rtrace/tests/bin/alloc_test

# allocation summary for the last backtrace: 3 block(s) with total size 3000
# allocation summary for the last backtrace: 3 block(s) with total size 3006
# allocation summary for the last backtrace: 5 block(s) with total size 5015
# Resource - memory (memory allocation in bytes):
# 29 block(s) leaked with total size of 29039 bytes
