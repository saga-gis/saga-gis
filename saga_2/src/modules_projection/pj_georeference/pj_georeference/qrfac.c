/* qrfac.c -- compute qr factorization of matrix. */
#include <math.h>
#include "cminpak.h"

extern double dpmpar[];
void qrfac(int m,int n,double **a,int pivot,int ipvt[],
    double rdiag[],double acnorm[],double wa[])
{
    int i,j,jp1,k,kmax,minmn;
    double ajnorm,epsmch,sum,temp;
	
/* get machine precision */
    epsmch = dpmpar[0];
/* compute the initial column norms and initialize several arrays */
    for (j = 0;j < n; j++) {
        acnorm[j] = colnorm(m,j,0,a);
        rdiag[j] = acnorm[j];
        wa[j] = rdiag[j];
        if (pivot) ipvt[j] = j;
    }
/* reduce a to r with householder transformations */
    minmn = (m < n) ? m : n;
    for (j = 0;j < minmn; j++) {
        if (pivot) {
/* bring column with largest norm into the pivot position */
            kmax = j;
            for  (k = j;k < n; k++)
                if (rdiag[k] > rdiag[kmax]) kmax = k;
            if (kmax != j) {
                for (i = 0;i < m; i++) {
                    temp = a[j][i];
                    a[j][i] = a[kmax][i];
                    a[kmax][i] = temp;
                }
                rdiag[kmax] = rdiag[j];
                wa[kmax] = wa[j];
                k = ipvt[j];
                ipvt[j] = ipvt[kmax];
                ipvt[kmax] = k;
            }
        }
/* compute the householder transformation */
        ajnorm = colnorm(m,j,j,a);
        if (ajnorm != 0.0) {
            if (a[j][j] < 0.0) ajnorm = -ajnorm;
            for (i = j;i < m; i++)
                a[j][i] /= ajnorm;
            a[j][j] += 1.0;
            jp1 = j + 1;
            if (n > jp1) {
                for (k = jp1;k < n; k++) {
                    sum = 0.0;
                    for (i = j;i < m; i++)
                        sum += a[j][i]*a[k][i];
                    temp = sum / a[j][j];
                    for (i = j; i < m; i++)
                        a[k][i] -=temp*a[j][i];
                    if (!pivot || !rdiag[k]) continue;
                    temp = a[k][j] / rdiag[k];
                    rdiag[k] *= sqrt(max(0.0,1.0-temp*temp));
                    if (0.5 * (rdiag[k] * rdiag[k] / (wa[k] * wa[k])) > epsmch) continue;
                    rdiag[k] = colnorm(m,k,jp1,a);
                    wa[k] = rdiag[k];
                }
            }
        }
        rdiag[j] = -ajnorm;
    }
}


