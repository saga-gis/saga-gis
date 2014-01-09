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
//                  DLG_List_Table.cpp                   //
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
#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "dlg_list_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Table, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_Table, CDLG_List_Base)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_Table::CDLG_List_Table(CSG_Parameter_Table_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	_Set_Objects();
}

//---------------------------------------------------------
CDLG_List_Table::~CDLG_List_Table(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Table::_Set_Objects(void)
{
	bool				bList;
	int					i, j;
	CWKSP_Table_Manager	*pTables;
	CWKSP_Table			*pTable;

	//-----------------------------------------------------
	if( (pTables = g_pData->Get_Tables()) != NULL )
	{
		for(i=0; i<m_pList->Get_Count(); i++)
		{
			CWKSP_Base_Item	*pItem	= g_pData->Get(m_pList->asDataObject(i)->asTable());

			if( pItem )
			{
				m_pAdd->Append(pItem->Get_Name(), (void *)pItem);
			}
		}

		//-------------------------------------------------
		for(i=0; i<pTables->Get_Count(); i++)
		{
			pTable	= pTables->Get_Data(i);

			for(j=0, bList=true; j<(int)m_pAdd->GetCount() && bList; j++)
			{
				if( pTable == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pTable->Get_Name(), (void *)pTable);
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
