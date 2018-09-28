///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Tool Library                       //
//                     ta_lighting                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                SolarRadiationYear.cpp                 //
//                                                       //
//                Copyright (C) 2018 by                  //
//                     Olaf Conrad                       //
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
#include "SolarRadiationYear.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSolarRadiationYear::CSolarRadiationYear(void)
{
	Set_Name		(_TL("Potential Annual Insolation"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description(_TW(
		"Calculates the annual potential total insolation for given time steps "
		"and stores resulting time series in a grid collection. "
	));

	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grids("",
		"INSOLATION", _TL("Annual Insolation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int("",
		"STEPS"		, _TL("Number of Steps"),
		_TL("Number of time steps per year."),
		14, 2, true, 365, true
	);

	Parameters.Add_Choice("",
		"UNITS"		, _TL("Units"),
		_TL("Units for output radiation values."),
		CSG_String::Format("%s|%s|%s|",
			SG_T("kWh / m2"),
			SG_T("kJ / m2"),
			SG_T("J / cm2")
		), 0
	);

	Parameters.Add_Double("PERIOD",
		"HOUR_STEP"	, _TL("Resolution [h]"),
		_TL("Time step size for a day's calculation given in hours."),
		0.5, 0.0, true, 24.0, true
	);

	Parameters.Add_Int("",
		"YEAR"		, _TL("Reference Year"),
		_TL(""),
		2000
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiationYear::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();

	CSG_Grids	*pGrids	= Parameters("INSOLATION")->asGrids();

	pGrids->Create(Get_System());

	pGrids->Set_Name(_TL("Annual Insolation"));

	pGrids->Add_Attribute("ID"       , SG_DATATYPE_Short);
	pGrids->Add_Attribute("DayOfYear", SG_DATATYPE_Short);
	pGrids->Add_Attribute("Date"     , SG_DATATYPE_Date );

	//-----------------------------------------------------
	CSG_DateTime	Date(1, CSG_DateTime::Jan, Parameters("YEAR")->asInt());

	int		nSteps	= Parameters("STEPS")->asInt();
	double	dDays	= (Date.Get_NumberOfDays(Date.Get_Year()) - 1) / (double)nSteps;
	double	Day		= Date.Get_JDN();

	CSG_Grid	Direct(Get_System()), Diffus(Get_System());

	//-----------------------------------------------------
	for(int iStep=0; iStep<=nSteps && Process_Get_Okay(); iStep++, Day+=dDays)
	{
		Date.Set(Day);

		CSG_Grid	*pTotal	= SG_Create_Grid(Get_System()); bool bResult;

		SG_RUN_TOOL(bResult, "ta_lighting", 2,
			    SG_TOOL_PARAMETER_SET("GRD_DEM"   , pDEM   )
			&&  SG_TOOL_PARAMETER_SET("GRD_DIRECT", &Direct)
			&&  SG_TOOL_PARAMETER_SET("GRD_DIFFUS", &Diffus)
			&&  SG_TOOL_PARAMETER_SET("GRD_TOTAL" , pTotal )
			&&  SG_TOOL_PARAMETER_SET("DAY"       , Day    )
			&&  SG_TOOL_PARAMETER_SET("HOUR_STEP" , Parameters("HOUR_STEP"))
			&&  SG_TOOL_PARAMETER_SET("UNITS"     , Parameters("UNITS"    ))
		)

		if( !bResult )
		{
			delete(pTotal);

			return( false );
		}

		pGrids->Add_Grid(Day, pTotal, true);

		pGrids->Get_Attributes(iStep).Set_Value("ID"       , 1 + iStep);
		pGrids->Get_Attributes(iStep).Set_Value("DayOfYear", Date.Get_DayOfYear());
		pGrids->Get_Attributes(iStep).Set_Value("Date"     , Date.Format_ISODate());
	}

	//-----------------------------------------------------
	pGrids->Get_Attributes_Ptr()->Set_Field_Name(0, _TL("JDN"));

	pGrids->Set_Z_Attribute (2);
	pGrids->Set_Z_Name_Field(3);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
