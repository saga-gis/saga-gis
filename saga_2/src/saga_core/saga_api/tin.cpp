
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
//                       tin.cpp                         //
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
#include "tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN * SG_Create_TIN(void)
{
	return( new CTIN );
}

//---------------------------------------------------------
CTIN * SG_Create_TIN(const CTIN &TIN)
{
	return( new CTIN(TIN) );
}

//---------------------------------------------------------
CTIN * SG_Create_TIN(const char *File_Name)
{
	return( new CTIN(File_Name) );
}

//---------------------------------------------------------
CTIN * SG_Create_TIN(CShapes *pShapes)
{
	return( new CTIN(pShapes) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN::CTIN(void)
	: CDataObject()
{
	_On_Construction();
}

//---------------------------------------------------------
CTIN::CTIN(const CTIN &TIN)
	: CDataObject()
{
	_On_Construction();

	Create(TIN);
}

//---------------------------------------------------------
CTIN::CTIN(const char *File_Name)
	: CDataObject()
{
	_On_Construction();

	Create(File_Name);
}

//---------------------------------------------------------
CTIN::CTIN(CShapes *pShapes)
	: CDataObject()
{
	_On_Construction();

	Create(pShapes);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTIN::_On_Construction(void)
{
	m_Points			= NULL;
	m_nPoints			= 0;

	m_Edges				= NULL;
	m_nEdges			= 0;

	m_Triangles			= NULL;
	m_nTriangles		= 0;

	m_bUpdate			= true;

	m_Table.m_pOwner	= this;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::Create(const CTIN &TIN)
{
	return( Assign((CDataObject *)&TIN) );
}

//---------------------------------------------------------
bool CTIN::Create(const char *File_Name)
{
	CShapes	Shapes(File_Name);

	if( Create(&Shapes) )
	{
		Get_History().Add_Entry(LNG("[HST] Created from file"), File_Name);
		Get_History().Assign(Shapes.Get_History(), true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CTIN::Create(CShapes *pShapes)
{
	int		iShape, iPart, iPoint;
	CShape	*pShape;

	if( pShapes )
	{
		Destroy();

		SG_Callback_Message_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Create T.I.N. from shapes"), pShapes->Get_Name()), true);

		Set_Name(pShapes->Get_Name());

		Get_History().Assign(pShapes->Get_History());

		m_Table._Create(&pShapes->Get_Table());
		m_Table.Set_Name(pShapes->Get_Name());

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && SG_Callback_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Add_Point(pShape->Get_Point(iPoint, iPart), pShape->Get_Record(), false);
				}
			}
		}

		SG_Callback_Process_Set_Ready();

		if( _Triangulate() )
		{
			SG_Callback_Message_Add(LNG("[MSG] okay"), false);

			return( true );
		}
	}

	SG_Callback_Message_Add(LNG("[MSG] failed"), false);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN::~CTIN(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CTIN::Destroy(void)
{
	_Destroy_Triangles();
	_Destroy_Edges();
	_Destroy_Points();

	m_Table._Destroy();

	CDataObject::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::_Destroy_Points(void)
{
	if( m_nPoints > 0 )
	{
		for(int i=0; i<m_nPoints; i++)
		{
			delete(m_Points[i]);
		}

		SG_Free(m_Points);
		m_Points		= NULL;
		m_nPoints		= 0;
	}

	return( true );
}

//---------------------------------------------------------
bool CTIN::_Destroy_Edges(void)
{
	if( m_nEdges > 0 )
	{
		for(int i=0; i<m_nEdges; i++)
		{
			delete(m_Edges[i]);
		}

		SG_Free(m_Edges);
		m_Edges			= NULL;
		m_nEdges		= 0;
	}

	return( true );
}

//---------------------------------------------------------
bool CTIN::_Destroy_Triangles(void)
{
	if( m_nTriangles > 0 )
	{
		for(int i=0; i<m_nTriangles; i++)
		{
			delete(m_Triangles[i]);
		}

		SG_Free(m_Triangles);
		m_Triangles		= NULL;
		m_nTriangles	= 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::Assign(CDataObject *pObject)
{
	int				i;
	CTIN_Point		*pPoint;
	CTIN_Triangle	*pTriangle;
	CTIN			*pTIN;

	if(	pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		pTIN	= (CTIN *)pObject;

		Destroy();

		Set_Name(pTIN->Get_Name());

		Get_History().Assign(pTIN->Get_History());

		m_Table._Create(&pTIN->Get_Table());
		m_Table.Set_Name(pTIN->Get_Name());

		//-------------------------------------------------
		for(i=0; i<pTIN->Get_Point_Count(); i++)
		{
			pPoint	= pTIN->Get_Point(i);

			Add_Point(pPoint->Get_Point(), pPoint->Get_Record(), false);
		}

		//-------------------------------------------------
		for(i=0; i<pTIN->Get_Triangle_Count(); i++)
		{
			pTriangle	= pTIN->Get_Triangle(i);

			_Add_Triangle(
				Get_Point(pTriangle->Get_Point(0)->Get_Record()->Get_Index()),
				Get_Point(pTriangle->Get_Point(1)->Get_Record()->Get_Index()),
				Get_Point(pTriangle->Get_Point(2)->Get_Record()->Get_Index())
			);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::Save(const char *File_Name, int Format)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::Update(void)
{
	return( _Triangulate() );
}

//---------------------------------------------------------
CTIN_Point * CTIN::Add_Point(TSG_Point Point, CTable_Record *pRecord, bool bUpdateNow)
{
/*	for(int i=0; i<m_nPoints; i++)
	{
		if( m_Points[i]->Get_X() == Point.x && m_Points[i]->Get_Y() == Point.y )
		{
			return( NULL );
		}
	}
*/

	//-----------------------------------------------------
	m_Points			= (CTIN_Point **)SG_Realloc(m_Points, (m_nPoints + 1) * sizeof(CTIN_Point *));
	m_Points[m_nPoints]	= new CTIN_Point(m_nPoints, Point, m_Table._Add_Record(pRecord));
	m_nPoints++;

	m_bUpdate			= true;

	if( bUpdateNow )
	{
		_Triangulate();
	}

	return( m_Points[m_nPoints - 1] );
}

//---------------------------------------------------------
bool CTIN::Del_Point(int iPoint, bool bUpdateNow)
{
	int		i;

	if( iPoint >= 0 && iPoint < m_nPoints )
	{
		delete(m_Points[iPoint]);

		m_nPoints--;

		for(i=iPoint; i<m_nPoints; i++)
		{
			m_Points[i]	= m_Points[i + 1];
		}

		m_Points	= (CTIN_Point **)SG_Realloc(m_Points, m_nPoints * sizeof(CTIN_Point *));

		m_Table._Del_Record(iPoint);

		m_bUpdate	= true;

		if( bUpdateNow )
		{
			_Triangulate();
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CTIN::_Add_Edge(CTIN_Point *a, CTIN_Point *b)
{
	m_Edges		= (CTIN_Edge **)SG_Realloc(m_Edges, (m_nEdges + 1) * sizeof(CTIN_Edge *));
	m_Edges[m_nEdges++]	= new CTIN_Edge(a, b);

	return( true );
}

//---------------------------------------------------------
bool CTIN::_Add_Triangle(CTIN_Point *a, CTIN_Point *b, CTIN_Point *c)
{
	CTIN_Triangle	*pTriangle;

	m_Triangles	= (CTIN_Triangle **)SG_Realloc(m_Triangles, (m_nTriangles + 1) * sizeof(CTIN_Triangle *));
	m_Triangles[m_nTriangles++]	= pTriangle = new CTIN_Triangle(a, b, c);

	if( a->_Add_Neighbor(b) )
	{
		b->_Add_Neighbor(a);
		_Add_Edge(a, b);
	}

	if( b->_Add_Neighbor(c) )
	{
		c->_Add_Neighbor(b);
		_Add_Edge(b, c);
	}

	if( c->_Add_Neighbor(a) )
	{
		a->_Add_Neighbor(c);
		_Add_Edge(c, a);
	}

	a->_Add_Triangle(pTriangle);
	b->_Add_Triangle(pTriangle);
	c->_Add_Triangle(pTriangle);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTIN::_Extent_Update(void)
{
	if( m_bUpdate )
	{
		if( m_nPoints > 0 )
		{
			TSG_Rect	r;

			m_Extent.Assign(
				m_Points[0]->Get_X(), m_Points[0]->Get_Y(),
				m_Points[0]->Get_X(), m_Points[0]->Get_Y()
			);

			for(int i=1; i<m_nPoints; i++)
			{
				r.xMin	= r.xMax	= m_Points[i]->Get_X();
				r.yMin	= r.yMax	= m_Points[i]->Get_Y();

				m_Extent.Union(r);
			}
		}
		else
		{
			m_Extent.Assign(0.0, 0.0, 0.0, 0.0);
		}

		m_bUpdate	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
