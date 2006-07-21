
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
//               WKSP_Shapes_Manager.cpp                 //
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

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Manager::CWKSP_Shapes_Manager(void)
{
}

//---------------------------------------------------------
CWKSP_Shapes_Manager::~CWKSP_Shapes_Manager(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes_Manager::Get_Name(void)
{
	return( LNG("[CAP] Shapes") );
}

//---------------------------------------------------------
wxString CWKSP_Shapes_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf("<b>%s</b><br>", LNG("[CAP] Shapes"));

	for(int i=0; i<Get_Count(); i++)
	{
		s.Append(wxString::Format("%s: %d<br>", Get_Item(i)->Get_Name().c_str(), ((CWKSP_Base_Manager *)Get_Item(i))->Get_Count()));
	}

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Shapes_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Shapes"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_OPEN);

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
bool CWKSP_Shapes_Manager::On_Command(int Cmd_ID)
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
CWKSP_Shapes_Type * CWKSP_Shapes_Manager::Get_Shapes_Type(int Shape_Type)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Shape_Type == ((CWKSP_Shapes_Type *)Get_Item(i))->Get_Shapes_Type() )
		{
			return( (CWKSP_Shapes_Type *)Get_Item(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Shapes_Type * CWKSP_Shapes_Manager::_Get_Shapes_Type(int Shape_Type)
{
	CWKSP_Shapes_Type	*pItem;

	if( Shape_Type != SHAPE_TYPE_Undefined )
	{
		if( (pItem = Get_Shapes_Type(Shape_Type)) == NULL )
		{
			Add_Item(pItem = new CWKSP_Shapes_Type(Shape_Type));
		}

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
CWKSP_Shapes * CWKSP_Shapes_Manager::Get_Shapes(CShapes *pShapes)
{
	if( pShapes && Get_Shapes_Type(pShapes->Get_Type()) != NULL )
	{
		return( Get_Shapes_Type(pShapes->Get_Type())->Get_Shapes(pShapes) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Exists(CShapes *pShapes)
{
	return( Get_Shapes(pShapes) != NULL );
}

//---------------------------------------------------------
CWKSP_Shapes * CWKSP_Shapes_Manager::Add(CShapes *pShapes)
{
	if( pShapes && pShapes->is_Valid() && !Exists(pShapes) )
	{
		return( _Get_Shapes_Type(pShapes->Get_Type())->Add(pShapes) );
	}

	return( NULL );
}

//---------------------------------------------------------
CShapes * CWKSP_Shapes_Manager::Get_byFileName(const char *File_Name)
{
	CSG_String	s(File_Name);
	CWKSP_Shapes_Type	*pType;

	for(int i=0; i<Get_Count(); i++)
	{
		pType	= (CWKSP_Shapes_Type *)Get_Item(i);

		for(int j=0; j<pType->Get_Count(); j++)
		{
			if( !s.Cmp(pType->Get_Shapes(j)->Get_Shapes()->Get_File_Path()) )
			{
				return( pType->Get_Shapes(j)->Get_Shapes() );
			}
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
bool CWKSP_Shapes_Manager::Update(CShapes *pShapes, CParameters *pParameters)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
	{
		pItem->DataObject_Changed(pParameters);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Update_Views(CShapes *pShapes)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
	{
		pItem->Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Show(CShapes *pShapes)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
	{
		return( pItem->Show() );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::asImage(CShapes *pShapes, CGrid *pImage)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
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
bool CWKSP_Shapes_Manager::Get_Colors(CShapes *pShapes, CSG_Colors *pColors)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
	{
		return( pItem->Get_Colors(pColors) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Set_Colors(CShapes *pShapes, CSG_Colors *pColors)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
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
