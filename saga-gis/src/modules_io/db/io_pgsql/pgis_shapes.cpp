/**********************************************************
 * Version $Id: pgis_shapes.cpp 1646 2013-04-10 16:29:00Z oconrad $
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
#include "pgis_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPGIS_Shapes_Load::CPGIS_Shapes_Load(void)
{
	Set_Name		(_TL("Import Shapes from PostGIS"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Imports shapes from a PostGIS database."
	));

	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
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
void CPGIS_Shapes_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_String	s;
	CSG_Table	t;

	if( Get_Connection()->Table_Load(t, SG_T("geometry_columns")) )
	{
		for(int i=0; i<t.Get_Count(); i++)
		{
			s	+= t[i].asString(SG_T("f_table_name")) + CSG_String("|");
		}
	}

	Parameters("TABLES")->asChoice()->Set_Items(s);
}

//---------------------------------------------------------
bool CPGIS_Shapes_Load::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS() )
	{
		Error_Set(_TL("not a valid PostGIS database!"));

		return( false );
	}

	//-----------------------------------------------------
	TSG_Vertex_Type	tVertex;
	TSG_Shape_Type	tShape;
	CSG_String		Name, Field;
	CSG_Table		Info;
	CSG_Shapes		*pShapes;

	//-----------------------------------------------------
	pShapes	= Parameters("SHAPES")->asShapes();
	Name	= Parameters("TABLES")->asString();

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Load(Info, "geometry_columns", "*", "f_table_name = '" + Name + "'") || Info.Get_Count() != 1 )
	{
		Error_Set(_TL("unable to obtain geometry information") + CSG_String(":\n") + Name);

		return( false );
	}

	//-----------------------------------------------------
	if( !CSG_Shapes_OGIS_Converter::to_ShapeType(Info[0].asString("type"), tShape, tVertex) )
	{
		Error_Set(_TL("invalid or unsupported vector format") + CSG_String(":\n") + Info[0].asString("type"));

		return( false );
	}

	if( pShapes->Get_Type() != SHAPE_TYPE_Undefined && pShapes->Get_Type() != tShape )
	{
		pShapes	= SG_Create_Shapes(tShape, Name, NULL, tVertex);

		Parameters("SHAPES")->Set_Value(pShapes);
	}
	else
	{
		pShapes->Create(tShape, Name, NULL, tVertex);
	}

	//-----------------------------------------------------
	if( !Get_Connection()->Table_Load(*pShapes, Name) )
	{
		Error_Set(_TL("failed to load attributes"));

		return( false );
	}

	pShapes->Del_Field(pShapes->Get_Field(Field));

	pShapes->Get_Projection().Create(Info[0].asInt("srid"));

	Field	= Info[0].asString("f_geometry_column");

	//-----------------------------------------------------
	bool	bBinary	= true;

	if( !Get_Connection()->Table_Load(Info, Name, (bBinary ? "ST_AsBinary(" : "ST_AsText('") + Field + ") AS geom") || Info.Get_Count() != pShapes->Get_Count() )
	{
		Error_Set(_TL("failed to load geometries"));

		return( false );
	}

	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		if( bBinary )
		{
			CSG_Shapes_OGIS_Converter::from_WKBinary(Info[iShape].Get_Value(0)->asBinary(), pShapes->Get_Shape(iShape));
		}
		else
		{
			CSG_Shapes_OGIS_Converter::from_WKText  (Info[iShape].Get_Value(0)->asString(), pShapes->Get_Shape(iShape));
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
CPGIS_Shapes_Save::CPGIS_Shapes_Save(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Export Shapes to PostGIS"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Exports shapes to a PostGIS database."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Table Name"),
		_TL("if empty shapes layers's name is used as table name"),
		SG_T("")
	);

	Parameters.Add_Parameters(
		NULL	, "FLAGS"		, _TL("Constraints"),
		_TL("")
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
int CPGIS_Shapes_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SHAPES")) )
	{
		pParameters->Get_Parameter("NAME")->Set_Value(pParameter->asShapes() ? pParameter->asShapes()->Get_Name() : SG_T(""));

		Set_Constraints(pParameters->Get_Parameter("FLAGS")->asParameters(), pParameter->asShapes());
	}

	return( CSG_PG_Module::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CPGIS_Shapes_Save::On_Execute(void)
{
	if( !Get_Connection()->has_PostGIS() )
	{
		Error_Set(_TL("not a valid PostGIS database!"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pShapes;
	CSG_String	SQL, Name, Type, Field, SRID;

	pShapes	= Parameters("SHAPES")->asShapes();
	Name	= Parameters("NAME"  )->asString();	if( Name.Length() == 0 )	Name	= pShapes->Get_Name();

	SRID.Printf(SG_T("%d"), pShapes->Get_Projection().Get_EPSG());

	//-----------------------------------------------------
	if( !CSG_Shapes_OGIS_Converter::from_ShapeType(Type, pShapes->Get_Type(), pShapes->Get_Vertex_Type()) )
	{
		Error_Set(_TL("invalid or unsupported shape or vertex type"));

		return( false );
	}

	Get_Connection()->Begin();

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
	if( !Get_Connection()->Table_Exists(Name) && !Get_Connection()->Table_Create(Name, *pShapes, Get_Constraints(Parameters("FLAGS")->asParameters(), pShapes), false) )
	{
		Error_Set(_TL("could not create table"));

		Get_Connection()->Rollback();

		return( false );
	}

	//-----------------------------------------------------
	Field	= "geometry";
	Type	.Make_Upper();

	SQL	= "SELECT AddGeometryColumn('"
		+ Name  + "', '" // <table_name>
		+ Field + "', "  // <column_name>
		+ SRID  +  ", '" // <srid>
		+ Type  + "', "  // <type>
		+ (pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XY  ? "2"
		 : pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZ ? "3" : "4") + ")";		// <dimension>

	if( !Get_Connection()->Execute(SQL) )
	{
		Error_Set(_TL("could not create geometry field"));

		Get_Connection()->Rollback();

		return( false );
	}

	//-----------------------------------------------------
	bool	bBinary	= true;
	int		iShape, iField, nAdded;

	CSG_String	Insert	= "INSERT INTO \"" + Name + "\" (" + Field;

	for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
	{
		Insert	+= CSG_String(", ") + pShapes->Get_Field_Name(iField);
	}

	Insert	+= ") VALUES (";

	//-----------------------------------------------------
	for(iShape=0, nAdded=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( pShape->is_Valid() )
		{
			SQL	= Insert;

			if( bBinary )
			{
				CSG_Bytes	WKB;

				CSG_Shapes_OGIS_Converter::to_WKBinary(pShape, WKB);

				SQL	+= SG_T("ST_GeomFromWKB(E'\\\\x") + WKB.toHexString() + SG_T("', ") + SRID + SG_T(")");
			}
			else
			{
				CSG_String	WKT;

				CSG_Shapes_OGIS_Converter::to_WKText(pShape, WKT);

				SQL	+= SG_T("ST_GeomFromText('") + WKT + SG_T("', ") + SRID + SG_T(")");
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
				Message_Add(CSG_String::Format(SG_T("dropped %d. shape"), iShape));
			}
		}
	}

	//-----------------------------------------------------
	if( nAdded == 0 )
	{
		Get_Connection()->Rollback();

		return( false );
	}

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
