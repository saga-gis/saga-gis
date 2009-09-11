
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     io_table_odbc                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   PGIS_Shapes.cpp                     //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "Get_Connection.h"

#include "PGIS_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		is_PostGIS		(bool bDialogOnError)
{
	if( !is_Connected(bDialogOnError) )
	{
		return( false );
	}

	if( !g_Connection.is_Postgres() )
	{
		if( bDialogOnError )
		{
			SG_UI_Dlg_Message(_TL("Not a PostgreSQL database!"), _TL("Database Connection Error"));
		}

		return( false );
	}

	if( !g_Connection.Table_Exists(SG_T("spatial_ref_sys")) || !g_Connection.Table_Exists(SG_T("geometry_columns")) )
	{
		if( bDialogOnError )
		{
			SG_UI_Dlg_Message(_TL("Not a valid PostGIS database!"), _TL("Database Connection Error"));
		}

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
CPGIS_Shapes_Load::CPGIS_Shapes_Load(void)
{
	Set_Name		(_TL("ODBC/PostGIS Shapes Import"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Imports shapes from a PostGIS database via ODBC."
	));

	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		CSG_String::Format(SG_T("%s|"),
			_TL("--- no table available ---")
		)
	);
}

//---------------------------------------------------------
bool CPGIS_Shapes_Load::On_Before_Execution(void)
{
	if( !is_PostGIS() )
	{
		return( false );
	}

	Parameters("TABLES")->asChoice()->Set_Items(g_Connection.Get_Tables());

	return( true );
}

//---------------------------------------------------------
bool CPGIS_Shapes_Load::On_Execute(void)
{
	CSG_Shapes	*pShapes	= Parameters("POINTS")	->asShapes();

	if( g_Connection.Table_Load(*pShapes, pShapes->Get_Name()) )
	{
		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPGIS_Shapes_Save::CPGIS_Shapes_Save(void)
{
	Set_Name		(_TL("ODBC/PostGIS Shapes Export"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Imports shapes from a PostGIS database via ODBC."
	));

	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Table Name"),
		_TL("if empty shapes layers's name is used as table name"),
		SG_T("")
	);

	Parameters.Add_Choice(
		NULL	, "SRID"		, _TL("Spatial Reference"),
		_TL(""),
		CSG_String::Format(SG_T("%s|"),
			_TL("--- not available ---")
		)
	);
}

//---------------------------------------------------------
bool CPGIS_Shapes_Save::On_Before_Execution(void)
{
	if( !is_PostGIS() )
	{
		Message_Add(_TL("no spatial reference systems defined"));

		return( false );
	}

	CSG_Table	SRIDs;

	if( !g_Connection.Table_Load(SRIDs, SG_T("spatial_ref_sys")) || SRIDs.Get_Count() == 0 )
	{
		return( false );
	}

	CSG_String	s, sID;

	for(int i=0; i<SRIDs.Get_Count(); i++)
	{
		sID	 = SRIDs[i].asString(3);

		s	+= sID.AfterFirst('\"').BeforeFirst('\"') + SG_T("|");
	}

	Parameters("SRID")->asChoice()->Set_Items(s);

	return( true );
}

//---------------------------------------------------------
bool CPGIS_Shapes_Save::On_Execute(void)
{
	int			SRID;
	CSG_Shapes	*pShapes;
	CSG_String	Name, sType, sql;

	pShapes	= Parameters("SHAPES")	->asShapes();
	Name	= Parameters("NAME")	->asString();	if( Name.Length() == 0 )	Name	= pShapes->Get_Name();
	SRID	= Parameters("SRID")	->asInt();

	//-----------------------------------------------------
	if( !g_Connection.Table_Create(Name, *pShapes) )
	{
		return( false );
	}

	// SELECT AddGeometryColumn(<table_name>, <column_name>, <srid>, <type>, <dimension>)
	sql.Printf(SG_T("SELECT AddGeometryColumn('%s', '%s', %d, '%s', 2)"),
		Name.c_str(),
		SG_T("GEOMETRY"),
		SRID,
		SG_T("GEOMETRY")
	);

	if( !g_Connection.Execute(sql) )
	{
		Message_Add(_TL("could not create geometry field"));

		return( false );
	}

	g_Connection.Commit();

	if( 0 )
	{
		CSG_String	s;

		if( g_Connection.Table_Exists(Name) )
		{
			s	 = SG_T("DROP TABLE ");
			s	+= Name;
			g_Connection.Execute(s);
		}

		if( 0 )
		{
			s	 = SG_T("CREATE TABLE ");
			s	+= Name;
			s	+= SG_T("(");
			s	+= SG_T(" feld01 VARCHAR,");
			s	+= SG_T(" feld02 INTEGER,");
			s	+= SG_T(" feld03 DOUBLE");
			s	+= SG_T(")");
		}
		else
		{
			s	+= SG_T("CREATE TABLE ");
			s	+= Name;
			s	+= SG_T("(");
			s	+= SG_T(" feld01 character varying(255),");
			s	+= SG_T(" feld02 integer,");
			s	+= SG_T(" feld03 numeric");
			s	+= SG_T(")");
		}

		return( g_Connection.Execute(s) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
