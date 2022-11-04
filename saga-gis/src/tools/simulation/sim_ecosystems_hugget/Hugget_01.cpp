
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
#include "Hugget_01.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHugget_01::CHugget_01(void)
{
	Set_Name		(_TL("A Simple Litter System"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"A simple numerical litter system model demonstrating the usage of the Euler method. "
		"Litter carbon storage (C) is calculated in dependency of litter fall rate (Cinput) "
		"and rate constant for litter loss (Closs) as:\n"
		"<ul><li>C(t + 1) = C(t) + (Cinput - Closs * C(t)) * dt</li></ul>\n"
		"<h4>Typical values:</h4><table align=\"center\" frame=\"1\">"
		"<tr><th></th><th>Tropical Rainforest</th><th>Temperate forest</th><th>Boreal forest</th></tr>"
		"<tr><th>Litter fall rate [g/m<sup>2</sup>/yr]</th><td>500</td><td>240</td><td>50</td></tr>"
		"<tr><th>Litter loss rate [1/yr]</th><td>2.0</td><td>0.4</td><td>0.05</td></tr>"
		"</table>"
	));

	Add_Reference("Hugget, R.J.", "1993",
		"Modelling the Human Impact on Nature",
		"Oxford University Press."
	);

	//-----------------------------------------------------
	Parameters.Add_Table (""       , "TABLE"    , _TL("Litter"                       ), _TL(""         ), PARAMETER_OUTPUT);

	Parameters.Add_Double(""       , "TIME_SPAN", _TL("Time Span"                    ), _TL("[yrs]"    ),  50. , 0., true);
	Parameters.Add_Double(""       , "TIME_STEP", _TL("Time Interval"                ), _TL("[yrs]"    ),   0.1, 0., true);

	Parameters.Add_Double(""       , "C_INIT"   , _TL("Initial Litter Storage"       ), _TL("[g/m2]"   ),   0. , 0., true);

	Parameters.Add_Choice("",
		"PRESETS", _TL("Presets"), _TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Tropical forest"),
			_TL("Temperate forest"),
			_TL("Boreal forest"),
			_TL("Adjust parameters manually")
		), 3
	);

	Parameters.Add_Double("PRESETS", "C_INPUT"  , _TL("Litterfall Rate"              ), _TL("[g/m2/yr]"), 240. , 0., true);
	Parameters.Add_Double("PRESETS", "C_OUTPUT" , _TL("Rate Constant for Litter Loss"), _TL("[1/yr]"   ),   0.4, 0., true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHugget_01::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PRESETS") && pParameter->asInt() < 3 )
	{
		static const double Presets[3][2] = {
			{ 500., 2.   }, // Tropical forest
			{ 240., 0.4  }, // Temperate forest
			{  50., 0.05 }  // Boreal forest
		};

		const double *Preset = Presets[pParameter->asInt()];

		(*pParameters)["C_INPUT" ].Set_Value(Preset[0]);
		(*pParameters)["C_OUTPUT"].Set_Value(Preset[1]);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CHugget_01::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PRESETS") )
	{
		pParameter->Set_Children_Enabled(pParameter->asInt() >= 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHugget_01::On_Execute(void)
{
	//-----------------------------------------------------
	#define WRITE_RECORD { int i = pTable->Get_Count(); CSG_Table_Record *pRecord = pTable->Add_Record();\
		pRecord->Set_Value(0, i + 1    );\
		pRecord->Set_Value(1, i * dTime);\
		pRecord->Set_Value(2, C        );\
	}

	//-----------------------------------------------------
	CSG_Table *pTable = Parameters("TABLE")->asTable(); pTable->Destroy();
	pTable->Set_Name(_TL("Litter"));
	pTable->Add_Field("Step"  , SG_DATATYPE_Int   );
	pTable->Add_Field("Time"  , SG_DATATYPE_Double);
	pTable->Add_Field("Litter", SG_DATATYPE_Double);

	//-----------------------------------------------------
	double Time_Span = Parameters("TIME_SPAN")->asDouble();
	double dTime     = Parameters("TIME_STEP")->asDouble();

	double C         = Parameters("C_INIT"   )->asDouble();
	double C_Input   = Parameters("C_INPUT"  )->asDouble();
	double C_Loss    = Parameters("C_OUTPUT" )->asDouble(); if( C_Loss <= 0. ) { C_Loss = M_ALMOST_ZERO; }

	//-----------------------------------------------------
	if( dTime >= 1. / C_Loss )
	{
		if( !Error_Set(_TL("Time interval is to large for a stable solution.")) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	WRITE_RECORD;

	for(double Time=0.; Time<=Time_Span && Set_Progress(Time, Time_Span); Time+=dTime)
	{
		C += (C_Input - C_Loss * C) * dTime;

		WRITE_RECORD;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
