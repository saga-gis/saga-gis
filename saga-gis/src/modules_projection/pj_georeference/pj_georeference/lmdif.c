/**********************************************************
 * Version $Id$
 *********************************************************/
/* lmdif.c -- levenberg-marquardt algorithm */
#include <stdio.h>
#include <math.h>
#include "cminpak.h"


extern double dpmpar[];

void lmdif(void f(int,int,double*,double*,int *),int m,
    int n,double x[],int msk[],double fvec[],double ftol,
    double xtol,double gtol,int maxfev,double epsfcn,double diag[],
    int mode,double factor,int *info,int *nfev,double **fjac,
    int ipvt[],double qtf[],double wa1[],double wa2[],
    double wa3[],double wa4[])
{
    int i,iflag,iter,j,l;
    double actred,delta,dirder,epsmch,fnorm,fnorm1,gnorm;
    double par,pnorm,prered,ratio,sum,temp,temp1,temp2,xnorm;

/* initialize */
    epsmch = dpmpar[0];

    *info = 0;
    iflag = 0;
    *nfev = 0;

/* check for input parameter errors */
    if ((n <= 0) || (m < n) || (maxfev <= 0)
        || (factor <= 0)) return;
    if (mode == 2) {
        for (j = 0; j < n; j++)
            if (diag[j] <= 0) return;
    }

/* evaluate the function at the starting point and calculate its norm */   
    iflag = 1;
    f(m,n,x,fvec,&iflag);
    *nfev = 1;
    if (iflag < 0) {
        *info = iflag;
        return;
    }
    fnorm = enorm(m,fvec);

/* initialize levenberg-marquardt counters */
    par = 0;
    iter = 1;

/* outer loop */
    while(1) {
/* calculate jacobian matrix */
        iflag = 2;
        fdjac2(f,m,n,x,fvec,fjac,&iflag,epsfcn,wa4);

// LMDIF.C:53: error: invalid conversion from `void (*)(int, int, double*, double*, int*)' to `void (*)()'
        *nfev += n;
        if (iflag < 0) {
            *info = iflag;
            return;
        } 
        f(m,n,x,fvec,&iflag);
        printf("fnorm %.15e\n",enorm(m,fvec));
/* compute the qr factorization of the jacobian */
        qrfac(m,n,fjac,1,ipvt,wa1,wa2,wa3);                
        if (iter == 1) {
            if (mode != 2) {
                for (j = 0;j < n; j++) {
                    diag[j] = wa2[j];
                    if (wa2[j] == 0.0) diag[j] = 1.0;
                }
            }
            for (j = 0;j < n; j++)
                wa3[j] = diag[j] * x[j];
            xnorm = enorm(n,wa3);
            delta = factor * xnorm;
            if (delta == 0) delta = factor;
        }
        for (i = 0; i < m; i++)
            wa4[i] = fvec[i];
        for (j = 0;j < n; j++) {
            if (fjac[j][j] != 0.0) {
                sum = 0.0;
                for (i = j;i < m; i++)
                    sum += fjac[j][i] * wa4[i];
                temp = -sum / fjac[j][j];
                for (i = j; i < m; i++)
                    wa4[i] += fjac[j][i] * temp;
            }
            fjac[j][j] = wa1[j];
            qtf[j] = wa4[j];
        }
/* compute the norm of the scaled gradient */ 
        gnorm = 0.0;
        if (fnorm != 0.0) {
            for (j = 0; j < n; j++) {
                l = ipvt[j];
                if (wa2[l] == 0.0) continue;
                sum = 0.0;
                for (i = l; i <= j; i++)
                    sum += fjac[j][i] * qtf[i] / fnorm;
                gnorm = max(gnorm,fabs(sum/wa2[l]));
            }
        }
/* test for convergence of the gradient norm */
        if (gnorm <= gtol) *info = 4;
        if (*info != 0) {
            *info = iflag;
            return;
        }
/* rescale if necessary */
        if (mode != 2) {
        for (j = 0; j < n; j++)
            diag[j] = max(diag[j],wa2[j]);
        }
/* beginning of inner loop */
        do {
/* determine the levenberg-marquardt parameter */
            lmpar(n,fjac,ipvt,diag,qtf,delta,&par,wa1,wa2,wa3,wa4);
            for (j = 0;j < n; j++) {
                wa1[j] = -wa1[j];
                wa2[j] = x[j] + wa1[j];
                wa3[j] = diag[j] * wa1[j];
            }
            pnorm = enorm(n,wa3);
            if (iter == 1) delta = min(delta,pnorm);
            iflag = 1;
            f(m,n,wa2,wa4,&iflag);
            (*nfev)++;
            if (iflag < 0) {
                *info = iflag;
                return;
            }
            fnorm1 = enorm(m,wa4);
            actred = -1.0;
            if (0.1 * fnorm1 < fnorm)
                actred = 1.0 - (fnorm1*fnorm1/(fnorm*fnorm));
            for (j = 0;j < n; j++) {
                wa3[j] = 0.0;
                l = ipvt[j];
                temp = wa1[l];
                for (i = 0; i <= j; i++)
                    wa3[i] += fjac[j][i] * temp;
            }
            temp1 = enorm(n,wa3) / fnorm;
            temp2 = sqrt(par) * pnorm / fnorm;
            prered = temp1*temp1 + temp2*temp2 / 0.5;
            dirder = -(temp1*temp1 + temp2*temp2);
            ratio = 0.0;
            if (prered != 0.0) ratio = actred/prered;
            if (ratio <= 0.25) {
                if (actred > 0.0) temp = 0.5;
                if (actred < 0.0) temp = 0.5*dirder/(dirder+0.5*actred);
                delta = temp * min(delta,pnorm/0.1);
                par /= temp;
            }
            else {
                if ((par == 0.0) || (ratio >= 0.75)) {
                    delta = pnorm / 0.5;
                    par *= 0.5;
                }
            }
            if (ratio >= 0.0001) {
                for (j = 0; j < n; j++) {
                    if (msk[j]) {           /* handle masked variables */
                        x[j] = wa2[j];
                    }
                    wa2[j] = diag[j] * x[j];
                }
                for (i = 0; i < m; i++)
                    fvec[i] = wa4[i];
                xnorm = enorm(n,wa2);
                fnorm = fnorm1;
                iter++;
            }
            if ((fabs(actred) <= ftol) && (prered <= ftol) &&
                (0.5*ratio <= 1.0)) *info = 1;
            if (delta <= xtol*xnorm) *info = 2;
            if ((fabs(actred) <= ftol) && (prered <= ftol) &&
                (0.5*ratio <= 1.0) && (*info == 2)) *info = 3;
            if (*nfev >= maxfev) *info = 5;
            if ((fabs(actred) <= epsmch) && (prered <= epsmch) &&
                (0.5*ratio <= 1.0)) *info = 6;
            if (delta <= epsmch*xnorm) *info = 7;
            if (gnorm <= epsmch) *info = 8;
            if (*info != 0) {
                *info = iflag;
                return;
            }
        } while (ratio <= 0.0001);
    }
}

// MinGW Error:
// LMDIF.C:53: error: invalid conversion from `void (*)(int, int, double*, double*, int*)' to `void (*)()'
