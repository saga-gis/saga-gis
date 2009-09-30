
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
//                     shapes_ogis.h                     //
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
#ifndef HEADER_INCLUDED__SAGA_API_shapes_ogis_H
#define HEADER_INCLUDED__SAGA_API_shapes_ogis_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Bytes
{
public:
							CSG_Bytes		(void);
	bool					Create			(void);

							CSG_Bytes		(const CSG_Bytes &Bytes);
	bool					Create			(const CSG_Bytes &Bytes);

							CSG_Bytes		(const char *Bytes, int nBytes);
	bool					Create			(const char *Bytes, int nBytes);

	virtual ~CSG_Bytes(void);

	bool					Destroy			(void);

	int						Get_Count		(void)		const	{	return( m_nBytes );	}
	char *					Get_Bytes		(void)		const	{	return( m_Bytes );	}
	char *					Get_Bytes		(int i)		const	{	return( i >= 0 && i < m_nBytes ? m_Bytes + i : NULL );	}
	char					Get_Byte		(int i)		const	{	return( i >= 0 && i < m_nBytes ? m_Bytes[i]  : 0 );		}
	char					operator []		(int i)		const	{	return( Get_Byte(i) );	}

	bool					Assign			(const CSG_Bytes &Bytes);
	CSG_Bytes &				operator =		(const CSG_Bytes &Bytes)	{	Assign(Bytes);	return( *this );	}

	bool					Add				(const CSG_Bytes &Bytes);
	CSG_Bytes &				operator +=		(const CSG_Bytes &Bytes)	{	Add(Bytes);		return( *this );	}

	bool					Add				(void *Bytes, int nBytes, bool bSwapBytes);

	bool					Add				(char   Value)							{	return( Add(&Value, sizeof(Value), false) );	}
	CSG_Bytes &				operator +=		(char   Value)							{	Add(Value);		return( *this );	}
	bool					Add				(short  Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(short  Value)							{	Add(Value);		return( *this );	}
	bool					Add				(int    Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(int    Value)							{	Add(Value);		return( *this );	}
	bool					Add				(float  Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(float  Value)							{	Add(Value);		return( *this );	}
	bool					Add				(double Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(double Value)							{	Add(Value);		return( *this );	}

	char					asChar			(int i)	const;
	short					asShort			(int i)	const;
	int						asInt			(int i)	const;
	float					asFloat			(int i)	const;
	double					asDouble		(int i)	const;


private:

	int						m_nBytes;

	char					*m_Bytes;


	bool					_Inc_Array		(int nBytes);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_OGIS_Shapes_Converter
{
public:
	CSG_OGIS_Shapes_Converter(void);

	static bool				from_WKText			(const CSG_String &Text, CSG_Shape *pShape);
	static bool				to_WKText			(CSG_Shape *pShape, CSG_String &Text);

	static bool				from_WKBinary		(const CSG_Bytes &Bytes, CSG_Shape *pShape);
	static bool				to_WKBinary			(CSG_Shape *pShape, CSG_Bytes &Bytes);


private:

	static bool				_Read_Point			(const CSG_String &Text, TSG_Point &Point);
	static bool				_Read_Points		(const CSG_String &Text, CSG_Shape *pShape);
	static bool				_Read_Parts			(const CSG_String &Text, CSG_Shape *pShape);
	static bool				_Read_Polygon		(const CSG_String &Text, CSG_Shape *pShape);
	static bool				_Write_Point		(CSG_String &Text, const TSG_Point &Point);
	static bool				_Write_Points		(CSG_String &Text, CSG_Shape *pShape, int iPart);
	static bool				_Write_Parts		(CSG_String &Text, CSG_Shape *pShape);
	static bool				_Write_Polygon		(CSG_String &Text, CSG_Shape *pShape);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API_shapes_ogis_H
