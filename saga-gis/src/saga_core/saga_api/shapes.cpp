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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
CSG_String	SG_Get_ShapeType_Name(TSG_Shape_Type Type)
{
	switch( Type )
	{
	case SHAPE_TYPE_Point:		return( _TL("Point") );
	case SHAPE_TYPE_Points:		return( _TL("Points") );
	case SHAPE_TYPE_Line:		return( _TL("Line") );
	case SHAPE_TYPE_Polygon:	return( _TL("Polygon") );

	default:
	case SHAPE_TYPE_Undefined:	return( _TL("Undefined") );
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

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), _TL("Load shapes"), File_Name.c_str()), true);

	bool	bResult	= _Load_ESRI(File_Name);

	Set_File_Name(File_Name, true);
	Load_MetaData(File_Name);

	if( bResult )
	{
		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	}
	else
	{
		for(int iShape=Get_Count()-1; iShape >= 0; iShape--)
		{
			if( !Get_Shape(iShape)->is_Valid() )
			{
				Del_Shape(iShape);
			}
		}

		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Shapes::Create(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure, TSG_Vertex_Type Vertex_Type)
{
	Destroy();

	CSG_Table::Create(pStructure);

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
	if(	pObject && pObject->is_Valid() && (pObject->Get_ObjectType() == DATAOBJECT_TYPE_Shapes || pObject->Get_ObjectType() == DATAOBJECT_TYPE_PointCloud) )
	{
		pShapes	= (CSG_Shapes *)pObject;

		Create(pShapes->Get_Type(), pShapes->Get_Name(), pShapes, pShapes->Get_Vertex_Type());

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
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), _TL("Save shapes"), File_Name.c_str()), true);

	if( _Save_ESRI(File_Name) )
	{
		Set_Modified(false);

		Set_File_Name(File_Name, true);

		Save_MetaData(File_Name);

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

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
			default:
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
			if( m_Vertex_Type == SG_VERTEX_TYPE_XY )	// currently we have to disable this check for 3D shapefiles since the
														// _Update_Area() method can not handle polygons with no horizontal extent
			{
				//--------------------------------------------
				// ring direction: outer rings > clockwise, inner rings (lakes) > counterclockwise !

				if( (pPolygon->is_Lake(iPart) == pPolygon->is_Clockwise(iPart)) )
				{
					pPolygon->Revert_Points(iPart);
				}
			}

			//--------------------------------------------
			// last point == first point !

			if( !CSG_Point(pPolygon->Get_Point(0, iPart)).is_Equal(pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart)) )
			{
				((CSG_Shape *)pPolygon)->Add_Point(pPolygon->Get_Point(0, iPart), iPart);

				if( m_Vertex_Type != SG_VERTEX_TYPE_XY )
				{
					pPolygon->Set_Z(pPolygon->Get_Z(0, iPart), pPolygon->Get_Point_Count(iPart) - 1, iPart);

					if( m_Vertex_Type == SG_VERTEX_TYPE_XYZM )
					{
						pPolygon->Set_M(pPolygon->Get_M(0, iPart), pPolygon->Get_Point_Count(iPart) - 1, iPart);
					}
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
