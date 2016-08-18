/**********************************************************
 * Version $Id: ihacres_cal2.cpp 1261 2011-12-16 15:12:15Z oconrad $
 *********************************************************/
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ihacres_cal2                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ihacres_cal.cpp                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com                   //
//                                                       //
//                     2008-01-15                        //
///////////////////////////////////////////////////////////
//
//-------------------------------------------------------//
// References:
// Jakeman, A.J. / Hornberger, G.M (1993).
//   How Much Complexity Is Warranted in a
//	 Rainfall-Runoff Model?
//	 Water Resources Research, (29), NO. 8 (2637-2649)
// Kokkonen, T. S. et al. (2003).
//   Predicting daily flows in ungauged catchments:
//   model regionalization from catchment descriptors
//   at the Coweeta Hydrologic Laboratory, North Carolina
//   Hydrological Processes (17), 2219-2238
// Croke, B. F. W., W. S. Merritt, et al. (2004).
//   A dynamic model for predicting hydrologic response
//   to land cover changes in gauged and
//   ungauged catchments.
//   Journal Of Hydrology 291(1-2): 115-131.
//-------------------------------------------------------//

//---------------------------------------------------------
// ToDo
//---------------------------------------------------------
//	- beste Simulation automatisch rausschreiben
//---------------------------------------------------------
#include <iostream> // used for textfile output (test only)
#include <fstream>  // used for textfile output (test only)

#include <stdlib.h> // random numbers
#include <time.h>	// random numbers

#include "ihacres_cal2.h"
// #include "model_tools.h" // already in ihacres_eq.h
#include "convert_sl.h"
//---------------------------------------------------------

///////////////////////////////////////////////////////////////////////
//
//		Constructor
//
///////////////////////////////////////////////////////////////////////

Cihacres_cal2::Cihacres_cal2(void)
{
	//-----------------------------------------------------
	// 1. Info...
	Set_Name(_TL("IHACRES Calibration (2)"));

	Set_Author(SG_T("copyrights (c) 2008 Stefan Liersch"));

	Set_Description(_TW("Calibration Tool for the Model IHACRES \n \n \n"
		"Reference: \n \n"
		"Jakeman, A.J. / Hornberger, G.M. (1993). \n"
		"How Much Complexity Is Warranted in a Rainfall-Runoff Model? \n"
		"Water Resources Research, (29), NO. 8 (2637-2649) \n \n"
		"Croke, B. F. W., W. S. Merritt, et al. (2004).\n"
		"A dynamic model for predicting hydrologic response "
		"to land cover changes in gauged and "
		"ungauged catchments. \n"
		"Journal Of Hydrology 291(1-2): 115-131."
	));

	//-----------------------------------------------------
	// Create First Tool Dialog
	_CreateDialog1();
	//-----------------------------------------------------
}

///////////////////////////////////////////////////////////////////////
//
//		Destructor
//
///////////////////////////////////////////////////////////////////////

Cihacres_cal2::~Cihacres_cal2(void)
{
	//if (m_bSnowModule)
	//{
	//	delete m_pSnowModule;
	//	delete[] m_pMeltRate;
	//}
}


///////////////////////////////////////////////////////////////////////
//
//		On_Execute()
//
///////////////////////////////////////////////////////////////////////


//---------------------------------------------------------
// This function is executed when the user is pressing the OK button
// in the first module dialog
bool Cihacres_cal2::On_Execute(void)
{
	int			first, last;
	double		NSE_temp	= 0.0;
	double		NSE_max		= -9999.9;
	string		nse, nse_text;

	//---------------------------------------------------------
	// Assign parameters from First Tool Dialog
	m_pTable		= Parameters("TABLE")			->asTable();
	// Field numbers
	m_dateField		= Parameters("DATE_Field")		->asInt();
	m_dischargeField= Parameters("DISCHARGE_Field")	->asInt();
	m_pcpField		= Parameters("PCP_Field")		->asInt();
	m_tmpField		= Parameters("TMP_Field")		->asInt();
	m_inflowField	= Parameters("INFLOW_Field")	->asInt();

	m_bUpstream		= Parameters("bUPSTREAM")		->asBool();

	m_bTMP			= Parameters("USE_TMP")			->asBool();

	m_nsim			= Parameters("NSIM")			->asInt();
	m_area			= Parameters("AREA")			->asDouble();
	//m_dev_eRainDis	= Parameters("DEV_ERAINDIS")	->asDouble();
	//m_dev_eRainPCP	= Parameters("DEV_ERAINPCP")	->asDouble();
	m_storconf		= Parameters("STORAGE")			->asInt();
	m_IHAC_version	= Parameters("IHACVERS")		->asInt();
	m_bSnowModule	= Parameters("SNOW_TOOL")		->asBool();
	first = last	= 0;
	//---------------------------------------------------------

	//---------------------------------------------------------
	// After pressing OK in the first Dialog Dialog2 appears.
	// In the sec
	if ( _CreateDialog2() )
	//---------------------------------------------------------
	{
		//---------------------------------------------------------
		// Searching the first and the last record of the time range
		ihacres.AssignFirstLastRec(*m_pTable, first, last, m_date1, m_date2, m_dateField);
		//---------------------------------------------------------

		//---------------------------------------------------------
		// Initialize arrays
		m_nValues = last - first + 1;
		_InitPointers();
		//---------------------------------------------------------

		//---------------------------------------------------------
		// Assign selected time range to vector m_vec_date,
		// discharge[], pcp[], and tmp[]
		_ReadInputTable(first, last);
		//---------------------------------------------------------

		//---------------------------------------------------------
		// Convert streamflow from m3/s to mm/day
		if (m_bUpstream)
		{
			// if the subbasin is upstream (without external inflow) then everything
			// is as it should be.
			m_p_Q_obs_mmday = model_tools::m3s_to_mmday(m_p_Q_obs_m3s,m_p_Q_obs_mmday,m_nValues,m_area);
		} else {
			// if the subbasin is downstream of other subbasins, thus there
			// are external inflows the array m_p_Q_obs_mmday must be calculated as following:
			// m_p_Q_obs_mmday =
			// (observed streamflow at the outlet of the subbasin) - (observed inflow from the upstream subbasin(s))
			_Calc_ObsMinInflow();
			m_p_Q_obs_mmday = model_tools::m3s_to_mmday(m_p_Q_dif_m3s, m_p_Q_obs_mmday, m_nValues, m_area);
		}
		// calculate sum of observed discharge in [mm]
		m_sum_obsDisMM = ihacres.SumVector(m_p_Q_obs_mmday, m_nValues);
		//---------------------------------------------------------

		//---------------------------------------------------------
		m_pTable = SG_Create_Table();
		_CreateOutputTable();
		m_counter = 0;
		//---------------------------------------------------------

		///////////////////////////////////////////////////////////
		//
		//		SIMULATION
		//
		///////////////////////////////////////////////////////////

		// initialize random function
		srand((unsigned) time(NULL)); // using time.h

		for (int sim = 0; sim < m_nsim && Set_Progress(sim, m_nsim); sim++)
		{
			_CalcNonLinearModule();

			_CalcLinearModule();

			_CalcEfficiency();

			// write in output table if criterion is fulfilled
			NSE_temp = ihacres._Assign_NSE_temp(m_obj_func, m_NSE, m_NSE_highflow, m_NSE_lowflow);

			if (NSE_temp > m_NSEmin)
			{
				if (NSE_temp > NSE_max)
				{
					NSE_max = NSE_temp;
					nse = convert_sl::Double2String(NSE_max).c_str();
					nse_text = "max. NSE ";
					nse_text += nse;
					Process_Set_Text(CSG_String(nse_text.c_str()));
				}

				_WriteOutputTable();
			}
		}
		//---------------------------------------------------------
		m_pTable->Set_Name(SG_T("IHACRES_cal2"));
		Parameters("TABLEout")->Set_Value(m_pTable);



		//---------------------------------------------------------
		_DeletePointers();
		//---------------------------------------------------------

		return(true);

	} // end if (_CreateDialog2())


	return(false);
}


///////////////////////////////////////////////////////////////////////
//
//		PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------
//		InitPointers()
//---------------------------------------------------------------------
void Cihacres_cal2::_InitPointers()
{
	int n = m_nValues;

	m_vec_date.resize(n);
	if (!m_bUpstream)
	{
		m_p_Q_Inflow_m3s = new double[n];
		m_p_Q_dif_m3s	 = new double[n];
	}
	m_p_Q_obs_m3s	= new double[n];
	m_p_Q_obs_mmday	= new double[n];
	m_p_Q_sim_mmday	= new double[n];
	m_pPCP			= new double[n];
	m_pTMP			= new double[n];
	m_pExcessRain	= new double[n];
	m_pTw			= new double[n];
	m_pWI			= new double[n];
	if (m_bSnowModule)
		m_pMeltRate	= new double[n];
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//		DeletePointers()
//---------------------------------------------------------------------
void Cihacres_cal2::_DeletePointers()
{
	m_vec_date.resize(0);
	if (!m_bUpstream)
	{
		delete[] m_p_Q_Inflow_m3s;
		delete[] m_p_Q_dif_m3s;
	}
	delete[] m_p_Q_obs_m3s;
	delete[] m_p_Q_obs_mmday;
	delete[] m_p_Q_sim_mmday;
	delete[] m_pPCP;
	delete[] m_pTMP;
	delete[] m_pExcessRain;
	delete[] m_pTw;
	delete[] m_pWI;
	if (m_bSnowModule)
		delete[] m_pMeltRate;
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//		ReadInputTable()
//---------------------------------------------------------------------
void Cihacres_cal2::_ReadInputTable(int first, int last)
{
	int j, k;

	for (j = 0, k = first; j < m_nValues, k < last + 1; j++, k++)
	{
		// put the date string into the vector
		m_vec_date[j].append(CSG_String(m_pTable->Get_Record(k)->asString(m_dateField)));
		m_p_Q_obs_m3s[j]= m_pTable->Get_Record(k)->asDouble(m_dischargeField);
		m_pPCP[j]		= m_pTable->Get_Record(k)->asDouble(m_pcpField);
		m_pTMP[j]		= m_pTable->Get_Record(k)->asDouble(m_tmpField);
		if (!m_bUpstream)
			m_p_Q_Inflow_m3s[j] = m_pTable->Get_Record(k)->asDouble(m_inflowField);
	}
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//		Calculate observed streamflow time series if inflow occurs
//---------------------------------------------------------------------
void Cihacres_cal2::_Calc_ObsMinInflow()
{
	for (int i = 0; i < m_nValues; i++)
	{
		m_p_Q_dif_m3s[i] = m_p_Q_obs_m3s[i] - m_p_Q_Inflow_m3s[i];
		if (m_p_Q_dif_m3s[i] < 0.0) m_p_Q_obs_mmday[i] = 0;
	}
}
//---------------------------------------------------------------------



///////////////////////////////////////////////////////////////////////
//
//
//							SIMULATION
//
//
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
//		NON-LINEAR TOOL
//
///////////////////////////////////////////////////////////////////////

void Cihacres_cal2::_CalcNonLinearModule()
{
	double eR_init = 0.0;
	//-------------------------------------------------------------
	// Assign random values
	//-------------------------------------------------------------
	if (m_bSnowModule)
	{
		m_SnowParms.T_Rain	= model_tools::Random_double(m_T_Rain_lb, m_T_Rain_ub);
		m_SnowParms.T_Melt	= model_tools::Random_double(m_T_Melt_lb, m_T_Melt_ub);
		m_SnowParms.DD_FAC	= model_tools::Random_double(m_DD_FAC_lb, m_DD_FAC_ub);

		m_pSnowModule = new CSnowModule(m_pTMP, m_pPCP, m_nValues,
			m_SnowParms.T_Rain, m_SnowParms.T_Melt, m_SnowParms.DD_FAC);

		m_pMeltRate = m_pSnowModule->Get_MeltRate(m_pMeltRate, m_nValues);
		delete m_pSnowModule;
	}

	m_Tw	= model_tools::Random_double(m_TwConst_lb, m_TwConst_ub);
	m_f		= model_tools::Random_double(m_f_lb, m_f_ub);
	m_c		= model_tools::Random_double(m_c_lb, m_c_ub);

	if (m_IHAC_version == 1) { // Croke etal. (2005)
		m_l	= model_tools::Random_double(m_l_lb, m_l_ub);
		m_p	= model_tools::Random_double(m_p_lb, m_p_ub);
	}
	//-------------------------------------------------------------

	//-------------------------------------------------------------
	// Simulation (non-linear module)
	//-------------------------------------------------------------
	switch(m_IHAC_version)
	{
	case 0: // Jakeman & Hornberger (1993)
		if (m_bTMP)
		{
			ihacres.CalcWetnessTimeConst(m_pTMP, m_pTw, m_Tw, m_f, m_nValues);
		}

		if (m_bSnowModule)
		{
			ihacres.CalcWetnessIndex(m_pTw, m_pPCP, m_pTMP,	m_pWI, 0.5, m_c,
				m_bSnowModule, m_SnowParms.T_Rain, m_nValues);

			ihacres.CalcExcessRain(m_pPCP, m_pTMP, m_pWI,m_pExcessRain,eR_init,
				m_sum_eRainGTpcp, m_nValues, m_bSnowModule,
				m_SnowParms.T_Rain, m_SnowParms.T_Melt, m_pMeltRate);
		} else {
			ihacres.CalcWetnessIndex(m_pTw, m_pPCP, m_pTMP, m_pWI, 0.5, m_c,
				m_bSnowModule, 0, m_nValues);

			ihacres.CalcExcessRain(m_pPCP, m_pTMP, m_pWI, m_pExcessRain,eR_init,
				m_sum_eRainGTpcp, m_nValues, m_bSnowModule,
				0,0,0);
		} // end if (m_bSnowModule)

		break;
	case 1: // Croke et al. (2005) Redesign
		if (m_bTMP)
		{
			ihacres.CalcWetnessTimeConst_Redesign(m_pTMP, m_pTw, m_Tw, m_f, m_nValues);
		}

		if (m_bSnowModule)
		{
			ihacres.CalcWetnessIndex_Redesign(m_pTw, m_pPCP, m_pWI, 0.5,
				m_bSnowModule, m_SnowParms.T_Rain, m_nValues);

			ihacres.CalcExcessRain_Redesign(m_pPCP, m_pTMP, m_pWI, m_pExcessRain, eR_init,
				m_sum_eRainGTpcp, m_nValues,
				m_c, m_l, m_p,
				m_bSnowModule, m_SnowParms.T_Rain, m_SnowParms.T_Melt, m_pMeltRate);
		} else {
			ihacres.CalcWetnessIndex_Redesign(m_pTw, m_pPCP, m_pWI, 0.5,
				m_bSnowModule, 0, m_nValues);

			ihacres.CalcExcessRain_Redesign(m_pPCP, m_pTMP, m_pWI, m_pExcessRain, eR_init,
				m_sum_eRainGTpcp, m_nValues,
				m_c, m_l, m_p,
				m_bSnowModule, 0,0,0);
		}
		break;
	} // end switch(m_IHAC_version)
}
//---------------------------------------------------------------------



///////////////////////////////////////////////////////////////////////
//
//		LINEAR TOOL
//
///////////////////////////////////////////////////////////////////////

void Cihacres_cal2::_CalcLinearModule()
{
	//-------------------------------------------------------------
	// Assign random values
	//-------------------------------------------------------------
	switch(m_storconf)
	{
	case 0: // single storage
		m_a		= model_tools::Random_double(m_a_lb, m_a_ub);
		m_b		= model_tools::Random_double(m_b_lb, m_b_ub);
		break;
	case 1: // two storages in parallel
		do
		{
			m_aq		= model_tools::Random_double(m_aq_lb, m_aq_ub);
			m_as		= model_tools::Random_double(m_as_lb, m_as_ub);
			m_bq		= model_tools::Random_double(m_bq_lb, m_bq_ub);
			// Calculate parameter m_vq to check parms aq and bq
			// Equation after Jakeman & Hornberger (1993)
			m_vq	= m_bq / ( 1 + m_aq );
		}
		while (m_vq < 0.0 || m_vq > 1.0);
		m_bs = ihacres.Calc_Parm_BS(m_aq, m_as, m_bq);
		break;
	}
	//-------------------------------------------------------------

	//-------------------------------------------------------------
	// Simulate streamflow (linear module)
	//-------------------------------------------------------------
	switch(m_storconf)
	{
	case 0: // single storage
		ihacres.SimStreamflowSingle(m_pExcessRain, m_p_Q_obs_mmday[0],
			m_p_Q_sim_mmday, m_delay, m_a, m_b, m_nValues);
		break;
	case 1: // two storages in parallel
		ihacres.SimStreamflow2Parallel(m_pExcessRain, m_p_Q_sim_mmday, m_p_Q_obs_mmday[0],
			m_aq, m_as, m_bq, m_bs, m_vq, m_vs, m_IHAC_version, m_nValues, m_delay);
		break;
	case 2: // two storages in series
		break;
	} // end switch(m_StorConf)
}
//---------------------------------------------------------------------



///////////////////////////////////////////////////////////////////////
//
//		EFFICIENCY
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//		Nash-Sutcliffe efficiency
//---------------------------------------------------------------------
void Cihacres_cal2::_CalcEfficiency()
{
	m_NSE			= model_tools::CalcEfficiency(m_p_Q_obs_mmday, m_p_Q_sim_mmday,m_nValues);
	m_NSE_highflow	= model_tools::Calc_NSE_HighFlow(m_p_Q_obs_mmday, m_p_Q_sim_mmday, m_nValues);
	m_NSE_lowflow	= model_tools::Calc_NSE_LowFlow(m_p_Q_obs_mmday, m_p_Q_sim_mmday, m_nValues);
	m_PBIAS			= model_tools::Calc_PBIAS(m_p_Q_obs_mmday, m_p_Q_sim_mmday, m_nValues);
}
//---------------------------------------------------------------------





/////////////////////////////////////////////////////////////////////////////
//
//							CREATE TABLES
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//		Create output table
//---------------------------------------------------------------------
void Cihacres_cal2::_CreateOutputTable()
{
	// creating the column titles
	m_pTable->Add_Field("NSE",				SG_DATATYPE_Double);
	m_pTable->Add_Field("NSE_high",			SG_DATATYPE_Double);
	m_pTable->Add_Field("NSE_low",			SG_DATATYPE_Double);
	m_pTable->Add_Field("PBIAS",			SG_DATATYPE_Double);
	m_pTable->Add_Field("eR_ovest",			SG_DATATYPE_Double);
	m_pTable->Add_Field("vq",				SG_DATATYPE_Double);
	m_pTable->Add_Field("vs",				SG_DATATYPE_Double);
	m_pTable->Add_Field("T(q)",				SG_DATATYPE_Double);
	m_pTable->Add_Field("T(s)",				SG_DATATYPE_Double);
	m_pTable->Add_Field("Tw",				SG_DATATYPE_Double);
	m_pTable->Add_Field("f",				SG_DATATYPE_Double);
	m_pTable->Add_Field("c",				SG_DATATYPE_Double);
	if ( m_IHAC_version == 1 ) // Croke etal. (2005)
	{
		m_pTable->Add_Field("l",			SG_DATATYPE_Double);
		m_pTable->Add_Field("p",			SG_DATATYPE_Double);
	}
	if (m_bSnowModule)
	{
		m_pTable->Add_Field("T_Rain",		SG_DATATYPE_Double);
		m_pTable->Add_Field("T_Melt",		SG_DATATYPE_Double);
		m_pTable->Add_Field("DD_FAC",		SG_DATATYPE_Double);
	}
	switch(m_storconf)
	{
	case 0: // single
		m_pTable->Add_Field("a",			 SG_DATATYPE_Double);
		m_pTable->Add_Field("b",			 SG_DATATYPE_Double);
		break;
	case 1: // two storages in parallel
		m_pTable->Add_Field("aq",			 SG_DATATYPE_Double);
		m_pTable->Add_Field("as",			 SG_DATATYPE_Double);
		m_pTable->Add_Field("bq",			 SG_DATATYPE_Double);
		m_pTable->Add_Field("bs",			 SG_DATATYPE_Double);
		break;
	}
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//		Create output table
//---------------------------------------------------------------------
void Cihacres_cal2::_WriteOutputTable()
{
	int					field = 0;
	CSG_Table_Record	*pRecord;

	// add a new record to the table
	m_pTable->Add_Record();
	pRecord	= m_pTable->Get_Record(m_counter);

	// writing the data to the current row
	pRecord->Set_Value(field,m_NSE); field++;
	pRecord->Set_Value(field,m_NSE_highflow); field++;
	pRecord->Set_Value(field,m_NSE_lowflow); field++;
	pRecord->Set_Value(field,m_PBIAS); field++;
	pRecord->Set_Value(field,m_sum_eRainGTpcp); field++;
	pRecord->Set_Value(field,m_vq); field++;
	pRecord->Set_Value(field,m_vs); field++;
	pRecord->Set_Value(field,ihacres.Calc_TimeOfDecay(m_aq)); field++;
	pRecord->Set_Value(field,ihacres.Calc_TimeOfDecay(m_as)); field++;
	pRecord->Set_Value(field,m_Tw); field++;
	pRecord->Set_Value(field,m_f); field++;
	pRecord->Set_Value(field,m_c); field++;
	if ( m_IHAC_version == 1 ) // Croke et al. (2005)
	{
		pRecord->Set_Value(field,m_l); field++;
		pRecord->Set_Value(field,m_p); field++;
	}
	if ( m_bSnowModule )
	{
		pRecord->Set_Value(field,m_SnowParms.T_Rain); field++;
		pRecord->Set_Value(field,m_SnowParms.T_Melt); field++;
		pRecord->Set_Value(field,m_SnowParms.DD_FAC); field++;
	}
	switch(m_storconf)
	{
	case 0: // single
		pRecord->Set_Value(field,m_a); field++;
		pRecord->Set_Value(field,m_b); field++;
		break;
	case 1: // two storages in parallel
		pRecord->Set_Value(field,m_aq); field++;
		pRecord->Set_Value(field,m_as); field++;
		pRecord->Set_Value(field,m_bq); field++;
		pRecord->Set_Value(field,m_bs); field++;
		break;
	}
	m_counter++; // record counter
}
//---------------------------------------------------------------------






///////////////////////////////////////////////////////////////////////
//
//		DIALOGS
//
///////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------
//		DIALOG 1
//---------------------------------------------------------------------
void Cihacres_cal2::_CreateDialog1()
{
	CSG_Parameter	*pNode;
	CSG_String s;

	pNode = Parameters.Add_Table(
		NULL	, "TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "DATE_Field"	, _TL("Date Column"),
		_TL("Select the column containing the Date")
	);

	Parameters.Add_Table_Field(
		pNode	, "DISCHARGE_Field"	, _TL("Streamflow Column"),
		_TL("Select the Column containing Discharge Values")
	);

	Parameters.Add_Table_Field(
		pNode	, "PCP_Field"	, _TL("Precipitation Column"),
		_TL("Select the Column containing precipitation Values")
	);

	Parameters.Add_Table_Field(
		pNode	, "TMP_Field"	, _TL("Temperature Column"),
		_TL("Select the Column containing Temperature Values")
	);

	Parameters.Add_Table_Field(
		pNode	, "INFLOW_Field"	, _TL("Subbasin Inflow"),
		_TL("Select the column containing inflow data to the subbasin")
	);

	Parameters.Add_Value(
		pNode,	"bUPSTREAM",	_TL("Is the subbasin upstream (no external inflow)"),
		_TL("If checked, it means there is no external inflow to the subbasin"),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		pNode,	"USE_TMP",	_TL("Using temperature data?"),
		_TL("If checked, then temperature data are used."),
		PARAMETER_TYPE_Bool, true
	);

	pNode = Parameters.Add_Value(
		NULL,	"NSIM",	_TL("Number of Simulations"),
		_TL("Number of Simulations for Calibration"),
		PARAMETER_TYPE_Int,
		1000, 1, true, 10000000, true
	);

	pNode = Parameters.Add_Value(
		NULL,	"AREA",	_TL("Area of the Watershed in [km2]"),
		_TL("Area of the Watershed in [km2] used for unit conversion"),
		PARAMETER_TYPE_Double,
		100.0, 0.00001, true, 0.0, false
	);

	//s.Printf(SG_T("Node2", 2);
	//	pNode = Parameters.Add_Node(NULL,s,SG_T("Non-Linear Tool",_TL(""));

	//Parameters.Add_Value(
	//	pNode,	"DEV_ERAINDIS",	_TL("Maximum Deviation: ExcessRain-Streamflow [%]"),
	//	_TL("Maximum Deviation between estimated excess rainfall and observed streamflow volumes"),
	//	PARAMETER_TYPE_Double,
	//	5.0, 0.0, true, 50.0, true
	//);

	//Parameters.Add_Value(
	//	pNode,	"DEV_ERAINPCP",	_TL("Maximum Overestimation of ExcessRain [%]"),
	//	_TL("Maximum Sum of overestimated ExcessRain to total ExcessRain in [%]"),
	//	PARAMETER_TYPE_Double,
	//	5.0, 0.0, true, 50.0, true
	//);

	s.Printf(SG_T("Node3"), 3);
	pNode = Parameters.Add_Node(NULL,s,SG_T("Storage Configuration"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "STORAGE"		, _TL("Storage"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Single Storage"),			//  0
			_TL("Two Parallel Storages"),	//  1
			_TL("Two Storages in Series")	//  2
		)
	);

	s.Printf(SG_T("Node4"), 4);
	pNode = Parameters.Add_Node(NULL,s,_TL("IHACRES Version"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "IHACVERS"		, _TL("IHACRES Version"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Jakeman & Hornberger (1993)"),	//  0
			_TL("Croke et al. (2005)")			//	1
		)
	);

	Parameters.Add_Value(
		pNode,	"SNOW_TOOL",	_TL("Snow Tool on/off"),
		_TL("If checked the snow module is active"),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Table_Output(
		NULL	, "TABLEout"	, _TL("Table"),
		_TL("")
	);
	Parameters.Add_Table_Output(
		NULL	, "TABLEparms"	, _TL("Table"),
		_TL("")
	);
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//		DIALOG 2
//---------------------------------------------------------------------
bool Cihacres_cal2::_CreateDialog2()
{
	CSG_String		s;
	CSG_Parameters	P;  // used to add Parameters in the second dialog
	CSG_Parameter	*pNode, *pNode1;


	//	Dialog design
	P.Set_Name(_TL("IHACRES Model Parameters"));

	s.Printf(SG_T("Node1"), 1);
	pNode = P.Add_Node(NULL,s,SG_T("Time Range"),_TL(""));

	s.Printf(SG_T("FDAY") , 1-1);
	P.Add_String(pNode,s,_TL("First Day"),_TL(""),
		m_pTable->Get_Record(0)->asString(m_dateField));

	s.Printf(SG_T("LDAY") , 1-2);
	P.Add_String(pNode,s,_TL("Last Day"),_TL(""),
		m_pTable->Get_Record(m_pTable->Get_Record_Count()-1)->asString(m_dateField));

	//-----------------------------------------------------------------
	// Non-linear parameters
	//-----------------------------------------------------------------
	s.Printf(SG_T("Node2"), 2);
		pNode = P.Add_Node(NULL,s,SG_T("Non-Linear Tool"),_TL(""));

	P.Add_Value(
		pNode,	"TwFAC_lb",	_TL("(Tw) wetness decline time constant [lower bound]"),
		_TW("Tw is approximately the time constant, or inversely,"
		"the rate at which the catchment wetness declines in the absence of rainfall"),
		PARAMETER_TYPE_Double,
		1.0, 0.01, true, 150.0, true
	);
	P.Add_Value(
		pNode,	"TwFAC_ub",	_TL("(Tw) wetness decline time constant [upper bound]"),
		_TW("Tw is approximately the time constant, or inversely,SG_T("
		"the rate at which the catchment wetness declines in the absence of rainfall"),
		PARAMETER_TYPE_Double,
		50.0, 0.01, true, 150.0, true
		);

	if (m_bTMP) // if observed temperature data are used
	{
		P.Add_Value(
			pNode	, "TFAC_lb"		, _TL("(f) Temperature Modulation Factor [lower bound]"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			0.05, 0.0001, true, 5.0, true
		);
		P.Add_Value(
			pNode	, "TFAC_ub"		, _TL("(f) Temperature Modulation Factor [upper bound]"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			0.5, 0.0001, true, 5.0, true
		);
	}

	P.Add_Value(
		pNode,	"CFAC_lb",	_TL("(c) Parameter [lower bound]"),
		_TL("Parameter (c) to fit streamflow volume"),
		PARAMETER_TYPE_Double,
		0.001, 0.0, true, 1.0, true
	);
	P.Add_Value(
		pNode,	"CFAC_ub",	_TL("(c) Parameter [upper bound]"),
		_TL("Parameter (c) to fit streamflow volume"),
		PARAMETER_TYPE_Double,
		0.01, 0.0, true, 1.0, true
	);

	if (m_IHAC_version == 1) // Croke et al. (2005) Redesign
	{
		s.Printf(SG_T("Node2-2"), 2-2);
		pNode1 = P.Add_Node(pNode,s,SG_T("Soil moisture index threshold"),_TL(""));

		P.Add_Value(
			pNode1	, "SMI_L_lb"		, _TL("Soil moisture index (l) [lower bound]"),
			_TL("Soil moisture index threshold (l), lower bound"),
			PARAMETER_TYPE_Double,
			0.0, 0.0, true, 5.0, true
		);
		P.Add_Value(
			pNode1	, "SMI_L_ub"		, _TL("Soil moisture index (l) [upper bound]"),
			_TL("Soil moisture index threshold (l), upper bound"),
			PARAMETER_TYPE_Double,
			5.0, 0.0, true, 5.0, true
		);
		s.Printf(SG_T("Node2-3"), 2-3);
		pNode1 = P.Add_Node(pNode,s,_TL("Power on soil moisture"),_TL(""));

		P.Add_Value(
			pNode1	, "SM_P_lb"		, _TL("Power on soil (p) [lower bound]"),
			_TL("Power on soil moisture (p), lower bound"),
			PARAMETER_TYPE_Double,
			0.0, 0.0, true, 5.0, true
		);
		P.Add_Value(
			pNode1	, "SM_P_ub"		, _TL("Power on soil (p) [upper bound]"),
			_TL("Power on soil moisture (p), upper bound"),
			PARAMETER_TYPE_Double,
			5.0, 0.0, true, 5.0, true
		);
	}
	//-----------------------------------------------------------------
	// Linear module
	//-----------------------------------------------------------------
	switch(m_storconf)
	{
	case 0: // single storage
		s.Printf(SG_T("Node3"), 3);
		pNode = P.Add_Node(NULL,s,_TL("Linear Tool"),_TL(""));

		s.Printf(SG_T("Node3-1"), 3-1);
		pNode1 = P.Add_Node(pNode,s,_TL("Parameter (a)"),_TL(""));

		P.Add_Value(
			pNode1,	"AFAC_lb",	_TL("(a) [lower bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.5, -0.99, true, -0.01, true
		);
		P.Add_Value(
			pNode1,	"AFAC_ub",	_TL("(a) [upper bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.9, -0.99, true, -0.01, true
		);

		s.Printf(SG_T("Node3-2"), 3-2);
		pNode1 = P.Add_Node(pNode,s,_TL("Parameter (b)"),_TL(""));

		P.Add_Value(
			pNode1,	"BFAC_lb",	_TL("(b) [lower bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.0, 0.001, true, 1.0, true
		);
		P.Add_Value(
			pNode1,	"BFAC_ub",	_TL("(b) [upper bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			1.0, 0.001, true, 1.0, true
		);
		break;

	case 1: // two parallel storages
		s.Printf(SG_T("Node3"), 3);
		pNode = P.Add_Node(NULL,s,_TL("Linear Tool"),_TL(""));

		// Parameter a
		s.Printf(SG_T("Node3-1"), 3-1);
		pNode1 = P.Add_Node(pNode,s,_TL("Parameter (a)"),_TL(""));

		P.Add_Value(
			pNode1,	"AQ_lb",	_TL("a(q) [lower bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.5, -0.99, true, -0.01, true
		);
		P.Add_Value(
			pNode1,	"AQ_ub",	_TL("a(q) [upper bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.7, -0.99, true, -0.01, true
		);
		P.Add_Value(
			pNode1,	"AS_lb",	_TL("a(s) [lower bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.7, -0.99, true, -0.01, true
		);
		P.Add_Value(
			pNode1,	"AS_ub",	_TL("a(s) [upper bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.99, -0.99, true, -0.01, true
		);

		// Parameter b
		s.Printf(SG_T("Node3-2"), 3-2);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter (b)"),_TL(""));

		P.Add_Value(
			pNode1,	"BQ_lb",	_TL("b(q) [lower bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.0, 0.0, true, 1.0, true
		);
		P.Add_Value(
			pNode1,	"BQ_ub",	_TL("b(q) [upper bound]"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.5, 0.0, true, 1.0, true
		);
		break;
	case 2: // two storages in series
		break;
	} // end switch (m_storconf)


	//-----------------------------------------------------------------
	// Snow module
	//-----------------------------------------------------------------
	if (m_bSnowModule) // if snow module is active
	{
		s.Printf(SG_T("Node2-4"), 2-4);
		pNode = P.Add_Node(NULL,s,SG_T("Snow Tool"),_TL(""));

		P.Add_Value(
			pNode,	"T_RAIN_lb",	_TL("Temperature Threshold for Rainfall (lower bound)"),
			_TL("Below this threshold precipitation will fall as snow"),
			PARAMETER_TYPE_Double,
			-1.0, -10.0, true, 10.0, true
		);
		P.Add_Value(
			pNode,	"T_RAIN_ub",	_TL("Temperature Threshold for Rainfall (upper bound)"),
			_TL("Below this threshold precipitation will fall as snow"),
			PARAMETER_TYPE_Double,
			1.0, -10.0, true, 10.0, true
		);
		P.Add_Value(
			pNode,	"T_MELT_lb",	_TL("Temperature Threshold for Melting (lower bound)"),
			_TL("Above this threshold snow will start to melt"),
			PARAMETER_TYPE_Double,
			-1.0, -5.0, true, 10.0, true
		);
		P.Add_Value(
			pNode,	"T_MELT_ub",	_TL("Temperature Threshold for Melting (upper bound)"),
			_TL("Above this threshold snow will start to melt"),
			PARAMETER_TYPE_Double,
			1.0, -5.0, true, 10.0, true
		);
		P.Add_Value(
			pNode,	"DD_FAC_lb",	_TL("Day-Degree Factor (lower bound)"),
			_TL("Day-Degree Factor depends on catchment characteristics"),
			PARAMETER_TYPE_Double,
			0.7, 0.7, true, 9.2, true
		);
		P.Add_Value(
			pNode,	"DD_FAC_ub",	_TL("Day-Degree Factor (upper bound)"),
			_TL("Day-Degree Factor depends on catchment characteristics"),
			PARAMETER_TYPE_Double,
			9.2, 0.7, true, 9.2, true
		);
	}



	s.Printf(SG_T("Node4"), 4);
	pNode = P.Add_Node(NULL,s,SG_T("Time Delay after Start of Rainfall (INTEGER)"),_TL(""));

	P.Add_Value(
		pNode1,	"DELAY",	_TL("Time Delay (Rain-Runoff)"),
		_TL("The delay after the start of rainfall, before the discharge starts to rise."),
		PARAMETER_TYPE_Int,
		1, 1, true, 100, true
	);

	s.Printf(SG_T("Node6"), 6);
	pNode = P.Add_Node(NULL,s,SG_T("Nash-Sutcliffe Efficiency"),_TL(""));

	P.Add_Choice(
		pNode, "OBJ_FUNC"		, _TL("Objective Function"),
		_TL(""),
		_TL("NSE|NSE high flow|NSE low flow")
	);

	P.Add_Value(
		pNode,	"NSEMIN",	_TL("Minimum Nash-Sutcliffe Efficiency"),
		_TL("Minimum Nash-Sutcliffe Efficiency required to print simulation to calibration table"),
		PARAMETER_TYPE_Double,
		0.7, 0.1, true, 1.0, true
	);
	// End of second dialog design


	if( SG_UI_Dlg_Parameters(&P, _TL("Choose Time Range")) )
	{
		// assign data from second dialog

		m_date1			= P(CSG_String::Format(SG_T("FDAY"),m_dateField).c_str())->asString();
		m_date2			= P(CSG_String::Format(SG_T("LDAY"),m_dischargeField).c_str())->asString();

		//-------------------------------------------------
		// assign non-linear module parameters
		//-------------------------------------------------
		m_TwConst_lb	= P("TwFAC_lb")					->asDouble();
		m_TwConst_ub	= P("TwFAC_ub")					->asDouble();
		m_c_lb			= P("CFAC_lb")					->asDouble();
		m_c_ub			= P("CFAC_ub")					->asDouble();

		if (m_bTMP) // if observed temperature data are used
		{
			m_f_lb		= P("TFAC_lb")					->asDouble();
			m_f_ub		= P("TFAC_ub")					->asDouble();
		}
		if (m_IHAC_version == 1)
		{
			m_l_lb		= P("SMI_L_lb")					->asDouble();
			m_l_ub		= P("SMI_L_ub")					->asDouble();
			m_p_lb		= P("SM_P_lb")					->asDouble();
			m_p_ub		= P("SM_P_ub")					->asDouble();
		}
		//-------------------------------------------------

		//-------------------------------------------------
		// assign linear module parameters
		//-------------------------------------------------
		switch (m_storconf)
		{
		case 0:
			m_a_lb		= P("AFAC_lb")				->asDouble();
			m_a_ub		= P("AFAC_ub")				->asDouble();
			m_b_lb		= P("BFAC_lb")				->asDouble();
			m_b_ub		= P("BFAC_ub")				->asDouble();
			m_delay		= P("DELAY")				->asInt();
			m_NSEmin	= P("NSEMIN")				->asDouble();
			break;
		case 1: // two parallel storages
			m_aq_lb		= P("AQ_lb")				->asDouble();
			m_aq_ub		= P("AQ_ub")				->asDouble();
			m_as_lb		= P("AS_lb")				->asDouble();
			m_as_ub		= P("AS_ub")				->asDouble();
			m_bq_lb		= P("BQ_lb")				->asDouble();
			m_bq_ub		= P("BQ_ub")				->asDouble();

			m_delay		= P("DELAY")				->asInt();
			m_obj_func	= P("OBJ_FUNC")				->asInt();
			m_NSEmin	= P("NSEMIN")				->asDouble();
			break;
		case 2: // two storages in series
			break;
		} // end switch
		//-------------------------------------------------

		if (m_bSnowModule)
		{
			m_T_Rain_lb	= P("T_RAIN_lb")			->asDouble();
			m_T_Rain_ub	= P("T_RAIN_ub")			->asDouble();
			m_T_Melt_lb	= P("T_MELT_lb")			->asDouble();
			m_T_Melt_ub	= P("T_MELT_ub")			->asDouble();
			m_DD_FAC_lb	= P("DD_FAC_lb")			->asDouble();
			m_DD_FAC_ub	= P("DD_FAC_ub")			->asDouble();
		}
		return(true);
	} else {
		return(false);
	}
}
