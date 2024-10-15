
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       db_pgsql                        //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
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
	Set_Name		(_TL("Import Raster"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Imports grids from a PostGIS database."
	));

	Parameters.Add_Grid_List("", "GRIDS"    , _TL("Grids"         ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("", "DB_TABLES", _TL("Table"), _TL(""), "")->Set_UseInCMD(false);
	Parameters.Add_String("", "DB_TABLE" , _TL("Table"), _TL(""), "")->Set_UseInGUI(false);

	Parameters.Add_String   ("", "WHERE"    , _TL("Where"         ), _TL(""), "");
	Parameters.Add_Choice   ("", "MULTIPLE" , _TL("Multiple Bands"), _TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("single grids"),
			_TL("grid collection"),
			_TL("automatic")
		), 2
	);
}

//---------------------------------------------------------
void CRaster_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	if( has_GUI() )
	{
		CSG_String s; CSG_Table t;

		SG_UI_ProgressAndMsg_Lock(true);

		if( Get_Connection()->Table_Load(t, "raster_columns") )
		{
			for(sLong i=0; i<t.Get_Count(); i++)
			{
				s += t[i].asString("r_table_name") + CSG_String("|");
			}
		}

		SG_UI_ProgressAndMsg_Lock(false);

		CSG_Parameter *pParameter = pParameters->Get_Parameter("DB_TABLES");
		pParameter->asChoice()->Set_Items(s);
		pParameter->Set_Value(pParameter->asString());

		On_Parameter_Changed(pParameters, pParameters->Get_Parameter("DB_TABLES"));
	}
}

//---------------------------------------------------------
bool CRaster_Load::On_Execute(void)
{
	CSG_String DB_Table(Parameters(has_GUI() ? "DB_TABLES" : "DB_TABLE")->asString());

	CSG_Parameter_Grid_List *pGrids = Parameters("GRIDS")->asGridList(); pGrids->Del_Items();

	if( !Get_Connection()->Raster_Load(pGrids, DB_Table,
		Parameters("WHERE")->asString(), "", Parameters("MULTIPLE")->asInt()) )
	{
		Error_Fmt("%s:\n%s\n%s", _TL("unable to load raster data from PostGIS database"),
			Get_Connection()->Get_Connection().c_str(), DB_Table.c_str()
		);

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
CRaster_Load_Band::CRaster_Load_Band(void)
{
	Set_Name		(_TL("Import Single Raster Band"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Imports grids from a PostGIS database."
	));

	Parameters.Add_Grid_Output("", "GRID", _TL("Grid"), _TL(""));

	Parameters.Add_Choice("", "DB_TABLES", _TL("Table"), _TL(""), "")->Set_UseInCMD(false);
	Parameters.Add_String("", "DB_TABLE" , _TL("Table"), _TL(""), "")->Set_UseInGUI(false);

	Parameters.Add_Choice("", "BANDS" , _TL("Bands"          ), _TL(""), "")->Set_UseInCMD(false);
	Parameters.Add_String("", "RID"   , _TL("Band Identifier"), _TL(""), "")->Set_UseInGUI(false);
}

//---------------------------------------------------------
void CRaster_Load_Band::On_Connection_Changed(CSG_Parameters *pParameters)
{
	if( has_GUI() )
	{
		CSG_String s; CSG_Table t;

		SG_UI_ProgressAndMsg_Lock(true);

		if( Get_Connection()->Table_Load(t, "raster_columns") )
		{
			for(sLong i=0; i<t.Get_Count(); i++)
			{
				s += t[i].asString("r_table_name") + CSG_String("|");
			}
		}

		SG_UI_ProgressAndMsg_Lock(false);

		CSG_Parameter *pParameter = pParameters->Get_Parameter("DB_TABLES");
		pParameter->asChoice()->Set_Items(s);
		pParameter->Set_Value(pParameter->asString());

		On_Parameter_Changed(pParameters, pParameters->Get_Parameter("DB_TABLES"));
	}
}

//---------------------------------------------------------
int CRaster_Load_Band::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( has_GUI() && pParameter->Cmp_Identifier("DB_TABLES") )
	{
		CSG_String s; CSG_Table t;

		SG_UI_ProgressAndMsg_Lock(true);

		if( Get_Connection()->Table_Load(t, pParameter->asString(), "rid, name") )
		{
			for(sLong i=0; i<t.Get_Count(); i++)
			{
				s += CSG_String::Format("{%d}%s|", t[i].asInt(0), t[i].asString(1));
			}
		}

		SG_UI_ProgressAndMsg_Lock(false);

		pParameters->Get_Parameter("BANDS")->asChoice()->Set_Items(s);
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CRaster_Load_Band::On_Execute(void)
{
	CSG_String DB_Table(Parameters(has_GUI() ? "DB_TABLES" : "DB_TABLE")->asString()), Where;

	if( !has_GUI() || *Parameters("RID")->asString() )
	{
		Where.Printf("rid=%s", Parameters("RID")->asString());
	}
	else
	{
		Where.Printf("rid=%s", Parameters("BANDS")->asChoice()->Get_Item_Data(Parameters("BANDS")->asInt()).c_str());
	}

	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	if( !pGrid )
	{
		pGrid = SG_Create_Grid();
	}

	if( !Get_Connection()->Raster_Load(pGrid, DB_Table, Where) )
	{
		Error_Fmt("%s: %s (%s)", _TL("could not load raster"), DB_Table.c_str(), Where.c_str());

		if( pGrid != Parameters("GRID")->asGrid() )
		{
			delete(pGrid);
		}

		return( false );
	}

	if( pGrid != Parameters("GRID")->asGrid() )
	{
		Parameters("GRID")->Set_Value(pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRaster_Save::CRaster_Save(void)
{
	Set_Name		(_TL("Export Raster"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Exports grids to a PostGIS database."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"GRID_SYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_List("GRID_SYSTEM",
		"GRIDS"		, _TL("Bands"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice("",
		"TABLE"		, _TL("Add to Table"),
		_TL(""),
		""
	);

	Parameters.Add_String("TABLE",
		"NAME"		, _TL("Table Name"),
		_TL(""),
		""
	);

	Parameters.Add_Bool("TABLE",
		"GRID_NAME"	, _TL("Band Name Field"),
		_TL(""),
		true
	);

	Add_SRID_Picker();
}

//---------------------------------------------------------
void CRaster_Save::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, "raster_columns") )
	{
		for(sLong i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("r_table_name") + CSG_String("|");
		}
	}

	s	+= _TL("<not set>") + CSG_String("|");

	pParameters->Get_Parameter("TABLE")->asChoice()->Set_Items(s);
	pParameters->Get_Parameter("TABLE")->Set_Value((int)t.Get_Count());

	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("TABLE"));
	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("GRIDS"));
}

//---------------------------------------------------------
int CRaster_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("NAME") )
	{
		pParameter->Set_Value(CSG_PG_Connection::Make_Table_Name(pParameter->asString()));
	}

	if( pParameter->Cmp_Identifier("TABLE") )
	{
		bool	bCreate	= pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1;

		pParameters->Set_Enabled("NAME"     , bCreate);
		pParameters->Set_Enabled("GRID_NAME", bCreate);
	}

	if( pParameter->Cmp_Identifier("GRIDS") )
	{
		for(int i=0; i<pParameter->asGridList()->Get_Grid_Count(); i++)
		{
			CSG_Grid	*pGrid	= pParameter->asGridList()->Get_Grid(i);
			
			if( SG_Get_Data_Manager().Exists(pGrid) && pGrid->Get_Projection().is_Okay() && pGrid->Get_Projection().Get_EPSG() > 0 )
			{
				Set_SRID(pParameters, pGrid->Get_Projection());

				break;
			}
		}
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CRaster_Save::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS(2.0) )
	{
		Error_Set(_TL("PostGIS extension missing or too old"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	CSG_String	SavePoint, Table;

	Table	= Parameters("TABLE")->asInt() < Parameters("TABLE")->asChoice()->Get_Count() - 1
		? Parameters("TABLE")->asString()
		: Parameters("NAME" )->asString();

	if( Table.Length() == 0 )
	{
		Error_Set(_TL("no name has been specified for new raster table"));

		return( false );
	}

	//-----------------------------------------------------
	Get_Connection()->Begin(SavePoint = Get_Connection()->is_Transaction() ? "RASTER_SAVE" : "");

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Exists(Table) )
	{
		CSG_String	SQL	= "CREATE TABLE \"" + Table + "\" (\"rid\" serial PRIMARY KEY, \"raster\" raster"
			+ (Parameters("GRID_NAME")->asBool() ? ", \"name\" varchar(64))" : ")");
		
		if( !Get_Connection()->Execute(SQL) )
		{
			Get_Connection()->Rollback(SavePoint);

			return( false );
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		Process_Set_Text("%s: %s [%d/%d]", _TL("export grid"), pGrids->Get_Grid(i)->Get_Name(), i + 1, pGrids->Get_Grid_Count());

		if( !Get_Connection()->Raster_Save(pGrids->Get_Grid(i), Get_SRID(), Table, pGrids->Get_Grid(i)->Get_Name()) )
		{
			Get_Connection()->Rollback(SavePoint);

			return( false );
		}

		//{
		//	CSG_Bytes	WKB;

		//	if( CSG_Grid_OGIS_Converter::to_WKBinary(WKB, pGrids->Get_Grid(i), Get_SRID()) )
		//	{
		//		CSG_String	SQL	= "INSERT INTO \"" + Table + "\" (\"raster\", \"name\") VALUES("
		//			+ "ST_AddBand('" + WKB.toHexString() + "'::raster, '"
		//			+ CSG_PG_Connection::Get_Raster_Type_To_SQL(pGrids->Get_Grid(i)->Get_Type()) + "'::text, 0, NULL), '"
		//			+ pGrids->Get_Grid(i)->Get_Name() + "')";

		//		if( !Get_Connection()->Execute(SQL) )
		//		{
		//			Get_Connection()->Rollback(SavePoint);

		//			return( false );
		//		}
		//	}
		//}
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
CRaster_Collection_Save::CRaster_Collection_Save(void)
{
	Set_Name		(_TL("Export Grid Collection"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Exports a grid collection to a PostGIS database."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"GRID_SYSTEM", _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grids("GRID_SYSTEM",
		"GRIDS"      , _TL("Grid Collection"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_String("",
		"NAME"       , _TL("Name"),
		_TL(""),
		""
	);

	Parameters.Add_Choice("",
		"EXISTS"     , _TL("If table exists..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("abort"),
			_TL("replace")
		), 0
	);

	Add_SRID_Picker();
}

//---------------------------------------------------------
void CRaster_Collection_Save::On_Connection_Changed(CSG_Parameters *pParameters)
{
	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("GRIDS"));
}

//---------------------------------------------------------
int CRaster_Collection_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRIDS") )
	{
		CSG_Grids	*pGrids	= pParameter->asGrids();
			
		if( SG_Get_Data_Manager().Exists(pGrids) )
		{
			pParameters->Get_Parameter("NAME")->Set_Value(pGrids->Get_Name());

			if( pGrids->Get_Projection().is_Okay() && pGrids->Get_Projection().Get_EPSG() > 0 )
			{
				Set_SRID(pParameters, pGrids->Get_Projection());
			}
		}
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CRaster_Collection_Save::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS(2.0) )
	{
		Error_Set(_TL("PostGIS extension missing or too old"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	SavePoint, Table;

	Table	= Parameters("NAME")->asString();

	if( Table.is_Empty() )
	{
		Error_Set(_TL("no name has been specified for new raster table"));

		return( false );
	}

	if( Get_Connection()->Table_Exists(Table) && Parameters("EXISTS")->asInt() == 0 )
	{
		Error_Fmt("%s: %s", _TL("table already exists"), Table.c_str());

		return( false );
	}

	//-----------------------------------------------------
	Get_Connection()->Begin(SavePoint = Get_Connection()->is_Transaction() ? "RASTERS_SAVE" : "");

	if( Get_Connection()->Table_Exists(Table) && !Get_Connection()->Table_Drop(Table, false) )
	{
		Get_Connection()->Rollback(SavePoint);

		Error_Fmt("%s: %s", _TL("failed to replace existing table"), Table.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Rasters_Save(Parameters("GRIDS")->asGrids(), Get_SRID(), Table) )
	{
		Get_Connection()->Rollback(SavePoint);

		Error_Fmt("%s: %s", _TL("failed to save grid collection"), Table.c_str());

		return( false );
	}

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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		" Change the SRID of all rasters in the user-specified column and table."
	));

	Parameters.Add_Choice("",
		"TABLES"	, _TL("Tables"),
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

	if( Get_Connection()->Table_Load(t, "raster_columns") )
	{
		for(sLong i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("r_table_name") + CSG_String("|");
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

	Select.Printf("r_table_name='%s'", Parameters("TABLES")->asString());

	if( !Get_Connection()->Table_Load(Table, "raster_columns", "*", Select) || Table.Get_Count() != 1 )
	{
		return( false );
	}

	Select.Printf("SELECT UpdateRasterSRID('%s', '%s', %d)",
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
