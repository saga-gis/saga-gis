/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Shepard.cpp                       //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Andre Ringeler                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


#include <math.h>
#include <stdlib.h>
#include "Shepard.h"

double _missing_;

CShepard2d::CShepard2d (void)
{
    m_cells = NULL;
    m_next = NULL;
    m_rsq = NULL;
    m_a = NULL;
	_missing_ = -9999.99;
}

CShepard2d::~CShepard2d (void)
{
    if (m_cells)
    {
        free (m_cells);
        m_cells = NULL;
    }
    if (m_next)
    {
        free (m_next);
        m_next = NULL;
    }
    if (m_rsq)
    {
        free (m_rsq);
        m_rsq = NULL;
    }
    if (m_a)
    {
        free (m_a);
        m_a = NULL;
    }
}

void CShepard2d::Set_Missing(double missing)
{
	_missing_ = missing;
}

int CShepard2d::Interpolate (double *X, double * Y, double * F, int N_Points, int Quadratic_Neighbors, int Weighting_Neighbors )
{

    int nr, lmax;
	int status;

    if (N_Points < 6)
        return -1;

    lmax = min(40, N_Points - 1);
    if (Quadratic_Neighbors < 5 || Quadratic_Neighbors > lmax)
        return -1;
    if (Weighting_Neighbors < 1 || Weighting_Neighbors > lmax)
        return -1;

    nr = (int)(sqrt(N_Points / 3.0));
    if (nr < 1)
        nr = 1;

    this->CShepard2d::~CShepard2d();

    m_cells = (int *) malloc(nr * nr * sizeof(int));
    m_next = (int *) malloc(N_Points * sizeof(int));

    m_rsq = (double *) malloc(N_Points * sizeof(double));

    m_a = (double *) malloc(N_Points * 5 * sizeof(double));

    m_x = X;
    m_y = Y;
    m_f = F;

    m_nPoints = N_Points;
    m_nr = nr;

     qshep2_(&N_Points, X, Y, F, &Quadratic_Neighbors, &Weighting_Neighbors, &nr, m_cells, m_next, &xmin, &ymin, &dx, &dy, &rmax, m_rsq, m_a, &status);

	return status;
}

void CShepard2d::GetValue(double px, double py, double &q)
{
    if ( ! m_a )
		q = _missing_;
	else
        q = qs2val_(&px, &py, &m_nPoints , m_x, m_y, m_f, &m_nr, m_cells, m_next, &xmin, &ymin, &dx, &dy, &rmax, m_rsq, m_a);
}

/*/////////////////////////////////////////////////////////////////////////////////////
The folowing source ist an automatic translation 
with f2c of Module 660 in TOMS
 
QSHEP2D: Fortran routines implementing the quadratic Shepard method for
bivariate interpolation of scattered data. (See R. J. Renka, ACM TOMS 14 (1988)
pp. 149-150.).
 
Classes  :  E2b . Interpolation of scattered, non-gridded multivariate data
 
Type     : Fortran software in TOMS collection.
Access   : Some uses prohibited. Portable.
Precision: Single.
 
Details  : Fullsource
Sites    : (1) NETLIB
//////////////////////////////////////////////////////////////////////////////////////*/ 



struct
{
    double y;
}
stcom_;

#define stcom_1 stcom_

/* Table of constant values */

static int c__1 = 1;



int qshep2_(int *n,   double *x,   double *y,   double *f,   int *
                             nq, int *nw, int *nr, int *lcell, int *lnext, double *
                             xmin, double *ymin, double *dx, double *dy, double *rmax, double *rsq, double *a,
                             int *ier)
{
  
    static double rtol = 1e-5f;
    static double dtol = .01f;
    static double sf = 1.f;

    int lcell_dim1, lcell_offset, i__1, i__2, i__3;
    double r__1, r__2;

//    double sqrt(double);

    static double b[36]	/* was[6][6] */, c__;
    static int i__, j, k;
    static double s, t;
    static int ib;
    static double fk, av;
    static int nn, np;
    static double rq, xk, rs, yk;
    static int ip1, jp1;
    static double ddx, ddy;
    static int neq, lnp, nnq, nnr, nnw;
    static double xmn, sum, ymn, rws;
    static int irm1;
    static double dmin__;
    static int ierr, lmax;
    static double avsq;
    static int irow, npts[40];
    static double rsmx, rsold;
    extern int getnp2_(double *, double *, double *, double *,
                                            int *, int *, int *, double *, double *, double *, double *,
                                            int *, double *), store2_(int *, double *, double *, int *,
                                                                        int *, int *, double *, double *, double *, double *, int *),
        setup2_(double *, double *, double *, double *, double *, double *, double *,
                double *, double *, double *), givens_(double *, double *, double *, double *),
        rotate_(int *, double *, double *, double *, double *);
    static int nqwmax;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
    /* 						    01/08/90							*/
	/*																*/
    /*   THIS SUBROUTINE COMPUTES A SET OF PARAMETERS A AND RSQ		*/
    /* DEFINING A SMOOTH(ONCE CONTINUOUSLY DIFFERENTIABLE) BI-		*/
    /* VARIATE FUNCTION Q(X, Y) WHICH INTERPOLATES DATA VALUES F	*/
    /* AT SCATTERED NODES(X, Y).  THE INTERPOLANT Q MAY BE EVAL-	*/
    /* UATED AT AN ARBITRARY POINT BY FUNCTION QS2VAL, AND ITS		*/
    /* FIRST DERIVATIVES ARE COMPUTED BY SUBROUTINE QS2GRD.			*/
    /*   THE INTERPOLATION SCHEME IS A MODIFIED QUADRATIC SHEPARD	*/
    /* METHOD --													*/
	/*																*/
    /* Q =(W(1)*Q(1) + W(2)*Q(2)+..+W(N)*Q(N))/(W(1) + W(2)+..+W(N))*/
	/*																*/
    /* FOR BIVARIATE FUNCTIONS W(K) AND Q(K).  THE NODAL FUNC-		*/
    /* TIONS ARE GIVEN BY											*/
	/*																*/
    /* Q(K)(X, Y) = A(1, K)*(X - X(K))**2							*/
	/*		+ A(2, K)*(X - X(K))*(Y - Y(K))							*/
    /* 	    + A(3, K)*(Y - Y(K))**2 + A(4, K)*(X - X(K))			*/
    /* 	    + A(5, K)*(Y - Y(K))	 + F(K) .						*/
	/*																*/
    /* THUS, Q(K) IS A QUADRATIC FUNCTION WHICH INTERPOLATES THE	*/
    /* DATA VALUE AT NODE K.  ITS COEFFICIENTS A(, K) ARE OBTAINED	*/
    /* BY A WEIGHTED LEAST SQUARES FIT TO THE CLOSEST NQ DATA		*/
    /* POINTS WITH WEIGHTS SIMILAR TO W(K).	NOTE THAT THE RADIUS	*/
    /* OF INFLUENCE FOR THE LEAST SQUARES FIT IS FIXED FOR EACH		*/
    /* K, BUT VARIES WITH K.										*/
    /*   THE WEIGHTS ARE TAKEN TO BE								*/
	/*																*/
    /* W(K)(X, Y) =((R(K) - D(K))+ / R(K)*D(K))**2					*/
	/*																*/
    /* WHERE(R(K) - D(K))+ = 0 IF R(K) .LE. D(K) AND D(K)(X, Y) IS	*/
    /* THE EUCLIDEAN DISTANCE BETWEEN(X, Y) AND(X(K), Y(K)).  THE	*/
    /* RADIUS OF INFLUENCE R(K) VARIES WITH K AND IS CHOSEN SO		*/
    /* THAT NW NODES ARE WITHIN THE RADIUS.	NOTE THAT W(K) IS		*/
    /* NOT DEFINED AT NODE(X(K), Y(K)), BUT Q(X, Y) HAS LIMIT F(K)	*/
    /* AS(X, Y) APPROACHES(X(K), Y(K)).								*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	N = NUMBER OF NODES AND ASSOCIATED DATA VALUES.				*/
    /* 	    N .GE. 6.												*/
	/*																*/
    /* 	X, Y = ARRAYS OF LENGTH N CONTAINING THE CARTESIAN			*/
    /* 	      COORDINATES OF THE NODES.								*/
	/*																*/
    /* 	F = ARRAY OF LENGTH N CONTAINING THE DATA VALUES			*/
    /* 	    IN ONE - TO - ONE CORRESPONDENCE WITH THE NODES.		*/
	/*																*/
    /* 	NQ = NUMBER OF DATA POINTS TO BE USED IN THE LEAST			*/
    /* 	     SQUARES FIT FOR COEFFICIENTS DEFINING THE NODAL		*/
    /* 	     FUNCTIONS Q(K).  A HIGHLY RECOMMENDED VALUE IS			*/
    /* 	     NQ = 13.  5 .LE. NQ .LE. MIN(40, N - 1).				*/
	/*																*/
    /* 	NW = NUMBER OF NODES WITHIN(AND DEFINING) THE RADII			*/
    /* 	     OF INFLUENCE R(K) WHICH ENTER INTO THE WEIGHTS			*/
    /* 	     W(K).  FOR N SUFFICIENTLY LARGE, A RECOMMENDED			*/
    /* 	     VALUE IS NW = 19.	1 .LE. NW .LE. MIN(40, N - 1).		*/
	/*																*/
    /* 	NR = NUMBER OF ROWS AND COLUMNS IN THE CELL GRID DE-		*/
    /* 	     FINED IN SUBROUTINE STORE2.  A RECTANGLE CON-			*/
    /* 	     TAINING THE NODES IS PARTITIONED INTO CELLS IN			*/
    /* 	     ORDER TO INCREASE SEARCH EFFICIENCY.  NR =				*/
    /* 	     SQRT(N/3) IS RECOMMENDED.	NR .GE. 1.					*/
	/*																*/
    /* THE ABOVE PARAMETERS ARE NOT ALTERED BY THIS ROUTINE.		*/
	/*																*/
    /* 	LCELL = ARRAY OF LENGTH .GE. NR**2.							*/
	/*																*/
    /* 	LNEXT = ARRAY OF LENGTH .GE. N.								*/
	/*																*/
    /* 	RSQ = ARRAY OF LENGTH .GE. N.								*/
	/*																*/
    /* 	A = ARRAY OF LENGTH .GE. 5N.								*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	LCELL = NR BY NR ARRAY OF NODAL INDICES ASSOCIATED			*/
    /* 		WITH CELLS.  REFER TO STORE2.							*/
	/*																*/
    /* 	LNEXT = ARRAY OF LENGTH N CONTAINING NEXT - NODE INDI-		*/
    /* 		CES.  REFER TO STORE2.									*/
	/*																*/
    /* 	XMIN, YMIN, DX, DY = MINIMUM NODAL COORDINATES AND CELL		*/
    /* 			  DIMENSIONS.  REFER TO STORE2.						*/
	/*																*/
    /* 	RMAX = SQUARE ROOT OF THE LARGEST ELEMENT IN RSQ --			*/
    /* 	       MAXIMUM RADIUS R(K).									*/
	/*																*/
    /* 	RSQ = ARRAY CONTAINING THE SQUARES OF THE RADII R(K)		*/
    /* 	      WHICH ENTER INTO THE WEIGHTS W(K).					*/
	/*																*/
    /* 	A = 5 BY N ARRAY CONTAINING THE COEFFICIENTS FOR			*/
    /* 	    QUADRATIC NODAL FUNCTION Q(K) IN COLUMN K.				*/
	/*																*/
    /*   NOTE THAT THE ABOVE OUTPUT PARAMETERS ARE NOT DEFINED		*/
    /* UNLESS IER = 0.												*/
	/*																*/
    /* 	IER = ERROR INDICATOR --									*/
    /* 	      IER = 0 IF NO ERRORS WERE ENCOUNTERED.				*/
    /* 	      IER = 1 IF N, NQ, NW, OR NR IS OUT OF RANGE.			*/
    /* 	      IER = 2 IF DUPLICATE NODES WERE ENCOUNTERED.			*/
    /* 	      IER = 3 IF ALL NODES ARE COLLINEAR.					*/
	/*																*/
    /* MODULES REQUIRED BY QSHEP2 -- GETNP2, GIVENS, ROTATE,		*/
    /* 				  SETUP2, STORE2								*/
	/*																*/
    /* INTRINSIC FUNCTIONS CALLED BY QSHEP2 -- ABS, AMIN1, FLOAT,	*/
    /* 					    MAX0, MIN0, SQRT						*/
	/*																*/
    /****************************************************************/

    a -= 6;
    --rsq;
    --lnext;
    --f;
    --y;
    --x;
    lcell_dim1 = *nr;
    lcell_offset = 1 + lcell_dim1;
    lcell -= lcell_offset;

    /* LOCAL PARAMETERS --											*/
	/*																*/
    /* AV =	       ROOT - MEAN - SQUARE DISTANCE BETWEEN K AND THE	*/
    /* 		 NODES IN THE LEAST SQUARES SYSTEM(UNLESS				*/
    /* 		 ADDITIONAL NODES ARE INTRODUCED FOR STABIL-			*/
    /* 		 ITY).	THE FIRST 3 COLUMNS OF THE MATRIX				*/
    /* 		 ARE SCALED BY 1/AVSQ, THE LAST 2 BY 1/AV				*/
    /* AVSQ =       AV*AV											*/
    /* B =	       TRANSPOSE OF THE AUGMENTED REGRESSION MATRIX		*/
    /* C =	       FIRST COMPONENT OF THE PLANE ROTATION USED TO	*/
    /* 		 ZERO THE LOWER TRIANGLE OF B**T -- COMPUTED			*/
    /* 		 BY SUBROUTINE GIVENS									*/
    /* DDX, DDY =    LOCAL VARIABLES FOR DX AND DY					*/
    /* DMIN =       MINIMUM OF THE MAGNITUDES OF THE DIAGONAL		*/
    /* 		 ELEMENTS OF THE REGRESSION MATRIX AFTER				*/
    /* 		 ZEROS ARE INTRODUCED BELOW THE DIAGONAL				*/
    /* DTOL =       TOLERANCE FOR DETECTING AN ILL - CONDITIONED	*/
    /* 		 SYSTEM.  THE SYSTEM IS ACCEPTED WHEN DMIN				*/
    /* 		 .GE. DTOL												*/
    /* FK =	       DATA VALUE AT NODE K -- F(K)						*/
    /* I =	       INDEX FOR A, B, AND NPTS							*/
    /* IB =	       DO - LOOP INDEX FOR BACK SOLVE					*/
    /* IERR =       ERROR FLAG FOR THE CALL TO STORE2				*/
    /* IP1 =        I + 1											*/
    /* IRM1 =       IROW - 1										*/
    /* IROW =       ROW INDEX FOR B									*/
    /* J =	       INDEX FOR A AND B								*/
    /* JP1 =        J + 1											*/
    /* K =	       NODAL FUNCTION INDEX AND COLUMN INDEX FOR A		*/
    /* LMAX =       MAXIMUM NUMBER OF NPTS ELEMENTS(MUST BE CON-	*/
    /* 		 SISTENT WITH THE DIMENSION STATEMENT ABOVE)			*/
    /* LNP =        CURRENT LENGTH OF NPTS							*/
    /* NEQ =        NUMBER OF EQUATIONS IN THE LEAST SQUARES FIT	*/
    /* NN, NNQ, NNR = LOCAL COPIES OF N, NQ, AND NR					*/
    /* NNW =        LOCAL COPY OF NW								*/
    /* NP =	       NPTS ELEMENT										*/
    /* NPTS =       ARRAY CONTAINING THE INDICES OF A SEQUENCE OF	*/
    /* 		 NODES TO BE USED IN THE LEAST SQUARES FIT				*/
    /* 		 OR TO COMPUTE RSQ.  THE NODES ARE ORDERED				*/
    /* 		 BY DISTANCE FROM K AND THE LAST ELEMENT				*/
    /*(USUALLY INDEXED BY LNP) IS USED ONLY TO						*/
    /* 		 DETERMINE RQ, OR RSQ(K) IF NW .GT. NQ					*/
    /* NQWMAX =     MAX(NQ, NW)										*/
    /* RQ =	       RADIUS OF INFLUENCE WHICH ENTERS INTO THE		*/
    /* 		 WEIGHTS FOR Q(K)(SEE SUBROUTINE SETUP2)				*/
    /* RS =	       SQUARED DISTANCE BETWEEN K AND NPTS(LNP) --		*/
    /* 		 USED TO COMPUTE RQ AND RSQ(K)							*/
    /* RSMX =       MAXIMUM RSQ ELEMENT ENCOUNTERED					*/
    /* RSOLD =      SQUARED DISTANCE BETWEEN K AND NPTS(LNP - 1) -- */
    /* 		 USED TO COMPUTE A RELATIVE CHANGE IN RS				*/
    /* 		 BETWEEN SUCCEEDING NPTS ELEMENTS						*/
    /* RTOL =       TOLERANCE FOR DETECTING A SUFFICIENTLY LARGE	*/
    /* 		 RELATIVE CHANGE IN RS.  IF THE CHANGE IS				*/
    /* 		 NOT GREATER THAN RTOL, THE NODES ARE					*/
    /* 		 TREATED AS BEING THE SAME DISTANCE FROM K				*/
    /* RWS =        CURRENT VALUE OF RSQ(K)							*/
    /* S =	       SECOND COMPONENT OF THE PLANE GIVENS ROTATION	*/
    /* SF =	       MARQUARDT STABILIZATION FACTOR USED TO DAMP		*/
    /* 		 OUT THE FIRST 3 SOLUTION COMPONENTS(SECOND				*/
    /* 		 PARTIALS OF THE QUADRATIC) WHEN THE SYSTEM				*/
    /* 		 IS ILL - CONDITIONED.  AS SF INCREASES, THE			*/
    /* 		 FITTING FUNCTION APPROACHES A LINEAR					*/
    /* SUM =        SUM OF SQUARED EUCLIDEAN DISTANCES BETWEEN		*/
    /* 		 NODE K AND THE NODES USED IN THE LEAST					*/
    /* 		 SQUARES FIT(UNLESS ADDITIONAL NODES ARE				*/
    /* 		 ADDED FOR STABILITY)									*/
    /* T =	       TEMPORARY VARIABLE FOR ACCUMULATING A SCALAR		*/
    /* 		 PRODUCT IN THE BACK SOLVE								*/
    /* XK, YK =      COORDINATES OF NODE K -- X(K), Y(K)			*/
    /* XMN, YMN =    LOCAL VARIABLES FOR XMIN AND YMIN				*/

    nn = *n;
    nnq = *nq;
    nnw = *nw;
    nnr = *nr;
    nqwmax = max(nnq, nnw);
    /* Computing MIN */
    i__1 = 40, i__2 = nn - 1;
    lmax = min(i__1, i__2);
    if (5 > nnq || 1 > nnw || nqwmax > lmax || nnr < 1)
    {
        goto L20;
    }

    /* CREATE THE CELL DATA STRUCTURE, AND INITIALIZE RSMX. */

    store2_(&nn, &x[1], &y[1], &nnr, &lcell[lcell_offset], &lnext[1], &xmn, &
            ymn, &ddx, &ddy, &ierr);
    if (ierr != 0)
    {
        goto L22;
    }
    rsmx = 0.f;

    /* OUTER LOOP ON NODE K */

    i__1 = nn;
    for (k = 1; k <= i__1; ++k)
    {
        xk = x[k];
        yk = y[k];
        fk = f[k];

        /* MARK NODE K TO EXCLUDE IT FROM THE SEARCH FOR NEAREST */
        /*   NEIGHBORS. */

        lnext[k] = -lnext[k];

        /* INITIALIZE FOR LOOP ON NPTS. */

        rs = 0.f;
        sum = 0.f;
        rws = 0.f;
        rq = 0.f;
        lnp = 0;

        /* COMPUTE NPTS, LNP, RWS, NEQ, RQ, AND AVSQ. */

    L1:
        sum += rs;
        if (lnp == lmax)
        {
            goto L3;
        }
        ++lnp;
        rsold = rs;
        getnp2_(&xk, &yk, &x[1], &y[1], &nnr, &lcell[lcell_offset], &lnext[1],
                &xmn, &ymn, &ddx, &ddy, &np, &rs);
        if (rs == 0.f)
        {
            goto L21;
        }
        npts[lnp - 1] = np;
        if ((rs - rsold) / rs < rtol)
        {
            goto L1;
        }
        if (rws == 0.f && lnp > nnw)
        {
            rws = rs;
        }
        if (rq != 0.f || lnp <= nnq)
        {
            goto L2;
        }

        /*   RQ = 0(NOT YET COMPUTED) AND LNP .GT. NQ.	RQ = */
        /*     SQRT(RS) IS SUFFICIENTLY LARGE TO(STRICTLY) INCLUDE */
        /*     NQ NODES.  THE LEAST SQUARES FIT WILL INCLUDE NEQ = */
        /*     LNP - 1 EQUATIONS FOR 5 .LE. NQ .LE. NEQ .LT. LMAX */
        /*     .LE. N - 1. */

        neq = lnp - 1;
        rq = sqrt(rs);
        avsq = sum / (double) neq;

        /*   BOTTOM OF LOOP -- TEST FOR TERMINATION. */

    L2:
        if (lnp > nqwmax)
        {
            goto L4;
        }
        goto L1;

        /* ALL LMAX NODES ARE INCLUDED IN NPTS.	RWS AND/OR RQ**2 IS */
        /*(ARBITRARILY) TAKEN TO BE 10 PERCENT LARGER THAN THE */
        /*   DISTANCE RS TO THE LAST NODE INCLUDED. */

    L3:
        if (rws == 0.f)
        {
            rws = rs * 1.1f;
        }
        if (rq != 0.f)
        {
            goto L4;
        }
        neq = lmax;
        rq = sqrt(rs * 1.1f);
        avsq = sum / (double) neq;

        /* STORE RSQ(K), UPDATE RSMX IF NECESSARY, AND COMPUTE AV. */

    L4:
        rsq[k] = rws;
        if (rws > rsmx)
        {
            rsmx = rws;
        }
        av = sqrt(avsq);

        /* SET UP THE AUGMENTED REGRESSION MATRIX(TRANSPOSED) AS THE */
        /*   COLUMNS OF B, AND ZERO OUT THE LOWER TRIANGLE(UPPER */
        /*   TRIANGLE OF B) WITH GIVENS ROTATIONS -- QR DECOMPOSITION */
        /*   WITH ORTHOGONAL MATRIX Q NOT STORED. */

        i__ = 0;
    L5:
        ++i__;
        np = npts[i__ - 1];
        irow = min(i__, 6);
        setup2_(&xk, &yk, &fk, &x[np], &y[np], &f[np], &av, &avsq, &rq, &b[
                    irow * 6 - 6]);
        if (i__ == 1)
        {
            goto L5;
        }
        irm1 = irow - 1;
        i__2 = irm1;
        for (j = 1; j <= i__2; ++j)
        {
            jp1 = j + 1;
            givens_(&b[j + j * 6 - 7], &b[j + irow * 6 - 7], &c__, &s);
            /* L6: */
            i__3 = 6 - j;
            rotate_(&i__3, &c__, &s, &b[jp1 + j * 6 - 7], &b[jp1 + irow * 6 -
                    7]);
        }
        if (i__ < neq)
        {
            goto L5;
        }

        /* TEST THE SYSTEM FOR ILL - CONDITIONING. */

        /* Computing MIN */
        r__1 = dabs(b[0]), r__2 = dabs(b[7]), r__1 = min(r__1, r__2), r__2 =
                                      dabs(b[14]), r__1 = min(r__1, r__2), r__2 = dabs(b[21]), r__1 =
                                                              min(r__1, r__2), r__2 = dabs(b[28]);
        dmin__ = dmin(r__1, r__2);
        if (dmin__ * rq >= dtol)
        {
            goto L13;
        }
        if (neq == lmax)
        {
            goto L10;
        }

        /* INCREASE RQ AND ADD ANOTHER EQUATION TO THE SYSTEM TO */
        /*   IMPROVE THE CONDITIONING.  THE NUMBER OF NPTS ELEMENTS */
        /*   IS ALSO INCREASED IF NECESSARY. */

    L7:
        rsold = rs;
        ++neq;
        if (neq == lmax)
        {
            goto L9;
        }
        if (neq == lnp)
        {
            goto L8;
        }

        /*   NEQ .LT. LNP */

        np = npts[neq];
        /* Computing 2nd power */
        r__1 = x[np] - xk;
        /* Computing 2nd power */
        r__2 = y[np] - yk;
        rs = r__1 * r__1 + r__2 * r__2;
        if ((rs - rsold) / rs < rtol)
        {
            goto L7;
        }
        rq = sqrt(rs);
        goto L5;

        /*   ADD AN ELEMENT TO NPTS. */

    L8:
        ++lnp;
        getnp2_(&xk, &yk, &x[1], &y[1], &nnr, &lcell[lcell_offset], &lnext[1],
                &xmn, &ymn, &ddx, &ddy, &np, &rs);
        if (np == 0)
        {
            goto L21;
        }
        npts[lnp - 1] = np;
        if ((rs - rsold) / rs < rtol)
        {
            goto L7;
        }
        rq = sqrt(rs);
        goto L5;

    L9:
        rq = sqrt(rs * 1.1f);
        goto L5;

        /* STABILIZE THE SYSTEM BY DAMPING SECOND PARTIALS -- ADD */
        /*   MULTIPLES OF THE FIRST THREE UNIT VECTORS TO THE FIRST */
        /*   THREE EQUATIONS. */

    L10:
        for (i__ = 1; i__ <= 3; ++i__)
        {
            b[i__ + 29] = sf;
            ip1 = i__ + 1;
            for (j = ip1; j <= 6; ++j)
            {
                /* L11: */
                b[j + 29] = 0.f;
            }
            for (j = i__; j <= 5; ++j)
            {
                jp1 = j + 1;
                givens_(&b[j + j * 6 - 7], &b[j + 29], &c__, &s);
                /* L12: */
                i__3 = 6 - j;
                rotate_(&i__3, &c__, &s, &b[jp1 + j * 6 - 7], &b[jp1 + 29]);
            }
        }

        /* TEST THE STABILIZED SYSTEM FOR ILL - CONDITIONING. */

        /* Computing MIN */
        r__1 = dabs(b[0]), r__2 = dabs(b[7]), r__1 = min(r__1, r__2), r__2 =
                                      dabs(b[14]), r__1 = min(r__1, r__2), r__2 = dabs(b[21]), r__1 =
                                                              min(r__1, r__2), r__2 = dabs(b[28]);
        dmin__ = dmin(r__1, r__2);
        if (dmin__ * rq < dtol)
        {
            goto L22;
        }

        /* SOLVE THE 5 BY 5 TRIANGULAR SYSTEM FOR THE COEFFICIENTS */

    L13:
        for (ib = 1; ib <= 5; ++ib)
        {
            i__ = 6 - ib;
            t = 0.f;
            if (i__ == 5)
            {
                goto L15;
            }
            ip1 = i__ + 1;
            for (j = ip1; j <= 5; ++j)
            {
                /* L14: */
                t += b[j + i__ * 6 - 7] * a[j + k * 5];
            }
        L15:
            a[i__ + k * 5] = (b[i__ * 6 - 1] - t) / b[i__ + i__ * 6 - 7];
        }

        /* SCALE THE COEFFICIENTS TO ADJUST FOR THE COLUMN SCALING. */

        for (i__ = 1; i__ <= 3; ++i__)
        {
            /* L16: */
            a[i__ + k * 5] /= avsq;
        }
        a[k * 5 + 4] /= av;
        a[k * 5 + 5] /= av;

        /* UNMARK K AND THE ELEMENTS OF NPTS. */

        lnext[k] = -lnext[k];
        i__3 = lnp;
        for (i__ = 1; i__ <= i__3; ++i__)
        {
            np = npts[i__ - 1];
            /* L17: */
            lnext[np] = -lnext[np];
        }
        /* L18: */
    }

    /* NO ERRORS ENCOUNTERED. */

    *xmin = xmn;
    *ymin = ymn;
    *dx = ddx;
    *dy = ddy;
    *rmax = sqrt(rsmx);
    *ier = 0;
    return 0;

    /* N, NQ, NW, OR NR IS OUT OF RANGE. */

L20:
    *ier = 1;
    return 0;

    /* DUPLICATE NODES WERE ENCOUNTERED BY GETNP2. */

L21:
    *ier = 2;
    return 0;

    /* NO UNIQUE SOLUTION DUE TO COLLINEAR NODES. */

L22:
    *xmin = xmn;
    *ymin = ymn;
    *dx = ddx;
    *dy = ddy;
    *ier = 3;
    return 0;
} /* qshep2_ */

double qs2val_(double *px, double *py, int *n, double *x, double *y, double *f,
                   int *nr, int *lcell, int *lnext, double *xmin, double *ymin,
                   double *dx, double *dy, double *rmax, double *rsq, double *a)
{
    int lcell_dim1, lcell_offset, i__1, i__2;
    double ret_val;

//    double sqrt(double);

    static int i__, j, k;
    static double w, rd, ds;
    static int kp;
    static double rs, xp, yp, sw, rds, swq;
    static int imin, jmin, imax, jmax;
    static double delx, dely, dxsq, dysq;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
    /* 						    10/28/87							*/
	/*																*/
    /*   THIS FUNCTION RETURNS THE VALUE Q(PX, PY) WHERE Q IS THE	*/
    /* WEIGHTED SUM OF QUADRATIC NODAL FUNCTIONS DEFINED IN SUB-	*/
    /* ROUTINE QSHEP2.  QS2GRD MAY BE CALLED TO COMPUTE A GRADI-	*/
    /* ENT OF Q ALONG WITH THE VALUE, AND/OR TO TEST FOR ERRORS.	*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	PX, PY = CARTESIAN COORDINATES OF THE POINT P AT			*/
    /* 		WHICH Q IS TO BE EVALUATED.								*/
	/*																*/
    /* 	N = NUMBER OF NODES AND DATA VALUES DEFINING Q.				*/
    /* 	    N .GE. 6.												*/
	/*																*/
    /* 	X, Y, F = ARRAYS OF LENGTH N CONTAINING THE NODES AND		*/
    /* 		DATA VALUES INTERPOLATED BY Q.							*/
	/*																*/
    /* 	NR = NUMBER OF ROWS AND COLUMNS IN THE CELL GRID.			*/
    /* 	     REFER TO STORE2.  NR .GE. 1.							*/
	/*																*/
    /* 	LCELL = NR BY NR ARRAY OF NODAL INDICES ASSOCIATED			*/
    /* 		WITH CELLS.  REFER TO STORE2.							*/
	/*																*/
    /* 	LNEXT = ARRAY OF LENGTH N CONTAINING NEXT - NODE INDI-		*/
    /* 		CES.  REFER TO STORE2.									*/
	/*																*/
    /* 	XMIN, YMIN, DX, DY = MINIMUM NODAL COORDINATES AND CELL		*/
    /* 			  DIMENSIONS.  DX AND DY MUST BE					*/
    /* 			  POSITIVE.  REFER TO STORE2.						*/
	/*																*/
    /* 	RMAX = SQUARE ROOT OF THE LARGEST ELEMENT IN RSQ --			*/
    /* 	       MAXIMUM RADIUS.										*/
	/*																*/
    /* 	RSQ = ARRAY OF LENGTH N CONTAINING THE SQUARED RADII		*/
    /* 	      WHICH ENTER INTO THE WEIGHTS DEFINING Q.				*/
	/*																*/
    /* 	A = 5 BY N ARRAY CONTAINING THE COEFFICIENTS FOR THE		*/
    /* 	    NODAL FUNCTIONS DEFINING Q.								*/
	/*																*/
    /*   INPUT PARAMETERS ARE NOT ALTERED BY THIS FUNCTION.	THE		*/
    /* PARAMETERS OTHER THAN PX AND PY SHOULD BE INPUT UNALTERED	*/
    /* FROM THEIR VALUES ON OUTPUT FROM QSHEP2.  THIS FUNCTION		*/
    /* SHOULD NOT BE CALLED IF A NONZERO ERROR FLAG WAS RETURNED	*/
    /* BY QSHEP2.													*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	QS2VAL = FUNCTION VALUE Q(PX, PY) UNLESS N, NR, DX,			*/
    /* 		 DY, OR RMAX IS INVALID, IN WHICH CASE NO				*/
    /* 		 VALUE IS RETURNED.										*/
	/*																*/
    /* MODULES REQUIRED BY QS2VAL -- NONE							*/
	/*																*/
    /* INTRINSIC FUNCTIONS CALLED BY QS2VAL -- IFIX, SQRT			*/
	/*																*/
    /****************************************************************/

    /* Parameter adjustments										*/
    a -= 6;
    --rsq;
    --lnext;
    --f;
    --y;
    --x;
    lcell_dim1 = *nr;
    lcell_offset = 1 + lcell_dim1;
    lcell -= lcell_offset;

    /* Function Body */
    xp = *px;
    yp = *py;
    if (*n < 6 || *nr < 1 || *dx <= 0.f || *dy <= 0.f || *rmax < 0.f)
    {
        return _missing_;
    }

    /* SET IMIN, IMAX, JMIN, AND JMAX TO CELL INDICES DEFINING */
    /*   THE RANGE OF THE SEARCH FOR NODES WHOSE RADII INCLUDE */
    /*   P.	THE CELLS WHICH MUST BE SEARCHED ARE THOSE INTER- */
    /*   SECTED BY(OR CONTAINED IN) A CIRCLE OF RADIUS RMAX */
    /*   CENTERED AT P. */

    imin = (int)((xp - *xmin - *rmax) / *dx) + 1;
    imax = (int)((xp - *xmin + *rmax) / *dx) + 1;
    if (imin < 1)
    {
        imin = 1;
    }
    if (imax > *nr)
    {
        imax = *nr;
    }
    jmin = (int)((yp - *ymin - *rmax) / *dy) + 1;
    jmax = (int)((yp - *ymin + *rmax) / *dy) + 1;
    if (jmin < 1)
    {
        jmin = 1;
    }
    if (jmax > *nr)
    {
        jmax = *nr;
    }

    /* THE FOLLOWING IS A TEST FOR NO CELLS WITHIN THE CIRCLE */
    /*   OF RADIUS RMAX. */

    if (imin > imax || jmin > jmax)
    {
        goto L5;
    }

    /* ACCUMULATE WEIGHT VALUES IN SW AND WEIGHTED NODAL FUNCTION */
    /*   VALUES IN SWQ.  THE WEIGHTS ARE W(K) =((R - D)+/(R*D))**2 */
    /*   FOR R**2 = RSQ(K) AND D = DISTANCE BETWEEN P AND NODE K. */

    sw = 0.f;
    swq = 0.f;

    /* OUTER LOOP ON CELLS(I, J). */

    i__1 = jmax;
    for (j = jmin; j <= i__1; ++j)
    {
        i__2 = imax;
        for (i__ = imin; i__ <= i__2; ++i__)
        {
            k = lcell[i__ + j * lcell_dim1];
            if (k == 0)
            {
                goto L3;
            }

            /* INNER LOOP ON NODES K. */

        L1:
            delx = xp - x[k];
            dely = yp - y[k];
            dxsq = delx * delx;
            dysq = dely * dely;
            ds = dxsq + dysq;
            rs = rsq[k];
            if (ds >= rs)
            {
                goto L2;
            }
            if (ds == 0.f)
            {
                goto L4;
            }
            rds = rs * ds;
            rd = sqrt(rds);
            w = (rs + ds - rd - rd) / rds;
            sw += w;
            swq += w * (a[k * 5 + 1] * dxsq + a[k * 5 + 2] * delx * dely + a[
                            k * 5 + 3] * dysq + a[k * 5 + 4] * delx + a[k * 5 + 5] *
                        dely + f[k]);

            /* BOTTOM OF LOOP ON NODES IN CELL(I, J). */

        L2:
            kp = k;
            k = lnext[kp];
            if (k != kp)
            {
                goto L1;
            }
        L3:
            ;
        }
    }

    /* SW = 0 IFF P IS NOT WITHIN THE RADIUS R(K) FOR ANY NODE K. */

    if (sw == 0.f)
    {
        goto L5;
    }
    ret_val = swq / sw;
    return ret_val;

    /*(PX, PY) =(X(K), Y(K)) */

L4:
    ret_val = f[k];
    return ret_val;

    /* ALL WEIGHTS ARE 0 AT P. */

L5:
    ret_val = _missing_;
    return ret_val;
} 

int qs2grd_(double *px,   double *py,   int *n,   double *x,   double *y,
                             double *f, int *nr, int *lcell, int *lnext, double *xmin,
                             double *ymin, double *dx, double *dy, double *rmax, double *rsq, double *a, double *
                             q, double *qx, double *qy, int *ier)
{
    int lcell_dim1, lcell_offset, i__1, i__2;

//    double sqrt(double);

    static int i__, j, k;
    static double t, w, rd, ds;
    static int kp;
    static double qk, rs, xp, yp, sw, wx, wy, rds, qkx, qky, swq, sws, swx, swy;
    static int imin, jmin, imax, jmax;
    static double delx, dely, dxsq, dysq, swqx, swqy;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
    /* 						    10/28/87							*/
	/*																*/
    /*   THIS SUBROUTINE COMPUTES THE VALUE AND GRADIENT AT			*/
    /*(PX, PY) OF THE INTERPOLATORY FUNCTION Q DEFINED IN SUB-		*/
    /* ROUTINE QSHEP2.  Q(X, Y) IS A WEIGHTED SUM OF QUADRATIC		*/
    /* NODAL FUNCTIONS.												*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	PX, PY = CARTESIAN COORDINATES OF THE POINT AT WHICH		*/
    /* 		Q AND ITS PARTIALS ARE TO BE EVALUATED.					*/
	/*																*/
    /* 	N = NUMBER OF NODES AND DATA VALUES DEFINING Q.				*/
    /* 	    N .GE. 6.												*/
	/*																*/
    /* 	X, Y, F = ARRAYS OF LENGTH N CONTAINING THE NODES AND		*/
    /* 		DATA VALUES INTERPOLATED BY Q.							*/
	/*																*/
    /* 	NR = NUMBER OF ROWS AND COLUMNS IN THE CELL GRID.			*/
    /* 	     REFER TO STORE2.  NR .GE. 1.							*/
	/*																*/
    /* 	LCELL = NR BY NR ARRAY OF NODAL INDICES ASSOCIATED			*/
    /* 		WITH CELLS.  REFER TO STORE2.							*/
	/*																*/
    /* 	LNEXT = ARRAY OF LENGTH N CONTAINING NEXT - NODE INDI-		*/
    /* 		CES.  REFER TO STORE2.									*/
	/*																*/
    /* 	XMIN, YMIN, DX, DY = MINIMUM NODAL COORDINATES AND CELL		*/
    /* 			  DIMENSIONS.  DX AND DY MUST BE					*/
    /* 			  POSITIVE.  REFER TO STORE2.						*/
	/*																*/
    /* 	RMAX = SQUARE ROOT OF THE LARGEST ELEMENT IN RSQ --			*/
    /* 	       MAXIMUM RADIUS.										*/
	/*																*/
    /* 	RSQ = ARRAY OF LENGTH N CONTAINING THE SQUARED RADII		*/
    /* 	      WHICH ENTER INTO THE WEIGHTS DEFINING Q.				*/
	/*																*/
    /* 	A = 5 BY N ARRAY CONTAINING THE COEFFICIENTS FOR THE		*/
    /* 	    NODAL FUNCTIONS DEFINING Q.								*/
	/*																*/
    /*   INPUT PARAMETERS ARE NOT ALTERED BY THIS SUBROUTINE.		*/
    /* THE PARAMETERS OTHER THAN PX AND PY SHOULD BE INPUT UNAL-	*/
    /* TERED FROM THEIR VALUES ON OUTPUT FROM QSHEP2.  THIS SUB-	*/
    /* ROUTINE SHOULD NOT BE CALLED IF A NONZERO ERROR FLAG WAS		*/
    /* RETURNED BY QSHEP2.											*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	Q = VALUE OF Q AT(PX, PY) UNLESS IER .EQ. 1, IN				*/
    /* 	    WHICH CASE NO VALUES ARE RETURNED.						*/
	/*																*/
    /* 	QX, QY = FIRST PARTIAL DERIVATIVES OF Q AT(PX, PY)			*/
    /* 		UNLESS IER .EQ. 1.										*/
	/*																*/
    /* 	IER = ERROR INDICATOR										*/
    /* 	      IER = 0 IF NO ERRORS WERE ENCOUNTERED.				*/
    /* 	      IER = 1 IF N, NR, DX, DY OR RMAX IS INVALID.			*/
    /* 	      IER = 2 IF NO ERRORS WERE ENCOUNTERED BUT				*/
    /*(PX, PY) IS NOT WITHIN THE RADIUS R(K)						*/
    /* 		      FOR ANY NODE K(AND THUS Q = QX = QY = 0).			*/
	/*																*/
    /* MODULES REQUIRED BY QS2GRD -- NONE							*/
	/*																*/
    /* INTRINSIC FUNCTIONS CALLED BY QS2GRD -- IFIX, SQRT			*/
	/*																*/
    /* **************************************************************/

    a -= 6;
    --rsq;
    --lnext;
    --f;
    --y;
    --x;
    lcell_dim1 = *nr;
    lcell_offset = 1 + lcell_dim1;
    lcell -= lcell_offset;

    xp = *px;
    yp = *py;
    if (*n < 6 || *nr < 1 || *dx <= 0.f || *dy <= 0.f || *rmax < 0.f)
    {
        goto L5;
    }

    /* SET IMIN, IMAX, JMIN, AND JMAX TO CELL INDICES DEFINING */
    /*   THE RANGE OF THE SEARCH FOR NODES WHOSE RADII INCLUDE */
    /*   P.	THE CELLS WHICH MUST BE SEARCHED ARE THOSE INTER- */
    /*   SECTED BY(OR CONTAINED IN) A CIRCLE OF RADIUS RMAX */
    /*   CENTERED AT P. */

    imin = (int)((xp - *xmin - *rmax) / *dx) + 1;
    imax = (int)((xp - *xmin + *rmax) / *dx) + 1;
    if (imin < 1)
    {
        imin = 1;
    }
    if (imax > *nr)
    {
        imax = *nr;
    }
    jmin = (int)((yp - *ymin - *rmax) / *dy) + 1;
    jmax = (int)((yp - *ymin + *rmax) / *dy) + 1;
    if (jmin < 1)
    {
        jmin = 1;
    }
    if (jmax > *nr)
    {
        jmax = *nr;
    }

    /* THE FOLLOWING IS A TEST FOR NO CELLS WITHIN THE CIRCLE */
    /*   OF RADIUS RMAX. */

    if (imin > imax || jmin > jmax)
    {
        goto L6;
    }

    /* Q = SWQ/SW = SUM(W(K)*Q(K))/SUM(W(K)) WHERE THE SUM IS */
    /*   FROM K = 1 TO N, Q(K) IS THE QUADRATIC NODAL FUNCTION, */
    /*   AND W(K) =((R - D)+/(R*D))**2 FOR RADIUS R(K) AND DIST- */
    /*   ANCE D(K).	THUS */

    /* 	 QX =(SWQX*SW - SWQ*SWX)/SW**2  AND */
    /* 	 QY =(SWQY*SW - SWQ*SWY)/SW**2 */

    /*   WHERE SWQX AND SWX ARE PARTIAL DERIVATIVES WITH RESPECT */
    /*   TO X OF SWQ AND SW, RESPECTIVELY.  SWQY AND SWY ARE DE- */
    /*   FINED SIMILARLY. */

    sw = 0.f;
    swx = 0.f;
    swy = 0.f;
    swq = 0.f;
    swqx = 0.f;
    swqy = 0.f;

    /* OUTER LOOP ON CELLS(I, J). */

    i__1 = jmax;
    for (j = jmin; j <= i__1; ++j)
    {
        i__2 = imax;
        for (i__ = imin; i__ <= i__2; ++i__)
        {
            k = lcell[i__ + j * lcell_dim1];
            if (k == 0)
            {
                goto L3;
            }

            /* INNER LOOP ON NODES K. */

        L1:
            delx = xp - x[k];
            dely = yp - y[k];
            dxsq = delx * delx;
            dysq = dely * dely;
            ds = dxsq + dysq;
            rs = rsq[k];
            if (ds >= rs)
            {
                goto L2;
            }
            if (ds == 0.f)
            {
                goto L4;
            }
            rds = rs * ds;
            rd = sqrt(rds);
            w = (rs + ds - rd - rd) / rds;
            t = (rd - rs) * 2.f / (ds * rds);
            wx = delx * t;
            wy = dely * t;
            qkx = a[k * 5 + 1] * 2.f * delx + a[k * 5 + 2] * dely;
            qky = a[k * 5 + 2] * delx + a[k * 5 + 3] * 2.f * dely;
            qk = (qkx * delx + qky * dely) / 2.f;
            qkx += a[k * 5 + 4];
            qky += a[k * 5 + 5];
            qk = qk + a[k * 5 + 4] * delx + a[k * 5 + 5] * dely + f[k];
            sw += w;
            swx += wx;
            swy += wy;
            swq += w * qk;
            swqx = swqx + wx * qk + w * qkx;
            swqy = swqy + wy * qk + w * qky;

            /* BOTTOM OF LOOP ON NODES IN CELL(I, J). */

        L2:
            kp = k;
            k = lnext[kp];
            if (k != kp)
            {
                goto L1;
            }
        L3:
            ;
        }
    }

    /* SW = 0 IFF P IS NOT WITHIN THE RADIUS R(K) FOR ANY NODE K. */

    if (sw == 0.f)
    {
        goto L6;
    }
    *q = swq / sw;
    sws = sw * sw;
    *qx = (swqx * sw - swq * swx) / sws;
    *qy = (swqy * sw - swq * swy) / sws;
    *ier = 0;
    return 0;

    /*(PX, PY) =(X(K), Y(K)) */

L4:
    *q = f[k];
    *qx = a[k * 5 + 4];
    *qy = a[k * 5 + 5];
    *ier = 0;
    return 0;

    /* INVALID INPUT PARAMETER. */

L5:
    *ier = 1;
    return 0;

    /* NO CELLS CONTAIN A POINT WITHIN RMAX OF P, OR */
    /*   SW = 0 AND THUS DS .GE. RSQ(K) FOR ALL K. */

L6:
    *q = 0.f;
    *qx = 0.f;
    *qy = 0.f;
    *ier = 2;
    return 0;
} /* qs2grd_ */

/* Subroutine */ int getnp2_(double *px,   double *py,   double *x,   double *y,   int *
                             nr, int *lcell, int *lnext, double *xmin, double *ymin, double *dx,
                             double *dy, int *np, double *dsq)
{
    int lcell_dim1, lcell_offset, i__1, i__2;
    double r__1, r__2;

//    double sqrt(double);

    static int i__, j, l;
    static double r__;
    static int i0, j0, i1, i2, j1, j2, ln;
    static double xp, yp, rsq;
    static int imin, jmin, imax, jmax;
    static double delx, dely;
    static int lmin;
    static bool first;
    static double rsmin;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
	/*																*/
    /*   GIVEN A SET OF N NODES AND THE DATA STRUCTURE DEFINED IN	*/
    /* SUBROUTINE STORE2, THIS SUBROUTINE USES THE CELL METHOD TO	*/
    /* FIND THE CLOSEST UNMARKED NODE NP TO A SPECIFIED POINT P.	*/
    /* NP IS THEN MARKED BY SETTING LNEXT(NP) TO -LNEXT(NP).(A		*/
    /* NODE IS MARKED IF AND ONLY IF THE CORRESPONDING LNEXT ELE-	*/
    /* MENT IS NEGATIVE.  THE ABSOLUTE VALUES OF LNEXT ELEMENTS,	*/
    /* HOWEVER, MUST BE PRESERVED.)	THUS, THE CLOSEST M NODES TO	*/
    /* P MAY BE DETERMINED BY A SEQUENCE OF M CALLS TO THIS ROU-	*/
    /* TINE.  NOTE THAT IF THE NEAREST NEIGHBOR TO NODE K IS TO		*/
    /* BE DETERMINED(PX = X(K) AND PY = Y(K)), THEN K SHOULD BE		*/
    /* MARKED BEFORE THE CALL TO THIS ROUTINE.						*/
    /*   THE SEARCH IS BEGUN IN THE CELL CONTAINING(OR CLOSEST		*/
    /* TO) P AND PROCEEDS OUTWARD IN RECTANGULAR LAYERS UNTIL ALL	*/
    /* CELLS WHICH CONTAIN POINTS WITHIN DISTANCE R OF P HAVE		*/
    /* BEEN SEARCHED, WHERE R IS THE DISTANCE FROM P TO THE FIRST	*/
    /* UNMARKED NODE ENCOUNTERED(INFINITE IF NO UNMARKED NODES		*/
    /* ARE PRESENT).												*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	PX, PY = CARTESIAN COORDINATES OF THE POINT P WHOSE			*/
    /* 		NEAREST UNMARKED NEIGHBOR IS TO BE FOUND.				*/
	/*																*/
    /* 	X, Y = ARRAYS OF LENGTH N, FOR N .GE. 2, CONTAINING			*/
    /* 	      THE CARTESIAN COORDINATES OF THE NODES.				*/
	/*																*/
    /* 	NR = NUMBER OF ROWS AND COLUMNS IN THE CELL GRID.			*/
    /* 	     NR .GE. 1.												*/
	/*																*/
    /* 	LCELL = NR BY NR ARRAY OF NODAL INDICES ASSOCIATED			*/
    /* 		WITH CELLS.												*/
	/*																*/
    /* 	LNEXT = ARRAY OF LENGTH N CONTAINING NEXT - NODE INDI-		*/
    /* 		CES(OR THEIR NEGATIVES).								*/
	/*																*/
    /* 	XMIN, YMIN, DX, DY = MINIMUM NODAL COORDINATES AND CELL		*/
    /* 			  DIMENSIONS.  DX AND DY MUST BE					*/
    /* 			  POSITIVE.											*/
	/*																*/
    /*   INPUT PARAMETERS OTHER THAN LNEXT ARE NOT ALTERED BY		*/
    /* THIS ROUTINE.  WITH THE EXCEPTION OF(PX, PY) AND THE SIGNS	*/
    /* OF LNEXT ELEMENTS, THESE PARAMETERS SHOULD BE UNALTERED		*/
    /* FROM THEIR VALUES ON OUTPUT FROM SUBROUTINE STORE2.			*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	NP = INDEX(FOR X AND Y) OF THE NEAREST UNMARKED				*/
    /* 	     NODE TO P, OR 0 IF ALL NODES ARE MARKED OR NR			*/
    /* 	     .LT. 1 OR DX .LE. 0 OR DY .LE. 0.	LNEXT(NP)			*/
    /* 	     .LT. 0 IF NP .NE. 0.									*/
	/*																*/
    /* 	DSQ = SQUARED EUCLIDEAN DISTANCE BETWEEN P AND NODE			*/
    /* 	      NP, OR 0 IF NP = 0.									*/
	/*																*/
    /* MODULES REQUIRED BY GETNP2 -- NONE							*/
	/*																*/
    /* INTRINSIC FUNCTIONS CALLED BY GETNP2 -- IABS, IFIX, SQRT		*/
	/*																*/
    /****************************************************************/

    --x;
    --y;
    lcell_dim1 = *nr;
    lcell_offset = 1 + lcell_dim1;
    lcell -= lcell_offset;
    --lnext;

    xp = *px;
    yp = *py;

    /* TEST FOR INVALID INPUT PARAMETERS. */

    if (*nr < 1 || *dx <= 0.f || *dy <= 0.f)
    {
        goto L9;
    }

    /* INITIALIZE PARAMETERS -- */

    /*   FIRST = TRUE IFF THE FIRST UNMARKED NODE HAS YET TO BE */
    /* 	    ENCOUNTERED, */
    /*   IMIN, IMAX, JMIN, JMAX = CELL INDICES DEFINING THE RANGE OF */
    /* 			  THE SEARCH, */
    /*   DELX, DELY = PX - XMIN AND PY - YMIN, */
    /*   I0, J0 = CELL CONTAINING OR CLOSEST TO P, */
    /*   I1, I2, J1, J2 = CELL INDICES OF THE LAYER WHOSE INTERSEC- */
    /* 		  TION WITH THE RANGE DEFINED BY IMIN,..., */
    /* 		  JMAX IS CURRENTLY BEING SEARCHED. */

    first = true;
    imin = 1;
    imax = *nr;
    jmin = 1;
    jmax = *nr;
    delx = xp - *xmin;
    dely = yp - *ymin;
    i0 = (int)(delx / *dx) + 1;
    if (i0 < 1)
    {
        i0 = 1;
    }
    if (i0 > *nr)
    {
        i0 = *nr;
    }
    j0 = (int)(dely / *dy) + 1;
    if (j0 < 1)
    {
        j0 = 1;
    }
    if (j0 > *nr)
    {
        j0 = *nr;
    }
    i1 = i0;
    i2 = i0;
    j1 = j0;
    j2 = j0;

    /* OUTER LOOP ON LAYERS, INNER LOOP ON LAYER CELLS, EXCLUDING */
    /*   THOSE OUTSIDE THE RANGE(IMIN, IMAX) X(JMIN, JMAX). */

L1:
    i__1 = j2;
    for (j = j1; j <= i__1; ++j)
    {
        if (j > jmax)
        {
            goto L7;
        }
        if (j < jmin)
        {
            goto L6;
        }
        i__2 = i2;
        for (i__ = i1; i__ <= i__2; ++i__)
        {
            if (i__ > imax)
            {
                goto L6;
            }
            if (i__ < imin)
            {
                goto L5;
            }
            if (j != j1 && j != j2 && i__ != i1 && i__ != i2)
            {
                goto L5;
            }

            /* SEARCH CELL(I, J) FOR UNMARKED NODES L. */

            l = lcell[i__ + j * lcell_dim1];
            if (l == 0)
            {
                goto L5;
            }

            /*   LOOP ON NODES IN CELL(I, J). */

        L2:
            ln = lnext[l];
            if (ln < 0)
            {
                goto L4;
            }

            /*   NODE L IS NOT MARKED. */

            /* Computing 2nd power */
            r__1 = x[l] - xp;
            /* Computing 2nd power */
            r__2 = y[l] - yp;
            rsq = r__1 * r__1 + r__2 * r__2;
            if (! first)
            {
                goto L3;
            }

            /*   NODE L IS THE FIRST UNMARKED NEIGHBOR OF P ENCOUNTERED. */
            /*     INITIALIZE LMIN TO THE CURRENT CANDIDATE FOR NP, AND */
            /*     RSMIN TO THE SQUARED DISTANCE FROM P TO LMIN.  IMIN, */
            /*     IMAX, JMIN, AND JMAX ARE UPDATED TO DEFINE THE SMAL- */
            /*     LEST RECTANGLE CONTAINING A CIRCLE OF RADIUS R = */
            /*     SQRT(RSMIN) CENTERED AT P, AND CONTAINED IN(1, NR) X */
            /*(1, NR)(EXCEPT THAT, IF P IS OUTSIDE THE RECTANGLE */
            /*     DEFINED BY THE NODES, IT IS POSSIBLE THAT IMIN .GT. */
            /*     NR, IMAX .LT. 1, JMIN .GT. NR, OR JMAX .LT. 1).  FIRST */
            /*     IS RESET TO FALSE. */

            lmin = l;
            rsmin = rsq;
            r__ = sqrt(rsmin);
            imin = (int)((delx - r__) / *dx) + 1;
            if (imin < 1)
            {
                imin = 1;
            }
            imax = (int)((delx + r__) / *dx) + 1;
            if (imax > *nr)
            {
                imax = *nr;
            }
            jmin = (int)((dely - r__) / *dy) + 1;
            if (jmin < 1)
            {
                jmin = 1;
            }
            jmax = (int)((dely + r__) / *dy) + 1;
            if (jmax > *nr)
            {
                jmax = *nr;
            }
            first = false;
            goto L4;

            /*   TEST FOR NODE L CLOSER THAN LMIN TO P. */

        L3:
            if (rsq >= rsmin)
            {
                goto L4;
            }

            /*   UPDATE LMIN AND RSMIN. */

            lmin = l;
            rsmin = rsq;

            /*   TEST FOR TERMINATION OF LOOP ON NODES IN CELL(I, J). */

        L4:
            if (abs(ln) == l)
            {
                goto L5;
            }
            l = abs(ln);
            goto L2;
        L5:
            ;
        }
    L6:
        ;
    }

    /* TEST FOR TERMINATION OF LOOP ON CELL LAYERS. */

L7:
    if (i1 <= imin && i2 >= imax && j1 <= jmin && j2 >= jmax)
    {
        goto L8;
    }
    --i1;
    ++i2;
    --j1;
    ++j2;
    goto L1;

    /* UNLESS NO UNMARKED NODES WERE ENCOUNTERED, LMIN IS THE */
    /*   CLOSEST UNMARKED NODE TO P. */

L8:
    if (first)
    {
        goto L9;
    }
    *np = lmin;
    *dsq = rsmin;
    lnext[lmin] = -lnext[lmin];
    return 0;

    /* ERROR -- NR, DX, OR DY IS INVALID OR ALL NODES ARE MARKED. */

L9:
    *np = 0;
    *dsq = 0.f;
    return 0;
} /* getnp2_ */

/* Subroutine */ int givens_(double *a,   double *b,   double *c__,   double *s)
{
    /* Builtin functions */
//    double sqrt(double);

    /* Local variables */
    static double r__, u, v, aa, bb;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
	/*																*/
    /*   THIS ROUTINE CONSTRUCTS THE GIVENS PLANE ROTATION --		*/
    /*(C  S)														*/
    /* G =() WHERE C*C + S*S = 1 -- WHICH ZEROS THE SECOND			*/
    /*(-S  C)														*/
    /* ENTRY OF THE 2 - VECTOR(A B) - TRANSPOSE.  A CALL TO GIVENS	*/
    /* IS NORMALLY FOLLOWED BY A CALL TO ROTATE WHICH APPLIES		*/
    /* THE TRANSFORMATION TO A 2 BY N MATRIX.  THIS ROUTINE WAS		*/
    /* TAKEN FROM LINPACK.											*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	A, B = COMPONENTS OF THE 2 - VECTOR TO BE ROTATED.			*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	A = VALUE OVERWRITTEN BY R = +/-SQRT(A*A + B*B)				*/
	/*																*/
    /* 	B = VALUE OVERWRITTEN BY A VALUE Z WHICH ALLOWS C			*/
    /* 	    AND S TO BE RECOVERED AS FOLLOWS --						*/
    /* 	      C = SQRT(1 - Z*Z), S = Z     IF ABS(Z) .LE. 1.		*/
    /* 	      C = 1/Z, S = SQRT(1 - C*C) IF ABS(Z) .GT. 1.			*/
	/*																*/
    /* 	C = +/-(A/R)												*/
	/*																*/
    /* 	S = +/-(B/R)												*/
	/*																*/
    /* MODULES REQUIRED BY GIVENS -- NONE							*/
	/*																*/
    /* INTRINSIC FUNCTIONS CALLED BY GIVENS - ABS, SQRT				*/
	/*																*/
    /****************************************************************/


    /* LOCAL PARAMETERS -- */

    /* AA, BB = LOCAL COPIES OF A AND B */
    /* R =	  C*A + S*B = +/-SQRT(A*A + B*B) */
    /* U, V =   VARIABLES USED TO SCALE A AND B FOR COMPUTING R */

    aa = *a;
    bb = *b;
    if (dabs(aa) <= dabs(bb))
    {
        goto L1;
    }

    /* ABS(A) .GT. ABS(B) */

    u = aa + aa;
    v = bb / u;
    r__ = sqrt(v * v + .25f) * u;
    *c__ = aa / r__;
    *s = v * (*c__ + *c__);

    /* NOTE THAT R HAS THE SIGN OF A, C .GT. 0, AND S HAS */
    /*   SIGN(A)*SIGN(B). */

    *b = *s;
    *a = r__;
    return 0;

    /* ABS(A) .LE. ABS(B) */

L1:
    if (bb == 0.f)
    {
        goto L2;
    }
    u = bb + bb;
    v = aa / u;

    /* STORE R IN A. */

    *a = sqrt(v * v + .25f) * u;
    *s = bb / *a;
    *c__ = v * (*s + *s);

    /* NOTE THAT R HAS THE SIGN OF B, S .GT. 0, AND C HAS */
    /*   SIGN(A)*SIGN(B). */

    *b = 1.f;
    if (*c__ != 0.f)
    {
        *b = 1.f / *c__;
    }
    return 0;

    /* A = B = 0. */

L2:
    *c__ = 1.f;
    *s = 0.f;
    return 0;
} /* givens_ */

int rotate_(int *n,   double *c__,   double *s,   double *x,   double *y)
{
    int i__1;

    static int i__;
    static double xi, yi;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
	/*																*/
    /*(C  S)														*/
    /*   THIS ROUTINE APPLIES THE GIVENS ROTATION() TO THE			*/
    /*(-S  C)														*/
    /*(X(1) ... X(N))												*/
    /* 2 BY N MATRIX().												*/
    /*(Y(1) ... Y(N))												*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	N = NUMBER OF COLUMNS TO BE ROTATED.						*/
	/*																*/
    /* 	C, S = ELEMENTS OF THE GIVENS ROTATION.	THESE MAY BE		*/
    /* 	      DETERMINED BY SUBROUTINE GIVENS.						*/
	/*																*/
    /* 	X, Y = ARRAYS OF LENGTH .GE. N CONTAINING THE VECTORS		*/
    /* 	      TO BE ROTATED.										*/
	/*																*/
    /* PARAMETERS N, C, AND S ARE NOT ALTERED BY THIS ROUTINE.		*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	X, Y = ROTATED VECTORS.										*/
	/*																*/
    /* MODULES REQUIRED BY ROTATE -- NONE							*/
	/*																*/
    /****************************************************************/


    /* LOCAL PARAMETERS -- */

    /* I =	  DO - LOOP INDEX */
    /* XI, YI = X(I), Y(I) */

    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    if (*n <= 0 || *c__ == 1.f && *s == 0.f)
    {
        return 0;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__)
    {
        xi = x[i__];
        yi = y[i__];
        x[i__] = *c__ * xi + *s * yi;
        y[i__] = -(*s) * xi + *c__ * yi;
        /* L1: */
    }
    return 0;
} /* rotate_ */

/* Subroutine */ int setup2_(double *xk,   double *yk,   double *fk,   double *xi,   double *yi,
                             double *fi, double *s1, double *s2, double *r__, double *row)
{
//    double sqrt(double);

    static double d__;
    static int i__;
    static double w, w1, w2, dx, dy, dxsq, dysq;


    /****************************************************************/
	/*																*/
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
	/*																*/
    /*   THIS ROUTINE SETS UP THE I - TH ROW OF AN AUGMENTED RE-	*/
    /* GRESSION MATRIX FOR A WEIGHTED LEAST - SQUARES FIT OF A		*/
    /* QUADRATIC FUNCTION Q(X, Y) TO A SET OF DATA VALUES F, WHERE	*/
    /* Q(XK, YK) = FK.  THE FIRST 3 COLUMNS(QUADRATIC TERMS) ARE	*/
    /* SCALED BY 1/S2 AND THE FOURTH AND FIFTH COLUMNS(LINEAR		*/
    /* TERMS) ARE SCALED BY 1/S1.  THE WEIGHT IS(R - D)/(R*D) IF	*/
    /* R .GT. D AND 0 IF R .LE. D, WHERE D IS THE DISTANCE			*/
    /* BETWEEN NODES I AND K.										*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	XK, YK, FK = COORDINATES AND DATA VALUE AT NODE K --		*/
    /* 		   INTERPOLATED BY Q.									*/
	/*																*/
    /* 	XI, YI, FI = COORDINATES AND DATA VALUE AT NODE I.			*/
	/*																*/
    /* 	S1, S2 = RECIPROCALS OF THE SCALE FACTORS.					*/
	/*																*/
    /* 	R = RADIUS OF INFLUENCE ABOUT NODE K DEFINING THE			*/
    /* 	    WEIGHT.													*/
	/*																*/
    /* 	ROW = ARRAY OF LENGTH 6.									*/
	/*																*/
    /* INPUT PARAMETERS ARE NOT ALTERED BY THIS ROUTINE.			*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	ROW = VECTOR CONTAINING A ROW OF THE AUGMENTED				*/
    /* 	      REGRESSION MATRIX.									*/
	/*																*/
    /* MODULES REQUIRED BY SETUP2 -- NONE							*/
	/*																*/
    /* INTRINSIC FUNCTION CALLED BY SETUP2 -- SQRT					*/
	/*																*/
    /****************************************************************/


    /* LOCAL PARAMETERS - */

    /* I =	 DO - LOOP INDEX */
    /* DX =	 XI - XK */
    /* DY =	 YI - YK */
    /* DXSQ = DX*DX */
    /* DYSQ = DY*DY */
    /* D =	 DISTANCE BETWEEN NODES K AND I */
    /* W =	 WEIGHT ASSOCIATED WITH THE ROW */
    /* W1 =	 W/S1 */
    /* W2 =	 W/S2 */

    --row;

    dx = *xi - *xk;
    dy = *yi - *yk;
    dxsq = dx * dx;
    dysq = dy * dy;
    d__ = sqrt(dxsq + dysq);
    if (d__ <= 0.f || d__ >= *r__)
    {
        goto L1;
    }
    w = (*r__ - d__) / *r__ / d__;
    w1 = w / *s1;
    w2 = w / *s2;
    row[1] = dxsq * w2;
    row[2] = dx * dy * w2;
    row[3] = dysq * w2;
    row[4] = dx * w1;
    row[5] = dy * w1;
    row[6] = (*fi - *fk) * w;
    return 0;

    /* NODES K AND I COINCIDE OR NODE I IS OUTSIDE OF THE RADIUS */
    /*   OF INFLUENCE.  SET ROW TO THE ZERO VECTOR. */

L1:
    for (i__ = 1; i__ <= 6; ++i__)
    {
        /* L2: */
        row[i__] = 0.f;
    }
    return 0;
} /* setup2_ */

/* Subroutine */ int store2_(int *n,   double *x,   double *y,   int *nr,
                             int *lcell, int *lnext, double *xmin, double *ymin, double *dx,
                             double *dy, int *ier)
{
    int lcell_dim1, lcell_offset, i__1, i__2;

    static int i__, j, k, l, kb, nn, np1, nnr;
    static double xmn, ymn, xmx, ymx, delx, dely;


    /****************************************************************/
	/*																*/	
    /* 						ROBERT RENKA							*/
    /* 					UNIV. OF NORTH TEXAS						*/
    /*(817) 565 - 2767												*/
	/*																*/
    /*   GIVEN A SET OF N ARBITRARILY DISTRIBUTED NODES IN THE		*/
    /* PLANE, THIS SUBROUTINE CREATES A DATA STRUCTURE FOR A		*/
    /* CELL - BASED METHOD OF SOLVING CLOSEST - POINT PROBLEMS.	THE */
    /* SMALLEST RECTANGLE CONTAINING THE NODES IS PARTITIONED		*/
    /* INTO AN NR BY NR UNIFORM GRID OF CELLS, AND NODES ARE AS-	*/
    /* SOCIATED WITH CELLS.	IN PARTICULAR, THE DATA STRUCTURE		*/
    /* STORES THE INDICES OF THE NODES CONTAINED IN EACH CELL.		*/
    /* FOR A UNIFORM RANDOM DISTRIBUTION OF NODES, THE NEAREST		*/
    /* NODE TO AN ARBITRARY POINT CAN BE DETERMINED IN CONSTANT		*/
    /* EXPECTED TIME.												*/
	/*																*/
    /* ON INPUT --													*/
	/*																*/
    /* 	N = NUMBER OF NODES.  N .GE. 2.								*/
	/*																*/
    /* 	X, Y = ARRAYS OF LENGTH N CONTAINING THE CARTESIAN			*/
    /* 	      COORDINATES OF THE NODES.								*/
	/*																*/
    /* 	NR = NUMBER OF ROWS AND COLUMNS IN THE GRID.  THE			*/
    /* 	     CELL DENSITY(AVERAGE NUMBER OF NODES PER CELL)			*/
    /* 	     IS D = N/(NR**2).	A RECOMMENDED VALUE, BASED			*/
    /* 	     ON EMPIRICAL EVIDENCE, IS D = 3 -- NR =				*/
    /* 	     SQRT(N/3).  NR .GE. 1.									*/
	/*																*/
    /* THE ABOVE PARAMETERS ARE NOT ALTERED BY THIS ROUTINE.		*/
	/*																*/
    /* 	LCELL = ARRAY OF LENGTH .GE. NR**2.							*/
	/*																*/
    /* 	LNEXT = ARRAY OF LENGTH .GE. N.								*/
	/*																*/
    /* ON OUTPUT --													*/
	/*																*/
    /* 	LCELL = NR BY NR CELL ARRAY SUCH THAT LCELL(I, J)			*/
    /* 		CONTAINS THE INDEX(FOR X AND Y) OF THE					*/
    /* 		FIRST NODE(NODE WITH SMALLEST INDEX) IN					*/
    /* 		CELL(I, J), OR LCELL(I, J) = 0 IF NO NODES				*/
    /* 		ARE CONTAINED IN THE CELL.  THE UPPER RIGHT				*/
    /* 		CORNER OF CELL(I, J) HAS COORDINATES(XMIN+				*/
    /* 		I*DX, YMIN + J*DY).  LCELL IS NOT DEFINED IF			*/
    /* 		IER .NE. 0.												*/
	/*																*/
    /* 	LNEXT = ARRAY OF NEXT - NODE INDICES SUCH THAT				*/
    /* 		LNEXT(K) CONTAINS THE INDEX OF THE NEXT NODE			*/
    /* 		IN THE CELL WHICH CONTAINS NODE K, OR					*/
    /* 		LNEXT(K) = K IF K IS THE LAST NODE IN THE				*/
    /* 		CELL FOR K = 1,..., N.(THE NODES CONTAINED				*/
    /* 		IN A CELL ARE ORDERED BY THEIR INDICES.)				*/
    /* 		IF, FOR EXAMPLE, CELL(I, J) CONTAINS NODES				*/
    /* 		2, 3, AND 5(AND NO OTHERS), THEN LCELL(I, J)			*/
    /* 		= 2, LNEXT(2) = 3, LNEXT(3) = 5, AND					*/
    /* 		LNEXT(5) = 5.  LNEXT IS NOT DEFINED IF					*/
    /* 		IER .NE. 0.												*/
	/*																*/
    /* 	XMIN, YMIN = CARTESIAN COORDINATES OF THE LOWER LEFT		*/
    /* 		    CORNER OF THE RECTANGLE DEFINED BY THE				*/
    /* 		    NODES(SMALLEST NODAL COORDINATES) UN-				*/
    /* 		    LESS IER = 1.  THE UPPER RIGHT CORNER IS			*/
    /*(XMAX, YMAX) FOR XMAX = XMIN + NR*DX AND						*/
    /* 		    YMAX = YMIN + NR*DY.								*/
	/*																*/
    /* 	DX, DY = DIMENSIONS OF THE CELLS UNLESS IER = 1.  DX		*/
    /* 		=(XMAX - XMIN)/NR AND DY =(YMAX - YMIN)/NR				*/
    /* 		WHERE XMIN, XMAX, YMIN, AND YMAX ARE THE				*/
    /* 		EXTREMA OF X AND Y.										*/
	/*																*/
    /* 	IER = ERROR INDICATOR --									*/
    /* 	      IER = 0 IF NO ERRORS WERE ENCOUNTERED.				*/
    /* 	      IER = 1 IF N .LT. 2 OR NR .LT. 1.						*/
    /* 	      IER = 2 IF DX = 0 OR DY = 0.							*/
	/*																*/
    /* MODULES REQUIRED BY STORE2 -- NONE							*/
	/*																*/
    /* INTRINSIC FUNCTIONS CALLED BY STORE2 -- FLOAT, IFIX			*/
	/*																*/
    /****************************************************************/

    /* Parameter adjustments */
    --lnext;
    --y;
    --x;
    lcell_dim1 = *nr;
    lcell_offset = 1 + lcell_dim1;
    lcell -= lcell_offset;

    /* Function Body */
    nn = *n;
    nnr = *nr;
    if (nn < 2 || nnr < 1)
    {
        goto L4;
    }

    /* COMPUTE THE DIMENSIONS OF THE RECTANGLE CONTAINING THE */
    /*   NODES. */

    xmn = x[1];
    xmx = xmn;
    ymn = y[1];
    ymx = ymn;
    i__1 = nn;
    for (k = 2; k <= i__1; ++k)
    {
        if (x[k] < xmn)
        {
            xmn = x[k];
        }
        if (x[k] > xmx)
        {
            xmx = x[k];
        }
        if (y[k] < ymn)
        {
            ymn = y[k];
        }
        /* L1: */
        if (y[k] > ymx)
        {
            ymx = y[k];
        }
    }
    *xmin = xmn;
    *ymin = ymn;

    /* COMPUTE CELL DIMENSIONS AND TEST FOR ZERO AREA. */

    delx = (xmx - xmn) / (double) nnr;
    dely = (ymx - ymn) / (double) nnr;
    *dx = delx;
    *dy = dely;
    if (delx == 0.f || dely == 0.f)
    {
        goto L5;
    }

    /* INITIALIZE LCELL. */

    i__1 = nnr;
    for (j = 1; j <= i__1; ++j)
    {
        i__2 = nnr;
        for (i__ = 1; i__ <= i__2; ++i__)
        {
            /* L2: */
            lcell[i__ + j * lcell_dim1] = 0;
        }
    }

    /* LOOP ON NODES, STORING INDICES IN LCELL AND LNEXT. */

    np1 = nn + 1;
    i__2 = nn;
    for (k = 1; k <= i__2; ++k)
    {
        kb = np1 - k;
        i__ = (int)((x[kb] - xmn) / delx) + 1;
        if (i__ > nnr)
        {
            i__ = nnr;
        }
        j = (int)((y[kb] - ymn) / dely) + 1;
        if (j > nnr)
        {
            j = nnr;
        }
        l = lcell[i__ + j * lcell_dim1];
        lnext[kb] = l;
        if (l == 0)
        {
            lnext[kb] = kb;
        }
        /* L3: */
        lcell[i__ + j * lcell_dim1] = kb;
    }

    /* NO ERRORS ENCOUNTERED */

    *ier = 0;
    return 0;

    /* INVALID INPUT PARAMETER */

L4:
    *ier = 1;
    return 0;

    /* DX = 0 OR DY = 0 */

L5:
    *ier = 2;
    return 0;
} 
