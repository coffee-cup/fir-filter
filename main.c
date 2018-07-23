#include <stdio.h>

#define SIGNAL_LENGTH (200)
#define TAPS_LENGTH (100)

#define TRUE (1)
#define FALSE (0)

#define MACRO_FIR(input, coefs, taps, output, n)                               \
    ({                                                                         \
        register int k;                                                        \
        register int sum = 0;                                                  \
                                                                               \
        /* move input sample into taps */                                      \
        taps[0] = input[n];                                                    \
                                                                               \
        /* perform the convolution for a single output sample */               \
        for (k = TAPS_LENGTH - 1; k >= 0; k--) {                               \
            sum += (int)coefs[k] * (int)taps[k];                               \
        }                                                                      \
                                                                               \
        /* Place sum in output signal */                                       \
        output[n] = (short)(sum);                                              \
                                                                               \
        /* shift taps to right */                                              \
        for (k = TAPS_LENGTH - 1; k > 0; k--) {                                \
            taps[k] = taps[k - 1];                                             \
        }                                                                      \
    })

// Unoptimized C
void unoptimized_fir(short *input, short *coefs, short *taps, short *output,
                     short n) {
    int k;

    // move input sample into taps
    taps[0] = input[n];

    // perform the convolution for a single output sample
    int sum = 0;
    for (k = 0; k < TAPS_LENGTH; ++k) {
        sum += coefs[k] * taps[k];
    }

    // Place sum in output signal
    output[n] = sum;

    // shift taps to right
    for (k = TAPS_LENGTH - 1; k > 0; k--) {
        taps[k] = taps[k - 1];
    }
}

// Optimized C
/*
  Optimizations:
  1. Use short's (16 bit) instead of int's (32 bit)
  2. Use restrict pointers as arrays should never be the same
  3. Use registers for variables that will be accessed a lot
*/
inline void optimized_fir(short *input, short *coefs, short *taps,
                          short *output, short n) {
    register int k;
    register int sum = 0;

    // move input sample into taps
    taps[0] = input[n];

    // prologue
    k = TAPS_LENGTH - 1;
    sum += coefs[k] * taps[k];

    // perform the convolution for a single output sample
    for (; k > 0; k--) {
        sum += coefs[k - 1] * taps[k - 1];
        taps[k] = taps[k - 1];
    }

    // Place sum in output signal
    output[n] = (short)(sum);
}

void optimized_asm_fir(short *input, short *coefs, short *taps,
                              short *output, short n) {
    register int k;
    register int sum = 0;

    register short temp1;
    register short temp2;
    register int tk;

    // move input sample into taps
    taps[0] = input[n];

    // prologue
    sum += coefs[TAPS_LENGTH - 1] * taps[TAPS_LENGTH - 1];

    // k = TAPS_LENGTH - 1
    /* __asm__ volatile("mov %0, %[tl]" : "=r"(k) : [tl] "I"(TAPS_LENGTH * 2 -
     * 2)); */
    k = TAPS_LENGTH * 2 - 2;

    // label for loop
    __asm__ volatile("\n1:");

    while (k > 0) {
        // tk = k - 1
        __asm__ volatile("sub %0, %1, #2" : "=r"(tk) : "r"(k));

        // sum += coefs[k - 1] * taps[k - 1]
        __asm__ volatile("ldrsh %0, [%1, %2]"
                         : "=r"(temp1)
                         : "r"(coefs), "r"(tk));
        __asm__ volatile("ldrsh %0, [%1, %2]"
                         : "=r"(temp2)
                         : "r"(taps), "r"(tk));
        __asm__ volatile("mla %0, %1, %2, %3"
                         : "=r"(sum)
                         : "r"(temp1), "r"(temp2), "r"(sum));

        /* printf("%d\t%d\n", tk, sum); */

        // taps[k] = temp2
        /* __asm__ volatile("strh %1, [%0, %2]" : "=r"(taps) : "r"(temp2),  "r"(k)); */
        taps[k >> 1] = temp2;

        // k = k - 1
        __asm__ volatile("mov %0, %1" : "=r"(k) : "r"(tk));
    }

    // check loop condition
    /* __asm__ volatile("bne 1"); */

    // Place sum in output signal
    output[n] = (short)(sum);
}

// Initializes all the arrays with values for the testbench
void init(short *input, short *coefs, short *taps, short *output) {
    int i;

    // zero the taps
    for (i = 0; i < TAPS_LENGTH; ++i) {
        taps[i] = 0;
    }

    // set values for coefs
    // TODO: make this better
    short h[TAPS_LENGTH] = {
        6,  19, 2,  0,  7,  10, 5,  10, 17, 0,  16, 9,  16, 19, 19, 6,  19,
        18, 16, 7,  7,  15, 12, 17, 11, 17, 10, 11, 9,  11, 3,  4,  2,  4,
        19, 3,  7,  8,  15, 12, 3,  2,  2,  5,  9,  7,  6,  6,  2,  2,  0,
        18, 10, 4,  19, 16, 1,  8,  6,  15, 9,  0,  19, 14, 11, 0,  1,  15,
        11, 0,  0,  0,  16, 7,  3,  3,  13, 16, 17, 4,  19, 11, 12, 18, 15,
        10, 5,  19, 7,  9,  3,  12, 13, 18, 0,  14, 15, 9,  10, 1};
    for (i = 0; i < TAPS_LENGTH; ++i) {
        coefs[i] = h[i];
    }

    // set values for input signal and zero output
    // TODO: make this better
    short x[SIGNAL_LENGTH] = {
        3, 1, 2, 1, 3, 3, 1, 1, 0, 0, 1, 0, 1, 3, 1, 1, 2, 2, 3, 0, 2, 1, 0,
        0, 2, 1, 3, 3, 0, 2, 1, 1, 1, 0, 1, 1, 0, 1, 1, 3, 2, 1, 0, 1, 0, 3,
        2, 3, 3, 3, 2, 1, 1, 0, 1, 1, 3, 3, 0, 1, 1, 0, 3, 3, 2, 1, 2, 0, 2,
        1, 0, 2, 1, 3, 3, 3, 3, 0, 2, 3, 3, 0, 3, 3, 1, 0, 0, 0, 1, 2, 0, 2,
        2, 3, 1, 1, 3, 2, 1, 3, 0, 0, 0, 3, 2, 2, 0, 1, 2, 2, 0, 0, 3, 3, 1,
        0, 3, 0, 2, 2, 1, 3, 2, 1, 1, 3, 2, 3, 1, 3, 0, 1, 1, 3, 1, 1, 0, 3,
        2, 1, 2, 1, 0, 1, 2, 3, 0, 1, 1, 2, 0, 3, 2, 0, 3, 1, 3, 2, 0, 3, 3,
        2, 3, 2, 0, 0, 3, 2, 2, 1, 2, 1, 0, 2, 3, 0, 2, 1, 1, 2, 0, 2, 0, 0,
        0, 1, 2, 2, 2, 3, 0, 0, 3, 3, 3, 0, 1, 0, 0, 2};
    for (i = 0; i < SIGNAL_LENGTH; ++i) {
        input[i] = x[i];
        output[i] = 0;
    }
}

// Print the output array
void print_output(short *output) {
    int i;
    for (i = 0; i < SIGNAL_LENGTH; ++i) {
        printf("%d, ", output[i]);
    }
    printf("\n");
}

// Returns TRUE if actual == expected
int compare_output(short *actual, short *expected, int size) {
    int i;
    for (i = 0; i < size; ++i) {
        if (actual[i] != expected[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

int main(void) {
    short input[SIGNAL_LENGTH];
    short coefs[TAPS_LENGTH];
    short taps[TAPS_LENGTH];

    short output_unoptimized[SIGNAL_LENGTH];
    short output_optimized[SIGNAL_LENGTH];
    short output_macro[SIGNAL_LENGTH];
    short output_asm[SIGNAL_LENGTH];

    short expected[SIGNAL_LENGTH] = {
        18,   63,   37,   46,   62,   114,  108,  93,   123,  95,   150,  141,
        193,  210,  264,  224,  256,  327,  368,  305,  301,  371,  332,  361,
        338,  430,  445,  465,  496,  453,  501,  490,  452,  425,  543,  439,
        469,  487,  564,  610,  592,  581,  560,  580,  545,  524,  533,  609,
        564,  608,  615,  702,  691,  667,  671,  761,  756,  781,  761,  767,
        776,  800,  835,  913,  1015, 850,  807,  930,  922,  899,  894,  856,
        1023, 984,  937,  965,  1018, 1033, 1035, 1067, 1198, 1175, 1095, 1210,
        1264, 1231, 1211, 1187, 1219, 1265, 1289, 1261, 1278, 1377, 1347, 1197,
        1416, 1409, 1360, 1356, 1466, 1432, 1377, 1356, 1328, 1325, 1245, 1424,
        1336, 1463, 1374, 1378, 1290, 1421, 1328, 1218, 1331, 1461, 1276, 1355,
        1341, 1326, 1406, 1315, 1291, 1369, 1512, 1417, 1389, 1384, 1568, 1362,
        1427, 1495, 1548, 1487, 1408, 1549, 1507, 1448, 1410, 1523, 1534, 1554,
        1374, 1442, 1504, 1469, 1341, 1290, 1524, 1490, 1456, 1345, 1361, 1556,
        1512, 1401, 1471, 1505, 1514, 1493, 1402, 1478, 1397, 1352, 1452, 1635,
        1376, 1377, 1450, 1611, 1522, 1447, 1424, 1569, 1591, 1403, 1394, 1440,
        1550, 1429, 1297, 1351, 1471, 1376, 1398, 1318, 1469, 1448, 1395, 1365,
        1366, 1329, 1360, 1373, 1380, 1304, 1179, 1404};

    int i;

    // unoptimized
    /* init(input, coefs, taps, output_unoptimized); */
    /* for (i = 0; i < SIGNAL_LENGTH; ++i) { */
    /*     unoptimized_fir(input, coefs, taps, output_unoptimized, i); */
    /* } */

    // optimized
    /* init(input, coefs, taps, output_optimized); */
    /* for (i = 0; i < SIGNAL_LENGTH; ++i) { */
    /*     optimized_fir(input, coefs, taps, output_optimized, i); */
    /* } */

    // macro
    /* init(input, coefs, taps, output_macro); */
    /* for (i = 0; i < SIGNAL_LENGTH; ++i) { */
    /*     MACRO_FIR(input, coefs, taps, output_macro, i); */
    /* } */

    // optimized asm
    init(input, coefs, taps, output_asm);
    for (i = 0; i < SIGNAL_LENGTH; ++i) {
        optimized_asm_fir(input, coefs, taps, output_asm, i);
        /* optimized_fir(input, coefs, taps, output_asm, i); */
    }
    print_output(output_asm);

    // Print output signal
    /* printf("Actual\n"); */
    /* print_output(output); */

    /* printf("Expected\n"); */
    /* print_output(expected); */

    // Ensure output matches expected output
    /* if (!compare_output(output_unoptimized, expected, SIGNAL_LENGTH)) { */
    /*     printf("UNOPTIMIZED OUTPUT DOES NOT MATCH!\n"); */
    /* } */
    /* if (!compare_output(output_optimized, expected, SIGNAL_LENGTH)) { */
    /*     printf("OPTIMIZED OUTPUT DOES NOT MATCH!\n"); */
    /* } */
    /* if (!compare_output(output_macro, expected, SIGNAL_LENGTH)) { */
    /*     printf("MACRO OUTPUT DOES NOT MATCH!\n"); */
    /* } */

    return 0;
}
