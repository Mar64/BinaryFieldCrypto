#include <binaryfield_benchmark.h>

typedef struct benchmark_st {
	uint32_t num_tests;
	uint64_t * times;
	char * method_name;
} benchmark_t;

int compare_uint64_t(const void * a, const void * b) {
	uint64_t a1 = *(uint64_t *) a;
	uint64_t b1 = *(uint64_t *) b;
	
	if(a1 < b1) {
		return -1;
	} else if(a1 > b1) {
		return 1;
	} else {
		return 0;
	}
}

void print_stats(benchmark_t result) {
	qsort(result.times, result.num_tests, sizeof(uint64_t), compare_uint64_t);
	
	//Mean
	uint64_t sum = 0;
	for(int i = 0; i < result.num_tests; i++) {
		sum += result.times[i];
	}
	double mean = (double) sum / (double) result.num_tests;
	
	//Median
	double median;
	if(result.num_tests % 2 == 0) {
		median = ((double) (result.times[(result.num_tests / 2) - 1] + result.times[result.num_tests / 2]))/2;
	} else {
		median = result.times[result.num_tests / 2];
	}
	
	//Min & Max
	uint64_t min = result.times[0];
	uint64_t max = result.times[result.num_tests-1];
	
	printf("Benchmark of %s:\n", result.method_name);
	printf("Num tests: %" PRIu32 "\n", result.num_tests);
	printf("Mean: %.2f ns\n", mean);
	printf("Median: %.2f ns\n", median);
	printf("Min: %" PRIu64 " ns Max: %" PRIu64 " ns\n\n", min, max);
}

struct timespec start_time;
struct timespec stop_time;

clockid_t CLOCK_TYPE = CLOCK_REALTIME;

void start_timer() {
	clock_gettime(CLOCK_TYPE, &start_time);
}

uint64_t stop_timer() {
	clock_gettime(CLOCK_TYPE, &stop_time);
	return ((uint64_t ) stop_time.tv_sec - (uint64_t) start_time.tv_sec) * 1000000000 + (stop_time.tv_nsec - start_time.tv_nsec);
}

void benchmark_add() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t b[2];
	uint64_t c[2];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		rand_element(b);
		start_timer();
		add(a, b, c);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "add";
	print_stats(result);
}

void benchmark_mult_shiftadd() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t b[2];
	uint64_t c[2];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		rand_element(b);
		start_timer();
		mult_shiftadd(a, b, c);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "mult_shiftadd";
	print_stats(result);
}

void benchmark_mult_polynomial_rlcomb() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2] = {1, 0};
	uint64_t b[2] = {0,0};
	uint64_t c[4];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		rand_element(b);
		start_timer();
		mult_polynomial_rlcomb(a, b, c);
		times[i] = stop_timer();
	}

	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "mult_polynomial_rlcomb";
	print_stats(result);
}

void benchmark_mult_polynomial_lrcomb() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t b[2];
	uint64_t c[4];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		rand_element(b);
		start_timer();
		mult_polynomial_lrcomb(a, b, c);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "mult_polynomial_lrcomb";
	print_stats(result);
}

void benchmark_mult_polynomial_lrcomb_window8() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t b[2];
	uint64_t c[4];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		rand_element(b);
		start_timer();
		mult_polynomial_lrcomb_window8(a, b, c);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "mult_polynomial_lrcomb_window8";
	print_stats(result);
}

void benchmark_square_polynomial() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t square[4];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		start_timer();
		square_polynomial(a, square);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "square_polynomial";
	print_stats(result);
}

void benchmark_reduction_generic() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[4];
	uint64_t b[4];
	uint64_t ab[4];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		rand_element(b);
		mult_polynomial_lrcomb_window8(a, b, ab);
		start_timer();
		reduction_generic(ab);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "reduction_generic";
	print_stats(result);
}

void benchmark_extended_euclid() {
	int num_tests = 3;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t b[2];
	uint64_t d[2];
	uint64_t g[2];
	uint64_t h[2];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		start_timer();
		extended_euclid(a, b, d, g, h);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "extended_euclid";
	print_stats(result);
}

void benchmark_inv_euclid() {
	int num_tests = 3;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t inva[2];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		start_timer();
		inv_euclid(a, inva);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "inv_euclid";
	print_stats(result);
}

void benchmark_inv_binary() {
	int num_tests = 50;
	uint64_t times[num_tests];
	
	uint64_t a[2];
	uint64_t inva[2];
	for(int i = 0; i < num_tests; i++) {
		rand_element(a);
		start_timer();
		inv_binary(a, inva);
		times[i] = stop_timer();
	}
	
	benchmark_t result;
	result.num_tests = num_tests;
	result.times = times;
	result.method_name = "inv_binary";
	print_stats(result);
}

void benchmark_all() {
	benchmark_add();
	benchmark_mult_shiftadd();
	benchmark_mult_polynomial_rlcomb();
	benchmark_mult_polynomial_lrcomb();
	benchmark_mult_polynomial_lrcomb_window8();
	benchmark_square_polynomial();
	benchmark_reduction_generic();
	benchmark_extended_euclid();
	benchmark_inv_euclid();
	benchmark_inv_binary();
}
