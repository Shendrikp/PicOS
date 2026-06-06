/*------------------------------------------------------------------------/
/  Universal String Handler for Console Input and Output
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2021, ChaN, all right reserved.
/
/ xprintf module is an open source software. Redistribution and use of
/ xprintf module in source and binary forms, with or without modification,
/ are permitted provided that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/-------------------------------------------------------------------------*/

#include "xprintf_float.h"
#include <math.h>

// Buffer size for output
#define SZB_OUTPUT	32

void (*xfunc_output)(int);	/* Pointer to the default output device */
int (*xfunc_input)(void);	/* Pointer to the default input device */

static int ilog10 (float n)	/* Calculate log10(n) in integer output */
{
	int rv = 0;

	while (n >= 10) {	/* Decimate digit in right shift */
		if (n >= 100000) {
			n /= 100000; rv += 5;
		} else {
			n /= 10; rv++;
		}
	}
	while (n < 1) {		/* Decimate digit in left shift */
		if (n < 0.00001) {
			n *= 100000; rv -= 5;
		} else {
			n *= 10; rv--;
		}
	}
	return rv;
}


static float i10x (int n)	/* Calculate 10^n */
{
	float rv = 1;

	while (n > 0) {		/* Left shift */
		if (n >= 5) {
			rv *= 100000; n -= 5;
		} else {
			rv *= 10; n--;
		}
	}
	while (n < 0) {		/* Right shift */
		if (n <= -5) {
			rv /= 100000; n += 5;
		} else {
			rv /= 10; n++;
		}
	}
	return rv;
}

void xftoa (
	char* buf,	/* Buffer to output the generated string */
	float val,	/* Real number to output */
	int prec,	/* Number of fractinal digits */
	char fmt	/* Notation */
)
{
	int d;
	int e = 0, m = 0;
	char sign = 0;
	float w;
	const char *er = 0;


	if (isnan(val)) {			/* Not a number? */
		er = "NaN";
	} else {
		if (prec < 0) prec = 6;	/* Default precision (6 fractional digits) */
		if (val < 0) {			/* Nagative value? */
			val = -val; sign = '-';
		} else {
			sign = '+';
		}
		if (isinf(val)) {		/* Infinite? */
			er = "INF";
		} else {
			if (fmt == 'f') {	/* Decimal notation? */
				val += i10x(-prec) / 2;	/* Round (nearest) */
				m = ilog10(val);
				if (m < 0) m = 0;
				/* Buffer overflow? */
				if (m + prec + 3 >= SZB_OUTPUT) er = "OV";
			} else {			/* E notation */
				if (val != 0) {		/* Not a true zero? */
					/* Round (nearest) */
					val += i10x(ilog10(val) - prec) / 2;
					e = ilog10(val);
					/* Buffer overflow or E > +99? */
					if (e > 99 || prec + 6 >= SZB_OUTPUT) {
						er = "OV";
					} else {
						if (e < -99) e = -99;
						val /= i10x(e);	/* Normalize */
					}
				}
			}
		}
		if (!er) {	/* Not error condition */
			if (sign == '-') *buf++ = sign;	/* Add a - if negative value */
			do {				/* Put decimal number */
				w = i10x(m);				/* Snip the highest digit d */
				d = val / w; val -= d * w;
				if (m == -1) *buf++ = XF_DPC;	/* Insert a decimal separarot 
				if get into fractional part */
				*buf++ = '0' + d;			/* Put the digit */
			} while (--m >= -prec);			/* Output all digits specified 
			by prec */
			if (fmt != 'f') {	/* Put exponent if needed */
				*buf++ = fmt;
				if (e < 0) {
					e = -e; *buf++ = '-';
				} else {
					*buf++ = '+';
				}
				*buf++ = '0' + e / 10;
				*buf++ = '0' + e % 10;
			}
		}
	}
	if (er) {	/* Error condition? */
		if (sign) *buf++ = sign;		/* Add sign if needed */
		do *buf++ = *er++; while (*er);	/* Put error symbol */
	}
	*buf = 0;	/* Term */
}

void xputs (			/* Put a string to the default device */
	const char* str		/* Pointer to the string */
)
{
	while (*str != '\0')
		xfunc_output(*str++);
}

int xgets (			/* 0:End of stream, 1:A line arrived */
	char* buff,		/* Pointer to the buffer */
	int len			/* Buffer length */
)
{
	int c, i;


	if (!xfunc_input) return 0;	/* No input function is specified */

	i = 0;
	for (;;) {
		c = xfunc_input();			/* Get a char from the incoming stream */
		if (c < 0 || c == '\r') break;	/* End of stream or CR? */
		if (c == '\b' && i) {		/* BS? */
			i--;
			if (XF_INPUT_ECHO) xfunc_output(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	/* Visible chars? */
			buff[i++] = c;
			if (XF_INPUT_ECHO) xfunc_output(c);
		}
	}
	if (XF_INPUT_ECHO) {
		xfunc_output('\r');
		xfunc_output('\n');
	}
	buff[i] = 0;	/* Terminate with a \0 */
	return (int)(c == '\r');
}

/*----------------------------------------------*/
/* Get a value of integer string                */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
	    ^                           1st call returns 123 and next ptr
	       ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/
int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;


	*res = 0;

	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */

	if (c == '-') {		/* negative? */
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x':		/* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b':		/* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1;	/* single zero */
			if (c < '0' || c > '9') return 0;	/* invalid char */
			r = 8;		/* octal */
		}
	} else {
		if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
		r = 10;			/* decimal */
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;	/* invalid char */
		}
		if (c >= r) return 0;		/* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;			/* apply sign if needed */

	*res = val;
	return 1;
}

int xatof (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	float *res		/* Pointer to the valiable to store the value */
)
{
	float val;
	int s, f, e;
	unsigned char c;


	*res = 0;
	s = f = 0;

	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */
	if (c == '-') {			/* Negative? */
		c = *(++(*str)); s = 1; 
	} else if (c == '+') {	/* Positive? */
		c = *(++(*str));
	}
	if (c == XF_DPC) {		/* Leading dp? */
		f = -1; 			/* Start at fractional part */
		c = *(++(*str));
	}
	if (c <= ' ') return 0;	/* Wrong termination? */
	val = 0;
	while (c > ' ') {		/* Get a value of decimal */
		if (c == XF_DPC) {	/* Embedded dp? */
			if (f < 0) return 0;	/* Wrong dp? */
			f = -1;			/* Enter fractional part */
		} else {
			if (c < '0' || c > '9') break;	/* End of decimal? */
			c -= '0';
			if (f == 0) {	/* In integer part */
				val = val * 10 + c;
			} else {		/* In fractional part */
				val += i10x(f--) * c;
			}
		}
		c = *(++(*str));
	}
	if (c > ' ') {	/* It may be an exponent */
		if (c != 'e' && c != 'E') return 0;	/* Wrong character? */
		c = *(++(*str));
		if (c == '-') {
			c = *(++(*str)); s |= 2;	/* Negative exponent */
		} else if (c == '+') {
			c = *(++(*str));			/* Positive exponent */
		}
		if (c <= ' ') return 0;	/* Wrong termination? */
		e = 0;
		while (c > ' ') {		/* Get value of exponent */
			c -= '0';
			if (c > 9) return 0;	/* Not a numeral? */
			e = e * 10 + c;
			c = *(++(*str));
		}
		val *= i10x((s & 2) ? -e : e);	/* Apply exponent */
	}

	if (s & 1) val = -val;	/* Negate sign if needed */

	*res = val;
	return 1;
}
