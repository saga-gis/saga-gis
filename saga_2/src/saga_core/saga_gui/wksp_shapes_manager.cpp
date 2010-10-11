
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

	s.Printf(wxT("<b>%s</b><br>"), LNG("[CAP] Shapes"));

	for(int i=0; i<Get_Count(); i++)
	{
		s.Append(wxString::Format(wxT("%s: %d<br>"), Get_Item(i)->Get_Name().c_str(), ((CWKSP_Base_Manager *)Get_Item(i))->Get_Count()));
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
#include <wx/filename.h>

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module.h"

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Open_OGR(const wxChar *File_Name)
{
	CSG_Module	*pImport	= NULL;

	for(int i=0; i<g_pModules->Get_Count() && !pImport; i++)
	{
		wxFileName	fName(g_pModules->Get_Library(i)->Get_File_Name());

		if( !fName.GetName().Cmp(SG_T("io_gdal")) )
		{
			pImport	= g_pModules->Get_Library(i)->Get_Module(3)->Get_Module();	// OGR_Import
		}
	}

	SG_UI_Progress_Lock(true);

	if( !pImport
	||	!pImport->Get_Parameters()->Set_Parameter(SG_T("FILES"), PARAMETER_TYPE_FilePath, File_Name)
	||	!pImport->Execute() )
	{
		SG_UI_Progress_Lock(false);

		return( false );
	}

	SG_UI_Progress_Lock(false);

	CSG_Parameter_Shapes_List	*pShapes	= pImport->Get_Parameters()->Get_Parameter(SG_T("SHAPES"))->asShapesList();

	for(int i=0; i<pShapes->Get_Count(); i++)
	{
		SG_UI_DataObject_Add(pShapes->asShapes(i), SG_UI_DATAOBJECT_UPDATE_ONLY);
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
CWKSP_Shapes * CWKSP_Shapes_Manager::Get_Shapes(CSG_Shapes *pShapes)
{
	if( pShapes )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( ((CWKSP_Shapes_Type *)Get_Item(i))->Exists(pShapes) )
			{
				return( ((CWKSP_Shapes_Type *)Get_Item(i))->Get_Shapes(pShapes) );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Exists(CSG_Shapes *pShapes)
{
	return( Get_Shapes(pShapes) != NULL );
}

//---------------------------------------------------------
CWKSP_Shapes * CWKSP_Shapes_Manager::Add(CSG_Shapes *pShapes)
{
	if( pShapes && pShapes->is_Valid() && !Exists(pShapes) )
	{
		return( _Get_Shapes_Type(pShapes->Get_Type())->Add(pShapes) );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Shapes * CWKSP_Shapes_Manager::Get_byFileName(const wxChar *File_Name)
{
	CSG_String	s(File_Name);
	CWKSP_Shapes_Type	*pType;

	for(int i=0; i<Get_Count(); i++)
	{
		pType	= (CWKSP_Shapes_Type *)Get_Item(i);

		for(int j=0; j<pType->Get_Count(); j++)
		{
			if( !s.Cmp(pType->Get_Shapes(j)->Get_Shapes()->Get_File_Name()) )
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
bool CWKSP_Shapes_Manager::Update(CSG_Shapes *pShapes, CSG_Parameters *pParameters)
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
bool CWKSP_Shapes_Manager::Update_Views(CSG_Shapes *pShapes)
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
bool CWKSP_Shapes_Manager::Show(CSG_Shapes *pShapes, int Map_Mode)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
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
bool CWKSP_Shapes_Manager::asImage(CSG_Shapes *pShapes, CSG_Grid *pImage)
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
bool CWKSP_Shapes_Manager::Get_Colors(CSG_Shapes *pShapes, CSG_Colors *pColors)
{
	CWKSP_Shapes	*pItem;

	if( (pItem = Get_Shapes(pShapes)) != NULL )
	{
		return( pItem->Get_Colors(pColors) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Manager::Set_Colors(CSG_Shapes *pShapes, CSG_Colors *pColors)
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
