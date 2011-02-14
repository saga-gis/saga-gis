/**********************************************************
 * Version $Id$
 *********************************************************/
/******************************************************************************
 *
 * File:           csa.h
 *
 * Created:        16/10/2002
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        A header for csa library (2D data approximation with
 *                 bivariate C1 cubic spline)
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_CSA_H)
#define _CSA_H

//---------------------------------------------------------
extern int csa_verbose;

#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------

#if !defined(_POINT_STRUCT)
#define _POINT_STRUCT
typedef struct {
    double x;
    double y;
    double z;
} point;
#endif

struct csa;
typedef struct csa csa;

csa* csa_create();
void csa_destroy(csa* a);
void csa_addpoints(csa* a, int n, point points[]);
void csa_calculatespline(csa* a);
void csa_approximate_point(csa* a, point* p);
void csa_approximate_points(csa* a, int n, point* points);

void csa_setnpmin(csa* a, int npmin);
void csa_setnpmax(csa* a, int npmax);
void csa_setk(csa* a, int k);
void csa_setnppc(csa* a, double nppc);

//---------------------------------------------------------
#if defined(__GNUC__)
	static const double NaN = 0.0 / 0.0;

#elif defined(BIG_ENDIAN) || defined(_BIG_ENDIAN)
	static const long long lNaN = 0x7fffffffffffffff;
	#define NaN (*(double*)&lNaN)

#elif defined(_SAGA_VC)
	static const __int64 lNaN = 0xfff8000000000000;
	#define NaN (*(double*)&lNaN)

	#define copysign	_copysign
	#define isnan		_isnan

#else
	static const long long lNaN = 0xfff8000000000000;
	#define NaN (*(double*)&lNaN)
#endif

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------

#endif
