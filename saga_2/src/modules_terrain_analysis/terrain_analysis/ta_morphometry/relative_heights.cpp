
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Relative_Heights.cpp                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    e-mail:     conrad@geowiss.uni-hamburg.de          //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "relative_heights.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRelative_Heights::CRelative_Heights(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Relative Heights and Slope Positions"));

	Set_Author		(SG_T("J.Boehner, O.Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "HO"			, _TL("Slope Height"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "HU"			, _TL("Valley Depth"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "NH"			, _TL("Normalized Height"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SH"			, _TL("Standardized Height"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "MS"			, _TL("Mid-Slope Positon"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "W"			, _TL("w"),
		_TL(""),
		PARAMETER_TYPE_Double	,  0.5, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "T"			, _TL("t"),
		_TL(""),
		PARAMETER_TYPE_Double	, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "E"			, _TL("e"),
		_TL(""),
		PARAMETER_TYPE_Double	,  2.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRelative_Heights::On_Execute(void)
{
	double		w, t, e;
	CSG_Grid	*pDEM, *pHO, *pHU, *pNH, *pSH, *pMS;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM")	->asGrid();

	pHO			= Parameters("HO")	->asGrid();
	pHU			= Parameters("HU")	->asGrid();
	pNH			= Parameters("NH")	->asGrid();
	pSH			= Parameters("SH")	->asGrid();
	pMS			= Parameters("MS")	->asGrid();

	w			= Parameters("W")	->asDouble();
	t			= Parameters("T")	->asDouble();
	e			= Parameters("E")	->asDouble();

	//-----------------------------------------------------
	Message_Add(_TL("Pass 1"));
	Get_Heights(pDEM, pHO, true , w, t, e);

	Message_Add(_TL("Pass 2"));
	Get_Heights(pDEM, pHU, false, w, t, e);

	Get_Results(pDEM, pHO, pHU, pNH, pSH);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pNH->is_NoData(x, y) )
			{
				pMS->Set_NoData(x, y);
			}
			else
			{
				pMS->Set_Value(x, y, fabs(2.0 * pNH->asDouble(x, y) - 1.0));
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
bool CRelative_Heights::Get_Heights(CSG_Grid *pDEM, CSG_Grid *pH, bool bInverse, double w, double t, double e)
{
	CSG_Grid	Inverse;

	//-----------------------------------------------------
	if( bInverse )
	{
		Inverse.Create(pDEM);
		Inverse.Assign(pDEM);
		Inverse.Invert();
		pDEM	= &Inverse;
	}

	//-----------------------------------------------------
	Get_Heights_Catchment(pDEM, pH, w);

	Get_Heights_Modified (pDEM, pH, t, e);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRelative_Heights::Get_Heights_Catchment(CSG_Grid *pDEM, CSG_Grid *pH, double Weight)
{
	const double	MFD_Converge	= 1.1;

	int			n, x, y, i, ix, iy;
	double		z, d, dz[8], dzSum, c, w, h;
	CSG_Grid	C, W;

	//-----------------------------------------------------
	Process_Set_Text(_TL("Relative heights calculation..."));

	C.Create(*Get_System());
	W.Create(*Get_System());

	C.	Assign(Get_System()->Get_Cellarea());
	W.	Assign(0.0);
	pH->Assign(0.0);

	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( pDEM->is_NoData(n) )
		{
			pH->Set_NoData(n);
		}
	}

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( pDEM->Get_Sorted(n, x, y) )
		{
			z		= pDEM->asDouble(x, y);
			c		= C    .asDouble(x, y);
			w		= W    .asDouble(x, y) + pow(1.0 / c, Weight);		//{W[p] = (1/C[p])^w;}
			h		= pH  ->asDouble(x, y) + pow(1.0 / c, Weight) * z;	//{H[p] = (1/C[p])^w * M[p];}

			pH->Set_Value(x, y, h / w - z);	//{H[p] = -1 * (M[p] - H[p]/W[p]);}

			for(i=0, dzSum=0.0; i<8; i++)
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

				if( pDEM->is_InGrid(ix, iy) && (d = z - pDEM->asDouble(ix, iy)) > 0.0 )
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

						C  .Add_Value(ix, iy, d * c);
						W  .Add_Value(ix, iy, d * w);	//{W[p] = W[p] + W[p+pul] * UL[p] + W[p+pl] * LL[p] + W[p+pol] * OL[p] + W[p+po] * OO[p] + W[p+por] * OR[p] + W[p+pr] * RR[p] + W[p+pur] * UR[p] + W[p+pu] * UU[p]; gefunden = 1;}
						pH->Add_Value(ix, iy, d * h);	//{H[p] = H[p] + H[p+pul] * UL[p] + H[p+pl] * LL[p] + H[p+pol] * OL[p] + H[p+po] * OO[p] + H[p+por] * OR[p] + H[p+pr] * RR[p] + H[p+pur] * UR[p] + H[p+pu] * UU[p]; gefunden = 1;}
					}
				}
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
// in den folgenden drei Schritten werden Maxima der
// relativen Höhe H in Abhängigkeit der Neigung gesaugt.
// Der e-Parameter steuert die stärke der Maximum-Streckung

//---------------------------------------------------------
bool CRelative_Heights::Get_Heights_Modified(CSG_Grid *pDEM, CSG_Grid *pH, double t, double e)
{
	bool		bRecalculate;
	int			x, y, i, ix, iy, n;
	double		z, d;
	CSG_Grid	H, H_Last, T;

	//-----------------------------------------------------
	Process_Set_Text(_TL("Modify: pre-processing..."));

	T.Create(pH);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pDEM->Get_Gradient(x, y, z, d) && !pH->is_NoData(x, y) )
			{
				pH->Set_Value(x, y, d = pow(pH->asDouble(x, y), e));	// {X[p] = H[p]^e;}
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

	H     .Create(*pH);
	H_Last.Create(*pH);

	//-----------------------------------------------------
	for(i=0, n=1; n>0; i++)
	{
		for(y=0, n=0; y<Get_NY() && Process_Get_Okay(false); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !T.is_NoData(x, y) )
				{
					z	= T.asDouble(x, y) * Get_Local_Maximum(&H, x, y);

					if( z  > H.asDouble(x, y) )
					{
						n++;

						H.Set_Value(x, y, z);
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
					if( H.asDouble(x, y) != H_Last.asDouble(x, y) )
					{
						n++;

						H_Last.Set_Value(x, y, H.asDouble(x, y));
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
			for(iy=y-1, bRecalculate=false; iy<=y+1 && !bRecalculate; iy++)
			{
				for(ix=x-1; ix<=x+1 && !bRecalculate; ix++)
				{
					if( H.is_InGrid(ix, iy) && pH->is_InGrid(ix, iy) && H.asDouble(ix, iy) > pH->asDouble(ix, iy) )
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
						if( H.is_InGrid(ix, iy) )
						{
							n++;
							z	+= H.asDouble(ix, iy);
						}
					}
				}

				z	= z / (double)n;
			}
			else	
			{
				z	= H.asDouble(x, y);
			}

			pH->Set_Value(x, y, pow(z, 1.0 / e));
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
double CRelative_Heights::Get_Local_Maximum(CSG_Grid *pGrid, int x, int y)
{
	if( pGrid->is_InGrid(x, y) )
	{
		double	z	= pGrid->asDouble(x, y);

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( pGrid->is_InGrid(ix, iy) && pGrid->asDouble(ix, iy) > z )
			{
				z	= pGrid->asDouble(ix, iy);
			}
		}

		return( z );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// auf Grundlage von HO und HU wird die Normierte Höhe
// (NH) mit Höhenwerten zwischen minimal 0 (=unten) und
// maximal 1 (= oben) wie beim NDVI bestimmt. Die
// Standardisierte Höhe SH streckt das Relief auf ein
// Höhenspektrum zwischen theoretisch 0 (=unten) und
// dem Maximum der jeweiligen Scheitelbereiche, so dass
// die ursprüngliche Reliefenergie erhalten bleibt.
// Selbstverständlich muss für Anwendungen überprüft
// werden, ob und welche relativen Höhen (HU, HO, SH)
// logarithmiert dargestellt werden. Als Beispiel ist
// die Standardisierte Höhe logarithmiert berechnet (SHL)

//---------------------------------------------------------
bool CRelative_Heights::Get_Results(CSG_Grid *pDEM, CSG_Grid *pHO, CSG_Grid *pHU, CSG_Grid *pNH, CSG_Grid *pSH)
{
	int		x, y;
	double	ho, hu, nh, hMin;

	Process_Set_Text(_TL("Final processing..."));

	hMin	= pDEM->Get_ZMin();

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pDEM->is_NoData(x, y) && !pHO->is_NoData(x, y) && !pHU->is_NoData(x, y) )
			{
				ho	= pHO->asDouble(x, y);
				hu	= pHU->asDouble(x, y);

				nh	= 0.5 * (1.0 + (ho - hu) / (ho + hu));

				pNH->Set_Value(x, y, nh);
				pSH->Set_Value(x, y, nh * (pDEM->asDouble(x, y) - hMin) + hMin);	//, nh * pDEM->asDouble(x, y));
			}
			else
			{
				pNH->Set_NoData(x, y);
				pSH->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//        The original BSL script by J.Böhner
//---------------------------------------------------------

/*
Matrix Loop, O("sch-hoe.grd"), N("sch-nei.grd"), M, R, UL, LL, OL, OO, OR, RR, UR, UU, X, Y, Z, C, W, H, HO, HU, NH, SH, SHL;
Point ploop, p, pul, pu, pur, pl, pr, pol, po, por;
Float w, e, wul, wll, wol, woo, wor, wrr, wur, wuu;
Integer t, h, i, j, k, l, m, n, o, gefunden;

w = 0.5;
e = 2; 
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
W = O;
H = O;
HO = O;
HU = O;
NH = O;
SH = O;
SHL = O;

Loop.xanz = 100000;
Loop.yanz = 1;
h = 0;
i = 0;
j = 0;
k = 0;
l = 0;
m = 0;
n = 0;
o = 0;

// Das gesamte Verfahren besteht aus zwei identischen Schritten zur Bestimmung von Höhe über Kulmination (HO) und Höhe unter Kulmination (HU). Für HO wird das DGM zunächst umgedreht //
foreach p in M do 
{M[p] = -1 * O[p];}
// hier wird eine Hilfsmatrix X erzeugt, die in der folgenden Schleife fafür sorgt, dass noch nicht attributisierte Rasterzellen in Ihrer Position identifiziert werden können //
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
// hier werden Grids UL bis UU (im Uhrzeigersinn) erzeugt, die angeben, welchen Anteil des Inhalts einer benachbarten Rasterzelle in die Zielrasterzelle (zentrale Rasterzelle im 9er Feld) übergen wird //
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
// in den folgenden drei Schritten wird nach der "multiple flow methode" die Einzugsgebietsgröße C ermittelt //
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
{C[p] = Z[p] * M.dxy^2;}
// in den folgenden zwei Schritten werden Gewichte umgekehrt-proportional zur Einzugsgebietsgröße nach der multiple flow methode aufsummiert. Der w-parameter steuert die stärke der Gewichtung //
foreach p in W do
{W[p] = (1/C[p])^w;}
foreach ploop in Loop do
{	ploop.x = 1;
	k = k +1;
	gefunden = 0;
	foreach p in W do
	{	if (R[p] == k)
		{W[p] = W[p] + W[p+pul] * UL[p] + W[p+pl] * LL[p] + W[p+pol] * OL[p] + W[p+po] * OO[p] + W[p+por] * OR[p] + W[p+pr] * RR[p] + W[p+pur] * UR[p] + W[p+pu] * UU[p]; gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
setRandN(W);
// in den folgenden zwei Schritten werden Höhenwerte und Gewichte nach der multiple flow methode in der Matrix H aufsummiert, wobei lokale Maxima im Einzugsgebiet am stärksten in H eingehen. Der w-Parameter steuert die Stärke der Maximumgewichtung //
foreach p in H do
{H[p] = M[p] * (1/C[p])^w;}
foreach ploop in Loop do
{	ploop.x = 1;
	l = l +1;
	gefunden = 0;
	foreach p in H do
	{	if (R[p] == l)
		{H[p] = H[p] + H[p+pul] * UL[p] + H[p+pl] * LL[p] + H[p+pol] * OL[p] + H[p+po] * OO[p] + H[p+por] * OR[p] + H[p+pr] * RR[p] + H[p+pur] * UR[p] + H[p+pu] * UU[p]; gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
setRandN(H);
// im folgenden Schritt wird eine relative Höhe H über dem gewichteten arithmetischen Mittel der Einzugsgebietsgröße bestimmt //
foreach p in H do
{	if (isRand(p, M) == 0)
	{H[p] = -1 * (M[p] - H[p]/W[p]);}
}
setRandN(H);
// in den folgenden drei Schritten werden Maxima der relativen Höhe H in Abhängigkeit der Neigung gesaugt. Der e-Parameter steuert die stärke der Maximum-Streckung //
foreach p in X do
{X[p] = H[p]^e;}
foreach ploop in Loop do
{	ploop.x = 1;
	m = m +1;
	gefunden = 0;
	foreach p in X do
	{
	if ((((1/t^N[p])^exp(t^N[p])) * max9(p, X)) > X[p])
	{X[p] = (((1/t^N[p])^exp(t^N[p])) * max9(p, X)); gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
foreach p in HO do
{	if (isRand(p, M) == 0)
	{
	if(H[p]^e < X[p] || H[p+pul]^e < X[p+pul] || H[p+pl]^e < X[p+pl] || H[p+pol]^e < X[p+pol] || H[p+po]^e < X[p+po] || H[p+por]^e < X[p+por] || H[p+pr]^e < X[p+pr] || H[p+pur]^e < X[p+pur] || H[p+pu]^e < X[p+pu])
	{HO[p] = ((X[p] + X[p+pul] + X[p+pu] + X[p+pur] + X[p+pl] + X[p+pr] + X[p+pol] + X[p+po] + X[p+por])/9)^(1/e);}
	else	
	{HO[p] = X[p]^(1/e);}
	}
}
setRandN(HO);	
foreach p in Z do
{
h = 0;
i = 0;
j = 0;
k = 0;
l = 0;
m = 0;
}
// hier wird das DGM nicht umgedreht //
foreach p in M do 
{M[p] = O[p];}
// hier wird eine Hilfsmatrix X erzeugt, die in der folgenden Schleife fafür sorgt, dass noch nicht attributisierte Rasterzellen in Ihrer Position identifiziert werden können //
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
// hier werden Grids UL bis UU (im Uhrzeigersinn) erzeugt, die angeben, welchen Anteil des Inhalts einer benachbarten Rasterzelle in die Zielrasterzelle (zentrale Rasterzelle im 9er Feld) übergen wird //
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
// in den folgenden drei Schritten wird nach der "multiple flow methode" die Einzugsgebietsgröße C ermittelt //
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
{C[p] = Z[p] * M.dxy^2;}
// in den folgenden zwei Schritten werden Gewichte umgekehrt-proportional zur Einzugsgebietsgröße nach der multiple flow methode aufsummiert. Der w-parameter steuert die stärke der Gewichtung //
foreach p in W do
{W[p] = (1/C[p])^w;}
foreach ploop in Loop do
{	ploop.x = 1;
	k = k +1;
	gefunden = 0;
	foreach p in W do
	{	if (R[p] == k)
		{W[p] = W[p] + W[p+pul] * UL[p] + W[p+pl] * LL[p] + W[p+pol] * OL[p] + W[p+po] * OO[p] + W[p+por] * OR[p] + W[p+pr] * RR[p] + W[p+pur] * UR[p] + W[p+pu] * UU[p]; gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
setRandN(W);
// in den folgenden zwei Schritten werden Höhenwerte und Gewichte nach der multiple flow methode in der Matrix H aufsummiert, wobei lokale Maxima im Einzugsgebiet am stärksten in H eingehen. Der w-Parameter steuert die Stärke der Maximumgewichtung //
foreach p in H do
{H[p] = M[p] * (1/C[p])^w;}
foreach ploop in Loop do
{	ploop.x = 1;
	l = l +1;
	gefunden = 0;
	foreach p in H do
	{	if (R[p] == l)
		{H[p] = H[p] + H[p+pul] * UL[p] + H[p+pl] * LL[p] + H[p+pol] * OL[p] + H[p+po] * OO[p] + H[p+por] * OR[p] + H[p+pr] * RR[p] + H[p+pur] * UR[p] + H[p+pu] * UU[p]; gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
setRandN(H);
// im folgenden Schritt wird eine relative Höhe H über dem gewichteten arithmetischen Mittel der Einzugsgebietsgröße bestimmt //
foreach p in H do
{	if (isRand(p, M) == 0)
	{H[p] = -1 * (M[p] - H[p]/W[p]);}
}
setRandN(H);
// in den folgenden drei Schritten werden Maxima der relativen Höhe H in Abhängigkeit der Neigung gesaugt. Der e-Parameter steuert die stärke der Maximum-Streckung //
foreach p in X do
{X[p] = H[p]^e;}
foreach ploop in Loop do
{	ploop.x = 1;
	m = m +1;
	gefunden = 0;
	foreach p in X do
	{
	if ((((1/t^N[p])^exp(t^N[p])) * max9(p, X)) > X[p])
	{X[p] = (((1/t^N[p])^exp(t^N[p])) * max9(p, X)); gefunden = 1;}
	}
	if (gefunden == 0)
	{ploop.x = 100000;}
}
foreach p in HU do
{	if (isRand(p, M) == 0)
	{
	if(H[p]^e < X[p] || H[p+pul]^e < X[p+pul] || H[p+pl]^e < X[p+pl] || H[p+pol]^e < X[p+pol] || H[p+po]^e < X[p+po] || H[p+por]^e < X[p+por] || H[p+pr]^e < X[p+pr] || H[p+pur]^e < X[p+pur] || H[p+pu]^e < X[p+pu])
	{HU[p] = (((X[p] + X[p+pul] + X[p+pu] + X[p+pur] + X[p+pl] + X[p+pr] + X[p+pol] + X[p+po] + X[p+por])/9)^(1/e));}
	else	
	{HU[p] = (X[p]^(1/e));}
	}
}
setRandN (HU);
showMatrix(HO);
showMatrix(HU);
// auf Grundlage von HO und HU wird die Normierte Höhe (NH) mit Höhenwerten zwischen minimal 0 (=unten) und maximal 1 (= oben) wie beim NDVI bestimmt. Die Standardisierte Höhe SH streckt das Relief auf ein Höhenspektrum zwischen theoretisch 0 (=unten) und dem Maximum der jeweiligen Scheitelbereiche, so dass die ursprüngliche Reliefenergie erhalten bleibt. Selbstverständlich muss für Anwendungen überprüft werden, ob und welche relativen Höhen (HU, HO, SH) logarithmiert dargestellt werden. Als Beispiel ist die Standardisierte Höhe logarithmiert berechnet (SHL) //
foreach p in NH do
{NH[p] = 0.5 * (1 + ((HO[p] - HU[p])/(HO[p] + HU[p])));}
showMatrix(NH);
foreach p in SH do
{SH[p] = NH[p] * O[p];}
showMatrix(SH);
foreach p in SHL do
{SHL[p] = ln(SH[p]);}
showMatrix(SHL);
/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
