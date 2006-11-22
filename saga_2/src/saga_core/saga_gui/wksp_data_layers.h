
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
class CWKSP_Data_Layers_Item : public wxPanel
{
	DECLARE_CLASS(CWKSP_Data_Layers_Item)

public:
	CWKSP_Data_Layers_Item(wxWindow *pParent, class CWKSP_Layer *pLayer);
	CWKSP_Data_Layers_Item(wxWindow *pParent, const char *Title);
	virtual ~CWKSP_Data_Layers_Item(void)	{}

	void						On_Paint			(wxPaintEvent &event);

	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_LDClick	(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);

	bool						is_Title			(void)	{	return( m_pLayer == NULL );	}


private:

	wxString					m_Title;

	class CWKSP_Layer			*m_pLayer;

	CSG_Data_Object				*m_pObject;


	bool						_Set_Layer_Active	(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
class CWKSP_Data_Layers : public wxScrolledWindow
{
	DECLARE_CLASS(CWKSP_Data_Layers)

public:
	CWKSP_Data_Layers(wxWindow *pParent);
	virtual ~CWKSP_Data_Layers(void);

	void						On_Mouse_RDown		(wxMouseEvent &event);

	void						On_Size				(wxSizeEvent &event);

	void						Update				(void);

	void						Set_Item_Size		(int Size);
	int							Get_Item_Size		(void)		{	return( m_Size );	}


private:

	bool						m_bCategorised;

	int							m_xScroll, m_yScroll, m_nItems, m_Size;

	CWKSP_Data_Layers_Item		**m_Items;

	CSG_Parameters				m_Parameters;


	void						_Set_Positions		(void);

	bool						_Add_Items			(class CWKSP_Base_Item *pItem);
	bool						_Add_Item			(class CWKSP_Layer *pLayer);
	bool						_Add_Item			(const char *Title);
	bool						_Del_Items			(void);

//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
extern CWKSP_Data_Layers		*g_pLayers;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Layers_H
