
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
//                   data_manager.cpp                    //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "data_manager.h"
#include "tool_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Manager	g_Data_Manager;

//---------------------------------------------------------
CSG_Data_Manager &	SG_Get_Data_Manager	(void)
{
	return( g_Data_Manager );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Collection::CSG_Data_Collection(TSG_Data_Object_Type Type)
{
	m_Type = Type;
}

//---------------------------------------------------------
CSG_Data_Collection::~CSG_Data_Collection(void)
{
	Delete();
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Collection::Find(const CSG_String &File, bool bNative) const
{
	for(size_t i=0; i<Count(); i++)
	{
		if( !File.Cmp(Get(i)->Get_File_Name(bNative)) )
		{
			return( Get(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Exists(CSG_Data_Object *pObject) const
{
	for(size_t i=0; i<Count(); i++)
	{
		if( pObject == Get(i) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Add(CSG_Data_Object *pObject)
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
	{
		if( Exists(pObject) )
		{
			return( true );
		}

		if( m_Objects.Inc_Array() )
		{
			m_Objects[Count() - 1] = pObject;

			pObject->m_Managed++;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete(CSG_Data_Object *pObject, bool bDetach)
{
	for(size_t i=0; i<Count(); i++)
	{
		if( pObject == Get(i) )
		{
			return( Delete(i, bDetach) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete(size_t i, bool bDetach)
{
	if( i < Count() )
	{
		CSG_Data_Object *pObject = Get(i);

		if( pObject->m_Managed > 0 )
		{
			pObject->m_Managed--;
		}

		if( !bDetach && !pObject->m_Managed )
		{
			delete(pObject);
		}

		CSG_Data_Object **pObjects = (CSG_Data_Object **)m_Objects.Get_Array();

		for(size_t j=i+1; j<Count(); i++, j++)
		{
			pObjects[i] = pObjects[j];
		}

		m_Objects.Dec_Array();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete(bool bDetach, bool bUnsaved)
{
	for(size_t i=Count(); i>0; i--)
	{
		if( !bUnsaved || !SG_File_Exists(Get(i - 1)->Get_File_Name()) )
		{
			Delete(i - 1, bDetach);
		}
	}

	return( Count() == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Manager::CSG_Data_Manager(void)
{
	m_pTable      = new CSG_Data_Collection(SG_DATAOBJECT_TYPE_Table     );
	m_pShapes     = new CSG_Data_Collection(SG_DATAOBJECT_TYPE_Shapes    );
	m_pPointCloud = new CSG_Data_Collection(SG_DATAOBJECT_TYPE_PointCloud);
	m_pTIN        = new CSG_Data_Collection(SG_DATAOBJECT_TYPE_TIN       );
	m_pGrid       = new CSG_Data_Collection(SG_DATAOBJECT_TYPE_Grid      );
	m_pGrids      = new CSG_Data_Collection(SG_DATAOBJECT_TYPE_Grids     );
}

//---------------------------------------------------------
CSG_Data_Manager::~CSG_Data_Manager(void)
{
	Delete();

	delete(m_pTable     );
	delete(m_pShapes    );
	delete(m_pPointCloud);
	delete(m_pTIN       );
	delete(m_pGrid      );
	delete(m_pGrids     );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Collection * CSG_Data_Manager::_Get_Collection(CSG_Data_Object *pObject) const
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
	{
		switch( pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Table     : return( m_pTable      );
		case SG_DATAOBJECT_TYPE_Shapes    : return( m_pShapes     );
		case SG_DATAOBJECT_TYPE_PointCloud: return( m_pPointCloud );
		case SG_DATAOBJECT_TYPE_TIN       : return( m_pTIN        );
		case SG_DATAOBJECT_TYPE_Grid      : return( m_pGrid       );
		case SG_DATAOBJECT_TYPE_Grids     : return( m_pGrids      );

		default: break;
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Manager::Find(const CSG_String &File, bool bNative) const
{
	CSG_Data_Object *pObject;

	if( (pObject = Table     ().Find(File, bNative)) != NULL ) return( pObject );
	if( (pObject = Shapes    ().Find(File, bNative)) != NULL ) return( pObject );
	if( (pObject = PointCloud().Find(File, bNative)) != NULL ) return( pObject );
	if( (pObject = TIN       ().Find(File, bNative)) != NULL ) return( pObject );
	if( (pObject = Grid      ().Find(File, bNative)) != NULL ) return( pObject );
	if( (pObject = Grids     ().Find(File, bNative)) != NULL ) return( pObject );

	return(	NULL );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Exists(CSG_Data_Object *pObject) const
{
	if( pObject && pObject != DATAOBJECT_CREATE )
	{
		if( Table     ().Exists(pObject) ) return( true );
		if( Shapes    ().Exists(pObject) ) return( true );
		if( PointCloud().Exists(pObject) ) return( true );
		if( TIN       ().Exists(pObject) ) return( true );
		if( Grid      ().Exists(pObject) ) return( true );
		if( Grids     ().Exists(pObject) ) return( true );

		for(size_t i=0; i<Grids().Count(); i++)
		{
			for(int j=0; j<Grids(i).Get_Grid_Count(); j++)
			{
				if( pObject == Grids(i).Get_Grid_Ptr(j) )
				{
					return( true );
				}
			}
		}
	}

	return(	false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Manager::Add(CSG_Data_Object *pObject)
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
	{
		if( Exists(pObject) )
		{
			return( pObject ); // don't add more than once
		}

		CSG_Data_Collection *pCollection = _Get_Collection(pObject);

		if( pCollection && pCollection->Add(pObject) )
		{
			if( this == &g_Data_Manager ) // SAGA API's global data manager ?
			{
				SG_UI_DataObject_Add(pObject, SG_UI_DATAOBJECT_UPDATE); // for SAGA GUI !
			}

			return( pObject );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Manager::Add(const char       *File, TSG_Data_Object_Type Type) { return( Add(CSG_String(File), Type) ); }
CSG_Data_Object * CSG_Data_Manager::Add(const wchar_t    *File, TSG_Data_Object_Type Type) { return( Add(CSG_String(File), Type) ); }
CSG_Data_Object * CSG_Data_Manager::Add(const CSG_String &File, TSG_Data_Object_Type Type)
{
	if( Type == SG_DATAOBJECT_TYPE_Undefined )
	{
		if( SG_File_Cmp_Extension(File, "txt"     )
		||	SG_File_Cmp_Extension(File, "csv"     )
		||	SG_File_Cmp_Extension(File, "dbf"     ) )
		{
			Type = SG_DATAOBJECT_TYPE_Table;
		}

		if( SG_File_Cmp_Extension(File, "shp"     ) )
		{
			Type = SG_DATAOBJECT_TYPE_Shapes;
		}

		if( SG_File_Cmp_Extension(File, "sg-pts-z")
		||  SG_File_Cmp_Extension(File, "sg-pts"  )
		||  SG_File_Cmp_Extension(File, "spc"     ) )
		{
			Type = SG_DATAOBJECT_TYPE_PointCloud;
		}

		if(	SG_File_Cmp_Extension(File, "sg-grd-z")
		||	SG_File_Cmp_Extension(File, "sg-grd"  )
		||	SG_File_Cmp_Extension(File, "sgrd"    )
		||	SG_File_Cmp_Extension(File, "dgm"     )
		||	SG_File_Cmp_Extension(File, "grd"     ) )
		{
			Type = SG_DATAOBJECT_TYPE_Grid;
		}

		if( SG_File_Cmp_Extension(File, "sg-gds-z")
		||  SG_File_Cmp_Extension(File, "sg-gds"  ) )
		{
			Type = SG_DATAOBJECT_TYPE_Grids;
		}
	}

	//-----------------------------------------------------
	CSG_Data_Object *pObject;

	switch( Type )
	{
	case SG_DATAOBJECT_TYPE_Table     : pObject = new CSG_Table     (File); break;
	case SG_DATAOBJECT_TYPE_Shapes    : pObject = new CSG_Shapes    (File); break;
	case SG_DATAOBJECT_TYPE_TIN       : pObject = new CSG_TIN       (File); break;
	case SG_DATAOBJECT_TYPE_PointCloud: pObject = new CSG_PointCloud(File); break;
	case SG_DATAOBJECT_TYPE_Grid      : pObject = new CSG_Grid      (File); break;
	case SG_DATAOBJECT_TYPE_Grids     : pObject = new CSG_Grids     (File); break;
	default                           : pObject = NULL                    ; break;
	}

	if( pObject )
	{
		if( pObject->is_Valid() && Add(pObject) )
		{
			return( pObject );
		}

		delete(pObject);
	}

	//-----------------------------------------------------
	return( _Add_External(File) );
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Manager::_Add_External(const CSG_String &File)
{
	CSG_Data_Object *pData = NULL;

	if( !SG_File_Exists(File) )
	{
		return( pData );
	}

	CSG_Tool *pImport = NULL;

	SG_UI_Msg_Lock(true);

	//-----------------------------------------------------
	// Image Import

	if(	( SG_File_Cmp_Extension(File, "bmp")
	   || SG_File_Cmp_Extension(File, "gif")
	   || SG_File_Cmp_Extension(File, "jpg")
	   || SG_File_Cmp_Extension(File, "png")
	   || SG_File_Cmp_Extension(File, "pcx")
	   || SG_File_Cmp_Extension(File, "xpm") )
	&&  (pImport = SG_Get_Tool_Library_Manager().Create_Tool("io_grid_image", 1)) != NULL
	&&   pImport->Set_Parameter("FILE", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData = pImport->Get_Parameter("OUT_GRID")->asDataObject();
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pImport);

	//-----------------------------------------------------
	// GDAL Import

	if( !pData
	&&  (pImport = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData = pImport->Get_Parameter("GRIDS")->asList()->Get_Item(0);
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pImport);

	//-----------------------------------------------------
	// OGR Import

	if( !pData
	&&  (pImport = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 3)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData = pImport->Get_Parameter("SHAPES")->asList()->Get_Item(0);
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pImport);

	//-----------------------------------------------------
	// LAZ Import

	if( !pData && (SG_File_Cmp_Extension(File, "las") || SG_File_Cmp_Extension(File, "laz"))
	&&  (pImport = SG_Get_Tool_Library_Manager().Create_Tool("io_pdal", 0)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData = pImport->Get_Parameter("POINTS")->asList()->Get_Item(0);
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pImport);

	//-----------------------------------------------------
	// STL Import

	if( !pData && (SG_File_Cmp_Extension(File, "stl"))
	&&  (pImport = SG_Get_Tool_Library_Manager().Create_Tool("io_shapes", 11)) != NULL // Import Stereo Lithography File (STL)
	&&   pImport->Set_Parameter("FILE"  , File, PARAMETER_TYPE_FilePath)
	&&   pImport->Set_Parameter("METHOD", 2) ) // => TIN
	{
		CSG_Data_Manager Data; pImport->Set_Manager(&Data);

		if( pImport->Execute() )
		{
			Data.Delete(pData = pImport->Get_Parameter("TIN")->asTIN(), true); Add(pData);
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pImport);

	//-----------------------------------------------------
	SG_UI_Msg_Lock(false);

	return( pData );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table * CSG_Data_Manager::Add_Table(const CSG_String &File) { return( (CSG_Table *)Add(File, SG_DATAOBJECT_TYPE_Table) ); }
CSG_Table * CSG_Data_Manager::Add_Table(const char       *File) { return( (CSG_Table *)Add(File, SG_DATAOBJECT_TYPE_Table) ); }
CSG_Table * CSG_Data_Manager::Add_Table(const wchar_t    *File) { return( (CSG_Table *)Add(File, SG_DATAOBJECT_TYPE_Table) ); }
CSG_Table * CSG_Data_Manager::Add_Table(void)
{
	CSG_Table *pObject = new CSG_Table();
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_Shapes * CSG_Data_Manager::Add_Shapes(const CSG_String &File) { return( (CSG_Shapes *)Add(File, SG_DATAOBJECT_TYPE_Shapes) ); }
CSG_Shapes * CSG_Data_Manager::Add_Shapes(const char       *File) { return( (CSG_Shapes *)Add(File, SG_DATAOBJECT_TYPE_Shapes) ); }
CSG_Shapes * CSG_Data_Manager::Add_Shapes(const wchar_t    *File) { return( (CSG_Shapes *)Add(File, SG_DATAOBJECT_TYPE_Shapes) ); }
CSG_Shapes * CSG_Data_Manager::Add_Shapes(void)
{
	CSG_Shapes *pObject = new CSG_Shapes();

	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_PointCloud * CSG_Data_Manager::Add_PointCloud(const CSG_String &File) { return( (CSG_PointCloud *)Add(File, SG_DATAOBJECT_TYPE_PointCloud) ); }
CSG_PointCloud * CSG_Data_Manager::Add_PointCloud(const char       *File) { return( (CSG_PointCloud *)Add(File, SG_DATAOBJECT_TYPE_PointCloud) ); }
CSG_PointCloud * CSG_Data_Manager::Add_PointCloud(const wchar_t    *File) { return( (CSG_PointCloud *)Add(File, SG_DATAOBJECT_TYPE_PointCloud) ); }
CSG_PointCloud * CSG_Data_Manager::Add_PointCloud(void)
{
	CSG_PointCloud *pObject = new CSG_PointCloud();
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_TIN * CSG_Data_Manager::Add_TIN(const CSG_String &File) { return( (CSG_TIN *)Add(File, SG_DATAOBJECT_TYPE_TIN) ); }
CSG_TIN * CSG_Data_Manager::Add_TIN(const char       *File) { return( (CSG_TIN *)Add(File, SG_DATAOBJECT_TYPE_TIN) ); }
CSG_TIN * CSG_Data_Manager::Add_TIN(const wchar_t    *File) { return( (CSG_TIN *)Add(File, SG_DATAOBJECT_TYPE_TIN) ); }
CSG_TIN * CSG_Data_Manager::Add_TIN(void)
{
	CSG_TIN *pObject = new CSG_TIN();

	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_Grid * CSG_Data_Manager::Add_Grid(const CSG_String &File) { return( (CSG_Grid *)Add(File, SG_DATAOBJECT_TYPE_Grid) ); }
CSG_Grid * CSG_Data_Manager::Add_Grid(const char       *File) { return( (CSG_Grid *)Add(File, SG_DATAOBJECT_TYPE_Grid) ); }
CSG_Grid * CSG_Data_Manager::Add_Grid(const wchar_t    *File) { return( (CSG_Grid *)Add(File, SG_DATAOBJECT_TYPE_Grid) ); }
CSG_Grid * CSG_Data_Manager::Add_Grid(void)
{
	CSG_Grid *pObject = new CSG_Grid();
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_Grids * CSG_Data_Manager::Add_Grids(const CSG_String &File) { return( (CSG_Grids *)Add(File, SG_DATAOBJECT_TYPE_Grids) ); }
CSG_Grids * CSG_Data_Manager::Add_Grids(const char       *File) { return( (CSG_Grids *)Add(File, SG_DATAOBJECT_TYPE_Grids) ); }
CSG_Grids * CSG_Data_Manager::Add_Grids(const wchar_t    *File) { return( (CSG_Grids *)Add(File, SG_DATAOBJECT_TYPE_Grids) ); }
CSG_Grids * CSG_Data_Manager::Add_Grids(void)
{
	CSG_Grids *pObject = new CSG_Grids();

	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Manager::Delete(CSG_Data_Object *pObject, bool bDetach)
{
	CSG_Data_Collection *pCollection = _Get_Collection(pObject);

	return( pCollection && pCollection->Delete(pObject, bDetach) );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Delete(bool bDetach, bool bUnsaved)
{
	Table     ().Delete(bDetach, bUnsaved);
	Shapes    ().Delete(bDetach, bUnsaved);
	PointCloud().Delete(bDetach, bUnsaved);
	TIN       ().Delete(bDetach, bUnsaved);
	Grid      ().Delete(bDetach, bUnsaved);
	Grids     ().Delete(bDetach, bUnsaved);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Data_Manager::Get_Summary(void)	const
{
	CSG_String s;

	//-----------------------------------------------------
	if( Table().Count() )
	{
		s += CSG_String::Format("___\n%s [%zu %s]\n", _TL("Table"), Table().Count(), _TL("objects"));

		for(size_t i=0; i<Table().Count(); i++)
		{
			CSG_Table *pObject = Table()[i].asTable();

			s += CSG_String::Format("- [%d %s] %s\n",
				pObject->Get_Count(), _TL("records"),
				pObject->Get_Name()
			);
		}
	}

	//-----------------------------------------------------
	if( Shapes().Count() )
	{
		s += CSG_String::Format("___\n%s [%zu %s]\n", _TL("Shapes"), Shapes().Count(), _TL("objects"));

		for(size_t i=0; i<Shapes().Count(); i++)
		{
			CSG_Shapes *pObject = Shapes()[i].asShapes();

			s += CSG_String::Format("- [%s; %d %s] %s\n",
				pObject->Get_Type() == SHAPE_TYPE_Point   ? _TL("point"  ) :
				pObject->Get_Type() == SHAPE_TYPE_Points  ? _TL("points" ) :
				pObject->Get_Type() == SHAPE_TYPE_Line    ? _TL("line"   ) :
				pObject->Get_Type() == SHAPE_TYPE_Polygon ? _TL("polygon") : _TL("unknown"),
				pObject->Get_Count(), _TL("records"),
				pObject->Get_Name()
			);
		}
	}

	//-----------------------------------------------------
	if( PointCloud().Count() )
	{
		s += CSG_String::Format("___\n%s [%zu %s]\n", _TL("Point Cloud"), PointCloud().Count(), _TL("objects"));

		for(size_t i=0; i<PointCloud().Count(); i++)
		{
			CSG_PointCloud *pObject = PointCloud()[i].asPointCloud();

			s += CSG_String::Format("- [%d %s] %s\n",
				pObject->Get_Count(), _TL("records"),
				pObject->Get_Name()
			);
		}
	}

	//-----------------------------------------------------
	if( TIN().Count() )
	{
		s += CSG_String::Format("___\n%s [%zu %s]\n", _TL("TIN"), TIN().Count(), _TL("objects"));

		for(size_t i=0; i<TIN().Count(); i++)
		{
			CSG_TIN *pObject = TIN()[i].asTIN();

			s += CSG_String::Format("- [%d %s] %s\n",
				pObject->Get_Count(), _TL("nodes"),
				pObject->Get_Name()
			);
		}
	}

	//-----------------------------------------------------
	if( Grid().Count() )
	{
		s += CSG_String::Format("___\n%s [%zu %s]\n", _TL("Grid"), Grid().Count(), _TL("objects"));

		for(size_t i=0; i<Grid().Count(); i++)
		{
			CSG_Grid *pObject = Grid()[i].asGrid ();

			s += CSG_String::Format("- [%f | %dnx %dny | %fx %fy] %s\n",
				pObject->Get_Cellsize(), pObject->Get_NX(), pObject->Get_NY(), pObject->Get_XMin(), pObject->Get_YMin(),
				pObject->Get_Name()
			);
		}
	}

	//-----------------------------------------------------
	if( Grids().Count() )
	{
		s += CSG_String::Format("___\n%s [%zu %s]\n", _TL("Grid Collection"), Grids().Count(), _TL("objects"));

		for(size_t i=0; i<Grids().Count(); i++)
		{
			CSG_Grids *pObject = Grids()[i].asGrids();

			s += CSG_String::Format("- [%f | %dnx %dny %dnz | %fx %fy] %s\n",
				pObject->Get_Cellsize(), pObject->Get_NX(), pObject->Get_NY(), pObject->Get_NZ(), pObject->Get_XMin(), pObject->Get_YMin(),
				pObject->Get_Name()
			);
		}
	}

	//-----------------------------------------------------
	if( s.is_Empty() )
	{
		s	+= CSG_String::Format("%s - %s\n--- %s ---\n", _TL("Data Manager"), _TL("Summary"), _TL("no data"));
	}
	else
	{
		s.Prepend(CSG_String::Format("%s - %s\n", _TL("Data Manager"), _TL("Summary")));
	}

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
