const unsigned int sampleRate = 22050;
const unsigned int sampleCount = 3423;
const signed char samples[] = {//0
0, 0, 1, 1, 0, -1, 0, 2, 4, 8, 10, 12, 12, 13, 11, 11, //16
12, 10, 8, 6, 3, 3, 7, 15, 20, 23, 27, 30, 27, 22, 21, 17, //32
15, 11, 13, 16, 18, 18, 16, 13, 12, 11, 11, 7, 0, -4, -7, -7, //48
-7, -3, 1, 5, 8, 10, 10, 8, 5, -2, -4, -7, -1, 0, 9, 15, //64
14, 15, 16, 15, 11, 9, 5, 3, 0, 4, 6, 8, 11, 9, 5, 6, //80
7, 7, 4, 2, -5, -10, -14, -13, -12, -9, -11, -12, -12, -9, -9, -9, //96
-12, -11, -12, -19, -21, -21, -19, -25, -30, -30, -26, -25, -27, -30, -30, -33, //112
-37, -35, -26, -21, -20, -18, -18, -20, -24, -24, -25, -24, -22, -22, -21, -14, //128
-5, 4, 11, 22, 26, 29, 30, 37, 41, 48, 50, 48, 48, 51, 60, 64, //144
66, 66, 64, 58, 53, 51, 56, 58, 63, 64, 64, 64, 65, 68, 71, 76, //160
78, 80, 80, 81, 79, 77, 77, 76, 73, 74, 69, 64, 58, 57, 57, 59, //176
61, 62, 56, 55, 50, 42, 33, 25, 17, 12, 7, 9, 11, 16, 16, 13, //192
6, -5, -11, -17, -24, -28, -26, -25, -23, -24, -24, -31, -38, -46, -53, -60, //208
-63, -64, -63, -62, -60, -56, -52, -48, -50, -49, -45, -44, -42, -33, -26, -25, //224
-26, -26, -26, -23, -21, -24, -25, -27, -35, -37, -32, -31, -24, -22, -19, -21, //240
-23, -25, -24, -25, -27, -30, -32, -33, -34, -33, -29, -30, -28, -30, -33, -45, //256
-55, -66, -65, -63, -55, -41, -28, -16, -12, -12, -17, -27, -37, -50, -63, -68, //272
-65, -57, -51, -39, -22, -10, -3, -4, -2, -2, -2, -14, -25, -32, -36, -38, //288
-34, -20, -14, -4, 2, 8, 9, 5, -3, -4, -8, -9, -12, -9, -3, 2, //304
6, 12, 25, 28, 30, 28, 22, 20, 23, 22, 22, 28, 35, 41, 39, 36, //320
32, 30, 25, 16, 12, 15, 12, 10, 11, 18, 23, 26, 22, 16, 5, -2, //336
-17, -25, -34, -35, -36, -32, -29, -22, -18, -17, -22, -23, -26, -35, -47, -62, //352
-65, -64, -60, -52, -44, -41, -36, -41, -39, -41, -42, -50, -54, -57, -55, -51, //368
-43, -40, -30, -20, -14, -11, -2, -1, -1, 3, 12, 24, 31, 39, 44, 51, //384
59, 64, 59, 54, 49, 49, 54, 62, 63, 67, 69, 71, 79, 88, 93, 95, //400
96, 92, 91, 92, 97, 96, 95, 96, 95, 90, 88, 91, 93, 92, 93, 86, //416
78, 69, 62, 58, 54, 49, 44, 42, 35, 28, 23, 21, 15, 7, 1, -4, //432
-15, -18, -23, -32, -36, -33, -34, -39, -41, -42, -42, -42, -47, -49, -50, -53, //448
-57, -58, -57, -50, -46, -42, -43, -40, -36, -31, -29, -26, -26, -27, -30, -33, //464
-30, -22, -15, -12, -14, -11, -6, -4, -3, -2, 0, -3, -1, 3, 3, 3, //480
8, 12, 14, 14, 10, 9, 4, -2, -5, -5, -11, -12, -16, -14, -13, -6, //496
-4, 1, 3, 1, 3, 1, -4, -6, -4, -3, 1, 4, 4, -9, -12, -21, //512
-26, -26, -21, -14, -15, -15, -14, -17, -17, -18, -16, -17, -21, -30, -39, -39, //528
-39, -40, -40, -37, -37, -37, -34, -32, -35, -36, -35, -36, -39, -31, -27, -13, //544
-8, -3, -1, 1, -1, 0, -1, 4, 11, 14, 19, 27, 25, 26, 25, 27, //560
26, 28, 27, 28, 24, 19, 11, 6, 2, -1, -6, -6, -6, -9, -12, -19, //576
-22, -27, -30, -33, -33, -36, -38, -40, -42, -44, -44, -36, -38, -32, -27, -19, //592
-19, -16, -21, -15, -12, -10, -10, -5, 7, 13, 14, 16, 15, 17, 20, 25, //608
27, 32, 31, 31, 29, 26, 21, 22, 22, 23, 24, 20, 18, 18, 24, 25, //624
36, 38, 45, 43, 34, 26, 30, 33, 31, 31, 31, 32, 31, 31, 29, 29, //640
27, 28, 26, 26, 23, 23, 23, 25, 28, 23, 21, 11, 2, -2, -7, -15, //656
-24, -29, -33, -40, -43, -52, -55, -55, -54, -50, -46, -42, -37, -40, -40, -40, //672
-36, -36, -33, -34, -35, -31, -26, -14, -9, -2, 7, 12, 11, 11, 10, 7, //688
13, 16, 24, 30, 37, 39, 48, 51, 57, 60, 63, 59, 54, 45, 36, 35, //704
34, 30, 29, 19, 16, 7, 0, -11, -18, -27, -28, -25, -22, -15, -11, -17, //720
-25, -31, -36, -40, -41, -35, -33, -26, -22, -18, -18, -17, -18, -32, -42, -49, //736
-51, -47, -41, -38, -37, -30, -25, -22, -19, -13, -9, -13, -18, -22, -29, -33, //752
-31, -25, -23, -18, -16, -16, -14, -8, -8, -6, -7, -5, -8, -6, 4, 13, //768
26, 37, 39, 41, 37, 31, 24, 7, -2, -4, -3, 0, 6, 17, 21, 22, //784
25, 22, 15, 7, 0, -11, -17, -26, -34, -38, -40, -39, -34, -27, -25, -29, //800
-36, -40, -43, -41, -38, -39, -40, -31, -25, -8, 5, 9, 9, 9, 11, 2, //816
-1, 2, 5, 5, 14, 20, 27, 35, 43, 42, 38, 27, 14, 6, 7, 10, //832
25, 32, 35, 42, 45, 36, 34, 34, 29, 22, 19, 7, -1, -3, -2, 0, //848
14, 29, 31, 35, 41, 43, 35, 29, 27, 27, 29, 30, 52, 70, 79, 78, //864
79, 75, 58, 56, 55, 54, 58, 49, 47, 49, 55, 62, 69, 73, 71, 61, //880
49, 39, 26, 23, 17, 19, 17, 22, 34, 36, 46, 50, 37, 25, 6, -8, //896
-4, -3, -5, -12, -22, -25, -28, -24, -18, -19, -19, -23, -34, -45, -53, -54, //912
-44, -42, -38, -36, -30, -27, -26, -29, -27, -28, -41, -43, -43, -34, -37, -36, //928
-31, -23, -14, -1, 8, 19, 15, 8, 5, 2, 3, 20, 32, 40, 49, 51, //944
54, 52, 40, 35, 38, 41, 36, 42, 51, 50, 45, 38, 38, 30, 16, -1, //960
-5, -9, -19, -28, -34, -40, -41, -45, -38, -32, -34, -38, -43, -51, -59, -63, //976
-63, -62, -62, -56, -54, -55, -57, -61, -51, -47, -52, -55, -54, -54, -51, -47, //992
-40, -36, -31, -20, -7, 2, 13, 17, 17, 13, 13, 17, 17, 17, 15, 13, //1008
11, 16, 17, 25, 32, 38, 33, 32, 31, 31, 30, 31, 26, 22, 22, 20, //1024
11, 10, 10, 9, 2, -13, -20, -20, -27, -34, -37, -36, -39, -41, -44, -50, //1040
-50, -58, -63, -65, -65, -68, -65, -63, -56, -47, -42, -42, -45, -45, -46, -47, //1056
-48, -47, -32, -24, -15, -6, -1, -1, -5, -10, -17, -22, -27, -26, -23, -13, //1072
-6, 1, -1, -3, -3, -3, -8, -10, -6, -2, 1, -1, 7, 13, 17, 16, //1088
13, 11, 10, 9, 4, 4, 5, 7, 11, 14, 22, 28, 35, 37, 40, 49, //1104
52, 53, 59, 60, 61, 55, 47, 43, 45, 47, 49, 49, 49, 41, 32, 27, //1120
25, 25, 24, 16, 12, 13, 11, 10, 10, 14, 17, 8, 1, -5, -8, -11, //1136
-13, -11, -5, 3, 8, 10, 14, 21, 17, 9, 5, 1, 2, -1, -4, -2, //1152
4, 9, 7, 2, -2, -8, -17, -27, -42, -42, -42, -45, -48, -44, -47, -52, //1168
-63, -73, -77, -83, -89, -96, -95, -94, -94, -93, -90, -88, -86, -86, -90, -89, //1184
-83, -74, -67, -64, -56, -50, -46, -44, -37, -24, -17, -11, -9, -8, -4, 3, //1200
10, 21, 28, 32, 34, 35, 33, 33, 32, 29, 24, 25, 24, 23, 21, 20, //1216
24, 29, 31, 32, 31, 28, 15, 4, -3, -6, -5, 3, 11, 19, 21, 25, //1232
26, 27, 25, 21, 18, 17, 15, 17, 23, 31, 38, 37, 30, 28, 21, 14, //1248
7, 2, 5, 9, 14, 22, 31, 38, 48, 51, 49, 46, 48, 42, 39, 42, //1264
49, 52, 54, 50, 50, 46, 42, 39, 38, 33, 30, 25, 20, 12, 4, 3, //1280
5, 5, 8, 11, 15, 13, 8, 2, -5, -14, -26, -29, -30, -29, -28, -27, //1296
-27, -27, -28, -30, -31, -28, -29, -34, -40, -43, -40, -39, -36, -36, -31, -27, //1312
-24, -26, -22, -17, -17, -17, -10, -4, 3, 9, 16, 23, 20, 12, 5, -4, //1328
-8, -17, -18, -13, -7, -3, 4, 10, 13, 16, 15, 12, 11, 10, 8, 3, //1344
-1, 1, 10, 18, 23, 27, 30, 28, 21, 14, 13, 13, 18, 24, 32, 35, //1360
37, 41, 42, 47, 44, 42, 37, 33, 29, 31, 30, 30, 28, 25, 19, 15, //1376
13, 16, 19, 16, 11, 8, 6, 4, 2, 0, -1, -3, -5, -6, -10, -10, //1392
-5, -2, 2, 3, 4, 1, 3, 5, 11, 16, 23, 33, 40, 44, 46, 48, //1408
50, 52, 55, 57, 60, 64, 70, 73, 75, 76, 77, 80, 80, 80, 78, 75, //1424
69, 67, 63, 57, 55, 51, 46, 37, 32, 25, 21, 12, 2, -6, -6, -5, //1440
-6, -11, -14, -18, -22, -31, -33, -35, -33, -33, -40, -49, -53, -53, -56, -58, //1456
-58, -58, -58, -57, -55, -51, -48, -42, -37, -28, -24, -22, -18, -13, -5, 1, //1472
5, 6, 6, 6, 1, 0, 0, 0, 2, 8, 22, 32, 39, 43, 45, 36, //1488
28, 20, 14, 8, 3, -3, -1, 3, 5, 7, 9, 12, 10, 8, 6, 3, //1504
-8, -13, -19, -20, -21, -18, -12, -8, -12, -16, -20, -24, -31, -35, -37, -39, //1520
-39, -37, -39, -40, -45, -47, -55, -64, -65, -67, -69, -69, -68, -71, -70, -72, //1536
-74, -79, -88, -92, -97, -104, -106, -107, -101, -93, -81, -68, -57, -49, -45, -47, //1552
-47, -48, -51, -43, -39, -30, -24, -15, -7, 3, 5, 5, 1, -3, -8, -9, //1568
-4, 4, 16, 25, 40, 45, 46, 44, 40, 34, 22, 10, 3, 1, 2, 7, //1584
16, 27, 29, 30, 22, 4, -14, -33, -48, -56, -55, -44, -34, -24, -9, 5, //1600
11, 13, 6, -3, -22, -37, -56, -59, -57, -47, -34, -18, 7, 20, 24, 22, //1616
15, 0, -12, -24, -28, -30, -22, 1, 15, 31, 39, 42, 40, 35, 31, 25, //1632
15, 8, 4, 5, 13, 21, 32, 42, 52, 53, 52, 45, 37, 26, 18, 18, //1648
19, 24, 28, 37, 40, 43, 52, 51, 45, 32, 31, 29, 29, 25, 22, 24, //1664
23, 22, 23, 25, 19, 9, -4, -14, -18, -21, -22, -21, -19, -12, -6, -6, //1680
-6, -5, -3, -7, -11, -22, -27, -37, -39, -37, -29, -21, -12, -7, 1, 4, //1696
3, -5, -4, -5, -9, -11, -4, -1, 4, 8, 19, 26, 36, 40, 39, 35, //1712
38, 39, 37, 41, 46, 48, 49, 52, 51, 50, 54, 55, 51, 52, 51, 44, //1728
37, 35, 35, 26, 20, 11, 6, 2, 3, -4, -8, -10, -12, -19, -24, -25, //1744
-19, -15, -12, -18, -19, -21, -31, -42, -51, -57, -60, -61, -63, -62, -63, -63, //1760
-66, -65, -64, -66, -73, -74, -75, -69, -64, -60, -56, -56, -56, -56, -54, -49, //1776
-53, -55, -52, -51, -52, -51, -44, -35, -21, -16, -7, 0, 8, 7, 17, 26, //1792
36, 38, 39, 36, 37, 33, 44, 54, 65, 72, 78, 77, 76, 71, 67, 63, //1808
63, 65, 64, 61, 60, 58, 60, 60, 61, 57, 54, 51, 41, 34, 34, 37, //1824
32, 31, 26, 15, 9, 4, -2, -12, -19, -24, -25, -24, -19, -13, -11, -11, //1840
-18, -27, -34, -37, -41, -37, -31, -25, -19, -14, -11, -3, 3, 13, 23, 33, //1856
34, 35, 31, 29, 28, 23, 24, 27, 30, 29, 33, 31, 31, 28, 24, 19, //1872
11, 9, 8, 12, 17, 21, 27, 30, 31, 26, 21, 14, 6, -1, -11, -16, //1888
-23, -23, -26, -25, -22, -15, -10, -9, -8, -13, -17, -19, -16, -14, -13, -8, //1904
0, 3, 13, 18, 17, 15, 13, 6, 2, 1, 2, 9, 12, 21, 26, 28, //1920
23, 19, 12, -1, -11, -15, -24, -25, -28, -29, -25, -22, -17, -11, -9, -13, //1936
-12, -16, -23, -30, -31, -26, -24, -18, -14, -6, -6, -5, -3, -4, -2, -1, //1952
0, -3, 2, 3, 8, 7, 12, 15, 20, 23, 18, 15, 10, 2, -2, -9, //1968
-10, -16, -21, -25, -28, -37, -45, -45, -46, -49, -50, -46, -42, -45, -42, -42, //1984
-42, -48, -57, -60, -65, -66, -70, -58, -50, -42, -35, -27, -14, -9, -4, -1, //2000
0, -5, -10, -9, -1, 7, 17, 27, 46, 51, 53, 50, 46, 28, 16, 8, //2016
5, 6, 9, 18, 29, 35, 48, 53, 60, 50, 38, 24, 12, -5, -14, -20, //2032
-22, -20, -17, -19, -27, -35, -43, -58, -63, -69, -73, -74, -73, -72, -67, -65, //2048
-63, -58, -54, -49, -44, -32, -28, -23, -17, -12, -12, -20, -22, -26, -28, -27, //2064
-24, -15, -11, -7, -8, 0, 12, 18, 30, 38, 46, 47, 48, 41, 45, 51, //2080
52, 50, 55, 54, 50, 49, 48, 52, 56, 54, 46, 43, 37, 25, 19, 19, //2096
22, 25, 31, 33, 32, 30, 24, 16, 11, 13, 2, -7, -20, -30, -33, -26, //2112
-20, -10, -3, 0, -8, -14, -20, -25, -29, -31, -39, -42, -41, -37, -28, -11, //2128
0, 5, 9, 10, 6, 0, -3, -8, -8, 1, 13, 23, 35, 47, 54, 62, //2144
67, 67, 65, 61, 51, 35, 33, 35, 38, 39, 47, 59, 63, 57, 55, 54, //2160
52, 47, 42, 39, 34, 31, 42, 51, 62, 66, 63, 52, 43, 38, 34, 29, //2176
24, 21, 11, 2, -3, -5, -5, -4, -5, -8, -14, -26, -40, -49, -43, -34, //2192
-24, -16, -12, -17, -23, -28, -32, -36, -35, -31, -30, -29, -29, -22, -19, -10, //2208
-4, 4, 8, 4, -6, -14, -18, -22, -17, -9, -2, 6, 13, 17, 24, 21, //2224
16, 9, 4, 0, -6, -9, -7, -2, 0, 2, 4, 9, 11, 7, 4, -5, //2240
-14, -23, -25, -30, -29, -24, -17, -17, -17, -24, -33, -53, -60, -65, -61, -53, //2256
-40, -35, -38, -41, -44, -40, -32, -23, -16, -17, -23, -38, -48, -48, -42, -38, //2272
-29, -21, -16, -18, -17, -14, -7, 1, 11, 19, 23, 28, 25, 18, 18, 22, //2288
27, 33, 36, 32, 30, 29, 25, 18, 14, 13, 12, 12, 10, 4, 1, -1, //2304
-2, 1, 5, 7, 3, 0, -4, -5, -9, -17, -25, -31, -36, -36, -34, -29, //2320
-28, -33, -34, -35, -32, -20, -3, 10, 19, 20, 9, -3, -11, -21, -20, -19, //2336
-9, -8, -10, -14, -17, -18, -12, -7, 1, 1, 1, -4, -6, -11, -12, -13, //2352
3, 11, 17, 17, 24, 22, 20, 23, 27, 32, 36, 37, 30, 31, 32, 36, //2368
37, 40, 46, 47, 46, 43, 49, 51, 53, 45, 40, 36, 35, 34, 37, 36, //2384
28, 20, 12, 10, 11, 12, 17, 14, 6, -2, -11, -16, -21, -25, -31, -32, //2400
-31, -39, -47, -54, -54, -50, -44, -37, -29, -23, -24, -27, -31, -33, -31, -32, //2416
-24, -16, -10, -6, -9, -10, -8, -7, -5, 0, 4, 5, 2, 0, 2, 5, //2432
15, 17, 16, 15, 15, 11, 8, 7, 6, 10, 13, 17, 17, 19, 18, 15, //2448
12, 10, 8, 8, 10, 11, 20, 25, 32, 36, 35, 32, 28, 24, 17, 13, //2464
3, -4, -9, -10, -15, -17, -21, -25, -29, -31, -33, -30, -30, -31, -39, -45, //2480
-50, -50, -57, -63, -63, -62, -61, -59, -46, -36, -28, -24, -25, -28, -35, -44, //2496
-48, -50, -40, -23, -10, 2, 21, 29, 33, 37, 38, 32, 22, 11, 9, 13, //2512
21, 29, 40, 53, 70, 75, 73, 69, 64, 62, 51, 46, 45, 47, 52, 53, //2528
62, 67, 66, 65, 62, 54, 46, 39, 35, 31, 31, 33, 36, 40, 41, 36, //2544
30, 31, 30, 26, 22, 12, -1, -10, -14, -18, -11, -3, 5, 3, -1, -4, //2560
-13, -22, -26, -21, -22, -28, -32, -37, -36, -36, -34, -25, -17, -11, -15, -24, //2576
-35, -42, -42, -44, -35, -25, -14, -16, -18, -20, -22, -28, -34, -42, -45, -45, //2592
-41, -33, -20, -14, 0, 4, 2, -1, -5, -14, -23, -31, -33, -34, -34, -22, //2608
-7, 5, 16, 18, 14, -6, -17, -25, -34, -37, -29, -19, -8, 2, 12, 19, //2624
23, 20, 13, 13, 11, 16, 9, 6, 5, 3, 8, 20, 24, 27, 23, 16, //2640
1, -12, -16, -22, -22, -23, -26, -28, -29, -28, -30, -34, -41, -44, -48, -53, //2656
-59, -61, -58, -56, -53, -54, -48, -41, -37, -43, -42, -46, -46, -51, -42, -36, //2672
-23, -17, -9, -1, 8, 14, 21, 25, 26, 25, 26, 25, 23, 19, 22, 26, //2688
33, 39, 48, 58, 62, 55, 52, 48, 47, 43, 45, 49, 56, 64, 69, 66, //2704
65, 62, 62, 63, 68, 69, 68, 68, 65, 62, 60, 61, 58, 54, 46, 36, //2720
22, 15, 6, -2, -11, -17, -25, -26, -30, -37, -45, -58, -69, -79, -87, -88, //2736
-84, -73, -68, -63, -65, -64, -66, -70, -71, -71, -73, -76, -92, -98, -102, -102, //2752
-94, -87, -77, -76, -74, -72, -69, -65, -61, -57, -47, -39, -30, -18, -11, -9, //2768
-4, -2, 2, 7, 7, 3, 0, 1, 5, 18, 26, 35, 40, 41, 33, 31, //2784
29, 29, 29, 33, 40, 44, 49, 51, 53, 52, 52, 49, 47, 45, 41, 44, //2800
45, 48, 49, 48, 45, 40, 37, 35, 31, 25, 17, 13, 8, 3, 4, 9, //2816
15, 17, 17, 17, 16, 11, 3, -2, -13, -17, -25, -30, -32, -33, -37, -39, //2832
-41, -44, -47, -48, -54, -60, -63, -59, -51, -45, -41, -40, -39, -39, -41, -42, //2848
-40, -42, -42, -40, -33, -22, -7, 1, 10, 21, 27, 33, 34, 33, 27, 22, //2864
16, 15, 18, 29, 37, 46, 49, 51, 51, 55, 54, 56, 54, 51, 48, 48, //2880
46, 45, 40, 40, 41, 41, 41, 40, 36, 35, 27, 23, 22, 22, 19, 14, //2896
13, 10, 6, 4, 2, -3, -5, -5, 1, 9, 14, 13, 10, 3, 0, -6, //2912
-13, -20, -22, -22, -16, -11, 0, 7, 11, 16, 15, 16, 14, 14, 12, 10, //2928
8, 7, 10, 10, 10, 14, 20, 29, 37, 38, 33, 26, 10, 1, -5, -6, //2944
-8, -11, -17, -16, -18, -16, -16, -16, -19, -21, -21, -21, -19, -17, -20, -28, //2960
-28, -32, -28, -30, -30, -32, -37, -40, -41, -39, -41, -40, -41, -39, -40, -37, //2976
-38, -39, -42, -47, -48, -45, -40, -34, -28, -18, -10, -6, -7, -8, -9, -4, //2992
-1, 3, 5, 4, 1, 3, 5, 11, 12, 16, 17, 21, 23, 24, 20, 20, //3008
15, 14, 13, 19, 23, 28, 27, 27, 30, 37, 41, 43, 43, 44, 44, 45, //3024
45, 43, 42, 38, 36, 35, 36, 30, 22, 16, 15, 16, 15, 19, 21, 19, //3040
20, 19, 18, 14, 11, 5, 3, 7, 11, 9, 5, 2, -1, 1, 5, 9, //3056
9, 8, 3, -4, -6, -3, -4, -1, -2, -4, -1, 5, 10, 11, 12, 13, //3072
17, 19, 19, 14, 8, 8, 9, 14, 18, 26, 31, 35, 35, 37, 34, 28, //3088
20, 10, 2, 0, 2, 6, 9, 15, 19, 26, 31, 37, 36, 38, 36, 33, //3104
24, 22, 25, 27, 27, 26, 21, 16, 14, 10, 8, 9, 5, 0, -5, -9, //3120
-10, -15, -17, -19, -18, -17, -14, -12, -11, -13, -16, -19, -19, -18, -15, -13, //3136
-16, -21, -28, -29, -32, -33, -37, -36, -38, -39, -36, -36, -35, -32, -31, -29, //3152
-28, -28, -29, -23, -23, -21, -22, -20, -23, -19, -11, -4, 8, 16, 22, 25, //3168
24, 30, 31, 40, 46, 49, 52, 56, 59, 64, 71, 74, 76, 80, 81, 83, //3184
83, 85, 83, 86, 84, 86, 86, 85, 82, 81, 77, 75, 68, 63, 52, 45, //3200
37, 25, 17, 3, -6, -20, -28, -37, -40, -48, -55, -63, -68, -70, -70, -75, //3216
-82, -89, -95, -98, -102, -107, -114, -117, -115, -113, -109, -105, -106, -108, -107, -106, //3232
-104, -103, -101, -98, -94, -90, -84, -78, -69, -67, -63, -59, -54, -46, -36, -30, //3248
-26, -19, -15, -4, 0, 1, 3, 5, 11, 18, 21, 24, 30, 35, 40, 45, //3264
53, 55, 57, 57, 59, 63, 66, 70, 74, 79, 82, 84, 81, 82, 81, 77, //3280
67, 65, 64, 62, 59, 54, 53, 49, 46, 39, 30, 18, 6, -3, -12, -15, //3296
-20, -24, -31, -33, -41, -42, -55, -62, -66, -72, -81, -84, -93, -99, -104, -110, //3312
-116, -122, -124, -127, -125, -123, -118, -112, -106, -105, -102, -100, -94, -93, -92, -88, //3328
-83, -79, -74, -69, -67, -59, -56, -49, -36, -27, -18, -11, -7, -4, 0, 0, //3344
5, 5, 12, 19, 33, 36, 43, 46, 49, 56, 59, 58, 59, 56, 50, 48, //3360
50, 54, 58, 63, 67, 71, 72, 71, 68, 63, 54, 45, 36, 30, 27, 23, //3376
23, 28, 31, 35, 34, 34, 29, 23, 9, 1, -13, -24, -31, -35, -35, -28, //3392
-19, -11, -10, -7, -10, -16, -28, -32, -33, -34, -35, -36, -31, -24, -19, -13, //3408
-11, -10, -18, -22, -24, -22, -21, -16, -13, -10, -5, -2, 0, 1, 0, };
