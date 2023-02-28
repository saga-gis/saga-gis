
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
//              Grid_ProfileFromPoints.cpp               //
//                                                       //
//              Copyright (C) 2004 - 2022 by             //
//            Victor Olaya and Volker Wichmann           //
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
#include "Grid_ProfileFromPoints.h"


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
CProfileFromPoints::CProfileFromPoints(void)
{
	Set_Name		(_TL("Profile from Points"));

	Set_Author		("V.Olaya, V. Wichmann (c) 2004-2022");

	Set_Description (_TW(
		"The tool allows one to query a profile from an input grid "
		"(usually a DEM) for point coordinates stored in a table "
		"or shapefile. The profile is traced from one point to "
		"the next, sampling the grid values along each line segment. "
		"Optionally, additional grids can be queried "
		"whose values are added to the profile table. "
	));

	Parameters.Add_Grid	      (""     , "GRID"   , _TL("Elevation"),
		_TL("The input grid to query."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List  (""     , "VALUES" , _TL("Values"),
		_TL("Additional values to be collected along profile."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table      (""     , "TABLE"  , _TL("Coordinates Table"),
		_TL("Table with the point coordinates to query."),	
		PARAMETER_INPUT
	);
	Parameters.Add_Table_Field("TABLE", "X"      , _TL("X Coordinate"), _TL(""));
	Parameters.Add_Table_Field("TABLE", "Y"      , _TL("Y Coordinate"), _TL(""));

	Parameters.Add_Table      (""     , "RESULT" , _TL("Profile"),
		_TL("The output table with the queried profile values."), 
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool       (""     , "DIAGRAM", _TL("Show Diagram"),
		_TL(""),
		true
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProfileFromPoints::On_Execute(void)
{
	m_pDEM    = Parameters("GRID"  )->asGrid();
	m_pValues = Parameters("VALUES")->asGridList();

	//---------------------------------------------------------
	m_pPoints = Parameters("RESULT")->asTable();
	m_pPoints->Destroy();

	m_pPoints->Fmt_Name("%s [%s]", _TL("Profile"), m_pDEM->Get_Name());

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

	//---------------------------------------------------------
	CSG_Table *pTable = Parameters("TABLE")->asTable();
	int xField = Parameters("X")->asInt();
	int yField = Parameters("Y")->asInt();	

	CSG_Point B(
		pTable->Get_Record(0)->asDouble(xField),
		pTable->Get_Record(0)->asDouble(yField)
	);

	for(int i=1; i<pTable->Get_Record_Count(); i++)
	{
		CSG_Point A = B;

		B.x = pTable->Get_Record(i)->asDouble(xField);
		B.y = pTable->Get_Record(i)->asDouble(yField);

		Set_Profile(A, B, i == (pTable->Get_Record_Count() - 1));
	}

	//---------------------------------------------------------
	if( Parameters("DIAGRAM")->asBool() )
	{
		CSG_Parameters P; CSG_String Fields(CSG_Parameter_Table_Field::Get_Choices(*m_pPoints, true));

		P.Add_Bool  ("", "LEGEND"       , "", "", false);
		P.Add_Bool  ("", "Y_SCALE_TO_X" , "", "", true     );
		P.Add_Double("", "Y_SCALE_RATIO", "", "", 1.       );
		P.Add_Choice("", "X_FIELD"      , "", "", Fields, FIELD_DISTANCE); // Distance

		P.Add_Bool  ("", CSG_String::Format("FIELD_%d", FIELD_Z), "", "", true); // Z

		SG_UI_Diagram_Show(m_pPoints, &P);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProfileFromPoints::Set_Profile(const CSG_Point &A, const CSG_Point &B, bool bLastPoint)
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

	//-------------------------------------------------
	CSG_Point P(A);

	for(double d=0.; d<=n; d++, P.x+=dx, P.y+=dy)
	{
		Add_Point(P);
	}

	if( bLastPoint && SG_Get_Distance(P, B) > M_ALMOST_ZERO )
	{
		Add_Point(B);
	}

	return( true );
}

//---------------------------------------------------------
bool CProfileFromPoints::Add_Point(const CSG_Point &Point)
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
		CSG_Table_Record *pLast = m_pPoints->Get_Record(m_pPoints->Get_Count() - 1);

		Distance = SG_Get_Distance(Point.x, Point.y, pLast->asDouble(FIELD_X), pLast->asDouble(FIELD_Y));

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
	CSG_Table_Record *pPoint = m_pPoints->Add_Record();

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
