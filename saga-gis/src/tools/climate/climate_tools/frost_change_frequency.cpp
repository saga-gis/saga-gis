/**********************************************************
 * Version $Id: frost_change_frequency.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

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
//               frost_change_frequency.cpp              //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "frost_change_frequency.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFrost_Change_Frequency::CFrost_Change_Frequency(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Frost Change Frequency"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool calculates statistics about the frost change frequency "
		"either from monthly or daily minimum and maximum temperatures. "
		"In case of monthly observations these will be spline interpolated "
		"to gain a daily resolution. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "TMIN"		, _TL("Minimum Temperature"),
		_TL("Monthly (12) or daily (365) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "TMAX"		, _TL("Maximum Temperature"),
		_TL("Monthly (12) or daily (365) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "FREQUENCY"	, _TL("Frost Change Frequency"),
		_TL("The number of days with frost change."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(
		NULL	, "DT_MEAN"		, _TL("Mean Temperature Span"),
		_TL("Mean daily temperature span of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "DT_MAX"		, _TL("Maximum Temperature Span"),
		_TL("Maximum daily temperature span of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "DT_STDV"		, _TL("Standard Deviation of Temperature Span"),
		_TL("Standard deviation of daily temperature span of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "TMIN_MEAN"	, _TL("Mean Minimum Temperature"),
		_TL("Mean daily minimum temperature of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "TMIN_MIN"	, _TL("Minimum Temperature"),
		_TL("Coldest daily minimum temperature of all frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFrost_Change_Frequency::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFrost_Change_Frequency::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List		*pTmin	= Parameters("TMIN")->asGridList();
	CSG_Parameter_Grid_List		*pTmax	= Parameters("TMAX")->asGridList();

	if( !(pTmin->Get_Count() == 12 || pTmin->Get_Count() >= 365)
	||  !(pTmax->Get_Count() == 12 || pTmax->Get_Count() >= 365) )
	{
		Error_Set(_TL("Input has to be provided on a monthly (12) or daily (365) basis."));

		return( false );
	}

	CSG_Grid	*pFrequency	= Parameters("FREQUENCY")->asGrid();
	CSG_Grid	*pDT_Mean	= Parameters("DT_MEAN"  )->asGrid();
	CSG_Grid	*pDT_Max	= Parameters("DT_MAX"   )->asGrid();
	CSG_Grid	*pDT_StDv	= Parameters("DT_STDV"  )->asGrid();
	CSG_Grid	*pTmin_Mean	= Parameters("TMIN_MEAN")->asGrid();
	CSG_Grid	*pTmin_Min	= Parameters("TMIN_MIN" )->asGrid();

	DataObject_Set_Colors(pFrequency, 11, SG_COLORS_RED_GREEN, true);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Vector	Tmin, Tmax;

			if( Get_Daily(x, y, pTmin, Tmin)
			&&  Get_Daily(x, y, pTmax, Tmax) )
			{
				CSG_Simple_Statistics	Dif, Min;

				for(int i=0; i<365; i++)
				{
					if( Tmin[i] < 0 && Tmax[i] > 0 )	// frost change day
					{
						Dif	+= Tmax[i] - Tmin[i];
						Min	+= Tmin[i];
					}
				}

				//-----------------------------------------
				if( Dif.Get_Count() > 0 )
				{
					if( pFrequency ) pFrequency->Set_Value(x, y, Dif.Get_Count  ());
					if( pDT_Mean   ) pDT_Mean  ->Set_Value(x, y, Dif.Get_Mean   ());
					if( pDT_Max    ) pDT_Max   ->Set_Value(x, y, Dif.Get_Maximum());
					if( pDT_StDv   ) pDT_StDv  ->Set_Value(x, y, Dif.Get_StdDev ());
					if( pTmin_Mean ) pTmin_Mean->Set_Value(x, y, Min.Get_Mean   ());
					if( pTmin_Min  ) pTmin_Min ->Set_Value(x, y, Min.Get_Minimum());
				}
				else
				{
					if( pFrequency ) pFrequency->Set_Value (x, y, 0);
					if( pDT_Mean   ) pDT_Mean  ->Set_NoData(x, y);
					if( pDT_Max    ) pDT_Max   ->Set_NoData(x, y);
					if( pDT_StDv   ) pDT_StDv  ->Set_NoData(x, y);
					if( pTmin_Mean ) pTmin_Mean->Set_NoData(x, y);
					if( pTmin_Min  ) pTmin_Min ->Set_NoData(x, y);
				}
			}
			else
			{
				if( pFrequency ) pFrequency->Set_NoData(x, y);
				if( pDT_Mean   ) pDT_Mean  ->Set_NoData(x, y);
				if( pDT_Max    ) pDT_Max   ->Set_NoData(x, y);
				if( pDT_StDv   ) pDT_StDv  ->Set_NoData(x, y);
				if( pTmin_Mean ) pTmin_Mean->Set_NoData(x, y);
				if( pTmin_Min  ) pTmin_Min ->Set_NoData(x, y);
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
bool CFrost_Change_Frequency::Get_Daily(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily)
{
	if( pTemperatures->Get_Count() == 12 )
	{
		return( Get_From_Monthly(x, y, pTemperatures, Daily) );
	}

	if( pTemperatures->Get_Count() >= 365 )
	{
		return( Get_From_Daily(x, y, pTemperatures, Daily) );
	}

	return( false );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency::Get_From_Daily(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily)
{
	Daily.Create(365);

	for(int iDay=0; iDay<365; iDay++)
	{
		if( pTemperatures->asGrid(iDay)->is_NoData(x, y) )
		{
			return( false );
		}

		Daily[iDay]	= pTemperatures->asGrid(iDay)->asDouble(x, y);
	}

	return( true );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency::Get_From_Monthly(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily)
{
	static const int	MidOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	//	 0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
	{	15,  45,  74, 105, 135, 166, 196, 227, 258, 288, 319, 349	};

	//-----------------------------------------------------
	CSG_Spline	Spline;

	for(int iMonth=0; iMonth<12; iMonth++)
	{
		if( pTemperatures->asGrid(iMonth)->is_NoData(x, y) )
		{
			return( false );
		}

		Spline.Add(MidOfMonth[iMonth], pTemperatures->asGrid(iMonth)->asDouble(x, y));
	}

	Spline.Add(MidOfMonth[11] - 365, pTemperatures->asGrid(11)->asDouble(x, y));
	Spline.Add(MidOfMonth[10] - 365, pTemperatures->asGrid(10)->asDouble(x, y));

	Spline.Add(MidOfMonth[ 0] + 365, pTemperatures->asGrid( 0)->asDouble(x, y));
	Spline.Add(MidOfMonth[ 1] + 365, pTemperatures->asGrid( 1)->asDouble(x, y));

	//-----------------------------------------------------
	Daily.Create(365);

	for(int iDay=0; iDay<365; iDay++)
	{
		Daily[iDay]	= Spline.Get_Value(iDay);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
