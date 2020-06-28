#include <inttypes.h>
#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Alg 2.32 - Addition. Also extended it for special cases.
 * Precondition: Arrays have length 2
 */
void add(uint64_t * a, uint64_t * b, uint64_t * c);

/* 
 * Alg 2.33 Right-to-left shift-and-add field multiplication
 * Note that the result is reduced.
 * Preconditions: 
 * 	Arrays have length 2
 * 	a,b are max of order 126
 */
void mult_shiftadd(uint64_t * a, uint64_t * b, uint64_t * c);

/*
* Alg 2.34 Right-to-left comb method for polynomial multiplication
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
*/
void mult_polynomial_rlcomb(uint64_t * a, uint64_t * b, uint64_t * c);

/*
* Alg 2.35 Left-to-right comb method for polynomial multiplication
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
*/
void mult_polynomial_lrcomb(uint64_t * a, uint64_t * b, uint64_t * c);

/*
* Alg 2.36 Left-to-right comb method for polynomial multiplication with window size 8
* Note that it does not reduce the result.
* Preconditions:
*   c is of length 4
*   a,b are of max degree 126, length 2
*/
void mult_polynomial_lrcomb_window8(uint64_t * a, uint64_t * b, uint64_t * c);

/*
 * Alg 2.39 Polynomial squaring
 *
 * Preconditions:
 * a has max degree 126, length 2
 * c has length 4
 */
void square_polynomial(uint64_t * a, uint64_t * c);

/*
 * Alg 2.40 - Modular reduction (one bit at a time)
 *
 * Reduces c mod f, 
 * after which only the first 2 indices are guaranteed to be correct.
 * Precondition:
 * 	c has at most degree 2m-2
 *		c has length 4
 */
void reduction_generic(uint64_t * c);

/*  Alg 2.47 Extended Euclidean algorithm for binary polynomials
 * Output is d = gcd(a,b), and g,h so that ag + bh = d
 * Preconditions:
 *		All arrays have length 2
 * 	0 < degree(a) <= degree(b)
 */
 void extended_euclid(uint64_t * a, uint64_t * b, uint64_t * d, uint64_t * g, uint64_t * h);
 
 /* 
* Alg 2.48 - Inversion in binary field using the extended Euclidean algorithm
* Preconditions:
* 	Arrays have length 2
*	a has max degree 126 and is nonzero
*/
void inv_euclid(uint64_t * a, uint64_t * inv_a);

/*
* Alg 2.49 - Binary Inversion Algorithm
* Preconditions:
* 	Arrays have length 2
*	a has max degree 126
*/
void inv_binary(uint64_t * a, uint64_t * inv_a);

/*
 * Degree of polynomial
 * Precondition:
 * 	a has length 2
 */
 

/* ======== Helper Methods: ============= */

/*
 * Prints the array as an array of uint64_t.
 */
void print_array(uint64_t * a, uint32_t len);

/*
 * Prints the array as a binary polynomial.
 */
 void print_polynomial(uint64_t * a, uint32_t len);
 
 /*
  * Returns 1 if equal, else 0.
  */
 bool equal_polynomials(uint64_t * a, uint64_t * b, uint32_t len);
 
 /* 
  * Precondition: Indices are in increasing order, and are unique. 
  */
 void index_to_polynomial(uint64_t * indices, int len_i, uint64_t *a, uint32_t len_a);
 
 /*
  * Generates a random member of the binary field.
  * Precondition:
  * 	a has length 2
  */
 void rand_element(uint64_t * a);