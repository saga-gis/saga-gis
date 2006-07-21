
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
	return( LNG("[CAP] T.I.N.") );
}

//---------------------------------------------------------
wxString CWKSP_TIN_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf("<b>%s</b>:%d<br>", LNG("[CAP] T.I.N."), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_TIN_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] T.I.N."));

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
CWKSP_TIN * CWKSP_TIN_Manager::Get_TIN(CTIN *pTIN)
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
bool CWKSP_TIN_Manager::Exists(CTIN *pTIN)
{
	return( Get_TIN(pTIN) != NULL );
}

//---------------------------------------------------------
CWKSP_TIN * CWKSP_TIN_Manager::Add(CTIN *pTIN)
{
	CWKSP_TIN	*pItem;

	if( pTIN && pTIN->is_Valid() && !Exists(pTIN) && Add_Item(pItem = new CWKSP_TIN(pTIN)) )
	{
		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CTIN * CWKSP_TIN_Manager::Get_byFileName(const char *File_Name)
{
	CSG_String	s(File_Name);

	for(int i=0; i<Get_Count(); i++)
	{
		if( !s.Cmp(Get_TIN(i)->Get_TIN()->Get_File_Path()) )
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
bool CWKSP_TIN_Manager::Update(CTIN *pTIN, CParameters *pParameters)
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
bool CWKSP_TIN_Manager::Update_Views(CTIN *pTIN)
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
bool CWKSP_TIN_Manager::Show(CTIN *pTIN)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		return( pItem->Show() );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::asImage(CTIN *pTIN, CGrid *pImage)
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
bool CWKSP_TIN_Manager::Get_Colors(CTIN *pTIN, CSG_Colors *pColors)
{
	CWKSP_TIN	*pItem;

	if( (pItem = Get_TIN(pTIN)) != NULL )
	{
		return( pItem->Get_Colors(pColors) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_TIN_Manager::Set_Colors(CTIN *pTIN, CSG_Colors *pColors)
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
