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
//                   shapes_ogis.cpp                     //
//                                                       //
//          Copyright (C) 2010 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
inline bool CSG_Shapes_OGIS_Converter::_WKT_Read_Point(const CSG_String &Text, CSG_Shape *pShape, int iPart)
{
	double		x, y, z, m;

	switch( ((CSG_Shapes *)pShape->Get_Table())->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		if( SG_SSCANF(Text.c_str(), SG_T("%f %f"), &x, &y) == 2 )
		{
			pShape->Add_Point(x, y, iPart);
			
			return( true );
		}
		break;

	case SG_VERTEX_TYPE_XYZ:
		if( SG_SSCANF(Text.c_str(), SG_T("%f %f %f"), &x, &y, &z) == 3 )
		{
			pShape->Add_Point(x, y, iPart);
			pShape->Set_Z    (z, pShape->Get_Point_Count(iPart) - 1, iPart);
			
			return( true );
		}
		break;

	case SG_VERTEX_TYPE_XYZM:
		if( SG_SSCANF(Text.c_str(), SG_T("%f %f %f %f"), &x, &y, &z, &m) == 4 )
		{
			pShape->Add_Point(x, y, iPart);
			pShape->Set_Z    (z, pShape->Get_Point_Count(iPart) - 1, iPart);
			pShape->Set_M    (m, pShape->Get_Point_Count(iPart) - 1, iPart);
			
			return( true );
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKT_Read_Points(const CSG_String &Text, CSG_Shape *pShape)
{
	int			iPart	= pShape->Get_Part_Count();
	CSG_String	s(Text.AfterFirst('(').BeforeFirst(')'));

	while( s.Length() > 0 )
	{
		if( !_WKT_Read_Point(s, pShape, iPart) )
		{
			return( false );
		}

		s	= s.AfterFirst(',');
	}

	return( pShape->Get_Point_Count(iPart) > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKT_Read_Parts(const CSG_String &Text, CSG_Shape *pShape)
{
	CSG_String	s	= Text.AfterFirst('(').BeforeLast(')');

	while( s.Length() > 0 )
	{
		_WKT_Read_Points(s, pShape);

		s	= s.AfterFirst(',');
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKT_Read_Polygon(const CSG_String &Text, CSG_Shape *pShape)
{
	CSG_String	Part;

	for(int i=0, Level=-2; i<(int)Text.Length(); i++)
	{
		if( Text[i] == '(' )
		{
			Level++;
		}
		else if( Text[i] == ')' )
		{
			if( Level == 0 )
			{
				Part	+= Text[i];
				_WKT_Read_Parts(Part, pShape);
				Part.Clear();
			}

			Level--;
		}

		if( Level >= 0 )
		{
			Part	+= Text[i];
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::from_WKText(const CSG_String &Text, CSG_Shape *pShape)
{
	pShape->Del_Parts();

	switch( pShape->Get_Type() )
	{
	case SHAPE_TYPE_Point:
		if( !Text.BeforeFirst('(').CmpNoCase(SG_OGIS_TYPE_STR_Point) )
		{
			return( _WKT_Read_Point(Text.AfterFirst('(').BeforeFirst(')'), pShape, 0) );
		}
		break;

	case SHAPE_TYPE_Points:
		if( !Text.BeforeFirst('(').CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint) )
		{
			return( _WKT_Read_Parts(Text, pShape) );
		}
		break;

	case SHAPE_TYPE_Line:
		if( !Text.BeforeFirst('(').CmpNoCase(SG_OGIS_TYPE_STR_Line) )
		{
			return( _WKT_Read_Points(Text, pShape) );
		}

		if( !Text.BeforeFirst('(').CmpNoCase(SG_OGIS_TYPE_STR_MultiLine) )
		{
			return( _WKT_Read_Parts(Text, pShape) );
		}
		break;

	case SHAPE_TYPE_Polygon:
		if( !Text.BeforeFirst('(').CmpNoCase(SG_OGIS_TYPE_STR_Polygon) )
		{
			return( _WKT_Read_Parts(Text, pShape) );
		}

		if( !Text.BeforeFirst('(').CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon) )
		{
			return( _WKT_Read_Polygon(Text, pShape) );
		}
		break;
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Point(CSG_String &Text, CSG_Shape *pShape, int iPoint, int iPart)
{
	TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

	switch( ((CSG_Shapes *)pShape->Get_Table())->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		Text	+= CSG_String::Format(SG_T("%f %f")      , Point.x, Point.y);
		break;

	case SG_VERTEX_TYPE_XYZ:
		Text	+= CSG_String::Format(SG_T("%f %f %f")   , Point.x, Point.y, pShape->Get_Z(iPoint, iPart));
		break;

	case SG_VERTEX_TYPE_XYZM:
		Text	+= CSG_String::Format(SG_T("%f %f %f %f"), Point.x, Point.y, pShape->Get_Z(iPoint, iPart), pShape->Get_M(iPoint, iPart));
		break;
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Points(CSG_String &Text, CSG_Shape *pShape, int iPart)
{
	Text	+= SG_T("(");

	for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
	{
		if( iPoint > 0 )
		{
			Text	+= SG_T(", ");
		}

		_WKT_Write_Point(Text, pShape, iPoint, iPart);
	}

	if( pShape->Get_Type() == SHAPE_TYPE_Polygon && CSG_Point(pShape->Get_Point(0, iPart)) != pShape->Get_Point(pShape->Get_Point_Count(iPart) -1, iPart) )
	{
		Text	+= SG_T(", ");

		_WKT_Write_Point(Text, pShape, 0, iPart);
	}

	Text	+= SG_T(")");

	return( true );
}

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Parts(CSG_String &Text, CSG_Shape *pShape)
{
	Text	+= SG_T("(");

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( iPart > 0 )
		{
			Text	+= SG_T(", ");
		}

		_WKT_Write_Points(Text, pShape, iPart);
	}

	Text	+= SG_T(")");

	return( true );
}

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Polygon(CSG_String &Text, CSG_Shape *pShape)
{
	Text	+= SG_T("(");

	for(int iPart=0, nIslands=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) == false )
		{
			if( nIslands++ > 0 )
			{
				Text	+= SG_T(", ");
			}

			Text	+= SG_T("(");

			_WKT_Write_Points(Text, pShape, iPart);

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(jPart) && ((CSG_Shape_Polygon *)pShape)->Contains(pShape->Get_Point(0, jPart), iPart) )
				{
					Text	+= SG_T(", ");

					_WKT_Write_Points(Text, pShape, jPart);
				}
			}

			Text	+= SG_T(")");
		}
	}

	Text	+= SG_T(")");

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_WKText(CSG_Shape *pShape, CSG_String &Text)
{
	switch( pShape->Get_Type() )
	{
	default:
		return( false );

	case SHAPE_TYPE_Point:
		Text	= SG_OGIS_TYPE_STR_Point;
		_WKT_Write_Points(Text, pShape, 0);
		break;

	case SHAPE_TYPE_Points:
		Text	 = SG_OGIS_TYPE_STR_MultiPoint;
		_WKT_Write_Parts(Text, pShape);
		break;

	case SHAPE_TYPE_Line:
		Text	 = SG_OGIS_TYPE_STR_MultiLine;
		_WKT_Write_Parts(Text, pShape);
		break;

	case SHAPE_TYPE_Polygon:
		Text	 = SG_OGIS_TYPE_STR_MultiPolygon;
		_WKT_Write_Polygon(Text, pShape);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKB_Read_Point(CSG_Bytes &Bytes, bool bSwapBytes, CSG_Shape *pShape, int iPart)
{
	if( Bytes.is_EOF() )
	{
		return( false );
	}

	double	x, y;

	x	= Bytes.Read_Double(bSwapBytes);
	y	= Bytes.Read_Double(bSwapBytes);

	pShape->Add_Point(x, y, iPart);

	switch( ((CSG_Shapes *)pShape->Get_Table())->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		break;

	case SG_VERTEX_TYPE_XYZ:
		pShape->Set_Z(Bytes.Read_Double(bSwapBytes), pShape->Get_Point_Count(iPart) - 1, iPart);
		break;

	case SG_VERTEX_TYPE_XYZM:
		pShape->Set_Z(Bytes.Read_Double(bSwapBytes), pShape->Get_Point_Count(iPart) - 1, iPart);
		pShape->Set_M(Bytes.Read_Double(bSwapBytes), pShape->Get_Point_Count(iPart) - 1, iPart);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_Points(CSG_Bytes &Bytes, bool bSwapBytes, CSG_Shape *pShape)
{
	DWORD	iPart	= pShape->Get_Part_Count();
	DWORD	nPoints	= Bytes.Read_DWord(bSwapBytes);

	for(DWORD iPoint=0; iPoint<nPoints; iPoint++)
	{
		if( !_WKB_Read_Point(Bytes, bSwapBytes, pShape, iPart) )
		{
			return( false );
		}
	}

	return( pShape->Get_Point_Count(iPart) > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_Parts(CSG_Bytes &Bytes, bool bSwapBytes, CSG_Shape *pShape)
{
	DWORD	iPart, nParts	= Bytes.Read_DWord(bSwapBytes);

	for(iPart=0; iPart<nParts; iPart++)
	{
		if( !_WKB_Read_Points(Bytes, bSwapBytes, pShape) )
		{
			return( false );
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_MultiLine(CSG_Bytes &Bytes, bool bSwapBytes, CSG_Shape *pShape)
{
	DWORD	nLines	= Bytes.Read_DWord(bSwapBytes);

	for(DWORD iLine=0; iLine<nLines; iLine++)
	{
		bSwapBytes	= Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		if( Bytes.Read_DWord(bSwapBytes) != SG_OGIS_TYPE_LineString || !_WKB_Read_Points(Bytes, bSwapBytes, pShape) )
		{
			return( false );
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_MultiPolygon(CSG_Bytes &Bytes, bool bSwapBytes, CSG_Shape *pShape)
{
	DWORD	nPolygons	= Bytes.Read_DWord(bSwapBytes);

	for(DWORD iPolygon=0; iPolygon<nPolygons; iPolygon++)
	{
		bSwapBytes	= Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		if( Bytes.Read_DWord(bSwapBytes) != SG_OGIS_TYPE_Polygon || !_WKB_Read_Parts(Bytes, bSwapBytes, pShape) )
		{
			return( false );
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::from_WKBinary(CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	pShape->Del_Parts();

	if( Bytes.Get_Count() > 3 )
	{
		Bytes.Rewind();

		bool	bSwapBytes	= Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		switch( pShape->Get_Type() )
		{
		case SHAPE_TYPE_Point:
			if( Bytes.Read_DWord() == SG_OGIS_TYPE_Point )
			{
				return( _WKB_Read_Point(Bytes, bSwapBytes, pShape, 0) );
			}
			break;

		case SHAPE_TYPE_Points:
			if( Bytes.Read_DWord() == SG_OGIS_TYPE_MultiPoint )
			{
				return( _WKB_Read_Parts(Bytes, bSwapBytes, pShape) );
			}
			break;

		case SHAPE_TYPE_Line:
			switch( Bytes.Read_DWord() )
			{
			case SG_OGIS_TYPE_LineString:
				return( _WKB_Read_Points		(Bytes, bSwapBytes, pShape) );

			case SG_OGIS_TYPE_MultiLineString:
				return( _WKB_Read_MultiLine		(Bytes, bSwapBytes, pShape) );
			}
			break;

		case SHAPE_TYPE_Polygon:
			switch( Bytes.Read_DWord() )
			{
			case SG_OGIS_TYPE_Polygon:
				return( _WKB_Read_Parts			(Bytes, bSwapBytes, pShape) );

			case SG_OGIS_TYPE_MultiPolygon:
				return( _WKB_Read_MultiPolygon	(Bytes, bSwapBytes, pShape) );
			}
			break;
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKB_Write_Point(CSG_Bytes &Bytes, CSG_Shape *pShape, int iPoint, int iPart)
{
	TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

	Bytes	+= Point.x;
	Bytes	+= Point.y;

	switch( ((CSG_Shapes *)pShape->Get_Table())->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		break;

	case SG_VERTEX_TYPE_XYZ:
		Bytes	+= pShape->Get_Z(iPoint, iPart);
		break;

	case SG_VERTEX_TYPE_XYZM:
		Bytes	+= pShape->Get_Z(iPoint, iPart);
		Bytes	+= pShape->Get_M(iPoint, iPart);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Write_Points(CSG_Bytes &Bytes, CSG_Shape *pShape, int iPart)
{
	bool	bFirstTwice	= pShape->Get_Type() == SHAPE_TYPE_Polygon && CSG_Point(pShape->Get_Point(0, iPart)) != pShape->Get_Point(pShape->Get_Point_Count(iPart) -1, iPart);

	Bytes	+= (DWORD)(pShape->Get_Point_Count(iPart) + (bFirstTwice ? 1 : 0));

	for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
	{
		_WKB_Write_Point(Bytes, pShape, iPoint, iPart);
	}

	if( bFirstTwice )
	{
		_WKB_Write_Point(Bytes, pShape, 0, iPart);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Write_Parts(CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	Bytes	+= (DWORD)pShape->Get_Part_Count();

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( !_WKB_Write_Points(Bytes, pShape, iPart) )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Write_MultiLine(CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	Bytes	+= (DWORD)pShape->Get_Part_Count();

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		Bytes	+= (BYTE)SG_OGIS_BYTEORDER_NDR;
		Bytes	+= (DWORD)SG_OGIS_TYPE_LineString;

		if( !_WKB_Write_Points(Bytes, pShape, iPart) )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Write_MultiPolygon(CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	int		iPart, nPolygons, *nRings, *iPolygon;

	nPolygons	= 0;
	nRings		= new int[pShape->Get_Part_Count()];
	iPolygon	= new int[pShape->Get_Part_Count()];

	for(iPart=0, nPolygons=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		nRings  [iPart]	= 0;

		if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) == false )
		{
			nPolygons		++;
			nRings  [iPart]	++;
			iPolygon[iPart]	= iPart;

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(jPart) && ((CSG_Shape_Polygon *)pShape)->Contains(pShape->Get_Point(0, jPart), iPart) )
				{
					nRings  [iPart]++;
					iPolygon[jPart]	= iPart;
				}
			}
		}
	}

	Bytes	+= (DWORD)nPolygons;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( nRings[iPart] > 0 )
		{
			Bytes	+= (BYTE)SG_OGIS_BYTEORDER_NDR;
			Bytes	+= (DWORD)SG_OGIS_TYPE_Polygon;
			Bytes	+= (DWORD)nRings[iPart];

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( iPolygon[jPart] == iPart )
				{
					if( !_WKB_Write_Points(Bytes, pShape, jPart) )
					{
						delete[](nRings);
						delete[](iPolygon);
						return( false );
					}
				}
			}
		}
	}

	delete[](nRings);
	delete[](iPolygon);

	return( nPolygons > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_WKBinary(CSG_Shape *pShape, CSG_Bytes &Bytes)
{
	Bytes.Destroy();

	Bytes	+= (BYTE)SG_OGIS_BYTEORDER_NDR;

	switch( pShape->Get_Type() )
	{
	default:
		return( false );

	case SHAPE_TYPE_Point:
		Bytes	+= (DWORD)SG_OGIS_TYPE_Point;
		return( _WKB_Write_Point(Bytes, pShape, 0, 0) );

	case SHAPE_TYPE_Points:
		Bytes	+= (DWORD)SG_OGIS_TYPE_MultiPoint;
		return( _WKB_Write_Points(Bytes, pShape, 0) );

	case SHAPE_TYPE_Line:
		Bytes	+= (DWORD)SG_OGIS_TYPE_MultiLineString;
		return( _WKB_Write_MultiLine(Bytes, pShape) );

	case SHAPE_TYPE_Polygon:
		Bytes	+= (DWORD)SG_OGIS_TYPE_MultiPolygon;
		return( _WKB_Write_MultiPolygon(Bytes, pShape) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
