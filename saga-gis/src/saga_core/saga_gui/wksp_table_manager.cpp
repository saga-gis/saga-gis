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
{
}

//---------------------------------------------------------
CWKSP_Table_Manager::~CWKSP_Table_Manager(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Table_Manager::Get_Name(void)
{
	return( _TL("[CAP] Tables") );
}

//---------------------------------------------------------
wxString CWKSP_Table_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b>: %d<br>"), _TL("[CAP] Tables"), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Table_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("[CAP] Tables"));

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
bool CWKSP_Table_Manager::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Table * CWKSP_Table_Manager::Get_Table(CSG_Table *pTable)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pTable == Get_Table(i)->Get_Table() )
		{
			return( Get_Table(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Table_Manager::Exists(CSG_Table *pTable)
{
	return( Get_Table(pTable) != NULL );
}

//---------------------------------------------------------
CWKSP_Table * CWKSP_Table_Manager::Add(CSG_Table *pTable)
{
	CWKSP_Table	*pItem;

	if( pTable && !Exists(pTable) && Add_Item(pItem = new CWKSP_Table(pTable, this)) )
	{
		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Table * CWKSP_Table_Manager::Get_byFileName(const wxString &File_Name)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( !File_Name.Cmp(Get_Table(i)->Get_Table()->Get_File_Name()) )
		{
			return( Get_Table(i)->Get_Table() );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Table_Manager::Update(CSG_Table *pTable, CSG_Parameters *pParameters)
{
	CWKSP_Table	*pItem;

	if( (pItem = Get_Table(pTable)) != NULL )
	{
		return( pItem->DataObject_Changed(pParameters) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Table_Manager::Update_Views(CSG_Table *pTable)
{
	CWKSP_Table	*pItem;

	if( (pItem = Get_Table(pTable)) != NULL )
	{
		pItem->Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Table_Manager::Show(CSG_Table *pTable)
{
	CWKSP_Table	*pItem;

	if( (pItem = Get_Table(pTable)) != NULL )
	{
		pItem->Set_View(true);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
