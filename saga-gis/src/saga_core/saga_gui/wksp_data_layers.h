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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Layers_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Layers_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/scrolwin.h>


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
	CWKSP_Data_Button(wxWindow *pParent, class CWKSP_Base_Item *pItem);

	bool						is_Manager			(void);


private:

	class CWKSP_Base_Item		*m_pItem;


	void						On_Paint			(wxPaintEvent &event);
	void						On_Key				(wxKeyEvent   &event);
	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_LDClick	(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);

	bool						_Set_Active			(bool bKeepOthers);

	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Buttons : public wxScrolledWindow
{
	DECLARE_CLASS(CWKSP_Data_Buttons)

public:
	CWKSP_Data_Buttons(wxWindow *pParent);
	virtual ~CWKSP_Data_Buttons(void);

	void						Update_Buttons		(void);


private:

	int							m_nItems, m_xScroll, m_yScroll;

	CWKSP_Data_Button			**m_Items;


	void						On_Size				(wxSizeEvent  &event);
	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);

	bool						_Del_Items			(void);
	bool						_Add_Items			(class CWKSP_Base_Item    *pItem);
	bool						_Add_Item			(class CWKSP_Data_Item    *pItem);
	bool						_Add_Item			(class CWKSP_Base_Manager *pItem);

	void						_Set_Positions		(void);

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
