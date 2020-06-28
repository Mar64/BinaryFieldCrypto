#include <binaryfield.h>
#include <binaryfield_tests.h>
#include <binaryfield_benchmark.h>

int main() {	
	benchmark_all();
	
	run_tests();
	
	print_test_score();
	
	return 0;
}
