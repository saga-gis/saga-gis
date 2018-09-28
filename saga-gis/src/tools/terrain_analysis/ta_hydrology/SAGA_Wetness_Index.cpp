/**********************************************************
 * Version $Id: SAGA_Wetness_Index.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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

	Set_Author		("J.Boehner, O.Conrad (c) 2001");

	Set_Description	(_TW(
		"The 'SAGA Wetness Index' is, as the name says, similar to the "
		"'Topographic Wetness Index' (TWI), but it is based on a modified "
		"catchment area calculation ('Modified Catchment Area'), which does "
		"not think of the flow as very thin film. As result it predicts for "
		"cells situated in valley floors with a small vertical distance to "
		"a channel a more realistic, higher potential soil moisture compared "
		"to the standard TWI calculation."
	));

	Add_Reference(
		"Boehner, J., Koethe, R. Conrad, O., Gross, J., Ringeler, A., Selige, T.", "2002",
		"Soil Regionalisation by Means of Terrain Analysis and Process Parameterisation",
		"In: Micheli, E., Nachtergaele, F., Montanarella, L. [Ed.]: Soil Classification 2001. "
		"European Soil Bureau, Research Report No. 7, EUR 20398 EN, Luxembourg. pp.213-222.",
		SG_T("http://eusoils.jrc.ec.europa.eu/ESDB_Archive/eusoils_docs/esb_rr/EUR22646EN.pdf")
	);

	Add_Reference(
		"Boehner, J., Selige, T.", "2006",
		"Spatial prediction of soil attributes using terrain analysis and climate regionalisation",
		"In: Boehner, J., McCloy, K.R., Strobl, J. [Eds.]: SAGA - Analysis and Modelling Applications, "
		"Goettinger Geographische Abhandlungen, Goettingen: 13-28.",
		SG_T("http://downloads.sourceforge.net/saga-gis/gga115_02.pdf")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"WEIGHT"	, _TL("Weights"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"AREA"		, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"SLOPE"		, _TL("Catchment Slope"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"AREA_MOD"	, _TL("Modified Catchment Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"TWI"		, _TL("Topographic Wetness Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"SUCTION"	, _TL("Suction"),
		_TL("the lower this value is the stronger is the suction effect"),
		10.0, 0.0, true
	);

	Parameters.Add_Node("",
		"TWI_NODE"	, _TL("Topograpic Wetness Index Calculation"),
		_TL("")
	);

	Parameters.Add_Choice("TWI_NODE",
		"AREA_TYPE"	, _TL("Type of Area"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("total catchment area"),
			_TL("square root of catchment area"),
			_TL("specific catchment area")
		), 1
	);

	Parameters.Add_Choice("TWI_NODE",
		"SLOPE_TYPE", _TL("Type of Slope"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("local slope"),
			_TL("catchment slope")
		), 1
	);

	Parameters.Add_Double("",
		"SLOPE_MIN"	, _TL("Minimum Slope"),
		_TL(""),
		0.0, 0.0, true
	);

	Parameters.Add_Double("",
		"SLOPE_OFF"	, _TL("Offset Slope"),
		_TL(""),
		0.1, 0.0, true
	);

	Parameters.Add_Double("",
		"SLOPE_WEIGHT", _TL("Slope Weighting"),
		_TL("weighting factor for slope in index calculation"),
		1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSAGA_Wetness_Index::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM"     )->asGrid();
	m_pSlope	= Parameters("SLOPE"   )->asGrid();
	m_pArea		= Parameters("AREA"    )->asGrid();
	m_pAreaMod	= Parameters("AREA_MOD")->asGrid();
	m_pTWI		= Parameters("TWI"     )->asGrid();

	DataObject_Set_Colors(m_pArea   , 100, SG_COLORS_WHITE_BLUE);
	DataObject_Set_Colors(m_pAreaMod, 100, SG_COLORS_WHITE_BLUE);
	DataObject_Set_Colors(m_pSlope  , 100, SG_COLORS_YELLOW_RED);
	DataObject_Set_Colors(m_pTWI    , 100, SG_COLORS_RED_GREY_BLUE);

	m_pSlope->Set_Unit		(_TL("radians"));

	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	Get_Area();

	Get_Modified();

	Get_TWI();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSAGA_Wetness_Index::Get_Local_Maximum(CSG_Grid *pGrid, int x, int y)
{
	double	z	= pGrid->asDouble(x, y);

	for(int i=0, ix, iy; i<8; i++)
	{
		if( pGrid->Get_System().Get_Neighbor_Pos(i, x, y, ix, iy) && !pGrid->is_NoData(ix, iy) && pGrid->asDouble(ix, iy) > z )
		{
			z	= pGrid->asDouble(ix, iy);
		}
	}

	return( z );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Im Unterschied zu Freeman's urspruenglichen Verfahren
// wird der Winkel (atan(dz / dx)) und nicht das Gefaelle
// (dz / dx) fuer die Gewichtung der Abfluszanteile benutzt!

//---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_Area(void)
{
	const double	MFD_Converge	= 1.1;

	int		x, y, i, ix, iy;
	double	z, d, dz[8], dzSum, Area, Slope;

	CSG_Grid	*pWeight	= Parameters("WEIGHT")->asGrid();

	//-----------------------------------------------------
	Process_Set_Text(_TL("catchment area and slope..."));

	m_pArea ->Assign(0.0);
	m_pSlope->Assign(0.0);

	m_Suction.Create(*Get_System());
	m_Suction.Assign(0.0);

	double	Suction			= Parameters("SUCTION"     )->asDouble();
	double	Slope_Weight	= Parameters("SLOPE_WEIGHT")->asDouble();

	//-----------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		m_pDEM->Get_Sorted(n, x, y, true, false);

		if( m_pDEM->is_NoData(x, y) )
		{
			m_pArea ->Set_NoData(x, y);
			m_pSlope->Set_NoData(x, y);
			m_Suction.Set_NoData(x, y);
		}
		else
		{
			m_pDEM->Get_Gradient(x, y, Slope, d);

			d	= pow(Suction, Slope_Weight * Slope);
			m_Suction.Set_Value(x, y, pow(1.0 / d, exp(d)));

			Area	= m_pArea ->asDouble(x, y) + (!pWeight ? 1.0 : pWeight->is_NoData(x, y) ? 0.0 : pWeight->asDouble(x, y));
			Slope	= m_pSlope->asDouble(x, y) + Slope;

			m_pArea ->Set_Value(x, y, Area);
			m_pSlope->Set_Value(x, y, Slope / Area);

			for(i=0, dzSum=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
			{
				if( Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy) && !m_pDEM->is_NoData(ix, iy) && (d = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
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

						m_pArea ->Add_Value(ix, iy, d * Area);
						m_pSlope->Add_Value(ix, iy, d * Slope);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	*m_pArea	*= m_pArea->Get_Cellarea();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// in den folgenden drei Schritten wird die gesaugte
// Einzugsgebietsgroeße CS ermittelt, wobei der t-Parameter
// die Staerke der Saugung steuert. Werte unter 10 (z.B.  5)
// fuehren zu einer starken Saugung, Werte ueber 10 (z.B. 15)
// zu einer schwachen Saugung. Die gesaugte
// Einzugsgebietsgroeße selbst stellt bereits einen Parameter
// dar, der die raeumliche Relief-bedingte Feuchteverteilung
// in guter Weise annaehert

//---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_Modified(void)
{
	int			y;

	CSG_Grid	Area(*m_pArea);

	m_pAreaMod->Assign(m_pArea);

	//-----------------------------------------------------
	int		nChanges	= 1;

	for(int Iteration=1; nChanges && Process_Get_Okay(); Iteration++)
	{
		nChanges	= 0;

		#pragma omp parallel for private(y) reduction(+:nChanges)
		for(y=0; y<Get_NY(); y++)
		{
			Process_Get_Okay();

			for(int x=0; x<Get_NX(); x++)
			{
				if( !m_Suction.is_NoData(x, y) )
				{
					double z	= m_Suction.asDouble(x, y) * Get_Local_Maximum(&Area, x, y);

					if( z > Area.asDouble(x, y) )
					{
						nChanges++;

						Area.Set_Value(x, y, z);
					}
				}
			}
		}

		if( nChanges > 0 )
		{
			nChanges	= 0;

			#pragma omp parallel for private(y)
			for(y=0; y<Get_NY(); y++)
			{
				Process_Get_Okay();

				for(int x=0; x<Get_NX(); x++)
				{
					if( Area.asDouble(x, y) != m_pAreaMod->asDouble(x, y) )
					{
						nChanges++;

						m_pAreaMod->Set_Value(x, y, Area.asDouble(x, y));
					}
				}
			}
		}

		Process_Set_Text("pass %d (%d > 0)", Iteration, nChanges);
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("post-processing..."));

	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y++)
	{
		Process_Get_Okay();

		for(int x=0; x<Get_NX(); x++)
		{
			if( !Area.is_NoData(x, y) )
			{
				bool	bModify	= false;
				int		n		= 0;
				double	z		= 0.0;

				for(int iy=y-1; iy<=y+1; iy++)
				{
					for(int ix=x-1; ix<=x+1; ix++)
					{
						if( Area.is_InGrid(ix, iy) )
						{
							if( Area.asDouble(ix, iy) > m_pArea->asDouble(ix, iy) )
							{
								bModify	= true;
							}

							n	++;
							z	+= Area.asDouble(ix, iy);
						}
					}
				}

				m_pAreaMod->Set_Value(x, y, bModify ? z / n : Area.asDouble(x, y));
			}
			else
			{
				m_pAreaMod->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSAGA_Wetness_Index::Get_TWI(void)
{
	int		Area_Type	= Parameters("AREA_TYPE" )->asInt();
	int		Slope_Type	= Parameters("SLOPE_TYPE")->asInt();
	double	Slope_Min	= Parameters("SLOPE_MIN" )->asDouble() * M_DEG_TO_RAD;
	double	Slope_Off	= Parameters("SLOPE_OFF" )->asDouble() * M_DEG_TO_RAD;

	Process_Set_Text(_TL("topographic wetness index..."));

	#pragma omp parallel
	for(int y=0; y<Get_NY(); y++)
	{
		Process_Get_Okay();

		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pAreaMod->is_NoData(x, y) || m_pSlope->is_NoData(x, y) )
			{
				m_pTWI->Set_NoData(x, y);
			}
			else
			{
				double	s, a;

				if( Slope_Type == 1 )
				{
					s	= m_pSlope->asDouble(x, y);
				}
				else
				{
					m_pDEM->Get_Gradient(x, y, s, a);
				}

				s	= s + Slope_Off;
				s	= 6 * tan(Slope_Min < s ? s : Slope_Min);

				a	= m_pAreaMod->asDouble(x, y);

				switch( Area_Type )
				{
				case 1:	a	= sqrt(a);				break;	// square root of catchment area
				case 2:	a	= a / Get_Cellsize();	break;	// specific catchment area
				}

				m_pTWI->Set_Value(x, y, log(a / s));
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
// in den folgenden drei Schritten wird nach der "multiple flow methode" die Einzugsgebietsgroeße C ermittelt //
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
// in den folgenden drei Schritten wird die gesaugte Einzugsgebietsgroeße CS ermittelt, wobei der t-Parameter die Staerke der Saugung steuert. Werte unter 10 (z.B.: 5) fuehren zu einer starken Saugung, Werte ueber 10 (z.B. 15) zu einer schwachen Saugung. Die gesaugte Einzugsgebietsgroeße selbst stellt bereits einen Parameter Dar, der die raeumliche Relief-bedingte Feuchteverteilung in guter Weise annaehert //
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
