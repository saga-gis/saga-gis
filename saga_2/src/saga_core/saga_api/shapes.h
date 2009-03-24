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
//                       shapes.h                        //
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
#ifndef HEADER_INCLUDED__SAGA_API__shapes_H
#define HEADER_INCLUDED__SAGA_API__shapes_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Shape_Type
{
	SHAPE_TYPE_Undefined		= 0,
	SHAPE_TYPE_Point,
	SHAPE_TYPE_Points,
	SHAPE_TYPE_Line,
	SHAPE_TYPE_Polygon
}
TSG_Shape_Type;

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const SG_Char *	SG_Get_ShapeType_Name	(TSG_Shape_Type Type);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape : public CSG_Table_Record
{
	friend class CSG_Shapes;

public:

	//-----------------------------------------------------
	virtual void				Destroy				(void);

	bool						Assign				(CSG_Shape *pShape, bool bAssign_Attributes = true);

	TSG_Shape_Type				Get_Type			(void);

	virtual bool				is_Valid			(void)											= 0;

	//-----------------------------------------------------
	virtual int					Add_Point			(double x, double y,             int iPart = 0)	= 0;
	virtual int					Ins_Point			(double x, double y, int iPoint, int iPart = 0)	= 0;
	virtual int					Set_Point			(double x, double y, int iPoint, int iPart = 0)	= 0;
	virtual int					Del_Point			(                    int iPoint, int iPart = 0)	= 0;

	int							Add_Point			(TSG_Point Point,                int iPart = 0);
	int							Ins_Point			(TSG_Point Point,    int iPoint, int iPart = 0);
	int							Set_Point			(TSG_Point Point,    int iPoint, int iPart = 0);

	virtual int					Del_Part			(int iPart)										= 0;
	virtual int					Del_Parts			(void)											= 0;

	virtual int					Get_Part_Count		(void)											= 0;
	int							Get_Point_Count		(void);
	virtual int					Get_Point_Count		(int iPart)										= 0;
	virtual TSG_Point			Get_Point			(int iPoint, int iPart = 0)						= 0;


	//-----------------------------------------------------
	virtual const CSG_Rect &	Get_Extent			(void)											= 0;
	virtual const CSG_Rect &	Get_Extent			(int iPart)	{	return( Get_Extent() );		}

	int							Intersects			(TSG_Rect Extent);

	virtual double				Get_Distance		(TSG_Point Point)								= 0;
	virtual double				Get_Distance		(TSG_Point Point, int iPart)					= 0;
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next)				= 0;
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart)	= 0;


protected:

	CSG_Shape(class CSG_Shapes *pOwner, int Index);
	virtual ~CSG_Shape(void);

	virtual bool				On_Assign			(CSG_Shape *pShape)								= 0;
	virtual int					On_Intersects		(TSG_Rect Extent)								= 0;

	virtual void				_Invalidate			(void);

};


///////////////////////////////////////////////////////////
//														 //
//						Point							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape_Point : public CSG_Shape
{
	friend class CSG_Shapes;

public:

	virtual bool				is_Valid			(void)												{	return( true );				}

	virtual int					Add_Point			(double x, double y,             int iPart = 0);
	virtual int					Ins_Point			(double x, double y, int iPoint, int iPart = 0)		{	return( Add_Point(x, y) );	}
	virtual int					Set_Point			(double x, double y, int iPoint, int iPart = 0)		{	return( Add_Point(x, y) );	}
	virtual int					Del_Point			(                    int iPoint, int iPart = 0)		{	return( -1 );				}

	virtual int					Del_Part			(int iPart)											{	return( -1 );				}
	virtual int					Del_Parts			(void)												{	return( -1 );				}

	virtual int					Get_Part_Count		(void)												{	return( 1 );				}
	virtual int					Get_Point_Count		(int iPart)											{	return( 1 );				}
	virtual TSG_Point			Get_Point			(int iPoint, int iPart = 0)							{	return( m_Point );			}

	virtual const CSG_Rect &	Get_Extent			(void);

	virtual double				Get_Distance		(TSG_Point Point)									{	return( SG_Get_Distance(Point, m_Point) );	}
	virtual double				Get_Distance		(TSG_Point Point, int iPart)						{	return( SG_Get_Distance(Point, m_Point) );	}
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next)					{	Next = m_Point; return( SG_Get_Distance(Point, m_Point) );	}
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart)		{	Next = m_Point; return( SG_Get_Distance(Point, m_Point) );	}


protected:

	CSG_Shape_Point(class CSG_Shapes *pOwner, int Index);
	virtual ~CSG_Shape_Point(void);


	TSG_Point					m_Point;


	virtual bool				On_Assign			(CSG_Shape *pShape);
	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Points							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape_Part
{
	friend class CSG_Shape_Points;
	friend class CSG_Shape_Line;
	friend class CSG_Shape_Polygon;

public:

	bool						Destroy				(void);

	bool						Assign				(CSG_Shape_Part *pPart);

	const CSG_Rect &			Get_Extent			(void)	{	_Update_Extent();	return( m_Extent );	}

	int							Get_Count			(void)	{	return( m_nPoints );	}

	TSG_Point					Get_Point			(int iPoint)
	{
		if( iPoint >= 0 && iPoint < m_nPoints )
		{
			return( m_Points[iPoint] );
		}

		return( CSG_Point(0.0, 0.0) );
	}

	int							Add_Point			(TSG_Point Point               )	{	return( Add_Point(Point.x, Point.y)         );	}
	int							Ins_Point			(TSG_Point Point,    int iPoint)	{	return( Ins_Point(Point.x, Point.y, iPoint) );	}
	int							Set_Point			(TSG_Point Point,    int iPoint)	{	return( Set_Point(Point.x, Point.y, iPoint) );	}

	int							Add_Point			(double x, double y            );
	int							Ins_Point			(double x, double y, int iPoint);
	int							Set_Point			(double x, double y, int iPoint);
	int							Del_Point			(                    int iPoint);


protected:

	CSG_Shape_Part(class CSG_Shape_Points *pOwner);
	virtual ~CSG_Shape_Part(void);


	bool						m_bUpdate;

	int							m_nPoints, m_nBuffer;

	TSG_Point					*m_Points;

	CSG_Rect					m_Extent;

	CSG_Shape_Points			*m_pOwner;


	bool						_Alloc_Memory		(int nPoints);

	virtual void				_Invalidate			(void);

	virtual void				_Update_Extent		(void);

};


///////////////////////////////////////////////////////////
//														 //
//						Points							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape_Points : public CSG_Shape
{
	friend class CSG_Shapes;
	friend class CSG_Shape_Part;

public:

	virtual void				Destroy				(void);

	virtual bool				is_Valid			(void)		{	return( m_nParts > 0 && m_pParts[0]->Get_Count() > 0 );	}

	virtual int					Add_Point			(double x, double y,             int iPart = 0);
	virtual int					Ins_Point			(double x, double y, int iPoint, int iPart = 0);
	virtual int					Set_Point			(double x, double y, int iPoint, int iPart = 0);
	virtual int					Del_Point			(                    int iPoint, int iPart = 0);

	virtual int					Del_Part			(int iPart);
	virtual int					Del_Parts			(void);

	virtual int					Get_Part_Count		(void)		{	return( m_nParts );		}
	virtual CSG_Shape_Part *	Get_Part			(int iPart)	{	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart] : NULL );	}
	virtual int					Get_Point_Count		(int iPart)	{	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Get_Count() : 0 );	}

	virtual TSG_Point			Get_Point			(int iPoint, int iPart = 0)
	{
		if( iPart >= 0 && iPart < m_nParts && iPoint >= 0 && iPoint < m_pParts[iPart]->Get_Count() )
		{
			return( m_pParts[iPart]->Get_Point(iPoint) );
		}

		return( CSG_Point(0.0, 0.0) );
	}

	virtual const CSG_Rect &	Get_Extent			(void)		{	_Update_Extent();	return( m_Extent );	}

	virtual double				Get_Distance		(TSG_Point Point);
	virtual double				Get_Distance		(TSG_Point Point, int iPart);
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next);
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart);


protected:

	CSG_Shape_Points(class CSG_Shapes *pOwner, int Index);
	virtual ~CSG_Shape_Points(void);


	bool						m_bUpdate;

	int							m_nParts;

	CSG_Rect					m_Extent;

	CSG_Shape_Part				**m_pParts;


	int							_Add_Part			(void);

	virtual CSG_Shape_Part *	_Get_Part			(void)	{	return( new CSG_Shape_Part(this) );	}

	virtual void				_Invalidate			(void)
	{
		if( !m_bUpdate )
		{
			m_bUpdate	= true;

			CSG_Shape::_Invalidate();
		}
	}

	void						_Update_Extent		(void);

	virtual bool				On_Assign			(CSG_Shape *pShape);

	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Line							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape_Line : public CSG_Shape_Points
{
	friend class CSG_Shapes;

public:

	virtual bool				is_Valid			(void)	{	return( m_nParts > 0 && m_pParts[0]->Get_Count() > 1 );	}

	double						Get_Length			(void);
	double						Get_Length			(int iPart);

	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart);


protected:

	CSG_Shape_Line(class CSG_Shapes *pOwner, int Index);
	virtual ~CSG_Shape_Line(void);

	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Polygon							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape_Polygon_Part : public CSG_Shape_Part
{
	friend class CSG_Shape_Polygon;

public:

	bool						is_Clockwise		(void)	{	_Update_Area();	return( m_bClockwise == 1 );	}

	double						Get_Perimeter		(void)	{	_Update_Area();	return( m_Perimeter );	}

	double						Get_Area			(void)	{	_Update_Area();	return( m_Area );	}

	const TSG_Point &			Get_Centroid		(void)	{	_Update_Area();	return( m_Centroid );	}

	bool						is_Containing		(const TSG_Point &Point);
	bool						is_Containing		(double x, double y);

	double						Get_Distance		(TSG_Point Point, TSG_Point &Next);


protected:

	CSG_Shape_Polygon_Part(class CSG_Shape_Points *pOwner);
	virtual ~CSG_Shape_Polygon_Part(void);


	int							m_bClockwise, m_bLake;

	double						m_Area, m_Perimeter;

	TSG_Point					m_Centroid;


	virtual void				_Invalidate			(void);

	void						_Update_Area		(void);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shape_Polygon : public CSG_Shape_Points
{
	friend class CSG_Shapes;

public:

	virtual bool				is_Valid			(void)		{	return( m_nParts > 0 && m_pParts[0]->Get_Count() > 2 );	}


	CSG_Shape_Polygon_Part *	Get_Polygon_Part	(int iPart)	{	return( (CSG_Shape_Polygon_Part *)Get_Part(iPart) );	}

	bool						is_Lake				(int iPart);

	bool						is_Clockwise		(int iPart);

	double						Get_Perimeter		(int iPart);
	double						Get_Perimeter		(void);

	double						Get_Area			(int iPart);
	double						Get_Area			(void);

	TSG_Point					Get_Centroid		(int iPart);
	TSG_Point					Get_Centroid		(void);

	bool						is_Containing		(const TSG_Point &Point, int iPart);
	bool						is_Containing		(const TSG_Point &Point);
	bool						is_Containing		(double x, double y, int iPart);
	bool						is_Containing		(double x, double y);

	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart);


protected:

	CSG_Shape_Polygon(class CSG_Shapes *pOwner, int Index);
	virtual ~CSG_Shape_Polygon(void);


	int							m_bUpdate_Lakes;


	virtual CSG_Shape_Part *	_Get_Part			(void)	{	return( new CSG_Shape_Polygon_Part(this) );	}

	virtual void				_Invalidate			(void);

	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Shapes							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Shape_Copy_Mode
{
	SHAPE_NO_COPY	= 0,
	SHAPE_COPY_GEOM,
	SHAPE_COPY_ATTR,
	SHAPE_COPY
}
TSG_ADD_Shape_Copy_Mode;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shapes : public CSG_Table
{
	friend class CSG_Shape;

public:

	CSG_Shapes(void);

									CSG_Shapes	(const CSG_Shapes &Shapes);
	bool							Create		(const CSG_Shapes &Shapes);

									CSG_Shapes	(const CSG_String &File_Name);
	bool							Create		(const CSG_String &File_Name);

									CSG_Shapes	(TSG_Shape_Type Type, const SG_Char *Name = NULL, CSG_Table *pStructure = NULL);
	bool							Create		(TSG_Shape_Type Type, const SG_Char *Name = NULL, CSG_Table *pStructure = NULL);

	virtual ~CSG_Shapes(void);

	virtual bool					Destroy					(void);

	virtual TSG_Data_Object_Type	Get_ObjectType			(void)	const			{	return( DATAOBJECT_TYPE_Shapes );	}

	virtual bool					Assign					(CSG_Data_Object *pObject);

	virtual bool					Save					(const CSG_String &File_Name, int Format = 0);

	virtual bool					is_Valid				(void)	const			{	return( m_Type != SHAPE_TYPE_Undefined && Get_Count() >= 0 );		}

	TSG_Shape_Type					Get_Type				(void)	const			{	return( m_Type );		}

	const CSG_Rect &				Get_Extent				(void)					{	Update();	return( m_Extent );	}

	//-----------------------------------------------------
	CSG_Shape *						Add_Shape				(CSG_Table_Record *pCopy = NULL, TSG_ADD_Shape_Copy_Mode mCopy = SHAPE_COPY);
	bool							Del_Shape				(int iShape);
	bool							Del_Shape				(CSG_Shape *pShape);
	bool							Del_Shapes				(void)					{	return( Del_Records() );	}

	CSG_Shape *						Get_Shape				(TSG_Point Point, double Epsilon = 0.0);
	CSG_Shape *						Get_Shape				(int iShape)	const	{	return( (CSG_Shape *)Get_Record(iShape) );	}
	CSG_Shape *						Get_Shape_byIndex		(int Index)		const	{	return( (CSG_Shape *)Get_Record_byIndex(Index) );	}

	//-----------------------------------------------------
	virtual CSG_Shape *				Get_Selection			(int Index = 0)			{	return( (CSG_Shape *)CSG_Table::Get_Selection(Index) );	};
	const CSG_Rect &				Get_Selection_Extent	(void);

	virtual bool					Select					(CSG_Shape *pShape = NULL, bool bInvert = false);
	virtual bool					Select					(TSG_Rect Extent         , bool bInvert = false);


protected:

	TSG_Shape_Type					m_Type;

	CSG_Rect						m_Extent, m_Extent_Selected;


	virtual void					_On_Construction		(void);

	virtual CSG_Table_Record *		_Get_New_Record			(int Index);

	virtual bool					On_Update				(void);

	bool							_Load_ESRI				(const CSG_String &File_Name);
	bool							_Save_ESRI				(const CSG_String &File_Name);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe shapes construction */
SAGA_API_DLL_EXPORT CSG_Shapes *	SG_Create_Shapes	(void);

/** Safe shapes construction */
SAGA_API_DLL_EXPORT CSG_Shapes *	SG_Create_Shapes	(const CSG_Shapes &Shapes);

/** Safe shapes construction */
SAGA_API_DLL_EXPORT CSG_Shapes *	SG_Create_Shapes	(const CSG_String &File_Name);

/** Safe shapes construction */
SAGA_API_DLL_EXPORT CSG_Shapes *	SG_Create_Shapes	(TSG_Shape_Type Type, const SG_Char *Name = NULL, CSG_Table *pStructure = NULL);


///////////////////////////////////////////////////////////
//														 //
//				Point Region QuadTree					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_PRQuadTree_Item
{
public:

	virtual bool			is_Leaf			(void)	const	{	return( false );	}
	virtual bool			is_Node			(void)	const	{	return( false );	}

	CSG_Rect				Get_Extent		(void)	const	{	return( CSG_Rect(m_xCenter - m_Size, m_yCenter - m_Size, m_xCenter + m_Size, m_yCenter + m_Size) );	}
	double					Get_xMin		(void)	const	{	return( m_xCenter - m_Size );	}
	double					Get_yMin		(void)	const	{	return( m_yCenter - m_Size );	}
	double					Get_xCenter		(void)	const	{	return( m_xCenter          );	}
	double					Get_yCenter		(void)	const	{	return( m_yCenter          );	}
	double					Get_xMax		(void)	const	{	return( m_xCenter + m_Size );	}
	double					Get_yMax		(void)	const	{	return( m_yCenter + m_Size );	}
	double					Get_Size		(void)	const	{	return( m_Size * 2.0       );	}

	bool					Contains		(double x, double y)	const
	{
		return(	m_xCenter - m_Size <= x && x < m_xCenter + m_Size
			&&	m_yCenter - m_Size <= y && y < m_yCenter + m_Size );
	}

	class CSG_PRQuadTree_Leaf *	asLeaf		(void)	const	{	return( (class CSG_PRQuadTree_Leaf *)this );	}
	class CSG_PRQuadTree_Node *	asNode		(void)	const	{	return( (class CSG_PRQuadTree_Node *)this );	}


protected:

	CSG_PRQuadTree_Item(double xCenter, double yCenter, double Size)
	{
		m_xCenter	= xCenter;
		m_yCenter	= yCenter;
		m_Size		= Size;
	}


	double					m_xCenter, m_yCenter, m_Size;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_PRQuadTree_Leaf : public CSG_PRQuadTree_Item
{
	friend class CSG_PRQuadTree_Node;

public:

	virtual bool			is_Leaf			(void)	const	{	return( true );		}

	const TSG_Point &		Get_Point		(void)	const	{	return( m_Point );		}
	double					Get_X			(void)	const	{	return( m_Point.x );	}
	double					Get_Y			(void)	const	{	return( m_Point.y );	}
	double					Get_Z			(void)	const	{	return( m_z );			}


protected:

	CSG_PRQuadTree_Leaf(double xCenter, double yCenter, double Size, double x, double y, double z)
		: CSG_PRQuadTree_Item(xCenter, yCenter, Size)
	{
		m_Point.x	= x;
		m_Point.y	= y;
		m_z			= z;
	}


	double					m_z;

	TSG_Point				m_Point;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_PRQuadTree_Node : public CSG_PRQuadTree_Item
{
	friend class CSG_PRQuadTree;

public:

	virtual bool			is_Node			(void)	const	{	return( true );		}

	CSG_PRQuadTree_Item *	Get_Child		(int i)	const	{	return( i >= 0 && i < 4 ? m_pChildren[i] : NULL );	}

	bool					Add_Point		(double x, double y, double z);


protected:

	CSG_PRQuadTree_Node(double xCenter, double yCenter, double Size);
	CSG_PRQuadTree_Node(CSG_PRQuadTree_Leaf *pLeaf);
	virtual ~CSG_PRQuadTree_Node(void);


	CSG_PRQuadTree_Item		*m_pChildren[4];

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_PRQuadTree
{
public:
	CSG_PRQuadTree(void);
	CSG_PRQuadTree(const TSG_Rect &Extent);
	CSG_PRQuadTree(CSG_Shapes *pShapes, int Attribute);
	virtual ~CSG_PRQuadTree(void);

	bool						Create					(const CSG_Rect &Extent);
	bool						Create					(CSG_Shapes *pShapes, int Attribute);
	void						Destroy					(void);

	bool						Add_Point				(double x, double y, double z);

	const CSG_PRQuadTree_Node &	Get_Root				(void) const	{	return( *m_pRoot );			}

	bool						is_Okay					(void) const	{	return( m_pRoot != NULL );	}

	bool						Get_Nearest_Point		(double x, double y, TSG_Point &Point, double &Value, double &Distance);

	int							Select_Nearest_Points	(double x, double y, int maxPoints, double Radius = 0.0, int iQuadrant = -1);

	int							Get_Selected_Count		(void) const	{	return( m_nSelected );		}

	bool						Get_Selected_Point		(int i, double &x, double &y, double &z) const
	{
		if( i >= 0 && i < m_nSelected )
		{
			x	= m_Selected[i][0];
			y	= m_Selected[i][1];
			z	= m_Selected[i][2];

			return( true );
		}

		return( false );
	}


private:

	int							m_nSelected;

	CSG_Matrix					m_Selected;

	CSG_PRQuadTree_Node			*m_pRoot;

	bool						_Quadrant_Contains		(double x, double y, int iQuadrant, const TSG_Point &p);
	bool						_Radius_Contains		(double x, double y, double r, const TSG_Point &p);
	bool						_Radius_Contains		(double x, double y, double r, int iQuadrant, const TSG_Point &p);
	bool						_Quadrant_Intersects	(double x, double y, int iQuadrant, CSG_PRQuadTree_Item *pItem);
	bool						_Radius_Intersects		(double x, double y, double r, CSG_PRQuadTree_Item *pItem);
	bool						_Radius_Intersects		(double x, double y, double r, int iQuadrant, CSG_PRQuadTree_Item *pItem);

	void						_Get_Nearest_Point		(CSG_PRQuadTree_Item *pItem, double x, double y, double &maxDistance, double Point[4]);
	void						_Get_Nearest_Points		(CSG_PRQuadTree_Item *pItem, double x, double y, double &maxDistance, double Radius, int iQuadrant);

};


///////////////////////////////////////////////////////////
//														 //
//					Search Engine						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shapes_Search
{
public:
	CSG_Shapes_Search(void);
	CSG_Shapes_Search(CSG_Shapes *pPoints);

	virtual ~CSG_Shapes_Search(void);

	bool						Create				(CSG_Shapes *pPoints);
	void						Destroy				(void);

	bool						is_Valid			(void)	{	return( m_nPoints > 0 );	}

	CSG_Shape *					Get_Point_Nearest	(double x, double y);
	CSG_Shape *					Get_Point_Nearest	(double x, double y, int iQuadrant);

	int							Select_Radius		(double x, double y, double Radius, bool bSort = false, int MaxPoints = -1, int iQuadrant = -1);
	int							Select_Quadrants	(double x, double y, double Radius, int MaxPoints, int MinPoints = 0);
	int							Get_Selected_Count	(void)	{	return( m_nSelected );		}

	CSG_Shape *					Get_Selected_Point	(int iSelected)
	{
		if( iSelected >= 0 && iSelected < m_nSelected )
		{
			return( m_nSelected == m_Selected_Idx.Get_Count()
				? m_Selected[m_Selected_Idx[iSelected]]
				: m_Selected               [iSelected]
			);
		}

		return( NULL );
	}


protected:

	bool						m_bDestroy;

	int							m_nPoints, m_nSelected, m_Selected_Buf;

	double						*m_Selected_Dst;

	TSG_Point					*m_Pos;

	CSG_Index					m_Idx, m_Selected_Idx;

	CSG_Shape					**m_Selected;

	CSG_Shapes					*m_pPoints;


	void						_On_Construction	(void);

	int							_Get_Index_Next		(double Position);
	int							_Get_Point_Nearest	(double x, double y, int iQuadrant);
	void						_Select_Add			(CSG_Shape *pPoint, double Distance);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__shapes_H
