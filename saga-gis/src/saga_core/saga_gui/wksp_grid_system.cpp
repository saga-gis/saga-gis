
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
		NULL	, "NAME"	, LNG("Name"),
		LNG("Descriptive name for the grid system"),
		m_System.Get_Name()
	);
}

//---------------------------------------------------------
CWKSP_Grid_System::~CWKSP_Grid_System(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid_System::Parameters_Changed(void)
{
	CWKSP_Base_Item::Parameters_Changed();
}

//---------------------------------------------------------
wxString CWKSP_Grid_System::Get_Name(void)
{
	return( m_Parameters("NAME")->asString() );
}

//---------------------------------------------------------
wxString CWKSP_Grid_System::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b><table border=\"0\">"), LNG("[CAP] Grid System"));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Name")					, Get_Name().c_str()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%d</td></tr>"),
		LNG("[CAP] Number Of Grids")		, Get_Count()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%d (x) * %d (y) = %ld</td></tr>"),
		LNG("[CAP] Number Of Cells")		, m_System.Get_NX(), m_System.Get_NY(), m_System.Get_NCells()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%f</td></tr>"),
		LNG("[CAP] Cell Size")				, m_System.Get_Cellsize()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>[%f] - [%f] = [%f]</td></tr>"),
		LNG("[CAP] West-East")				, m_System.Get_XMin(), m_System.Get_XMax(), m_System.Get_XRange()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>[%f] - [%f] = [%f]</td></tr>"),
		LNG("[CAP] South-North")			, m_System.Get_YMin(), m_System.Get_YMax(), m_System.Get_YRange()
	));

	s.Append(wxT("</table>"));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid_System::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_System::Get_Grid(CSG_Grid *pGrid)
{
	if( pGrid )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( pGrid == Get_Grid(i)->Get_Grid() )
			{
				return( Get_Grid(i) );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Grid_System::Exists(CSG_Grid *pGrid)
{
	return( Get_Grid(pGrid) != NULL );
}

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_System::Add(CSG_Grid *pGrid)
{
	CWKSP_Grid	*pItem;

	if( pGrid && pGrid->Get_System() == m_System && !Exists(pGrid) && Add_Item(pItem = new CWKSP_Grid(pGrid)) )
	{
		return( pItem );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
