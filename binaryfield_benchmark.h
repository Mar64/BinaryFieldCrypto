#include <binaryfield.h>
#include <time.h>

void start_timer();

uint64_t stop_timer();

void benchmark_add();

void benchmark_mult_shiftadd();

void benchmark_mult_polynomial_rlcomb();

void benchmark_mult_polynomial_lrcomb();

void benchmark_mult_polynomial_lrcomb_window8();

void benchmark_square_polynomial();

void benchmark_reduction_generic();

void benchmark_extended_euclid();

void benchmark_inv_euclid();

void benchmark_inv_binary();

void benchmark_all();