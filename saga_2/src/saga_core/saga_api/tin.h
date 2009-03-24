
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TIN_Node : public CSG_Table_Record
{
	friend class CSG_TIN;

public:

	int							Get_ID					(void)			{	return( m_ID );			}

	const TSG_Point &			Get_Point				(void)			{	return( m_Point );		}
	double						Get_X					(void)			{	return( m_Point.x );	}
	double						Get_Y					(void)			{	return( m_Point.y );	}

	int							Get_Neighbor_Count		(void)			{	return( m_nNeighbors );	}
	CSG_TIN_Node *				Get_Neighbor			(int iNeighbor)	{	return( iNeighbor >= 0 && iNeighbor < m_nNeighbors ? m_Neighbors[iNeighbor] : NULL );	}
	double						Get_Gradient			(int iNeighbor, int iField);

	int							Get_Triangle_Count		(void)			{	return( m_nTriangles );	}
	class CSG_TIN_Triangle *	Get_Triangle			(int iTriangle)	{	return( iTriangle >= 0 && iTriangle < m_nTriangles ? m_Triangles[iTriangle] : NULL );	}

	bool						Get_Polygon				(CSG_Points &Points);
	double						Get_Polygon_Area		(void);


private:

	CSG_TIN_Node(CSG_TIN *pOwner, int Index);
	virtual ~CSG_TIN_Node(void);


	int							m_ID, m_nNeighbors, m_nTriangles;

	TSG_Point					m_Point;

	CSG_TIN_Node				**m_Neighbors;

	class CSG_TIN_Triangle		**m_Triangles;


	bool						_Add_Neighbor			(CSG_TIN_Node *pNeighbor);
	bool						_Add_Triangle			(class CSG_TIN_Triangle *pTriangle);

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

	CSG_TIN_Node *				Get_Node				(int iNode)	{	return( m_Nodes[iNode % 2] );	}


private:

	CSG_TIN_Edge(CSG_TIN_Node *a, CSG_TIN_Node *b);
	virtual ~CSG_TIN_Edge(void);


	CSG_TIN_Node				*m_Nodes[2];

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

	CSG_TIN_Node *				Get_Node				(int iNode)		{	return( m_Nodes[iNode % 3] );	}

	const CSG_Rect &			Get_Extent				(void)			{	return( m_Extent );	}

	double						Get_Area				(void)			{	return( m_Area );	}

	bool						is_Containing			(const TSG_Point &Point);
	bool						is_Containing			(double x, double y);

	bool						Get_Gradient			(int zField, double &Decline, double &Azimuth);

	void						Get_CircumCircle		(TSG_Point &Point, double &Radius)	{	Point	= m_Center;	Radius	= m_Radius;	}
	double						Get_CircumCircle_Radius	(void)			{	return( m_Radius );	}
	TSG_Point					Get_CircumCircle_Point	(void)			{	return( m_Center );	}


private:

	CSG_TIN_Triangle(CSG_TIN_Node *a, CSG_TIN_Node *b, CSG_TIN_Node *c);
	virtual ~CSG_TIN_Triangle(void);


	double						m_Area, m_Radius;

	TSG_Point					m_Center;

	CSG_Rect					m_Extent;

	CSG_TIN_Node				*m_Nodes[3];

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TIN : public CSG_Table
{
public:

	CSG_TIN(void);

									CSG_TIN		(const CSG_TIN &TIN);
	bool							Create		(const CSG_TIN &TIN);

									CSG_TIN		(CSG_Shapes *pShapes);
	bool							Create		(CSG_Shapes *pShapes);

									CSG_TIN		(const CSG_String &File_Name);
	bool							Create		(const CSG_String &File_Name);

	virtual ~CSG_TIN(void);

	virtual bool					Destroy					(void);

	virtual TSG_Data_Object_Type	Get_ObjectType			(void)	const			{	return( DATAOBJECT_TYPE_TIN );	}

	virtual bool					Assign					(CSG_Data_Object *pObject);

	virtual bool					Save					(const CSG_String &File_Name, int Format = 0);

	virtual bool					is_Valid				(void)	const			{	return( Get_Count() >= 3 );	}

	const CSG_Rect &				Get_Extent				(void)					{	Update();	return( m_Extent );	}


	//-----------------------------------------------------
	CSG_TIN_Node *					Add_Node				(TSG_Point Point, CSG_Table_Record *pRecord, bool bUpdateNow);
	bool							Del_Node				(int iNode, bool bUpdateNow);

	int								Get_Node_Count			(void)		const	{	return( Get_Count() );	}
	CSG_TIN_Node *					Get_Node				(int Index)	const	{	return( (CSG_TIN_Node *)Get_Record(Index) );	}

	int								Get_Edge_Count			(void)		const	{	return( m_nEdges );		}
	CSG_TIN_Edge *					Get_Edge				(int Index)	const	{	return( Index >= 0 && Index < m_nEdges     ? m_Edges[Index]     : NULL );	}

	int								Get_Triangle_Count		(void)		const	{	return( m_nTriangles );	}
	CSG_TIN_Triangle *				Get_Triangle			(int Index)	const	{	return( Index >= 0 && Index < m_nTriangles ? m_Triangles[Index] : NULL );	}


protected:

	typedef struct
	{
		int							p1, p2;
	}
	TTIN_Edge;

	typedef struct
	{
		int							p1, p2, p3;
	}
	TTIN_Triangle;


protected:

	int								m_nEdges, m_nTriangles;

	CSG_Rect						m_Extent;

	CSG_TIN_Edge					**m_Edges;

	CSG_TIN_Triangle				**m_Triangles;


	void							_On_Construction		(void);

	virtual CSG_Table_Record *		_Get_New_Record			(int Index);

	virtual bool					On_Update				(void);

	bool							_Destroy_Nodes			(void);
	bool							_Destroy_Edges			(void);
	bool							_Destroy_Triangles		(void);

	bool							_Add_Edge				(CSG_TIN_Node *a, CSG_TIN_Node *b);
	bool							_Add_Triangle			(CSG_TIN_Node *a, CSG_TIN_Node *b, CSG_TIN_Node *c);

	bool							_Triangulate			(void);
	bool							_Triangulate			(CSG_TIN_Node **Nodes, int nNodes, TTIN_Triangle *Triangles, int &nTriangles);
	int								_CircumCircle			(double xp, double yp, double x1, double y1, double x2, double y2, double x3, double y3, double *xc, double *yc, double *r);

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
SAGA_API_DLL_EXPORT CSG_TIN *	SG_Create_TIN			(const CSG_String &File_Name);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__tin_H
