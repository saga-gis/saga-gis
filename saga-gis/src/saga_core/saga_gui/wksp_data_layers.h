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
//                  WKSP_Data_Layers.h                   //
//                                                       //
//          Copyright (C) 2006 by Olaf Conrad            //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Layers_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Layers_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/scrolwin.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Button : public wxPanel
{
	DECLARE_CLASS(CWKSP_Data_Button)

public:
	CWKSP_Data_Button(wxWindow *pParent, class CWKSP_Layer *pLayer);
	CWKSP_Data_Button(wxWindow *pParent, const wxString &Title);
	virtual ~CWKSP_Data_Button(void)	{}

	void						On_Paint			(wxPaintEvent &event);

	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_LDClick	(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);

	bool						is_Title			(void)		{	return( m_pLayer == NULL );	}


private:

	wxString					m_Title;

	class CWKSP_Layer			*m_pLayer;

	CSG_Data_Object				*m_pObject;


	bool						_Select				(bool bKeepOthers);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
class CWKSP_Data_Buttons : public wxScrolledWindow
{
	DECLARE_CLASS(CWKSP_Data_Buttons)

public:
	CWKSP_Data_Buttons(wxWindow *pParent);
	virtual ~CWKSP_Data_Buttons(void);

	void						On_Mouse_RDown		(wxMouseEvent &event);

	void						On_Size				(wxSizeEvent  &event);

	void						Update_Buttons		(void);

	int							Get_Item_Size		(void)		{	return( m_Size );	}
	long						Get_Active_Color	(void)		{	return( m_Active_Color );	}


private:

	bool						m_bCategorised;

	int							m_xScroll, m_yScroll, m_nItems, m_Size;

	long						m_Active_Color;

	CWKSP_Data_Button			**m_Items;

	CSG_Parameters				m_Parameters;


	void						_Set_Positions		(void);

	bool						_Add_Items			(class CWKSP_Base_Item *pItem);
	bool						_Add_Item			(class CWKSP_Layer *pLayer);
	bool						_Add_Item			(const wxString &Title);
	bool						_Del_Items			(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
extern CWKSP_Data_Buttons		*g_pData_Buttons;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Layers_H
