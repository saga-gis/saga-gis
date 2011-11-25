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
//                 WKSP_TIN_Manager.cpp                  //
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

#include "wksp_map_manager.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_TIN_Manager::CWKSP_TIN_Manager(void)
{
}

//---------------------------------------------------------
CWKSP_TIN_Manager::~CWKSP_TIN_Manager(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_TIN_Manager::Get_Name(void)
{
	return( _TL("[CAP] TIN") );
}

//---------------------------------------------------------
wxString CWKSP_TIN_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b>:%d<br>"), _TL("[CAP] TIN"), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_TIN_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("[CAP] TIN"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TIN_OPEN);

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
bool CWKSP_TIN_Manager::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

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
CWKSP_TIN * CWKSP_TIN_Manager::Get_TIN(CSG_TIN *pTIN)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pTIN == Get_TIN(i)->Get_TIN() )
		{
			return( Get_TIN(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::Exists(CSG_TIN *pTIN)
{
	return( Get_TIN(pTIN) != NULL );
}

//---------------------------------------------------------
CWKSP_TIN * CWKSP_TIN_Manager::Add(CSG_TIN *pTIN)
{
	CWKSP_TIN	*pItem;

	if( pTIN && pTIN->is_Valid() && !Exists(pTIN) && Add_Item(pItem = new CWKSP_TIN(pTIN)) )
	{
		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_TIN * CWKSP_TIN_Manager::Get_byFileName(const wxChar *File_Name)
{
	CSG_String	s(File_Name);

	for(int i=0; i<Get_Count(); i++)
	{
		if( !s.Cmp(Get_TIN(i)->Get_TIN()->Get_File_Name()) )
		{
			return( Get_TIN(i)->Get_TIN() );
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
bool CWKSP_TIN_Manager::Update(CSG_TIN *pTIN, CSG_Parameters *pParameters)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		pItem->DataObject_Changed(pParameters);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::Update_Views(CSG_TIN *pTIN)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		pItem->Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::Show(CSG_TIN *pTIN, int Map_Mode)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		switch( Map_Mode )
		{
		case SG_UI_DATAOBJECT_SHOW:
			return( pItem->Show() );

		case SG_UI_DATAOBJECT_SHOW_NEW_MAP:
			g_pMaps->Add(pItem, NULL);

		case SG_UI_DATAOBJECT_SHOW_LAST_MAP:
			return( pItem->Show(g_pMaps->Get_Map(g_pMaps->Get_Count() - 1)) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::asImage(CSG_TIN *pTIN, CSG_Grid *pImage)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		return( pItem->asImage(pImage) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_TIN_Manager::Get_Colors(CSG_TIN *pTIN, CSG_Colors *pColors)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		return( pItem->Get_Colors(pColors) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::Set_Colors(CSG_TIN *pTIN, CSG_Colors *pColors)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		pItem->DataObject_Changed(pColors);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
