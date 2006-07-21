
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Normalise.cpp                   //
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
#include "Grid_Normalise.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Normalise::CGrid_Normalise(void)
{
	Set_Name(_TL("Grid Normalisation"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Normalise the values of a grid. "
		"The arithmetic mean and the standard deviation is calculated based on "
		"all grid cell values to create a grid with normalised values. ")
	);

	Parameters.Add_Grid(
		NULL	, "INPUT"	,_TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Normalised Grid"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		"",
		CSG_String::Format("%s|%s|", _TL("Standard Deviation"), _TL("(0.0 < x < 1.0)"))
	);
}

//---------------------------------------------------------
CGrid_Normalise::~CGrid_Normalise(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Normalise::On_Execute(void)
{
	int		x, y;
	double	Minimum, Range;
	CGrid	*pInput, *pOutput;

	//-----------------------------------------------------
	pInput	= Parameters("INPUT")	->asGrid();
	pOutput	= Parameters("OUTPUT")	->asGrid();

	if( pInput != pOutput )
	{
		pOutput->Assign(pInput);
	}

	pOutput->Set_Name(CSG_String::Format("%s (%s)", pInput->Get_Name(), _TL("Grid_Normalised")));

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:
		pOutput->Normalise();
		break;

	case 1:
		if( (Minimum = pInput->Get_ZMin()) < (Range = pInput->Get_ZMax()) )
		{
			Range	= 1.0 / (Range - Minimum);

			for(y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					pOutput->Set_Value(x, y, (pInput->asDouble(x, y) - Minimum) * Range);
				}
			}
		}
		break;
	}

	return( true );
}
