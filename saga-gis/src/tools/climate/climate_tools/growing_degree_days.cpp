
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
//                growing_degree_days.cpp                //
//                                                       //
//                 Copyright (C) 2017 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "climate_tools.h"

#include "growing_degree_days.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrowing_Degree_Days::CGrowing_Degree_Days(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Growing Degree Days"));

	Set_Author		("D.N. Karger (c) 2017");

	Set_Description(_TW(
		"This tool calculates growing degree days from daily or from spline interpolated monthly observations."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"TMEAN"	, _TL("Mean Monthly Temperatures"),
		_TL("Monthly (12) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("",
		"TBASE"	, _TL("Base Temperature"),
		_TL("Base temperature in degree Celsius"),
		0.0
	);

	Parameters.Add_Grid("",
		"NGDD"	, _TL("Number of Days above Base Temperature"),
		_TL("Number of days above base temperature."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"TSUM"	, _TL("Growing Degree Days"),
		_TL("Integral of daily temperature above base temperature."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"FIRST"	, _TL("First Growing Degree Day"),
		_TL("First growing degree day of the year (1-365)."),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"LAST"	, _TL("Last Growing Degree Day"),
		_TL("Last growing degree day of the year (1-365)."),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrowing_Degree_Days::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pTmean	= Parameters("TMEAN")->asGridList();

	if( pTmean->Get_Grid_Count() != 12 && pTmean->Get_Grid_Count() < 365 )
	{
		SG_UI_Msg_Add_Error(_TL("There has to be one input grid eiter for each month (12) or for each day of the year (365)."));

		return( false );
	}

	CSG_Grid	*pNGDD	= Parameters("NGDD" )->asGrid();
	CSG_Grid	*pTSum	= Parameters("TSUM" )->asGrid();
	CSG_Grid	*pFirst	= Parameters("FIRST")->asGrid();
	CSG_Grid	*pLast	= Parameters("LAST" )->asGrid();

	double	Tbase	= Parameters("TBASE")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Vector	T;

			//---------------------------------------------
			// 1. get the temperatures for all 365 days of the year

			if( pTmean->Get_Grid_Count() == 12 )
			{
				CSG_Vector	Tmonth(12);

				for(int i=0; i<Tmonth.Get_N(); i++)
				{
					if( pTmean->Get_Grid(i)->is_NoData(x, y) )
					{
						Tmonth.Destroy();
					}
					else
					{
						Tmonth[i] = pTmean->Get_Grid(i)->asDouble(x, y);
					}
				}

				if( Tmonth.Get_N() == 12 )
				{
					CT_Get_Daily_Splined(T, Tmonth);
				}
			}
			else // if( pTmean->Get_Count() >= 365 )
			{
				T.Create(365);

				for(int i=0; i<365; i++)
				{
					if( pTmean->Get_Grid(i)->is_NoData(x, y) )
					{
						T.Destroy();
					}
					else
					{
						T[i] = pTmean->Get_Grid(i)->asDouble(x, y);
					}
				}
			}

			//---------------------------------------------
			// 2. analyze the temperatures

			if( pFirst ) pFirst->Set_NoData(x, y);
			if( pLast  ) pLast ->Set_NoData(x, y);

			if( T.Get_N() < 365 )	// insufficient data !
			{
				pNGDD->Set_NoData(x, y);
				pTSum->Set_NoData(x, y);
			}
			else
			{
				T	-= Tbase;	// we only need temperatures relative to Tbase

				CSG_Simple_Statistics	Tgrow;

				for(int i=0; i<365; i++)
				{
					if( T[i] > 0.0 )	// it's a growing degree day !
					{
						Tgrow	+= T[i];

						if( pFirst && T[(365 + i - 1) % 365] <= 0.0 )	// is the previous day a not growing day ?
						{
							pFirst->Set_Value(x, y, 1 + i);
						}

						if( pLast  && T[(365 + i + 1) % 365] <= 0.0 )	// is the following day a not growing day ?
						{
							pLast ->Set_Value(x, y, 1 + i);	// in the end this will be the last growing degree day !
						}
					}
				}

				pNGDD->Set_Value(x, y, (double)Tgrow.Get_Count());

				if( Tgrow.Get_Count() > 0 )
				{
					pTSum->Set_Value(x, y, Tgrow.Get_Sum());
				}
				else
				{
					pTSum->Set_NoData(x, y);
				}
			}
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
