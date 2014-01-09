/**********************************************************
 * Version $Id: pgis_raster.cpp 1646 2013-04-10 16:29:00Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       io_pgsql                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pgis_raster.cpp                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//                University of Hamburg                  //
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
#include "pgis_raster.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRaster_Load::CRaster_Load(void)
{
	Set_Name		(_TL("Import Raster from PostGIS"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Imports grids from a PostGIS database."
	));

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
void CRaster_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, SG_T("raster_columns")) )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString(SG_T("r_table_name")) + CSG_String("|");
		}
	}

	pParameters->Get_Parameter("TABLES")->asChoice()->Set_Items(s);
}

//---------------------------------------------------------
bool CRaster_Load::On_Execute(void)
{
	CSG_String	Table	= Parameters("TABLES")->asString();

	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	pGrids->Del_Items();

	if( !Get_Connection()->Raster_Load(Table, "", "", "", pGrids) )//, "rid=4") )
	{
		return( false );
	}

	return( pGrids->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRaster_Save::CRaster_Save(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Export Raster to PostGIS"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Exports grids to a PostGIS database."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid_System(
		NULL	, "GRID_SYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_List(
		pNode	, "GRIDS"		, _TL("Bands"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Table Name"),
		_TL(""),
		""
	);

	Parameters.Add_Value(
		NULL	, "GRID_NAME"	, _TL("Add Grid Name Field"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Add_SRID_Picker();
}

//---------------------------------------------------------
bool CRaster_Save::On_Execute(void)
{
	if( !Get_Connection()->Table_Exists(SG_T("spatial_ref_sys")) || !Get_Connection()->Table_Exists(SG_T("geometry_columns")) )
	{
		SG_UI_Dlg_Message(_TL("Not a valid PostGIS database!"), _TL("Database Connection Error"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	CSG_String	SavePoint, Name	= Parameters("NAME")->asString();

	if( Name.Length() == 0 )
		return( false );

	bool	bGridName	= Parameters("GRID_NAME")->asBool();

	//-----------------------------------------------------
	Get_Connection()->Begin(SavePoint = Get_Connection()->is_Transaction() ? "RASTER_SAVE" : "");

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Exists(Name) )
	{
		CSG_String	SQL	= "CREATE TABLE \"" + Name + "\" (\"rid\" serial PRIMARY KEY, \"raster\" raster"
			+ (bGridName ? ", \"name\" varchar(64))" : ")");
		
		if( !Get_Connection()->Execute(SQL) )
		{
			Get_Connection()->Rollback(SavePoint);

			return( false );
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		CSG_Bytes	WKB;

		Process_Set_Text(CSG_String::Format(SG_T("%s: %s [%d/%d]"), _TL("export grid"), pGrids->asGrid(i)->Get_Name(), i + 1, pGrids->Get_Count()));

		if( !bGridName )
		{
			if( !Get_Connection()->Raster_Save(pGrids->asGrid(i), Get_SRID(), Name, "") )
			{
				Get_Connection()->Rollback(SavePoint);

				return( false );
			}
		}
		else if( CSG_Grid_OGIS_Converter::to_WKBinary(WKB, pGrids->asGrid(i), Get_SRID()) )
		{
			CSG_String	SQL	= "INSERT INTO \"" + Name + "\" (\"raster\", \"name\") VALUES("
				+ "ST_AddBand('" + WKB.toHexString() + "'::raster, '"
				+ CSG_PG_Connection::Get_Raster_Type_To_SQL(pGrids->asGrid(i)->Get_Type()) + "'::text, 0, NULL), '"
				+ pGrids->asGrid(i)->Get_Name() + "')";

			if( !Get_Connection()->Execute(SQL) )
			{
				Get_Connection()->Rollback(SavePoint);

				return( false );
			}
		}
	}

	//-----------------------------------------------------
	Get_Connection()->Commit(SavePoint);

	Get_Connection()->GUI_Update();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRaster_SRID_Update::CRaster_SRID_Update(void)
{
	Set_Name		(_TL("Update Raster SRID"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		" Change the SRID of all rasters in the user-specified column and table."
	));

	Parameters.Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		""
	);

	Add_SRID_Picker();
}

//---------------------------------------------------------
void CRaster_SRID_Update::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, SG_T("raster_columns")) )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString(SG_T("r_table_name")) + CSG_String("|");
		}
	}

	pParameters->Get_Parameter("TABLES")->asChoice()->Set_Items(s);
}

//---------------------------------------------------------
bool CRaster_SRID_Update::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS(2.1) )	{	Error_Set(_TL("not supported by PostGIS versions less than 2.1"));	return( false );	}

	//-----------------------------------------------------
	CSG_String	Select;
	CSG_Table	Table;

	Select.Printf(SG_T("r_table_name='%s'"), Parameters("TABLES")->asString());

	if( !Get_Connection()->Table_Load(Table, "raster_columns", "*", Select) || Table.Get_Count() != 1 )
	{
		return( false );
	}

	Select.Printf(SG_T("SELECT UpdateRasterSRID('%s', '%s', %d)"),
		Parameters("TABLES")->asString(),
		Table[0].asString("r_raster_column"),
		Get_SRID()
	);

	//-----------------------------------------------------
	if( !Get_Connection()->Execute(Select) )
	{
		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
	