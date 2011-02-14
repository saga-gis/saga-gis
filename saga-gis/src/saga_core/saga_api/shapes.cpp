
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
#include "pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char *	SG_Get_ShapeType_Name(TSG_Shape_Type Type)
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
CSG_Shapes *		SG_Create_Shapes(void)
{
	return( new CSG_Shapes );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(const CSG_Shapes &Shapes)
{
	switch( Shapes.Get_ObjectType() )
	{
	case DATAOBJECT_TYPE_Shapes:
		return( new CSG_Shapes(Shapes) );

	case DATAOBJECT_TYPE_PointCloud:
		return( SG_Create_PointCloud(*((CSG_PointCloud *)&Shapes)) );

	default:
		return( NULL );
	}
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(const CSG_String &File_Name)
{
	return( new CSG_Shapes(File_Name) );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure, TSG_Vertex_Type Vertex_Type)
{
	return( new CSG_Shapes(Type, Name, pStructure, Vertex_Type) );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(CSG_Shapes *pTemplate)
{
	if( pTemplate )
	{
		switch( pTemplate->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Shapes:
			return( new CSG_Shapes(pTemplate->Get_Type(), pTemplate->Get_Name(), pTemplate, pTemplate->Get_Vertex_Type()) );

		case DATAOBJECT_TYPE_PointCloud:
			return( SG_Create_PointCloud((CSG_PointCloud *)pTemplate) );

		default:
			break;
		}
	}

	return( new CSG_Shapes() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes::CSG_Shapes(void)
	: CSG_Table()
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Shapes::CSG_Shapes(const CSG_Shapes &Shapes)
	: CSG_Table()
{
	_On_Construction();

	Create(Shapes);
}

//---------------------------------------------------------
CSG_Shapes::CSG_Shapes(const CSG_String &File_Name)
	: CSG_Table()
{
	_On_Construction();

	Create(File_Name);
}

//---------------------------------------------------------
CSG_Shapes::CSG_Shapes(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure, TSG_Vertex_Type Vertex_Type)
	: CSG_Table()
{
	_On_Construction();

	Create(Type, Name, pStructure, Vertex_Type);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shapes::_On_Construction(void)
{
	CSG_Table::_On_Construction();

	m_Type			= SHAPE_TYPE_Undefined;
	m_Vertex_Type	= SG_VERTEX_TYPE_XY;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::Create(const CSG_Shapes &Shapes)
{
	return( Assign((CSG_Data_Object *)&Shapes) );
}

//---------------------------------------------------------
bool CSG_Shapes::Create(const CSG_String &File_Name)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Load shapes"), File_Name.c_str()), true);

	if( _Load_ESRI(File_Name) )
	{
		for(int iShape=Get_Count()-1; iShape >= 0; iShape--)
		{
			if( !Get_Shape(iShape)->is_Valid() )
			{
				Del_Shape(iShape);
			}
		}

		Set_File_Name(File_Name);

		Load_MetaData(File_Name);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	Destroy();	// loading failure...

	SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes::Create(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure, TSG_Vertex_Type Vertex_Type)
{
	Destroy();

	_Create(pStructure);

	Set_Name(Name);

	m_Type			= Type;
	m_Vertex_Type	= Vertex_Type;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes::~CSG_Shapes(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Shapes::Destroy(void)
{
	return( CSG_Table::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::Assign(CSG_Data_Object *pObject)
{
	int			iShape;
	CSG_Shape	*pShape;
	CSG_Shapes	*pShapes;

	//-----------------------------------------------------
	if(	pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		pShapes	= (CSG_Shapes *)pObject;

		Create(pShapes->Get_Type(), pShapes->Get_Name(), pShapes);

		for(iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= Add_Shape();
			pShape->Assign(pShapes->Get_Shape(iShape));
		}

		SG_UI_Process_Set_Ready();

		Update();

		Get_History()	= pObject->Get_History();

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
bool CSG_Shapes::Save(const CSG_String &File_Name, int Format)
{
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Save shapes"), File_Name.c_str()), true);

	if( _Save_ESRI(File_Name) )
	{
		Set_Modified(false);

		Set_File_Name(File_Name);

		Save_MetaData(File_Name);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
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
CSG_Table_Record * CSG_Shapes::_Get_New_Record(int Index)
{
	switch( m_Type )
	{
	case SHAPE_TYPE_Point:
		switch( m_Vertex_Type )
		{
		case SG_VERTEX_TYPE_XY:	default:
			return( new CSG_Shape_Point		(this, Index) );

		case SG_VERTEX_TYPE_XYZ:
			return( new CSG_Shape_Point_Z	(this, Index) );

		case SG_VERTEX_TYPE_XYZM:
			return( new CSG_Shape_Point_ZM	(this, Index) );
		}

	case SHAPE_TYPE_Points:
		return( new CSG_Shape_Points	(this, Index) );

	case SHAPE_TYPE_Line:
		return( new CSG_Shape_Line		(this, Index) );

	case SHAPE_TYPE_Polygon:
		return( new CSG_Shape_Polygon	(this, Index) );

	default:
		return( NULL );
	}
}

//---------------------------------------------------------
CSG_Shape * CSG_Shapes::Add_Shape(CSG_Table_Record *pCopy, TSG_ADD_Shape_Copy_Mode mCopy)
{
	CSG_Shape	*pShape	= (CSG_Shape *)Add_Record();

	if( pShape && pCopy )
	{
		if( (mCopy == SHAPE_COPY || mCopy == SHAPE_COPY_ATTR) )
		{
			((CSG_Table_Record *)pShape)->Assign(pCopy);
		}

		if( (mCopy == SHAPE_COPY || mCopy == SHAPE_COPY_GEOM) && pCopy->Get_Table()->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
		{
			pShape->Assign((CSG_Shape *)pCopy, false);
		}
	}

	return( pShape );
}

//---------------------------------------------------------
bool CSG_Shapes::Del_Shape(CSG_Shape *pShape)
{
	return( Del_Record(pShape->Get_Index()) );
}

bool CSG_Shapes::Del_Shape(int iShape)
{
	return( Del_Record(iShape) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::On_Update(void)
{
	if( Get_Count() > 0 )
	{
		CSG_Shape	*pShape	= Get_Shape(0);

		m_Extent	= pShape->Get_Extent();
		m_ZMin		= pShape->Get_ZMin();
		m_ZMax		= pShape->Get_ZMax();
		m_MMin		= pShape->Get_MMin();
		m_MMax		= pShape->Get_MMax();

		for(int i=1; i<Get_Count(); i++)
		{
			pShape	= Get_Shape(i);

			m_Extent.Union(pShape->Get_Extent());

			switch( m_Vertex_Type )
			{
			case SG_VERTEX_TYPE_XYZM:
				if( m_MMin > pShape->Get_MMin() )	m_MMin	= pShape->Get_MMin();
				if( m_MMax < pShape->Get_MMax() )	m_MMax	= pShape->Get_MMax();

			case SG_VERTEX_TYPE_XYZ:
				if( m_ZMin > pShape->Get_ZMin() )	m_ZMin	= pShape->Get_ZMin();
				if( m_ZMax < pShape->Get_ZMax() )	m_ZMax	= pShape->Get_ZMax();
				break;
			}
		}
	}
	else
	{
		m_Extent.Assign(0.0, 0.0, 0.0, 0.0);
	}

	return( CSG_Table::On_Update() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CSG_Shapes::Get_Shape(TSG_Point Point, double Epsilon)
{
	int			iShape;
	double		d, dNearest;
	CSG_Rect	r(Point.x - Epsilon, Point.y - Epsilon, Point.x + Epsilon, Point.y + Epsilon);
	CSG_Shape	*pShape, *pNearest;

	pNearest	= NULL;

	if( r.Intersects(Get_Extent()) != INTERSECTION_None )
	{
		for(iShape=0, dNearest=-1.0; iShape<Get_Count(); iShape++)
		{
			pShape	= Get_Shape(iShape);

			if( pShape->Intersects(r) )
			{
				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					if( r.Intersects(pShape->Get_Extent(iPart)) )
					{
						d	= pShape->Get_Distance(Point, iPart);

						if( d == 0.0 )
						{
							return( pShape );
						}
						else if( d > 0.0 && d <= Epsilon && (pNearest == NULL || d < dNearest) )
						{
							dNearest	= d;
							pNearest	= pShape;
						}
					}
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
bool CSG_Shapes::Make_Clean(void)
{
	if( m_Type != SHAPE_TYPE_Polygon )
	{
		return( true );
	}

	for(int iShape=0; iShape<Get_Count() && SG_UI_Process_Set_Progress(iShape, Get_Count()); iShape++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)Get_Shape(iShape);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			//--------------------------------------------
			// last point == first point !

			if( !CSG_Point(pPolygon->Get_Point(0, iPart)).is_Equal(pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart)) )
			{
				((CSG_Shape *)pPolygon)->Add_Point(pPolygon->Get_Point(0, iPart), iPart);
			}

			//--------------------------------------------
			// ring direction !

			if( (pPolygon->is_Lake(iPart) == false && pPolygon->is_Clockwise(iPart) == false)
			||	(pPolygon->is_Lake(iPart) ==  true && pPolygon->is_Clockwise(iPart) ==  true) )
			{
				for(int i=0, j=pPolygon->Get_Point_Count(iPart)-1; i<j; i++, j--)
				{
					TSG_Point	iPoint	= pPolygon->Get_Point(i, iPart);
					TSG_Point	jPoint	= pPolygon->Get_Point(j, iPart);
					pPolygon->Set_Point(iPoint.x, iPoint.y, j, iPart);
					pPolygon->Set_Point(jPoint.x, jPoint.y, i, iPart);
				}
			}

			//--------------------------------------------
			// no self intersection !

		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
