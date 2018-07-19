#include <stdio.h>

#define SIGNAL_LENGTH (8)
#define TAPS_LENGTH (8)
#define NUM_COEFS (4)

void unoptimized(int *input, int *coefs, int *taps, int *output) {
  int yn = 0;
  int i, k;

  // loop throught the output signal
  for (i = 0; i < SIGNAL_LENGTH; ++i) {

    // shift data to right
    for (k = TAPS_LENGTH - 1; k > 0; k--) {
      taps[k] = taps[k - 1];
    }

    // move input sample into taps
    taps[0] = input[i];

    // clear output sample
    yn = 0;

    // perform the convolution for a single output sample
    for (k = 0; k < NUM_COEFS; ++k) {
      yn += coefs[k] * taps[k];
    }

    // save yn to output position
    output[i] = yn;
  }
}

void print_output(int *output) {
  int i;
  for (i = 0; i < SIGNAL_LENGTH; ++i) {
    printf("%d, ", output[i]);
  }
  printf("\n");
}

void init(int *input, int *coefs, int *taps, int *output) {
    int i;

    // zero the taps
    for (i = 0; i < TAPS_LENGTH; ++i) {
        taps[i] = 0;
    }

    // set values for coefs
    // TODO: make this better
    int h[NUM_COEFS] = {3, -1, 2, 1};
    for (i = 0; i < NUM_COEFS; ++i) {
        coefs[i] = h[i];
    }

    // set values for input signal and zero output
    // TODO: make this better
    int x[SIGNAL_LENGTH] = {2, 4, 6, 4, 2, 0, 0, 0};
    for (i = 0; i < SIGNAL_LENGTH; ++i) {
        input[i] = x[i];
        output[i] = 0;
    }
}

int main(void) {
  int input[SIGNAL_LENGTH];
  int coefs[NUM_COEFS];
  int taps[TAPS_LENGTH];
  int output[SIGNAL_LENGTH];

  // Initialize all arrays
  init(input, coefs, taps, output);

  // Compute output signal
  unoptimized(input, coefs, taps, output);

  // Print output signal
  print_output(output);

  return 0;
}
