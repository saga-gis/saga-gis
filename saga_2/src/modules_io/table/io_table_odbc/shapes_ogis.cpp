
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       saga_api                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    shapes_ogis.cpp                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "shapes_ogis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(void)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;
}

//---------------------------------------------------------
bool CSG_Bytes::Create(void)
{
	return( Destroy() );
}

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(const CSG_Bytes &Bytes)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;

	Create(Bytes);
}

bool CSG_Bytes::Create(const CSG_Bytes &Bytes)
{
	return( Assign(Bytes) );
}

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(const char *Bytes, int nBytes)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;

	Create(Bytes, nBytes);
}

bool CSG_Bytes::Create(const char *Bytes, int nBytes)
{
	Destroy();

	return( Add((void *)Bytes, nBytes, false) );
}

//---------------------------------------------------------
CSG_Bytes::~CSG_Bytes(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Bytes::Destroy(void)
{
	if( m_Bytes )
	{
		SG_Free(m_Bytes);

		m_Bytes		= NULL;
		m_nBytes	= 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Bytes::_Inc_Array(int nBytes)
{
	char	*Bytes	= (char *)SG_Realloc(m_Bytes, (m_nBytes + nBytes) * sizeof(char));

	if( Bytes )
	{
		m_Bytes		= Bytes;
		m_nBytes	+= nBytes;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Bytes::Assign(const CSG_Bytes &Bytes)
{
	Destroy();

	if( _Inc_Array(Bytes.m_nBytes) )
	{
		memcpy(m_Bytes, Bytes.m_Bytes, m_nBytes);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Bytes::Add(const CSG_Bytes &Bytes)
{
	return( Add(Bytes.m_Bytes, Bytes.m_nBytes, false) );
}

//---------------------------------------------------------
bool CSG_Bytes::Add(void *Bytes, int nBytes, bool bSwapBytes)
{
	int		Offset	= m_nBytes;

	if( _Inc_Array(nBytes) )
	{
		memcpy(m_Bytes + Offset, Bytes, nBytes);

		if( bSwapBytes )
		{
			SG_Swap_Bytes(m_Bytes + Offset, nBytes);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
char CSG_Bytes::asChar(int i)	const
{
	return( Get_Byte(i) );
}

short CSG_Bytes::asShort(int i)	const
{
	return( *((short  *)Get_Bytes(i)) );
}

int CSG_Bytes::asInt(int i)	const
{
	return( *((int    *)Get_Bytes(i)) );
}

float CSG_Bytes::asFloat(int i)	const
{
	return( *((float  *)Get_Bytes(i)) );
}

double CSG_Bytes::asDouble(int i)	const
{
	return( *((double *)Get_Bytes(i)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_OGIS_Shapes_Converter::CSG_OGIS_Shapes_Converter(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Read_Point(const CSG_String &Text, TSG_Point &Point)
{
	return( Text.asDouble(Point.x) && Text.AfterFirst(SG_T(' ')).asDouble(Point.y) );
}

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Read_Points(const CSG_String &Text, CSG_Shape *pShape)
{
	int			iPart	= pShape->Get_Part_Count();
	TSG_Point	Point;
	CSG_String	s(Text.AfterFirst('(').BeforeFirst(')'));

	while( s.Length() > 0 )
	{
		if( _Read_Point(s, Point) )
		{
			pShape->Add_Point(Point, iPart);
		}

		s	= s.AfterFirst(',');
	}

	return( pShape->Get_Point_Count(iPart) > 0 );
}

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Read_Parts(const CSG_String &Text, CSG_Shape *pShape)
{
	CSG_String	s	= Text.AfterFirst('(').BeforeLast(')');

	while( s.Length() > 0 )
	{
		_Read_Points(s, pShape);

		s	= s.AfterFirst(',');
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Read_Polygon(const CSG_String &Text, CSG_Shape *pShape)
{
	CSG_String	s(Text.AfterFirst('(').BeforeLast(')'));

	while( s.AfterFirst('(').Length() > 0 )
	{
		for(int i=0, Level=-1; i<(int)s.Length() && Level!=0; i++)
		{
			if( s[i] == '(' )
			{
				if( Level < 0 )
					Level	= 0;

				Level++;
			}
			else if( s[i] == ')' )
			{
				Level--;

				if( Level == 0 )
				{
					_Read_Parts(s.Left(i + 1), pShape);

					s	= s.Right(s.Length() - i);
				}
			}
		}
	}

	return( pShape->Get_Part_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_OGIS_Shapes_Converter::from_WKText(const CSG_String &Text, CSG_Shape *pShape)
{
	pShape->Del_Parts();

	switch( pShape->Get_Type() )
	{
	case SHAPE_TYPE_Point:
		if( !Text.BeforeFirst('(').Cmp(SG_T("POINT")) )
		{
			TSG_Point	Point;

			if( _Read_Point(Text.AfterFirst('(').BeforeFirst(')'), Point) )
			{
				pShape->Add_Point(Point);
			}
		}
		break;

	case SHAPE_TYPE_Points:
		if( !Text.BeforeFirst('(').Cmp(SG_T("MULTIPOINT")) )
		{
			_Read_Parts(Text, pShape);
		}
		break;

	case SHAPE_TYPE_Line:
		if( !Text.BeforeFirst('(').Cmp(SG_T("LINESTRING")) )
		{
			return( _Read_Points(Text, pShape) );
		}

		if( !Text.BeforeFirst('(').Cmp(SG_T("MULTILINESTRING")) )
		{
			_Read_Parts(Text, pShape);
		}
		break;

	case SHAPE_TYPE_Polygon:
		if( !Text.BeforeFirst('(').Cmp(SG_T("POLYGON")) )
		{
			_Read_Parts(Text, pShape);
		}

		if( !Text.BeforeFirst('(').Cmp(SG_T("MULTIPOLYGON")) )
		{
			_Read_Polygon(Text, pShape);
		}
		break;
	}

	return( pShape->Get_Part_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Write_Point(CSG_String &Text, const TSG_Point &Point)
{
	Text	+= CSG_String::Format(SG_T("%f %f"), Point.x, Point.y);

	return( true );
}

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Write_Points(CSG_String &Text, CSG_Shape *pShape, int iPart)
{
	Text	+= SG_T("(");

	for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
	{
		if( iPoint > 0 )
		{
			Text	+= SG_T(", ");
		}

		_Write_Point(Text, pShape->Get_Point(iPoint, iPart));
	}

	if( pShape->Get_Type() == SHAPE_TYPE_Polygon && CSG_Point(pShape->Get_Point(0, iPart)) != pShape->Get_Point(pShape->Get_Point_Count(iPart) -1, iPart) )
	{
		Text	+= SG_T(", ");

		_Write_Point(Text, pShape->Get_Point(0, iPart));
	}

	Text	+= SG_T(")");

	return( true );
}

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Write_Parts(CSG_String &Text, CSG_Shape *pShape)
{
	Text	+= SG_T("(");

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( iPart > 0 )
		{
			Text	+= SG_T(", ");
		}

		_Write_Points(Text, pShape, iPart);
	}

	Text	+= SG_T(")");

	return( true );
}

//---------------------------------------------------------
inline bool CSG_OGIS_Shapes_Converter::_Write_Polygon(CSG_String &Text, CSG_Shape *pShape)
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

			_Write_Points(Text, pShape, iPart);

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(jPart) && ((CSG_Shape_Polygon *)pShape)->is_Containing(pShape->Get_Point(0, jPart), iPart) )
				{
					Text	+= SG_T(", ");

					_Write_Points(Text, pShape, jPart);
				}
			}

			Text	+= SG_T(")");
		}
	}

	Text	+= SG_T(")");

	return( true );
}

//---------------------------------------------------------
bool CSG_OGIS_Shapes_Converter::to_WKText(CSG_Shape *pShape, CSG_String &Text)
{
	switch( pShape->Get_Type() )
	{
	default:
		return( false );

	case SHAPE_TYPE_Point:
		Text	= SG_T("POINT");
		_Write_Points(Text, pShape, 0);
		break;

	case SHAPE_TYPE_Points:
		Text	 = SG_T("MULTIPOINT");
		_Write_Parts(Text, pShape);
		break;

	case SHAPE_TYPE_Line:
		Text	 = SG_T("MULTILINESTRING");
		_Write_Parts(Text, pShape);
		break;

	case SHAPE_TYPE_Polygon:
		Text	 = SG_T("MULTIPOLYGON");
		_Write_Polygon(Text, pShape);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_OGIS_Shapes_Converter::from_WKBinary(const CSG_Bytes &Bytes, CSG_Shape *pShape)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_OGIS_Shapes_Converter::to_WKBinary(CSG_Shape *pShape, CSG_Bytes &Bytes)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
