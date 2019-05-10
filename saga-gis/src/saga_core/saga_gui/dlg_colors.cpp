
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
//                    DLG_Colors.cpp                     //
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
#include <saga_api/saga_api.h>

#include <wx/odcombo.h>

#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "dlg_colors.h"
#include "dlg_colors_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CColorPresets : public wxOwnerDrawnComboBox
{
public:
	CColorPresets(wxWindow *pParent)
	{
		wxArrayString	Items;	CSG_Colors	Colors;

		for(int i=0; Colors.Set_Predefined(i); i++)
		{
			Items.Add(CSG_Colors::Get_Predefined_Name(i));
		}

		Create(pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, Items, wxCB_READONLY); //wxNO_BORDER|wxCB_READONLY);

		SetSelection(0);
	}

	//-----------------------------------------------------
	void	Draw_Colors		(wxDC &dc, const wxRect &r, int Palette) const
	{
		CSG_Colors	Colors(r.GetWidth(), Palette);

		for(int i=0, x=r.GetLeft(); i<Colors.Get_Count(); i++)
		{
			int	xx = x; x = r.GetLeft() + (int)((i + 1.) * r.GetWidth() / (double)Colors.Get_Count());

			Draw_FillRect(dc, Get_Color_asWX(Colors.Get_Color(i)), xx, r.GetTop(), x, r.GetBottom());
		}
	}

	//-----------------------------------------------------
	virtual void OnDrawItem(wxDC &dc, const wxRect &rect, int item, int flags) const
	{
		if( item != wxNOT_FOUND )
		{
			wxRect	r = rect; r.Deflate(1);

			Draw_Colors(dc, r, item);
		}
	}

	//-----------------------------------------------------
	virtual void OnDrawBackground( wxDC& dc, const wxRect &rect, int item, int flags ) const
	{
		if( (flags & (wxODCB_PAINTING_CONTROL|wxODCB_PAINTING_SELECTED)) )
		{
			wxOwnerDrawnComboBox::OnDrawBackground(dc, rect, item, flags);

			return;
		}

		wxColour	Color	= SYS_Get_Color(wxSYS_COLOUR_WINDOW);

		dc.SetBrush(wxBrush(Color));
		dc.SetPen  (wxPen  (Color));

		dc.DrawRectangle(rect);
	}

	virtual wxCoord OnMeasureItem(size_t WXUNUSED(item)) const
	{
		return( (wxCoord)( 0.8 * GetSize().GetHeight()) );
	}

	virtual wxCoord OnMeasureItemWidth(size_t WXUNUSED(item)) const
	{
		return( (wxCoord)(10.0 * GetSize().GetWidth ()) ); // return( -1 ); // will be measured from text width
	}
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Colors, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Colors, CDLG_Base)
	EVT_BUTTON(ID_BTN_LOAD            , CDLG_Colors::On_Load     )
	EVT_BUTTON(ID_BTN_SAVE            , CDLG_Colors::On_Save     )
	EVT_BUTTON(ID_BTN_COLORS_COUNT    , CDLG_Colors::On_Count    )
	EVT_BUTTON(ID_BTN_COLORS_MIRROR   , CDLG_Colors::On_Mirror   )
	EVT_BUTTON(ID_BTN_COLORS_INVERT   , CDLG_Colors::On_Invert   )
	EVT_BUTTON(ID_BTN_COLORS_GREYSCALE, CDLG_Colors::On_Greyscale)
	EVT_BUTTON(ID_BTN_COLORS_RANDOM   , CDLG_Colors::On_Random   )
	EVT_BUTTON(ID_BTN_COLORS_PRESET   , CDLG_Colors::On_Preset   )

	EVT_COMBOBOX(wxID_ANY             , CDLG_Colors::On_ComboBox )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Colors::CDLG_Colors(CSG_Colors *pColors)
	: CDLG_Base(-1, _TL("Colors"))
{
	m_pOriginal	= pColors;
	m_pColors	= new CSG_Colors();
	m_pColors->Assign(pColors);

	m_pControl	= new CDLG_Colors_Control(this, m_pColors);

	Add_Button(ID_BTN_LOAD);
	Add_Button(ID_BTN_SAVE);
	Add_Button(-1);
	Add_Control(new CColorPresets(Get_Controls()));
//	Add_Button(-1);
	Add_Button(ID_BTN_COLORS_COUNT    );
	Add_Button(ID_BTN_COLORS_MIRROR   );
	Add_Button(ID_BTN_COLORS_INVERT   );
	Add_Button(ID_BTN_COLORS_GREYSCALE);
	Add_Button(ID_BTN_COLORS_RANDOM   );
//	Add_Button(ID_BTN_COLORS_PRESET   );

	Set_Positions();
}

//---------------------------------------------------------
CDLG_Colors::~CDLG_Colors(void)
{
	delete(m_pColors);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors::Set_Position(wxRect r)
{
	m_pControl->SetSize(r);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors::Save_Changes(void)
{
	m_pOriginal->Assign(m_pColors);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors::On_Load(wxCommandEvent &event)
{
	wxString	File_Path;

	if( DLG_Open(File_Path, ID_DLG_COLORS_OPEN) )
	{
		if( m_pColors->Load(&File_Path) == false )
		{
			DLG_Message_Show(_TL("Colors file could not be imported."), _TL("Load Colors"));
		}
		else
		{
			m_pControl->Refresh(false);
		}
	}
}

//---------------------------------------------------------
void CDLG_Colors::On_Save(wxCommandEvent &event)
{
	wxString	File_Path;

	if( DLG_Save(File_Path, ID_DLG_COLORS_SAVE) )
	{
		if( m_pColors->Save(&File_Path, false) == false )
		{
			DLG_Message_Show(_TL("Colors file could not be exported."), _TL("Save Colors"));
		}
	}
}

//---------------------------------------------------------
void CDLG_Colors::On_Count(wxCommandEvent &event)
{
	int	Count	= m_pColors->Get_Count();

	if( DLG_Get_Number(Count ) )
	{
		m_pColors->Set_Count(Count);

		m_pControl->Refresh(false);
	}
}

//---------------------------------------------------------
void CDLG_Colors::On_Mirror(wxCommandEvent &event)
{
	m_pColors->Revert();

	m_pControl->Refresh(false);
}

//---------------------------------------------------------
void CDLG_Colors::On_Invert(wxCommandEvent &event)
{
	m_pColors->Invert();

	m_pControl->Refresh(false);
}

//---------------------------------------------------------
void CDLG_Colors::On_Greyscale(wxCommandEvent &event)
{
	m_pColors->Greyscale();

	m_pControl->Refresh(false);
}

//---------------------------------------------------------
void CDLG_Colors::On_Random(wxCommandEvent &event)
{
	m_pColors->Random();

	m_pControl->Refresh(false);
}

//---------------------------------------------------------
void CDLG_Colors::On_Preset(wxCommandEvent &event)
{
	int	Palette;

	if( DLG_Colors(Palette) )
	{
		m_pColors->Set_Palette(Palette, false, m_pColors->Get_Count());

		m_pControl->Refresh(false);
	}
}

//---------------------------------------------------------
void CDLG_Colors::On_ComboBox(wxCommandEvent &event)
{
	if( !event.GetEventObject()->IsKindOf(CLASSINFO(wxComboCtrl)) )	// Don't show messages for the log output window (it'll crash)
	{
		return;
	}

	if( event.GetEventType() == wxEVT_COMBOBOX )
	{
		m_pColors->Set_Palette(event.GetSelection(), false, m_pColors->Get_Count());

		m_pControl->Refresh(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
