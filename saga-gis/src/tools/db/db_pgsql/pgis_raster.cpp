/**********************************************************
 * Version $Id: pgis_raster.cpp 1646 2013-04-10 16:29:00Z oconrad $
 *********************************************************/

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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Imports grids from a PostGIS database."
	));

	Parameters.Add_Grid_List(NULL,
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL,
		"TABLES"	, _TL("Tables"),
		_TL(""),
		""
	);

	Parameters.Add_String(NULL,
		"WHERE"		, _TL("Where"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
void CRaster_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, "raster_columns") )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("r_table_name") + CSG_String("|");
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

	if( !Get_Connection()->Raster_Load(pGrids, Table, Parameters("WHERE")->asString()) )
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
CRaster_Load_Band::CRaster_Load_Band(void)
{
	Set_Name		(_TL("Import Single Raster Band from PostGIS"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Imports grids from a PostGIS database."
	));

	Parameters.Add_Grid_Output(NULL,
		"GRID"		, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Choice(NULL,
		"TABLES"	, _TL("Tables"),
		_TL(""),
		""
	);

	Parameters.Add_Choice(NULL,
		"BANDS"		, _TL("Bands"),
		_TL(""),
		""
	)->Set_UseInCMD(false);

	Parameters.Add_String(NULL,
		"RID"		, _TL("Raster Band Identifier"),
		_TL(""),
		""
	)->Set_UseInGUI(false);
}

//---------------------------------------------------------
void CRaster_Load_Band::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	SG_UI_Progress_Lock(true); SG_UI_Msg_Lock(true);

	if( Get_Connection()->Table_Load(t, "raster_columns") )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("r_table_name") + CSG_String("|");
		}
	}

	SG_UI_Progress_Lock(false); SG_UI_Msg_Lock(false);

	pParameters->Get_Parameter("TABLES")->asChoice()->Set_Items(s);

	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("TABLES"));
}

//---------------------------------------------------------
int CRaster_Load_Band::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLES") )
	{
		CSG_String	s;
		CSG_Table	t;

		SG_UI_Progress_Lock(true); SG_UI_Msg_Lock(true);

		if( Get_Connection()->Table_Load(t, pParameter->asString(), "rid, name") )
		{
			for(int i=0; i<t.Get_Count(); i++)
			{
				s	+= CSG_String::Format("{%d}%s|", t[i].asInt(0), t[i].asString(1));
			}
		}

		SG_UI_Progress_Lock(false); SG_UI_Msg_Lock(false);

		pParameters->Get_Parameter("BANDS")->asChoice()->Set_Items(s);
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CRaster_Load_Band::On_Execute(void)
{
	CSG_String	Table	= Parameters("TABLES")->asString(), Where;

	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();
	
	if( !pGrid )
	{
		pGrid	= SG_Create_Grid();
	}

	if( !SG_UI_Get_Window_Main() || *Parameters("RID")->asString() )
	{
		Where.Printf("rid=%s", Parameters("RID")->asString());
	}
	else
	{
		Where.Printf("rid=%s", Parameters("BANDS")->asChoice()->Get_Item_Data(Parameters("BANDS")->asInt()).c_str());
	}

	if( !Get_Connection()->Raster_Load(pGrid, Table, Where) )
	{
		Error_Fmt("%s: %s (%s)", _TL("could not load raster"), Table.c_str(), Where.c_str());

		if( pGrid != Parameters("GRID")->asGrid() )
		{
			delete(pGrid);
		}

		return( false );
	}

	Parameters("GRID")->Set_Value(pGrid);

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
	//-----------------------------------------------------
	Set_Name		(_TL("Export Raster to PostGIS"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Exports grids to a PostGIS database."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System(NULL,
		"GRID_SYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_List(Parameters("GRID_SYSTEM"),
		"GRIDS"		, _TL("Bands"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(NULL,
		"TABLE"		, _TL("Add to Table"),
		_TL(""),
		""
	);

	Parameters.Add_String(Parameters("TABLE"),
		"NAME"		, _TL("Table Name"),
		_TL(""),
		""
	);

	Parameters.Add_Bool(Parameters("TABLE"),
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
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("r_table_name") + CSG_String("|");
		}
	}

	s	+= _TL("<not set>") + CSG_String("|");

	pParameters->Get_Parameter("TABLE")->asChoice()->Set_Items(s);
	pParameters->Get_Parameter("TABLE")->Set_Value(t.Get_Count());

	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("TABLE"));
	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("GRIDS"));
}

//---------------------------------------------------------
int CRaster_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLE") )
	{
		bool	bCreate	= pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1;

		pParameters->Set_Enabled("NAME"     , bCreate);
		pParameters->Set_Enabled("GRID_NAME", bCreate);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "GRIDS") )
	{
		for(int i=0; i<pParameter->asList()->Get_Count(); i++)
		{
			CSG_Grid	*pGrid	= pParameter->asGridList()->asGrid(i);
			
			if( SG_Get_Data_Manager().Exists(pGrid) && pGrid->Get_Projection().is_Okay() && pGrid->Get_Projection().Get_EPSG() > 0 )
			{
				Set_SRID(pParameters, pGrid->Get_Projection().Get_EPSG());

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
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		Process_Set_Text(CSG_String::Format("%s: %s [%d/%d]", _TL("export grid"), pGrids->asGrid(i)->Get_Name(), i + 1, pGrids->Get_Count()));

		if( !Get_Connection()->Raster_Save(pGrids->asGrid(i), Get_SRID(), Table, pGrids->asGrid(i)->Get_Name()) )
		{
			Get_Connection()->Rollback(SavePoint);

			return( false );
		}

		//{
		//	CSG_Bytes	WKB;

		//	if( CSG_Grid_OGIS_Converter::to_WKBinary(WKB, pGrids->asGrid(i), Get_SRID()) )
		//	{
		//		CSG_String	SQL	= "INSERT INTO \"" + Table + "\" (\"raster\", \"name\") VALUES("
		//			+ "ST_AddBand('" + WKB.toHexString() + "'::raster, '"
		//			+ CSG_PG_Connection::Get_Raster_Type_To_SQL(pGrids->asGrid(i)->Get_Type()) + "'::text, 0, NULL), '"
		//			+ pGrids->asGrid(i)->Get_Name() + "')";

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
CRaster_SRID_Update::CRaster_SRID_Update(void)
{
	Set_Name		(_TL("Update Raster SRID"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		" Change the SRID of all rasters in the user-specified column and table."
	));

	Parameters.Add_Choice(NULL,
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
		for(int i=0; i<t.Get_Count(); i++)
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
	