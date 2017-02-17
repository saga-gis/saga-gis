/**********************************************************
 * Version $Id: pgis_shapes.cpp 1646 2013-04-10 16:29:00Z oconrad $
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
#include "pgis_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Load::CShapes_Load(void)
{
	Set_Name		(_TL("Import Shapes from PostGIS"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Imports shapes from a PostGIS database."
	));

	Parameters.Add_Shapes(NULL,
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL,
		"TABLES"	, _TL("Tables"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
void CShapes_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, "geometry_columns") )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("f_table_name") + CSG_String("|");
		}
	}

	pParameters->Get_Parameter("TABLES")->asChoice()->Set_Items(s);
}

//---------------------------------------------------------
bool CShapes_Load::On_Execute(void)
{
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();
	CSG_String	Name		= Parameters("TABLES")->asString();

	if( !Get_Connection()->Shapes_Load(pShapes, Name) )
	{
		Error_Set(_TL("unable to load vector data from PostGIS database") + CSG_String(":\n") + Name);

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
	//-----------------------------------------------------
	Set_Name		(_TL("Export Shapes to PostGIS"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Exports shapes to a PostGIS database."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(NULL,
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Set_Constraints(&Parameters, "SHAPES");

	Parameters.Add_String(NULL,
		"NAME"		, _TL("Table Name"),
		_TL("if empty shapes layers's name is used as table name"),
		""
	);

	Parameters.Add_Choice(NULL,
		"EXISTS"	, _TL("If table exists..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
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
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES") )
	{
		CSG_Shapes	*pShapes	= pParameter->asShapes() ? pParameter->asShapes() : NULL;

		if( pShapes && *pShapes->Get_Name() )
		{
			pParameters->Get_Parameter("NAME")->Set_Value(pShapes->Get_Name());
		}

		Set_SRID(pParameters, pShapes ? pShapes->Get_Projection().Get_EPSG() : -1);
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
	CSG_Shapes	*pShapes;
	CSG_String	SQL, Name, Type, Field, SavePoint;

	pShapes		= Parameters("SHAPES")->asShapes();
	Name		= Parameters("NAME"  )->asString();	if( Name.Length() == 0 )	Name	= pShapes->Get_Name();

	Field		= "geometry";

	int	SRID	= Get_SRID();

	//-----------------------------------------------------
	if( !CSG_Shapes_OGIS_Converter::from_ShapeType(Type, pShapes->Get_Type(), pShapes->Get_Vertex_Type()) )
	{
		Error_Set(_TL("invalid or unsupported shape or vertex type"));

		return( false );
	}

	//-----------------------------------------------------
	Get_Connection()->Begin(SavePoint = Get_Connection()->is_Transaction() ? "SHAPES_SAVE" : "");

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Name) )
	{
		Message_Add(_TL("table already exists") + CSG_String(": ") + Name);

		switch( Parameters("EXISTS")->asInt() )
		{
		case 0:	// abort export
			return( false );

		case 1:	// replace existing table
			Message_Add(_TL("trying to drop table") + CSG_String(": ") + Name);

			if( !Get_Connection()->Table_Drop(Name, false) )
			{
				Message_Add(CSG_String(" ...") + _TL("failed") + "!");

				return( false );
			}

			break;

		case 2:	// append records, if table structure allows
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
		// SELECT AddGeometryColumn(<table_name>, <column_name>, <srid>, <type>, <dimension>)

		SQL.Printf("SELECT AddGeometryColumn('%s', '%s', %d, '%s', %d)",
			Name.c_str(), Field.c_str(), SRID, Type.c_str(),
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
	bool	bBinary	= Get_Connection()->has_Version(9);

	int		iShape, iField, nAdded;

	CSG_String	Insert	= "INSERT INTO \"" + Name + "\" (" + Field;

	for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
	{
		Insert	+= CSG_String(", ") + pShapes->Get_Field_Name(iField);
	}

	Insert	+= ") VALUES (";

	//-----------------------------------------------------
	for(iShape=0, nAdded=0; iShape==nAdded && iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( pShape->is_Valid() )
		{
			SQL	= Insert;

			if( bBinary )
			{
				CSG_Bytes	WKB;

				CSG_Shapes_OGIS_Converter::to_WKBinary(pShape, WKB);

				SQL	+= "ST_GeomFromWKB(E'\\\\x" + WKB.toHexString() + CSG_String::Format("', %d)", SRID);
			}
			else
			{
				CSG_String	WKT;

				CSG_Shapes_OGIS_Converter::to_WKText(pShape, WKT);

				SQL	+= "ST_GeomFromText('" + WKT + CSG_String::Format("', %d)", SRID);
			}

			for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
			{
				CSG_String	s = pShape->asString(iField);

				switch( pShapes->Get_Field_Type(iField) )
				{
				case SG_DATATYPE_String:
				case SG_DATATYPE_Date:
					if( 1 )
					{
						char	*_s	= NULL; if( s.to_ASCII(&_s) ) s = _s; SG_FREE_SAFE(_s);
					}

					s.Replace("'", "\"");

					s	= "'" + s + "'";
					break;
				}

				SQL	+= ", "  + s;
			}

			SQL	+= ")";

			if( Get_Connection()->Execute(SQL) )
			{
				nAdded++;
			}
			else
			{
				Message_Add(CSG_String::Format("%s [%d/%d]", _TL("could not save shape"), 1 + iShape, pShapes->Get_Count()));
			}
		}
	}

	//-----------------------------------------------------
	if( nAdded < pShapes->Get_Count() )
	{
		Message_Add(SQL);

		Get_Connection()->Rollback(SavePoint);

		return( false );
	}

	Get_Connection()->Commit(SavePoint);

	Get_Connection()->GUI_Update();

	Get_Connection()->Add_MetaData(*pShapes, Name);

	pShapes->Set_Modified(false);

	return( true );
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

	Parameters.Add_Choice(NULL,
		"TABLES"	, _TL("Tables"),
		_TL(""),
		""
	);

	Add_SRID_Picker();
}

//---------------------------------------------------------
void CShapes_SRID_Update::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, "geometry_columns") )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("f_table_name") + CSG_String("|");
		}
	}

	pParameters->Get_Parameter("TABLES")->asChoice()->Set_Items(s);
}

//---------------------------------------------------------
bool CShapes_SRID_Update::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS() )	{	Error_Set(_TL("no PostGIS layer"));	return( false );	}

	//-----------------------------------------------------
	CSG_String	Select;
	CSG_Table	Table;

	Select.Printf("f_table_name='%s'", Parameters("TABLES")->asString());

	if( !Get_Connection()->Table_Load(Table, "geometry_columns", "*", Select) || Table.Get_Count() != 1 )
	{
		return( false );
	}

	Select.Printf("SELECT UpdateGeometrySRID('%s', '%s', %d)",
		Parameters("TABLES")->asString(),
		Table[0].asString("f_geometry_column"),
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
CShapes_Join::CShapes_Join(void)
{
	Set_Name		(_TL("Import Shapes with Joined Data from PostGIS (GUI)"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Imports shapes with joined data from a PostGIS database."
	));

	Parameters.Add_Shapes(NULL,
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL                    ,  "GEO_TABLE", _TL("Geometry Table"), _TL(""), "");
	Parameters.Add_Choice(Parameters( "GEO_TABLE"),  "GEO_KEY"  , _TL("Key"           ), _TL(""), "");

	Parameters.Add_Choice(NULL                    , "JOIN_TABLE", _TL("Join Table"    ), _TL(""), "");
	Parameters.Add_Choice(Parameters("JOIN_TABLE"), "JOIN_KEY"  , _TL("Key"           ), _TL(""), "");

	Parameters.Add_Parameters(NULL,
		"FIELDS"	, _TL("Fields"),
		_TL("")
	);

	Parameters.Add_String(NULL,
		"WHERE"		, _TL("Where"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
void CShapes_Join::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, "geometry_columns") )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString("f_table_name") + CSG_String("|");
		}
	}

	pParameters->Get_Parameter("GEO_TABLE")->asChoice()->Set_Items(s);

	CSG_Parameter	*pParameter	= pParameters->Get_Parameter("JOIN_TABLE");

	pParameter->asChoice()->Set_Items(Get_Connection()->Get_Tables());
	pParameter->Set_Value(pParameter->asString());

	On_Parameter_Changed(pParameters, pParameter);
}

//---------------------------------------------------------
int CShapes_Join::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(),  "GEO_TABLE")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "JOIN_TABLE") )
	{
		Update_Fields(pParameters,  true);
		Update_Fields(pParameters, false);
	}

	if( !SG_STR_CMP(pParameters->Get_Identifier(), "FIELDS") && !pParameter->Get_Parent() )
	{
		for(int i=0; i<pParameter->Get_Children_Count(); i++)
		{
			pParameter->Get_Child(i)->Set_Enabled(pParameter->asBool() == false);
		}
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
void CShapes_Join::Update_Fields(CSG_Parameters *pParameters, bool bGeoTable)
{
	CSG_Parameters	&Fields	= *pParameters->Get_Parameter("FIELDS")->asParameters();

	if( bGeoTable )
	{
		Fields.Del_Parameters();
	}

	CSG_String	FieldList, geoField, Table(pParameters->Get_Parameter(bGeoTable ? "GEO_TABLE" : "JOIN_TABLE")->asString());
	CSG_Table	t	= Get_Connection()->Get_Field_Desc(Table);
	CSG_Parameter	*pNode	= Fields.Add_Bool(NULL, Table, Table, "");

	Get_Connection()->Shapes_Geometry_Info(Table, &geoField, NULL);

	for(int i=0; i<t.Get_Count(); i++)
	{
		if( geoField.Cmp(t[i].asString(0)) )
		{
			FieldList	+= t[i].asString(0) + CSG_String("|");
			Fields.Add_Bool(pNode, CSG_String::Format("%s.%s", Table.c_str(), t[i].asString(0)), t[i].asString(0), "");
		}
	}

	pParameters->Get_Parameter(bGeoTable ? "GEO_KEY" : "JOIN_KEY")->asChoice()->Set_Items(FieldList);
}

//---------------------------------------------------------
bool CShapes_Join::On_Execute(void)
{
	CSG_String	geoTable	= Parameters("GEO_TABLE" )->asString();
	CSG_String	joinTable	= Parameters("JOIN_TABLE")->asString();

	if( !geoTable.Cmp(joinTable) )
	{
		Error_Set(_TL("Geometry and join table must not be identical."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Join, Where	= Parameters("WHERE")->asString();

	Join.Printf("%s.%s=%s.%s",
		 geoTable.c_str(), Parameters( "GEO_KEY")->asString(),
		joinTable.c_str(), Parameters("JOIN_KEY")->asString()
	);

	if( !Where.is_Empty() )
	{
		Join	+= " AND (" + Where + ")";
	}

	//-----------------------------------------------------
	CSG_String	Fields;

	CSG_Parameters	&P	= *Parameters("FIELDS")->asParameters();

	for(int i=0; i<P.Get_Count(); i++)
	{
		if( P[i].Get_Parent() && (P[i].asBool() || P[i].Get_Parent()->asBool()) )
		{
			if( !Fields.is_Empty() )
			{
				Fields	+= ",";
			}

			Fields	+= P[i].Get_Identifier();
		}
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Shapes_Load(Parameters("SHAPES")->asShapes(), geoTable + "." + joinTable, geoTable, joinTable, Fields, Join) )
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
