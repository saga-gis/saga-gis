
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "Grid_Profile.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FIELD_ID = 0,
	FIELD_DISTANCE,
	FIELD_OVERLAND,
	FIELD_X,
	FIELD_Y,
	FIELD_Z,
	FIELD_VALUES
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Profile::CGrid_Profile(void)
{
	Set_Name		(_TL("Profile"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description	(_TW(
		"Create interactively profiles from a grid based DEM\n"
		"Use left mouse button clicks into a map window to add profile points."
		"A right mouse button click will finish the profile."
	));

	Parameters.Add_Grid     ("", "DEM"    , _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("", "VALUES" , _TL("Values"),
		_TL("Additional values to be collected along profile."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes   ("", "POINTS" , _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes   ("", "LINE"   , _TL("Profile Line"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Bool     ("", "DIAGRAM", _TL("Show Diagram"),
		_TL(""),
		true
	)->Set_UseInCMD(false);

	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile::On_Execute(void)
{
	m_bAdd    = false;

	m_pDEM    = Parameters("DEM"   )->asGrid();
	m_pValues = Parameters("VALUES")->asGridList();
	m_pPoints = Parameters("POINTS")->asShapes();
	m_pLine   = Parameters("LINE"  )->asShapes();

	//-----------------------------------------------------
	m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("Profile [%s]"), m_pDEM->Get_Name()));
	m_pLine->Add_Field("ID"	, SG_DATATYPE_Int);

	DataObject_Update(m_pLine, SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", _TL("Profile"), m_pDEM->Get_Name()));

	m_pPoints->Add_Field("ID"      , SG_DATATYPE_Int   ); // FIELD_ID
	m_pPoints->Add_Field("Distance", SG_DATATYPE_Double); // FIELD_DISTANCE
	m_pPoints->Add_Field("Overland", SG_DATATYPE_Double); // FIELD_OVERLAND
	m_pPoints->Add_Field("X"       , SG_DATATYPE_Double); // FIELD_X
	m_pPoints->Add_Field("Y"       , SG_DATATYPE_Double); // FIELD_Y
	m_pPoints->Add_Field("Z"       , SG_DATATYPE_Double); // FIELD_Z

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		m_pPoints->Add_Field(m_pValues->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	if( Parameters("DIAGRAM")->asBool() )
	{
		CSG_Parameters P; CSG_String Fields(CSG_Parameter_Table_Field::Get_Choices(*m_pPoints, true));

		P.Add_Int   ("", "WINDOW_ARRANGE", "", "", SG_UI_WINDOW_ARRANGE_MDI_TILE_HOR|SG_UI_WINDOW_ARRANGE_TDI_SPLIT_BOTTOM);

		P.Add_Bool  ("", "LEGEND"        , "", "", false);
		P.Add_Bool  ("", "Y_SCALE_TO_X"  , "", "", true     );
		P.Add_Double("", "Y_SCALE_RATIO" , "", "", 1.       );
		P.Add_Choice("", "X_FIELD"       , "", "", Fields, FIELD_DISTANCE); // Distance

		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z), "", "", true); // Z

		SG_UI_Diagram_Show(m_pPoints, &P);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Profile::On_Execute_Finish(void)
{
	if( m_bAdd )
	{
		Set_Profile();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	default:
		break;

	case TOOL_INTERACTIVE_LDOWN:
		if( !m_bAdd )
		{
			m_bAdd = true;
			m_pLine->Del_Shapes();
			m_pLine->Add_Shape()->Set_Value(0, 1);
		}

		m_pLine->Get_Shape(0)->Add_Point(Get_System().Fit_to_Grid_System(ptWorld));
		DataObject_Update(m_pLine);
		break;

	case TOOL_INTERACTIVE_RDOWN:
		Set_Profile();
		m_bAdd = false;
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile::Set_Profile(void)
{
	m_pPoints->Del_Shapes();

	CSG_Shape *pLine = m_pLine->Get_Shape(0);

	if( pLine == NULL || pLine->Get_Point_Count(0) < 2 )
	{
		return( false );
	}

	if( pLine && pLine->Get_Point_Count(0) > 1 )
	{
		CSG_Point B = pLine->Get_Point();

		for(int i=1; i<pLine->Get_Point_Count(0); i++)
		{
			CSG_Point A = B; B = pLine->Get_Point(i);

			if( !A.is_Equal(B) )
			{
				Set_Profile(A, B);
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pLine  );
	DataObject_Update(m_pPoints);

	return( m_pPoints->Get_Count() > 0 );
}

//---------------------------------------------------------
bool CGrid_Profile::Set_Profile(TSG_Point A, TSG_Point B)
{
	double dx = fabs(B.x - A.x);
	double dy = fabs(B.y - A.y), n;

	if( dx <= 0. && dy <= 0. )
	{
		return( false );
	}

	if( dx > dy )
	{
		dx /= Get_Cellsize();
		n   = dx;
		dy /= dx;
		dx  = Get_Cellsize();
	}
	else
	{
		dy /= Get_Cellsize();
		n   = dy;
		dx /= dy;
		dy  = Get_Cellsize();
	}

	dx = A.x < B.x ? dx : -dx;
	dy = A.y < B.y ? dy : -dy;

	//-----------------------------------------------------
	for(double d=0.; d<=n; d++, A.x+=dx, A.y+=dy)
	{
		Add_Point(A);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Profile::Add_Point(CSG_Point Point)
{
	int x, y; Get_System().Get_World_to_Grid(x, y, Point);

	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double z = m_pDEM->asDouble(x, y), Distance, Overland;

	if( m_pPoints->Get_Count() == 0 )
	{
		Distance = Overland = 0.;
	}
	else
	{
		CSG_Shape *pLast = m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);

		Distance = SG_Get_Distance(Point, pLast->Get_Point());

		if( Distance == 0. )
		{
			return( false );
		}

		Overland  = pLast->asDouble(FIELD_Z) - z;
		Overland  = sqrt(Distance*Distance + Overland*Overland);

		Distance += pLast->asDouble(FIELD_DISTANCE);
		Overland += pLast->asDouble(FIELD_OVERLAND);
	}

	//-----------------------------------------------------
	CSG_Shape *pPoint = m_pPoints->Add_Shape();

	pPoint->Add_Point(Point);

	pPoint->Set_Value(FIELD_ID      , m_pPoints->Get_Count());
	pPoint->Set_Value(FIELD_DISTANCE, Distance);
	pPoint->Set_Value(FIELD_OVERLAND, Overland);
	pPoint->Set_Value(FIELD_X       , Point.x);
	pPoint->Set_Value(FIELD_Y       , Point.y);
	pPoint->Set_Value(FIELD_Z       ,       z);

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		pPoint->Set_Value(FIELD_VALUES + i, m_pValues->Get_Grid(i)->asDouble(x, y));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
