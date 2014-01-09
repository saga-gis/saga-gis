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
CWKSP_Shapes_Type::CWKSP_Shapes_Type(TSG_Shape_Type Type)
{
	m_Shape_Type	= Type;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes_Type::Get_Name(void)
{
	return( SG_Get_ShapeType_Name(m_Shape_Type).c_str() );
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
	wxMenu	*pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes * CWKSP_Shapes_Type::Get_Data(CSG_Shapes *pObject)
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
CWKSP_Shapes * CWKSP_Shapes_Type::Add_Data(CSG_Shapes *pObject)
{
	CWKSP_Shapes	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL && pObject->Get_Type() == m_Shape_Type )
	{
		switch( pObject->Get_Type() )
		{
		default:																			break;
		case SHAPE_TYPE_Point:		Add_Item(pItem = new CWKSP_Shapes_Point  (pObject));	break;
		case SHAPE_TYPE_Points:		Add_Item(pItem = new CWKSP_Shapes_Points (pObject));	break;
		case SHAPE_TYPE_Line:		Add_Item(pItem = new CWKSP_Shapes_Line   (pObject));	break;
		case SHAPE_TYPE_Polygon:	Add_Item(pItem = new CWKSP_Shapes_Polygon(pObject));	break;
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
