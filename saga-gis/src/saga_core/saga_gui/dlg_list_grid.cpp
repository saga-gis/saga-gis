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
//                  DLG_List_Grid.cpp                    //
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

#include "res_controls.h"

#include "wksp_data_manager.h"
#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"

#include "dlg_list_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Grid, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_Grid, CDLG_List_Base)
	EVT_CHOICE		(ID_COMBOBOX_SELECT, CDLG_List_Grid::On_Select_System)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_Grid::CDLG_List_Grid(CSG_Parameter_Grid_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	CWKSP_Grid_Manager	*pManager	= g_pData->Get_Grids();

	if( pManager == NULL )
	{
		m_pSystem  = NULL;
		m_pSystems = NULL;
	}
	else if( pList->Get_System() )
	{
		m_pSystem	= pManager->Get_System(*pList->Get_System());
		m_pSystems	= NULL;
	}
	else
	{
		m_pSystem	= NULL;
		m_pSystems	= new wxChoice(this, ID_COMBOBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );

		for(int i=0; i<pManager->Get_Count(); i++)
		{
			m_pSystems->Append(pManager->Get_System(i)->Get_Name());
		}

		m_pSystems->Append( _TL("<all grid systems>") );
		m_pSystems->SetSelection(m_pSystems->GetCount() - 1);
	}

	_Set_Data();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Grid::Set_Position(wxRect r)
{
	CDLG_List_Base::Set_Position(r);

	if( m_pSystems )
	{
		r.Deflate(5);

		int	Height	= m_pSystems->GetSize().GetHeight();
		int	Width	= r.GetWidth() / 2 - (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST);

		m_pSystems	->SetSize(r.GetLeft(), r.GetTop()         , Width,                 Height);
		m_pSelect	->SetSize(r.GetLeft(), r.GetTop() + Height, Width, r.GetHeight() - Height);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Grid::On_Select_System(wxCommandEvent &event)
{
	_Set_Data();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Grid::_Set_Data(void)
{
	m_pSelect->Clear();

	if( m_pSystem )
	{
		Set_Data(m_pSystem);
	}
	else if( m_pSystems )
	{
		CWKSP_Grid_Manager	*pManager	= g_pData->Get_Grids();

		if( pManager )
		{
			int	iSystem	= m_pSystems->GetSelection();

			if( 0 <= iSystem && iSystem < pManager->Get_Count() )
			{
				Set_Data(pManager->Get_System(iSystem));
			}
			else
			{
				for(iSystem=0; iSystem<pManager->Get_Count(); iSystem++)
				{
					Set_Data(pManager->Get_System(iSystem));
				}
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
