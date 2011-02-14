/* qrsolv.c -- solve a system of equations */
#include <math.h>
#include "cminpak.h"

void qrsolv(int n,double **r,int ipvt[],double diag[],
    double qtb[],double x[],double sdiag[],double wa[])
{
    int i,j,jp1,k,kp1,l,nsing;
    double qtbpj,sum,temp,dsin,dcos,dtan,dcotan;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++)
            r[j][i] = r[i][j];
        x[j] = r[j][j];
        wa[j] = qtb[j];
    }
    for (j = 0; j < n; j++) {
        l = ipvt[j];
        if (diag[l] != 0.0) {
            for (k = j; k < n; k++)
                sdiag[k] = 0.0;
            sdiag[j] = diag[l];
            qtbpj = 0.0;
            for (k = j; k < n; k++) {
                if (sdiag[k] == 0.0) continue;
                if (fabs(r[k][k]) < fabs(sdiag[k])) {
                    dcotan = r[k][k] / sdiag[k];
                    dsin = 1.0 / sqrt(1.0 + dcotan * dcotan);
                    dcos = dsin * dcotan;
                }
                else {
                    dtan = sdiag[k] / r[k][k];
                    dcos = 1.0 / sqrt(1.0 + dtan * dtan);
                    dsin = dcos * dtan;
                }
                r[k][k] = dcos * r[k][k] + dsin * sdiag[k];
                temp = dcos * wa[k] + dsin * qtbpj;
                qtbpj = -dsin * wa[k] + dcos * qtbpj;
                wa[k] = temp;
                kp1 = k + 1;
                if (n <= kp1) continue;
                for (i = kp1; i < n; i++) {
                    temp = dcos * r[k][i] + dsin * sdiag[i];
                    sdiag[i] = -dsin * r[k][i] + dcos * sdiag[i];
                    r[k][i] = temp;
                }
            }
        }
        sdiag[j] = r[j][j];
        r[j][j] = x[j];
    }
    nsing = n;
    for (j = 0; j < n; j++) {
        if ((sdiag[j] == 0.0) && (nsing == n))
            nsing = j;
        if (nsing < n)
            wa[j] = 0.0;
    }
    if (nsing >= 1) {
        for (k = 0; k < nsing; k++) {
            j = nsing - k - 1;
            sum = 0.0;
            jp1 = j + 1;
            if (nsing > jp1) {
                for (i = jp1; i < nsing; i++)
                    sum += r[j][i] * wa[i];
            }
            wa[j] = (wa[j] - sum) / sdiag[j];
        }
    }
    for (j = 0; j < n; j++) {
        l = ipvt[j];
        x[l] = wa[j];
    }
}
                 
