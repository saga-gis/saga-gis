
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
#include "wx/wx.h"
#include "wx/print.h"
#include "wx/printdlg.h"

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
CVIEW_Layout_Info::CVIEW_Layout_Info(CWKSP_Map *pMap)
{
	m_pMap		= pMap;

	m_pPrint	= new wxPrintData;
	m_pPage		= new wxPageSetupDialogData;

	m_pPrint	->SetOrientation		(wxLANDSCAPE);
	m_pPage		->SetPrintData			(*m_pPrint);
	m_pPage		->SetMarginTopLeft		(wxPoint(10, 10));
	m_pPage		->SetMarginBottomRight	(wxPoint(10, 10));
}

//---------------------------------------------------------
CVIEW_Layout_Info::~CVIEW_Layout_Info(void)
{
	delete(m_pPage);
	delete(m_pPrint);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Setup_Print(void)
{
	wxPrintDialogData	Data(*m_pPrint);
	wxPrintDialog		dlg(MDI_Get_Frame(), &Data);

//	dlg.GetPrintDialogData().SetSetupDialog(true);
	
	if( dlg.ShowModal() == wxID_OK )
	{
		*m_pPrint	= dlg.GetPrintDialogData().GetPrintData();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Setup_Page(void)
{
	(*m_pPage)	= *m_pPrint;

	wxPageSetupDialog	dlg(MDI_Get_Frame(), m_pPage);

	if( dlg.ShowModal() == wxID_OK )
	{
		(*m_pPrint) = dlg.GetPageSetupData().GetPrintData();
		(*m_pPage)	= dlg.GetPageSetupData();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Print(void)
{
	wxPrintDialogData	Data(*m_pPrint);
	wxPrinter			Printer(&Data);

	if( Printer.Print(MDI_Get_Frame(), Get_Printout(), true) )
	{
		(*m_pPrint)	= Printer.GetPrintDialogData().GetPrintData();
		(*m_pPage)	= Printer.GetPrintDialogData().GetPrintData();

		return( true );
	}

	if( wxPrinter::GetLastError() == wxPRINTER_ERROR )
		MSG_Error_Add(LNG("[ERR] There was a problem printing.\nPerhaps your current printer is not set correctly?"));
	else
		MSG_Error_Add(LNG("[ERR] You canceled printing"));

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Layout_Info::Print_Preview(void)
{
	wxPrintDialogData	Data(*m_pPrint);
	wxPrintPreview		*pPreview;
	wxPreviewFrame		*pFrame;

	pPreview	= new wxPrintPreview(Get_Printout(), Get_Printout(), &Data);

	if( pPreview->Ok() )
	{
		pFrame	= new wxPreviewFrame(pPreview, (wxFrame *)MDI_Get_Frame(), LNG("[CAP] Print Preview"), wxPoint(100, 100), wxSize(600, 650), wxDEFAULT_FRAME_STYLE|wxMAXIMIZE);
		pFrame->Centre(wxBOTH);
		pFrame->Initialize();
		pFrame->Show(true);

		return( true );
	}

	delete(pPreview);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout_Printout * CVIEW_Layout_Info::Get_Printout(void)
{
	return( new CVIEW_Layout_Printout(this) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxSize CVIEW_Layout_Info::Get_PaperSize(void)
{
	wxSize	s(m_pPage->GetPaperSize());

	if(	(m_pPrint->GetOrientation() == wxLANDSCAPE && s.x < s.y)
	||	(m_pPrint->GetOrientation() == wxPORTRAIT  && s.x > s.y) )
	{
		s	= wxSize(s.y, s.x);
		m_pPage->SetPaperSize(s);
	}

	return( s );

//	return( m_pPage->GetPaperSize() );
}

//---------------------------------------------------------
wxRect CVIEW_Layout_Info::Get_Margins(void)
{
	wxRect	r(Get_PaperSize());
	wxPoint	pTL(Get_Margin_TopLeft()), pBR(Get_Margin_BottomRight());

	return( wxRect(pTL, wxSize(r.GetWidth() - pTL.x - pBR.x, r.GetHeight() - pTL.y - pBR.y)) );
}

//---------------------------------------------------------
wxPoint CVIEW_Layout_Info::Get_Margin_TopLeft(void)
{
	return( m_pPage->GetMarginTopLeft() );
}

//---------------------------------------------------------
wxPoint CVIEW_Layout_Info::Get_Margin_BottomRight(void)
{
	return( m_pPage->GetMarginBottomRight() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CVIEW_Layout_Info::Get_Name(void)
{
	return( LNG("SAGA: Print Map") );
}

//---------------------------------------------------------
int CVIEW_Layout_Info::Get_Page_Count(void)
{
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Info::Fit_Scale(void)
{
	int			fMap;
	double		zMap, zLegend, Scale, dx, dy;
	wxRect		rMap, rLegend;
	CSG_Rect	rWorld;

	if( _Get_Layout(Get_PaperSize(), zMap, fMap, rMap, zLegend, rLegend) )
	{
		rWorld	= m_pMap->Get_World(rMap);
		Scale	= 100.0 * zMap * rWorld.Get_XRange() / (double)rMap.GetWidth();

		if( DLG_Get_Number(Scale, LNG("[CAP] Fit Map Scale"), LNG("[DLG] Scale 1 : ")) )
		{
			dx	= Scale * (double)rMap.GetWidth () / zMap / 100.0 / 2.0;
			dy	= Scale * (double)rMap.GetHeight() / zMap / 100.0 / 2.0;

			rWorld.Assign(
				rWorld.Get_XCenter() - dx, rWorld.Get_YCenter() - dy,
				rWorld.Get_XCenter() + dx, rWorld.Get_YCenter() + dy
			);

			m_pMap->Set_Extent(rWorld);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::_Get_Layout(wxSize sClient, double &zMap, int &fMap, wxRect &rMap, double &zLegend, wxRect &rLegend)
{
	int		Space;
	wxSize	sLegend;
	wxRect	rFrame(Get_Margins());

	//-----------------------------------------------------
	zMap	= sClient.GetWidth() / (double)Get_PaperSize().GetWidth();
	rFrame	= wxRect(
				(int)(zMap * rFrame.GetLeft  ()),
				(int)(zMap * rFrame.GetTop   ()),
				(int)(zMap * rFrame.GetWidth ()),
				(int)(zMap * rFrame.GetHeight())
			);

	Space	= (int)(10.0 * zMap);

	//-----------------------------------------------------
	if( m_pMap->Get_Print_Legend() && m_pMap->Get_Legend_Size(sLegend, zMap) )
	{
		zLegend	= (double)rFrame.GetHeight() / (double)sLegend.y;

		if( zLegend * sLegend.x > 0.2 * (double)rFrame.GetWidth() )
		{
			zLegend	= 0.2 * (double)rFrame.GetWidth() / (double)sLegend.x;
		}

		rFrame.SetWidth (rFrame.GetWidth() - (int)(zLegend * sLegend.x + Space));

		rLegend	= wxRect(rFrame.GetRight() + Space, rFrame.GetTop(), (int)(zLegend * sLegend.x), (int)(zLegend * sLegend.y));
	}
	else
	{
		rLegend	= wxRect(0, 0, 0, 0);
	}

	//-----------------------------------------------------
	fMap	= (int)(m_pMap->Get_Print_Frame() * zMap);
	rMap	= rFrame;
	rMap.Deflate(fMap);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Info::Draw(wxDC &dc)
{
	int		fMap;
	double	zMap, zLegend, Scale;
	wxRect	rMap, rLegend;

	if( _Get_Layout(dc.GetSize(), zMap, fMap, rMap, zLegend, rLegend) )
	{
		//-------------------------------------------------
		if( rLegend.GetWidth() > 0 )
		{
			m_pMap->Draw_Legend(dc, zMap, zLegend, wxPoint(rLegend.GetX(), rLegend.GetY()));

			Draw_Edge(dc, EDGE_STYLE_SIMPLE, rLegend.GetLeft()  - (int)(5 * zMap), rLegend.GetTop(), rLegend.GetRight(), rLegend.GetBottom());
		}

		//-------------------------------------------------
		if( rMap.GetWidth() > 0 )
		{
			m_pMap->Draw_Map(dc, zMap, rMap, false);

			if( fMap > 0 )
			{
				m_pMap->Draw_Frame(dc, rMap, fMap);
			}

			//---------------------------------------------
			Scale	= 100.0 * zMap * m_pMap->Get_World(rMap).Get_XRange() / (double)rMap.GetWidth();
			dc.DrawText(wxString::Format(wxT("%s 1:%s"), LNG("[CAP] Map Scale"), Get_SignificantDecimals_String(Scale).c_str()), rMap.GetLeft(), rMap.GetBottom() + fMap);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
