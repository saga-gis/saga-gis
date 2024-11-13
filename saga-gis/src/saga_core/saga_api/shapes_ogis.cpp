
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	CSG_Shapes_OGIS_Converter::Type_asWKText	(DWORD Type)
{
	switch( Type )
	{
	case SG_OGIS_TYPE_Point					: return( "Point"                );
	case SG_OGIS_TYPE_LineString			: return( "LineString"           );
	case SG_OGIS_TYPE_Polygon				: return( "Polygon"              );
	case SG_OGIS_TYPE_MultiPoint			: return( "MultiPoint"           );
	case SG_OGIS_TYPE_MultiLineString		: return( "MultiLineString"      );
	case SG_OGIS_TYPE_MultiPolygon			: return( "MultiPolygon"         );
	case SG_OGIS_TYPE_GeometryCollection	: return( "GeometryCollection"   );
	case SG_OGIS_TYPE_PolyhedralSurface		: return( "PolyhedralSurface"    );
	case SG_OGIS_TYPE_TIN					: return( "TIN"                  );
	case SG_OGIS_TYPE_Triangle				: return( "Triangle"             );

	case SG_OGIS_TYPE_PointZ				: return( "PointZ"               );
	case SG_OGIS_TYPE_LineStringZ			: return( "LineStringZ"          );
	case SG_OGIS_TYPE_PolygonZ				: return( "PolygonZ"             );
	case SG_OGIS_TYPE_MultiPointZ			: return( "MultiPointZ"          );
	case SG_OGIS_TYPE_MultiLineStringZ		: return( "MultiLineStringZ"     );
	case SG_OGIS_TYPE_MultiPolygonZ			: return( "MultiPolygonZ"        );
	case SG_OGIS_TYPE_GeometryCollectionZ	: return( "GeometryCollectionZ"  );
	case SG_OGIS_TYPE_PolyhedralSurfaceZ	: return( "PolyhedralSurfaceZ"   );
	case SG_OGIS_TYPE_TINZ					: return( "TINZ"                 );
	case SG_OGIS_TYPE_TriangleZ				: return( "TriangleZ"            );

	case SG_OGIS_TYPE_PointM				: return( "PointM"               );
	case SG_OGIS_TYPE_LineStringM			: return( "LineStringM"          );
	case SG_OGIS_TYPE_PolygonM				: return( "PolygonM"             );
	case SG_OGIS_TYPE_MultiPointM			: return( "MultiPointM"          );
	case SG_OGIS_TYPE_MultiLineStringM		: return( "MultiLineStringM"     );
	case SG_OGIS_TYPE_MultiPolygonM			: return( "MultiPolygonM"        );
	case SG_OGIS_TYPE_GeometryCollectionM	: return( "GeometryCollectionM"  );
	case SG_OGIS_TYPE_PolyhedralSurfaceM	: return( "PolyhedralSurfaceM"   );
	case SG_OGIS_TYPE_TINM					: return( "TINM"                 );
	case SG_OGIS_TYPE_TriangleM				: return( "TriangleM"            );

	case SG_OGIS_TYPE_PointZM				: return( "PointZM"              );
	case SG_OGIS_TYPE_LineStringZM			: return( "LineStringZM"         );
	case SG_OGIS_TYPE_PolygonZM				: return( "PolygonZM"            );
	case SG_OGIS_TYPE_MultiPointZM			: return( "MultiPointZM"         );
	case SG_OGIS_TYPE_MultiLineStringZM		: return( "MultiLineStringZM"    );
	case SG_OGIS_TYPE_MultiPolygonZM		: return( "MultiPolygonZM"       );
	case SG_OGIS_TYPE_GeometryCollectionZM	: return( "GeometryCollectionZM" );
	case SG_OGIS_TYPE_PolyhedralSurfaceZM	: return( "PolyhedralSurfaceZM"  );
	case SG_OGIS_TYPE_TINZM					: return( "TINZM"                );
	case SG_OGIS_TYPE_TriangleZM			: return( "TriangleZM"           );
	}

	return( "" );
}

//---------------------------------------------------------
DWORD CSG_Shapes_OGIS_Converter::Type_asWKBinary(const CSG_String &Type)
{
	#define TYPE_AS_WKB(t)	if( !Type.CmpNoCase(Type_asWKText(t)) )	return( t );

	TYPE_AS_WKB(SG_OGIS_TYPE_Point                );
	TYPE_AS_WKB(SG_OGIS_TYPE_LineString           );
	TYPE_AS_WKB(SG_OGIS_TYPE_Polygon              );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPoint           );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiLineString      );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPolygon         );
	TYPE_AS_WKB(SG_OGIS_TYPE_GeometryCollection   );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolyhedralSurface    );
	TYPE_AS_WKB(SG_OGIS_TYPE_TIN                  );
	TYPE_AS_WKB(SG_OGIS_TYPE_Triangle             );

	TYPE_AS_WKB(SG_OGIS_TYPE_PointZ               );
	TYPE_AS_WKB(SG_OGIS_TYPE_LineStringZ          );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolygonZ             );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPointZ          );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiLineStringZ     );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPolygonZ        );
	TYPE_AS_WKB(SG_OGIS_TYPE_GeometryCollectionZ  );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolyhedralSurfaceZ   );
	TYPE_AS_WKB(SG_OGIS_TYPE_TINZ                 );
	TYPE_AS_WKB(SG_OGIS_TYPE_TriangleZ            );

	TYPE_AS_WKB(SG_OGIS_TYPE_PointM               );
	TYPE_AS_WKB(SG_OGIS_TYPE_LineStringM          );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolygonM             );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPointM          );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiLineStringM     );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPolygonM        );
	TYPE_AS_WKB(SG_OGIS_TYPE_GeometryCollectionM  );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolyhedralSurfaceM   );
	TYPE_AS_WKB(SG_OGIS_TYPE_TINM                 );
	TYPE_AS_WKB(SG_OGIS_TYPE_TriangleM            );

	TYPE_AS_WKB(SG_OGIS_TYPE_PointZM              );
	TYPE_AS_WKB(SG_OGIS_TYPE_LineStringZM         );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolygonZM            );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPointZM         );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiLineStringZM    );
	TYPE_AS_WKB(SG_OGIS_TYPE_MultiPolygonZM       );
	TYPE_AS_WKB(SG_OGIS_TYPE_GeometryCollectionZM );
	TYPE_AS_WKB(SG_OGIS_TYPE_PolyhedralSurfaceZM  );
	TYPE_AS_WKB(SG_OGIS_TYPE_TINZM                );
	TYPE_AS_WKB(SG_OGIS_TYPE_TriangleZM           );

	return( SG_OGIS_TYPE_Undefined );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Read_Point(const CSG_String &Text, CSG_Shape *pShape, int iPart)
{
	double x, y, z, m;

	switch( pShape->Get_Vertex_Type() )
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
	CSG_String s(Text.AfterFirst('(').BeforeFirst(')')); int iPart = pShape->Get_Part_Count();

	while( s.Length() > 0 )
	{
		if( !_WKT_Read_Point(s, pShape, iPart) )
		{
			return( false );
		}

		s = s.AfterFirst(',');
	}

	return( pShape->Get_Point_Count(iPart) > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKT_Read_Parts(const CSG_String &Text, CSG_Shape *pShape)
{
	CSG_String s(Text.AfterFirst('(').BeforeLast(')'));

	while( s.Length() > 0 )
	{
		_WKT_Read_Points(s, pShape);

		s = s.AfterFirst(',');
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKT_Read_Polygon(const CSG_String &Text, CSG_Shape *pShape)
{
	CSG_String Part;

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
				Part += Text[i];
				_WKT_Read_Parts(Part, pShape);
				Part.Clear();
			}

			Level--;
		}

		if( Level >= 0 )
		{
			Part += Text[i];
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::from_WKText(const CSG_String &Text, CSG_Shape *pShape)
{
	pShape->Del_Parts();

	CSG_String Type(Text.BeforeFirst('(')); Type.Trim_Both();

	if( pShape->Get_Type() == to_ShapeType(Type) )
	{
		switch( Type_asWKBinary(Type) )
		{
		case SG_OGIS_TYPE_Point            :
		case SG_OGIS_TYPE_PointZ           :
		case SG_OGIS_TYPE_PointM           :
		case SG_OGIS_TYPE_PointZM          : return( _WKT_Read_Point(Text.AfterFirst('(').BeforeFirst(')'), pShape, 0) );

		case SG_OGIS_TYPE_MultiPoint       :
		case SG_OGIS_TYPE_MultiPointZ      :
		case SG_OGIS_TYPE_MultiPointM      :
		case SG_OGIS_TYPE_MultiPointZM     : return( _WKT_Read_Points (Text, pShape) );

		case SG_OGIS_TYPE_LineString       :
		case SG_OGIS_TYPE_LineStringZ      :
		case SG_OGIS_TYPE_LineStringM      :
		case SG_OGIS_TYPE_LineStringZM     : return( _WKT_Read_Points (Text, pShape) );

		case SG_OGIS_TYPE_MultiLineString  :
		case SG_OGIS_TYPE_MultiLineStringZ :
		case SG_OGIS_TYPE_MultiLineStringM :
		case SG_OGIS_TYPE_MultiLineStringZM:
			return( _WKT_Read_Parts  (Text, pShape) );

		case SG_OGIS_TYPE_Polygon          :
		case SG_OGIS_TYPE_PolygonZ         :
		case SG_OGIS_TYPE_PolygonM         :
		case SG_OGIS_TYPE_PolygonZM        : return( _WKT_Read_Parts  (Text, pShape) );

		case SG_OGIS_TYPE_MultiPolygon     :
		case SG_OGIS_TYPE_MultiPolygonZ    :
		case SG_OGIS_TYPE_MultiPolygonM    :
		case SG_OGIS_TYPE_MultiPolygonZM   : return( _WKT_Read_Polygon(Text, pShape) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Point(CSG_String &Text, CSG_Shape *pShape, int iPoint, int iPart)
{
	TSG_Point Point = pShape->Get_Point(iPoint, iPart);

	switch( ((CSG_Shapes *)pShape->Get_Table())->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		Text	+= CSG_String::Format("%f %f"      , Point.x, Point.y);
		break;

	case SG_VERTEX_TYPE_XYZ:
		Text	+= CSG_String::Format("%f %f %f"   , Point.x, Point.y, pShape->Get_Z(iPoint, iPart));
		break;

	case SG_VERTEX_TYPE_XYZM:
		Text	+= CSG_String::Format("%f %f %f %f", Point.x, Point.y, pShape->Get_Z(iPoint, iPart), pShape->Get_M(iPoint, iPart));
		break;
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Points(CSG_String &Text, CSG_Shape *pShape, int iPart)
{
	Text += "(";

	for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
	{
		if( iPoint > 0 )
		{
			Text += ", ";
		}

		_WKT_Write_Point(Text, pShape, iPoint, iPart);
	}

	if( pShape->Get_Type() == SHAPE_TYPE_Polygon && CSG_Point(pShape->Get_Point(0, iPart)) != pShape->Get_Point(pShape->Get_Point_Count(iPart) -1, iPart) )
	{
		Text += ", ";

		_WKT_Write_Point(Text, pShape, 0, iPart);
	}

	Text += ")";

	return( true );
}

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Parts(CSG_String &Text, CSG_Shape *pShape)
{
	Text += "(";

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( iPart > 0 )
		{
			Text += ", ";
		}

		_WKT_Write_Points(Text, pShape, iPart);
	}

	Text += ")";

	return( true );
}

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKT_Write_Polygon(CSG_String &Text, CSG_Shape *pShape)
{
	Text += "(";

	for(int iPart=0, nIslands=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) == false )
		{
			if( nIslands++ > 0 )
			{
				Text += ", ";
			}

			Text += "(";

			_WKT_Write_Points(Text, pShape, iPart);

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(jPart) && ((CSG_Shape_Polygon *)pShape)->Contains(pShape->Get_Point(0, jPart), iPart) )
				{
					Text += ", ";

					_WKT_Write_Points(Text, pShape, jPart);
				}
			}

			Text += ")";
		}
	}

	Text += ")";

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_WKText(CSG_Shape *pShape, CSG_String &Text)
{
	switch( pShape->Get_Type() )
	{
	case SHAPE_TYPE_Point  : _WKT_Write_Points (Text = from_ShapeType(pShape->Get_Type(), pShape->Get_Vertex_Type()), pShape, 0); break;
	case SHAPE_TYPE_Points : _WKT_Write_Parts  (Text = from_ShapeType(pShape->Get_Type(), pShape->Get_Vertex_Type()), pShape   ); break;
	case SHAPE_TYPE_Line   : _WKT_Write_Parts  (Text = from_ShapeType(pShape->Get_Type(), pShape->Get_Vertex_Type()), pShape   ); break;
	case SHAPE_TYPE_Polygon: _WKT_Write_Polygon(Text = from_ShapeType(pShape->Get_Type(), pShape->Get_Vertex_Type()), pShape   ); break;

	default:
		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Shapes_OGIS_Converter::_WKB_Read_Point(CSG_Bytes &Bytes, bool bSwapBytes, TSG_Vertex_Type Vertex, CSG_Shape *pShape, int iPart)
{
	if( !Bytes.is_EOF() )
	{
		double x = Bytes.Read_Double(bSwapBytes);
		double y = Bytes.Read_Double(bSwapBytes);

		pShape->Add_Point(x, y, iPart);

		switch( Vertex )
		{
		default:
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

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_Points(CSG_Bytes &Bytes, bool bSwapBytes, TSG_Vertex_Type Vertex, CSG_Shape *pShape)
{
	DWORD nPoints = Bytes.Read_DWord(bSwapBytes), iPart = pShape->Get_Part_Count();

	for(DWORD iPoint=0; iPoint<nPoints; iPoint++)
	{
		if( !_WKB_Read_Point(Bytes, bSwapBytes, Vertex, pShape, iPart) )
		{
			return( false );
		}
	}

	return( pShape->Get_Point_Count(iPart) > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_Polygon(CSG_Bytes &Bytes, bool bSwapBytes, TSG_Vertex_Type Vertex, CSG_Shape *pShape)
{
	int nParts = (int)Bytes.Read_DWord(bSwapBytes);

	for(int iPart=0; iPart<nParts; iPart++)
	{
		if( !_WKB_Read_Points(Bytes, bSwapBytes, Vertex, pShape) )
		{
			return( false );
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_MultiPoint(CSG_Bytes &Bytes, bool bSwapBytes, TSG_Vertex_Type Vertex, CSG_Shape *pShape)
{
	DWORD nPoints = Bytes.Read_DWord(bSwapBytes), iPart = pShape->Get_Part_Count();

	for(DWORD iPoint=0; iPoint<nPoints; iPoint++)
	{
		bSwapBytes = Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		TSG_Shape_Type Geometry; TSG_Vertex_Type _Vertex; to_ShapeType(Bytes.Read_DWord(bSwapBytes), Geometry, _Vertex);

		if( Geometry != SHAPE_TYPE_Point || Vertex != _Vertex || !_WKB_Read_Point(Bytes, bSwapBytes, Vertex, pShape, iPart) )
		{
			return( false );
		}
	}

	return( pShape->Get_Point_Count(iPart) > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_MultiLine(CSG_Bytes &Bytes, bool bSwapBytes, TSG_Vertex_Type Vertex, CSG_Shape *pShape)
{
	DWORD nLines = Bytes.Read_DWord(bSwapBytes);

	for(DWORD iLine=0; iLine<nLines; iLine++)
	{
		bSwapBytes = Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		TSG_Shape_Type Geometry; TSG_Vertex_Type _Vertex; to_ShapeType(Bytes.Read_DWord(bSwapBytes), Geometry, _Vertex);

		if( Geometry != SHAPE_TYPE_Line || Vertex != _Vertex || !_WKB_Read_Points(Bytes, bSwapBytes, Vertex, pShape) )
		{
			return( false );
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Read_MultiPolygon(CSG_Bytes &Bytes, bool bSwapBytes, TSG_Vertex_Type Vertex, CSG_Shape *pShape)
{
	DWORD nPolygons = Bytes.Read_DWord(bSwapBytes);

	for(DWORD iPolygon=0; iPolygon<nPolygons; iPolygon++)
	{
		bSwapBytes = Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		TSG_Shape_Type Geometry; TSG_Vertex_Type _Vertex; to_ShapeType(Bytes.Read_DWord(bSwapBytes), Geometry, _Vertex);

		if( Geometry != SHAPE_TYPE_Polygon || Vertex != _Vertex || !_WKB_Read_Polygon(Bytes, bSwapBytes, Vertex, pShape) )
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

	if( Bytes.Get_Count() >= 4 )
	{
		Bytes.Rewind();

		bool bSwapBytes = Bytes.Read_Byte() != SG_OGIS_BYTEORDER_NDR;

		DWORD Type = Bytes.Read_DWord();

		if( pShape->Get_Type() == to_ShapeType(Type) )
		{
			switch( Type )
			{
			case SG_OGIS_TYPE_Point            : return( _WKB_Read_Point       (Bytes, bSwapBytes, SG_VERTEX_TYPE_XY  , pShape, 0) );
			case SG_OGIS_TYPE_PointZ           : return( _WKB_Read_Point       (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape, 0) );
			case SG_OGIS_TYPE_PointM           : return( _WKB_Read_Point       (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape, 0) );
			case SG_OGIS_TYPE_PointZM          : return( _WKB_Read_Point       (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZM, pShape, 0) );

			case SG_OGIS_TYPE_MultiPoint       : return( _WKB_Read_MultiPoint  (Bytes, bSwapBytes, SG_VERTEX_TYPE_XY  , pShape   ) );
			case SG_OGIS_TYPE_MultiPointZ      : return( _WKB_Read_MultiPoint  (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_MultiPointM      : return( _WKB_Read_MultiPoint  (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_MultiPointZM     : return( _WKB_Read_MultiPoint  (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZM, pShape   ) );

			case SG_OGIS_TYPE_LineString       : return( _WKB_Read_Points      (Bytes, bSwapBytes, SG_VERTEX_TYPE_XY  , pShape   ) );
			case SG_OGIS_TYPE_LineStringZ      : return( _WKB_Read_Points      (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_LineStringM      : return( _WKB_Read_Points      (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_LineStringZM     : return( _WKB_Read_Points      (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZM, pShape   ) );

			case SG_OGIS_TYPE_MultiLineString  : return( _WKB_Read_MultiLine   (Bytes, bSwapBytes, SG_VERTEX_TYPE_XY  , pShape   ) );
			case SG_OGIS_TYPE_MultiLineStringZ : return( _WKB_Read_MultiLine   (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_MultiLineStringM : return( _WKB_Read_MultiLine   (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_MultiLineStringZM: return( _WKB_Read_MultiLine   (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZM, pShape   ) );

			case SG_OGIS_TYPE_Polygon          : return( _WKB_Read_Polygon     (Bytes, bSwapBytes, SG_VERTEX_TYPE_XY  , pShape   ) );
			case SG_OGIS_TYPE_PolygonZ         : return( _WKB_Read_Polygon     (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_PolygonM         : return( _WKB_Read_Polygon     (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_PolygonZM        : return( _WKB_Read_Polygon     (Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZM, pShape   ) );

			case SG_OGIS_TYPE_MultiPolygon     : return( _WKB_Read_MultiPolygon(Bytes, bSwapBytes, SG_VERTEX_TYPE_XY  , pShape   ) );
			case SG_OGIS_TYPE_MultiPolygonZ    : return( _WKB_Read_MultiPolygon(Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_MultiPolygonM    : return( _WKB_Read_MultiPolygon(Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZ , pShape   ) );
			case SG_OGIS_TYPE_MultiPolygonZM   : return( _WKB_Read_MultiPolygon(Bytes, bSwapBytes, SG_VERTEX_TYPE_XYZM, pShape   ) );
			}
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
	TSG_Point Point = pShape->Get_Point(iPoint, iPart);

	Bytes += Point.x;
	Bytes += Point.y;

	switch( pShape->Get_Vertex_Type() )
	{
	case SG_VERTEX_TYPE_XY:
		break;

	case SG_VERTEX_TYPE_XYZ:
		Bytes += pShape->Get_Z(iPoint, iPart);
		break;

	case SG_VERTEX_TYPE_XYZM:
		Bytes += pShape->Get_Z(iPoint, iPart);
		Bytes += pShape->Get_M(iPoint, iPart);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Write_Points(CSG_Bytes &Bytes, CSG_Shape *pShape, int iPart)
{
	bool bFirstTwice = pShape->Get_Type() == SHAPE_TYPE_Polygon && CSG_Point(pShape->Get_Point(0, iPart)) != pShape->Get_Point(0, iPart, false);

	Bytes += (DWORD)(pShape->Get_Point_Count(iPart) + (bFirstTwice ? 1 : 0));

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
bool CSG_Shapes_OGIS_Converter::_WKB_Write_MultiPoint(CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	Bytes += (DWORD)pShape->Get_Point_Count(); // total number of points

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			Bytes += (BYTE)SG_OGIS_BYTEORDER_NDR;

			switch( pShape->Get_Vertex_Type() )
			{
			case SG_VERTEX_TYPE_XY  : Bytes += (DWORD)SG_OGIS_TYPE_Point  ; break;
			case SG_VERTEX_TYPE_XYZ : Bytes += (DWORD)SG_OGIS_TYPE_PointZ ; break;
			case SG_VERTEX_TYPE_XYZM: Bytes += (DWORD)SG_OGIS_TYPE_PointZM; break;
			}

			if( !_WKB_Write_Point(Bytes, pShape, iPoint, iPart) )
			{
				return( false );
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::_WKB_Write_MultiLine(CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	Bytes += (DWORD)pShape->Get_Part_Count();

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		Bytes += (BYTE)SG_OGIS_BYTEORDER_NDR;

		switch( pShape->Get_Vertex_Type() )
		{
		case SG_VERTEX_TYPE_XY  : Bytes += (DWORD)SG_OGIS_TYPE_LineString  ; break;
		case SG_VERTEX_TYPE_XYZ : Bytes += (DWORD)SG_OGIS_TYPE_LineStringZ ; break;
		case SG_VERTEX_TYPE_XYZM: Bytes += (DWORD)SG_OGIS_TYPE_LineStringZM; break;
		}

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
	int nPolygons = 0; CSG_Array_Int nRings(pShape->Get_Part_Count()), iPolygon(pShape->Get_Part_Count());

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		nRings[iPart] = 0;

		if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) == false )
		{
			nPolygons       ++;
			nRings  [iPart] ++;
			iPolygon[iPart] = iPart;

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(jPart) && ((CSG_Shape_Polygon *)pShape)->Contains(pShape->Get_Point(0, jPart), iPart) )
				{
					nRings  [iPart] ++;
					iPolygon[jPart] = iPart;
				}
			}
		}
	}

	Bytes += (DWORD)nPolygons;

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( nRings[iPart] > 0 )
		{
			Bytes += (BYTE)SG_OGIS_BYTEORDER_NDR;

			switch( pShape->Get_Vertex_Type() )
			{
			case SG_VERTEX_TYPE_XY  : Bytes += (DWORD)SG_OGIS_TYPE_Polygon  ; break;
			case SG_VERTEX_TYPE_XYZ : Bytes += (DWORD)SG_OGIS_TYPE_PolygonZ ; break;
			case SG_VERTEX_TYPE_XYZM: Bytes += (DWORD)SG_OGIS_TYPE_PolygonZM; break;
			}

			Bytes += (DWORD)nRings[iPart];

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( iPolygon[jPart] == iPart )
				{
					if( !_WKB_Write_Points(Bytes, pShape, jPart) )
					{
						return( false );
					}
				}
			}
		}
	}

	return( nPolygons > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_WKBinary(CSG_Shape *pShape, CSG_Bytes &Bytes)
{
	DWORD Type;

	if( from_ShapeType(Type, pShape->Get_Type(), pShape->Get_Vertex_Type()) )
	{
		Bytes.Destroy();

		Bytes += (BYTE)SG_OGIS_BYTEORDER_NDR;
		Bytes += Type;

		switch( pShape->Get_Type() )
		{
		case SHAPE_TYPE_Point  : return( _WKB_Write_Point       (Bytes, pShape, 0, 0) );
		case SHAPE_TYPE_Points : return( _WKB_Write_MultiPoint  (Bytes, pShape      ) );
		case SHAPE_TYPE_Line   : return( _WKB_Write_MultiLine   (Bytes, pShape      ) );
		case SHAPE_TYPE_Polygon: return( _WKB_Write_MultiPolygon(Bytes, pShape      ) );
		default: break;
		}
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
		case SHAPE_TYPE_Point  : Type = Type_asWKText(SG_OGIS_TYPE_Point            ); return( true );
		case SHAPE_TYPE_Points : Type = Type_asWKText(SG_OGIS_TYPE_MultiPoint       ); return( true );
		case SHAPE_TYPE_Line   : Type = Type_asWKText(SG_OGIS_TYPE_MultiLineString  ); return( true );
		case SHAPE_TYPE_Polygon: Type = Type_asWKText(SG_OGIS_TYPE_MultiPolygon     ); return( true );
		default: break;
		}
		break;

	case SG_VERTEX_TYPE_XYZ:
		switch( Shape )
		{
		case SHAPE_TYPE_Point  : Type = Type_asWKText(SG_OGIS_TYPE_PointZ           ); return( true );
		case SHAPE_TYPE_Points : Type = Type_asWKText(SG_OGIS_TYPE_MultiPointZ      ); return( true );
		case SHAPE_TYPE_Line   : Type = Type_asWKText(SG_OGIS_TYPE_MultiLineStringZ ); return( true );
		case SHAPE_TYPE_Polygon: Type = Type_asWKText(SG_OGIS_TYPE_MultiPolygonZ    ); return( true );
		default: break;
		}
		break;

	case SG_VERTEX_TYPE_XYZM:
		switch( Shape )
		{
		case SHAPE_TYPE_Point  : Type = Type_asWKText(SG_OGIS_TYPE_PointZM          ); return( true );
		case SHAPE_TYPE_Points : Type = Type_asWKText(SG_OGIS_TYPE_MultiPointZM     ); return( true );
		case SHAPE_TYPE_Line   : Type = Type_asWKText(SG_OGIS_TYPE_MultiLineStringZM); return( true );
		case SHAPE_TYPE_Polygon: Type = Type_asWKText(SG_OGIS_TYPE_MultiPolygonZM   ); return( true );
		default: break;
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::from_ShapeType(DWORD &Type, TSG_Shape_Type Shape, TSG_Vertex_Type Vertex)
{
	switch( Vertex )
	{
	case SG_VERTEX_TYPE_XY:
		switch( Shape )
		{
		case SHAPE_TYPE_Point  : Type = SG_OGIS_TYPE_Point          ; return( true );
		case SHAPE_TYPE_Points : Type = SG_OGIS_TYPE_MultiPoint     ; return( true );
		case SHAPE_TYPE_Line   : Type = SG_OGIS_TYPE_MultiLineString; return( true );
		case SHAPE_TYPE_Polygon: Type = SG_OGIS_TYPE_MultiPolygon   ; return( true );
		default: break;
		}
		break;

	case SG_VERTEX_TYPE_XYZ:
		switch( Shape )
		{
		case SHAPE_TYPE_Point  : Type = SG_OGIS_TYPE_PointZ          ; return( true );
		case SHAPE_TYPE_Points : Type = SG_OGIS_TYPE_MultiPointZ     ; return( true );
		case SHAPE_TYPE_Line   : Type = SG_OGIS_TYPE_MultiLineStringZ; return( true );
		case SHAPE_TYPE_Polygon: Type = SG_OGIS_TYPE_MultiPolygonZ   ; return( true );
		default: break;
		}
		break;

	case SG_VERTEX_TYPE_XYZM:
		switch( Shape )
		{
		case SHAPE_TYPE_Point  : Type = SG_OGIS_TYPE_PointZM          ; return( true );
		case SHAPE_TYPE_Points : Type = SG_OGIS_TYPE_MultiPointZM     ; return( true );
		case SHAPE_TYPE_Line   : Type = SG_OGIS_TYPE_MultiLineStringZM; return( true );
		case SHAPE_TYPE_Polygon: Type = SG_OGIS_TYPE_MultiPolygonZM   ; return( true );
		default: break;
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CSG_Shapes_OGIS_Converter::from_ShapeType(TSG_Shape_Type Shape, TSG_Vertex_Type Vertex)
{
	CSG_String Type;

	from_ShapeType(Type, Shape, Vertex);

	return( Type );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_ShapeType(const CSG_String &Type, TSG_Shape_Type &Shape, TSG_Vertex_Type &Vertex)
{
	switch( Type_asWKBinary(Type) )
	{
	case SG_OGIS_TYPE_Point             : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_MultiPoint        : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_LineString        : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_MultiLineString   : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_Polygon           : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_MultiPolygon      : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );

	case SG_OGIS_TYPE_PointZ            : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPointZ       : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_LineStringZ       : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiLineStringZ  : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_PolygonZ          : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPolygonZ     : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );

	case SG_OGIS_TYPE_PointM            : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPointM       : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_LineStringM       : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiLineStringM  : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_PolygonM          : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPolygonM     : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );

	case SG_OGIS_TYPE_PointZM           : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_MultiPointZM      : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_LineStringZM      : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_MultiLineStringZM : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_PolygonZM         : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_MultiPolygonZM    : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	}

	Shape = SHAPE_TYPE_Undefined; Vertex = SG_VERTEX_TYPE_XY;

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes_OGIS_Converter::to_ShapeType(DWORD Type, TSG_Shape_Type &Shape, TSG_Vertex_Type &Vertex)
{
	switch( Type )
	{
	case SG_OGIS_TYPE_Point             : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_MultiPoint        : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_LineString        : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_MultiLineString   : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_Polygon           : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );
	case SG_OGIS_TYPE_MultiPolygon      : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XY  ;	return( true );

	case SG_OGIS_TYPE_PointZ            : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPointZ       : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_LineStringZ       : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiLineStringZ  : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_PolygonZ          : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPolygonZ     : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );

	case SG_OGIS_TYPE_PointM            : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPointM       : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_LineStringM       : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiLineStringM  : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_PolygonM          : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );
	case SG_OGIS_TYPE_MultiPolygonM     : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZ ;	return( true );

	case SG_OGIS_TYPE_PointZM           : Shape = SHAPE_TYPE_Point  ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_MultiPointZM      : Shape = SHAPE_TYPE_Points ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_LineStringZM      : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_MultiLineStringZM : Shape = SHAPE_TYPE_Line   ; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_PolygonZM         : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	case SG_OGIS_TYPE_MultiPolygonZM    : Shape = SHAPE_TYPE_Polygon; Vertex = SG_VERTEX_TYPE_XYZM;	return( true );
	}

	Shape = SHAPE_TYPE_Undefined; Vertex = SG_VERTEX_TYPE_XY;

	return( false );
}

//---------------------------------------------------------
TSG_Shape_Type CSG_Shapes_OGIS_Converter::to_ShapeType(const CSG_String &Type)
{
	TSG_Shape_Type Geometry; TSG_Vertex_Type Vertex; to_ShapeType(Type, Geometry, Vertex);

	return( Geometry );
}

//---------------------------------------------------------
TSG_Shape_Type CSG_Shapes_OGIS_Converter::to_ShapeType(DWORD Type)
{
	TSG_Shape_Type Geometry; TSG_Vertex_Type Vertex; to_ShapeType(Type, Geometry, Vertex);

	return( Geometry );
}

//---------------------------------------------------------
TSG_Vertex_Type CSG_Shapes_OGIS_Converter::to_VertexType(const CSG_String &Type)
{
	TSG_Shape_Type Geometry; TSG_Vertex_Type Vertex; to_ShapeType(Type, Geometry, Vertex);

	return( Vertex );
}

//---------------------------------------------------------
TSG_Vertex_Type CSG_Shapes_OGIS_Converter::to_VertexType(DWORD Type)
{
	TSG_Shape_Type Geometry; TSG_Vertex_Type Vertex; to_ShapeType(Type, Geometry, Vertex);

	return( Vertex );
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
// | endianness    | byte        | 1:ndr/little endian          |
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

	bool   bSwap   = Bytes.Read_Byte  () == 0; // endianness: 1=ndr/little endian, 0=xdr/big endian
	short  version = Bytes.Read_Short (bSwap); // version
	short  nBands  = Bytes.Read_Short (bSwap); // number of bands
	double dx      = Bytes.Read_Double(bSwap); // scaleX
	double dy      = Bytes.Read_Double(bSwap); // scaleY
	double xMin    = Bytes.Read_Double(bSwap); // ipX
	double yMax    = Bytes.Read_Double(bSwap); // ipY
	double skewX   = Bytes.Read_Double(bSwap); // skewX
	double skewY   = Bytes.Read_Double(bSwap); // skewY
	int    SRID    = Bytes.Read_Int   (bSwap); // srid
	short  NX      = Bytes.Read_Short (bSwap); // width
	short  NY      = Bytes.Read_Short (bSwap); // height

	//-----------------------------------------------------
	// Band

	TSG_Data_Type Type; BYTE Flags = Bytes.Read_Byte();

	switch( Flags & 0x0F )
	{
	case  0: Type = SG_DATATYPE_Bit   ; break; //  0:  1-bit boolean
	case  1: Type = SG_DATATYPE_Char  ; break; //  1:  2-bit unsigned integer
	case  2: Type = SG_DATATYPE_Char  ; break; //  2:  4-bit unsigned integer
	case  3: Type = SG_DATATYPE_Char  ; break; //  3:  8-bit   signed integer
	case  4: Type = SG_DATATYPE_Byte  ; break; //  4:  8-bit unsigned integer
	case  5: Type = SG_DATATYPE_Short ; break; //  5: 16-bit   signed integer
	case  6: Type = SG_DATATYPE_Word  ; break; //  6: 16-bit unsigned integer
	case  7: Type = SG_DATATYPE_Int   ; break; //  7: 32-bit   signed integer
	case  8: Type = SG_DATATYPE_DWord ; break; //  8: 32-bit unsigned integer
	case 10: Type = SG_DATATYPE_Float ; break; // 10: 32-bit float
	case 11: Type = SG_DATATYPE_Double; break; // 11: 64-bit float
	}

//	Flags |= 0x80; // isOffline: no, never here!
	Flags |= 0x40; // hasNodataValue
//	Flags |= 0x20; // isNoDataValue: no, never here!
//	Flags |= 0x10; // reserved (unused)

	if( !pGrid->Create(Type, NX, NY, dx, xMin + 0.5 * dx, yMax - (NY - 0.5) * dx) )
	{
		return( false );
	}

	pGrid->Get_Projection().Create(SRID);

	double noData;

	switch( pGrid->Get_Type() )
	{
	case SG_DATATYPE_Bit   : noData	= Bytes.Read_Byte  (     ); break; //  0:  1-bit boolean
	case SG_DATATYPE_Char  : noData	= Bytes.Read_Char  (     ); break; //  3:  8-bit   signed integer
	case SG_DATATYPE_Byte  : noData	= Bytes.Read_Byte  (     ); break; //  4:  8-bit unsigned integer
	case SG_DATATYPE_Short : noData	= Bytes.Read_Short (bSwap); break; //  5: 16-bit   signed integer
	case SG_DATATYPE_Word  : noData	= Bytes.Read_Word  (bSwap); break; //  6: 16-bit unsigned integer
	case SG_DATATYPE_Int   : noData	= Bytes.Read_Int   (bSwap); break; //  7: 32-bit   signed integer
	case SG_DATATYPE_DWord : noData	= Bytes.Read_DWord (bSwap); break; //  8: 32-bit unsigned integer
	case SG_DATATYPE_Float : noData	= Bytes.Read_Float (bSwap); break; //  9: 32-bit float
	case SG_DATATYPE_Double: noData	= Bytes.Read_Double(bSwap); break; // 10: 64-bit float
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
			case SG_DATATYPE_Bit   : pGrid->Set_Value(x, y, Bytes.Read_Byte  (     )); break; //  0:  1-bit boolean
			case SG_DATATYPE_Char  : pGrid->Set_Value(x, y, Bytes.Read_Char  (     )); break; //  3:  8-bit   signed integer
			case SG_DATATYPE_Byte  : pGrid->Set_Value(x, y, Bytes.Read_Byte  (     )); break; //  4:  8-bit unsigned integer
			case SG_DATATYPE_Short : pGrid->Set_Value(x, y, Bytes.Read_Short (bSwap)); break; //  5: 16-bit   signed integer
			case SG_DATATYPE_Word  : pGrid->Set_Value(x, y, Bytes.Read_Word  (bSwap)); break; //  6: 16-bit unsigned integer
			case SG_DATATYPE_Int   : pGrid->Set_Value(x, y, Bytes.Read_Int   (bSwap)); break; //  7: 32-bit   signed integer
			case SG_DATATYPE_DWord : pGrid->Set_Value(x, y, Bytes.Read_DWord (bSwap)); break; //  8: 32-bit unsigned integer
			case SG_DATATYPE_Float : pGrid->Set_Value(x, y, Bytes.Read_Float (bSwap)); break; //  9: 32-bit float
			case SG_DATATYPE_Double: pGrid->Set_Value(x, y, Bytes.Read_Double(bSwap)); break; // 10: 64-bit float
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
		SRID = pGrid->Get_Projection().Get_EPSG();
	}

	Bytes += (BYTE  )1                    ; // endianness
	Bytes += (short )0                    ; // version
	Bytes += (short )1                    ; // number of bands
	Bytes += (double)pGrid->Get_Cellsize(); // scaleX
	Bytes += (double)pGrid->Get_Cellsize(); // scaleY
	Bytes += (double)pGrid->Get_XMin(true); // ipX
	Bytes += (double)pGrid->Get_YMax(true); // ipY
	Bytes += (double)0.                   ; // skewX
	Bytes += (double)0.                   ; // skewY
	Bytes += (int   )SRID                 ; // srid
	Bytes += (short )pGrid->Get_NX()      ; // width
	Bytes += (short )pGrid->Get_NY()      ; // height

	//-----------------------------------------------------
	// Band

	BYTE	Flags;

	switch( pGrid->Get_Type() )
	{
	case SG_DATATYPE_Bit   : Flags =  0; break; //  0:  1-bit boolean
//	case SG_DATATYPE_      : Flags =  1; break; //  1:  2-bit unsigned integer
//	case SG_DATATYPE_      : Flags =  2; break; //  2:  4-bit unsigned integer
	case SG_DATATYPE_Char  : Flags =  3; break; //  3:  8-bit   signed integer
	case SG_DATATYPE_Byte  : Flags =  4; break; //  4:  8-bit unsigned integer
	case SG_DATATYPE_Short : Flags =  5; break; //  5: 16-bit   signed integer
	case SG_DATATYPE_Word  : Flags =  6; break; //  6: 16-bit unsigned integer
	case SG_DATATYPE_Int   : Flags =  7; break; //  7: 32-bit   signed integer
	case SG_DATATYPE_DWord : Flags =  8; break; //  8: 32-bit unsigned integer
	default                : Flags = 10; break; // 10: 32-bit float
	case SG_DATATYPE_Double: Flags = 11; break; // 11: 64-bit float
	}

//	Flags |= 0x80; // isOffline: no, never here!
	Flags |= 0x40; // hasNodataValue
//	Flags |= 0x20; // isNoDataValue: no, never here!
//	Flags |= 0x10; // reserved (unused)

	Bytes += Flags;

	switch( pGrid->Get_Type() )
	{
	case SG_DATATYPE_Bit   : Bytes += (BYTE  )0                        ; break; //  0:  1-bit boolean
	case SG_DATATYPE_Char  : Bytes += (char  )pGrid->Get_NoData_Value(); break; //  3:  8-bit   signed integer
	case SG_DATATYPE_Byte  : Bytes += (BYTE  )pGrid->Get_NoData_Value(); break; //  4:  8-bit unsigned integer
	case SG_DATATYPE_Short : Bytes += (short )pGrid->Get_NoData_Value(); break; //  5: 16-bit   signed integer
	case SG_DATATYPE_Word  : Bytes += (WORD  )pGrid->Get_NoData_Value(); break; //  6: 16-bit unsigned integer
	case SG_DATATYPE_Int   : Bytes += (int   )pGrid->Get_NoData_Value(); break; //  7: 32-bit   signed integer
	case SG_DATATYPE_DWord : Bytes += (DWORD )pGrid->Get_NoData_Value(); break; //  8: 32-bit unsigned integer
	default                : Bytes += (float )pGrid->Get_NoData_Value(); break; //  9: 32-bit float
	case SG_DATATYPE_Double: Bytes += (double)pGrid->Get_NoData_Value(); break; // 10: 64-bit float
	}

	for(int y=0; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			double Value = pGrid->is_NoData(x, y) ? pGrid->Get_NoData_Value() : pGrid->asDouble(x, y);

			switch( pGrid->Get_Type() )
			{
			case SG_DATATYPE_Bit   : Bytes += (BYTE  )Value; break; //  0:  1-bit boolean
			case SG_DATATYPE_Char  : Bytes += (char  )Value; break; //  3:  8-bit   signed integer
			case SG_DATATYPE_Byte  : Bytes += (BYTE  )Value; break; //  4:  8-bit unsigned integer
			case SG_DATATYPE_Short : Bytes += (short )Value; break; //  5: 16-bit   signed integer
			case SG_DATATYPE_Word  : Bytes += (WORD  )Value; break; //  6: 16-bit unsigned integer
			case SG_DATATYPE_Int   : Bytes += (int   )Value; break; //  7: 32-bit   signed integer
			case SG_DATATYPE_DWord : Bytes += (DWORD )Value; break; //  8: 32-bit unsigned integer
			default                : Bytes += (float )Value; break; //  9: 32-bit float
			case SG_DATATYPE_Double: Bytes += (double)Value; break; // 10: 64-bit float
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
