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
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            LeastCostPathProfile_Points.cpp            //
//                                                       //
//              Copyright (C) 2004-2010 by               //
//      Olaf Conrad, Victor Olaya & Volker Wichmann      //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "LeastCostPathProfile_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VALUE_OFFSET	5


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLeastCostPathProfile_Points::CLeastCostPathProfile_Points(void)
{
	Set_Name		(_TL("Least Cost Paths"));

	Set_Author		("O. Conrad, V. Olaya, V. Wichmann (c) 2004-2010");

	Set_Description(_TW(
		"This tool allows one to compute least cost path profile(s). It takes an "
		"accumulated cost surface grid and a point shapefile as input. Each "
		"point in the shapefile represents a source for which the least cost path "
		"is calculated.\n"
		"In case the point shapefile has more than one source point "
		"defined, a separate least cost path is calculated for each point. "
		"The tool outputs a point and a line shapefile for each least cost path.\n "
		"The tool allows for optional input grids. The cell values of these grids "
		"along the least cost path are written to the outputs as additional table fields.\n"
	));

	Parameters.Add_Shapes(
		NULL	, "SOURCE"	, _TL("Source Point(s)"),
		_TL("Point shapefile with source point(s)"),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL	, "DEM"		, _TL("Accumulated Cost Surface"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "VALUES"	, _TL("Values"),
		_TL("Allows writing cell values from additional grids to the output"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes_List(
		NULL	, "POINTS"	, _TL("Profile Points"),
		_TL("Least cost path profile points"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes_List(
		NULL	, "LINE"	, _TL("Profile Lines"),
		_TL("Least cost path profile lines"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile_Points::On_Execute(void)
{
	CSG_Shapes					*pSources;
	CSG_Parameter_Shapes_List	*pList_Points, *pList_Lines;

	m_pDEM			= Parameters("DEM"   )->asGrid();
	m_pValues		= Parameters("VALUES")->asGridList();
	pSources		= Parameters("SOURCE")->asShapes();
	pList_Points	= Parameters("POINTS")->asShapesList();
	pList_Lines		= Parameters("LINE"  )->asShapesList();

	//-----------------------------------------------------
	pList_Points->Del_Items();
	pList_Lines ->Del_Items();

	//-----------------------------------------------------
	for(int i=0, x, y; i<pSources->Get_Count(); i++)
	{
		CSG_Shape	*pShape		= pSources->Get_Shape(i);

		if( Get_System().Get_World_to_Grid(x, y, pShape->Get_Point(0)) && m_pDEM->is_InGrid(x, y) )
		{
			//-----------------------------------------------------
			m_pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, CSG_String::Format("%s [%s] %d", _TL("Profile Points"), m_pDEM->Get_Name(), i + 1));

			m_pPoints->Add_Field("ID", SG_DATATYPE_Int);
			m_pPoints->Add_Field("D" , SG_DATATYPE_Double);
			m_pPoints->Add_Field("X" , SG_DATATYPE_Double);
			m_pPoints->Add_Field("Y" , SG_DATATYPE_Double);
			m_pPoints->Add_Field("Z" , SG_DATATYPE_Double);

			for(int j=0; j<m_pValues->Get_Grid_Count(); j++)
			{
				m_pPoints->Add_Field(m_pValues->Get_Grid(j)->Get_Name(), SG_DATATYPE_Double);
			}

			//-----------------------------------------------------
			m_pLines	= SG_Create_Shapes(SHAPE_TYPE_Line, CSG_String::Format("%s [%s] %d", _TL("Profile Line"), m_pDEM->Get_Name(), i + 1));

			m_pLines->Add_Field("ID", SG_DATATYPE_Int);
			m_pLines->Add_Shape()->Set_Value(0, 1);

			//-----------------------------------------------------
			int	Direction;

			while( Add_Point(x, y) && (Direction = m_pDEM->Get_Gradient_NeighborDir(x, y, true, false)) >= 0 )
			{
				x	+= Get_xTo(Direction);
				y	+= Get_yTo(Direction);
			}

			//-----------------------------------------------------
			if( m_pPoints->Get_Count() > 0 )
			{
				pList_Points->Add_Item(m_pPoints);
				pList_Lines ->Add_Item(m_pLines );
			}
			else
			{
				delete(m_pPoints);
				delete(m_pLines );
			}
		}
	}

	return( pList_Points->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile_Points::Add_Point(int x, int y)
{
	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Point	Point	= Get_System().Get_Grid_to_World(x, y);

	double	Distance	= 0.0;

	if( m_pPoints->Get_Count() > 0 )
	{
		CSG_Shape	*pLast	= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);

		Distance	= SG_Get_Distance(Point, pLast->Get_Point(0)) + pLast->asDouble(1);
	}

	//-----------------------------------------------------
	CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

	pPoint->Add_Point(Point);

	pPoint->Set_Value(0, m_pPoints->Get_Count());
	pPoint->Set_Value(1, Distance);
	pPoint->Set_Value(2, Point.x);
	pPoint->Set_Value(3, Point.y);
	pPoint->Set_Value(4, m_pDEM->asDouble(x, y));

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		pPoint->Set_Value(VALUE_OFFSET + i, m_pValues->Get_Grid(i)->asDouble(x, y));
	}

	m_pLines->Get_Shape(0)->Add_Point(Point);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
