
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
//                  Visibility_Point.cpp                 //
//                                                       //
//            Copyright (C) 2003, 2013, 2022 by          //
//               Olaf Conrad, Volker Wichmann            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Visibility_Point.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVisibility::Create(CSG_Parameters &Parameters)
{
	Parameters.Add_Grid("", "ELEVATION" , _TL("Elevation" ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("", "VISIBILITY", _TL("Visibility"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("",
		"METHOD"    , _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Visibility"),
			_TL("Shade"),
			_TL("Distance"),
			_TL("Size")
		), 3
	);

	Parameters.Add_Choice("METHOD",
		"UNIT"      , _TL("Unit"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("radians"),
			_TL("degree")
		), 1
	);

	Parameters.Add_Bool("METHOD",
		"CUMULATIVE", _TL("Cumulative"),
		_TL("If not set, output is the maximum size, or the cumulated sizes otherwise."),
		false
	);

	Parameters.Add_Bool("",
		"NODATA"    , _TL("Ignore No-Data"),
		_TL("Ignore elevations that have been marked as no-data."),
		false
	);

	return( true );
}

//---------------------------------------------------------
bool CVisibility::Initialize(const CSG_Parameters &Parameters)
{
	m_pDEM          = Parameters("ELEVATION" )->asGrid();
	m_pVisibility   = Parameters("VISIBILITY")->asGrid();
	m_Method        = Parameters("METHOD"    )->asInt ();
	m_bIgnoreNoData = Parameters("NODATA"    )->asBool();
	m_bDegree       = Parameters("UNIT"      )->asInt () == 1;
	m_bCumulative   = Parameters("CUMULATIVE")->asBool();

	m_pDEM->Set_Max_Samples(m_pDEM->Get_NCells());	// we use max z (queried by Get_Max()) as a breaking condition in ray tracing

	Reset();

	CSG_Colors Colors; CSG_String Unit;

	switch( m_Method )
	{
	default: // Visibility
		Colors.Create(2, SG_COLORS_BLACK_WHITE, false);
		break;

	case  1: // Shade
		Colors.Create(2, SG_COLORS_BLACK_WHITE, true);
		Unit = _TL("radians");
		break;

	case  2: // Distance
		Colors.Set_Ramp(SG_GET_RGB(255, 255, 191), SG_GET_RGB(  0,  95,   0));
		break;

	case  3: // Size
		Colors.Set_Ramp(SG_GET_RGB(  0,  95,   0), SG_GET_RGB(255, 255, 191));
		Unit = m_bDegree ? _TL("degree") : _TL("radians");
		break;
	}

	SG_UI_DataObject_Colors_Set(m_pVisibility, &Colors);

	m_pVisibility->Set_Unit(Unit);

	return( true );
}

//---------------------------------------------------------
bool CVisibility::Finalize(bool bShow)
{
	CSG_Parameters Parameters;

	int Update = bShow ? SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE : SG_UI_DATAOBJECT_UPDATE;

	switch( m_Method )
	{
	case  0: // Visibility
		Parameters.Add_Range("", "METRIC_ZRANGE", "", "", 0., 1.);
		SG_UI_DataObject_Update(m_pVisibility, Update, &Parameters);
		break;

	case  1: // Shade
		Parameters.Add_Range("", "METRIC_ZRANGE", "", "", 0., M_PI_090);
		SG_UI_DataObject_Update(m_pVisibility, Update, &Parameters);
		break;

	default: // Distance, Size
		SG_UI_DataObject_Show  (m_pVisibility, Update);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CVisibility::Reset(void)
{
	switch( m_Method )
	{
	case  0: m_pVisibility->Assign(      0.); break; // Visibility
	case  1: m_pVisibility->Assign(M_PI_090); break; // Shade
	default: m_pVisibility->Assign_NoData( ); break; // Distance, Size
	}

	return( true );
}

//---------------------------------------------------------
bool CVisibility::Set_Visibility(int xOrigin, int yOrigin, double Height, bool bReset)
{
	if( !m_pDEM->is_InGrid(xOrigin, yOrigin) )
	{
		return( false );
	}

	if( bReset )
	{
		Reset();
	}

	double zOrigin = m_pDEM->asDouble(xOrigin, yOrigin) + Height;
	double zMax    = m_pDEM->Get_Max();

	//-----------------------------------------------------
	for(int y=0; y<m_pDEM->Get_NY() && SG_UI_Process_Set_Progress(y, m_pDEM->Get_NY()); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<m_pDEM->Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pVisibility->Set_NoData(x, y);
			}
			else
			{
				double dx = xOrigin - x;
				double dy = yOrigin - y;
				double dz = zOrigin - m_pDEM->asDouble(x, y);

				//-----------------------------------------
				if( _Trace_Point(x, y, dx, dy, dz, xOrigin, yOrigin, zMax) )
				{
					switch( m_Method )
					{
					default: { // Visibility
						m_pVisibility->Set_Value(x, y, 1.);
						break; }

					case  1: { // Shade
						double dec, azi; const double Exaggeration = 1.;

						if( m_pDEM->Get_Gradient(x, y, dec, azi) )
						{
							dec	= M_PI_090 - atan(Exaggeration * tan(dec));

							double decSrc = atan2(dz, sqrt(dx*dx + dy*dy));
							double aziSrc = atan2(dx, dy);

							double d = acos(sin(dec) * sin(decSrc) + cos(dec) * cos(decSrc) * cos(azi - aziSrc)); if( d > M_PI_090 ) { d = M_PI_090; }

							if( m_pVisibility->asDouble(x, y) > d )
							{
								m_pVisibility->Set_Value(x, y, d);
							}
						}
						break; }

					case  2: { // Distance
						double d = m_pDEM->Get_Cellsize() * sqrt(dx*dx + dy*dy);

						if( m_pVisibility->is_NoData(x, y) || m_pVisibility->asDouble(x, y) > d )
						{
							m_pVisibility->Set_Value(x, y, d);
						}
						break; }

					case  3: { // Size
						double d = m_pDEM->Get_Cellsize() * sqrt(dx*dx + dy*dy);

						if( d > 0. )
						{
							d = atan2(fabs(Height), d); if( m_bDegree ) { d *= M_RAD_TO_DEG; }

							if( m_pVisibility->is_NoData(x, y) || (!m_bCumulative && m_pVisibility->asDouble(x, y) < d) )
							{
								m_pVisibility->Set_Value(x, y, d);
							}
							else if( m_bCumulative )
							{
								m_pVisibility->Add_Value(x, y, d);
							}
						}
						break; }
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CVisibility::_Trace_Point(int x, int y, double dx, double dy, double dz, int xOrigin, int yOrigin, double zMax)
{
	double d = fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);

	if( d > 0. )
	{
		double dist = sqrt(dx*dx + dy*dy);

		dx /= d; dy /= d; dz /= d; d = dist / d;

		double id = 0.;
		double ix = 0.5 + x;
		double iy = 0.5 + y;
		double iz = m_pDEM->asDouble(x, y);

		while( id < dist )
		{
			id += d;
			ix += dx;
			iy += dy;
			iz += dz;

			x   = (int)ix;
			y   = (int)iy;

			if( !m_pDEM->is_InGrid(x, y) )
			{
				if( !m_bIgnoreNoData || !m_pDEM->Get_System().is_InGrid(x, y) )
				{
					return( false );
				}
			}
			else
			{
				if( iz < m_pDEM->asDouble(x, y) )
				{
					return( false );
				}

				if( iz > zMax )
				{
					return( true );
				}
			}

			if (x == xOrigin && y == yOrigin)
			{
				// because of floating point precision issues the check "id < dist" can fail on
				// the observer cell and we move on into another cell, so check this explicitly

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
CVisibility_Point::CVisibility_Point(void)
{
	Set_Name		(_TL("Visibility Analysis"));

	Set_Author		("O.Conrad (c) 2022");

	Set_Description(_TW(
		"Visibility or viewshed analysis."
	));

	Create(Parameters);

	Parameters.Add_Double("",
		"HEIGHT"	, _TL("Height"),
		_TL("Height of the light source or observer above ground."),
		10., 0., true
	);

	Parameters.Add_Bool("",
		"MULTIPLE"	, _TL("Add Multiple Locations"),
		_TL("Add multiple light source or observer positions."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVisibility_Point::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("UNIT"      , (*pParameters)("METHOD")->asInt() == 3); // Size
	pParameters->Set_Enabled("CUMULATIVE", (*pParameters)("METHOD")->asInt() == 3); // Size

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CVisibility_Point::On_Execute(void)
{
	Initialize(Parameters);

	return( true );
}

//---------------------------------------------------------
bool CVisibility_Point::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if(	Mode == TOOL_INTERACTIVE_LDOWN )
	{
		double Height = Parameters("HEIGHT"  )->asDouble();
		bool   bReset = Parameters("MULTIPLE")->asBool() == false;

		if( Set_Visibility(Get_xGrid(), Get_yGrid(), Height, bReset) )
		{
			Finalize(true);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVisibility_Points::CVisibility_Points(void)
{
	Set_Name		(_TL("Visibility Analysis"));

	Set_Author		("V.Wichmann (c) 2013");

	Set_Description(_TW(
		"This tool performs a visibility analysis using "
		"light source or observer points from a points layer."
	));

	Create(Parameters);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL("Observer points."),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field_or_Const("POINTS",
		"HEIGHT"	, _TL("Height"),
		_TL("Height of the light source or observer above ground."),
		10., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVisibility_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("UNIT"      , (*pParameters)("METHOD")->asInt() == 3); // Size
	pParameters->Set_Enabled("CUMULATIVE", (*pParameters)("METHOD")->asInt() == 3); // Size

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CVisibility_Points::On_Execute(void)
{
	Initialize(Parameters);

	CSG_Shapes *pPoints = Parameters("POINTS")->asShapes();

	int     Field = Parameters("HEIGHT")->asInt   ();
	double Height = Parameters("HEIGHT")->asDouble();

	//-----------------------------------------------------
	for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Process_Get_Okay(); iPoint++)
	{
		Process_Set_Text("%s %lld...", _TL("processing observer"), 1 + iPoint);

		CSG_Shape &Point = *pPoints->Get_Shape(iPoint);

		int x, y; Get_System().Get_World_to_Grid(x, y, Point.Get_Point());

		Set_Visibility(x, y, Field < 0 ? Height : Point.asDouble(Field), false);
	}

	//-----------------------------------------------------
	Finalize(false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
