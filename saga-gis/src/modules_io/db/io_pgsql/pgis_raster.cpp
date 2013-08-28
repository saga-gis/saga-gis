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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
CPGIS_Raster_Load::CPGIS_Raster_Load(void)
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
void CPGIS_Raster_Load::On_Connection_Changed(CSG_Parameters *pParameters)
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
bool CPGIS_Raster_Load::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_String	Name, Field;
	CSG_Table	Table;

	Name	= Parameters("TABLES")->asString();

	if( !Get_Connection()->Table_Load(Table, "raster_columns", "*", CSG_String("r_table_name = '") + Name + "'") )
	{
		return( false );
	}

	if( Table.Get_Count() != 1 )
	{
		return( false );
	}

	Field	= Table[0].asString("r_raster_column");

	//-----------------------------------------------------
	CSG_Bytes_Array	Bands;

	if( !Get_Connection()->Raster_Load(Bands, Name, Field) )//, "rid=4") )
	{
		return( false );
	}

	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	pGrids->Del_Items();

	Process_Set_Text(_TL("data conversion"));

	for(int i=0; i<Bands.Get_Count(); i++)
	{
		if( Bands[i].Get_Count() > 0 )
		{
			CSG_Grid	*pGrid	= SG_Create_Grid();

			if( CSG_Grid_OGIS_Converter::from_WKBinary(Bands[i], pGrid) )
			{
				pGrid->Set_Name(Bands.Get_Count() == 2 ? Name : CSG_String::Format(SG_T("%s [%02d]"), Name.c_str(), i + 1));

				pGrids->Add_Item(pGrid);
			}
			else
			{
				delete(pGrid);
			}
		}
	}

	return( pGrids->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPGIS_Raster_Save::CPGIS_Raster_Save(void)
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

	Parameters.Add_Choice(
		NULL	, "EXISTS"		, _TL("If table exists..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("abort export"),
			_TL("replace existing table"),
			_TL("append records, if table structure allows")
		), 0
	);
}

//---------------------------------------------------------
bool CPGIS_Raster_Save::On_Execute(void)
{
	if( !Get_Connection()->Table_Exists(SG_T("spatial_ref_sys")) || !Get_Connection()->Table_Exists(SG_T("geometry_columns")) )
	{
		SG_UI_Dlg_Message(_TL("Not a valid PostGIS database!"), _TL("Database Connection Error"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	CSG_String	Name	= Parameters("NAME")->asString();

	if( Name.Length() == 0 )
		return( false );

	//-----------------------------------------------------
	Get_Connection()->Begin();

	if( !Get_Connection()->Table_Exists(Name)
	&&  !Get_Connection()->Execute("CREATE TABLE \"" + Name + "\" (\"rid\" serial PRIMARY KEY, \"rast\" raster)") )
	{
		Get_Connection()->Rollback();

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		CSG_Bytes	WKB;

		if( CSG_Grid_OGIS_Converter::to_WKBinary(WKB, pGrids->asGrid(i), pGrids->asGrid(i)->Get_Projection().Get_EPSG()) )
		{
			CSG_String	SQL;

		//	SQL	= "INSERT INTO \"" + Name + "\" (\"rast\") VALUES('" + WKB.toHexString() + "'::raster)";

			SQL	= "INSERT INTO \"" + Name + "\" (\"rast\") VALUES(ST_AddBand('" + WKB.toHexString() + "'::raster, '"
				+ CSG_PG_Connection::Get_Type_To_SQL(pGrids->asGrid(i)->Get_Type()) + "'::text, 0, NULL))";

			if( !Get_Connection()->Execute(SQL) )
			{
				Get_Connection()->Rollback();

				return( false );
			}
		}
	}

	//-----------------------------------------------------
	Get_Connection()->Commit();

	Get_Connection()->GUI_Update();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
	