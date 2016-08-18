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
		"Analytical hillshading calculation.\n"
		"Method 'Ambient Occlusion' is based on concepts of Tarini et al. (2006), but only "
		"the northern half-space is considered.\n"
		"\n"
		"References:\n"
		"Tarini, M. / Cignoni, P. / Montani, C. (2006): Ambient Occlusion and Edge Cueing "
		"to Enhance Real Time Molecular Visualization. "
		"IEEE Transactions on Visualization and Computer Graphics, Vol. 12, No. 5, pp. 1237-1244.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ELEVATION"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SHADE"			, _TL("Analytical Hillshading"),
		_TL("The angle between the surface and the incoming light beams, measured in radians."),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Shading Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Standard"),
			_TL("Standard (max. 90Degree)"),
			_TL("Combined Shading"),
			_TL("Ray Tracing"),
			_TL("Ambient Occlusion")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "AZIMUTH"			, _TL("Azimuth [Degree]"),
		_TL("Direction of the light source, measured in degree clockwise from the north direction."),
		PARAMETER_TYPE_Double		, 315
	);

	Parameters.Add_Value(
		NULL	, "DECLINATION"		, _TL("Height [Degree]"),
		_TL("Height of the light source, measured in degree above the horizon."),
		PARAMETER_TYPE_Double		, 45
	);

	Parameters.Add_Value(
		NULL	, "EXAGGERATION"	, _TL("Exaggeration"),
		_TL("The terrain exaggeration factor allows one to increase the shading contrasts in flat areas."),
		PARAMETER_TYPE_Double		, 4
	);

	Parameters.Add_Choice(
		NULL	, "SHADOW"			, _TL("Shadow"),
		_TL("Choose 'slim' to trace grid node's shadow, 'fat' to trace the whole cell's shadow. The first is slightly faster but might show some artifacts."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("slim"),
			_TL("fat")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "NDIRS"			, _TL("Number of Directions"),
		_TW("Number of sample directions for ambient occlusion. Divides azimuth range (270 to 0 to 90 degree) into sectors. "
			"Declination (0 to 90 degree) is divided in (Number of Directions / 4) sectors."),
		PARAMETER_TYPE_Int, 8.0, 2, true
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"			, _TL("Search Radius"),
		_TL("Radius used to trace for shadows (ambient occlusion) [map units]."),
		PARAMETER_TYPE_Double, 10.0, 0.001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHillShade::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		pParameters->Get_Parameter("AZIMUTH"     )->Set_Enabled(pParameter->asInt()  < 4);
		pParameters->Get_Parameter("DECLINATION" )->Set_Enabled(pParameter->asInt()  < 4);
		pParameters->Get_Parameter("EXAGGERATION")->Set_Enabled(pParameter->asInt()  < 4);
		pParameters->Get_Parameter("SHADOW"      )->Set_Enabled(pParameter->asInt() == 3);
		pParameters->Get_Parameter("NDIRS"       )->Set_Enabled(pParameter->asInt() == 4);
		pParameters->Get_Parameter("RADIUS"      )->Set_Enabled(pParameter->asInt() == 4);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillShade::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM			= Parameters("ELEVATION"   )->asGrid();
	m_pShade		= Parameters("SHADE"       )->asGrid();
	m_zScale		= Parameters("EXAGGERATION")->asDouble();

	double	Azimuth	= Parameters("AZIMUTH"     )->asDouble() * M_DEG_TO_RAD;
	double	Height	= Parameters("DECLINATION" )->asDouble() * M_DEG_TO_RAD;
	double	dRadius	= Parameters("RADIUS"      )->asDouble();
	int		iDirs	= Parameters("NDIRS"       )->asInt();

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	Get_Shading     (Azimuth, Height, false, false);	break;
	case 1:	Get_Shading     (Azimuth, Height, true , false);	break;
	case 2:	Get_Shading     (Azimuth, Height, false, true );	break;
	case 3:	Shadow	        (Azimuth, Height);					break;
	case 4:	AmbientOcclusion(iDirs, dRadius);					break;
	}

	//-----------------------------------------------------
	m_pShade->Set_Unit(_TL("radians"));

	DataObject_Set_Colors(m_pShade, 100, SG_COLORS_BLACK_WHITE, true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CHillShade::Get_Shading(double Azimuth, double Height, bool bDelimit, bool bCombine)
{
	//-----------------------------------------------------
	double	sinHgt	= sin(Height);
	double	cosHgt	= cos(Height);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	s, a, d;

			if( !m_pDEM->Get_Gradient(x, y, s, a) )
			{
				m_pShade->Set_NoData(x, y);
			}
			else
			{
				s	= tan(s);
				d	= M_PI_090 - atan(m_zScale * s);
				d	= acos(sin(d) * sinHgt + cos(d) * cosHgt * cos(a - Azimuth));

				if( bDelimit && d > M_PI_090 )
				{
					d	= M_PI_090;
				}

				if( bCombine )
				{
					d	*= s / M_PI_090;
				}

				m_pShade->Set_Value(x, y, d);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CHillShade::Shadow(double Azimuth, double Height)
{
	//-----------------------------------------------------
	Get_Shading(Azimuth, Height, true, false);

	//-----------------------------------------------------
	double	dx	= sin(Azimuth + M_PI_180);
	double	dy	= cos(Azimuth + M_PI_180);

	if( fabs(dx) > fabs(dy) )
	{
		dy	/= fabs(dx);
		dx	 = dx < 0 ? -1 : 1;
	}
	else if( fabs(dy) > fabs(dx) )
	{
		dx	/= fabs(dy);
		dy	 = dy < 0 ? -1 : 1;
	}
	else
	{
		dx	 = dx < 0 ? -1 : 1;
		dy	 = dy < 0 ? -1 : 1;
	}

	double	dz	= tan(Height) * sqrt(dx*dx + dy*dy) * Get_Cellsize();

	//-----------------------------------------------------
	const double	dShadow	= 0.49;

	int	Shadowing	= Parameters("SHADOW")->asInt();

	m_Shade.Create(*Get_System(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				switch( Shadowing )
				{
				default:	// slim
					Shadow_Trace(x, y, m_pDEM->asDouble(x, y), dx, dy, dz);
					break;

				case 1:		// fat
					Shadow_Trace(x - dShadow, y - dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
					Shadow_Trace(x + dShadow, y - dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
					Shadow_Trace(x - dShadow, y + dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
					Shadow_Trace(x + dShadow, y + dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
					break;
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Shade.Destroy();
}

//---------------------------------------------------------
void CHillShade::Shadow_Trace(double x, double y, double z, double dx, double dy, double dz)
{
	for(x+=dx+0.5, y+=dy+0.5, z-=dz; ; x+=dx, y+=dy, z-=dz)
	{
		int	ix	= (int)x,	iy	= (int)y;

		if( !is_InGrid(ix, iy) )
		{
			return;
		}

		if( !m_pDEM->is_NoData(ix, iy) )
		{
			double	zDiff	= z - m_pDEM->asDouble(ix, iy);

			if( zDiff <= 0.0 )
			{
				return;
			}

			m_Shade.Set_Value(ix, iy, zDiff);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CHillShade::AmbientOcclusion(int iDirs, double dRadius)
{
	CSG_Points_Z		Directions;
	
	Directions.Set_Count(iDirs);

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
