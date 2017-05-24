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
//                       INFO.cpp                        //
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
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_images.h"

#include "info.h"
#include "info_messages.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_MESSAGES	= 0,
	IMG_EXECUTION,
	IMG_ERROR
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CINFO	*g_pINFO	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CINFO, wxNotebook)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CINFO::CINFO(wxWindow *pParent)
	: wxNotebook(pParent, ID_WND_INFO, wxDefaultPosition, wxDefaultSize, wxNB_TOP|wxNB_MULTILINE, _TL("Messages"))
{
	g_pINFO		= this;

	//-----------------------------------------------------
	AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));

	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_INFO_MESSAGES);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_INFO_EXECUTION);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_INFO_ERROR);
	
	//-----------------------------------------------------
	m_pGeneral		= new CINFO_Messages(this);
	m_pExecution	= new CINFO_Messages(this);
	m_pErrors		= new CINFO_Messages(this);
}

//---------------------------------------------------------
void CINFO::Add_Pages(void)
{
	AddPage(m_pGeneral  , _TL("General"  ), false, IMG_MESSAGES);
	AddPage(m_pExecution, _TL("Execution"), false, IMG_EXECUTION);
	AddPage(m_pErrors   , _TL("Errors"   ), false, IMG_ERROR);
}

//---------------------------------------------------------
CINFO::~CINFO(void)
{
	g_pINFO		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
