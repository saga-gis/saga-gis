
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    shapes_io.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"
#include "table_dbase.h"
#include "tool_library.h"
#include "data_manager.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::On_Reload(void)
{
	return( Create(Get_File_Name(false)) );
}

//---------------------------------------------------------
bool CSG_Shapes::On_Delete(void)
{
	CSG_String	File_Name	= Get_File_Name(true);

	SG_File_Delete(File_Name);

	SG_File_Set_Extension(File_Name, "shp"); SG_File_Delete(File_Name);	// shapes
	SG_File_Set_Extension(File_Name, "shx"); SG_File_Delete(File_Name);	// shape index
	SG_File_Set_Extension(File_Name, "dbf"); SG_File_Delete(File_Name);	// attributes
	SG_File_Set_Extension(File_Name, "prj"); SG_File_Delete(File_Name);	// projection
	SG_File_Set_Extension(File_Name, "sbn"); SG_File_Delete(File_Name);	// spatial index
	SG_File_Set_Extension(File_Name, "sbx"); SG_File_Delete(File_Name);	// spatial index
	SG_File_Set_Extension(File_Name, "atx"); SG_File_Delete(File_Name);	// attribute index
	SG_File_Set_Extension(File_Name, "xml"); SG_File_Delete(File_Name);	// metadata
	SG_File_Set_Extension(File_Name, "cpg"); SG_File_Delete(File_Name);	// code page
	SG_File_Set_Extension(File_Name, "qix"); SG_File_Delete(File_Name);	// quadtree spatial index

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static TSG_Shape_File_Format gSG_Shape_File_Format_Default = SHAPE_FILE_FORMAT_ESRI;

//---------------------------------------------------------
bool					SG_Shapes_Set_File_Format_Default	(int Format)
{
	switch( Format )
	{
	case SHAPE_FILE_FORMAT_ESRI      :
	case SHAPE_FILE_FORMAT_GeoPackage:
	case SHAPE_FILE_FORMAT_GeoJSON   :
		gSG_Shape_File_Format_Default = (TSG_Shape_File_Format)Format;
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
TSG_Shape_File_Format	SG_Shapes_Get_File_Format_Default	(void)
{
	return( gSG_Shape_File_Format_Default );
}

//---------------------------------------------------------
CSG_String				SG_Shapes_Get_File_Extension_Default	(void)
{
	switch( gSG_Shape_File_Format_Default )
	{
	default:
	case SHAPE_FILE_FORMAT_ESRI      :	return( "shp"     );
	case SHAPE_FILE_FORMAT_GeoPackage:	return( "gpkg"    );
	case SHAPE_FILE_FORMAT_GeoJSON   :	return( "geojson" );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::Save(const CSG_String &File, int Format)
{
	if( File.is_Empty() )
	{
		return( *Get_File_Name(false) ? Save(Get_File_Name(false), Format) : false );
	}

	if( Format == SHAPE_FILE_FORMAT_Undefined )
	{
		Format = gSG_Shape_File_Format_Default;

		if( SG_File_Cmp_Extension(File, "shp"    ) ) { Format = SHAPE_FILE_FORMAT_ESRI      ; }
		if( SG_File_Cmp_Extension(File, "gpkg"   ) ) { Format = SHAPE_FILE_FORMAT_GeoPackage; }
		if( SG_File_Cmp_Extension(File, "geojson") ) { Format = SHAPE_FILE_FORMAT_GeoJSON   ; }

		if( SG_File_Cmp_Extension(File, "txt"    ) ) { return( _Save_Text (File, true, '\t') ); }
		if( SG_File_Cmp_Extension(File, "csv"    ) ) { return( _Save_Text (File, true,  ',') ); }
		if( SG_File_Cmp_Extension(File, "dbf"    ) ) { return( _Save_DBase(File)             ); }
	}

	//-----------------------------------------------------
	bool bResult = false;

	SG_UI_Msg_Add(CSG_String::Format("%s %s: %s...", _TL("Saving"), _TL("shapes"), File.c_str()), true);

	switch( Format )
	{
	case SHAPE_FILE_FORMAT_ESRI      : bResult = _Save_ESRI(File           ); break;
	case SHAPE_FILE_FORMAT_GeoPackage: bResult = _Save_GDAL(File, "GPKG"   ); break;
	case SHAPE_FILE_FORMAT_GeoJSON   : bResult = _Save_GDAL(File, "GeoJSON"); break;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(File, true);

		SG_UI_Process_Set_Ready();
		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Process_Set_Ready();
	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::_Load_GDAL(const CSG_String &File_Name)
{
	CSG_Data_Manager Manager;

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 3);	// Import Shapes

	if( pTool )
	{
		if( pTool->Settings_Push(&Manager) && pTool->Set_Parameter("FILES", File_Name, PARAMETER_TYPE_FilePath) )
		{
			SG_UI_Msg_Lock(true);
			pTool->Execute();
			SG_UI_Msg_Lock(false);
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
	}

	//-----------------------------------------------------
	CSG_Shapes *pShapes = Manager.Shapes().Count() ? Manager.Shapes()[0].asShapes() : NULL;

	if( !pShapes || !Create(*pShapes) )
	{
		return( false );
	}

	Get_MetaData  () = pShapes->Get_MetaData  ();
	Get_Projection() = pShapes->Get_Projection();

	//-----------------------------------------------------
	if( SG_File_Cmp_Extension(File_Name, "gpkg"   )
	||  SG_File_Cmp_Extension(File_Name, "GeoJSON")	)
	{
		Set_File_Name(File_Name, true);
	}
	else
	{
		Set_File_Name(File_Name, false);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes::_Save_GDAL(const CSG_String &File_Name, const CSG_String &Driver)
{
	bool bResult;

	SG_UI_Msg_Lock(true);

	SG_RUN_TOOL(bResult, "io_gdal", 4,	// Export Shapes
		    SG_TOOL_PARAMETER_SET("SHAPES", this)
		&&	SG_TOOL_PARAMETER_SET("FORMAT", Driver)
		&&	SG_TOOL_PARAMETER_SET("FILE"  , File_Name)
	);

	SG_UI_Msg_Lock(false);

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::_Load_ESRI(const CSG_String &File_Name)
{
	CSG_File Stream;

	//-----------------------------------------------------
	// Open Encoding File...

	m_Encoding = SG_FILE_ENCODING_ANSI;

	if( Stream.Open(SG_File_Make_Path("", File_Name, "cpg")) )
	{
		CSG_String Line;

		if( Stream.Read_Line(Line) && Line.Find("UTF-8") >= 0 )
		{
			m_Encoding = SG_FILE_ENCODING_UTF8;
		}

		Stream.Close();
	}

	//-----------------------------------------------------
	// Open DBase File...

	CSG_Table_DBase	DBase(m_Encoding);

	if( DBase.Open_Read(SG_File_Make_Path("", File_Name, "dbf"), this, false) && DBase.Get_Field_Count() < 1 )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("[%s] DBase %s.", _TL("Warning"), _TL("file does not provide data.")));

		DBase.Close();
	}

	if( DBase.is_Open() )
	{
		DBase.Move_First();
	}
	else
	{
		Add_Field("FID", SG_DATATYPE_Int);
	}

	//-----------------------------------------------------
	// Open Shapes File...

	if( !Stream.Open(SG_File_Make_Path("", File_Name, "shp"), SG_FILE_R, true) )
	{
		SG_UI_Msg_Add_Error(_TL("Shape file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	// Read File Header (100 Bytes)...

	CSG_Buffer File_Header(100);

	if( Stream.Read(File_Header.Get_Data(), sizeof(char), 100) != 100 )
	{
		SG_UI_Msg_Add_Error(_TL("corrupted file header"));

		return( false );
	}

	if( File_Header.asInt( 0,  true) != 9994 )	// Byte 00 -> File Code 9994 (Integer Big)...
	{
		SG_UI_Msg_Add_Error(_TL("invalid file code"));

		return( false );
	}

	if( File_Header.asInt(28, false) != 1000 )	// Byte 28 -> Version 1000 (Integer Little)...
	{
		SG_UI_Msg_Add_Error(_TL("unsupported file version"));

		return( false );
	}

	int Type = File_Header.asInt(32);

	switch( Type )	// Byte 32 -> Shape Type (Integer Little)...
	{
	case  1: m_Type = SHAPE_TYPE_Point  ; m_Vertex_Type = SG_VERTEX_TYPE_XY  ; break; // Point
	case  8: m_Type = SHAPE_TYPE_Points ; m_Vertex_Type = SG_VERTEX_TYPE_XY  ; break; // MultiPoint
	case  3: m_Type = SHAPE_TYPE_Line   ; m_Vertex_Type = SG_VERTEX_TYPE_XY  ; break; // PolyLine
	case  5: m_Type = SHAPE_TYPE_Polygon; m_Vertex_Type = SG_VERTEX_TYPE_XY  ; break; // Polygon

	case 11: m_Type = SHAPE_TYPE_Point  ; m_Vertex_Type = SG_VERTEX_TYPE_XYZM; break; // PointZ
	case 18: m_Type = SHAPE_TYPE_Points ; m_Vertex_Type = SG_VERTEX_TYPE_XYZM; break; // MultiPointZ
	case 13: m_Type = SHAPE_TYPE_Line   ; m_Vertex_Type = SG_VERTEX_TYPE_XYZM; break; // PolyLineZ
	case 15: m_Type = SHAPE_TYPE_Polygon; m_Vertex_Type = SG_VERTEX_TYPE_XYZM; break; // PolygonZ

	case 21: m_Type = SHAPE_TYPE_Point  ; m_Vertex_Type = SG_VERTEX_TYPE_XYZ ; break; // PointM
	case 28: m_Type = SHAPE_TYPE_Points ; m_Vertex_Type = SG_VERTEX_TYPE_XYZ ; break; // MultiPointM
	case 23: m_Type = SHAPE_TYPE_Line   ; m_Vertex_Type = SG_VERTEX_TYPE_XYZ ; break; // PolyLineM
	case 25: m_Type = SHAPE_TYPE_Polygon; m_Vertex_Type = SG_VERTEX_TYPE_XYZ ; break; // PolygonM

	case 31: // unsupported: MultiPatch...
	default: // unsupported...
		SG_UI_Msg_Add_Error(_TL("unsupported shape type."));
		return( false );
	}

	//-----------------------------------------------------
	// Load Shapes...

	CSG_Buffer Record_Header(8), Content; sLong Length = Stream.Length();

	for(int iShape=0; Stream.Tell() < Length && SG_UI_Process_Set_Progress(Stream.Tell(), Length); iShape++)
	{
		if( Stream.Read(Record_Header.Get_Data(0), sizeof(int), 2) != 2 ) // read record header
		{
			SG_UI_Msg_Add_Error(_TL("corrupted record header"));

			return( false );
		}

		if( Record_Header.asInt(0, true) != iShape + 1 ) // record number
		{
			SG_UI_Msg_Add_Error(CSG_String::Format("%s (%d != %d)", _TL("corrupted shapefile."), Record_Header.asInt(0, true), iShape + 1));

			return( false );
		}

		size_t nBytes = 2 * (size_t)Record_Header.asInt(4, true); // content length as 16-bit words !!!

		if( !Content.Set_Size(nBytes, false) )
		{
			SG_UI_Msg_Add_Error(_TL("memory allocation error."));

			return( false );
		}

		if( Stream.Read(Content.Get_Data(), sizeof(char), nBytes) != nBytes )
		{
			SG_UI_Msg_Add_Error(_TL("corrupted shapefile."));

			return( false );
		}

		if( DBase.is_Open() && DBase.isDeleted() )
		{
			continue; // nop
		}

		if( Content.asInt(0) != Type )
		{
			if( Content.asInt(0) == 0 )
			{
				// null shape is allowed !!!
			}
			else
			{
				SG_UI_Msg_Add_Error(_TL("corrupted shapefile."));

				return( false );
			}
		}

		//-------------------------------------------------
		else
		{
			CSG_Shape *pShape = Add_Shape(); TSG_Point *pPoint; int nPoints, nParts, *Parts; double *pZ = NULL, *pM = NULL;

			switch( m_Type )
			{
			//---------------------------------------------
			case SHAPE_TYPE_Point: ////////////////////////

				pPoint = (TSG_Point *)Content.Get_Data(4);

				pShape->Add_Point(pPoint->x, pPoint->y);

				switch( m_Vertex_Type )	// read Z + M
				{
				default: break;
				case SG_VERTEX_TYPE_XYZM: pShape->Set_M(Content.asDouble(28), 0);
				case SG_VERTEX_TYPE_XYZ : pShape->Set_Z(Content.asDouble(20), 0);
				}

				break;

			//---------------------------------------------
			case SHAPE_TYPE_Points: ///////////////////////

				nPoints = Content.asInt(36);
				pPoint  = (TSG_Point *)Content.Get_Data(40);

				switch( m_Vertex_Type )	// read Z + M
				{
				default: break;

				case SG_VERTEX_TYPE_XYZ:
					pZ = 56 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(56 + nPoints * 16) : NULL; // [40 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					break;

				case SG_VERTEX_TYPE_XYZM:
					pZ = 56 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(56 + nPoints * 16) : NULL; // [40 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					pM = 72 + nPoints * 32 <= (int)Length ? (double *)Content.Get_Data(72 + nPoints * 24) : NULL; // [40 + nPoints * 16 + 2 * 8] + [nPoints * 8 + 2 * 8] + [nPoints * 8]
					break;
				}

				//-----------------------------------------
				for(int iPoint=0; iPoint<nPoints; iPoint++, pPoint++)
				{
					pShape->Add_Point(pPoint->x, pPoint->y);

					if( pZ ) { pShape->Set_Z(*(pZ++), iPoint); }
					if( pM ) { pShape->Set_M(*(pM++), iPoint); }
				}

				break;

			//---------------------------------------------
			case SHAPE_TYPE_Line   : //////////////////////
			case SHAPE_TYPE_Polygon: //////////////////////

				nParts  = Content.asInt(36);
				nPoints = Content.asInt(40);
				Parts   = (int       *)Content.Get_Data(44);
				pPoint  = (TSG_Point *)Content.Get_Data(44 + 4 * nParts);

				switch( m_Vertex_Type )	// read Z + M
				{
				default: break;

				case SG_VERTEX_TYPE_XYZ:
					pZ	= 60 + nParts * 4 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(60 + nParts * 4 + nPoints * 16) : NULL;	// [44 + nParts * 4 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					break;

				case SG_VERTEX_TYPE_XYZM:
					pZ	= 60 + nParts * 4 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(60 + nParts * 4 + nPoints * 16) : NULL;	// [44 + nParts * 4 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					pM	= 76 + nParts * 4 + nPoints * 32 <= (int)Length ? (double *)Content.Get_Data(76 + nParts * 4 + nPoints * 24) : NULL;	// [44 + nParts * 4 + nPoints * 16 + 2 * 8] + [nPoints * 8 + 2 * 8] +  [nPoints * 8]
					break;
				}

				//-----------------------------------------
				for(int iPoint=0, iPart=0, Offset=0; iPoint<nPoints; iPoint++, pPoint++, Offset++)
				{
					if( iPart < nParts - 1 && iPoint >= Parts[iPart + 1] )
					{
						iPart++; Offset = 0;
					}

					pShape->Add_Point(pPoint->x, pPoint->y, iPart);

					if( pZ ) { pShape->Set_Z(*(pZ++), Offset, iPart); }
					if( pM ) { pShape->Set_M(*(pM++), Offset, iPart); }
				}

				break;

			//---------------------------------------------
			default: break;
			}

			//---------------------------------------------
			if( DBase.is_Open() )
			{
				for(int iField=0; iField<Get_Field_Count(); iField++)
				{
					switch( DBase.Get_Field_Type(iField) )
					{
					default:
						pShape->Set_Value(iField, DBase.asString(iField));
						break;

					case DBF_FT_FLOAT: case DBF_FT_NUMERIC:
						{
							double Value;

							if( DBase.asDouble(iField, Value) )
							{
								pShape->Set_Value(iField, Value);
							}
							else
							{
								pShape->Set_NoData(iField);
							}
						}
					break;
					}
				}

				DBase.Move_Next();
			}
			else
			{
				pShape->Set_Value(0, iShape);
			}
		}
	}

	//-----------------------------------------------------
	Get_Projection().Load(SG_File_Make_Path("", File_Name, "prj"));

	Load_MetaData(File_Name);

	CSG_MetaData *pFields = Get_MetaData_DB().Get_Child("FIELDS");

	if( pFields && pFields->Get_Children_Count() == Get_Field_Count() )
	{
		for(int iField=0; iField<Get_Field_Count(); iField++)
		{
			Set_Field_Name(iField, pFields->Get_Content(iField));
		}
	}

	Set_File_Name(File_Name, true);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define Set_Content_Length(n)	Record_Header.Set_Value(4, (int)(n), true);\
	fSHP.Write(Record_Header.Get_Data(), sizeof(int), 2);\
	fSHX.Write_Int(fSHP_Size, true);\
	fSHX.Write_Int((n)      , true);\
	fSHP_Size	+= 4 + (n);\
	fSHX_Size	+= 4;

//---------------------------------------------------------
bool CSG_Shapes::_Save_ESRI(const CSG_String &File_Name)
{
	//-----------------------------------------------------
	// Determine Shape Type...

	int Type;

	switch( m_Type )
	{
	case SHAPE_TYPE_Point   : Type  =  1; break;
	case SHAPE_TYPE_Points  : Type  =  8; break;
	case SHAPE_TYPE_Line    : Type  =  3; break;
	case SHAPE_TYPE_Polygon : Type  =  5; break;
	default: return( false );
	}

	TSG_Vertex_Type	Vertex_Type	= m_Vertex_Type == 0 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZM;

	switch( Vertex_Type )
	{
	case SG_VERTEX_TYPE_XY  :             break;
	case SG_VERTEX_TYPE_XYZ : Type += 20; break; // M
	case SG_VERTEX_TYPE_XYZM: Type += 10; break; // Z (+M)
	default: return( false );
	}

	//-----------------------------------------------------
	// DBase File Access...

	SG_File_Delete(SG_File_Make_Path("", File_Name, "cpg"));

	if( m_Encoding == SG_FILE_ENCODING_UTF8 )
	{
		CSG_File Stream;

		if( Stream.Open(SG_File_Make_Path("", File_Name, "cpg"), SG_FILE_W, false) )
		{
			Stream.Printf("UTF-8\n");

			Stream.Close();
		}
	}

	CSG_Table_DBase DBase(m_Encoding);

	if( !DBase.Open_Write(SG_File_Make_Path("", File_Name, "dbf"), this, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	// Shape File Access...

	CSG_File fSHP, fSHX;

	if( !fSHX.Open(SG_File_Make_Path("", File_Name, "shx"), SG_FILE_W, true) )
	{
		SG_UI_Msg_Add_Error(_TL("could not create index file"));

		return( false );
	}

	if( !fSHP.Open(SG_File_Make_Path("", File_Name, "shp"), SG_FILE_W, true) )
	{
		SG_UI_Msg_Add_Error(_TL("could not create shape file"));

		return( false );
	}

	//-----------------------------------------------------
	// Save Header...

	Make_Clean();	// polygons: first == last point, inner rings > anti-clockwise...

	Update();

	CSG_Buffer File_Header(100);

	File_Header.Set_Value( 0, 9994               , true ); // Byte  0  Integer Big     File Code = 9994
	File_Header.Set_Value( 4, 0                  , true ); // Byte  4  Integer Big     unused
	File_Header.Set_Value( 8, 0                  , true ); // Byte  8  Integer Big     unused
	File_Header.Set_Value(12, 0                  , true ); // Byte 12  Integer Big     unused
	File_Header.Set_Value(16, 0                  , true ); // Byte 16  Integer Big     unused
	File_Header.Set_Value(20, 0                  , true ); // Byte 20  Integer Big     unused
	File_Header.Set_Value(24, 0                  , true ); // Byte 24  Integer Big     File Length
	File_Header.Set_Value(28, 1000               , false); // Byte 28  Integer Little  Version   = 1000
	File_Header.Set_Value(32, Type               , false); // Byte 32  Integer Little  Shape Type
	File_Header.Set_Value(36, m_Extent.Get_XMin(), false); // Byte 36  Double  Little  Bounding Box Xmin
	File_Header.Set_Value(44, m_Extent.Get_YMin(), false); // Byte 44  Double  Little  Bounding Box Ymin
	File_Header.Set_Value(52, m_Extent.Get_XMax(), false); // Byte 52  Double  Little  Bounding Box Xmax
	File_Header.Set_Value(60, m_Extent.Get_YMax(), false); // Byte 60  Double  Little  Bounding Box Ymax
	File_Header.Set_Value(68,          Get_ZMin(), false); // Byte 68  Double  Little  Bounding Box Zmin
	File_Header.Set_Value(76,          Get_ZMax(), false); // Byte 76  Double  Little  Bounding Box Zmax
	File_Header.Set_Value(84,          Get_MMin(), false); // Byte 84  Double  Little  Bounding Box Mmin
	File_Header.Set_Value(92,          Get_MMax(), false); // Byte 92  Double  Little  Bounding Box Mmax

	fSHP.Write(File_Header.Get_Data(), sizeof(char), 100);
	fSHX.Write(File_Header.Get_Data(), sizeof(char), 100);

	int fSHP_Size = 50; // file size measured in 16-bit words...
	int fSHX_Size = 50; // file size measured in 16-bit words...

	//-----------------------------------------------------
	// Save Shapes...

	for(sLong iShape=0; iShape<Get_Count() && SG_UI_Process_Set_Progress(iShape, Get_Count()); iShape++)
	{
		CSG_Shape *pShape = Get_Shape(iShape);

		//-------------------------------------------------
		// geometries...

		CSG_Buffer Record_Header(8), Content;

		Record_Header.Set_Value(0, (int)iShape + 1, true);	// record number

		//-------------------------------------------------
		switch( m_Type )			// write content header
		{
		default: break;

		//-------------------------------------------------
		case SHAPE_TYPE_Point:		///////////////////////

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY  : Set_Content_Length(10); break;
			case SG_VERTEX_TYPE_XYZ : Set_Content_Length(14); break;
			case SG_VERTEX_TYPE_XYZM: Set_Content_Length(18); break;
			}

			fSHP.Write_Int   (Type);

			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Points:		///////////////////////

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY  : Set_Content_Length(20 +  8 * pShape->Get_Point_Count()); break;
			case SG_VERTEX_TYPE_XYZ : Set_Content_Length(28 + 12 * pShape->Get_Point_Count()); break;
			case SG_VERTEX_TYPE_XYZM: Set_Content_Length(36 + 16 * pShape->Get_Point_Count()); break;
			}

			fSHP.Write_Int   (Type);
			fSHP.Write_Double(pShape->Get_Extent().Get_XMin());
			fSHP.Write_Double(pShape->Get_Extent().Get_YMin());
			fSHP.Write_Double(pShape->Get_Extent().Get_XMax());
			fSHP.Write_Double(pShape->Get_Extent().Get_YMax());
			fSHP.Write_Int   (pShape->Get_Point_Count());

			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Line:		///////////////////////
		case SHAPE_TYPE_Polygon:	///////////////////////

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY  : Set_Content_Length(22 + 2 * pShape->Get_Part_Count() +  8 * pShape->Get_Point_Count()); break;
			case SG_VERTEX_TYPE_XYZ : Set_Content_Length(30 + 2 * pShape->Get_Part_Count() + 12 * pShape->Get_Point_Count()); break;
			case SG_VERTEX_TYPE_XYZM: Set_Content_Length(38 + 2 * pShape->Get_Part_Count() + 16 * pShape->Get_Point_Count()); break;
			}

			fSHP.Write_Int   (Type);
			fSHP.Write_Double(pShape->Get_Extent().Get_XMin());
			fSHP.Write_Double(pShape->Get_Extent().Get_YMin());
			fSHP.Write_Double(pShape->Get_Extent().Get_XMax());
			fSHP.Write_Double(pShape->Get_Extent().Get_YMax());
			fSHP.Write_Int   (pShape->Get_Part_Count());
			fSHP.Write_Int   (pShape->Get_Point_Count());

			for(int iPart=0, iPoint=0; iPart<pShape->Get_Part_Count(); iPoint+=pShape->Get_Point_Count(iPart++))
			{
				fSHP.Write_Int(iPoint);
			}

			break;
		}

		//-------------------------------------------------
		TSG_Point Point;

		switch( m_Type ) // write point data
		{
		default: break;

		//-------------------------------------------------
		case SHAPE_TYPE_Point  : //////////////////////////

			fSHP.Write(&(Point = pShape->Get_Point()), sizeof(TSG_Point));

			//---------------------------------------------
			if( Vertex_Type != SG_VERTEX_TYPE_XY )
			{
				fSHP.Write_Double(pShape->Get_Z(0));

				if( Vertex_Type == SG_VERTEX_TYPE_XYZM )
				{
					fSHP.Write_Double(pShape->Get_M(0));
				}
			}

			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Points : //////////////////////////
		case SHAPE_TYPE_Line   : //////////////////////////
		case SHAPE_TYPE_Polygon: //////////////////////////

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					fSHP.Write(&(Point = pShape->Get_Point(iPoint, iPart)), sizeof(TSG_Point));
				}
			}

			//---------------------------------------------
			if( Vertex_Type != SG_VERTEX_TYPE_XY )
			{
				fSHP.Write_Double(pShape->Get_ZMin());
				fSHP.Write_Double(pShape->Get_ZMax());

				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						fSHP.Write_Double(pShape->Get_Z(iPoint, iPart));
					}
				}

				if( Vertex_Type == SG_VERTEX_TYPE_XYZM )
				{
					fSHP.Write_Double(pShape->Get_MMin());
					fSHP.Write_Double(pShape->Get_MMax());

					for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
					{
						for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
						{
							fSHP.Write_Double(pShape->Get_M(iPoint, iPart));
						}
					}
				}
			}
		}

		//-------------------------------------------------
		// attributes...

		DBase.Add_Record();

		for(int iField=0; iField<Get_Field_Count(); iField++)
		{
			if( pShape->is_NoData(iField) )
			{
				DBase.Set_NoData(iField);
			}
			else switch( DBase.Get_Field_Type(iField) )
			{
			default:
				DBase.Set_Value(iField, pShape->asString(iField));
				break;

			case DBF_FT_FLOAT: case DBF_FT_NUMERIC:
				DBase.Set_Value(iField, pShape->asDouble(iField));
				break;
			}
		}

		DBase.Flush_Record();
	}

	//-----------------------------------------------------
	// File Sizes...

	fSHP.Seek(24);
	fSHP.Write_Int(fSHP_Size, true);

	fSHX.Seek(24);
	fSHX.Write_Int(fSHX_Size, true);

	//-----------------------------------------------------
	Get_Projection().Save(SG_File_Make_Path("", File_Name, "prj"));

	//-----------------------------------------------------
	CSG_MetaData *pFields = Get_MetaData_DB().Get_Child("FIELDS");

	if( !pFields )
	{
		pFields = Get_MetaData_DB().Add_Child("FIELDS");
	}

	pFields->Del_Children();

	for(int iField=0; iField<Get_Field_Count(); iField++)
	{
		pFields->Add_Child("FIELD", Get_Field_Name(iField))->Add_Property("TYPE", gSG_Data_Type_Identifier[Get_Field_Type(iField)]);
	}

	Get_MetaData().Del_Child("GDAL_DRIVER");

	Save_MetaData(File_Name);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
