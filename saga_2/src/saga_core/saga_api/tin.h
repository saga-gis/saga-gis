
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
class CTIN_Triangle;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTIN_Point
{
	friend class CTIN;

public:

	int							Get_ID					(void)			{	return( m_ID );			}

	const TGEO_Point &			Get_Point				(void)			{	return( m_Point );		}

	double						Get_X					(void)			{	return( m_Point.x );	}
	double						Get_Y					(void)			{	return( m_Point.y );	}

	CTable_Record *				Get_Record				(void)			{	return( m_pRecord );	}

	int							Get_Neighbor_Count		(void)			{	return( m_nNeighbors );	}
	CTIN_Point *				Get_Neighbor			(int iNeighbor)	{	return( iNeighbor >= 0 && iNeighbor < m_nNeighbors ? m_Neighbors[iNeighbor] : NULL );	}
	double						Get_Gradient			(int iNeighbor, int iField);

	int							Get_Triangle_Count		(void)			{	return( m_nTriangles );	}
	CTIN_Triangle *				Get_Triangle			(int iTriangle)	{	return( iTriangle >= 0 && iTriangle < m_nTriangles ? m_Triangles[iTriangle] : NULL );	}

	bool						Get_Polygon				(TGEO_Point **ppPoints, int &nPoints);
	double						Get_Polygon_Area		(void);


private:

	CTIN_Point(void);
	CTIN_Point(int ID, TGEO_Point Point, CTable_Record *pRecord);
	virtual ~CTIN_Point(void);


	int							m_ID, m_nNeighbors, m_nTriangles;

	TGEO_Point					m_Point;

	CTable_Record				*m_pRecord;

	CTIN_Point					**m_Neighbors;

	CTIN_Triangle				**m_Triangles;


	bool						_Add_Neighbor			(CTIN_Point *pNeighbor);
	bool						_Add_Triangle			(CTIN_Triangle *pTriangle);

	bool						_Del_Relations			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTIN_Edge
{
	friend class CTIN;

public:

	CTIN_Point *				Get_Point				(int iPoint)	{	return( m_Points[iPoint % 2] );	}


private:

	CTIN_Edge(CTIN_Point *a, CTIN_Point *b);
	virtual ~CTIN_Edge(void);


	CTIN_Point					*m_Points[2];

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTIN_Triangle
{
	friend class CTIN;

public:

	CTIN_Point *				Get_Point				(int iPoint)	{	return( m_Points[iPoint % 3] );	}

	const CGEO_Rect &			Get_Extent				(void)			{	return( m_Extent );	}

	double						Get_Area				(void)			{	return( m_Area );	}

	bool						is_Containing			(const TGEO_Point &Point);
	bool						is_Containing			(double x, double y);

	bool						Get_Gradient			(int zField, double &Decline, double &Azimuth);

	void						Get_CircumCircle		(TGEO_Point &Point, double &Radius)	{	Point	= m_Center;	Radius	= m_Radius;	}
	double						Get_CircumCircle_Radius	(void)			{	return( m_Radius );	}
	TGEO_Point					Get_CircumCircle_Point	(void)			{	return( m_Center );	}


private:

	CTIN_Triangle(CTIN_Point *a, CTIN_Point *b, CTIN_Point *c);
	virtual ~CTIN_Triangle(void);


	double						m_Area, m_Radius;

	TGEO_Point					m_Center;

	CGEO_Rect					m_Extent;

	CTIN_Point					*m_Points[3];

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTIN : public CDataObject
{
public:

	CTIN(void);

								CTIN					(const CTIN &TIN);
	bool						Create					(const CTIN &TIN);

								CTIN					(CShapes *pShapes);
	bool						Create					(CShapes *pShapes);

								CTIN					(const char *File_Name);
	bool						Create					(const char *File_Name);

	virtual ~CTIN(void);

	virtual bool				Destroy					(void);

	virtual TDataObject_Type	Get_ObjectType			(void)			{	return( DATAOBJECT_TYPE_TIN );	}

	virtual bool				Assign					(CDataObject *pObject);

	virtual bool				Save					(const char *File_Name, int Format = 0);

	virtual bool				is_Valid				(void)			{	return( m_nPoints >= 3 );	}

	CTable &					Get_Table				(void)			{	return( m_Table );		}

	CTIN_Point *				Add_Point				(TGEO_Point Point, CTable_Record *pRecord, bool bUpdateNow);
	bool						Del_Point				(int iPoint, bool bUpdateNow);

	int							Get_Point_Count			(void)			{	return( m_nPoints );	}
	CTIN_Point *				Get_Point				(int Index)		{	return( Index >= 0 && Index < m_nPoints    ? m_Points[Index]    : NULL );	}

	int							Get_Edge_Count			(void)			{	return( m_nEdges );		}
	CTIN_Edge *					Get_Edge				(int Index)		{	return( Index >= 0 && Index < m_nEdges     ? m_Edges[Index]     : NULL );	}

	int							Get_Triangle_Count		(void)			{	return( m_nTriangles );	}
	CTIN_Triangle *				Get_Triangle			(int Index)		{	return( Index >= 0 && Index < m_nTriangles ? m_Triangles[Index] : NULL );	}

	CGEO_Rect					Get_Extent				(void)			{	_Extent_Update();	return( m_Extent );	}


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

	bool						m_bUpdate;

	int							m_nPoints, m_nEdges, m_nTriangles;

	CTIN_Point					**m_Points;

	CTIN_Edge					**m_Edges;

	CTIN_Triangle				**m_Triangles;

	CGEO_Rect					m_Extent;

	CTable						m_Table;


	void						_On_Construction		(void);

	bool						_Destroy_Points			(void);
	bool						_Destroy_Edges			(void);
	bool						_Destroy_Triangles		(void);

	void						_Extent_Update			(void);

	bool						_Add_Edge				(CTIN_Point *a, CTIN_Point *b);
	bool						_Add_Triangle			(CTIN_Point *a, CTIN_Point *b, CTIN_Point *c);

	bool						_Triangulate			(void);
	bool						_Triangulate			(CTIN_Point **Points, int nPoints, TTIN_Triangle *Triangles, int &nTriangles);
	int							_CircumCircle			(double xp, double yp, double x1, double y1, double x2, double y2, double x3, double y3, double *xc, double *yc, double *r);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe TIN construction */
SAGA_API_DLL_EXPORT CTIN *		API_Create_TIN			(void);

/** Safe TIN construction */
SAGA_API_DLL_EXPORT CTIN *		API_Create_TIN			(const CTIN &TIN);

/** Safe TIN construction */
SAGA_API_DLL_EXPORT CTIN *		API_Create_TIN			(CShapes *pShapes);

/** Safe TIN construction */
SAGA_API_DLL_EXPORT CTIN *		API_Create_TIN			(const char *File_Name);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__tin_H
