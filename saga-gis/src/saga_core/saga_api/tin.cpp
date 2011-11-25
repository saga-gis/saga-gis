/**********************************************************
 * Version $Id$
 *********************************************************/

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
CSG_TIN * SG_Create_TIN(const CSG_String &File_Name)
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
	: CSG_Table()
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(const CSG_TIN &TIN)
	: CSG_Table()
{
	_On_Construction();

	Create(TIN);
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(const CSG_String &File_Name)
	: CSG_Table()
{
	_On_Construction();

	Create(File_Name);
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(CSG_Shapes *pShapes)
	: CSG_Table()
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
	CSG_Table::_On_Construction();

	m_Edges				= NULL;
	m_nEdges			= 0;

	m_Triangles			= NULL;
	m_nTriangles		= 0;
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
bool CSG_TIN::Create(const CSG_String &File_Name)
{
	CSG_Shapes	Shapes(File_Name);

	if( Create(&Shapes) )
	{
		Get_History().Add_Child(_TL("[HST] Created from file"), File_Name);
		Get_History()	+= Shapes.Get_History();

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
	Destroy();

	if( pShapes && pShapes->is_Valid() )
	{
		SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), _TL("[MSG] Create TIN from shapes"), pShapes->Get_Name()), true);

		_Create(pShapes);

		Set_Name(pShapes->Get_Name());

		//-------------------------------------------------
		for(int iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Add_Node(pShape->Get_Point(iPoint, iPart), pShape, false);
				}
			}
		}

		SG_UI_Process_Set_Ready();

		if( Update() )
		{
			SG_UI_Msg_Add(_TL("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

			return( true );
		}
	}

	SG_UI_Msg_Add(_TL("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

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
	_Destroy_Nodes();

	CSG_Table::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::_Destroy_Nodes(void)
{
	return( Del_Records() );
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
	if(	pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		CSG_TIN	*pTIN	= (CSG_TIN *)pObject;

		Destroy();

		_Create(pTIN);

		Set_Name(pTIN->Get_Name());

		Get_History()	= pTIN->Get_History();

		//-------------------------------------------------
		for(int iNode=0; iNode<pTIN->Get_Node_Count(); iNode++)
		{
			CSG_TIN_Node	*pNode	= pTIN->Get_Node(iNode);

			Add_Node(pNode->Get_Point(), pNode, false);
		}

		//-------------------------------------------------
		for(int iTriangle=0; iTriangle<pTIN->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle	*pTriangle	= pTIN->Get_Triangle(iTriangle);

			_Add_Triangle(
				Get_Node(pTriangle->Get_Node(0)->Get_Index()),
				Get_Node(pTriangle->Get_Node(1)->Get_Index()),
				Get_Node(pTriangle->Get_Node(2)->Get_Index())
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
bool CSG_TIN::Save(const CSG_String &File_Name, int Format)
{
	bool	bResult	= false;

	if( Get_Triangle_Count() > 0 )
	{
		switch( Format )
		{
		case 0:	default:
			{
				CSG_Shapes	Points;

				Points.Create(SHAPE_TYPE_Point, Get_Name(), this);

				for(int i=0; i<Get_Node_Count(); i++)
				{
					CSG_TIN_Node	*pNode	= Get_Node(i);

					Points.Add_Shape(pNode)->Add_Point(pNode->Get_Point());
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
CSG_Table_Record * CSG_TIN::_Get_New_Record(int Index)
{
	return( new CSG_TIN_Node(this, Index) );
}

//---------------------------------------------------------
CSG_TIN_Node * CSG_TIN::Add_Node(TSG_Point Point, CSG_Table_Record *pRecord, bool bUpdateNow)
{
	CSG_TIN_Node	*pNode	= (CSG_TIN_Node *)Add_Record(pRecord);

	if( pNode )
	{
		pNode->m_Point	= Point;

		if( bUpdateNow )
		{
			Update();
		}
	}

	return( pNode );
}

//---------------------------------------------------------
bool CSG_TIN::Del_Node(int iNode, bool bUpdateNow)
{
	if( Del_Record(iNode) )
	{
		if( bUpdateNow )
		{
			Update();
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
inline bool CSG_TIN::_Add_Edge(CSG_TIN_Node *a, CSG_TIN_Node *b)
{
	m_Edges		= (CSG_TIN_Edge **)SG_Realloc(m_Edges, (m_nEdges + 1) * sizeof(CSG_TIN_Edge *));
	m_Edges[m_nEdges++]	= new CSG_TIN_Edge(a, b);

	return( true );
}

//---------------------------------------------------------
bool CSG_TIN::_Add_Triangle(CSG_TIN_Node *a, CSG_TIN_Node *b, CSG_TIN_Node *c)
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
bool CSG_TIN::On_Update(void)
{
	return( _Triangulate() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
