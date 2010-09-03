
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
//                WKSP_Shapes_Points.cpp                 //
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
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_map_control.h"

#include "wksp_shapes_points.h"
#include "wksp_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Points::CWKSP_Shapes_Points(CSG_Shapes *pShapes)
	: CWKSP_Shapes_Point(pShapes)
{
	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Shapes_Points::~CWKSP_Shapes_Points(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Points::On_Create_Parameters(void)
{
	CWKSP_Shapes_Point::On_Create_Parameters();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Points::On_DataObject_Changed(void)
{
	CWKSP_Shapes_Point::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes_Points::On_Parameters_Changed(void)
{
	CWKSP_Shapes_Point::On_Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Points::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CWKSP_Shapes_Point::On_Parameter_Changed(pParameters, pParameter);

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Points::_Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, bool bSelection)
{
	if( (m_iSize >= 0 && pShape->is_NoData(m_iSize)) || (m_iColor >= 0 && pShape->is_NoData(m_iColor)) )
		return;

	//-----------------------------------------------------
	int		Size;

	if( CWKSP_Shapes_Point::_Draw_Initialize(dc_Map, Size, pShape, bSelection) )
	{
		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point_Int	p	= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

				Draw_Symbol(dc_Map.dc, p.x, p.y, Size);
			}
		}

		//-------------------------------------------------
		if( bSelection )
		{
			CWKSP_Shapes_Point::_Draw_Initialize(dc_Map);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Points::_Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)
{
	TSG_Point_Int	p(dc_Map.World2DC(pShape->Get_Extent().Get_Center()));

	dc_Map.dc.DrawText(pShape->asString(m_iLabel, m_Label_Prec), p.x, p.y);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
