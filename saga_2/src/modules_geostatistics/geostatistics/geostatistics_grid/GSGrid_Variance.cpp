
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  GSGrid_Variance.cpp                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GSGrid_Variance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Variance::CGSGrid_Variance(void)
{
	Set_Name(_TL("Representativeness (Grid)"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL("Representativeness - calculation of the variance within a given search radius.\n"
		"\n"
		"Reference:\n"
		"- Boehner, J., Koethe, R., Trachinow, C. (1997): "
		"'Weiterentwicklung der automatischen Reliefanalyse auf der Basis von Digitalen Gelaendemodellen', "
		"Goettinger Geographische Abhandlungen, Vol.100, p.3-21\n")
	);

	Parameters.Add_Grid(
		NULL	, "INPUT"	, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"	, _TL("Representativeness"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"	, _TL("Radius (Cells)"),
		"",
		PARAMETER_TYPE_Int, 10
	);

	Parameters.Add_Value(
		NULL	, "EXPONENT", _TL("Exponent"),
		"",
		PARAMETER_TYPE_Double, 1
	);
}

//---------------------------------------------------------
CGSGrid_Variance::~CGSGrid_Variance(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Variance::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT"	)->asGrid();
	pOutput		= Parameters("RESULT"	)->asGrid();

	maxRadius	= Parameters("RADIUS"	)->asInt();
	Exponent	= Parameters("EXPONENT"	)->asDouble();

	//-----------------------------------------------------
	Initialize();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			pOutput->Set_Value(x,y, Get_Laenge(x,y) );
		}
	}

	//-----------------------------------------------------
	Finalize();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGSGrid_Variance::Initialize(void)
{
	pOutput->Assign_NoData();

	V		= (double *)malloc((maxRadius + 2) * sizeof(double));
	Z		= (int    *)malloc((maxRadius + 2) * sizeof(int   ));

	g		= (double *)malloc((maxRadius + 2) * sizeof(double));
	m		= (double *)malloc((maxRadius + 2) * sizeof(double));

	rLength	= (int    *)malloc((maxRadius + 2) * sizeof(int   ));

	Init_Radius();
}

//---------------------------------------------------------
void CGSGrid_Variance::Init_Radius(void)
{
	int		k, maxZ;

	long	i, j, iijj, 
			rr, r1r1, z;

	maxZ		= z	= 0;
	rLength[0]	= 0;

	x_diff		= y_diff	= NULL;

	for(k=1; k<=maxRadius; k++) 
	{
		rr		= k*k;
		r1r1	= (k - 1) * (k - 1);

		for(i=-k; i<=k; i++)
		{
			for(j=-k; j<=k; j++) 
			{
				iijj	= i*i + j*j;

				if( iijj<=rr && iijj>r1r1 ) 
				{
					if( maxZ<=z )
					{
						maxZ	+= 1000;
						x_diff	= (int *)realloc(x_diff,maxZ*sizeof(int));
						y_diff	= (int *)realloc(y_diff,maxZ*sizeof(int));
					}

					x_diff[z]	= j;
					y_diff[z]	= i;

					z++;
				}
			}
		}

		rLength[k]	= z;
	}
}

//---------------------------------------------------------
void CGSGrid_Variance::Finalize(void)
{
	free(V);
	free(Z);

	free(rLength);
	free(g);
	free(m);

	free(x_diff);
	free(y_diff);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGSGrid_Variance::Get_Laenge(int x, int y)
{
	int		iRadius, Count;

	double	d;

	//-----------------------------------------------------
	V[0]	= Get_GSGrid_Variance(x,y,1,Count);
	Z[0]	= Count;

	for(iRadius=1; iRadius<maxRadius; iRadius++)
	{
		V[iRadius]	= V[iRadius-1] + Get_GSGrid_Variance(x, y, iRadius + 1, Count);
		Z[iRadius]	= Z[iRadius-1] + Count;
	}

	for(iRadius=0; iRadius<maxRadius; iRadius++)
	{
		V[iRadius]	/= (double)Z[iRadius];
	}

	//-----------------------------------------------------
	d		= Get_Steigung();

	if( d == 0.0 )
		return( Get_Cellsize() * maxRadius );
	else
		return( V[maxRadius-1] / d / 2.0 );
}
 
//---------------------------------------------------------
double CGSGrid_Variance::Get_GSGrid_Variance(int x, int y, int iRadius, int &Count)
{
	int		i, ix, iy;

	double	d, z, Variance;

	Variance	= 0;
	z			= pInput->asDouble(x,y);

	for(i=rLength[iRadius-1], Count=0; i<rLength[iRadius]; i++, Count++)
	{
		ix	= x + x_diff[i];
		if( ix < 0 )
			ix	= 0;
		else if( ix >= Get_NX() )
			ix	= Get_NX() - 1;

		iy	= y + y_diff[i];
		if( iy < 0 )
			iy	= 0;
		else if( iy >= Get_NY() )
			iy	= Get_NY() - 1;

		d			= z - pInput->asDouble(ix,iy);
		Variance	+= d * d;
	}

	return( Variance );
}

//---------------------------------------------------------
double CGSGrid_Variance::Get_Steigung(void)
{
	int		i;

	double	summe_mg, summe_g;

	//-----------------------------------------------------
	// Steigungen berechnen...
	m[0]		= V[0] / Get_Cellsize();

	for(i=1; i<maxRadius; i++)
		m[i]	= (V[i] - V[i-1]) / Get_Cellsize();

	//-----------------------------------------------------
	// Gewichte berechnen (inverse distance)...
	for(i=0; i<maxRadius; i++)
		g[i]	= pow(Get_Cellsize() * (i + 1), -Exponent);

	//-----------------------------------------------------
	// Berechne Summe der gewichteten Steigungen und Summe der Gewichte...
	summe_mg	= 0;
	summe_g		= 0;

	for(i=0; i<maxRadius; i++)
	{
		summe_mg	+= m[i] * g[i];
		summe_g		+= g[i];
	}

	return( summe_mg / summe_g );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
