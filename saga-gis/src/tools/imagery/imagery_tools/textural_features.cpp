/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 textural_features.cpp                 //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "textural_features.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	ASM	= 0,
	CONTRAST,
	CORRELATION,
	VARIANCE,
	IDM,
	SUM_AVERAGE,
	SUM_ENTROPY,
	SUM_VARIANCE,
	ENTROPY,
	DIF_VARIANCE,
	DIF_ENTROPY,
	MOC_1,
	MOC_2,
	g_nFeatures
};

//---------------------------------------------------------
const CSG_String	g_Features[g_nFeatures][2]	=
{
	{ "ASM"         , _TL("Angular Second Moment"   ) },
	{ "CONTRAST"    , _TL("Contrast"                ) },
	{ "CORRELATION" , _TL("Correlation"             ) },
	{ "VARIANCE"    , _TL("Variance"                ) },
	{ "IDM"         , _TL("Inverse Diff Moment"     ) },
	{ "SUM_AVERAGE" , _TL("Sum Average"             ) },
	{ "SUM_ENTROPY" , _TL("Sum Entropy"             ) },
	{ "SUM_VARIANCE", _TL("Sum Variance"            ) },
	{ "ENTROPY"     , _TL("Entropy"                 ) },
	{ "DIF_VARIANCE", _TL("Difference Variance"     ) },
	{ "DIF_ENTROPY" , _TL("Difference Entropy"      ) },
	{ "MOC_1"       , _TL("Measure of Correlation-1") },
	{ "MOC_2"       , _TL("Measure of Correlation-2") }
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTextural_Features::CTextural_Features(void)
{
	Set_Name		(_TL("Textural Features"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Textural features.\n"
		"This tool is based on the GRASS GIS implementation by Carmine Basco (r.texture)."
		"\n"
		"<h3>References</h3><ul><li>"
		"<b>Haralick, R.M.; Shanmugam, K.; Dinstein, I. (1973):</b> Textural Features for Image Classification. "
		"IEEE Transactions on Systems, Man, and Cybernetics. SMC-3 (6): 610–621. "
		"<a target=\"_blank\" href=\"http://haralick.org/journals/TexturalFeatures.pdf\">online</a>"
		"</li></ul>"
	));

	Parameters.Add_Grid("",
		"GRID"		, _TL("Grid"),
		_TL(""), 
		PARAMETER_INPUT
	);

	for(int i=0; i<g_nFeatures; i++)
	{
		Parameters.Add_Grid(NULL, g_Features[i][0], g_Features[i][1], _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	Parameters.Add_Choice("",
		"DIRECTION"	, _TL("Direction"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("all"),
			_TL("N-S"),
			_TL("NE-SW"),
			_TL("E-W"),
			_TL("SE-NW")
		), 0
	);

	Parameters.Add_Int("",
		"RADIUS"	, _TL("Radius"),
		_TL("kernel radius in cells"),
		1, 1, true
	);

	Parameters.Add_Int("",
		"DISTANCE"	, _TL("Distance"),
		_TL("The distance between two samples."),
		1, 1, true
	);

	Parameters.Add_Int("",
		"MAX_CATS"	, _TL("Maximum Number of Categories"),
		_TL(""),
		256, 2, true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTextural_Features::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pFeatures[g_nFeatures];

	{
		int	i, n	= 0;

		for(i=0; i<g_nFeatures; i++)
		{
			if( (pFeatures[i] = Parameters(g_Features[i][0])->asGrid()) != NULL )
			{
				n++;
			}
		}

		if( n == 0 )
		{
			Error_Set(_TL("Nothing to do. No feature has been selected."));

			return( false );
		}
	}

	//-----------------------------------------------------
	m_pGrid	= Parameters("GRID")->asGrid();

	if( m_pGrid->Get_Range() <= 0.0 )
	{
		Error_Set(_TL("Nothing to do. No variation in input grid."));

		return( false );
	}

	//-----------------------------------------------------
	m_Radius	= Parameters("RADIUS"  )->asInt();
	m_MaxCats	= Parameters("MAX_CATS")->asInt();

	int	Distance	= Parameters("DISTANCE" )->asInt();
	int	Direction	= Parameters("DIRECTION")->asInt();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Matrix	P[4];

			if( !Get_Matrices(x, y, Distance, P) )
			{
				for(int i=0; i<g_nFeatures; i++)
				{
					if( pFeatures[i] )
					{
						pFeatures[i]->Set_NoData(x, y);
					}
				}
			}
			else
			{
				CSG_Vector	Features(g_nFeatures);

				switch( Direction )
				{
				default: // all
					Get_Features(Features, P[0]);
					Get_Features(Features, P[1]);
					Get_Features(Features, P[2]);
					Get_Features(Features, P[3]);
					break;

				case  1: // N-S
					Get_Features(Features, P[0]);
					break;

				case  2: // NE-SW
					Get_Features(Features, P[1]);
					break;

				case  3: // E-W
					Get_Features(Features, P[2]);
					break;

				case  4: // SE-NW
					Get_Features(Features, P[3]);
					break;
				}

				for(int i=0; i<g_nFeatures; i++)
				{
					if( pFeatures[i] )
					{
						pFeatures[i]->Set_Value(x, y, Direction ? Features[i] : Features[i] / 4);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CTextural_Features::Get_Value(int x, int y)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		return( (int)((m_pGrid->asDouble(x, y) - m_pGrid->Get_Min()) * (m_MaxCats - 1) / m_pGrid->Get_Range()) );
	}

	return( -1 );
}

//---------------------------------------------------------
bool CTextural_Features::Get_Matrices(int x, int y, int d, CSG_Matrix P[4])
{
	if( m_pGrid->is_NoData(x, y) )
	{
		return( false );
	}

	size_t	iTone, nTones;

	int		ix, iy;

	//-----------------------------------------------------
	CSG_Array_Int	Tones(m_MaxCats);

	for(iTone=0; iTone<Tones.Get_Size(); iTone++)
	{
		Tones[iTone] = -1;
	}

	// Determine the number of different gray scales (not maxval)
	for(iy=y-m_Radius; iy<=y+m_Radius; iy++)
	{
		for(ix=x-m_Radius; ix<=x+m_Radius; ix++)
		{
			int	Value	= Get_Value(ix, iy);

			if( Value < 0 )
			{
				return( false );
			}

			Tones[Value] = Value;
		}
	}

	// Collapse array, taking out all zero values
	for(iTone=0, nTones=0; iTone<Tones.Get_Size(); iTone++)
	{
		if( Tones[iTone] >= 0 )
		{
			Tones[nTones++]	= Tones[iTone];
		}
	}

	Tones.Set_Array(nTones);

	//-----------------------------------------------------
	P[0].Create((int)nTones, (int)nTones);
	P[1].Create((int)nTones, (int)nTones);
	P[2].Create((int)nTones, (int)nTones);
	P[3].Create((int)nTones, (int)nTones);

	//-----------------------------------------------------
	// Find gray-Tones spatial dependence matrix
	for(iy=y-m_Radius; iy<=y+m_Radius; iy++)
	{
		for(ix=x-m_Radius; ix<=x+m_Radius; ix++)
		{
			int j = 0; while(Tones[j] != Get_Value(ix, iy)) j++;

			if( ix + d <= x + m_Radius )
			{
				int i = 0; while(Tones[i] != Get_Value(ix + d, iy    )) i++;

				P[0][j][i]++;
				P[0][i][j]++;
			}

			if( iy + d <= y + m_Radius )
			{
				int i = 0; while(Tones[i] != Get_Value(ix   , iy + d)) i++;

				P[2][j][i]++;
				P[2][i][j]++;
			}

			if( iy + d <= y + m_Radius && ix - d >= x - m_Radius )
			{
				int i = 0; while(Tones[i] != Get_Value(ix - d, iy + d)) i++;

				P[1][j][i]++;
				P[1][i][j]++;
			}

			if( iy + d <= y + m_Radius && ix + d <= x + m_Radius )
			{
				int i = 0; while(Tones[i] != Get_Value(ix + d, iy + d)) i++;

				P[3][j][i]++;
				P[3][i][j]++;
			}
		}
	}

	//-----------------------------------------------------
	// Normalize gray-Tones spatial dependence matrix
	int	n	= 1 + 2 * m_Radius;

	P[0]	*= 1. / (2. * (n    ) * (n - 1));
	P[1]	*= 1. / (2. * (n - 1) * (n - 1));
	P[2]	*= 1. / (2. * (n - 1) * (n    ));
	P[3]	*= 1. / (2. * (n - 1) * (n - 1));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// in the following are those parts of the original grass implementation
// (r.texture/h_measure.c) responsible for the calculation of the 'measures' from the
// occurrence/co-occurrence matrices:
//
// MODULE:       r.texture
// AUTHOR(S):    Carmine Basco - basco@unisannio.it
//               with hints from: 
// 			prof. Giulio Antoniol - antoniol@ieee.org
// 			prof. Michele Ceccarelli - ceccarelli@unisannio.it
// 
//---------------------------------------------------------

#define EPSILON 0.000000001

double f1_asm(const double **P, int Ng)
{
    double	sum	= 0.0;

    for(int i=0; i<Ng; i++)
		for(int j=0; j<Ng; j++)
			sum	+= P[i][j] * P[i][j];

    return sum;
}

double f2_contrast(const double **P, int Ng)
{
    int i, j, n;
    double sum, bigsum = 0;

    for (n = 0; n < Ng; n++) {
	sum = 0;
	for (i = 0; i < Ng; i++) {
	    for (j = 0; j < Ng; j++) {
		if ((i - j) == n || (j - i) == n) {
		    sum += P[i][j];
		}
	    }
	}
	bigsum += n * n * sum;
    }
    return bigsum;
}

double f3_corr(const double **P, int Ng, const double *px)
{
    int i, j;
    double sum_sqrx = 0, sum_sqry = 0, tmp = 0;
    double meanx = 0, meany = 0, stddevx, stddevy;


    /* Now calculate the means and standard deviations of px and py */

    /*- fix supplied by J. Michael Christensen, 21 Jun 1991 */

    /*- further modified by James Darrell McCauley, 16 Aug 1991
     *     after realizing that meanx=meany and stddevx=stddevy
     */
    for (i = 0; i < Ng; i++) {
	meanx += px[i] * i;
	sum_sqrx += px[i] * i * i;

	for (j = 0; j < Ng; j++)
	    tmp += i * j * P[i][j];
    }
    meany = meanx;
    sum_sqry = sum_sqrx;
    stddevx = sqrt(sum_sqrx - (meanx * meanx));
    stddevy = stddevx;

    return (tmp - meanx * meany) / (stddevx * stddevy);
}

/* Sum of Squares: Variance */
double f4_var(const double **P, int Ng)
{
    int i, j;
    double mean = 0, var = 0;

    /*- Corrected by James Darrell McCauley, 16 Aug 1991
     *  calculates the mean intensity level instead of the mean of
     *  cooccurrence matrix elements
     */
    for (i = 0; i < Ng; i++)
	for (j = 0; j < Ng; j++)
	    mean += i * P[i][j];

    for (i = 0; i < Ng; i++)
	for (j = 0; j < Ng; j++)
	    var += (i + 1 - mean) * (i + 1 - mean) * P[i][j];

    return var;
}

/* Inverse Difference Moment */
double f5_idm(const double **P, int Ng)
{
    int i, j;
    double idm = 0;

    for (i = 0; i < Ng; i++)
	for (j = 0; j < Ng; j++)
	    idm += P[i][j] / (1 + (i - j) * (i - j));

    return idm;
}

/* Sum Average */
double f6_savg(const double **P, int Ng, const double *Pxpys)
{
    int i;
    double savg = 0;

    for (i = 0; i < 2 * Ng - 1; i++)
	savg += (i + 2) * Pxpys[i];

    return savg;
}

/* Sum Variance */
double f7_svar(const double **P, int Ng, double S, const double *Pxpys)
{
    int i;
    double var = 0;

    for (i = 0; i < 2 * Ng - 1; i++)
	var += (i + 2 - S) * (i + 2 - S) * Pxpys[i];

    return var;
}

/* Sum Entropy */
double f8_sentropy(const double **P, int Ng, const double *Pxpys)
{
    int i;
    double sentr = 0;

    for (i = 0; i < 2 * Ng - 1; i++)
	sentr -= Pxpys[i] * log10(Pxpys[i] + EPSILON);

    return sentr;
}

/* Entropy */
double f9_entropy(const double **P, int Ng)
{
    int i, j;
    double entropy = 0;

    for (i = 0; i < Ng; i++) {
	for (j = 0; j < Ng; j++) {
	    entropy += P[i][j] * log10(P[i][j] + EPSILON);
	}
    }

    return -entropy;
}

/* Difference Variance */
double f10_dvar(const double **P, int Ng, const double *Pxpyd)
{
    int i, tmp;
    double sum = 0, sum_sqr = 0, var = 0;

    /* Now calculate the variance of Pxpy (Px-y) */
    for (i = 0; i < Ng; i++) {
	sum += Pxpyd[i];
	sum_sqr += Pxpyd[i] * Pxpyd[i];
    }
    tmp = Ng * Ng;
    var = ((tmp * sum_sqr) - (sum * sum)) / (tmp * tmp);

    return var;
}

/* Difference Entropy */
double f11_dentropy(const double **P, int Ng, const double *Pxpyd)
{
    int i;
    double sum = 0;

    for (i = 0; i < Ng; i++)
	sum += Pxpyd[i] * log10(Pxpyd[i] + EPSILON);

    return -sum;
}

/* Information Measures of Correlation */
double f12_icorr(const double **P, int Ng, const double *px, const double *py)
{
    int i, j;
    double hx = 0, hy = 0, hxy = 0, hxy1 = 0;

    for (i = 0; i < Ng; i++)
	for (j = 0; j < Ng; j++) {
	    hxy1 -= P[i][j] * log10(px[i] * py[j] + EPSILON);
	    hxy -= P[i][j] * log10(P[i][j] + EPSILON);
	}

    /* Calculate entropies of px and py - is this right? */
    for (i = 0; i < Ng; i++) {
	hx -= px[i] * log10(px[i] + EPSILON);
	hy -= py[i] * log10(py[i] + EPSILON);
    }

    /* fprintf(stderr,"hxy1=%f\thxy=%f\thx=%f\thy=%f\n",hxy1,hxy,hx,hy); */
    return ((hxy - hxy1) / (hx > hy ? hx : hy));
}

/* Information Measures of Correlation */
double f13_icorr(const double **P, int Ng, const double *px, const double *py)
{
    int i, j;
    double hxy = 0, hxy2 = 0;

    for (i = 0; i < Ng; i++) {
	for (j = 0; j < Ng; j++) {
	    hxy2 -= px[i] * py[j] * log10(px[i] * py[j] + EPSILON);
	    hxy -= P[i][j] * log10(P[i][j] + EPSILON);
	}
    }

    /* fprintf(stderr,"hx=%f\thxy2=%f\n",hx,hxy2); */
    return (sqrt(fabs(1 - exp(-2.0 * (hxy2 - hxy)))));
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTextural_Features::Get_Features(CSG_Vector &Features, const CSG_Matrix &P)
{
	//-----------------------------------------------------
	int	nTones	= P.Get_NCols();

	CSG_Vector	px   (nTones    );
	CSG_Vector	py   (nTones    );
	CSG_Vector	Pxpys(nTones * 2);
	CSG_Vector	Pxpyd(nTones * 2);

	for(int i=0; i<nTones; i++)
	{
		for(int j=0; j<nTones; j++)
		{
			px[i]				+= P[i][j];
			py[j]				+= P[i][j];
			Pxpys[i + j]		+= P[i][j];
			Pxpyd[abs(i - j)]	+= P[i][j];
		}
	}

	//-----------------------------------------------------
	double	Sum_Entropy;

	Features[ASM         ]	+= f1_asm      (P, nTones);
	Features[CONTRAST    ]	+= f2_contrast (P, nTones);
	Features[CORRELATION ]	+= f3_corr     (P, nTones, px);
	Features[VARIANCE    ]	+= f4_var      (P, nTones);
	Features[IDM         ]	+= f5_idm      (P, nTones);
	Features[SUM_AVERAGE ]	+= f6_savg     (P, nTones, Pxpys);
	Features[SUM_ENTROPY ]	+= Sum_Entropy	= f8_sentropy (P, nTones, Pxpys);
	Features[SUM_VARIANCE]	+= f7_svar     (P, nTones, Sum_Entropy, Pxpys);
	Features[ENTROPY     ]	+= f9_entropy  (P, nTones);
	Features[DIF_VARIANCE]	+= f10_dvar    (P, nTones, Pxpyd);
	Features[DIF_ENTROPY ]	+= f11_dentropy(P, nTones, Pxpyd);
	Features[MOC_1       ]	+= f12_icorr   (P, nTones, px, py);
	Features[MOC_2       ]	+= f13_icorr   (P, nTones, px, py);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
