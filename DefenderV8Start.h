const unsigned int sampleRate = 22050;
const unsigned int sampleCount = 8844;
const signed char samples[] = {
-1, 2, 1, -5, -9, -9, 1, 8, 9, 5, 1, -2, -9, -11, -13, -14, 
-18, -28, -33, -30, -19, -9, -4, -7, -14, -17, -12, 1, 8, -1, -15, -23, 
-22, -18, -16, -13, -4, 4, 0, -8, 1, 15, 24, 27, 27, 24, 5, -5, 
-11, -12, -11, -13, -10, -7, -8, -14, -14, 0, 3, 1, 1, 0, -6, -16, 
-8, 6, 13, 8, -3, -6, -5, -2, -1, 3, 11, 11, 10, 10, 13, 14, 
12, 11, 15, 21, 27, 31, 22, 13, 10, 14, 24, 28, 16, 0, -4, 12, 
29, 21, -5, -22, -9, 25, 47, 31, 24, 39, 62, 78, 83, 75, 57, 28, 
-4, -32, -53, -52, -51, -41, -14, 27, 39, 46, 59, 64, 38, -42, -62, -25, 
39, 74, 48, -19, -16, 16, 12, -45, -115, -105, -49, 14, 50, 60, 37, 9, 
-18, -28, -7, 36, 69, 105, 115, 106, 56, -75, -114, -113, -98, -88, -86, -59, 
5, 78, 111, 116, 101, 97, 106, 104, 84, 20, -111, -118, -113, -84, -13, 51, 
32, -59, -115, -81, 39, 116, 111, 99, 2, -98, -119, -84, 12, 66, 1, -91, 
-127, -47, 29, 82, 109, 106, 44, 28, 71, 67, -25, -100, -112, -41, 24, 15, 
-39, -74, -28, 35, 97, 115, 115, 46, -62, -112, -117, -98, -57, -60, -91, -97, 
-35, 71, 122, 60, -3, -21, 25, 89, 38, -73, -114, -118, -81, 26, 115, 108, 
48, -23, -32, 25, 114, 110, 58, 4, -28, -76, -102, -113, -116, -67, 44, 116, 
116, 50, -62, -97, -27, 111, 108, 28, -56, -94, -90, -70, -36, 3, 35, 62, 
61, 36, -9, -62, -74, -25, 76, 90, 59, 12, -31, -56, -37, -12, 0, 8, 
22, 62, 79, 42, -39, -103, -116, -86, -17, 34, 32, 5, -2, 38, 80, 71, 
1, -75, -101, -69, -22, 24, 61, 73, 56, 47, 47, 45, 27, -34, -63, -60, 
-33, -24, -56, -114, -103, -42, 38, 88, 61, 5, -35, -27, 21, 69, 65, 6, 
-47, -53, -18, 28, 59, 32, -13, -38, -13, 62, 77, 34, -50, -106, -120, -58, 
22, 51, 18, -28, -50, -27, 12, 59, 93, 90, 23, -24, -46, -39, -10, 27, 
47, 13, -41, -77, -69, -29, 22, 37, 54, 69, 59, -4, -43, -56, -45, -21, 
3, 34, 39, 20, -22, -62, -71, 5, 80, 114, 101, 36, -67, -69, -31, 15, 
29, 2, -59, -60, -31, 5, 29, 31, 17, 16, 17, 9, -11, -47, -58, -48, 
-22, 11, 31, 27, 22, 19, 21, 31, 44, 39, 13, -20, -42, -42, -15, -1, 
4, 9, 20, 33, 51, 57, 50, 24, -12, -51, -48, -19, 9, 14, -3, -16, 
2, 23, 26, 8, -14, -24, -17, -9, -7, -6, -11, -11, -1, 18, 38, 44, 
23, 0, -18, -23, -14, -2, -1, -12, -24, -28, -18, 24, 54, 66, 53, 22, 
-13, -40, -34, -18, -3, -4, -24, -40, -17, 20, 46, 39, -5, -19, -12, 3, 
10, 2, -11, -3, 9, 18, 20, 14, 2, -3, -7, -11, -15, -15, -12, -9, 
-9, -10, -8, 9, 26, 42, 45, 20, -23, -54, -28, 15, 48, 52, 10, -29, 
-49, -32, 16, 63, 66, 29, -4, -11, 1, 15, 9, -8, -15, 0, 24, 32, 
11, -14, -24, -18, -1, 26, 29, 21, 6, -9, -18, -21, -14, 0, 21, 38, 
33, 14, 3, 11, 29, 41, 23, -6, -22, -20, -11, -6, -6, -5, -4, -1, 
6, 14, 12, 1, -15, -31, -36, -26, -18, -19, -23, -19, -9, -12, -19, -19, 
-9, 3, 12, 7, -4, -16, -21, -15, 3, 3, -14, -36, -45, -31, -18, -13, 
-12, -8, -1, 3, -3, -10, -13, -12, -2, 18, 26, 25, 10, -15, -37, -28, 
-9, 4, 6, -4, -31, -41, -33, -4, 26, 35, 6, -21, -25, -7, 14, 11, 
-6, -16, -14, 5, 29, 26, -1, -31, -42, -21, 16, 42, 17, -23, -48, -46, 
-7, 11, 12, 7, 9, 22, 29, 19, 7, 3, 6, 11, 9, -3, -20, -35, 
-40, -20, -2, 9, 14, 20, 27, 20, 9, 3, 5, 8, 6, -8, -19, -27, 
-29, -22, -6, -6, -10, -12, -8, -3, 3, 13, 23, 26, 21, 14, 8, 2, 
-4, -2, 8, 16, 9, -1, -1, 11, 21, 18, 1, -25, -50, -58, -43, 2, 
12, -2, -17, -12, 28, 45, 39, 18, -1, -8, 2, 10, 5, -14, -33, -39, 
-28, -17, -15, -18, -23, -26, -20, -7, 9, 19, 19, 2, -7, -5, 0, 4, 
5, -1, -7, -11, -9, -3, 13, 24, 25, 9, -19, -40, -31, -5, 15, 17, 
8, -10, -16, -14, -9, -4, 2, 21, 40, 53, 49, 26, -1, -13, -1, 12, 
18, 14, 7, 4, 0, -2, -1, 4, 22, 30, 27, 14, -4, -21, -22, -6, 
11, 21, 16, 0, -1, 6, 19, 33, 42, 39, 34, 26, 14, 1, -13, -18, 
-16, -13, -11, -8, 5, 18, 28, 28, 14, -7, -26, -23, -12, -6, -8, -18, 
-27, -19, -2, 16, 26, 22, 11, 3, 4, 12, 18, 0, -20, -32, -27, -13, 
-5, -19, -32, -35, -24, -6, 13, 10, -3, -13, -11, 2, 14, 10, 5, 7, 
15, 26, 35, 36, 33, 21, 3, -24, -31, -31, -27, -23, -14, 2, 3, 0, 
3, 16, 39, 39, 28, 12, -3, -14, -19, -16, -14, -18, -27, -31, -13, 6, 
14, 12, 7, 13, 19, 18, 10, 2, 4, 13, 16, 14, 11, 11, 10, 5, 
5, 8, 11, 8, -6, -17, -20, -13, 2, 18, 31, 26, 17, 8, 0, -4, 
-2, 5, 10, 2, -16, -37, -33, -18, -8, -10, -17, -17, -11, -5, -2, 0, 
1, 4, 3, 1, 0, -2, -9, -16, -20, -20, -18, -13, -11, -15, -22, -23, 
-20, -15, -4, 3, 9, 15, 21, 29, 27, 21, 11, 2, -2, -3, -4, -8, 
-12, -13, -12, -8, -4, 1, 7, 9, 7, 9, 13, 15, 15, 13, 10, 6, 
2, 3, 8, 10, 6, 4, 5, 5, 2, -2, 0, 0, -4, -9, -11, -4, 
3, 7, 9, 5, 2, 2, 8, 11, 5, -12, -38, -40, -26, -9, 2, 1, 
-10, -12, -3, 12, 23, 24, 17, 8, 2, -1, -2, 2, 3, 4, 6, 7, 
5, -3, -10, -10, -1, 10, 11, 2, -9, -16, -16, -15, -18, -23, -27, -25, 
-18, -10, -1, 0, 1, 2, 5, 11, 13, 10, 4, -3, -8, -2, 3, 1, 
-6, -14, -17, -11, -5, 1, 2, -1, -2, 5, 15, 21, 21, 14, 3, 2, 
4, 6, 7, 5, -2, -4, -5, -3, 0, 2, 2, 2, 4, 9, 12, 3, 
-8, -19, -24, -24, -22, -20, -18, -15, -11, -8, -3, -1, -2, -5, -8, -12, 
-14, -15, -17, -20, -23, -24, -19, -14, -10, -9, -11, -11, -8, -8, -9, -9, 
-9, -11, -17, -21, -21, -18, -10, 3, 7, 4, -2, -7, -6, 0, 8, 12, 
10, 5, -4, -7, -5, 0, -1, -8, -15, -12, -5, 2, 4, 4, 8, 12, 
15, 17, 19, 16, 13, 10, 7, 5, 4, 7, 10, 10, 8, 5, 2, 4, 
12, 19, 22, 19, 10, 4, 1, -3, -10, -18, -20, -18, -10, 2, 12, 14, 
8, 4, 4, 8, 12, 9, 5, 1, 2, 5, 4, -2, -8, -10, -4, 5, 
11, 7, 1, -3, -1, 7, 19, 20, 16, 9, 3, 4, 5, 3, 0, -2, 
-3, -1, -2, -3, -5, -8, -7, 3, 11, 14, 12, 8, 4, 4, 5, 6, 
7, 5, 2, 0, 1, 5, 10, 14, 14, 7, 2, 0, 2, 8, 10, 9, 
4, -3, -9, -13, -12, -14, -15, -14, -13, -9, -8, -9, -10, -9, -3, 1, 
1, -4, -8, -8, -4, -2, -2, -5, -9, -12, -14, -12, -11, -13, -16, -14, 
-6, -1, -1, -8, -15, -16, -12, -6, -3, 0, 0, 0, 4, 11, 18, 21, 
16, 11, 6, 4, 4, 7, 21, 27, 27, 21, 13, 9, 11, 17, 21, 19, 
10, -6, -11, -13, -12, -10, -8, -5, -4, -1, 3, 4, 2, -3, -7, -7, 
-6, -2, 3, 4, 3, 1, -1, 0, 0, -1, -4, -8, -13, -19, -20, -21, 
-22, -23, -24, -23, -19, -12, -4, 2, 6, 5, 6, 8, 9, 10, 6, 3, 
0, -1, -1, -1, 1, 3, 3, 2, 1, 1, 0, -2, -5, -6, -7, -12, 
-16, -18, -17, -13, -9, -7, -7, -8, -8, -7, -4, 2, 7, 9, 7, 1, 
-11, -13, -10, -8, -7, -7, -8, -9, -9, -6, -1, 5, 14, 18, 18, 15, 
9, 0, 0, 1, 2, 1, -2, -4, -5, -3, 0, 6, 11, 16, 16, 14, 
14, 13, 11, 11, 13, 14, 11, 7, 3, 4, 7, 10, 10, 9, 6, 5, 
5, 9, 12, 13, 9, 4, 2, 2, 2, 4, 7, 10, 13, 17, 22, 24, 
25, 22, 16, 12, 8, 6, 2, -3, -6, -6, -5, -3, 0, 1, 2, 4, 
8, 11, 12, 9, 1, -10, -12, -13, -13, -14, -16, -17, -16, -12, -6, -1, 
5, 7, 6, 4, 3, 3, 2, -1, -5, -10, -15, -17, -13, -10, -10, -11, 
-11, -3, 3, 7, 7, 6, 3, 1, 2, 4, 6, 4, 0, -4, -2, 2, 
8, 11, 7, 1, -5, -6, -3, 1, 6, 6, 4, 0, -4, -7, -6, -3, 
-2, -4, -8, -12, -10, -7, -3, 0, 0, -3, -5, -6, -4, -2, 0, -3, 
-4, -2, 0, 3, 4, 5, 3, 1, 1, 0, -2, -2, -1, 0, 0, 0, 
-3, -7, -9, -10, -8, -2, 2, 3, 1, -1, -3, -7, -9, -11, -13, -15, 
-16, -14, -10, -5, -2, 1, 1, -2, -5, -4, -1, 1, -3, -10, -15, -17, 
-18, -22, -23, -22, -19, -15, -11, -6, -4, -3, -2, -1, 0, -2, -5, -8, 
-8, -6, -5, -5, -3, 0, 3, 4, 1, -2, -2, 1, 4, 7, 11, 9, 
5, 1, 1, 13, 20, 23, 22, 18, 16, 17, 19, 23, 25, 24, 21, 13, 
9, 8, 9, 12, 14, 15, 10, 4, 1, 3, 12, 14, 11, 5, -1, -3, 
-1, 2, 5, 7, 9, 12, 12, 9, 4, -1, -3, -3, -3, -7, -13, -17, 
-19, -19, -17, -14, -13, -13, -14, -14, -10, -9, -10, -12, -12, -9, -9, -12, 
-17, -20, -20, -16, -13, -11, -11, -12, -13, -14, -12, -10, -9, -6, -2, 2, 
5, 5, 4, 4, 2, -2, -7, -12, -16, -16, -13, -8, -4, -2, -3, -5, 
-5, -3, -1, -2, -10, -16, -17, -15, -10, -4, -2, -1, 0, 4, 12, 25, 
32, 37, 38, 35, 28, 24, 20, 18, 19, 22, 26, 25, 20, 15, 12, 13, 
14, 13, 9, 5, 2, 4, 5, 7, 9, 11, 13, 9, 4, -1, -3, -2, 
-2, -4, -7, -8, -8, -7, -5, -5, -2, 2, 6, 8, 5, 0, -3, -2, 
3, 9, 11, 6, -4, -13, -16, -13, -8, -4, -2, -3, -6, -11, -12, -11, 
-9, -6, -3, -1, -1, -1, -1, 2, 5, 4, 0, -6, -13, -17, -18, -17, 
-17, -18, -17, -14, -6, -1, 1, 2, 2, 3, 7, 12, 15, 14, 9, -2, 
-4, -5, -4, -5, -9, -15, -15, -11, -7, -3, 1, 5, 10, 14, 17, 19, 
17, 13, 9, 5, 4, 4, 3, 0, -6, -12, -15, -7, 5, 15, 22, 25, 
25, 23, 19, 15, 11, 6, -1, -8, -9, -10, -10, -11, -13, -11, -7, -1, 
3, 3, 0, -2, 0, 4, 7, 8, 5, 3, 1, 0, 0, -3, -2, 0, 
3, 4, 1, -7, -12, -14, -13, -10, -6, -6, -7, -7, -5, 0, 4, 5, 
4, 3, 2, 0, -7, -10, -9, -7, -6, -10, -13, -16, -15, -11, -4, 3, 
3, 2, 2, 4, 7, 8, 8, 9, 10, 9, 4, 0, 0, 1, 4, 5, 
3, 1, -3, -10, -15, -18, -16, -15, -13, -11, -9, -4, 1, 6, 11, 13, 
14, 14, 16, 16, 13, 10, 9, 11, 13, 14, 14, 13, 10, 8, 7, 7, 
10, 12, 11, 7, 2, -2, -6, -9, -8, -5, -3, -5, -9, -13, -13, -13, 
-12, -13, -12, -8, -7, -9, -9, -7, -5, -3, -3, -5, -9, -10, -8, -8, 
-11, -15, -16, -15, -7, -1, 2, 2, -2, -6, -8, -7, -5, 0, 2, -1, 
-6, -12, -16, -17, -13, -5, -2, -3, -7, -13, -14, -7, 1, 6, 8, 7, 
3, 1, 0, -1, -3, -5, -6, -6, -8, -9, -8, -4, -1, 0, -2, -6, 
-10, -12, -10, -6, -3, 0, 0, -5, -9, -8, -2, 4, 4, 0, -4, -4, 
1, 8, 13, 10, 4, -1, -5, -6, -4, -2, -3, -6, -9, -7, -5, -2, 
1, 2, 4, 8, 10, 12, 14, 17, 17, 15, 11, 7, 5, 3, 0, -6, 
-11, -13, -11, -6, -1, -2, -3, -4, -4, -2, 3, 5, 7, 7, 7, 10, 
13, 15, 15, 13, 12, 11, 7, 1, -6, -9, -8, -5, -3, -3, -5, -7, 
-9, -8, -8, -7, -5, -2, 3, 4, 2, 0, -3, -4, -5, -7, -10, -13, 
-14, -14, -13, -11, -6, -1, 1, 7, 11, 15, 17, 18, 15, 9, 6, 7, 
10, 10, 7, 2, -6, -13, -17, -17, -10, -6, -2, 2, 6, 15, 20, 21, 
21, 19, 18, 11, 6, 0, -6, -11, -14, -14, -13, -10, -5, 0, 6, 10, 
15, 19, 21, 20, 18, 17, 15, 10, 1, -11, -24, -31, -37, -42, -45, -41, 
-35, -27, -20, -13, -7, 5, 15, 23, 26, 24, 18, 11, 8, 5, 2, -2, 
-8, -11, -13, -13, -11, -9, -9, -6, 1, 7, 8, 3, -4, -6, -6, -4, 
-3, -3, -4, -5, -7, -8, -9, -8, -5, -2, 1, 3, 3, 2, 2, 5, 
8, 11, 11, 9, 7, 4, 2, 2, 6, 10, 11, 8, 5, 1, -4, -4, 
-2, 2, 5, 3, 2, 2, 5, 7, 6, -1, -6, -12, -15, -17, -14, -11, 
-11, -13, -14, -14, -7, 0, 8, 15, 20, 23, 20, 17, 14, 15, 17, 17, 
10, 4, -2, -5, -5, -8, -11, -15, -16, -15, -10, -1, 4, 8, 13, 16, 
13, 10, 7, 5, 5, 6, 9, 8, 4, -1, -9, -13, -12, -8, -3, 2, 
6, 7, 5, 1, -4, -6, -7, -6, -8, -12, -19, -26, -28, -27, -22, -15, 
-8, -3, -4, -6, -5, 3, 12, 18, 12, 3, -4, -8, -10, -12, -16, -18, 
-23, -30, -37, -40, -35, -24, -11, 1, 6, 7, 6, 8, 12, 13, 10, -2, 
-7, -7, -7, -5, -4, -7, -10, -12, -11, -9, 1, 7, 11, 16, 19, 20, 
24, 28, 31, 31, 28, 19, 13, 9, 5, 1, -3, -11, -16, -15, -11, -5, 
2, 6, 9, 12, 14, 15, 17, 20, 23, 24, 20, 13, -3, -15, -24, -32, 
-36, -35, -33, -27, -20, -12, -5, 0, 0, 1, 5, 13, 21, 22, 10, -7, 
-19, -23, -20, -14, -14, -16, -18, -16, -9, -5, -5, -7, -8, -5, 3, 7, 
7, 5, 2, -3, -5, -4, -1, 1, -1, -9, -12, -10, -3, 5, 12, 14, 
12, 11, 10, 11, 14, 15, 15, 11, 6, 0, -4, -4, -2, 0, 4, 8, 
8, 5, 3, 4, 4, -1, -7, -12, -14, -16, -17, -13, -8, -2, 3, 8, 
15, 17, 10, 2, -1, 0, 0, -3, -8, -9, -5, 0, 6, 9, 12, 13, 
11, 8, 5, 5, 5, 7, 11, 14, 9, -2, -10, -12, -10, -6, -7, -9, 
-9, -6, -2, 4, 8, 9, 9, 11, 10, 7, 2, -4, -10, -13, -12, -14, 
-17, -19, -14, -6, 0, -3, -9, -12, -9, 5, 18, 29, 33, 31, 22, 7, 
1, -2, -2, -4, -7, -10, -11, -11, -12, -12, -8, -6, -5, -7, -9, -9, 
-6, -3, 0, 3, 4, 5, 9, 12, 11, 6, 3, 9, 15, 18, 19, 18, 
18, 17, 13, 6, 2, 1, -1, -5, -11, -19, -27, -32, -31, -23, -13, -5, 
4, 11, 16, 19, 20, 22, 23, 26, 25, 24, 21, 11, 0, -7, -2, 7, 
14, 14, 10, 9, 13, 16, 14, 6, -13, -21, -24, -22, -18, -16, -15, -14, 
-12, -9, -9, -12, -15, -12, -8, -6, -5, -3, 3, 8, 12, 13, 9, 2, 
-2, -1, 2, 4, 3, 0, -3, -7, -9, -11, -6, 0, 3, -2, -11, -18, 
-11, 2, 13, 15, 5, -9, -20, -16, -7, -2, -4, -15, -15, -5, 9, 23, 
29, 17, 7, 3, 4, 10, 18, 23, 19, 14, 14, 16, 17, 12, 7, 5, 
5, 4, 1, 0, -2, -7, -13, -17, -14, -6, 2, 4, 2, -2, -3, 1, 
9, 17, 26, 33, 35, 34, 25, 12, 1, 3, 13, 17, 8, -9, -23, -20, 
-11, -4, -3, -8, -11, -7, 2, 15, 23, 21, 12, 7, 7, 8, 6, -5, 
-15, -20, -20, -14, -4, 10, 14, 13, 9, 1, -8, -9, -5, 1, 3, -2, 
-18, -28, -33, -30, -20, -8, 3, 3, -2, -4, 2, 19, 28, 32, 32, 29, 
23, 11, 1, -8, -17, -27, -36, -45, -43, -36, -24, -15, -10, -7, 0, 9, 
17, 21, 15, 3, -9, -16, -15, -13, -12, -13, -13, -9, -2, 9, 11, 7, 
3, 1, 4, 2, -9, -22, -31, -29, -21, -15, -21, -26, -19, 0, 23, 26, 
19, 13, 12, 18, 28, 28, 21, 10, 0, -4, 2, 8, 11, 12, 12, 14, 
15, 10, 3, -3, -7, -11, -14, -15, -15, -16, -18, -16, -13, -10, -9, -8, 
-7, -4, 0, 1, 0, -3, -7, -6, -1, 3, 4, 2, 1, 7, 13, 13, 
9, 4, 3, 4, 2, 0, 0, -9, -22, -35, -38, -30, -10, -2, 0, -1, 
-1, 1, 12, 25, 37, 41, 33, 18, 0, -2, 1, 3, 1, -1, 1, 6, 
9, 11, 13, 16, 17, 16, 14, 14, 12, -3, -19, -32, -37, -37, -31, -27, 
-20, -12, -5, 0, 8, 11, 12, 15, 19, 19, 2, -12, -17, -10, 2, 10, 
5, 0, -1, 2, 10, 24, 26, 21, 11, 4, 3, -2, -15, -30, -40, -40, 
-32, -31, -29, -22, -11, -2, 5, 12, 21, 30, 31, 21, -7, -18, -20, -15, 
-8, -1, -1, -2, -7, -11, -12, -7, -5, -5, -9, -16, -22, -25, -20, -8, 
1, -1, -23, -38, -42, -35, -21, -9, -4, -4, 0, 9, 18, 19, 12, 7, 
5, 5, 6, 2, -5, -12, -13, -9, -5, -10, -20, -27, -26, -16, -3, 6, 
5, 2, 2, 4, 5, 1, 0, 2, 7, 10, 7, 6, 5, 0, -9, -16, 
-12, -6, -1, -2, -4, -2, 1, 4, 8, 13, 18, 19, 14, 8, 7, 11, 
17, 20, 26, 31, 35, 38, 37, 30, 17, 4, -4, -6, -4, -7, -16, -24, 
-26, -24, -22, -19, -11, 4, 20, 31, 26, 16, 9, 6, 8, 9, 1, -13, 
-28, -37, -29, -14, -1, 5, 5, 4, 2, 0, 0, 1, 2, 3, -3, -8, 
-9, -8, -6, -10, -15, -15, -7, 6, 15, 8, 1, 2, 10, 16, 12, -2, 
-7, -4, 3, 8, 7, 4, 3, 6, 12, 14, 4, -7, -17, -20, -19, -15, 
-7, -7, -10, -12, -9, 3, 11, 14, 11, 5, -6, -19, -19, -18, -25, -39, 
-51, -48, -37, -32, -34, -34, -19, 0, 20, 32, 35, 31, 21, 13, 4, -3, 
-7, -8, -5, -9, -15, -20, -21, -15, -4, 11, 25, 29, 26, 21, 21, 21, 
16, 8, 0, -2, -7, -16, -26, -28, -14, 3, 15, 21, 20, 14, 6, 4, 
5, 3, 0, 0, 1, -1, -5, -8, -12, -13, -9, -5, -5, -8, -15, -19, 
-14, -5, 2, 3, 3, 7, 16, 23, 25, 21, 16, 18, 23, 25, 22, 13, 
-3, -8, -6, -2, 2, 7, 8, 5, -2, -9, -10, -1, 9, 19, 21, 15, 
5, -4, -2, 4, 8, 9, 3, 1, 0, -2, -4, -6, -7, -9, -10, -10, 
-12, -16, -25, -24, -13, 2, 13, 14, 5, -3, -5, 0, 12, 27, 27, 20, 
5, -10, -18, -21, -22, -24, -26, -26, -20, -14, -7, 2, 11, 18, 19, 16, 
11, 6, -1, -3, 7, 16, 24, 23, 13, -5, -9, -4, 3, 10, 11, 3, 
-3, -9, -14, -18, -22, -20, -13, -4, 1, -4, -21, -27, -23, -14, -7, -5, 
-11, -14, -10, 2, 17, 25, 19, 10, 6, 9, 15, 21, 19, 15, 8, 1, 
-3, 2, 6, 7, 5, 4, 5, 2, -5, -15, -21, -20, -9, -3, -2, -5, 
-5, 2, 19, 27, 30, 26, 17, 8, 10, 16, 16, 12, 6, -3, -11, -21, 
-26, -23, -14, -2, -2, -4, -3, -4, -11, -15, -16, -13, -10, -7, -7, -12, 
-17, -18, -9, 6, 28, 33, 27, 14, 1, -1, 9, 20, 21, 12, -2, -14, 
-10, -4, 0, 2, 2, 7, 10, 12, 10, 6, -2, -6, -9, -12, -16, -25, 
-32, -28, -17, -2, 10, 12, 4, 2, 4, 9, 13, 14, 8, 0, -5, -5, 
-1, -2, -6, -10, -10, -4, -3, -14, -24, -29, -28, -19, -6, -4, -7, -9, 
-11, -12, -7, -2, 5, 9, 8, 3, -7, -4, 8, 24, 32, 18, 2, -12, 
-20, -21, -15, -3, -1, -1, -5, -15, -32, -37, -32, -19, -8, -9, -26, -34, 
-31, -17, 1, 17, 32, 37, 36, 28, 18, 7, 8, 10, 12, 12, 8, -2, 
-9, -9, -2, 11, 21, 19, 5, -9, -16, -11, 4, 9, 6, 1, -4, -7, 
-3, 3, 6, 6, 6, 10, 20, 23, 20, 12, 5, 3, 7, 15, 21, 20, 
13, 2, 4, 13, 24, 27, 13, -21, -35, -33, -18, -6, -9, -19, -24, -19, 
-8, 2, 4, 0, -1, 1, 5, 11, 19, 25, 27, 27, 29, 34, 31, 19, 
4, -6, -8, 2, 9, 10, 4, -5, -11, -8, 0, 6, 8, 6, 1, 3, 
10, 17, 16, 7, -6, 0, 16, 31, 35, 30, 19, 15, 13, 15, 18, 15, 
7, -1, -7, -7, -8, -17, -25, -28, -22, -8, 6, 11, 4, -4, -9, -7, 
3, 2, -7, -20, -27, -21, 1, 11, 11, 5, -1, 0, 3, 3, -1, -8, 
-14, -7, -1, -2, -13, -27, -32, -21, -12, -11, -19, -31, -44, -40, -27, -9, 
3, 3, -4, 2, 16, 27, 27, 20, 8, 2, -3, -6, -12, -24, -31, -34, 
-29, -21, -15, -20, -29, -29, -16, 5, 22, 20, 7, -2, 3, 18, 34, 27, 
7, -15, -26, -21, 0, 7, 4, -4, -10, -9, 4, 12, 13, 10, 6, 9, 
11, 8, -3, -18, -28, -28, -23, -21, -23, -24, -20, -4, 10, 21, 29, 28, 
19, 14, 9, 2, -8, -10, 1, 5, 2, -2, -2, 4, 15, 19, 19, 8, 
-9, -21, -23, -26, -31, -35, -38, -35, -25, -12, 3, 13, 16, 9, 5, 7, 
15, 24, 27, 22, 15, 7, -1, -7, -8, -6, -3, 0, -1, -9, -20, -21, 
-13, -5, -1, -12, -23, -24, -17, -5, 6, 10, 8, 4, 3, 4, 9, 14, 
18, 16, 10, 2, 1, 7, 12, 13, 9, 0, -19, -29, -34, -33, -28, -21, 
-17, -13, -9, -6, -4, 2, 9, 16, 18, 15, 10, -1, -10, -15, -18, -19, 
-13, -1, 11, 21, 28, 32, 36, 33, 26, 21, 21, 21, 11, 1, -6, -3, 
5, 11, 5, -8, -16, -17, -12, 1, 11, 19, 21, 18, 16, 21, 18, 7, 
-8, -21, -22, -19, -18, -13, -3, 8, 17, 15, 9, 4, 0, -1, 8, 13, 
7, -7, -16, -10, -3, 0, 3, 1, -7, -21, -15, 7, 32, 47, 47, 36, 
31, 25, 16, 6, -5, -15, -28, -40, -41, -31, -9, 5, 14, 19, 21, 18, 
15, 20, 25, 20, 4, -15, -22, -26, -26, -14, 7, 31, 34, 25, 13, 8, 
15, 28, 21, 2, -21, -39, -36, -18, 1, 4, -10, -29, -33, -17, -5, -9, 
-22, -31, -21, -3, 10, 9, -5, -19, -9, 12, 30, 33, 27, 21, 18, 11, 
2, -5, -4, -2, -4, -15, -25, -29, -26, -15, -2, 2, -7, -18, -9, 10, 
23, 23, 16, 2, -7, -13, -11, -2, 8, 12, 10, 8, 8, 7, 9, 16, 
16, 7, -7, -19, -19, -8, 0, -1, -13, -30, -48, -44, -24, 0, 14, 8, 
-24, -39, -42, -34, -21, -5, 2, 8, 17, 29, 40, 40, 31, 22, 15, 7, 
0, -10, -12, -3, 10, 18, 9, -5, -18, -31, -44, -48, -31, -18, -16, -23, 
-31, -34, -26, -19, -12, -4, 2, 3, 3, 8, 16, 22, 22, 10, 7, 12, 
17, 16, 7, -16, -24, -19, -4, 11, 3, -18, -32, -26, -9, 3, -6, -19, 
-26, -26, -23, -18, -5, 7, 19, 30, 36, 31, 18, 10, 14, 29, 43, 45, 
35, 23, 15, 14, 3, -16, -35, -45, -41, -27, -9, -6, -6, -2, 2, 4, 
0, -2, 5, 20, 29, 14, -4, -7, 5, 21, 26, 11, 4, 7, 14, 21, 
22, 14, 3, -10, -20, -27, -32, -34, -30, -20, -8, -3, -10, -19, -26, -24, 
-11, 9, 30, 28, 15, 7, 6, 3, -8, -24, -38, -41, -30, -12, -14, -25, 
-33, -28, -11, 16, 20, 11, -5, -17, -9, 7, 20, 20, 9, -1, 5, 20, 
31, 36, 38, 38, 29, 21, 14, 13, 12, 3, -9, -20, -30, -34, -32, -19, 
-5, 7, 11, 14, 21, 36, 35, 25, 14, 8, 11, 10, 0, -11, -16, -9, 
17, 32, 40, 40, 32, 20, 13, 19, 25, 21, 9, -5, -11, -19, -29, -35, 
-33, -24, -21, -21, -20, -22, -24, -9, 11, 30, 38, 37, 28, 24, 17, 8, 
1, 0, -2, -13, -25, -31, -27, -13, -8, -6, -5, -1, 6, 8, 8, 11, 
15, 16, 13, -3, -13, -17, -15, -11, 0, 6, 4, -7, -23, -31, -17, -1, 
4, -4, -19, -31, -35, -26, -17, -14, -20, -29, -24, -12, 1, 11, 14, 12, 
9, 9, 11, 15, 10, -3, 1, 17, 34, 36, 8, -11, -16, -11, -7, -11, 
-21, -23, -23, -21, -20, -18, -11, -2, 12, 27, 39, 47, 41, 33, 28, 23, 
14, 0, -8, -12, -10, -2, 8, 19, 23, 19, 11, 5, -2, -8, -16, -25, 
-33, -37, -34, -24, -15, -14, -19, -21, -2, 22, 32, 22, 2, -13, -11, -6, 
-2, -1, -3, 1, 11, 22, 27, 20, 2, -23, -25, -19, -15, -17, -27, -34, 
-38, -35, -24, -12, 0, -3, -8, -7, 1, 12, 19, 19, 21, 27, 33, 27, 
17, 7, -3, -10, -10, 5, 15, 14, -3, -20, -23, -11, 3, 8, 4, 1, 
8, 17, 19, 11, 0, -9, -7, 1, 9, 11, 10, 13, 19, 20, 13, 1, 
-8, -12, -9, -9, -12, -17, -24, -31, -30, -25, -21, -14, 1, 15, 28, 37, 
41, 41, 42, 38, 23, 2, -15, -22, -22, -18, -18, -25, -36, -42, -34, -17, 
-6, -6, -13, -12, 3, 22, 34, 28, 11, -5, 1, 15, 30, 35, 21, 8, 
2, 3, 6, 3, -16, -29, -34, -32, -28, -27, -36, -40, -36, -22, -4, 13, 
8, -1, -3, 9, 25, 24, 3, -16, -18, -4, 14, 30, 27, 19, 10, 3, 
-8, -17, -28, -33, -26, -10, 5, 0, -10, -17, -20, -12, 5, 11, 14, 14, 
13, 19, 24, 23, 16, 9, 2, 1, 8, 15, 20, 25, 29, 18, -1, -17, 
-22, -17, -6, -3, -5, -8, -9, -8, -4, 2, 5, 6, 5, 3, -5, -21, 
-31, -26, -11, 5, 1, -7, -8, -3, 3, 8, 12, 13, 7, -4, -15, -17, 
-17, -18, -17, -13, -1, 6, 10, 9, 7, 7, 15, 21, 24, 20, 8, -11, 
-16, -12, -1, 9, 11, 1, -11, -19, -15, -4, 6, 2, -8, -13, -10, 0, 
12, 9, 2, 4, 17, 26, 8, -17, -35, -37, -28, -15, -1, 0, -5, -5, 
2, 19, 23, 15, -1, -9, -2, 19, 17, 2, -17, -30, -31, -13, 1, 7, 
3, -1, 7, 19, 26, 26, 18, 9, 0, -5, -11, -17, -21, -21, -11, -3, 
3, 5, 2, -5, -1, 7, 9, 3, -4, 1, 16, 29, 34, 32, 25, 4, 
-6, -8, -3, 2, 8, 14, 20, 23, 21, 18, 14, 15, 9, -4, -16, -22, 
-19, -17, -21, -26, -28, -23, -15, -8, 1, 10, 19, 32, 36, 34, 31, 31, 
35, 35, 27, 14, 1, -8, -12, -14, -21, -32, -40, -40, -30, -26, -25, -28, 
-28, -12, 6, 22, 27, 21, 12, 9, 18, 29, 31, 28, 23, 17, 15, 11, 
6, 4, 5, 6, 6, 1, -8, -19, -29, -28, -20, -9, -4, -11, -28, -27, 
-11, 7, 16, 5, -3, -2, 4, 7, 3, -10, -11, 2, 21, 35, 37, 17, 
5, -2, -6, -17, -35, -39, -40, -42, -46, -47, -41, -30, -14, 5, 18, 19, 
4, -4, -1, 12, 24, 24, 19, 16, 14, 12, 10, 15, 20, 19, 15, 17, 
19, 10, -7, -25, -33, -27, -10, -2, 1, 2, 5, 10, 11, 13, 22, 32, 
33, 19, -9, -26, -37, -36, -28, -15, -10, -7, -4, -2, 1, 10, 19, 28, 
30, 26, 18, 3, -4, -10, -12, -12, -5, -1, -7, -19, -25, -22, -15, -16, 
-19, -22, -25, -32, -29, -16, 3, 16, 20, 15, 9, 10, 18, 33, 42, 33, 
20, 15, 20, 30, 29, 18, 10, 7, 7, 1, -15, -29, -45, -58, -58, -44, 
-26, -33, -48, -55, -42, -4, 11, 7, -2, 0, 15, 48, 57, 45, 20, -2, 
-4, 22, 32, 23, 0, -21, -27, -16, -3, 3, 2, -8, -31, -33, -19, -2, 
5, 1, 0, 7, 11, 6, -7, -18, -12, 1, 13, 18, 9, -20, -26, -10, 
19, 40, 40, 14, -1, -8, -8, 0, 18, 24, 15, -2, -10, -3, 8, 3, 
1, 4, 10, 8, -17, -38, -48, -46, -33, -11, -10, -22, -36, -35, -19, 7, 
12, 7, -3, -14, -17, -6, 1, 2, -4, -14, -17, -3, 16, 25, 22, 15, 
14, 14, 6, -9, -23, -33, -35, -29, -22, -17, -15, -12, -7, 0, 9, 21, 
33, 35, 29, 22, 14, 11, 13, 10, 1, -11, -16, -9, 17, 31, 32, 25, 
17, 14, 21, 29, 35, 31, 19, 10, 19, 33, 33, 16, -12, -37, -32, -18, 
-9, -12, -20, -18, -5, 6, 5, -7, -22, -18, -6, 3, 5, 1, -8, -8, 
5, 26, 40, 35, -1, -23, -30, -24, -14, -8, -17, -30, -35, -33, -29, -27, 
-29, -25, -14, 0, 10, 4, -7, -12, -8, 2, 11, 8, 4, -1, 1, 9, 
16, 16, 16, 17, 20, 26, 34, 28, 11, -11, -28, -34, -32, -27, -18, -7, 
3, 8, 10, 14, 16, 16, 17, 21, 21, 25, 35, 43, 37, 20, 1, -9, 
-9, -7, -19, -30, -30, -22, -13, -12, -25, -32, -30, -20, -3, 26, 26, 11, 
0, 4, 18, 25, 22, 20, 18, 12, 9, 22, 36, 39, 25, 4, -16, -16, 
-16, -20, -21, -18, -8, -5, -12, -24, -29, -19, -9, 0, 8, 7, 0, -4, 
-1, 3, 2, -4, -8, -2, 7, 17, 29, 39, 33, 18, 2, -8, -11, -11, 
-18, -26, -34, -39, -41, -37, -21, -11, -5, 1, 13, 32, 31, 20, 9, 1, 
-4, -9, -7, -3, 1, 5, 11, 23, 27, 26, 25, 27, 25, 20, 15, 11, 
7, 5, -5, -14, -23, -32, -42, -50, -42, -30, -20, -17, -17, -12, -8, -8, 
-5, 5, 17, 27, 30, 34, 37, 36, 34, 32, 26, 17, 9, 3, -1, -4, 
-10, -17, -23, -27, -37, -51, -65, -72, -65, -47, -19, -8, -4, -8, -15, -13, 
5, 26, 36, 32, 22, 19, 28, 35, 37, 34, 24, 7, 1, 0, -4, -9, 
-13, -13, -17, -25, -34, -36, -20, -4, 6, 5, -1, -8, -16, -14, -1, 19, 
36, 41, 35, 29, 28, 30, 30, 22, 13, 6, 5, 3, -11, -25, -32, -28, 
-17, -11, -10, -11, -9, 0, 10, 14, 11, 2, -7, -12, -10, 2, 9, 6, 
-3, -8, -6, 9, 18, 18, 8, -7, -17, -14, -4, 5, 4, -3, -10, -4, 
5, 13, 17, 16, 13, 12, 12, 10, 5, -2, -17, -27, -33, -30, -20, -11, 
-11, -15, -17, -13, -3, 15, 21, 19, 12, 4, -4, -17, -21, -17, -13, -15, 
-23, -29, -30, -26, -15, 0, 18, 20, 14, 5, -2, -4, 1, 6, 6, -1, 
-10, -10, -2, 5, 5, 0, -7, -15, -18, -17, -12, -2, 10, 21, 22, 16, 
6, -3, -8, -10, -13, -16, -18, -22, -31, -34, -29, -17, -3, 10, 22, 26, 
29, 36, 45, 45, 33, 17, 2, -7, -9, -13, -23, -32, -36, -35, -32, -28, 
-28, -33, -37, -31, -9, 3, 6, 2, -1, 3, 18, 26, 25, 21, 15, 12, 
16, 18, 15, 4, -12, -27, -22, -13, -9, -14, -20, -12, 0, 6, 1, -6, 
-7, -1, 3, 0, -9, -18, -21, -17, -10, -4, -1, -1, 1, 8, 21, 35, 
45, 41, 31, 18, 5, -3, -5, -3, -6, -15, -26, -31, -23, 3, 13, 16, 
16, 21, 31, 30, 21, 14, 14, 20, 19, 8, -5, -13, -16, -18, -19, -19, 
-19, -21, -23, -13, 5, 24, 34, 34, 26, 18, 18, 18, 14, 8, -1, -8, 
-3, 5, 6, -2, -14, -8, 8, 17, 10, 0, -2, 7, 14, 16, 9, 0, 
-10, -10, -5, 1, 4, -1, -1, 3, 7, 10, 10, 15, 24, 31, 31, 27, 
26, 29, 31, 31, 28, 23, 16, 18, 23, 22, 14, 2, -2, 5, 13, 13, 
3, -9, -17, -14, -13, -16, -21, -25, -15, 1, 13, 14, 4, -12, -9, 7, 
25, 34, 32, 28, 23, 16, 11, 8, -1, -11, -22, -29, -32, -33, -29, -23, 
-18, -18, -24, -33, -33, -26, -14, 3, 20, 29, 18, 1, -10, -9, -6, -5, 
-5, -4, -3, -5, -5, 1, 10, 15, 13, 8, 5, 5, 4, 2, -1, -4, 
-13, -20, -23, -20, -15, -12, -13, -12, -7, 1, 6, 1, -12, -24, -27, -18, 
-7, -1, -7, -15, -14, -2, 10, 6, -3, -8, -7, -3, 0, -3, -1, 12, 
27, 34, 22, 7, -3, -4, 3, 9, 0, -18, -39, -53, -55, -41, -28, -18, 
-14, -11, -10, -8, 0, 12, 20, 18, 5, 1, 2, 5, 8, 8, 8, 9, 
4, -8, -21, -27, -22, -17, -16, -16, -17, -28, -37, -41, -34, -21, -12, -13, 
-13, -8, 3, 14, 23, 28, 26, 28, 31, 28, 5, -14, -24, -19, -7, -1, 
-11, -22, -28, -28, -22, -11, -9, -15, -27, -36, -33, -4, 16, 27, 29, 26, 
19, 12, 16, 22, 22, 15, -10, -26, -34, -35, -31, -25, -22, -23, -21, -17, 
-10, -3, 9, 17, 22, 22, 20, 20, 26, 36, 43, 43, 35, 20, 16, 16, 
20, 21, 14, -10, -25, -32, -26, -13, 5, 7, 7, 8, 9, 10, 10, 8, 
2, -2, 1, 8, 9, 6, 5, 11, 19, 32, 37, 35, 25, 10, -2, -5, 
3, 7, 4, -7, -17, -18, -8, 6, 17, 22, 14, 1, -9, -12, -16, -26, 
-35, -27, -13, -5, -6, -14, -18, -5, 18, 40, 48, 24, -6, -26, -24, -5, 
12, 7, -12, -27, -29, -19, -7, -4, -7, -5, 5, 16, 17, 3, -13, -17, 
-5, 13, 18, 5, -12, -25, -26, -18, -6, -3, -4, -6, -4, 5, 12, 16, 
17, 14, 13, 16, 13, 3, -10, -22, -29, -27, -25, -23, -24, -27, -31, -22, 
-4, 19, 33, 30, 6, -3, 1, 17, 29, 23, 7, -7, -10, -4, 1, -4, 
-12, -16, -17, -16, -17, -24, -26, -17, 0, 15, 19, 9, -4, -11, -5, 9, 
22, 18, 8, -1, -2, 6, 21, 20, 12, 8, 7, 3, -5, -16, -26, -33, 
-36, -25, -15, -9, -8, -6, 4, 28, 39, 46, 49, 47, 24, 7, 5, 15, 
26, 31, 24, 8, -12, -27, -31, -24, -9, -9, -15, -20, -16, 0, 5, 7, 
9, 11, 10, 9, 10, 12, 14, 17, 21, 21, 16, 10, 9, 16, 21, 11, 
-7, -19, -20, -11, 7, 15, 16, 17, 18, 17, 9, 3, -1, -5, -10, -18, 
-18, -14, -10, -5, -2, -5, -7, -4, 2, 7, 6, -1, -6, -11, -17, -22, 
-23, -18, -14, -15, -18, -18, -7, 5, 13, 15, 9, 0, 2, 12, 20, 21, 
13, 0, -1, 1, 1, -2, -6, -16, -19, -17, -14, -13, -15, -15, -11, -4, 
4, 11, 27, 32, 31, 25, 16, 7, -8, -13, -14, -14, -12, -12, -17, -24, 
-27, -26, -24, -20, -17, -14, -15, -16, -16, -14, -14, -18, -19, -12, 11, 19, 
21, 21, 20, 21, 18, 11, 9, 15, 17, 13, 1, -5, -14, -24, -30, -28, 
-28, -33, -36, -29, -11, 20, 38, 46, 41, 23, -1, -23, -15, 7, 23, 20, 
-8, -23, -23, -7, 12, 22, 14, 3, -2, 2, 6, 5, -1, -2, -3, -8, 
-19, -27, -24, -18, -9, 2, 14, 25, 21, 9, -1, 3, 17, 27, 12, -10, 
-25, -24, -10, -1, 7, 10, 5, -2, 0, 5, 9, 6, -3, -16, -31, -30, 
-21, -13, -11, -14, -7, 4, 10, 8, 2, 0, 11, 24, 31, 32, 14, -3, 
-12, -9, 0, 5, 1, -3, -4, -5, -9, -17, -27, -27, -25, -24, -22, -21, 
-25, -30, -28, -15, 0, 6, -3, -13, -14, -3, 10, 12, 6, 1, 3, 9, 
19, 21, 21, 20, 20, 20, 13, 4, -5, -11, -13, -15, -25, -31, -34, -37, 
-36, -21, -5, 6, 5, 1, 4, 24, 37, 41, 42, 44, 42, 28, 18, 15, 
15, 12, -4, -19, -30, -30, -20, -7, 9, 11, 11, 11, 12, 13, 5, -7, 
-22, -35, -39, -24, -6, 7, 9, 2, -1, 21, 39, 45, 42, 35, 27, 16, 
7, -3, -15, -24, -23, -14, -5, 0, 1, 4, 20, 34, 43, 43, 33, 19, 
5, -3, -12, -21, -26, -27, -27, -25, -17, -5, 5, 13, 12, 8, 5, 6, 
9, 15, 20, 19, 13, 6, 10, 17, 20, 16, 11, 12, 21, 16, 2, -14, 
-23, -22, -17, -15, -16, -15, -11, 1, 7, 7, 4, 2, 2, 4, 4, 2, 
-1, 0, 9, 13, 16, 17, 14, 9, -2, -8, -11, -12, -14, -15, -14, -13, 
-13, -12, -14, -16, -17, -17, -15, -10, 2, 27, 38, 39, 35, 32, 29, 23, 
20, 16, 6, -9, -32, -37, -33, -28, -27, -32, -44, -47, -41, -29, -16, -5, 
6, 12, 18, 22, 24, 24, 28, 31, 24, 6, -16, -33, -28, -14, 0, 9, 
8, -7, -11, -11, -7, -3, -2, -5, -17, -33, -45, -47, -39, -29, -13, 6, 
23, 34, 34, 29, 26, 32, 44, 49, 35, 10, -17, -31, -27, -3, 6, 4, 
-5, -16, -26, -27, -20, -13, -10, -14, -22, -26, -29, -29, -23, -10, 15, 28, 
35, 35, 28, 20, 14, 17, 18, 14, 6, -3, -5, -1, 8, 15, 19, 21, 
26, 32, 34, 29, 12, 5, 4, 7, 10, 6, -8, -15, -19, -15, -9, -9, 
-21, -30, -31, -22, -5, 20, 28, 27, 19, 16, 22, 30, 25, 11, -4, -15, 
-19, -18, -16, -18, -24, -31, -23, -7, 5, 10, 10, 10, 5, -3, -11, -13, 
-11, -8, -5, -6, -15, -22, -17, 13, 33, 38, 27, 14, 10, 18, 19, 14, 
4, -9, -20, -30, -25, -17, -14, -15, -19, -18, -15, -12, -9, -9, -17, -20, 
-17, -9, -2, 0, 4, 9, 10, 2, -11, -25, -25, -21, -17, -17, -22, -21, 
-16, -9, 0, 9, 15, 14, 9, 5, 1, -6, -15, -15, -7, 4, 14, 19, 
26, 30, 29, 22, 12, 4, -1, -7, -16, -23, -27, -27, -22, -12, 4, 18, 
29, 28, 16, 0, -10, -7, 3, 21, 29, 29, 21, 10, -5, -7, -1, 6, 
6, 1, -7, -14, -23, -31, -32, -21, -19, -30, -45, -55, -54, -35, -16, 0, 
9, 12, 10, 5, 6, 11, 14, 10, -5, -12, -12, -6, 1, 6, 7, 6, 
6, 9, 14, 16, 9, 4, 0, -2, -1, -1, 0, 2, 5, 7, 10, 15, 
14, 6, -4, -9, -7, 5, 12, 15, 10, 1, -3, 4, 14, 21, 22, 19, 
8, -4, -13, -13, -7, 0, -2, -8, -8, -1, 14, 29, 26, 15, 4, -3, 
-9, -9, -1, 9, 16, 14, 7, -1, 1, 4, 2, -4, -13, -13, -8, -2, 
-2, -8, -8, 4, 19, 25, 19, 10, 6, 9, 12, 10, 6, -4, -5, 0, 
6, 11, 8, -2, -2, 3, 8, 9, 3, -9, -14, -15, -11, -2, 15, 17, 
8, -5, -9, -3, 19, 30, 33, 29, 16, -2, -16, -5, 13, 22, 15, -7, 
-10, -1, 8, 7, -6, -23, -22, -13, -6, -4, -13, -18, -12, 5, 25, 34, 
24, 18, 22, 30, 27, 12, -11, -15, -12, -11, -17, -30, -26, -13, 1, 4, 
-10, -36, -36, -20, -2, 7, 6, -2, -1, 7, 17, 22, 14, 1, -11, -18, 
-19, -15, -12, -17, -21, -17, -8, 2, 14, 20, 21, 17, 9, -6, -15, -22, 
-28, -32, -31, -21, -16, -16, -16, -10, 3, 27, 38, 38, 30, 19, 12, 14, 
20, 23, 17, 2, -33, -53, -63, -57, -39, -23, -19, -25, -22, -9, 6, 11, 
4, -3, -5, -1, 9, 30, 35, 29, 18, 10, 6, -11, -28, -36, -33, -28, 
-32, -34, -26, -15, -6, -4, -10, -13, -13, -7, 0, 0, -9, -7, 6, 20, 
29, 26, 22, 26, 32, 33, 25, 6, 2, 4, 9, 11, 8, -3, -15, -19, 
-9, 9, 18, 2, -17, -24, -15, -1, 11, 13, 12, 7, 1, -1, 4, 9, 
11, 8, 4, 2, 2, 5, 5, 1, -9, -28, -34, -33, -26, -16, 5, 20, 
29, 27, 17, 6, 2, 7, 12, 10, -2, -18, -29, -15, 8, 21, 15, -13, 
-20, -13, -3, 0, -7, -17, -14, -4, 6, 11, 11, 0, -3, 4, 15, 20, 
1, -18, -28, -26, -15, -3, 3, 1, -5, -15, -23, -21, -11, -11, -18, -24, 
-24, -16, -11, -9, -11, -16, -16, -6, 4, 5, 1, 1, 11, 34, 40, 37, 
29, 24, 18, 5, -12, -22, -21, -16, -19, -23, -20, -11, 0, 7, 7, 3, 
-6, -16, -21, -6, 9, 16, 19, 27, 41, 52, 44, 31, 25, 24, 17, -8, 
-23, -32, -35, -34, -28, -18, -6, 0, 0, -5, -10, -9, -3, 9, 24, 36, 
38, 25, 7, -5, -4, 8, 9, 0, -10, -13, -4, 12, 14, 12, 9, 9, 
7, 2, -4, -9, -11, -13, -14, -13, -9, -3, 2, 5, 0, -9, -13, -10, 
1, 19, 21, 14, 8, 9, 16, 11, -6, -23, -30, -21, -4, 13, 4, -15, 
-26, -19, 8, 11, -5, -23, -24, -8, 23, 27, 17, -1, -20, -32, -31, -22, 
-16, -18, -22, -9, 10, 21, 14, -2, -12, -1, 16, 28, 31, 25, 18, 7, 
7, 12, 14, 8, -10, -19, -21, -15, -6, 0, 1, -4, -11, -15, -8, 8, 
26, 22, 10, 5, 9, 12, 4, -3, -6, -10, -16, -16, -12, -13, -22, -27, 
-19, 5, 6, -5, -13, -8, 13, 20, 20, 11, -3, -13, -8, 5, 9, -2, 
-16, -22, -17, -11, -5, 1, 2, -11, -19, -16, -5, 3, -1, -8, -2, 13, 
29, 37, 35, 16, 2, 0, 5, 3, -17, -28, -26, -10, 4, 6, -10, -16, 
-9, 3, 11, 5, -11, -19, -16, 0, 22, 39, 28, 7, -8, -4, 16, 38, 
30, 9, -7, -12, -4, 2, 3, -4, -18, -27, -7, 17, 22, 4, -19, -28, 
-3, 20, 28, 21, 5, -14, -12, 4, 21, 25, 16, 2, 3, 10, 16, 15, 
7, -4, 2, 13, 15, -4, -47, -59, -47, -24, -3, 4, -7, -16, -17, -5, 
17, 42, 55, 38, 10, -11, -12, 5, 12, 10, 2, 0, 7, 15, 3, -19, 
-38, -45, -35, -7, 8, 10, 3, -8, -4, 17, 34, 33, 9, -20, -40, -28, 
-8, 9, 17, 17, 8, 2, 0, 0, -1, -17, -33, -46, -43, -26, -7, -7, 
-20, -18, 3, 28, 37, 14, -5, -12, -8, 1, 13, 8, -6, -18, -24, -26, 
-33, -40, -44, -38, -23, -2, 29, 36, 28, 12, 0, -2, 5, 10, 8, 4, 
-4, -13, -8, 7, 22, 23, 2, -9, -10, -8, -14, -23, -30, -23, -11, 2, 
10, 3, -18, -15, 5, 28, 37, 16, -6, -17, -14, -3, 7, 6, -1, 3, 
17, 32, 30, -17, -49, -55, -30, 8, 36, 11, -31, -60, -56, -22, 23, 22, 
4, -17, -26, -16, 18, 36, 40, 34, 24, 14, 13, 12, 8, -3, -17, -32, 
-32, -27, -20, -10, 5, 24, 24, 16, 7, 2, -2, -6, -6, 1, 11, 12, 
-4, -14, -13, -5, 3, 6, 9, 9, 8, 10, 16, 28, 30, 27, 22, 19, 
20, 17, 1, -21, -37, -38, -20, 6, 7, -4, -16, -22, -16, -8, -1, 3, 
5, 7, 8, 10, 11, 9, 5, 6, 14, 15, 9, 1, 0, 11, 16, 13, 
6, 4, 9, 18, 16, 8, 0, -1, 7, 20, 17, -1, -21, -31, -25, -22, 
-21, -16, -5, 6, 3, -4, -12, -18, -17, 2, 20, 31, 25, 8, -1, 22, 
42, 41, 13, -20, -36, -24, -7, -3, -13, -24, -23, -7, 17, 33, 20, -14, 
-45, -23, 18, 44, 35, 3, -21, -6, 17, 25, 13, -19, -27, -19, 0, 19, 
24, 7, -4, -4, 2, 1, -9, -22, -13, 3, 12, 8, -8, -9, 0, 12, 
22, 25, 6, -12, -24, -25, -14, 5, 25, 24, 13, 0, -10, -14, -6, 7, 
19, 16, -2, -20, -12, 4, 14, 13, 6, -6, -8, 0, 12, 20, 10, -1, 
-2, 7, 13, 3, -26, -33, -29, -20, -13, -12, -12, -9, 0, 13, 24, 26, 
16, 10, 15, 29, 38, 23, -2, -23, -31, -30, -28, -35, -42, -43, -37, -26, 
-10, -4, -1, 4, 13, 21, 20, 9, -2, -8, -13, -17, -22, -21, -15, -12, 
-16, -29, -33, -25, -7, 11, 20, 28, 31, 26, 9, -9, -28, -36, -42, -42, 
-34, -21, -7, -6, -3, 3, 8, 8, -12, -25, -24, -7, 9, 8, -4, -14, 
-16, -9, 3, 21, 15, -7, -32, -45, -39, -11, 5, 8, -1, -17, -27, -15, 
7, 29, 40, 34, 2, -11, -7, 0, -2, -13, -26, -20, -7, 3, 2, -12, 
-8, 8, 22, 18, -2, -16, -2, 16, 24, 18, 8, 3, 6, 9, 3, -9, 
-22, -21, -10, 11, 26, 25, -2, -14, -10, 3, 10, 8, 0, -6, -17, -28, 
-32, -18, -3, 12, 23, 25, 13, -9, -12, 0, 19, 33, 37, 28, 16, 8, 
8, 13, 9, -5, -20, -27, -28, -24, -19, -22, -22, -8, 14, 30, 19, 1, 
-7, 4, 28, 48, 39, 20, 2, -1, 14, 27, 9, -18, -30, -19, 0, 6, 
-8, -20, -13, 10, 44, 44, 26, 0, -19, -18, 14, 31, 26, 6, -7, 8, 
23, 19, -2, -20, -21, -1, 5, 0, -6, -10, -10, 0, 12, 23, 23, 12, 
-9, -13, -12, -11, -10, -4, 6, 6, -2, -11, -9, 0, -4, -22, -30, -19, 
0, 9, -1, -11, -10, -1, 8, 13, 7, -6, -16, -16, -8, 6, 10, 8, 
3, -5, -15, -26, -34, -27, -6, 13, 4, -14, -22, -15, -4, 2, 9, 14, 
15, 12, 9, 13, 29, 48, 57, 51, 38, 14, 1, -10, -20, -27, -31, -30, 
-21, -8, 0, -1, -5, 1, 15, 24, 26, 23, 19, 15, 5, -8, -15, -9, 
14, 18, 7, -6, -8, 9, 18, 24, 25, 22, 13, 3, 9, 12, 4, -13, 
-24, -14, -1, 3, -3, -6, 8, 17, 17, 12, 13, 22, 25, 12, -3, -14, 
-20, -20, -11, -2, -2, -16, -31, -27, -15, -4, 3, 11, 19, 12, -3, -12, 
-6, 11, 35, 39, 28, 7, -14, -28, -33, -29, -20, -12, -9, -11, -20, -23, 
-21, -12, -1, 8, 13, 11, -2, -13, -9, 16, 26, 23, 17, 15, 14, 7, 
-2, -8, -5, 5, 9, -2, -12, -13, -6, 2, -1, -11, -22, -28, -26, -19, 
3, 11, 6, -4, -10, -2, 3, 2, 1, 7, 17, 24, 20, 9, -2, -5, 
-5, -2, 1, 2, -6, -16, -18, -13, -13, -24, -42, -55, -49, -32, -12, 5, 
11, 8, 0, 5, 20, 32, 30, -2, -25, -32, -20, -9, -11, -22, -20, -18, 
-23, -25, -15, 18, 33, 31, 19, 5, -7, -3, 7, 14, 12, -1, -23, -32, 
-34, -32, -33, -32, -9, 16, 25, 10, -14, -23, -7, 10, 17, 14, 9, 9, 
11, 11, 7, 1, -2, -3, -2, 4, 12, 12, -3, -6, 5, 14, 7, -8, 
-17, -7, 0, -5, -13, -12, -5, 0, 5, 10, 11, 4, 2, 5, 9, 10, 
9, 11, 22, 31, 31, 19, -2, -12, -17, -20, -22, -20, -23, -29, -32, -26, 
-10, 6, 8, 1, 2, 12, 20, 2, -17, -16, 7, 28, 29, 6, -2, 3, 
12, 18, 19, 8, -12, -28, -26, -10, 0, -13, -24, -19, -1, 11, 4, -3, 
3, 19, 33, 31, 15, 12, 13, 10, 0, -13, -20, -28, -29, -21, -8, -13, 
-35, -49, -36, -4, 19, 14, -2, -8, 7, 33, 59, 54, 35, 11, -4, 0, 
30, 33, 14, -8, -14, -8, -8, -17, -22, -19, -13, -15, -26, -30, -18, 6, 
31, 47, 46, 38, 28, 22, 26, 37, 30, 11, -12, -27, -27, -23, -25, -31, 
-36, -34, -31, -33, -32, -23, -10, -1, 7, 15, 22, 22, 18, 24, 34, 38, 
32, 17, 8, 13, 11, -6, -23, -26, -10, -6, -14, -25, -26, -12, 23, 38, 
40, 32, 21, 18, 22, 16, -4, -23, -29, -14, -2, 1, -6, -12, -9, 16, 
31, 33, 21, 5, -3, 11, 23, 27, 21, 12, 9, 9, -1, -20, -36, -40, 
-27, -18, -12, -11, -13, -12, 3, 13, 18, 16, 12, 16, 17, 8, -4, -9, 
-6, 2, 0, -7, -11, -10, -7, -2, 1, 2, 2, 4, 9, 11, 10, 5, 
-1, -3, 1, -1, -13, -30, -37, -31, -10, -3, -1, -2, -7, -9, -3, 7, 
15, 12, -3, -25, -19, -3, 8, 1, -14, -15, -1, 10, 13, 8, -4, -6, 
4, 19, 29, 28, 6, -11, -23, -30, -35, -37, -31, -22, -14, -18, -28, -19, 
5, 28, 36, 33, 29, 33, 42, 44, 32, 12, -4, -6, -3, -5, -15, -28, 
-39, -39, -32, -21, -12, -5, 7, 18, 29, 35, 37, 40, 46, 48, 37, 17, 
-3, -14, -10, -7, -11, -19, -27, -35, -31, -20, -8, -6, -9, 0, 20, 42, 
51, 44, 20, 14, 20, 29, 31, 21, -13, -41, -62, -70, -63, -33, -20, -23, 
-34, -39, -26, -2, 0, -6, -6, 8, 29, 45, 39, 28, 18, 16, 27, 30, 
18, -9, -33, -41, -26, -14, -6, -3, -1, 0, -11, -21, -24, -15, -5, -3, 
-5, -4, 1, 9, 17, 17, 9, -3, -8, -3, 6, 11, 9, 9, 13, 19, 
26, 22, 12, -3, -17, -25, -31, -33, -32, -25, -17, -13, -5, -1, 3, 10, 
20, 32, 33, 32, 31, 31, 29, 13, -2, -17, -33, -51, -63, -55, -36, -13, 
2, 5, 0, 7, 18, 27, 33, 35, 28, 13, -3, -14, -11, -1, 6, 0, 
-7, -8, -2, 2, 1, 0, -4, -6, -1, 7, 2, -14, -30, -33, -13, -1, 
-2, -9, -17, -23, -16, -2, 11, 20, 24, 25, 31, 30, 16, -2, -14, -12, 
-3, -2, -11, -28, -45, -48, -33, -20, -17, -26, -35, -27, -10, 1, 1, 2, 
13, 26, 38, 43, 38, 24, 10, 8, 2, -9, -23, -32, -38, -40, -43, -43, 
-40, -31, -19, -2, 11, 18, 23, 27, 21, 12, 11, 19, 28, 24, 12, 3, 
2, 7, 14, 15, 14, 4, -12, -21, -5, 11, 14, 4, -8, -11, -3, -6, 
-19, -30, -31, -19, -8, 5, 20, 30, 34, 26, 12, -2, -12, -17, -26, -43, 
-45, -33, -15, -4, 4, 13, 19, 14, 1, -7, 4, 11, 13, 16, 26, 39, 
39, 29, 21, 19, 19, 17, 11, -3, -20, -31, -29, -8, 7, 7, -9, -29, 
-32, 1, 30, 42, 29, 7, -5, 7, 23, 29, 23, 9, -5, 0, 6, 2, 
-8, -12, -7, -6, -10, -20, -36, -43, -28, -3, 16, 22, 17, 11, 17, 27, 
31, 24, -2, -8, 2, 17, 20, 8, -9, -9, 0, 8, 8, -1, -22, -27, 
-19, -6, 4, 0, -5, 2, 17, 29, 23, -3, -10, 0, 17, 25, 22, 15, 
17, 18, 17, 10, -18, -36, -39, -23, 1, 19, 23, 12, 7, 13, 20, 16, 
-15, -36, -41, -32, -23, -14, -6, 7, 17, 23, 21, 2, -13, -19, -9, 10, 
22, 14, 2, -3, 1, 10, 11, -3, -24, -39, -39, -28, -14, -10, -4, 4, 
10, 8, -8, -17, -18, -11, -2, 4, 0, -11, -26, -33, -28, -7, 6, 9, 
-1, -16, -19, -4, 3, 8, 16, 25, 20, -1, -24, -34, -24, -4, 12, 8, 
5, 4, 0, -10, -17, -10, 0, 7, 5, -14, -30, -40, -38, -24, -7, -2, 
-18, -38, -46, -37, -4, 15, 23, 26, 26, 18, -14, -25, -16, 8, 30, 40, 
29, 13, 0, -10, -16, -19, -17, -17, -23, -33, -41, -44, };