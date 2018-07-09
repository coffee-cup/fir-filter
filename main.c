/* #include "arm_neon.h" */

/* uint32x2_t aa, bb, ss; */

/* int main(void) { ss = vadd_u32(aa, bb); } */

int fir(int *x, int *coefs, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += x[i] * coefs[i];
    }
    return sum;
}

int main(void) {
    int x[5] = {1, 2, 3, 4, 5};
    int coefs[5] = {1, 2, 3, 4, 5};
    fir(x, coefs, 5);

    return 0;
}
