// MyTDist.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <math.h>

/************ loggamma(x) -- gamma.c ªèªêî¢? *************/

#define PI      3.14159265358979324  /* $\pi$ */
#define LOG_2PI 1.83787706640934548  /* $\log 2\pi$ */
#define N       8

#define B0  1                 /* ì¤ù»ªÏBernoulli? */
#define B1  (-1.0 / 2.0)
#define B2  ( 1.0 / 6.0)
#define B4  (-1.0 / 30.0)
#define B6  ( 1.0 / 42.0)
#define B8  (-1.0 / 30.0)
#define B10 ( 5.0 / 66.0)
#define B12 (-691.0 / 2730.0)
#define B14 ( 7.0 / 6.0)
#define B16 (-3617.0 / 510.0)



double loggamma(double x)  /* «¬«ó«Þ??ªÎ?? */
{
    double v, w;

    v = 1;
    while (x < N) {  v *= x;  x++;  }
    w = 1 / (x * x);
    return ((((((((B16 / (16 * 15))  * w + (B14 / (14 * 13))) * w
                + (B12 / (12 * 11))) * w + (B10 / (10 *  9))) * w
                + (B8  / ( 8 *  7))) * w + (B6  / ( 6 *  5))) * w
                + (B4  / ( 4 *  3))) * w + (B2  / ( 2 *  1))) / x
                + 0.5 * LOG_2PI - log(v) - x + (x - 0.5) * log(x);
}

double p_beta(double x, double a, double b)
{
    int k;
    double p1, q1, p2, q2, d, previous;

    if (a <= 0) return HUGE_VAL;
    if (b <= 0) {
        if (x <  1) return 0;
        if (x == 1) return 1;
        /* else */  return HUGE_VAL;
    }
    if (x > (a + 1) / (a + b + 2))
        return 1 - p_beta(1 - x, b, a);
    if (x <= 0)  return 0;
    p1 = 0;  q1 = 1;
    p2 = exp(a * log(x) + b * log(1 - x)
        + loggamma(a + b) - loggamma(a) - loggamma(b)) / a;
    q2 = 1;
    for (k = 0; k < 200; ) {
        previous = p2;
        d = - (a + k) * (a + b + k) * x
            / ((a + 2 * k) * (a + 2 * k + 1));
        p1 = p1 * d + p2;  q1 = q1 * d + q2;
        k++;
        d = k * (b - k) * x / ((a + 2 * k - 1) * (a + 2 * k));
        p2 = p2 * d + p1;  q2 = q2 * d + q1;
        if (q2 == 0) {
            p2 = HUGE_VAL;  continue;
        }
        p1 /= q2;  q1 /= q2;  p2 /= q2;  q2 = 1;
        if (p2 == previous) return p2;
    }
    printf("p_beta: ?áÖª·ªÞª»ªó.\n");
    return p2;
}



double p_tdist(double t, int df)  /* t ÝÂøÖªÎù»ö°ü¬áã */
{
    //return 1 - 0.5 * p_beta(df / (df + t * t), 0.5 * df, 0.5);
    return p_beta(df / (df + t * t), 0.5 * df, 0.5);
}


#if 0
int main(int argc, char* argv[])
{
//	printf("Hello World!\n");

	printf("1ohm:  p-val=0.000  p_t(13.281 ,  89) = %.3f\n",  p_t( (13.281),   89));
	printf("2ohm:  p-val=0.000  p_t(-45.268,  89) = %.3f\n",  p_t( (-45.268),  89));
	printf("3ohm:  p-val=0.568  p_t(0.573  ,  89) = %.3f\n",  p_t( (0.573),    89));
	printf("4ohm:  p-val=0.000  p_t(-47.256,  89) = %.3f\n",  p_t( (-47.256),  89));
	printf("5ohm:  p-val=0.000  p_t(-59.333,  89) = %.3f\n",  p_t( (-59.333),  89));
	printf("10ohm: p-val=0.000  p_t(-10.684,  89) = %.3f\n",  p_t( (-10.684),  89));

	printf("\n");
	printf("1ohm:  p-val=0.000  p_t(49.024, 89) = %.3f\n",   p_t( (49.024),  89));
	printf("2ohm:  p-val=0.000  p_t(10.244, 89) = %.3f\n",   p_t( (10.244),  89));
	printf("3ohm:  p-val=0.000  p_t(8.360 , 89) = %.3f\n",   p_t( (8.360),   89));
	printf("4ohm:  p-val=0.124  p_t(1.552 , 89) = %.3f\n",   p_t( (1.552),   89));
	printf("5ohm:  p-val=0.350  p_t(0.939 , 89) = %.3f\n",   p_t( (0.939),   89));
	printf("10ohm: p-val=0.000  p_t(8.882 , 89) = %.3f\n",   p_t( (8.882),   89));

	printf("1ohm:  p-val=0.000  p_t(12.115, 49) = %.3f\n",  p_t( (12.115),   49));
	printf("2ohm:  p-val=0.000  p_t(29.940, 49) = %.3f\n",  p_t( (29.940),   49));
	printf("3ohm:  p-val=0.008  p_t(2.782 , 49) = %.3f\n",  p_t( (2.782),    49));
	printf("4ohm:  p-val=0.000  p_t(36.442, 49) = %.3f\n",  p_t( (36.442),   49));
	printf("5ohm:  p-val=0.000  p_t(42.986, 49) = %.3f\n",  p_t( (42.986),   49));
	printf("10ohm: p-val=0.000  p_t(11.173, 49) = %.3f\n",  p_t( (11.173),   49));

	printf("\n");
	printf("p-val=0.000  p_t(45.268.89) = %.3f\n",  p_t( (45.268),  89));

	getchar();
	return 0;
}

#endif



#if 0
#include "nmath.h"
#include "dpq.h"

double pt(double x, double n, int lower_tail, int log_p)
{
/* return  P[ T <= x ]	where
 * T ~ t_{n}  (t distrib. with n degrees of freedom).
 *	--> ./pnt.c for NON-central
 */
    double val, nx;
#ifdef IEEE_754
    if (ISNAN(x) || ISNAN(n))
	return x + n;
#endif
    if (n <= 0.0) ML_ERR_return_NAN;

    if(!R_FINITE(x))
	return (x < 0) ? R_DT_0 : R_DT_1;
    if(!R_FINITE(n))
	return pnorm(x, 0.0, 1.0, lower_tail, log_p);

#ifdef R_version_le_260
    if (n > 4e5) { /*-- Fixme(?): test should depend on `n' AND `x' ! */
	/* Approx. from	 Abramowitz & Stegun 26.7.8 (p.949) */
	val = 1./(4.*n);
	return pnorm(x*(1. - val)/sqrt(1. + x*x*2.*val), 0.0, 1.0,
		     lower_tail, log_p);
    }
#endif

    nx = 1 + (x/n)*x;
    /* FIXME: This test is probably losing rather than gaining precision,
     * now that pbeta(*, log_p = TRUE) is much better.
     * Note however that a version of this test *is* needed for x*x > D_MAX */
    if(nx > 1e100) { /* <==>  x*x > 1e100 * n  */
	/* Danger of underflow. So use Abramowitz & Stegun 26.5.4
	   pbeta(z, a, b) ~ z^a(1-z)^b / aB(a,b) ~ z^a / aB(a,b),
	   with z = 1/nx,  a = n/2,  b= 1/2 :
	*/
	double lval;
	lval = -0.5*n*(2*log(fabs(x)) - log(n))
		- lbeta(0.5*n, 0.5) - log(0.5*n);
	val = log_p ? lval : exp(lval);
    } else {
	val = (n > x * x)
	    ? pbeta (x * x / (n + x * x), 0.5, n / 2., /*lower_tail*/0, log_p)
	    : pbeta (1. / nx,             n / 2., 0.5, /*lower_tail*/1, log_p);
    }

    /* Use "1 - v"  if	lower_tail  and	 x > 0 (but not both):*/
    if(x <= 0.)
	lower_tail = !lower_tail;

    if(log_p) {
	if(lower_tail) return log1p(-0.5*exp(val));
	else return val - M_LN2; /* = log(.5* pbeta(....)) */
    }
    else {
	val /= 2.;
	return R_D_Cval(val);
    }
}
#endif
