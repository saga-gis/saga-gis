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
//                 WKSP_Grid_System.cpp                  //
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
#include "res_commands.h"

#include "helper.h"

#include "wksp_grid_system.h"
#include "wksp_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid_System::CWKSP_Grid_System(const CSG_Grid_System &System)
{
	m_System	= System;

	m_Parameters.Add_String(
		NULL	, "NAME"	, _TL("Name"),
		_TL("Descriptive name for the grid system"),
		m_System.Get_Name()
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid_System::Get_Name(void)
{
	return( m_Parameters("NAME")->asString() );
}

//---------------------------------------------------------
wxString CWKSP_Grid_System::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b>"), _TL("Grid System"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR (_TL("Name")				, Get_Name());
	DESC_ADD_INT (_TL("Number of Grids")	, Get_Count());
	DESC_ADD_FLT (_TL("West")				, m_System.Get_XMin());
	DESC_ADD_FLT (_TL("East")				, m_System.Get_XMax());
	DESC_ADD_FLT (_TL("West-East")			, m_System.Get_XRange());
	DESC_ADD_FLT (_TL("South")				, m_System.Get_YMin());
	DESC_ADD_FLT (_TL("North")				, m_System.Get_YMax());
	DESC_ADD_FLT (_TL("South-North")		, m_System.Get_YRange());
	DESC_ADD_FLT (_TL("Cell Size")			, m_System.Get_Cellsize());
	DESC_ADD_INT (_TL("Number of Columns")	, m_System.Get_NX());
	DESC_ADD_INT (_TL("Number of Rows")		, m_System.Get_NY());
	DESC_ADD_LONG(_TL("Number of Cells")	, m_System.Get_NCells());

	s	+= wxT("</table>");

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid_System::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_System::Get_Data(CSG_Grid *pObject)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pObject == Get_Data(i)->Get_Object() )
		{
			return( Get_Data(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_System::Add_Data(CSG_Grid *pObject)
{
	CWKSP_Grid	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL )
	{
		Add_Item(pItem = new CWKSP_Grid(pObject));
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
