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
//                WKSP_Table_Manager.cpp                 //
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
#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Table_Manager::CWKSP_Table_Manager(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Table_Manager::Get_Name(void)
{
	return( _TL("Tables") );
}

//---------------------------------------------------------
wxString CWKSP_Table_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b>: %d<br>"), _TL("Tables"), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Table_Manager::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(_TL("Tables"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLES_OPEN);

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Table * CWKSP_Table_Manager::Get_Data(CSG_Table *pObject)
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
CWKSP_Table * CWKSP_Table_Manager::Add_Data(CSG_Table *pObject)
{
	CWKSP_Table	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL )
	{
		Add_Item(pItem = new CWKSP_Table(pObject));
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
