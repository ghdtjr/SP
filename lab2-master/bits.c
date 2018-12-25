/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
	int result = ~((~x)|(~y));
	return result;
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
	int snum = n << 3; // change to byte num
	int c1num = x >> snum; // drive to the first byte
	int result = c1num&(255); // 255 == 11111111
	return result;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
	int num = x >> n;
	int oppose = 33 + (~n); //(32-n)

	int comp = ~0; // comp = -1
	int comp1 = (comp << (oppose+(~0)));
	comp1 = comp1 <<1; // because <<32 causes different values
	comp1 = ~comp1;
					      
	return comp1&num;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
	int cnt = 0; // store num of ones
	int result = 0;
	int a = 1;
	a += a<<8; // cut the byte and count
	a += a<<16;

	cnt += a&x;
	cnt += a&(x>>1);
	cnt += a&(x>>2);
	cnt += a&(x>>3);
	cnt += a&(x>>4);
	cnt += a&(x>>5);
	cnt += a&(x>>6);
	cnt += a&(x>>7);

	result = cnt&0xff;
	result += (cnt>>8)&0xff;
	result += (cnt>>16)&0xff;
	result += (cnt>>24)&0xff;

	return result;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
	int num1 = (~x) + 1; // == -x
	int result = num1|x;
	result = ~result;
	result = result>>31;
	result = (~result) + 1;
	return result;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
	int a = 1;
	int result = a << 31;
	return result;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
	int oppose = 33 + (~n);
	int num = (x << oppose);
	int num2 = num >> oppose;
	return !(x^num2);
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) { // add 1, if negative and have 1s in the divding bit
	int num2 = 1<<n;
	num2 = num2 + (~0);
	num2 = !!(x&num2);
	return (x >> n) + (num2&(x>>31));
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
	return ~x + 1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  	int num = x >> 31;
	int result = !((num | (!x))); // !(negative or zero)
	return result;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
	int num2, num3;
	int sub = y+(~x)+1; // y-x
	int num1 = (x^y)>>31; // if x,y has same sign -> 000000... or not 11111...
	int a = sub>>31;
	num1 = !!((!((a)|(!sub))) & (~num1));
	num2 = (((x^y)>>31)&((~((y>>31)))&1)); //(negative,positive), (positive,negative)
	num3 = !(sub); // x==y
	return num1|num2|num3;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
	int cnt = 0;
	int num1 = (1<<16)+(~0); // first search par
	int Sub = (x + (~num1) + 1); // x - mid value
	int shiftnum = !((Sub>>31)|(!Sub)); // Sub is positive
	int tempx = (x >>(shiftnum <<4));
	cnt += (shiftnum<<4);

	num1 = (1<<8)+(~0); // second search par
	Sub = (tempx + (~num1) + 1);
	shiftnum = !((Sub>>31)|(!Sub));
	tempx = (tempx >>(shiftnum <<3));
	cnt += ((shiftnum<<3));

	num1 = (1<<4)+(~0); // third search par
	Sub = (tempx + (~num1) + 1);
	shiftnum = !((Sub>>31)|(!Sub));
	tempx = (tempx >>(shiftnum <<2));
	cnt += ((shiftnum<<2));
	
	num1 = (1<<2)+(~0); // fourth search par
	Sub = (tempx + (~num1) + 1);
	shiftnum = !((Sub>>31)|(!Sub));
	tempx = (tempx >>(shiftnum <<1));
	cnt += (shiftnum<<1);

	num1 = 1; // fifth search par
	Sub = (tempx + (~num1) + 1);
	shiftnum = !((Sub>>31)|(!Sub));
	cnt += shiftnum;

	return cnt;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
 	unsigned nanexp = (0xff<<23);
	if ( ((nanexp&uf) == nanexp) && (uf& ((1<<23)+(~0)))) { // if exp is 1111111 & there are fraction
		return uf;
	}
	return uf+(1<<31);
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  	int n = 30;
	int tempx, frc, exp;
	int msb = ((x>>31)<<31); // msb = 100000000... or 000000000...
	if (x == 0) return 0;
	if (x == 0x80000000) return 0xcf000000;
	if (msb){// if negative
		x = -x;
	}
	tempx = x<<1;

	while (tempx^0x80000000) {// until searching leftest 1
		n = n-1;
		tempx = tempx<<1;
	}

	exp = ((n+127)<<23); // 0 exp frc
	frc = (tempx&(~(0x80000000)));
	frc = frc >> 8;

	if ((frc&1)&&(tempx&128) || ((tempx&255)==255)){
		if (frc>23){
			exp += 1;
			frc = 0;
		}
		return (msb|exp|frc) + 1;
	}
	return msb|exp|frc;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
	unsigned nanexp = (0xff<<23);
	if (uf == 0 | uf == (1<<31)) { //case of 0
		return uf;
	}
	if ((nanexp&uf) == nanexp) { // case of nan or infinity
		return uf;
	}
	if (!((uf>>23)&0xff)) { // exp = 0000 0000
		return ((uf<<1) |((uf>>31)<<31));
	}
	return uf + (1<<23); 
}
