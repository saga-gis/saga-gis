
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
#include "PGIS_Shapes.h"


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
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	if( !Get_Connection()->is_Postgres() )
	{
		SG_UI_Dlg_Message(_TL("Not a PostgreSQL database!"), _TL("Database Connection Error"));

		return( false );
	}

	if( !Get_Connection()->Table_Exists(SG_T("spatial_ref_sys")) || !Get_Connection()->Table_Exists(SG_T("geometry_columns")) )
	{
		SG_UI_Dlg_Message(_TL("Not a valid PostGIS database!"), _TL("Database Connection Error"));

		return( false );
	}

	CSG_Table	Geo_Tables;

	if( !Get_Connection()->Table_Load(Geo_Tables, SG_T("geometry_columns")) )
	{
		return( false );
	}

	CSG_String	s;

	for(int i=0; i<Geo_Tables.Get_Count(); i++)
	{
		s	+= Geo_Tables[i].asString(SG_T("f_table_name")) + CSG_String("|");
	}

	Parameters("TABLES")->asChoice()->Set_Items(s);

	return( true );
}

//---------------------------------------------------------
bool CPGIS_Shapes_Load::On_Execute(void)
{
	CSG_String	SQL, Geo_Table, Geo_Type, Geo_Field;
	CSG_Table	Geo_Tables;
	CSG_Shapes	*pShapes;

	pShapes		= Parameters("SHAPES")	->asShapes();
	Geo_Table	= Parameters("TABLES")	->asString();

	SQL.Printf(SG_T("SELECT * FROM geometry_columns WHERE f_table_name = '%s'"), Geo_Table.c_str());

	if( !Get_Connection()->Table_From_Query(SG_T("*"), SG_T("geometry_columns"), CSG_String::Format(SG_T("f_table_name = '%s'"), Geo_Table.c_str()), SG_T(""), Geo_Tables) )
	{
		return( false );
	}

	if( Geo_Tables.Get_Count() != 1 )
	{
		return( false );
	}

	Geo_Type	= Geo_Tables[0].asString(SG_T("type"));
	Geo_Field	= Geo_Tables[0].asString(SG_T("f_geometry_column"));

	//-----------------------------------------------------
	     if( !Geo_Type.Cmp(SG_T("POINT")) )				pShapes->Create(SHAPE_TYPE_Point	, Geo_Table);
	else if( !Geo_Type.Cmp(SG_T("MULTIPOINT")) )		pShapes->Create(SHAPE_TYPE_Points	, Geo_Table);
	else if( !Geo_Type.Cmp(SG_T("LINESTRING")) )		pShapes->Create(SHAPE_TYPE_Line		, Geo_Table);
	else if( !Geo_Type.Cmp(SG_T("MULTILINESTRING")) )	pShapes->Create(SHAPE_TYPE_Line		, Geo_Table);
	else if( !Geo_Type.Cmp(SG_T("POLYGON")) )			pShapes->Create(SHAPE_TYPE_Polygon	, Geo_Table);
	else if( !Geo_Type.Cmp(SG_T("MULTIPOLYGON")) )		pShapes->Create(SHAPE_TYPE_Polygon	, Geo_Table);
	else
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Load(*pShapes, Geo_Table) )
	{
		return( false );
	}

//	SQL.Printf(SG_T("SELECT AsText(%s) AS geom FROM %s"), Geo_Field.c_str(), Geo_Table.c_str());
	SQL.Printf(SG_T("AsText(%s) AS geom"), Geo_Field.c_str());

	if( !Get_Connection()->Table_From_Query(SQL, Geo_Table, SG_T(""), SG_T(""), Geo_Tables) )
	{
		return( false );
	}

	if( Geo_Tables.Get_Count() != pShapes->Get_Count() )
	{
		return( false );
	}

	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_OGIS_Shapes_Converter::from_WKText(Geo_Tables[iShape].asString(0), pShapes->Get_Shape(iShape));
	}

	return( true );
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
		PARAMETER_INPUT
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
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	if( !Get_Connection()->is_Postgres() )
	{
		SG_UI_Dlg_Message(_TL("Not a PostgreSQL database!"), _TL("Database Connection Error"));

		return( false );
	}

	if( !Get_Connection()->Table_Exists(SG_T("spatial_ref_sys")) || !Get_Connection()->Table_Exists(SG_T("geometry_columns")) )
	{
		SG_UI_Dlg_Message(_TL("Not a valid PostGIS database!"), _TL("Database Connection Error"));

		return( false );
	}

	Parameters("SRID")->asChoice()->Set_Items(SG_Get_Projections().Get_Names());

/*	if( Parameters("SRID")->asChoice()->Get_Count() > 1 )
		return( true );

	CSG_Table	SRIDs;

	if( !Get_Connection()->Table_Load(SRIDs, SG_T("spatial_ref_sys")) || SRIDs.Get_Count() == 0 )
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
/**/
	return( true );
}

//---------------------------------------------------------
bool CPGIS_Shapes_Save::On_Execute(void)
{
	int			SRID;
	CSG_Shapes	*pShapes;
	CSG_String	SQL, Geo_Table, Geo_Type, Geo_Field;

	pShapes		= Parameters("SHAPES")	->asShapes();
	Geo_Table	= Parameters("NAME")	->asString();	if( Geo_Table.Length() == 0 )	Geo_Table	= pShapes->Get_Name();
	SRID		= Parameters("SRID")	->asInt();
	SRID		= SG_Get_Projections().Get_SRID_byNamesIndex(SRID);

	switch( pShapes->Get_Type() )
	{
	default:
		Geo_Type	= SG_T("GEOMETRY");			// GEOMETRYCOLLECTION
		Geo_Field	= SG_T("geometry");
		break;

	case SHAPE_TYPE_Point:
		Geo_Type	= SG_T("POINT");
		Geo_Field	= SG_T("geo_point");
		break;

	case SHAPE_TYPE_Points:
		Geo_Type	= SG_T("MULTIPOINT");
		Geo_Field	= SG_T("geo_points");
		break;

	case SHAPE_TYPE_Line:
		Geo_Type	= SG_T("MULTILINESTRING");	// LINESTRING
		Geo_Field	= SG_T("geo_line");
		break;

	case SHAPE_TYPE_Polygon:
		Geo_Type	= SG_T("MULTIPOLYGON");		// POLYGON
		Geo_Field	= SG_T("geo_polygon");
		break;
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Create(Geo_Table, *pShapes, false) )
	{
		Get_Connection()->Rollback();

		return( false );
	}

	//-----------------------------------------------------
	SQL.Printf(SG_T("SELECT AddGeometryColumn('%s', '%s', %d, '%s', %d)"),
		Geo_Table.c_str(),	// <table_name>
		Geo_Field.c_str(),	// <column_name>
		SRID,				// <srid>
		Geo_Type.c_str(),	// <type>
		2					// <dimension>
	);

	if( !Get_Connection()->Execute(SQL) )
	{
		Get_Connection()->Rollback();

		Message_Add(_TL("could not create geometry field"));

		return( false );
	}

	//-----------------------------------------------------
	int			iShape, iField, nAdded;
	CSG_String	Insert, Fields, s;

	Fields	= Geo_Field;

	for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
	{
		Fields	+= CSG_String(", ") + pShapes->Get_Field_Name(iField);
	}

	Insert.Printf(SG_T("INSERT INTO %s (%s) VALUES (%%s)"), Geo_Table.c_str(), Fields.c_str());

	for(iShape=0, nAdded=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( pShape->is_Valid() )
		{
			CSG_OGIS_Shapes_Converter::to_WKText(pShape, s);

			Fields.Printf(SG_T("GeomFromText('%s', %d)"), s.c_str(), SRID);

			for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
			{
				s	= pShape->asString(iField);

				if( pShapes->Get_Field_Type(iField) == SG_DATATYPE_String )
				{
					s.Replace(SG_T("'"), SG_T("\""));
					s	= SG_T("'") + s + SG_T("'");
				}

				Fields	+= SG_T(", ")  + s;
			}

			SQL.Printf(Insert.c_str(), Fields.c_str());

			if( Get_Connection()->Execute(SQL) )
			{
				nAdded++;
			}
		}
	}

	//-----------------------------------------------------
	if( nAdded == 0 )
	{
		Get_Connection()->Rollback();

		Get_Connection()->Table_Drop(Geo_Table);

		return( false );
	}

	return( Get_Connection()->Commit() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
