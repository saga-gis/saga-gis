/**********************************************************
 * Version $Id: wktb.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       wktb.cpp                        //
//                                                       //
//                 Copyright (C) 2012 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wktb.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SEPARATOR	SG_T('|')


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CWKT_Import::CWKT_Import(void)
{
	Set_Name		(_TL("Import Simple Features from Well Known Text"));

	Set_Author		(SG_T("O. Conrad (c) 2012"));

	Set_Description	(_TW(
		"Imports vector data from 'well known text' (WKT) simple features format.\n"
		"This import tool assumes that all features in a file are of the same type.\n"
		"Instead of importing from file(s), the tool also supports the conversion "
		"from a string provided with the 'WKT String' parameter.\n"
		"\n"
		"References:\n"
		"<a href=\"http://www.opengeospatial.org/\">Open Geospatial Consortium</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"			, _TL("WKT Import"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("Well Known Text Format (*.wkt)")	, SG_T("*.wkt"),
			_TL("All Files")						, SG_T("*.*")
		), NULL, false, false, true
 	);

	Parameters.Add_String(
		NULL	, "WKT"				, _TL("WKT String"),
		_TL("Import WKT from string instead of file. Just paste the WKT."),
		SG_T(""), true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKT_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_String		WKT_str;
	CSG_Strings		Files;

	WKT_str			= Parameters("WKT")->asString();

	if( (!Parameters("FILE")->asFilePath()->Get_FilePaths(Files) || Files.Get_Count() == 0) && WKT_str.Length() <= 0 )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide either a file input or WKT as a string!"));
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Shapes_List	*pList	= Parameters("SHAPES")->asShapesList();

	pList->Del_Items();

	//-----------------------------------------------------
	if( Files.Get_Count() > 0 )
	{
		for(int iFile=0; iFile<Files.Get_Count() && Process_Get_Okay(); iFile++)
		{
			CSG_String	WKT;
			CSG_Shapes	Shapes;
			CSG_File	Stream;

			if( Stream.Open(Files[iFile], SG_FILE_R, false) && Stream.Read(WKT, Stream.Length()) && Get_Type(WKT, Shapes) )
			{
				Parse_WKT(WKT, &Shapes);

				if( Shapes.is_Valid() && Shapes.Get_Count() > 0 )
				{
					Shapes.Set_Name(SG_File_Get_Name(Files[iFile], false));

					pList->Add_Item(SG_Create_Shapes(Shapes));
				}
			}
		}
	}
	else
	{
		CSG_Shapes	Shapes;

		Get_Type(WKT_str, Shapes);

		Parse_WKT(WKT_str, &Shapes);

		if( Shapes.is_Valid() && Shapes.Get_Count() > 0 )
		{
			Shapes.Set_Name("WKT_from_String");

			pList->Add_Item(SG_Create_Shapes(Shapes));
		}
	}


	return( pList->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define IS_OF_TYPE(t, t_shape, t_vertex)	{	CSG_String	Type(CSG_Shapes_OGIS_Converter::Type_asWKText(t)); Type.Make_Upper();	if( WKT.Find(Type) >= 0 )	{\
	Shapes.Create(t_shape, NULL, NULL, t_vertex);\
	Shapes.Add_Field("OID", SG_DATATYPE_Int);\
	WKT.Replace(Type, SEPARATOR + Type);\
	return( true );\
}	}

//---------------------------------------------------------
bool CWKT_Import::Get_Type(CSG_String &WKT, CSG_Shapes &Shapes)
{
	Shapes.Destroy();

	WKT.Make_Upper();

	IS_OF_TYPE(SG_OGIS_TYPE_MultiPointZM     , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_MultiPointZ      , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_MultiPointM      , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_MultiPoint       , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_MultiLineStringZM, SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_MultiLineStringZ , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_MultiLineStringM , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_MultiLineString  , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_MultiPolygonZM   , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_MultiPolygonZ    , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_MultiPolygonM    , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_MultiPolygon     , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_PointZM          , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_PointZ           , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_PointM           , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_Point            , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_LineStringZM     , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_LineStringZ      , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_LineStringM      , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_LineString       , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_PolygonZM        , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_PolygonZ         , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_PolygonM         , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_Polygon          , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XY  );

	return( false );
}

//---------------------------------------------------------
void CWKT_Import::Parse_WKT(CSG_String &WKT, CSG_Shapes *pShapes)
{
	while( WKT.Length() > 0 )
	{
		WKT	= WKT.AfterFirst(SEPARATOR);

		CSG_Shape	*pShape	= pShapes->Add_Shape();

		pShape->Set_Value(0, pShapes->Get_Count());

		if( !CSG_Shapes_OGIS_Converter::from_WKText(WKT.BeforeFirst(SEPARATOR), pShape) )
		{
			pShapes->Del_Shape(pShapes->Get_Count() - 1);
		}
	}
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKT_Export::CWKT_Export(void)
{
	Set_Name		(_TL("Export Simple Features to Well Known Text"));

	Set_Author		(SG_T("O. Conrad (c) 2012"));

	Set_Description	(_TW(
		"Exports vector data to 'well known text' (WKT) simple features format.\n"
		"\n"
		"References:\n"
		"<a href=\"http://www.opengeospatial.org/\">Open Geospatial Consortium</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"			, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("Well Known Text Format (*.wkt)")	, SG_T("*.wkt"),
			_TL("All Files")						, SG_T("*.*")
		), NULL, true
 	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKT_Export::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_W, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
	{
		CSG_String	WKT;

		if( CSG_Shapes_OGIS_Converter().to_WKText(pShapes->Get_Shape(iShape), WKT) )
		{
			Stream.Write(WKT);
			Stream.Write("\n");
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
