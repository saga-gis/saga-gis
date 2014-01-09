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
//                   DLG_List_TIN.cpp                    //
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
#include <saga_api/saga_api.h>

#include "wksp_data_manager.h"
#include "wksp_tin_manager.h"
#include "wksp_tin.h"

#include "dlg_list_tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_TIN, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_TIN, CDLG_List_Base)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_TIN::CDLG_List_TIN(CSG_Parameter_TIN_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	_Set_Objects();
}

//---------------------------------------------------------
CDLG_List_TIN::~CDLG_List_TIN(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_TIN::_Set_Objects(void)
{
	bool				bList;
	int					i, j;
	CWKSP_TIN_Manager	*pTINs;
	CWKSP_TIN			*pTIN;

	//-----------------------------------------------------
	if( (pTINs = g_pData->Get_TINs()) != NULL )
	{
		for(i=0; i<m_pList->Get_Count(); i++)
		{
			CWKSP_Base_Item	*pItem	= g_pData->Get(m_pList->asDataObject(i)->asTIN());

			if( pItem )
			{
				m_pAdd->Append(pItem->Get_Name(), (void *)pItem);
			}
		}

		//-------------------------------------------------
		for(i=0; i<pTINs->Get_Count(); i++)
		{
			pTIN	= pTINs->Get_Data(i);

			for(j=0, bList=true; j<(int)m_pAdd->GetCount() && bList; j++)
			{
				if( pTIN == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pTIN->Get_Name(), (void *)pTIN);
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
