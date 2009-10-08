
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
//                University of Hamburg                  //
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
	Set_Name		(_TL("ViGrA - Edge Detection"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"References:\n"
		"ViGrA - Vision with Generic Algorithms\n"
		"<a target=\"_blank\" href=\"http://hci.iwr.uni-heidelberg.de/vigra\">http://hci.iwr.uni-heidelberg.de</a>"
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Detector type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Canny"),
			_TL("Shen-Castan")
		)
	);

	Parameters.Add_Value(
		NULL	, "SCALE"		, _TL("Operator scale"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Gradient threshold"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_Edges::On_Execute(void)
{
	int			Type;
	double		Scale, Threshold;
	CSG_Grid	*pInput, *pOutput;

	pInput		= Parameters("INPUT")		->asGrid();
	pOutput		= Parameters("OUTPUT")		->asGrid();
	Type		= Parameters("TYPE")		->asInt();
	Scale		= Parameters("SCALE")		->asDouble();
	Threshold	= Parameters("THRESHOLD")	->asDouble();

	//-----------------------------------------------------
	vigra::FImage	Input;
	vigra::BImage	Output(Get_NX(), Get_NY());

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	Output	= 0;
    
	switch( Type )
	{
	default:
	case 0:	// Canny
		cannyEdgeImage					(srcImageRange(Input), destImage(Output), Scale, Threshold, 1);
		break;

	case 1:	// Shen-Castan
		differenceOfExponentialEdgeImage(srcImageRange(Input), destImage(Output), Scale, Threshold, 1);
		break;
	}

	//-----------------------------------------------------
	Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);

	pOutput->Set_NoData_Value(0);

	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), Get_Name()));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
