
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
//                        tin.h                          //
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
#ifndef HEADER_INCLUDED__SAGA_API__tin_H
#define HEADER_INCLUDED__SAGA_API__tin_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"

//---------------------------------------------------------
class CSG_TIN_Triangle;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TIN_Point
{
	friend class CSG_TIN;

public:

	int							Get_ID					(void)			{	return( m_ID );			}

	const TSG_Point &			Get_Point				(void)			{	return( m_Point );		}

	double						Get_X					(void)			{	return( m_Point.x );	}
	double						Get_Y					(void)			{	return( m_Point.y );	}

	CSG_Table_Record *			Get_Record				(void)			{	return( m_pRecord );	}

	bool						Set_Value				(int iField, const SG_Char *Value)		{	return( m_pRecord->Set_Value(iField, Value) );	}
	bool						Set_Value				(int iField, double         Value)		{	return( m_pRecord->Set_Value(iField, Value) );	}
	const SG_Char *				asString				(int iField, int Decimals = -1)	const	{	return( m_pRecord->asString(iField, Decimals) );	}
	SG_Char						asChar					(int iField)					const	{	return( m_pRecord->asChar  (iField) );	}
	short						asShort					(int iField)					const	{	return( m_pRecord->asShort (iField) );	}
	int							asInt					(int iField)					const	{	return( m_pRecord->asInt   (iField) );	}
	float						asFloat					(int iField)					const	{	return( m_pRecord->asChar  (iField) );	}
	double						asDouble				(int iField)					const	{	return( m_pRecord->asChar  (iField) );	}

	int							Get_Neighbor_Count		(void)			{	return( m_nNeighbors );	}
	CSG_TIN_Point *				Get_Neighbor			(int iNeighbor)	{	return( iNeighbor >= 0 && iNeighbor < m_nNeighbors ? m_Neighbors[iNeighbor] : NULL );	}
	double						Get_Gradient			(int iNeighbor, int iField);

	int							Get_Triangle_Count		(void)			{	return( m_nTriangles );	}
	CSG_TIN_Triangle *			Get_Triangle			(int iTriangle)	{	return( iTriangle >= 0 && iTriangle < m_nTriangles ? m_Triangles[iTriangle] : NULL );	}

	bool						Get_Polygon				(CSG_Points &Points);
	double						Get_Polygon_Area		(void);


private:

	CSG_TIN_Point(void);
	CSG_TIN_Point(int ID, TSG_Point Point, CSG_Table_Record *pRecord);
	virtual ~CSG_TIN_Point(void);


	int							m_ID, m_nNeighbors, m_nTriangles;

	TSG_Point					m_Point;

	CSG_Table_Record			*m_pRecord;

	CSG_TIN_Point				**m_Neighbors;

	CSG_TIN_Triangle			**m_Triangles;


	bool						_Add_Neighbor			(CSG_TIN_Point *pNeighbor);
	bool						_Add_Triangle			(CSG_TIN_Triangle *pTriangle);

	bool						_Del_Relations			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TIN_Edge
{
	friend class CSG_TIN;

public:

	CSG_TIN_Point *				Get_Point				(int iPoint)	{	return( m_Points[iPoint % 2] );	}


private:

	CSG_TIN_Edge(CSG_TIN_Point *a, CSG_TIN_Point *b);
	virtual ~CSG_TIN_Edge(void);


	CSG_TIN_Point				*m_Points[2];

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TIN_Triangle
{
	friend class CSG_TIN;

public:

	CSG_TIN_Point *				Get_Point				(int iPoint)	{	return( m_Points[iPoint % 3] );	}

	const CSG_Rect &			Get_Extent				(void)			{	return( m_Extent );	}

	double						Get_Area				(void)			{	return( m_Area );	}

	bool						is_Containing			(const TSG_Point &Point);
	bool						is_Containing			(double x, double y);

	bool						Get_Gradient			(int zField, double &Decline, double &Azimuth);

	void						Get_CircumCircle		(TSG_Point &Point, double &Radius)	{	Point	= m_Center;	Radius	= m_Radius;	}
	double						Get_CircumCircle_Radius	(void)			{	return( m_Radius );	}
	TSG_Point					Get_CircumCircle_Point	(void)			{	return( m_Center );	}


private:

	CSG_TIN_Triangle(CSG_TIN_Point *a, CSG_TIN_Point *b, CSG_TIN_Point *c);
	virtual ~CSG_TIN_Triangle(void);


	double						m_Area, m_Radius;

	TSG_Point					m_Center;

	CSG_Rect					m_Extent;

	CSG_TIN_Point				*m_Points[3];

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TIN : public CSG_Data_Object
{
public:

	CSG_TIN(void);

								CSG_TIN					(const CSG_TIN &TIN);
	bool						Create					(const CSG_TIN &TIN);

								CSG_TIN					(CSG_Shapes *pShapes);
	bool						Create					(CSG_Shapes *pShapes);

								CSG_TIN					(const SG_Char *File_Name);
	bool						Create					(const SG_Char *File_Name);

	virtual ~CSG_TIN(void);

	virtual bool				Destroy					(void);

	virtual TSG_Data_Object_Type	Get_ObjectType		(void)		const	{	return( DATAOBJECT_TYPE_TIN );	}

	virtual bool				Assign					(CSG_Data_Object *pObject);

	virtual bool				Save					(const SG_Char *File_Name, int Format = 0);

	virtual bool				is_Valid				(void)		const	{	return( m_nPoints >= 3 );	}

	CSG_Table &					Get_Table				(void)				{	return( m_Table );		}

	bool						Update					(void);

	CSG_TIN_Point *				Add_Point				(TSG_Point Point, CSG_Table_Record *pRecord, bool bUpdateNow);
	bool						Del_Point				(int iPoint, bool bUpdateNow);

	int							Get_Point_Count			(void)		const	{	return( m_nPoints );	}
	CSG_TIN_Point *				Get_Point				(int Index)	const	{	return( Index >= 0 && Index < m_nPoints    ? m_Points[Index]    : NULL );	}

	int							Get_Edge_Count			(void)		const	{	return( m_nEdges );		}
	CSG_TIN_Edge *				Get_Edge				(int Index)	const	{	return( Index >= 0 && Index < m_nEdges     ? m_Edges[Index]     : NULL );	}

	int							Get_Triangle_Count		(void)		const	{	return( m_nTriangles );	}
	CSG_TIN_Triangle *			Get_Triangle			(int Index)	const	{	return( Index >= 0 && Index < m_nTriangles ? m_Triangles[Index] : NULL );	}

	CSG_Rect					Get_Extent				(void)				{	Update();	return( m_Extent );	}


protected:

	typedef struct
	{
		int						p1, p2;
	}
	TTIN_Edge;

	typedef struct
	{
		int						p1, p2, p3;
	}
	TTIN_Triangle;


protected:

	int							m_nPoints, m_nEdges, m_nTriangles;

	CSG_TIN_Point				**m_Points;

	CSG_TIN_Edge				**m_Edges;

	CSG_TIN_Triangle			**m_Triangles;

	CSG_Rect					m_Extent;

	CSG_Table					m_Table;


	void						_On_Construction		(void);

	bool						_Destroy_Points			(void);
	bool						_Destroy_Edges			(void);
	bool						_Destroy_Triangles		(void);

	virtual void				On_Update				(void);

	bool						_Add_Edge				(CSG_TIN_Point *a, CSG_TIN_Point *b);
	bool						_Add_Triangle			(CSG_TIN_Point *a, CSG_TIN_Point *b, CSG_TIN_Point *c);

	bool						_Triangulate			(void);
	bool						_Triangulate			(CSG_TIN_Point **Points, int nPoints, TTIN_Triangle *Triangles, int &nTriangles);
	int							_CircumCircle			(double xp, double yp, double x1, double y1, double x2, double y2, double x3, double y3, double *xc, double *yc, double *r);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe TIN construction */
SAGA_API_DLL_EXPORT CSG_TIN *	SG_Create_TIN			(void);

/** Safe TIN construction */
SAGA_API_DLL_EXPORT CSG_TIN *	SG_Create_TIN			(const CSG_TIN &TIN);

/** Safe TIN construction */
SAGA_API_DLL_EXPORT CSG_TIN *	SG_Create_TIN			(CSG_Shapes *pShapes);

/** Safe TIN construction */
SAGA_API_DLL_EXPORT CSG_TIN *	SG_Create_TIN			(const SG_Char *File_Name);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__tin_H
