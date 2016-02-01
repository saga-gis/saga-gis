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
//                   Cost Analysis                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              LeastCostPathProfile.cpp                 //
//                                                       //
//                 Copyright (C) 2004 by                 //
//               Olaf Conrad & Victor Olaya              //
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
#include "LeastCostPathProfile.h"


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
CLeastCostPathProfile::CLeastCostPathProfile(void)
{
	Set_Name		(_TL("Least Cost Path"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Creates a least cost past profile using an accumulated cost surface."
	));

	Parameters.Add_Grid(
		NULL	, "DEM"		, _TL("Accumulated cost"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "VALUES"	, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "POINTS"	, _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "LINE"	, _TL("Profile Line"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile::On_Execute(void)
{
	m_pDEM		= Parameters("DEM"   )->asGrid    ();
	m_pValues	= Parameters("VALUES")->asGridList();
	m_pPoints	= Parameters("POINTS")->asShapes  ();
	m_pLines	= Parameters("LINE"  )->asShapes  ();

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", _TL("Profile"), m_pDEM->Get_Name()));

	m_pPoints->Add_Field("ID", SG_DATATYPE_Int   );
	m_pPoints->Add_Field("D" , SG_DATATYPE_Double);
	m_pPoints->Add_Field("X" , SG_DATATYPE_Double);
	m_pPoints->Add_Field("Y" , SG_DATATYPE_Double);
	m_pPoints->Add_Field("Z" , SG_DATATYPE_Double);

	for(int i=0; i<m_pValues->Get_Count(); i++)
	{
		m_pPoints->Add_Field(m_pValues->asGrid(i)->Get_Name(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	m_pLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", _TL("Profile"), m_pDEM->Get_Name()));
	m_pLines->Add_Field("ID", SG_DATATYPE_Int);
	m_pLine	= m_pLines->Add_Shape();
	m_pLine->Set_Value(0, 1);

	//-----------------------------------------------------
//	DataObject_Update(m_pDEM, true);

	Set_Drag_Mode(MODULE_INTERACTIVE_DRAG_NONE);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	case MODULE_INTERACTIVE_LDOWN:
	case MODULE_INTERACTIVE_MOVE_LDOWN:
		return( Set_Profile(Get_System()->Fit_to_Grid_System(ptWorld)) );

	default:
		return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile::Set_Profile(TSG_Point ptWorld)
{
	m_pPoints->Del_Shapes();
	m_pLine  ->Del_Parts();

	//-----------------------------------------------------
	int		x, y, Direction;

	if( Get_Grid_Pos(x, y) )
	{
		while( Add_Point(x, y) && (Direction = m_pDEM->Get_Gradient_NeighborDir(x, y, true, false)) >= 0 )
		{
			x	+= Get_xTo(Direction);
			y	+= Get_yTo(Direction);
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pLines , false);
	DataObject_Update(m_pPoints, false);

	return( m_pPoints->Get_Count() > 0 );
}

//---------------------------------------------------------
bool CLeastCostPathProfile::Add_Point(int x, int y)
{
	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Point	Point	= Get_System()->Get_Grid_to_World(x, y);

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

	for(int i=0; i<m_pValues->Get_Count(); i++)
	{
		pPoint->Set_Value(VALUE_OFFSET + i, m_pValues->asGrid(i)->asDouble(x, y));
	}

	m_pLine->Add_Point(Point);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------