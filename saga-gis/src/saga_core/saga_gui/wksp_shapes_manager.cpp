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
#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "helper.h"

#include "wksp_data_manager.h"

#include "wksp_map_manager.h"

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
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes_Manager::Get_Name(void)
{
	return( _TL("Shapes") );
}

//---------------------------------------------------------
wxString CWKSP_Shapes_Manager::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Shapes"));

	s	+= "<table border=\"0\">";

	DESC_ADD_INT(_TL("Shape Layers"), Get_Items_Count());

	for(int i=0; i<Get_Count(); i++)
	{
		DESC_ADD_INT(Get_Item(i)->Get_Name().c_str(), ((CWKSP_Base_Manager *)Get_Item(i))->Get_Count());
	}

	s	+= wxT("</table>");

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Shapes_Manager::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(_TL("Shapes"));

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
CWKSP_Shapes_Type * CWKSP_Shapes_Manager::Get_Shapes_Type(TSG_Shape_Type Type)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Type == ((CWKSP_Shapes_Type *)Get_Item(i))->Get_Shape_Type() )
		{
			return( (CWKSP_Shapes_Type *)Get_Item(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Shapes_Type * CWKSP_Shapes_Manager::_Get_Shapes_Type(TSG_Shape_Type Type)
{
	CWKSP_Shapes_Type	*pItem	= NULL;

	if( Type != SHAPE_TYPE_Undefined )
	{
		if( (pItem = Get_Shapes_Type(Type)) == NULL )
		{
			Add_Item(pItem = new CWKSP_Shapes_Type(Type));
		}
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes * CWKSP_Shapes_Manager::Get_Data(CSG_Shapes *pObject)
{
	CWKSP_Shapes	*pItem	= NULL;

	for(int i=0; !pItem && i<Get_Count(); i++)
	{
		pItem	= ((CWKSP_Shapes_Type *)Get_Item(i))->Get_Data(pObject);
	}

	if( !pItem && SG_Get_Data_Manager().Exists(pObject) && _Get_Shapes_Type(pObject->Get_Type()) )
	{
		pItem	= _Get_Shapes_Type(pObject->Get_Type())->Add_Data(pObject);
	}

	return( pItem );
}

//---------------------------------------------------------
CWKSP_Shapes * CWKSP_Shapes_Manager::Add_Data(CSG_Shapes *pObject)
{
	CWKSP_Shapes	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL && pObject->is_Valid() )
	{
		pItem	= _Get_Shapes_Type(pObject->Get_Type())->Add_Data(pObject);
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
