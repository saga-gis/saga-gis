
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
//                 SAGA_Frame_Layout.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_Layout_H
#define _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_Layout_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if SAGA_GUI_FRAME_LAYOUT == 1
	#define __SAGA_GUI_USE_wxFL__

	class wxFrameLayout;

#else
	#define __SAGA_GUI_USE_wxDOCKIT__

	class wxDockWindow;
	class wxBarHolder;
	class wxLayoutManager;
	class wxSlideBar;

#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char *	SAGA_GUI_Get_Version_FrameLayout	(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSAGA_Frame_Layout
{
public:

	CSAGA_Frame_Layout(class wxMDIParentFrame *pFrame);
	virtual ~CSAGA_Frame_Layout(void);


	void						Show_Initially				(void);

	class wxToolBarBase *		TB_Create					(int ID);
	void						TB_Add						(class wxToolBarBase *pToolBar, const char *Name);
	void						TB_Add_Item					(class wxToolBarBase *pToolBar, bool bCheck, int Cmd_ID);
	void						TB_Add_Separator			(class wxToolBarBase *pToolBar);

	void						Bar_Add						(wxWindow *pWindow, wxString Name);
	void						Bar_Toggle					(wxWindow *pWindow);
	void						Bar_Show					(wxWindow *pWindow, bool bShow);


private:

	typedef struct
	{
		wxWindow				*pWnd;

#if defined(__SAGA_GUI_USE_wxDOCKIT__)

		wxDockWindow			*pWnd_Dock;

		wxBarHolder				*pWnd_Bar;

#endif
	}
	TBar;


private:

	class wxMDIParentFrame		*m_pFrame;

#ifdef __SAGA_GUI_USE_wxFL__

	wxFrameLayout				*m_pLayout;

#elif defined(__SAGA_GUI_USE_wxDOCKIT__)

	wxLayoutManager				*m_pLayout;

	wxSlideBar					*m_pSlideBar;

#endif

	int							m_nBars;

	TBar						*m_Bars;


	void						_Bar_Position_Read			(wxWindow *pWindow, wxString Name, bool bToolbar);
	void						_Bar_Position_Write			(TBar &Bar);

	TBar *						_Bar_Find					(wxWindow *pWindow);
	void						_Bar_Initialise				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_Layout_H
