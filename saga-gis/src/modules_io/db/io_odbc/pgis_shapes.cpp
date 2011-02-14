
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
#include "pgis_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPGIS_Shapes_Load::CPGIS_Shapes_Load(void)
{
	Set_Name		(_TL("PostGIS Shapes Import"));

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

	if( !Get_Connection()->is_PostgreSQL() )
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
	TSG_Vertex_Type	tVertex;
	TSG_Shape_Type	tShape;
	CSG_String		Select, Geo_Table, Geo_Type, Geo_Field;
	CSG_Table		Geo_Tables;
	CSG_Shapes		*pShapes;

	pShapes		= Parameters("SHAPES")	->asShapes();
	Geo_Table	= Parameters("TABLES")	->asString();

	Select.Printf(SG_T("SELECT * FROM geometry_columns WHERE f_table_name = '%s'"), Geo_Table.c_str());

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
	if( !Get_Connection()->Table_Load(*pShapes, Geo_Table) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( 1 )	// WKBinary
	{
		CSG_Bytes_Array	BLOBs;

		Select.Printf(SG_T("AsBinary(%s) AS geom"), Geo_Field.c_str());

		if( !Get_Connection()->Table_Load_BLOBs(BLOBs, Geo_Table, Select, SG_T(""), SG_T("")) )
		{
			return( false );
		}

		if( BLOBs.Get_Count() != pShapes->Get_Count() )
		{
			return( false );
		}

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shapes_OGIS_Converter::from_WKBinary(BLOBs[iShape], pShapes->Get_Shape(iShape));
		}
	}

	//-----------------------------------------------------
	else	// WKText
	{
		CSG_Table	Shapes;

		Select.Printf(SG_T("AsText(%s) AS geom"), Geo_Field.c_str());

		if( !Get_Connection()->Table_Load(Shapes, Geo_Table, Select, SG_T(""), SG_T("")) )
		{
			return( false );
		}

		if( Shapes.Get_Count() != pShapes->Get_Count() )
		{
			return( false );
		}

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shapes_OGIS_Converter::from_WKText(Shapes[iShape].asString(0), pShapes->Get_Shape(iShape));
		}
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
	Set_Name		(_TL("PostGIS Shapes Export"));

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

	Parameters.Add_Parameters(
		NULL	, "FLAGS"		, _TL("Constraints"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "EXISTS"		, _TL("If table exists..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s"),
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

	return( 0 );
}

//---------------------------------------------------------
bool CPGIS_Shapes_Save::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	if( !Get_Connection()->is_PostgreSQL() )
	{
		SG_UI_Dlg_Message(_TL("Not a PostgreSQL database!"), _TL("Database Connection Error"));

		return( false );
	}

	if( !Get_Connection()->Table_Exists(SG_T("spatial_ref_sys")) || !Get_Connection()->Table_Exists(SG_T("geometry_columns")) )
	{
		SG_UI_Dlg_Message(_TL("Not a valid PostGIS database!"), _TL("Database Connection Error"));

		return( false );
	}

	Parameters("SRID")->asChoice()->Set_Items(SG_Get_Projections().Get_Names_List());

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
	CSG_String	SQL, Geo_Table, Geo_Type, Geo_Field, sSRID;

	pShapes		= Parameters("SHAPES")	->asShapes();
	Geo_Table	= Parameters("NAME")	->asString();	if( Geo_Table.Length() == 0 )	Geo_Table	= pShapes->Get_Name();

	if( !Parameters("SRID")->asChoice()->Get_Data(SRID) )
	{
		SRID	= -1;
	}

	sSRID.Printf(SG_T("%d"), SRID);

	//-----------------------------------------------------
	switch( pShapes->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		switch( pShapes->Get_Type() )
		{	default:	return( false );
		case SHAPE_TYPE_Point:		Geo_Type	= SG_OGIS_TYPE_STR_Point;			Geo_Field	= SG_T("geo_point");		break;
		case SHAPE_TYPE_Points:		Geo_Type	= SG_OGIS_TYPE_STR_MultiPoint;		Geo_Field	= SG_T("geo_points");		break;
		case SHAPE_TYPE_Line:		Geo_Type	= SG_OGIS_TYPE_STR_MultiLine;		Geo_Field	= SG_T("geo_line");			break;
		case SHAPE_TYPE_Polygon:	Geo_Type	= SG_OGIS_TYPE_STR_MultiPolygon;	Geo_Field	= SG_T("geo_polygon");		break;
		}
		break;

	case SG_VERTEX_TYPE_XYZ:
		switch( pShapes->Get_Type() )
		{	default:	return( false );
		case SHAPE_TYPE_Point:		Geo_Type	= SG_OGIS_TYPE_STR_Point_Z;			Geo_Field	= SG_T("geo_point_z");		break;
		case SHAPE_TYPE_Points:		Geo_Type	= SG_OGIS_TYPE_STR_MultiPoint_Z;	Geo_Field	= SG_T("geo_points_z");		break;
		case SHAPE_TYPE_Line:		Geo_Type	= SG_OGIS_TYPE_STR_MultiLine_Z;		Geo_Field	= SG_T("geo_line_z");		break;
		case SHAPE_TYPE_Polygon:	Geo_Type	= SG_OGIS_TYPE_STR_MultiPolygon_Z;	Geo_Field	= SG_T("geo_polygon_z");	break;
		}
		break;

	case SG_VERTEX_TYPE_XYZM:
		switch( pShapes->Get_Type() )
		{	default:	return( false );
		case SHAPE_TYPE_Point:		Geo_Type	= SG_OGIS_TYPE_STR_Point_ZM;		Geo_Field	= SG_T("geo_point_zm");		break;
		case SHAPE_TYPE_Points:		Geo_Type	= SG_OGIS_TYPE_STR_MultiPoint_ZM;	Geo_Field	= SG_T("geo_points_zm");	break;
		case SHAPE_TYPE_Line:		Geo_Type	= SG_OGIS_TYPE_STR_MultiLine_ZM;	Geo_Field	= SG_T("geo_line_zm");		break;
		case SHAPE_TYPE_Polygon:	Geo_Type	= SG_OGIS_TYPE_STR_MultiPolygon_ZM;	Geo_Field	= SG_T("geo_polygon_zm");	break;
		}
		break;
	}

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Geo_Table) )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("table already exists"), Geo_Table.c_str()));

		switch( Parameters("EXISTS")->asInt() )
		{
		case 0:	// abort export
			return( false );

		case 1:	// replace existing table
			Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("trying to drop table"), Geo_Table.c_str()));

			if( !Get_Connection()->Table_Drop(Geo_Table, false) )
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
	if( !Get_Connection()->Table_Exists(Geo_Table) && !Get_Connection()->Table_Create(Geo_Table, *pShapes, Get_Constraints(Parameters("FLAGS")->asParameters(), pShapes), false) )
	{
		Get_Connection()->Rollback();

		return( false );
	}

	//-----------------------------------------------------
	SQL.Printf(SG_T("SELECT AddGeometryColumn('%s', '%s', %d, '%s', %d)"),
		Geo_Table.c_str(),				// <table_name>
		Geo_Field.c_str(),				// <column_name>
		SRID,							// <srid>
		Geo_Type.Make_Upper().c_str(),	// <type>
		2								// <dimension>
	);

	if( !Get_Connection()->Execute(SQL) )
	{
		Get_Connection()->Rollback();

		Message_Add(_TL("could not create geometry field"));

		return( false );
	}

	//-----------------------------------------------------
	int			iShape, iField, nAdded;
	CSG_String	Insert, Fields, sWKT;

	Fields	= Geo_Field;

	for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
	{
		Fields	+= CSG_String(", ") + pShapes->Get_Field_Name(iField);
	}

	Insert.Printf(SG_T("INSERT INTO %s (%s) VALUES ("), Geo_Table.c_str(), Fields.c_str());

	for(iShape=0, nAdded=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( pShape->is_Valid() )
		{
			SQL	= Insert;

			CSG_Shapes_OGIS_Converter::to_WKText(pShape, sWKT);

			SQL	+= SG_T("GeomFromText('") + sWKT + SG_T("', ") + sSRID + SG_T(")");

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
