/**********************************************************
 * Version $Id: data_manager.cpp 1495 2012-10-19 14:03:30Z oconrad $
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
#include "module_library.h"


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

	m_Objects.Create(sizeof(CSG_Data_Object *));
}

//---------------------------------------------------------
CSG_Data_Collection::~CSG_Data_Collection(void)
{
	Delete_All();
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Data_Collection::Get(const CSG_String &File) const
{
	for(size_t i=0; i<Count(); i++)
	{
		if( !File.Cmp(Get(i)->Get_File_Name()) )
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
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE && pObject->Get_ObjectType() == m_Type )
	{
		if( Exists(pObject) )
		{
			return( true );
		}

		if( m_Objects.Inc_Array() )
		{
			((CSG_Data_Object **)m_Objects.Get_Array())[Count() - 1]	= pObject;

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
	: CSG_Data_Collection(pManager, DATAOBJECT_TYPE_Grid)
{}

//---------------------------------------------------------
bool CSG_Grid_Collection::Exists(CSG_Data_Object *pObject) const
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
	{
		return( CSG_Data_Collection::Exists(pObject) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Collection::Add(CSG_Data_Object *pObject)
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE && pObject->Get_ObjectType() == DATAOBJECT_TYPE_Grid )
	{
		CSG_Grid_System	System	= ((CSG_Grid *)pObject)->Get_System();

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
	m_pTable		= new CSG_Data_Collection(this, DATAOBJECT_TYPE_Table     );
	m_pTIN			= new CSG_Data_Collection(this, DATAOBJECT_TYPE_TIN       );
	m_pPoint_Cloud	= new CSG_Data_Collection(this, DATAOBJECT_TYPE_PointCloud);
	m_pShapes		= new CSG_Data_Collection(this, DATAOBJECT_TYPE_Shapes    );

	m_Grid_Systems.Create(sizeof(CSG_Grid_Collection *));
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
		default: break;

		case DATAOBJECT_TYPE_Table:      return( m_pTable       );
		case DATAOBJECT_TYPE_TIN:        return( m_pTIN         );
		case DATAOBJECT_TYPE_PointCloud: return( m_pPoint_Cloud );
		case DATAOBJECT_TYPE_Shapes:     return( m_pShapes      );

		case DATAOBJECT_TYPE_Grid:       return( Get_Grid_System(((CSG_Grid *)pObject)->Get_System()) );
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
CSG_Data_Object *  CSG_Data_Manager::Find(const CSG_String &File) const
{
	CSG_Data_Object	*pObject;

	if( (pObject = m_pTable      ->Get(File)) != NULL )	return( pObject );
	if( (pObject = m_pTIN        ->Get(File)) != NULL )	return( pObject );
	if( (pObject = m_pPoint_Cloud->Get(File)) != NULL )	return( pObject );
	if( (pObject = m_pShapes     ->Get(File)) != NULL )	return( pObject );

	for(size_t i=0; i<Grid_System_Count(); i++)
	{
		if( (pObject = Get_Grid_System(i)->Get(File)) != NULL )	return( pObject );
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

	if( pCollection == NULL && pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE && pObject->Get_ObjectType() == DATAOBJECT_TYPE_Grid && m_Grid_Systems.Inc_Array() )
	{
		pCollection	= new CSG_Grid_Collection(this);

		((CSG_Data_Collection **)m_Grid_Systems.Get_Array())[m_Grid_Systems.Get_Size() - 1]	= pCollection;
	}

	return( pCollection && pCollection->Add(pObject) );
}

//---------------------------------------------------------
bool CSG_Data_Manager::Add(const CSG_String &File, TSG_Data_Object_Type Type)
{
	//-----------------------------------------------------
	if( Type == DATAOBJECT_TYPE_Undefined )
	{
		if( SG_File_Cmp_Extension(File, SG_T("txt" ))
		||	SG_File_Cmp_Extension(File, SG_T("csv" ))
		||	SG_File_Cmp_Extension(File, SG_T("dbf" )) )
		{
			Type	= DATAOBJECT_TYPE_Table;
		}

		if( SG_File_Cmp_Extension(File, SG_T("shp" )) )
		{
			Type	= DATAOBJECT_TYPE_Shapes;
		}

		if( SG_File_Cmp_Extension(File, SG_T("spc" )) )
		{
			Type	= DATAOBJECT_TYPE_PointCloud;
		}

		if(	SG_File_Cmp_Extension(File, SG_T("sgrd"))
		||	SG_File_Cmp_Extension(File, SG_T("dgm" ))
		||	SG_File_Cmp_Extension(File, SG_T("grd" )) )
		{
			Type	= DATAOBJECT_TYPE_Grid;
		}
	}

	//-----------------------------------------------------
	CSG_Data_Object	*pObject;

	switch( Type )
	{
	default:							pObject	= NULL;							break;
	case DATAOBJECT_TYPE_Table:			pObject	= new CSG_Table			(File);	break;
	case DATAOBJECT_TYPE_Shapes:		pObject	= new CSG_Shapes		(File);	break;
	case DATAOBJECT_TYPE_TIN:			pObject	= new CSG_TIN			(File);	break;
	case DATAOBJECT_TYPE_PointCloud:	pObject	= new CSG_PointCloud	(File);	break;
	case DATAOBJECT_TYPE_Grid:			pObject	= new CSG_Grid			(File);	break;
	}

	if( pObject )
	{
		if( pObject->is_Valid() )
		{
			return( Add(pObject) );
		}

		delete(pObject);
	}

	//-----------------------------------------------------
	return( _Add_External(File) );
}

//---------------------------------------------------------
bool CSG_Data_Manager::_Add_External(const CSG_String &File)
{
	//-----------------------------------------------------
	bool		bResult	= false;

	CSG_Module	*pImport;

	if( !SG_File_Exists(File) )
	{
		return( false );
	}

	//-----------------------------------------------------
	// Image Import

	if(	(	SG_File_Cmp_Extension(File, SG_T("bmp"))
		||	SG_File_Cmp_Extension(File, SG_T("gif"))
		||	SG_File_Cmp_Extension(File, SG_T("jpg"))
		||	SG_File_Cmp_Extension(File, SG_T("png"))
		||	SG_File_Cmp_Extension(File, SG_T("pcx")) )
	&&  (pImport = SG_Get_Module_Library_Manager().Get_Module("io_grid_image", 1)) != NULL
	&&   pImport->Set_Parameter("FILE", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);
		bResult	= pImport->Execute();
		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------
	// GDAL Import

	if( !bResult
	&&  (pImport = SG_Get_Module_Library_Manager().Get_Module("io_gdal", 0)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);
		bResult	= pImport->Execute();
		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------
	// OGR Import

	if( !bResult
	&&  (pImport = SG_Get_Module_Library_Manager().Get_Module("io_gdal", 3)) != NULL
	&&   pImport->Set_Parameter("FILES", File, PARAMETER_TYPE_FilePath) )
	{
		pImport->Set_Manager(this);
		bResult	= pImport->Execute();
		pImport->Set_Manager(&g_Data_Manager);
	}

	//-----------------------------------------------------


	return( bResult );
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
	if( pCollection->m_Type == DATAOBJECT_TYPE_Grid )
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
		if( pCollection->m_Type == DATAOBJECT_TYPE_Grid && pCollection->Count() == 0 )
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
#define SUMMARY_ADD_INT(label, value)	s += CSG_String::Format(SG_T("<tr><td valign=\"top\">%s</td><td valign=\"top\">%d</td></tr>"), label, value)

//---------------------------------------------------------
CSG_String CSG_Data_Manager::Get_Summary(void)	const
{
	//-----------------------------------------------------
	CSG_String	s;

/*	s	+= CSG_String::Format(SG_T("<b>%s</b>"), _TL("Tool Libraries"));

	s	+= SG_T("<table border=\"0\">");

	SUMMARY_ADD_INT(_TL("Available Libraries"), Get_Count());
	SUMMARY_ADD_INT(_TL("Available Tools"    ), nModules);

	s	+= SG_T("</table>");

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("<hr><b>%s:</b><table border=\"1\">"), _TL("Tool Libraries"));

	s	+= CSG_String::Format(SG_T("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>"),
			_TL("Library"),
			_TL("Tools"),
			_TL("Name"),
			_TL("Location")
		);

	for(i=0; i<Get_Count(); i++)
	{
		s	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%d</td><td>%s</td><td>%s</td></tr>"),
				SG_File_Get_Name(Get_Library(i)->Get_File_Name(), false).c_str(),
				Get_Library(i)->Get_Count(),
				Get_Library(i)->Get_Name().c_str(),
				SG_File_Get_Path(Get_Library(i)->Get_File_Name()).c_str()
			);
	}

	s	+= SG_T("</table>");

*/
	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
