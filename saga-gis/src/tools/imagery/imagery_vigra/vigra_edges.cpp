
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
//                    vigra_edges.cpp                    //
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
#include "vigra_edges.h"

//---------------------------------------------------------
#include <vigra/edgedetection.hxx>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_Edges::CViGrA_Edges(void)
{
	Set_Name		(_TL("Edge Detection (ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Edge detection."
	));

	Add_Reference("http://ukoethe.github.io/vigra/", SG_T("ViGrA - Vision with Generic Algorithms"));

	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "TYPE"		, _TL("Detector type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Canny"),
			_TL("Shen-Castan")
		)
	);

	Parameters.Add_Double(
		"", "SCALE"		, _TL("Operator scale"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "THRESHOLD"	, _TL("Gradient threshold"),
		_TL(""),
		1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_Edges::On_Execute(void)
{
	//-----------------------------------------------------
	vigra::FImage	Input;	vigra::BImage	Output(Get_NX(), Get_NY());

	CSG_Grid	*pInput	= Parameters("INPUT")->asGrid();

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	Output	= 0;
    
	//-----------------------------------------------------
	switch( Parameters("TYPE")->asInt() )
	{
	default:	// Canny
		cannyEdgeImage(
			srcImageRange(Input), destImage(Output), Parameters("SCALE")->asDouble(), Parameters("THRESHOLD")->asDouble(), 1
		);
		break;

	case  1:	// Shen-Castan
		differenceOfExponentialEdgeImage(
			srcImageRange(Input), destImage(Output), Parameters("SCALE")->asDouble(), Parameters("THRESHOLD")->asDouble(), 1
		);
		break;
	}

	//-----------------------------------------------------
	CSG_Grid	*pOutput	= Parameters("OUTPUT")->asGrid();

	Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);

	pOutput->Set_NoData_Value(0);

	pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), Get_Name().c_str());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
