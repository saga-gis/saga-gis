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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
CSG_Data_Manager		g_Data_Manager;

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
CSG_Data_Collection::CSG_Data_Collection(CSG_Data_Manager *pManager, TSG_Data_Object_Type Type)
{
	m_pManager	= pManager;
	m_Type		= Type;
}

//---------------------------------------------------------
CSG_Data_Collection::~CSG_Data_Collection(void)
{
	Delete_All();
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Collection::Get(const CSG_String &File, bool bNative) const
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
			m_Objects[Count() - 1]	= pObject;

			if( m_pManager == &g_Data_Manager )
			{
				SG_UI_DataObject_Add(pObject, SG_UI_DATAOBJECT_UPDATE_ONLY);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete(CSG_Data_Object *pObject, bool bDetachOnly)
{
	CSG_Data_Object	**pObjects	= (CSG_Data_Object **)m_Objects.Get_Array();

	size_t	i, n;

	for(i=0, n=0; i<Count(); i++)
	{
		if( pObject == Get(i) )
		{
			if( !bDetachOnly )
			{
				delete(Get(i));

				bDetachOnly	= true;	// just in case the same object has been added more than once
			}
		}
		else
		{
			pObjects[n++]	= pObjects[i];
		}
	}

	if( n < m_Objects.Get_Size() )
	{
		m_Objects.Set_Array(n);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete(size_t i, bool bDetachOnly)
{
	return( Delete(Get(i), bDetachOnly) );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete_All(bool bDetachOnly)
{
	if( !bDetachOnly )
	{
		for(size_t i=0; i<Count(); i++)
		{
			delete(Get(i));
		}
	}

	m_Objects.Set_Array(0);

	return( true );
}

//---------------------------------------------------------
bool CSG_Data_Collection::Delete_Unsaved(bool bDetachOnly)
{
	for(size_t i=Count(); i>0; i--)
	{
		if( !SG_File_Exists(Get(i - 1)->Get_File_Name()) )
		{
			Delete(i, bDetachOnly);
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
CSG_Grid_Collection::CSG_Grid_Collection(CSG_Data_Manager *pManager)
	: CSG_Data_Collection(pManager, SG_DATAOBJECT_TYPE_Grid)
{}

//---------------------------------------------------------
bool CSG_Grid_Collection::Exists(CSG_Data_Object *pObject) const
{
	if( pObject == DATAOBJECT_NOTSET || pObject == DATAOBJECT_CREATE )
	{
		return( false );
	}

	for(size_t i=0; i<Count(); i++)
	{
		if( pObject == Get(i) )
		{
			return( true );
		}
		else if( Get(i)->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
		{	// does object (CSG_Grid) belong to a grid collection (CSG_Grids)
			CSG_Grids	*pGrids	= (CSG_Grids *)Get(i);

			for(int j=0; j<pGrids->Get_NZ(); j++)
			{
				if( pObject == pGrids->Get_Grid_Ptr(j) )
				{
					return( true );
				}
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Collection::Add(CSG_Data_Object *pObject)
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
	{
		CSG_Grid_System	System;

		switch( pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid :	System	= ((CSG_Grid  *)pObject)->Get_System(); break;
		case SG_DATAOBJECT_TYPE_Grids:	System	= ((CSG_Grids *)pObject)->Get_System(); break;

		default:	return( false );
		}

		if( System.is_Valid() )
		{
			if( Count() == 0 || !m_System.is_Valid() )
			{
				m_System	= System;
			}

			if( m_System == System )
			{
				return( CSG_Data_Collection::Add(pObject) );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Manager::CSG_Data_Manager(void)
{
	m_pTable		= new CSG_Data_Collection(this, SG_DATAOBJECT_TYPE_Table     );
	m_pTIN			= new CSG_Data_Collection(this, SG_DATAOBJECT_TYPE_TIN       );
	m_pPoint_Cloud	= new CSG_Data_Collection(this, SG_DATAOBJECT_TYPE_PointCloud);
	m_pShapes		= new CSG_Data_Collection(this, SG_DATAOBJECT_TYPE_Shapes    );
}

//---------------------------------------------------------
CSG_Data_Manager::~CSG_Data_Manager(void)
{
	Delete_All();

	delete(m_pTable      );
	delete(m_pTIN        );
	delete(m_pPoint_Cloud);
	delete(m_pShapes     );
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
		case SG_DATAOBJECT_TYPE_Table     : return( m_pTable       );
		case SG_DATAOBJECT_TYPE_TIN       : return( m_pTIN         );
		case SG_DATAOBJECT_TYPE_PointCloud: return( m_pPoint_Cloud );
		case SG_DATAOBJECT_TYPE_Shapes    : return( m_pShapes      );
		case SG_DATAOBJECT_TYPE_Grid      : return( Get_Grid_System(((CSG_Grid  *)pObject)->Get_System()) );
		case SG_DATAOBJECT_TYPE_Grids     : return( Get_Grid_System(((CSG_Grids *)pObject)->Get_System()) );

		default: break;
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_Collection * CSG_Data_Manager::Get_Grid_System(const CSG_Grid_System &System) const
{
	for(size_t i=0; i<Grid_System_Count(); i++)
	{
		CSG_Grid_Collection	*pSystem	= Get_Grid_System(i);

		if( pSystem->is_Equal(System) )
		{
			return( pSystem );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Exists(const CSG_Grid_System &System) const
{
	return( Get_Grid_System(System) != NULL );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Exists(CSG_Data_Object *pObject) const
{
	if( m_pTable      ->Exists(pObject) )	return( true );
	if( m_pTIN        ->Exists(pObject) )	return( true );
	if( m_pPoint_Cloud->Exists(pObject) )	return( true );
	if( m_pShapes     ->Exists(pObject) )	return( true );

	for(size_t i=0; i<Grid_System_Count(); i++)
	{
		if( Get_Grid_System(i)->Exists(pObject) )	return( true );
	}

	return(	false );
}

//---------------------------------------------------------
CSG_Data_Object *  CSG_Data_Manager::Find(const CSG_String &File, bool bNative) const
{
	CSG_Data_Object	*pObject;

	if( (pObject = m_pTable      ->Get(File, bNative)) != NULL )	return( pObject );
	if( (pObject = m_pTIN        ->Get(File, bNative)) != NULL )	return( pObject );
	if( (pObject = m_pPoint_Cloud->Get(File, bNative)) != NULL )	return( pObject );
	if( (pObject = m_pShapes     ->Get(File, bNative)) != NULL )	return( pObject );

	for(size_t i=0; i<Grid_System_Count(); i++)
	{
		if( (pObject = Get_Grid_System(i)->Get(File, bNative)) != NULL )	return( pObject );
	}

	return(	NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Manager::Add(CSG_Data_Object *pObject)
{
	CSG_Data_Collection	*pCollection	= _Get_Collection(pObject);

	if( pCollection == NULL && pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE && (pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid || pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids) && m_Grid_Systems.Inc_Array() )
	{
		pCollection	= new CSG_Grid_Collection(this);

		m_Grid_Systems[m_Grid_Systems.Get_Size() - 1]	= pCollection;
	}

	return( pCollection && pCollection->Add(pObject) );
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Manager::Add(const CSG_String &File, TSG_Data_Object_Type Type)
{
	//-----------------------------------------------------
	if( Type == SG_DATAOBJECT_TYPE_Undefined )
	{
		if( SG_File_Cmp_Extension(File, "txt"     )
		||	SG_File_Cmp_Extension(File, "csv"     )
		||	SG_File_Cmp_Extension(File, "dbf"     ) )
		{
			Type	= SG_DATAOBJECT_TYPE_Table;
		}

		if( SG_File_Cmp_Extension(File, "shp"     ) )
		{
			Type	= SG_DATAOBJECT_TYPE_Shapes;
		}

		if( SG_File_Cmp_Extension(File, "sg-pts-z")
		||  SG_File_Cmp_Extension(File, "sg-pts"  )
		||  SG_File_Cmp_Extension(File, "spc"     ) )
		{
			Type	= SG_DATAOBJECT_TYPE_PointCloud;
		}

		if(	SG_File_Cmp_Extension(File, "sg-grd-z")
		||	SG_File_Cmp_Extension(File, "sg-grd"  )
		||	SG_File_Cmp_Extension(File, "sgrd"    )
		||	SG_File_Cmp_Extension(File, "dgm"     )
		||	SG_File_Cmp_Extension(File, "grd"     ) )
		{
			Type	= SG_DATAOBJECT_TYPE_Grid;
		}

		if( SG_File_Cmp_Extension(File, "sg-gds-z")
		||  SG_File_Cmp_Extension(File, "sg-gds"  ) )
		{
			Type	= SG_DATAOBJECT_TYPE_Grids;
		}
	}

	//-----------------------------------------------------
	CSG_Data_Object	*pObject;

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
	CSG_Data_Object *pData	= NULL;

	if( !SG_File_Exists(File) )
	{
		return( pData );
	}

	CSG_Tool	*pImport;

	SG_UI_Msg_Lock(true);

	//-----------------------------------------------------
	// Image Import

	if(	(	SG_File_Cmp_Extension(File, "bmp")
		||	SG_File_Cmp_Extension(File, "gif")
		||	SG_File_Cmp_Extension(File, "jpg")
		||	SG_File_Cmp_Extension(File, "png")
		||	SG_File_Cmp_Extension(File, "pcx") )
	&&  (pImport = SG_Get_Tool_Library_Manager().Get_Tool("io_grid_image", 1)) != NULL
	&&   pImport->Set_Parameter("FILE", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData	= (*pImport->Get_Parameters())("OUT_GRID")->asDataObject();
		}

		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------
	// GDAL Import

	if( !pData
	&&  (pImport = SG_Get_Tool_Library_Manager().Get_Tool("io_gdal", 0)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData	= (*pImport->Get_Parameters())("GRIDS")->asList()->Get_Item(0);
		}

		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------
	// OGR Import

	if( !pData
	&&  (pImport = SG_Get_Tool_Library_Manager().Get_Tool("io_gdal", 3)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData	= (*pImport->Get_Parameters())("SHAPES")->asList()->Get_Item(0);
		}

		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------
	// LAS Import

	if( !pData && SG_File_Cmp_Extension(File, "las")
	&&  (pImport = SG_Get_Tool_Library_Manager().Get_Tool("io_shapes_las", 1)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);

		if( pImport->Execute() )
		{
			pData	= (*pImport->Get_Parameters())("POINTS")->asDataObject();
		}

		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------
	SG_UI_Msg_Lock(false);

	return( pData );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table * CSG_Data_Manager::Add_Table(void)
{
	CSG_Table	*pObject	= new CSG_Table();
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_TIN * CSG_Data_Manager::Add_TIN(void)
{
	CSG_TIN	*pObject	= new CSG_TIN();
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_PointCloud * CSG_Data_Manager::Add_PointCloud(void)
{
	CSG_PointCloud	*pObject	= new CSG_PointCloud();
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_Shapes * CSG_Data_Manager::Add_Shapes(TSG_Shape_Type Type)
{
	CSG_Shapes	*pObject	= new CSG_Shapes(Type);
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_Grid * CSG_Data_Manager::Add_Grid(const CSG_Grid_System &System, TSG_Data_Type Type)
{
	CSG_Grid	*pObject	= System.is_Valid() ? new CSG_Grid(System, Type) : NULL;
	
	if( pObject && !Add(pObject) )
	{
		delete(pObject); pObject = NULL;
	}

	return( pObject );
}

//---------------------------------------------------------
CSG_Grid * CSG_Data_Manager::Add_Grid(int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Data_Type Type)
{
	return( Add_Grid(CSG_Grid_System(Cellsize, xMin, yMin, NX, NY), Type) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Manager::Delete(CSG_Data_Collection *pCollection, bool bDetachOnly)
{
	if( pCollection == NULL || pCollection->m_pManager != this )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pCollection == m_pTable       )	{	return( pCollection->Delete_All(bDetachOnly) );	}
	if( pCollection == m_pTIN         )	{	return( pCollection->Delete_All(bDetachOnly) );	}
	if( pCollection == m_pPoint_Cloud )	{	return( pCollection->Delete_All(bDetachOnly) );	}
	if( pCollection == m_pShapes      )	{	return( pCollection->Delete_All(bDetachOnly) );	}

	//-----------------------------------------------------
	if( pCollection->m_Type == SG_DATAOBJECT_TYPE_Grid )
	{
		CSG_Grid_Collection	**pSystems	= (CSG_Grid_Collection **)m_Grid_Systems.Get_Array();

		size_t	i, n;

		for(i=0, n=0; i<m_Grid_Systems.Get_Size(); i++)
		{
			if( pCollection == pSystems[i] )
			{
				if( bDetachOnly )
				{
					pSystems[i]->Delete_All(bDetachOnly);
				}

				delete(pSystems[i]);
			}
			else
			{
				pSystems[n++]	= pSystems[i];
			}
		}

		if( n < m_Grid_Systems.Get_Size() )
		{
			m_Grid_Systems.Set_Array(n);

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Delete(CSG_Data_Object *pObject, bool bDetachOnly)
{
	CSG_Data_Collection	*pCollection	= _Get_Collection(pObject);

	if( pCollection && pCollection->Delete(pObject, bDetachOnly) )
	{
		if( pCollection->m_Type == SG_DATAOBJECT_TYPE_Grid && pCollection->Count() == 0 )
		{
			Delete(pCollection, bDetachOnly);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Delete(const CSG_Grid_System &System, bool bDetachOnly)
{
	return( Delete(Get_Grid_System(System), bDetachOnly) );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Delete_All(bool bDetachOnly)
{
	m_pTable      ->Delete_All(bDetachOnly);
	m_pTIN        ->Delete_All(bDetachOnly);
	m_pPoint_Cloud->Delete_All(bDetachOnly);
	m_pShapes     ->Delete_All(bDetachOnly);

	for(size_t i=0; i<Grid_System_Count(); i++)
	{
		CSG_Grid_Collection	*pSystem	= Get_Grid_System(i);

		pSystem->Delete_All(bDetachOnly);

		delete(pSystem);
	}

	m_Grid_Systems.Set_Array(0);

	return( true );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Delete_Unsaved(bool bDetachOnly)
{
	m_pTable      ->Delete_Unsaved(bDetachOnly);
	m_pTIN        ->Delete_Unsaved(bDetachOnly);
	m_pPoint_Cloud->Delete_Unsaved(bDetachOnly);
	m_pShapes     ->Delete_Unsaved(bDetachOnly);

	for(size_t i=Grid_System_Count(); i>0; i--)
	{
		CSG_Grid_Collection	*pSystem	= Get_Grid_System(i - 1);

		pSystem->Delete_Unsaved(bDetachOnly);

		if( pSystem->Count() == 0 )
		{
			Delete(pSystem);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Data_Manager::Get_Summary(void)	const
{
	CSG_String	s;

	//-----------------------------------------------------
	if( Get_Table()->Count() > 0 )
	{
		s	+= CSG_String::Format("___\n%s [%d %s]\n", _TL("Table"), Get_Table()->Count(), _TL("objects"));

		for(size_t i=0; i<Get_Table()->Count(); i++)
		{
			CSG_Table	*pObject	= (CSG_Table *)Get_Table()->Get(i);

			s	+= CSG_String::Format("- %s [%d %s]\n", pObject->Get_Name(), pObject->Get_Count(), _TL("records"));
		}
	}

	//-----------------------------------------------------
	if( Get_Table()->Count() > 0 )
	{
		s	+= CSG_String::Format("___\n%s [%d %s]\n", _TL("Shapes"), Get_Shapes()->Count(), _TL("objects"));

		for(size_t i=0; i<Get_Shapes()->Count(); i++)
		{
			CSG_Shapes	*pObject	= (CSG_Shapes *)Get_Shapes()->Get(i);

			s	+= CSG_String::Format("- %s [%s; %d %s]\n", pObject->Get_Name(),
				pObject->Get_Type() == SHAPE_TYPE_Point   ? _TL("point"  ) :
				pObject->Get_Type() == SHAPE_TYPE_Points  ? _TL("points" ) :
				pObject->Get_Type() == SHAPE_TYPE_Line    ? _TL("line"   ) :
				pObject->Get_Type() == SHAPE_TYPE_Polygon ? _TL("polygon") : _TL("unknown"),
				pObject->Get_Count(), _TL("records"));
		}
	}

	//-----------------------------------------------------
	if( Get_Point_Cloud()->Count() > 0 )
	{
		s	+= CSG_String::Format("___\n%s [%d %s]\n", _TL("Point Cloud"), Get_Point_Cloud()->Count(), _TL("objects"));

		for(size_t i=0; i<Get_Table()->Count(); i++)
		{
			CSG_PointCloud	*pObject	= (CSG_PointCloud *)Get_Point_Cloud()->Get(i);

			s	+= CSG_String::Format("- %s [%d %s]\n", pObject->Get_Name(), pObject->Get_Count(), _TL("records"));
		}
	}

	//-----------------------------------------------------
//	if( Get_TIN()->Count() > 0 )
//	{
//		s	+= CSG_String::Format("___\n%s [%d %s]\n", _TL("TIN"), Get_TIN()->Count(), _TL("objects"));
//
//		for(size_t i=0; i<Get_TIN()->Count(); i++)
//		{
//			CSG_TIN	*pObject	= (CSG_TIN *)Get_TIN()->Get(i);
//
//			s	+= CSG_String::Format("- %s [%d %s]\n", pObject->Get_Name(), pObject->Get_Count(), _TL("nodes"));
//		}
//	}

	//-----------------------------------------------------
	if( Grid_System_Count() > 0 )
	{
		s	+= CSG_String::Format("___\n%s [%d %s]\n", _TL("Grid System"), Grid_System_Count(), _TL("systems"));

		for(size_t i=0; i<Grid_System_Count(); i++)
		{
			CSG_Grid_Collection	*pSystem	= Get_Grid_System(i);

			s	+= CSG_String::Format("- %s [%s; %d %s]\n", _TL("Grid System"), pSystem->m_System.Get_Name(), pSystem->Count(), _TL("objects"));

			for(size_t j=0; j<pSystem->Count(); j++)
			{
				if( pSystem->Get(j)->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid )
				{
					CSG_Grid	*pObject	= (CSG_Grid *)pSystem->Get(j);

					s	+= CSG_String::Format("- %s [%s]\n", pObject->Get_Name(), SG_Data_Type_Get_Name(pObject->Get_Type()));
				}

				if( pSystem->Get(j)->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
				{
					CSG_Grids	*pObject	= (CSG_Grids *)pSystem->Get(j);

					s	+= CSG_String::Format("- %s [%s; %d %s]\n", pObject->Get_Name(), SG_Data_Type_Get_Name(pObject->Get_Type()), pObject->Get_NZ(), _TL("grids"));
				}
			}
		}
	}

	//-----------------------------------------------------
	if( s.is_Empty() )
	{
		s	= CSG_String::Format("%s - %s\n--- %s ---", _TL("Data Manager"), _TL("Summary"), _TL("no data"));
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
