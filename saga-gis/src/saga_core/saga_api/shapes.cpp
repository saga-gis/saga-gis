
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
#include "shapes.h"
#include "pointcloud.h"
#include "tool_library.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	SG_Get_ShapeType_Name(TSG_Shape_Type Type)
{
	switch( Type )
	{
	case SHAPE_TYPE_Point  : return( _TL("Point"    ) );
	case SHAPE_TYPE_Points : return( _TL("Points"   ) );
	case SHAPE_TYPE_Line   : return( _TL("Line"     ) );
	case SHAPE_TYPE_Polygon: return( _TL("Polygon"  ) );
	default                : return( _TL("Undefined") );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
	case SG_DATAOBJECT_TYPE_Shapes:
		return( new CSG_Shapes(Shapes) );

	case SG_DATAOBJECT_TYPE_PointCloud:
		return( SG_Create_PointCloud(*((CSG_PointCloud *)&Shapes)) );

	default:
		return( NULL );
	}
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(const char       *File) { return( SG_Create_Shapes(CSG_String(File)) ); }
CSG_Shapes *		SG_Create_Shapes(const wchar_t    *File) { return( SG_Create_Shapes(CSG_String(File)) ); }
CSG_Shapes *		SG_Create_Shapes(const CSG_String &File)
{
	CSG_Shapes *pShapes = new CSG_Shapes();

	if( pShapes->Create(File) )
	{
		return( pShapes );
	}

	delete(pShapes); return( NULL );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pTemplate, TSG_Vertex_Type Vertex_Type)
{
	return( new CSG_Shapes(Type, Name, pTemplate, Vertex_Type) );
}

//---------------------------------------------------------
CSG_Shapes *		SG_Create_Shapes(CSG_Shapes *pTemplate)
{
	if( pTemplate )
	{
		switch( pTemplate->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Shapes:
			return( new CSG_Shapes(pTemplate->Get_Type(), pTemplate->Get_Name(), pTemplate, pTemplate->Get_Vertex_Type()) );

		case SG_DATAOBJECT_TYPE_PointCloud:
			return( SG_Create_PointCloud((CSG_PointCloud *)pTemplate) );

		default:
			break;
		}
	}

	return( new CSG_Shapes() );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
	_On_Construction(); Create(Shapes);
}

//---------------------------------------------------------
CSG_Shapes::CSG_Shapes(const char       *File) : CSG_Shapes(CSG_String(File)) {}
CSG_Shapes::CSG_Shapes(const wchar_t    *File) : CSG_Shapes(CSG_String(File)) {}
CSG_Shapes::CSG_Shapes(const CSG_String &File)
	: CSG_Table()
{
	_On_Construction(); Create(File);
}

//---------------------------------------------------------
CSG_Shapes::CSG_Shapes(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pTemplate, TSG_Vertex_Type Vertex_Type)
	: CSG_Table()
{
	_On_Construction(); Create(Type, Name, pTemplate, Vertex_Type);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shapes::_On_Construction(void)
{
	CSG_Table::_On_Construction();

	m_Type        = SHAPE_TYPE_Undefined;
	m_Vertex_Type = SG_VERTEX_TYPE_XY;

	m_Encoding    = SG_FILE_ENCODING_UTF8;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::Create(const CSG_Shapes &Shapes)
{
	return( Assign((CSG_Data_Object *)&Shapes) );
}

//---------------------------------------------------------
bool CSG_Shapes::Create(const char       *File) { return( Create(CSG_String(File)) ); }
bool CSG_Shapes::Create(const wchar_t    *File) { return( Create(CSG_String(File)) ); }
bool CSG_Shapes::Create(const CSG_String &File)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s %s: %s...", _TL("Loading"), _TL("shapes"), File.c_str()), true);

	bool bResult = false;

	//-----------------------------------------------------
	if( File.BeforeFirst(':').Cmp("PGSQL") == 0 )	// database source
	{
		CSG_String s(File);

		s = s.AfterFirst(':'); CSG_String Host (s.BeforeFirst(':'));
		s = s.AfterFirst(':'); CSG_String Port (s.BeforeFirst(':'));
		s = s.AfterFirst(':'); CSG_String DBase(s.BeforeFirst(':'));
		s = s.AfterFirst(':'); CSG_String Table(s.BeforeFirst(':'));

		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("db_pgsql", 20, true); // CPGIS_Shapes_Load

		if(	pTool != NULL )
		{
			SG_UI_ProgressAndMsg_Lock(true);

			CSG_String Connection(DBase + " [" + Host + ":" + Port + "]");

			bResult = pTool->Set_Manager(NULL) && pTool->On_Before_Execution()
			       && pTool->Set_Parameter("CONNECTION", Connection)
			       && pTool->Set_Parameter("DB_TABLE"  , Table     )
			       && pTool->Set_Parameter("SHAPES"    , this      )
			       && pTool->Execute();

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			SG_UI_ProgressAndMsg_Lock(false);
		}
	}
	else
	{
		if( SG_File_Cmp_Extension(File, "shp") )
		{
			bResult = _Load_ESRI(File);
		}

		if( !bResult )
		{
			bResult = _Load_GDAL(File);
		}
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);
		Set_Update_Flag();

		SG_UI_Process_Set_Ready();
		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	//-----------------------------------------------------
	for(sLong iShape=Get_Count()-1; iShape>=0; iShape--)	// be kind, keep at least those shapes that have been loaded successfully
	{
		if( !Get_Shape(iShape)->is_Valid() )
		{
			Del_Shape(iShape);
		}
	}

	if( Get_Count() <= 0 )
	{
		Destroy();
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();
	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
bool CSG_Shapes::Create(TSG_Shape_Type Type, const SG_Char *Name, CSG_Table *pTemplate, TSG_Vertex_Type Vertex_Type)
{
	Destroy();

	CSG_Table::Create(pTemplate);

	if( Name )
	{
		Set_Name(CSG_String(Name));
	}

	m_Type = Type; m_Vertex_Type = Vertex_Type;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes::~CSG_Shapes(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Shapes::Destroy(void)
{
	if( CSG_Table::Destroy() )
	{
		m_Type = SHAPE_TYPE_Undefined;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::Assign(CSG_Data_Object *pObject, bool bProgress)
{
	if( pObject->asShapes(true) && CSG_Data_Object::Assign(pObject, bProgress) )
	{
		CSG_Shapes *pShapes = pObject->asShapes(true);

		Create(pShapes->Get_Type(), pShapes->Get_Name(), pShapes, pShapes->Get_Vertex_Type());

		Get_Projection().Create(pShapes->Get_Projection());

		for(sLong i=0; i<pShapes->Get_Count() && (!bProgress || SG_UI_Process_Set_Progress(i, pShapes->Get_Count())); i++)
		{
			Add_Shape(pShapes->Get_Shape(i));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_Record * CSG_Shapes::_Get_New_Record(sLong Index)
{
	switch( m_Type )
	{
	case SHAPE_TYPE_Polygon     : return( new CSG_Shape_Polygon (this, Index) );
	case SHAPE_TYPE_Line        : return( new CSG_Shape_Line    (this, Index) );
	case SHAPE_TYPE_Points      : return( new CSG_Shape_Points  (this, Index) );
	case SHAPE_TYPE_Point       :
		switch( m_Vertex_Type )
		{
		case SG_VERTEX_TYPE_XY  : return( new CSG_Shape_Point   (this, Index) );
		case SG_VERTEX_TYPE_XYZ : return( new CSG_Shape_Point_Z (this, Index) );
		case SG_VERTEX_TYPE_XYZM: return( new CSG_Shape_Point_ZM(this, Index) );
		default:
			return( NULL );
		}
	default:
		return( NULL );
	}
}

//---------------------------------------------------------
CSG_Shape * CSG_Shapes::Add_Shape(CSG_Table_Record *pCopy, TSG_ADD_Shape_Copy_Mode mCopy)
{
	CSG_Shape *pShape = (CSG_Shape *)Add_Record();

	if( pShape && pCopy && mCopy != SHAPE_NO_COPY )
	{
		pShape->Assign(pCopy, mCopy);
	}

	return( pShape );
}

//---------------------------------------------------------
bool CSG_Shapes::Del_Shape(CSG_Shape *pShape)
{
	return( Del_Record(pShape->Get_Index()) );
}

bool CSG_Shapes::Del_Shape(sLong Index)
{
	return( Del_Record(Index) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::On_Update(void)
{
	if( Get_Count() > 0 )
	{
		CSG_Shape *pShape = Get_Shape(0);

		m_Extent = pShape->Get_Extent();
		m_ZMin   = pShape->Get_ZMin();
		m_ZMax   = pShape->Get_ZMax();
		m_MMin   = pShape->Get_MMin();
		m_MMax   = pShape->Get_MMax();

		for(sLong i=1; i<Get_Count(); i++)
		{
			pShape = Get_Shape(i);

			m_Extent.Union(pShape->Get_Extent());

			switch( m_Vertex_Type )
			{
			case SG_VERTEX_TYPE_XYZM:
				if( m_MMin > pShape->Get_MMin() ) m_MMin = pShape->Get_MMin();
				if( m_MMax < pShape->Get_MMax() ) m_MMax = pShape->Get_MMax();

			case SG_VERTEX_TYPE_XYZ:
				if( m_ZMin > pShape->Get_ZMin() ) m_ZMin = pShape->Get_ZMin();
				if( m_ZMax < pShape->Get_ZMax() ) m_ZMax = pShape->Get_ZMax();
				break;

			default:
				break;
			}
		}
	}
	else
	{
		m_Extent.Create(0., 0., 0., 0.);
	}

	return( CSG_Table::On_Update() );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CSG_Shapes::Get_Shape(const CSG_Point &Point, double Epsilon)
{
	CSG_Rect r(Point.x - Epsilon, Point.y - Epsilon, Point.x + Epsilon, Point.y + Epsilon);

	CSG_Shape *pNearest = NULL;

	if( r.Intersects(Get_Extent()) != INTERSECTION_None )
	{
		double dNearest = -1.;

		for(sLong iShape=0; iShape<Get_Count(); iShape++)
		{
			CSG_Shape *pShape = Get_Shape(iShape);

			if( pShape->Intersects(r) )
			{
				double d = pShape->Get_Distance(Point);

				if( d == 0. )
				{
					return( pShape );
				}
				else if( d > 0. && d <= Epsilon && (pNearest == NULL || d < dNearest) )
				{
					dNearest = d;
					pNearest = pShape;
				}
			}
		}
	}

	return( pNearest );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes::Make_Clean(void)
{
	if( m_Type != SHAPE_TYPE_Polygon )
	{
		return( true );
	}

	for(sLong iShape=0; iShape<Get_Count() && SG_UI_Process_Set_Progress(iShape, Get_Count()); iShape++)
	{
		CSG_Shape_Polygon *pPolygon = (CSG_Shape_Polygon *)Get_Shape(iShape);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			// currently we have to disable this check for 3D shapefiles since the
			// _Update_Area() method can not handle polygons with no horizontal extent
			if( m_Vertex_Type == SG_VERTEX_TYPE_XY )
			{
				if( pPolygon->is_Lake(iPart) == pPolygon->is_Clockwise(iPart) )
				{// ring direction: outer rings > clockwise, inner rings (lakes) > counterclockwise !
					pPolygon->Revert_Points(iPart);
				}
			}

			//---------------------------------------------
			if( !CSG_Point(pPolygon->Get_Point(0, iPart)).is_Equal(pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart)) )
			{ // last point == first point !
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
			// to do: ensure there is no self intersection !
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
