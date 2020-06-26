#include <inttypes.h>
#include <stdbool.h> 
#include <stdio.h>

#include <binaryfield.h>
#include <binaryfield_tests.h>

/* ============================== Setup for Tests ===========================*/

typedef struct result_st {
	bool success;
	char * fail_msg;
} result_t;

uint64_t num_tests_run = 0;
uint64_t num_tests_passed = 0;

void eval_test(result_t result) {
	num_tests_run++;
	if(result.success) {
		num_tests_passed++;
	} else {
		printf("\n%s\n", result.fail_msg);
	}
}

uint64_t get_num_tests_run() {
	return num_tests_run;
}

uint64_t get_num_tests_passed() {
	return num_tests_passed;
}

void print_score() {
	printf("\n%" PRIu64 " out of %" PRIu64 " tests passed.\n", num_tests_passed, num_tests_run);
}

void reset_score() {
	num_tests_run = num_tests_passed = 0;
}

/* ============================ equal_polynomials ========================== */

result_t equal_polynomials_positive_case() {
	// Arrange
	uint64_t a[2] = { 4, 756 };
	uint64_t b[2] = { 4 , 756 };
	
	//Act & Assert
	bool were_equal = equal_polynomials(a, b, 2);
	
	//Return
	result_t result;
	result.success = were_equal;
	result.fail_msg = "equal_polynomials_positive_case FAILED";
	return result;
}

result_t equal_polynomials_negative_case() {
	//Arrange
	uint64_t a[2] = { 4, 756 };
	uint64_t b[2] = { 4 , 755 };
	
	//Act & Assert
	bool were_different = !equal_polynomials(a, b, 2);
	
	//Return
	result_t result;
	result.success = were_different;
	result.fail_msg = "equal_polynomials_negative_case FAILED";
	return result;
}

void equal_polynomials_correctness_tests() {
	eval_test(equal_polynomials_positive_case());
	eval_test(equal_polynomials_negative_case());
}

/* ======================= index_to_polynomial ======================*/

result_t index_to_polynomial_zero() {
	//Arrange
	uint64_t zero_polynomial[2] = {0, 0};
	uint64_t zero_index[0];
	uint64_t polynomial[2];
	
	//Act
	index_to_polynomial(zero_index, 0, polynomial, 2);
	
	//Assert
	bool correct = equal_polynomials(polynomial, zero_polynomial, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "index_to_polynomial_zero FAILED";
	return result;
}

result_t index_to_polynomial_case() {
	//Arrange
	uint64_t expected[2] = {255, 9};
	uint64_t indices[10] = {0, 1, 2, 3, 4, 5, 6, 7, 64, 67};
	uint64_t  actual[2];
	
	//Act
	index_to_polynomial(indices, 10, actual, 2);
	
	//Assert
	bool correct = equal_polynomials(expected, actual, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "index_to_polynomial_case FAILED";
	return result;
}

void index_to_polynomial_correctness_tests() {
	eval_test(index_to_polynomial_zero());
	eval_test(index_to_polynomial_case());
}

/* add */

result_t add_simple_case() {
	//Arrange
	uint64_t a[2] = { 4, 756 };
	uint64_t b[2] = { 15 , 2 };
	
	uint64_t expected[2] = {11, 758};
	uint64_t actual[2];
	
	//Act
	add(a, b, actual);
	
	//Assert
	bool correct = equal_polynomials(expected, actual, 2);

	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "add_simple_case FAILED";
	return result;
}

result_t add_not_modifying_operands() {
	//Arrange
	uint64_t a0[2] = {2345789652, 7907634};
	uint64_t a1[2] = {2345789652, 7907634};
	uint64_t b0[2] = {984533, 4485001};
	uint64_t b1[2] = {984533, 4485001};
	uint64_t c[2];
	
	//Act
	add(a1, b1, c);
	
	//Assert
	bool samea = equal_polynomials(a0, a1, 2);
	bool sameb = equal_polynomials(b0, b1, 2);
	
	//Return
	result_t result;
	result.success = samea && sameb;
	result.fail_msg = "add_not_modifying_operands FAILED";
	return result;
}

result_t add_associative() {
	//Arrange
	uint64_t a[2] = {11130986, 2277649113};
	uint64_t b[2] = {55669294, 1538974023345};
	uint64_t c[2] = {54, 7548};
	
	uint64_t ab[2];
	uint64_t bc[2];
	uint64_t abc0[2];
	uint64_t abc1[2];
	
	//Act 
	add(a, b, ab);
	add(ab, c, abc0);
	add(b, c, bc);
	add(a, bc, abc1);
	
	//Assert
	bool correct = equal_polynomials(abc0, abc1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "add_associative FAILED";
	return result;
}

result_t add_commutative() {
	//Arrange
	uint64_t a[2] = {645, 89};
	uint64_t b[2] = {123456789, 404};
	
	uint64_t ab[2];
	uint64_t ba[2];
	
	//Act
	add(a, b, ab);
	add(b, a, ba);
	
	//Assert
	bool correct = equal_polynomials(ab, ba, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "add_commutative FAILED";
	return result;
}

result_t add_with_itself_is_zero() {
	//Arrange
	uint64_t a[2] = {907876534, 35451354321325687};
	uint64_t zero[2] = {0, 0};
	uint64_t sum[2];
	
	//Act
	add(a, a, sum);
	
	//Assert
	bool correct = equal_polynomials(sum, zero, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "add_with_itself_is_zero FAILED";
	return result;
}

result_t add_with_zero() {
	//Arrange
	uint64_t a[2] = {3467922, 891355};
	uint64_t zero[2] = {0, 0};
	uint64_t sum[2];
	
	//Act
	add(a, zero, sum);
	
	//Assert
	bool correct = equal_polynomials(sum, a, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "add_with_zero FAILED";
	return result;
}

void add_correctness_tests() {
	eval_test(add_simple_case());
	eval_test(add_not_modifying_operands());
	eval_test(add_associative());
	eval_test(add_commutative());
	eval_test(add_with_itself_is_zero());
	eval_test(add_with_zero());
}

/* ==================== mult_shiftadd ========================== */

result_t mult_shiftadd_reduction_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[2] = {0, 63};
	index_to_polynomial(indicesa, 2, a, 2);
	uint64_t b[2];
	uint64_t indicesb[2] = {1, 126};
	index_to_polynomial(indicesb, 2, b, 2);
	
	uint64_t expected[2];
	uint64_t indicese[5] = {1, 62, 64, 125, 126};
	index_to_polynomial(indicese, 5, expected, 2);
	uint64_t actual[2];
	
	//Act
	mult_shiftadd(a, b, actual);
	
	//Assert
	bool correct = equal_polynomials(expected, actual, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_shiftadd_reduction_case FAILED";
	return result;
}

result_t mult_shiftadd_not_modifying_operands() {
	//Arrange
	uint64_t a0[2] = {7457821, 982346};
	uint64_t a1[2] = {7457821, 982346};
	uint64_t b0[2] = {214763, 21384965476};
	uint64_t b1[2] = {214763, 21384965476};
	uint64_t ab[2];
	
	//Act
	mult_shiftadd(a1, b1, ab);
	
	//Assert
	bool samea = equal_polynomials(a0, a1, 2);
	bool sameb = equal_polynomials(b0, b1, 2);
	
	//Return
	result_t result;
	result.success = samea && sameb;
	result.fail_msg = "mult_shiftadd_not_modifying_operands FAILED";
	return result;
}

result_t mult_shiftadd_associative() {
	//Arrange
	uint64_t a[2] = {348998, 9982583241};
	uint64_t b[2] = {457913580245, 3267254};
	uint64_t c[2] = {4777529578, 109254765824675479};
	uint64_t ab[2];
	uint64_t bc[2];
	uint64_t abc0[2];
	uint64_t abc1[2];
	
	//Act
	mult_shiftadd(a, b, ab);
	mult_shiftadd(ab, c, abc0);
	mult_shiftadd(b, c, bc);
	mult_shiftadd(a, bc, abc1);
	
	//Assert
	bool correct = equal_polynomials(abc0, abc1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_shiftadd_associative FAILED";
	return result;
}

result_t mult_shiftadd_commutative() {
	//Arrange
	uint64_t a[2] = {456212998, 923754669};
	uint64_t b[2] = {21347582114, 99742511909847};
	uint64_t ab[2];
	uint64_t ba[2];
	
	//Act
	mult_shiftadd(a, b, ab);
	mult_shiftadd(b, a, ba);
	
	//Assert
	bool correct = equal_polynomials(ab, ba, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_shiftadd_commutative FAILED";
	return result;
}

result_t mult_shiftadd_nonzero_with_one_is_same() {
	//Arrange
	uint64_t a[2] = {349845618442122, 22837443219};
	uint64_t one[2] = {1, 0};
	uint64_t prod[2];
	
	//Act
	mult_shiftadd(a, one, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, a, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_shiftadd_nonzero_with_one_is_same FAILED";
	return result;
}

result_t mult_shiftadd_zero_with_one_is_zero() {
	//Arrange
	uint64_t zero[2] = {0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[2];
	
	//Act
	mult_shiftadd(zero, one, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, zero, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_shiftadd_zero_with_one_is_zero FAILED";
	return result;
}

result_t mult_shiftadd_nonzero_with_zero_is_zero() {
	//Arrange
	uint64_t a[2] = {46738975118344, 838765834718001284};
	uint64_t zero[2] = {0, 0};
	uint64_t prod[2];
	
	//Act
	mult_shiftadd(a, zero, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, zero, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_shiftadd_nonzero_with_zero_is_zero FAILED";
	return result;
}

void mult_shiftadd_correctness_tests() {
	eval_test(mult_shiftadd_reduction_case());
	eval_test(mult_shiftadd_not_modifying_operands());
	eval_test(mult_shiftadd_associative());
	eval_test(mult_shiftadd_commutative());
	eval_test(mult_shiftadd_nonzero_with_one_is_same());
	eval_test(mult_shiftadd_zero_with_one_is_zero());
	eval_test(mult_shiftadd_nonzero_with_zero_is_zero());
}

/* ======================== reduction_generic ============================== */

result_t reduction_generic_case_just_over() {
	//Arrange
	uint64_t a[4];
	uint64_t indicesa[4] = {1, 64, 126, 189};
	index_to_polynomial(indicesa, 4, a, 4);
	uint64_t expected[2];
	uint64_t indicese[5] = {1, 62, 64, 125, 126};
	index_to_polynomial(indicese, 5, expected, 2);
	
	//Act
	reduction_generic(a);
	
	//Assert
	bool correct = equal_polynomials(a, expected, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "reduction_generic_case_just_over FAILED";
	return result;
}

result_t reduction_generic_case_large_degree() {
	//Arrange
	uint64_t a[4];
	uint64_t indicesa[3] = {0, 198, 252};
	index_to_polynomial(indicesa, 3, a, 4);
	uint64_t expected[2];
	uint64_t indicese[7] = {0, 7, 61, 70, 71, 124, 125};
	index_to_polynomial(indicese, 7, expected, 2);
	
	//Act
	reduction_generic(a);
	
	//Assert
	bool correct = equal_polynomials(a, expected, 2) && a[2] == 0 && a[3] == 0;
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "reduction_generic_case_large_degree FAILED";
	return result;
}

result_t reduction_generic_not_reducing_when_less_than_f() {
	//Arrange
	uint64_t a0[4];
	uint64_t a1[4];
	uint64_t indicesa[3] = {1, 63, 126};
	index_to_polynomial(indicesa, 3, a0, 4);
	index_to_polynomial(indicesa, 3, a1, 4);
	
	//Act
	reduction_generic(a1);
	
	//Assert
	bool correct = equal_polynomials(a0, a1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "reduction_generic_not_reducing_when_less_than_f FAILED";
	return result;
}

void reduction_generic_correctness_tests() {
	eval_test(reduction_generic_case_just_over());
	eval_test(reduction_generic_case_large_degree());
	eval_test(reduction_generic_not_reducing_when_less_than_f());
}

/* ======================= square_polynomial ======================== */

result_t square_polynomial_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[3] = {3, 89, 126};
	index_to_polynomial(indicesa, 3, a, 2);
	uint64_t square[4];
	
	uint64_t expected[4];
	uint64_t indicese[3] = {6, 178, 252};
	index_to_polynomial(indicese, 3, expected, 4);
	
	//Act
	square_polynomial(a, square);
	
	//Assert
	bool correct = equal_polynomials(square, expected, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "square_polynomial_case FAILED";
	return result;
}

result_t square_polynomial_not_modying_operand() {
	//Arrange
	uint64_t a0[2] = {12845, 947587563};
	uint64_t a1[2] = {12845, 947587563};
	uint64_t square[4];
	
	//Act
	square_polynomial(a1, square);
	
	//Assert
	bool correct = equal_polynomials(a0, a1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "square_polynomial_not_modying_operand FAILED";
	return result;
}

result_t square_polynomial_crossreference_shiftadd() {
	//Arrange
	uint64_t a[2] = {9854673521, 1934865191416751322};
	uint64_t asquare[4];
	uint64_t aprod[4];
	
	//Act
	square_polynomial(a, asquare);
	reduction_generic(asquare);
	mult_shiftadd(a, a, aprod);
	
	//Assert
	bool correct = equal_polynomials(asquare, aprod, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "square_polynomial_crossreference_shiftadd FAILED";
	return result;
}

result_t square_polynomial_zero_still_zero() {
	//Arrange
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t square[4];
	
	//Act
	square_polynomial(zero, square);
	
	//Assert
	bool correct = equal_polynomials(zero, square, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "square_polynomial_zero_still_zero FAILED";
	return result;
}

result_t square_polynomial_one_still_one() {
	//Arrange
	uint64_t one[4] = {1, 0, 0, 0};
	uint64_t square[4];
	
	//Act
	square_polynomial(one, square);
	
	//Assert
	bool correct = equal_polynomials(one, square, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "square_polynomial_one_still_one FAILED";
	return result;
}

void square_polynomial_correctness_tests() {
	eval_test(square_polynomial_case());
	eval_test(square_polynomial_not_modying_operand());
	eval_test(square_polynomial_crossreference_shiftadd());
	eval_test(square_polynomial_zero_still_zero());
	eval_test(square_polynomial_one_still_one());
}

/* ============= mult_polynomial_rlcomb ============= */

result_t mult_polynomial_rlcomb_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[3] = {11, 60, 120};
	index_to_polynomial(indicesa, 3, a, 2);
	uint64_t b[2];
	uint64_t indicesb[4] = {0, 1, 12, 48};
	index_to_polynomial(indicesb, 4, b, 2);
	uint64_t ab[4];
	
	uint64_t expected[4];
	uint64_t indicese[12] = {11, 12, 23, 59, 60, 61, 72, 108, 120, 121, 132, 168};
	index_to_polynomial(indicese, 12, expected, 4);
	//Act
	mult_polynomial_rlcomb(a, b, ab);
	
	//Arrange
	bool correct = equal_polynomials(ab, expected, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_case FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_not_modifying_operands() {
	//Arrange
	uint64_t a0[2] = {61234788655, 319876564234};
	uint64_t a1[2] = {61234788655, 319876564234};
	uint64_t b0[2] = {73846273644, 3883114};
	uint64_t b1[2] = {73846273644, 3883114};
	uint64_t ab[4];
	
	//Act
	mult_polynomial_rlcomb(a1, b1, ab);
	
	//Arrange
	bool samea = equal_polynomials(a0, a1, 2);
	bool sameb = equal_polynomials(b0, b1, 2);
	
	//Return
	result_t result;
	result.success = samea && sameb;
	result.fail_msg = "mult_polynomial_rlcomb_not_modifying_operands FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_correct_reduced_product() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[2] = {0, 63};
	index_to_polynomial(indicesa, 2, a, 2);
	uint64_t b[2];
	uint64_t indicesb[2] = {1, 126};
	index_to_polynomial(indicesb, 2, b, 2);
	uint64_t ab[4];
	uint64_t expected[2];
	uint64_t indicese[5] = {1, 62, 64, 125, 126};
	index_to_polynomial(indicese, 5, expected, 2);
	
	//Act
	mult_polynomial_rlcomb(a, b, ab);
	reduction_generic(ab);
	
	//Assert
	bool correct = equal_polynomials(ab, expected, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_correct_reduced_product FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_crossreference_shiftadd() {
	//Arrange
	uint64_t a[2] = {195305335744, 9731685385};
	uint64_t b[2] = {368753267511, 6535242803291};
	uint64_t ab0[4];
	uint64_t ab1[2];
	
	//Act
	mult_polynomial_rlcomb(a, b, ab0);
	reduction_generic(ab0);
	mult_shiftadd(a, b, ab1);
	
	//Assert
	bool correct = equal_polynomials(ab0, ab1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_crossreference_shiftadd FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_crossreference_square() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[4] = {0, 1, 8, 121};
	index_to_polynomial(indicesa, 4, a, 2);
	uint64_t aprod[4];
	uint64_t asquare[4];
	
	//Act
	mult_polynomial_rlcomb(a, a, aprod);
	square_polynomial(a, asquare);
	
	//Assert
	bool correct = equal_polynomials(aprod, asquare, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_crossreference_square FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_associative() {
	//Arrange
	uint64_t a[2] = {982645834113, 385748251};
	uint64_t b[2] = {44556677811, 29004512};
	uint64_t c[2] = {4989536911084, 9083790112734};
	uint64_t ab[4];
	uint64_t bc[4];
	uint64_t abc0[4];
	uint64_t abc1[4];
	
	//Act
	mult_polynomial_rlcomb(a, b, ab);
	reduction_generic(ab);
	mult_polynomial_rlcomb(ab, c, abc0);
	reduction_generic(abc0);
	
	mult_polynomial_rlcomb(b, c, bc);
	reduction_generic(bc);
	mult_polynomial_rlcomb(a, bc, abc1);
	reduction_generic(abc1);
	
	//Assert
	bool correct = equal_polynomials(abc0, abc1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_associative FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_commutative() {
	//Arrange
	uint64_t a[2] = {196510234564, 6541022201920};
	uint64_t b[2] = {39547821113997, 55447890135};
	uint64_t ab[4];
	uint64_t ba[4];
	
	//Act
	mult_polynomial_rlcomb(a, b, ab);
	mult_polynomial_rlcomb(b, a, ba);
	
	//Assert
	bool correct = equal_polynomials(ab, ba, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_commutative FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_nonzero_with_one_is_same() {
	//Arrange
	uint64_t a[4] = {544321763682, 861435418451, 0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_rlcomb(a, one, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, a, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_nonzero_with_one_is_same FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_zero_with_one_is_zero() {
	//Arrange
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_rlcomb(zero, one, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, zero, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_zero_with_one_is_zero FAILED";
	return result;
}

result_t mult_polynomial_rlcomb_nonzero_with_zero_is_zero() {
	//Arrange
	uint64_t a[2] = {821943719461, 139684785652467};
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_rlcomb(a, zero, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, zero, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_rlcomb_nonzero_with_zero_is_zero FAILED";
	return result;
}

void mult_polynomial_rlcomb_correctness_tests() {
	eval_test(mult_polynomial_rlcomb_case());
	eval_test(mult_polynomial_rlcomb_not_modifying_operands());
	eval_test(mult_polynomial_rlcomb_correct_reduced_product());
	eval_test(mult_polynomial_rlcomb_crossreference_shiftadd());
	eval_test(mult_polynomial_rlcomb_crossreference_square());
	eval_test(mult_polynomial_rlcomb_associative());
	eval_test(mult_polynomial_rlcomb_commutative());
	eval_test(mult_polynomial_rlcomb_nonzero_with_one_is_same());
	eval_test(mult_shiftadd_zero_with_one_is_zero());
	eval_test(mult_shiftadd_nonzero_with_zero_is_zero());
}

/* ======================= mult_polynomial_lrcomb ================== ====*/

result_t mult_polynomial_lrcomb_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[5] = {2, 17, 19, 34, 120};
	index_to_polynomial(indicesa, 5, a, 2);
	uint64_t b[2];
	uint64_t indicesb[4] = {1, 11, 39, 43};
	index_to_polynomial(indicesb, 4, b, 2);
	uint64_t ab[4];
	uint64_t expected[4];
	uint64_t indicese[18] = {3, 13, 18, 20, 28, 30, 35, 41, 56, 58, 60, 62, 73, 77, 121, 131, 159, 163};
	index_to_polynomial(indicese, 18, expected, 4);
	
	//Act
	mult_polynomial_lrcomb(a, b, ab);
	
	//Assert
	bool correct = equal_polynomials(expected, ab, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_case FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_not_modifying_operands() {
	//Arrange
	uint64_t a0[2] = {854736813847, 192831};
	uint64_t a1[2] = {854736813847, 192831};
	uint64_t b0[2] = {239845711767547, 1394078067};
	uint64_t b1[2] = {239845711767547, 1394078067};
	uint64_t ab[4];
	
	//Act
	mult_polynomial_lrcomb(a1, b1, ab);
	
	//Arrange
	bool samea = equal_polynomials(a0, a1, 2);
	bool sameb = equal_polynomials(b0, b1, 2);
	
	//Return
	result_t result;
	result.success = samea && sameb;
	result.fail_msg = "mult_polynomial_lrcomb_not_modifying_operands FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_correct_reduced_product() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[3] = {0, 3, 126};
	index_to_polynomial(indicesa, 3, a, 2);
	uint64_t b[2];
	uint64_t indicesb[3] = {40, 48, 56};
	index_to_polynomial(indicesb, 3, b, 2);
	uint64_t ab[4];
	uint64_t expected[2];
	uint64_t indicese[12] = {39, 40, 43, 47, 48, 51, 55, 56, 59, 102, 110, 118};
	index_to_polynomial(indicese, 12, expected, 2);
	
	//Act
	mult_polynomial_lrcomb(a, b, ab);
	reduction_generic(ab);
	
	//Assert
	bool correct = equal_polynomials(ab, expected, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_correct_reduced_product FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_crossreference_shiftadd() {
	//Arrange
	uint64_t a[2] = {1935724302108, 485126568431};
	uint64_t b[2] = {7234567891076432, 983221};
	uint64_t ab0[4];
	uint64_t ab1[2];
	
	//Act
	mult_polynomial_lrcomb(a, b, ab0);
	reduction_generic(ab0);
	mult_shiftadd(a, b, ab1);
	
	//Assert
	bool correct = equal_polynomials(ab0, ab1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_crossreference_shiftadd FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_crossreference_rlcomb() {
	//Arrange
	uint64_t a[2] = {6234567985233, 18638245347};
	uint64_t b[2] = {9087872, 2305473123560};
	uint64_t ab0[4];
	uint64_t ab1[4];
	
	//Act
	mult_polynomial_lrcomb(a, b, ab0);
	mult_polynomial_rlcomb(a, b, ab1);
	
	//Assert
	bool correct = equal_polynomials(ab0, ab1, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_crossreference_rlcomb FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_crossreference_square() {
	//Arrange
	uint64_t a[2] = {15996746004, 7855486213975};
	uint64_t aprod[4];
	uint64_t asquare[4];
	
	//Act
	mult_polynomial_lrcomb(a, a, aprod);
	square_polynomial(a, asquare);
	
	//Assert
	bool correct = equal_polynomials(aprod, asquare, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_crossreference_square FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_associative() {
	//Arrange
	uint64_t a[2] = {4982476259832, 782568945165680};
	uint64_t b[2] = {87113654853248, 78845310568541};
	uint64_t c[2] = {1678764890924356, 296794359344};
	uint64_t ab[4];
	uint64_t bc[4];
	uint64_t abc0[4];
	uint64_t abc1[4];
	
	//Act
	mult_polynomial_lrcomb(a, b, ab);
	reduction_generic(ab);
	mult_polynomial_lrcomb(ab, c, abc0);
	reduction_generic(abc0);
	mult_polynomial_lrcomb(b, c, bc);
	reduction_generic(bc);
	mult_polynomial_lrcomb(a, bc, abc1);
	reduction_generic(abc1);
	
	//Assert
	bool correct = equal_polynomials(abc0, abc1, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_associative FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_commutative() {
	//Arrange
	uint64_t a[2] = {67821035862049, 851586217803};
	uint64_t b[2] = {8527932468052827, 784512058952025852};
	uint64_t ab[4];
	uint64_t ba[4];
	
	//Act
	mult_polynomial_lrcomb(a, b, ab);
	mult_polynomial_lrcomb(b, a, ba);
	
	//Assert
	bool correct = equal_polynomials(ab, ba, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_commutative FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_nonzero_with_one_is_same() {
	//Arrange
	uint64_t a[4] = {331245254110210982, 127931, 0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_lrcomb(a, one, prod);
	
	//Assert
	bool correct = equal_polynomials(a, prod, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_nonzero_with_one_is_same FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_zero_with_one_is_zero() {
	//Arrange
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_lrcomb(zero, one, prod);
	
	//Assert
	bool correct = equal_polynomials(zero, prod, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_zero_with_one_is_zero FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_nonzero_with_zero_is_zero() {
	//Arrange
	uint64_t a[2] = {962478133025520, 1002369712026};
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_lrcomb(a, zero, prod);
	
	//Assert
	bool correct = equal_polynomials(zero, prod, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_nonzero_with_zero_is_zero FAILED";
	return result;
}

void mult_polynomial_lrcomb_correctness_tests() {
	eval_test(mult_polynomial_lrcomb_case());
	eval_test(mult_polynomial_lrcomb_not_modifying_operands());
	eval_test(mult_polynomial_lrcomb_correct_reduced_product());
	eval_test(mult_polynomial_lrcomb_crossreference_shiftadd());
	eval_test(mult_polynomial_lrcomb_crossreference_square());
	eval_test(mult_polynomial_lrcomb_crossreference_rlcomb());
	eval_test(mult_polynomial_lrcomb_associative());
	eval_test(mult_polynomial_lrcomb_commutative());
	eval_test(mult_polynomial_lrcomb_nonzero_with_one_is_same());
	eval_test(mult_polynomial_lrcomb_zero_with_one_is_zero());
	eval_test(mult_polynomial_lrcomb_nonzero_with_zero_is_zero());
}

/* ======================= mult_polynomial_lrcomb_window8 =============== */


result_t mult_polynomial_lrcomb_window8_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[4] = {0, 13, 20, 126};
	index_to_polynomial(indicesa, 4, a, 2);
	uint64_t b[2];
	uint64_t indicesb[4] = {1, 20, 40, 50};
	index_to_polynomial(indicesb, 4, b, 2);
	uint64_t expected_ab[4];
	uint64_t indicesab[14] = {1, 14, 20, 21, 33, 50, 53, 60, 63, 70, 127, 146, 166, 176};
	index_to_polynomial(indicesab, 14, expected_ab, 4);
	uint64_t ab[4];
	//1 + z^20 + z^40 + z^50 +  z^14 + z^33 + z^53 + z^63  + z^21 + z^40 + z^60 + z^70 + z^127 + z^146 + z^166 + z^176
	// 1, 14, 20, 21, 33, 50, 53, 60, 63, 70, 127, 146, 166, 176
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab);
	
	//Assert
	bool correct = equal_polynomials(ab, expected_ab, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_case FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_not_modifying_operands() {
	//Arrange
	uint64_t a0[2] = {12529730, 896271426};
	uint64_t a1[2] = {12529730, 896271426};
	uint64_t b0[2] = {93478201365, 87966666};
	uint64_t b1[2] = {93478201365, 87966666};
	uint64_t ab[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a1, b1, ab);
	
	//Assert
	bool samea = equal_polynomials(a0, a1, 2);
	bool sameb = equal_polynomials(b0, b1, 2);
	
	//Return
	result_t result;
	result.success = samea && sameb;
	result.fail_msg = "mult_polynomial_lrcomb_window8_not_modifying_operands FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_correct_reduced_product() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[2] = {64, 119};
	index_to_polynomial(indicesa, 2, a, 2);
	uint64_t b[2];
	uint64_t indicesb[3] = {7, 11, 13};
	index_to_polynomial(indicesb, 3, b, 2);
	uint64_t expected_ab[4];
	uint64_t indicesab[8] = {3, 5, 66, 68, 71, 75, 77, 126};
	index_to_polynomial(indicesab, 8, expected_ab, 4);
	uint64_t ab[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab);
	reduction_generic(ab);
	
	//Assert
	bool correct = equal_polynomials(ab, expected_ab, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_correct_reduced_product FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_crossreference_shiftadd() {
	//Arrange
	uint64_t a[2] = {259398971125881, 98752520481};
	uint64_t b[2] = {973025584, 89930471};
	uint64_t ab0[4];
	uint64_t ab1[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab0);
	reduction_generic(ab0);
	mult_shiftadd(a, b, ab1);
	
	//Assert
	bool correct = equal_polynomials(ab0, ab1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_crossreference_shiftadd FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_crossreference_square() {
	//Arrange
	uint64_t a[2] = {4682043888526, 7512369852};
	uint64_t aprod[4];
	uint64_t asquare[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, a, aprod);
	square_polynomial(a, asquare);
	
	//Assert
	bool correct = equal_polynomials(aprod, asquare, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_crossreference_square FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_crossreference_rlcomb() {
	//Arrange
	uint64_t a[2] = {60048762562, 1698205};
	uint64_t b[2] = {9976523204, 3078924893};
	uint64_t ab0[4];
	uint64_t ab1[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab0);
	mult_polynomial_rlcomb(a, b, ab1);
	
	//Assert
	bool correct = equal_polynomials(ab0, ab1, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_crossreference_rlcomb FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_crossreference_lrcomb() {
	//Arrange
	uint64_t a[2] = {3489732302458, 716982012693};
	uint64_t b[2] = {258549317521, 27759624};
	uint64_t ab0[4];
	uint64_t ab1[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab0);
	mult_polynomial_lrcomb(a, b, ab1);
	
	//Assert
	bool correct = equal_polynomials(ab0, ab1, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_crossreference_lrcomb FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_associative() {
	//Arrange
	uint64_t a[2] = {2578901634820, 9687512123};
	uint64_t b[2] = {256623241,78229720};
	uint64_t c[2] = {9387501468, 1479983045501};
	uint64_t ab[4];
	uint64_t bc[4];
	uint64_t abc0[4];
	uint64_t abc1[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab);
	reduction_generic(ab);
	mult_polynomial_lrcomb_window8(ab, c, abc0);
	reduction_generic(abc0);
	mult_polynomial_lrcomb_window8(b, c, bc);
	reduction_generic(bc);
	mult_polynomial_lrcomb_window8(a, bc, abc1);
	reduction_generic(abc1);
	
	//Assert
	bool correct = equal_polynomials(abc0, abc1, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_associative FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_commutative() {
	//Arrange
	uint64_t a[2] = {501789354, 918834462};
	uint64_t b[2] = {300788114, 77824889};
	uint64_t ab[4];
	uint64_t ba[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, b, ab);
	mult_polynomial_lrcomb_window8(b, a, ba);
	
	//Assert
	bool correct = equal_polynomials(ab, ba, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_commutative FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_nonzero_with_one_is_same() {
	//Arrange
	uint64_t a[4] = {501789354, 918834462, 0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, one, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, a, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_nonzero_with_one_is_same FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_zero_with_one_is_zero() {
	//Arrange
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t one[2] = {1, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_lrcomb_window8(zero, one, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, zero, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_zero_with_one_is_zero FAILED";
	return result;
}

result_t mult_polynomial_lrcomb_window8_nonzero_with_zero_is_zero() {
	//Arrange
	uint64_t a[2] = {9328775055, 113355781};
	uint64_t zero[4] = {0, 0, 0, 0};
	uint64_t prod[4];
	
	//Act
	mult_polynomial_lrcomb_window8(a, zero, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, zero, 4);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "mult_polynomial_lrcomb_window8_nonzero_with_zero_is_zero FAILED";
	return result;
}

void mult_polynomial_lrcomb_window8_correctness_tests() {
	eval_test(mult_polynomial_lrcomb_window8_case());
	eval_test(mult_polynomial_lrcomb_not_modifying_operands());
	eval_test(mult_polynomial_lrcomb_correct_reduced_product());
	eval_test(mult_polynomial_lrcomb_window8_crossreference_shiftadd());
	eval_test(mult_polynomial_lrcomb_window8_crossreference_square());
	eval_test(mult_polynomial_lrcomb_window8_crossreference_rlcomb());
	eval_test(mult_polynomial_lrcomb_window8_crossreference_lrcomb());
	eval_test(mult_polynomial_lrcomb_window8_associative());
	eval_test(mult_polynomial_lrcomb_window8_commutative());
	eval_test(mult_polynomial_lrcomb_window8_nonzero_with_one_is_same());
	eval_test(mult_polynomial_lrcomb_window8_zero_with_one_is_zero());
	eval_test(mult_polynomial_lrcomb_window8_nonzero_with_zero_is_zero());
}

/* =======================extended_euclid ============================== */

result_t extended_euclid_coprime_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[3] = {0, 3, 40};
	index_to_polynomial(indicesa, 3, a, 2);
	uint64_t b[2];
	uint64_t indicesb[2] = {2, 83};
	index_to_polynomial(indicesb, 2, b, 2);
	uint64_t expected_gcd[2] = {1, 0};
	uint64_t expected_g[2];
	uint64_t indicesg[43] = {0, 2, 3, 5, 6, 7, 9, 12, 14, 15, 19, 20, 22, 23, 24, 25, 27, 28, 30, 32, 33, 35, 36, 39, 41, 43, 44, 45, 48, 49, 51, 55, 58, 59, 60, 61, 65, 67, 72, 73, 79, 81, 82};
	index_to_polynomial(indicesg, 46, expected_g, 2);
	uint64_t expected_h[2];
	uint64_t indicesh[16] = {0, 5, 6, 8, 12, 15, 16, 17, 18, 22, 24, 29, 30, 36, 38, 39};
	index_to_polynomial(indicesh, 17, expected_h, 2);
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	
	//Act
	extended_euclid(a, b, gcd, g, h);
	
	//Assert
	bool correct_gcd = equal_polynomials(gcd, expected_gcd, 2);
	bool correct_g = equal_polynomials(g, expected_g, 2);
	bool correct_h = equal_polynomials(h, expected_h, 2);
	
	//Return
	result_t result;
	result.success = correct_gcd && correct_g && correct_h;
	result.fail_msg = "extended_euclid_coprime_case FAILED";
	return result;
}

result_t extended_euclid_common_factor_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[4] = {1, 2, 4, 5};
	index_to_polynomial(indicesa, 4, a, 2);
	uint64_t b[2];
	uint64_t indicesb[4] = {3, 4, 5, 6};
	index_to_polynomial(indicesb, 4, b, 2);
	uint64_t expected_gcd[2];
	uint64_t indicesgcd[2] = {1, 3};
	index_to_polynomial(indicesgcd, 2, expected_gcd, 2);
	uint64_t expected_g[2];
	uint64_t indicesg[3] = {0, 1, 2};
	index_to_polynomial(indicesg, 3, expected_g, 2);
	uint64_t expected_h[2];
	uint64_t indicesh[2] = {0, 1};
	index_to_polynomial(indicesh, 2, expected_h, 2);
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	
	//Act
	extended_euclid(a, b, gcd, g, h);
	
	//Assert
	bool correct_gcd = equal_polynomials(gcd, expected_gcd, 2);
	bool correct_g = equal_polynomials(g, expected_g, 2);
	bool correct_h = equal_polynomials(h, expected_h, 2);
	
	//Return
	result_t result;
	result.success = correct_gcd && correct_g && correct_h;
	result.fail_msg = "extended_euclid_common_factor_case FAILED";
	return result;
}

result_t extended_euclid_cross_reference_g_and_h() {
	//Arrange
	uint64_t a[2] = {528723069, 9831505791};
	uint64_t b[2] = {120687981035, 8412205886};
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	uint64_t ag[2];
	uint64_t bh[2];
	uint64_t sum[2];
	
	//Act
	extended_euclid(a, b, gcd, g, h);
	mult_shiftadd(a, g, ag);
	mult_shiftadd(b, h, bh);
	add(ag, bh, sum);
	
	//Assert
	bool correct = equal_polynomials(sum, gcd, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "extended_euclid_cross_reference_g_and_h FAILED";
	return result;
}

result_t extended_euclid_not_modifying_operands() {
	//Arrange
	uint64_t a0[2] = {252242103978283, 7892013649722};
	uint64_t a1[2] = {252242103978283, 7892013649722};
	uint64_t b0[2] = {710369852471, 3028945612774};
	uint64_t b1[2] = {710369852471, 3028945612774};
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	
	//Act
	extended_euclid(a1, b1, gcd, g, h);
	
	//Assert
	bool samea = equal_polynomials(a0, a1, 2);
	bool sameb = equal_polynomials(b0, b1, 2);
	
	//Return
	result_t result;
	result.success = samea && sameb;
	result.fail_msg = "extended_euclid_not_modifying_operands FAILED";
	return result;
}

result_t extended_euclid_one_with_nonzero_has_gcd_one() {
	//Arrange
	uint64_t one[2] = {1, 0};
	uint64_t b[2] = {7645869580749671, 758135679};
	uint64_t expected_gcd[2] = {1, 0};
	uint64_t expected_g[2] = {1, 0};
	uint64_t expected_h[2] = {0, 0};
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	
	//Act
	extended_euclid(one, b, gcd, g, h);
	
	//Assert
	bool correct_gcd = equal_polynomials(gcd, expected_gcd, 2);
	bool correct_g = equal_polynomials(g, expected_g, 2);
	bool correct_h = equal_polynomials(h, expected_h, 2);
	
	//Return
	result_t result;
	result.success = correct_gcd && correct_g && correct_h;
	result.fail_msg = "extended_euclid_one_with_nonzero_has_gcd_one FAILED";
	return result;
}

void extended_euclid_correctness_tests() {
	eval_test(extended_euclid_coprime_case());
	eval_test(extended_euclid_common_factor_case());
	eval_test(extended_euclid_cross_reference_g_and_h());
	eval_test(extended_euclid_not_modifying_operands());
	eval_test(extended_euclid_one_with_nonzero_has_gcd_one());
}

/* ================================ inv_euclid ================================ */

result_t inv_euclid_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[3] = {126};
	index_to_polynomial(indicesa, 3, a, 2);
	uint64_t expected_inva[2];
	uint64_t indicesinva[3] = {0, 1, 64};
	index_to_polynomial(indicesinva, 3, expected_inva, 2);
	uint64_t inva[2];
	
	//Act
	inv_euclid(a, inva);
	
	//Assert
	bool correct = equal_polynomials(inva, expected_inva, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_euclid_case FAILED";
	return result;
}

result_t inv_euclid_not_modifying_operand() {
	//Arrange
	uint64_t a0[2] = {18820369423762, 77812309820}; 
	uint64_t a1[2] = {18820369423762, 77812309820}; 
	uint64_t inva[2];
	
	//Act
	inv_euclid(a1, inva);
	
	//Assert
	bool correct = equal_polynomials(a0, a1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_euclid_not_modifying_operands FAILED";
	return result;
}

result_t inv_euclid_crossreference_extended_euclid() {
	//Arrange
	uint64_t a[2] = {68791025222456, 910257638};
	uint64_t f[2];
	uint64_t indicesf[3] = {0, 63, 127};
	index_to_polynomial(indicesf, 3, f, 2);
	uint64_t inva[2];
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	
	//Act
	inv_euclid(a, inva);
	extended_euclid(a, f, gcd, g, h);
	
	//Assert
	bool correct = equal_polynomials(inva, g, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_euclid_crossreference_extended_euclid FAILED";
	return result;
}

result_t inv_euclid_product_of_inverses_is_inverse_of_product() {
	//Arrange
	uint64_t a[2] = {25846057920451, 2468420775};
	uint64_t b[2] = {31148700972, 7201781698};
	uint64_t ab[2];
	uint64_t inva[2];
	uint64_t invb[2];
	uint64_t inva_times_invb[2];
	uint64_t prod[2];
	uint64_t one[2] = {1, 0};
	
	//Act
	mult_shiftadd(a, b, ab);
	inv_euclid(a, inva);
	inv_euclid(b, invb);
	mult_shiftadd(inva, invb, inva_times_invb);
	mult_shiftadd(ab, inva_times_invb, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, one, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_euclid_product_of_inverses_is_inverse_of_product FAILED";
	return result;
}

void inv_euclid_correctness_tests() {
	eval_test(inv_euclid_case());
	eval_test(inv_euclid_not_modifying_operand());
	eval_test(inv_euclid_crossreference_extended_euclid());
	eval_test(inv_euclid_product_of_inverses_is_inverse_of_product());
}

/* =================== inv_binary ========================= */

result_t inv_binary_case() {
	//Arrange
	uint64_t a[2];
	uint64_t indicesa[2] = {1, 64};
	index_to_polynomial(indicesa, 2, a, 2);
	uint64_t expected_inva[2];
	uint64_t indicesinva[3] = {61, 125, 126};
	index_to_polynomial(indicesinva, 3, expected_inva, 2);
	uint64_t inva[2];
	
	//Act
	inv_binary(a, inva);
	
	//Assert
	bool correct = equal_polynomials(inva, expected_inva, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_binary_case FAILED";
	return result;
}

result_t inv_binary_not_modifying_operand() {
	//Arrange
	uint64_t a0[2] = {711032552186, 801382846842977};
	uint64_t a1[2] = {711032552186, 801382846842977};
	uint64_t inva[2];
	
	//Act
	inv_binary(a1, inva);
	
	//Assert
	bool correct = equal_polynomials(a0, a1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_binary_not_modifying_operand FAILED";
	return result;
}

result_t inv_binary_crossreference_extended_euclid() {
	//Arrange
	uint64_t a[2] = {5489300711, 9782012354};
	uint64_t f[2];
	uint64_t indicesf[3] = {0, 63, 127};
	index_to_polynomial(indicesf, 3, f, 2);
	uint64_t inva[2];
	uint64_t gcd[2];
	uint64_t g[2];
	uint64_t h[2];
	
	//Act
	inv_binary(a, inva);
	extended_euclid(a, f, gcd, g, h);
	
	//Assert
	bool correct = equal_polynomials(inva, g, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_binary_crossreference_extended_euclid FAILED";
	return result;
}

result_t inv_binary_crossreference_inv_euclid() {
	//Arrange
	uint64_t a[2] = {469820358228, 972302255};
	uint64_t inva0[2];
	uint64_t inva1[2];
	
	//Act
	inv_binary(a, inva0);
	inv_euclid(a, inva1);
	
	//Assert
	bool correct = equal_polynomials(inva0, inva1, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_binary_crossreference_inv_euclid FAILED";
	return result;
}

result_t inv_binary_product_of_inverses_is_inverse_of_product() {
	//Arrange
	uint64_t a[2] = {3790115856821, 403885642179};
	uint64_t b[2] = {617459943, 942720364782};
	uint64_t ab[2];
	uint64_t inva[2];
	uint64_t invb[2];
	uint64_t inva_times_invb[2];
	uint64_t prod[2];
	uint64_t one[2] = {1, 0};
	
	//Act
	mult_shiftadd(a, b, ab);
	inv_binary(a, inva);
	inv_binary(b, invb);
	mult_shiftadd(inva, invb, inva_times_invb);
	mult_shiftadd(ab, inva_times_invb, prod);
	
	//Assert
	bool correct = equal_polynomials(prod, one, 2);
	
	//Return
	result_t result;
	result.success = correct;
	result.fail_msg = "inv_binary_product_of_inverses_is_inverse_of_product FAILED";
	return result;
}

void inv_binary_correctness_tests() {
	eval_test(inv_binary_case());
	eval_test(inv_binary_not_modifying_operand());
	eval_test(inv_binary_crossreference_extended_euclid());
	eval_test(inv_binary_crossreference_inv_euclid());
	eval_test(inv_binary_product_of_inverses_is_inverse_of_product());
}

void run_tests() {
	equal_polynomials_correctness_tests();
	index_to_polynomial_correctness_tests();
	add_correctness_tests();
	mult_shiftadd_correctness_tests();
	reduction_generic_correctness_tests();
	square_polynomial_correctness_tests();
	mult_polynomial_rlcomb_correctness_tests();
	mult_polynomial_lrcomb_correctness_tests();
	mult_polynomial_lrcomb_window8_correctness_tests();
	extended_euclid_correctness_tests();
	inv_euclid_correctness_tests();
	inv_binary_correctness_tests();
}

int main() {
	
	run_tests();
	
	print_score();
	
	return 0;
}
