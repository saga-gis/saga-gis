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
//                    WKSP_Table.cpp                     //
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
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_base_control.h"

#include "wksp_table.h"

#include "view_table.h"
#include "view_table_diagram.h"
#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Table::CWKSP_Table(CSG_Table *pTable)
	: CWKSP_Data_Item(pTable)
{
	m_pView		= NULL;
	m_pDiagram	= NULL;

	//-----------------------------------------------------
	On_Create_Parameters();

	DataObject_Changed();
}

//---------------------------------------------------------
CWKSP_Table::~CWKSP_Table(void)
{
	Set_View	(false);
	Set_Diagram	(false);

	if( m_pObject->Get_ObjectType() != DATAOBJECT_TYPE_Table )
	{
		m_pObject	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Table::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b>"), _TL("Table"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR(_TL("Name")		, m_pObject->Get_Name());
	DESC_ADD_STR(_TL("Description")	, m_pObject->Get_Description());
	DESC_ADD_STR(_TL("File")		, SG_File_Exists(m_pObject->Get_File_Name()) ? m_pObject->Get_File_Name() : _TL("memory"));
	DESC_ADD_STR(_TL("Modified")	, m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_INT(_TL("Attributes")	, Get_Table()->Get_Field_Count());
	DESC_ADD_INT(_TL("Records")		, Get_Table()->Get_Record_Count());

	s	+= wxT("</table>");

	s	+= Get_TableInfo_asHTML(Get_Table());

	//-----------------------------------------------------
//	s	+= wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History"));
//	s	+= m_pTable->Get_History().Get_HTML();
//	s	+= wxString::Format(wxT("</font"));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Table::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(m_pObject->Get_Name());

	if( m_pObject->Get_ObjectType() == DATAOBJECT_TYPE_Table )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLES_SAVE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLES_SAVEAS);
	}

	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_TABLES_SHOW);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_TABLES_DIAGRAM);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLES_SCATTERPLOT);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Table::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Data_Item::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		Set_View(true);
		break;

	case ID_CMD_TABLES_SHOW:
		Toggle_View();
		break;

	case ID_CMD_TABLES_DIAGRAM:
		Toggle_Diagram();
		break;

	case ID_CMD_TABLES_SCATTERPLOT:
		Add_ScatterPlot(Get_Table());
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Table::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Data_Item::On_Command_UI(event) );

	case ID_CMD_TABLES_SHOW:
		event.Check(m_pView != NULL);
		break;

	case ID_CMD_TABLES_DIAGRAM:
		event.Check(m_pDiagram != NULL);
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
void CWKSP_Table::Set_View(bool bShow)
{
	if( bShow && !m_pView )
	{
		m_pView	= new CVIEW_Table(this);
	}
	else if( !bShow && m_pView )
	{
		m_pView->Destroy();

		delete(m_pView);
	}
}

//---------------------------------------------------------
void CWKSP_Table::Toggle_View(void)
{
	Set_View( m_pView == NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Table::Set_Diagram(bool bShow)
{
	if( bShow && !m_pDiagram )
	{
		m_pDiagram	= new CVIEW_Table_Diagram(this);
	}
	else if( !bShow && m_pDiagram )
	{
		m_pDiagram->Destroy();

		delete(m_pDiagram);
	}
}

//---------------------------------------------------------
void CWKSP_Table::Toggle_Diagram(void)
{
	Set_Diagram( m_pDiagram == NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Table::Show(int Flags)
{
	Set_View(true);

	return( true );
}

//---------------------------------------------------------
void CWKSP_Table::On_Update_Views(bool bAll)
{
	if( m_pView )
	{
		m_pView->Update_Table();
	}

	if( m_pDiagram )
	{
		m_pDiagram->Update_Diagram();
	}
}

//---------------------------------------------------------
bool CWKSP_Table::View_Closes(wxMDIChildFrame *pView)
{
	if		( wxDynamicCast(pView, CVIEW_Table) )
	{
		m_pView		= NULL;
	}
	else if	( wxDynamicCast(pView, CVIEW_Table_Diagram) )
	{
		m_pDiagram	= NULL;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
