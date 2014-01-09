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
//                    VIEW_Layout.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "view_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Layout : public CVIEW_Base
{
public:
	CVIEW_Layout(class CVIEW_Layout_Info *pInfo);
	virtual ~CVIEW_Layout(void);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	void							On_Size				(wxSizeEvent  &event);

	void							On_Page_Setup		(wxCommandEvent &event);
	void							On_Print_Setup		(wxCommandEvent &event);
	void							On_Print			(wxCommandEvent &event);
	void							On_Print_Preview	(wxCommandEvent &event);
	void							On_Fit_Scale		(wxCommandEvent &event);

	class CVIEW_Layout_Info *		Get_Info			(void)	{	return( m_pInfo );	}

	void							Ruler_Set_Position	(int x, int y);
	void							Ruler_Refresh		(double xMin, double xMax, double yMin, double yMax);

	bool							Refresh_Layout		(void);


private:

	class CVIEW_Ruler				*m_pRuler_X, *m_pRuler_Y;

	class CVIEW_Layout_Info			*m_pInfo;

	class CVIEW_Layout_Control		*m_pControl;


private:

	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_Layout)

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_H
