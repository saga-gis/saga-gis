
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
//                  Grid_Colors_Fit.cpp                  //
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
#include "Grid_Colors_Fit.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Colors_Fit::CGrid_Colors_Fit(void)
{
	Set_Name(_TL("Fit Color Palette to Grid Values"));

	Set_Author(_TL("Copyrights (c) 2005 by Olaf Conrad"));

	Set_Description(
		""
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "COUNT"	, _TL("Number of Colors"),
		"",
		PARAMETER_TYPE_Int	, 100, 2, true
	);

	Parameters.Add_Choice(
		NULL	, "SCALE"	, _TL("Scale"),
		"",

		CAPI_String::Format("%s|%s|",
			_TL("Grid range"),
			_TL("User defined range")
		), 0
	);

	Parameters.Add_Range(
		NULL	, "RANGE"	, _TL("User defined range"),
		"",
		0.0, 1.0
	);
}

//---------------------------------------------------------
CGrid_Colors_Fit::~CGrid_Colors_Fit(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Colors_Fit::On_Execute(void)
{
	int		iColor;
	long	aC, bC;
	double	aZ, bZ, dColor, zMin, zRange;
	CColors	Colors_Old, Colors_New;
	CGrid	*pGrid;

	pGrid	= Parameters("GRID")->asGrid();

	Colors_New.Set_Count(Parameters("COUNT")->asInt());

	switch( Parameters("SCALE")->asInt() )
	{
	case 0:	default:
		zMin	= pGrid->Get_ZMin();
		zRange	= pGrid->Get_ZMax() - zMin;
		break;

	case 1:
		zMin	= Parameters("RANGE")->asRange()->Get_LoVal();
		zRange	= Parameters("RANGE")->asRange()->Get_HiVal() - zMin;
		break;
	}

	DataObject_Get_Colors(pGrid, Colors_Old);

	if( Colors_Old.Get_Count() > 1 && pGrid->Get_ZRange() > 0.0 && zRange != 0.0 )
	{
		dColor	= 100.0 / Colors_Old.Get_Count();

		aZ		= 0.0;
		aC		= Colors_Old.Get_Color(0);

		for(iColor=1; iColor<Colors_Old.Get_Count()-1; iColor++)
		{
			bZ	= aZ;
			bC	= aC;
			aZ	= (pGrid->Get_Percentile(100.0 - iColor * dColor, false) - zMin) / zRange;
			aC	= Colors_Old.Get_Color(iColor);
			_Set_Colors(Colors_New, bZ, bC, aZ, aC);
		}

		bZ	= aZ;
		bC	= aC;
		aZ	= 1.0;
		aC	= Colors_Old.Get_Color(Colors_Old.Get_Count() - 1);
		_Set_Colors(Colors_New, bZ, bC, aZ, aC);

		DataObject_Set_Colors	(pGrid, Colors_New);
		DataObject_Update		(pGrid, zMin, zMin + zRange);

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
void CGrid_Colors_Fit::_Set_Colors(CColors &Colors, double pos_a, long color_a, double pos_b, long color_b)
{
	int		a, b;

	a	= (int)(pos_a * Colors.Get_Count());	if( a < 0 )	a	= 0;	else if( a >= Colors.Get_Count() )	a	= Colors.Get_Count() - 1;
	b	= (int)(pos_b * Colors.Get_Count());	if( b < 0 )	b	= 0;	else if( b >= Colors.Get_Count() )	b	= Colors.Get_Count() - 1;

	Colors.Set_Ramp(color_a, color_b, a, b);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
