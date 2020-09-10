
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
//                 VIEW_Layout_Info.cpp                  //
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
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wx/wx.h"
#include "wx/print.h"
#include "wx/printdlg.h"
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#include <saga_api/saga_api.h>

#include "helper.h"
#include "dc_helper.h"

#include "res_dialogs.h"

#include "wksp_map.h"

#include "view_layout_info.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Item : public CSGDI_Layout_Items::CSGDI_Layout_Item
{
public:
	CVIEW_Layout_Info	*m_pLayout;	CSG_Parameters	m_Parameters;

	CLayout_Item(CVIEW_Layout_Info *pLayout)
		: m_pLayout(pLayout)
	{}

	void				Set_Rect_Percentage	(double x, double y, double width, double height)
	{
		wxSize	Size(m_pLayout->Get_PaperSize());

		m_Rect.x      = (int)(0.5 + x      * Size.GetWidth () / 100.);
		m_Rect.width  = (int)(0.5 + width  * Size.GetWidth () / 100.);
		m_Rect.y      = (int)(0.5 + y      * Size.GetHeight() / 100.);
		m_Rect.height = (int)(0.5 + height * Size.GetHeight() / 100.);
	}

	virtual bool		Properties			(wxWindow *pParent)
	{
		return( m_Parameters.Get_Count() > 0 && DLG_Parameters(&m_Parameters) );
	}
};

//---------------------------------------------------------
class CLayout_Map : public CLayout_Item
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Map );	}

	//-----------------------------------------------------
	CLayout_Map(CVIEW_Layout_Info *pLayout, CWKSP_Map *pMap)
		: CLayout_Item(pLayout)
	{
		Set_Rect_Percentage(5, 5, 75, 85);

		m_Parameters.Add_Bool(""          , "FRAME_SHOW", _TL("Frame"), _TL(""), true);
		m_Parameters.Add_Int ("FRAME_SHOW", "FRAME_SIZE", _TL("Size" ), _TL(""), 10, 5, true);
	}

	//-----------------------------------------------------
	virtual bool		Draw			(wxDC &dc)
	{
		int	Frame	= m_Parameters["FRAME_SHOW"].asBool() ? m_Parameters["FRAME_SIZE"].asInt() : 0;

		wxRect	r(m_Rect); if( Frame > 5 ) { r.Deflate(Frame); } r = m_pLayout->Get_PaperToDC(r);

		double	PaperToDC	= m_pLayout->Get_PaperToDC();

		m_pLayout->Get_Map()->Draw_Map(dc, PaperToDC, r, LAYER_DRAW_FLAG_NOEDITS);

		if( Frame >= 5 )
		{
			m_pLayout->Get_Map()->Draw_Frame(dc, r, (int)(0.5 + PaperToDC * Frame));
		}

		return( true );
	}
};

//---------------------------------------------------------
class CLayout_Legend : public CLayout_Item
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Legend );	}

	//-----------------------------------------------------
	CLayout_Legend(CVIEW_Layout_Info *pLayout, CWKSP_Map *pMap)
		: CLayout_Item(pLayout)
	{
		Set_Rect_Percentage(85, 5, 10, 85);
	}

	//-----------------------------------------------------
	virtual bool		Draw			(wxDC &dc)
	{
		wxSize	Size;	double	d	= m_pLayout->Get_PaperToDC();

		if( m_pLayout->Get_Map()->Get_Legend_Size(Size, d) )
		{
			wxRect	r(m_pLayout->Get_PaperToDC(m_Rect));

			double	Scale	= r.GetHeight() / (double)Size.y;

			if( Scale * Size.x > r.GetWidth() )
			{
				Scale	= r.GetWidth() / (double)Size.x;
			}

			m_pLayout->Get_Map()->Draw_Legend(dc, d, Scale, r.GetLeftTop());

			return( true );
		}

		return( false );
	}
};

//---------------------------------------------------------
class CLayout_Scalebar : public CLayout_Item
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Scalebar );	}

	//-----------------------------------------------------
	CLayout_Scalebar(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout)
	{
		Set_Rect_Percentage(5, 95, 75, 2.5);
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
	//	m_pLayout->Get_Map()->Draw_ScaleBar(dc, m_pLayout->Get_Map()->Get_Extent(), m_pLayout->Get_PaperToDC(m_Rect));

		return( true );
	}
};

//---------------------------------------------------------
class CLayout_Scale : public CLayout_Item
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Scale );	}

	//-----------------------------------------------------
	CLayout_Scale(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout)
	{
		Set_Rect_Percentage(5, 95, 75, 2.5);
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		wxRect	rPaper(m_pLayout->Get_PaperToDC(m_Rect));

		double	Scale	= m_pLayout->Get_Map()->Get_World(rPaper).Get_XRange() / (rPaper.GetWidth() * 0.001 / m_pLayout->Get_PaperToDC());
	//	double	Scale	= m_pMap->Get_World(dc_rMap).Get_XRange() / (dc_rMap.GetWidth() * 0.001 / dPaperToDC);

		wxRect	rDC(m_pLayout->Get_PaperToDC(m_Rect));

		dc.DrawText(wxString::Format("%s 1:%s", _TL("Scale"), Get_SignificantDecimals_String(Scale).c_str()),
			rDC.GetLeft  (),
			rDC.GetBottom()
		);

		return( true );
	}
};

//---------------------------------------------------------
class CLayout_Label : public CLayout_Item
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Label );	}

	//-----------------------------------------------------
	CLayout_Label(CVIEW_Layout_Info *pLayout, const wxString &Text = "", bool bLongText = false)
		: CLayout_Item(pLayout)
	{
		Set_Rect_Percentage(10, 10, 100, 30);

		m_Parameters.Add_String("", "TEXT" , _TL("Text" ), _TL(""), _TL("Text"), bLongText);
		m_Parameters.Add_Font  ("", "FONT" , _TL("Font" ), _TL(""));
		m_Parameters.Add_Choice("", "ALIGN", _TL("Align"), _TL(""), CSG_String::Format("%s|%s|%s", _TL("left"), _TL("center"), _TL("right")));

		if( !Text.IsEmpty() )
		{
			CSG_String _Text(&Text); m_Parameters["TEXT"].Set_Value(_Text);
		}

		Properties(MDI_Get_Frame());
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		wxRect	r(m_pLayout->Get_PaperToDC(m_Rect));

		int	x, y, Align;

		switch( m_Parameters["ALIGN"].asInt() )
		{
		default: Align = TEXTALIGN_LEFT   ; x = r.x              ; y = r.y; break;
		case  1: Align = TEXTALIGN_XCENTER; x = r.x + r.width / 2; y = r.y; break;
		case  2: Align = TEXTALIGN_RIGHT  ; x = r.x + r.width    ; y = r.y; break;
		}

		wxFont	Font, oldFont(dc.GetFont()); wxColour Color, oldColor = dc.GetTextForeground();

		Set_Font(m_Parameters("FONT"), Font, Color);

		Font.SetPointSize((int)(0.5 + Font.GetPointSize() * m_pLayout->Get_PaperToDC()));

		dc.SetFont(Font);
		dc.SetTextForeground(Color);

		Draw_Text(dc, Align, x, y, m_Parameters["TEXT"].asString());

		dc.SetFont(oldFont);	// restore old font and color
		dc.SetTextForeground(oldColor);

		return( true );
	}
};

//---------------------------------------------------------
class CLayout_Text : public CLayout_Label
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Text );	}

	//-----------------------------------------------------
	CLayout_Text(CVIEW_Layout_Info *pLayout, const wxString &Text = "")
		: CLayout_Label(pLayout, Text, true)
	{}
};

//---------------------------------------------------------
class CLayout_Image : public CLayout_Item
{
public:
	virtual int			Get_ID			(void)	const	{	return( CVIEW_Layout_Info::ItemID_Image );	}

	//-----------------------------------------------------
	CLayout_Image(CVIEW_Layout_Info *pLayout, const wxImage &Image)
		: CLayout_Item(pLayout), m_Image(Image)
	{
		Set_Size(m_Image.GetSize());
	}

	CLayout_Image(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout)
	{
		Load_Image();
	}

	//-----------------------------------------------------
	wxImage	m_Image;

	bool				Set_Size			(const wxSize &Size)
	{
		m_Rect.x      =  10;
		m_Rect.y      =  10;
		m_Rect.width  = (int)(200 * Size.x / (double)Size.y);
		m_Rect.height = 200;

		return( true );
	}

	//-----------------------------------------------------
	bool				Load_Image			(void)
	{
		const wxString	Filter = wxString::Format(
			"%s|*.png;*.jpg;*.tif;*.tiff;*.bmp|"
			"%s (*.png)|*.png|"
			"%s (*.jpg)|*.jpg|"
			"%s (*.tif)|*.tif;*.tiff|"
			"%s|*.*",
			_TL("Recognized Files"),
			_TL("Portable Network Graphics"),
			_TL("JPEG"),
			_TL("Tagged Image File Format"),
			_TL("All Files")
		);

		wxString	File;

		return( DLG_Open(File, _TL("Load Image"), Filter) && m_Image.LoadFile(File) && m_Image.IsOk() && Set_Size(m_Image.GetSize()) );
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		if( m_Image.IsOk() && m_Rect.GetWidth() > 0 && m_Rect.GetHeight() > 0 )
		{
			wxRect	r(m_pLayout->Get_PaperToDC(m_Rect));

			dc.DrawBitmap(wxBitmap(m_Image.Scale(r.GetWidth(), r.GetHeight())), r.GetLeft(), r.GetTop());

			return( true );
		}

		return( false );
	}
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Layout_Printout : public wxPrintout
{
public:
	CVIEW_Layout_Printout(CVIEW_Layout_Info *pLayout)
		: wxPrintout(pLayout->Get_Name()), m_pLayout(pLayout)
	{}

	//-----------------------------------------------------
	virtual bool				OnPrintPage		(int iPage)
	{
		wxDC	*pDC	= HasPage(iPage) ? GetDC() : NULL;

		if( pDC )
		{
			return( m_pLayout->Draw(*pDC, true) );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool				HasPage			(int iPage)
	{
		return( iPage > 0 && iPage <= m_pLayout->Get_Page_Count() );
	}

	//-----------------------------------------------------
	virtual void				GetPageInfo		(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
	{
		*minPage		= 1;
		*maxPage		= m_pLayout->Get_Page_Count();

		*selPageFrom	= 1;
		*selPageTo		= m_pLayout->Get_Page_Count();
	}


protected:

	CVIEW_Layout_Info			*m_pLayout;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout_Info::CVIEW_Layout_Info(CWKSP_Map *pMap)
	: m_pMap(pMap)
{
	m_pPrintData	= new wxPrintData;
	m_pPrintData->SetOrientation      (wxLANDSCAPE    );
	m_pPrintData->SetPaperId          (wxPAPER_A4     );

	m_pPrintPage	= new wxPageSetupDialogData;
	m_pPrintPage->SetPrintData        (*m_pPrintData  );
	m_pPrintPage->SetMarginTopLeft    (wxPoint(10, 10));
	m_pPrintPage->SetMarginBottomRight(wxPoint(10, 10));

	m_Items.Add(new CLayout_Map   (this, pMap));
	m_Items.Add(new CLayout_Legend(this, pMap));

	m_Zoom		= 1.;
	m_PaperToDC	= 1.;
}

//---------------------------------------------------------
CVIEW_Layout_Info::~CVIEW_Layout_Info(void)
{
	delete(m_pPrintPage);
	delete(m_pPrintData);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CVIEW_Layout_Info::Get_Name(void)
{
	return( wxString::Format("SAGA %s: %s", _TL("Printout"), m_pMap->Get_Name().c_str()) );
}

//---------------------------------------------------------
int CVIEW_Layout_Info::Get_Page_Count(void)
{
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxSize CVIEW_Layout_Info::Get_PaperSize(void)
{
	wxSize	Size(m_pPrintPage->GetPaperSize());

	if(	(m_pPrintData->GetOrientation() == wxLANDSCAPE && Size.x < Size.y)
	||	(m_pPrintData->GetOrientation() == wxPORTRAIT  && Size.x > Size.y) )
	{
		m_pPrintPage->SetPaperSize(Size = wxSize(Size.y, Size.x));
	}

	return( Size );
}

//---------------------------------------------------------
wxRect CVIEW_Layout_Info::Get_Margins(void)
{
	wxSize	Size(Get_PaperSize());

	wxPoint	TL(Get_Margin_TopLeft()), BR(Get_Margin_BottomRight());

	return( wxRect(TL, wxSize(Size.x - TL.x - BR.x, Size.y - TL.y - BR.y)) );
}

//---------------------------------------------------------
wxPoint CVIEW_Layout_Info::Get_Margin_TopLeft(void)
{
	return( m_pPrintPage->GetMarginTopLeft() );
}

//---------------------------------------------------------
wxPoint CVIEW_Layout_Info::Get_Margin_BottomRight(void)
{
	return( m_pPrintPage->GetMarginBottomRight() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Page_Setup(void)
{
	(*m_pPrintPage)	= *m_pPrintData;

	wxPageSetupDialog	dlg(MDI_Get_Frame(), m_pPrintPage);

	if( dlg.ShowModal() == wxID_OK )
	{
		(*m_pPrintData) = dlg.GetPageSetupData().GetPrintData();
		(*m_pPrintPage)	= dlg.GetPageSetupData();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Print_Setup(void)
{
	wxPrintDialog	dlg(MDI_Get_Frame(), m_pPrintData);

	if( dlg.ShowModal() == wxID_OK )
	{
		*m_pPrintData	= dlg.GetPrintDialogData().GetPrintData();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Print_Preview(void)
{
	wxPrintPreview	*pPreview	= new wxPrintPreview(new CVIEW_Layout_Printout(this), new CVIEW_Layout_Printout(this), m_pPrintData);

	if( pPreview->Ok() )
	{
		wxPreviewFrame	*pFrame	= new wxPreviewFrame(pPreview, (wxFrame *)MDI_Get_Frame(), _TL("Print Preview"), wxPoint(100, 100), wxSize(600, 650), wxDEFAULT_FRAME_STYLE|wxMAXIMIZE);

		pFrame->Centre(wxBOTH);
		pFrame->Initialize();
		pFrame->Show(true);

		return( true );
	}

	delete(pPreview);

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Print(void)
{
	wxPrintDialogData	PrintData(*m_pPrintData);

	wxPrinter			Printer(&PrintData);

	if( Printer.Print(MDI_Get_Frame(), new CVIEW_Layout_Printout(this), true) )
	{
		(*m_pPrintData)	= Printer.GetPrintDialogData().GetPrintData();
		(*m_pPrintPage)	= Printer.GetPrintDialogData().GetPrintData();

		return( true );
	}

	if( wxPrinter::GetLastError() == wxPRINTER_ERROR )
	{
		MSG_Error_Add(_TL("There was a problem with printing.\nPerhaps your current printer is not set correctly?"));
	}
	else
	{
		MSG_Error_Add(_TL("You canceled printing"));
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Load(void)
{
	return( true );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Save(void)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Can_Delete(void)
{
	CLayout_Item	*pActive	= (CLayout_Item *)m_Items.Get_Active();

	return( pActive
		&& (pActive->Get_ID() == ItemID_Label
		||  pActive->Get_ID() == ItemID_Text
		||  pActive->Get_ID() == ItemID_Image)
	);
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Toggle_Item(int ItemID)
{
	CLayout_Item	*pItem	= Get_Item(ItemID);

	if( pItem )
	{
		m_Items       .Del(pItem, true);
		m_Items_Hidden.Add(pItem);

		return( true );
	}

	for(size_t i=0; i<m_Items_Hidden.Get_Count(); i++)
	{
		if( ((CLayout_Item *)m_Items_Hidden(i))->Get_ID() == ItemID )
		{
			pItem	= (CLayout_Item *)m_Items_Hidden(i);

			m_Items_Hidden.Del(pItem, true);
			m_Items       .Add(pItem);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CLayout_Item * CVIEW_Layout_Info::Get_Item(int ItemID)
{
	for(size_t i=0; i<m_Items.Get_Count(); i++)
	{
		if( ((CLayout_Item *)m_Items(i))->Get_ID() == ItemID )
		{
			return( (CLayout_Item *)m_Items(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Add_Item(int ItemID)
{
	switch( ItemID )
	{
	case ItemID_Label: m_Items.Add(new CLayout_Label(this), true); break;
	case ItemID_Text : m_Items.Add(new CLayout_Text (this), true); break;
	case ItemID_Image: m_Items.Add(new CLayout_Image(this), true); break;
	}

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Clipboard_Paste(void)
{
	bool	bResult	= false;

	Set_Buisy_Cursor(true);

	if( wxTheClipboard->Open() )
	{
		if( wxTheClipboard->IsSupported(wxDF_TEXT) )
		{
			wxTextDataObject	Data;

			if( wxTheClipboard->GetData(Data) )
			{
				m_Items.Add(new CLayout_Text(this, Data.GetText()));

				bResult	= true;
			}
		}

		if( wxTheClipboard->IsSupported(wxDF_BITMAP) )
		{
			wxBitmapDataObject	Data;

			if( wxTheClipboard->GetData(Data) )
			{
				m_Items.Add(new CLayout_Image(this, Data.GetBitmap().ConvertToImage()));

				bResult	= true;
			}
		}

		wxTheClipboard->Close();
	}

	Set_Buisy_Cursor(false);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Set_Zoom(double Zoom)
{
	if( Zoom > 0. && Zoom != m_Zoom )
	{
		m_Items.Scale(Zoom / m_Zoom);

		m_Zoom	= Zoom;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
wxRect CVIEW_Layout_Info::Get_PaperToDC(const wxRect &Rect)	const
{
	return( wxRect(
		(int)(0.5 + m_PaperToDC * Rect.x     ),
		(int)(0.5 + m_PaperToDC * Rect.y     ),
		(int)(0.5 + m_PaperToDC * Rect.width ),
		(int)(0.5 + m_PaperToDC * Rect.height))
	);
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Draw(wxDC &dc, bool bScale)
{
	m_PaperToDC	= !bScale ? 1. : dc.GetSize().GetWidth() / (m_Zoom * Get_PaperSize().GetWidth());

	m_Items.Draw(dc);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Info::_Fit_Scale(void)
{
	wxRect		dc_rMap(Get_Margins());
	CSG_Rect	rWorld(m_pMap->Get_World(dc_rMap));

	double	dDCToMeter	= 0.001 / m_PaperToDC;
	double	Scale		= rWorld.Get_XRange() / (dDCToMeter * dc_rMap.GetWidth());

	if( DLG_Get_Number(Scale, _TL("Fit Map Scale"), _TL("Scale 1 : ")) )
	{
		double	dx	= Scale * dDCToMeter * dc_rMap.GetWidth ();
		double	dy	= Scale * dDCToMeter * dc_rMap.GetHeight();

		rWorld.Assign(
			rWorld.Get_XCenter() - 0.5 * dx, rWorld.Get_YCenter() - 0.5 * dy,
			rWorld.Get_XCenter() + 0.5 * dx, rWorld.Get_YCenter() + 0.5 * dy
		);

		m_pMap->Set_Extent(rWorld);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
