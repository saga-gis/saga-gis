/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Color_Rotate.cpp                 //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Color_Rotate.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Rotate::CGrid_Color_Rotate(void)
{
	Set_Name		(_TL("Color Palette Rotation"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"The 'Color Palette Rotator' rotates the grids color palette. "
	));

	Parameters.Add_Grid(	NULL, "GRID"	, _TL("Grid")	, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Colors(	NULL, "COLORS"	, _TL("Colors")	, _TL(""));
	Parameters.Add_Value(	NULL, "DIR"		, _TL("Down")	, _TL(""), PARAMETER_TYPE_Bool, true);

}

//---------------------------------------------------------
CGrid_Color_Rotate::~CGrid_Color_Rotate(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Color_Rotate::On_Execute(void)
{
	bool	bDown;
	int		i;
	long	c;
	CSG_Grid	*pGrid;
	CSG_Colors	*pColors;

	pGrid	= Parameters("GRID")->asGrid();
	pColors	= Parameters("COLORS")->asColors();
	bDown	= Parameters("DIR")->asBool();

	if( pColors->Get_Count() > 1 )
	{
		do
		{
			if( bDown )
			{
				for(i=0, c=pColors->Get_Color(0); i<pColors->Get_Count() - 1; i++)
				{
					pColors->Set_Color(i, pColors->Get_Color(i + 1));
				}

				pColors->Set_Color(pColors->Get_Count() - 1, c);
			}
			else
			{
				for(i=pColors->Get_Count()-1, c=pColors->Get_Color(pColors->Get_Count()-1); i>0; i--)
				{
					pColors->Set_Color(i, pColors->Get_Color(i - 1));
				}

				pColors->Set_Color(0, c);
			}

			DataObject_Set_Colors(pGrid, *pColors);
			DataObject_Update(pGrid, true);
		}
		while( Process_Get_Okay(true) );

		return( true );
	}

	return( false );
}
