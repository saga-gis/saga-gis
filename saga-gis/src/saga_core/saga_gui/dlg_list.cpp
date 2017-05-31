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
//                     dlg_list.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"

#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"
#include "wksp_grids.h"

#include "dlg_list.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DLG_LIST_BTN_WIDTH	40
#define DLG_LIST_BTN_DIST	4


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Base, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_Base, CDLG_Base)
	EVT_BUTTON			(ID_BTN_ADD			, CDLG_List_Base::On_Add)
	EVT_BUTTON			(ID_BTN_ADD_ALL		, CDLG_List_Base::On_Add_All)
	EVT_BUTTON			(ID_BTN_DELETE		, CDLG_List_Base::On_Del)
	EVT_BUTTON			(ID_BTN_DELETE_ALL	, CDLG_List_Base::On_Del_All)
	EVT_BUTTON			(ID_BTN_UP			, CDLG_List_Base::On_Up)
	EVT_BUTTON			(ID_BTN_DOWN		, CDLG_List_Base::On_Down)

	EVT_LISTBOX_DCLICK	(ID_LISTBOX_SELECT	, CDLG_List_Base::On_DClick_Add)
	EVT_LISTBOX_DCLICK	(ID_LISTBOX_ADD		, CDLG_List_Base::On_DClick_Del)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_Base::CDLG_List_Base(CSG_Parameter_List *pParameter, wxString Caption)
	: CDLG_Base(-1, Caption)
{
	m_pParameter	= pParameter;
	m_Type			= SG_DATAOBJECT_TYPE_Undefined;

	m_pChoices		= new wxListBox(this, ID_LISTBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB);
	m_pSelection	= new wxListBox(this, ID_LISTBOX_ADD   , wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB);

	m_pBtn_Add_All	= new wxButton (this, ID_BTN_ADD_ALL   , ">>");
	m_pBtn_Add		= new wxButton (this, ID_BTN_ADD       , ">" );
	m_pBtn_Del		= new wxButton (this, ID_BTN_DELETE    , "<" );
	m_pBtn_Del_All	= new wxButton (this, ID_BTN_DELETE_ALL, "<<");
	m_pBtn_Up		= new wxButton (this, ID_BTN_UP        , CTRL_Get_Name(ID_BTN_UP));
	m_pBtn_Down		= new wxButton (this, ID_BTN_DOWN      , CTRL_Get_Name(ID_BTN_DOWN));

	m_Btn_Height	= m_pBtn_Add->GetDefaultSize().y;

	//-----------------------------------------------------
	for(int i=m_pParameter->Get_Item_Count()-1; i>=0; i--)
	{
		CWKSP_Data_Item	*pItem	= g_pData->Get(m_pParameter->Get_Item(i));

		if( pItem && SG_Get_Data_Manager().Exists(m_pParameter->Get_Item(i)) )
		{
			m_pSelection->Insert(pItem->Get_Name(), 0, (void *)m_pParameter->Get_Item(i));
		}
		else
		{
			m_pParameter->Del_Item(i);
		}
	}

	//-----------------------------------------------------
	Set_Positions();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Base::Set_Position(wxRect r)
{
	r.Deflate(5);

	int	Center	= r.GetLeft() + r.GetWidth() / 2;

	r.SetWidth(r.GetWidth() / 2 - (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST));
	m_pChoices->SetSize(r);

	r.SetLeft(Center            + (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST));
	m_pSelection->SetSize(r);

	r		= wxRect(Center - DLG_LIST_BTN_WIDTH / 2, r.GetTop(), DLG_LIST_BTN_WIDTH, m_Btn_Height);
	m_pBtn_Add_All->SetSize(r);

	r.SetTop(r.GetBottom() + DLG_LIST_BTN_DIST);
	m_pBtn_Add->SetSize(r);

	r.SetTop(r.GetBottom() + DLG_LIST_BTN_DIST * 2);
	m_pBtn_Del->SetSize(r);

	r.SetTop(r.GetBottom() + DLG_LIST_BTN_DIST);
	m_pBtn_Del_All->SetSize(r);

	r.SetTop(r.GetBottom() + DLG_LIST_BTN_DIST * 2);
	m_pBtn_Up->SetSize(r);

	r.SetTop(r.GetBottom() + DLG_LIST_BTN_DIST);
	m_pBtn_Down->SetSize(r);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Base::Set_Data(CWKSP_Base_Manager *pManager)
{
	if( pManager )
	{
		for(int iItem=0; iItem<pManager->Get_Count(); iItem++)
		{
			CWKSP_Data_Item	*pItem	= (CWKSP_Data_Item *)pManager->Get_Item(iItem);

			if( m_Type == SG_DATAOBJECT_TYPE_Undefined || m_Type == pItem->Get_Object()->Get_ObjectType() )
			{
				for(size_t j=0; j<m_pSelection->GetCount() && pItem; j++)
				{
					if( pItem->Get_Object() == m_pSelection->GetClientData(j) )
					{
						pItem	= NULL;
					}
				}

				if( pItem )
				{
					m_pChoices->Append(pItem->Get_Name(), (void *)pItem->Get_Object());
				}
			}
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Base::Save_Changes(void)
{
	m_pParameter->Del_Items();

	for(unsigned int i=0; i<m_pSelection->GetCount(); i++)
	{
		m_pParameter->Add_Item((CSG_Data_Object *)m_pSelection->GetClientData(i));
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDLG_List_Base::_Compare_Up(int *first, int *second)
{
	return( *first - *second );
}

//---------------------------------------------------------
int CDLG_List_Base::_Compare_Down(int *first, int *second)
{
	return( *second - *first );
}

//---------------------------------------------------------
void CDLG_List_Base::On_DClick_Del(wxCommandEvent &event)
{
	_Del();
}

void CDLG_List_Base::On_Del(wxCommandEvent &event)
{
	_Del();
}

void CDLG_List_Base::_Del(void)
{
	wxArrayInt	Selections;

	if( m_pSelection->GetSelections(Selections) > 0 )
	{
		Selections.Sort(_Compare_Down);

		for(size_t i=0; i<Selections.GetCount(); i++)
		{
			size_t	j	= Selections.Item(i);

			m_pChoices->Append(m_pSelection->GetString(j), m_pSelection->GetClientData(j));
			m_pSelection->Delete(j);
		}
	}
}

void CDLG_List_Base::On_Del_All(wxCommandEvent &event)
{
	for(unsigned int i=0; i<m_pSelection->GetCount(); i++)
	{
		m_pChoices->Append(m_pSelection->GetString(i), m_pSelection->GetClientData(i));
	}

	m_pSelection->Clear();
}

//---------------------------------------------------------
void CDLG_List_Base::On_DClick_Add(wxCommandEvent &event)
{
	_Add();
}

void CDLG_List_Base::On_Add(wxCommandEvent &event)
{
	_Add();
}

void CDLG_List_Base::_Add(void)
{
	wxArrayInt	Selections;

	if( m_pChoices->GetSelections(Selections) > 0 )
	{
		Selections.Sort(_Compare_Down);

		for(size_t i=0, Position=m_pSelection->GetCount(); i<Selections.GetCount(); i++)
		{
			size_t	j	= Selections.Item(i);

			m_pSelection->Insert(m_pChoices->GetString(j), Position, m_pChoices->GetClientData(j));
			m_pChoices->Delete(j);
		}
	}
}

void CDLG_List_Base::On_Add_All(wxCommandEvent &event)
{
	for(unsigned int i=0; i<m_pChoices->GetCount(); i++)
	{
		m_pSelection->Append(m_pChoices->GetString(i), m_pChoices->GetClientData(i));
	}

	m_pChoices->Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Base::On_Up(wxCommandEvent &event)
{
	wxArrayInt	Selections;

	if( m_pSelection->GetSelections(Selections) > 0 && Selections.GetCount() < m_pSelection->GetCount() )
	{
		Selections.Sort(_Compare_Up);

		if( Selections.Item(0) > 0 )
		{
			for(size_t i=0; i<Selections.GetCount(); i++)
			{
				size_t	j	= Selections.Item(i);

				wxString	String	= m_pSelection->GetString    (j);
				void		*pData	= m_pSelection->GetClientData(j);

				m_pSelection->SetString    (j    , m_pSelection->GetString    (j - 1));
				m_pSelection->SetClientData(j    , m_pSelection->GetClientData(j - 1));
				m_pSelection->SetSelection (j    , m_pSelection->IsSelected   (j - 1));

				m_pSelection->SetString    (j - 1, String);
				m_pSelection->SetClientData(j - 1, pData);
				m_pSelection->SetSelection (j - 1, true);
			}
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Base::On_Down(wxCommandEvent &event)
{
	wxArrayInt	Selections;

	if( m_pSelection->GetSelections(Selections) > 0 && Selections.GetCount() < m_pSelection->GetCount() )
	{
		Selections.Sort(_Compare_Down);

		if( (int)Selections.Item(0) < m_pSelection->GetCount() - 1 )
		{
			for(size_t i=0; i<Selections.GetCount(); i++)
			{
				size_t	j	= Selections.Item(i);

				wxString	String	= m_pSelection->GetString(j);
				void		*pData	= m_pSelection->GetClientData(j);

				m_pSelection->SetString    (j    , m_pSelection->GetString    (j + 1));
				m_pSelection->SetClientData(j    , m_pSelection->GetClientData(j + 1));
				m_pSelection->SetSelection (j    , m_pSelection->IsSelected   (j + 1));

				m_pSelection->SetString    (j + 1, String);
				m_pSelection->SetClientData(j + 1, pData);
				m_pSelection->SetSelection (j + 1, true);
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
IMPLEMENT_CLASS(CDLG_List_Table, CDLG_List_Base)

//---------------------------------------------------------
CDLG_List_Table::CDLG_List_Table(CSG_Parameter_Table_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	Set_Data(g_pData->Get_Tables());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Shapes, CDLG_List_Base)

//---------------------------------------------------------
CDLG_List_Shapes::CDLG_List_Shapes(CSG_Parameter_Shapes_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	m_Shape_Type	= pList->Get_Shape_Type();

	_Set_Data();
}

//---------------------------------------------------------
void CDLG_List_Shapes::_Set_Data(void)
{
	CWKSP_Shapes_Manager	*pManager	= g_pData->Get_Shapes();

	if( m_Shape_Type == SHAPE_TYPE_Undefined )
	{
		if( pManager )
		{
			for(int i=0; i<pManager->Get_Count(); i++)
			{
				Set_Data((CWKSP_Shapes_Type *)pManager->Get_Item(i));
			}
		}

		Set_Data(g_pData->Get_PointClouds());
	}
	else
	{
		if( pManager )
		{
			Set_Data(pManager->Get_Shapes_Type(m_Shape_Type));
		}

		if( m_Shape_Type == SHAPE_TYPE_Point )
		{
			Set_Data(g_pData->Get_PointClouds());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_PointCloud, CDLG_List_Base)

//---------------------------------------------------------
CDLG_List_PointCloud::CDLG_List_PointCloud(CSG_Parameter_PointCloud_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	Set_Data(g_pData->Get_PointClouds());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_TIN, CDLG_List_Base)

//---------------------------------------------------------
CDLG_List_TIN::CDLG_List_TIN(CSG_Parameter_TIN_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	Set_Data(g_pData->Get_TINs());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Grid_Base, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_Grid_Base, CDLG_List_Base)
	EVT_CHOICE		(ID_COMBOBOX_SELECT, CDLG_List_Grid_Base::On_Select_System)
END_EVENT_TABLE()

//---------------------------------------------------------
CDLG_List_Grid_Base::CDLG_List_Grid_Base(CSG_Parameter_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{}

//---------------------------------------------------------
void CDLG_List_Grid_Base::Set_Position(wxRect r)
{
	CDLG_List_Base::Set_Position(r);

	if( m_pSystems )
	{
		r.Deflate(5);

		int	Height	= m_pSystems->GetSize().GetHeight();
		int	Width	= r.GetWidth() / 2 - (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST);

		m_pSystems	->SetSize(r.GetLeft(), r.GetTop()         , Width,                 Height);
		m_pChoices	->SetSize(r.GetLeft(), r.GetTop() + Height, Width, r.GetHeight() - Height);
	}
}

//---------------------------------------------------------
void CDLG_List_Grid_Base::On_Select_System(wxCommandEvent &event)
{
	_Set_Data();
}

//---------------------------------------------------------
void CDLG_List_Grid_Base::_Set_Data(void)
{
	m_pChoices->Clear();

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
IMPLEMENT_CLASS(CDLG_List_Grid, CDLG_List_Grid_Base)

//---------------------------------------------------------
CDLG_List_Grid::CDLG_List_Grid(CSG_Parameter_Grid_List *pList, wxString Caption)
	: CDLG_List_Grid_Base(pList, Caption)
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
IMPLEMENT_CLASS(CDLG_List_Grids, CDLG_List_Grid_Base)

//---------------------------------------------------------
CDLG_List_Grids::CDLG_List_Grids(CSG_Parameter_Grids_List *pList, wxString Caption)
	: CDLG_List_Grid_Base(pList, Caption)
{
	m_Type	= SG_DATAOBJECT_TYPE_Grids;

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
		//	if( pManager->Get_Grids_Count() > 0 )
			{
				m_pSystems->Append(pManager->Get_System(i)->Get_Name());
			}
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
