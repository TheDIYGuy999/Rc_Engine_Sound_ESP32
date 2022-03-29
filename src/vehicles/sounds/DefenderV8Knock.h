const unsigned int knockSampleRate = 22050;
const unsigned int knockSampleCount = 834;
const signed char knockSamples[] = {
0, -1, 0, 0, 1, 2, 2, 3, 4, 5, 6, 7, 7, 7, 7, 6,
5, 4, 3, 1, 0, -1, -3, -4, -6, -7, -8, -8, -9, -9, -9, -9, 
-10, -10, -11, -12, -13, -15, -17, -18, -21, -24, -26, -29, -31, -32, -34, -35, 
-36, -37, -37, -39, -39, -39, -40, -41, -42, -45, -46, -47, -49, -51, -52, -54, 
-56, -56, -56, -58, -58, -58, -58, -59, -59, -59, -61, -62, -63, -64, -66, -66, 
-68, -69, -69, -69, -69, -69, -69, -69, -69, -70, -70, -71, -71, -72, -72, -72, 
-73, -72, -72, -72, -71, -71, -68, -67, -66, -66, -65, -65, -65, -65, -65, -66, 
-66, -65, -65, -65, -64, -63, -63, -62, -61, -60, -59, -58, -57, -56, -55, -54, 
-52, -51, -50, -48, -47, -45, -44, -43, -42, -41, -40, -39, -39, -38, -38, -38, 
-38, -38, -37, -36, -36, -35, -35, -33, -32, -30, -29, -27, -27, -26, -25, -23, 
-22, -22, -22, -21, -21, -21, -21, -22, -22, -22, -22, -22, -22, -22, -21, -21, 
-19, -18, -17, -17, -16, -14, -13, -13, -13, -13, -14, -14, -14, -14, -14, -14, 
-14, -13, -12, -11, -10, -8, -6, -5, -4, -3, -2, -2, -1, 0, 2, 3, 
4, 6, 10, 13, 15, 17, 20, 23, 25, 27, 29, 31, 32, 34, 36, 38, 
40, 43, 45, 48, 51, 54, 56, 59, 62, 65, 67, 68, 70, 72, 75, 76, 
78, 80, 81, 82, 85, 87, 89, 91, 94, 97, 99, 101, 103, 104, 106, 108, 
107, 108, 109, 110, 110, 112, 113, 114, 116, 117, 120, 121, 122, 123, 123, 123, 
124, 125, 125, 125, 125, 125, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 
127, 126, 126, 125, 124, 123, 122, 122, 121, 120, 119, 118, 116, 115, 113, 112, 
111, 110, 109, 108, 107, 107, 106, 106, 106, 105, 104, 104, 103, 102, 101, 100, 
99, 97, 96, 95, 94, 92, 91, 89, 86, 85, 84, 82, 81, 80, 77, 76, 
75, 74, 72, 71, 68, 66, 64, 62, 60, 56, 54, 51, 49, 47, 44, 42, 
40, 38, 37, 36, 34, 33, 32, 30, 29, 27, 25, 24, 22, 19, 17, 14, 
11, 9, 6, 5, 3, 1, 0, -1, -1, -2, -3, -4, -5, -7, -9, -10, 
-11, -14, -16, -17, -18, -19, -19, -19, -19, -19, -19, -20, -20, -20, -21, -22, 
-22, -23, -23, -23, -23, -22, -21, -20, -17, -16, -16, -14, -14, -12, -12, -12, 
-12, -12, -12, -11, -10, -9, -8, -6, -5, -2, 0, 1, 2, 3, 4, 4, 
4, 5, 5, 5, 6, 7, 8, 10, 11, 12, 14, 15, 16, 16, 16, 16, 
15, 14, 14, 12, 12, 12, 12, 13, 13, 14, 14, 14, 14, 13, 12, 12, 
9, 8, 7, 6, 5, 5, 5, 5, 6, 7, 8, 9, 10, 10, 10, 10, 
9, 6, 3, 1, -1, -4, -7, -11, -13, -15, -16, -17, -17, -18, -18, -19, 
-20, -21, -23, -24, -26, -29, -31, -33, -35, -37, -39, -42, -44, -47, -49, -50, 
-53, -56, -58, -62, -64, -65, -67, -69, -71, -73, -74, -75, -76, -76, -77, -77, 
-77, -78, -79, -79, -80, -82, -83, -84, -84, -85, -85, -85, -84, -84, -84, -83, 
-83, -82, -82, -82, -81, -79, -78, -77, -76, -74, -72, -69, -67, -66, -65, -63, 
-62, -61, -61, -60, -60, -60, -59, -59, -58, -57, -56, -55, -53, -51, -49, -47, 
-44, -42, -40, -37, -35, -33, -32, -30, -28, -27, -25, -24, -21, -19, -18, -17, 
-15, -14, -11, -9, -8, -7, -5, -5, -3, -3, -3, -2, -2, -1, 0, 2, 
3, 5, 7, 11, 13, 16, 18, 19, 20, 22, 22, 23, 23, 24, 25, 26, 
27, 29, 31, 33, 37, 39, 42, 44, 46, 48, 48, 49, 49, 49, 50, 51, 
53, 54, 56, 58, 61, 64, 66, 68, 69, 70, 70, 70, 70, 70, 70, 71, 
71, 72, 73, 74, 74, 75, 75, 75, 75, 75, 75, 75, 73, 72, 70, 69, 
68, 66, 64, 62, 60, 59, 58, 57, 56, 55, 53, 52, 51, 49, 47, 45, 
43, 41, 39, 37, 35, 32, 30, 28, 25, 24, 22, 20, 18, 16, 14, 13, 
11, 9, 7, 5, 3, 2, 1, -1, -3, -5, -7, -9, -12, -13, -16, -19, 
-21, -23, -24, -25, -26, -26, -26, -26, -27, -28, -29, -30, -31, -33, -34, -37, 
-38, -37, -38, -39, -39, -38, -37, -38, -37, -36, -37, -37, -36, -35, -36, -35, 
-36, -36, -35, -36, -35, -33, -31, -31, -30, -29, -28, -28, -28, -28, -29, -30, 
-29, -29, -29, -30, -29, -28, -27, -25, -25, -25, -24, -24, -23, -23, -23, -24, 
-24, -25, -26, -26, -27, -27, -28, -27, -28, -27, -27, -26, -26, -25, -25, -24, 
-24, -24, -23, -21, -19, -18, -18, -17, -16, -15, -15, -15, -15, -16, -16, -15, 
-14, -13, -13, -12, -10, -8, -7, -6, -5, -4, -4, -3, -2, -1, -1, 0, 
0, 0, };