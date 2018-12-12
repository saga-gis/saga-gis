
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   random_points.cpp                   //
//                                                       //
//                 Copyright (C) 2018 by                 //
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
#include "random_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRandom_Points::CRandom_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create Random Points"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Create a layer with randomly distributed points."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"	, _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent"),
			_TL("polygon")
		), 0
	);

	Parameters.Add_Grid_System("EXTENT",
		"GRIDSYSTEM", _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes("EXTENT",
		"SHAPES"	, _TL("Shapes Extent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("EXTENT",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double("EXTENT", "XMIN", _TL("Left"   ), _TL(""));
	Parameters.Add_Double("EXTENT", "XMAX", _TL("Right"  ), _TL(""));
	Parameters.Add_Double("EXTENT", "YMIN", _TL("Bottom" ), _TL(""));
	Parameters.Add_Double("EXTENT", "YMAX", _TL("Top"    ), _TL(""));
	Parameters.Add_Int   ("EXTENT", "NX"  , _TL("Columns"), _TL(""), 1, 1, true);
	Parameters.Add_Int   ("EXTENT", "NY"  , _TL("Rows"   ), _TL(""), 1, 1, true);

	Parameters.Add_Double("",
		"BUFFER"	, _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0.0, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"COUNT"		, _TL("Number of Points"),
		_TL(""),
		100, 1, true
	);

	Parameters.Add_Double("",
		"DISTANCE"	, _TL("Distance"),
		_TL("Minimum distance a point should keep to each other."),
		0, 0, true
	);

	Parameters.Add_Int("",
		"ITERATIONS", _TL("Iterations"),
		_TL("Maximum number of iterations to find a suitable place for a point."),
		1000, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CRandom_Points::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CRandom_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	int	Extent	= (*pParameters)("EXTENT")->asInt();

	pParameters->Set_Enabled("XMIN"      , Extent == 0);
	pParameters->Set_Enabled("XMAX"      , Extent == 0);
	pParameters->Set_Enabled("YMIN"      , Extent == 0);
	pParameters->Set_Enabled("YMAX"      , Extent == 0);
	pParameters->Set_Enabled("NX"        , Extent == 0);
	pParameters->Set_Enabled("NY"        , Extent == 0);
	pParameters->Set_Enabled("GRIDSYSTEM", Extent == 1);
	pParameters->Set_Enabled("SHAPES"    , Extent == 2);
	pParameters->Set_Enabled("POLYGONS"  , Extent == 3);
	pParameters->Set_Enabled("BUFFER"    , Extent != 3);	// no buffering for polygon clip
	pParameters->Set_Enabled("ITERATIONS", Extent == 3 || (*pParameters)("DISTANCE")->asDouble() > 0.);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRandom_Points::On_Execute(void)
{
	//--------------------------------------------------------
	switch( Parameters("EXTENT")->asInt() )
	{
	case 0:	// user defined
		m_Extent.Assign(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);
		break;

	case 1: // grid system
		m_Extent.Assign(Parameters("GRIDSYSTEM")->asGrid_System()->Get_Extent());
		break;

	case 2:	// shapes extent
		m_Extent.Assign(Parameters("SHAPES"  )->asShapes()->Get_Extent());
		break;

	case 3:	// polygon
		m_Extent.Assign(Parameters("POLYGONS")->asShapes()->Get_Extent());
		break;
	}

	if( Parameters("BUFFER")->asDouble() > 0.0 && Parameters("EXTENT")->asInt() != 3 )	// no buffering for polygon clip
	{
		m_Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
	}

	m_pPolygons	= Parameters("EXTENT")->asInt() == 3 ? Parameters("POLYGONS")->asShapes() : NULL;

	m_Distance	= Parameters("DISTANCE")->asDouble();

	if( m_Distance > 0. )
	{
		m_Search.Create(m_Extent);
	}

	int	nPoints		= Parameters("COUNT")->asInt();

	int	Iterations	= Parameters("ITERATIONS")->asInt();

	//--------------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	pPoints->Create(SHAPE_TYPE_Point, _TL("Random Points"));

	pPoints->Add_Field("ID", SG_DATATYPE_Int);

	//--------------------------------------------------------
	for(int i=0, bOkay=1; i<nPoints && bOkay; i++)
	{
		for(bOkay=Iterations; bOkay; bOkay--)
		{
			TSG_Point	Point;

			if( Get_Point(Point) )
			{
				CSG_Shape	*pPoint	= pPoints->Add_Shape();

				pPoint->Set_Point(Point, 0);
				pPoint->Set_Value(0, i + 1);

				break;
			}
		}
	}

	//--------------------------------------------------------
	m_Search.Destroy();

	if( pPoints->Get_Count() < nPoints )
	{
		Message_Fmt("%s (%d < %d)", _TL("Failed to place the desired number of points"), pPoints->Get_Count(), nPoints);
	}

	return( pPoints->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CRandom_Points::Get_Point(TSG_Point &Point)
{
	Point.x	= CSG_Random::Get_Uniform(m_Extent.Get_XMin(), m_Extent.Get_XMax());
	Point.y	= CSG_Random::Get_Uniform(m_Extent.Get_YMin(), m_Extent.Get_YMax());

	return( Check_Polygons(Point) && Check_Distance(Point) );
}

//---------------------------------------------------------
bool CRandom_Points::Check_Polygons(const TSG_Point &Point)
{
	if( m_pPolygons )
	{
		for(int i=0; i<m_pPolygons->Get_Count(); i++)
		{
			if( ((CSG_Shape_Polygon *)m_pPolygons->Get_Shape(i))->Contains(Point) )
			{
				return( true );
			}
		}

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CRandom_Points::Check_Distance(const TSG_Point &Point)
{
	if( m_Distance > 0. && m_Search.is_Okay() )
	{
		if( m_Search.Get_Point_Count() > 0 )
		{
			TSG_Point	p;	double	Value, Distance;

			if( m_Search.Get_Nearest_Point(Point, p, Value, Distance) && Distance < m_Distance )
			{
				return( false );
			}
		}

		m_Search.Add_Point(Point, 0.);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
