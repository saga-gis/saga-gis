
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

#include "res_commands.h"
#include "res_dialogs.h"

#include "wksp_map.h"

#include "view_layout_info.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PointsPerMM		(25.4 / 72.)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Item : public CSGDI_Layout_Items::CSGDI_Layout_Item
{
public:
	virtual int			Get_Type		(void)	const	= 0;

	//-----------------------------------------------------
	CLayout_Item(CVIEW_Layout_Info *pLayout, double x = 10, double y = 10, double width = 100, double height = 100)
		: m_pLayout(pLayout)
	{
		m_Parameters.Set_Name("properties");

		wxSize	Size(m_pLayout->Get_PaperSize());	// default size

		m_Rect.x      = (int)(0.5 + x      * Size.GetWidth () / 100.);
		m_Rect.width  = (int)(0.5 + width  * Size.GetWidth () / 100.);
		m_Rect.y      = (int)(0.5 + y      * Size.GetHeight() / 100.);
		m_Rect.height = (int)(0.5 + height * Size.GetHeight() / 100.);
	}

	//-----------------------------------------------------
	virtual bool		Properties			(wxWindow *pParent)
	{
		if( m_Parameters.Get_Count() > 0 && DLG_Parameters(&m_Parameters) )
		{
			Adjust_Size();

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool		Adjust_Size			(void)
	{
		return( true );
	}

	//-----------------------------------------------------
	CVIEW_Layout_Info	*m_pLayout;	CSG_Parameters	m_Parameters;
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Map : public CLayout_Item
{
public:
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Map );	}

	//-----------------------------------------------------
	CLayout_Map(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout, 2, 2, 73, 83)
	{
		m_Parameters.Add_Bool(""          , "FRAME_SHOW", _TL("Frame"), _TL(""), true);
		m_Parameters.Add_Int ("FRAME_SHOW", "FRAME_SIZE", _TL("Size" ), _TL(""), 5, 2, true);
	}

	//-----------------------------------------------------
	wxRect				Get_Rect_DC		(void)
	{
		wxRect	r(m_pLayout->Get_Item2Paper(m_Rect));
		
		if( m_Parameters["FRAME_SHOW"].asBool() )
		{
			r.Deflate(m_Parameters["FRAME_SIZE"].asInt());
		}

		return(	m_pLayout->Get_Paper2DC(r) );
	}

	//-----------------------------------------------------
	virtual bool		Draw			(wxDC &dc)
	{
		wxRect	rFrame(m_pLayout->Get_Item2DC(m_Rect)), rMap(Get_Rect_DC());

		m_pLayout->Get_Map()->Draw_Map(dc, m_pLayout->Get_Paper2DC(), rMap, LAYER_DRAW_FLAG_NOEDITS);

		if( m_Parameters["FRAME_SHOW"].asBool() )
		{
			m_pLayout->Get_Map()->Draw_Frame(dc, m_pLayout->Get_Map()->Get_World(rMap), rMap, rMap.x - rFrame.x, false);
		}

		return( true );
	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Legend : public CLayout_Item
{
public:
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Legend );	}

	//-----------------------------------------------------
	CLayout_Legend(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout, 77, 2, 21, 48)
	{}

	//-----------------------------------------------------
	virtual bool		Adjust_Size			(void)
	{
		wxSize	Size;

		if( m_pLayout->Get_Map()->Get_Legend_Size(Size, 1.) )
		{
			double	Ratio	= Size.y / (double)Size.x;

			Set_Ratio(Ratio);

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool		Draw			(wxDC &dc)
	{
		Adjust_Size();

		wxSize	Size;

		if( m_pLayout->Get_Map()->Get_Legend_Size(Size, m_pLayout->Get_Paper2DC()) )
		{
			wxRect	r(m_pLayout->Get_Item2DC(m_Rect));

			double	Scale	= r.GetHeight() / (double)Size.y;

			if( Scale * Size.x > r.GetWidth() )
			{
				Scale	= r.GetWidth() / (double)Size.x;
			}

			m_pLayout->Get_Map()->Draw_Legend(dc, m_pLayout->Get_Paper2DC(), Scale, r.GetLeftTop());

			return( true );
		}

		return( false );
	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Scalebar : public CLayout_Item
{
public:
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Scalebar );	}

	//-----------------------------------------------------
	CLayout_Scalebar(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout, 2, 87, 50, 5)
	{
		m_Parameters.Add_Choice("",
			"UNIT"	, _TL("Unit"),
			_TL(""),
			CSG_String::Format("%s|%s",
				_TL("do not show"),
				_TL("automatically")
			), 1
		);

		m_Parameters.Add_Choice("",
			"STYLE"	, _TL("Style"),
			_TL(""),
			CSG_String::Format("%s|%s",
				_TL("scale line"),
				_TL("alternating scale bar")
			), 1
		);
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		int	Style	= SCALE_STYLE_LINECONN|SCALE_STYLE_GLOOMING|SCALE_STYLE_UNIT_BELOW;

		if( m_Parameters("STYLE")->asInt() == 1 )
		{
			Style	|= SCALE_STYLE_BLACKWHITE;
		}

		wxRect	r(m_pLayout->Get_Item2DC(m_Rect)), rMap(((CLayout_Map *)m_pLayout->Get_Item(CVIEW_Layout_Info::ItemID_Map))->Get_Rect_DC());

		double	Length	= r.width * m_pLayout->Get_Map()->Get_World(rMap).Get_XRange() / rMap.width;

		CSG_String	Unit;

		if( m_Parameters("UNIT")->asInt() >= 1 )
		{
			CSG_Projection	Projection(m_pLayout->Get_Map()->Get_Projection());

			if( Projection.is_Okay() )
			{
				Unit	= SG_Get_Projection_Unit_Name(Projection.Get_Unit(), true);

				if( Unit.is_Empty() )	Unit	= Projection.Get_Unit_Name();

				if( Projection.Get_Unit() == SG_PROJ_UNIT_Meter && Length > 10000. )
				{
					Unit	 = SG_Get_Projection_Unit_Name(SG_PROJ_UNIT_Kilometer, true);

					Length	/= 1000.;
				}
			}
		}

		Draw_Scale(dc, r, 0., Length, SCALE_HORIZONTAL, SCALE_TICK_TOP, Style, Unit.c_str());

		return( true );
	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Scale : public CLayout_Item
{
public:
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Scale );	}

	//-----------------------------------------------------
	CLayout_Scale(CVIEW_Layout_Info *pLayout)
		: CLayout_Item(pLayout, 77, 50, 21, 21)
	{
		m_Parameters.Add_String("", "TEXT"    , _TL("Text"    ), _TL(""), _TL("Scale"));
		m_Parameters.Add_Font  ("", "FONT"    , _TL("Font"    ), _TL(""));
		m_Parameters.Add_Int   ("", "DECIMALS", _TL("Decimals"), _TL("Ignored if set to -1."), 0, -1, true);

		Set_Sizer(false);

		Adjust_Size();
	}

	//-----------------------------------------------------
	virtual bool		Adjust_Size			(void)
	{
		wxRect	r(m_Rect);
		wxFont	Font; wxColour Color; Set_Font(m_Parameters("FONT"), Font, Color);
		wxMemoryDC	dc; dc.GetMultiLineTextExtent(Get_Scale_Text(), &r.width, &r.height, NULL, &Font);

		r.width  = (int)(0.5 + r.width  * PointsPerMM / m_pLayout->Get_Item2Paper());
		r.height = (int)(0.5 + r.height * PointsPerMM / m_pLayout->Get_Item2Paper());

		Set_Rect(r);

		return( true );
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		Adjust_Size();

		wxRect	r(m_pLayout->Get_Item2DC(m_Rect)), rMap(((CLayout_Map *)m_pLayout->Get_Item(CVIEW_Layout_Info::ItemID_Map))->Get_Rect_DC());

		wxFont	Font, oldFont(dc.GetFont()); wxColour Color, oldColor = dc.GetTextForeground();

		Set_Font(m_Parameters("FONT"), Font, Color);
		Font.Scale((float)(PointsPerMM * m_pLayout->Get_Paper2DC()));
		dc.SetFont(Font);
		dc.SetTextForeground(Color);

		Draw_Text(dc, TEXTALIGN_CENTER, r.x + r.width / 2, r.y + r.height / 2, Get_Scale_Text());

		dc.SetFont(oldFont);	// restore old font and color
		dc.SetTextForeground(oldColor);

		return( true );
	}

	//-----------------------------------------------------
	wxString			Get_Scale_Text		(void)
	{
		wxRect	rMap(((CLayout_Map *)m_pLayout->Get_Item(CVIEW_Layout_Info::ItemID_Map))->Get_Rect_DC());

		double	Scale	= 1000. * m_pLayout->Get_Map()->Get_World(rMap).Get_XRange() / (rMap.width / m_pLayout->Get_Paper2DC());	// to meter

		wxString	Text(m_Parameters["TEXT"].asString());
		
		Text	+= Text.IsEmpty() ? " 1 : " : "1 : ";

		int	Decimals	= m_Parameters["DECIMALS"].asInt();

		if( Decimals < 0 )
		{
			Text	+= Get_SignificantDecimals_String(Scale);
		}
		else
		{
			Text	+= wxString::Format("%.*f", Decimals, Scale);
		}

		return( Text );
	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Label : public CLayout_Item
{
public:
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Label );	}

	//-----------------------------------------------------
	CLayout_Label(CVIEW_Layout_Info *pLayout, bool bProperties = false, const wxString &Text = "", bool bLongText = false)
		: CLayout_Item(pLayout, 10, 10, 100, 30)
	{
		m_Parameters.Add_String("", "TEXT" , _TL("Text" ), _TL(""), _TL("Text"), bLongText);
		m_Parameters.Add_Font  ("", "FONT" , _TL("Font" ), _TL(""));
		m_Parameters.Add_Choice("", "ALIGN", _TL("Align"), _TL(""), CSG_String::Format("%s|%s|%s", _TL("left"), _TL("center"), _TL("right")));

		Set_Sizer(false);

		if( !Text.IsEmpty() )
		{
			CSG_String _Text(&Text); m_Parameters["TEXT"].Set_Value(_Text);
		}

		if( bProperties )
		{
			Properties(MDI_Get_Frame());
		}
	}

	//-----------------------------------------------------
	virtual bool		Adjust_Size			(void)
	{
		wxRect	r(m_Rect);
		wxFont	Font; wxColour Color; Set_Font(m_Parameters("FONT"), Font, Color);
		wxMemoryDC	dc; dc.GetMultiLineTextExtent(m_Parameters["TEXT"].asString(), &r.width, &r.height, NULL, &Font);

		r.width  = (int)(0.5 + r.width  * PointsPerMM / m_pLayout->Get_Item2Paper());
		r.height = (int)(0.5 + r.height * PointsPerMM / m_pLayout->Get_Item2Paper());

		Set_Rect(r);

		return( true );
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		wxRect	r(m_pLayout->Get_Item2DC(m_Rect));

		int	x, y, Align;

		switch( m_Parameters["ALIGN"].asInt() )
		{
		default: Align = TEXTALIGN_LEFT   ; x = r.x              ; y = r.y; break;
		case  1: Align = TEXTALIGN_XCENTER; x = r.x + r.width / 2; y = r.y; break;
		case  2: Align = TEXTALIGN_RIGHT  ; x = r.x + r.width    ; y = r.y; break;
		}

		wxFont	Font, oldFont(dc.GetFont()); wxColour Color, oldColor = dc.GetTextForeground();

		Set_Font(m_Parameters("FONT"), Font, Color);
		Font.Scale((float)(PointsPerMM * m_pLayout->Get_Paper2DC()));
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
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Text );	}

	//-----------------------------------------------------
	CLayout_Text(CVIEW_Layout_Info *pLayout, bool bProperties = false, const wxString &Text = "")
		: CLayout_Label(pLayout, bProperties, Text, true)
	{}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLayout_Image : public CLayout_Item
{
public:
	virtual int			Get_Type		(void)	const	{	return( CVIEW_Layout_Info::ItemID_Image );	}

	//-----------------------------------------------------
	CLayout_Image(CVIEW_Layout_Info *pLayout, bool bDialog = false)
		: CLayout_Item(pLayout)
	{
		On_Construction();

		if( bDialog )
		{
			Load();
		}
	}

	CLayout_Image(CVIEW_Layout_Info *pLayout, const CSG_String &File)
		: CLayout_Item(pLayout)
	{
		On_Construction();

		if( !m_Image.LoadFile(File.c_str()) )
		{
			Load();
		}
	}

	CLayout_Image(CVIEW_Layout_Info *pLayout, const wxImage &Image)
		: CLayout_Item(pLayout), m_Image(Image)
	{
		On_Construction();

		Set_Size(m_Image.GetSize());
	}

	//-----------------------------------------------------
	wxImage	m_Image;	CSG_String	m_File;

	bool				On_Construction		(void)
	{
		m_Parameters.Add_FilePath("", "FILE", _TL("File"), _TL(""),
			CSG_String::Format(
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
			)
		);

		return( true );
	}

	bool				Set_Size			(const wxSize &Size)
	{
		m_Rect.x      =  10;
		m_Rect.y      =  10;
		m_Rect.width  = (int)(200 * Size.x / (double)Size.y);
		m_Rect.height = 200;

		return( true );
	}

	//-----------------------------------------------------
	bool				Load				(void)
	{
		wxString	File;

		return( DLG_Open(File, _TL("Load Image"), m_Parameters["FILE"].asFilePath()->Get_Filter()) && Load(File, true) );
	}

	bool				Load				(const wxString &File, bool bAdjustSize)
	{
		if( wxFileExists(File) && m_Image.LoadFile(File) && m_Image.IsOk() )
		{
			m_File	= File.wc_str();

			m_Parameters["FILE"].Set_Value(m_File);

			if( bAdjustSize )
			{
				Set_Size(m_Image.GetSize());
			}

			return( true );
		}

		return( false );
	}

	bool				Save				(void)
	{
		wxString	File;	int	Type;

		if( m_Image.IsOk() && DLG_Image_Save(File, Type) && m_Image.SaveFile(File, (wxBitmapType)Type) )
		{
			m_File	= File.wc_str();

			m_Parameters["FILE"].Set_Value(m_File);

			return( true );
		}

		return( false );
	}

	bool				Restore				(void)
	{
		return( m_Image.IsOk() && Set_Size(m_Image.GetSize()) );
	}

	//-----------------------------------------------------
	virtual bool		Properties			(wxWindow *pParent)
	{
		if( CLayout_Item::Properties(pParent) )
		{
			if( m_File.Cmp(m_Parameters["FILE"].asString()) )
			{
				if( !Load(m_Parameters["FILE"].asString(), false) )
				{
					m_Parameters["FILE"].Set_Value(m_File);
				}
			}

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool		Draw				(wxDC &dc)
	{
		if( m_Rect.GetWidth() > 0 && m_Rect.GetHeight() > 0 )
		{
			wxRect	r(m_pLayout->Get_Item2DC(m_Rect));

			if( m_Image.IsOk() )
			{
				dc.DrawBitmap(wxBitmap(m_Image.Scale(r.GetWidth(), r.GetHeight())), r.GetLeft(), r.GetTop());
			}
			else
			{
				wxBrush oldBrush(dc.GetBrush()); dc.SetBrush(*wxTRANSPARENT_BRUSH);
				wxPen   oldPen  (dc.GetPen  ()); dc.SetPen  (*wxRED_PEN          );

				dc.DrawRectangle(r);
				dc.DrawLine(r.GetBottomLeft (), r.GetTopRight());
				dc.DrawLine(r.GetBottomRight(), r.GetTopLeft ());

				dc.SetBrush(oldBrush);
				dc.SetPen  (oldPen  );
			}

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
			if( !m_Bmp.IsOk() )	// initialize bitmap...
			{
				wxSize	Size(pDC->GetSize());

				double	DPI	= Size.GetWidth() * 25.4 / m_pLayout->Get_PaperSize().GetWidth();

				if( DPI > m_pLayout->Get_Parameter("MAX_DPI").asDouble() )
				{
					DPI	= m_pLayout->Get_Parameter("MAX_DPI").asDouble();

					Size.x = (int)(0.5 + m_pLayout->Get_PaperSize().GetWidth () * DPI / 25.4);
					Size.y = (int)(0.5 + m_pLayout->Get_PaperSize().GetHeight() * DPI / 25.4);
				}

				m_Bmp.Create(Size.x, Size.y);

				wxMemoryDC	dc_Bmp(m_Bmp);

				dc_Bmp.SetBackground(*wxWHITE_BRUSH);
				dc_Bmp.Clear();

				m_pLayout->Draw(dc_Bmp, true);
			}

			//---------------------------------------------
			if( m_Bmp.IsOk() )	// bitmap has been initialized...
			{
				wxMemoryDC	dc_Bmp(m_Bmp);

				if( m_Bmp.GetWidth () != pDC->GetSize().GetWidth ()
				||  m_Bmp.GetHeight() != pDC->GetSize().GetHeight() )
				{
					pDC->StretchBlit(wxPoint(0, 0), pDC->GetSize(), &dc_Bmp, wxPoint(0, 0), dc_Bmp.GetSize());
				}
				else
				{
					pDC->       Blit(wxPoint(0, 0), pDC->GetSize(), &dc_Bmp, wxPoint(0, 0));
				}

				return( true );
			}
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

	wxBitmap					m_Bmp;

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
	m_pPrintPage->SetMarginTopLeft    (wxPoint(10, 10)); // millimetres
	m_pPrintPage->SetMarginBottomRight(wxPoint(10, 10)); // millimetres

	m_Items.Add(new CLayout_Map     (this));
	m_Items.Add(new CLayout_Legend  (this));
	m_Items.Add(new CLayout_Scalebar(this));
	m_Items.Add(new CLayout_Scale   (this));

	m_Items.Hide(Get_Item(ItemID_Scale));

	m_Zoom       = 1.;
	m_Item2Paper = 1.;
	m_Item2DC    = 1.;
	m_Paper2DC   = 1.;

	//-----------------------------------------------------
	m_Parameters.Add_Int("",
		"MAX_DPI"	, _TL("Maximum Resolution"),
		_TL("Maximum resolution [dots per inch], ignored if zero."),
		300, 0, true
	);
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
bool CVIEW_Layout_Info::Properties(void)
{
	return( DLG_Parameters(&m_Parameters) );
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

	wxPoint	TL(Get_Margin_TopLeft()), BR(Get_Margin_BottomRight()); // millimetres

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
	wxString	File, Filter = wxString::Format(
		"%s|*.sg-layout;*.xml|"
		"%s (*.sg-layout)|*.sg-layout|"
		"%s (*.xml)|*.xml|"
		"%s|*.*",
		_TL("Recognized Files"),
		_TL("SAGA Print Layout"),
		_TL("XML Files"),
		_TL("All Files")
	);

	if( DLG_Open(File, wxString::Format("%s %s", _TL("Load"), _TL("Print Layout")), Filter) )
	{
		CSG_MetaData	Layout;

		return( Layout.Load(&File) && Load(Layout) );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Load(const CSG_MetaData &Layout)
{
	if( !Layout.Cmp_Name("layout") || !Layout("general") || !Layout("items") )
	{
		return( false );
	}

	if( SG_Compare_Version(Layout.Get_Property("saga-version"), "7.8.0") < 0 )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s %s: %s", _TL("Warning"), _TL("unsupported version"), Layout.Get_Property("saga-version")));
	}

	for(size_t i=m_Items.Get_Count(); i>0; i--)
	{
		CLayout_Item	*pItem	= (CLayout_Item *)m_Items.Get_Item(i - 1);

		if( pItem->Get_Type() == ItemID_Label
		||  pItem->Get_Type() == ItemID_Text
		||  pItem->Get_Type() == ItemID_Image )
		{
			m_Items.Del(pItem);
		}
	}

	//-----------------------------------------------------
	const CSG_MetaData	&General = Layout["general"];

	if( General("parameters") )
	{
		m_Parameters.Serialize(*General("parameters"), false);
	}

	//-----------------------------------------------------
	const CSG_MetaData	&Items   = Layout["items"];

	for(int i=0; i<Items.Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Item	= Items[i];

		CLayout_Item *pItem = NULL; int	Type; if( !Item.Get_Property("type", Type) ) { Type = ItemID_None; }

		switch( Type )
		{
		case ItemID_Map     : pItem = Get_Item(ItemID_Map     ); break;
		case ItemID_Legend  : pItem = Get_Item(ItemID_Legend  ); break;
		case ItemID_Scalebar: pItem = Get_Item(ItemID_Scalebar); break;
		case ItemID_Scale   : pItem = Get_Item(ItemID_Scale   ); break;

		case ItemID_Label   : pItem = new CLayout_Label  (this); break;
		case ItemID_Text    : pItem = new CLayout_Text   (this); break;
		case ItemID_Image   : pItem = new CLayout_Image  (this); break;
		}

		if( pItem )
		{
			if( Item("left") && Item("top") && Item("width") && Item("height") )
			{
				wxRect	r;	double	d;

				Item["left"  ].Get_Content().asDouble(d); r.x      = (int)(0.5 + d / m_Item2Paper);
				Item["top"   ].Get_Content().asDouble(d); r.y      = (int)(0.5 + d / m_Item2Paper);
				Item["width" ].Get_Content().asDouble(d); r.width  = (int)(0.5 + d / m_Item2Paper);
				Item["height"].Get_Content().asDouble(d); r.height = (int)(0.5 + d / m_Item2Paper);

				pItem->Set_Rect(r);
			}

			if( Item("parameters") )
			{
				pItem->m_Parameters.Serialize(*Item("parameters"), false);
			}

			if( pItem->Get_Type() == ItemID_Label
			||  pItem->Get_Type() == ItemID_Text
			||  pItem->Get_Type() == ItemID_Image )
			{
				m_Items.Add(pItem);
			}
			else
			{
				m_Items.Move_Top(pItem);

				if( Item.Cmp_Property("show", "false", true) )
				{
					m_Items.Hide(pItem);
				}
				else
				{
					m_Items.Show(pItem);
				}
			}

			if( Type == ItemID_Image )
			{
				((CLayout_Image *)pItem)->Load(pItem->m_Parameters["FILE"].asString(), false);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Save(void)	const
{
	wxString	File, Filter = wxString::Format(
		"%s (*.sg-layout)|*.sg-layout|"
		"%s (*.xml)|*.xml|"
		"%s|*.*",
		_TL("SAGA Print Layout"),
		_TL("XML Files"),
		_TL("All Files")
	);

	if( DLG_Save(File, wxString::Format("%s %s", _TL("Save"), _TL("Print Layout")), Filter) )
	{
		CSG_MetaData	Layout;

		return( Save(Layout) && Layout.Save(&File) );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Save(CSG_MetaData &Layout)	const
{
	Layout.Set_Name    ("layout");
	Layout.Add_Property("saga-version", SAGA_VERSION);

	//-----------------------------------------------------
	CSG_MetaData	&General = *Layout.Add_Child("general");

	m_Parameters.Serialize(*General.Add_Child());

	//-----------------------------------------------------
	CSG_MetaData	&Items   = *Layout.Add_Child("items");

	for(size_t i=0; i<m_Items.Get_Count(); i++)
	{
		CSG_MetaData	&Item	= *Items.Add_Child("item");

		CLayout_Item	*pItem	= (CLayout_Item *)m_Items.Get_Item(i);

		Item.Add_Property("type" , pItem->Get_Type());
		Item.Add_Property("show" , pItem->is_Shown());

		wxRect	r(pItem->Get_Rect());

		Item.Add_Child("left"  , r.x      * m_Item2Paper);
		Item.Add_Child("top"   , r.y      * m_Item2Paper);
		Item.Add_Child("width" , r.width  * m_Item2Paper);
		Item.Add_Child("height", r.height * m_Item2Paper);

		pItem->m_Parameters.Serialize(*Item.Add_Child());
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::is_Shown(int ItemID)
{
	CLayout_Item	*pItem	= Get_Item(ItemID);

	return( pItem && pItem->is_Shown() );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Can_Hide(CLayout_Item *pItem)
{
	if( !pItem  )
	{
		pItem	= (CLayout_Item *)m_Items.Get_Active();
	}

	return( pItem
		&& (pItem->Get_Type() == ItemID_Legend
		||  pItem->Get_Type() == ItemID_Scalebar
		||  pItem->Get_Type() == ItemID_Scale)
	);
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Can_Delete(CLayout_Item *pItem)
{
	if( !pItem  )
	{
		pItem	= (CLayout_Item *)m_Items.Get_Active();
	}

	return( pItem
		&& (pItem->Get_Type() == ItemID_Label
		||  pItem->Get_Type() == ItemID_Text
		||  pItem->Get_Type() == ItemID_Image)
	);
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Toggle_Item(CLayout_Item *pItem)
{
	if( !pItem  )
	{
		pItem	= (CLayout_Item *)m_Items.Get_Active();
	}

	if( pItem && Can_Hide(pItem) )
	{
		return( pItem->is_Shown() ? m_Items.Hide(pItem) : m_Items.Show(pItem) );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Toggle_Item(int ItemID)
{
	CLayout_Item	*pItem	= Get_Item(ItemID);

	if( pItem )
	{
		return( pItem->is_Shown() ? m_Items.Hide(pItem) : m_Items.Show(pItem) );
	}

	return( false );
}

//---------------------------------------------------------
CLayout_Item * CVIEW_Layout_Info::Get_Item(int ItemID)
{
	for(size_t i=0; i<m_Items.Get_Count(); i++)
	{
		if( ((CLayout_Item *)m_Items(i))->Get_Type() == ItemID )
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
	case ItemID_Label: m_Items.Add(new CLayout_Label(this, true), true); break;
	case ItemID_Text : m_Items.Add(new CLayout_Text (this, true), true); break;
	case ItemID_Image: m_Items.Add(new CLayout_Image(this, true), true); break;
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
				m_Items.Add(new CLayout_Text(this, true, Data.GetText()));

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
wxMenu * CVIEW_Layout_Info::Menu_Get_Active(void)
{
	if( m_Items.Get_Active() )
	{
		wxMenu	*pMenu	= new wxMenu;

		if( Can_Hide() )
		{
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_HIDE);
			pMenu->AppendSeparator();
		}

		if( Can_Delete() )
		{
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_DELETE);
			pMenu->AppendSeparator();
		}

		if( m_Items.Get_Count() > 1 )
		{
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_MOVE_TOP   );
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_MOVE_UP    );
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_MOVE_DOWN  );
			pMenu->AppendSeparator();
		}

		if( ((CLayout_Item *)m_Items.Get_Active())->Get_Type() == ItemID_Image )
		{
			CLayout_Image	*pItem	= (CLayout_Image *)m_Items.Get_Active();

			if( !SG_File_Exists(pItem->m_File) )
			{
				CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_IMAGE_SAVE);
			}

			CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_IMAGE_RESTORE);
			pMenu->AppendSeparator();
		}

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_ITEM_PROPERTIES );

		return( pMenu );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Menu_On_Command(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_LAYOUT_ITEM_PROPERTIES : m_Items.Active_Properties()      ; break;
	case ID_CMD_LAYOUT_ITEM_DELETE     : m_Items.Del(m_Items.Get_Active()); break;
	case ID_CMD_LAYOUT_ITEM_HIDE       : Toggle_Item()                    ; break;

	case ID_CMD_LAYOUT_ITEM_MOVE_TOP   : m_Items.Active_Move_Top   (); break;
	case ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM: m_Items.Active_Move_Bottom(); break;
	case ID_CMD_LAYOUT_ITEM_MOVE_UP    : m_Items.Active_Move_Up    (); break;
	case ID_CMD_LAYOUT_ITEM_MOVE_DOWN  : m_Items.Active_Move_Down  (); break;

	case ID_CMD_LAYOUT_IMAGE_SAVE      : ((CLayout_Image *)m_Items.Get_Active())->Save   (); break;
	case ID_CMD_LAYOUT_IMAGE_RESTORE   : ((CLayout_Image *)m_Items.Get_Active())->Restore(); break;
	}

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Menu_On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_LAYOUT_ITEM_MOVE_TOP   : event.Enable(!m_Items.Active_is_Top   ()); break;
	case ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM: event.Enable(!m_Items.Active_is_Bottom()); break;
	case ID_CMD_LAYOUT_ITEM_MOVE_UP    : event.Enable(!m_Items.Active_is_Top   ()); break;
	case ID_CMD_LAYOUT_ITEM_MOVE_DOWN  : event.Enable(!m_Items.Active_is_Bottom()); break;
	}

	return( true );
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

		m_Zoom	= Zoom;	m_Item2Paper	= 1. / m_Zoom;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Draw(wxDC &dc, bool bPrintOut)
{
	m_Paper2DC	= dc.GetSize().GetWidth() / ((double)Get_PaperSize().GetWidth());

	if( bPrintOut )
	{
		m_Item2DC = m_Item2Paper * m_Paper2DC;

		m_Items.Draw(dc);

		m_Item2DC	= 1.;

		return( true );
	}

	//-----------------------------------------------------
	m_Items.Draw(dc);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Export(void)
{
	wxString	File;	int	Type;	int DPI;

	if( DLG_Image_Save(File, Type, "", Get_Name()) && DLG_Get_Number(DPI = 150, _TL("Export to Image Resolution"), _TL("Dots per Inch")) )
	{
		Set_Buisy_Cursor(true);

		wxRect		r(Get_Rect_Scaled(Get_PaperSize(), DPI / 25.4));
		wxBitmap	Bmp(r.GetWidth(), r.GetHeight());
		wxMemoryDC	dc(Bmp);

		dc.SetBackground(*wxWHITE_BRUSH); dc.Clear();

		Draw(dc, true);

		dc.SelectObject(wxNullBitmap);

		if( Bmp.SaveFile(File, (wxBitmapType)Type) )
		{
			Set_Buisy_Cursor(false);

			return( true );
		}

		Set_Buisy_Cursor(false);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Info::_Fit_Scale(void)
{
	wxRect		dc_rMap(Get_Margins());
	CSG_Rect	rWorld(m_pMap->Get_World(dc_rMap));

	double	dDCToMeter	= 0.001 / m_Item2DC;
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
