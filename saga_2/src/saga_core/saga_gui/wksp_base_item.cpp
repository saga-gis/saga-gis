
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
//                  WKSP_Base_Item.cpp                   //
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

#include "active.h"

#include "wksp_base_item.h"
#include "wksp_base_manager.h"

#include "wksp_module_control.h"
#include "wksp_data_control.h"
#include "wksp_map_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item *	Get_Active_Item(void)
{
	return( g_pACTIVE ? g_pACTIVE->Get_Item() : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item::CWKSP_Base_Item(void)
{
	m_bManager	= false;
	m_pManager	= NULL;
	m_ID		= 0;
}

//---------------------------------------------------------
CWKSP_Base_Item::~CWKSP_Base_Item(void)
{
	if( g_pACTIVE && g_pACTIVE->Get_Item() == this )
	{
		g_pACTIVE->Set_Active(NULL);
	}

	if( m_pManager )
	{
		m_pManager->Del_Item(this);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Base_Item::Get_Index(void)
{
	int		iItem;

	if( m_pManager )
	{
		for(iItem=0; iItem<m_pManager->Get_Count(); iItem++)
		{
			if( this == m_pManager->Get_Item(iItem) )
			{
				return( iItem );
			}
		}
	}

	return( -1 );
}

//---------------------------------------------------------
CWKSP_Base_Control * CWKSP_Base_Item::Get_Control(void)
{
	switch( Get_Type() )
	{
    default:
        return( NULL );

	case WKSP_ITEM_Module_Manager:
	case WKSP_ITEM_Module_Library:
	case WKSP_ITEM_Module:
		return( g_pModule_Ctrl );

	case WKSP_ITEM_Data_Manager:
	case WKSP_ITEM_Table_Manager:
	case WKSP_ITEM_Table:
	case WKSP_ITEM_Shapes_Manager:
	case WKSP_ITEM_Shapes_Type:
	case WKSP_ITEM_Shapes:
	case WKSP_ITEM_TIN_Manager:
	case WKSP_ITEM_TIN:
	case WKSP_ITEM_Grid_Manager:
	case WKSP_ITEM_Grid_System:
	case WKSP_ITEM_Grid:
		return( g_pData_Ctrl );

	case WKSP_ITEM_Map_Manager:
	case WKSP_ITEM_Map:
	case WKSP_ITEM_Map_Layer:
		return( g_pMap_Ctrl );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
void print_parameter(FILE *Stream, CSG_Parameter *p, int Level)
{
	if( Level > 0 || !p->Get_Parent() )
	{
		int		i;

		for(i=0; i<Level; i++)
			fprintf(Stream, ">");

		fprintf(Stream, "%s\t%s\n", p->Get_Name(), p->Get_Description(PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_TEXT|PARAMETER_DESCRIPTION_PROPERTIES, "\t").c_str());

		for(i=0; i<p->Get_Children_Count(); i++)
			print_parameter(Stream, p->Get_Child(i), Level + 1);
	}
}

#include "res_dialogs.h"

bool CWKSP_Base_Item::On_Command(int Cmd_ID)
{
	FILE		*Stream;
	CSG_Parameters	*P;
	wxString	File_Path;

	if( (P = Get_Parameters()) != NULL && Cmd_ID == WXK_F9 && DLG_Save(File_Path, "Write Parameters", "Text Files (*.txt)|*.txt|All Files|*.*") && (Stream = fopen(File_Path, "w")) != NULL )
	{
		for(int i=0; i<P->Get_Count(); i++)
			print_parameter(Stream, P->Get_Parameter(i), 0);

		fclose(Stream);
	}

	return( false );
}/**/

//---------------------------------------------------------
bool CWKSP_Base_Item::On_Command(int Cmd_ID)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Item::On_Command_UI(wxUpdateUIEvent &event)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Base_Item::Parameters_Changed(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
