/**********************************************************
 * Version $Id$
 *********************************************************/
//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Shepard_H
#define HEADER_INCLUDED__Shepard_H

//---------------------------------------------------------
#ifndef min
	#define min(a, b)	((a) <= (b) ? (a) : (b))
#endif

#ifndef max
	#define max(a, b)	((a) >= (b) ? (a) : (b))
#endif

#define dmin(a, b)	(double)min(a, b)
#define dmax(a, b)	(double)max(a, b)


static int qshep2_(int *n, double *x, double *y, double *f, int *
            nq, int *nw, int *nr, int *lcell, int *lnext, double *
            xmin, double *ymin, double *dx, double *dy, double *rmax, double *rsq, double *a,
            int *ier);

static double qs2val_(double *px, double *py, int *n, double *x, double *y, double *f,
               int *nr, int *lcell, int *lnext, double *xmin, double *ymin,
               double *dx, double *dy, double *rmax, double *rsq, double *a);

class CShepard2d
{
public:
	       CShepard2d (void);
        ~CShepard2d (void);
        int Interpolate (double *X, double * Y, double * F, int N_Points, int Quadratic_Neighbors, int Weighting_Neighbors );
        void GetValue (double px, double py, double &q);
		void Set_Missing(double missing);
 
    protected:
        double *m_x, *m_y, *m_f;
        int *m_cells;
        int *m_next;
        double *m_rsq;
        double *m_a;
        double xmin, ymin, dx, dy, rmax;

        int m_nPoints;
        int m_nr;
        void Remove (void)
        {
            this->CShepard2d::~CShepard2d();
        }

        int GetGradient (double px, double py, double &q, double &qx, double &qy);
};


#endif // #ifndef HEADER_INCLUDED__Shepard_H
