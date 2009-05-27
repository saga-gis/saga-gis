
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
	return( new CSG_Shapes(Shapes) );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(const CSG_String &File_Name)
{
	return( new CSG_Shapes(File_Name) );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure)
{
	return( new CSG_Shapes(Type, Name, pStructure) );
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
CSG_Shapes::CSG_Shapes(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure)
	: CSG_Table()
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
void CSG_Shapes::_On_Construction(void)
{
	CSG_Table::_On_Construction();

	m_Type	= SHAPE_TYPE_Undefined;
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
bool CSG_Shapes::Create(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pStructure)
{
	Destroy();

	_Create(pStructure);

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
bool CSG_Shapes::Save(const CSG_String &File_Name, int Format)
{
	bool		bResult		= false;
	CSG_String	sFile_Name	= SG_File_Make_Path(NULL, File_Name, SG_T("shp"));

	switch( Format )
	{
	case 0: default:
		bResult	= _Save_ESRI(sFile_Name);
		break;
	}

	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(sFile_Name);

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
CSG_Table_Record * CSG_Shapes::_Get_New_Record(int Index)
{
	switch( m_Type )
	{
	case SHAPE_TYPE_Point:		return( new CSG_Shape_Point		(this, Index) );
	case SHAPE_TYPE_Points:		return( new CSG_Shape_Points	(this, Index) );
	case SHAPE_TYPE_Line:		return( new CSG_Shape_Line		(this, Index) );
	case SHAPE_TYPE_Polygon:	return( new CSG_Shape_Polygon	(this, Index) );
	default:					return( NULL );
	}
}

//---------------------------------------------------------
CSG_Shape * CSG_Shapes::Add_Shape(CSG_Table_Record *pCopy, TSG_ADD_Shape_Copy_Mode mCopy)
{
	CSG_Shape	*pShape	= (CSG_Shape *)Add_Record();

	if( pShape && pCopy )
	{
		if( mCopy == SHAPE_COPY || mCopy == SHAPE_COPY_ATTR )
		{
			((CSG_Table_Record *)pShape)->Assign(pCopy);
		}

		if( mCopy == SHAPE_COPY || mCopy == SHAPE_COPY_GEOM && pCopy->Get_Table()->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
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
		m_Extent	= Get_Shape(0)->Get_Extent();

		for(int i=1; i<Get_Count(); i++)
		{
			m_Extent.Union(Get_Shape(i)->Get_Extent());
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
