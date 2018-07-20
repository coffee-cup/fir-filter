#include <stdio.h>

#define SIGNAL_LENGTH (8)
#define TAPS_LENGTH (8)
#define NUM_COEFS (4)

#define TRUE (1)
#define FALSE (0)

// Unoptimized C
void unoptimized_fir(short *input, short *coefs, short *taps, short *output, short n) {
  int k;

  // move input sample into taps
  taps[0] = input[n];

  // perform the convolution for a single output sample
  int sum = 0;
  for (k = 0; k < NUM_COEFS; ++k) {
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
inline void optimized_fir(short *input, short * coefs,
                   short *taps, short *output,
                   short n) {

  register int k;
  register int sum = 0;

  // move input sample into taps
  taps[0] = input[n];

  // perform the convolution for a single output sample
  for (k = NUM_COEFS - 1; k >= 0; k--) {
    sum += (int)coefs[k] * (int)taps[k];
  }

  // Place sum in output signal
  output[n] = (short)(sum);

  // shift taps to right
  for (k = TAPS_LENGTH - 1; k > 0; k--) {
    taps[k] = taps[k - 1];
  }
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
  short h[NUM_COEFS] = {3, -1, 2, 1};
  for (i = 0; i < NUM_COEFS; ++i) {
    coefs[i] = h[i];
  }

  // set values for input signal and zero output
  // TODO: make this better
  short x[SIGNAL_LENGTH] = {2, 4, 6, 4, 2, 0, 0, 0};
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
  short coefs[NUM_COEFS];
  short taps[TAPS_LENGTH];
  short output[SIGNAL_LENGTH];

  short expected[SIGNAL_LENGTH] = {6, 10, 18, 16, 18, 12, 8, 2};

  // Initialize all arrays
  init(input, coefs, taps, output);

  /* START FIR */

  // loop through the output signal
  int i;
  for (i = 0; i < SIGNAL_LENGTH; ++i) {
    unoptimized_fir(input, coefs, taps, output, i);
  }

  /* END FIR */

  // Print output signal
  printf("Actual\n");
  print_output(output);

  printf("Expected\n");
  print_output(expected);

  // Ensure output matches expected output
  if (!compare_output(output, expected, SIGNAL_LENGTH)) {
    printf("OUTPUT DOES NOT MATCH!\n");
  }

  return 0;
}
