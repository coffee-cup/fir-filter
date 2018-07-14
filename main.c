#include <stdio.h>


void fir(int *x, int *h, int *y, unsigned int num_coefs, const unsigned int N) {
  int X[8] = {0}; // set working array to 0
  int yn = 0;

  int i, k;

  // we will have N outputs
  for (i = 0; i < N; i++) {

    // shift data to right
    for (k = N - 1; k > 0; k--) {
      X[k] = X[k - 1];
    }

    // move input sample to buffer
    X[0] = x[i];

    // clear output sample
    yn = 0;

    for (k = 0; k < num_coefs; k++) {
      yn += h[k] * X[k]; // multiply data on coefficients with accumulation
    }

    // save yn to output position
    y[i] = yn;
  }
}

void print_output(int *y, unsigned int N) {
  int i;
  for (i = 0; i < N; ++i) {
    printf("%d, ", y[i]);
  }
  printf("\n");
}

int main(void) {
  unsigned int N = 5;
  unsigned int num_coefs = 4;

  int x[8] = {2, 4, 6, 4, 2, 0, 0, 0};
  int h[4] = {3, -1, 2, 1};
  int y[8] = {0};

  fir(x, h, y, 4, 8);
  print_output(y, 8);

  return 0;
}
