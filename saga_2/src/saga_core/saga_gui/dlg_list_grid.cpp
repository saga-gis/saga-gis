
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
// $Id: dlg_list_grid.cpp,v 1.6 2007-03-22 14:23:33 oconrad Exp $

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
	CWKSP_Grid_Manager	*pManager;

	if( (pManager = g_pData->Get_Grids()) != NULL )
	{
		if( pList->Get_System() )
		{
			m_pSystem	= pManager->Get_System(pList->Get_System());
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

			m_pSystems->Append( wxT("[VAL] [all grid systems]") );
			m_pSystems->SetSelection(m_pSystems->GetCount() - 1);

			Set_Positions();
		}

		_Set_Objects();
	}
}

//---------------------------------------------------------
CDLG_List_Grid::~CDLG_List_Grid(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Grid::Set_Position(wxRect r)
{
	int		Width, Height;

	CDLG_List_Base::Set_Position(r);

	if( m_pSystems )
	{
		r.Deflate(5);

		Height	= m_pSystems->GetSize().GetHeight();
		Width	= r.GetWidth() / 2 - (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST);

		m_pSystems	->SetSize(wxRect(r.GetLeft(), r.GetTop(), Width, Height));
		m_pSelect	->SetSize(wxRect(r.GetLeft(), r.GetTop() + Height, Width, r.GetHeight() - Height));
		m_pSelect	->Refresh();
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
	_Set_Grids();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Grid::_Set_Objects(void)
{
	//-----------------------------------------------------
	for(int i=0; i<m_pList->Get_Count(); i++)
	{
		m_pAdd->Append(m_pList->asDataObject(i)->Get_Name(), m_pList->asDataObject(i));
	}

	//-----------------------------------------------------
	if( ((CSG_Parameter_Grid_List *)m_pList)->Get_System() )
	{
		_Set_Grids(m_pSystem);
	}
	else
	{
		_Set_Grids();
	}
}

//---------------------------------------------------------
void CDLG_List_Grid::_Set_Grids(void)
{
	CWKSP_Grid_Manager	*pManager;

	m_pSelect->Clear();

	if( (pManager = g_pData->Get_Grids()) != NULL )
	{
		int		i;

		if( !m_pSystems || (i = m_pSystems->GetSelection()) >= pManager->Get_Count() || i < 0 )
		{
			for(i=0; i<pManager->Get_Count(); i++)
			{
				_Set_Grids(pManager->Get_System(i));
			}
		}
		else
		{
			_Set_Grids(pManager->Get_System(i));
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Grid::_Set_Grids(CWKSP_Grid_System *pSystem)
{
	bool		bList;
	int			i, j;
	CSG_Grid	*pGrid;

	if( pSystem )
	{
		for(i=0; i<pSystem->Get_Count(); i++)
		{
			pGrid	= pSystem->Get_Grid(i)->Get_Grid();

			for(j=0, bList=true; j<(int)m_pAdd->GetCount() && bList; j++)
			{
				if( pGrid == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pGrid->Get_Name(), pGrid);
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
