
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
CSG_TIN * SG_Create_TIN(void)
{
	return( new CSG_TIN );
}

//---------------------------------------------------------
CSG_TIN * SG_Create_TIN(const CSG_TIN &TIN)
{
	return( new CSG_TIN(TIN) );
}

//---------------------------------------------------------
CSG_TIN * SG_Create_TIN(const SG_Char *File_Name)
{
	return( new CSG_TIN(File_Name) );
}

//---------------------------------------------------------
CSG_TIN * SG_Create_TIN(CSG_Shapes *pShapes)
{
	return( new CSG_TIN(pShapes) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_TIN::CSG_TIN(void)
	: CSG_Data_Object()
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(const CSG_TIN &TIN)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(TIN);
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(const SG_Char *File_Name)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(File_Name);
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(CSG_Shapes *pShapes)
	: CSG_Data_Object()
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
void CSG_TIN::_On_Construction(void)
{
	m_Points			= NULL;
	m_nPoints			= 0;

	m_Edges				= NULL;
	m_nEdges			= 0;

	m_Triangles			= NULL;
	m_nTriangles		= 0;

	m_Table.m_pOwner	= this;

	Set_Update_Flag();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::Create(const CSG_TIN &TIN)
{
	return( Assign((CSG_Data_Object *)&TIN) );
}

//---------------------------------------------------------
bool CSG_TIN::Create(const SG_Char *File_Name)
{
	CSG_Shapes	Shapes(File_Name);

	if( Create(&Shapes) )
	{
		Get_History().Add_Entry(LNG("[HST] Created from file"), File_Name);
		Get_History().Assign(Shapes.Get_History(), true);

		Set_File_Name(File_Name);
		Set_Modified(false);
		Set_Update_Flag();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_TIN::Create(CSG_Shapes *pShapes)
{
	int		iShape, iPart, iPoint;
	CSG_Shape	*pShape;

	if( pShapes )
	{
		Destroy();

		SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Create TIN from shapes"), pShapes->Get_Name()), true);

		Set_Name(pShapes->Get_Name());

		Get_History().Assign(pShapes->Get_History());

		m_Table._Create(pShapes);
		m_Table.Set_Name(pShapes->Get_Name());

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Add_Point(pShape->Get_Point(iPoint, iPart), pShape, false);
				}
			}
		}

		SG_UI_Process_Set_Ready();

		if( _Triangulate() )
		{
			SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

			return( true );
		}
	}

	SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_TIN::~CSG_TIN(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_TIN::Destroy(void)
{
	_Destroy_Triangles();
	_Destroy_Edges();
	_Destroy_Points();

	m_Table._Destroy();

	CSG_Data_Object::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::_Destroy_Points(void)
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
bool CSG_TIN::_Destroy_Edges(void)
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
bool CSG_TIN::_Destroy_Triangles(void)
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
bool CSG_TIN::Assign(CSG_Data_Object *pObject)
{
	int				i;
	CSG_TIN_Point		*pPoint;
	CSG_TIN_Triangle	*pTriangle;
	CSG_TIN			*pTIN;

	if(	pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		pTIN	= (CSG_TIN *)pObject;

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
bool CSG_TIN::Save(const SG_Char *File_Name, int Format)
{
	bool	bResult	= false;

	if( Get_Triangle_Count() > 0 )
	{
		switch( Format )
		{
		case 0:	default:
			{
				CSG_Shapes	Points;

				Points.Create(SHAPE_TYPE_Point, Get_Name(), &Get_Table());

				for(int i=0; i<Get_Point_Count(); i++)
				{
					Points.	Add_Shape(Get_Point(i)->Get_Record())
						->	Add_Point(Get_Point(i)->Get_Point());
				}

				bResult	= Points.Save(File_Name);
			}
			break;
		}
	}

	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(File_Name);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::Update(void)
{
	return( _Triangulate() );
}

//---------------------------------------------------------
CSG_TIN_Point * CSG_TIN::Add_Point(TSG_Point Point, CSG_Table_Record *pRecord, bool bUpdateNow)
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
	m_Points			= (CSG_TIN_Point **)SG_Realloc(m_Points, (m_nPoints + 1) * sizeof(CSG_TIN_Point *));
	m_Points[m_nPoints]	= new CSG_TIN_Point(m_nPoints, Point, m_Table._Add_Record(pRecord));
	m_nPoints++;

	Set_Update_Flag();

	if( bUpdateNow )
	{
		_Triangulate();
	}

	return( m_Points[m_nPoints - 1] );
}

//---------------------------------------------------------
bool CSG_TIN::Del_Point(int iPoint, bool bUpdateNow)
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

		m_Points	= (CSG_TIN_Point **)SG_Realloc(m_Points, m_nPoints * sizeof(CSG_TIN_Point *));

		m_Table._Del_Record(iPoint);

		Set_Update_Flag();

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
inline bool CSG_TIN::_Add_Edge(CSG_TIN_Point *a, CSG_TIN_Point *b)
{
	m_Edges		= (CSG_TIN_Edge **)SG_Realloc(m_Edges, (m_nEdges + 1) * sizeof(CSG_TIN_Edge *));
	m_Edges[m_nEdges++]	= new CSG_TIN_Edge(a, b);

	return( true );
}

//---------------------------------------------------------
bool CSG_TIN::_Add_Triangle(CSG_TIN_Point *a, CSG_TIN_Point *b, CSG_TIN_Point *c)
{
	CSG_TIN_Triangle	*pTriangle;

	m_Triangles	= (CSG_TIN_Triangle **)SG_Realloc(m_Triangles, (m_nTriangles + 1) * sizeof(CSG_TIN_Triangle *));
	m_Triangles[m_nTriangles++]	= pTriangle = new CSG_TIN_Triangle(a, b, c);

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
void CSG_TIN::On_Update(void)
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
