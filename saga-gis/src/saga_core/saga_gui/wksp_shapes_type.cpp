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
//                 WKSP_Shapes_Type.cpp                  //
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

#include "wksp_shapes_type.h"

#include "wksp_shapes_point.h"
#include "wksp_shapes_points.h"
#include "wksp_shapes_line.h"
#include "wksp_shapes_polygon.h"



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Type::CWKSP_Shapes_Type(int Shapes_Type)
{
	m_Shapes_Type	= Shapes_Type;
}

//---------------------------------------------------------
CWKSP_Shapes_Type::~CWKSP_Shapes_Type(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes_Type::Get_Name(void)
{
	return( SG_Get_ShapeType_Name((TSG_Shape_Type)m_Shapes_Type) );
}

//---------------------------------------------------------
wxString CWKSP_Shapes_Type::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s:</b>%d<br>"), Get_Name().c_str(), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Shapes_Type::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Type::On_Command(int Cmd_ID)
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
CWKSP_Shapes * CWKSP_Shapes_Type::Get_Shapes(CSG_Shapes *pShapes)
{
	if( pShapes )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( pShapes == Get_Shapes(i)->Get_Shapes() )
			{
				return( Get_Shapes(i) );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Type::Exists(CSG_Shapes *pShapes)
{
	return( Get_Shapes(pShapes) != NULL );
}

//---------------------------------------------------------
CWKSP_Shapes * CWKSP_Shapes_Type::Add(CSG_Shapes *pShapes)
{
	CWKSP_Shapes	*pItem;

	if( pShapes && pShapes->Get_Type() == m_Shapes_Type && !Exists(pShapes) )
	{
		switch( pShapes->Get_Type() )
		{
		default:
			return( false );

		case SHAPE_TYPE_Point:
			pItem	= new CWKSP_Shapes_Point	(pShapes);
			break;

		case SHAPE_TYPE_Points:
			pItem	= new CWKSP_Shapes_Points	(pShapes);
			break;

		case SHAPE_TYPE_Line:
			pItem	= new CWKSP_Shapes_Line		(pShapes);
			break;

		case SHAPE_TYPE_Polygon:
			pItem	= new CWKSP_Shapes_Polygon	(pShapes);
			break;
		}

		if( Add_Item(pItem) )
		{
			return( pItem );
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
