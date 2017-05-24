/**********************************************************
 * Version $Id: Hugget_01.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 sim_ecosystems_hugget                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Hugget_01.cpp                      //
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
#include "Hugget_01.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHugget_01::CHugget_01(void)
{
	Set_Name	(_TL("01: A Simple Litter System"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"A simple litter system model using the euler method. "
		"Carbon storage C is calculated in dependency of litter fall rate (Cinput) "
		"and rate constant for litter loss (Closs) as:\n"
		"C(t + 1) = C(t) + (Cinput - Closs * C(t)) * dt\n"

		"\nTypical values:\n"
		"\n- Tropical Rainforest:\n"
		"-- Litter fall rate: 500 [g/m<sup>2</sup>/a]\n"
		"-- Litter loss rate: 2.0 [1/a]\n"

		"\n- Temperate forest:\n"
		"-- Litter fall rate: 240 [g/m<sup>2</sup>/a]\n"
		"-- Litter loss rate: 0.4 [1/a]\n"

		"\n- Boreal forest:\n"
		"-- Litter fall rate: 50 [g/m<sup>2</sup>/a]\n"
		"-- Litter loss rate: 0.05 [1/a]\n"

		"\nReference:"
		"\nHugget, R.J. (1993): 'Modelling the Human Impact on Nature', Oxford University Press.\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Results"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "TIME_SPAN"	, _TL("Time Span"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "TIME_STEP"	, _TL("Time Interval [a]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.1, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "C_INIT"		, _TL("Initial Litter Storage [g/m\xb2]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "C_INPUT"		, _TL("Litterfall Rate [g/m\xb2/a]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 240.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "C_OUTPUT"	, _TL("Rate Constant for Litter Loss [1/a]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.4, 0.0, true
	);
}

//---------------------------------------------------------
CHugget_01::~CHugget_01(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define WRITE_RECORD	pRecord	= pTable->Add_Record();\
						pRecord->Set_Value(0, iStep);\
						pRecord->Set_Value(1, iStep * dTime);\
						pRecord->Set_Value(2, C);

//---------------------------------------------------------
bool CHugget_01::On_Execute(void)
{
	int				iStep, nSteps;
	double			sTime, dTime, C, C_Input, C_Loss;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;

	//-----------------------------------------------------
	sTime	= Parameters("TIME_SPAN")	->asDouble();
	dTime	= Parameters("TIME_STEP")	->asDouble();
	nSteps	= (int)(sTime / dTime);

	C		= Parameters("C_INIT")		->asDouble();
	C_Input	= Parameters("C_INPUT")		->asDouble();
	C_Loss	= Parameters("C_OUTPUT")	->asDouble();

	pTable	= Parameters("TABLE")		->asTable();
	pTable->Destroy();
	pTable->Set_Name(_TL("A Simple Litter System"));
	pTable->Add_Field("STEP"	, SG_DATATYPE_Int);
	pTable->Add_Field("TIME"	, SG_DATATYPE_Double);
	pTable->Add_Field("CARBON"	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	if( C_Loss <= 0.0 )
	{
		C_Loss	= M_ALMOST_ZERO;
	}

	if( dTime >= 1.0 / C_Loss )
	{
		if( !Error_Set(_TL("Time interval is to large for a stable solution.")) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	for(iStep=0; iStep<nSteps && Set_Progress(iStep, nSteps); iStep++)
	{
		WRITE_RECORD;

		C	= C + (C_Input - C_Loss * C) * dTime;
	}

	WRITE_RECORD;

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
