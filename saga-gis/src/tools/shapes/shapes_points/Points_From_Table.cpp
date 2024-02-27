
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Points_From_Table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	Parameters.Add_String("POINTS", "CRS_PROJ"     , _TL("PROJ Parameters"), _TL(""),     "")->Set_UseInGUI(false);
	Parameters.Add_Int   ("POINTS", "CRS_CODE"     , _TL("Code ID"        ), _TL(""),     -1)->Set_UseInGUI(false);
	Parameters.Add_String("POINTS", "CRS_AUTHORITY", _TL("Code Authority" ), _TL(""), "EPSG")->Set_UseInGUI(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_Table::On_Before_Execution(void)
{
	if( has_GUI() )
	{
		m_pCRS = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 15, true);	// CCRS_Picker

		m_pCRS->Set_Parameter("CRS_EPSG"     , Parameters["CRS_CODE"     ].asInt   ());
		m_pCRS->Set_Parameter("CRS_EPSG_AUTH", Parameters["CRS_AUTHORITY"].asString());
		m_pCRS->Set_Parameter("CRS_PROJ4"    , Parameters["CRS_PROJ"     ].asString());

		Parameters.Add_Parameters("POINTS", "CRS_PICKER", _TL("Coordinate Reference System"), _TL(""))
			->asParameters()->Create(*m_pCRS->Get_Parameters());
	}

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CPoints_From_Table::On_After_Execution(void)
{
	if( Parameters("CRS_PICKER") )
	{
		Parameters.Del_Parameter("CRS_PICKER");
		SG_Get_Tool_Library_Manager().Delete_Tool(m_pCRS);
		m_pCRS = NULL;
	}

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CPoints_From_Table::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CRS_PICKER") )
	{
		pParameters->Set_Parameter("CRS_CODE"     , (*pParameter->asParameters())("CRS_EPSG"     )->asInt   ());
		pParameters->Set_Parameter("CRS_AUTHORITY", (*pParameter->asParameters())("CRS_EPSG_AUTH")->asInt   ());
		pParameters->Set_Parameter("CRS_PROJ"     , (*pParameter->asParameters())("CRS_PROJ4"    )->asString());
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
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

	if( Points.Get_Projection().Create(Parameters["CRS_CODE"].asInt(), Parameters["CRS_AUTHORITY"].asString())
	||  Points.Get_Projection().Create(Parameters["CRS_PROJ"].asString(), SG_PROJ_FMT_Proj4) )
	{
		Message_Fmt("\n%s: %s\n", _TL("CRS"), Points.Get_Projection().Get_Proj4().c_str());
	}

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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
