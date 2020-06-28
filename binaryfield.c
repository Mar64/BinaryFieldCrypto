#include <binaryfield.h>
#include <binaryfield_benchmark.h>
/**  
* BinaryField.c
* Implements a binary field with reduction polynomial f(z) = z^127 + z^63 + 1
*/
#define pow2to63 9223372036854775808U
#define pow2to56 72057594037927936U

/* Reduction polynomial f(z) = z^127 + z^63 + 1 */
uint64_t f[2] = {pow2to63+1, pow2to63};
/* f(z) = z^127 + r(z) */
uint64_t r[2] = {pow2to63+1, 0};

void clear_array(uint64_t * a, int len) {
	for(int i = 0; i < len; i++) {
		a[i] = 0;
	}
}

void print_array(uint64_t * a, uint32_t len) {
	for(int i = 0; i < len; i++) {
		printf("%" PRIu64, a[i]);
		if(i + 1 < len) {
			printf(",");
		}
	}
	printf("\n");
}

void print_polynomial(uint64_t * a, uint32_t len) {
	bool first_flag = 1;
	for(int i = len - 1; i >= 0; i--) {
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
					uint64_t exponent = 64*i +j;
					printf("z^%" PRIu64 "", exponent);
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

bool equal_polynomials(uint64_t * a, uint64_t * b, uint32_t len) {
	for(int i = 0; i < len; i++) {
		if(a[i] != b[i]) {
			return 0;
		}
	}
	return 1;
}

 /* Precondition: Indices are in increasing order, and are unique. */
 void index_to_polynomial(uint64_t * indices, int len_i, uint64_t *a, uint32_t len_a) {
	clear_array(a, len_a);
	int i = 0, j = 0;
	int digit = 0;
	uint64_t digit_val = 1;
	while(i < len_i && j < len_a) {
		if(indices[i] == digit) {
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

 /*
  * Generates a random member of the binary field.
  * Precondition:
  * 	a has length 2
  */
void rand_element(uint64_t * a) {
	clear_array(a, 2);
	int i = 0, j = 0;
	while(!(i == 1 && j == 63)) {
		a[i] = a[i]*2 + rand() % 2;
		if(j == 63) {
			i++;
		}
		j = (j+1) % 64;
	}
}

/* Left shifts the polynomial, does not reduce */
void lshift_polynomial(uint64_t * a, int len) {
	int carry = 0;
	for(int i = 0; i < len; i++) {
		int carry1 = (a[i] & pow2to63) == pow2to63;
		a[i] <<= 1;
		if(carry) {
			a[i]++;
		}
		carry = carry1;
	}
}

/* Right shifts the polynomial, does not reduce */
void rshift_polynomial(uint64_t * a, int len) {
	int carry = 0;
	for(int i = len - 1; i >= 0; i--) {
		int carry1 = a[i] % 2;
		a[i] /= 2;
		if(carry) {
			a[i] += pow2to63;
		}
		carry = carry1;
	}
}

void add_ext(uint64_t * a, uint64_t * b, uint64_t * c, int len) {
	for(int  i = 0; i  < len; i++) {
		c[i] = a[i] ^ b[i];
	}
}

/* 
* Alg 2.32 - Addition. Also extended it for special cases.
* Precondition: Arrays have length 2
*/
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
	//memcpy(b1, b, sizeof(uint64_t)*2);
	b1[0] = b[0];
	b1[1] = b[1];
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

/*
* Alg 2.36 Left-to-right comb method for polynomial multiplication with window size 8
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
*/
void mult_polynomial_lrcomb_window8(uint64_t * a, uint64_t * b, uint64_t * c) {
	/* Step 1 */
	uint64_t num_polynomials = 256;
	
	/* Product can't actually fill more than first 3 words, but need to conform to mult */
	uint64_t bu[num_polynomials][4]; 
	for(uint64_t i = 0; i < num_polynomials; i++) {
		clear_array(bu[i], 4);
		uint64_t u[2] = {i, 0};
		/* This is faster method than lr, shift with B instead of C: */
		mult_polynomial_rlcomb(u, b, bu[i]); 
	}
	/* Step 2 */
	clear_array(c, 4);
	
	/* Step 3 */
	uint64_t digit_val = pow2to63;
	for(int k = 7; k >= 0; k--) {
		/* Step 3.1 */
		for(int j = 0; j < 2; j++) {
			int u = 0;
			uint64_t digit_val1 = digit_val;
			for(int i = 7; i >= 0; i--) {
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
		if(k != 0) {
			uint64_t old_carry = 0;
			uint64_t carry = 0;
			for(int i = 0; i < 4; i++) {
				carry = c[i] & 0xFF00000000000000;
				carry /= pow2to56;
				c[i] <<= 8;
				c[i] |= old_carry;
				old_carry = carry;
			}
		}
	}
}

/* Karatsuba multiplication NOT DONE YET, also should think more about iterative approach*/
int has_karatsuba_precomputed = 0;

uint64_t karatsuba_polynomials[256];

void karatsuba_precompute() {
	uint64_t a[2] = {0,0};
	uint64_t b[2] = {0,0};
	uint64_t c[2] = {0,0};
	
	//only need half these calcs, commutative mult
	for(a[0] = 0; a[0] < 16; a[0]++) {
		for(b[0] = 0; b[0] < 16; b[0]++) {
			c[0]=0;
			mult_polynomial_rlcomb(a, b, c);
			karatsuba_polynomials[a[0]*16+b[0]] = c[0];
		}
	}
	has_karatsuba_precomputed = 1;
}

void karatsuba_inner(uint64_t * a, uint64_t * b, uint64_t * c, int bitlen) {
	if(bitlen == 4) {
		c[0] = karatsuba_polynomials[a[0]*16 + b[0]];
		c[1] = 0;
		return;
	}
	
}

void mult_karatsuba_recursive(uint64_t * a, uint64_t * b, uint64_t * c) {
	if(!has_karatsuba_precomputed) {
		karatsuba_precompute();
	}
	karatsuba_inner(a, b, c, 128);
}

/*
* Alg 2.39 Polynomial squaring
*/
int has_square_precomputed = 0;

uint16_t squaring_bytes[256];

void squaring_precompute() {
	for(int i = 0; i < 256; i++) {
		for(int digit_val = 1; digit_val < 256; digit_val *= 2) {
			if((i & digit_val) == digit_val) {
				squaring_bytes[i] += digit_val*digit_val;
			}
		}
	}
	has_square_precomputed = 1;
}

/*
* Preconditions:
* a has max degree 126, length 2
* c has length 4
*/
void square_polynomial(uint64_t * a, uint64_t * c) {
	/* Step 1 */
	if(!has_square_precomputed) {
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

/* Alg 2.40 - Modular reduction (one bit at a time) */

int has_reduction_precomputed = 0;

uint64_t reduction_polynomials[64][2];

void reduction_generic_precompute() {
	reduction_polynomials[0][0] = r[0];
	reduction_polynomials[0][1] = r[1];
	for(int i = 1; i < 64; i++) {
		reduction_polynomials[i][0] = reduction_polynomials[i-1][0];
		reduction_polynomials[i][1] = reduction_polynomials[i-1][1];
		lshift_polynomial(reduction_polynomials[i], 2);
	}
	
	has_reduction_precomputed = 1;
}

/*
 * Reduces c mod f, 
 * after which only the first 2 indices are guaranteed to be correct.
 * Precondition:
 * 	c has at most degree 2m-2
 *		c has length 4
 */
void reduction_generic(uint64_t * c) {
	/* Step 1 */
	if(!has_reduction_precomputed) {
		reduction_generic_precompute();
	}
	/* Step 2 */
	uint64_t digit_val = pow2to63 / 8;
	int index = 3;
	for(int digit = 252; digit >= 127; digit--) {
		/* Step 2.1 */
		if((c[index] & digit_val) == digit_val) {
			int j = (digit - 127)/64;
			int k = (digit - 127) - 64*j;
			add(reduction_polynomials[k], &c[j], &c[j]);
		}
		
		if(digit % 64 == 0) {
			digit_val = pow2to63;
			index--;
		} else {
			digit_val /= 2;
		}
	}
	//Algorithm in book is slightly wrong for some f, here is a general fix:
	c[1] &= 0x7FFFFFFFFFFFFFFF;
	c[2] = c[3] = 0;
}

/*  Alg 2.47 Extended Euclidean algorithm for binary polynomials */

/*
* Preconditions:
*	a has length 2
*/
int degree_oldreference(uint64_t * a) {
	int digit = 63;
	uint64_t digit_val = pow2to63;
	int j;
	if(a[1] == 0) {
		j = 0; 
	} else {
		j = 1;
	}
	while(digit >= 0 && (a[j] & digit_val) == 0) {
		digit--;
		digit_val /= 2;
	}
	if(digit < 0) {
		return 0;
	} else {
		return 64*j + digit;
	}
}

int degree_len2(uint64_t * a) {
	if(a[1] == 0) {
		if(a[0] == 0) {
			return 0;
		}
		int upper = a[0] >> 32;
		if(upper == 0) {
			return 31 - __builtin_clz(a[0]);
		}
		return 63 - __builtin_clz(upper);
	} else {
		int upper = a[1] >> 32;
		if(upper == 0) {
			return 95 - __builtin_clz(a[1]);
		}
		return 127 - __builtin_clz(upper);
	}
}

int degree(uint64_t * a, int len) {
	int degree = 0;
	for(int i = len -1; i >= 0; i--) {
		if(a[i] == 0) {
			continue;
		}
		int upper = a[i] >> 32;
		if(upper == 0) {
			return 31 + 64*i - __builtin_clz(a[i]);
		}
		return 63 + 64*i - __builtin_clz(upper);
	}
}

/*
* Precondition:
*	a and b have min length len
*/
void swap_arrays(uint64_t * a, uint64_t * b, int len) {
	for(int i = 0; i < len; i++) {
		uint64_t temp = a[i];
		a[i] = b[i];
		b[i] = temp;
	}
}	

void lshift_polynomial_optimized(uint64_t * a, int shift) {
	if(shift == 0) {
		return;
	} else if(shift > 63) {
		a[1] = a[0] << (shift - 64);
		a[0] = 0;
	} else {
		uint64_t carry = a[0] >> (64 - shift);
		a[0] <<= shift;
		a[1] <<= shift;
		a[1] |= carry;
	}
}

/*
* Output is d = gcd(a,b), and g,h so that ag + bh = d
* Preconditions:
*	All arrays have length 2
* 	0 < degree(a) <= degree(b)
*/
void extended_euclid(uint64_t * a, uint64_t * b, uint64_t * d, uint64_t * g, uint64_t * h) {
	/* Step 1 */
	uint64_t * u = malloc(2*sizeof(uint64_t));
	uint64_t * v = malloc(2*sizeof(uint64_t));
	memcpy(u, a, sizeof(uint64_t)*2);
	memcpy(v, b, sizeof(uint64_t)*2);
	//int deg_u = degree_len2(u);
	int deg_u;
	if(u[1] == 0) {
		if(u[0] == 0) {
			deg_u = 0;
		} else {
			int upper = u[0] >> 32;
			if(upper == 0) {
				deg_u = 31 - __builtin_clz(u[0]);
			} else {
				deg_u = 63 - __builtin_clz(upper);
			}
		}
	} else {
		int upper = u[1] >> 32;
		if(upper == 0) {
			deg_u = 95 - __builtin_clz(u[1]);
		} else {
			deg_u = 127 - __builtin_clz(upper);
		}
	}
	//int deg_v = degree_len2(v);
	int deg_v;
	if(v[1] == 0) {
		if(v[0] == 0) {
			deg_v = 0;
		} else {
			int upper = v[0] >> 32;
			if(upper == 0) {
				deg_v = 31 - __builtin_clz(v[0]);
			} else {
				deg_v = 63 - __builtin_clz(upper);
			}
		}
	} else {
		int upper = v[1] >> 32;
		if(upper == 0) {
			deg_v = 95 - __builtin_clz(v[1]);
		} else {
			deg_v = 127 - __builtin_clz(upper);
		}
	}

	/* Step 2 */ //Size might be wrong but my logic is that deg(u)-deg(v) is max 126, and original g1 can only feed from u-v so this is max degree
	uint64_t * g1 = malloc(2*sizeof(uint64_t));
	g1[0] = 1;
	g1[1] = 0;
	uint64_t * g2 = malloc(2*sizeof(uint64_t));
	g2[0] = g2[1] = 0;
	uint64_t * h1 = malloc(2*sizeof(uint64_t));
	h1[0] = h1[1] = 0;
	uint64_t * h2 = malloc(2*sizeof(uint64_t));
	h2[0] = 1;
	h2[1] = 0;
	
	/* Step 3 */
	while (u[0] > 0 || u[1] > 0) {
		/* Step 3.1 */
		int j = deg_u - deg_v;
		
		/* Step 3.2 */
		if(j < 0) {
			uint64_t * tmp = u;
			u = v;
			v = tmp;
			
			tmp = g1;
			g1 = g2;
			g2 = tmp;
			
			tmp = h1;
			h1 = h2;
			h2 = tmp;
			
			j = -j;
			
			int temp8 = deg_u;
			deg_u =  deg_v;
			deg_v = temp8;
		}
		
		/* Step 3.3 */
		uint64_t shift_polynomial[2];
		memcpy(shift_polynomial, v, sizeof(uint64_t)*2);
		//lshift_polynomial_optimized(shift_polynomial, j);
		if(j != 0) {
			if(j > 63) {
				shift_polynomial[1] = shift_polynomial[0] << (j - 64);
				shift_polynomial[0] = 0;
			} else {
				uint64_t carry = shift_polynomial[0] >> (64 - j);
				shift_polynomial[0] <<= j;
				shift_polynomial[1] <<= j;
				shift_polynomial[1] |= carry;
			}
		}
		//add(shift_polynomial, u, u);
		u[0] = shift_polynomial[0] ^ u[0];
		u[1] = shift_polynomial[1] ^ u[1];
		//deg_u = degree_len2(u);
		if(u[1] == 0) {
			if(u[0] == 0) {
				deg_u = 0;
			} else {
				int upper = u[0] >> 32;
				if(upper == 0) {
					deg_u = 31 - __builtin_clz(u[0]);
				} else {
					deg_u = 63 - __builtin_clz(upper);
				}
			}
		} else {
			int upper = u[1] >> 32;
			if(upper == 0) {
				deg_u = 95 - __builtin_clz(u[1]);
			} else {
				deg_u = 127 - __builtin_clz(upper);
			}
		}
		
		/*Step 3.4 */
		memcpy(shift_polynomial, g2, sizeof(uint64_t)*2);
		//lshift_polynomial_optimized(shift_polynomial, j);
		if(j != 0) {
			if(j > 63) {
				shift_polynomial[1] = shift_polynomial[0] << (j - 64);
				shift_polynomial[0] = 0;
			} else {
				uint64_t carry = shift_polynomial[0] >> (64 - j);
				shift_polynomial[0] <<= j;
				shift_polynomial[1] <<= j;
				shift_polynomial[1] |= carry;
			}
		}
		//add(shift_polynomial, g1, g1);
		g1[0] = shift_polynomial[0] ^ g1[0];
		g1[1] = shift_polynomial[1] ^ g1[1];
		
		memcpy(shift_polynomial, h2, sizeof(uint64_t)*2);
		//lshift_polynomial_optimized(shift_polynomial, j);
		if(j != 0) {
			if(j > 63) {
				shift_polynomial[1] = shift_polynomial[0] << (j - 64);
				shift_polynomial[0] = 0;
			} else {
				uint64_t carry = shift_polynomial[0] >> (64 - j);
				shift_polynomial[0] <<= j;
				shift_polynomial[1] <<= j;
				shift_polynomial[1] |= carry;
			}
		}
		//add(shift_polynomial, h1, h1);
		h1[0] = shift_polynomial[0] ^ h1[0];
		h1[1] = shift_polynomial[1] ^ h1[1];
	}
	/* Step 4 */
	//swap_arrays(d, v, 2);
	d[0] = v[0];
	d[1] = v[1];
	//swap_arrays(g, g2, 2);
	g[0] = g2[0];
	g[1] = g2[1];
	//swap_arrays(h, h2, 2);
	h[0] = h2[0];
	h[1] = h2[1];
	free(u);
	free(v);
	free(g1);
	free(g2);
	free(h1);
	free(h2);
}

/* 
* Alg 2.48 - Inversion in binary field using the extended Euclidean algorithm
* Preconditions:
* 	Arrays have length 2
*	a has max degree 126 and is nonzero
*/
void inv_euclid(uint64_t * a, uint64_t * inv_a) {
	/* Step 1 */
	uint64_t * u = malloc(2* sizeof(uint64_t));
	uint64_t * v = malloc(2*sizeof(uint64_t));
	memcpy(u, a, sizeof(uint64_t)*2);
	memcpy(v, f, sizeof(uint64_t)*2);
	int deg_u;
	/* Calculating degree of u */
	if(u[1] == 0) {
		if(u[0] == 0) {
			deg_u = 0;
		} else {
			int upper = u[0] >> 32;
			if(upper == 0) {
				deg_u = 31 - __builtin_clz(u[0]);
			} else {
				deg_u = 63 - __builtin_clz(upper);
			}
		}
	} else {
		int upper = u[1] >> 32;
		if(upper == 0) {
			deg_u = 95 - __builtin_clz(u[1]);
		} else {
			deg_u = 127 - __builtin_clz(upper);
		}
	}
	//int deg_u = degree_len2(u);
	int deg_v = 127;

	/* Step 2 */ //Size might be wrong but my logic is that deg(u)-deg(v) is max 126, and original g1 can only feed from u-v so this is max degree
	uint64_t * g1 = malloc(2*sizeof(uint64_t));
	g1[0] = 1;
	g1[1] = 0;
	uint64_t * g2 = malloc(2*sizeof(uint64_t));
	g2[0] = g2[1] = 0;
	
	/* Step 3 */
	while (!(u[0] == 1 && u[1] == 0)) {
		/* Step 3.1 */
		int j = deg_u - deg_v;
		
		/* Step 3.2 */
		if(j < 0) {
			//swap_arrays(u, v, 2);
			uint64_t * tmp = u;
			u = v;
			v = tmp;
			
			tmp = g1;
			g1 = g2;
			g2 = tmp;
			//swap_arrays(g1, g2, 2);
			
			j = -j;
			
			int temp8 = deg_u;
			deg_u =  deg_v;
			deg_v = temp8;
		}
		/* Step 3.3 */
		uint64_t shift_polynomial[2];
		/* shift v */
		memcpy(shift_polynomial, v, sizeof(uint64_t)*2);
		if(j != 0) {
			if(j > 63) {
				shift_polynomial[1] = shift_polynomial[0] << (j - 64);
				shift_polynomial[0] = 0;
			} else {
				uint64_t carry = shift_polynomial[0] >> (64 - j);
				shift_polynomial[0] <<= j;
				shift_polynomial[1] <<= j;
				shift_polynomial[1] |= carry;
			}
		}
		//lshift_polynomial_optimized(shift_polynomial, j);
		u[0] = shift_polynomial[0] ^ u[0];
		u[1] = shift_polynomial[1] ^ u[1];
		//add(shift_polynomial, u, u);
		
		/* Calculating degree of u */
		if(u[1] == 0) {
			if(u[0] == 0) {
				deg_u = 0;
			} else {
				int upper = u[0] >> 32;
				if(upper == 0) {
					deg_u = 31 - __builtin_clz(u[0]);
				} else {
					deg_u = 63 - __builtin_clz(upper);
				}
			}
		} else {
			int upper = u[1] >> 32;
			if(upper == 0) {
				deg_u = 95 - __builtin_clz(u[1]);
			} else {
				deg_u = 127 - __builtin_clz(upper);
			}
		}
		//deg_u = degree_len2(u);
		
		/*Step 3.4 */
		/*shift g2 */
		memcpy(shift_polynomial, g2, sizeof(uint64_t)*2);
		if(j != 0) {
			if(j > 63) {
				shift_polynomial[1] = shift_polynomial[0] << (j - 64);
				shift_polynomial[0] = 0;
			} else {
				uint64_t carry = shift_polynomial[0] >> (64 - j);
				shift_polynomial[0] <<= j;
				shift_polynomial[1] <<= j;
				shift_polynomial[1] |= carry;
			}
		}
		//lshift_polynomial_optimized(shift_polynomial, j);
		g1[0] = shift_polynomial[0] ^ g1[0];
		g1[1] = shift_polynomial[1] ^ g1[1];
		//add(shift_polynomial, g1, g1);
	}
	/* Step 4 */
	inv_a[0] = g1[0];
	inv_a[1] = g1[1];
	free(u);
	free(v);
	free(g1);
	free(g2);
}

/*
* Alg 2.49 - Binary Inversion Algorithm
* Preconditions:
* 	Arrays have length 2
*	a has max degree 126
*/
void inv_binary(uint64_t * a, uint64_t * inv_a) {
	/* Step 1 */
	uint64_t u[2];
	uint64_t v[2];
	memcpy(u, a, sizeof(uint64_t)*2);
	memcpy(v, f, sizeof(uint64_t)*2);
	
	/* Step 2 */
	uint64_t g1[2] = {1, 0 };
	uint64_t g2[2] = {0, 0 };
	
	/* Step 3 */
	while(!(u[0] == 1 && u[1] == 0) && !(v[0] == 1 && v[1] == 0)) {
		/* Step 3.1 */
		while(u[0] % 2 == 0) {
			rshift_polynomial(u, 2);
			if(g1[0] % 2 == 1) {
				add(f, g1, g1);
			}
			rshift_polynomial(g1, 2);
		}
		/* Step 3.2 */
		while(v[0] % 2 == 0) {
			rshift_polynomial(v, 2);
			if(g2[0] % 2 == 1) {
				add(f, g2, g2);
			}
			rshift_polynomial(g2, 2);
		}
		/* Step 3.3 */
		if(u[1] > v[1] || (u[1] == v[1] && u[0] > v[0])) {
			add(v, u, u);
			add(g2, g1, g1);
		} else {
			add(u, v, v);
			add(g1, g2, g2);
		}
	}
	/* Step 4 */
	if(u[0] == 1 && u[1] == 0) {
		swap_arrays(inv_a, g1, 2);
	} else {
		swap_arrays(inv_a, g2, 2);
	}
}

/*
void main() {*/
	/* Initialize rand */
	/*srand(time(NULL));
	
	printf("Reduction polynomial f(z): \n");
	print_polynomial(f, 2);
	
	uint64_t rnd[2];
	rand_element(rnd);
	printf("\nRandom element\n");
	print_polynomial(rnd, 2);
	
	printf("\nFixed polynomial a\n");
	int a_coefficients[5] = {0, 2, 3, 5, 64};
	uint64_t a[2];
	coefficients_to_polynomial(a_coefficients, 5, a, 2);
	print_polynomial(a, 2);
	
	printf("\nFixed polynomial b\n");
	int b_coefficients[7] = {1, 2, 4, 8, 16, 32, 64};
	uint64_t b[2];
	coefficients_to_polynomial(b_coefficients, 7, b, 2);
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
	coefficients_to_polynomial(NULL, 0, zero, 2);
	print_polynomial(zero,2);
	
	printf("\nFixed polynomial c\n");
	int c_coefficients[1] = {2};
	uint64_t c[2];
	coefficients_to_polynomial(c_coefficients, 1, c, 2);
	print_polynomial(c,2);
	
	printf("\nFixed polynomial d\n");
	int d_coefficients[2] = {0, 126};
	uint64_t d[2];
	coefficients_to_polynomial(d_coefficients, 2, d, 2);
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
	
	printf("\nDegree of zero %" PRIu8 "\n", degree(zero));
	printf("\nDegree of b %" PRIu8 "\n", degree(b));
	
	printf("\nExtended Euclidean Algorithm Test 1\n");
	int coefficients_euclid_a[4] = {3, 4, 6, 7};
	uint64_t euclid_a[2];
	coefficients_to_polynomial(coefficients_euclid_a, 4, euclid_a, 2);
	printf("euclid_a: ");
	print_polynomial(euclid_a, 2);
	
	int coefficients_euclid_b[4] = {3, 5, 7, 9};
	uint64_t euclid_b[2];
	coefficients_to_polynomial(coefficients_euclid_b, 4, euclid_b, 2);
	printf("euclid_b: ");
	print_polynomial(euclid_b, 2);
	
	uint64_t result_d[2];
	uint64_t result_g[2];
	uint64_t result_h[2];
	extended_euclid(euclid_a, euclid_b, result_d, result_g, result_h);
	printf("d: ");
	print_polynomial(result_d, 2);
	printf("g: ");
	print_polynomial(result_g, 2);
	printf("h: ");
	print_polynomial(result_h, 2);
	
	printf("\nExtended Euclidean Algorithm Test 2\n");
	printf("c: ");
	print_polynomial(c, 2);
	
	printf("r: ");
	print_polynomial(r, 2);
	
	extended_euclid(c, r, result_d, result_g, result_h);
	printf("d: ");
	print_polynomial(result_d, 2);
	printf("g: ");
	print_polynomial(result_g, 2);
	printf("h: ");
	print_polynomial(result_h, 2);
	
	printf("\nRuntime test of Extended Euclidean\n");
	clock_t start, end;
	double time_sum = 0;
	double time_min = 99999;
	double time_max = -1;
	int num_tests = 10;
	for(int i = 0; i < num_tests; i++) {
		uint64_t test_a[2];
		rand_element(a);
		uint64_t test_b[2];
		rand_element(b);
		
		start = clock();
		extended_euclid(test_a, test_b, result_d, result_g, result_h);
		end = clock();
		double time = ((double) (end - start )) / CLOCKS_PER_SEC;
		printf("Runtime %d: %.6fs\n", i+1, time);
		time_sum += time;
		
		if(time_min > time) {
			time_min = time;
		}
		if(time_max < time) {
			time_max = time;
		}
	}
	double time_average = time_sum / num_tests;
	printf("Average runtime: %.6f\n", time_average);
	printf("Min runtime: %.6f Max runtime: %.6f\n", time_min, time_max);
	
	printf("\nInversion of a with Extended Euclid\n");
	uint64_t inv_a[2];
	inv_euclid(a, inv_a);
	print_polynomial(inv_a, 2);
	printf("\nMultiplying inverse a with a\n");
	uint64_t should_be1[2];
	mult_shiftadd(a, inv_a, should_be1);
	print_polynomial(should_be1, 2);
	
	printf("\nInversion of a with Binary Algorithm\n");
	uint64_t inv_a1[2];
	inv_binary(a, inv_a1);
	print_polynomial(inv_a1, 2);
	
	printf("\nPrecomputation of reduction polynomials\n");
	reduction_generic_precompute();
	print_polynomial(reduction_polynomials[0], 2);
	print_polynomial(reduction_polynomials[1], 2);
	print_polynomial(reduction_polynomials[63], 2);
	
	printf("\nReducing result of mult using left to right comb method with window\n");
	reduction_generic(result_mult_lrcombwindow);
	print_polynomial(result_mult_lrcombwindow, 2);
}*/
