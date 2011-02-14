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
	Set_Name		(_TL("ViGrA - Distance"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"References:\n"
		"ViGrA - Vision with Generic Algorithms\n"
		"<a target=\"_blank\" href=\"http://hci.iwr.uni-heidelberg.de/vigra\">http://hci.iwr.uni-heidelberg.de</a>"
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"	, _TL("Features"),
		_TL("Features are all pixels different not representing no-data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Distance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "NORM"	, _TL("Type of distance calculation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Chessboard"),
			_TL("Manhattan"),
			_TL("Euclidean")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_Distance::On_Execute(void)
{
	int			Norm;
	CSG_Grid	*pInput, *pOutput;

	pInput	= Parameters("INPUT")	->asGrid();
	pOutput	= Parameters("OUTPUT")	->asGrid();
	Norm	= Parameters("NORM")	->asInt();

	//-----------------------------------------------------
	vigra::FImage	Input, Output(Get_NX(), Get_NY());

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	distanceTransform(srcImageRange(Input), destImage(Output), pInput->Get_NoData_Value(), Norm);

	Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);

	//-----------------------------------------------------
	pOutput->Multiply(pOutput->Get_Cellsize());

	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s - %s]"), pInput->Get_Name(), Get_Name(), Parameters("NORM")->asString()));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
