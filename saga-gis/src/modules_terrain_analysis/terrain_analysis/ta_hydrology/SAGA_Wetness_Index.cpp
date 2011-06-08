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
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 SAGA_Wetness_Index.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "SAGA_Wetness_Index.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSAGA_Wetness_Index::CSAGA_Wetness_Index(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("SAGA Wetness Index"));

	Set_Author		(SG_T("(c) 2001 by J.Boehner, O.Conrad"));

	Set_Description	(_TW(
		"The 'SAGA Wetness Index' is, as the name says, similar to the "
		"'Topographic Wetness Index' (TWI), but it is based on a modified "
		"catchment area calculation ('Modified Catchment Area'), which does "
		"not think of the flow as very thin film. As result it predicts for "
		"cells situated in valley floors with a small vertical distance to "
		"a channel a more realistic, higher potential soil moisture compared "
		"to the standard TWI calculation.\n\n"

		"References\n"

		"- Boehner, J., Koethe, R. Conrad, O., Gross, J., Ringeler, A., Selige, T. (2002): "
		"Soil Regionalisation by Means of Terrain Analysis and Process Parameterisation. "
		"In: Micheli, E., Nachtergaele, F., Montanarella, L. [Ed.]: Soil Classification 2001. "
		"European Soil Bureau, Research Report No. 7, EUR 20398 EN, Luxembourg. pp.213-222."
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "C"			, _TL("Catchment area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "GN"			, _TL("Catchment slope"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CS"			, _TL("Modified catchment area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SB"			, _TL("Wetness index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "T"			, _TL("t"),
		_TL(""),
		PARAMETER_TYPE_Double	, 10.0, 0.0, true
	);
}

//---------------------------------------------------------
CSAGA_Wetness_Index::~CSAGA_Wetness_Index(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSAGA_Wetness_Index::On_Execute(void)
{
	double	a, t;
	CSG_Grid	*pDEM, *pC, *pGN, *pCS, *pSB;

	//-----------------------------------------------------
	pDEM	= Parameters("DEM")	->asGrid();
	pC		= Parameters("C")	->asGrid();
	pGN		= Parameters("GN")	->asGrid();
	pCS		= Parameters("CS")	->asGrid();
	pSB		= Parameters("SB")	->asGrid();

	t		= Parameters("T")	->asDouble();
	a		= 0.0174532;

	//-----------------------------------------------------
	Get_Area_Catchment	(pDEM, pC, pGN);

	Get_Area_Modified	(pDEM, pC, pCS, t);

	Get_Wetness_Index	(pDEM, pCS, pSB, a);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Im Unterschied zu Freeman's urspruenglichen Verfahren
// wird der Winkel (atan(dz / dx)) und nicht das Gefaelle
// (dz / dx) fuer die Gewichtung der Abfluszanteile benutzt!

//---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_Area_Catchment(CSG_Grid *pDEM, CSG_Grid *pC, CSG_Grid *pS)
{
	const double	MFD_Converge	= 1.1;

	int		x, y, i, ix, iy;
	double	z, d, dz[8], dzSum, c, s;

	//-----------------------------------------------------
	Process_Set_Text(_TL("Catchment area calculation..."));

	pC->Assign(1.0);
	pS->Assign(0.0);

	//-----------------------------------------------------
	for(long n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		pDEM->Get_Sorted(n, x, y, true, false);

		if( pDEM->is_NoData(x, y) )
		{
			pC->Set_NoData(x, y);
		}
		else
		{
			pDEM->Get_Gradient(x, y, s, c);

			c	= pC->asDouble(x, y);
			s	= pS->asDouble(x, y) + s;

			pC->Set_Value(x, y, c);
			pS->Set_Value(x, y, s / c);

			for(i=0, dzSum=0.0, z=pDEM->asDouble(x, y); i<8; i++)
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

				if( is_InGrid(ix, iy) && !pDEM->is_NoData(ix, iy) && (d = z - pDEM->asDouble(ix, iy)) > 0.0 )
				{
					dzSum	+= (dz[i] = pow(atan(d / Get_Length(i)), MFD_Converge));
				}
				else
				{
					dz[i]	= 0.0;
				}
			}

			if( dzSum > 0.0 )
			{
				for(i=0; i<8; i++)
				{
					if( dz[i] > 0.0 )
					{
						ix		= Get_xTo(i, x);
						iy		= Get_yTo(i, y);

						d		= dz[i] / dzSum;

						pC->Add_Value(ix, iy, d * c);
						pS->Add_Value(ix, iy, d * s);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	*pC	*= pC->Get_Cellarea();

	return( true );
}

/*/---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_Area_Catchment(CSG_Grid *pDEM, CSG_Grid *pC, CSG_Grid *pGN)
{
	int		n, x, y, i, ix, iy;
	double	z, d, dz[8], dzSum, c, w, sw, sn, Slope, Azimuth,
			Weight	= 0.5,
			a		= Get_Cellsize() * Get_Cellsize(),
			MFD_Converge	= 1.1;
	CSG_Grid	W, SW, SN;

	//-----------------------------------------------------
	Process_Set_Text("Catchment area/slope calculation...");

	pC ->Assign(0.0);
	pGN->Assign(0.0);

	W .Create(pDEM);
	SW.Create(pDEM);
	SN.Create(pDEM);

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		pDEM->Get_Sorted(n, x, y, true, false);

		if( pDEM->is_NoData(x, y) )
		{
			pC ->Set_NoData(x, y);
			pGN->Set_NoData(x, y);
		}
		else
		{
			pDEM->Get_Gradient(x, y, Slope, Azimuth);

			z		= pDEM->asDouble(x, y);
			c		= pC  ->asDouble(x, y) + a;
			w		= W    .asDouble(x, y) + pow(c, Weight);	//{ W[p] = C[p]^w;}
			sw		= SW   .asDouble(x, y) + w;					//{SW[p] = W[p];}
			sn		= SN   .asDouble(x, y) + w * Slope;			//{SN[p] = W[p] * N[p];}

			for(i=0, dzSum=0.0; i<8; i++)
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

				if( is_InGrid(ix, iy) && !pDEM->is_NoData(ix, iy) && (d = z - pDEM->asDouble(ix, iy)) > 0.0 )
				{
					dzSum	+= (dz[i] = pow(atan(d / Get_Length(i)), MFD_Converge));
				}
				else
				{
					dz[i]	= 0.0;
				}
			}

			if( dzSum > 0.0 )
			{
				for(i=0; i<8; i++)
				{
					if( dz[i] > 0.0 )
					{
						ix		= Get_xTo(i, x);
						iy		= Get_yTo(i, y);

						d		= dz[i] / dzSum;

						pC->Add_Value(ix, iy, d * c);
						W  .Add_Value(ix, iy, d * w);	//{ W[p] =  W[p] +  W[p+pul] * UL[p] +  W[p+pl] * LL[p] +  W[p+pol] * OL[p] +  W[p+po] * OO[p] +  W[p+por] * OR[p] +  W[p+pr] * RR[p] +  W[p+pur] * UR[p] +  W[p+pu] * UU[p]; gefunden = 1;}
						SW .Add_Value(ix, iy, d * sw);	//{SW[p] = SW[p] + SW[p+pul] * UL[p] + SW[p+pl] * LL[p] + SW[p+pol] * OL[p] + SW[p+po] * OO[p] + SW[p+por] * OR[p] + SW[p+pr] * RR[p] + SW[p+pur] * UR[p] + SW[p+pu] * UU[p]; gefunden = 1;}
						SN .Add_Value(ix, iy, d * sn);	//{SN[p] = SN[p] + SN[p+pul] * UL[p] + SN[p+pl] * LL[p] + SN[p+pol] * OL[p] + SN[p+po] * OO[p] + SN[p+por] * OR[p] + SN[p+pr] * RR[p] + SN[p+pur] * UR[p] + SN[p+pu] * UU[p]; gefunden = 1;}
					}
				}
			}
		}

		pC ->Set_Value(x, y, c);
		pGN->Set_Value(x, y, sn / sw);	//{GN[p] = SN[p] / SW[p];}
	}

	//-----------------------------------------------------
	return( true );
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// in den folgenden drei Schritten wird die gesaugte
// Einzugsgebietsgroeﬂe CS ermittelt, wobei der t-Parameter
// die Staerke der Saugung steuert. Werte unter 10 (z.B.  5)
// fuehren zu einer starken Saugung, Werte ueber 10 (z.B. 15)
// zu einer schwachen Saugung. Die gesaugte
// Einzugsgebietsgroeﬂe selbst stellt bereits einen Parameter
// dar, der die raeumliche Relief-bedingte Feuchteverteilung
// in guter Weise annaehert

//---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_Area_Modified(CSG_Grid *pDEM, CSG_Grid *pC, CSG_Grid *pCS, double t)
{
	bool	bRecalculate;
	int		x, y, i, ix, iy, n;
	double	z, d;
	CSG_Grid	C, C_Last, T;

	//-----------------------------------------------------
	Process_Set_Text(_TL("Modify: pre-processing..."));

	T.Create(pC);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pDEM->is_NoData(x, y) && !pC->is_NoData(x, y) )
			{
				pDEM->Get_Gradient(x, y, z, d);
				z	= pow(t, z);
				z	= pow(1.0 / z, exp(z));
				T.Set_Value(x, y, z);
			}
			else
			{
				T.Set_NoData(x, y);
			}
		}
	}

	C     .Create(pC);
	C     .Assign(pC);
	C_Last.Create(pC);
	C_Last.Assign(pC);

	//-----------------------------------------------------
	for(i=0, n=1; n>0; i++)
	{
		for(y=0, n=0; y<Get_NY() && Process_Get_Okay(false); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !T.is_NoData(x, y) )
				{
					z	= T.asDouble(x, y) * Get_Local_Maximum(&C, x, y);

					if( z  > C.asDouble(x, y) )
					{
						n++;

						C.Set_Value(x, y, z);
					}
				}
			}
		}

		if( n > 0 )
		{
			for(y=0, n=0; y<Get_NY() && Process_Get_Okay(false); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					if( C.asDouble(x, y) != C_Last.asDouble(x, y) )
					{
						n++;

						C_Last.Set_Value(x, y, C.asDouble(x, y));
					}
				}
			}
		}

		Process_Set_Text(CSG_String::Format(SG_T("Modify: %d. iteration [%d > 0]"), i, n));
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Modify: post-processing..."));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pC->is_NoData(x, y) )
			{
				pCS->Set_NoData(x, y);
			}
			else
			{
				for(iy=y-1, bRecalculate=false; iy<=y+1 && !bRecalculate; iy++)
				{
					for(ix=x-1; ix<=x+1 && !bRecalculate; ix++)
					{
						if( C.is_InGrid(ix, iy) && pC->is_InGrid(ix, iy) && C.asDouble(ix, iy) > pC->asDouble(ix, iy) )
						{
							bRecalculate	= true;
						}
					}
				}

				if( bRecalculate )
				{
					for(iy=y-1, z=0.0, n=0; iy<=y+1; iy++)
					{
						for(ix=x-1; ix<=x+1; ix++)
						{
							if( C.is_InGrid(ix, iy) )
							{
								n++;
								z	+= C.asDouble(ix, iy);
							}
						}
					}

					z	= z / (double)n;
				}
				else	
				{
					z	= C.asDouble(x, y);
				}

				pCS->Set_Value(x, y, z);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
double CSAGA_Wetness_Index::Get_Local_Maximum(CSG_Grid *pGrid, int x, int y)
{
	int		i, ix, iy;
	double	z	= pGrid->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( pGrid->is_InGrid(ix, iy) && pGrid->asDouble(ix, iy) > z )
		{
			z	= pGrid->asDouble(ix, iy);
		}
	}

	return( z );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// in den folgenden zwei Schritten wird der SAGA
// Bodenfeuchteindex SB ermittelt. Der a-Parameter muss
// bei den Settings definiert werden und sorgt dafuer,
// das nicht durch 0 dividiert wird

//---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_Wetness_Index(CSG_Grid *pDEM, CSG_Grid *pCS, CSG_Grid *pSB, double a)
{
	int		x, y;
	double	Slope, Azimuth;

	Process_Set_Text(_TL("Wetness index calculation..."));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pDEM->is_NoData(x, y) && !pCS->is_NoData(x, y) )
			{
				pDEM->Get_Gradient(x, y, Slope, Azimuth);

				pSB->Set_Value(x, y, log(pCS->asDouble(x, y) / tan(a + Slope)));
			}
			else
			{
				pSB->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//        The original BSL script by J.Boehner
//---------------------------------------------------------

/*
Matrix Loop, O("sch-hoe.grd"), N("sch-nei.grd"), M, R, UL, LL, OL, OO, OR, RR, UR, UU, X, Y, Z, C, CS, SB;
Point ploop, p, pul, pu, pur, pl, pr, pol, po, por;
Float a, wul, wll, wol, woo, wor, wrr, wur, wuu;
Integer t, h, i, j, gefunden;

a = 0.0174532;
t = 10; 
pul.x = -1;	pul.y = -1; 
pu.x = 0;	pu.y = -1;
pur.x = 1;	pur.y = -1;
pl.x = -1;	pl.y = 0;
pr.x = 1;	pr.y = 0;
pol.x = -1;	pol.y = 1;
po.x = 0;	po.y = 1;
por.x = 1;	por.y = 1;

M = O;
R = O;
UL = O;
LL = O;
OL = O;
OO = O;
OR = O;
RR = O;
UR = O;
UU = O;
X = O;
Y = O;
Z = O;
C = O;
CS = O;
SB = O;

Loop.xanz = 100000;
Loop.yanz = 1;
h = 0;
i = 0;
j = 0;

// hier wird eine Hilfsmatrix X erzeugt, die in der folgenden Schleife fafuer sorgt, dass noch nicht attributisierte Rasterzellen in Ihrer Position identifiziert werden koennen //
foreach p in X do
{	if (p.x == 0 || p.x == M.xanz - 1 || p.y == 0 || p.y == M.yanz - 1)
	{X[p] = -10000;}
	else
	{X[p] = M[p];}
}
// hier wird eine Matrix R erzeugt, die ausgehend von lokalen Maxima fortlaufend Rangplatzziffernerzeugt //
foreach ploop in Loop do
{	ploop.x = 1;
	h = h +1;
	gefunden = 0;
	foreach p in X do
	{
	if (X[p] == max9(p, X) && X[p] > -10000)
	{X[p] = -10000; R[p] = h; gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
setRandN(R);
// hier wird eine Hilfsmatrix R erzeugt, die Rangplatzziffern am Rand duch 0 ersetzt //
foreach p in R do
{	if (p.x == 0 || p.x == M.xanz - 1 || p.y == 0 || p.y == M.yanz - 1)
	{R[p] = 0;}
	else
	{R[p] = R[p];}
}
// hier wird eine Matrix Z erzeugt, die die positiven Winkel zu den 8 Nachbarzellen aufsummiert //
foreach p in Z do
{
if(p.x == 0 && p.y == 0)
	{
	if((M[p] - M[p+po]) > 0) 
	{woo = atan	((M[p] - M[p+po])	/	M.dxy);}
	else
	{woo = 0;}
	if((M[p] - M[p+por]) > 0) 
	{wor = atan	((M[p] - M[p+por])	/	(2 * M.dxy^2)^0.5);}
	else
	{wor = 0;}
	if((M[p] - M[p+pr]) > 0) 
	{wrr = atan	((M[p] - M[p+pr])	/	M.dxy);}
	else
	{wrr = 0;}
	if((woo + wor + wrr) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = woo + wor + wrr;}
	}
else
{
if(p.x == 0 && p.y == (M.yanz - 1))
	{
	if((M[p] - M[p+pr]) > 0) 
	{wrr = atan	((M[p] - M[p+pr])	/	M.dxy);}
	else
	{wrr = 0;}
	if((M[p] - M[p+pur]) > 0) 
	{wur = atan	((M[p] - M[p+pur])	/	(2 * M.dxy^2)^0.5);}
	else
	{wur = 0;}
	if((M[p] - M[p+pu]) > 0) 
	{wuu = atan	((M[p] - M[p+pu])	/	M.dxy);}
	else
	{wuu = 0;}
	if((wrr + wur + wuu) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wrr + wur + wuu;}
	}
else	
{
if(p.x == M.xanz - 1 && p.y == M.yanz - 1)
	{
	if((M[p] - M[p+pul]) > 0) 
	{wul = atan	((M[p] - M[p+pul])	/	(2 * M.dxy^2)^0.5);}
	else
	{wul = 0;}
	if((M[p] - M[p+pl]) > 0) 
	{wll = atan	((M[p] - M[p+pl])	/	M.dxy);}
	else
	{wll = 0;}
	if((M[p] - M[p+pu]) > 0) 
	{wuu = atan	((M[p] - M[p+pu])	/	M.dxy);}
	else
	{wuu = 0;}
	if((wul + wll + wuu) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wul + wll + wuu;}
	}
else	
{
if(p.x == M.xanz - 1 && p.y == 0)
	{
	if((M[p] - M[p+pl]) > 0) 
	{wll = atan	((M[p] - M[p+pl])	/	M.dxy);}
	else
	{wll = 0;}
	if((M[p] - M[p+pol]) > 0) 
	{wol = atan	((M[p] - M[p+pol])	/	(2 * M.dxy^2)^0.5);}
	else
	{wol = 0;}
	if((M[p] - M[p+po]) > 0) 
	{woo = atan	((M[p] - M[p+po])	/	M.dxy);}
	else
	{woo = 0;}
	if((wll + wol + woo) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wll + wol + woo;}
	}
else	
{
if(p.x == 0)
	{
	if((M[p] - M[p+po]) > 0) 
	{woo = atan	((M[p] - M[p+po])	/	M.dxy);}
	else
	{woo = 0;}
	if((M[p] - M[p+por]) > 0) 
	{wor = atan	((M[p] - M[p+por])	/	(2 * M.dxy^2)^0.5);}
	else
	{wor = 0;}
	if((M[p] - M[p+pr]) > 0) 
	{wrr = atan	((M[p] - M[p+pr])	/	M.dxy);}
	else
	{wrr = 0;}
	if((M[p] - M[p+pur]) > 0) 
	{wur = atan	((M[p] - M[p+pur])	/	(2 * M.dxy^2)^0.5);}
	else
	{wur = 0;}
	if((M[p] - M[p+pu]) > 0) 
	{wuu = atan	((M[p] - M[p+pu])	/	M.dxy);}
	else
	{wuu = 0;}
	if((woo + wor + wrr + wur + wuu) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = woo + wor + wrr + wur + wuu;}
	}
else	
{
if(p.x == M.xanz - 1)
	{
	if((M[p] - M[p+pul]) > 0) 
	{wul = atan	((M[p] - M[p+pul])	/	(2 * M.dxy^2)^0.5);}
	else
	{wul = 0;}
	if((M[p] - M[p+pl]) > 0) 
	{wll = atan	((M[p] - M[p+pl])	/	M.dxy);}
	else
	{wll = 0;}
	if((M[p] - M[p+pol]) > 0) 
	{wol = atan	((M[p] - M[p+pol])	/	(2 * M.dxy^2)^0.5);}
	else
	{wol = 0;}
	if((M[p] - M[p+po]) > 0) 
	{woo = atan	((M[p] - M[p+po])	/	M.dxy);}
	else
	{woo = 0;}
	if((M[p] - M[p+pu]) > 0) 
	{wuu = atan	((M[p] - M[p+pu])	/	M.dxy);}
	else
	{wuu = 0;}
	if((wul + wll + wol + woo + wuu) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wul + wll + wol + woo + wuu;}
	}
else	
{
if(p.y == 0)
	{
	if((M[p] - M[p+pl]) > 0) 
	{wll = atan	((M[p] - M[p+pl])	/	M.dxy);}
	else
	{wll = 0;}
	if((M[p] - M[p+pol]) > 0) 
	{wol = atan	((M[p] - M[p+pol])	/	(2 * M.dxy^2)^0.5);}
	else
	{wol = 0;}
	if((M[p] - M[p+po]) > 0) 
	{woo = atan	((M[p] - M[p+po])	/	M.dxy);}
	else
	{woo = 0;}
	if((M[p] - M[p+por]) > 0) 
	{wor = atan	((M[p] - M[p+por])	/	(2 * M.dxy^2)^0.5);}
	else
	{wor = 0;}
	if((M[p] - M[p+pr]) > 0) 
	{wrr = atan	((M[p] - M[p+pr])	/	M.dxy);}
	else
	{wrr = 0;}
	if((wll + wol + woo + wor + wrr) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wll + wol + woo + wor + wrr;}
	}
else	
{
if(p.y == M.yanz - 1)
	{
	if((M[p] - M[p+pul]) > 0) 
	{wul = atan	((M[p] - M[p+pul])	/	(2 * M.dxy^2)^0.5);}
	else
	{wul = 0;}
	if((M[p] - M[p+pl]) > 0) 
	{wll = atan	((M[p] - M[p+pl])	/	M.dxy);}
	else
	{wll = 0;}
	if((M[p] - M[p+pr]) > 0) 
	{wrr = atan	((M[p] - M[p+pr])	/	M.dxy);}
	else
	{wrr = 0;}
	if((M[p] - M[p+pur]) > 0) 
	{wur = atan	((M[p] - M[p+pur])	/	(2 * M.dxy^2)^0.5);}
	else
	{wur = 0;}
	if((M[p] - M[p+pu]) > 0) 
	{wuu = atan	((M[p] - M[p+pu])	/	M.dxy);}
	else
	{wuu = 0;}
	if((wul + wll + wrr + wur + wuu) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wul + wll + wrr + wur + wuu;}
	}
else	
{	
	if((M[p] - M[p+pul]) > 0) 
	{wul = atan	((M[p] - M[p+pul])	/	(2 * M.dxy^2)^0.5);}
	else
	{wul = 0;}
	if((M[p] - M[p+pl]) > 0) 
	{wll = atan	((M[p] - M[p+pl])	/	M.dxy);}
	else
	{wll = 0;}
	if((M[p] - M[p+pol]) > 0) 
	{wol = atan	((M[p] - M[p+pol])	/	(2 * M.dxy^2)^0.5);}
	else
	{wol = 0;}
	if((M[p] - M[p+po]) > 0) 
	{woo = atan	((M[p] - M[p+po])	/	M.dxy);}
	else
	{woo = 0;}
	if((M[p] - M[p+por]) > 0) 
	{wor = atan	((M[p] - M[p+por])	/	(2 * M.dxy^2)^0.5);}
	else
	{wor = 0;}
	if((M[p] - M[p+pr]) > 0) 
	{wrr = atan	((M[p] - M[p+pr])	/	M.dxy);}
	else
	{wrr = 0;}
	if((M[p] - M[p+pur]) > 0) 
	{wur = atan	((M[p] - M[p+pur])	/	(2 * M.dxy^2)^0.5);}
	else
	{wur = 0;}
	if((M[p] - M[p+pu]) > 0) 
	{wuu = atan	((M[p] - M[p+pu])	/	M.dxy);}
	else
	{wuu = 0;}
	if((wul + wll + wol + woo + wor + wrr + wur + wuu) == 0)
	{Z[p] = 0;}
	else
	{Z[p] = wul + wll + wol + woo + wor + wrr + wur + wuu;}
	}
}}}}}}}}
// hier werden Grids UL bis UU (im Uhrzeigersinn) erzeugt, die angeben, welchen Anteil des Inhalts einer benachbarten Rasterzelle in die Zielrasterzelle (zentrale Rasterzelle im 9er Feld) uebergen wird //
foreach p in UL do
{	if (p.x == 0 || p.y == 0)
	{UL[p] = 0;}
	else	{
		if((M[p] - M[p+pul]) < 0 && Z[p+pul] > 0) 
		{UL[p] = (atan((M[p+pul] - M[p])/(2 * M.dxy^2)^0.5))/Z[p+pul];}
		else
		{UL[p] = 0;}
		}
}
foreach p in LL do
{	if (p.x == 0)
	{LL[p] = 0;}
	else	{
		if((M[p] - M[p+pl]) < 0 && Z[p+pl] > 0) 
		{LL[p] = (atan((M[p+pl] - M[p])/M.dxy))/Z[p+pl];}
		else
		{LL[p] = 0;}
		}
}
foreach p in OL do
{	if (p.x == 0 || p.y == M.yanz - 1)
	{OL[p] = 0;}
	else	{
		if((M[p] - M[p+pol]) < 0 && Z[p+pol] > 0) 
		{OL[p] = (atan((M[p+pol] - M[p])/(2 * M.dxy^2)^0.5))/Z[p+pol];}
		else
		{OL[p] = 0;}
		}
}
foreach p in OO do
{	if (p.y == M.yanz - 1)
	{OO[p] = 0;}
	else	{
		if((M[p] - M[p+po]) < 0 && Z[p+po] > 0) 
		{OO[p] = (atan((M[p+po] - M[p])/M.dxy))/Z[p+po];}
		else
		{OO[p] = 0;}
		}
}
foreach p in OR do
{	if (p.x == M.xanz - 1 || p.y == M.yanz - 1)
	{OR[p] = 0;}
	else	{
		if((M[p] - M[p+por]) < 0 && Z[p+por] > 0) 
		{OR[p] = (atan((M[p+por] - M[p])/(2 * M.dxy^2)^0.5))/Z[p+por];}
		else
		{OR[p] = 0;}
		}
}
foreach p in RR do
{	if (p.x == M.xanz - 1)
	{RR[p] = 0;}
	else	{
		if((M[p] - M[p+pr]) < 0 && Z[p+pr] > 0) 
		{RR[p] = (atan((M[p+pr] - M[p])/M.dxy))/Z[p+pr];}
		else
		{RR[p] = 0;}
		}
}
foreach p in UR do
{	if (p.x == M.xanz - 1 || p.y == 0)
	{UR[p] = 0;}
	else	{
		if((M[p] - M[p+pur]) < 0 && Z[p+pur] > 0) 
		{UR[p] = (atan((M[p+pur] - M[p])/(2 * M.dxy^2)^0.5))/Z[p+pur];}
		else
		{UR[p] = 0;}
		}
}
foreach p in UU do
{	if (p.y == 0)
	{UU[p] = 0;}
	else	{
		if((M[p] - M[p+pu]) < 0 && Z[p+pu] > 0) 
		{UU[p] = (atan((M[p+pu] - M[p])/M.dxy))/Z[p+pu];}
		else
		{UU[p] = 0;}
		}
}
// in den folgenden drei Schritten wird nach der "multiple flow methode" die Einzugsgebietsgroeﬂe C ermittelt //
foreach p in Z do
{Z[p] = 1;}
foreach ploop in Loop do
{	ploop.x = 1;
	i = i +1;
	gefunden = 0;
	foreach p in Z do
	{	if (R[p] == i)
		{Z[p] = 1 + Z[p+pul] * UL[p] + Z[p+pl] * LL[p] + Z[p+pol] * OL[p] + Z[p+po] * OO[p] + Z[p+por] * OR[p] + Z[p+pr] * RR[p] + Z[p+pur] * UR[p] + Z[p+pu] * UU[p]; gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
setRandN(Z);
foreach p in C do
{C[p] = Z[p] * O.dxy^2;}
// in den folgenden drei Schritten wird die gesaugte Einzugsgebietsgroeﬂe CS ermittelt, wobei der t-Parameter die Staerke der Saugung steuert. Werte unter 10 (z.B.: 5) fuehren zu einer starken Saugung, Werte ueber 10 (z.B. 15) zu einer schwachen Saugung. Die gesaugte Einzugsgebietsgroeﬂe selbst stellt bereits einen Parameter Dar, der die raeumliche Relief-bedingte Feuchteverteilung in guter Weise annaehert //
foreach p in X do
{X[p] = C[p];}
foreach ploop in Loop do
{	ploop.x = 1;
	j = j +1;
	gefunden = 0;
	foreach p in X do
	{
	if ((((1/t^N[p])^exp(t^N[p])) * max9(p, X)) > X[p])
	{X[p] = (((1/t^N[p])^exp(t^N[p])) * max9(p, X)); gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
foreach p in CS do
{	if (isRand(p, M) == 0)
	{
	if(X[p] > C[p] || X[p+pul] > C[p+pul] || X[p+pl] > X[p+pl] || X[p+pol] > C[p+pol] || X[p+po] > C[p+po] || X[p+por] > C[p+por] || X[p+pr] > C[p+pr] || X[p+pur] > C[p+pur] || X[p+pu] > C[p+pu])
	{CS[p] = ln((X[p] + X[p+pul] + X[p+pu] + X[p+pur] + X[p+pl] + X[p+pr] + X[p+pol] + X[p+po] + X[p+por])/9);}
	else	
	{CS[p] = ln(X[p]);}
	}
}
setRandN (CS);	
showMatrix(CS);
// in den folgenden zwei Schritten wird der SAGA Bodenfeuchteindex SB ermittelt. Der a-Parameter muss bei den Settings definiert werden und sorgt dafuer, das nicht durch 0 dividiert wird //
foreach p in SB do
{SB[p] = exp(CS[p])/tan(N[p] + a);}
foreach p in SB do
{SB[p] = ln(SB[p]);}	
showMatrix(SB);
/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
