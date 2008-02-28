
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
bool CSG_Shapes::_Load_ESRI(const SG_Char *File_Name)
{
	bool		bError;

	char		buf_Header[100];

	int			Type_File, Type_Shape, Type_Ext,
				FileCode, FileLength, Version,
				RecordNumber, ContentLength,
				iShape, iPart, nParts, iPoint, nPoints,
				buf_nParts, *buf_nPoints;

	TSG_Point	dPoint;

	TSG_Rect	dRect;

	CSG_String	fName;

	CSG_File	Stream;

	CSG_Shape	*pShape;


	//-----------------------------------------------------
	// Load Attributes...

	fName	= SG_File_Make_Path(NULL, File_Name, SG_T("dbf"));

	if( !m_Table._Create(fName, SG_T('\t')) || m_Table.Get_Record_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	// Open Shapes File...

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Load shapes"), File_Name), true);

	fName	= SG_File_Make_Path(NULL, File_Name, SG_T("shp"));

	if( !Stream.Open(fName, SG_FILE_R, true) )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	// Load File-Header (100-Bytes)...

	Stream.Read(buf_Header, sizeof(char), 100);

	FileCode	= SG_Mem_Get_Int	(buf_Header +  0, true );	// Byte 0		-> File Code 9994 Integer Big...
	// ...														// Byte 4-20	-> Unused 0 Integer Big...
	FileLength	= SG_Mem_Get_Int	(buf_Header + 24, true );	// Byte 24		-> File Length File Length Integer Big...
	Version		= SG_Mem_Get_Int	(buf_Header + 28, false);	// Byte 28		-> Version 1000 Integer Little...
	Type_File	= SG_Mem_Get_Int	(buf_Header + 32, false);	// Byte 32		-> Shape m_Type Shape m_Type Integer Little...
	dRect.xMin	= SG_Mem_Get_Double	(buf_Header + 36, false);	// Byte 36		-> Bounding Box Xmin Double Little...
	dRect.yMin	= SG_Mem_Get_Double	(buf_Header + 44, false);	// Byte 44		-> Bounding Box Ymin Double Little...
	dRect.xMax	= SG_Mem_Get_Double	(buf_Header + 52, false);	// Byte 52		-> Bounding Box Xmax Double Little...
	dRect.yMax	= SG_Mem_Get_Double	(buf_Header + 60, false);	// Byte 60		-> Bounding Box Ymax Double Little...
	// ...														// Byte 68*		-> Bounding Box Zmin Double Little...
	// ...														// Byte 76*		-> Bounding Box Zmax Double Little...
	// ...														// Byte 84*		-> Bounding Box Mmin Double Little...
	// ...														// Byte 92*		-> Bounding Box Mmax Double Little...

	m_Extent.Assign(dRect);

	switch( Type_File )
	{
	default:	m_Type	= SHAPE_TYPE_Undefined;	break;	// unsupported...
	case 31:	m_Type	= SHAPE_TYPE_Undefined;	break;	// unsupported: MultiPatch...

	case 1:		m_Type	= SHAPE_TYPE_Point;		Type_Ext	= 0;	break;	// Point
	case 8:		m_Type	= SHAPE_TYPE_Points;	Type_Ext	= 0;	break;	// MultiPoint
	case 3:		m_Type	= SHAPE_TYPE_Line;		Type_Ext	= 0;	break;	// PolyLine
	case 5:		m_Type	= SHAPE_TYPE_Polygon;	Type_Ext	= 0;	break;	// Polygon

	case 11:	m_Type	= SHAPE_TYPE_Point;		Type_Ext	= 2;	break;	// PointZ
	case 18:	m_Type	= SHAPE_TYPE_Points;	Type_Ext	= 2;	break;	// MultiPointZ
	case 13:	m_Type	= SHAPE_TYPE_Line;		Type_Ext	= 2;	break;	// PolyLineZ
	case 15:	m_Type	= SHAPE_TYPE_Polygon;	Type_Ext	= 2;	break;	// PolygonZ

	case 21:	m_Type	= SHAPE_TYPE_Point;		Type_Ext	= 1;	break;	// PointM
	case 28:	m_Type	= SHAPE_TYPE_Points;	Type_Ext	= 1;	break;	// MultiPointM
	case 23:	m_Type	= SHAPE_TYPE_Line;		Type_Ext	= 1;	break;	// PolyLineM
	case 25:	m_Type	= SHAPE_TYPE_Polygon;	Type_Ext	= 1;	break;	// PolygonM
	}

	if( Stream.is_EOF() || FileCode != 9994 || Version != 1000 || m_Type == SHAPE_TYPE_Undefined )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file invalid or of unsupported type."));

		return( false );
	}

	//-------------------------------------------------
	// Load Shapes...

	m_nShapes	= m_nBuffer	= m_Table.Get_Record_Count();
	m_Shapes	= (CSG_Shape **)SG_Malloc(m_nShapes * sizeof(CSG_Shape *));

	buf_nParts	= 0;
	buf_nPoints	= NULL;

	for(iShape=0, bError=false; iShape<m_nShapes && SG_UI_Process_Set_Progress(iShape, m_nShapes); iShape++)
	{
		switch( m_Type )
		{
		default:
		case SHAPE_TYPE_Point:		m_Shapes[iShape]	= pShape	= new CSG_Shape_Point	(this, m_Table.Get_Record(iShape));	break;
		case SHAPE_TYPE_Points:		m_Shapes[iShape]	= pShape	= new CSG_Shape_Points	(this, m_Table.Get_Record(iShape));	break;
		case SHAPE_TYPE_Line:		m_Shapes[iShape]	= pShape	= new CSG_Shape_Line	(this, m_Table.Get_Record(iShape));	break;
		case SHAPE_TYPE_Polygon:	m_Shapes[iShape]	= pShape	= new CSG_Shape_Polygon	(this, m_Table.Get_Record(iShape));	break;
		}

		RecordNumber	= Stream.Read_Int(true);
		ContentLength	= Stream.Read_Int(true);

		Stream.Read(&Type_Shape, sizeof(int));

		if( Type_Shape != Type_File || Stream.is_EOF() )
		{
			bError	= true;
		}
		else
		{
			switch( m_Type )
			{
			default:
				SG_UI_Msg_Add_Error(LNG("[ERR] Corrupted shape file."));
				break;

			//---------------------------------------------
			case SHAPE_TYPE_Point:

				Stream.Read(&dPoint	, sizeof(TSG_Point));
				pShape->Add_Point(dPoint.x, dPoint.y);

				//-----------------------------------------
				if( Type_Ext != 0 )
				{
					if( Type_Ext == 1 )	// read Z
					{
						Stream.Read_Double(false);
					}

					// read M (optional)
					Stream.Read_Double(false);
				}
				break;

			//---------------------------------------------
			case SHAPE_TYPE_Points:

				Stream.Read(&dRect	, sizeof(TSG_Rect));
				Stream.Read(&nPoints, sizeof(int));

				for(iPoint=0; iPoint<nPoints; iPoint++)
				{
					Stream.Read(&dPoint, sizeof(TSG_Point));
					pShape->Add_Point(dPoint.x, dPoint.y);
				}

				//-----------------------------------------
				if( Type_Ext != 0 )
				{
					if( Type_Ext == 1 )	// read Z
					{
						Stream.Read_Double(false);
						Stream.Read_Double(false);

						for(iPoint=0; iPoint<nPoints; iPoint++)
							Stream.Read_Double(false);
					}

					// read M (optional)
					Stream.Read_Double(false);
					Stream.Read_Double(false);

					for(iPoint=0; iPoint<nPoints; iPoint++)
						Stream.Read_Double(false);
					break;
				}
				break;

			//---------------------------------------------
			case SHAPE_TYPE_Line:
			case SHAPE_TYPE_Polygon:

				Stream.Read(&dRect	, sizeof(TSG_Rect));
				Stream.Read(&nParts	, sizeof(int));
				Stream.Read(&nPoints, sizeof(int));

				//-----------------------------------------
				if( buf_nParts <= nParts )
				{
					buf_nParts	= nParts + 1;
					buf_nPoints	= (int *)SG_Realloc(buf_nPoints, buf_nParts * sizeof(int));
				}

				for(iPart=0; iPart<nParts; iPart++)
				{
					Stream.Read(buf_nPoints + iPart, sizeof(int));
				}

				buf_nPoints[nParts]	= nPoints;

				//-----------------------------------------
				for(iPoint=0, iPart=0; iPoint<nPoints; iPoint++)
				{
					if( iPoint >= buf_nPoints[iPart + 1] && iPart < nParts - 1 )
					{
						iPart++;
					}

					Stream.Read(&dPoint, sizeof(TSG_Point));
					pShape->Add_Point(dPoint.x, dPoint.y, iPart);
				}

				//-----------------------------------------
				if( Type_Ext != 0 )
				{
					if( Type_Ext == 1 )	// read Z
					{
						Stream.Read_Double(false);
						Stream.Read_Double(false);

						for(iPoint=0; iPoint<nPoints; iPoint++)
							Stream.Read_Double(false);
					}

					// read M (optional)
					Stream.Read_Double(false);
					Stream.Read_Double(false);

					for(iPoint=0; iPoint<nPoints; iPoint++)
						Stream.Read_Double(false);
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	// Clean up...

	if( buf_nPoints )
	{
		SG_Free(buf_nPoints);
	}

	if( bError )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file is corrupted."));
	}
	else
	{
		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	}

	SG_UI_Process_Set_Ready();

	return( bError == false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define Save_ESRI_RecordHeader	Stream    .Write_Int(RecordNumber++, true);\
								Stream    .Write_Int(ContentLength , true);\
								Stream_Idx.Write_Int(FileLength	   , true);\
								Stream_Idx.Write_Int(ContentLength , true);\
								FileLength		+= 4 + ContentLength;\
								FileLength_idx	+= 4;\

//---------------------------------------------------------
bool CSG_Shapes::_Save_ESRI(const SG_Char *File_Name)
{
	char		buf_Header[100];

	int			FileLength, FileLength_idx, Type_File,
				RecordNumber, ContentLength,
				iShape, iPoint, iPart, nPoints;

	TSG_Point	dPoint;

	TSG_Rect	dRect;

	CSG_String	fName;

	CSG_File	Stream, Stream_Idx;

	CSG_Shape	*pShape;


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

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Save shapes"), File_Name), true);

	fName	= SG_File_Make_Path(NULL, File_Name, SG_T("shx"));

	if( !Stream_Idx.Open(fName, SG_FILE_W, true) )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape index file could not be opened."));

		return( false );
	}

	fName	= SG_File_Make_Path(NULL, File_Name, SG_T("shp"));

	if( !Stream.Open(fName, SG_FILE_W, true) )
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(LNG("[ERR] Shape file could not be opened."));

		return( false );
	}

	SG_UI_Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), LNG("[DAT] Save shapes"), fName.c_str() ));

	//-----------------------------------------------------
	// Save Header...

	_Extent_Update();

	dRect	= m_Extent.m_rect;

	SG_Mem_Set_Int		(buf_Header	+  0, 9994		, true );	// Byte 0	-> File Code 9994 Integer Big...
	SG_Mem_Set_Int		(buf_Header +  4, 0			, true );	// Byte 4	-> unused Integer Big...
	SG_Mem_Set_Int		(buf_Header +  8, 0			, true );	// Byte 8	-> unused Integer Big...
	SG_Mem_Set_Int		(buf_Header + 12, 0			, true );	// Byte 12	-> unused Integer Big...
	SG_Mem_Set_Int		(buf_Header + 16, 0			, true );	// Byte 16	-> unused Integer Big...
	SG_Mem_Set_Int		(buf_Header + 20, 0			, true );	// Byte 20	-> unused Integer Big...
	SG_Mem_Set_Int		(buf_Header + 24, 0			, true );	// Byte 24	-> File Length File Length Integer Big...
	SG_Mem_Set_Int		(buf_Header + 28, 1000		, false);	// Byte 28	-> Version 1000 Integer Little...
	SG_Mem_Set_Int		(buf_Header + 32, Type_File	, false);	// Byte 32	-> Shape m_Type Shape m_Type Integer Little...
	SG_Mem_Set_Double	(buf_Header + 36, dRect.xMin, false);	// Byte 36	-> Bounding Box Xmin Double Little...
	SG_Mem_Set_Double	(buf_Header + 44, dRect.yMin, false);	// Byte 44	-> Bounding Box Ymin Double Little...
	SG_Mem_Set_Double	(buf_Header + 52, dRect.xMax, false);	// Byte 52	-> Bounding Box Xmax Double Little...
	SG_Mem_Set_Double	(buf_Header + 60, dRect.yMax, false);	// Byte 60	-> Bounding Box Ymax Double Little...
	SG_Mem_Set_Double	(buf_Header + 68, 0			, false);	// Byte 68	-> Bounding Box Zmin Double Little...
	SG_Mem_Set_Double	(buf_Header + 76, 0			, false);	// Byte 76	-> Bounding Box Zmax Double Little...
	SG_Mem_Set_Double	(buf_Header + 84, 0			, false);	// Byte 84	-> Bounding Box Mmin Double Little...
	SG_Mem_Set_Double	(buf_Header + 92, 0			, false);	// Byte 92	-> Bounding Box Mmax Double Little...

	Stream		.Write(buf_Header, sizeof(char), 100);
	Stream_Idx	.Write(buf_Header, sizeof(char), 100);

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
			Stream.Write(&Type_File	, sizeof(Type_File));

			//---------------------------------------------
			// Shape-Points...
			dPoint			= pShape->Get_Point(0);
			Stream.Write(&dPoint	, sizeof(TSG_Point));
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

			Stream.Write(&Type_File	, sizeof(Type_File));
			Stream.Write(&dRect		, sizeof(TSG_Rect));
			Stream.Write(&nPoints	, sizeof(int));

			//-----------------------------------------
			// Shape-Points...
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					dPoint		= pShape->Get_Point(iPoint, iPart);
					Stream.Write(&dPoint, sizeof(TSG_Point));
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

			Stream.Write(&Type_File	, sizeof(Type_File));
			Stream.Write(&dRect		, sizeof(TSG_Rect));
			Stream.Write(&iPart		, sizeof(int));
			Stream.Write(&nPoints	, sizeof(int));

			for(iPart=0, iPoint=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				Stream.Write(&iPoint, sizeof(int));
				iPoint	+= pShape->Get_Point_Count(iPart);
			}

			//---------------------------------------------
			// Shape-Points...
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					dPoint		= pShape->Get_Point(iPoint, iPart);
					Stream.Write(&dPoint, sizeof(TSG_Point));
				}
			}
			break;
		}
	}

	//-----------------------------------------------------
	// File Sizes...

	Stream		.Seek(24);
	Stream_Idx	.Seek(24);

	Stream		.Write_Int(FileLength    , true);
	Stream_Idx	.Write_Int(FileLength_idx, true);

	SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

	SG_UI_Process_Set_Ready();

	//-----------------------------------------------------
	// Attributes...

	fName	= SG_File_Make_Path(NULL, File_Name, SG_T("dbf"));

	return( m_Table.Save(fName, TABLE_FILETYPE_DBase) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
