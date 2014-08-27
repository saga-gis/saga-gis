/**********************************************************
 * Version $Id: Grid_Flow_Profile.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Flow_Profile.cpp                 //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
#include "Grid_Flow_Profile.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VALUE_OFFSET	6


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Flow_Profile::CGrid_Flow_Profile(void)
{
	Set_Name(_TL("Flow Path Profile"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description	(_TW(
		"Create interactively flow path profiles from a grid based DEM\n"
		"Use a left mouse button click to create a flow profile starting from the clicked point.")
	);

	Parameters.Add_Grid(
		NULL, "DEM"		, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, "VALUES"	, _TL("Values"),
		_TL("Additional values that shall be saved to the output table."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "POINTS"	, _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "LINE"	, _TL("Profile Line"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}

//---------------------------------------------------------
CGrid_Flow_Profile::~CGrid_Flow_Profile(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Flow_Profile::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")		->asGrid();
	m_pValues	= Parameters("VALUES")	->asGridList();
	m_pPoints	= Parameters("POINTS")	->asShapes();
	m_pLine		= Parameters("LINE")	->asShapes();

	DataObject_Update(m_pDEM, true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Flow_Profile::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	default:
		break;

	case MODULE_INTERACTIVE_LDOWN:
		Set_Profile(Get_System()->Fit_to_Grid_System(ptWorld));
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Flow_Profile::Set_Profile(TSG_Point ptWorld)
{
	int			x, y, i;

	//-----------------------------------------------------
	if( Get_System()->Get_World_to_Grid(x, y, ptWorld) && m_pDEM->is_InGrid(x, y) )
	{
		m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(_TL("Profile [%s]"), m_pDEM->Get_Name()));

		m_pPoints->Add_Field("ID"				, SG_DATATYPE_Int);
		m_pPoints->Add_Field(_TL("Distance")			, SG_DATATYPE_Double);
		m_pPoints->Add_Field(_TL("Distance Overland"), SG_DATATYPE_Double);
		m_pPoints->Add_Field("X"				, SG_DATATYPE_Double);
		m_pPoints->Add_Field("Y"				, SG_DATATYPE_Double);
		m_pPoints->Add_Field("Z"				, SG_DATATYPE_Double);

		for(i=0; i<m_pValues->Get_Count(); i++)
		{
			m_pPoints->Add_Field(m_pValues->asGrid(i)->Get_Name(), SG_DATATYPE_Double);
		}

		//-----------------------------------------------------
		m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format(_TL("Profile [%s]"), m_pDEM->Get_Name()));
		m_pLine->Add_Field("ID"	, SG_DATATYPE_Int);
		m_pLine->Add_Shape()->Set_Value(0, 1);

		//-----------------------------------------------------
		Set_Profile(x, y);

		//-----------------------------------------------------
		DataObject_Update(m_pLine	, false);
		DataObject_Update(m_pPoints	, false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Flow_Profile::Set_Profile(int x, int y)
{
	int		Direction;

	if( Add_Point(x, y) && (Direction = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 )
	{
		Set_Profile(
			Get_System()->Get_xTo(Direction, x),
			Get_System()->Get_yTo(Direction, y)
		);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Flow_Profile::Add_Point(int x, int y)
{
	int			i;
	double		z, Distance, Distance_2;
	TSG_Point	Point;
	CSG_Shape		*pPoint, *pLast;

	if( m_pDEM->is_InGrid(x, y) )
	{
		z		= m_pDEM->asDouble(x, y);
		Point	= Get_System()->Get_Grid_to_World(x, y);

		if( m_pPoints->Get_Count() == 0 )
		{
			Distance	= 0.0;
			Distance_2	= 0.0;
		}
		else
		{
			pLast		= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);
			Distance	= SG_Get_Distance(Point, pLast->Get_Point(0));

			Distance_2	= pLast->asDouble(5) - z;
			Distance_2	= sqrt(Distance*Distance + Distance_2*Distance_2);

			Distance	+= pLast->asDouble(1);
			Distance_2	+= pLast->asDouble(2);
		}

		pPoint	= m_pPoints->Add_Shape();
		pPoint->Add_Point(Point);

		pPoint->Set_Value(0, m_pPoints->Get_Count());
		pPoint->Set_Value(1, Distance);
		pPoint->Set_Value(2, Distance_2);
		pPoint->Set_Value(3, Point.x);
		pPoint->Set_Value(4, Point.y);
		pPoint->Set_Value(5, z);

		for(i=0; i<m_pValues->Get_Count(); i++)
		{
			pPoint->Set_Value(VALUE_OFFSET + i, m_pValues->asGrid(i)->asDouble(x, y));
		}

		m_pLine->Get_Shape(0)->Add_Point(Point);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
