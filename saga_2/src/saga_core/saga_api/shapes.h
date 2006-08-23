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
typedef enum EShape_Type
{
	SHAPE_TYPE_Undefined		= 0,
	SHAPE_TYPE_Point,
	SHAPE_TYPE_Points,
	SHAPE_TYPE_Line,
	SHAPE_TYPE_Polygon
}
TShape_Type;

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const char *	SG_Get_ShapeType_Name	(TShape_Type Type);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShape
{
	friend class CShapes;

public:

	//-----------------------------------------------------
	virtual void				Destroy				(void);

	bool						Assign				(CShape *pShape, bool bAssign_Attributes = true);

	CTable_Record *				Get_Record			(void)		{	return( m_pRecord );	}

	TShape_Type					Get_Type			(void);

	virtual bool				is_Valid			(void)											= 0;

	bool						is_Selected			(void)		{	return( m_pRecord->is_Selected() );	}


	//-----------------------------------------------------
	virtual int					Add_Point			(double x, double y,             int iPart = 0)	= 0;
	virtual int					Ins_Point			(double x, double y, int iPoint, int iPart = 0)	= 0;
	virtual int					Set_Point			(double x, double y, int iPoint, int iPart = 0)	= 0;
	virtual int					Del_Point			(                    int iPoint, int iPart = 0)	= 0;

	int							Add_Point			(TSG_Point Point,               int iPart = 0);
	int							Ins_Point			(TSG_Point Point,   int iPoint, int iPart = 0);
	int							Set_Point			(TSG_Point Point,   int iPoint, int iPart = 0);

	virtual int					Del_Part			(int iPart)										= 0;
	virtual int					Del_Parts			(void)											= 0;

	virtual int					Get_Part_Count		(void)											= 0;
	virtual int					Get_Point_Count		(int iPart)										= 0;
	virtual TSG_Point			Get_Point			(int iPoint, int iPart = 0)						= 0;


	//-----------------------------------------------------
	virtual CSG_Rect			Get_Extent			(void)											= 0;

	int							Intersects			(TSG_Rect Extent);

	virtual double				Get_Distance		(TSG_Point Point)								= 0;
	virtual double				Get_Distance		(TSG_Point Point, int iPart)					= 0;
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next)				= 0;
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart)	= 0;


protected:

	CTable_Record				*m_pRecord;

	class CShapes				*m_pOwner;


	CShape(class CShapes *pOwner, CTable_Record *pRecord);
	virtual ~CShape(void);

	virtual bool				On_Assign			(CShape *pShape)								= 0;
	virtual int					On_Intersects		(TSG_Rect Extent)								= 0;

	virtual void				_Extent_Invalidate	(void);

};


///////////////////////////////////////////////////////////
//														 //
//						Point							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShape_Point : public CShape
{
	friend class CShapes;

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

	virtual CSG_Rect			Get_Extent			(void);

	virtual double				Get_Distance		(TSG_Point Point)									{	return( SG_Get_Distance(Point, m_Point) );	}
	virtual double				Get_Distance		(TSG_Point Point, int iPart)						{	return( SG_Get_Distance(Point, m_Point) );	}
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next)					{	Next = m_Point; return( SG_Get_Distance(Point, m_Point) );	}
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart)		{	Next = m_Point; return( SG_Get_Distance(Point, m_Point) );	}


protected:

	CShape_Point(class CShapes *pOwner, CTable_Record *pRecord);
	virtual ~CShape_Point(void);


	TSG_Point					m_Point;


	virtual bool				On_Assign			(CShape *pShape);
	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Points							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShape_Points : public CShape
{
	friend class CShapes;

public:

	virtual void				Destroy				(void);

	virtual bool				is_Valid			(void)	{	return( m_nParts > 0 && m_nPoints[0] > 0 );	}

	virtual int					Add_Point			(double x, double y,             int iPart = 0);
	virtual int					Ins_Point			(double x, double y, int iPoint, int iPart = 0);
	virtual int					Set_Point			(double x, double y, int iPoint, int iPart = 0);
	virtual int					Del_Point			(                    int iPoint, int iPart = 0);

	virtual int					Del_Part			(int iPart);
	virtual int					Del_Parts			(void);

	virtual int					Get_Part_Count		(void)
	{
		return( m_nParts );
	}

	virtual int					Get_Point_Count		(int iPart)
	{
		return( iPart >= 0 && iPart < m_nParts ? m_nPoints[iPart] : 0 );
	}

	virtual TSG_Point			Get_Point			(int iPoint, int iPart = 0)
	{
		if( iPart >= 0 && iPart < m_nParts && iPoint >= 0 && iPoint < m_nPoints[iPart] )
		{
			return( m_Points[iPart][iPoint] );
		}

		return( CSG_Point(0.0, 0.0) );
	}

	virtual CSG_Rect			Get_Extent			(void)	{	_Extent_Update();	return( m_Extent );	}

	virtual double				Get_Distance		(TSG_Point Point);
	virtual double				Get_Distance		(TSG_Point Point, int iPart);
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next);
	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart);


protected:

	CShape_Points(class CShapes *pOwner, CTable_Record *pRecord);
	virtual ~CShape_Points(void);


	bool						m_bUpdate;

	int							*m_nPoints, m_nParts;

	TSG_Point					**m_Points;

	CSG_Rect					m_Extent;


	int							_Add_Part			(void);

	virtual void				_Extent_Invalidate	(void)
	{
		if( !m_bUpdate )
		{
			m_bUpdate	= true;

			CShape::_Extent_Invalidate();
		}
	}

	void						_Extent_Update		(void);

	virtual bool				On_Assign			(CShape *pShape);

	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Line							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShape_Line : public CShape_Points
{
	friend class CShapes;

public:

	virtual bool				is_Valid			(void)	{	return( m_nParts > 0 && m_nPoints[0] > 1 );	}

	double						Get_Length			(void);
	double						Get_Length			(int iPart);

	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart);


protected:

	CShape_Line(class CShapes *pOwner, CTable_Record *pRecord);
	virtual ~CShape_Line(void);

	virtual int					On_Intersects		(TSG_Rect Region);

};


///////////////////////////////////////////////////////////
//														 //
//						Polygon							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShape_Polygon : public CShape_Points
{
	friend class CShapes;

public:

	virtual bool				is_Valid			(void)	{	return( m_nParts > 0 && m_nPoints[0] > 2 );	}


	bool						is_Containing		(const TSG_Point &Point);
	bool						is_Containing		(double x, double y);
	bool						is_Containing		(const TSG_Point &Point, int iPart);
	bool						is_Containing		(double x, double y, int iPart);

	bool						is_Clockwise		(int iPart);

	bool						is_Lake				(int iPart);

	double						Get_Perimeter		(void);
	double						Get_Perimeter		(int iPart);

	double						Get_Area			(void);
	double						Get_Area			(int iPart);

	TSG_Point					Get_Centroid		(void);
	TSG_Point					Get_Centroid		(int iPart);

	virtual double				Get_Distance		(TSG_Point Point, TSG_Point &Next, int iPart);


protected:

	CShape_Polygon(class CShapes *pOwner, CTable_Record *pRecord);
	virtual ~CShape_Polygon(void);

	virtual int					On_Intersects		(TSG_Rect Region);

	double						_Get_Area			(int iPart);

};


///////////////////////////////////////////////////////////
//														 //
//						Shapes							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShapes : public CDataObject
{
	friend class CShape;

public:

	CShapes(void);

								CShapes	(const CShapes &Shapes);
	bool						Create	(const CShapes &Shapes);

								CShapes	(const char *File_Name);
	bool						Create	(const char *File_Name);

								CShapes	(TShape_Type Type, const char *Name = NULL, CTable *pStructure = NULL);
	bool						Create	(TShape_Type Type, const char *Name = NULL, CTable *pStructure = NULL);

	virtual ~CShapes(void);

	virtual bool				Destroy					(void);

	virtual TDataObject_Type	Get_ObjectType			(void)	const		{	return( DATAOBJECT_TYPE_Shapes );	}

	virtual bool				Assign					(CDataObject *pObject);

	virtual bool				Save					(const char *File_Name, int Format = 0);

	virtual bool				is_Valid				(void)	const		{	return( m_Type != SHAPE_TYPE_Undefined && m_nShapes >= 0 );				}

	TShape_Type					Get_Type				(void)	const		{	return( m_Type );		}

	CTable &					Get_Table				(void)				{	return( m_Table );		}

	CSG_Rect					Get_Extent				(void)				{	_Extent_Update();	return( m_Extent );	}

	//-----------------------------------------------------
	CShape *					Add_Shape				(CTable_Record *pValues = NULL);
	bool						Del_Shape				(int iShape);
	bool						Del_Shape				(CShape *pShape);

	int							Get_Count				(void)	const		{	return( m_nShapes );	}
	CShape *					Get_Shape				(int iShape)		{	return( iShape >= 0 && iShape < m_nShapes ? m_Shapes[iShape] : NULL );	}
	int							Get_Shape_Index			(CShape *pShape)	{	return( pShape ? pShape->Get_Record()->Get_Index() : -1 );	}

	CShape *					Get_Shape				(TSG_Point Point, double Epsilon = 0.0);

	//-----------------------------------------------------
	int							Get_Selection_Count		(void)				{	return( m_Table.m_nSelected );	}
	CShape *					Get_Selection			(int Index = 0);
	const CSG_Rect &			Get_Selection_Extent	(void);

	bool						Select					(TSG_Rect Extent		, bool bInvert = false);
	bool						Select					(int iShape				, bool bInvert = false);
	bool						Select					(CShape *pShape = NULL	, bool bInvert = false);

	int							Del_Selection			(void);


protected:

	bool						m_bUpdate;

	int							m_nShapes;

	TShape_Type					m_Type;

	CSG_Rect					m_Extent, m_Extent_Selected;

	CTable						m_Table;

	CShape						**m_Shapes;


	void						_On_Construction		(void);

	void						_Extent_Invalidate		(void)				{	m_bUpdate	= true;	}
	void						_Extent_Update			(void);

	CShape *					_Add_Shape				(CTable_Record *pRecord);

	bool						_Load_ESRI				(const char *File_Name);
	bool						_Save_ESRI				(const char *File_Name);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe shapes construction */
SAGA_API_DLL_EXPORT CShapes *	SG_Create_Shapes	(void);

/** Safe shapes construction */
SAGA_API_DLL_EXPORT CShapes *	SG_Create_Shapes	(const CShapes &Shapes);

/** Safe shapes construction */
SAGA_API_DLL_EXPORT CShapes *	SG_Create_Shapes	(const char *FileName);

/** Safe shapes construction */
SAGA_API_DLL_EXPORT CShapes *	SG_Create_Shapes	(TShape_Type Type, char *Name = NULL, CTable *pStructure = NULL);


///////////////////////////////////////////////////////////
//														 //
//					Search Engine						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CShapes_Search
{
public:
	CShapes_Search(void);
	CShapes_Search(CShapes *pPoints);

	virtual ~CShapes_Search(void);

	bool						Create				(CShapes *pPoints);
	void						Destroy				(void);

	bool						is_Valid			(void)	{	return( m_nPoints > 0 );	}

	CShape *					Get_Point_Nearest	(double x, double y);
	CShape *					Get_Point_Nearest	(double x, double y, int iQuadrant);

	int							Select_Radius		(double x, double y, double Radius, bool bSort = false, int MaxPoints = -1);
	int							Get_Selected_Count	(void)	{	return( m_nSelected );		}

	CShape *					Get_Selected_Point	(int iSelected)
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

	CShape						**m_Selected;

	CShapes						*m_pPoints;


	void						_On_Construction	(void);

	int							_Get_Index_Next		(double Position);
	int							_Get_Point_Nearest	(double x, double y, int iQuadrant);
	void						_Select_Add			(CShape *pPoint, double Distance);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__shapes_H
