
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
//                   Grid_Profile.cpp                    //
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
#include "Grid_Profile.h"


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
CGrid_Profile::CGrid_Profile(void)
{
	Set_Name		(_TL("Profile"));

	Set_Author		(_TL("Copyrights (c) 2004 by Olaf Conrad"));

	Set_Description	(_TW(
		"Create interactively profiles from a grid based DEM\n"
		"Use left mouse button clicks into a map window to add profile points."
		"A right mouse button click will finish the profile."
	));

	Set_Drag_Mode	(MODULE_INTERACTIVE_DRAG_LINE);

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, "VALUES"		, _TL("Values"),
		_TL("Additional values that shall be saved to the output table."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "POINTS"		, _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "LINE"		, _TL("Profile Line"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}

//---------------------------------------------------------
CGrid_Profile::~CGrid_Profile(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")		->asGrid();
	m_pValues	= Parameters("VALUES")	->asGridList();
	m_pPoints	= Parameters("POINTS")	->asShapes();
	m_pLine		= Parameters("LINE")	->asShapes();

	m_bAdd		= false;

	DataObject_Update(m_pDEM, true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	default:
		break;

	case MODULE_INTERACTIVE_LDOWN:
		if( !m_bAdd )
		{
			m_bAdd	= true;
			m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("Profile [%s]"), m_pDEM->Get_Name()));
			m_pLine->Get_Table().Add_Field("ID"	, TABLE_FIELDTYPE_Int);
			m_pLine->Add_Shape()->Get_Record()->Set_Value(0, 1);
		}

		m_pLine->Get_Shape(0)->Add_Point(Get_System()->Fit_to_Grid_System(ptWorld));

		DataObject_Update(m_pLine, false);
		break;

	case MODULE_INTERACTIVE_RDOWN:
		Set_Profile();
		m_bAdd	= false;
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
bool CGrid_Profile::Set_Profile(void)
{
	int			i;
	TSG_Point	A, B;
	CSG_Shape	*pLine;

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(_TL("Profile [%s]"), m_pDEM->Get_Name()));

	m_pPoints->Get_Table().Add_Field("ID"				, TABLE_FIELDTYPE_Int);
	m_pPoints->Get_Table().Add_Field(_TL("Distance")			, TABLE_FIELDTYPE_Double);
	m_pPoints->Get_Table().Add_Field(_TL("Distance Overland"), TABLE_FIELDTYPE_Double);
	m_pPoints->Get_Table().Add_Field("X"				, TABLE_FIELDTYPE_Double);
	m_pPoints->Get_Table().Add_Field("Y"				, TABLE_FIELDTYPE_Double);
	m_pPoints->Get_Table().Add_Field("Z"				, TABLE_FIELDTYPE_Double);

	for(i=0; i<m_pValues->Get_Count(); i++)
	{
		m_pPoints->Get_Table().Add_Field(m_pValues->asGrid(i)->Get_Name(), TABLE_FIELDTYPE_Double);
	}

	//-----------------------------------------------------
	if( (pLine = m_pLine->Get_Shape(0)) != NULL && pLine->Get_Point_Count(0) > 1 )
	{
		B	= pLine->Get_Point(0);

		for(i=1; i<pLine->Get_Point_Count(0); i++)
		{
			A	= B;
			B	= pLine->Get_Point(i);

			Set_Profile(A, B);
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pLine	, false);
	DataObject_Update(m_pPoints	, false);

	return( m_pPoints->Get_Count() > 0 );
}

//---------------------------------------------------------
bool CGrid_Profile::Set_Profile(TSG_Point A, TSG_Point B)
{
	double		dx, dy, d, n;
	TSG_Point	p;

	//-----------------------------------------------------
	dx	= fabs(B.x - A.x);
	dy	= fabs(B.y - A.y);

	if( dx > 0.0 || dy > 0.0 )
	{
		if( dx > dy )
		{
			dx	/= Get_Cellsize();
			n	 = dx;
			dy	/= dx;
			dx	 = Get_Cellsize();
		}
		else
		{
			dy	/= Get_Cellsize();
			n	 = dy;
			dx	/= dy;
			dy	 = Get_Cellsize();
		}

		if( B.x < A.x )
		{
			dx	= -dx;
		}

		if( B.y < A.y )
		{
			dy	= -dy;
		}

		//-------------------------------------------------
		for(d=0.0, p.x=A.x, p.y=A.y; d<=n; d++, p.x+=dx, p.y+=dy)
		{
			Add_Point(p);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_Profile::Add_Point(CSG_Point Point)
{
	int			x, y, i;
	double		z, Distance, Distance_2;
	CSG_Shape	*pPoint, *pLast;

	if( Get_System()->Get_World_to_Grid(x, y, Point) && m_pDEM->is_InGrid(x, y) )
	{
		z	= m_pDEM->asDouble(x, y);

		if( m_pPoints->Get_Count() == 0 )
		{
			Distance	= 0.0;
			Distance_2	= 0.0;
		}
		else
		{
			pLast		= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);
			Distance	= SG_Get_Distance(Point, pLast->Get_Point(0));

			if( Distance == 0.0 )
			{
				return( false );
			}

			Distance_2	= pLast->Get_Record()->asDouble(5) - z;
			Distance_2	= sqrt(Distance*Distance + Distance_2*Distance_2);

			Distance	+= pLast->Get_Record()->asDouble(1);
			Distance_2	+= pLast->Get_Record()->asDouble(2);
		}

		pPoint	= m_pPoints->Add_Shape();
		pPoint->Add_Point(Point);

		pPoint->Get_Record()->Set_Value(0, m_pPoints->Get_Count());
		pPoint->Get_Record()->Set_Value(1, Distance);
		pPoint->Get_Record()->Set_Value(2, Distance_2);
		pPoint->Get_Record()->Set_Value(3, Point.Get_X());
		pPoint->Get_Record()->Set_Value(4, Point.Get_Y());
		pPoint->Get_Record()->Set_Value(5, z);

		for(i=0; i<m_pValues->Get_Count(); i++)
		{
			pPoint->Get_Record()->Set_Value(VALUE_OFFSET + i, m_pValues->asGrid(i)->asDouble(x, y, true));
		}

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
