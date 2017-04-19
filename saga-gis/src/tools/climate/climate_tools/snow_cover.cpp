
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    snow_cover.cpp                     //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                      Olaf Conrad                      //
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
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "snow_cover.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSnow_Cover::CSnow_Cover(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Snow Cover"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"The 'Snow Cover' tool uses a simple model to estimate snow cover "
		"statistics from climate data. When temperature falls below zero "
		"any precipitation is accumulated as snow. Temperatures above zero "
		"will diminish accumulated snow successively until it is gone completely. "
		"Simulation is done on a daily basis. If you supply the tool with "
		"monthly averages, temperatures will be interpolated using a spline "
		"and precipitation will be splitted into separate events. The latter "
		"is done with respect to the monthly mean temperature, i.e. the higher "
		"the temperature the more concentrated are precipitation events and vice "
		"versa. "
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential snow_cover position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "T", _TL("Mean Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "P", _TL("Precipitation"   ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "DAYS"    , _TL("Snow Cover Days"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "MEAN"    , _TL("Average"        ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "MAXIMUM" , _TL("Maximum"        ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "QUANTILE", _TL("Quantile"       ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	//-----------------------------------------------------
	Parameters.Add_Double("QUANTILE",
		"QUANT_VAL"	, _TL("Value"),
		_TL(""),
		50.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice("",
		"TIME"		, _TL("Time"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("Year"),
			_TL("January - March"),
			_TL("April - June"),
			_TL("July - September"),
			_TL("October - December"),
			_TL("Single Month")
		)
	);

	Parameters.Add_Choice("TIME",
		"MONTH"		, _TL("Month"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("January"),
			_TL("February"),
			_TL("March"),
			_TL("April"),
			_TL("May"),
			_TL("June"),
			_TL("July"),
			_TL("August"),
			_TL("September"),
			_TL("October"),
			_TL("November"),
			_TL("December")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSnow_Cover::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "QUANTILE") )
	{
		pParameters->Set_Enabled("QUANT_VAL", pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TIME") )
	{
		pParameters->Set_Enabled("MONTH", pParameter->asInt() == 5);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSnow_Cover::On_Execute(void)
{
	//-----------------------------------------------------
	m_pT	= Parameters("T")->asGridList();
	m_pP	= Parameters("P")->asGridList();

	if( (m_pT->Get_Count() != 12 && m_pT->Get_Count() < 365)
	||  (m_pP->Get_Count() != 12 && m_pP->Get_Count() < 365) || m_pT->Get_Count() != m_pP->Get_Count() )
	{
		Error_Set(_TL("Input has to be provided on a monthly (12) or daily (365) basis."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pDays		= Parameters("DAYS"    )->asGrid();
	CSG_Grid	*pMean		= Parameters("MEAN"    )->asGrid();
	CSG_Grid	*pMaximum	= Parameters("MAXIMUM" )->asGrid();
	CSG_Grid	*pQuantile	= Parameters("QUANTILE")->asGrid();

	//-----------------------------------------------------
	CSG_Colors	Colors;

	Colors.Set_Palette(SG_COLORS_RAINBOW   ,  true, 11); Colors.Set_Color(0, 200, 200, 200);

	DataObject_Set_Colors(pDays    , Colors);

	Colors.Set_Palette(SG_COLORS_WHITE_BLUE, false, 11); Colors.Set_Color(0, 233, 244, 255);

	DataObject_Set_Colors(pMean    , Colors);
	DataObject_Set_Colors(pMaximum , Colors);
	DataObject_Set_Colors(pQuantile, Colors);

	double	Quantile	= Parameters("QUANT_VAL")->asDouble();

	//-----------------------------------------------------
	const int Month[]	= {
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC YEAR
		 0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334, 365
	};

	int	Days[2];

	switch( Parameters("TIME")->asInt() )
	{
	default: Days[0] =  1; Days[1] = 12; break; // Year
	case  1: Days[0] =  1; Days[1] =  3; break; // January - March
	case  2: Days[0] =  4; Days[1] =  6; break; // April - June
	case  3: Days[0] =  7; Days[1] =  9; break; // July - September
	case  4: Days[0] = 10; Days[1] = 12; break; // October - December
	case  5: Days[0] =     Days[1] =
		Parameters("MONTH")->asInt() + 1; break; // Month
	}

	Days[0] = Month[Days[0] - 1];
	Days[1] = Month[Days[1]    ];

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
#ifndef _DEBUG
		#pragma omp parallel for
#endif
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics	Statistics(pQuantile != NULL);
			CCT_Snow_Accumulation	Snow;

			if( Get_Snow_Cover(x, y, Snow) )
			{
				if( Snow.Get_Snow_Days() > 0 )
				{
					for(int i=Days[0]; i<Days[1]; i++)
					{
						if( Snow[i] > 0.0 )
						{
							Statistics	+= Snow[i];
						}
					}
				}

				pDays->Set_Value(x, y, Statistics.Get_Count());
			}
			else
			{
				pDays->Set_NoData(x, y);
			}

			//---------------------------------------------
			if( Statistics.Get_Count() > 0 )
			{
				SG_GRID_PTR_SAFE_SET_VALUE(pMean    , x, y, Statistics.Get_Mean    ());
				SG_GRID_PTR_SAFE_SET_VALUE(pMaximum , x, y, Statistics.Get_Maximum ());
				SG_GRID_PTR_SAFE_SET_VALUE(pQuantile, x, y, Statistics.Get_Quantile(Quantile));
			}
			else
			{
				SG_GRID_PTR_SAFE_SET_NODATA(pMean    , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pMaximum , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pQuantile, x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSnow_Cover::Get_Snow_Cover(int x, int y, CCT_Snow_Accumulation &Snow)
{
	CSG_Vector	T, P;

	//-----------------------------------------------------
	if( m_pT->Get_Count() == 12 )
	{
		double	Tm[12], Pm[12];

		for(int i=0; i<12; i++)
		{
			if( m_pT->asGrid(i)->is_NoData(x, y) || m_pP->asGrid(i)->is_NoData(x, y) )
			{
				return( false );
			}

			Tm[i]	= m_pT->asGrid(i)->asDouble(x, y);
			Pm[i]	= m_pP->asGrid(i)->asDouble(x, y);
		}

		if( !CT_Get_Daily_Splined(T, Tm) || !CT_Get_Daily_Precipitation(P, Pm, Tm) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	else
	{
		T.Create(365); P.Create(365);

		for(int i=0; i<365; i++)
		{
			if( m_pT->asGrid(i)->is_NoData(x, y) || m_pP->asGrid(i)->is_NoData(x, y) )
			{
				return( false );
			}

			T[i]	= m_pT->asGrid(i)->asDouble(x, y);
			P[i]	= m_pP->asGrid(i)->asDouble(x, y);
		}
	}

	//-----------------------------------------------------
	return( Snow.Calculate(T, P) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
