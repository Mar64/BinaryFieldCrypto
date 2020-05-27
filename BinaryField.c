/* For uint64_t */
#include <inttypes.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**  
* BinaryField.c
* Implements a binary field with reduction polynomial f(z) = z^127 + z^63 + 1
*/
#define pow2to63 9223372036854775808U

/* Reduction polynomial f(z) = z^127 + z^63 + 1 */
uint64_t f[2] = {pow2to63+1, pow2to63};
/* f(z) = z^127 + r(z) */
uint64_t r[2] = {pow2to63+1, 0};

void clear_array(uint64_t * a, int size) {
	for(int i = 0; i < size; i++) {
		a[i] = 0;
	}
}

void print_array(uint64_t * a, int size) {
	for(int i = 0; i < size; i++) {
		printf("%" PRIu64, a[i]);
		if(i + 1 < size) {
			printf(",");
		}
	}
	printf("\n");
}

void print_polynomial(uint64_t * a, int size) {
	int first_flag = 1;
	for(int i = size - 1; i >= 0; i--) {
		uint64_t digit = pow2to63; //Start with last
		for(int j = 63; j >= 0; j--) {
			uint64_t comparison = a[i] & digit;
			if(comparison == digit) {
				if(!first_flag) {
					printf("+");
				}
				first_flag = 0;
				
				if(i == 0 && j == 0) {
					printf("1");
				} else {
					int exponent = 64*i +j;
					printf("z^%d", exponent);
				}
			}
			digit /= 2;
		}
	}
	if(first_flag) {
		printf("0");
	}
	printf("\n");
}

/* Precondition: Coefficients are in increasing order, all elements are unique. */
void convert_to_element(int * coefficients, int size_c, uint64_t *a, int size_a) {
	clear_array(a, size_a);
	int i = 0, j = 0;
	int digit = 0;
	uint64_t digit_val = 1;
	while(i < size_c && j < size_a) {
		if(coefficients[i] == digit) {
			a[j] += digit_val;
			i++;
		}
		digit++;
		if(digit % 64 == 0) {
			j++;
			digit_val = 1;
		} else {
			digit_val *= 2;
		}
	}
}

//Should gen random elements of field (set last bit 0)
//Not optimal in any way, is there better way of gen random 64 bits?
void rand_element(uint64_t * a) {
	clear_array(a, 2);
	int i = 0, j = 0;
	while(!(i == 2-1 && j == 63)) {
		a[i] = a[i]*2 + rand() % 2;
		if(j == 63) {
			i++;
		}
		j = (j+1) % 64;
	}
}
/* Left shifts the polynomial, does not reduce */
void lshift_polynomial(uint64_t * a, int size) {
	int carry = 0;
	for(int i = 0; i < size; i++) {
		int carry1 = (a[i] & pow2to63) == pow2to63;
		a[i] <<= 1;
		if(carry) {
			a[i]++;
		}
		carry = carry1;
	}
}

/* 
* Alg 2.32 - Addition. Also extended it for special cases.
* Precondition: Arrays have length 2
*/

void add_ext(uint64_t * a, uint64_t * b, uint64_t * c, int size) {
	for(int  i = 0; i  < size; i++) {
		c[i] = a[i] ^ b[i];
	}
}

void add(uint64_t * a, uint64_t * b, uint64_t * c) {
	add_ext(a, b, c, 2);
}

/* 
* Alg 2.33 Right-to-left shift-and-add field multiplication
* Note that the result is reduced.
* Preconditions: 
* 	Arrays have length 2
* 	a,b are max of order 126
*/
void mult_shiftadd(uint64_t * a, uint64_t * b, uint64_t * c) {
	/* Step 0, preserving original b */
	uint64_t b1[2];
	memcpy(b1, b, sizeof(uint64_t)*2);
	
	/* Step 1 */
	if(a[0] % 2 == 1) {
		memcpy(c, b, sizeof(uint64_t)*2);
	} else {
		clear_array(c, 2);
	}
	
	/* Step 2 */
	int j = 0;
	uint64_t digit_val = 2;
	for(int i = 1; i < 127; i++) {
		/* Step 2.1 */
		lshift_polynomial(b1, 2);
		if((b1[1] & pow2to63) == pow2to63) {
			b1[1] &= pow2to63 - 1; // Mask
			add(r, b1, b1);
		}
		
		/* Step 2.2 */
		if((a[j] & digit_val) == digit_val) {
			add(b1, c, c);
		}
		
		if(i == 63) {
			j++;
			digit_val = 1;
		} else {
			digit_val *= 2;
		}
	}
}

/*
* Alg 2.34 Right-to-left comb method for polynomial multiplication
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
*/
void mult_polynomial_rlcomb(uint64_t * a, uint64_t * b, uint64_t * c) {
	/* Step 0, preserve b */
	uint64_t b1[3]; //Longer to not lose bits when shifting
	memcpy(b1, b, sizeof(uint64_t)*2);
	b1[2] = 0;
	
	/* Step 1 */
	clear_array(c, 4);
	
	/* Step 2 */
	uint64_t digit_val = 1;
	for(int k = 0; k < 64; k++) {
		/* Step 2.1 */
		for(int j = 0; j < 2; j++) {
			if((a[j] & digit_val) == digit_val) {
				add_ext(b1, &c[j], &c[j], 3);
			}
		}
		/* Step 2.2 */
		if(k < 63) {
			lshift_polynomial(b1, 3);
		}
		digit_val *= 2;
	}
}

/*
* Alg 2.35 Left-to-right comb method for polynomial multiplication
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
*/
void mult_polynomial_lrcomb(uint64_t * a, uint64_t * b, uint64_t * c) {
	/* Step 1 */
	clear_array(c, 4);
	
	/* Step 2 */
	uint64_t digit_val = pow2to63;
	for(int k = 63; k >= 0; k--) {
		/* Step 2.1 */
		for(int j = 0; j < 2; j++) {
			if((a[j] & digit_val) == digit_val) {
				add(b, &c[j], &c[j]);
			}
		}
		/* Step 2.2 */
		if(k != 0) {
			lshift_polynomial(c, 4);
		}
		digit_val /= 2;
	}
}

/*
* Alg 2.36 Left-to-right comb method for polynomial multiplication with window
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
* 	w divides 64, for practical reasons < 32.
*/
void mult_polynomial_lrcomb_window(uint64_t * a, uint64_t * b, uint64_t * c, int w) {
	/* Step 1 */
	uint64_t num_polynomials = 1ULL << w;
	printf("num_polynomials: %" PRIu64 " \n", num_polynomials);
	/* Product can't actually fill more than first 3 words, but need to conform to mult */
	uint64_t bu[num_polynomials][4]; 
	for(uint64_t i = 0; i < num_polynomials; i++) {
		clear_array(bu[i], 4);
		bu[i][0] = i;
		/* This is faster method than lr, shift with B instead of C: */
		mult_polynomial_rlcomb(b, bu[i], bu[i]); 
	}
	/* Step 2 */
	clear_array(c, 4);
	
	/* Step 3 */
	uint64_t digit_val = pow2to63;
	for(int k = (64/w)-1; k >= 0; k--) {
		/* Step 3.1 */
		for(int j = 0; j < 2; j++) {
			int u = 0;
			uint64_t digit_val1 = digit_val;
			for(int i = w - 1; i >= 0; i--) {
				u <<= 1;
				if((a[j] & digit_val1) == digit_val1) {
					u++;
				}
				digit_val1 /= 2;
			}
			add_ext(bu[u], &c[j], &c[j], 3);
		}
		digit_val /= num_polynomials;
		
		/* Step 3.2 */
		if(k != 0) { //TO BE OPTIMIZED, make generic n-shift w/ carry handling?
			for(int i = 0; i < w; i++) {
				lshift_polynomial(c, 4);
			}
		}
	}
}

/* Karatsuba 

/*
* Alg 2.39 Polynomial squaring
*/
int has_precomputed = 0;

uint16_t squaring_bytes[256];

void squaring_precompute() {
	for(int i = 0; i < 256; i++) {
		for(int digit_val = 1; digit_val < 256; digit_val *= 2) {
			if((i & digit_val) == digit_val) {
				squaring_bytes[i] += digit_val*digit_val;
			}
		}
	}
	has_precomputed = 1;
}

/*
* Preconditions:
* a has max degree 126, length 2
* c has length 4
*/
void squaring_polynomial(uint64_t * a, uint64_t * c) {
	/* Step 1 */
	if(!has_precomputed) {
		squaring_precompute();
	}
	
	/* Step 2 */
	for(int i = 0; i < 2; i++) {
		/* Step 2.1 */
		uint8_t u[8];
		uint64_t val_a = a[i];
		
		u[0] = val_a & 0xFF;
		for(int j = 1; j < 8; j++) {
			val_a >>= 8;
			u[j] = val_a & 0xFF;
		}
		
		/* Step 2.2 */
		c[2*i] = squaring_bytes[u[0]] | 
			(uint64_t) squaring_bytes[u[1]] << 16 | 
			(uint64_t) squaring_bytes[u[2]] << 32 |
			(uint64_t) squaring_bytes[u[3]] << 48;
		
		c[2*i+1] = squaring_bytes[u[4]] | 
			(uint64_t) squaring_bytes[u[5]] << 16 | 
			(uint64_t) squaring_bytes[u[6]] << 32 |
			(uint64_t) squaring_bytes[u[7]] << 48;
	}
}

/* Recuction */

/*  Alg 2.47 Extended Euclidean algorithm for binary polynomials */

/*
* Preconditions:
*	a has length 2
*/
uint8_t degree_polynomial(uint64_t * a) {
	uint8_t digit = 63;
	uint64_t digit_val = pow2to63;
	int j;
	if(a[1] == 0) {
		j = 0; 
	} else {
		j = 1;
	}
	while(digit < 128 && (a[j] & digit_val) == 0) {
		digit--;
		digit_val /= 2;
	}
	if(digit >= 128) {
		return 0;
	} else {
		return 64*j + digit;
	}
}

/*
* Preconditions:
*	Arrays have length 2
*/
void extended_euclid(uint64_t * a, uint8_t deg_a, uint64_t b, uint8_t deg_b) {
	
}

void main() {
	/* Initialize rand */
	srand(time(NULL));
	
	printf("Reduction polynomial f(z): \n");
	print_polynomial(f, 2);
	
	uint64_t rnd[2];
	rand_element(rnd);
	printf("\nRandom element\n");
	print_polynomial(rnd, 2);
	
	printf("\nFixed polynomial a\n");
	int a_coefficients[5] = {0, 2, 3, 5, 64};
	uint64_t a[2];
	convert_to_element(a_coefficients, 5, a, 2);
	print_polynomial(a, 2);
	
	printf("\nFixed polynomial b\n");
	int b_coefficients[7] = {1, 2, 4, 8, 16, 32, 64};
	uint64_t b[2];
	convert_to_element(b_coefficients, 7, b, 2);
	print_polynomial(b, 2);
	
	printf("\na + b \n");
	uint64_t sum[2];
	add(a, b, sum);
	print_polynomial(sum, 2);
	
	printf("\nShifted sum by 1\n");
	lshift_polynomial(sum, 2);
	print_polynomial(sum, 2);
	
	printf("\nFixed polynomial 0\n");
	uint64_t zero[2];
	convert_to_element(NULL, 0, zero, 2);
	print_polynomial(zero,2);
	
	printf("\nFixed polynomial c\n");
	int c_coefficients[1] = {2};
	uint64_t c[2];
	convert_to_element(c_coefficients, 1, c, 2);
	print_polynomial(c,2);
	
	printf("\nFixed polynomial d\n");
	int d_coefficients[2] = {0, 126};
	uint64_t d[2];
	convert_to_element(d_coefficients, 2, d, 2);
	print_polynomial(d, 2);
	
	printf("\nShift-and-add mult d * r\n");
	uint64_t result_mult_shiftadd[2];
	mult_shiftadd(d,r,result_mult_shiftadd);
	print_polynomial(result_mult_shiftadd, 2);
	
	printf("\nRight-to-left comb mult d * r\n");
	uint64_t result_mult_rlcomb[4];
	mult_polynomial_rlcomb(d,r,result_mult_rlcomb);
	print_array(result_mult_rlcomb, 4);
	print_polynomial(result_mult_rlcomb, 4);
	
	printf("\nLeft-to-right comb mult d * r \n");
	uint64_t result_mult_lrcomb[4];
	mult_polynomial_lrcomb(d,r,result_mult_lrcomb);
	print_array(result_mult_lrcomb, 4);
	print_polynomial(result_mult_lrcomb, 4);
	
	printf("\nLeft-to-right comb mult d*r, w = 16\n");
	uint64_t result_mult_lrcombwindow[4];
	mult_polynomial_lrcomb_window(d,r,result_mult_lrcombwindow, 16);
	print_array(result_mult_lrcombwindow, 4);
	print_polynomial(result_mult_lrcombwindow, 4);
	
	printf("\nSquaring b \n");
	squaring_precompute();
	uint64_t result_squaring[4];
	squaring_polynomial(b, result_squaring);
	print_polynomial(result_squaring, 4);
	
	printf("\nDegree of zero %" PRIu8 "\n", degree_polynomial(zero));
	printf("\nDegree of b %" PRIu8 "\n", degree_polynomial(b));
}
