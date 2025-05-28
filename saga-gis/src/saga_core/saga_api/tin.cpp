
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#include "tin.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
CSG_TIN * SG_Create_TIN(const char       *File) { return( SG_Create_TIN(CSG_String(File)) ); }
CSG_TIN * SG_Create_TIN(const wchar_t    *File) { return( SG_Create_TIN(CSG_String(File)) ); }
CSG_TIN * SG_Create_TIN(const CSG_String &File)
{
	CSG_TIN *pTIN = new CSG_TIN();

	if( pTIN->Create(File) )
	{
		return( pTIN );
	}

	delete(pTIN); return( NULL );
}

//---------------------------------------------------------
CSG_TIN * SG_Create_TIN(CSG_Shapes *pShapes)
{
	return( new CSG_TIN(pShapes) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
	_On_Construction(); Create(TIN);
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(const char       *File) : CSG_TIN(CSG_String(File)) {}
CSG_TIN::CSG_TIN(const wchar_t    *File) : CSG_TIN(CSG_String(File)) {}
CSG_TIN::CSG_TIN(const CSG_String &File)
	: CSG_Table()
{
	_On_Construction(); Create(File);
}

//---------------------------------------------------------
CSG_TIN::CSG_TIN(CSG_Shapes *pShapes)
	: CSG_Table()
{
	_On_Construction(); Create(pShapes);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_TIN::_On_Construction(void)
{
	CSG_Table::_On_Construction();

	m_Edges     = NULL; m_nEdges     = 0;
	m_Triangles = NULL; m_nTriangles = 0;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::Create(const CSG_TIN &TIN)
{
	return( Assign((CSG_Data_Object *)&TIN) );
}

//---------------------------------------------------------
bool CSG_TIN::Create(const char       *File) { return( Create(CSG_String(File)) ); }
bool CSG_TIN::Create(const wchar_t    *File) { return( Create(CSG_String(File)) ); }
bool CSG_TIN::Create(const CSG_String &File)
{
	CSG_Shapes Shapes(File);

	if( Create(&Shapes) )
	{
		Get_History().Add_Child(_TL("Created from file"), File);
		Get_History().Add_Children(Shapes.Get_History());

		Set_File_Name(File, true);
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
		SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Create TIN from shapes"), pShapes->Get_Name()), true);

		CSG_Table::Create(pShapes);

		Set_Name(pShapes->Get_Name());

		//-------------------------------------------------
		for(sLong iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape *pShape = pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Add_Node(pShape->Get_Point(iPoint, iPart), pShape, false);
				}
			}
		}

		SG_UI_Process_Set_Ready();

		m_bTriangulate = true;

		if( Update() )
		{
			SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

			return( true );
		}
	}

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
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
//                                                       //
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
		for(sLong i=0; i<m_nEdges; i++)
		{
			delete(m_Edges[i]);
		}

		SG_Free(m_Edges); m_Edges = NULL; m_nEdges = 0;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_TIN::_Destroy_Triangles(void)
{
	if( m_nTriangles > 0 )
	{
		for(sLong i=0; i<m_nTriangles; i++)
		{
			delete(m_Triangles[i]);
		}

		SG_Free(m_Triangles); m_Triangles = NULL; m_nTriangles = 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::Assign(CSG_Data_Object *pObject, bool bProgress)
{
	if(	pObject && pObject->asTIN() && pObject->is_Valid() )
	{
		CSG_TIN *pTIN = pObject->asTIN();

		Destroy();

		CSG_Table::Create(pTIN);

		Set_Name(pTIN->Get_Name());

		Get_History() = pTIN->Get_History();

		//-------------------------------------------------
		for(sLong iNode=0; iNode<pTIN->Get_Node_Count(); iNode++)
		{
			CSG_TIN_Node *pNode = pTIN->Get_Node(iNode);

			Add_Node(pNode->Get_Point(), pNode, false);
		}

		//-------------------------------------------------
		for(sLong iTriangle=0; iTriangle<pTIN->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle *pTriangle = pTIN->Get_Triangle(iTriangle);

			_Add_Triangle(
				Get_Node(pTriangle->Get_Node(0)->Get_Index()),
				Get_Node(pTriangle->Get_Node(1)->Get_Index()),
				Get_Node(pTriangle->Get_Node(2)->Get_Index())
			);
		}

		m_bTriangulate = false;
		Update();
		m_bTriangulate = pTIN->m_bTriangulate;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::Triangulate(bool bOn, bool bUpdate)
{
	if( m_bTriangulate != bOn )
	{
		m_bTriangulate = bOn;
	}

	if( bUpdate )
	{
		Update();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::Save(const CSG_String &File, int Format)
{
	if( File.is_Empty() )
	{
		return( *Get_File_Name(false) ? Save(Get_File_Name(false), Format) : false );
	}

	bool bResult = false;

	if( Get_Triangle_Count() > 0 )
	{
		switch( Format )
		{
		case 0:	default: {
			CSG_Shapes Points(SHAPE_TYPE_Point, Get_Name(), this);

			for(sLong i=0; i<Get_Node_Count(); i++)
			{
				Points.Add_Shape(Get_Node(i))->Add_Point(Get_Node(i)->Get_Point());
			}

			bResult = Points.Save(File);

			break; }
		}
	}

	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(File, true);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_Record * CSG_TIN::_Get_New_Record(sLong Index)
{
	return( new CSG_TIN_Node(this, Index) );
}

//---------------------------------------------------------
CSG_TIN_Node * CSG_TIN::Add_Node(const TSG_Point &Point, CSG_Table_Record *pRecord, bool bUpdateNow)
{
	CSG_TIN_Node *pNode = (CSG_TIN_Node *)Add_Record(pRecord);

	if( pNode )
	{
		pNode->m_Point = Point;

		if( bUpdateNow )
		{
			Update();
		}
	}

	return( pNode );
}

//---------------------------------------------------------
bool CSG_TIN::Del_Node(sLong Index, bool bUpdateNow)
{
	if( Del_Record(Index) )
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_TIN_Triangle * CSG_TIN::Add_Triangle(CSG_TIN_Node *p0, CSG_TIN_Node *p1, CSG_TIN_Node *p2)
{
	return( _Add_Triangle((CSG_TIN_Node *)p0, (CSG_TIN_Node *)p1, (CSG_TIN_Node *)p2) );
}

CSG_TIN_Triangle * CSG_TIN::Add_Triangle(CSG_TIN_Node *p[3])
{
	return( Add_Triangle(p[0], p[1], p[2]) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline CSG_TIN_Edge * CSG_TIN::_Add_Edge(CSG_TIN_Node *a, CSG_TIN_Node *b)
{
	CSG_TIN_Edge *pEdge = new CSG_TIN_Edge(a, b);

	m_Edges = (CSG_TIN_Edge **)SG_Realloc(m_Edges, (m_nEdges + 1) * sizeof(CSG_TIN_Edge *));
	m_Edges[m_nEdges++]	= pEdge;

	return( pEdge );
}

//---------------------------------------------------------
CSG_TIN_Triangle * CSG_TIN::_Add_Triangle(CSG_TIN_Node *a, CSG_TIN_Node *b, CSG_TIN_Node *c)
{
	CSG_TIN_Triangle *pTriangle = new CSG_TIN_Triangle(a, b, c);

	m_Triangles	= (CSG_TIN_Triangle **)SG_Realloc(m_Triangles, (m_nTriangles + 1) * sizeof(CSG_TIN_Triangle *));
	m_Triangles[m_nTriangles++]	= pTriangle;

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

	return( pTriangle );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::On_Update(void)
{
	if( m_bTriangulate )
	{
		return( _Triangulate() );
	}

	if( Get_Node_Count() >= 3 ) // Update extent...
	{
		m_Extent.Assign(Get_Node(0)->Get_Point(), Get_Node(0)->Get_Point());

		for(int i=1; i<Get_Node_Count(); i++)
		{
			m_Extent.Union(Get_Node(i)->Get_Point());
		}

		return( true );
	}

	m_Extent.Assign(0., 0., 0., 0.);

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_TIN::On_Reload(void)
{
	return( Create(Get_File_Name(false)) );
}

//---------------------------------------------------------
bool CSG_TIN::On_Delete(void)
{
	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
