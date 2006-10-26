
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::_Load_ESRI(const char *File_Name)
{
	char		buf_Header[100];

	int			Type_File, Type_Shape,
				FileCode, FileLength, Version,
				RecordNumber, ContentLength,
				iShape, iPart, nParts, iPoint, nPoints,
				buf_nParts, *buf_nPoints;

	FILE		*Stream;

	TSG_Point	dPoint;

	TSG_Rect	dRect;

	CSG_Shape		*pShape;

	CSG_String	fName;


	//-----------------------------------------------------
	// Load Attributes...

	fName	= SG_File_Make_Path(NULL, File_Name, "dbf");

	if( !m_Table._Create(fName, '\t') || m_Table.Get_Record_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	// Open Shapes File...

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Load shapes"), File_Name), true);

	fName	= SG_File_Make_Path(NULL, File_Name, "shp");

	if( (Stream = fopen(fName, "rb")) == NULL )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	// Load File-Header (100-Bytes)...

	fread(buf_Header, 100, sizeof(char), Stream);

	FileCode	= SG_Read_Int		(buf_Header +  0, true );	// Byte 0		-> File Code 9994 Integer Big...
	// ...														// Byte 4-20	-> Unused 0 Integer Big...
	FileLength	= SG_Read_Int		(buf_Header + 24, true );	// Byte 24		-> File Length File Length Integer Big...
	Version		= SG_Read_Int		(buf_Header + 28, false);	// Byte 28		-> Version 1000 Integer Little...
	Type_File	= SG_Read_Int		(buf_Header + 32, false);	// Byte 32		-> Shape m_Type Shape m_Type Integer Little...
	dRect.xMin	= SG_Read_Double	(buf_Header + 36, false);	// Byte 36		-> Bounding Box Xmin Double Little...
	dRect.yMin	= SG_Read_Double	(buf_Header + 44, false);	// Byte 44		-> Bounding Box Ymin Double Little...
	dRect.xMax	= SG_Read_Double	(buf_Header + 52, false);	// Byte 52		-> Bounding Box Xmax Double Little...
	dRect.yMax	= SG_Read_Double	(buf_Header + 60, false);	// Byte 60		-> Bounding Box Ymax Double Little...
	// ...														// Byte 68*		-> Bounding Box Zmin Double Little...
	// ...														// Byte 76*		-> Bounding Box Zmax Double Little...
	// ...														// Byte 84*		-> Bounding Box Mmin Double Little...
	// ...														// Byte 92*		-> Bounding Box Mmax Double Little...

	m_Extent.Assign(dRect);

	switch( Type_File )
	{
	case 1:		// Point...
		m_Type	= SHAPE_TYPE_Point;
		break;

	case 8:		// Multipoint...
		m_Type	= SHAPE_TYPE_Points;
		break;

	case 3:		// Line...
		m_Type	= SHAPE_TYPE_Line;
		break;

	case 5:		// Polygon...
		m_Type	= SHAPE_TYPE_Polygon;
		break;

	default:	// unsupported...
		m_Type	= SHAPE_TYPE_Undefined;
		break;
	}

	if( feof(Stream) || FileCode != 9994 || Version != 1000 || m_Type == SHAPE_TYPE_Undefined )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file header is invalid."));

		fclose(Stream);

		return( false );
	}

	//-------------------------------------------------
	// Load Shapes...

	buf_nParts	= 0;
	buf_nPoints	= NULL;

	for(iShape=0; iShape<m_Table.Get_Record_Count() && SG_UI_Process_Set_Progress(iShape, m_Table.Get_Record_Count()); iShape++)
	{
		RecordNumber	= SG_Read_Int(Stream, true);
		ContentLength	= SG_Read_Int(Stream, true);

		fread(&Type_Shape, 1, sizeof(int), Stream);

		if( Type_Shape != Type_File || feof(Stream) )
		{
			SG_UI_Msg_Add(LNG("[MSG] failed"), false);

			SG_UI_Msg_Add_Error(LNG("[ERR] Shape file is corrupted."));

			break;
		}
		else
		{
			pShape	= _Add_Shape(m_Table.Get_Record(iShape));

			switch( Type_Shape )
			{
			//---------------------------------------------
			case 1:			// Point...
				fread(&dPoint	, 1, sizeof(TSG_Point), Stream);
				pShape->Add_Point(dPoint.x, dPoint.y);
				break;

			//---------------------------------------------
			case 8:			// Multipoint...
				fread(&dRect	, 1, sizeof(TSG_Rect)	, Stream);
				fread(&nPoints	, 1, sizeof(int)		, Stream);

				for(iPoint=0; iPoint<nPoints; iPoint++)
				{
					fread(&dPoint, 1, sizeof(TSG_Point), Stream);
					pShape->Add_Point(dPoint.x, dPoint.y);
				}
				break;

			//---------------------------------------------
			case 3: case 5:	// Line, Polygon...
				fread(&dRect	, 1, sizeof(TSG_Rect)	, Stream);
				fread(&nParts	, 1, sizeof(int)		, Stream);
				fread(&nPoints	, 1, sizeof(int)		, Stream);

				//-----------------------------------------
				if( buf_nParts <= nParts )
				{
					buf_nParts	= nParts + 1;
					buf_nPoints	= (int *)SG_Realloc(buf_nPoints, buf_nParts * sizeof(int));
				}

				for(iPart=0; iPart<nParts; iPart++)
				{
					fread(buf_nPoints + iPart, 1, sizeof(int), Stream);
				}

				buf_nPoints[nParts]	= nPoints;

				//-----------------------------------------
				for(iPoint=0, iPart=0; iPoint<nPoints; iPoint++)
				{
					if( iPoint >= buf_nPoints[iPart + 1] && iPart < nParts - 1 )
					{
						iPart++;
					}

					fread(&dPoint, 1, sizeof(TSG_Point), Stream);
					pShape->Add_Point(dPoint.x, dPoint.y, iPart);
				}
				break;
			}
		}
	}

	SG_UI_Msg_Add(LNG("[MSG] okay"), false);

	SG_UI_Process_Set_Ready();

	//-----------------------------------------------------
	// Clean up...

	if( buf_nPoints )
	{
		SG_Free(buf_nPoints);
	}

	fclose(Stream);

	return( Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define Save_ESRI_RecordHeader	SG_Write_Int(Stream	, RecordNumber++, true);\
								SG_Write_Int(Stream	, ContentLength	, true);\
								SG_Write_Int(Stream_Idx, FileLength	, true);\
								SG_Write_Int(Stream_Idx, ContentLength	, true);\
								FileLength		+= 4 + ContentLength;\
								FileLength_idx	+= 4;\

//---------------------------------------------------------
bool CSG_Shapes::_Save_ESRI(const char *File_Name)
{
	char		buf_Header[100];

	int			FileLength, FileLength_idx, Type_File,
				RecordNumber, ContentLength,
				iShape, iPoint, iPart, nPoints;

	FILE		*Stream, *Stream_Idx;

	TSG_Point	dPoint;

	TSG_Rect	dRect;

	CSG_Shape		*pShape;

	CSG_String	fName;


	//-----------------------------------------------------
	// Set Shape m_Type...

	switch( m_Type )
	{
	case SHAPE_TYPE_Point:		// Point...
		Type_File	= 1;
		break;

	case SHAPE_TYPE_Points:		// Multipoint...
		Type_File	= 8;
		break;

	case SHAPE_TYPE_Line:		// Line, Polyline...
		Type_File	= 3;
		break;

	case SHAPE_TYPE_Polygon:	// Polygon...
		Type_File	= 5;
		break;

	default:					// unsupported...
		return( false );
	}


	//-----------------------------------------------------
	// File Access...

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Save shapes"), File_Name), true);

	fName	= SG_File_Make_Path(NULL, File_Name, "shx");

	if( (Stream_Idx = fopen(fName, "wb")) == NULL )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape index file could not be opened."));

		return( false );
	}

	fName	= SG_File_Make_Path(NULL, File_Name, "shp");

	if( (Stream     = fopen(fName, "wb")) == NULL )
	{
		fclose(Stream_Idx);

		SG_UI_Msg_Add(LNG("[MSG] failed"), false);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file could not be opened."));

		return( false );
	}

	SG_UI_Process_Set_Text(CSG_String::Format("%s: %s", LNG("[DAT] Save shapes"), fName.c_str()));

	//-----------------------------------------------------
	// Save Header...

	_Extent_Update();

	dRect	= m_Extent.m_rect;

	SG_Write_Int	(buf_Header	+  0, 9994		, true );	// Byte 0	-> File Code 9994 Integer Big...
	SG_Write_Int	(buf_Header +  4, 0			, true );	// Byte 4	-> unused Integer Big...
	SG_Write_Int	(buf_Header +  8, 0			, true );	// Byte 8	-> unused Integer Big...
	SG_Write_Int	(buf_Header + 12, 0			, true );	// Byte 12	-> unused Integer Big...
	SG_Write_Int	(buf_Header + 16, 0			, true );	// Byte 16	-> unused Integer Big...
	SG_Write_Int	(buf_Header + 20, 0			, true );	// Byte 20	-> unused Integer Big...
	SG_Write_Int	(buf_Header + 24, 0			, true );	// Byte 24	-> File Length File Length Integer Big...
	SG_Write_Int	(buf_Header + 28, 1000		, false);	// Byte 28	-> Version 1000 Integer Little...
	SG_Write_Int	(buf_Header + 32, Type_File	, false);	// Byte 32	-> Shape m_Type Shape m_Type Integer Little...
	SG_Write_Double(buf_Header + 36, dRect.xMin, false);	// Byte 36	-> Bounding Box Xmin Double Little...
	SG_Write_Double(buf_Header + 44, dRect.yMin, false);	// Byte 44	-> Bounding Box Ymin Double Little...
	SG_Write_Double(buf_Header + 52, dRect.xMax, false);	// Byte 52	-> Bounding Box Xmax Double Little...
	SG_Write_Double(buf_Header + 60, dRect.yMax, false);	// Byte 60	-> Bounding Box Ymax Double Little...
	SG_Write_Double(buf_Header + 68, 0			, false);	// Byte 68	-> Bounding Box Zmin Double Little...
	SG_Write_Double(buf_Header + 76, 0			, false);	// Byte 76	-> Bounding Box Zmax Double Little...
	SG_Write_Double(buf_Header + 84, 0			, false);	// Byte 84	-> Bounding Box Mmin Double Little...
	SG_Write_Double(buf_Header + 92, 0			, false);	// Byte 92	-> Bounding Box Mmax Double Little...

	fwrite(buf_Header, 100, sizeof(char), Stream);
	fwrite(buf_Header, 100, sizeof(char), Stream_Idx);

	FileLength		= 50;	// FileLength measured in 16-bit words...
	FileLength_idx	= 50;	// FileLength measured in 16-bit words...

	RecordNumber	= 1;

	//-----------------------------------------------------
	// Save Shapes...

	for(iShape=0; iShape<Get_Count() && SG_UI_Process_Set_Progress(iShape, Get_Count()); iShape++)
	{
		pShape	= Get_Shape(iShape);

		switch( Type_File )
		{
		//-------------------------------------------------
		case 1:			// Point...
			//---------------------------------------------
			// Record-Header...
			ContentLength	= 10;	// ShapeType + Point...
			Save_ESRI_RecordHeader;

			//---------------------------------------------
			// Shape-Header...
			fwrite(&Type_File						, 1, sizeof(Type_File)	, Stream);

			//---------------------------------------------
			// Shape-Points...
			dPoint			= pShape->Get_Point(0);
			fwrite(&dPoint		, 1, sizeof(TSG_Point), Stream);
			break;

		//-------------------------------------------------
		case 8:			// Multipoint...
			//---------------------------------------------
			// Total Number of Points in Shape...
			for(iPart=0, nPoints=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				nPoints	+= pShape->Get_Point_Count(iPart);
			}

			//-----------------------------------------
			// Record-Header...
			ContentLength	= 10;	// ShapeType + Point...
			Save_ESRI_RecordHeader;

			//-----------------------------------------
			// Shape-Header...
			dRect			= pShape->Get_Extent();

			fwrite(&Type_File	, 1, sizeof(Type_File)	, Stream);
			fwrite(&dRect		, 1, sizeof(TSG_Rect)	, Stream);
			fwrite(&nPoints		, 1, sizeof(int)		, Stream);

			//-----------------------------------------
			// Shape-Points...
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					dPoint		= pShape->Get_Point(iPoint, iPart);
					fwrite(&dPoint, 1, sizeof(TSG_Point), Stream);
				}
			}
			break;

		//-------------------------------------------------
		case 3: case 5:	// Line, Polygon...
			//---------------------------------------------
			// Total Number of Points in Shape...
			for(iPart=0, nPoints=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				nPoints	+= pShape->Get_Point_Count(iPart);
			}

			//---------------------------------------------
			// Record-Header...
			ContentLength	= 22 + 2 * pShape->Get_Part_Count() + 8 * nPoints;	// ShapeType + nParts + nParts*Offsets + nPoints...
			Save_ESRI_RecordHeader;

			//---------------------------------------------
			// Shape-Header...
			dRect			= pShape->Get_Extent();
			iPart			= pShape->Get_Part_Count();

			fwrite(&Type_File	, 1, sizeof(Type_File)	, Stream);
			fwrite(&dRect		, 1, sizeof(TSG_Rect)	, Stream);
			fwrite(&iPart		, 1, sizeof(int)		, Stream);
			fwrite(&nPoints		, 1, sizeof(int)		, Stream);

			for(iPart=0, iPoint=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				fwrite(&iPoint	, 1, sizeof(int)		, Stream);
				iPoint	+= pShape->Get_Point_Count(iPart);
			}

			//---------------------------------------------
			// Shape-Points...
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					dPoint		= pShape->Get_Point(iPoint, iPart);
					fwrite(&dPoint, 1, sizeof(TSG_Point), Stream);
				}
			}
			break;
		}
	}

	//-----------------------------------------------------
	// File Sizes...

	fseek(Stream	, 24, SEEK_SET);
	fseek(Stream_Idx, 24, SEEK_SET);

	SG_Write_Int(Stream	, FileLength	, true);
	SG_Write_Int(Stream_Idx, FileLength_idx, true);

	fclose(Stream);
	fclose(Stream_Idx);

	SG_UI_Msg_Add(LNG("[MSG] okay"), false);

	SG_UI_Process_Set_Ready();

	//-----------------------------------------------------
	// Attributes...

	fName	= SG_File_Make_Path(NULL, File_Name, "dbf");

	return( m_Table.Save(fName, TABLE_FILETYPE_DBase) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
