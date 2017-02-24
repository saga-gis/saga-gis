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
	s	+= wxString::Format("<h4>%s</h4>", _TL("Grid System"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR (_TL("Name"             ), Get_Name());
	DESC_ADD_INT (_TL("Number of Grids"  ), Get_Count());
	DESC_ADD_STR (_TL("West"             ), SG_Get_String(m_System.Get_XMin    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("East"             ), SG_Get_String(m_System.Get_XMax    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("West-East"        ), SG_Get_String(m_System.Get_XRange  (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South"            ), SG_Get_String(m_System.Get_YMin    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("North"            ), SG_Get_String(m_System.Get_YMax    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South-North"      ), SG_Get_String(m_System.Get_YRange  (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("Cell Size"        ), SG_Get_String(m_System.Get_Cellsize(), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_INT (_TL("Number of Columns"), m_System.Get_NX());
	DESC_ADD_INT (_TL("Number of Rows"   ), m_System.Get_NY());
	DESC_ADD_LONG(_TL("Number of Cells"  ), m_System.Get_NCells());

	s	+= "</table>";

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
