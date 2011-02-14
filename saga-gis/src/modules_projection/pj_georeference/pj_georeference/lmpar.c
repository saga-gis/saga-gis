/**********************************************************
 * Version $Id$
 *********************************************************/
/* lmpar.c -- compute parameter for least squares solution */
#include <math.h>
#include "cminpak.h"
extern double dpmpar[];

void lmpar(int n,double **r,int ipvt[],double diag[],double qtb[],
    double delta,double *par,double x[],double sdiag[],double wa1[],
    double wa2[])
{
    int i,iter,j,jp1,k,l,nsing;
    double dxnorm,dwarf,fp,gnorm,parc,parl,paru;
    double sum,temp;

    dwarf = dpmpar[1];
    nsing = n;
    for (j = 0; j < n; j++) {
        wa1[j] = qtb[j];
        if ((r[j][j] == 0.0) && (nsing == n))
            nsing = j;
        if (nsing < n) wa1[j] = 0.0;
    }
    if (nsing >= 1) {
        for (k = 0;k < nsing; k++) {
            j = nsing - k - 1;
            wa1[j] /= r[j][j];
            temp = wa1[j];
            if (j < 1) continue;
            for (i = 0; i < j; i++)
                wa1[i] -= r[j][i] * temp;
        }
    }
    for (j = 0; j < n; j++) {
        l = ipvt[j];
        x[l] = wa1[j];
    }
    iter = 0;
    for (j = 0; j < n; j++)
        wa2[j] = diag[j] * x[j];
    dxnorm = enorm(n,wa2);
    fp = dxnorm - delta;
    if (fp <= 0.1*delta) {
        if (iter == 0)
            *par = 0.0;
        return;
    }
    parl = 0.0;
    if (nsing >= n) {
        for (j = 0; j < n; j++) {
            l = ipvt[j];
            wa1[j] = diag[l] * wa2[l] / dxnorm;
        }
        for (j = 0; j < n; j++) {
            sum = 0.0;
            if (j >= 1) {
                for (i = 0; i < j; i++)
                    sum += r[j][i] * wa1[i];
            }
            wa1[j] = (wa1[j] - sum) / r[j][j];
        }
        temp = enorm(n,wa1);
        parl = ((fp / delta) / temp) / temp;
    }
    for (j = 0;j < n; j++) {
	sum = 0.0;
        for (i = 0; i <= j; i++)
	    sum += r[j][i] * qtb[i];
	l = ipvt[j];
	wa1[j] = sum / diag[l];
    }
    gnorm = enorm(n,wa1);
    paru = gnorm / delta;
    if (paru == 0.0)
	paru = dwarf / min(delta,0.1);
    *par = max(*par,parl);
    *par = min(*par,paru);
    if (*par == 0.0)
	*par = gnorm / dxnorm;
    while (1) {
        iter++;
        if (*par == 0.0)
            *par = max(dwarf,0.001 * paru);
        temp = sqrt(*par);
        for (j = 0; j < n; j++)
            wa1[j] = temp * diag[j];
        qrsolv(n,r,ipvt,wa1,qtb,x,sdiag,wa2);
        for (j = 0; j < n; j++)
            wa2[j] = diag[j] * x[j];
        dxnorm = enorm(n,wa2);
        temp = fp;
        fp = dxnorm - delta;

        if ((fabs(fp) <= 0.1*delta) || (parl == 0.0) && (fp <= temp)
            && (temp > 0.0) || iter == 10) {
            if (iter == 0)
                *par = 0.0;
            return;
        }
        for (j = 0; j < n; j++) {
            l = ipvt[j];
            wa1[j] = diag[l] * wa2[l] / dxnorm;
        }
        for (j = 0; j < n; j++) {
            wa1[j] /= sdiag[j];
            temp = wa1[j];
            jp1 = j + 1;
            if (jp1 < n)
                for (i = jp1; i < n; i++)
                    wa1[i] -= r[j][i] * temp;
        }
        temp = enorm(n,wa1);
        parc = ((fp/delta) / temp) / temp;
        if (fp > 0.0)
            parl = max(parl,*par);
        if (fp < 0.0)
            paru = min(paru,*par);
        *par = max(parl,*par+parc);
    }
}
			 
