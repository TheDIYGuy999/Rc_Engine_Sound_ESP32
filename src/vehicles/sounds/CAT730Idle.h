const unsigned int sampleRate = 22050;
const unsigned int sampleCount = 3280;
const signed char samples[] = {//0
-1, 20, 49, 42, 9, -10, 10, 20, 2, -26, -35, -23, -5, -2, -2, -3, //16
-7, -11, -7, -2, 1, 1, -3, -35, -55, -50, -20, 9, 15, 1, 9, 26, //32
33, 23, -3, -14, -16, -5, 14, 24, -5, -29, -25, 11, 54, 77, 73, 75, //48
92, 110, 105, 47, -2, -34, -48, -59, -67, -60, -42, -30, -33, -37, -22, 21, //64
28, 10, -18, -39, -54, -67, -84, -97, -95, -73, -38, -33, -36, -29, -5, 23, //80
49, 53, 50, 44, 41, 39, 23, -15, -56, -74, -59, -19, -7, -10, -22, -35, //96
-41, -30, -26, -38, -59, -71, -58, -37, -16, -3, -5, -12, 12, 47, 64, 38, //112
-15, -57, -69, -56, -34, -11, 4, 4, 12, 39, 72, 91, 98, 110, 118, 117, //128
107, 95, 78, 42, 26, 23, 18, -5, -60, -85, -93, -94, -94, -95, -88, -82, //144
-73, -62, -49, -28, -16, -4, 10, 25, 37, 48, 50, 42, 21, -3, -21, -26, //160
-25, -25, -23, -20, -18, -17, -12, -5, -3, 0, 19, 27, 20, 2, -8, -5, //176
-6, -20, -33, -36, -33, -30, -29, -28, -32, -36, -28, 13, 46, 68, 80, 83, //192
79, 73, 78, 85, 80, 69, 68, 75, 71, 49, 22, 1, -17, -20, -13, -1, //208
0, -18, -54, -65, -60, -44, -26, -5, 3, 8, 10, 7, -5, -40, -61, -65, //224
-54, -36, -23, -7, 7, 20, 28, 30, 31, 27, 18, 13, 18, 27, 13, -10, //240
-25, -26, -22, -18, -7, -4, -15, -30, -37, -27, -16, -3, 13, 21, 17, 5, //256
4, 0, -14, -30, -31, -18, -12, -6, 0, -5, -39, -55, -53, -44, -35, -20, //272
17, 29, 23, 12, 7, -2, -22, -20, -7, -4, -23, -51, -53, -54, -58, -55, //288
-37, -5, 10, 25, 40, 43, 30, 29, 34, 33, 30, 32, 42, 43, 42, 48, //304
54, 49, 28, 20, 15, 6, -4, -19, -34, -50, -58, -51, -38, -26, -22, -17, //320
-14, -13, -6, 11, 19, 22, 23, 22, 6, -7, -17, -29, -43, -54, -40, -17, //336
2, 15, 23, 26, 16, 2, -10, -21, -26, -11, 13, 33, 44, 53, 65, 80, //352
74, 62, 46, 30, 14, 7, 15, 22, 21, 14, 4, -3, -12, -18, -21, -25, //368
-32, -29, -18, -2, 12, 22, 34, 34, 27, 20, 15, -5, -26, -46, -56, -62, //384
-72, -87, -93, -100, -111, -120, -119, -108, -104, -98, -83, -61, -30, -11, 6, 17, //400
19, 18, 17, 16, 12, 13, 18, 22, 17, 15, 17, 21, 23, 26, 30, 31, //416
29, 27, 27, 17, 1, -14, -22, -28, -34, -38, -35, -33, -36, -41, -42, -40, //432
-39, -40, -40, -38, -31, -26, -23, -21, -18, -14, -19, -29, -35, -31, -16, 16, //448
36, 52, 62, 67, 66, 54, 41, 29, 23, 25, 31, 33, 30, 24, 16, 8, //464
-6, -18, -29, -38, -42, -42, -39, -34, -28, -22, -17, -3, 9, 16, 12, 4, //480
0, 9, 20, 30, 39, 43, 39, 23, 15, 11, 9, 10, 24, 40, 52, 57, //496
57, 52, 42, 37, 38, 39, 36, 31, 26, 24, 17, 7, -4, -19, -29, -33, //512
-31, -28, -31, -40, -41, -41, -39, -30, -11, 16, 22, 22, 24, 27, 21, 16, //528
10, 0, -12, -24, -25, -22, -19, -13, -6, -2, 0, 1, -1, -9, -25, -40, //544
-41, -38, -34, -30, -23, -15, -9, -2, -3, -17, -37, -46, -37, -27, -17, -7, //560
11, 22, 31, 35, 32, 24, 16, 19, 25, 30, 37, 41, 31, 12, -2, -3, //576
1, 7, 11, 19, 28, 32, 32, 24, 11, -2, -12, -14, -14, -14, -14, -16, //592
-21, -26, -27, -27, -29, -30, -25, -14, 6, 15, 20, 20, 18, 17, 19, 21, //608
22, 21, 18, 7, -7, -17, -23, -25, -22, -14, -8, -2, 2, 5, 10, 7, //624
-2, -7, -8, -7, -8, -9, -9, -10, -14, -17, -10, 2, 11, 12, 6, -2, //640
2, 12, 17, 18, 27, 62, 85, 90, 80, 65, 45, 4, -16, -16, -6, 4, //656
21, 36, 37, 11, -25, -39, -24, -34, -62, -72, -42, 7, 49, 50, 48, 45, //672
39, 30, 28, 19, 8, 5, 13, 15, 2, -3, 3, 2, -14, -30, -23, -25, //688
-43, -53, -10, 25, 30, 14, 6, 11, 5, -14, -31, -48, -64, -66, -27, -2, //704
1, 1, 15, 34, 25, 19, 32, 46, 37, 14, 28, 49, 49, 35, 34, 53, //720
54, 49, 51, 53, 21, -11, -28, -32, -35, -38, -32, -25, -30, -39, -42, -30, //736
-18, -4, 7, 11, 10, 7, 5, -1, -10, -17, -24, -29, -21, -5, 5, 2, //752
-9, -13, -17, -18, -7, 8, 2, -16, -15, 9, 34, 43, 27, 15, 2, -11, //768
-21, -31, -44, -58, -61, -54, -45, -35, -30, -33, -43, -51, -46, -31, -31, -30, //784
-17, 7, 42, 58, 64, 50, 13, -27, -46, -41, -38, -32, -14, 7, 7, -6, //800
-12, -8, -8, -18, -19, -14, -9, -4, 2, 6, -2, -15, -22, -24, -24, -20, //816
-8, 9, 21, 26, 36, 45, 47, 39, 26, 14, -3, -14, -21, -22, -13, 4, //832
38, 58, 66, 63, 56, 48, 38, 20, -1, -16, -25, -25, -10, 17, 41, 48, //848
37, 10, 3, 8, 19, 31, 38, 32, 26, 30, 42, 55, 61, 56, 51, 47, //864
44, 47, 56, 59, 52, 35, 12, -20, -35, -49, -62, -68, -65, -45, -33, -28, //880
-22, -12, 6, 15, 22, 28, 33, 37, 43, 45, 44, 37, 26, 13, -5, -15, //896
-22, -30, -39, -48, -48, -41, -28, -9, 10, 19, 10, -2, -6, -2, -4, -20, //912
-33, -45, -55, -62, -63, -62, -66, -70, -68, -56, -39, -39, -49, -63, -78, -88, //928
-88, -78, -71, -71, -69, -51, -28, -6, 7, 18, 32, 59, 76, 85, 81, 65, //944
46, 29, 21, 7, -10, -21, -21, -21, -25, -30, -30, -31, -48, -61, -69, -73, //960
-76, -76, -66, -55, -47, -41, -33, -23, -25, -28, -24, -9, 11, 35, 41, 33, //976
10, -21, -41, -36, -25, -21, -24, -26, -14, -1, 14, 28, 38, 45, 54, 63, //992
71, 70, 60, 50, 44, 46, 47, 45, 41, 22, 6, 0, 4, 11, 13, 17, //1008
28, 43, 54, 57, 56, 46, 35, 28, 30, 36, 35, 29, 25, 26, 26, 26, //1024
25, 18, 2, -17, -26, -20, -15, -13, -8, 1, 9, 18, 24, 25, 20, 11, //1040
7, 13, 13, 6, -4, -13, -28, -40, -45, -44, -37, -30, -20, -12, -4, 4, //1056
15, 27, 32, 19, -1, -15, -18, -16, -21, -28, -34, -36, -36, -35, -36, -39, //1072
-42, -40, -30, -2, 15, 26, 29, 25, 18, 22, 33, 41, 40, 31, 22, 20, //1088
14, 4, -5, -10, -14, -18, -19, -18, -17, -21, -29, -37, -42, -43, -41, -39, //1104
-42, -46, -48, -45, -42, -45, -49, -47, -35, -16, 7, 10, 8, 8, 17, 31, //1120
40, 34, 22, 10, 1, -5, -9, -10, -10, -9, -8, -4, -4, -6, -11, -19, //1136
-23, -20, -13, -4, -2, -4, -9, -13, -13, -14, -15, -17, -19, -19, -18, -14, //1152
-9, 0, 13, 18, 22, 25, 24, 20, 13, 16, 25, 33, 35, 23, 12, 2, //1168
-9, -20, -33, -50, -53, -48, -40, -35, -30, -23, -14, -6, -1, 1, 1, -1, //1184
-5, -7, -2, 8, 22, 27, 29, 32, 36, 42, 45, 48, 54, 63, 69, 70, //1200
66, 62, 56, 49, 41, 29, 20, 12, 4, -5, -18, -22, -24, -29, -40, -52, //1216
-58, -51, -44, -37, -28, -20, -6, 5, 14, 17, 17, 18, 21, 22, 21, 19, //1232
17, 10, 10, 18, 33, 41, 37, 24, 20, 21, 20, 15, 15, 18, 21, 22, //1248
20, 12, -6, -13, -13, -13, -14, -15, -7, 1, 6, 10, 14, 15, 11, 5, //1264
0, -7, -15, -25, -25, -20, -14, -10, -10, -16, -18, -15, -9, -11, -32, -45, //1280
-47, -46, -50, -54, -38, -17, 0, 12, 26, 43, 49, 30, 6, -6, -2, 7, //1296
-1, -17, -25, -21, -13, -11, -11, -5, 4, 4, -7, -23, -22, -17, -11, -2, //1312
32, 51, 53, 41, 37, 51, 75, 63, 32, 8, 7, 24, 19, -2, -26, -38, //1328
-43, -52, -60, -62, -56, -47, -38, -25, -12, 5, 14, 9, -21, -41, -52, -58, //1344
-59, -44, 12, 49, 62, 49, 26, 11, 4, -1, -11, -25, -38, -44, -28, 1, //1360
26, 37, 35, 42, 62, 85, 96, 94, 87, 97, 114, 127, 124, 107, 75, 65, //1376
66, 68, 57, 28, -24, -42, -41, -30, -23, -27, -48, -47, -20, 18, 43, 32, //1392
19, 26, 52, 76, 83, 64, 36, 5, -22, -37, -43, -57, -72, -79, -76, -68, //1408
-60, -55, -44, -24, -2, 21, 47, 56, 60, 65, 73, 74, 42, 4, -34, -57, //1424
-63, -60, -63, -77, -99, -115, -112, -75, -51, -42, -46, -43, -22, 30, 50, 42, //1440
14, -16, -34, -26, -9, 5, 13, 15, 17, 22, 23, 18, 7, -6, -21, -23, //1456
-17, -10, -8, -9, -4, 10, 27, 39, 39, 10, -24, -54, -70, -67, -44, -27, //1472
-10, 4, 10, 9, -5, -20, -44, -69, -82, -78, -53, -42, -37, -31, -26, -16, //1488
-5, 12, 33, 47, 52, 44, 37, 37, 47, 59, 54, 6, -33, -59, -72, -83, //1504
-102, -107, -105, -95, -71, -35, 19, 37, 40, 42, 49, 62, 83, 94, 97, 91, //1520
81, 68, 51, 24, -5, -24, -32, -30, -17, 6, 33, 51, 60, 75, 91, 101, //1536
101, 94, 77, 64, 53, 47, 45, 37, 7, -13, -23, -24, -28, -40, -58, -60, //1552
-57, -50, -40, -14, -2, 1, 0, 1, 5, 11, 14, 17, 16, 8, -4, -13, //1568
-15, -24, -38, -52, -67, -71, -66, -56, -44, -35, -31, -31, -38, -52, -70, -86, //1584
-93, -88, -74, -53, -33, -17, -5, 20, 50, 67, 62, 39, 30, 25, 19, 14, //1600
14, 23, 26, 17, -1, -22, -34, -26, -16, -16, -23, -22, 0, 14, 17, 14, //1616
15, 17, 14, 11, 10, 9, -2, -29, -42, -49, -55, -60, -60, -50, -42, -34, //1632
-28, -22, -14, -3, 15, 30, 38, 41, 52, 62, 66, 65, 61, 53, 29, 7, //1648
-9, -19, -30, -51, -57, -54, -45, -35, -22, 0, 9, 18, 31, 47, 59, 59, //1664
53, 49, 45, 36, 20, 14, 10, 9, 7, 6, -1, -9, -14, -12, -8, -5, //1680
-3, 3, 10, 11, 7, 4, 8, 11, 13, 14, 16, 8, -2, -7, -5, -1, //1696
-2, -6, -1, 9, 22, 30, 28, 18, 6, 0, 3, 10, 27, 35, 40, 40, //1712
36, 32, 32, 35, 36, 37, 36, 26, 19, 15, 11, 1, -15, -28, -22, -10, //1728
-4, -4, -8, -20, -31, -37, -35, -27, -17, -12, -4, 9, 28, 49, 76, 83, //1744
78, 66, 56, 50, 39, 28, 17, 10, 4, -7, -17, -29, -44, -55, -60, -63, //1760
-71, -84, -94, -97, -91, -83, -72, -63, -57, -60, -69, -71, -69, -67, -64, -59, //1776
-47, -37, -28, -27, -33, -44, -43, -35, -26, -20, -13, 1, 8, 9, 7, 9, //1792
13, 18, 19, 17, 12, 3, -12, -14, -7, 8, 23, 32, 29, 26, 26, 28, //1808
30, 30, 28, 28, 30, 32, 34, 28, 18, 9, 4, -2, -11, -26, -29, -25, //1824
-15, -3, 6, 13, 17, 23, 30, 35, 27, 16, 4, -5, -8, -7, -11, -21, //1840
-32, -38, -37, -34, -40, -44, -40, -28, -17, -7, -2, 3, 3, 1, -1, -3, //1856
-8, -13, -15, -14, -14, -19, -22, -24, -27, -33, -36, -33, -30, -28, -25, -18, //1872
-6, -1, 3, 3, 2, 1, 1, -1, -4, -12, -24, -34, -33, -22, -7, 10, //1888
26, 48, 57, 58, 53, 46, 41, 42, 46, 46, 41, 32, 23, 24, 24, 19, //1904
13, 11, 11, 14, 23, 36, 47, 59, 63, 64, 57, 41, 23, 5, -2, -3, //1920
0, 6, 6, -5, -12, -16, -18, -18, -9, 1, 11, 17, 19, 16, 7, 9, //1936
18, 26, 22, 10, -5, -7, -10, -17, -25, -26, -17, -4, 11, 19, 14, -15, //1952
-29, -32, -31, -35, -43, -35, -10, 20, 38, 35, 9, -5, -15, -23, -34, -46, //1968
-49, -33, -4, 29, 49, 48, 25, 18, 21, 21, 8, -24, -37, -43, -43, -40, //1984
-36, -41, -46, -43, -33, -23, -21, -19, -10, 3, 21, 44, 76, 80, 74, 70, //2000
75, 77, 49, 16, -15, -42, -60, -59, -18, 15, 34, 44, 50, 45, 28, 5, //2016
-18, -39, -56, -63, -49, -29, -15, -10, -14, -21, -16, -2, 10, 10, -11, -22, //2032
-19, 2, 33, 60, 64, 53, 56, 80, 110, 122, 99, 81, 81, 95, 110, 101, //2048
69, 38, 25, 35, 51, 47, 23, -5, -22, -21, 5, 17, 12, -6, -23, -31, //2064
-25, -24, -38, -64, -87, -87, -47, -26, -30, -52, -72, -73, -57, -41, -34, -38, //2080
-42, -26, -2, 20, 31, 28, 14, -12, -21, -21, -19, -17, -6, 11, 31, 46, //2096
52, 48, 17, -14, -37, -41, -29, -15, 0, 11, 20, 21, 16, 0, -14, -31, //2112
-47, -52, -44, -30, -21, -6, 14, 36, 49, 52, 42, 25, 10, 2, -4, -11, //2128
-21, -24, -17, -6, 4, 6, 5, 4, 2, 1, 0, -6, -19, -30, -32, -19, //2144
-13, -15, -22, -29, -32, -25, -18, -16, -21, -31, -40, -40, -35, -37, -53, -77, //2160
-100, -93, -78, -64, -56, -51, -39, -25, -4, 15, 23, 17, 4, 7, 19, 30, //2176
33, 24, 12, 5, 7, 18, 33, 50, 55, 57, 59, 61, 60, 44, 20, -8, //2192
-29, -37, -33, -30, -34, -41, -46, -40, -15, -2, -1, -12, -24, -25, -17, -10, //2208
-13, -26, -44, -60, -60, -58, -59, -65, -68, -55, -36, -14, 3, 12, 17, 22, //2224
30, 41, 49, 55, 60, 66, 74, 82, 88, 89, 83, 74, 61, 46, 32, 15, //2240
9, 4, -3, -11, -19, -21, -17, -13, -14, -19, -20, -6, 12, 30, 41, 44, //2256
43, 47, 53, 54, 49, 38, 26, 28, 36, 45, 48, 46, 42, 43, 45, 45, //2272
39, 26, 22, 24, 27, 27, 23, 10, 5, 1, 0, 0, -2, -1, 7, 20, //2288
32, 37, 33, 28, 22, 16, 6, -9, -35, -46, -48, -47, -45, -44, -42, -35, //2304
-24, -12, 1, 12, 17, 25, 33, 39, 40, 34, 26, 17, 6, -7, -17, -25, //2320
-25, -24, -23, -26, -35, -35, -31, -30, -34, -40, -40, -28, -13, 3, 15, 18, //2336
17, 25, 39, 49, 47, 32, 26, 28, 35, 38, 31, -4, -28, -37, -32, -25, //2352
-29, -36, -35, -25, -7, 14, 32, 28, 15, 7, 7, 13, 14, 6, -4, -8, //2368
-2, 14, 17, 14, 7, -1, -8, -10, -6, -4, -5, -5, -5, -10, -19, -27, //2384
-33, -40, -49, -51, -48, -46, -44, -38, -29, -30, -33, -35, -37, -42, -46, -42, //2400
-33, -23, -14, 1, 0, -10, -21, -28, -38, -57, -57, -44, -31, -22, -11, 12, //2416
17, 9, 0, -3, -9, -15, -16, -10, -5, -6, -4, -5, -14, -22, -17, -5, //2432
-8, -22, -25, -16, -7, -6, -4, -5, -16, -32, -38, -24, -19, -21, -19, -9, //2448
6, 27, 34, 27, 14, 7, 22, 30, 23, 9, 0, -6, -14, -13, -8, -8, //2464
-16, -23, -17, -13, -18, -26, -29, -30, -30, -28, -21, -15, -10, 6, 25, 41, //2480
52, 59, 68, 76, 70, 59, 48, 38, 24, 10, -5, -14, -10, 3, 23, 29, //2496
29, 27, 27, 34, 39, 35, 20, 1, -12, -18, -24, -35, -45, -47, -41, -23, //2512
-9, 2, 9, 12, 22, 31, 33, 25, 13, 2, -3, -5, -13, -21, -21, -14, //2528
-2, 0, -2, -8, -16, -26, -27, -28, -35, -43, -46, -26, -9, 0, 0, -3, //2544
-4, 0, 9, 19, 22, 19, 14, 16, 19, 19, 18, 14, 3, 6, 19, 34, //2560
43, 52, 66, 72, 77, 87, 96, 78, 52, 40, 47, 55, 52, 48, 49, 40, //2576
27, 26, 37, 38, 23, 17, 24, 28, 24, 31, 37, 23, -9, -38, -58, -75, //2592
-93, -98, -88, -76, -64, -50, -37, -36, -41, -35, -26, -17, -3, 13, 12, -23, //2608
-32, -15, 0, -11, -36, -43, -34, -36, -43, -41, -32, -36, -32, -13, 5, 7, //2624
3, 19, 47, 76, 94, 96, 63, 35, 29, 43, 52, 26, 7, 9, 22, 25, //2640
5, -57, -98, -122, -121, -102, -72, -58, -43, -19, 14, 47, 70, 59, 42, 31, //2656
33, 41, 34, 12, -19, -47, -61, -57, -53, -56, -63, -69, -71, -52, -18, 21, //2672
44, 50, 51, 58, 64, 68, 67, 52, 10, -14, -24, -28, -35, -44, -59, -71, //2688
-75, -58, -28, -11, -28, -31, -1, 40, 65, 69, 70, 71, 67, 65, 68, 51, //2704
15, -12, -8, 24, 58, 77, 64, 36, 5, -9, 0, -3, -21, -36, -33, -22, //2720
-12, -5, 1, -11, -47, -88, -103, -84, -62, -42, -27, -9, 8, 37, 71, 92, //2736
95, 85, 76, 58, 32, 15, 10, -4, -23, -34, -26, -17, -30, -48, -65, -84, //2752
-104, -113, -83, -55, -40, -32, -18, -4, 0, -4, -9, -20, -39, -49, -28, -4, //2768
4, 1, 2, 15, 16, 13, 12, 13, 11, 7, 17, 33, 46, 50, 49, 52, //2784
58, 63, 60, 44, 9, -5, -10, -12, -15, -12, -1, 8, 11, 7, 1, -9, //2800
-16, -25, -38, -56, -72, -84, -87, -88, -80, -61, -30, -14, -1, 7, 6, -7, //2816
-25, -19, 2, 30, 58, 77, 86, 81, 69, 53, 37, 24, 20, 10, -2, -5, //2832
5, 26, 26, 16, 3, -7, -10, 0, 9, 13, 13, 19, 33, 35, 30, 23, //2848
14, 1, -17, -17, -14, -18, -30, -42, -54, -65, -76, -81, -78, -67, -56, -41, //2864
-28, -25, -27, -24, -17, -9, 1, 11, 15, 8, 3, 8, 18, 26, 27, 26, //2880
28, 31, 35, 34, 25, 18, 14, 12, 8, 2, -14, -32, -51, -64, -67, -59, //2896
-51, -38, -19, 0, 12, 22, 24, 22, 14, 6, -1, -13, -18, -17, -14, -17, //2912
-30, -34, -33, -34, -37, -34, -11, 11, 33, 51, 66, 75, 82, 90, 94, 89, //2928
74, 47, 30, 15, 4, 0, 0, -7, -15, -21, -23, -24, -24, -22, -15, -3, //2944
12, 26, 38, 40, 36, 25, 12, 4, 5, 9, 10, 11, 12, 14, 12, 12, //2960
12, 9, 6, 10, 19, 29, 34, 32, 26, 10, 3, 4, 10, 13, 9, 9, //2976
13, 17, 11, -3, -25, -34, -34, -25, -9, 6, 16, 15, 13, 13, 13, 11, //2992
8, 3, -1, -2, 4, 28, 44, 55, 59, 60, 63, 72, 76, 74, 63, 47, //3008
22, 14, 12, 11, 3, -13, -39, -50, -55, -58, -63, -67, -72, -75, -73, -69, //3024
-61, -47, -37, -26, -16, -8, -3, -1, -8, -21, -36, -49, -59, -64, -64, -61, //3040
-57, -55, -54, -52, -47, -43, -40, -38, -30, -23, -19, -20, -22, -27, -31, -32, //3056
-32, -28, -21, -7, 0, 5, 4, 0, -3, 1, 11, 27, 45, 60, 68, 63, //3072
53, 42, 27, 8, -17, -25, -25, -19, -11, 0, 14, 16, 11, 4, -1, -1, //3088
2, 4, 9, 15, 23, 36, 39, 34, 22, 8, -5, -18, -22, -23, -22, -19, //3104
-9, 4, 19, 29, 31, 25, 17, 15, 16, 16, 15, 15, 12, 10, 7, 4, //3120
0, -12, -24, -37, -47, -54, -55, -53, -52, -48, -41, -27, -9, 15, 25, 28, //3136
24, 17, 9, 11, 16, 21, 22, 24, 30, 36, 42, 47, 48, 46, 43, 47, //3152
57, 68, 77, 78, 71, 59, 48, 39, 29, 8, -9, -22, -30, -35, -41, -50, //3168
-54, -58, -61, -58, -42, -24, -6, 11, 22, 27, 25, 20, 14, 9, 4, 5, //3184
9, 12, 13, 13, 9, -6, -21, -37, -50, -56, -54, -36, -20, -6, 4, 8, //3200
7, 3, -2, -10, -19, -24, -14, 7, 34, 57, 71, 72, 58, 48, 41, 38, //3216
35, 30, 26, 31, 43, 56, 63, 54, 35, 14, 0, -2, 5, 17, 22, 24, //3232
30, 42, 64, 70, 66, 50, 31, 18, 16, 22, 22, 13, -3, -20, -28, -22, //3248
-17, -22, -37, -62, -68, -59, -43, -29, -28, -44, -54, -54, -45, -35, -35, -67, //3264
-96, -116, -119, -107, -87, -85, -93, -100, -97, -81, -44, -32, -32, -40, -42, -20, };