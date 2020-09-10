
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
//                  VIEW_Layout_Info.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>

//---------------------------------------------------------
#include <saga_gdi/sgdi_layout_items.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Layout_Info
{
public:
	enum
	{
		ItemID_Map	= 0,
		ItemID_Legend,
		ItemID_Scalebar,
		ItemID_Scale,
		ItemID_Label,
		ItemID_Text,
		ItemID_Image,
		ItemID_None
	};

	CVIEW_Layout_Info(class CWKSP_Map *pMap);
	virtual ~CVIEW_Layout_Info(void);

	CSGDI_Layout_Items				m_Items;

	class CWKSP_Map *				Get_Map					(void)	{	return( m_pMap );	}

	wxString						Get_Name				(void);
	int								Get_Page_Count			(void);

	wxSize							Get_PaperSize			(void);
	wxRect							Get_Margins				(void);
	wxPoint							Get_Margin_TopLeft		(void);
	wxPoint							Get_Margin_BottomRight	(void);

	bool							Page_Setup				(void);
	bool							Print_Setup				(void);
	bool							Print					(void);
	bool							Print_Preview			(void);

	bool							Set_Zoom				(double Zoom);
	double							Get_Zoom				(void)	const	{	return( m_Zoom );	}

	wxRect							Get_PaperToDC			(const wxRect &Rect)	const;
	double							Get_PaperToDC			(void)	const	{	return( m_PaperToDC );	}

	bool							Draw					(wxDC &dc, bool bScale = false);

	bool							Load					(void);
	bool							Save					(void);

	bool							Can_Delete				(void);
	bool							Toggle_Item				(int ItemID);
	bool							Add_Item				(int ItemID);
	class CLayout_Item *			Get_Item				(int ItemID);
	bool							Clipboard_Paste			(void);


protected:

	double							m_Zoom, m_PaperToDC;

	class CWKSP_Map					*m_pMap;

	class wxPrintData				*m_pPrintData;

	class wxPageSetupData			*m_pPrintPage;

	CSGDI_Layout_Items				m_Items_Hidden;


	void							_Fit_Scale				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H
