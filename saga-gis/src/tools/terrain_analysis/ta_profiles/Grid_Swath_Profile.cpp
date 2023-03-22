
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
//                 Grid_Swath_Profile.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Grid_Swath_Profile.h"


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
	FIELD_X,
	FIELD_Y,
	FIELD_Z,
	FIELD_Z_MEAN,
	FIELD_Z_MIN,
	FIELD_Z_MAX,
	FIELD_Z_SD_MIN,
	FIELD_Z_SC_MAX,
	FIELD_VALUES
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Swath_Profile::CGrid_Swath_Profile(void)
{
	Set_Name		(_TL("Swath Profile"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Create interactively swath profiles from a grid based DEM\n"
		"Use left mouse button clicks into a map window to add profile points."
		"A right mouse button click will finish the profile.\n"
		"Generated outputs for the swath profile are arithmetic mean, "
		"minimum, maximum values and the standard deviation.\n"
	));

	Parameters.Add_Grid     ("", "DEM"   , _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("", "VALUES", _TL("Values"),
		_TL("Additional values to be collected."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes   ("", "POINTS", _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes   ("", "LINE"  , _TL("Swath Profile"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double   ("", "WIDTH" , _TL("Swath Width"),
		_TL("Swath width measured in map units."),
		100., 0., true
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
bool CGrid_Swath_Profile::On_Execute(void)
{
	m_bAdd    = false;

	m_pDEM    = Parameters("DEM"   )->asGrid();
	m_pValues = Parameters("VALUES")->asGridList();
	m_pPoints = Parameters("POINTS")->asShapes();
	m_pLine   = Parameters("LINE"  )->asShapes();
	m_Width   = Parameters("WIDTH" )->asDouble() / 2.;

	//-----------------------------------------------------
	m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", _TL("Profile"), m_pDEM->Get_Name()));
	m_pLine->Add_Field("ID", SG_DATATYPE_Int);

	DataObject_Update(m_pLine, SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", _TL("Profile"), m_pDEM->Get_Name()));

	m_pPoints->Add_Field("ID"        , SG_DATATYPE_Int   ); // FIELD_ID
	m_pPoints->Add_Field("Distance"  , SG_DATATYPE_Double); // FIELD_DISTANCE
	m_pPoints->Add_Field("X"         , SG_DATATYPE_Double); // FIELD_X
	m_pPoints->Add_Field("Y"         , SG_DATATYPE_Double); // FIELD_Y
	m_pPoints->Add_Field("Z"         , SG_DATATYPE_Double); // FIELD_Z
	m_pPoints->Add_Field("Z (mean)"  , SG_DATATYPE_Double); // FIELD_Z_MEAN
	m_pPoints->Add_Field("Z (min)"   , SG_DATATYPE_Double); // FIELD_Z_MIN
	m_pPoints->Add_Field("Z (max)"   , SG_DATATYPE_Double); // FIELD_Z_MAX
	m_pPoints->Add_Field("Z (min_sd)", SG_DATATYPE_Double); // FIELD_Z_SD_MIN
	m_pPoints->Add_Field("Z (max_sd)", SG_DATATYPE_Double); // FIELD_Z_SD_MAX

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		m_pPoints->Add_Field(m_pValues->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format("%s (mean)"  , m_pValues->Get_Grid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format("%s (min)"   , m_pValues->Get_Grid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format("%s (max)"   , m_pValues->Get_Grid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format("%s (min_sd)", m_pValues->Get_Grid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format("%s (max_sd)", m_pValues->Get_Grid(i)->Get_Name()), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	if( Parameters("DIAGRAM")->asBool() )
	{
		CSG_Parameters P; CSG_String Fields(CSG_Parameter_Table_Field::Get_Choices(*m_pPoints, true));

		P.Add_Int   ("", "WINDOW_ARRANGE", "", "", SG_UI_WINDOW_ARRANGE_MDI_TILE_HOR|SG_UI_WINDOW_ARRANGE_TDI_SPLIT_BOTTOM);

		P.Add_Bool  ("", "LEGEND"        , "", "", false    );
		P.Add_Bool  ("", "Y_SCALE_TO_X"  , "", "", true     );
		P.Add_Double("", "Y_SCALE_RATIO" , "", "", 1.       );
		P.Add_Choice("", "X_FIELD"       , "", "", Fields, FIELD_DISTANCE); // Distance

		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z     ), "", "", true);
		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z_MEAN), "", "", true);
		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z_MIN ), "", "", true);
		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z_MAX ), "", "", true);

		SG_UI_Diagram_Show(m_pPoints, &P);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::On_Execute_Finish(void)
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
bool CGrid_Swath_Profile::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
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
bool CGrid_Swath_Profile::Set_Profile(void)
{
	m_pPoints->Del_Shapes();

	CSG_Shape *pLine = m_pLine->Get_Shape(0);

	if( pLine && pLine->Get_Point_Count(0) > 1 )
	{
		CSG_Shape *pLeft  = m_pLine->Get_Shape(1); if( pLeft  ) pLeft ->Del_Parts(); else pLeft  = m_pLine->Add_Shape();
		CSG_Shape *pRight = m_pLine->Get_Shape(2); if( pRight ) pRight->Del_Parts(); else pRight = m_pLine->Add_Shape();

		CSG_Point B = pLine->Get_Point(0);

		for(int i=1; i<pLine->Get_Point_Count(0); i++)
		{
			CSG_Point A = B; B = pLine->Get_Point(i);

			if( !A.is_Equal(B) )
			{
				CSG_Point P = B - A; double d = m_Width / SG_Get_Distance(A, B);

				P.Assign(-d * P.y, d * P.x);

				CSG_Point Left  = A - P;
				CSG_Point Right = A + P;

				Set_Profile(A, B, Left, Right);

				pLeft ->Add_Point(Left , i - 1); Left  = B - P; pLeft ->Add_Point(Left , i - 1);
				pRight->Add_Point(Right, i - 1); Right = B + P; pRight->Add_Point(Right, i - 1);
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pLine  );
	DataObject_Update(m_pPoints);

	return( m_pPoints->Get_Count() > 0 );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::Set_Profile(CSG_Point A, CSG_Point B, CSG_Point Left, CSG_Point Right)
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
	dy = A.x < B.x ? dy : -dy;

	//-----------------------------------------------------
	CSG_Point dStep( dx, dy);
	CSG_Point Step (-dy, dx);

	if( fabs(Step.x) > fabs(Step.y) )
	{
		if( Left.x > Right.x )
		{
			CSG_Point p = Left; Left = Right; Right = p;
		}

		if( Step.x < 0. )
		{
			Step.Assign(-Step.x, -Step.y);
		}
	}
	else
	{
		if( Left.y > Right.y )
		{
			CSG_Point p = Left; Left = Right; Right = p;
		}

		if( Step.y < 0. )
		{
			Step.Assign(-Step.x, -Step.y);
		}
	}

	//-------------------------------------------------
	for(double d=0.; d<=n; d++, A+=dStep, Left+=dStep, Right+=dStep)
	{
		Add_Point(A, Left, Right, Step);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::Add_Point(CSG_Point Point, CSG_Point Left, CSG_Point Right, CSG_Point Step)
{
	if( !m_pDEM->is_InGrid_byPos(Point) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double Distance;

	if( m_pPoints->Get_Count() == 0 )
	{
		Distance = 0.;
	}
	else
	{
		CSG_Shape *pLast = m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);

		Distance = SG_Get_Distance(Point, pLast->Get_Point(0));

		if( Distance == 0. )
		{
			return( false );
		}

		Distance += pLast->asDouble(FIELD_DISTANCE);
	}

	//-----------------------------------------------------
	CSG_Shape *pPoint = m_pPoints->Add_Shape();

	pPoint->Add_Point(Point);

	pPoint->Set_Value(FIELD_ID      , m_pPoints->Get_Count());
	pPoint->Set_Value(FIELD_DISTANCE, Distance);
	pPoint->Set_Value(FIELD_X       , Point.x);
	pPoint->Set_Value(FIELD_Y       , Point.y);

	Add_Swath(pPoint, FIELD_Z, m_pDEM, Left, Right, Step);

	for(int i=0, j=FIELD_VALUES; i<m_pValues->Get_Grid_Count(); i++, j+=6)
	{
		Add_Swath(pPoint, j, m_pValues->Get_Grid(i), Left, Right, Step);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::Add_Swath(CSG_Shape *pPoint, int iEntry, CSG_Grid *pGrid, CSG_Point Left, CSG_Point Right, CSG_Point Step)
{
	double Value;

	if( pGrid->Get_Value(pPoint->Get_Point(0), Value) )
	{
		pPoint->Set_Value(iEntry, Value);
	}
	else
	{
		pPoint->Set_NoData(iEntry);
	}

	//-----------------------------------------------------
	double iRun, dRun, nRun;

	if( Step.x > Step.y )
	{
		iRun = Left .x;
		dRun = Step .x;
		nRun = Right.x;
	}
	else
	{
		iRun = Left .y;
		dRun = Step .y;
		nRun = Right.y;
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics Statistics;

	for( ; iRun<=nRun; iRun+=dRun, Left+=Step)
	{
		if( pGrid->Get_Value(Left, Value) )
		{
			Statistics += Value;
		}
	}

	//-----------------------------------------------------
	if( Statistics.Get_Count() > 0 )
	{
		pPoint->Set_Value(iEntry + 1, Statistics.Get_Mean   ());
		pPoint->Set_Value(iEntry + 2, Statistics.Get_Minimum());
		pPoint->Set_Value(iEntry + 3, Statistics.Get_Maximum());
		pPoint->Set_Value(iEntry + 4, Statistics.Get_Mean() - Statistics.Get_StdDev());
		pPoint->Set_Value(iEntry + 5, Statistics.Get_Mean() + Statistics.Get_StdDev());

		return( true );
	}

	pPoint->Set_NoData(iEntry + 1);
	pPoint->Set_NoData(iEntry + 2);
	pPoint->Set_NoData(iEntry + 3);
	pPoint->Set_NoData(iEntry + 4);
	pPoint->Set_NoData(iEntry + 5);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
