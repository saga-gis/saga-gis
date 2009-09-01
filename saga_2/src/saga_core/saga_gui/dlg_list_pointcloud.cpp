
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
//                DLG_List_PointCloud.cpp                //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Bundesstrasse 55                       //
//                20146 Hamburg                          //
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
#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"

#include "dlg_list_pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_PointCloud, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_PointCloud, CDLG_List_Base)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_PointCloud::CDLG_List_PointCloud(CSG_Parameter_PointCloud_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	_Set_Objects();
}

//---------------------------------------------------------
CDLG_List_PointCloud::~CDLG_List_PointCloud(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_PointCloud::_Set_Objects(void)
{
	bool						bList;
	int							i, j;
	CWKSP_PointCloud_Manager	*pPointClouds;
	CSG_PointCloud				*pPointCloud;

	//-----------------------------------------------------
	if( (pPointClouds = g_pData->Get_PointClouds()) != NULL )
	{
		for(i=0; i<m_pList->Get_Count(); i++)
		{
			m_pAdd->Append(m_pList->asDataObject(i)->Get_Name(), m_pList->asDataObject(i));
		}

		//-------------------------------------------------
		for(i=0; i<pPointClouds->Get_Count(); i++)
		{
			pPointCloud	= pPointClouds->Get_PointCloud(i)->Get_PointCloud();

			for(j=0, bList=true; j<(int)m_pAdd->GetCount() && bList; j++)
			{
				if( pPointCloud == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pPointCloud->Get_Name(), pPointCloud);
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
