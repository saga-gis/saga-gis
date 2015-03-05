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
//                      morphometry                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    param_scale.cpp                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     Olaf Conrad                       //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "param_scale.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define FLAT	1
#define PIT		2
#define CHANNEL 3
#define PASS	4
#define RIDGE	5
#define PEAK	6

//---------------------------------------------------------
#define LUT_SET_CLASS(id, name, color)	{ CSG_Table_Record *pR = pLUT->asTable()->Add_Record(); pR->Set_Value(0, color); pR->Set_Value(1, name); pR->Set_Value(3, id); pR->Set_Value(3, id); }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParam_Scale::CParam_Scale(void)
{
	Set_Name		(_TL("Morphometric Features"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Uses a multi-scale approach by fitting quadratic "
		"parameters to any size window (via least squares) "
		"to derive slope, aspect and curvatures (optional output) "
		"for subsequent classification of morphometric features "
		"(peaks, ridges, passes, channels, pits and planes). "
		"This is the method as proposed and implemented by Jo Wood "
		"(1996) in LandSerf and GRASS GIS (r.param.scale). "
		"\n\n"
		"Optional output is described in the following. "
		"Generalised elevation is the smoothed input DEM. "
		"Slope is the magnitude of maximum gradient. It is given "
		"for steepest slope angle and measured in degrees. "
		"Aspect is the direction of maximum gradient. "
		"Profile curvature is the curvature intersecting with the "
		"plane defined by the Z axis and maximum gradient direction. "
		"Positive values describe convex profile curvature, negative "
		"values concave profile. Plan curvature is the horizontal "
		"curvature, intersecting with the XY plane. Longitudinal "
		"curvature is the profile curvature intersecting with the "
		"plane defined by the surface normal and maximum gradient "
		"direction. Cross-sectional curvature is the tangential "
		"curvature intersecting with the plane defined by the surface "
		"normal and a tangent to the contour - perpendicular to "
		"maximum gradient direction. Minimum curvature is measured "
		"in direction perpendicular to the direction of of maximum "
		"curvature. The maximum curvature is measured in any direction. "
		"\n\n"
		"References:"
		"\n\n"
		"Wood, J. (1996): The Geomorphological characterisation of Digital Elevation Models. "
		"Diss., Department of Geography, University of Leicester, U.K. "
		"<a target=\"_blank\" href=\"http://www.soi.city.ac.uk/~jwo/phd/\">online</a>."
		"\n\n"
		"Wood, J. (2009): Geomorphometry in LandSerf. "
		"In: Hengl, T. and Reuter, H.I. [Eds.]: Geomorphometry: Concepts, Software, Applications. "
		"Developments in Soil Science, Elsevier, Vol.33, 333-349."
		"\n\n"
		"<a target=\"_blank\" href=\"http://www.landserf.org/\">LandSerf Homepage</a>."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL, "FEATURES" , _TL("Morphometric Features")    , _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Byte);
	Parameters.Add_Grid(NULL, "ELEVATION", _TL("Generalized Surface")      , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "SLOPE"    , _TL("Slope")                    , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "ASPECT"   , _TL("Aspect")                   , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "PROFC"    , _TL("Profile Curvature")        , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "PLANC"    , _TL("Plan Curvature")           , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "LONGC"    , _TL("Longitudinal Curvature")   , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "CROSC"    , _TL("Cross-Sectional Curvature"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "MAXIC"    , _TL("Maximum Curvature")        , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "MINIC"    , _TL("Minimum Curvature")        , _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Value(
		NULL	, "SIZE"		, _TL("Scale Radius (Cells)"),
		_TL("Size of processing window (= 1 + 2 * radius) given as number of cells"),
		PARAMETER_TYPE_Int, 5, 1, true
	);

	Parameters.Add_Value(
		NULL	, "TOL_SLOPE"		, _TL("Slope Tolerance"),
		_TL("Slope tolerance that defines a 'flat' surface (degrees)"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		NULL	, "TOL_CURVE"		, _TL("Curvature Tolerance"),
		_TL("Curvature tolerance that defines 'planar' surface"),
		PARAMETER_TYPE_Double, 0.0001
	);

	Parameters.Add_Value(
		NULL	, "EXPONENT"	, _TL("Distance Weighting Exponent"),
		_TL("Exponent for distance weighting (0.0-4.0)"),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 4.0, true
	);

	Parameters.Add_Value(
		NULL	, "ZSCALE"		, _TL("Vertical Scaling"),
		_TL("Vertical scaling factor"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		NULL	, "CONSTRAIN"	, _TL("Constrain"),
		_TL("Constrain model through central window cell"),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GRID_SET_NODATA(pGrid)		if( pGrid ) pGrid->Set_NoData(x, y);
#define GRID_SET_VALUE(pGrid, z)	if( pGrid ) pGrid->Set_Value (x, y, z);

//---------------------------------------------------------
bool CParam_Scale::On_Execute(void)
{
	//-----------------------------------------------------
	bool		bConstrain;
	int			Index[6];
	double		zScale, Tol_Slope, Tol_Curve;
	CSG_Matrix	Normal;

	//-----------------------------------------------------
	bConstrain	= Parameters("CONSTRAIN")->asBool();
	zScale		= Parameters("ZSCALE"   )->asDouble();	if( zScale <= 0.0 )	{	zScale	= 1.0;	}
	Tol_Slope	= Parameters("TOL_SLOPE")->asDouble();
	Tol_Curve	= Parameters("TOL_CURVE")->asDouble();

	m_pDEM		= Parameters("DEM"      )->asGrid();

	//-----------------------------------------------------
	CSG_Grid	*pFeature	= Parameters("FEATURES" )->asGrid();
	CSG_Grid	*pElevation	= Parameters("ELEVATION")->asGrid();
	CSG_Grid	*pSlope		= Parameters("SLOPE"    )->asGrid();
	CSG_Grid	*pAspect	= Parameters("ASPECT"   )->asGrid();
	CSG_Grid	*pProfC		= Parameters("PROFC"    )->asGrid();
	CSG_Grid	*pPlanC		= Parameters("PLANC"    )->asGrid();
	CSG_Grid	*pLongC		= Parameters("LONGC"    )->asGrid();
	CSG_Grid	*pCrosC		= Parameters("CROSC"    )->asGrid();
	CSG_Grid	*pMiniC		= Parameters("MINIC"    )->asGrid();
	CSG_Grid	*pMaxiC		= Parameters("MAXIC"    )->asGrid();

	//-----------------------------------------------------
	if( !Get_Weights() )
	{
		return( false );
	}

	if( !Get_Normal(Normal) )
	{
		return( false );
	}

	// To constrain the quadtratic through the central cell, ignore the calculations involving the
	// coefficient f. Since these are all in the last row and column of the matrix, simply redimension.
	if( !SG_Matrix_LU_Decomposition(bConstrain ? 5 : 6, Index, Normal.Get_Data()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Vector	Observed;

			double	elevation, slope, aspect, profc, planc, longc, crosc, minic, maxic;

			if( Get_Observed(x, y, Observed, bConstrain)
			&&  SG_Matrix_LU_Solve(bConstrain ? 5 : 6, Index, Normal, Observed.Get_Data()) )
			{
				Get_Parameters(zScale, Observed.Get_Data(), elevation, slope, aspect, profc, planc, longc, crosc, minic, maxic);

				GRID_SET_VALUE(pFeature  , Get_Feature(slope, minic, maxic, crosc, Tol_Slope, Tol_Curve));
				GRID_SET_VALUE(pElevation, elevation + m_pDEM->asDouble(x, y));	// Add central elevation back
				GRID_SET_VALUE(pSlope    , slope);
				GRID_SET_VALUE(pAspect   , aspect);
				GRID_SET_VALUE(pProfC    , profc);
				GRID_SET_VALUE(pPlanC    , planc);
				GRID_SET_VALUE(pLongC    , longc);
				GRID_SET_VALUE(pCrosC    , crosc);
				GRID_SET_VALUE(pMiniC    , minic);
				GRID_SET_VALUE(pMaxiC    , maxic);
			}
			else
			{
				GRID_SET_NODATA(pFeature);
				GRID_SET_NODATA(pElevation);
				GRID_SET_NODATA(pSlope);
				GRID_SET_NODATA(pAspect);
				GRID_SET_NODATA(pProfC);
				GRID_SET_NODATA(pPlanC);
				GRID_SET_NODATA(pLongC);
				GRID_SET_NODATA(pCrosC);
				GRID_SET_NODATA(pMiniC);
				GRID_SET_NODATA(pMaxiC);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pFeature, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		LUT_SET_CLASS(FLAT   , _TL("Planar"       ), SG_GET_RGB(180, 180, 180));
		LUT_SET_CLASS(PIT    , _TL("Pit"          ), SG_GET_RGB(  0,   0,   0));
		LUT_SET_CLASS(CHANNEL, _TL("Channel"      ), SG_GET_RGB(  0,   0, 255));
		LUT_SET_CLASS(PASS   , _TL("Pass (saddle)"), SG_GET_RGB(  0, 255,   0));
		LUT_SET_CLASS(RIDGE  , _TL("Ridge"        ), SG_GET_RGB(255, 255,   0));
		LUT_SET_CLASS(PEAK   , _TL("Peak"         ), SG_GET_RGB(255,   0,   0));

		DataObject_Set_Parameter(pFeature, pLUT);

		DataObject_Set_Parameter(pFeature, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	DataObject_Set_Colors(pSlope , 11, SG_COLORS_YELLOW_RED);
	DataObject_Set_Colors(pAspect, 11, SG_COLORS_ASPECT_3);
	DataObject_Set_Colors(pProfC , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pPlanC , 11, SG_COLORS_RED_GREY_BLUE, false);
	DataObject_Set_Colors(pLongC , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pCrosC , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pMiniC , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pMaxiC , 11, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CParam_Scale::Get_Parameters(double zScale, double coeff[6], double &elevation, double &slope, double &aspect, double &profc, double &planc, double &longc, double &crosc, double &minic, double &maxic)
{
	// Quadratic function in the form of
	//  z = ax^2 + by^2 + cxy + dx + ey + f

	// Rescale coefficients if a Z scaling is required.
	double	a	= coeff[0] * zScale;
	double	b	= coeff[1] * zScale;
	double	c	= coeff[2] * zScale;
	double	d	= coeff[3] * zScale;
	double	e	= coeff[4] * zScale;
	double	f	= coeff[5];	// f does not need rescaling as it is only used for smoothing.

	elevation	= f;

	slope		= M_RAD_TO_DEG * atan(sqrt((d * d) + (e * e)));

	aspect		= M_RAD_TO_DEG * atan2(e, d); // W=0, N=+90, E=180, S=-90(=270)
	aspect		= fmod(270.0 - aspect, 360.0);

	profc		= !d && !e ? 0.0
				: -2.0 * (a * d * d + b * e * e + c * e * d)
				/ ((e * e + d * d) * pow(1.0 + d * d + e * e, 1.5));

	planc		= !d && !e ? 0.0
				:  2.0 * (b * d * d + a * e * e - c * d * e)
				/ pow(e * e + d * d, 1.5);

	longc		= !d && !e ? 0.0
				: -2.0 * (a * d * d + b * e * e + c * d * e)
				/ (d * d + e * e);

	crosc		= !d && !e ? 0.0
				: -2.0 * (b * d * d + a * e * e - c * d * e)
				/ (d * d + e * e);

	minic		= -a - b - sqrt((a - b) * (a - b) + c * c);

	maxic		= -a - b + sqrt((a - b) * (a - b) + c * c);

	return( true );
}

//---------------------------------------------------------
inline int CParam_Scale::Get_Feature(double slope, double minic, double maxic, double crosc, double Tol_Slope, double Tol_Curve)
{
	//-----------------------------------------------------
	//	Feature | slope | crosc | maxic | minic
	//  --------+-------+-------+-------+-------
	//	Peak    |   0   |   #   |  +ve  |  +ve
	//	Ridge   |   0   |   #   |  +ve  |   0
	//	        |  +ve  |  +ve  |   #   |   #
	//	Pass    |   0   |   #   |  +ve  |  -ve
	//	Plane   |   0   |   #   |   0   |   0
	//	        |  +ve  |   0   |   #   |   #
	//	Channel |   0   |   #   |   0   |  -ve
	//	        |  +ve  |  -ve  |   #   |   #
	//	Pit     |   0   |   #   |  -ve  |  -ve
	//
	//	Table 5.3 Simplified feature classification criteria.
	//	#  indicates undefined, or not part of selection criteria.
	//	http://www.geog.le.ac.uk/jwo/research/dem_char/thesis/05feat.htm

	//-----------------------------------------------------
	// Case 1: Surface is sloping. Cannot be a peak, pass or pit.
	// Therefore calculate the cross-sectional curvature to
	// characterise as channel, ridge or planar.
	if( slope > Tol_Slope )
	{
		if( crosc > Tol_Curve )
		{
			return( RIDGE );
		}
		else if( crosc < -Tol_Curve )
		{
			return( CHANNEL );
		}
		else
		{
			return( FLAT );
		}
	}

	//-----------------------------------------------------
	// Case 2: Surface has (approximately) vertical slope normal.
	// Feature can be of any type.
	else
	{
		if( maxic > Tol_Curve )
		{
			if( minic > Tol_Curve )
			{
				return( PEAK );
			}
			else if( minic < -Tol_Curve )
			{
				return( PASS );
			}
			else
			{
				return( RIDGE );
			}
		}
		else if( maxic < -Tol_Curve )
		{
			if( minic < -Tol_Curve )
			{
				return( PIT );
			}
		}
		else
		{
			if( minic < -Tol_Curve )
			{
				return( CHANNEL );
			}
			else if( minic > Tol_Curve && minic < -Tol_Curve )
			{
				return( FLAT );
			}
		}
	}

	return( FLAT );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// find_weight() Function to find the weightings matrix for the
//               observed cell values.
//               Uses an inverse distance function that can be
//               calibrated with an exponent (0= no decay,
//               1=linear decay, 2=squared distance decay etc.)
//               V.1.1, Jo Wood, 11th May, 1995.
//---------------------------------------------------------
bool CParam_Scale::Get_Weights(void)
{
	if( (m_Radius = Parameters("SIZE")->asInt()) < 1 || !m_Weights.Create(1 + 2 * m_Radius, 1 + 2 * m_Radius) )
	{
		return( false );
	}

	double	Exponent	= Parameters("EXPONENT")->asDouble();

	// Find inverse distance of all cells to centre.
	for(int y=0; y<m_Weights.Get_NY(); y++)
	{
		for(int x=0; x<m_Weights.Get_NX(); x++)
		{
			m_Weights[y][x]	= 1.0 / pow(1.0 + SG_Get_Length(m_Radius - x, m_Radius - y), Exponent);
		}
	}

	return( true );
}

//---------------------------------------------------------
// find_normal() - Function to find the set of normal equations
//                 that allow a quadratic trend surface to be
//                 fitted through N  points using least squares
//                 V.1.0, Jo Wood, 27th November, 1994.
//---------------------------------------------------------
bool CParam_Scale::Get_Normal(CSG_Matrix &Normal)
{
	double	x1, y1,	x2, y2, x3, y3, x4, y4, xy2, x2y, xy3, x3y, x2y2, xy, N;	// coefficients of X-products.

	x1 = y1 = x2 = y2 = x3 = y3 = x4 = y4 = xy2 = x2y = xy3 = x3y = x2y2 = xy = N = 0.0;

	// Calculate matrix of sums of squares and cross products
	for(int y=0; y<m_Weights.Get_NY(); y++)
	{
		double	dy	= Get_Cellsize() * (y - m_Radius);

		for(int x=0; x<m_Weights.Get_NX(); x++)
		{
			double	dw	= m_Weights[y][x];

			double	dx	= Get_Cellsize() * (x - m_Radius);

			x4		+= dw * dx * dx * dx * dx;
			x2y2	+= dw * dx * dx * dy * dy;
			x3y		+= dw * dx * dx * dx * dy;
			x3		+= dw * dx * dx * dx;
			x2y		+= dw * dx * dx * dy;
			x2		+= dw * dx * dx;

			y4		+= dw * dy * dy * dy * dy;
			xy3		+= dw * dx * dy * dy * dy;
			xy2		+= dw * dx * dy * dy;
			y3		+= dw * dy * dy * dy;
			y2		+= dw * dy * dy;

			xy		+= dw * dx * dy;

			x1		+= dw * dx;

			y1		+= dw * dy;

			N		+= dw;
		}
	}

	// Store cross-product matrix elements.
	Normal.Create(6, 6);

	Normal[0][0] = x4;
	Normal[0][1] = Normal[1][0] = x2y2;
	Normal[0][2] = Normal[2][0] = x3y;
	Normal[0][3] = Normal[3][0] = x3;
	Normal[0][4] = Normal[4][0] = x2y;
	Normal[0][5] = Normal[5][0] = x2;

	Normal[1][1] = y4;
	Normal[1][2] = Normal[2][1] = xy3;
	Normal[1][3] = Normal[3][1] = xy2;
	Normal[1][4] = Normal[4][1] = y3;
	Normal[1][5] = Normal[5][1] = y2;

	Normal[2][2] = x2y2;
	Normal[2][3] = Normal[3][2] = x2y;
	Normal[2][4] = Normal[4][2] = xy2;
	Normal[2][5] = Normal[5][2] = xy;

	Normal[3][3] = x2;
	Normal[3][4] = Normal[4][3] = xy;
	Normal[3][5] = Normal[5][3] = x1;

	Normal[4][4] = y2;
	Normal[4][5] = Normal[5][4] = y1;

	Normal[5][5] = N;

	return( true );
}

//---------------------------------------------------------
// find_obs() - Function to find the observed vector as part of
//              the set of normal equations for least squares.
//              V.1.0, Jo Wood, 11th December, 1994.
//---------------------------------------------------------
bool CParam_Scale::Get_Observed(int x, int y, CSG_Vector &Observed, bool bConstrain)
{
	if( m_pDEM->is_NoData(x, y)
	||  x < m_Radius || x > Get_NX() - m_Radius
	||  y < m_Radius || y > Get_NY() - m_Radius )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		ix, iy, jx, jy;
	double	dx, dy, dz, z;

	Observed.Create(6);

	z	= m_pDEM->asDouble(x, y);

	for(iy=0, jy=y-m_Radius, dy=-m_Radius*Get_Cellsize(); iy<m_Weights.Get_NY(); iy++, jy++, dy+=Get_Cellsize())
	{
		for(ix=0, jx=x-m_Radius, dx=-m_Radius*Get_Cellsize(); ix<m_Weights.Get_NX(); ix++, jx++, dx+=Get_Cellsize())
		{
			dz	= m_pDEM->is_InGrid(jx, jy) ? m_pDEM->asDouble(jx, jy) - z : 0.0;

			if( dz )
			{
				dz	*= m_Weights[iy][ix];

				Observed[0]	+= dz * dx * dx;
				Observed[1]	+= dz * dy * dy;
				Observed[2]	+= dz * dx * dy;
				Observed[3]	+= dz * dx;
				Observed[4]	+= dz * dy;

				if( !bConstrain )	// if constrained, should remain 0.0
				{
					Observed[5]	+= dz;
				}
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
