const unsigned int sampleRate = 22050;
const unsigned int sampleCount = 3521;
const signed char samples[] = {//0
0, -1, 0, 2, 6, 15, 35, 52, 69, 80, 79, 72, 75, 74, 65, 53, //16
41, 20, 11, 0, -13, -19, -19, -26, -35, -41, -42, -43, -43, -39, -32, -25, //32
-17, -10, -16, -23, -22, -18, -17, -15, -15, -7, 4, 11, 13, 17, 20, 17, //48
15, 7, -4, -12, -20, -32, -45, -56, -65, -74, -78, -81, -86, -87, -89, -96, //64
-96, -92, -88, -82, -74, -74, -75, -72, -67, -63, -56, -47, -38, -30, -19, -13, //80
-8, 3, 5, -1, -3, -7, -12, -14, -20, -29, -32, -29, -28, -26, -22, -18, //96
-18, -21, -22, -23, -28, -28, -24, -21, -21, -16, -4, 3, 2, 4, 12, 21, //112
24, 23, 28, 33, 39, 51, 59, 62, 67, 68, 60, 45, 34, 19, 7, -4, //128
-12, -19, -22, -29, -33, -31, -26, -18, -7, -1, 6, 13, 22, 32, 46, 54, //144
61, 68, 78, 94, 102, 108, 109, 114, 118, 121, 125, 126, 126, 126, 127, 118, //160
110, 101, 90, 78, 61, 52, 45, 40, 34, 29, 30, 33, 36, 39, 44, 50, //176
64, 73, 80, 81, 83, 90, 94, 98, 97, 93, 90, 87, 87, 86, 82, 79, //192
74, 64, 62, 60, 55, 48, 41, 37, 31, 23, 15, 7, 4, 5, 4, -3, //208
-11, -13, -6, -4, -4, -4, -6, -13, -17, -18, -13, -6, -1, 8, 12, 18, //224
27, 31, 32, 36, 43, 48, 46, 41, 37, 34, 25, 9, -3, -10, -17, -18, //240
-18, -22, -26, -24, -20, -22, -25, -25, -21, -3, 7, 14, 24, 37, 46, 49, //256
46, 44, 49, 53, 51, 37, 31, 32, 31, 23, 11, 5, 2, -5, -20, -35, //272
-54, -62, -75, -87, -87, -84, -76, -78, -85, -92, -95, -87, -81, -76, -64, -48, //288
-32, -12, 0, 9, 19, 27, 36, 41, 43, 44, 45, 46, 44, 47, 52, 53, //304
53, 53, 46, 42, 37, 28, 16, 0, -6, -12, -19, -25, -31, -35, -39, -43, //320
-44, -42, -36, -21, -10, 3, 19, 35, 54, 66, 74, 79, 87, 95, 96, 94, //336
98, 104, 105, 98, 82, 73, 61, 43, 27, 10, 0, -9, -16, -26, -33, -29, //352
-26, -23, -25, -30, -30, -35, -40, -41, -41, -41, -40, -32, -35, -40, -28, -15, //368
-6, 0, 6, 20, 31, 40, 54, 57, 60, 61, 57, 44, 35, 35, 28, 13, //384
-2, -19, -23, -29, -34, -33, -30, -22, -22, -21, -12, -6, 10, 21, 30, 43, //400
52, 62, 68, 68, 68, 61, 51, 50, 45, 39, 37, 40, 40, 22, 11, 0, //416
-5, -7, -22, -34, -42, -48, -43, -51, -64, -65, -62, -55, -53, -55, -47, -39, //432
-36, -34, -32, -28, -10, 2, 13, 23, 37, 51, 51, 53, 51, 49, 48, 41, //448
35, 24, 11, 1, -12, -30, -41, -48, -61, -75, -80, -78, -70, -59, -50, -43, //464
-31, -28, -23, -10, 0, 2, 17, 28, 27, 30, 41, 52, 47, 45, 47, 40, //480
35, 33, 32, 34, 32, 36, 41, 34, 28, 21, 13, 1, -17, -26, -40, -48, //496
-52, -60, -67, -69, -74, -69, -55, -48, -40, -31, -23, -15, -5, 5, 17, 26, //512
28, 28, 29, 33, 33, 31, 23, 15, 9, 5, 0, -8, -15, -19, -28, -42, //528
-47, -47, -48, -50, -54, -56, -57, -58, -65, -69, -66, -68, -65, -60, -60, -50, //544
-41, -36, -28, -18, -8, 11, 22, 28, 26, 26, 33, 35, 28, 21, 16, 16, //560
7, -2, -13, -27, -41, -55, -66, -71, -79, -83, -83, -81, -83, -84, -86, -88, //576
-86, -81, -79, -71, -55, -41, -32, -13, -3, 4, 19, 26, 33, 43, 50, 51, //592
48, 51, 57, 60, 56, 47, 39, 35, 30, 23, 14, 2, -10, -14, -16, -23, //608
-22, -24, -33, -42, -48, -50, -48, -41, -37, -37, -30, -23, -14, -4, -1, 6, //624
18, 30, 33, 32, 41, 44, 45, 39, 31, 26, 15, 6, -7, -23, -34, -44, //640
-52, -61, -65, -65, -64, -67, -73, -79, -89, -97, -100, -96, -88, -78, -63, -47, //656
-38, -26, -12, -3, 7, 16, 21, 26, 30, 32, 33, 30, 31, 32, 22, 10, //672
-5, -21, -33, -49, -61, -71, -78, -75, -69, -63, -58, -55, -49, -44, -43, -37, //688
-32, -25, -16, -10, -3, -3, -1, 5, 12, 20, 27, 31, 34, 40, 46, 50, //704
53, 51, 48, 45, 40, 26, 8, -7, -17, -27, -36, -46, -49, -45, -43, -42, //720
-43, -45, -46, -48, -42, -34, -30, -22, -13, -2, 16, 30, 45, 52, 51, 55, //736
57, 53, 49, 47, 50, 53, 56, 60, 56, 50, 44, 29, 23, 16, 9, 3, //752
-12, -21, -25, -30, -36, -39, -36, -34, -34, -29, -19, -14, -11, -5, -5, -3, //768
1, 9, 13, 17, 26, 32, 36, 43, 46, 46, 42, 35, 31, 25, 17, 7, //784
-3, -14, -25, -29, -31, -34, -38, -37, -35, -36, -39, -42, -39, -31, -22, -17, //800
-13, -12, -11, -11, -7, 2, 11, 23, 34, 38, 35, 37, 40, 37, 35, 37, //816
36, 29, 15, -2, -21, -31, -41, -49, -56, -60, -55, -46, -40, -40, -36, -23, //832
-8, -5, 0, 12, 26, 35, 40, 45, 52, 63, 71, 79, 84, 89, 96, 94, //848
85, 87, 93, 93, 92, 88, 82, 81, 72, 61, 49, 37, 22, 13, -1, -16, //864
-29, -41, -55, -56, -55, -53, -46, -38, -31, -20, -10, 6, 26, 41, 50, 64, //880
78, 88, 90, 87, 89, 96, 99, 96, 96, 101, 102, 97, 87, 79, 70, 63, //896
52, 36, 20, 5, -2, -11, -16, -18, -18, -19, -20, -21, -17, -13, -13, -10, //912
-2, 10, 19, 23, 36, 43, 45, 44, 44, 48, 58, 61, 59, 56, 51, 45, //928
30, 18, 13, 10, 7, -14, -25, -33, -42, -49, -58, -56, -48, -43, -37, -32, //944
-27, -11, 2, 7, 15, 28, 37, 35, 35, 39, 43, 43, 42, 45, 45, 46, //960
51, 54, 56, 59, 59, 60, 57, 44, 39, 35, 31, 27, 21, 8, 1, -5, //976
-12, -19, -27, -30, -32, -37, -34, -25, -15, -4, 2, 3, 10, 18, 20, 21, //992
25, 25, 24, 26, 28, 31, 31, 33, 35, 26, 16, 10, 8, 4, -4, -7, //1008
-9, -11, -8, -4, -6, -13, -17, -22, -25, -27, -34, -38, -32, -20, -8, 5, //1024
21, 31, 40, 46, 46, 44, 44, 36, 28, 25, 23, 13, 3, -6, -11, -11, //1040
-18, -32, -34, -33, -36, -48, -65, -67, -64, -62, -64, -62, -53, -45, -39, -39, //1056
-35, -28, -22, -18, -12, -12, -10, -2, -3, -4, -7, -8, -13, -18, -16, -16, //1072
-14, -14, -16, -18, -21, -23, -25, -28, -35, -40, -43, -44, -45, -49, -57, -57, //1088
-57, -51, -48, -49, -45, -42, -42, -39, -28, -16, -10, -12, -11, -9, -11, -15, //1104
-20, -23, -26, -27, -27, -33, -34, -32, -28, -25, -29, -28, -23, -25, -33, -37, //1120
-40, -41, -39, -38, -37, -40, -49, -45, -43, -50, -55, -54, -51, -46, -42, -40, //1136
-35, -21, -18, -15, -16, -18, -19, -19, -19, -20, -14, -11, -8, -12, -17, -15, //1152
-17, -21, -21, -22, -21, -22, -22, -18, -17, -13, -11, -12, -17, -21, -22, -27, //1168
-26, -18, -14, -5, 0, 3, 5, 0, 2, 5, 4, 3, 7, 18, 30, 32, //1184
26, 18, 12, 8, 0, -3, -4, -4, -7, -12, -14, -16, -15, -19, -25, -22, //1200
-15, -8, -5, -4, 1, 1, 1, 7, 10, 14, 25, 29, 27, 27, 30, 33, //1216
36, 31, 31, 38, 46, 49, 48, 46, 40, 29, 15, -8, -20, -32, -41, -44, //1232
-49, -53, -48, -40, -31, -27, -25, -19, -13, -12, -9, -5, -1, 8, 18, 24, //1248
24, 27, 29, 24, 14, 2, -7, -12, -16, -20, -25, -26, -25, -27, -33, -42, //1264
-46, -42, -42, -47, -48, -47, -47, -47, -44, -36, -29, -22, -14, -11, -9, -3, //1280
-1, -4, -7, -8, -8, -5, 0, 2, -1, -2, 7, 12, 15, 16, 20, 26, //1296
23, 16, 8, 2, -3, -11, -15, -12, -10, -14, -15, -16, -19, -17, -16, -16, //1312
-14, -8, -1, 3, 0, -3, 3, 6, 6, 4, 5, 9, 6, 1, -3, -1, //1328
4, 10, 14, 15, 15, 17, 17, 9, 2, 0, 2, 5, 11, 13, 12, 10, //1344
10, 11, 7, 8, 13, 23, 31, 35, 35, 35, 41, 51, 56, 60, 65, 66, //1360
67, 66, 61, 53, 46, 38, 27, 21, 22, 16, 4, -11, -20, -24, -35, -44, //1376
-50, -52, -49, -44, -45, -47, -44, -39, -29, -18, -9, -4, 3, 9, 15, 22, //1392
29, 38, 50, 60, 61, 60, 62, 64, 59, 53, 51, 51, 46, 40, 35, 31, //1408
20, 5, -9, -21, -30, -32, -28, -28, -26, -21, -16, -14, -11, -2, 5, 11, //1424
16, 22, 25, 30, 37, 38, 37, 43, 47, 48, 47, 45, 45, 49, 46, 38, //1440
31, 27, 15, 10, 6, 3, 3, 0, -11, -19, -27, -34, -37, -38, -35, -33, //1456
-30, -24, -20, -14, -4, 4, 12, 20, 22, 23, 28, 30, 28, 26, 27, 31, //1472
34, 33, 29, 23, 19, 15, 13, 10, 6, 4, 3, -4, -13, -23, -27, -29, //1488
-35, -40, -43, -44, -44, -38, -32, -23, -9, 2, 12, 22, 26, 32, 41, 50, //1504
62, 68, 72, 79, 87, 92, 99, 103, 102, 98, 88, 77, 56, 40, 30, 21, //1520
12, -7, -23, -36, -45, -49, -52, -50, -50, -46, -37, -27, -20, -12, -3, 5, //1536
15, 23, 30, 36, 44, 51, 57, 60, 59, 65, 70, 66, 64, 69, 71, 73, //1552
76, 71, 68, 67, 62, 51, 36, 20, 9, -7, -24, -36, -41, -45, -48, -39, //1568
-31, -27, -23, -15, -4, 2, 15, 29, 40, 51, 73, 78, 78, 80, 80, 75, //1584
61, 55, 53, 51, 48, 46, 44, 40, 33, 28, 22, 15, 14, 16, 16, 13, //1600
15, 18, 17, 15, 14, 18, 14, 6, 2, -3, -7, -4, 8, 7, 7, 15, //1616
24, 27, 30, 28, 30, 35, 29, 18, 13, 2, -5, -11, -15, -20, -23, -24, //1632
-25, -28, -32, -29, -28, -26, -20, -14, -4, 4, 8, 9, 10, 8, 9, 11, //1648
8, 7, 11, 10, 8, 6, 6, 5, -3, -13, -15, -15, -15, -20, -21, -19, //1664
-27, -36, -47, -56, -65, -69, -71, -76, -76, -72, -64, -59, -54, -49, -38, -24, //1680
-7, -2, 3, 9, 13, 28, 35, 34, 32, 35, 33, 21, 12, 4, -5, -14, //1696
-22, -31, -33, -35, -41, -45, -60, -71, -75, -79, -89, -97, -98, -95, -93, -90, //1712
-88, -91, -90, -88, -87, -85, -80, -61, -49, -46, -39, -22, -9, -11, -11, -13, //1728
-18, -22, -26, -38, -51, -58, -62, -64, -64, -68, -77, -83, -86, -93, -100, -106, //1744
-110, -106, -105, -107, -105, -109, -110, -111, -112, -98, -86, -76, -64, -51, -41, -24, //1760
-8, 5, 13, 22, 33, 34, 37, 42, 46, 48, 46, 44, 38, 31, 27, 20, //1776
5, -6, -17, -24, -26, -27, -30, -31, -30, -29, -28, -32, -34, -33, -31, -26, //1792
-18, -8, 5, 15, 20, 26, 41, 47, 51, 59, 64, 61, 59, 55, 48, 42, //1808
36, 25, 2, -14, -32, -54, -73, -86, -91, -95, -96, -92, -86, -79, -77, -74, //1824
-71, -69, -62, -50, -42, -30, -14, -1, 13, 22, 30, 33, 34, 38, 39, 37, //1840
38, 38, 33, 28, 28, 24, 18, 9, 4, 0, -14, -31, -45, -53, -57, -54, //1856
-53, -45, -38, -36, -35, -38, -31, -23, -15, -6, 8, 15, 21, 29, 32, 34, //1872
41, 40, 34, 29, 25, 19, 14, 13, 13, 15, 14, 3, -6, -17, -26, -32, //1888
-42, -60, -63, -64, -65, -64, -62, -53, -47, -42, -35, -25, -9, 5, 19, 32, //1904
45, 59, 83, 97, 105, 111, 116, 119, 118, 113, 101, 85, 67, 49, 39, 32, //1920
25, 13, -2, -19, -30, -37, -44, -51, -57, -58, -58, -63, -66, -61, -50, -38, //1936
-24, -11, 1, 13, 20, 25, 35, 44, 56, 75, 85, 94, 91, 85, 83, 77, //1952
72, 64, 52, 41, 34, 29, 28, 24, 17, 12, 2, -4, -3, -4, -8, -10, //1968
-4, -5, -4, -2, -6, -12, -11, -8, -5, -2, -2, 2, 6, 12, 22, 31, //1984
36, 43, 47, 48, 49, 51, 53, 54, 48, 42, 38, 30, 13, 4, -5, -15, //2000
-25, -33, -36, -40, -43, -44, -40, -37, -33, -29, -29, -29, -25, -15, -3, 11, //2016
23, 32, 42, 51, 53, 53, 53, 50, 48, 51, 50, 44, 34, 22, 6, 2, //2032
-5, -12, -18, -26, -35, -40, -45, -42, -31, -20, -15, -14, -12, -9, -7, -5, //2048
0, 9, 20, 32, 41, 51, 60, 66, 64, 60, 67, 76, 78, 78, 77, 75, //2064
68, 66, 64, 59, 51, 42, 31, 23, 12, 2, -4, -11, -10, -7, -6, -4, //2080
-3, -1, 3, 10, 14, 18, 26, 33, 38, 49, 59, 62, 58, 58, 54, 49, //2096
45, 40, 35, 31, 22, 15, 8, -6, -16, -24, -35, -41, -39, -34, -35, -42, //2112
-45, -36, -29, -23, -17, -10, 0, 9, 14, 17, 17, 16, 22, 25, 17, 18, //2128
22, 19, 14, 14, 16, 19, 23, 29, 39, 43, 35, 35, 35, 31, 28, 24, //2144
21, 14, 5, 1, -5, -19, -20, -17, -15, -8, 1, 3, 0, 5, 14, 14, //2160
13, 19, 26, 34, 40, 47, 54, 58, 64, 70, 72, 76, 86, 87, 90, 90, //2176
82, 71, 65, 57, 48, 37, 25, 7, -8, -16, -19, -27, -35, -31, -26, -23, //2192
-19, -12, -7, -3, 7, 23, 41, 54, 70, 73, 68, 66, 64, 46, 34, 27, //2208
19, 17, 17, 5, 0, -6, -12, -12, -14, -22, -25, -29, -36, -46, -56, -53, //2224
-51, -51, -49, -46, -35, -26, -15, -5, 4, 9, 19, 24, 27, 29, 28, 29, //2240
26, 20, 13, 7, 3, -9, -15, -20, -28, -40, -51, -62, -66, -69, -74, -79, //2256
-79, -71, -65, -64, -60, -53, -36, -21, -16, -15, -4, 8, 17, 21, 20, 19, //2272
28, 37, 38, 44, 47, 44, 37, 31, 28, 25, 21, 11, -3, -21, -25, -31, //2288
-43, -53, -63, -67, -69, -74, -86, -98, -112, -119, -123, -125, -121, -115, -112, -106, //2304
-97, -89, -80, -66, -66, -64, -55, -45, -45, -51, -46, -39, -28, -16, -16, -20, //2320
-21, -23, -24, -26, -34, -35, -36, -41, -47, -53, -59, -60, -59, -61, -63, -62, //2336
-54, -53, -57, -62, -58, -39, -32, -28, -21, -4, 14, 27, 29, 35, 46, 59, //2352
76, 78, 79, 84, 91, 95, 82, 71, 63, 55, 42, 32, 16, 2, -9, -18, //2368
-25, -40, -47, -52, -56, -63, -70, -74, -77, -79, -78, -75, -71, -57, -51, -46, //2384
-38, -29, -11, -3, 3, 11, 21, 29, 34, 34, 28, 25, 26, 22, 0, -13, //2400
-17, -26, -38, -56, -65, -72, -77, -84, -92, -91, -85, -84, -85, -83, -82, -65, //2416
-49, -43, -41, -37, -16, -4, 1, 4, 13, 23, 32, 33, 27, 22, 20, 15, //2432
7, 0, -6, -12, -17, -27, -31, -38, -44, -50, -56, -60, -58, -54, -56, -58, //2448
-56, -47, -38, -30, -26, -22, -4, 12, 25, 34, 40, 49, 59, 59, 55, 49, //2464
43, 41, 39, 33, 31, 29, 23, 5, -10, -20, -25, -27, -32, -35, -28, -23, //2480
-22, -25, -30, -33, -34, -38, -40, -38, -34, -28, -20, -9, 4, 17, 30, 35, //2496
40, 49, 53, 48, 51, 49, 44, 39, 28, 11, 1, -8, -11, -15, -24, -36, //2512
-37, -33, -30, -33, -35, -32, -28, -21, -14, -12, -6, 8, 22, 28, 30, 40, //2528
61, 73, 82, 89, 95, 100, 102, 98, 93, 88, 85, 83, 78, 74, 70, 67, //2544
67, 61, 52, 44, 36, 31, 25, 11, 3, -1, -3, -2, -5, -13, -13, -8, //2560
-5, -1, 1, 4, 7, 10, 16, 25, 35, 43, 43, 41, 37, 24, 15, 10, //2576
11, 18, 21, 17, 11, 2, -6, -15, -27, -34, -37, -36, -31, -26, -23, -22, //2592
-20, -16, -13, -15, -16, -15, -14, -13, -10, -5, 2, 10, 17, 21, 26, 44, //2608
55, 59, 54, 50, 54, 56, 49, 42, 37, 34, 31, 28, 24, 18, 13, 10, //2624
-2, -4, -1, 3, 6, 7, 11, 15, 16, 18, 21, 33, 46, 55, 58, 62, //2640
69, 79, 80, 75, 68, 63, 60, 53, 42, 32, 25, 22, 12, 6, -3, -12, //2656
-17, -19, -29, -35, -34, -33, -38, -50, -51, -51, -52, -50, -46, -35, -21, -8, //2672
3, 11, 23, 48, 62, 72, 80, 89, 101, 109, 107, 99, 89, 82, 78, 75, //2688
67, 55, 47, 40, 24, 16, 9, 1, -3, -6, -12, -22, -28, -27, -25, -24, //2704
-25, -28, -33, -32, -25, -20, -17, -16, -7, 6, 30, 45, 48, 48, 52, 55, //2720
52, 54, 58, 62, 63, 64, 64, 56, 45, 38, 32, 14, -1, -16, -26, -33, //2736
-41, -52, -52, -49, -50, -47, -39, -31, -28, -19, -15, -14, -5, -1, 7, 10, //2752
7, 7, 9, 10, 15, 23, 27, 32, 37, 37, 36, 29, 25, 25, 15, 7, //2768
6, 2, -1, -1, -7, -5, -1, 2, 11, 12, 7, 9, 15, 17, 19, 19, //2784
19, 26, 38, 45, 39, 35, 36, 39, 34, 24, 25, 22, 21, 14, 3, -8, //2800
-12, -20, -30, -30, -27, -31, -35, -38, -38, -35, -27, -24, -21, -18, -13, -8, //2816
-10, -6, -1, 3, 11, 15, 16, 24, 26, 29, 34, 28, 25, 23, 23, 23, //2832
21, 20, 24, 24, 20, 22, 24, 12, 7, 3, -12, -24, -35, -47, -55, -57, //2848
-57, -54, -43, -36, -30, -23, -15, -6, 6, 7, 8, 15, 21, 23, 19, 14, //2864
17, 19, 10, 1, -1, -8, -16, -21, -30, -45, -44, -41, -45, -51, -52, -60, //2880
-63, -62, -66, -70, -68, -66, -69, -78, -85, -88, -84, -79, -72, -61, -57, -54, //2896
-47, -46, -43, -33, -28, -31, -30, -26, -21, -23, -31, -44, -47, -45, -42, -45, //2912
-54, -52, -48, -49, -54, -59, -57, -54, -53, -55, -60, -59, -56, -59, -62, -65, //2928
-65, -57, -58, -54, -42, -36, -36, -34, -26, -20, -10, 1, -14, -15, -5, -1, //2944
-1, -5, -14, -13, -16, -24, -32, -40, -39, -38, -41, -43, -42, -30, -25, -27, //2960
-26, -26, -29, -31, -35, -42, -38, -27, -22, -7, 3, 5, 15, 31, 41, 44, //2976
51, 59, 68, 72, 66, 62, 54, 42, 31, 22, -1, -19, -30, -36, -48, -64, //2992
-76, -80, -78, -80, -81, -83, -85, -86, -81, -74, -72, -55, -39, -30, -18, -8, //3008
0, 5, 5, 7, 8, 6, 11, 11, 5, -4, -12, -17, -24, -33, -43, -55, //3024
-63, -72, -77, -77, -73, -71, -67, -56, -58, -60, -51, -46, -48, -40, -33, -26, //3040
-20, -15, -9, 2, 13, 23, 33, 42, 48, 55, 60, 63, 64, 58, 51, 45, //3056
35, 22, 7, -16, -33, -47, -59, -71, -77, -80, -76, -65, -54, -45, -36, -27, //3072
-18, -10, -4, 3, 13, 20, 26, 28, 28, 35, 44, 53, 64, 66, 63, 57, //3088
55, 51, 51, 54, 54, 51, 49, 43, 32, 20, 8, -9, -21, -28, -30, -32, //3104
-37, -40, -39, -36, -39, -40, -32, -29, -20, -6, 6, 14, 35, 49, 58, 65, //3120
70, 74, 81, 84, 86, 87, 87, 84, 83, 77, 68, 61, 54, 39, 32, 22, //3136
14, 10, 7, 9, 7, 7, 12, 21, 28, 31, 39, 44, 50, 56, 61, 63, //3152
60, 59, 64, 70, 75, 80, 81, 80, 78, 74, 72, 70, 63, 60, 62, 54, //3168
41, 29, 15, 0, -15, -25, -38, -46, -47, -47, -45, -44, -40, -36, -33, -28, //3184
-22, -16, -8, -2, 4, 10, 13, 21, 30, 36, 39, 44, 44, 41, 40, 39, //3200
37, 38, 39, 37, 30, 24, 16, 5, -8, -19, -28, -34, -40, -45, -46, -42, //3216
-41, -39, -31, -25, -18, -9, 0, 14, 24, 30, 36, 42, 46, 48, 55, 61, //3232
61, 61, 63, 62, 58, 55, 53, 54, 58, 52, 40, 30, 22, 15, 6, -3, //3248
-11, -20, -27, -32, -34, -37, -37, -35, -36, -32, -24, -15, -11, -8, 0, 19, //3264
34, 44, 51, 55, 56, 59, 60, 52, 40, 37, 39, 35, 30, 25, 21, 15, //3280
-3, -8, -12, -22, -28, -32, -37, -46, -52, -48, -43, -40, -43, -39, -33, -29, //3296
-26, -22, -19, -11, 1, 9, 18, 24, 38, 51, 56, 60, 71, 73, 71, 72, //3312
68, 64, 53, 43, 38, 28, 13, -4, -16, -28, -35, -37, -40, -45, -43, -39, //3328
-37, -30, -22, -16, -13, -9, -4, 3, 13, 16, 24, 28, 28, 33, 43, 43, //3344
41, 44, 46, 38, 31, 29, 21, 14, 7, -5, -14, -24, -32, -37, -41, -51, //3360
-53, -56, -60, -59, -59, -59, -62, -60, -55, -49, -37, -25, -5, 18, 42, 62, //3376
74, 80, 92, 98, 94, 88, 83, 83, 81, 71, 61, 41, 28, 18, 8, -8, //3392
-24, -40, -53, -70, -79, -75, -67, -55, -45, -39, -30, -17, -5, -5, 1, 6, //3408
7, 18, 33, 44, 61, 69, 75, 84, 81, 77, 76, 76, 70, 61, 55, 46, //3424
36, 18, -4, -31, -49, -68, -85, -102, -126, -126, -126, -126, -126, -126, -115, -110, //3440
-106, -95, -88, -81, -56, -46, -35, -17, -1, 20, 32, 44, 51, 53, 55, 64, //3456
71, 72, 72, 71, 72, 74, 76, 75, 65, 50, 27, 13, -10, -32, -45, -55, //3472
-70, -79, -93, -107, -113, -121, -126, -125, -127, -117, -97, -68, -50, -33, -17, -8, //3488
0, 15, 21, 28, 33, 33, 31, 21, 14, 11, 8, -7, -28, -36, -48, -59, //3504
-66, -68, -71, -74, -74, -80, -87, -93, -108, -107, -100, -88, -70, -46, -33, -22, //3520
-13, };
