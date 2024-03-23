const unsigned int knockSampleRate = 22050;
const unsigned int knockSampleCount = 516;
const signed char knockSamples[] = {//0
-1, 6, 20, 28, 35, 40, 54, 71, 87, 98, 101, 107, 118, 124, 127, 125, //16
119, 104, 96, 88, 77, 63, 50, 36, 27, 25, 27, 27, 25, 23, 20, 16, //32
17, 26, 39, 40, 41, 44, 51, 57, 63, 67, 71, 74, 77, 81, 87, 92, //48
94, 91, 88, 89, 87, 81, 71, 58, 43, 25, 17, 5, -5, -13, -19, -25, //64
-29, -36, -43, -47, -47, -41, -38, -36, -30, -20, -10, -7, -4, 2, 10, 15, //80
18, 25, 34, 43, 49, 42, 37, 33, 31, 29, 27, 18, 9, -6, -24, -39, //96
-48, -61, -67, -72, -77, -78, -77, -76, -78, -83, -87, -89, -89, -94, -94, -89, //112
-83, -75, -63, -56, -49, -40, -29, -12, -2, 9, 17, 20, 19, 20, 24, 24, //128
18, 11, 4, -6, -8, -10, -18, -32, -46, -50, -53, -57, -62, -70, -75, -73, //144
-72, -68, -64, -58, -47, -33, -23, -15, -12, -5, 4, 11, 15, 20, 30, 42, //160
42, 36, 30, 23, 10, 2, -6, -14, -25, -38, -49, -48, -49, -58, -67, -71, //176
-67, -67, -69, -72, -74, -70, -65, -61, -57, -52, -44, -31, -24, -24, -25, -19, //192
-7, 4, 2, 2, 8, 11, 8, 8, 8, 4, 5, 8, 11, 10, 9, 5, //208
0, -6, -15, -23, -27, -28, -30, -35, -38, -38, -37, -42, -43, -30, -26, -28, //224
-25, -20, -13, -4, 1, 5, 9, 13, 12, 15, 18, 21, 24, 26, 26, 25, //240
23, 20, 17, 13, 8, -4, -15, -22, -26, -43, -56, -62, -60, -58, -62, -64, //256
-58, -49, -41, -33, -25, -11, -4, 2, 8, 14, 22, 22, 20, 17, 17, 19, //272
17, 13, 6, 1, -3, -7, -16, -27, -36, -39, -41, -49, -57, -62, -63, -61, //288
-63, -71, -73, -74, -76, -74, -70, -68, -66, -65, -63, -62, -59, -59, -56, -51, //304
-46, -38, -28, -19, -8, 1, 3, 4, 7, 9, 8, 6, 7, 6, 2, -5, //320
-9, -12, -16, -28, -35, -36, -36, -36, -32, -35, -43, -49, -48, -45, -35, -29, //336
-27, -21, -10, 3, 12, 13, 17, 25, 36, 46, 49, 54, 55, 53, 51, 46, //352
41, 32, 22, 11, 5, -1, -10, -19, -23, -23, -19, -21, -25, -26, -26, -25, //368
-24, -25, -26, -25, -21, -15, -2, 4, 8, 14, 19, 25, 29, 33, 35, 36, //384
38, 40, 37, 30, 25, 19, 12, -3, -12, -17, -21, -24, -27, -28, -27, -29, //400
-33, -34, -30, -23, -16, -10, -2, 9, 25, 31, 36, 40, 41, 46, 47, 47, //416
48, 49, 49, 52, 54, 58, 60, 61, 54, 43, 42, 41, 41, 38, 40, 42, //432
45, 45, 43, 44, 47, 48, 49, 53, 57, 61, 64, 68, 71, 72, 69, 64, //448
61, 59, 54, 46, 42, 40, 38, 32, 26, 22, 22, 19, 11, 4, 0, -1, //464
4, 6, 7, 9, 11, 15, 24, 26, 28, 26, 25, 24, 23, 20, 15, 14, //480
12, 14, 15, 17, 20, 23, 25, 25, 25, 28, 30, 31, 28, 22, 17, 14, //496
15, 13, 6, 3, 4, 6, 7, 5, 0, -2, -4, -5, -10, -17, -19, -18, //512
-18, -18, -17, -13, };
