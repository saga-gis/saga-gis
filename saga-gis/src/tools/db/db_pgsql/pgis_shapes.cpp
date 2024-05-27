
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
//                   PGIS_Shapes.cpp                     //
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
#include "pgis_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Load::CShapes_Load(void)
{
	Set_Name		(_TL("Import Shapes"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Imports shapes from a PostGIS database."
	));

	Parameters.Add_Shapes     ("", "SHAPES"    , _TL("Shapes"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Shapes_List("", "COLLECTION", _TL("Shapes"), _TL(""), PARAMETER_OUTPUT);

	if( has_GUI() )
	{
		Parameters.Add_Choice("", "DB_TABLE", _TL("Table"), _TL(""), ""); // GUI => select from available tables
	}
	else
	{
		Parameters.Add_String("", "DB_TABLE", _TL("Table"), _TL(""), ""); // CMD => table's db name
	}
}

//---------------------------------------------------------
void CShapes_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	if( has_GUI() )
	{
		CSG_String s; CSG_Table t;

		SG_UI_ProgressAndMsg_Lock(true);

		if( Get_Connection()->Table_Load(t, "geometry_columns") )
		{
			for(sLong i=0; i<t.Get_Count(); i++)
			{
				s += t[i].asString("f_table_name") + CSG_String("|");
			}
		}

		SG_UI_ProgressAndMsg_Lock(false);

		CSG_Parameter *pParameter = pParameters->Get_Parameter("DB_TABLE");
		pParameter->asChoice()->Set_Items(s);
		pParameter->Set_Value(pParameter->asString());
	}
}

//---------------------------------------------------------
bool CShapes_Load::On_Execute(void)
{
	CSG_String DB_Table(Parameters("DB_TABLE")->asString());

	CSG_Table Geometries; TSG_Shape_Type Geometry = SHAPE_TYPE_Undefined;

	if( Get_Connection()->Table_Load(Geometries, "geometry_columns") )
	{
		CSG_Table_Record *pInfo = Geometries.Find_Record(Geometries.Find_Field("f_table_name"), DB_Table);

		if( pInfo )
		{
			Geometry = CSG_Shapes_OGIS_Converter::to_ShapeType(pInfo->asString("type"));
		}

		if( Geometry == SHAPE_TYPE_Undefined ) // Geometry Collection ?!
		{
			CSG_Shapes *pShapes[4];

			if( !Get_Connection()->Shapes_Load(pShapes, DB_Table) )
			{
				Error_Set(_TL("unable to load vector data from PostGIS database") + CSG_String(":\n") + DB_Table);

				return( false );
			}

			CSG_Parameter_Shapes_List *pCollection = Parameters("COLLECTION")->asShapesList(); pCollection->Del_Items();

			for(int i=0; i<4; i++)
			{
				pCollection->Add_Item(pShapes[i]);
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes(); pShapes->Destroy();

	if( !Get_Connection()->Shapes_Load(pShapes, DB_Table) )
	{
		Error_Set(_TL("unable to load vector data from PostGIS database") + CSG_String(":\n") + DB_Table);

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
CShapes_Save::CShapes_Save(void)
{
	Set_Name		(_TL("Export Shapes"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Exports shapes to a PostGIS database."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Set_Constraints(&Parameters, "SHAPES");

	Parameters.Add_String("",
		"NAME"		, _TL("Table Name"),
		_TL("if empty shapes layers's name is used as table name"),
		""
	);

	Parameters.Add_Choice("",
		"EXISTS"	, _TL("If table exists..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("abort export"),
			_TL("replace existing table"),
			_TL("append records, if table structure allows")
		), 0
	);

	Add_SRID_Picker();
}

//---------------------------------------------------------
int CShapes_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("NAME") )
	{
		pParameter->Set_Value(CSG_PG_Connection::Make_Table_Name(pParameter->asString()));
	}

	if( pParameter->Cmp_Identifier("SHAPES") )
	{
		if( pParameter->asShapes() )
		{
			pParameters->Set_Parameter("NAME", CSG_PG_Connection::Make_Table_Name(pParameter->asShapes()->Get_Name()));

			Set_SRID(pParameters, pParameter->asShapes()->Get_Projection());
		}
		else
		{
			pParameters->Set_Parameter("NAME", "");

			Set_SRID(pParameters, -1);
		}
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CShapes_Save::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS() )
	{
		Error_Set(_TL("not a valid PostGIS database!"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	CSG_String	Name	= Parameters("NAME")->asString(); if( Name.Length() == 0 ) { Name = pShapes->Get_Name(); }

	//-----------------------------------------------------
	CSG_String	geoType;

	if( !CSG_Shapes_OGIS_Converter::from_ShapeType(geoType, pShapes->Get_Type(), pShapes->Get_Vertex_Type()) )
	{
		Error_Set(_TL("invalid or unsupported shape or vertex type"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	SavePoint	= Get_Connection()->is_Transaction() ? "SHAPES_SAVE" : "";

	Get_Connection()->Begin(SavePoint);

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Name) )
	{
		Message_Fmt("\n%s: %s", _TL("table already exists"), Name.c_str());

		switch( Parameters("EXISTS")->asInt() )
		{
		default:	// abort export
			return( false );

		case  2:	// append records, if table structure allows it
			break;

		case  1:	// replace existing table
			Message_Fmt("\n%s: %s...", _TL("trying to drop table"), Name.c_str());

			if( !Get_Connection()->Table_Drop(Name, false) )
			{
				Message_Fmt(_TL("failed"));

				return( false );
			}

			Message_Fmt(_TL("okay"));
			break;
		}
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Exists(Name) )
	{
		if( !Get_Connection()->Table_Create(Name, *pShapes, Get_Constraints(&Parameters, "SHAPES"), false) )
		{
			Error_Set(_TL("could not create table"));

			Get_Connection()->Rollback(SavePoint);

			return( false );
		}

		//-------------------------------------------------
		CSG_String	SQL;	// SELECT AddGeometryColumn(<table_name>, <column_name>, <srid>, <type>, <dimension>)

		SQL.Printf("SELECT AddGeometryColumn('%s', '%s', %d, '%s', %d)",
			Name.c_str(), SG_T("geometry"), Get_SRID(), geoType.c_str(),
			pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XY  ? 2 :
			pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZ ? 3 : 4
		);

		if( !Get_Connection()->Execute(SQL) )
		{
			Error_Set(_TL("could not create geometry field"));

			Get_Connection()->Rollback(SavePoint);

			return( false );
		}
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Shapes_Insert(pShapes, Name) )
	{
		Get_Connection()->Rollback(SavePoint);

		return( false );
	}

	Get_Connection()->Commit(SavePoint);

	Get_Connection()->GUI_Update();

	Get_Connection()->Add_MetaData(*pShapes, Name);

	pShapes->Set_Modified(false);

	return( true );

//---------------------------------------------------------
//	bool	bBinary	= Get_Connection()->has_Version(9);
//
//	CSG_String	Insert	= "INSERT INTO \"" + Name + "\" (\"geometry";
//
//	for(int iField=0; iField<pShapes->Get_Field_Count(); iField++)
//	{
//		Insert	+= CSG_String("\", \"") + CSG_PG_Connection::Make_Table_Field_Name(pShapes, iField);
//	}
//
//	Insert	+= "\") VALUES (";
//
//	//-----------------------------------------------------
//	bool	bResult	= true;
//
//	for(sLong iShape=0; iShape<pShapes->Get_Count() && bResult && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
//	{
//		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);
//
//		if( !pShape->is_Valid() )
//		{
//			bResult	= false;
//
//			continue;
//		}
//
//		//-------------------------------------------------
//		CSG_String	SQL	= Insert;
//
//		if( bBinary )
//		{
//			CSG_Bytes	WKB;	CSG_Shapes_OGIS_Converter::to_WKBinary(pShape, WKB);
//
//			SQL	+= "ST_GeomFromWKB(E'\\\\x" + WKB.toHexString() + CSG_String::Format("', %d)", Get_SRID());
//		}
//		else
//		{
//			CSG_String	WKT;	CSG_Shapes_OGIS_Converter::to_WKText  (pShape, WKT);
//
//			SQL	+= "ST_GeomFromText('" + WKT + CSG_String::Format("', %d)", Get_SRID());
//		}
//
//		for(int iField=0; iField<pShapes->Get_Field_Count(); iField++)
//		{
//			CSG_String	s = pShape->asString(iField);
//
//			switch( pShapes->Get_Field_Type(iField) )
//			{
//			case SG_DATATYPE_String:
//			case SG_DATATYPE_Date  :
//				s.Replace("'", "\"");	s	= "'" + s + "'";
//				break;
//			}
//
//			SQL	+= ", "  + s;
//		}
//
//		SQL	+= ")";
//
//		if( !Get_Connection()->Execute(SQL) )
//		{
//			bResult	= false;
//		}
//	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_SRID_Update::CShapes_SRID_Update(void)
{
	Set_Name		(_TL("Update Shapes SRID"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		" Change the SRID of all geometries in the user-specified column and table."
	));

	if( has_GUI() )
	{
		Parameters.Add_Choice("", "DB_TABLE", _TL("Table"), _TL(""), ""); // GUI => select from available tables
	}
	else
	{
		Parameters.Add_String("", "DB_TABLE", _TL("Table"), _TL(""), ""); // CMD => table's db name
	}

	Add_SRID_Picker();
}

//---------------------------------------------------------
void CShapes_SRID_Update::On_Connection_Changed(CSG_Parameters *pParameters)
{
	if( has_GUI() )
	{
		CSG_String s; CSG_Table t;

		SG_UI_ProgressAndMsg_Lock(true);

		if( Get_Connection()->Table_Load(t, "geometry_columns") )
		{
			for(sLong i=0; i<t.Get_Count(); i++)
			{
				s += t[i].asString("f_table_name") + CSG_String("|");
			}
		}

		SG_UI_ProgressAndMsg_Lock(false);

		CSG_Parameter *pParameter = pParameters->Get_Parameter("DB_TABLE");
		pParameter->asChoice()->Set_Items(s);
		pParameter->Set_Value(pParameter->asString());
	}
}

//---------------------------------------------------------
bool CShapes_SRID_Update::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS() )	{	Error_Set(_TL("no PostGIS layer"));	return( false );	}

	//-----------------------------------------------------
	CSG_Table Table; CSG_String Select, DB_Table(Parameters("DB_TABLE")->asString());

	Select.Printf("f_table_name='%s'", DB_Table.c_str());

	if( !Get_Connection()->Table_Load(Table, "geometry_columns", "*", Select) || Table.Get_Count() != 1 )
	{
		return( false );
	}

	Select.Printf("SELECT UpdateGeometrySRID('%s', '%s', %d)",
		DB_Table.c_str(), Table[0].asString("f_geometry_column"), Get_SRID()
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
CShapes_Join::CShapes_Join(void)
{
	Set_Name		(_TL("Import Shapes with Joined Data"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"Imports shapes with joined data from a PostGIS database."
	));

	Parameters.Add_Shapes(""          , "SHAPES"    , _TL("Shapes"        ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice(""          , "GEO_TABLE" , _TL("Geometry Table"), _TL(""), "");
	Parameters.Add_Choice("GEO_TABLE" , "GEO_KEY"   , _TL("Key"           ), _TL(""), "");

	Parameters.Add_Choice(""          , "JOIN_TABLE", _TL("Join Table"    ), _TL(""), "");
	Parameters.Add_Choice("JOIN_TABLE", "JOIN_KEY"  , _TL("Key"           ), _TL(""), "");

	Parameters.Add_String(""          , "FIELDS"    , _TL("Fields"        ), _TL(""), "");
	Parameters.Add_String(""          , "WHERE"     , _TL("Where"         ), _TL(""), "");
	Parameters.Add_String(""          , "GROUP"     , _TL("Group by"      ), _TL(""), "");
	Parameters.Add_String(""          , "HAVING"    , _TL("Having"        ), _TL(""), "");
	Parameters.Add_String(""          , "ORDER"     , _TL("Order by"      ), _TL(""), "");
	Parameters.Add_Bool  (""          , "DISTINCT"  , _TL("Distinct"      ), _TL("")    );
}

//---------------------------------------------------------
void CShapes_Join::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String s; CSG_Table t;

	if( Get_Connection()->Table_Load(t, "geometry_columns") )
	{
		for(sLong i=0; i<t.Get_Count(); i++)
		{
			s += t[i].asString("f_table_name") + CSG_String("|");
		}
	}

	#define SET_ITEMS(id, items) { CSG_Parameter *p = (*pParameters)(id); p->asChoice()->Set_Items(items); p->Set_Value(0);\
		On_Parameter_Changed(pParameters, p);\
	}

	SET_ITEMS("GEO_TABLE", s); SET_ITEMS("JOIN_TABLE", Get_Connection()->Get_Tables());
}

//---------------------------------------------------------
int CShapes_Join::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier( "GEO_TABLE")
	||  pParameter->Cmp_Identifier("JOIN_TABLE") )
	{
		CSG_String Names, Geometry;

		CSG_Table Description = Get_Connection()->Get_Field_Desc(pParameter->asString());

		Get_Connection()->Shapes_Geometry_Info(pParameter->asString(), &Geometry, NULL);

		for(sLong i=0; i<Description.Get_Count(); i++)
		{
			if( Geometry.Cmp(Description[i].asString(0)) )
			{
				Names += Description[i].asString(0) + CSG_String("|");
			}
		}

		pParameter->Get_Child(0)->asChoice()->Set_Items(Names);
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CShapes_Join::On_Execute(void)
{
	CSG_String  geoTable = Parameters( "GEO_TABLE")->asString();
	CSG_String joinTable = Parameters("JOIN_TABLE")->asString();

	if( !geoTable.Cmp(joinTable) )
	{
		Error_Set(_TL("Geometry and join table must not be identical."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String Join; Join.Printf("\"%s\".\"%s\"=\"%s\".\"%s\"",
		 geoTable.c_str(), Parameters( "GEO_KEY")->asString(),
		joinTable.c_str(), Parameters("JOIN_KEY")->asString()
	);

	bool     Distinct = Parameters("DISTINCT")->asBool  ();
	CSG_String Fields = Parameters("FIELDS"  )->asString();
	CSG_String  Where = Parameters("WHERE"   )->asString();
	CSG_String  Group = Parameters("GROUP"   )->asString();
	CSG_String Having = Parameters("HAVING"  )->asString();
	CSG_String  Order = Parameters("ORDER"   )->asString();

	if( !Where.is_Empty() )
	{
		Where = " AND (" + Where + ")";
	}

	//-----------------------------------------------------
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes(); pShapes->Destroy();

	if( !Get_Connection()->Shapes_Load(pShapes, geoTable + "." + joinTable, geoTable, "\"" + joinTable + "\"", Fields, Join + Where, Group, Having, Order, Distinct, true) )
	{
		Error_Set(_TL("unable to load vector data from PostGIS database") + CSG_String(":\n") + geoTable + "." + joinTable);

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
CShapes_Join_GUI::CShapes_Join_GUI(void)
{
	Set_Name		(_TL("Import Shapes with Joined Data (GUI)"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Imports shapes with joined data from a PostGIS database."
	));

	Parameters.Add_Shapes    (""          , "SHAPES"    , _TL("Shapes"        ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice    (""          ,  "GEO_TABLE", _TL("Geometry Table"), _TL(""), "");
	Parameters.Add_Choice    ("GEO_TABLE" ,  "GEO_KEY"  , _TL("Key"           ), _TL(""), "");

	Parameters.Add_Choice    (""          , "JOIN_TABLE", _TL("Join Table"    ), _TL(""), "");
	Parameters.Add_Choice    ("JOIN_TABLE", "JOIN_KEY"  , _TL("Key"           ), _TL(""), "");

	Parameters.Add_Parameters(""          , "FIELDS"    , _TL("Fields"        ), _TL(""));
	Parameters.Add_String    (""          , "WHERE"     , _TL("Where"         ), _TL(""), "");
}

//---------------------------------------------------------
void CShapes_Join_GUI::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String s; CSG_Table t;

	if( Get_Connection()->Table_Load(t, "geometry_columns") )
	{
		for(sLong i=0; i<t.Get_Count(); i++)
		{
			s += t[i].asString("f_table_name") + CSG_String("|");
		}
	}

	#define SET_ITEMS(id, items) { CSG_Parameter *p = (*pParameters)(id); p->asChoice()->Set_Items(items); p->Set_Value(0);\
		On_Parameter_Changed(pParameters, p);\
	}

	SET_ITEMS("GEO_TABLE", s); SET_ITEMS("JOIN_TABLE", Get_Connection()->Get_Tables());
}

//---------------------------------------------------------
int CShapes_Join_GUI::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameters->Cmp_Identifier("FIELDS") == false )
	{
		if( pParameter->Cmp_Identifier( "GEO_TABLE")
		||  pParameter->Cmp_Identifier("JOIN_TABLE") )
		{
			Update_Fields(pParameters, pParameter);
		}
	}

	if( pParameters->Cmp_Identifier("FIELDS") == true && !pParameter->Get_Parent() )
	{
		for(int i=0; i<pParameter->Get_Children_Count(); i++)
		{
			pParameter->Get_Child(i)->Set_Enabled(pParameter->asBool() == false);
		}
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
void CShapes_Join_GUI::Update_Fields(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Parameters &Field_Check = *(*pParameters)("FIELDS")->asParameters();

	CSG_Parameter *pField_All = Field_Check(pParameter->Get_Identifier());

	if( pField_All == NULL )
	{
		pField_All = Field_Check.Add_Bool("", pParameter->Get_Identifier(), pParameter->asString(), "", false);
	}
	else
	{
		pField_All->Set_Name(pParameter->asString()); pField_All->Set_Value(false); // select all

		while( pField_All->Get_Children_Count() )
		{
			Field_Check.Del_Parameter(pField_All->Get_Child(0)->Get_Identifier());
		}
	}

	CSG_String Field_Names, Geometry;

	Get_Connection()->Shapes_Geometry_Info(pParameter->asString(), &Geometry, NULL);

	CSG_Table Description = Get_Connection()->Get_Field_Desc(pParameter->asString());

	for(sLong i=0; i<Description.Get_Count(); i++)
	{
		if( Geometry.Cmp(Description[i].asString(0)) )
		{
			Field_Names += Description[i].asString(0) + CSG_String("|");
			Field_Check.Add_Bool(pParameter->Get_Identifier(), CSG_String::Format("%s.%s",
				pParameter->Get_Name(), Description[i].asString(0)), Description[i].asString(0), ""
			);
		}
	}

	pParameter->Get_Child(0)->asChoice()->Set_Items(Field_Names);
}

//---------------------------------------------------------
bool CShapes_Join_GUI::On_Execute(void)
{
	CSG_String  geoTable = Parameters( "GEO_TABLE")->asString();
	CSG_String joinTable = Parameters("JOIN_TABLE")->asString();

	if( !geoTable.Cmp(joinTable) )
	{
		Error_Set(_TL("Geometry and join table must not be identical."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String Join, Where = Parameters("WHERE")->asString();

	Join.Printf("\"%s\".\"%s\"=\"%s\".\"%s\"",
		geoTable.c_str(), Parameters( "GEO_KEY")->asString(),
		joinTable.c_str(), Parameters("JOIN_KEY")->asString()
	);

	if( !Where.is_Empty() )
	{
		Join += " AND (" + Where + ")";
	}

	//-----------------------------------------------------
	CSG_String Fields; CSG_Parameters &P = *Parameters("FIELDS")->asParameters();

	for(int i=0; i<P.Get_Count(); i++)
	{
		if( P[i].Get_Parent() && (P[i].asBool() || P[i].Get_Parent()->asBool()) )
		{
			if( !Fields.is_Empty() )
			{
				Fields += ",";
			}

			Fields += CSG_String::Format("\"%s\".\"%s\"", P[i].Get_Parent()->Get_Name(), P[i].Get_Name());
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes(); pShapes->Destroy();

	if( !Get_Connection()->Shapes_Load(pShapes, geoTable + "." + joinTable, geoTable, "\"" + joinTable + "\"", Fields, Join) )
	{
		Error_Set(_TL("unable to load vector data from PostGIS database") + CSG_String(":\n") + geoTable + "." + joinTable);

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
