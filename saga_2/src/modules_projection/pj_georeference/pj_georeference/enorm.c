/* enorm.c -- compute euclidean norm of vector */
#include <math.h>
#include "cminpak.h"

double enorm(int n, double x[])
{
	int i;
	double sum;

    sum = x[0] * x[0];
    for (i=1;i<n;i++)
        sum += x[i]*x[i];
	return sqrt(sum);
}

/* compute row norm for column c from row r to row m */

double rownorm(int m, int r, int c, double **x)
{
	int i;
	double sum;

    sum = x[r][c] * x[r][c];
    for (i = r+1;i < m; i++)
                sum += x[i][c] * x[i][c];

	return sqrt(sum);
}

double colnorm(int m, int r, int c, double **x)
{
	int i;
	double sum;

    sum = x[r][c] * x[r][c];
    for (i = c+1;i < m; i++)
                sum += x[r][i] * x[r][i];

	return sqrt(sum);
}

