
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
//                   ACTIVE_Layers.h                     //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Layers_H
#define _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Layers_H


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
class CACTIVE_Layers_Item : public wxPanel
{
	DECLARE_CLASS(CACTIVE_Layers_Item)

public:
	CACTIVE_Layers_Item(wxWindow *pParent, int ax, int ay, int dx, int dy, class CWKSP_Layer *pLayer);
	virtual ~CACTIVE_Layers_Item(void)	{}

	void						On_Paint		(wxPaintEvent &event);


private:

	class CWKSP_Layer			*m_pLayer;


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
class CACTIVE_Layers : public wxScrolledWindow
{
	DECLARE_CLASS(CACTIVE_Layers)

public:
	CACTIVE_Layers(wxWindow *pParent);
	virtual ~CACTIVE_Layers(void);

	void						On_Mouse_Motion	(wxMouseEvent &event);
	void						On_Key_Down		(wxKeyEvent   &event);

	virtual void				OnDraw			(wxDC &dc);

	void						Set_Item		(class CWKSP_Base_Item *pItem);


private:

	int							m_xScroll, m_yScroll, m_nItems;

	double						m_Zoom;

	CACTIVE_Layers_Item			**m_Items;

	class CWKSP_Base_Item		*m_pItem;


	void						_Layers_Clear	(void);
	CACTIVE_Layers_Item *		_Layers_Add		(int ax, int ay, int dx, int dy, CWKSP_Layer *pLayer);
	CWKSP_Layer *				_Layers_Get		(wxPoint p);

	bool						_Add_Items		(wxSize &Size, class CWKSP_Base_Item *pItem);
	bool						_Add_Item		(wxSize &Size, class CWKSP_Layer *pLayer);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Layers_H
