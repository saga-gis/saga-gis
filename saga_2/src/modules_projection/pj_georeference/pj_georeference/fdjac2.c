/* fdjac2.c -- calculate approximation to jacobian matrix. */
#include <math.h>
#include "cminpak.h"

extern double dpmpar[];

void fdjac2(void f(int,int,double *,double *,int *),int m,
    int n,double x[],double fvec[],double **fjac,
	int *iflag,double epsfcn,double wa[])
{
	int i,j;
	double eps,epsmch,h,temp;

	epsmch = (epsfcn > dpmpar[0]) ? epsfcn : dpmpar[0];
	eps = sqrt(epsmch);

	for (j = 0;j < n; j++) {
		temp = x[j];
		if (temp == 0.0) h = eps;
                else h = eps * fabs(temp);
		x[j] = temp + h;
		f(m,n,x,wa,iflag);
		if (*iflag < 0) break;
		x[j] = temp;
		for (i = 0;i < m; i++)
			fjac[j][i] = (wa[i] - fvec[i]) / h;
	}
}
