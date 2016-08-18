/**********************************************************
 * Version $Id: ihacres_elev.cpp 1261 2011-12-16 15:12:15Z oconrad $
 *********************************************************/
///////////////////////////////////////////////////////////
//                    ihacres_elev.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2008-01-08                        //
///////////////////////////////////////////////////////////

#include "ihacres_elev.h"
#include "convert_sl.h"
#include "model_tools.h"

// TEST OUTPUT ONLY
#include <fstream>

//---------------------------------------------------------------------

Cihacres_elev::Cihacres_elev()
{
	//-----------------------------------------------------
	// 1. Info...
	Set_Name(_TL("IHACRES Elevation Bands"));

	Set_Author(SG_T("copyrights (c) 2008 Stefan Liersch"));

	Set_Description(_TW("The Rainfall-Runoff Model IHACRES \n \n \n"
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

	Parameters.Add_Table_Output(
		NULL	, "TABLEout"	, _TL("Table"),
		_TL("")
	);

	///////////////////////////////////////////////////////////////////
	// FIRST TOOL DIALOG
	_CreateDialog1();
	///////////////////////////////////////////////////////////////////
}

Cihacres_elev::~Cihacres_elev()
{}
//---------------------------------------------------------------------

bool Cihacres_elev::On_Execute()
{
	CSG_Parameters P;
	//std::ofstream f("_out_elev.txt");

	// Assign Parameters from first Tool Dialog
	//---------------------------------------------------------
	int eb			= Parameters("NELEVBANDS")		->asInt();
	m_nElevBands	= eb + 2; // because list starts with 2 !
	m_Area_tot		= Parameters("AREA_tot")		->asDouble();
	m_IHAC_version	= Parameters("IHACVERS")		->asInt();
	m_StorConf		= Parameters("STORAGE")			->asInt();
	m_bSnowModule	= Parameters("SNOW_TOOL")		->asBool();
	//---------------------------------------------------------

	//---------------------------------------------------------
	// Assign number of storages
	m_nStorages = ihacres.Assign_nStorages(m_StorConf);
	//---------------------------------------------------------

	//---------------------------------------------------------
	// Initialize pointers
	_Init_ElevBands(m_nElevBands);
	m_p_linparms = new C_IHAC_LinearParms(m_nElevBands,m_nStorages);
	m_p_nonlinparms = new C_IHAC_NonLinearParms(m_nElevBands);
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
		// SNOW TOOL
		//---------------------------------------------------------
		for (int i = 0; i < m_nElevBands; i++)
		{
			if (m_bSnowModule)
			{
				_CalcSnowModule(i);
			}

			_Simulate_NonLinearModule(i);

			_Simulate_Streamflow(i);
		}

		//---------------------------------------------------------

		m_pTable = SG_Create_Table();
		_CreateTableSim();
		// add tables to SAGA Workspace
		m_pTable->Set_Name(SG_T("IHACRES_ElevBands_output"));
		Parameters("TABLEout")->Set_Value(m_pTable);

		delete[] m_p_elevbands; // sämtliche Unter-Pointer noch löschen
		delete[] m_p_pcpField;
		delete[] m_p_tmpField;
		delete m_p_linparms;
		delete m_p_nonlinparms;
		if (m_bSnowModule) delete[] m_pSnowparms;
		
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

void Cihacres_elev::_Init_ElevBands(int n)
{
	// instantiate elevation bands
	m_p_elevbands = new Cihacres_elev_bands[n];

	// instantiate field numbers
	m_p_pcpField = new int[n];
	m_p_tmpField = new int[n];

	if ( m_bSnowModule) {
		m_pSnowparms = new CSnowParms[n];
	}

}
//---------------------------------------------------------------------

void Cihacres_elev::_Init_Pointers(int nvals)
{
	m_vec_date.resize(nvals);
	m_p_Q_obs_m3s = new double[nvals];
	m_p_Q_obs_mmday = new double[nvals];
	
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

void Cihacres_elev::_ReadInputFile()
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

void Cihacres_elev::_CalcSnowModule(int eb)
{
	m_p_SnowModule = new CSnowModule(m_p_elevbands[eb].m_p_tmp, m_p_elevbands[eb].m_p_pcp, m_nValues,
									m_pSnowparms[eb].T_Rain, m_pSnowparms[eb].T_Melt, m_pSnowparms[eb].DD_FAC);

	m_p_elevbands[eb].m_p_MeltRate = m_p_SnowModule->Get_MeltRate(m_p_elevbands[eb].m_p_MeltRate, m_nValues);
	m_p_elevbands[eb].m_p_SnowStorage = m_p_SnowModule->Get_SnowStorage(m_p_elevbands[eb].m_p_SnowStorage, m_nValues);

	delete m_p_SnowModule;
}
//---------------------------------------------------------------------

void Cihacres_elev::_Simulate_NonLinearModule(int eb)
{
	double eR_init = 0.0;
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
//---------------------------------------------------------------------

void Cihacres_elev::_Simulate_Streamflow(int eb)
{
	//----------------------------------------------------------

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
			m_p_linparms, eb, m_vq, m_vs, m_nValues, m_delay);
		break;
	case 2: // two storages in series
		break;
	} // end switch(m_StorConf)
}
//---------------------------------------------------------------------

void Cihacres_elev::_CreateTableSim()
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
	m_pTable->Add_Field(SG_T("Flow_SIM"),	SG_DATATYPE_Double);

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
void Cihacres_elev::_CreateDialog1()
{
	CSG_Parameter	*pNode;
	CSG_String		s;

	pNode = Parameters.Add_Choice(
		NULL	, "NELEVBANDS"		, SG_T("Number of elevation bands"),
		_TL(""),
		SG_T("2|3|4|5|6|7|8|9|10")
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
		pNode,	"SNOW_TOOL",	_TL("Using the snow-melt module?"),
		_TL("If checked, snow-melt module is used."),
		PARAMETER_TYPE_Bool, false
	);
}
//---------------------------------------------------------------------
// DIALOG 2
//---------------------------------------------------------------------

bool Cihacres_elev::_CreateDialog2()
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
		SG_T("Select the column containing the observed streamflow time series)")
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
		tmpName += SG_T(")");
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
	//}

		// Parameters of non-linear module -------------------------
		tmpNode = SG_T("Node");
		tmpNode+=convert_sl::Int2String(i+150).c_str();
		s.Printf(tmpNode.c_str(), i+150);
		pNode1 = P.Add_Node(pNode,s,SG_T("Non-Linear Tool"),_TL(""));

		tmpName = SG_T("TwFAC(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	_TL("Wetness decline time constant (Tw)"),
			_TW("Tw is approximately the time constant, or inversely, "
			"the rate at which the catchment wetness declines in the absence of rainfall"),
			PARAMETER_TYPE_Double,
			1.0, 0.01, true, 150.0, true
		);

		tmpName = SG_T("TFAC(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1, tmpName, SG_T("Temperature Modulation Factor (f)"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			1.0, 0.0001, true, 10.0, true
		);

		tmpName = SG_T("CFAC(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,tmpName,	_TL("Parameter (c)"),
			_TL("Parameter (c) to fit streamflow volume"),
			PARAMETER_TYPE_Double,
			0.001, 0.0, true, 1.0, true
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

			tmpName = SG_T("LFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Parameter (l)"),
				_TL("Soil moisture index threshold"),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 5.0, true
			);

			tmpName = SG_T("PFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Parameter (p)"),
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
			pNode1 = P.Add_Node(pNode,s,SG_T("Linear Tool"),_TL(""));

			tmpName = SG_T("AFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(a)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.8, -0.99, true, -0.01, true
			);

			tmpName = SG_T("BFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(b)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				0.2, 0.001, true, 1.0, true
			);
			break;

		case 1: // two parallel storages
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+250).c_str();
			s.Printf(tmpNode.c_str(), i+250);
			pNode1 = P.Add_Node(pNode,s,SG_T("Linear Tool"),_TL(""));

			// Parameter a
			tmpName = SG_T("AQ(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,tmpName,	_TL("a(q)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.7, -0.99, true, -0.01, true
			);

			tmpName = SG_T("AS(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(s)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.9, -0.99, true, -0.01, true
			);

			// Parameter b
			tmpName = SG_T("BQ(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("b(q)"),
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
			pNode1,tmpName,	SG_T("Time Delay (Rain-Runoff)"),
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
			pNode1 = P.Add_Node(pNode,s,SG_T("Snow Tool Parameters"),_TL(""));
			
			tmpName = SG_T("T_RAIN(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,tmpName,	SG_T("Temperature Threshold for Rainfall)"),
				SG_T("Below this threshold precipitation will fall as snow"),
				PARAMETER_TYPE_Double,
				-1.0, -10.0, true, 10.0, true
			);

			tmpName = SG_T("T_MELT(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,tmpName,	SG_T("Temperature Threshold for Melting"),
				SG_T("Above this threshold snow will start to melt"),
				PARAMETER_TYPE_Double,
				1.0, -5.0, true, 10.0, true
			);

			tmpName = SG_T("DD_FAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,tmpName,	SG_T("Day-Degree Factor"),
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
			tmpName = SG_T("TwFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nonlinparms->mp_tw[i]	= P(tmpName)		->asDouble();

			// get f
			tmpName = SG_T("TFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nonlinparms->mp_f[i]	= P(tmpName)		->asDouble();

			// get c
			tmpName = SG_T("CFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nonlinparms->mp_c[i]	= P(tmpName)		->asDouble();

			switch(m_IHAC_version)
			{
			case 0: // Jakeman & Hornberger (1993)
				break;
			case 1: // Croke et al. (2005)
				// get l
				tmpName = SG_T("LFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nonlinparms->mp_l[i]= P(tmpName)		->asDouble();

				// get p
				tmpName = SG_T("PFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nonlinparms->mp_p[i]= P(tmpName)		->asDouble();
			}

			// linear module parameters
			switch(m_nStorages)
			{
			case 1: // single storage
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+250).c_str();
				// get a
				tmpName = SG_T("AFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->a[i]		= P(tmpName)		->asDouble();

				// get b
				tmpName = SG_T("BFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->b[i]		= P(tmpName)		->asDouble();
				break;
			case 2: // two storages
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+250).c_str();
				// get aq
				tmpName = SG_T("AQ(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->aq[i]		= P(tmpName)		->asDouble();

				// get bq
				tmpName = SG_T("BQ(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->bq[i]		= P(tmpName)		->asDouble();

				// get as
				tmpName = SG_T("AS(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->as[i]		= P(tmpName)		->asDouble();

				// get bs
				m_p_linparms->bs[i] = ihacres.Calc_Parm_BS(m_p_linparms->aq[i],m_p_linparms->as[i],m_p_linparms->bq[i]);
				break;
			}

			// get delay
			tmpNode = SG_T("Node");
			tmpNode+=convert_sl::Int2String(i+300).c_str();

			tmpName = SG_T("DELAY(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_delay						= P(tmpName)		->asInt();	

			if (m_bSnowModule)
			{
				tmpNode = SG_T("Node");
				tmpNode+=convert_sl::Int2String(i+350).c_str();

				// get T_RAIN
				tmpName = SG_T("T_RAIN(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms[i].T_Rain		= P(tmpName)		->asDouble();

				// get T_MELT
				tmpName = SG_T("T_MELT(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms[i].T_Melt		= P(tmpName)		->asDouble();

				// get DD_FAC
				tmpName = SG_T("DD_FAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms[i].DD_FAC		= P(tmpName)		->asDouble();
			}
		}

		return(true);
	}
	return(false);
}

//---------------------------------------------------------------------
// DIALOG 3
//---------------------------------------------------------------------

bool Cihacres_elev::_CreateDialog3()
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
