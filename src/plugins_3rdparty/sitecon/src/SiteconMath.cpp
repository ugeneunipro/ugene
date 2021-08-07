/*
Module:       chisq.c
Purpose:      compute approximations to chisquare distribution probabilities
Contents:     pochisq(), critchi()
Uses:         poz() in z.c (Algorithm 209)
Programmer:   Gary Perlman
Organization: Wang Institute, Tyngsboro, MA 01879
Tester:       compile with -DCHISQTEST to include main program
Copyright:    none
Tabstops:     4
*/

#include "SiteconMath.h"

namespace U2 {

/*VAR normal z value */
double poz(double z) {
    double y, x, w;

    if (z == 0.0) {
        x = 0.0;
    } else {
        y = 0.5 * fabs(z);
        if (y >= (Z_MAX * 0.5)) {
            x = 1.0;
        } else if (y < 1.0) {
            w = y * y;
            x = ((((((((0.000124818987 * w - 0.001075204047) * w + 0.005198775019) * w - 0.019198292004) * w + 0.059054035642) * w - 0.151968751364) * w + 0.319152932694) * w - 0.531923007300) * w + 0.797884560593) * y * 2.0;
        } else {
            y -= 2.0;
            x = (((((((((((((-0.000045255659 * y + 0.000152529290) * y - 0.000019538132) * y - 0.000676904986) * y + 0.001390604284) * y - 0.000794620820) * y - 0.002034254874) * y + 0.006549791214) * y - 0.010557625006) * y + 0.011630447319) * y - 0.009279453341) * y + 0.005353579108) * y - 0.002141268741) * y + 0.000535310849) * y + 0.999936657524;
        }
    }
    return (z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5));
}

double pochisq(double x, int df) /* obtained chi-square value  degrees of freedom */
{
    double a, y = 0., s;
    double e, c, z;
    /* computes probability of normal z score */
    int even; /* true if df is an even number */

    if (x <= 0.0 || df < 1) {
        return (1.0);
    }

    a = 0.5 * x;
    even = (2 * (df / 2)) == df;
    if (df > 1) {
        y = ex(-a);
    }
    s = (even ? y : (2.0 * poz(-sqrt(x))));
    if (df > 2) {
        x = 0.5 * (df - 1.0);
        z = (even ? 1.0 : 0.5);
        if (a > BIGX) {
            e = (even ? 0.0 : LOG_SQRT_PI);
            c = log(a);
            while (z <= x) {
                e = log(z) + e;
                s += ex(c * z - a - e);
                z += 1.0;
            }
            return (s);
        } else {
            e = (even ? 1.0 : (I_SQRT_PI / sqrt(a)));
            c = 0.0;
            while (z <= x) {
                e = e * (a / z);
                c = c + e;
                z += 1.0;
            }
            return (c * y + s);
        }
    } else {
        return (s);
    }
}

/*FUNCTION critchi: compute critical chi square value to produce given p */

double critchi(double p, int df) {
    double minchisq = 0.0;
    double maxchisq = CHI_MAX;
    double chisqval;

    if (p <= 0.0) {
        return (maxchisq);
    } else if (p >= 1.0) {
        return (0.0);
    }

    chisqval = df / sqrt(p); /* fair first value */
    while (maxchisq - minchisq > CHI_EPSILON) {
        if (pochisq(chisqval, df) < p) {
            maxchisq = chisqval;
        } else {
            minchisq = chisqval;
        }
        chisqval = (maxchisq + minchisq) * 0.5;
    }
    return (chisqval);
}

}  // namespace U2
