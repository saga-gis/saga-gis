
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
CDLG_List_Table::CDLG_List_Table(CParameter_Table_List *pList, wxString Caption)
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
	CTable				*pTable;

	//-----------------------------------------------------
	if( (pTables = g_pData->Get_Tables()) != NULL )
	{
		for(i=0; i<m_pList->Get_Count(); i++)
		{
			m_pAdd->Append(m_pList->asDataObject(i)->Get_Name(), m_pList->asDataObject(i));
		}

		//-------------------------------------------------
		for(i=0; i<pTables->Get_Count(); i++)
		{
			pTable	= pTables->Get_Table(i)->Get_Table();

			for(j=0, bList=true; j<m_pAdd->GetCount() && bList; j++)
			{
				if( pTable == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pTable->Get_Name(), pTable);
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
