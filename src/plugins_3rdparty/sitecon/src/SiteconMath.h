#ifndef _U2_SITECON_MATH_H_
#define _U2_SITECON_MATH_H_

#include <math.h>

namespace U2 {

#define CHI_EPSILON 0.000001 /* accuracy of critchi approximation */
#define CHI_MAX 99999.0 /* maximum chi square value */

#define LOG_SQRT_PI 0.5723649429247000870717135 /* log (sqrt (pi)) */
#define I_SQRT_PI 0.5641895835477562869480795 /* 1 / sqrt (pi) */
#define BIGX 20.0 /* max value to represent exp (x) */
#define ex(x) (((x) < -BIGX) ? 0.0 : exp(x))
#define Z_EPSILON 0.000001 /* accuracy of critz approximation */
#define Z_MAX 6.0 /* maximum meaningful z value */

double pochisq(double x, int df);
double critchi(double p, int df);
double poz(double z);

}    // namespace U2

#endif
