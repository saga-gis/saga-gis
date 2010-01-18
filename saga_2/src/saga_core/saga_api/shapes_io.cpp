
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	int				Type, iField, iPart, nParts, *Parts, iPoint, nPoints;
	double			*pZ, *pM;
	TSG_Point		*pPoint;
	CSG_Buffer		File_Header(100), Record_Header(8), Content;
	CSG_File		fSHP;
	CSG_Table_DBase	fDBF;

	//-----------------------------------------------------
	// Open DBase File...

	if( !fDBF.Open(SG_File_Make_Path(NULL, File_Name, SG_T("dbf"))) )
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] DBase file could not be opened."));

		return( false );
	}

	if( !fDBF.Move_First() || fDBF.Get_Record_Count() <= 0 )
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] DBase file does not contain any records."));

		return( false );
	}

	for(iField=0; iField<fDBF.Get_FieldCount(); iField++)
	{
		switch( fDBF.Get_FieldType(iField) )
		{
		case DBF_FT_LOGICAL:
			Add_Field(SG_STR_MBTOSG(fDBF.Get_FieldName(iField)), SG_DATATYPE_Char);
			break;

		case DBF_FT_CHARACTER:	default:
			Add_Field(SG_STR_MBTOSG(fDBF.Get_FieldName(iField)), SG_DATATYPE_String);
			break;

		case DBF_FT_DATE:
			Add_Field(SG_STR_MBTOSG(fDBF.Get_FieldName(iField)), SG_DATATYPE_Date);
			break;

		case DBF_FT_NUMERIC:
			Add_Field(SG_STR_MBTOSG(fDBF.Get_FieldName(iField)), fDBF.Get_FieldDecimals(iField) > 0
					? SG_DATATYPE_Double
					: SG_DATATYPE_Long
				);
			break;
		}
	}

	//-----------------------------------------------------
	// Open Shapes File...

	if( !fSHP.Open(SG_File_Make_Path(NULL, File_Name, SG_T("shp")), SG_FILE_R, true) )
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	// Read File Header (100 Bytes)...

	if( fSHP.Read(File_Header.Get_Data(), sizeof(char), 100) != 100 )
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] corrupted file header"));

		return( false );
	}

	if( File_Header.asInt( 0,  true) != 9994 )	// Byte 00 -> File Code 9994 (Integer Big)...
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] invalid file code"));

		return( false );
	}

	if( File_Header.asInt(28, false) != 1000 )	// Byte 28 -> Version 1000 (Integer Little)...
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] unsupported file version"));

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
		SG_UI_Msg_Add_Error(LNG("[ERR] unsupported shape type."));

		return( false );
	}

	//-----------------------------------------------------
	// Load Shapes...

	for(int iShape=0; iShape<fDBF.Get_Record_Count() && SG_UI_Process_Set_Progress(iShape, fDBF.Get_Record_Count()); iShape++)
	{
		if( fSHP.Read(Record_Header.Get_Data(0), sizeof(int), 2) != 2 )		// read record header
		{
			SG_UI_Msg_Add_Error(LNG("[ERR] corrupted record header"));

			return( false );
		}

		if( Record_Header.asInt(0, true) != iShape + 1 )					// record number
		{
			SG_UI_Msg_Add_Error(LNG("[ERR] corrupted shapefile."));

			return( false );
		}

		if( !Content.Set_Size(2 * Record_Header.asInt(4, true), false) )	// content length as 16-bit words !!!
		{
			SG_UI_Msg_Add_Error(LNG("[ERR] memory allocation error."));

			return( false );
		}

		if( !fSHP.Read(Content.Get_Data(), sizeof(char), 2 * Record_Header.asInt(4, true)) )
		{
			SG_UI_Msg_Add_Error(LNG("[ERR] corrupted shapefile."));

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
				SG_UI_Msg_Add_Error(LNG("[ERR] corrupted shapefile."));

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

				switch( m_Vertex_Type )	// reak Z + M
				{
				case SG_VERTEX_TYPE_XYZM:	pShape->Set_M(Content.asDouble(28), 0);
				case SG_VERTEX_TYPE_XYZ:	pShape->Set_Z(Content.asDouble(20), 0);
				}

				break;

			//---------------------------------------------
			case SHAPE_TYPE_Points: ///////////////////////

				nPoints	= Content.asInt(36);
				pPoint	= (TSG_Point *)Content.Get_Data(40);

				switch( m_Vertex_Type )	// reak Z + M
				{
				case SG_VERTEX_TYPE_XYZ:
					pZ	= (double *)Content.Get_Data(40 + nPoints * 16 + 16);
					break;

				case SG_VERTEX_TYPE_XYZM:
					pZ	= (double *)Content.Get_Data(40 + nPoints * 16 + 16);
					pM	= pZ + nPoints + 2;
					break;
				}

				//-----------------------------------------
				for(iPoint=0; iPoint<nPoints; iPoint++, pPoint++)
				{
					pShape->Add_Point(pPoint->x, pPoint->y);

					switch( m_Vertex_Type )	// reak Z + M
					{
					case SG_VERTEX_TYPE_XYZM:	pShape->Set_M(*pM, iPoint);	pM++;
					case SG_VERTEX_TYPE_XYZ:	pShape->Set_Z(*pZ, iPoint);	pZ++;
					}
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
					pZ	= (double *)Content.Get_Data(40 + nPoints * 16 + 16);
					break;

				case SG_VERTEX_TYPE_XYZM:
					pZ	= (double *)Content.Get_Data(40 + nPoints * 16 + 16);
					pM	= pZ + nPoints + 2;
					break;
				}


				//-----------------------------------------
				for(iPoint=0, iPart=0; iPoint<nPoints; iPoint++, pPoint++)
				{
					if( iPart < nParts - 1 && iPoint >= Parts[iPart + 1] )
					{
						iPart++;
					}

					pShape->Add_Point(pPoint->x, pPoint->y, iPart);

					switch( m_Vertex_Type )	// read Z + M
					{
					case SG_VERTEX_TYPE_XYZM:	pShape->Set_M(*pM, iPoint, iPart);	pM++;
					case SG_VERTEX_TYPE_XYZ:	pShape->Set_Z(*pZ, iPoint, iPart);	pZ++;
					}
				}

				break;
			}

			//---------------------------------------------
			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				switch( Get_Field_Type(iField) )
				{
				case SG_DATATYPE_Char:
					pShape->Set_Value(iField, SG_STR_MBTOSG(fDBF.asString(iField)) );
					break;

				case SG_DATATYPE_String:	default:
					pShape->Set_Value(iField, SG_STR_MBTOSG(fDBF.asString(iField)) );
					break;

				case SG_DATATYPE_Date:
					pShape->Set_Value(iField, fDBF.asDouble(iField) );
					break;

				case SG_DATATYPE_Long:
					pShape->Set_Value(iField, fDBF.asInt(iField) );
					break;

				case SG_DATATYPE_Double:
					pShape->Set_Value(iField, fDBF.asDouble(iField) );
					break;
				}
			}
		}

		fDBF.Move_Next();
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();

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

	switch( m_Vertex_Type )
	{
	case SG_VERTEX_TYPE_XY:						break;
	case SG_VERTEX_TYPE_XYZM:	Type	+= 10;	break;	// Z (+M)
	case SG_VERTEX_TYPE_XYZ:	Type	+= 20;	break;	// M
	default:	return( false );
	}

	//-----------------------------------------------------
	// DBase File Access...

	CSG_Table_DBase::TFieldDesc	*dbfFields	= new CSG_Table_DBase::TFieldDesc[Get_Field_Count()];

	for(iField=0; iField<Get_Field_Count(); iField++)
	{
		strncpy(dbfFields[iField].Name, SG_STR_SGTOMB(Get_Field_Name(iField)), 11);

		switch( Get_Field_Type(iField) )
		{
		case SG_DATATYPE_String:	default:
			dbfFields[iField].Type		= DBF_FT_CHARACTER;
			dbfFields[iField].Width		= (BYTE)255;
			break;

		case SG_DATATYPE_Date:
			dbfFields[iField].Type		= DBF_FT_DATE;
			dbfFields[iField].Width		= (BYTE)8;
			break;

		case SG_DATATYPE_Char:
			dbfFields[iField].Type		= DBF_FT_CHARACTER;
			dbfFields[iField].Width		= (BYTE)1;
			break;

		case SG_DATATYPE_Short:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_Long:
		case SG_DATATYPE_Color:
			dbfFields[iField].Type		= DBF_FT_NUMERIC;
			dbfFields[iField].Width		= (BYTE)16;
			dbfFields[iField].Decimals	= (BYTE)0;
			break;

		case SG_DATATYPE_Float:
		case SG_DATATYPE_Double:
			dbfFields[iField].Type		= DBF_FT_NUMERIC;
			dbfFields[iField].Width		= (BYTE)16;
			dbfFields[iField].Decimals	= (BYTE)8;
			break;
		}
	}

	if( !fDBF.Open(SG_File_Make_Path(NULL, File_Name, SG_T("dbf")), Get_Field_Count(), dbfFields) )
	{
		delete[](dbfFields);

		SG_UI_Msg_Add_Error(LNG("[ERR] dbase file could not be opened"));

		return( false );
	}

	delete[](dbfFields);

	//-----------------------------------------------------
	// Shape File Access...

	if( !fSHX.Open(SG_File_Make_Path(NULL, File_Name, SG_T("shx")), SG_FILE_W, true) )
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] index file could not be opened"));

		return( false );
	}

	if( !fSHP.Open(SG_File_Make_Path(NULL, File_Name, SG_T("shp")), SG_FILE_W, true) )
	{
		SG_UI_Msg_Add_Error(LNG("[ERR] shape file could not be opened."));

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

			switch( m_Vertex_Type )
			{
			case SG_VERTEX_TYPE_XY:		Set_Content_Length(10);	break;
			case SG_VERTEX_TYPE_XYZ:	Set_Content_Length(14);	break;
			case SG_VERTEX_TYPE_XYZM:	Set_Content_Length(18);	break;
			}

			fSHP.Write_Int		(Type);

			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Points:		///////////////////////

			switch( m_Vertex_Type )
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

			switch( m_Vertex_Type )
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

			for(iPart=0, iPoint=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				fSHP.Write_Int(iPoint);

				iPoint	+= pShape->Get_Point_Count(iPart);
			}

			break;
		}

		//-------------------------------------------------
		switch( m_Type )			// write point data
		{
		default:	break;

		//-------------------------------------------------
		case SHAPE_TYPE_Point:		///////////////////////

			fSHP.Write(&pShape->Get_Point(0), sizeof(TSG_Point));

			//---------------------------------------------
			if( m_Vertex_Type != SG_VERTEX_TYPE_XY )
			{
				fSHP.Write_Double(pShape->Get_Z(0));

				if( m_Vertex_Type == SG_VERTEX_TYPE_XYZM )
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
					fSHP.Write(&pShape->Get_Point(iPoint, iPart), sizeof(TSG_Point));
				}
			}

			//---------------------------------------------
			if( m_Vertex_Type != SG_VERTEX_TYPE_XY )
			{
				fSHP.Write_Double(pShape->Get_ZMin());
				fSHP.Write_Double(pShape->Get_ZMin());

				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						fSHP.Write_Double(pShape->Get_Z(iPoint, iPart));
					}
				}

				if( m_Vertex_Type == SG_VERTEX_TYPE_XYZM )
				{
					fSHP.Write_Double(pShape->Get_MMin());
					fSHP.Write_Double(pShape->Get_MMin());

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
			switch( fDBF.Get_FieldType(iField) )
			{
			case DBF_FT_DATE:
			case DBF_FT_CHARACTER:
				fDBF.Set_Value(iField, SG_STR_SGTOMB(pShape->asString(iField)));
				break;

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
	SG_UI_Process_Set_Ready();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
