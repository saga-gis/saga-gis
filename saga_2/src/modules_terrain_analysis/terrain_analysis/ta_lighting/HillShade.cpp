
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
	Set_Name(_TL("Analytical Hillshading"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(
		_TL("Analytical hillshading calculation.")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "SHADE"			, _TL("Analytical Hillshading"),
		_TL("The angle between the surface and the incoming light beams, measured in radians."),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Shading Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Standard"),
			_TL("Standard (max. 90Degree)"),
			_TL("Combined Shading"),
			_TL("Ray Tracing")
		), 2
	);

	Parameters.Add_Value(
		NULL	, "AZIMUTH"			, _TL("Azimuth [Degree]"),
		_TL("Direction of the light source, measured in degree clockwise from the north direction."),
		PARAMETER_TYPE_Double		, 315
	);

	Parameters.Add_Value(
		NULL	, "DECLINATION"		, _TL("Declination [Degree]"),
		_TL("Declination of the light source, measured in degree above the horizon."),
		PARAMETER_TYPE_Double		, 45
	);

	Parameters.Add_Value(
		NULL	, "EXAGGERATION"	, _TL("Exaggeration"),
		_TL("The terrain exaggeration factor allows to increase the shading contrasts in flat areas."),
		PARAMETER_TYPE_Double		, 4
	);
}

//---------------------------------------------------------
CHillShade::~CHillShade(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillShade::On_Execute(void)
{
	double	Azimuth, Declination;

	//-----------------------------------------------------
	pDTM			= Parameters("ELEVATION")	->asGrid();

	pHillShade		= Parameters("SHADE")		->asGrid();

	Azimuth			= Parameters("AZIMUTH")		->asDouble() * M_DEG_TO_RAD;
	Declination		= Parameters("DECLINATION")	->asDouble() * M_DEG_TO_RAD;
	Exaggeration	= Parameters("EXAGGERATION")->asDouble();

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:
		Get_Shading	(Azimuth, Declination, false, false);
		break;

	case 1:
		Get_Shading	(Azimuth, Declination, true , false);
		break;

	case 2:
		Get_Shading	(Azimuth, Declination, false, true);
		break;

	case 3:
		RayTrace	(Azimuth, Declination);
		break;
	}

	//-----------------------------------------------------
	pHillShade->Set_ZFactor(M_RAD_TO_DEG);

	DataObject_Set_Colors(pHillShade, 100, SG_COLORS_BLACK_WHITE, true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CHillShade::Get_Shading(double Azimuth, double Declination, bool bDelimit, bool bCombine)
{
	int		x, y;
	double	s, a, sinDec, cosDec, d;

	//-----------------------------------------------------
	sinDec	= sin(Declination);
	cosDec	= cos(Declination);

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pDTM->Get_Gradient(x, y, s, a) )
			{
				pHillShade->Set_NoData(x, y);
			}
			else
			{
				s	= tan(s);
				d	= M_PI_090 - atan(Exaggeration * s);
				d	= acos(sin(d) * sinDec + cos(d) * cosDec * cos(a - Azimuth));

				if( bDelimit && d > M_PI_090 )
				{
					d	= M_PI_090;
				}

				if( bCombine )
				{
					d	*= s / M_PI_090;
				}

				pHillShade->Set_Value(x, y, d);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CHillShade::RayTrace(double Azimuth, double Declination)
{
	int		x, y, ix, iy, xStart, yStart, xStep, yStep;
	double	dx, dy, dz;

	//-----------------------------------------------------
	Get_Shading(Azimuth, Declination, true, false);

	//-----------------------------------------------------
	Azimuth	= Azimuth + M_PI_180;

	if( sin(Azimuth) > 0.0 )
	{
		xStart	= 0;
		xStep	= 1;
	}
	else
	{
		xStart	= Get_NX() - 1;
		xStep	= -1;
	}

	if( cos(Azimuth) > 0.0 )
	{
		yStart	= 0;
		yStep	= 1;
	}
	else
	{
		yStart	= Get_NY() - 1;
		yStep	= -1;
	}

	//-----------------------------------------------------
	dx		= sin(Azimuth);
	dy		= cos(Azimuth);

	if( fabs(dx) > fabs(dy) )
	{
		dy	/= fabs(dx);
		dx	= dx < 0 ? -1 : 1;
	}
	else if( fabs(dy) > fabs(dx) )
	{
		dx	/= fabs(dy);
		dy	= dy < 0 ? -1 : 1;
	}
	else
	{
		dx	= dx < 0 ? -1 : 1;
		dy	= dy < 0 ? -1 : 1;
	}

	dz		= tan(Declination) * sqrt(dx*dx + dy*dy) * Get_Cellsize();

	//-----------------------------------------------------
	for(iy=0, y=yStart; iy<Get_NY() && Set_Progress(iy); iy++, y+=yStep)
	{
		for(ix=0, x=xStart; ix<Get_NX(); ix++, x+=xStep)
		{
			RayTrace_Trace(x, y, dx, dy, dz);
		}
	}
}

//---------------------------------------------------------
void CHillShade::RayTrace_Trace(int x, int y, double dx, double dy, double dz)
{
	double	ix, iy, iz;

	if( !pDTM->is_NoData(x, y) )
	{
		for(ix=x+0.5, iy=y+0.5, iz=pDTM->asDouble(x, y); ; )
		{
			ix	+= dx;
			iy	+= dy;
			iz	-= dz;

			x	= (int)ix;
			y	= (int)iy;

			if( !is_InGrid(x, y) || pDTM->asDouble(x, y) > iz )
			{
				break;
			}
			else if( pDTM->is_InGrid(x, y) )
			{
				pHillShade->Set_Value(x, y, M_PI_090);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
