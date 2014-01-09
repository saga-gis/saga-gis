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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "view_layout_printout.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Layout_Info
{
public:
	CVIEW_Layout_Info(class CWKSP_Map *pMap);
	virtual ~CVIEW_Layout_Info(void);

	class CWKSP_Map *				Get_Map					(void)	{	return( m_pMap );	}

	bool							Setup_Print				(void);
	bool							Setup_Page				(void);
	bool							Print					(void);
	bool							Print_Preview			(void);

	wxSize							Get_PaperSize			(void);
	wxRect							Get_Margins				(void);
	wxPoint							Get_Margin_TopLeft		(void);
	wxPoint							Get_Margin_BottomRight	(void);

	wxString						Get_Name				(void);
	int								Get_Page_Count			(void);

	CVIEW_Layout_Printout *			Get_Printout			(void);

	void							Fit_Scale				(void);

	bool							Draw					(wxDC &dc);


protected:

	class CWKSP_Map					*m_pMap;

	class wxPrintData				*m_pPrint;

	class wxPageSetupData			*m_pPage;


	bool							_Get_Layout				(wxSize sClient, double &zMap, int &fMap, wxRect &rMap, double &zLegend, wxRect &rLegend);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Info_H
