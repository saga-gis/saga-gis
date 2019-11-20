
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              temperature_lapserates.cpp               //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                  Dirk Nikolaus Karger                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version >=2 of the License. //
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
//    e-mail:     dirk.karger@wsl.ch                     //
//                                                       //
//    contact:    D.N. Karger                            //
//                Swiss Federal Research Institute WSL   //
//                Zürcherstrasse 111                     //
//                8930 Birmensdorf                       //
//                Switzerland                            //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "temperature_lapse_rates.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTemperature_Lapse_Rates::CTemperature_Lapse_Rates(void)
{
	Set_Name		(_TL("Temperature Lapse Rates"));

	Set_Author		("D.N. Karger (c) 2019");

	Set_Description	(_TW(
		"This tool selects daily temperature lapse rates for minimum and maximum temperatures "
		"from hourly lapse rates by selecting the time at which minimum or maximum temperatures occured "
		"and then returns the respective lapse rate."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"TEMP"		, _TL("Atmospheric Lapse Rates"),
		_TL("grid stack of hourly atmospheric lapse rates"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"TGROUND"	, _TL("Surface Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"LAPSE"		, _TL("Temperature Lapse Rate at Extreme"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"TEXTREME"	, _TL("Daily Extreme Temperature"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"TIME"		, _TL("Hour of Daily Extreme Temperature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"EXTREME"	, _TL("Temperature extreme"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("minimum"),
			_TL("maximum")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTemperature_Lapse_Rates::On_Execute(void)
{
	CSG_Parameter_Grid_List *pTlapse  = Parameters("TEMP"   )->asGridList();
	CSG_Parameter_Grid_List	*pTground = Parameters("TGROUND")->asGridList();

	if( pTlapse->Get_Grid_Count() < 24. )
	{
		Error_Set(_TL("lapse rates grid list provides less than 24 grids"));
	}

	if( pTlapse->Get_Grid_Count() > pTground->Get_Grid_Count() )
	{
		Error_Set(_TL("surface temperatures grid list provides less than 24 grids"));
	}

	CSG_Grid	*pLapse    = Parameters("LAPSE"   )->asGrid();
	CSG_Grid	*pTExtreme = Parameters("TEXTREME")->asGrid();
	CSG_Grid	*pTime     = Parameters("TIME"    )->asGrid();

	bool	bMinimum	= Parameters("EXTREME")->asInt() == 0;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			int		iExtreme	= 0;
			double	TExtreme	= pTlapse->Get_Grid(0)->asDouble(x, y);

			for(int i=1; i<pTlapse->Get_Grid_Count(); i++)
			{
				double	T	= pTlapse->Get_Grid(i)->asDouble(x, y);

				if( bMinimum )
				{
					if( TExtreme > T )
					{
						TExtreme = T; iExtreme = i;
					}
				}
				else
				{
					if( TExtreme < T )
					{
						TExtreme = T; iExtreme = i;
					}
				}
			}

			if( pLapse    ) pLapse   ->Set_Value(x, y, TExtreme);
			if( pTExtreme ) pTExtreme->Set_Value(x, y, pTground->Get_Grid(iExtreme)->asDouble(x, y));
			if( pTime     ) pTime    ->Set_Value(x, y, iExtreme);
		}
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
