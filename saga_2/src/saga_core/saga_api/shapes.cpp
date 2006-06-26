
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
//                     shapes.cpp                        //
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
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char *	API_Get_ShapeType_Name(TShape_Type Type)
{
	switch( Type )
	{
	case SHAPE_TYPE_Point:		return( LNG("[DAT] Point") );
	case SHAPE_TYPE_Points:		return( LNG("[DAT] Points") );
	case SHAPE_TYPE_Line:		return( LNG("[DAT] Line") );
	case SHAPE_TYPE_Polygon:	return( LNG("[DAT] Polygon") );

	default:
	case SHAPE_TYPE_Undefined:	return( LNG("[DAT] Undefined") );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes *		API_Create_Shapes(void)
{
	return( new CShapes );
}

//---------------------------------------------------------
CShapes *		API_Create_Shapes(const CShapes &Shapes)
{
	return( new CShapes(Shapes) );
}

//---------------------------------------------------------
CShapes *		API_Create_Shapes(const char *File_Name)
{
	return( new CShapes(File_Name) );
}

//---------------------------------------------------------
CShapes *		API_Create_Shapes(TShape_Type Type, char *Name, CTable *pStructure)
{
	return( new CShapes(Type, Name, pStructure) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes::CShapes(void)
	: CDataObject()
{
	_On_Construction();
}

//---------------------------------------------------------
CShapes::CShapes(const CShapes &Shapes)
	: CDataObject()
{
	_On_Construction();

	Create(Shapes);
}

//---------------------------------------------------------
CShapes::CShapes(const char *File_Name)
	: CDataObject()
{
	_On_Construction();

	Create(File_Name);
}

//---------------------------------------------------------
CShapes::CShapes(TShape_Type Type, const char *Name, CTable *pStructure)
	: CDataObject()
{
	_On_Construction();

	Create(Type, Name, pStructure);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes::_On_Construction(void)
{
	m_Type				= SHAPE_TYPE_Undefined;

	m_Shapes			= NULL;
	m_nShapes			= 0;

	m_bUpdate			= true;

	m_Table.m_pOwner	= this;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes::Create(const CShapes &Shapes)
{
	return( Assign((CDataObject *)&Shapes) );
}

//---------------------------------------------------------
bool CShapes::Create(const char *File_Name)
{
	int		iShape;

	Destroy();

	if( _Load_ESRI(File_Name) )
	{
		if( Get_Count() < m_Table.Get_Record_Count() )
		{
			for(iShape=m_Table.Get_Record_Count()-1; iShape >= Get_Count(); iShape--)
			{
				m_Table._Del_Record(iShape);
			}
		}

		for(iShape=Get_Count()-1; iShape >= 0; iShape--)
		{
			if( !Get_Shape(iShape)->is_Valid() )
			{
				Del_Shape(iShape);
			}
		}

		Set_File_Path(File_Name);

		if( !Get_History().Load(File_Name, HISTORY_EXT_SHAPES) )
		{
			Get_History().Add_Entry(LNG("[HST] Loaded from file"), File_Name);
		}

		return( true );
	}

	Destroy();	// loading failure...

	return( false );
}

//---------------------------------------------------------
bool CShapes::Create(TShape_Type Type, const char *Name, CTable *pStructure)
{
	Destroy();

	m_Table._Create(pStructure);
	m_Table.Set_Name(Name);

	Set_Name(Name);

	m_Type	= Type;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes::~CShapes(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CShapes::Destroy(void)
{
	if( m_nShapes > 0 )
	{
		for(int i=0; i<m_nShapes; i++)
		{
			delete(m_Shapes[i]);
		}

		API_Free(m_Shapes);
		m_Shapes	= NULL;
		m_nShapes	= 0;
	}

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
bool CShapes::Assign(CDataObject *pObject)
{
	int		iShape;
	CShape	*pShape;
	CShapes	*pShapes;

	//-----------------------------------------------------
	if(	pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		pShapes	= (CShapes *)pObject;

		Create(pShapes->Get_Type(), pShapes->Get_Name(), &pShapes->Get_Table());

		for(iShape=0; iShape<pShapes->Get_Count() && API_Callback_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= Add_Shape();
			pShape->Assign(pShapes->Get_Shape(iShape));
		}

		API_Callback_Process_Set_Ready();

		_Extent_Update();

		Get_History().Assign(pObject->Get_History());

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
bool CShapes::Save(const char *File_Name, int Format)
{
	bool		bResult;

	switch( Format )
	{
	case 0: default:
		bResult	= _Save_ESRI(File_Name);
		break;
	}

	if( bResult )
	{
		Set_File_Path(File_Name);

		Get_History().Save(File_Name, HISTORY_EXT_SHAPES);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShape * CShapes::_Add_Shape(CTable_Record *pRecord)
{
	CShape	*pShape	= NULL;

	if( pRecord )
	{
		switch( m_Type )
		{
	    default:
	        break;

		case SHAPE_TYPE_Point:
			pShape	= new CShape_Point	(this, pRecord);
			break;

		case SHAPE_TYPE_Points:
			pShape	= new CShape_Points	(this, pRecord);
			break;

		case SHAPE_TYPE_Line:
			pShape	= new CShape_Line	(this, pRecord);
			break;

		case SHAPE_TYPE_Polygon:
			pShape	= new CShape_Polygon(this, pRecord);
			break;
		}

		if( pShape )
		{
			m_Shapes				= (CShape **)API_Realloc(m_Shapes, (m_nShapes + 1) * sizeof(CShape *));
			m_Shapes[m_nShapes++]	= pShape;

			m_bUpdate				= true;

			Set_Modified();
		}
	}

	return( pShape );
}

//---------------------------------------------------------
CShape * CShapes::Add_Shape(CTable_Record *pValues)
{
	if( m_Type != SHAPE_TYPE_Undefined )
	{
		return( _Add_Shape(m_Table._Add_Record(pValues)) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CShapes::Del_Shape(CShape *pShape)
{
	return( Del_Shape(pShape->Get_Record()->Get_Index()) );
}

bool CShapes::Del_Shape(int iShape)
{
	int		i;

	if( iShape >= 0 && iShape < m_nShapes )
	{
		if( m_Shapes[iShape]->is_Selected() )
		{
			Select(m_Shapes[iShape], true);
		}

		delete(m_Shapes[iShape]);

		m_nShapes--;

		for(i=iShape; i<m_nShapes; i++)
		{
			m_Shapes[i]	= m_Shapes[i + 1];
		}

		m_Shapes	= (CShape **)API_Realloc(m_Shapes, m_nShapes * sizeof(CShape *));

		m_Table._Del_Record(iShape);

		m_bUpdate	= true;

		Set_Modified();

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
void CShapes::_Extent_Update(void)
{
	int		i;

	if( m_bUpdate )
	{
		if( m_nShapes > 0 )
		{
			m_Extent	= m_Shapes[0]->Get_Extent();

			for(i=1; i<m_nShapes; i++)
			{
				m_Extent.Union(m_Shapes[i]->Get_Extent());
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
CShape * CShapes::Get_Shape(TGEO_Point Point, double Epsilon)
{
	int			iShape;
	double		d, dNearest;
	CGEO_Rect	r(Point.x - Epsilon, Point.y - Epsilon, Point.x + Epsilon, Point.y + Epsilon);
	CShape		*pShape, *pNearest;

	pNearest	= NULL;

	if( r.Intersects(Get_Extent()) != INTERSECTION_None )
	{
		for(iShape=0, dNearest=-1.0; iShape<m_nShapes; iShape++)
		{
			pShape	= m_Shapes[iShape];

			if( pShape->Intersects(r) )
			{
				d	= pShape->Get_Distance(Point);

				if( d == 0.0 )
				{
					return( pShape );
				}

				if( d > 0.0 && d <= Epsilon && (pNearest == NULL || d < dNearest) )
				{
					dNearest	= d;
					pNearest	= pShape;
				}
			}
		}
	}

	return( pNearest );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
