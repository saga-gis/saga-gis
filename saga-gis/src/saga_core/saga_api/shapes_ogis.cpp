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
		if( SG_SSCANF(Text.c_str(), SG_T("%lf %lf"), &x, &y) == 2 )
		{
			pShape->Add_Point(x, y, iPart);
			
			return( true );
		}
		break;

	case SG_VERTEX_TYPE_XYZ:
		if( SG_SSCANF(Text.c_str(), SG_T("%lf %lf %lf"), &x, &y, &z) == 3 )
		{
			pShape->Add_Point(x, y, iPart);
			pShape->Set_Z    (z, pShape->Get_Point_Count(iPart) - 1, iPart);
			
			return( true );
		}
		break;

	case SG_VERTEX_TYPE_XYZM:
		if( SG_SSCANF(Text.c_str(), SG_T("%lf %lf %lf %lf"), &x, &y, &z, &m) == 4 )
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

	CSG_String	Type	= Text.BeforeFirst('(');

	Type.Trim(true);
	Type.Trim(false);

	switch( pShape->Get_Type() )
	{
	case SHAPE_TYPE_Point:
		if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Point) )
		{
			return( _WKT_Read_Point(Text.AfterFirst('(').BeforeFirst(')'), pShape, 0) );
		}
		break;

	case SHAPE_TYPE_Points:
		if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint) )
		{
			return( _WKT_Read_Parts(Text, pShape) );
		}
		break;

	case SHAPE_TYPE_Line:
		if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Line) )
		{
			return( _WKT_Read_Points(Text, pShape) );
		}

		if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine) )
		{
			return( _WKT_Read_Parts(Text, pShape) );
		}
		break;

	case SHAPE_TYPE_Polygon:
		if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon) )
		{
			return( _WKT_Read_Parts(Text, pShape) );
		}

		if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon) )
		{
			return( _WKT_Read_Polygon(Text, pShape) );
		}
		break;
	default:
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
		default:
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
bool CSG_Shapes_OGIS_Converter::from_ShapeType(CSG_String &Type, TSG_Shape_Type Shape, TSG_Vertex_Type Vertex)
{
	switch( Vertex )
	{
	case SG_VERTEX_TYPE_XY:
		switch( Shape )
		{
		case SHAPE_TYPE_Point:   Type	= SG_OGIS_TYPE_STR_Point;           return( true );
		case SHAPE_TYPE_Points:  Type	= SG_OGIS_TYPE_STR_MultiPoint;      return( true );
		case SHAPE_TYPE_Line:    Type	= SG_OGIS_TYPE_STR_MultiLine;       return( true );
		case SHAPE_TYPE_Polygon: Type	= SG_OGIS_TYPE_STR_MultiPolygon;    return( true );
		default:	return( false );
		}
		break;

	case SG_VERTEX_TYPE_XYZ:
		switch( Shape )
		{
		case SHAPE_TYPE_Point:   Type	= SG_OGIS_TYPE_STR_Point_Z;         return( true );
		case SHAPE_TYPE_Points:  Type	= SG_OGIS_TYPE_STR_MultiPoint_Z;    return( true );
		case SHAPE_TYPE_Line:    Type	= SG_OGIS_TYPE_STR_MultiLine_Z;     return( true );
		case SHAPE_TYPE_Polygon: Type	= SG_OGIS_TYPE_STR_MultiPolygon_Z;  return( true );
		default:	return( false );
		}
		break;

	case SG_VERTEX_TYPE_XYZM:
		switch( Shape )
		{
		case SHAPE_TYPE_Point:   Type	= SG_OGIS_TYPE_STR_Point_ZM;        return( true );
		case SHAPE_TYPE_Points:  Type	= SG_OGIS_TYPE_STR_MultiPoint_ZM;   return( true );
		case SHAPE_TYPE_Line:    Type	= SG_OGIS_TYPE_STR_MultiLine_ZM;    return( true );
		case SHAPE_TYPE_Polygon: Type	= SG_OGIS_TYPE_STR_MultiPolygon_ZM; return( true );
		default:	return( false );
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_ShapeType(const CSG_String &Type, TSG_Shape_Type &Shape, TSG_Vertex_Type &Vertex)
{
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Point           ) )	{	Shape	= SHAPE_TYPE_Point;		Vertex	= SG_VERTEX_TYPE_XY;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint      ) )	{	Shape	= SHAPE_TYPE_Points;	Vertex	= SG_VERTEX_TYPE_XY;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Line            ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XY;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine       ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XY;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon         ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XY;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon    ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XY;	return( true );	}

	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Point_Z         ) )	{	Shape	= SHAPE_TYPE_Point;		Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint_Z    ) )	{	Shape	= SHAPE_TYPE_Points;	Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Line_Z          ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine_Z     ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon_Z       ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon_Z  ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}

	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Point_M         ) )	{	Shape	= SHAPE_TYPE_Point;		Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint_M    ) )	{	Shape	= SHAPE_TYPE_Points;	Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Line_M          ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine_M     ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon_M       ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon_M  ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XYZ;	return( true );	}

	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Point_ZM        ) )	{	Shape	= SHAPE_TYPE_Point;		Vertex	= SG_VERTEX_TYPE_XYZM;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPoint_ZM   ) )	{	Shape	= SHAPE_TYPE_Points;	Vertex	= SG_VERTEX_TYPE_XYZM;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Line_ZM         ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XYZM;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiLine_ZM    ) )	{	Shape	= SHAPE_TYPE_Line;		Vertex	= SG_VERTEX_TYPE_XYZM;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_Polygon_ZM      ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XYZM;	return( true );	}
	if( !Type.CmpNoCase(SG_OGIS_TYPE_STR_MultiPolygon_ZM ) )	{	Shape	= SHAPE_TYPE_Polygon;	Vertex	= SG_VERTEX_TYPE_XYZM;	return( true );	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Basic Type definitions
// byte : 1 byte
// uint16 : 16 bit unsigned integer (2 bytes)
// uint32 : 32 bit unsigned integer (4 bytes)
// float64 : double precision floating point number (8 bytes)
//
// +------------------------------------------------------------+
// | RASTER                                                     |
// +---------------+-------------+------------------------------+
// | - name -      |  - type -   | - meaning -                  |
// +---------------+-------------+------------------------------+
// | endiannes     | byte        | 1:ndr/little endian          |
// |               |             | 0:xdr/big endian             |
// +---------------+-------------+------------------------------+
// | version       | uint16      | format version (0 for this   |
// |               |             | structure)                   |
// +---------------+-------------+------------------------------+
// | nBands        | uint16      | Number of bands              |
// +---------------+-------------+------------------------------+
// | scaleX        | float64     | pixel width                  |
// |               |             | in geographical units        |
// +---------------+-------------+------------------------------+
// | scaleY        | float64     | pixel height                 |
// |               |             | in geographical units        |
// +---------------+-------------+------------------------------+
// | ipX           | float64     | X ordinate of upper-left     |
// |               |             | pixel's upper-left corner    |
// |               |             | in geographical units        |
// +---------------+-------------+------------------------------+
// | ipY           | float64     | Y ordinate of upper-left     |
// |               |             | pixel's upper-left corner    |
// |               |             | in geographical units        |
// +---------------+-------------+------------------------------+
// | skewX         | float64     | rotation about Y-axis        |
// +---------------+-------------+------------------------------+
// | skewY         | float64     | rotation about X-axis        |
// +---------------+-------------+------------------------------+
// | srid          | int32       | Spatial reference id         |
// +---------------+-------------+------------------------------+
// | width         | uint16      | number of pixel columns      |
// +---------------+-------------+------------------------------+
// | height        | uint16      | number of pixel rows         |
// +---------------+-------------+------------------------------+
// | bands[nBands] | RASTERBAND  | Bands data                   |
// +---------------+-------------+------------------------------+

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_OGIS_Converter::from_WKBinary(CSG_Bytes &Bytes, class CSG_Grid *pGrid)
{
	Bytes.Rewind();

	//-----------------------------------------------------
	// Raster System

	bool	bSwap	= Bytes.Read_Byte  () == 0;	// endiannes: 1=ndr/little endian, 0=xdr/big endian
	short	version	= Bytes.Read_Short (bSwap);	// version
	short	nBands	= Bytes.Read_Short (bSwap);	// number of bands
	double	dx		= Bytes.Read_Double(bSwap);	// scaleX
	double	dy		= Bytes.Read_Double(bSwap);	// scaleY
	double	xMin	= Bytes.Read_Double(bSwap);	// ipX
	double	yMax	= Bytes.Read_Double(bSwap);	// ipY
	double	skewX	= Bytes.Read_Double(bSwap);	// skewX
	double	skewY	= Bytes.Read_Double(bSwap);	// skewY
	int   	SRID	= Bytes.Read_Int   (bSwap);	// srid
	short 	NX		= Bytes.Read_Short (bSwap);	// width
	short 	NY		= Bytes.Read_Short (bSwap);	// height

	//-----------------------------------------------------
	// Band

	TSG_Data_Type	Type;

	BYTE	Flags	= Bytes.Read_Byte();

	switch( Flags & 0x0F )
	{
	case  0:	Type	= SG_DATATYPE_Bit   ; break;	//  0:  1-bit boolean
	case  1:	Type	= SG_DATATYPE_Char  ; break;	//  1:  2-bit unsigned integer
	case  2:	Type	= SG_DATATYPE_Char  ; break;	//  2:  4-bit unsigned integer
	case  3:	Type	= SG_DATATYPE_Char  ; break;	//  3:  8-bit   signed integer
	case  4:	Type	= SG_DATATYPE_Byte  ; break;	//  4:  8-bit unsigned integer
	case  5:	Type	= SG_DATATYPE_Short ; break;	//  5: 16-bit   signed integer
	case  6:	Type	= SG_DATATYPE_Word  ; break;	//  6: 16-bit unsigned integer
	case  7:	Type	= SG_DATATYPE_Int   ; break;	//  7: 32-bit   signed integer
	case  8:	Type	= SG_DATATYPE_DWord ; break;	//  8: 32-bit unsigned integer
	case 10:	Type	= SG_DATATYPE_Float ; break;	// 10: 32-bit float
	case 11:	Type	= SG_DATATYPE_Double; break;	// 11: 64-bit float
	}

//	Flags	|= 0x80;	// isOffline: no, never here!
	Flags	|= 0x40;	// hasNodataValue
//	Flags	|= 0x20;	// isNoDataValue: no, never here!
//	Flags	|= 0x10;	// reserved (unused)

	if( !pGrid->Create(Type, NX, NY, dx, xMin + 0.5 * dx, yMax - (NY - 0.5) * dx) )
	{
		return( false );
	}

	pGrid->Get_Projection().Create(SRID);

	double	noData;

	switch( pGrid->Get_Type() )
	{
	case SG_DATATYPE_Bit   : noData	= Bytes.Read_Byte  (     ); break;	//  0:  1-bit boolean
	case SG_DATATYPE_Char  : noData	= Bytes.Read_Char  (     ); break;	//  3:  8-bit   signed integer
	case SG_DATATYPE_Byte  : noData	= Bytes.Read_Byte  (     ); break;	//  4:  8-bit unsigned integer
	case SG_DATATYPE_Short : noData	= Bytes.Read_Short (bSwap); break;	//  5: 16-bit   signed integer
	case SG_DATATYPE_Word  : noData	= Bytes.Read_Word  (bSwap); break;	//  6: 16-bit unsigned integer
	case SG_DATATYPE_Int   : noData	= Bytes.Read_Int   (bSwap); break;	//  7: 32-bit   signed integer
	case SG_DATATYPE_DWord : noData	= Bytes.Read_DWord (bSwap); break;	//  8: 32-bit unsigned integer
	case SG_DATATYPE_Float : noData	= Bytes.Read_Float (bSwap); break;	//  9: 32-bit float
	case SG_DATATYPE_Double: noData	= Bytes.Read_Double(bSwap); break;	// 10: 64-bit float
	default:
		break;
	}

	pGrid->Set_NoData_Value(noData);

	for(int y=0; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			switch( pGrid->Get_Type() )
			{
			case SG_DATATYPE_Bit   : pGrid->Set_Value(x, y, Bytes.Read_Byte  (     )); break;	//  0:  1-bit boolean
			case SG_DATATYPE_Char  : pGrid->Set_Value(x, y, Bytes.Read_Char  (     )); break;	//  3:  8-bit   signed integer
			case SG_DATATYPE_Byte  : pGrid->Set_Value(x, y, Bytes.Read_Byte  (     )); break;	//  4:  8-bit unsigned integer
			case SG_DATATYPE_Short : pGrid->Set_Value(x, y, Bytes.Read_Short (bSwap)); break;	//  5: 16-bit   signed integer
			case SG_DATATYPE_Word  : pGrid->Set_Value(x, y, Bytes.Read_Word  (bSwap)); break;	//  6: 16-bit unsigned integer
			case SG_DATATYPE_Int   : pGrid->Set_Value(x, y, Bytes.Read_Int   (bSwap)); break;	//  7: 32-bit   signed integer
			case SG_DATATYPE_DWord : pGrid->Set_Value(x, y, Bytes.Read_DWord (bSwap)); break;	//  8: 32-bit unsigned integer
			case SG_DATATYPE_Float : pGrid->Set_Value(x, y, Bytes.Read_Float (bSwap)); break;	//  9: 32-bit float
			case SG_DATATYPE_Double: pGrid->Set_Value(x, y, Bytes.Read_Double(bSwap)); break;	// 10: 64-bit float
			default:
				break;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_OGIS_Converter::to_WKBinary(CSG_Bytes &Bytes, class CSG_Grid *pGrid, int SRID)
{
	Bytes.Clear();

	//-----------------------------------------------------
	// Raster System

	if( pGrid->Get_Projection().Get_EPSG() > 0 )
	{
		SRID	= pGrid->Get_Projection().Get_EPSG();
	}

	Bytes	+= (BYTE  )1;						// endiannes
	Bytes	+= (short )0;						// version
	Bytes	+= (short )1;						// number of bands
	Bytes	+= (double)pGrid->Get_Cellsize();	// scaleX
	Bytes	+= (double)pGrid->Get_Cellsize();	// scaleY
	Bytes	+= (double)pGrid->Get_XMin(true);	// ipX
	Bytes	+= (double)pGrid->Get_YMax(true);	// ipY
	Bytes	+= (double)0.0;						// skewX
	Bytes	+= (double)0.0;						// skewY
	Bytes	+= (int   )SRID;					// srid
	Bytes	+= (short )pGrid->Get_NX();			// width
	Bytes	+= (short )pGrid->Get_NY();			// height

	//-----------------------------------------------------
	// Band

	BYTE	Flags;

	switch( pGrid->Get_Type() )
	{
	case SG_DATATYPE_Bit   : Flags =  0; break;	//  0:  1-bit boolean
//	case SG_DATATYPE_      : Flags =  1; break;	//  1:  2-bit unsigned integer
//	case SG_DATATYPE_      : Flags =  2; break;	//  2:  4-bit unsigned integer
	case SG_DATATYPE_Char  : Flags =  3; break;	//  3:  8-bit   signed integer
	case SG_DATATYPE_Byte  : Flags =  4; break;	//  4:  8-bit unsigned integer
	case SG_DATATYPE_Short : Flags =  5; break;	//  5: 16-bit   signed integer
	case SG_DATATYPE_Word  : Flags =  6; break;	//  6: 16-bit unsigned integer
	case SG_DATATYPE_Int   : Flags =  7; break;	//  7: 32-bit   signed integer
	case SG_DATATYPE_DWord : Flags =  8; break;	//  8: 32-bit unsigned integer
	case SG_DATATYPE_Float : Flags = 10; break;	// 10: 32-bit float
	case SG_DATATYPE_Double: Flags = 11; break;	// 11: 64-bit float
	default:
		break;
	}

//	Flags	|= 0x80;	// isOffline: no, never here!
	Flags	|= 0x40;	// hasNodataValue
//	Flags	|= 0x20;	// isNoDataValue: no, never here!
//	Flags	|= 0x10;	// reserved (unused)

	Bytes	+= Flags;

	switch( pGrid->Get_Type() )
	{
	case SG_DATATYPE_Bit   : Bytes	+= (BYTE  )0; break;	//  0:  1-bit boolean
	case SG_DATATYPE_Char  : Bytes	+= (char  )pGrid->Get_NoData_Value(); break;	//  3:  8-bit   signed integer
	case SG_DATATYPE_Byte  : Bytes	+= (BYTE  )pGrid->Get_NoData_Value(); break;	//  4:  8-bit unsigned integer
	case SG_DATATYPE_Short : Bytes	+= (short )pGrid->Get_NoData_Value(); break;	//  5: 16-bit   signed integer
	case SG_DATATYPE_Word  : Bytes	+= (WORD  )pGrid->Get_NoData_Value(); break;	//  6: 16-bit unsigned integer
	case SG_DATATYPE_Int   : Bytes	+= (int   )pGrid->Get_NoData_Value(); break;	//  7: 32-bit   signed integer
	case SG_DATATYPE_DWord : Bytes	+= (DWORD )pGrid->Get_NoData_Value(); break;	//  8: 32-bit unsigned integer
	case SG_DATATYPE_Float : Bytes	+= (float )pGrid->Get_NoData_Value(); break;	//  9: 32-bit float
	case SG_DATATYPE_Double: Bytes	+= (double)pGrid->Get_NoData_Value(); break;	// 10: 64-bit float
	default:
		break;
	}

	for(int y=0; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			switch( pGrid->Get_Type() )
			{
			case SG_DATATYPE_Bit   : Bytes	+= (BYTE  )pGrid->asDouble(x, y); break;	//  0:  1-bit boolean
			case SG_DATATYPE_Char  : Bytes	+= (char  )pGrid->asDouble(x, y); break;	//  3:  8-bit   signed integer
			case SG_DATATYPE_Byte  : Bytes	+= (BYTE  )pGrid->asDouble(x, y); break;	//  4:  8-bit unsigned integer
			case SG_DATATYPE_Short : Bytes	+= (short )pGrid->asDouble(x, y); break;	//  5: 16-bit   signed integer
			case SG_DATATYPE_Word  : Bytes	+= (WORD  )pGrid->asDouble(x, y); break;	//  6: 16-bit unsigned integer
			case SG_DATATYPE_Int   : Bytes	+= (int   )pGrid->asDouble(x, y); break;	//  7: 32-bit   signed integer
			case SG_DATATYPE_DWord : Bytes	+= (DWORD )pGrid->asDouble(x, y); break;	//  8: 32-bit unsigned integer
			case SG_DATATYPE_Float : Bytes	+= (float )pGrid->asDouble(x, y); break;	//  9: 32-bit float
			case SG_DATATYPE_Double: Bytes	+= (double)pGrid->asDouble(x, y); break;	// 10: 64-bit float
			default:
				break;	  
			}
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
