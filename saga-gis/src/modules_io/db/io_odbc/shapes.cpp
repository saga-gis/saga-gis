/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        io_odbc                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Shapes.cpp                       //
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
#include "get_connection.h"

#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_Load::CPoints_Load(void)
{
	Set_Name		(_TL("Points Import"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Imports points from a database via ODBC."
	));

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
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
bool CPoints_Load::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	CSG_String	Table(Parameters("TABLES")->asString());

	return( true );
}

//---------------------------------------------------------
bool CPoints_Load::On_Execute(void)
{
	if( Get_Connection()->is_Connected() )
	{
		CSG_Parameter_Choice	*pTables	= Parameters("TABLES")	->asChoice();
		CSG_Shapes				*pShapes	= Parameters("POINTS")	->asShapes();

		return( Get_Connection()->Table_Load(*pShapes, pTables->asString()) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_Save::CPoints_Save(void)
{
	Set_Name		(_TL("Points Export"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Exports points to a database via ODBC."
	));

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);
}

//---------------------------------------------------------
bool CPoints_Save::On_Execute(void)
{
	if( Get_Connection()->is_Connected() )
	{
		CSG_Shapes	*pShapes	= Parameters("POINTS")	->asShapes();

		return( Get_Connection()->Table_Save(pShapes->Get_Name(), *pShapes) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Load::CShapes_Load(void)
{
	Set_Name		(_TL("Shapes Import"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Imports shapes from a database via ODBC."
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
bool CShapes_Load::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	CSG_String	Table(Parameters("TABLES")->asString());

	Parameters("TABLES")->asChoice()->Set_Items(Get_Connection()->Get_Tables());
	Parameters("TABLES")->Set_Value(Table);

	return( true );
}

//---------------------------------------------------------
bool CShapes_Load::On_Execute(void)
{
	if( !Get_Connection()->is_Connected() )
	{
		return( false );
	}
	
	TSG_Vertex_Type	tVertex;
	TSG_Shape_Type	tShape;
	CSG_String		Select, Geo_Table, Geo_Type, Geo_Field;
	CSG_Table		Geo_Tables;
	CSG_Shapes		*pShapes;

	pShapes		= Parameters("SHAPES")	->asShapes();
	Geo_Table	= Parameters("TABLES")	->asString();

/*	Select.Printf(SG_T("SELECT * FROM geometry_columns WHERE f_table_name = '%s'"), Geo_Table.c_str());

	if( !Get_Connection()->Table_Load(Geo_Tables, SG_T("geometry_columns"), SG_T("*"), CSG_String::Format(SG_T("f_table_name = '%s'"), Geo_Table.c_str()), SG_T("")) )
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
	     if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Point           ) )	{	tShape	= SHAPE_TYPE_Point;		tVertex	= SG_VERTEX_TYPE_XY;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint      ) )	{	tShape	= SHAPE_TYPE_Points;	tVertex	= SG_VERTEX_TYPE_XY;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Line            ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XY;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine       ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XY;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon         ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XY;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon    ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XY;	}

	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Point_Z         ) )	{	tShape	= SHAPE_TYPE_Point;		tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint_Z    ) )	{	tShape	= SHAPE_TYPE_Points;	tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Line_Z          ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine_Z     ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon_Z       ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon_Z  ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XYZ;	}

	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Point_M         ) )	{	tShape	= SHAPE_TYPE_Point;		tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint_M    ) )	{	tShape	= SHAPE_TYPE_Points;	tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Line_M          ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine_M     ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon_M       ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XYZ;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon_M  ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XYZ;	}

	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Point_ZM        ) )	{	tShape	= SHAPE_TYPE_Point;		tVertex	= SG_VERTEX_TYPE_XYZM;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint_ZM   ) )	{	tShape	= SHAPE_TYPE_Points;	tVertex	= SG_VERTEX_TYPE_XYZM;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Line_ZM         ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XYZM;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine_ZM    ) )	{	tShape	= SHAPE_TYPE_Line;		tVertex	= SG_VERTEX_TYPE_XYZM;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon_ZM      ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XYZM;	}
	else if( !Geo_Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon_ZM ) )	{	tShape	= SHAPE_TYPE_Polygon;	tVertex	= SG_VERTEX_TYPE_XYZM;	}

	else
	{
		return( false );
	}
/**/
	tShape	= SHAPE_TYPE_Point;
	tVertex	= SG_VERTEX_TYPE_XY;
	Geo_Field	= SG_T("geom");

	if( pShapes->Get_Type() != SHAPE_TYPE_Undefined && pShapes->Get_Type() != tShape )
	{
		pShapes	= SG_Create_Shapes(tShape, Geo_Table, NULL, tVertex);

		Parameters("SHAPES")->Set_Value(pShapes);
	}
	else
	{
		pShapes->Create(tShape, Geo_Table, NULL, tVertex);
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Load(*pShapes, Geo_Table, true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Bytes_Array	BLOBs;

	Select.Printf(SG_T("%s"), Geo_Field.c_str());

	if( !Get_Connection()->Table_Load_BLOBs(BLOBs, Geo_Table, Select, SG_T(""), SG_T("")) )
	{
		return( false );
	}

	if( BLOBs.Get_Count() != pShapes->Get_Count() )
	{
		return( false );
	}

	CSG_Table	t;	t.Add_Field(SG_T("geom"), SG_DATATYPE_String);

	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_String	s	= SG_T("01");
		
		for(int i=1; i<BLOBs[iShape].Get_Count(); i++)
			s	+= (char)BLOBs[iShape][i];

		CSG_Bytes	b;
		b.fromHexString(s);
		CSG_Shapes_OGIS_Converter::from_WKBinary(b, pShapes->Get_Shape(iShape));

	//	CSG_Shapes_OGIS_Converter::from_WKBinary(BLOBs[iShape], pShapes->Get_Shape(iShape));

		CSG_Table_Record	*pr = t.Add_Record();	pr->Set_Value(0, (const SG_Char *)BLOBs[iShape].Get_Bytes());
	}

	DataObject_Add(SG_Create_Table(t));

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
	Set_Name		(_TL("Shapes Export"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Exports shapes to a database via ODBC."
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
bool CShapes_Save::On_Execute(void)
{
	if( !Get_Connection()->is_Connected() )
	{
		return( false );
	}

	int			iShape, iField, nAdded;
	CSG_String	Insert, Fields, sWKT;
	CSG_String	SQL, Name, Geo_Type, Geo_Field, sSRID;
	CSG_Shapes	*pShapes;

	pShapes		= Parameters("SHAPES")	->asShapes();
	Name		= Parameters("NAME")	->asString();	if( Name.Length() == 0 )	Name	= pShapes->Get_Name();

//	if( !Parameters("SRID")->asChoice()->Get_Data(SRID) )
//	{
//		SRID	= -1;
//	}

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Name) == true )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("table already exists"), Name.c_str()));

		switch( Parameters("EXISTS")->asInt() )
		{
		case 0:	// abort export
			return( false );

		case 1:	// replace existing table
			Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("trying to drop table"), Name.c_str()));

			if( !Get_Connection()->Table_Drop(Name, false) )
			{
				Message_Add(CSG_String::Format(SG_T(" ...%s!"), _TL("failed")));

				return( false );
			}
			break;

		case 2:	// append records, if table structure allows
			break;
		}
	}

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Name) == false )
	{
		CSG_Table	Structure;

		Structure.Add_Field(SG_T("GEOM"), SG_DATATYPE_Binary);

		for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
		{
			Structure.Add_Field(pShapes->Get_Field_Name(iField), pShapes->Get_Field_Type(iField));
		}

		if( !Get_Connection()->Table_Create(Name, Structure) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	Fields	= SG_T("GEOM");

	for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
	{
		Fields	+= CSG_String(", ") + pShapes->Get_Field_Name(iField);
	}

	Insert.Printf(SG_T("INSERT INTO %s (%s) VALUES ("), Name.c_str(), Fields.c_str());

	for(iShape=0, nAdded=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Bytes	Bytes;
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( pShape->is_Valid() && CSG_Shapes_OGIS_Converter::to_WKBinary(pShape, Bytes) )
		{
			SQL	= Insert;
			
			if( Get_Connection()->is_PostgreSQL() )
			{
			//	SQL	+= SG_T("decode('") + Bytes.toHexString() + SG_T("', 'hex')");
				SQL	+= SG_T("'\\x") + Bytes.toHexString() + SG_T("'");
			}
			else
			{
				SQL	+= SG_T("'0x") + Bytes.toHexString() + SG_T("'");
			}

			for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
			{
				CSG_String	s = pShape->asString(iField);

				if( pShapes->Get_Field_Type(iField) == SG_DATATYPE_String )
				{
					s.Replace(SG_T("'"), SG_T("\""));
					s	= SG_T("'") + s + SG_T("'");
				}

				SQL	+= SG_T(", ")  + s;
			}

			SQL	+= SG_T(")");

			if( Get_Connection()->Execute(SQL) )
			{
				nAdded++;
			}
			else
			{
				Message_Add(CSG_String::Format(SG_T("dropped %d. shape"), 1 + iShape));
			}
		}
	}

	//-----------------------------------------------------
	if( nAdded == 0 )
	{
		Get_Connection()->Rollback();

		Get_Connection()->Table_Drop(Name);

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
