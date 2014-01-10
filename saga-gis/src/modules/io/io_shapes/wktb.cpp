/**********************************************************
 * Version $Id: wktb.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKT_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Strings		Files;

	if( !Parameters("FILE")->asFilePath()->Get_FilePaths(Files) || Files.Get_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Shapes_List	*pList	= Parameters("SHAPES")->asShapesList();

	pList->Del_Items();

	//-----------------------------------------------------
	for(int iFile=0, iPosition; iFile<Files.Get_Count() && Process_Get_Okay(); iFile++)
	{
		CSG_String	WKT;
		CSG_Shapes	Shapes;
		CSG_File	Stream;

		if( Stream.Open(Files[iFile], SG_FILE_R, false) && Stream.Read(WKT, Stream.Length()) && Get_Type(WKT, Shapes) )
		{
			while( WKT.Length() > 0 )
			{
				WKT	= WKT.AfterFirst(SEPARATOR);

				CSG_Shape	*pShape	= Shapes.Add_Shape();

				pShape->Set_Value(0, Shapes.Get_Count());

				if( !CSG_Shapes_OGIS_Converter::from_WKText(WKT.BeforeFirst(SEPARATOR), pShape) )
				{
					Shapes.Del_Shape(Shapes.Get_Count() - 1);
				}
			}

			if( Shapes.is_Valid() && Shapes.Get_Count() > 0 )
			{
				Shapes.Set_Name(SG_File_Get_Name(Files[iFile], false));

				pList->Add_Item(SG_Create_Shapes(Shapes));
			}
		}
	}

	return( pList->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define IS_OF_TYPE(t_wkt, t_shape, t_vertex)	{	CSG_String	Type(t_wkt); Type.Make_Upper();	if( WKT.Find(Type) >= 0 )	{\
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

	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPoint_ZM  , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPoint_Z   , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPoint_M   , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPoint     , SHAPE_TYPE_Points , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiLine_ZM   , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiLine_Z    , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiLine_M    , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiLine      , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPolygon_ZM, SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPolygon_Z , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPolygon_M , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_MultiPolygon   , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_STR_Point_ZM       , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Point_Z        , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Point_M        , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Point          , SHAPE_TYPE_Point  , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_STR_Line_ZM        , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Line_Z         , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Line_M         , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Line           , SHAPE_TYPE_Line   , SG_VERTEX_TYPE_XY  );

	IS_OF_TYPE(SG_OGIS_TYPE_STR_Polygon_ZM     , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZM);
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Polygon_Z      , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Polygon_M      , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XYZ );
	IS_OF_TYPE(SG_OGIS_TYPE_STR_Polygon        , SHAPE_TYPE_Polygon, SG_VERTEX_TYPE_XY  );

	return( false );
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
