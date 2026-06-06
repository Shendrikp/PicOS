/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2021     */
/*------------------------------------------------------------------------*/

#ifndef XPRINTF_DEF
#define XPRINTF_DEF
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XF_DPC			'.'	/* Decimal separator for floating point */
#define	XF_INPUT_ECHO	1	/* 1: Echo back input chars in xgets function */

#if defined(__GNUC__) && __GNUC__ >= 10
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#define xdev_out(func) xfunc_output = (void(*)(int))(func)
extern void (*xfunc_output)(int);

#define xdev_in(func) xfunc_input = (int(*)(void))(func)
extern int (*xfunc_input)(void);

void xputs (const char* str);
int xgets (char* buff, int len);

int xatoi (char** str, long* res);
int xatof (char** str, float* res);
void xftoa (char* buf,	float val, int prec, char fmt);

#ifdef __cplusplus
}
#endif

#endif
