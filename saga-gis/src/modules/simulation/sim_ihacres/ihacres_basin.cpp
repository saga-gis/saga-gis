/**********************************************************
 * Version $Id: ihacres_basin.cpp 1261 2011-12-16 15:12:15Z oconrad $
 *********************************************************/
///////////////////////////////////////////////////////////
//                    ihacres_basin.cpp                  //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2008-01-30                        //
///////////////////////////////////////////////////////////

//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// - lag COEFFICIENT in write output table IMPLEMENTIEREN !!!!!!!

//*******************************************************//


#define NULL 0

#include "ihacres_basin.h"
#include "convert_sl.h"
#include "model_tools.h"
//---------------------------------------------------------------------

Cihacres_basin::Cihacres_basin()
{
	//-----------------------------------------------------
	// 1. Info...
	Set_Name(_TL("IHACRES Basin"));

	Set_Author(SG_T("copyrights (c) 2008 Stefan Liersch"));

	Set_Description(_TW(
		"The Rainfall-Runoff Model IHACRES \n \n \n"
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
	// FIRST MODULE DIALOG
	_CreateDialog1();
	///////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------

Cihacres_basin::~Cihacres_basin()
{}
//---------------------------------------------------------------------




//---------------------------------------------------------------------
//		ON_EXECUTE
//---------------------------------------------------------------------

bool Cihacres_basin::On_Execute()
{
	CSG_Parameters P;
	//std::ofstream f("_out_elev.txt");

	// Assign Parameters from first Module Dialog
	//---------------------------------------------------------
	int nSBS		= Parameters("NSUBBASINS")		->asInt();
	m_nSubbasins	= nSBS + 2; // because list starts with 2 !
	m_IHAC_version	= Parameters("IHACVERS")		->asInt();
	m_StorConf		= Parameters("STORAGE")			->asInt();
	m_bSnowModule	= Parameters("SNOW_MODULE")		->asBool();
	//---------------------------------------------------------


	//---------------------------------------------------------
	// Assign number of storages
	m_nStorages = ihacres.Assign_nStorages(m_StorConf);
	//---------------------------------------------------------

	//---------------------------------------------------------
	// Initialize pointers
	_Init_Subbasins(m_nSubbasins);
	m_p_linparms = new C_IHAC_LinearParms(m_nSubbasins,m_nStorages);
	m_p_nonlinparms = new C_IHAC_NonLinearParms(m_nSubbasins);
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
		//m_p_Q_obs_mmday = model_tools::m3s_to_mmday(m_p_Q_obs_m3s, m_p_Q_obs_mmday, m_nValues, m_Area_tot);
		
		//---------------------------------------------------------
		// SNOW MODULE
		//---------------------------------------------------------
		double Q_init = 1.0; // ???
		for (int i = 0; i < m_nSubbasins; i++)
		{
			if (m_bSnowModule)
			{
				_CalcSnowModule(i);
			}

			_Simulate_NonLinearModule(i);

			_Simulate_Streamflow(i, Q_init);
		}

		//---------------------------------------------------------

		m_pTable = SG_Create_Table();
		_CreateTableSim();
		// add tables to SAGA Workspace
		m_pTable->Set_Name(_TL("IHACRES_Basin_output"));
		Parameters("TABLEout")->Set_Value(m_pTable);

		delete[] m_pSubbasin; // sämtliche Unter-Pointer noch löschen
		delete[] m_p_pcpField;
		delete[] m_p_tmpField;
		delete m_p_linparms;
		delete m_p_nonlinparms;
		if (m_bSnowModule) delete m_pSnowparms;
		
		return(true);
	} // end if ( _CreateDialog2() )

	return(false);
}


//---------------------------------------------------------------------

void Cihacres_basin::_Init_Subbasins(int n)
{
	// instantiate elevation bands
	m_pSubbasin = new Cihacres_subbasin[n];

	// instantiate field numbers
	m_p_pcpField = new int[n];
	m_p_tmpField = new int[n];

	if (m_bSnowModule)
	{
		m_pSnowparms = new CSnowParms[n];
	}
}
//---------------------------------------------------------------------

void Cihacres_basin::_Init_Pointers(int n)
{
	m_vec_date.resize(n);
	m_p_Q_obs_m3s = new double[n];
	m_p_Q_obs_mmday = new double[n];
	
	for (int i = 0; i < m_nSubbasins; i++)
	{
		m_pSubbasin[i].m_pPCP = new double[n];
		m_pSubbasin[i].m_pTMP = new double[n];
		m_pSubbasin[i].m_pER = new double[n];
		m_pSubbasin[i].m_p_Q_sim_mmday = new double[n];
		m_pSubbasin[i].m_pTw = new double[n];
		m_pSubbasin[i].m_pWI = new double[n];
	}
	if (m_bSnowModule)
	{
		for (int eb = 0; eb < m_nSubbasins; eb++)
		{
			m_pSubbasin[eb].m_pSnowStorage = new double[n];
			m_pSubbasin[eb].m_pMeltRate = new double[n];
		}
	}
}
//---------------------------------------------------------------------


void Cihacres_basin::_ReadInputFile()
{
	for (int j = 0, k = m_first; j < m_nValues, k < m_last + 1; j++, k++)
	{
		m_vec_date[j].append(CSG_String(m_p_InputTable->Get_Record(k)->asString(m_dateField)));
		m_p_Q_obs_m3s[j] = m_p_InputTable->Get_Record(k)->asDouble(m_streamflowField);
		
		for (int eb = 0; eb < m_nSubbasins; eb++)
		{
			m_pSubbasin[eb].m_pPCP[j] = m_p_InputTable->Get_Record(k)->asDouble(m_p_pcpField[eb]);
			m_pSubbasin[eb].m_pTMP[j] = m_p_InputTable->Get_Record(k)->asDouble(m_p_tmpField[eb]);
		}
	}
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------

void Cihacres_basin::_CalcSnowModule(int iSBS)
{
	m_p_SnowModule = new CSnowModule(m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pPCP, m_nValues,
									m_pSnowparms[iSBS].T_Rain, m_pSnowparms[iSBS].T_Melt, m_pSnowparms[iSBS].DD_FAC);

	m_pSubbasin[iSBS].m_pMeltRate = m_p_SnowModule->Get_MeltRate(m_pSubbasin[iSBS].m_pMeltRate, m_nValues);
	m_pSubbasin[iSBS].m_pSnowStorage = m_p_SnowModule->Get_SnowStorage(m_pSubbasin[iSBS].m_pSnowStorage, m_nValues);

	delete m_p_SnowModule;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------

void Cihacres_basin::_Simulate_NonLinearModule(int iSBS)
{
	double eR_init = 0.0;
	double WI_init = 0.5;
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
		ihacres.CalcWetnessTimeConst(m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pTw,
				m_p_nonlinparms, iSBS, m_nValues); 
				// 0 = index (only one instance of m_p_nonlinparms)

		if (m_bSnowModule)
		{
			ihacres.CalcWetnessIndex(m_pSubbasin[iSBS].m_pTw, m_pSubbasin[iSBS].m_pPCP,
				m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pWI, WI_init, m_p_nonlinparms->mp_c[iSBS],
				m_bSnowModule, m_pSnowparms[iSBS].T_Rain, m_nValues);

			ihacres.CalcExcessRain(m_pSubbasin[iSBS].m_pPCP, m_pSubbasin[iSBS].m_pTMP,
				m_pSubbasin[iSBS].m_pWI, m_pSubbasin[iSBS].m_pER, eR_init,
				m_pSubbasin[iSBS].m_sum_eRainGTpcp, m_nValues, m_bSnowModule,
				m_pSnowparms[iSBS].T_Rain, m_pSnowparms[iSBS].T_Melt,
				m_pSubbasin[iSBS].m_pMeltRate);
		} else {

			ihacres.CalcWetnessIndex(m_pSubbasin[iSBS].m_pTw, m_pSubbasin[iSBS].m_pPCP,
				m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pWI, WI_init, m_p_nonlinparms->mp_c[iSBS],
				m_bSnowModule, 0, m_nValues);

			ihacres.CalcExcessRain(m_pSubbasin[iSBS].m_pPCP, m_pSubbasin[iSBS].m_pTMP,
				m_pSubbasin[iSBS].m_pWI, m_pSubbasin[iSBS].m_pER, eR_init,
				m_pSubbasin[iSBS].m_sum_eRainGTpcp, m_nValues, m_bSnowModule,
				0,0,0);
		}
		break;
	case 1: // Croke et al. (2005)
		ihacres.CalcWetnessTimeConst_Redesign(m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pTw,
			m_p_nonlinparms, iSBS, m_nValues);	// 0 = index (only one instance of m_p_nonlinparms)

		if (m_bSnowModule)
		{
			ihacres.CalcWetnessIndex_Redesign(m_pSubbasin[iSBS].m_pTw, m_pSubbasin[iSBS].m_pPCP, m_pSubbasin[iSBS].m_pWI, WI_init,
				m_bSnowModule, m_pSnowparms[iSBS].T_Rain, m_nValues);

			ihacres.CalcExcessRain_Redesign(m_pSubbasin[iSBS].m_pPCP, m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pWI, 
				m_pSubbasin[iSBS].m_pER, eR_init, m_pSubbasin[iSBS].m_sum_eRainGTpcp, m_nValues, 
				m_p_nonlinparms->mp_c[iSBS], m_p_nonlinparms->mp_l[iSBS], m_p_nonlinparms->mp_p[iSBS],
				m_bSnowModule, m_pSnowparms[iSBS].T_Rain, m_pSnowparms[iSBS].T_Melt, m_pSubbasin[iSBS].m_pMeltRate);
		} else {
			ihacres.CalcWetnessIndex_Redesign(m_pSubbasin[iSBS].m_pTw, m_pSubbasin[iSBS].m_pPCP, m_pSubbasin[iSBS].m_pWI, WI_init,
				m_bSnowModule, 0, m_nValues);

			ihacres.CalcExcessRain_Redesign(m_pSubbasin[iSBS].m_pPCP, m_pSubbasin[iSBS].m_pTMP, m_pSubbasin[iSBS].m_pWI, 
				m_pSubbasin[iSBS].m_pER, eR_init, m_pSubbasin[iSBS].m_sum_eRainGTpcp, m_nValues, 
				m_p_nonlinparms->mp_c[iSBS], m_p_nonlinparms->mp_l[iSBS], m_p_nonlinparms->mp_p[iSBS],
				m_bSnowModule, 0,0,0);
		}
		break;
	} // end switch(m_IHAC_vers)
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------

void Cihacres_basin::_Simulate_Streamflow(int iSBS, double Q_init)
{
	//----------------------------------------------------------

	//----------------------------------------------------------
	// calculate streamflow
	//----------------------------------------------------------
	switch(m_StorConf)
	{
	case 0: // single storage
		ihacres.SimStreamflowSingle(m_pSubbasin[iSBS].m_pER, Q_init,
			m_pSubbasin[iSBS].m_p_Q_sim_mmday, m_pSubbasin[iSBS].m_delay,
			m_p_linparms->a[iSBS], m_p_linparms->b[iSBS], m_nValues);
		break;
	case 1: // two storages in parallel
		ihacres.SimStreamflow2Parallel(m_pSubbasin[iSBS].m_pER,
			m_pSubbasin[iSBS].m_p_Q_sim_mmday, Q_init,
			m_p_linparms, iSBS, m_vq, m_vs, m_nValues, m_pSubbasin[iSBS].m_delay);
		break;
	case 2: // two storages in series
		break;
	} // end switch(m_StorConf)
}
//---------------------------------------------------------------------




///////////////////////////////////////////////////////////////////////
//
//		CREATE TABLE
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void Cihacres_basin::_CreateTableSim()
{
	int i = 0; // used in function Get_Record(i)
	CSG_Table_Record	*pRecord;
	CSG_String			tmpName;
	double				sim_sbs, sim;

	// creating the column titles
	m_pTable->Add_Field("Date",		SG_DATATYPE_String);
	m_pTable->Add_Field("Flow_OBS",	SG_DATATYPE_Double);

	for (int sbs = 0; sbs < m_nSubbasins; sbs++)
	{
		tmpName = "SBS_";
		tmpName += convert_sl::Int2String(sbs+1).c_str();
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
		sim_sbs = 0.0;
		sim = 0.0;
		for (int sbs = 0; sbs < m_nSubbasins; sbs++)
		{
			sim_sbs = model_tools::mmday_to_m3s(m_pSubbasin[sbs].m_p_Q_sim_mmday[j],m_pSubbasin[sbs].m_area);
			//pRecord->Set_Value(3+eb,model_tools::mmday_to_m3s(m_p_elevbands[eb].m_p_streamflow_sim[j],m_p_elevbands[eb].m_area));
			pRecord->Set_Value(2+sbs, sim_sbs);
			sim += sim_sbs;
		}
		pRecord->Set_Value(2+m_nSubbasins,sim);
		i++;
	}
}





///////////////////////////////////////////////////////////////////////
//
//		                          DIALOGS
//
///////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
// DIALOG 1
//---------------------------------------------------------------------
void Cihacres_basin::_CreateDialog1()
{
	CSG_Parameter	*pNode;
	CSG_String		s;

	pNode = Parameters.Add_Choice(
		NULL	, "NSUBBASINS"		, _TL("Number of sub-basins"),
		_TL(""),
		SG_T("2|3|4|5|6|7|8|9|10")
	);

	s.Printf(SG_T("Node1"), 1);
	pNode = Parameters.Add_Node(NULL,s,_TL("IHACRES Version"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "IHACVERS"		, _TL("IHACRES Version"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Jakeman & Hornberger (1993)"),	//  0
			_TL("Croke et al. (2005) !!! not yet implemented !!!")	//	1
		)
	);

	s.Printf(SG_T("Node2"), 2);
	pNode = Parameters.Add_Node(NULL,s,_TL("Storage Configuration"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "STORAGE"		, _TL("Storage"),
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
}


//---------------------------------------------------------------------
// DIALOG 2
//---------------------------------------------------------------------

bool Cihacres_basin::_CreateDialog2()
{
	//std::ofstream f("_out_elev.txt");

	int				i;
	CSG_Parameters	P;  // used to add Parameters in the second dialog
	CSG_Parameter	*pNode, *pNode1;
	CSG_String		s;
	CSG_String		tmpNode, tmpName;

	P.Set_Name(_TL("IHACRES Basin (Dialog 2)"));
	// Input file ----------------------------------------------
	pNode = P.Add_Table(
		NULL	, "TABLE"	, _TL("IHACRES Input Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	P.Add_Table_Field(
		pNode	, "DATE_Field"	, _TL("Date Column"),
		_TL("Select the column containing the Date")
	);

	P.Add_Table_Field(
		pNode	, "DISCHARGE_Field"	, _TL("Streamflow (obs.) Column"),
		_TL("Select the column containing the observed streamflow time series")
	);
	
	for (i = 0; i < m_nSubbasins; i++)
	{
		tmpNode = convert_sl::Int2String(i+1).c_str();

		tmpName = _TL("PCP Column: Subbasin: ");
		tmpName+=tmpNode;
		P.Add_Table_Field(
			pNode	, tmpName.c_str(), tmpName.c_str(),
			_TL("Select Precipitation Column")
		);

		tmpName = _TL("TMP Column: Subbasin: ");
		tmpName+=tmpNode;
		P.Add_Table_Field(
			pNode	, tmpName.c_str()	, tmpName.c_str(),
			_TL("Select Temperature Column")
		);
	}
	// Input file ----------------------------------------------

	for (i = 0; i < m_nSubbasins; i++)
	{
		tmpNode = _TL("Node");
		tmpNode+=convert_sl::Int2String(i+100).c_str();
		tmpName = _TL("Subbasin ");
		tmpName+=convert_sl::Int2String(i+1).c_str();

		s.Printf(tmpNode.c_str(), i+100);
		pNode = P.Add_Node(NULL,s,tmpName.c_str(),_TL(""));
		
		tmpName = _TL("Area [km2] Subbasin(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode,	tmpName, _TL("Area [km2]"),
			_TL(""),
			PARAMETER_TYPE_Double
		);

		tmpName = "Lag Subbasin(";
		tmpName += tmpNode;
		tmpName += ")";
		P.Add_Value(
			pNode,	tmpName, _TL("Lag coefficient"),
			_TL(""),
			PARAMETER_TYPE_Int
		);


		// Parameters of non-linear module -------------------------

		tmpNode = "Node";
		tmpNode+=convert_sl::Int2String(i+150).c_str();
		s.Printf(tmpNode.c_str(), i+150);
		pNode1 = P.Add_Node(pNode,s,_TL("Non-Linear Module"),_TL(""));

		tmpName = _TL("TwFAC(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	_TL("Wetness decline time constant (Tw)"),
			_TW("Tw is approximately the time constant, or inversely,"
			"the rate at which the catchment wetness declines in the absence of rainfall"),
			PARAMETER_TYPE_Double,
			1.0, 0.01, true, 150.0, true
		);

		tmpName = _TL("TFAC(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1	, tmpName	, _TL("Temperature Modulation Factor (f)"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			1.0, 0.0001, true, 10.0, true
		);

		tmpName = _TL("CFAC(");
		tmpName += tmpNode;
		tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	_TL("Parameter (c)"),
			_TL("Parameter (c) to fit streamflow volume"),
			PARAMETER_TYPE_Double,
			0.001, 0.0, true, 1.0, true
		);

		switch(m_IHAC_version)
		{
		case 0: // Jakeman & Hornberger (1993)
			break;
		case 1: // Croke et al. (2005)
			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+200).c_str();
			s.Printf(tmpNode.c_str(), i+200);
			pNode1 = P.Add_Node(pNode,s,_TL("Soil Moisture Power Eq."),_TL(""));

			tmpName = "LFAC(";
			tmpName += tmpNode;
			tmpName += ")";
			P.Add_Value(
				pNode1,	tmpName, _TL("Parameter (l)"),
				_TL("Soil moisture index threshold"),
				PARAMETER_TYPE_Double,
				0.0, 0.0, true, 5.0, true
			);

			tmpName = "PFAC(";
			tmpName += tmpNode;
			tmpName += ")";
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
			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+250).c_str();
			s.Printf(tmpNode.c_str(), i+250);
			pNode1 = P.Add_Node(pNode,s,_TL("Linear Module"),_TL(""));

			tmpName = _TL("AFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("(a)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.8, -0.99, true, -0.01, true
			);

			tmpName = _TL("BFAC(");
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
			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+250).c_str();
			s.Printf(tmpNode.c_str(), i+250);
			pNode1 = P.Add_Node(pNode,s,_TL("Linear Module"),_TL(""));

			// Parameter a

			tmpName = _TL("AQ(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(q)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.7, -0.99, true, -0.01, true
			);

			tmpName = _TL("AS(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("a(s)"),
				_TL(""),
				PARAMETER_TYPE_Double,
				-0.9, -0.99, true, -0.01, true
			);

			// Parameter b

			tmpName = _TL("BQ(");
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

		tmpNode = _TL("Node");
		tmpNode+=convert_sl::Int2String(i+300).c_str();
		s.Printf(tmpNode.c_str(), i+300);
		pNode1 = P.Add_Node(pNode,s,_TL("Time Delay after Start of Rainfall (INTEGER)"),_TL(""));
			
		tmpName = _TL("DELAY(");
			tmpName += tmpNode;
			tmpName += _TL(")");
		P.Add_Value(
			pNode1,	tmpName,	_TL("Time Delay (Rain-Runoff)"),
			_TL("The delay after the start of rainfall, before the discharge starts to rise."),
			PARAMETER_TYPE_Int,
			0, 1, true, 100, true
		);

		// snow module parameters ----------------------------------
		if (m_bSnowModule)
		{
			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+350).c_str();
			s.Printf(tmpNode.c_str(), i+350);
			pNode1 = P.Add_Node(pNode,s,_TL("Snow Module Parameters"),_TL(""));
			
			tmpName = _TL("T_RAIN(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Temperature Threshold for Rainfall"),
				_TL("Below this threshold precipitation will fall as snow"),
				PARAMETER_TYPE_Double,
				-1.0, -10.0, true, 10.0, true
			);

			tmpName = _TL("T_MELT(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Temperature Threshold for Melting"),
				_TL("Above this threshold snow will start to melt"),
				PARAMETER_TYPE_Double,
				1.0, -5.0, true, 10.0, true
			);

			tmpName = _TL("DD_FAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			P.Add_Value(
				pNode1,	tmpName,	_TL("Day-Degree Factor"),
				_TL("Day-Degree Factor depends on catchment characteristics"),
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
		for (int i = 0; i < m_nSubbasins; i++)
		{
			tmpNode = convert_sl::Int2String(i+1).c_str();
			
			// get precipitation column of Subbasin[i]
			tmpName = _TL("PCP Column: Subbasin: ");
			tmpName+=tmpNode;
			m_p_pcpField[i]			= P(tmpName)		->asInt();

			// get temperature column of Subbasin[i]
			tmpName = _TL("TMP Column: Subbasin: ");
			tmpName+=tmpNode;
			m_p_tmpField[i]			= P(tmpName)		->asInt();

			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+100).c_str();

			// get area[km2] of Subbasin[i]
			tmpName = _TL("Area [km2] Subbasin(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_pSubbasin[i].m_area	= P(tmpName)		->asDouble();

			// get lag coefficient of Subbasin[i]
			tmpName = _TL("Lag Subbasin(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_pSubbasin[i].m_lag	= P(tmpName)		->asInt();
			
			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+150).c_str();
			// get Tw
			tmpName = _TL("TwFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nonlinparms->mp_tw[i]	= P(tmpName)	->asDouble();

			// get f
			tmpName = _TL("TFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nonlinparms->mp_f[i]	= P(tmpName)	->asDouble();

			// get c
			tmpName = _TL("CFAC(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_p_nonlinparms->mp_c[i]	= P(tmpName)	->asDouble();

			switch(m_IHAC_version)
			{
			case 0: // Jakeman & Hornberger (1993)
				break;
			case 1: // Croke et al. (2005)
				tmpNode = _TL("Node");
				tmpNode+=convert_sl::Int2String(i+200).c_str();
				// get l
				tmpName = _TL("LFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nonlinparms->mp_l[i]= P(tmpName)	->asDouble();

				// get p
				tmpName = _TL("PFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_nonlinparms->mp_p[i]= P(tmpName)	->asDouble();
			}

			// linear module parameters
			switch(m_nStorages)
			{
			case 1: // single storage
				tmpNode = _TL("Node");
				tmpNode+=convert_sl::Int2String(i+250).c_str();
				// get a
				tmpName = _TL("AFAC()");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->a[i]		= P(tmpName)	->asDouble();

				// get b
				tmpName = _TL("BFAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->b[i]		= P(tmpName)	->asDouble();
				break;
			case 2: // two storages
				tmpNode = _TL("Node");
				tmpNode+=convert_sl::Int2String(i+250).c_str();
				// get aq
				tmpName = _TL("AQ(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->aq[i]		= P(tmpName)	->asDouble();

				// get bq
				tmpName = _TL("BQ(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->bq[i]		= P(tmpName)	->asDouble();

				// get as
				tmpName = _TL("AS(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_p_linparms->as[i]		= P(tmpName)	->asDouble();
				m_p_linparms->bs[i]		= ihacres.Calc_Parm_BS(m_p_linparms->aq[i],m_p_linparms->as[i],m_p_linparms->bq[i]);
				break;
			}

			// get delay
			tmpNode = _TL("Node");
			tmpNode+=convert_sl::Int2String(i+300).c_str();
			tmpName = _TL("DELAY(");
			tmpName += tmpNode;
			tmpName += _TL(")");
			m_pSubbasin[i].m_delay		= P(tmpName)	->asInt();

			if (m_bSnowModule)
			{
				tmpNode = _TL("Node");
				tmpNode+=convert_sl::Int2String(i+350).c_str();
				tmpName = _TL("T_RAIN(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms[i].T_Rain	= P(tmpName)	->asDouble();
				tmpName = _TL("T_MELT(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms[i].T_Melt	= P(tmpName)	->asDouble();
				tmpName = _TL("DD_FAC(");
				tmpName += tmpNode;
				tmpName += _TL(")");
				m_pSnowparms[i].DD_FAC	= P(tmpName)	->asDouble();
			}

		} // end for (int i = 0; i < m_nSubbasins; i++)

		return(true);
	}
	return(false);
}

//---------------------------------------------------------------------
// DIALOG 3
//---------------------------------------------------------------------

bool Cihacres_basin::_CreateDialog3()
{
	CSG_String		s;
	CSG_Parameters	P;  // used to add Parameters in the second dialog
	CSG_Parameter	*pNode;

	//	Dialog design
	P.Set_Name(_TL("Choose Time Range"));

	s.Printf(SG_T("Node"), 1);
	pNode = P.Add_Node(NULL,s,_TL("Time Range"),_TL(""));

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
