
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        VIGRA                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  vigra_distance.cpp                   //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "vigra_distance.h"

//---------------------------------------------------------
#include <vigra/distancetransform.hxx>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_Distance::CViGrA_Distance(void)
{
	Set_Name		(_TL("Distance (ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Distance to feature cells on a raster. Feature cells are all cells not representing a no-data value."
	));

	Add_Reference("http://ukoethe.github.io/vigra/", SG_T("ViGrA - Vision with Generic Algorithms"));

	Parameters.Add_Grid(
		"", "INPUT"	, _TL("Features"),
		_TL("Features are all pixels not representing a no-data value."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Distance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "NORM"	, _TL("Type of distance calculation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Chessboard"),
			_TL("Manhattan"),
			_TL("Euclidean")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_Distance::On_Execute(void)
{
	CSG_Grid	*pInput  = Parameters("INPUT" )->asGrid();
	CSG_Grid	*pOutput = Parameters("OUTPUT")->asGrid();

	//-----------------------------------------------------
	vigra::FImage	Input, Output(Get_NX(), Get_NY());

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	distanceTransform(srcImageRange(Input), destImage(Output), pInput->Get_NoData_Value(), Parameters("NORM")->asInt());

	Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);

	//-----------------------------------------------------
	pOutput->Multiply(pOutput->Get_Cellsize());

	pOutput->Fmt_Name("%s [%s - %s]", pInput->Get_Name(), Get_Name().c_str(), Parameters("NORM")->asString());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
