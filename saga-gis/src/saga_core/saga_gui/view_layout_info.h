
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
		ItemID_Scalebar,
		ItemID_Scale,
		ItemID_Legend,
		ItemID_Label,
		ItemID_Text,
		ItemID_Image,
		ItemID_None
	};

	static const char *				Get_Item_Type_Name		(int Type);


	//-----------------------------------------------------
	CVIEW_Layout_Info(class CWKSP_Map *pMap);
	virtual ~CVIEW_Layout_Info(void);

	CSGDI_Layout_Items				m_Items;

	class CWKSP_Map *				Get_Map					(void)	{	return( m_pMap );	}

	wxString						Get_Name				(void);
	int								Get_Page_Count			(void);

	CSG_Parameter &					Get_Parameter			(const CSG_String &ID)	{	return( m_Parameters[ID] );	}

	bool							Properties				(void);

	wxSize							Get_PaperSize			(void);
	wxRect							Get_Margins				(void);
	wxPoint							Get_Margin_TopLeft		(void);
	wxPoint							Get_Margin_BottomRight	(void);

	bool							Page_Setup				(void);
	bool							Print_Setup				(void);
	bool							Print					(void);
	bool							Print_Preview			(void);

	bool							Load					(void);
	bool							Load					(const CSG_MetaData &Layout);
	bool							Save					(void)	const;
	bool							Save					(CSG_MetaData &Layout)	const;

	bool							is_Shown				(int ItemID);
	bool							Can_Hide				(class CLayout_Item *pItem = NULL);
	bool							Can_Delete				(class CLayout_Item *pItem = NULL);
	bool							Toggle_Item				(class CLayout_Item *pItem = NULL);
	bool							Toggle_Item				(int ItemID);
	bool							Add_Item				(int ItemID);
	class CLayout_Item *			Get_Item				(int ItemID);
	bool							Clipboard_Paste			(void);

	wxMenu *						Menu_Get_Active			(void);
	bool							Menu_On_Command			(wxCommandEvent  &event);
	bool							Menu_On_Command_UI		(wxUpdateUIEvent &event);

	bool							Set_Zoom				(double Zoom);

	double							Get_Paper2DC			(void)					const	{	return(                       m_Paper2DC     );	}
	wxRect							Get_Paper2DC			(const wxRect &Rect)	const	{	return( Get_Rect_Scaled(Rect, m_Paper2DC   ) );	}

	bool							Draw					(wxDC &dc, bool bPrintOut = false);

	bool							Export					(void);


protected:

	double							m_Zoom, m_Paper2DC;

	CSG_Parameters					m_Parameters;

	class CWKSP_Map					*m_pMap;

	class wxPrintData				*m_pPrintData;

	class wxPageSetupData			*m_pPrintPage;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H
