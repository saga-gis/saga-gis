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
//                  DLG_List_Base.cpp                    //
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
#include "wksp_grid.h"
#include "wksp_table.h"
#include "wksp_shapes.h"
#include "wksp_tin.h"
#include "wksp_pointcloud.h"

#include "dlg_list_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_Base::CDLG_List_Base(CSG_Parameter_List *pList, wxString Caption)
	: CDLG_Base(-1, Caption)
{
	m_pList			= pList;

	m_pSelect		= new wxListBox	(this, ID_LISTBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB);
	m_pAdd			= new wxListBox	(this, ID_LISTBOX_ADD   , wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB);

	m_pBtn_Add_All	= new wxButton	(this, ID_BTN_ADD_ALL   , wxT(">>"));
	m_pBtn_Add		= new wxButton	(this, ID_BTN_ADD       , wxT(">"));
	m_pBtn_Del		= new wxButton	(this, ID_BTN_DELETE    , wxT("<"));
	m_pBtn_Del_All	= new wxButton	(this, ID_BTN_DELETE_ALL, wxT("<<"));
	m_pBtn_Up		= new wxButton	(this, ID_BTN_UP        , CTRL_Get_Name(ID_BTN_UP));
	m_pBtn_Down		= new wxButton	(this, ID_BTN_DOWN      , CTRL_Get_Name(ID_BTN_DOWN));

	m_btn_height	= m_pBtn_Add->GetDefaultSize().y;

	//-----------------------------------------------------
	for(int i=m_pList->Get_Count()-1; i>=0; i--)
	{
		CWKSP_Data_Item	*pItem	= g_pData->Get(m_pList->asDataObject(i));

		if( pItem && SG_Get_Data_Manager().Exists(m_pList->asDataObject(i)) )
		{
			m_pAdd->Insert(pItem->Get_Name(), 0, (void *)m_pList->asDataObject(i));
		}
		else
		{
			m_pList->Del_Item(i);
		}
	}

	//-----------------------------------------------------
	Set_Positions();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Base::Set_Position(wxRect r)
{
	int		Center;

	r.Deflate(5);
	Center	= r.GetLeft() + r.GetWidth() / 2;

	r.SetWidth(r.GetWidth() / 2 - (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST));
	m_pSelect->SetSize(r);

	r.SetLeft(Center            + (DLG_LIST_BTN_WIDTH / 2 + DLG_LIST_BTN_DIST));
	m_pAdd->SetSize(r);

	r		= wxRect(Center - DLG_LIST_BTN_WIDTH / 2, r.GetTop(), DLG_LIST_BTN_WIDTH, m_btn_height);
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
//														 //
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

			for(size_t j=0; j<m_pAdd->GetCount() && pItem; j++)
			{
				if( pItem->Get_Object() == m_pAdd->GetClientData(j) )
				{
					pItem	= NULL;
				}
			}

			if( pItem )
			{
				m_pSelect->Append(pItem->Get_Name(), (void *)pItem->Get_Object());
			}
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Base::Save_Changes(void)
{
	m_pList->Del_Items();

	for(unsigned int i=0; i<m_pAdd->GetCount(); i++)
	{
		m_pList->Add_Item((CSG_Data_Object *)m_pAdd->GetClientData(i));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int ArrayInt_CMP_Up(int *first, int *second)
{
	return( *first - *second );
}

//---------------------------------------------------------
int ArrayInt_CMP_Down(int *first, int *second)
{
	return( *second - *first );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

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

	if( m_pAdd->GetSelections(Selections) > 0 )
	{
		Selections.Sort(ArrayInt_CMP_Down);

		for(size_t i=0; i<Selections.GetCount(); i++)
		{
			size_t	j	= Selections.Item(i);

			m_pSelect->Append(m_pAdd->GetString(j), m_pAdd->GetClientData(j));
			m_pAdd->Delete(j);
		}
	}
}

void CDLG_List_Base::On_Del_All(wxCommandEvent &event)
{
	for(unsigned int i=0; i<m_pAdd->GetCount(); i++)
	{
		m_pSelect->Append(m_pAdd->GetString(i), m_pAdd->GetClientData(i));
	}

	m_pAdd->Clear();
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

	if( m_pSelect->GetSelections(Selections) > 0 )
	{
		Selections.Sort(ArrayInt_CMP_Down);

		for(size_t i=0, Position=m_pAdd->GetCount(); i<Selections.GetCount(); i++)
		{
			size_t	j	= Selections.Item(i);

			m_pAdd->Insert(m_pSelect->GetString(j), Position, m_pSelect->GetClientData(j));
			m_pSelect->Delete(j);
		}
	}
}

void CDLG_List_Base::On_Add_All(wxCommandEvent &event)
{
	for(unsigned int i=0; i<m_pSelect->GetCount(); i++)
	{
		m_pAdd->Append(m_pSelect->GetString(i), m_pSelect->GetClientData(i));
	}

	m_pSelect->Clear();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Base::On_Up(wxCommandEvent &event)
{
	wxArrayInt	Selections;

	if( m_pAdd->GetSelections(Selections) > 0 && Selections.GetCount() < m_pAdd->GetCount() )
	{
		Selections.Sort(ArrayInt_CMP_Up);

		if( Selections.Item(0) > 0 )
		{
			for(size_t i=0; i<Selections.GetCount(); i++)
			{
				size_t	j	= Selections.Item(i);

				wxString	String	= m_pAdd->GetString    (j);
				void		*pData	= m_pAdd->GetClientData(j);

				m_pAdd->SetString    (j    , m_pAdd->GetString    (j - 1));
				m_pAdd->SetClientData(j    , m_pAdd->GetClientData(j - 1));
				m_pAdd->SetSelection (j    , m_pAdd->IsSelected   (j - 1));

				m_pAdd->SetString    (j - 1, String);
				m_pAdd->SetClientData(j - 1, pData);
				m_pAdd->SetSelection (j - 1, true);
			}
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Base::On_Down(wxCommandEvent &event)
{
	wxArrayInt	Selections;

	if( m_pAdd->GetSelections(Selections) > 0 && Selections.GetCount() < m_pAdd->GetCount() )
	{
		Selections.Sort(ArrayInt_CMP_Down);

		if( (int)Selections.Item(0) < m_pAdd->GetCount() - 1 )
		{
			for(size_t i=0; i<Selections.GetCount(); i++)
			{
				size_t	j	= Selections.Item(i);

				wxString	String	= m_pAdd->GetString(j);
				void		*pData	= m_pAdd->GetClientData(j);

				m_pAdd->SetString    (j    , m_pAdd->GetString    (j + 1));
				m_pAdd->SetClientData(j    , m_pAdd->GetClientData(j + 1));
				m_pAdd->SetSelection (j    , m_pAdd->IsSelected   (j + 1));

				m_pAdd->SetString    (j + 1, String);
				m_pAdd->SetClientData(j + 1, pData);
				m_pAdd->SetSelection (j + 1, true);
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
