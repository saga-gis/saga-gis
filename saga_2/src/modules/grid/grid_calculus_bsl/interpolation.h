#ifndef __INTERPOL_H 
#define __INTERPOL_H 


#include "grid_bsl.h"

double LinPol(double x0, double y0, double x1, double y1, double x);

class MathError
{
public:
	MathError(){};
};

class InterpolError: public MathError
{
public:
	InterpolError(const char *s) : S(s) {};	

	const char *S;
};

class Interpolation
{
public:
	Interpolation( GridWerte& I);
	virtual ~Interpolation() {};

	void setParams(double minx, double miny, double dx, int anzx, int anzy);
	virtual void interpol(GridWerte& G) = 0;
	void setZwischenwerte(void);
	void setDoppeltewerte(void);
	bool IsOk(void);
protected:
	 GridWerte& W;
	double MinX, MinY, DX;
	int AnzahlX, AnzahlY;
};

class QSpline : public Interpolation
{
public:
	QSpline( GridWerte& I) : Interpolation(I) {};
	~QSpline() {};

	virtual void interpol(GridWerte& G);

private:
	void spline(double x[],double y[],int n, double yp1,double ypn,double y2[], double tmp_u[]);
	void splint(double xa[],double ya[],double y2a[],int n, double x,double *y);
	void splie2(double x1a[], double x2a[], double **ya, int m, int n, double **y2a);
	void splin2(double x1a[], double x2a[], double **ya, double **y2a,
				int m, int n, double x1, double x2, double  *y,
				double *ytmp, double *yytmp, double *tmp_u);
	void mysplin(double x1a[], double x2a[], double **ya, double **y2a,
				  GridWerte& Erg, int mm, int nn,
				 double *ytmp, double *yytmp, double *tmp_u);
};

class Resample : public Interpolation
{
public:
	Resample( GridWerte& I) : Interpolation(I), Tiefe(4) {};
	~Resample() {};

	void setTiefe(int T);
	virtual void interpol(GridWerte& G);

private:
	double sinc(double x);

	int Tiefe;
};


#endif
