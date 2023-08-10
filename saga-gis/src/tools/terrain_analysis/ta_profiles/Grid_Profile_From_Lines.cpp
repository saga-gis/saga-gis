
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
//              Grid_Profile_From_Lines.cpp              //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Grid_Profile_From_Lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FIELD_ID_LINE = 0,
	FIELD_ID_POINT,
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
CGrid_Profile_From_Lines::CGrid_Profile_From_Lines(void)
{
	Set_Name		(_TL("Profiles from Lines"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Create profiles from a grid based DEM for each line of a lines layer. "
	));

	Parameters.Add_Grid       (""     , "DEM"     , _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List  (""     , "VALUES"  , _TL("Values"),
		_TL("Additional values to be collected along profile."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes     (""     , "LINES"   , _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Field("LINES", "NAME"    , _TL("Name"),
		_TL("Attribute to use for splitted line naming (=> each line as new profile)"),
		true
	);

	Parameters.Add_Shapes     (""     , "PROFILE" , _TL("Profiles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes_List(""     , "PROFILES", _TL("Profiles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Bool       (""     , "SPLIT"   , _TL("Each Line as new Profile"),
		_TL(""),
		false
	);

	Parameters.Add_Bool       (""     , "DIAGRAM", _TL("Show Diagram"),
		_TL(""),
		false
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Profile_From_Lines::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SPLIT") )
	{
		pParameters->Set_Enabled("PROFILE" , pParameter->asBool() == false);
	//	pParameters->Set_Enabled("PROFILES", pParameter->asBool() ==  true); // not necessary, (optional) output lists are never visible!
		pParameters->Set_Enabled("NAME"    , pParameter->asBool() ==  true);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::On_Execute(void)
{
	m_pDEM    = Parameters("DEM"   )->asGrid();
	m_pValues = Parameters("VALUES")->asGridList();
	m_pLines  = Parameters("LINES" )->asShapes();

	//-----------------------------------------------------
	if( Parameters("SPLIT")->asBool() == false )
	{
		if( (m_pPoints = Parameters("PROFILE")->asShapes()) == NULL )
		{
			Parameters("PROFILE")->Set_Value(m_pPoints = SG_Create_Shapes(SHAPE_TYPE_Point));
		}

		Init_Profile(m_pPoints, CSG_String::Format("%s [%s]", m_pDEM->Get_Name(), _TL("Profile")));

		for(sLong i=0; i<m_pLines->Get_Count() && Set_Progress(i, m_pLines->Get_Count()); i++)
		{
			Set_Profile(i, m_pLines->Get_Shape(i));
		}

		Show_Profile(m_pPoints);

		return( true );
	}

	//-----------------------------------------------------
	else // if( Parameters("SPLIT")->asBool() == true )
	{
		int Name = Parameters("NAME")->asInt();

		Parameters("PROFILES")->asShapesList()->Del_Items();

		for(sLong i=0; i<m_pLines->Get_Count() && Set_Progress(i, m_pLines->Get_Count()); i++)
		{
			Init_Profile(m_pPoints = SG_Create_Shapes(), Name < 0
				? CSG_String::Format("%s [%s %d]", m_pDEM->Get_Name(), _TL("Profile"), i + 1)
				: CSG_String::Format("%s [%s %s]", m_pDEM->Get_Name(), _TL("Profile"), m_pLines->Get_Shape(i)->asString(Name))
			);

			Set_Profile(i, m_pLines->Get_Shape(i));

			Parameters("PROFILES")->asShapesList()->Add_Item(m_pPoints);

			Show_Profile(m_pPoints);
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Init_Profile(CSG_Shapes *pPoints, const CSG_String &Name)
{
	if( pPoints )
	{
		pPoints->Create(SHAPE_TYPE_Point, Name);

		pPoints->Add_Field("ID_LINE" , SG_DATATYPE_Long  ); // FIELD_ID_LINE
		pPoints->Add_Field("ID_POINT", SG_DATATYPE_Int   ); // FIELD_ID_POINT
		pPoints->Add_Field("DISTANCE", SG_DATATYPE_Double); // FIELD_DISTANCE
		pPoints->Add_Field("OVERLAND", SG_DATATYPE_Double); // FIELD_OVERLAND
		pPoints->Add_Field("X"       , SG_DATATYPE_Double); // FIELD_X
		pPoints->Add_Field("Y"       , SG_DATATYPE_Double); // FIELD_Y
		pPoints->Add_Field("Z"       , SG_DATATYPE_Double); // FIELD_Z

		for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
		{
			pPoints->Add_Field(m_pValues->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Show_Profile(CSG_Shapes *pPoints)
{
	if( Parameters("DIAGRAM")->asBool() )
	{
		CSG_Parameters P; CSG_String Fields(CSG_Parameter_Table_Field::Get_Choices(*pPoints, true));

		P.Add_Bool  ("", "LEGEND"       , "", "", false);
		P.Add_Bool  ("", "Y_SCALE_TO_X" , "", "", true     );
		P.Add_Double("", "Y_SCALE_RATIO", "", "", 1.       );
		P.Add_Choice("", "X_FIELD"      , "", "", Fields, FIELD_DISTANCE); // Distance

		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z), "", "", true); // Z

		DataObject_Add(pPoints);

		return( SG_UI_Diagram_Show(pPoints, &P) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Set_Profile(sLong ID_Line, CSG_Shape *pLine)
{
	if( pLine && pLine->Get_Point_Count(0) < 2 )
	{
		return( false );
	}

	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		CSG_Point B = pLine->Get_Point(0, iPart);

		for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
		{
			CSG_Point A = B; B = pLine->Get_Point(iPoint, iPart);

			Set_Profile(ID_Line, iPoint == 1, A, B, iPart == pLine->Get_Part_Count() - 1 && iPoint == pLine->Get_Point_Count(iPart) - 1);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Set_Profile(sLong ID_Line, bool bStart, const TSG_Point &A, const TSG_Point &B, bool bLastPoint)
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
	CSG_Point P(A);

	for(double d=0.; d<=n; d++, P.x+=dx, P.y+=dy)
	{
		Add_Point(ID_Line, bStart, P);

		bStart = false;
	}

	if( bLastPoint && SG_Get_Distance(P, B) > M_ALMOST_ZERO )
	{
		Add_Point(ID_Line, bStart, B);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Add_Point(sLong ID_Line, bool bStart, const TSG_Point &Point)
{
	int x, y; Get_System().Get_World_to_Grid(x, y, Point);

	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double z = m_pDEM->asDouble(x, y), Distance, Overland;

	if( bStart || m_pPoints->Get_Count() == 0 )
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

	pPoint->Set_Value(FIELD_ID_LINE , ID_Line);
	pPoint->Set_Value(FIELD_ID_POINT, m_pPoints->Get_Count());
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
