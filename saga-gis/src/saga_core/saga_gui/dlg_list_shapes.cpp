/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 DLG_List_Shapes.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
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
#include <saga_api/saga_api.h>

#include "wksp_data_manager.h"
#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"
#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"

#include "dlg_list_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Shapes, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_Shapes, CDLG_List_Base)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_Shapes::CDLG_List_Shapes(CSG_Parameter_Shapes_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	m_Shape_Type	= pList->Get_Shape_Type();

	_Set_Objects();
}

//---------------------------------------------------------
CDLG_List_Shapes::~CDLG_List_Shapes(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Shapes::_Set_Objects(void)
{
	int					i;
	CWKSP_Base_Item		*pItem;
	CWKSP_Base_Manager	*pManager;

	//-----------------------------------------------------
	for(i=0; i<m_pList->Get_Count(); i++)
	{
		if(	(pItem = g_pData->Get(m_pList->asDataObject(i)->asShapes    ())) != NULL
		||	(pItem = g_pData->Get(m_pList->asDataObject(i)->asPointCloud())) != NULL )
		{
			m_pAdd->Append(pItem->Get_Name(), (void *)pItem);
		}
	}

	//-----------------------------------------------------
	if( m_Shape_Type == SHAPE_TYPE_Undefined )
	{
		if( (pManager = g_pData->Get_Shapes()) != NULL )
		{
			for(i=0; i<pManager->Get_Count(); i++)
			{
				_Set_Shapes((CWKSP_Shapes_Type *)pManager->Get_Item(i));
			}
		}

		if( (pManager = g_pData->Get_PointClouds()) != NULL )
		{
			_Set_Shapes(pManager);
		}
	}
	else
	{
		if( (pManager = g_pData->Get_Shapes()) != NULL )
		{
			_Set_Shapes(((CWKSP_Shapes_Manager *)pManager)->Get_Shapes_Type(m_Shape_Type));
		}

		if( m_Shape_Type == SHAPE_TYPE_Point && (pManager = g_pData->Get_PointClouds()) != NULL )
		{
			_Set_Shapes(pManager);
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Shapes::_Set_Shapes(CWKSP_Base_Manager *pType)
{
	if( pType )
	{
		for(int i=0; i<pType->Get_Count(); i++)
		{
			bool			bList		= true;
			CWKSP_Base_Item	*pShapes	= pType->Get_Item(i);

			for(int j=0; j<(int)m_pAdd->GetCount() && bList; j++)
			{
				if( pShapes == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pShapes->Get_Name(), (void *)pShapes);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
