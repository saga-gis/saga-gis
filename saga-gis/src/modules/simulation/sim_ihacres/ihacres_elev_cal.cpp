/**********************************************************
 * Version $Id: ihacres_elev_cal.cpp 1261 2011-12-16 15:12:15Z oconrad $
 *********************************************************/
///////////////////////////////////////////////////////////
//                    ihacres_elev_cal.cpp               //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2008-01-08                        //
///////////////////////////////////////////////////////////

#include "ihacres_elev_cal.h"
#include "convert_sl.h"
#include "model_tools.h"

#include <stdlib.h> // random numbers
#include <time.h>	// random numbers

// TEST OUTPUT ONLY
#include <fstream>

//---------------------------------------------------------------------

Cihacres_elev_cal::Cihacres_elev_cal()
{
	//-----------------------------------------------------
	// 1. Info...
	Set_Name(_TL("IHACRES Elevation Bands Calibration"));

	Set_Author(SG_T("copyrights (c) 2008 Stefan Liersch"));

	Set_Description(_TW("The Rainfall-Runoff Model IHACRES \n \n \n"
		"Reference: \n \n"
		"Jakeman, A.J. / Hornberger, G.M. (1993). \n"
		"How Much Complexity Is Warranted in a Rainfall-Runoff Model? \n"
		"Water Resources Research, (29), NO. 8 (2637-2649) \n \n"   
		"Kokkonen, T. S. et al. (2003). \n"
		"Predicting daily flows in ungauged catchments:"
		"model regionalization from catchment descriptors"
		"at the Coweeta Hydrologic Laboratory, North Carolina \n "
		"Hydrological Processes (17), 2219-2238 \n \n"
		"Croke, B. F. W., W. S. Merritt, et al. (2004).\n"
		"A dynamic model for predicting hydrologic response"
		"to land cover changes in gauged and"
		"ungauged catchments. \n"
		"Journal Of Hydrology 291(1-2): 115-131."
	));

	Parameters.Add_Table_Output(
		NULL	, "TABLEout"	, _TL("Table"),
		_TL("")
	);
	Parameters.Add_Table_Output(
		NULL	, "TABLEparms"	, _TL("Table"),
		_TL("")
	);

	///////////////////////////////////////////////////////////////////
	// FIRST MODULE DIALOG
	_CreateDialog1();
	///////////////////////////////////////////////////////////////////
}

Cihacres_elev_cal::~Cihacres_elev_cal()
{}
//---------------------------------------------------------------------

bool Cihacres_elev_cal::On_Execute()
{
	double		NSE_temp	= 0.0;
	double		NSE_max		= -9999.9;
	string		nse, nse_text;

	CSG_Parameters P;
	//std::ofstream f("_out_elev.txt");

	// Assign Parameters from first Module Dialog
	//---------------------------------------------------------
	int eb			= Parameters("NELEVBANDS")		->asInt();
	m_nElevBands	= eb + 2; // because list starts with 2 !
	m_nsim			= Parameters("NSIM")			->asInt();
	m_Area_tot		= Parameters("AREA_tot")		->asDouble();
	m_IHAC_version	= Parameters("IHACVERS")		->asInt();
	m_StorConf		= Parameters("STORAGE")			->asInt();
	m_bSnowModule	= Parameters("SNOW_MODULE")		->asBool();
	m_obj_func		= Parameters("OBJ_FUNC")		->asInt();
	m_NSEmin		= Parameters("NSEMIN")			->asDouble();

	//---------------------------------------------------------

	//---------------------------------------------------------
	// Assign number of storages
	m_nStorages = ihacres.Assign_nStorages(m_StorConf);
	//---------------------------------------------------------

	//---------------------------------------------------------
	// Initialize pointers
	_Init_ElevBands(m_nElevBands);
	m_p_linparms	= new C_IHAC_LinearParms(m_nElevBands,m_nStorages);
	m_p_lin_lb		= new C_IHAC_LinearParms(m_nElevBands,m_nStorages);
	m_p_lin_ub		= new C_IHAC_LinearParms(m_nElevBands,m_nStorages);
	m_p_nonlinparms = new C_IHAC_NonLinearParms(m_nElevBands);
	m_p_nl_lb		= new C_IHAC_NonLinearParms(m_nElevBands);
	m_p_nl_ub		= new C_IHAC_NonLinearParms(m_nElevBands);
	//---------------------------------------------------------

	//---------------------------------------------------------
	// open second and third user dialog
	if ( _CreateDialog2() && _CreateDialog3())
	{
		//---------------------------------------------------------
		// searching the first and the last record of the time range
		ihacres.AssignFirstLastRec(*m_p_InputTable, m_first, m_last, m_date1, m_date2, m_dateField);
		m_nValues = m_last - m_first + 1;
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		_Init_Pointers(m_nValues);
		//---------------------------------------------------------

		//---------------------------------------------------------
		// read input table
		_ReadInputFile();
		//---------------------------------------------------------

		//---------------------------------------------------------
		// PERFORM STREAMFLOW SIMULATION
		// FOR EACH ELEVATION BAND
		//---------------------------------------------------------
		// Convert Streamflow vector from m3/s*day-1 to mm/day
		m_p_Q_obs_mmday = model_tools::m3s_to_mmday(m_p_Q_obs_m3s, m_p_Q_obs_mmday, m_nValues, m_Area_tot);
		
		//---------------------------------------------------------
		m_pTable_parms = SG_Create_Table();
		_CreateTableParms();
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
			_Simulate_NonLinearModule();

			_Simulate_Streamflow();

			_Sum_Streamflow();

			m_NSE = m_NSE_lowflow = m_NSE_highflow = m_PBIAS = 0;
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

				_WriteTableParms();
			}
		}
		//---------------------------------------------------------

		m_pTable_parms->Set_Name(SG_T("ihacres_elevbands_cal"));
		Parameters("TABLEparms")->Set_Value(m_pTable_parms);

		//m_pTable = SG_Create_Table();
		//_CreateTableSim();
		//// add tables to SAGA Workspace
		//m_pTable->Set_Name("ihacres_elevBands_output");
		//Parameters("TABLEout")->Set_Value(m_pTable);

		delete[] m_p_elevbands; // sämtliche Unter-Pointer noch löschen
		delete[] m_p_pcpField;
		delete[] m_p_tmpField;
		delete[] m_p_Q_obs_m3s;
		delete[] m_p_Q_obs_mmday;
		delete[] m_p_Q_sim_mmday;
		delete m_p_linparms;
		delete m_p_nonlinparms;
		delete m_p_lin_lb;
		delete m_p_lin_ub;
		delete m_p_nl_lb;
		delete m_p_nl_ub;
		if (m_bSnowModule) {
			delete[] m_pSnowparms;
			delete[] m_pSnowparms_lb;
			delete[] m_pSnowparms_ub;
		}
		delete[] m_vq;
		delete[] m_vs;
		
		return(true);
	} // end if ( _CreateDialog2() )

		// delete[] m_p_elevbands;
		// delete[] m_p_pcpFields;
		// delete[] m_p_tmpFields;
		// delete m_p_linparms;
		// delete m_p_nonlinparms;
	return(false);
}

//---------------------------------------------------------------------

void Cihacres_elev_cal::_Init_ElevBands(int nvals)
{
	// instantiate elevation bands
	m_p_elevbands = new Cihacres_elev_bands[nvals];

	// instantiate field numbers
	m_p_pcpField = new int[nvals];
	m_p_tmpField = new int[nvals];

	if (m_bSnowModule)
	{
		m_pSnowparms	= new CSnowParms[nvals];
		m_pSnowparms_lb	= new CSnowParms[nvals];
		m_pSnowparms_ub	= new CSnowParms[nvals];
	}

	m_vq = new double[nvals];
	m_vs = new double[nvals];
}
//---------------------------------------------------------------------

void Cihacres_elev_cal::_Init_Pointers(int nvals)
{
	m_vec_date.resize(nvals);
	m_p_Q_obs_m3s = new double[nvals];
	m_p_Q_obs_mmday = new double[nvals];
	m_p_Q_sim_mmday = new double[nvals];
	
	for (int eb = 0; eb < m_nElevBands; eb++)
	{
		m_p_elevbands[eb].m_p_pcp = new double[nvals];
		m_p_elevbands[eb].m_p_tmp = new double[nvals];
		m_p_elevbands[eb].m_p_ER = new double[nvals];
		m_p_elevbands[eb].m_p_streamflow_sim = new double[nvals];
		m_p_elevbands[eb].m_p_Tw = new double[nvals];
		m_p_elevbands[eb].m_p_WI = new double[nvals];
	
		if (m_bSnowModule) {
			m_p_elevbands[eb].m_p_SnowStorage = new double[nvals];
			m_p_elevbands[eb].m_p_MeltRate = new double[nvals];
		}
	}
}
//---------------------------------------------------------------------

void Cihacres_elev_cal::_ReadInputFile()
{
	for (int j = 0, k = m_first; j < m_nValues, k < m_last + 1; j++, k++)
	{
		m_vec_date[j].append(CSG_String(m_p_InputTable->Get_Record(k)->asString(m_dateField)));
		m_p_Q_obs_m3s[j] = m_p_InputTable->Get_Record(k)->asDouble(m_streamflowField);
		
		for (int eb = 0; eb < m_nElevBands; eb++)
		{
			m_p_elevbands[eb].m_p_pcp[j] = m_p_InputTable->Get_Record(k)->asDouble(m_p_pcpField[eb]);
			m_p_elevbands[eb].m_p_tmp[j] = m_p_InputTable->Get_Record(k)->asDouble(m_p_tmpField[eb]);
		}
	}
}
//---------------------------------------------------------------------

void Cihacres_elev_cal::_CalcSnowModule(int eb)
{
	m_p_SnowModule = new CSnowModule(m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_pcp, m_nValues,
									m_pSnowparms[eb].T_Rain, m_pSnowparms[eb].T_Melt, m_pSnowparms[eb].DD_FAC);

	m_p_elevbands[eb].m_p_MeltRate = m_p_SnowModule->Get_MeltRate(m_p_elevbands[eb].m_p_MeltRate, m_nValues);
	m_p_elevbands[eb].m_p_SnowStorage = m_p_SnowModule->Get_SnowStorage(m_p_elevbands[eb].m_p_SnowStorage, m_nValues);

	delete m_p_SnowModule;
}
//---------------------------------------------------------------------

void Cihacres_elev_cal::_Simulate_NonLinearModule()
{
	double eR_init = 0.0;
	for (int eb = 0; eb < m_nElevBands; eb++)
	{
		//-------------------------------------------------------------
		// Assign random values
		//-------------------------------------------------------------
		if (m_bSnowModule)
		{
			m_pSnowparms[eb].T_Rain	= model_tools::Random_double(m_pSnowparms_lb[eb].T_Rain,m_pSnowparms_ub[eb].T_Rain);
			m_pSnowparms[eb].T_Melt	= model_tools::Random_double(m_pSnowparms_lb[eb].T_Melt,m_pSnowparms_ub[eb].T_Melt);
			m_pSnowparms[eb].DD_FAC = model_tools::Random_double(m_pSnowparms_lb[eb].DD_FAC,m_pSnowparms_ub[eb].DD_FAC);

			m_p_SnowModule = new CSnowModule(m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_pcp, m_nValues,
				m_pSnowparms[eb].T_Rain, m_pSnowparms[eb].T_Melt, m_pSnowparms[eb].DD_FAC);

			m_p_elevbands[eb].m_p_MeltRate = m_p_SnowModule->Get_MeltRate(m_p_elevbands[eb].m_p_MeltRate, m_nValues);
			delete m_p_SnowModule;
		}
	
		m_p_nonlinparms->mp_tw[eb]	= model_tools::Random_double(m_p_nl_lb->mp_tw[eb], m_p_nl_ub->mp_tw[eb]);
		m_p_nonlinparms->mp_f[eb]	= model_tools::Random_double(m_p_nl_lb->mp_f[eb], m_p_nl_ub->mp_f[eb]);
		m_p_nonlinparms->mp_c[eb]	= model_tools::Random_double(m_p_nl_lb->mp_c[eb], m_p_nl_ub->mp_c[eb]);

		if (m_IHAC_version == 1) { // Croke etal. (2005)
			m_p_nonlinparms->mp_l[eb]= model_tools::Random_double(m_p_nl_ub->mp_l[eb],m_p_nl_ub->mp_l[eb]);
			m_p_nonlinparms->mp_p[eb]= model_tools::Random_double(m_p_nl_ub->mp_p[eb],m_p_nl_ub->mp_p[eb]);
		}

		//----------------------------------------------------------
		// calculate excess rainfall time series
		//----------------------------------------------------------
		switch(m_IHAC_version)
		{
		case 0: // Jakeman & Hornberger (1993)
			// The parameter index (fourth parameter) is zero here, because 
			// the parameter settings of the non-linear module are in all elevationbands equal.
			// If they should be different the index parameter can be used to identify the
			// corresponding elevation band.
			ihacres.CalcWetnessTimeConst(m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_Tw,
					m_p_nonlinparms, eb, m_nValues); 
					// 0 = index (only one instance of m_p_nonlinparms)

			if (m_bSnowModule)
			{
				ihacres.CalcWetnessIndex(m_p_elevbands[eb].m_p_Tw, m_p_elevbands[eb].m_p_pcp,
					m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_WI, 0.5, m_p_nonlinparms->mp_c[eb],
					m_bSnowModule, m_pSnowparms[eb].T_Rain, m_nValues);

				ihacres.CalcExcessRain(m_p_elevbands[eb].m_p_pcp, m_p_elevbands[eb].m_p_tmp,
					m_p_elevbands[eb].m_p_WI, m_p_elevbands[eb].m_p_ER, eR_init,
					m_p_elevbands[eb].m_sum_eRainGTpcp, m_nValues, m_bSnowModule,
					m_pSnowparms[eb].T_Rain, m_pSnowparms[eb].T_Melt,
					m_p_elevbands[eb].m_p_MeltRate);
			} else {

				ihacres.CalcWetnessIndex(m_p_elevbands[eb].m_p_Tw, m_p_elevbands[eb].m_p_pcp,
					m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_WI, 0.5, m_p_nonlinparms->mp_c[eb],
					m_bSnowModule, 0, m_nValues);

				ihacres.CalcExcessRain(m_p_elevbands[eb].m_p_pcp, m_p_elevbands[eb].m_p_tmp,
					m_p_elevbands[eb].m_p_WI, m_p_elevbands[eb].m_p_ER, eR_init,
					m_p_elevbands[eb].m_sum_eRainGTpcp, m_nValues, m_bSnowModule,
					0,0,0);
			}
			break;
		case 1: // Croke et al. (2005)
			ihacres.CalcWetnessTimeConst_Redesign(m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_Tw,
				m_p_nonlinparms, eb, m_nValues);	// 0 = index (only one instance of m_p_nonlinparms)

			if (m_bSnowModule)
			{
				ihacres.CalcWetnessIndex_Redesign(m_p_elevbands[eb].m_p_Tw, m_p_elevbands[eb].m_p_pcp, m_p_elevbands[eb].m_p_WI, 0.5,
					m_bSnowModule, m_pSnowparms[eb].T_Rain, m_nValues);

				ihacres.CalcExcessRain_Redesign(m_p_elevbands[eb].m_p_pcp, m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_WI, 
					m_p_elevbands[eb].m_p_ER, eR_init, m_p_elevbands[eb].m_sum_eRainGTpcp, m_nValues, 
					m_p_nonlinparms->mp_c[eb], m_p_nonlinparms->mp_l[eb], m_p_nonlinparms->mp_p[eb],
					m_bSnowModule, m_pSnowparms[eb].T_Rain, m_pSnowparms[eb].T_Melt, m_p_elevbands[eb].m_p_MeltRate);
			} else {
				ihacres.CalcWetnessIndex_Redesign(m_p_elevbands[eb].m_p_Tw, m_p_elevbands[eb].m_p_pcp, m_p_elevbands[eb].m_p_WI, 0.5,
					m_bSnowModule, 0, m_nValues);

				ihacres.CalcExcessRain_Redesign(m_p_elevbands[eb].m_p_pcp, m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_WI, 
					m_p_elevbands[eb].m_p_ER, eR_init, m_p_elevbands[eb].m_sum_eRainGTpcp, m_nValues, 
					m_p_nonlinparms->mp_c[eb], m_p_nonlinparms->mp_l[eb], m_p_nonlinparms->mp_p[eb],
					m_bSnowModule, 0,0,0);
			}
			break;
		} // end switch(m_IHAC_vers)
	}
}
//---------------------------------------------------------------------

void Cihacres_elev_cal::_Simulate_Streamflow()
{
	//-------------------------------------------------------------
	// Assign random values
	//-------------------------------------------------------------
	for (int eb = 0; eb < m_nElevBands; eb++)
	{
		switch(m_StorConf)
		{
		case 0: // single storage
			m_p_linparms->a[eb]	= model_tools::Random_double(m_p_lin_lb->a[eb],m_p_lin_ub->a[eb]);
			m_p_linparms->b[eb]	= model_tools::Random_double(m_p_lin_lb->b[eb],m_p_lin_ub->b[eb]);
			break;
		case 1: // two storages in parallel
			do
			{
				m_p_linparms->aq[eb] = model_tools::Random_double(m_p_lin_lb->aq[eb], m_p_lin_ub->aq[eb]);
				m_p_linparms->as[eb] = model_tools::Random_double(m_p_lin_lb->as[eb], m_p_lin_ub->as[eb]);
				m_p_linparms->bq[eb] = model_tools::Random_double(m_p_lin_lb->bq[eb], m_p_lin_ub->bq[eb]);
				// Calculate parameter m_vq to check parms aq and bq
				// Equation after Jakeman & Hornberger (1993)
				m_vq[eb] = m_p_linparms->bq[eb] / ( 1 + m_p_linparms->aq[eb] );
			}
			while (m_vq[eb] < 0.0 || m_vq[eb] > 1.0);
			m_p_linparms->bs[eb] = ihacres.Calc_Parm_BS(m_p_linparms->aq[eb], m_p_linparms->as[eb], m_p_linparms->bq[eb]);
			break;
		}
	

		//----------------------------------------------------------
		// calculate streamflow
		//----------------------------------------------------------
		switch(m_StorConf)
		{
		case 0: // single storage
			ihacres.SimStreamflowSingle(m_p_elevbands[eb].m_p_ER, m_p_Q_obs_mmday[0],
				m_p_elevbands[eb].m_p_streamflow_sim, m_delay,
				m_p_linparms->a[eb], m_p_linparms->b[eb], m_nValues);
			break;
		case 1: // two storages in parallel
			ihacres.SimStreamflow2Parallel(m_p_elevbands[eb].m_p_ER,
				m_p_elevbands[eb].m_p_streamflow_sim, m_p_Q_obs_mmday[0],
				m_p_linparms, eb, m_vq[eb], m_vs[eb], m_nValues, m_delay);
			break;
		case 2: // two storages in series
			break;
		} // end switch(m_StorConf)
	}// end for (int eb...
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//		Summarize streamflow from elevation bands
//---------------------------------------------------------------------
void Cihacres_elev_cal::_Sum_Streamflow()
{
	//std::ofstream f("_cal_elev.txt");
	
	double sum = 0.0;
	for (int n = 0; n < m_nValues; n++)
	{
		for (int eb = 0; eb < m_nElevBands; eb++) {
			sum += m_p_elevbands[eb].m_p_streamflow_sim[n] * m_p_elevbands[eb].m_area / m_Area_tot;
			//f << m_p_elevbands[eb].m_p_streamflow_sim[n] << ", ");
		}
		//f << std::endl;
		m_p_Q_sim_mmday[n] = sum;
		sum = 0.0;
	}
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//		Nash-Sutcliffe efficiency
//---------------------------------------------------------------------
void Cihacres_elev_cal::_CalcEfficiency()
{
	m_NSE			= model_tools::CalcEfficiency(m_p_Q_obs_mmday, m_p_Q_sim_mmday, m_nValues);
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
void Cihacres_elev_cal::_CreateTableParms()
{
	char c[12];

	// creating the column titles
	m_pTable_parms->Add_Field("NSE",				SG_DATATYPE_Double);
	m_pTable_parms->Add_Field("NSE_high",			SG_DATATYPE_Double);
	m_pTable_parms->Add_Field("NSE_low",			SG_DATATYPE_Double);
	m_pTable_parms->Add_Field("PBIAS",				SG_DATATYPE_Double);
	//m_pTable_parms->Add_Field("eR_ovest",			SG_DATATYPE_Double);

	for (int i = 0; i < m_nElevBands; i++)
	{
		sprintf(c,"%s_%d","vq",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		sprintf(c,"%s_%d","vs",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		sprintf(c,"%s_%d","T(q)",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		sprintf(c,"%s_%d","T(s)",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		sprintf(c,"%s_%d","Tw",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		sprintf(c,"%s_%d","f",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		sprintf(c,"%s_%d","c",i+1);
		m_pTable_parms->Add_Field(c,					SG_DATATYPE_Double);
		if ( m_IHAC_version == 1 ) // Croke etal. (2005)
		{
			sprintf(c,"%s_%d","l",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","p",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
		}
		if (m_bSnowModule)
		{
			sprintf(c,"%s_%d","T_Rain",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","T_Melt",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","DD_FAC",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
		}
		switch(m_StorConf)
		{
		case 0: // single
			sprintf(c,"%s_%d","a",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","b",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			break;
		case 1: // two storages in parallel
			sprintf(c,"%s_%d","aq",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","as",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","bq",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			sprintf(c,"%s_%d","bs",i+1);
			m_pTable_parms->Add_Field(c,				SG_DATATYPE_Double);
			break;
		}
	}
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//		Create output table
//---------------------------------------------------------------------
void Cihacres_elev_cal::_WriteTableParms()
{
	int					field = 0;
	CSG_Table_Record	*pRecord;

	// add a new record to the table
	m_pTable_parms->Add_Record();
	pRecord	= m_pTable_parms->Get_Record(m_counter);

	// writing the data to the current row
	pRecord->Set_Value(field,m_NSE); field++;
	pRecord->Set_Value(field,m_NSE_highflow); field++;
	pRecord->Set_Value(field,m_NSE_lowflow); field++;
	pRecord->Set_Value(field,m_PBIAS); field++;
	//pRecord->Set_Value(field,m_sum_eRainGTpcp); field++;

	for (int eb = 0; eb < m_nElevBands; eb++)
	{
		pRecord->Set_Value(field,m_vq[eb]); field++;
		pRecord->Set_Value(field,m_vs[eb]); field++;
		pRecord->Set_Value(field,ihacres.Calc_TimeOfDecay(m_p_linparms->aq[eb])); field++;
		pRecord->Set_Value(field,ihacres.Calc_TimeOfDecay(m_p_linparms->as[eb])); field++;
		pRecord->Set_Value(field,m_p_nonlinparms->mp_tw[eb]); field++;
		pRecord->Set_Value(field,m_p_nonlinparms->mp_f[eb]); field++;
		pRecord->Set_Value(field,m_p_nonlinparms->mp_c[eb]); field++;
		if ( m_IHAC_version == 1 )
		{
			pRecord->Set_Value(field,m_p_nonlinparms->mp_l[eb]); field++;
			pRecord->Set_Value(field,m_p_nonlinparms->mp_p[eb]); field++;
		}
		if ( m_bSnowModule )
		{
			pRecord->Set_Value(field,m_pSnowparms[eb].T_Rain); field++;
			pRecord->Set_Value(field,m_pSnowparms[eb].T_Melt); field++;
			pRecord->Set_Value(field,m_pSnowparms[eb].DD_FAC); field++;
		}
		switch ( m_StorConf )
		{
		case 0: // single storage
			pRecord->Set_Value(field,m_p_linparms->a[eb]); field++;
			pRecord->Set_Value(field,m_p_linparms->b[eb]); field++;
			break;
		case 1:
			pRecord->Set_Value(field,m_p_linparms->aq[eb]); field++;
			pRecord->Set_Value(field,m_p_linparms->as[eb]); field++;
			pRecord->Set_Value(field,m_p_linparms->bq[eb]); field++;
			pRecord->Set_Value(field,m_p_linparms->bs[eb]); field++;
			break;
		}
	}
	m_counter++;
}
//---------------------------------------------------------------------



//---------------------------------------------------------------------
void Cihacres_elev_cal::_CreateTableSim()
{
	int i = 0; // used in function Get_Record(i)
	CSG_Table_Record	*pRecord;
	CSG_String			tmpName;
	double				sim_eb, sim;

	// creating the column titles
	m_pTable->Add_Field("Date",		SG_DATATYPE_String);
	m_pTable->Add_Field("Flow_OBS",	SG_DATATYPE_Double);

	for (int eb = 0; eb < m_nElevBands; eb++)
	{
		tmpName = SG_T("ELEVB_");
		tmpName += convert_sl::Int2String(eb+1).c_str();
		m_pTable->Add_Field(tmpName.c_str(),	SG_DATATYPE_Double);
	}
	m_pTable->Add_Field("Flow_SIM",	SG_DATATYPE_Double);

	for (int j = 0; j < m_nValues; j++)
	{
		m_pTable->Add_Record();
		pRecord = m_pTable->Get_Record(i);

		// writing the data into the rows
		pRecord->Set_Value(0,CSG_String(m_vec_date[j].c_str()));
		pRecord->Set_Value(1,m_p_Q_obs_m3s[j]);
		sim_eb = 0.0;
		sim = 0.0;
		for (int eb = 0; eb < m_nElevBands; eb++)
		{
			sim_eb = model_tools::mmday_to_m3s(m_p_elevbands[eb].m_p_streamflow_sim[j],m_p_elevbands[eb].m_area);
			//pRecord->Set_Value(3+eb,model_tools::mmday_to_m3s(m_p_elevbands[eb].m_p_streamflow_sim[j],m_p_elevbands[eb].m_area));
			pRecord->Set_Value(2+eb, sim_eb);
			sim += sim_eb;
		}
		pRecord->Set_Value(2+m_nElevBands,sim);
		i++;
	}
}


//---------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
//
//		                          DIALOGS
//
///////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
// DIALOG 1
//---------------------------------------------------------------------
void Cihacres_elev_cal::_CreateDialog1()
{
	CSG_Parameter	*pNode;
	CSG_String		s;

	pNode = Parameters.Add_Choice(
		NULL	, "NELEVBANDS"		, SG_T("Number of elevation bands"),
		_TL(""),
		SG_T("2|3|4|5|6|7|8|9|10")
	);

	pNode = Parameters.Add_Value(
		NULL,	"NSIM",	_TL("Number of Simulations"),
		_TL("Number of Simulations for Calibration"),
		PARAMETER_TYPE_Int,
		1000, 1, true, 10000000, true
	);

	pNode = Parameters.Add_Value(
		pNode,	"AREA_tot", _TL("Total Catchment Area [km2]"),
			_TL(""),
			PARAMETER_TYPE_Double
	);

	s.Printf(SG_T("Node1"), 1);
	pNode = Parameters.Add_Node(NULL,s,SG_T("IHACRES Version"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "IHACVERS"		, SG_T("IHACRES Version"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Jakeman & Hornberger (1993)"),	//  0
			_TL("Croke et al. (2005) !!! not yet implemented !!!")	//	1
		)
	);

	s.Printf(SG_T("Node2"), 2);
	pNode = Parameters.Add_Node(NULL,s,SG_T("Storage Configuration"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "STORAGE"		, SG_T("Storage"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Single Storage"),			//  0
			_TL("Two Parallel Storages"),	//  1 
			_TL("Two Storages in Series !!! not yet implemented !!!")	//  2 
		)
	);

	Parameters.Add_Value(
		pNode,	"SNOW_MODULE",	_TL("Using the snow-melt module?"),
		_TL("If checked, snow-melt module is used."),
		PARAMETER_TYPE_Bool, false
	);

	s.Printf(SG_T("Node6"), 6);
	pNode = Parameters.Add_Node(NULL,s,SG_T("Nash-Sutcliffe Efficiency"),_TL(""));

	Parameters.Add_Choice(
		pNode, "OBJ_FUNC"		, SG_T("Objective Function"),
		_TL(""),
		SG_T("NSE|NSE high flow|NSE low flow")
	);

	Parameters.Add_Value(
		pNode,	"NSEMIN",	SG_T("Minimum Nash-Sutcliffe Efficiency"),
		SG_T("Minimum Nash-Sutcliffe Efficiency required to print simulation to calibration table"),
		PARAMETER_TYPE_Double,
		0.7, 0.1, true, 1.0, true
	);
}
//---------------------------------------------------------------------
// DIALOG 2
//---------------------------------------------------------------------

bool Cihacres_elev_cal::_CreateDialog2()
{
	int		i;

	//std::ofstream f("_out_elev.txt");

	CSG_Parameters	P;  // used to add Parameters in the second dialog
	CSG_Parameter	*pNode, *pNode1;
	CSG_String		s;
	CSG_String		tmpNode, tmpName;

	P.Set_Name(_TL("IHACRES Elevation Bands (Dialog 2)"));
	// Input file ----------------------------------------------
	pNode = P.Add_Table(
		NULL	, "TABLE"	, _TL("IHACRES Input Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	P.Add_Table_Field(
		pNode	, "DATE_Field"	, _TL("Date Column"),
		SG_T("Select the column containing the Date")
	);

	P.Add_Table_Field(
		pNode	, "DISCHARGE_Field"	, _TL("Streamflow (obs.) Column"),
		SG_T("Select the column containing the observed streamflow time series")
	);
	
	for (i = 0; i < m_nElevBands; i++)
	{
		tmpNode = convert_sl::Int2String(i+1).c_str();
		//s.Printf(tmpNode.c_str(), i);
		//pNode1 = P.Add_Node(NULL,s,SG_T("Elevation Band Input",_TL(""));

		tmpName = SG_T("PCP Column: Elevation Band: ");
		tmpName+=tmpNode;
		P.Add_Table_Field(
			pNode	, tmpName.c_str(), tmpName.c_str(),
			SG_T("Select Precipitation Column")
		);

		tmpName = SG_T("TMP Column: Elevation Band: ");
		tmpName+=tmpNode;
		P.Add_Table_Field(
			pNode	, tmpName.c_str()	, tmpName.c_str(),
			SG_T("Select Temperature Column")
		);
	}
	// Input file ----------------------------------------------

	for (i = 0; i < m_nElevBands; i++)
	{
		tmpNode = SG_T("Node");
		tmpNode+=convert_sl::Int2String(i+100).c_str();
		tmpName = SG_T("Elevation Band ");
		tmpName+=convert_sl::Int2String(i+1).c_str();

		s.Printf(tmpNode.c_str(), i+100);
		pNode = P.Add_Node(NULL,s,tmpName.c_str(),_TL(""));
		
		tmpName = SG_T("Area [km2] Elev(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode,	tmpName, _TL("Area [km2]"),
			_TL(""),
			PARAMETER_TYPE_Double
		);

		tmpName = SG_T("Mean Elevation [m.a.s.l] Elev(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode,	tmpName, _TL("Mean Elevation [m.a.s.l]"),
			_TL(""),
			PARAMETER_TYPE_Double
		);


		// Parameters of non-linear module -------------------------

		tmpNode = SG_T("Node");
		tmpNode+=convert_sl::Int2String(i+150).c_str();
		s.Printf(tmpNode.c_str(), i+150);
		pNode1 = P.Add_Node(pNode,s,SG_T("Non-Linear Module"),_TL(""));

		tmpName = SG_T("TwFAC_lb(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	_TL("(Tw) wetness decline time constant [lower bound]"),
			_TW("Tw is approximately the time constant, or inversely,"
			"the rate at which the catchment wetness declines in the absence of rainfall"),
			PARAMETER_TYPE_Double,
			1.0, 0.01, true, 150.0, true
		);
		tmpName = SG_T("TwFAC_ub(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	_TL("(Tw) wetness decline time constant [upper bound]"),
			_TW("Tw is approximately the time constant, or inversely,"
			"the rate at which the catchment wetness declines in the absence of rainfall"),
			PARAMETER_TYPE_Double,
			1.0, 0.01, true, 150.0, true
			);

		tmpName = SG_T("TFAC_lb(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1, tmpName, SG_T("(f) Temperature Modulation Factor [lower bound]"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			0.05, 0.0001, true, 5.0, true
		);

		tmpName = SG_T("TFAC_ub(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1, tmpName, SG_T("(f) Temperature Modulation Factor [upper bound]"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			0.5, 0.0001, true, 5.0, true
		);
	
		tmpName = SG_T("CFAC_lb(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,tmpName,	_TL("(c) Parameter [lower bound]"),
			_TL("Parameter (c) to fit streamflow volume"),
			PARAMETER_TYPE_Double,
			0.001, 0.0, true, 1.0, true
		);
		tmpName = SG_T("CFAC_ub(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,tmpName,	_TL("(c) Parameter [upper bound]"),
			_TL("Parameter (c) to fit streamflow volume"),
			PARAMETER_TYPE_Double,
			0.01, 0.0, true, 1.0, true
		);

		switch(m_IHAC_version)
		{
		case 0: // Jakeman & Hornberger (1993)
			break;
		case 1: // Croke et al. (2005)
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+200).c_str();
			s.Printf(tmpNode.c_str(), i+200);
			pNode1 = P.Add_Node(pNode,s,SG_T("Soil Moisture Power Eq."),_TL(""));

			tmpName = SG_T("LFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName, _TL("Parameter (l) [lower bound]"),
				_TL("Soil moisture index threshold"),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 5.0, true
			);
			tmpName = SG_T("LFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName, _TL("Parameter (l) [upper bound]"),
				_TL("Soil moisture index threshold"),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 5.0, true
			);

			tmpName = SG_T("PFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Parameter (p) [lower bound]"),
				_TL("non-linear response term"),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 5.0, true
			);
			tmpName = SG_T("PFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Parameter (p) [upper bound]"),
				_TL("non-linear response term"),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 5.0, true
			);
			break;
		}
		// Parameters of non-linear module -------------------------

		// Parameters of linear module -----------------------------
		switch(m_StorConf)
		{
		case 0: // single storage
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+250).c_str();
			s.Printf(tmpNode.c_str(), i+250);
			pNode1 = P.Add_Node(pNode,s,SG_T("Linear Module"),_TL(""));

			tmpName = SG_T("AFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(a) [lower bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.8, -0.99, true, -0.01, true
			);
			tmpName = SG_T("AFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(a) [upper bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.8, -0.99, true, -0.01, true
			);

			tmpName = SG_T("BFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(b) [lower bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				0.2, 0.001, true, 1.0, true
			);
			tmpName = SG_T("BFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(b) [upper bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				0.2, 0.001, true, 1.0, true
			);
			break;

		case 1: // two parallel storages
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+250).c_str();
			s.Printf(tmpNode.c_str(), i+250);
			pNode1 = P.Add_Node(pNode,s,SG_T("Linear Module"),_TL(""));

			// Parameter a

			tmpName = SG_T("AQ_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(q) [lower bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.7, -0.99, true, -0.01, true
			);
			tmpName = SG_T("AQ_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(q) [upper bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.7, -0.99, true, -0.01, true
			);

			tmpName = SG_T("AS_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(s) [lower bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.9, -0.99, true, -0.01, true
			);
			tmpName = SG_T("AS_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(s) [upper bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.9, -0.99, true, -0.01, true
			);

			// Parameter b

			tmpName = SG_T("BQ_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("b(q) [lower bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 1.0, true
			);
			tmpName = SG_T("BQ_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("b(q) [upper bound]"),
				_TL(""),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 1.0, true
			);
			break;

		case 2: // two storages in series
			break;
		} // end switch (storconf)
		// Parameters of linear module -----------------------------

		tmpNode = SG_T("Node");
		tmpNode+=convert_sl::Int2String(i+300).c_str();
		s.Printf(tmpNode.c_str(), i+300);
		pNode1 = P.Add_Node(pNode,s,SG_T("Time Delay after Start of Rainfall (INTEGER)"),_TL(""));
			
		tmpName = SG_T("DELAY(");
			tmpName += tmpNode;
			tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	SG_T("Time Delay (Rain-Runoff)"),
			SG_T("The delay after the start of rainfall, before the discharge starts to rise."),
			PARAMETER_TYPE_Int,
			0, 1, true, 100, true
		);

		// snow module parameters ----------------------------------
		if (m_bSnowModule)
		{
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+350).c_str();
			s.Printf(tmpNode.c_str(), i+350);
			pNode1 = P.Add_Node(pNode,s,SG_T("Snow Module Parameters"),_TL(""));
			
			tmpName = SG_T("T_RAIN_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	SG_T("Temperature Threshold for Rainfall [lower bound]"),
				SG_T("Below this threshold precipitation will fall as snow"),
				PARAMETER_TYPE_Double,
				-1.0, -10.0, true, 10.0, true
			);
			tmpName = SG_T("T_RAIN_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	SG_T("Temperature Threshold for Rainfall [upper bound]"),
				SG_T("Below this threshold precipitation will fall as snow"),
				PARAMETER_TYPE_Double,
				-1.0, -10.0, true, 10.0, true
			);

			tmpName = SG_T("T_MELT_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	SG_T("Temperature Threshold for Melting [lower bound]"),
				SG_T("Above this threshold snow will start to melt"),
				PARAMETER_TYPE_Double,
				1.0, -5.0, true, 10.0, true
			);
			tmpName = SG_T("T_MELT_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	SG_T("Temperature Threshold for Melting [upper bound]"),
				SG_T("Above this threshold snow will start to melt"),
				PARAMETER_TYPE_Double,
				1.0, -5.0, true, 10.0, true
			);

			tmpName = SG_T("DD_FAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	SG_T("Day-Degree Factor [lower bound]"),
				SG_T("Day-Degree Factor depends on catchment characteristics"),
				PARAMETER_TYPE_Double,
				0.7, 0.7, true, 9.2, true
			);
			tmpName = SG_T("DD_FAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	SG_T("Day-Degree Factor [upper bound]"),
				SG_T("Day-Degree Factor depends on catchment characteristics"),
				PARAMETER_TYPE_Double,
				0.7, 0.7, true, 9.2, true
			);
		}
		// snow module parameters ----------------------------------
	}	
	
	if( SG_UI_Dlg_Parameters(&P, _TL("IHACRES Distributed Input Dialog 2")) )
	{
		// input table
		m_p_InputTable		= P("TABLE")				->asTable();
		// field numbers
		m_dateField			= P("DATE_Field")			->asInt();
		m_streamflowField	= P("DISCHARGE_Field")		->asInt();
		for (int i = 0; i < m_nElevBands; i++)
		{
			tmpNode = convert_sl::Int2String(i+1).c_str();
			
			// get precipitation column of Elevation Band[i]
			tmpName = SG_T("PCP Column: Elevation Band: ");
			tmpName+=tmpNode;
			m_p_pcpField[i]			= P(tmpName)		->asInt();

			// get temperature column of Elevation Band[i]
			tmpName = SG_T("TMP Column: Elevation Band: ");
			tmpName+=tmpNode;
			m_p_tmpField[i]			= P(tmpName)		->asInt();

			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+100).c_str();

			// get area[km2] of Elevation Band[i]
			tmpName = SG_T("Area [km2] Elev(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_elevbands[i].m_area	= P(tmpName)		->asDouble();

			// get mean elevation of Elevation Band[i]
			tmpName = SG_T("Mean Elevation [m.a.s.l] Elev(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_elevbands[i].m_mean_elev =P(tmpName)	->asDouble();


			// non-linear module parameters
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+150).c_str();
			// get Tw
			tmpName = SG_T("TwFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nl_lb->mp_tw[i]			= P(tmpName)	->asDouble();

			tmpName = SG_T("TwFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nl_ub->mp_tw[i]			= P(tmpName)	->asDouble();

			// get f
			tmpName = SG_T("TFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nl_lb->mp_f[i]			= P(tmpName)	->asDouble();

			tmpName = SG_T("TFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nl_ub->mp_f[i]			= P(tmpName)	->asDouble();

			// get c
			tmpName = SG_T("CFAC_lb(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nl_lb->mp_c[i]			= P(tmpName)	->asDouble();

			tmpName = SG_T("CFAC_ub(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nl_ub->mp_c[i]			= P(tmpName)	->asDouble();

			switch(m_IHAC_version)
			{
			case 0: // Jakeman & Hornberger (1993)
				break;
			case 1: // Croke et al. (2005)
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+200).c_str();
				// get l
				tmpName = SG_T("LFAC_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nl_lb->mp_l[i]= P(tmpName)	->asDouble();

				tmpName = SG_T("LFAC_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nl_ub->mp_l[i]= P(tmpName)	->asDouble();

				// get p
				tmpName = SG_T("PFAC_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nl_lb->mp_p[i]= P(tmpName)	->asDouble();

				tmpName = SG_T("PFAC_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nl_ub->mp_p[i]= P(tmpName)	->asDouble();
			}

			// linear module parameters
			switch(m_nStorages)
			{
			case 1: // single storage
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+250).c_str();
				// get a
				tmpName = SG_T("AFAC_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_lb->a[i]		= P(tmpName)	->asDouble();

				tmpName = SG_T("AFAC_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_ub->a[i]		= P(tmpName)	->asDouble();

				// get b
				tmpName = SG_T("BFAC_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_lb->b[i]		= P(tmpName)	->asDouble();

				tmpName = SG_T("BFAC_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_ub->b[i]		= P(tmpName)	->asDouble();
				break;
			case 2: // two storages
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+250).c_str();
				// get aq
				tmpName = SG_T("AQ_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_lb->aq[i]		= P(tmpName)	->asDouble();

				tmpName = SG_T("AQ_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_ub->aq[i]		= P(tmpName)	->asDouble();

				// get bq
				tmpName = SG_T("BQ_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_lb->bq[i]		= P(tmpName)	->asDouble();

				tmpName = SG_T("BQ_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_ub->bq[i]		= P(tmpName)	->asDouble();

				// get as
				tmpName = SG_T("AS_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_lb->as[i]		= P(tmpName)	->asDouble();

				tmpName = SG_T("AS_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_lin_ub->as[i]		= P(tmpName)	->asDouble();
				break;
			}

			// get delay
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+300).c_str();
			tmpName = SG_T("DELAY(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_delay		= P(tmpName)					->asInt();

			if (m_bSnowModule)
			{
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+350).c_str();

				tmpName = SG_T("T_RAIN_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms_lb[i].T_Rain	= P(tmpName)	->asDouble();

				tmpName = SG_T("T_RAIN_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms_ub[i].T_Rain	= P(tmpName)	->asDouble();

				tmpName = SG_T("T_MELT_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms_lb[i].T_Melt	= P(tmpName)	->asDouble();

				tmpName = SG_T("T_MELT_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms_ub[i].T_Melt	= P(tmpName)	->asDouble();

				tmpName = SG_T("DD_FAC_lb(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms_lb[i].DD_FAC	= P(tmpName)	->asDouble();

				tmpName = SG_T("DD_FAC_ub(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms_ub[i].DD_FAC	= P(tmpName)	->asDouble();
			}

		} // end for (int i = 0; i < m_nSubbasins; i++)

		return(true);
	}
	return(false);
}

//---------------------------------------------------------------------
// DIALOG 3
//---------------------------------------------------------------------

bool Cihacres_elev_cal::_CreateDialog3()
{
	CSG_String		s;
	CSG_Parameters	P;  // used to add Parameters in the second dialog
	CSG_Parameter	*pNode;

	//	Dialog design
	P.Set_Name(_TL("Choose Time Range"));

	s.Printf(SG_T("Node1"), 1);
	pNode = P.Add_Node(NULL,s,SG_T("Time Range"),_TL(""));

	s.Printf(SG_T("FDAY") , 1-1);
	P.Add_String(pNode,s,_TL("First Day"),_TL(""),
				 m_p_InputTable->Get_Record(0)->asString(m_dateField));

	s.Printf(SG_T("LDAY") , 1-2);
	P.Add_String(pNode,s,_TL("Last Day"),_TL(""),
				 m_p_InputTable->Get_Record(m_p_InputTable->Get_Record_Count()-1)->asString(m_dateField));

	if( SG_UI_Dlg_Parameters(&P, _TL("Choose Time Range")) )
	{
		///////////////////////////////////////////////////////////////
		//
		//                ASSIGN DATA FROM SECOND DIALOG
		//
		///////////////////////////////////////////////////////////////
		m_date1		= P(CSG_String::Format(SG_T("FDAY"),m_dateField).c_str())->asString();
		m_date2		= P(CSG_String::Format(SG_T("LDAY"),m_streamflowField).c_str())->asString();
		return(true);
	}
	return(false);
}
