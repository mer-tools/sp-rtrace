version=1.0, arch=x86_64-linux-gnu, time=2010.9.4 15:03:33, process=../bin/alloc_test, pid=9399, 
<1> : memory (memory allocation in bytes)
# heap status information:
#   heap bottom 0xbda000
#   heap top 0xbfb000
#   lowest block 0xbda010
#   highest block 0xbe19e0
#   non-mapped space allocated from system 135168
#   number of free chunks 1
#   number of fastbin blocks 0
#   number of mapped regions 0
#   space in mapped regions 0
#   maximum total allocated space 0
#   space available in freed fastbin blocks 0
#   total allocated space 31184
#   total free space 103984
#   top-most, releasable (via malloc_trim) space 103984
# tracing module: [0] main (1.0)
# tracing module: [1] memory (1.0)
1. [01:07:12.771] malloc(1003) = 0xbda010
	0x4006f1 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
2. [01:07:12.771] malloc(1002) = 0xbdaa60
	0x4006b6 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
3. [01:07:12.771] malloc(1001) = 0xbdae60
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
4. [01:07:12.771] malloc(1000) = 0xbdb260
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
5. [01:07:12.771] malloc(2000) = 0xbdb650
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
6. [01:07:12.771] free(0xbdb650)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
7. [01:07:12.771] malloc(2001) = 0xbdb650
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
8. [01:07:12.771] free(0xbdb650)
	0x4006d7 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
9. [01:07:12.771] malloc(2002) = 0xbdb650
	0x4006e1 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
10. [01:07:12.771] free(0xbdb650)
	0x400712 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
11. [01:07:12.771] malloc(2003) = 0xbdb650
	0x40071c <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
12. [01:07:12.771] free(0xbdb650)
	0x400741 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
13. [01:07:12.771] malloc(1003) = 0xbdb650
	0x4006f1 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
14. [01:07:12.771] malloc(1002) = 0xbdba50
	0x4006b6 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
15. [01:07:12.771] malloc(1001) = 0xbdbe50
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
16. [01:07:12.771] malloc(1000) = 0xbdc250
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
17. [01:07:12.771] malloc(2000) = 0xbdc640
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
18. [01:07:12.771] free(0xbdc640)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
19. [01:07:12.771] malloc(2001) = 0xbdc640
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
20. [01:07:12.771] free(0xbdc640)
	0x4006d7 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
21. [01:07:12.771] malloc(2002) = 0xbdc640
	0x4006e1 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
22. [01:07:12.771] free(0xbdc640)
	0x400712 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
23. [01:07:12.771] malloc(2003) = 0xbdc640
	0x40071c <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
24. [01:07:12.771] free(0xbdc640)
	0x400741 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
25. [01:07:12.771] malloc(1003) = 0xbdc640
	0x4006f1 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
26. [01:07:12.771] malloc(1002) = 0xbdca40
	0x4006b6 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
27. [01:07:12.771] malloc(1001) = 0xbdce40
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
28. [01:07:12.771] malloc(1000) = 0xbdd240
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
29. [01:07:12.771] malloc(2000) = 0xbdd630
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
30. [01:07:12.771] free(0xbdd630)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
31. [01:07:12.771] malloc(2001) = 0xbdd630
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
32. [01:07:12.771] free(0xbdd630)
	0x4006d7 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
33. [01:07:12.771] malloc(2002) = 0xbdd630
	0x4006e1 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
34. [01:07:12.771] free(0xbdd630)
	0x400712 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
35. [01:07:12.771] malloc(2003) = 0xbdd630
	0x40071c <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
36. [01:07:12.771] free(0xbdd630)
	0x400741 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
37. [01:07:12.771] malloc(1003) = 0xbdd630
	0x4006f1 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
38. [01:07:12.771] malloc(1002) = 0xbdda30
	0x4006b6 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
39. [01:07:12.771] malloc(1001) = 0xbdde30
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
40. [01:07:12.771] malloc(1000) = 0xbde230
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
41. [01:07:12.771] malloc(2000) = 0xbde620
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
42. [01:07:12.771] free(0xbde620)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
43. [01:07:12.771] malloc(2001) = 0xbde620
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
44. [01:07:12.771] free(0xbde620)
	0x4006d7 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
45. [01:07:12.771] malloc(2002) = 0xbde620
	0x4006e1 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
46. [01:07:12.771] free(0xbde620)
	0x400712 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
47. [01:07:12.771] malloc(2003) = 0xbde620
	0x40071c <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
48. [01:07:12.771] free(0xbde620)
	0x400741 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
49. [01:07:12.771] malloc(1003) = 0xbde620
	0x4006f1 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
50. [01:07:12.771] malloc(1002) = 0xbdea20
	0x4006b6 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
51. [01:07:12.771] malloc(1001) = 0xbdee20
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
52. [01:07:12.771] malloc(1000) = 0xbdf220
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
53. [01:07:12.771] malloc(2000) = 0xbdf610
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
54. [01:07:12.771] free(0xbdf610)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
55. [01:07:12.771] malloc(2001) = 0xbdf610
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
56. [01:07:12.771] free(0xbdf610)
	0x4006d7 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
57. [01:07:12.771] malloc(2002) = 0xbdf610
	0x4006e1 <unknown>
	0x40070a <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
58. [01:07:12.771] free(0xbdf610)
	0x400712 <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
59. [01:07:12.771] malloc(2003) = 0xbdf610
	0x40071c <unknown>
	0x400739 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
60. [01:07:12.771] free(0xbdf610)
	0x400741 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
61. [01:07:12.771] malloc(1002) = 0xbdf610
	0x4006b6 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
62. [01:07:12.771] malloc(1001) = 0xbdfa10
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
63. [01:07:12.771] malloc(1000) = 0xbdfe10
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
64. [01:07:12.771] malloc(2000) = 0xbe0200
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
65. [01:07:12.771] free(0xbe0200)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
66. [01:07:12.771] malloc(2001) = 0xbe0200
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
67. [01:07:12.771] free(0xbe0200)
	0x4006d7 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
68. [01:07:12.771] malloc(2002) = 0xbe0200
	0x4006e1 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
69. [01:07:12.771] free(0xbe0200)
	0x400766 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
70. [01:07:12.771] malloc(1002) = 0xbe0200
	0x4006b6 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
71. [01:07:12.771] malloc(1001) = 0xbe0600
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
72. [01:07:12.771] malloc(1000) = 0xbe0a00
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
73. [01:07:12.771] malloc(2000) = 0xbe0df0
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
74. [01:07:12.771] free(0xbe0df0)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
75. [01:07:12.771] malloc(2001) = 0xbe0df0
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
76. [01:07:12.771] free(0xbe0df0)
	0x4006d7 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
77. [01:07:12.771] malloc(2002) = 0xbe0df0
	0x4006e1 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
78. [01:07:12.771] free(0xbe0df0)
	0x400766 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
79. [01:07:12.771] malloc(1002) = 0xbe0df0
	0x4006b6 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
80. [01:07:12.771] malloc(1001) = 0xbe11f0
	0x40067b <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
81. [01:07:12.771] malloc(1000) = 0xbe15f0
	0x400652 <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
82. [01:07:12.771] malloc(2000) = 0xbe19e0
	0x40066b <unknown>
	0x400694 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
83. [01:07:12.771] free(0xbe19e0)
	0x40069c <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
84. [01:07:12.771] malloc(2001) = 0xbe19e0
	0x4006a6 <unknown>
	0x4006cf <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
85. [01:07:12.771] free(0xbe19e0)
	0x4006d7 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
86. [01:07:12.771] malloc(2002) = 0xbe19e0
	0x4006e1 <unknown>
	0x40075e <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
87. [01:07:12.771] free(0xbe19e0)
	0x400766 <unknown>
	0x400780 <unknown>
	0x7f6124cc7c4d <unknown>
	0x400589 <unknown>
