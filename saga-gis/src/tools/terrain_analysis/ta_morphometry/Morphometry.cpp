
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Morphometry.cpp                    //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Morphometry.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMorphometry::CMorphometry(void)
{
	Set_Name		(_TL("Slope, Aspect, Curvature"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Calculates the local morphometric terrain parameters slope, aspect and if supported "
		"by the chosen method also the curvature. Besides tangential curvature also its "
		"horizontal and vertical components (i.e. plan and profile curvature) can be calculated."
	));

	Add_Reference("Travis, M.R., Elsner, G.H., Iverson, W.D., Johnson, C.G.", "1975",
		"VIEWIT: computation of seen areas, slope, and aspect for land-use planning",
		"USDA F.S. Gen. Tech. Rep. PSW-11/1975, 70p. Berkeley, California, U.S.A."
	);

	Add_Reference("Tarboton, D.G.", "1997",
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",
		"Water Resources Research, Vol.33, No.2, p.309-319."
	);

	Add_Reference("Horn, B. K.", "1981",
		"Hill shading and the relectance map",
		"Proceedings of the IEEE, v. 69, no. 1, p.14-47."
	);

	Add_Reference("Beasley, D.B., Huggins, L.F.", "1982",
		"ANSWERS: User's manual",
		"U.S. EPA-905/9-82-001, Chicago, IL. 54pp."
	);

	Add_Reference("Costa-Cabral, M., Burges, S.J.", "1994",
		"Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas",
		"Water Resources Research, v. 30, no. 6, p.1681-1692."
	);

	Add_Reference("Evans, I.S.", "1979",
		"An integrated system of terrain analysis and slope mapping",
		"Final report on grant DA-ERO-591-73-G0040, University of Durham, England."
	);

	Add_Reference("Bauer, J., Rohdenburg, H., Bork, H.-R.", "1985",
		"Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse",
		"In: Bork, H.-R., Rohdenburg, H. [Eds.]: Parameteraufbereitung fuer deterministische Gebietswassermodelle, Grundlagenarbeiten zur Analyse von Agrar-Oekosystemen, Landschaftsgenese und Landschaftsoekologie, H.10, p.1-15."
	);

	Add_Reference("Heerdegen, R.G., Beran, M.A.", "1982",
		"Quantifying source areas through land surface curvature",
		"Journal of Hydrology, Vol.57."
	);

	Add_Reference("Olaya, V.", "2006",
		"Basic Land-Surface Parameters",
		"In: Hengl, T., Reuter, H.I. [Eds.]: Geomorphometry: Concepts, Software, Applications. Developments in Soil Science, Elsevier, Vol.33, 141-169."
	);

	Add_Reference("Zevenbergen, L.W., Thorne, C.R.", "1987",
		"Quantitative analysis of land surface topography",
		"Earth Surface Processes and Landforms, 12: 47-56."
	);

	Add_Reference("Haralick, R.M.", "1983",
		"Ridge and valley detection on digital images",
		"Computer Vision, Graphics and Image Processing, Vol.22, No.1, p.28-38."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "ASPECT"	, _TL("Aspect"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "C_GENE"	, _TL("General Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_PROF"	, _TL("Profile Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_PLAN"	, _TL("Plan Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_TANG"	, _TL("Tangential Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_LONG"	, _TL("Longitudinal Curvature"),
		_TL("Zevenbergen & Thorne (1987) refer to this as profile curvature"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_CROS"	, _TL("Cross-Sectional Curvature"),
		_TL("Zevenbergen & Thorne (1987) refer to this as plan curvature"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_MINI"	, _TL("Minimal Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_MAXI"	, _TL("Maximal Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_TOTA"	, _TL("Total Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "C_ROTO"	, _TL("Flow Line Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("maximum slope (Travis et al. 1975)"),
			_TL("maximum triangle slope (Tarboton 1997)"),
			_TL("least squares fitted plane (Horn 1981, Costa-Cabral & Burgess 1996)"),
			_TL("6 parameter 2nd order polynom (Evans 1979)"),
			_TL("6 parameter 2nd order polynom (Heerdegen & Beran 1982)"),
			_TL("6 parameter 2nd order polynom (Bauer, Rohdenburg, Bork 1985)"),
			_TL("9 parameter 2nd order polynom (Zevenbergen & Thorne 1987)"),
			_TL("10 parameter 3rd order polynom (Haralick 1983)")
		), 6
	);

	Parameters.Add_Choice(
		"SLOPE" , "UNIT_SLOPE"	, _TL("Unit"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("radians"),
			_TL("degree"),
			_TL("percent rise")
		), 0
	);

	Parameters.Add_Choice(
		"ASPECT", "UNIT_ASPECT"	, _TL("Unit"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("radians"),
			_TL("degree")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMorphometry::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		bool	bOn;
		
		bOn	= pParameter->asInt() >= 3 || pParameter->asInt() == 0;
		pParameters->Set_Enabled("C_GENE", bOn);
		pParameters->Set_Enabled("C_PROF", bOn);
		pParameters->Set_Enabled("C_PLAN", bOn);

		bOn	= pParameter->asInt() >= 3;
		pParameters->Set_Enabled("C_TANG", bOn);
		pParameters->Set_Enabled("C_LONG", bOn);
		pParameters->Set_Enabled("C_CROS", bOn);
		pParameters->Set_Enabled("C_MINI", bOn);
		pParameters->Set_Enabled("C_MAXI", bOn);
		pParameters->Set_Enabled("C_TOTA", bOn);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMorphometry::On_Execute(void)
{
	m_pDTM		= Parameters("ELEVATION")->asGrid();

	m_pSlope	= Parameters("SLOPE"    )->asGrid();
	m_pAspect	= Parameters("ASPECT"   )->asGrid();

	m_pC_Gene	= Parameters("C_GENE"   )->asGrid();
	m_pC_Prof	= Parameters("C_PROF"   )->asGrid();
	m_pC_Plan	= Parameters("C_PLAN"   )->asGrid();
	m_pC_Tang	= Parameters("C_TANG"   )->asGrid();
	m_pC_Long	= Parameters("C_LONG"   )->asGrid();
	m_pC_Cros	= Parameters("C_CROS"   )->asGrid();
	m_pC_Mini	= Parameters("C_MINI"   )->asGrid();
	m_pC_Maxi	= Parameters("C_MAXI"   )->asGrid();
	m_pC_Tota	= Parameters("C_TOTA"   )->asGrid();
	m_pC_Roto	= Parameters("C_ROTO"   )->asGrid();

	int	Method	= Parameters("METHOD"   )->asInt ();

	if( Method == 0 )
	{
		m_pC_Tang = m_pC_Long = m_pC_Cros = m_pC_Mini = m_pC_Maxi = m_pC_Tota = m_pC_Roto = NULL;
	}
	else if( Method < 3 )
	{
		m_pC_Gene = m_pC_Prof = m_pC_Plan =
		m_pC_Tang = m_pC_Long = m_pC_Cros = m_pC_Mini = m_pC_Maxi = m_pC_Tota = m_pC_Roto = NULL;
	}

	//-----------------------------------------------------
	DataObject_Set_Colors(m_pSlope , 11, SG_COLORS_RED_GREEN    ,  true);
	DataObject_Set_Colors(m_pAspect, 11, SG_COLORS_ASPECT_3     , false);
	DataObject_Set_Colors(m_pC_Gene, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Prof, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Plan, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Tang, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Long, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Cros, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Mini, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Maxi, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(m_pC_Tota, 11, SG_COLORS_YELLOW_RED   , false);
	DataObject_Set_Colors(m_pC_Roto, 11, SG_COLORS_RED_GREY_BLUE,  true);

	//-----------------------------------------------------
	m_Unit_Slope	= Parameters("UNIT_SLOPE" )->asInt();

	if( m_Unit_Slope == 0 )
	{
		m_pSlope->Set_Unit(_TL("Radians"));
	}
	else if( m_Unit_Slope == 1 )
	{
		m_pSlope->Set_Unit(_TL("Degree"));
	}
	else // if( m_Unit_Slope == 2 )
	{
		m_pSlope->Set_Unit(_TL("Percent"));
	}

	//-----------------------------------------------------
	m_Unit_Aspect	= Parameters("UNIT_ASPECT")->asInt();

	if( m_Unit_Aspect == 0 )
	{
		m_pAspect->Set_Unit(_TL("Radians"));
	}
	else // if( m_Unit_Aspect == 1 )
	{
		m_pAspect->Set_Unit(_TL("Degree"));
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_NoData(x, y) )
			{
				Set_NoData(x, y);
			}
			else switch( Method )
			{
			case  0: Set_MaximumSlope(x, y); break;
			case  1: Set_Tarboton    (x, y); break;
			case  2: Set_LeastSquare (x, y); break;
			case  3: Set_Evans       (x, y); break;
			case  4: Set_Heerdegen   (x, y); break;
			case  5: Set_BRM         (x, y); break;
			default: Set_Zevenbergen (x, y); break;
			case  7: Set_Haralick    (x, y); break;
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
// Indexing of the Submatrix:
//
//  +-------+    +-------+    +-------+
//  | 7 0 1 |    | 2 5 8 |    | 8 5 2 |
//  | 6 * 2 | => | 1 4 7 | or | 7 4 1 |
//  | 5 4 3 |    | 0 3 6 |    | 6 3 0 |
//  +-------+    +-------+    +-------+
//
//---------------------------------------------------------
inline void CMorphometry::Get_SubMatrix3x3(int x, int y, double Z[9], int Orientation)
{
	static const int	Indexes[][8]	=
	{
		{ 5, 8, 7, 6, 3, 0, 1, 2 },
		{ 5, 2, 1, 0, 3, 6, 7, 8 }
	};

	int	*Index	= (int *)Indexes[Orientation];

	double	z	= m_pDTM->asDouble(x, y);

	Z[4]		= 0.0;

	for(int i=0; i<8; i++)
	{
		int ix	= Get_xTo(i, x);
		int iy	= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) )
		{
			Z[Index[i]]	= m_pDTM->asDouble(ix, iy) - z;
		}
		else
		{
			ix	= Get_xTo(i + 4, x);
			iy	= Get_yTo(i + 4, y);

			if( m_pDTM->is_InGrid(ix, iy) )
			{
				Z[Index[i]]	= z - m_pDTM->asDouble(ix, iy);
			}
			else
			{
				Z[Index[i]]	= 0.0;
			}
		}
	}
}

//---------------------------------------------------------
inline void CMorphometry::Get_SubMatrix5x5(int x, int y, double Z[25])
{
	double	z	= m_pDTM->asDouble(x,y);

	for(int i=0, iy=y-2; iy<=y+2; iy++)
	{
		int	jy	= iy < 0 ? 0 : (iy >= Get_NY() ? Get_NY() - 1 : iy);

		for(int ix=x-2; ix<=x+2; ix++, i++)
		{
			int	jx	= ix < 0 ? 0 : (ix >= Get_NX() ? Get_NX() - 1 : ix);

			Z[i]	= m_pDTM->is_InGrid(jx, jy) ? m_pDTM->asDouble(jx, jy) - z : 0.0;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SET_NODATA(grid)		if( grid ) grid->Set_NoData(x, y);
#define SET_VALUE(grid, value)	if( grid ) grid->Set_Value(x, y, value);

//---------------------------------------------------------
inline void CMorphometry::Set_NoData(int x, int y)
{
	SET_NODATA(m_pSlope )
	SET_NODATA(m_pAspect)
	SET_NODATA(m_pC_Gene)
	SET_NODATA(m_pC_Prof)
	SET_NODATA(m_pC_Plan)
	SET_NODATA(m_pC_Tang)
	SET_NODATA(m_pC_Long)
	SET_NODATA(m_pC_Cros)
	SET_NODATA(m_pC_Mini)
	SET_NODATA(m_pC_Maxi)
	SET_NODATA(m_pC_Tota)
	SET_NODATA(m_pC_Roto)
}

//---------------------------------------------------------
inline void CMorphometry::Set_Gradient(int x, int y, double Slope, double Aspect)
{
	if( m_Unit_Slope == 1 )
	{
		SET_VALUE(m_pSlope, Slope * M_RAD_TO_DEG);
	}
	else if( m_Unit_Slope == 2 )
	{
		SET_VALUE(m_pSlope, 100.0 * tan(Slope));
	}
	else
	{
		SET_VALUE(m_pSlope, Slope);
	}

	//-----------------------------------------------------
	if( m_Unit_Aspect == 1 && Aspect >= 0.0 )
	{
		SET_VALUE(m_pAspect, Aspect * M_RAD_TO_DEG);
	}
	else
	{
		SET_VALUE(m_pAspect, Aspect);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CMorphometry::Set_From_Polynom(int x, int y, double r, double t, double s, double p, double q)
{
	//-----------------------------------------------------
	double	p2_q2	= p*p + q*q;

	Set_Gradient(x, y, atan(sqrt(p2_q2)),
		  p != 0.0 ? M_PI_180 + atan2(q, p)
		: q >  0.0 ? M_PI_270
		: q <  0.0 ? M_PI_090
		: m_pAspect ? m_pAspect->Get_NoData_Value() : -1
	);

	//-----------------------------------------------------
	if( p2_q2 )
	{
		double	spq = s * p * q, p2 = p*p, q2 = q*q;	r	*= 2;	t	*= 2;

		SET_VALUE(m_pC_Gene, -2 * (r + t));
		SET_VALUE(m_pC_Prof, -(r * p2 + t * q2 + 2 * spq) / (p2_q2 * pow(1 + p2_q2, 1.5)));
		SET_VALUE(m_pC_Plan, -(t * p2 + r * q2 - 2 * spq) / (        pow(    p2_q2, 1.5)));
		SET_VALUE(m_pC_Tang, -(t * p2 + r * q2 - 2 * spq) / (p2_q2 * pow(1 + p2_q2, 0.5)));
		SET_VALUE(m_pC_Long, -2 * (r * p2 + t * q2 + spq) / (p2_q2                      ));
		SET_VALUE(m_pC_Cros, -2 * (t * p2 + r * q2 - spq) / (p2_q2                      ));
		SET_VALUE(m_pC_Mini, -r/2 - t/2 - sqrt(0.5 * (r - t)*(r - t) + s*s));
		SET_VALUE(m_pC_Maxi, -r/2 - t/2 + sqrt(0.5 * (r - t)*(r - t) + s*s));
		SET_VALUE(m_pC_Tota, r*r + 2 * s*s + t*t);
		SET_VALUE(m_pC_Roto, (p2 - q2) * s - p * q * (r - t));	// rotor
	//	SET_VALUE(m_pC_Gaus, (r * t - 2 * s*s) / (1 + p2_q2));	// total gaussian
	}
}


///////////////////////////////////////////////////////////
//														 //
//					The Methods							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Maximum Slope (Travis et al., 1975, Peucker & Douglas, 1975))
//
// Travis, M.R., Elsner, G.H., Iverson, W.D., and Johnson, C.G. 1975:
//		VIEWIT: computation of seen areas, slope, and aspect for land-use planning.
//		USDA F.S. Gen. Tech. Rep. PSW-11/1975, 70p. Berkeley, California, U.S.A.
//
//---------------------------------------------------------
void CMorphometry::Set_MaximumSlope(int x, int y)
{
	int		i, ix, iy, j, Aspect;
	double	z, Z[8], Slope, Curv, hCurv, a, b;

	//-----------------------------------------------------
	z		= m_pDTM->asDouble(x, y);
    Slope	= Curv	= 0.0;
	Aspect	= -1;

	for(i=0; i<8; i++)
	{
		if( !m_pDTM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) )
		{
			Z[i]	= 0.0;
		}
		else
		{
			Z[i]	= atan((z - m_pDTM->asDouble(ix, iy)) / Get_Length(i));
			Curv	+= Z[i];

			if( Z[i] > Slope )
			{
				Aspect	= i;
				Slope	= Z[i];
			}
		}
	}

	Set_Gradient(x, y, Slope, Aspect * M_PI_045);

	//-------------------------------------------------
	if( Aspect < 0.0 )
	{
		SET_NODATA(m_pAspect);

		SET_NODATA(m_pC_Gene);
		SET_NODATA(m_pC_Prof);
		SET_NODATA(m_pC_Plan);
	}
	else
	{
		//---------------------------------------------
		// Let's now estimate the plan curvature...

		for(i=Aspect+1, j=0, a=0.0; i<Aspect+8; i++, j++)
		{
			if( Z[i % 8] < 0.0 )
			{
				a	= j + Z[(i - 1) % 8] / (Z[(i - 1) % 8] - Z[i % 8]);
				break;
			}
		}

		if( a != 0.0 )
		{
			for(i=Aspect+7, j=0, b=0.0; i>Aspect; i--, j++)
			{
				if( Z[i % 8] < 0.0 )
				{
					b	= j + Z[(i + 1) % 8] / (Z[(i + 1) % 8] - Z[i % 8]);
					break;
				}
			}

			hCurv	=  45.0 * (a + b) - 180.0;
		}
		else
		{
			hCurv	=  180.0;
		}

		//---------------------------------------------
		SET_VALUE(m_pC_Gene, Curv);
		SET_VALUE(m_pC_Prof, Z[Aspect] + Z[(Aspect + 4) % 8]);
		SET_VALUE(m_pC_Plan, hCurv);
	}
}

//---------------------------------------------------------
// Maximum Triangle Slope
//
// Tarboton, D.G. (1997):
//		'A new method for the determination of flow directions and upslope areas in grid digital elevation models',
//		Water Resources Research, Vol.33, No.2, p.309-319
//
//---------------------------------------------------------
void CMorphometry::Set_Tarboton(int x, int y)
{
	int		i, ix, iy, j;
	double	z, Z[8], iSlope, iAspect, Slope, Aspect, G, H;

	//-----------------------------------------------------
	z		= m_pDTM->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		ix		= Get_xTo(i, x);
		iy		= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) )
		{
			Z[i]	=  m_pDTM->asDouble(ix, iy);
		}
		else
		{
			ix		= Get_xTo(i + 4, x);
			iy		= Get_yTo(i + 4, y);

			if( m_pDTM->is_InGrid(ix, iy) )
			{
				Z[i]	=  z - (m_pDTM->asDouble(ix, iy) - z);
			}
			else
			{
				Z[i]	=  z;
			}
		}
	}

	//---------------------------------------------
    Slope	=  0.0;
	Aspect	= -1.0;

	for(i=0, j=1; i<8; i++, j=(j+1)%8)
	{
		if( i % 2 )	// i => diagonal
		{
			G		= (z    - Z[j]) / Get_Cellsize();
			H		= (Z[j]	- Z[i]) / Get_Cellsize();
		}
		else		// i => orthogonal
		{
			G		= (z    - Z[i]) / Get_Cellsize();
			H		= (Z[i]	- Z[j]) / Get_Cellsize();
		}

		if( H < 0.0 )
		{
			iAspect	= 0.0;
			iSlope	= G;
		}
		else if( H > G )
		{
			iAspect	= M_PI_045;
			iSlope	= (z - Z[i % 2 ? i : j]) / (sqrt(2.0) * Get_Cellsize());
		}
		else
		{
			iAspect	= atan(H / G);
			iSlope	= sqrt(G*G + H*H);
		}

		if( iSlope > Slope )
		{
			Aspect	= i * M_PI_045 + (i % 2 ? M_PI_045 - iAspect : iAspect);
			Slope	= iSlope;
		}
	}

	//---------------------------------------------
	if( Aspect < 0.0 )
	{
		Set_NoData(x, y);
	}
	else
	{
		Set_Gradient(x, y, atan(Slope), Aspect);
	}
}

//---------------------------------------------------------
// Least Squares or Best Fit Plane (Horn 1981, Beasley & Huggins 1982, Costa-Cabral & Burgess 1994)
//
// Horn, B. K. (1981):
//      Hill shading and the relectance map.
//      Proceedings of the IEEE, v. 69, no. 1, p 14-47.
//
// Beasley, D.B. and Huggins, L.F. 1982:
//		ANSWERS: User’s manual.
//		U.S. EPA-905/9-82-001, Chicago, IL. 54pp.
//
// Costa-Cabral, M., and Burges, S.J., 1994:
//		Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas
//		Water Resources Research, v. 30, no. 6, p. 1681-1692.
//
//---------------------------------------------------------
void CMorphometry::Set_LeastSquare(int x, int y)
{
	double	Z[9], a, b;

	Get_SubMatrix3x3(x, y, Z);

	a		= ((Z[2] + 2 * Z[5] + Z[8]) - (Z[0] + 2 * Z[3] + Z[6])) / (8 * Get_Cellsize());
	b		= ((Z[6] + 2 * Z[7] + Z[8]) - (Z[0] + 2 * Z[1] + Z[2])) / (8 * Get_Cellsize());

	Set_Gradient(x, y, atan(sqrt(a*a + b*b)),
		  a != 0.0 ? M_PI_180 + atan2(b, a)
		: b >  0.0 ? M_PI_270
		: b <  0.0 ? M_PI_090
		: m_pAspect ? m_pAspect->Get_NoData_Value() : -1
	);
}

//---------------------------------------------------------
// Quadratic Function Approximation (Heerdegen & Beran, 1984)
//
// Evans, I.S. (1979):
//		An integrated system of terrain analysis and slope mapping.
//		Final report on grant DA-ERO-591-73-G0040. University of Durham, England.
//
//---------------------------------------------------------
// f(z) = Ax^2 + By^2 + Cxy + Dx + Ey + F
//
//---------------------------------------------------------
void CMorphometry::Set_Evans(int x, int y)
{
	double	Z[9], A, B, C, D, E;

	Get_SubMatrix3x3(x, y, Z, 1);

	A	= (Z[0] + Z[2] + Z[3] + Z[5] + Z[6] + Z[8] - 2 * (Z[1] + Z[4] + Z[7])) / (6 * Get_Cellarea());
	B	= (Z[0] + Z[1] + Z[2] + Z[6] + Z[7] + Z[8] - 2 * (Z[3] + Z[4] + Z[5])) / (6 * Get_Cellarea());
	C	= (Z[2] + Z[6] - Z[0] - Z[8])                                          / (4 * Get_Cellarea());
	D	= (Z[2] + Z[5] + Z[8] - Z[0] - Z[3] - Z[6])                            / (6 * Get_Cellsize());
    E	= (Z[0] + Z[1] + Z[2] - Z[6] - Z[7] - Z[8])                            / (6 * Get_Cellsize());

	Set_From_Polynom(x, y, A, B, C, D, E);
}

//---------------------------------------------------------
// Quadratic Function Approximation (Heerdegen & Beran, 1984)
//
// Heerdegen, R.G. / Beran, M.A. (1982):
//		Quantifying source areas through land surface curvature.
//		Journal of Hydrology, Vol.57
//
//---------------------------------------------------------
// f(z) = Ax^2 + By^2 + Cxy + Dx + Ey + F
//
//---------------------------------------------------------
void CMorphometry::Set_Heerdegen(int x, int y)
{
	double	Z[9], A, B, C, D, E, a, b;

	Get_SubMatrix3x3(x, y, Z);

	a	=   Z[0] + Z[2] + Z[3] + Z[5] + Z[6] + Z[8];
	b	=   Z[0] + Z[1] + Z[2] + Z[6] + Z[7] + Z[8];

	A	= (0.3 * a - 0.2 * b)                        / (    Get_Cellarea());
	B	= (0.3 * b - 0.2 * a)                        / (    Get_Cellarea());
	C	= ( Z[0] - Z[2]               - Z[6] + Z[8]) / (4 * Get_Cellarea());
	D	= (-Z[0] + Z[2] - Z[3] + Z[5] - Z[6] + Z[8]) / (6 * Get_Cellsize());
    E	= (-Z[0] - Z[1] - Z[2] + Z[6] + Z[7] + Z[8]) / (6 * Get_Cellsize());

	Set_From_Polynom(x, y, A, B, C, D, E);
}

//---------------------------------------------------------
// Quadratic Function Approximation (Bauer, Rohdenburg & Bork, 1985)
//
// Bauer, J. / Rohdenburg, H. / Bork, H.-R., (1985):
//		'Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse',
//		Landschaftsgenese und Landschaftsoekologie, H.10, Parameteraufbereitung fuer deterministische Gebiets-Wassermodelle,
//		Grundlagenarbeiten zu Analyse von Agrar-Oekosystemen, (Eds.: Bork, H.-R. / Rohdenburg, H.), p.1-15
//
//---------------------------------------------------------
// f(z) = Ax^2 + By^2 + Cxy + Dx + Ey + F
//
//---------------------------------------------------------
void CMorphometry::Set_BRM(int x, int y)
{
	double	Z[9], A, B, C, D, E;

	Get_SubMatrix3x3(x, y, Z);

	A	= ( (Z[0] + Z[2] + Z[3] + Z[5] + Z[6] + Z[8]) - 2 * (Z[1] + Z[4] + Z[7]) ) / (    Get_Cellarea());
	B	= ( (Z[0] + Z[6] + Z[1] + Z[7] + Z[2] + Z[8]) - 2 * (Z[3] + Z[4] + Z[5]) ) / (    Get_Cellarea());
    C	= (  Z[8] + Z[0] - Z[7] )                                                  / (4 * Get_Cellarea());
	D	= ( (Z[2] - Z[0]) + (Z[5] - Z[3]) + (Z[8]-Z[6]) )                          / (6 * Get_Cellsize());
	E	= ( (Z[6] - Z[0]) + (Z[7] - Z[1]) + (Z[8]-Z[2]) )                          / (6 * Get_Cellsize());

	Set_From_Polynom(x, y, A, B, C, D, E);
}

//---------------------------------------------------------
// Quadratic Function Approximation (Zevenbergen und Thorne, 1986)
//
// Zevenbergen, L.W. and C.R. Thorne. 1987:
//		Quantitative analysis of land surface topography
//		Earth Surface Processes and Landforms, 12: 47-56.
//
//---------------------------------------------------------
// f(z) = Ax^2y^2 + Bx^2y + Cxy^2 + Dx^2 + Ey^2 + Fxy + Gx + Hy + I
//
//---------------------------------------------------------
void CMorphometry::Set_Zevenbergen(int x, int y)
{
	double	Z[9], D, E, F, G, H;

	Get_SubMatrix3x3(x, y, Z);

	D	= ((Z[3] + Z[5]) / 2.0 - Z[4]) / (    Get_Cellarea());
	E	= ((Z[1] + Z[7]) / 2.0 - Z[4]) / (    Get_Cellarea());
	F	=  (Z[0] - Z[2] - Z[6] + Z[8]) / (4 * Get_Cellarea());
	G	=  (Z[5] - Z[3])               / (2 * Get_Cellsize());
    H	=  (Z[7] - Z[1])               / (2 * Get_Cellsize());

	Set_From_Polynom(x, y, D, E, F, G, H);
}

//---------------------------------------------------------
// Cubic Function Approximation (Haralick, 1991)
//
// R.M. Haralick (1983):
//		'Ridge and Valley Detection on digital images',
//		Computer Vision, Graphics and Image Processing, Vol.22, No.1, p.28-38
//
//---------------------------------------------------------
// f(z) = Ax^3 + By^3 + Cx^2y + Dxy^2 + Ex^2 + Fy^2 + Gxy + Hx + Iy + J
//
//---------------------------------------------------------
void CMorphometry::Set_Haralick(int x, int y)
{
	//-----------------------------------------------------
	// Matrices for Finite Difference solution...

	const int 	Mtrx[][5][5]	= {
	{	{ 31,- 5,-17,- 5, 31}, {-44,-62,-68,-62,-44}, {  0,  0,  0,  0,  0}, { 44, 62, 68, 62, 44}, {-31,  5, 17,  5,-31}	},
	{	{ 31,-44,  0, 44,-31}, {- 5,-62,  0, 62,  5}, {-17,-68,  0, 68, 17}, {- 5,-62,  0, 62,  5}, { 31,-44,  0, 44,-31}	},
	{	{  2,  2,  2,  2,  2}, {- 1,- 1,- 1,- 1,- 1}, {- 2,- 2,- 2,- 2,- 2}, {- 1,- 1,- 1,- 1,- 1}, {  2,  2,  2,  2,  2}	},
	{	{  4,  2,  0,- 2,- 4}, {  2,  1,  0,- 1,- 2}, {  0,  0,  0,  0,  0}, {- 2,- 1,  0,  1,  2}, {- 4,- 2,  0,  2,  4}	},
	{	{  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}	},	};

	const int	QMtrx[]			= { 4200, 4200, 700, 1000, 700 };

	//-----------------------------------------------------
	int		i, ix, iy, n;
	double	Sum, Z[25], k[5];

	Get_SubMatrix5x5(x, y, Z);

	for(i=0; i<5; i++)
	{
		for(n=0, Sum=0.0, iy=0; iy<5; iy++)
		{
			for(ix=0; ix<5; ix++, n++)
			{
				Sum	+= Z[n] * Mtrx[i][ix][iy];
			}
		}

		k[i]	= Sum / QMtrx[i];
	}

	Set_From_Polynom(x, y, k[4], k[2], k[3], k[1], k[0]);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
