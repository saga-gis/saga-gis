/**********************************************************
 * Version $Id: vigra_morphology.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                  vigra_morphology.cpp                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "vigra_morphology.h"

//---------------------------------------------------------
#include <vigra/flatmorphology.hxx>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_Morphology::CViGrA_Morphology(void)
{
	Set_Name		(_TL("Morphological Filter (ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"References:\n"
		"ViGrA - Vision with Generic Algorithms\n"
		"<a target=\"_blank\" href=\"http://hci.iwr.uni-heidelberg.de/vigra\">http://hci.iwr.uni-heidelberg.de</a>"
	));

	Parameters.Add_Grid("",
		"INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT, SG_DATATYPE_Byte
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Operation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Dilation"),
			_TL("Erosion"),
			_TL("Median"),
			_TL("User defined rank")
		)
	);

	Parameters.Add_Int("",
		"RADIUS"	, _TL("Radius (cells)"),
		_TL(""),
		1, 0, true
	);

	Parameters.Add_Double("",
		"RANK"		, _TL("User defined rank"),
		_TL(""),
		0.5, 0.0, true, 1.0, true
	);

	Parameters.Add_Bool("",
		"RESCALE"	, _TL("Rescale Values (0-255)"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_Morphology::On_Execute(void)
{
	bool		bRescale;
	int			Type, Radius;
	double		Rank;
	CSG_Grid	*pInput, *pOutput, Rescaled;

	pInput		= Parameters("INPUT"  )->asGrid();
	pOutput		= Parameters("OUTPUT" )->asGrid();
	Type		= Parameters("TYPE"   )->asInt();
	Radius		= Parameters("RADIUS" )->asInt();
	Rank		= Parameters("RANK"   )->asDouble();
	bRescale	= Parameters("RESCALE")->asBool();

	//-----------------------------------------------------
	if( bRescale )
	{
		Rescaled.Create(Get_System(), SG_DATATYPE_Byte);

		for(sLong i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			Rescaled.Set_Value(i, 0.5 + (pInput->asDouble(i) - pInput->Get_Min()) * 255.0 / pInput->Get_Range());
		}

		pInput	= &Rescaled;
	}

	//-----------------------------------------------------
	vigra::BImage	Input, Output(Get_NX(), Get_NY());

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	switch( Type )
	{
	case 0:	// Dilation
		discDilation		(srcImageRange(Input), destImage(Output), Radius);
		break;

	case 1:	// Erosion
		discErosion			(srcImageRange(Input), destImage(Output), Radius);
		break;

	case 2:	// Median
		discMedian			(srcImageRange(Input), destImage(Output), Radius);
		break;

	case 3:	// User defined rank
		discRankOrderFilter	(srcImageRange(Input), destImage(Output), Radius, Rank);
		break;
	}

	//-----------------------------------------------------
	Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);

	pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), Get_Name().c_str());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
