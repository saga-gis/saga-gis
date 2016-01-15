/**********************************************************
 * Version $Id$
 *********************************************************/

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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"

#include "table_dbase.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::_Load_ESRI(const CSG_String &File_Name)
{
	int				Type, iField, iPart, nParts, *Parts, iPoint, nPoints, iOffset;
	double			*pZ	= NULL, *pM	= NULL;
	TSG_Point		*pPoint;
	CSG_Buffer		File_Header(100), Record_Header(8), Content;
	CSG_File		fSHP;
	CSG_Table_DBase	fDBF;

	//-----------------------------------------------------
	// Open DBase File...

	if( !fDBF.Open_Read(SG_File_Make_Path(NULL, File_Name, SG_T("dbf")), this, false) )
	{
		SG_UI_Msg_Add_Error(_TL("DBase file could not be opened."));

		return( false );
	}

	if( !fDBF.Move_First() || fDBF.Get_Record_Count() <= 0 )
	{
		SG_UI_Msg_Add_Error(_TL("DBase file does not contain any records."));

		return( false );
	}

	//-----------------------------------------------------
	// Open Shapes File...

	if( !fSHP.Open(SG_File_Make_Path(NULL, File_Name, SG_T("shp")), SG_FILE_R, true) )
	{
		SG_UI_Msg_Add_Error(_TL("Shape file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	// Read File Header (100 Bytes)...

	if( fSHP.Read(File_Header.Get_Data(), sizeof(char), 100) != 100 )
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

	switch( Type = File_Header.asInt(32) )	// Byte 32 -> Shape Type (Integer Little)...
	{
	case 1:		m_Type	= SHAPE_TYPE_Point;		m_Vertex_Type	= SG_VERTEX_TYPE_XY;	break;	// Point
	case 8:		m_Type	= SHAPE_TYPE_Points;	m_Vertex_Type	= SG_VERTEX_TYPE_XY;	break;	// MultiPoint
	case 3:		m_Type	= SHAPE_TYPE_Line;		m_Vertex_Type	= SG_VERTEX_TYPE_XY;	break;	// PolyLine
	case 5:		m_Type	= SHAPE_TYPE_Polygon;	m_Vertex_Type	= SG_VERTEX_TYPE_XY;	break;	// Polygon

	case 11:	m_Type	= SHAPE_TYPE_Point;		m_Vertex_Type	= SG_VERTEX_TYPE_XYZM;	break;	// PointZ
	case 18:	m_Type	= SHAPE_TYPE_Points;	m_Vertex_Type	= SG_VERTEX_TYPE_XYZM;	break;	// MultiPointZ
	case 13:	m_Type	= SHAPE_TYPE_Line;		m_Vertex_Type	= SG_VERTEX_TYPE_XYZM;	break;	// PolyLineZ
	case 15:	m_Type	= SHAPE_TYPE_Polygon;	m_Vertex_Type	= SG_VERTEX_TYPE_XYZM;	break;	// PolygonZ

	case 21:	m_Type	= SHAPE_TYPE_Point;		m_Vertex_Type	= SG_VERTEX_TYPE_XYZ;	break;	// PointM
	case 28:	m_Type	= SHAPE_TYPE_Points;	m_Vertex_Type	= SG_VERTEX_TYPE_XYZ;	break;	// MultiPointM
	case 23:	m_Type	= SHAPE_TYPE_Line;		m_Vertex_Type	= SG_VERTEX_TYPE_XYZ;	break;	// PolyLineM
	case 25:	m_Type	= SHAPE_TYPE_Polygon;	m_Vertex_Type	= SG_VERTEX_TYPE_XYZ;	break;	// PolygonM

	default:	// unsupported...
	case 31:	// unsupported: MultiPatch...
		SG_UI_Msg_Add_Error(_TL("unsupported shape type."));

		return( false );
	}

	//-----------------------------------------------------
	// Load Shapes...

	for(int iShape=0; iShape<fDBF.Get_Record_Count() && SG_UI_Process_Set_Progress(iShape, fDBF.Get_Record_Count()); iShape++)
	{
		if( fSHP.Read(Record_Header.Get_Data(0), sizeof(int), 2) != 2 )		// read record header
		{
			SG_UI_Msg_Add_Error(_TL("corrupted record header"));

			return( false );
		}

		if( Record_Header.asInt(0, true) != iShape + 1 )					// record number
		{
			SG_UI_Msg_Add_Error(_TL("corrupted shapefile."));

			return( false );
		}

		size_t	Length	= 2 * Record_Header.asInt(4, true);	// content length as 16-bit words !!!

		if( !Content.Set_Size(Length, false) )
		{
			SG_UI_Msg_Add_Error(_TL("memory allocation error."));

			return( false );
		}

		if( fSHP.Read(Content.Get_Data(), sizeof(char), Length) != Length )
		{
			SG_UI_Msg_Add_Error(_TL("corrupted shapefile."));

			return( false );
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
			CSG_Shape	*pShape	= Add_Shape();

			switch( m_Type )
			{
			default:	break;

			//---------------------------------------------
			case SHAPE_TYPE_Point: ////////////////////////

				pPoint	= (TSG_Point *)Content.Get_Data(4);

				pShape->Add_Point(pPoint->x, pPoint->y);

				switch( m_Vertex_Type )	// read Z + M
				{
				case SG_VERTEX_TYPE_XYZM:	pShape->Set_M(Content.asDouble(28), 0);
				case SG_VERTEX_TYPE_XYZ:	pShape->Set_Z(Content.asDouble(20), 0);
				default:	break;
				}

				break;

			//---------------------------------------------
			case SHAPE_TYPE_Points: ///////////////////////

				nPoints	= Content.asInt(36);
				pPoint	= (TSG_Point *)Content.Get_Data(40);

				switch( m_Vertex_Type )	// read Z + M
				{
				case SG_VERTEX_TYPE_XYZ:
					pZ	= 56 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(56 + nPoints * 16) : NULL;	// [40 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					break;

				case SG_VERTEX_TYPE_XYZM:
					pZ	= 56 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(56 + nPoints * 16) : NULL;	// [40 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					pM	= 72 + nPoints * 32 <= (int)Length ? (double *)Content.Get_Data(72 + nPoints * 24) : NULL;	// [40 + nPoints * 16 + 2 * 8] + [nPoints * 8 + 2 * 8] + [nPoints * 8]
					break;
				default:	
					break;
				}

				//-----------------------------------------
				for(iPoint=0; iPoint<nPoints; iPoint++, pPoint++)
				{
					pShape->Add_Point(pPoint->x, pPoint->y);

					if( pZ )	{	pShape->Set_Z(*(pZ++), iPoint);	}
					if( pM )	{	pShape->Set_M(*(pM++), iPoint);	}
				}

				break;

			//---------------------------------------------
			case SHAPE_TYPE_Line:    //////////////////////
			case SHAPE_TYPE_Polygon: //////////////////////

				nParts	= Content.asInt(36);
				nPoints	= Content.asInt(40);
				Parts	= (int       *)Content.Get_Data(44);
				pPoint	= (TSG_Point *)Content.Get_Data(44 + 4 * nParts);

				switch( m_Vertex_Type )	// read Z + M
				{
				case SG_VERTEX_TYPE_XYZ:
					pZ	= 60 + nParts * 4 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(60 + nParts * 4 + nPoints * 16) : NULL;	// [44 + nParts * 4 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					break;

				case SG_VERTEX_TYPE_XYZM:
					pZ	= 60 + nParts * 4 + nPoints * 24 <= (int)Length ? (double *)Content.Get_Data(60 + nParts * 4 + nPoints * 16) : NULL;	// [44 + nParts * 4 + nPoints * 16 + 2 * 8] + [nPoints * 8]
					pM	= 76 + nParts * 4 + nPoints * 32 <= (int)Length ? (double *)Content.Get_Data(76 + nParts * 4 + nPoints * 24) : NULL;	// [44 + nParts * 4 + nPoints * 16 + 2 * 8] + [nPoints * 8 + 2 * 8] +  [nPoints * 8]
					break;
				default:
					break;
				}


				//-----------------------------------------
				iOffset = 0;

				for(iPoint=0, iPart=0; iPoint<nPoints; iPoint++, pPoint++)
				{
					if( iPart < nParts - 1 && iPoint >= Parts[iPart + 1] )
					{
						iPart++;
						iOffset = 0;
					}

					pShape->Add_Point(pPoint->x, pPoint->y, iPart);

					if( pZ )	{	pShape->Set_Z(*(pZ++), iOffset, iPart);	}
					if( pM )	{	pShape->Set_M(*(pM++), iOffset, iPart);	}

					iOffset++;
				}

				break;
			}

			//---------------------------------------------
			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				switch( fDBF.Get_Field_Type(iField) )
				{
				default:
					pShape->Set_Value(iField, fDBF.asString(iField));
					break;

				case DBF_FT_FLOAT:
				case DBF_FT_NUMERIC:
					{
						double	Value;

						if( fDBF.asDouble(iField, Value) )
							pShape->Set_Value(iField, Value);
						else
							pShape->Set_NoData(iField);
					}
					break;
				}
			}
		}

		fDBF.Move_Next();
	}

	//-----------------------------------------------------
	Get_Projection().Load(SG_File_Make_Path(NULL, File_Name, SG_T("prj")), SG_PROJ_FMT_WKT);

	//-----------------------------------------------------
	Load_MetaData(File_Name);

	CSG_MetaData	*pFields	= Get_MetaData_DB().Get_Child("FIELDS");

	if( pFields && pFields->Get_Children_Count() == Get_Field_Count() )
	{
		for(iField=0; iField<Get_Field_Count(); iField++)
		{
			Set_Field_Name(iField, pFields->Get_Content(iField));
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
#define Set_Content_Length(n)	Record_Header.Set_Value(4, (int)(n), true);\
								fSHP.Write(Record_Header.Get_Data(), sizeof(int), 2);\
								fSHX.Write_Int(fSHP_Size, true);\
								fSHX.Write_Int((n)      , true);\
								fSHP_Size	+= 4 + (n);\
								fSHX_Size	+= 4;

//---------------------------------------------------------
bool CSG_Shapes::_Save_ESRI(const CSG_String &File_Name)
{
	int				Type, fSHP_Size, fSHX_Size, iField, iPart, iPoint, nPoints;
	TSG_Point		Point;
	CSG_Buffer		File_Header(100), Record_Header(8), Content;
	CSG_File		fSHP, fSHX;
	CSG_Table_DBase	fDBF;

	//-----------------------------------------------------
	// Determine Shape Type...

	switch( m_Type )
	{
	case SHAPE_TYPE_Point:		Type	=  1;	break;
	case SHAPE_TYPE_Points:		Type	=  8;	break;
	case SHAPE_TYPE_Line:		Type	=  3;	break;
	case SHAPE_TYPE_Polygon:	Type	=  5;	break;
	default:	return( false );
	}

	TSG_Vertex_Type	Vertex_Type	= m_Vertex_Type == 0 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZM;

	switch( Vertex_Type )
	{
	case SG_VERTEX_TYPE_XY:						break;
	case SG_VERTEX_TYPE_XYZ:	Type	+= 20;	break;	// M
	case SG_VERTEX_TYPE_XYZM:	Type	+= 10;	break;	// Z (+M)
	default:	return( false );
	}

	//-----------------------------------------------------
	// DBase File Access...

	if( !fDBF.Open_Write(SG_File_Make_Path(NULL, File_Name, SG_T("dbf")), this, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	// Shape File Access...

	if( !fSHX.Open(SG_File_Make_Path(NULL, File_Name, SG_T("shx")), SG_FILE_W, true) )
	{
		SG_UI_Msg_Add_Error(_TL("index file could not be opened"));

		return( false );
	}

	if( !fSHP.Open(SG_File_Make_Path(NULL, File_Name, SG_T("shp")), SG_FILE_W, true) )
	{
		SG_UI_Msg_Add_Error(_TL("shape file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	// Save Header...

	Make_Clean();	// polygons: first == last point, inner rings > anti-clockwise...

	Update();

	File_Header.Set_Value( 0, 9994					, true );	// Byte  0  Integer Big     File Code = 9994
	File_Header.Set_Value( 4, 0						, true );	// Byte  4  Integer Big     unused
	File_Header.Set_Value( 8, 0						, true );	// Byte  8  Integer Big     unused
	File_Header.Set_Value(12, 0						, true );	// Byte 12  Integer Big     unused
	File_Header.Set_Value(16, 0						, true );	// Byte 16  Integer Big     unused
	File_Header.Set_Value(20, 0						, true );	// Byte 20  Integer Big     unused
	File_Header.Set_Value(24, 0						, true );	// Byte 24  Integer Big     File Length
	File_Header.Set_Value(28, 1000					, false);	// Byte 28  Integer Little  Version   = 1000
	File_Header.Set_Value(32, Type					, false);	// Byte 32  Integer Little  Shape Type
	File_Header.Set_Value(36, m_Extent.Get_XMin()	, false);	// Byte 36  Double  Little  Bounding Box Xmin
	File_Header.Set_Value(44, m_Extent.Get_YMin()	, false);	// Byte 44  Double  Little  Bounding Box Ymin
	File_Header.Set_Value(52, m_Extent.Get_XMax()	, false);	// Byte 52  Double  Little  Bounding Box Xmax
	File_Header.Set_Value(60, m_Extent.Get_YMax()	, false);	// Byte 60  Double  Little  Bounding Box Ymax
	File_Header.Set_Value(68,          Get_ZMin()	, false);	// Byte 68  Double  Little  Bounding Box Zmin
	File_Header.Set_Value(76,          Get_ZMax()	, false);	// Byte 76  Double  Little  Bounding Box Zmax
	File_Header.Set_Value(84,          Get_MMin()	, false);	// Byte 84  Double  Little  Bounding Box Mmin
	File_Header.Set_Value(92,          Get_MMax()	, false);	// Byte 92  Double  Little  Bounding Box Mmax

	fSHP.Write(File_Header.Get_Data(), sizeof(char), 100);
	fSHX.Write(File_Header.Get_Data(), sizeof(char), 100);

	fSHP_Size	= 50;	// file size measured in 16-bit words...
	fSHX_Size	= 50;	// file size measured in 16-bit words...

	//-----------------------------------------------------
	// Save Shapes...

	for(int iShape=0; iShape<Get_Count() && SG_UI_Process_Set_Progress(iShape, Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= Get_Shape(iShape);

		//-------------------------------------------------
		// geometries...

		Record_Header.Set_Value(0, iShape + 1, true);	// record number

		for(iPart=0, nPoints=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			nPoints	+= pShape->Get_Point_Count(iPart);	// total number of points in shape
		}

		//-------------------------------------------------
		switch( m_Type )			// write content header
		{
		default:	break;

		//-------------------------------------------------
		case SHAPE_TYPE_Point:		///////////////////////

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY:		Set_Content_Length(10);	break;
			case SG_VERTEX_TYPE_XYZ:	Set_Content_Length(14);	break;
			case SG_VERTEX_TYPE_XYZM:	Set_Content_Length(18);	break;
			}

			fSHP.Write_Int		(Type);

			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Points:		///////////////////////

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY:		Set_Content_Length(20 +  8 * nPoints);	break;
			case SG_VERTEX_TYPE_XYZ:	Set_Content_Length(28 + 12 * nPoints);	break;
			case SG_VERTEX_TYPE_XYZM:	Set_Content_Length(36 + 16 * nPoints);	break;
			}

			fSHP.Write_Int		(Type);
			fSHP.Write_Double	(pShape->Get_Extent().Get_XMin());
			fSHP.Write_Double	(pShape->Get_Extent().Get_YMin());
			fSHP.Write_Double	(pShape->Get_Extent().Get_XMax());
			fSHP.Write_Double	(pShape->Get_Extent().Get_YMax());
			fSHP.Write_Int		(nPoints);

			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Line:		///////////////////////
		case SHAPE_TYPE_Polygon:	///////////////////////

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY:		Set_Content_Length(22 + 2 * pShape->Get_Part_Count() +  8 * nPoints);	break;
			case SG_VERTEX_TYPE_XYZ:	Set_Content_Length(30 + 2 * pShape->Get_Part_Count() + 12 * nPoints);	break;
			case SG_VERTEX_TYPE_XYZM:	Set_Content_Length(38 + 2 * pShape->Get_Part_Count() + 16 * nPoints);	break;
			}

			fSHP.Write_Int		(Type);
			fSHP.Write_Double	(pShape->Get_Extent().Get_XMin());
			fSHP.Write_Double	(pShape->Get_Extent().Get_YMin());
			fSHP.Write_Double	(pShape->Get_Extent().Get_XMax());
			fSHP.Write_Double	(pShape->Get_Extent().Get_YMax());
			fSHP.Write_Int		(pShape->Get_Part_Count());
			fSHP.Write_Int		(nPoints);

			for(iPart=0, iPoint=0; iPart<pShape->Get_Part_Count(); iPoint+=pShape->Get_Point_Count(iPart++))
			{
				fSHP.Write_Int(iPoint);
			}

			break;
		}

		//-------------------------------------------------
		switch( m_Type )			// write point data
		{
		default:	break;

		//-------------------------------------------------
		case SHAPE_TYPE_Point:		///////////////////////

			fSHP.Write(&(Point = pShape->Get_Point(0)), sizeof(TSG_Point));

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
		case SHAPE_TYPE_Points:		///////////////////////
		case SHAPE_TYPE_Line:		///////////////////////
		case SHAPE_TYPE_Polygon:	///////////////////////

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					fSHP.Write(&(Point = pShape->Get_Point(iPoint, iPart)), sizeof(TSG_Point));
				}
			}

			//---------------------------------------------
			if( Vertex_Type != SG_VERTEX_TYPE_XY )
			{
				fSHP.Write_Double(pShape->Get_ZMin());
				fSHP.Write_Double(pShape->Get_ZMax());

				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						fSHP.Write_Double(pShape->Get_Z(iPoint, iPart));
					}
				}

				if( Vertex_Type == SG_VERTEX_TYPE_XYZM )
				{
					fSHP.Write_Double(pShape->Get_MMin());
					fSHP.Write_Double(pShape->Get_MMax());

					for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
					{
						for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
						{
							fSHP.Write_Double(pShape->Get_M(iPoint, iPart));
						}
					}
				}
			}
		}

		//-------------------------------------------------
		// attributes...

		fDBF.Add_Record();

		for(iField=0; iField<Get_Field_Count(); iField++)
		{
			if( pShape->is_NoData(iField) )
			{
				fDBF.Set_NoData(iField);
			}
			else switch( fDBF.Get_Field_Type(iField) )
			{
			default:
				fDBF.Set_Value(iField, CSG_String(pShape->asString(iField)));
				break;

			case DBF_FT_FLOAT:
			case DBF_FT_NUMERIC:
				fDBF.Set_Value(iField, pShape->asDouble(iField));
				break;
			}
		}

		fDBF.Flush_Record();
	}

	//-----------------------------------------------------
	// File Sizes...

	fSHP.Seek(24);
	fSHP.Write_Int(fSHP_Size, true);

	fSHX.Seek(24);
	fSHX.Write_Int(fSHX_Size, true);

	//-----------------------------------------------------
	Get_Projection().Save(SG_File_Make_Path(NULL, File_Name, SG_T("prj")), SG_PROJ_FMT_WKT);

	//-----------------------------------------------------
	CSG_MetaData	*pFields	= Get_MetaData_DB().Get_Child("FIELDS");

	if( !pFields )
	{
		pFields	= Get_MetaData_DB().Add_Child("FIELDS");
	}

	pFields->Del_Children();

	for(iField=0; iField<Get_Field_Count(); iField++)
	{
		pFields->Add_Child("FIELD", Get_Field_Name(iField))->Add_Property("TYPE", gSG_Data_Type_Identifier[Get_Field_Type(iField)]);
	}

	Save_MetaData(File_Name);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
