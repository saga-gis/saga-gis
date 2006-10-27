
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
#include "Morphometry.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMorphometry::CMorphometry(void)
{
	Set_Name(_TL("Local Morphometry"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"Calculates local morphometric terrain attributes (i.e. slope, aspect and curvatures).\n\n"
		"References:\n\n"

		"Maximum Slope\n"
		"- Travis, M.R. / Elsner, G.H. / Iverson, W.D. / Johnson, C.G. (1975):\n"
		"    'VIEWIT: computation of seen areas, slope, and aspect for land-use planning',\n"
		"    USDA F.S. Gen. Tech. Rep. PSW-11/1975, 70p. Berkeley, California, U.S.A.\n\n"

		"Maximum Triangle Slope\n"
		"- Tarboton, D.G. (1997):\n"
		"    'A new method for the determination of flow directions and upslope areas in grid digital elevation models',\n"
		"    Water Ressources Research, Vol.33, No.2, p.309-319\n\n"

		"Least Squares or Best Fit Plane\n"
		"- Beasley, D.B. / Huggins, L.F. (1982):\n"
		"    'ANSWERS: User’s manual',\n"
		"    U.S. EPA-905/9-82-001, Chicago, IL. 54pp.\n\n"

		"- Costa-Cabral, M., and Burges, S.J., (1994):\n"
		"    'Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas',\n"
		"    Water Resources Research, v. 30, no. 6, p. 1681-1692.\n\n"

		"Fit 2.Degree Polynom\n"
		"- Bauer, J. / Rohdenburg, H. / Bork, H.-R. (1985):\n"
		"    'Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse',\n"
		"    Landschaftsgenese und Landschaftsoekologie, H.10, Parameteraufbereitung fuer deterministische Gebiets-Wassermodelle,\n"
		"    Grundlagenarbeiten zu Analyse von Agrar-Oekosystemen, (Eds.: Bork, H.-R. / Rohdenburg, H.), p.1-15\n\n"

		"- Heerdegen, R.G. / Beran, M.A. (1982):\n"
		"    'Quantifying source areas through land surface curvature',\n"
		"    Journal of Hydrology, Vol.57\n\n"

		"- Zevenbergen, L.W. / Thorne, C.R. (1987):\n"
		"    'Quantitative analysis of land surface topography',\n"
		"    Earth Surface Processes and Landforms, 12: 47-56.\n\n"

		"Fit 3.Degree Polynom\n"
		"- R.M. Haralick (1983):\n"
		"    'Ridge and valley detection on digital images',\n"
		"    Computer Vision, Graphics and Image Processing, Vol.22, No.1, p.28-38\n\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "SLOPE"		, _TL("Slope"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "ASPECT"		, _TL("Aspect"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CURV"		, _TL("Curvature"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "HCURV"		, _TL("Plan Curvature"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "VCURV"		, _TL("Profile Curvature"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|",
			_TL("Maximum Slope (Travis et al. 1975)"),
			_TL("Maximum Triangle Slope (Tarboton 1997)"),
			_TL("Least Squares Fit Plane (Costa-Cabral & Burgess 1996)"),
			_TL("Fit 2.Degree Polynom (Bauer, Rohdenburg, Bork 1985)"),
			_TL("Fit 2.Degree Polynom (Heerdegen & Beran 1982)"),
			_TL("Fit 2.Degree Polynom (Zevenbergen & Thorne 1987)"),
			_TL("Fit 3.Degree Polynom (Haralick 1983)")
		), 5
	);
}

//---------------------------------------------------------
CMorphometry::~CMorphometry(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMorphometry::On_Execute(void)
{
	int			x, y, Method;
	CSG_Colors	Colors;

	//-----------------------------------------------------
	Method		= Parameters("METHOD"   )->asInt();

	pDTM		= Parameters("ELEVATION")->asGrid();
	pSlope		= Parameters("SLOPE"    )->asGrid();
	pAspect		= Parameters("ASPECT"   )->asGrid();
	pCurvature	= Parameters("CURV"     )->asGrid();
	pCurv_Horz	= Parameters("HCURV"    )->asGrid();
	pCurv_Vert	= Parameters("VCURV"    )->asGrid();
	pCurv_Tang	= NULL;//Parameters("TCURV"    )->asGrid();

	//-----------------------------------------------------
	pSlope->Set_ZFactor	(M_RAD_TO_DEG);
	pSlope->Set_Unit	("Degree");
	DataObject_Set_Colors(pSlope	, 100, SG_COLORS_YELLOW_RED);

	pAspect->Set_ZFactor(M_RAD_TO_DEG);
	pAspect->Set_Unit	("Degree");
	Colors.Set_Count(3);
	Colors.Set_Color(0,	SG_GET_RGB(255, 255, 190));
	Colors.Set_Color(1, SG_GET_RGB( 64,   0,   0));
	Colors.Set_Color(2, SG_GET_RGB(255, 255, 190));
	Colors.Set_Count(100);
	DataObject_Set_Colors(pAspect	, Colors);

	DataObject_Set_Colors(pCurvature, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pCurv_Vert, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pCurv_Horz, 100, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	_DX_2		= Get_Cellsize() * Get_Cellsize();
	_4DX_2		= 4.0 * _DX_2;
	_6DX		= 6.0 * Get_Cellsize();
	_2DX		= 2.0 * Get_Cellsize();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			switch( Method )
			{
			case 0:
				Do_MaximumSlope(	x, y );
				break;

			case 1:
				Do_Tarboton(		x, y );
				break;

			case 2:
				Do_LeastSquare(		x, y );
				break;

			case 3:
				Do_FD_BRM(			x, y );
				break;

			case 4:
				Do_FD_Heerdegen(	x, y );
				break;

			case 5:
				Do_FD_Zevenbergen(	x, y );
				break;

			case 6:
				Do_FD_Haralick(		x, y );
				break;
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
inline void CMorphometry::Set_Parameters(int x, int y, double Slope, double Aspect, double Curv, double vCurv, double hCurv, double tCurv)
{
	if( pSlope )
	{
		pSlope->Set_Value(		x, y, Slope );
	}

	if( pAspect )
	{
		pAspect->Set_Value(		x, y, Aspect );
	}

	if( pCurvature )
	{
		pCurvature->Set_Value(	x, y,  Curv );
	}

	if( pCurv_Vert )
	{
		pCurv_Vert->Set_Value(	x, y, vCurv );
	}

	if( pCurv_Horz )
	{
		pCurv_Horz->Set_Value(	x, y, hCurv );
	}

	if( pCurv_Tang )
	{
		pCurv_Tang->Set_Value(	x, y, tCurv );
	}
}

//---------------------------------------------------------
inline void CMorphometry::Set_Parameters_Derive(int x, int y, double D, double E, double F, double G, double H)
{
	double	k1, k2, Curv, vCurv, hCurv, tCurv;

	k1		= F * G * H;
	k2		= G*G + H*H;

	if( k2 )
	{
		Curv	= -2.0 * (E + D);
		vCurv	= -2.0 * (D * G*G + E * H*H + k1) /  k2;
		hCurv	= -2.0 * (D * H*H + E * G*G - k1) /  k2;
		tCurv	= -2.0 * (D * H*H + E * G*G - k1) / (k2 * sqrt(k2 + 1.0));

//		Curv	= -2.0 * (E + D + F*F);
//		vCurv	= -2.0 * (D * G*G + E * H*H + k1) / (k2 *  pow(k2 + 1.0, 3.0 / 2.0));
//		hCurv	= -2.0 * (D * H*H + E * G*G - k1) / (      pow(k2      , 3.0 / 2.0));
//		tCurv	= -2.0 * (D * H*H + E * G*G - k1) / (k2 * sqrt(k2 + 1.0)           );
	}
	else
	{
		Curv	= vCurv	= hCurv	= tCurv	= 0.0;
	}

	if( G != 0.0 )
	{
		Set_Parameters(x, y, atan(sqrt(k2)), M_PI_180 + atan2(H, G)	, Curv, vCurv, hCurv, tCurv);
	}
	else if( H > 0.0 )
	{
		Set_Parameters(x, y, atan(sqrt(k2)), M_PI_270				, Curv, vCurv, hCurv, tCurv);
	}
	else if( H < 0.0 )
	{
		Set_Parameters(x, y, atan(sqrt(k2)), M_PI_090				, Curv, vCurv, hCurv, tCurv);
	}
	else
	{
		Set_Parameters(x, y, atan(sqrt(k2)), 0.0					, Curv, vCurv, hCurv, tCurv);

		if( pAspect )
		{
			pAspect->Set_NoData(x, y);
		}
	}
}

//---------------------------------------------------------
inline void CMorphometry::Set_Parameters_NoData(int x, int y, bool bCompletely)
{
	if( bCompletely )
	{
		if( pSlope )
		{
			pSlope->Set_NoData(		x, y );
		}

		if( pAspect )
		{
			pAspect->Set_NoData(	x, y );
		}

		if( pCurvature )
		{
			pCurvature->Set_NoData(	x, y );
		}

		if( pCurv_Vert )
		{
			pCurv_Vert->Set_NoData(	x, y );
		}

		if( pCurv_Horz )
		{
			pCurv_Horz->Set_NoData(	x, y );
		}

		if( pCurv_Tang )
		{
			pCurv_Tang->Set_NoData(	x, y );
		}
	}
	else
	{
		Set_Parameters(x, y, 0.0, 0.0, 0.0, 0.0, 0.0);

		if( pAspect )
		{
			pAspect->Set_NoData(	x, y );
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Indexing of the Submatrix:
//
//  +-------+    +-------+
//  | 7 0 1 |    | 2 5 8 |
//  | 6 * 2 | => | 1 4 7 |
//  | 5 4 3 |    | 0 3 6 |
//  +-------+    +-------+
//
//---------------------------------------------------------
inline bool CMorphometry::Get_SubMatrix3x3(int x, int y, double SubMatrix[9])
{
	static int	iSub[]	= { 5, 8, 7, 6, 3, 0, 1, 2 };

	int		i, ix, iy;
	double	z;

	if( pDTM->is_NoData(x, y) )
	{
		Set_Parameters_NoData(x, y, true);
	}
	else
	{
		z				= pDTM->asDouble(x, y);
		SubMatrix[4]	= 0.0;

		for(i=0; i<8; i++)
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( pDTM->is_InGrid(ix, iy) )
			{
				SubMatrix[iSub[i]]	= pDTM->asDouble(ix, iy) - z;
			}
			else
			{
				ix	= Get_xTo(i + 4, x);
				iy	= Get_yTo(i + 4, y);

				if( pDTM->is_InGrid(ix, iy) )
				{
					SubMatrix[iSub[i]]	= z - pDTM->asDouble(ix, iy);
				}
				else
				{
					SubMatrix[iSub[i]]	= 0.0;
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CMorphometry::Get_SubMatrix5x5(int x, int y, double SubMatrix[25])
{
	int		i, ix, iy, jx, jy;
	double	z;

	if( pDTM->is_NoData(x, y) )
	{
		Set_Parameters_NoData(x, y, true);
	}
	else
	{
		z	= pDTM->asDouble(x,y);

		for(i=0, iy=y-2; iy<=y+2; iy++)
		{
			jy	= iy < 0 ? 0 : ( iy >= Get_NY() ? Get_NY() - 1 : iy );

			for(ix=x-2; ix<=x+2; ix++, i++)
			{
				jx	= ix < 0 ? 0 : ( ix >= Get_NX() ? Get_NX() - 1 : ix );

				SubMatrix[i]	= pDTM->is_InGrid(jx, jy) ? pDTM->asDouble(jx, jy) - z : 0.0;
			}
		}

		return( true );
	}

	return( false );
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
void CMorphometry::Do_MaximumSlope(int x, int y)
{
	int		i, ix, iy, j, Aspect;

	double	z, zm[8], Slope, Curv, hCurv, a, b;

	//-----------------------------------------------------
	if( pDTM->is_NoData(x, y) )
	{
		Set_Parameters_NoData(x, y, true);
	}
	else
	{
		//-------------------------------------------------
		z		= pDTM->asDouble(x, y);
        Slope	= Curv	= 0.0;

		for(Aspect=-1, i=0; i<8; i++)
		{
			ix		= Get_xTo(i, x);
			iy		= Get_yTo(i, y);

			if( !pDTM->is_InGrid(ix, iy) )
			{
				zm[i]	= 0.0;
			}
			else
			{
				zm[i]	= atan((z - pDTM->asDouble(ix, iy)) / Get_Length(i));
				Curv	+= zm[i];

				if( zm[i] > Slope )
				{
					Aspect	= i;
					Slope	= zm[i];
				}
			}
		}

		//-------------------------------------------------
		if( Aspect < 0.0 )
		{
			Set_Parameters_NoData(x, y);
		}
		else
		{
			//---------------------------------------------
			// Let's now estimate the plan curvature...

			for(i=Aspect+1, j=0, a=0.0; i<Aspect+8; i++, j++)
			{
				if( zm[i % 8] < 0.0 )
				{
					a	= j + zm[(i - 1) % 8] / (zm[(i - 1) % 8] - zm[i % 8]);
					break;
				}
			}

			if( a != 0.0 )
			{
				for(i=Aspect+7, j=0, b=0.0; i>Aspect; i--, j++)
				{
					if( zm[i % 8] < 0.0 )
					{
						b	= j + zm[(i + 1) % 8] / (zm[(i + 1) % 8] - zm[i % 8]);
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
			Set_Parameters(x, y,
				Slope,
				Aspect * M_PI_045,
				Curv,
				zm[Aspect] + zm[(Aspect + 4) % 8],
				hCurv
			);
		}
    }
}

//---------------------------------------------------------
// Maximum Triangle Slope
//
// Tarboton, D.G. (1997):
//		'A new method for the determination of flow directions and upslope areas in grid digital elevation models',
//		Water Ressources Research, Vol.33, No.2, p.309-319
//
//---------------------------------------------------------
void CMorphometry::Do_Tarboton(int x, int y)
{
	int		i, ix, iy, j;
	double	z, zm[8], iSlope, iAspect, Slope, Aspect, G, H;

	//-----------------------------------------------------
	if( pDTM->is_NoData(x, y) )
	{
		Set_Parameters_NoData(x, y, true);
	}
	else
	{
		z		= pDTM->asDouble(x, y);

		for(i=0; i<8; i++)
		{
			ix		= Get_xTo(i, x);
			iy		= Get_yTo(i, y);

			if( pDTM->is_InGrid(ix, iy) )
			{
				zm[i]	=  pDTM->asDouble(ix, iy);
			}
			else
			{
				ix		= Get_xTo(i + 4, x);
				iy		= Get_yTo(i + 4, y);

				if( pDTM->is_InGrid(ix, iy) )
				{
					zm[i]	=  z - (pDTM->asDouble(ix, iy) - z);
				}
				else
				{
					zm[i]	=  z;
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
				G		= (z		- zm[j]) / Get_Cellsize();
				H		= (zm[j]	- zm[i]) / Get_Cellsize();
			}
			else		// i => orthogonal
			{
				G		= (z		- zm[i]) / Get_Cellsize();
				H		= (zm[i]	- zm[j]) / Get_Cellsize();
			}

			if( H < 0.0 )
			{
				iAspect	= 0.0;
				iSlope	= G;
			}
			else if( H > G )
			{
				iAspect	= M_PI_045;
				iSlope	= (z - zm[i % 2 ? i : j]) / (sqrt(2.0) * Get_Cellsize());
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
			Set_Parameters_NoData(x, y);
		}
		else
		{
			Set_Parameters(x, y, atan(Slope), Aspect);
		}
	}
}

//---------------------------------------------------------
// Least Squares or Best Fit Plane (Beasley & Huggins 1982, Costa-Cabral & Burgess 1994)
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
void CMorphometry::Do_LeastSquare(int x, int y)
{
	double	zm[9], a, b;

	if( Get_SubMatrix3x3(x, y, zm) )
	{
		a = ((zm[2] + 2 * zm[5] + zm[8]) - (zm[0] + 2 * zm[3] + zm[6])) / (8 * Get_Cellsize());
		b = ((zm[6] + 2 * zm[7] + zm[8]) - (zm[0] + 2 * zm[1] + zm[2])) / (8 * Get_Cellsize());

		if( a != 0.0 )
		{
			Set_Parameters(x, y, atan( sqrt(a*a + b*b) ), M_PI_180 + atan2(b, a));
		}
		else if( b > 0.0 )
		{
			Set_Parameters(x, y, atan( sqrt(a*a + b*b) ), M_PI_270);
		}
		else if( b < 0.0 )
		{
			Set_Parameters(x, y, atan( sqrt(a*a + b*b) ), M_PI_090);
		}
		else
		{
			Set_Parameters_NoData(x, y);
		}
	}
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
void CMorphometry::Do_FD_BRM(int x, int y)
{
	double	zm[9], D, E, F, G, H;

	if( Get_SubMatrix3x3(x, y, zm) )
	{
		D	= ( (zm[0] + zm[2] + zm[3] + zm[5] + zm[6] + zm[8]) - 2 * (zm[1] + zm[4] + zm[7]) )	/ _DX_2;
		E	= ( (zm[0] + zm[6] + zm[1] + zm[7] + zm[2] + zm[8]) - 2 * (zm[3] + zm[4] + zm[5]) )	/ _DX_2;
        F	= (  zm[8] + zm[0] - zm[7] )														/ _4DX_2;
		G	= ( (zm[2] - zm[0]) + (zm[5] - zm[3]) + (zm[8]-zm[6]) )								/ _6DX;
		H	= ( (zm[6] - zm[0]) + (zm[7] - zm[1]) + (zm[8]-zm[2]) )								/ _6DX;

		Set_Parameters_Derive(x, y, D, E, F, G, H);
	}
}

//---------------------------------------------------------
// Quadratic Function Approximation (Heerdegen & Beran, 1984)
//
// Heerdegen, R.G. / Beran, M.A. (1982):
//		'Quantifying source areas through land surface curvature',
//		Journal of Hydrology, Vol.57
//
//---------------------------------------------------------
void CMorphometry::Do_FD_Heerdegen(int x, int y)
{
	double	zm[9], D, E, F, G, H, a, b;

	//-----------------------------------------------------
	if( Get_SubMatrix3x3(x, y, zm) )
	{
		a	=   zm[0] + zm[2] + zm[3] + zm[5] + zm[6] + zm[8];
		b	=   zm[0] + zm[1] + zm[2] + zm[6] + zm[7] + zm[8];
		D	= (0.3 * a - 0.2 * b)								/ _DX_2;
		E	= (0.3 * b - 0.2 * a)								/ _DX_2;
		F	= ( zm[0] - zm[2]                 - zm[6] + zm[8])	/ _4DX_2;
		G	= (-zm[0] + zm[2] - zm[3] + zm[5] - zm[6] + zm[8])	/ _6DX;
        H	= (-zm[0] - zm[1] - zm[2] + zm[6] + zm[7] + zm[8])	/ _6DX;

		Set_Parameters_Derive(x, y, D, E, F, G, H);
	}
}

//---------------------------------------------------------
// Quadratic Function Approximation (Zevenbergen und Thorne, 1986)
//
// Zevenbergen, L.W. and C.R. Thorne. 1987:
//		Quantitative analysis of land surface topography
//		Earth Surface Processes and Landforms, 12: 47-56.
//
//---------------------------------------------------------
void CMorphometry::Do_FD_Zevenbergen(int x, int y)
{
	double	zm[9], D, E, F, G, H;

	//-----------------------------------------------------
	if( Get_SubMatrix3x3(x, y, zm) )
	{
		D	= ((zm[3] + zm[5]) / 2.0 - zm[4])	/ _DX_2;
		E	= ((zm[1] + zm[7]) / 2.0 - zm[4])	/ _DX_2;
		F	=  (zm[0] - zm[2] - zm[6] + zm[8])	/ _4DX_2;
		G	=  (zm[5] - zm[3])					/ _2DX;
        H	=  (zm[7] - zm[1])					/ _2DX;

		Set_Parameters_Derive(x, y, D, E, F, G, H);
	}
}

//---------------------------------------------------------
// Cubic Function Approximation (Haralick, 1991)
//
// R.M. Haralick (1983):
//		'Ridge and Valley Detection on digital images',
//		Computer Vision, Graphics and Image Processing, Vol.22, No.1, p.28-38
//
//---------------------------------------------------------
void CMorphometry::Do_FD_Haralick(int x, int y)
{
	//-----------------------------------------------------
	// Finite Differenzen Methode Matrizen...

	const int 	Mtrx[][5][5]	= {
	{	{ 31,- 5,-17,- 5, 31}, {-44,-62,-68,-62,-44}, {  0,  0,  0,  0,  0}, { 44, 62, 68, 62, 44}, {-31,  5, 17,  5,-31}	},
	{	{ 31,-44,  0, 44,-31}, {- 5,-62,  0, 62,  5}, {-17,-68,  0, 68, 17}, {- 5,-62,  0, 62,  5}, { 31,-44,  0, 44,-31}	},
	{	{  2,  2,  2,  2,  2}, {- 1,- 1,- 1,- 1,- 1}, {- 2,- 2,- 2,- 2,- 2}, {- 1,- 1,- 1,- 1,- 1}, {  2,  2,  2,  2,  2}	},
	{	{  4,  2,  0,- 2,- 4}, {  2,  1,  0,- 1,- 2}, {  0,  0,  0,  0,  0}, {- 2,- 1,  0,  1,  2}, {- 4,- 2,  0,  2,  4}	},
	{	{  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}, {  2,- 1,- 2,- 1,  2}	},	};

	const int	QMtrx[]			= { 4200, 4200, 700, 1000, 700 };

	//-----------------------------------------------------
	int		i, ix, iy, n;
	double	Sum, zm[25], k[5];

	//-----------------------------------------------------
	if( Get_SubMatrix5x5(x, y, zm) )
	{
		for(i=0; i<5; i++)
		{
			for(n=0, Sum=0.0, iy=0; iy<5; iy++)
			{
				for(ix=0; ix<5; ix++, n++)
				{
					Sum	+= zm[n] * Mtrx[i][ix][iy];
				}
			}

			k[i]	= Sum / QMtrx[i];
		}

		Set_Parameters_Derive(x, y, k[4], k[2], k[3], k[1], k[0]);
    }
}
