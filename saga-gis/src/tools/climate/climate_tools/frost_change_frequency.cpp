
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
#include "frost_change_frequency.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFrost_Change_Frequency_Calculator::CFrost_Change_Frequency_Calculator(void)
{
	m_pTmin	= NULL;
	m_pTmax	= NULL;
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Calculator::Set_Temperatures(CSG_Parameter_Grid_List *pTmin, CSG_Parameter_Grid_List *pTmax)
{
	if( !(pTmin->Get_Grid_Count() == 12 || pTmin->Get_Grid_Count() >= 365)
	||  !(pTmax->Get_Grid_Count() == 12 || pTmax->Get_Grid_Count() >= 365) )
	{
		SG_UI_Msg_Add_Error(_TL("Input has to be provided on a monthly (12) or daily (365) basis."));

		return( false );
	}

	m_pTmin	= pTmin;
	m_pTmax	= pTmax;

	return( true );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Calculator::Get_Statistics(int x, int y, CSG_Simple_Statistics &Dif, CSG_Simple_Statistics &Min)
{
	CSG_Vector	Tmin, Tmax;

	return( Get_Statistics(x, y, Dif, Min, Tmin, Tmax) );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Calculator::Get_Statistics(int x, int y, CSG_Simple_Statistics &Dif, CSG_Simple_Statistics &Min, CSG_Vector &Tmin, CSG_Vector &Tmax)
{
	if( !Get_Daily(x, y, m_pTmin, Tmin)
	||  !Get_Daily(x, y, m_pTmax, Tmax) )
	{
		return( false );
	}

	for(int i=0; i<365; i++)
	{
		if( Tmin[i] < 0.0 && Tmax[i] > 0.0 )	// frost change day
		{
			Dif	+= Tmax[i] - Tmin[i];
			Min	+= Tmin[i];
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Calculator::Get_Daily(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily)
{
	if( pTemperatures->Get_Grid_Count() == 12 )
	{
		return( Get_From_Monthly(x, y, pTemperatures, Daily) );
	}

	if( pTemperatures->Get_Grid_Count() >= 365 )
	{
		return( Get_From_Daily(x, y, pTemperatures, Daily) );
	}

	return( false );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Calculator::Get_From_Daily(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily)
{
	Daily.Create(365);

	for(int iDay=0; iDay<365; iDay++)
	{
		if( pTemperatures->Get_Grid(iDay)->is_NoData(x, y) )
		{
			return( false );
		}

		Daily[iDay]	= pTemperatures->Get_Grid(iDay)->asDouble(x, y);
	}

	return( true );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Calculator::Get_From_Monthly(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily)
{
	static const int	MidOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	//	 0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
	{	15,  45,  74, 105, 135, 166, 196, 227, 258, 288, 319, 349	};

	//-----------------------------------------------------
	CSG_Spline	Spline;

	Spline.Add(MidOfMonth[10] - 365, pTemperatures->Get_Grid(10)->asDouble(x, y));
	Spline.Add(MidOfMonth[11] - 365, pTemperatures->Get_Grid(11)->asDouble(x, y));

	for(int iMonth=0; iMonth<12; iMonth++)
	{
		if( pTemperatures->Get_Grid(iMonth)->is_NoData(x, y) )
		{
			return( false );
		}

		Spline.Add(MidOfMonth[iMonth], pTemperatures->Get_Grid(iMonth)->asDouble(x, y));
	}

	Spline.Add(MidOfMonth[ 0] + 365, pTemperatures->Get_Grid( 0)->asDouble(x, y));
	Spline.Add(MidOfMonth[ 1] + 365, pTemperatures->Get_Grid( 1)->asDouble(x, y));

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
	Parameters.Add_Grid_List("",
		"TMIN"		, _TL("Minimum Temperature"),
		_TL("Monthly (12) or daily (365) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"TMAX"		, _TL("Maximum Temperature"),
		_TL("Monthly (12) or daily (365) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"FREQUENCY"	, _TL("Frost Change Frequency"),
		_TL("The number of days with frost change."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"DT_MEAN"	, _TL("Mean Temperature Span"),
		_TL("Mean daily temperature span of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"DT_MAX"	, _TL("Maximum Temperature Span"),
		_TL("Maximum daily temperature span of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"DT_STDV"	, _TL("Standard Deviation of Temperature Span"),
		_TL("Standard deviation of daily temperature span of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TMIN_MEAN"	, _TL("Mean Minimum Temperature"),
		_TL("Mean daily minimum temperature of frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TMIN_MIN"	, _TL("Minimum Temperature"),
		_TL("Coldest daily minimum temperature of all frost change days."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
bool CFrost_Change_Frequency::On_Execute(void)
{
	//-----------------------------------------------------
	CFrost_Change_Frequency_Calculator	Calculator;

	if( !Calculator.Set_Temperatures(Parameters("TMIN")->asGridList(), Parameters("TMAX")->asGridList()) )
	{
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
			CSG_Simple_Statistics	Dif, Min;

			if( Calculator.Get_Statistics(x, y, Dif, Min) )
			{
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFrost_Change_Frequency_Interactive::CFrost_Change_Frequency_Interactive(void)
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
	Parameters.Add_Grid_List("",
		"TMIN"		, _TL("Minimum Temperature"),
		_TL("Monthly (12) or daily (365) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"TMAX"		, _TL("Maximum Temperature"),
		_TL("Monthly (12) or daily (365) temperature observations."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"STATISTICS", _TL("Frost Change Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"TDAILY"	, _TL("Daily Temperatures"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Interactive::On_Execute(void)
{
	if( !m_Calculator.Set_Temperatures(Parameters("TMIN")->asGridList(), Parameters("TMAX")->asGridList()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pStatistics	= Parameters("STATISTICS")->asTable();
	m_pStatistics->Destroy();
	m_pStatistics->Set_Name(_TL("Frost Change Statistics"));
	m_pStatistics->Add_Field("NAME" , SG_DATATYPE_String);
	m_pStatistics->Add_Field("VALUE", SG_DATATYPE_Double);
	m_pStatistics->Add_Record()->Set_Value(0, _TL("X"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Y"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Frost Change Frequency"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Mean Temperature Span"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Maximum Temperature Span"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Standard Deviation of Temperature Span"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Mean Minimum Temperature"));
	m_pStatistics->Add_Record()->Set_Value(0, _TL("Minimum Temperature"));

	//-----------------------------------------------------
	m_pDaily	= Parameters("TDAILY")->asTable();
	m_pDaily->Destroy();
	m_pDaily->Set_Name(_TL("Daily Temperatures"));
	m_pDaily->Add_Field("Day"   , SG_DATATYPE_Int   );
	m_pDaily->Add_Field("Tmin"  , SG_DATATYPE_Double);
	m_pDaily->Add_Field("Tmax"  , SG_DATATYPE_Double);
	m_pDaily->Add_Field("Tspan" , SG_DATATYPE_Double);
	m_pDaily->Add_Field("Change", SG_DATATYPE_Int   );
	m_pDaily->Set_Record_Count(365);

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CFrost_Change_Frequency_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode != TOOL_INTERACTIVE_LDOWN && Mode != TOOL_INTERACTIVE_MOVE_LDOWN )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	x, y;

	if( !Get_System().Get_World_to_Grid(x, y, ptWorld) || !Get_System().is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pStatistics->Set_Value(0, 1, ptWorld.Get_X());
	m_pStatistics->Set_Value(1, 1, ptWorld.Get_Y());

	CSG_Simple_Statistics	Dif, Min;	CSG_Vector	Tmin, Tmax;

	if( m_Calculator.Get_Statistics(x, y, Dif, Min, Tmin, Tmax) )
	{
		m_pStatistics->Set_Value(2, 1, Dif.Get_Count());

		if( Dif.Get_Count() > 0 )
		{
			m_pStatistics->Set_Value(3, 1, Dif.Get_Mean   ());
			m_pStatistics->Set_Value(4, 1, Dif.Get_Maximum());
			m_pStatistics->Set_Value(5, 1, Dif.Get_StdDev ());
			m_pStatistics->Set_Value(6, 1, Min.Get_Mean   ());
			m_pStatistics->Set_Value(7, 1, Min.Get_Minimum());
		}
		else for(int i=3; i<m_pStatistics->Get_Count(); i++)
		{
			m_pStatistics->Get_Record(i)->Set_NoData(1);
		}

		m_pDaily->Set_Record_Count(365);

		for(int i=0; i<365; i++)
		{
			CSG_Table_Record	*pDay	= m_pDaily->Get_Record(i);

			pDay->Set_Value(0, i);
			pDay->Set_Value(1, Tmin[i]);
			pDay->Set_Value(2, Tmax[i]);
			pDay->Set_Value(3, Tmax[i] - Tmin[i]);

			if( Tmin[i] < 0 && Tmax[i] > 0 )
			{
				pDay->Set_Value(4, 0.0);
			}
			else
			{
				pDay->Set_NoData(4);
			}
		}
	}
	else for(int i=2; i<m_pStatistics->Get_Count(); i++)
	{
		m_pStatistics->Get_Record(i)->Set_NoData(1);
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
