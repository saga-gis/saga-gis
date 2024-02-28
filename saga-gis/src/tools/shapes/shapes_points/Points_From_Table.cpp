
/*******************************************************************************
    Points_From_Table.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Points_From_Table.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_From_Table::CPoints_From_Table(void)
{
	Set_Name		(_TL("Construct Points from Table"));

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"Construct a points layer from coordinates "
		"as provided by a table's attributes. "
	));

	Parameters.Add_Table("", "TABLE", _TL("Table"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Table_Field("TABLE", "X", _TL("X"), _TL(""));
	Parameters.Add_Table_Field("TABLE", "Y", _TL("Y"), _TL(""));
	Parameters.Add_Table_Field("TABLE", "Z", _TL("Z"), _TL(""), true);

	Parameters.Add_Shapes("", "POINTS", _TL("Points"), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Point);

	m_CRS.Create(Parameters, "POINTS");
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_Table::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CPoints_From_Table::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CPoints_From_Table::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_Table::On_Execute(void)
{
	CSG_Table &Table = *Parameters("TABLE")->asTable();

	int x = Parameters("X")->asInt();
	int y = Parameters("Y")->asInt();
	int z = Parameters("Z")->asInt();

	if( Table.Get_Field_Count() < 1 || Table.Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid input table"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes &Points = *Parameters("POINTS")->asShapes();

	Points.Create(SHAPE_TYPE_Point, Table.Get_Name(), &Table, z < 0 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZ);

	m_CRS.Get_CRS(Points.Get_Projection(), true);

	//-----------------------------------------------------
	for(sLong i=0; i<Table.Get_Count() && Set_Progress(i, Table.Get_Count()); i++)
	{
		CSG_Table_Record &Record = *Table.Get_Record(i);

		if( !Record.is_NoData(x) && !Record.is_NoData(y) )
		{
			CSG_Shape &Point = *Points.Add_Shape(&Record, SHAPE_COPY_ATTR);

			Point.Set_Point(Record.asDouble(x), Record.asDouble(y));

			if( z >= 0 )
			{
				Point.Set_Z(Record.asDouble(z));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
