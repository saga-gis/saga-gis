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
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     HillShade.cpp                     //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "HillShade.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHillShade::CHillShade(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Analytical Hillshading"));

	Set_Author		("O.Conrad, V.Wichmann (c) 2003-2013");

	Set_Description(_TW(
		"This tool performs an analytical hillshade computation for an elevation grid. "
		"The 'Standard' method simply calculates the angle at which light coming from the "
		"position of the light source would hit the surface. This method can produce angles "
		"greater than 90 degree. With the second method all values are kept within the "
		"range of 0-90 degree. It can be enhanced with shadowing effects, where shadowed "
		"cells will be marked with a value of exactly 90 degree. 'Shadows Only' creates "
		"a mask for the shadowed areas and sets all other cells to no-data. 'Combined Shading' "
		"takes the values of the standard method and multiplies these with the normalized slope. "
		"'Ambient Occlusion' is based on the concepts of Tarini et al. (2006), but only "
		"the northern half-space is considered here. "
	));

	Add_Reference(
		"Tarini, M. / Cignoni, P. / Montani, C.", "2006",
		"Ambient Occlusion and Edge Cueing to Enhance Real Time Molecular Visualization",
		"IEEE Transactions on Visualization and Computer Graphics, Vol. 12, No. 5, pp. 1237-1244."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SHADE"			, _TL("Analytical Hillshading"),
		_TL("The angle between the surface and the incoming light beams, measured in radians."),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"		, _TL("Shading Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("Standard"),
			_TL("Limited Maximum"),
			_TL("With Shadows"),
			_TL("Shadows Only"),
			_TL("Ambient Occlusion"),
			_TL("Combined Shading")
		), 0
	);

	Parameters.Add_Choice("",
		"POSITION"		, _TL("Sun's Position"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("azimuth and height"),
			_TL("date and time")
		), 0
	);

	Parameters.Add_Double("POSITION",
		"AZIMUTH"		, _TL("Azimuth"),
		_TL("Direction of the light source, measured in degree clockwise from the North direction."),
		315.0, 0.0, true, 360.0, true
	);

	Parameters.Add_Double("POSITION",
		"DECLINATION"	, _TL("Height"),
		_TL("Height of the light source, measured in degree above the horizon."),
		45.0, 0.0, true, 90.0, true
	);

	Parameters.Add_Date("POSITION",
		"DATE"			, _TL("Day"),
		_TL(""),
		CSG_DateTime::Now().Get_JDN()
	);

	Parameters.Add_Double("POSITION",
		"TIME"			, _TL("Hour"),
		_TL(""),
		12.0, 0.0, true, 24.0, true
	);

	Parameters.Add_Double("",
		"EXAGGERATION"	, _TL("Exaggeration"),
		_TL("The terrain exaggeration factor allows one to increase the shading contrasts in flat areas."),
		1.0
	);

	Parameters.Add_Choice("",
		"UNIT"			, _TL("Unit"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("radians"),
			_TL("degree")
		), 0
	);

	Parameters.Add_Choice("",
		"SHADOW"		, _TL("Shadow"),
		_TL("Choose 'slim' to trace grid node's shadow, 'fat' to trace the whole cell's shadow. The first is slightly faster but might show some artifacts."),
		CSG_String::Format("%s|%s|",
			_TL("slim"),
			_TL("fat")
		), 0
	);

	Parameters.Add_Int("",
		"NDIRS"			, _TL("Number of Directions"),
		_TW("Number of sample directions for ambient occlusion. Divides azimuth range (270 to 0 to 90 degree) into sectors. "
			"Declination (0 to 90 degree) is divided in (Number of Directions / 4) sectors."),
		8, 2, true
	);

	Parameters.Add_Double("",
		"RADIUS"		, _TL("Search Radius"),
		_TL("Radius used to trace for shadows (ambient occlusion) [map units]."),
		10.0, 0.001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHillShade::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("POSITION"    , pParameter->asInt() != 4);
		pParameters->Set_Enabled("EXAGGERATION", pParameter->asInt() != 4 && pParameter->asInt() != 3);
		pParameters->Set_Enabled("UNIT"        , pParameter->asInt() <  3);
		pParameters->Set_Enabled("SHADOW"      , pParameter->asInt() == 2 || pParameter->asInt() == 3);
		pParameters->Set_Enabled("NDIRS"       , pParameter->asInt() == 4);
		pParameters->Set_Enabled("RADIUS"      , pParameter->asInt() == 4);
	}

	if(	pParameter->Cmp_Identifier("POSITION") )
	{
		pParameters->Set_Enabled("AZIMUTH"     , pParameter->asInt() == 0);
		pParameters->Set_Enabled("DECLINATION" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("DATE"        , pParameter->asInt() == 1);
		pParameters->Set_Enabled("TIME"        , pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillShade::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("ELEVATION")->asGrid();
	m_pShade	= Parameters("SHADE"    )->asGrid();

	//-----------------------------------------------------
	bool	bResult;

	switch( Parameters("METHOD")->asInt() )
	{
	default: bResult = Get_Shading(false, false); break; // Standard
	case  1: bResult = Get_Shading( true, false); break; // Limited Maximum
	case  5: bResult = Get_Shading(false,  true); break; // With Shadows
	case  2: bResult = Get_Shadows(false       ); break; // Shadows Only
	case  3: bResult = Get_Shadows( true       ); break; // Ambient Occlusion
	case  4: bResult = AmbientOcclusion(       ); break; // Combined Shading
	}

	if( !bResult )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("METHOD")->asInt() >= 3 )
	{
		m_pShade->Set_Unit(_TL(""));
	}
	else if( Parameters("UNIT")->asInt() == 0 )
	{
		m_pShade->Set_Unit(_TL("radians"));
	}
	else
	{
		m_pShade->Set_Unit(_TL("degree"));

		m_pShade->Multiply(M_RAD_TO_DEG);
	}

	//-----------------------------------------------------
	if( Parameters("METHOD")->asInt() == 3 )	// Shadows Only
	{
		DataObject_Set_Parameter(m_pShade, "UNISYMBOL_COLOR", (int)SG_COLOR_BLACK);
		DataObject_Set_Parameter(m_pShade, "COLORS_TYPE", 0);	// Single Symbol
	}
	else
	{
		DataObject_Set_Colors   (m_pShade, 11, SG_COLORS_BLACK_WHITE, true);
		DataObject_Set_Parameter(m_pShade, "COLORS_TYPE", 3);	// Graduated Colors
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillShade::Get_Position(double &Azimuth, double &Decline)
{
	if( Parameters("POSITION")->asInt() == 0 )
	{
		Azimuth	= Parameters("AZIMUTH"    )->asDouble() * M_DEG_TO_RAD;
		Decline	= Parameters("DECLINATION")->asDouble() * M_DEG_TO_RAD;
	}
	else
	{
		CSG_Shapes	Origin(SHAPE_TYPE_Point), GCS;

		Origin.Get_Projection().Create(m_pDEM->Get_Projection());
		Origin.Add_Shape()->Add_Point(m_pDEM->Get_System().Get_Extent().Get_Center());

		SG_RUN_TOOL_ExitOnError("pj_proj4", 2,	// Coordinate Transformation (Shapes)
				SG_TOOL_PARAMETER_SET("SOURCE", &Origin)
			&&	SG_TOOL_PARAMETER_SET("TARGET", &GCS)
		)

		TSG_Point	Center	= GCS.Get_Shape(0)->Get_Point(0);

		CSG_DateTime	Date(Parameters("DATE")->asDate()->Get_Date());

		double	Hour	= Parameters("TIME")->asDouble();

		Date.Set_Hour(Hour);
		Date.Set(floor(Date.Get_JDN()) - 0.5 + Hour / 24.0);	// relate to UTC, avoid problems with daylight saving time

		SG_Get_Sun_Position(Date, 0.0, Center.y * M_DEG_TO_RAD, Decline, Azimuth);

		Message_Fmt("\n%s: %f", _TL("Longitude"), Center.x);
		Message_Fmt("\n%s: %f", _TL("Latitude" ), Center.y);
		Message_Fmt("\n%s: %f", _TL("Azimuth"  ), Azimuth * M_RAD_TO_DEG);
		Message_Fmt("\n%s: %f", _TL("Height"   ), Decline * M_RAD_TO_DEG);
	}

	return( Decline >= 0.0 );
}

//---------------------------------------------------------
bool CHillShade::Get_Shading(bool bDelimit, bool bCombine)
{
	double	Azimuth, Decline;

	if( !Get_Position(Azimuth, Decline) )
	{
		return( false );
	}

	double	sinDecline	= sin(Decline);
	double	cosDecline	= cos(Decline);

	double	Scale	= Parameters("EXAGGERATION")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( !m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_pShade->Set_NoData(x, y);
			}
			else
			{
				if( Scale != 1.0 )
				{
					Slope	= atan(Scale * tan(Slope));
				}

				double	d	= M_PI_090 - Slope;

				d	= acos(sin(d) * sinDecline + cos(d) * cosDecline * cos(Aspect - Azimuth));

				if( bDelimit && d > M_PI_090 )
				{
					d	= M_PI_090;
				}

				if( bCombine )
				{
					d	*= Slope / M_PI_090;
				}

				m_pShade->Set_Value(x, y, d);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SHADOW_SLIM		0x0
#define SHADOW_FAT_X	0x1
#define SHADOW_FAT_Y	0x2

#define EPSILON			0.0001

//---------------------------------------------------------
bool CHillShade::Get_Shadows(bool bMask)
{
	double	Azimuth, Decline;

	if( !Get_Position(Azimuth, Decline) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	Shadow	= SHADOW_SLIM;
	double	dx	= sin(Azimuth + M_PI_180);
	double	dy	= cos(Azimuth + M_PI_180);

	if( fabs(dx) - fabs(dy) > EPSILON )
	{
		dy	/= fabs(dx);
		dx	 = dx < 0 ? -1 : 1;

		if( Parameters("SHADOW")->asInt() && fabs(dy) > EPSILON )
			Shadow	= SHADOW_FAT_X;
	}
	else if( fabs(dy) - fabs(dx) > EPSILON )
	{
		dx	/= fabs(dy);
		dy	 = dy < 0 ? -1 : 1;

		if( Parameters("SHADOW")->asInt() && fabs(dx) > EPSILON )
			Shadow	= SHADOW_FAT_Y;
	}
	else	// diagonal
	{
		dx	 = dx < 0 ? -1 : 1;
		dy	 = dy < 0 ? -1 : 1;

		if( Parameters("SHADOW")->asInt() )
			Shadow	= SHADOW_FAT_X|SHADOW_FAT_Y;
	}

	double	dz	= tan(Decline) * sqrt(dx*dx + dy*dy) * Get_Cellsize();

	//-----------------------------------------------------
	if( bMask )
	{
		m_pShade->Assign_NoData();
	}
	else
	{
		Get_Shading(true, false);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Set_Shadow_Trace(x, y, m_pDEM->asDouble(x, y), dx, dy, dz, Shadow);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CHillShade::Set_Shadow_Trace(double x, double y, double z, double dx, double dy, double dz, int Shadow)
{
	for(x+=dx+0.5, y+=dy+0.5, z-=dz; ; x+=dx, y+=dy, z-=dz)
	{
		int	ix	= (int)x,	iy	= (int)y;

		if( !is_InGrid(ix, iy) )
		{
			return( true );
		}

		if( !m_pDEM->is_NoData(ix, iy) )
		{
			if( z < m_pDEM->asDouble(ix, iy) )
			{
				return( true );
			}

			m_pShade->Set_Value(ix, iy, M_PI_090);

			if( Shadow & SHADOW_FAT_X )
			{
				int	xx	= x - ix < 0.5 ? ix - 1 : ix + 1;

				if( m_pDEM->is_InGrid(xx, iy) && z < m_pDEM->asDouble(xx, iy) )
				{
					m_pShade->Set_Value(xx, iy, M_PI_090);
				}
			}

			if( Shadow & SHADOW_FAT_Y )
			{
				int	yy	= y - iy < 0.5 ? iy - 1 : iy + 1;

				if( m_pDEM->is_InGrid(ix, yy) && z < m_pDEM->asDouble(ix, yy) )
				{
					m_pShade->Set_Value(ix, yy, M_PI_090);
				}
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillShade::AmbientOcclusion(void)
{
	double	dRadius	= Parameters("RADIUS")->asDouble();
	int		iDirs	= Parameters("NDIRS" )->asInt();

	CSG_Points_Z	Directions;	Directions.Set_Count(iDirs);

	for(int i=0; i<iDirs; i++)
	{
		Directions[i].z	= (M_PI_180 * i) / iDirs; // only northern half-space, otherwise: (M_PI_360 * i) / iDirs;
		Directions[i].x	= sin(Directions[i].z - M_PI_090);
		Directions[i].y	= cos(Directions[i].z - M_PI_090);
	}

	m_pShade->Assign(0.0);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	s, a;

			if( !m_pDEM->Get_Gradient(x, y, s, a) )
			{
				m_pShade->Set_NoData(x, y);
				continue;
			}

			CSG_Point_Z	Normal(sin(s) * sin(a), sin(s) * cos(a), cos(s));

			//-----------------------------------------------------
			for(int i=0; i<Directions.Get_Count(); i++)
			{
				for(int j=0; j<Directions.Get_Count(); j++)
				{
					Directions[i].z	= sin((M_PI_090 * j) / (iDirs / 4.0));
					
					double	dAngle =	(Normal.Get_X() * Directions[i].x + Normal.Get_Y() * Directions[i].y + Normal.Get_Z() * Directions[i].z);

					if (dAngle <= 0.0)
						continue;

					if( AmbientOcclusion_Trace(x, y, Directions[i], dRadius) )
					{
						m_pShade->Add_Value(x, y, dAngle);
					}
				}
			}

			if( !m_pShade->is_NoData(x, y) )
				m_pShade->Set_Value(x, y, M_PI - m_pShade->asDouble(x, y) / (iDirs * (iDirs / 4.0)));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CHillShade::AmbientOcclusion_Trace(int x, int y, CSG_Point_Z Direction, double dRadius)
{
	double		dDistance, iDistance, dx, dy, dz, ix, iy, iz, z;

	z			= m_pDEM->asDouble(x, y);
	dx			= Direction.Get_X();
	dy			= Direction.Get_Y();
	dz			= tan( asin(Direction.Get_Z()) ) * sqrt(dx*dx + dy*dy) * Get_Cellsize();
	ix			= x;
	iy			= y;
	iz			= m_pDEM->asDouble(x, y);
	dDistance	= 0.0;
	iDistance	= Get_Cellsize() * M_GET_LENGTH(dx, dy);

	while( is_InGrid(x, y) && dDistance <= dRadius )
	{
		ix	+= dx;	x	= (int)(0.5 + ix);
		iy	+= dy;	y	= (int)(0.5 + iy);
		iz	+= dz;

		if( m_pDEM->is_InGrid(x, y) && m_pDEM->asDouble(x, y) > iz )
			return( false );

		dDistance	+= iDistance;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
