/**********************************************************
 * Version $Id: ihacres_v1.cpp 1261 2011-12-16 15:12:15Z oconrad $
 *********************************************************/
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ihacres_v1                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ihacres_v1.cpp                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com                   //
//                                                       //
//                     2006-08-31                        //
//-------------------------------------------------------//
//                                                       
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
#include <iostream> // used for textfile output (test only)
#include <fstream>  // used for textfile output (test only)

#include "ihacres_v1.h"
//#include "ihacres_cal.h"
#include "model_tools.h"
//---------------------------------------------------------

// constructor
Cihacres_v1::Cihacres_v1(void)
{
	//-----------------------------------------------------
	// 1. Info...
	Set_Name(_TL("IHACRES Version 1.0"));

	Set_Author(SG_T("copyrights (c) 2008 Stefan Liersch"));

	Set_Description(_TW("The Rainfall-Runoff Model IHACRES \n \n \n"
		"Reference: \n \n"
		"Jakeman, A.J. / Hornberger, G.M. (1993). \n"
		"How Much Complexity Is Warranted in a Rainfall-Runoff Model? \n"
		"Water Resources Research, (29), NO. 8 (2637-2649) \n \n"   
		"Croke, B. F. W. et al.(2004).\n"
		"A dynamic model for predicting hydrologic response "
		"to land cover changes in gauged and "
		"ungauged catchments. \n"
		"Journal Of Hydrology 291(1-2): 115-131."
	));

	///////////////////////////////////////////////////////////////////
	// FIRST TOOL DIALOG
	CreateDialog1();
	///////////////////////////////////////////////////////////////////
}

//---------------------------------------------------------
// destructor
Cihacres_v1::~Cihacres_v1(void)
{}

//---------------------------------------------------------

bool Cihacres_v1::On_Execute(void)
{
	///////////////////////////////////////////////////////////////////
	//
	//								VARIABLES
	//
	///////////////////////////////////////////////////////////////////
	int				j,k;		//counter
	CSG_Table		*pTable, *pTable_out, *pTable_parms, *pTable_settings;

	bool			bTMP;		// true, if temperature data are used to
								// to refine wetness index estimation.
	bool			writeAllTimeSeries;
	int				storconf;	// storage configuration
								// 0 = single, 1 = two parallel, 2 = two in series

	// Assign Parameters from first Tool Dialog
	//---------------------------------------------------------
	pTable			= Parameters("TABLE")			->asTable();
	// Field numbers
	dateField		= Parameters("DATE_Field")		->asInt();
	dischargeField	= Parameters("DISCHARGE_Field")	->asInt();
	pcpField		= Parameters("PCP_Field")		->asInt();
	tmpField		= Parameters("TMP_Field")		->asInt();

	bTMP			= Parameters("USE_TMP")			->asBool();
	c				= Parameters("CFAC")			->asDouble();
	TwConst			= Parameters("TwFAC")			->asDouble();
	m_area			= Parameters("AREA")			->asDouble();
	storconf		= Parameters("STORAGE")			->asInt();
	IHAC_version	= Parameters("IHACVERS")		->asInt();
	bSnowModule		= Parameters("SNOW_TOOL")		->asBool();
	writeAllTimeSeries = Parameters("WRITEALL_TS")	->asBool();
	first = last	= 0;
	//---------------------------------------------------------

	///////////////////////////////////////////////////////////////////
	// Create Second Dialog and assign linear module parameters
	if ( CreateDialog2(bTMP, storconf, IHAC_version, pTable, dateField, dischargeField) ) {
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////
		// searching the first and the last record of the time range
		ihacres->AssignFirstLastRec(*pTable, first, last, date1, date2, dateField);
		///////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////
		//					INITIALIZE VECTORS
		sizeAll = last - first + 1;
		// global array variables
		date.resize(sizeAll);
		m_Q_obs_m3s.resize(sizeAll);
		m_Q_obs_mmday.resize(sizeAll);
		precipitation.resize(sizeAll);
		if (bTMP) temperature.resize(sizeAll);

		for (j = 0, k = first; j < sizeAll, k < last + 1; j++, k++)
		{
			date[j].append(CSG_String (pTable->Get_Record(k)->asString(dateField)));
			m_Q_obs_m3s[j]			 = pTable->Get_Record(k)->asDouble(dischargeField);
			precipitation[j]		 = pTable->Get_Record(k)->asDouble(pcpField);
			if (bTMP) temperature[j] = pTable->Get_Record(k)->asDouble(tmpField);
		}

		if (bSnowModule && bTMP)
		{
			m_pSnowModule = new CSnowModule(temperature, precipitation, T_Rain, T_Melt, DD_FAC);
			if (!bSnowModule) m_pSnowModule = NULL;
			//m_pSnowModule->MeltRate = new double[sizeAll];
			//m_pSnowModule->SnowStorage = new double[sizeAll];
		}

		//---------------------------------------------------------
		// Convert streamflow from m3/s to mm/day
		///////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////
		//
		//					INSTANTIATE CLASS IHACRES
		//
		///////////////////////////////////////////////////////////////

		// different constructors are available for class ihacres_eq,
		// depending on availability of temperature data,
		// storage characteristics, and data storage properties (vector / arrays)

		switch(storconf)
		{
		case 0: // single storage
			ihacres = new Cihacres_eq(date,
									  m_Q_obs_m3s,
									  precipitation,
									  temperature,
									  TwConst, f, c, l, p,
									  a, 0, b, 0,
									  m_area,
									  bTMP,
									  IHAC_version,
									  storconf,
									  bSnowModule,
									  m_pSnowModule,
									  delay);
			break;
		case 1: // two storages in parallel
			// if temperature data available
			// and using vectors to store time series data
			// and 2 storages in parallel
			ihacres = new Cihacres_eq(date,
									  m_Q_obs_m3s,
									  precipitation,
									  temperature,
									  TwConst, f, c, l, p,
									  aq, as, bq, bs,
									  m_area,
									  bTMP,
									  IHAC_version,
									  storconf,
									  bSnowModule,
									  m_pSnowModule,
									  delay);
			break;
		} // end switch

		//---------------------------------------------------------
		// create output tables and add them to SAGA workspace
		//---------------------------------------------------------
		pTable_out   = SG_Create_Table();
		CreateTableSim(pTable_out, date, m_Q_obs_m3s, ihacres->get_streamflow_sim());//ihacres->streamflow_sim);
		// add tables to SAGA Workspace
		pTable_out->Set_Name(SG_T("IHACRES_Sim"));
		Parameters("TABLEout")->Set_Value(pTable_out);
			
		pTable_settings   = SG_Create_Table();
		if (bSnowModule)
		{
			CreateTableSettings(pTable_settings, TwConst, c, f, aq, as, bq, bs,
								ihacres->get_vq(), ihacres->get_vs(),
								bSnowModule, m_pSnowModule->Get_T_Rain(), m_pSnowModule->Get_T_Melt(), m_pSnowModule->Get_DD_FAC(),
								delay,
								model_tools::CalcRunoffCoeff(m_Q_obs_mmday,precipitation), ihacres->get_NSE(),
								m_area);
		} else {
			CreateTableSettings(pTable_settings, TwConst, c, f, aq, as, bq, bs,
								ihacres->get_vq(), ihacres->get_vs(),
								bSnowModule, 0, 0, 0,
								delay,
								model_tools::CalcRunoffCoeff(m_Q_obs_mmday,precipitation), ihacres->get_NSE(),
								m_area);
		}
		pTable_settings->Set_Name(SG_T("IHACRES_Settings"));
		Parameters("TABLEsettings")->Set_Value(pTable_settings);

		if (writeAllTimeSeries)
		{
			pTable_parms = SG_Create_Table();
			CreateTableParms(pTable_parms, date, m_Q_obs_m3s, precipitation, temperature,
							ihacres->get_streamflow_sim(), ihacres->get_excessRain(),
							ihacres->get_WetnessIndex(), ihacres->get_Tw());
			pTable_parms->Set_Name(SG_T("IHACRES_Parms"));
			Parameters("TABLEparms")->Set_Value(pTable_parms);
		}

		///////////////////////////////////////////////////////////////
		//						DELETE AND RESIZE
		///////////////////////////////////////////////////////////////
		delete ihacres;
		date.resize(0);
		m_Q_obs_m3s.resize(0);
		m_Q_obs_mmday.resize(0);
		precipitation.resize(0);
		if (bTMP) temperature.resize(0);
		if (bSnowModule) delete m_pSnowModule;
		///////////////////////////////////////////////////////////////

		//---------------------------------------------------------
		return( true );   // bool Cihacres_cal::On_Execute(void)
	} else {
		return(false);
	}
} // end function On_Execute()


///////////////////////////////////////////////////////////////////////
//
//                          PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
//                          CREATE OUTPUT TABLES
//
///////////////////////////////////////////////////////////////////////

void Cihacres_v1::CreateTableSim(CSG_Table *pTable, date_array date, vector_d strfl_obs, vector_d strfl_sim)
{
	int i = 0; // used in function Get_Record(i)
	CSG_Table_Record	*pRecord;

	// creating the column titles
	pTable->Add_Field("Date",		SG_DATATYPE_String);
	pTable->Add_Field("Flow OBS",	SG_DATATYPE_Double);
	pTable->Add_Field("Flow SIM",	SG_DATATYPE_Double);

	for (unsigned int j = 0; j < date.size(); j++)
	{
		pTable->Add_Record();
		pRecord = pTable->Get_Record(i);

		// writing the data into the rows
		pRecord->Set_Value(0,CSG_String(date[j].c_str()));
		pRecord->Set_Value(1,strfl_obs[j]);
		pRecord->Set_Value(2,model_tools::mmday_to_m3s(strfl_sim[j],m_area));
		i++;
	}
}

void Cihacres_v1::CreateTableParms(CSG_Table *pTable, date_array date, vector_d strfl_obs, vector_d precipitation, vector_d temperature,
								   vector_d strfl_sim, vector_d excessRain, vector_d wi, vector_d Tw)
{
	int i = 0; // used in function Get_Record(i)
	CSG_Table_Record	*pRecord;

	// creating the column titles
	pTable->Add_Field("Date",			SG_DATATYPE_String);
	pTable->Add_Field("Flow OBS",		SG_DATATYPE_Double);
	pTable->Add_Field("Flow SIM",		SG_DATATYPE_Double);
	pTable->Add_Field("TMP",			SG_DATATYPE_Double);
	pTable->Add_Field("PCP",			SG_DATATYPE_Double);
	pTable->Add_Field("ExcessRain",		SG_DATATYPE_Double);
	pTable->Add_Field("WetnessIndex",	SG_DATATYPE_Double);
	pTable->Add_Field("Tau",			SG_DATATYPE_Double);

	for (unsigned int j = 0; j < date.size(); j++)
	{
		pTable->Add_Record();
		pRecord = pTable->Get_Record(i);

		// writing the data into the rows
		pRecord->Set_Value(0,CSG_String(date[j].c_str()));
		pRecord->Set_Value(1,strfl_obs[j]);
		pRecord->Set_Value(2,model_tools::mmday_to_m3s(strfl_sim[j], m_area));
		pRecord->Set_Value(3,temperature[j]);
		pRecord->Set_Value(4,precipitation[j]);
		pRecord->Set_Value(5,excessRain[j]);
		pRecord->Set_Value(6,wi[j]);
		pRecord->Set_Value(7,Tw[j]);
		i++;
	}
}

void Cihacres_v1::CreateTableSettings(CSG_Table *pTable, double Tw, double c, double f,
									  double aq, double as, double bq, double bs,
									  double vq , double vs,
									  bool bSnowModule, double T_Rain, double T_Melt, double DD_FAC,
									  int delay,
									  double RRCoef, double NSE,
									  double m_area)
{
	int count = 1;
	CSG_Table_Record	*pRecord;

	// creating the column titles
	pTable->Add_Field("Parameters",	SG_DATATYPE_String);
	pTable->Add_Field("Settings",	SG_DATATYPE_Double);

	pTable->Add_Record();
	pRecord = pTable->Get_Record(0);
	pRecord->Set_Value(0,SG_T("Tau"));
	pRecord->Set_Value(1,Tw);	

	pTable->Add_Record();
	pRecord = pTable->Get_Record(1);
	pRecord->Set_Value(0,SG_T("c"));
	pRecord->Set_Value(1,c);	

	pTable->Add_Record();
	pRecord = pTable->Get_Record(2);
	pRecord->Set_Value(0,SG_T("f"));
	pRecord->Set_Value(1,f);

	pTable->Add_Record();
	pRecord = pTable->Get_Record(3);
	pRecord->Set_Value(0,SG_T("a(q)"));
	pRecord->Set_Value(1,aq);

	pTable->Add_Record();
	pRecord = pTable->Get_Record(4);
	pRecord->Set_Value(0,SG_T("a(s)"));
	pRecord->Set_Value(1,as);

	pTable->Add_Record();
	pRecord = pTable->Get_Record(5);
	pRecord->Set_Value(0,SG_T("b(q)"));
	pRecord->Set_Value(1,bq);

	pTable->Add_Record();
	pRecord = pTable->Get_Record(6);
	pRecord->Set_Value(0,SG_T("b(s)"));
	pRecord->Set_Value(1,bs);
	//pRecord->Set_Value(1,vs * (1 + as));

	pTable->Add_Record();
	pRecord = pTable->Get_Record(7);
	pRecord->Set_Value(0,SG_T("v(q)"));
	pRecord->Set_Value(1,vq);

	pTable->Add_Record();
	pRecord = pTable->Get_Record(8);
	pRecord->Set_Value(0,SG_T("v(s)"));
	pRecord->Set_Value(1,vs);

	pTable->Add_Record();
	pRecord= pTable->Get_Record(9);
	pRecord->Set_Value(0,SG_T("T(q)"));
	pRecord->Set_Value(1,ihacres->Calc_TimeOfDecay(aq));

	pTable->Add_Record();
	pRecord= pTable->Get_Record(10);
	pRecord->Set_Value(0,SG_T("T(s)"));
	pRecord->Set_Value(1,ihacres->Calc_TimeOfDecay(as));

	if (bSnowModule)
	{
		pTable->Add_Record();
		pRecord = pTable->Get_Record(10+count);
		pRecord->Set_Value(0,SG_T("T_Rain"));
		pRecord->Set_Value(1,T_Rain);
		count++;

		pTable->Add_Record();
		pRecord = pTable->Get_Record(10+count);
		pRecord->Set_Value(0,SG_T("T_Melt"));
		pRecord->Set_Value(1,T_Melt);
		count++;

		pTable->Add_Record();
		pRecord = pTable->Get_Record(10+count);
		pRecord->Set_Value(0,SG_T("DD_Fac"));
		pRecord->Set_Value(1,DD_FAC);
		count++;
	}

	pTable->Add_Record();
	pRecord = pTable->Get_Record(10+count);
	pRecord->Set_Value(0,SG_T("RR-Delay"));
	pRecord->Set_Value(1,delay);
	count++;

	pTable->Add_Record();
	pRecord = pTable->Get_Record(10+count);
	pRecord->Set_Value(0,SG_T("RRCoef"));
	pRecord->Set_Value(1,RRCoef);
	count++;

	pTable->Add_Record();
	pRecord = pTable->Get_Record(10+count);
	pRecord->Set_Value(0,SG_T("NSE"));
	pRecord->Set_Value(1,NSE);
	count++;

	pTable->Add_Record();
	pRecord = pTable->Get_Record(10+count);
	pRecord->Set_Value(0,SG_T("Area"));
	pRecord->Set_Value(1,m_area);
	count++;
}


///////////////////////////////////////////////////////////////////////
//
//		                          DIALOGS
//
///////////////////////////////////////////////////////////////////////

void Cihacres_v1::CreateDialog1()
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
		SG_T("Select the column containing the Date")
	);

	Parameters.Add_Table_Field(
		pNode	, "DISCHARGE_Field"	, _TL("Streamflow Column"),
		SG_T("Select the Column containing Discharge Values")
	);

	Parameters.Add_Table_Field(
		pNode	, "PCP_Field"	, _TL("Precipitation Column"),
		SG_T("Select the Column containing precipitation Values")
	);

	Parameters.Add_Table_Field(
		pNode	, "TMP_Field"	, _TL("Temperature Column"),
		SG_T("Select the Column containing Temperature Values")
	);

	Parameters.Add_Value(
		pNode,	"USE_TMP",	_TL("Using temperature data?"),
		_TL("If checked, then temperature data are used."),
		PARAMETER_TYPE_Bool, true
	);

	pNode = Parameters.Add_Value(
		NULL,	"AREA",	_TL("Area of the Watershed in [km2]"),
		_TL("Area of the Watershed in [km2] used for unit conversion"),
		PARAMETER_TYPE_Double,
		100.0, 0.00001, true, 0.0, false
	);

	s.Printf(SG_T("Node2"), 2);
		pNode = Parameters.Add_Node(NULL,s,SG_T("Non-Linear Tool"),_TL(""));

	Parameters.Add_Value(
		pNode,	"CFAC",	_TL("Parameter (c)"),
		_TL("Parameter (c) to fit streamflow volume"),
		PARAMETER_TYPE_Double,
		0.001, 0.0, true, 1.0, true
	);
	Parameters.Add_Value(
		pNode,	"TwFAC",	_TL("(Tw) wetness decline time constant"),
		_TW("Tw is approximately the time constant, or inversely,"
		"the rate at which the catchment wetness declines in the absence of rainfall"),
		PARAMETER_TYPE_Double,
		1.0, 0.01, true, 150.0, true
	);

	s.Printf(SG_T("Node3"), 3);
	pNode = Parameters.Add_Node(NULL,s,SG_T("Storage Configuration"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "STORAGE"		, _TL("Storage"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Single Storage"),			//  0
			_TL("Two Parallel Storages"),	//  1 
			_TL("Two Storages in Series !!! not yet implemented !!!")	//  2 
		)
	);

	s.Printf(SG_T("Node4"), 4);
	pNode = Parameters.Add_Node(NULL,s,SG_T("IHACRES Version"),_TL(""));

	Parameters.Add_Choice(
		pNode	, "IHACVERS"		, _TL("IHACRES Version"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Jakeman & Hornberger (1993)"),	//  0
			_TL("Croke et al. (2005)")	//	1

		)
	);

	Parameters.Add_Value(
		pNode,	"SNOW_TOOL",	_TL("Using the snow-melt module?"),
		_TL("If checked, snow-melt module is used."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL,	"WRITEALL_TS",	_TL("Write all calculated Time Series in a table?"),
		_TL("If checked, then a second output table with all Time Series data is created."),
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
	Parameters.Add_Table_Output(
		NULL	, "TABLEsettings", _TL("Table"),
		_TL("")
	);
}
//---------------------------------------------------------------------
// DIALOG 2
//---------------------------------------------------------------------

bool Cihacres_v1::CreateDialog2(bool bTMP, int storconf, int IHAC_version,
								CSG_Table *pTable,
								int dateField, int dischargeField)
{
	CSG_String		s;
	CSG_Parameters	P;  // used to add Parameters in the second dialog
	CSG_Parameter	*pNode, *pNode1;

	//	Dialog design
	P.Set_Name(_TL("IHACRES Model Parameters (Calibration)"));

	s.Printf(SG_T("Node1"), 1);
	pNode = P.Add_Node(NULL,s,SG_T("Time Range"),_TL(""));

	s.Printf(SG_T("FDAY") , 1-1);
	P.Add_String(pNode,s,_TL("First Day"),_TL(""),
				 pTable->Get_Record(0)->asString(dateField));

	s.Printf(SG_T("LDAY") , 1-2);
	P.Add_String(pNode,s,_TL("Last Day"),_TL(""),
				 pTable->Get_Record(pTable->Get_Record_Count()-1)->asString(dateField));


	if (bTMP) // if observed temperature data are used
	{
		s.Printf(SG_T("Node2"), 2);
		pNode = P.Add_Node(NULL,s,SG_T("Non-Linear Tool"),_TL(""));

		s.Printf(SG_T("Node2-1"), 2-1);
		pNode1 = P.Add_Node(pNode,s,SG_T("Temperature Modulation Factor"),_TL(""));

		P.Add_Value(
			pNode1	, "TFac"		, _TL("Temperature Modulation Factor (f)"),
			_TL("Temperature Modulation Factor f"),
			PARAMETER_TYPE_Double,
			1.0, 0.0001, true, 10.0, true
		);
	} // End if (bTMP)

	switch(IHAC_version)
	{
	case 0: // Jakeman & Hornberger (1993)
		break;
	case 1: // Croke et al. (2005)
		s.Printf(SG_T("Node4"), 4);
		pNode = P.Add_Node(NULL,s,SG_T("Soil Moisture Power Eq."),_TL(""));

		s.Printf(SG_T("Node4-1"), 4-1);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter l"),_TL(""));

		P.Add_Value(
			pNode1,"L",_TL("Soil moisture index (l)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.0, 0.0, true, 5.0, true
		);
		
		s.Printf(SG_T("Node4-2"), 4-2);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter p"),_TL(""));

		P.Add_Value(
			pNode1,"P",_TL("Power on soil (p)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.0, 0.0, true, 5.0, true
		);
		break;
	} // end switch (IHAC_version)

	switch(storconf)
	{
	case 0: // single storage
		s.Printf(SG_T("Node3"), 3);
		pNode = P.Add_Node(NULL,s,SG_T("Linear Tool"),_TL(""));

		s.Printf(SG_T("Node3-1"), 3-1);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter (a)"),_TL(""));

		P.Add_Value(
			pNode1,	"AFAC",	_TL("(a)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.8, -0.99, true, -0.01, true
		);

		s.Printf(SG_T("Node3-2"), 3-2);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter (b)"),_TL(""));

		P.Add_Value(
			pNode1,	"BFAC",	_TL("(b)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.2, 0.001, true, 1.0, true
		);
		break;

	case 1: // two parallel storages
		s.Printf(SG_T("Node3"), 3);
		pNode = P.Add_Node(NULL,s,SG_T("Linear Tool"),_TL(""));

		// Parameter a
		s.Printf(SG_T("Node3-1"), 3-1);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter (a)"),_TL(""));

		P.Add_Value(
			pNode1,	"AQ",	_TL("a(q)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.7, -0.99, true, -0.01, true
		);
		P.Add_Value(
			pNode1,	"AS",	_TL("a(s)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			-0.9, -0.99, true, -0.01, true
		);

		// Parameter b
		s.Printf(SG_T("Node3-2"), 3-2);
		pNode1 = P.Add_Node(pNode,s,SG_T("Parameter (b)"),_TL(""));

		P.Add_Value(
			pNode1,	"BQ",	_TL("b(q)"),
			_TL(""),
			PARAMETER_TYPE_Double,
			0.0, 0.0, true, 1.0, true
		);
		break;

	case 2: // two storages in series
		break;
	} // end switch (storconf)

	if (bSnowModule)
	{
	s.Printf(SG_T("Node5"), 5);
	pNode1 = P.Add_Node(NULL,s,SG_T("Snow Tool Parameters"),_TL(""));
		
		P.Add_Value(
			pNode1,	"T_RAIN",	_TL("Temperature Threshold for Rainfall"),
			_TL("Below this threshold precipitation will fall as snow"),
			PARAMETER_TYPE_Double,
			-1.0, -10.0, true, 10.0, true
		);
		P.Add_Value(
			pNode1,	"T_MELT",	_TL("Temperature Threshold for Melting"),
			_TL("Above this threshold snow will start to melt"),
			PARAMETER_TYPE_Double,
			1.0, -5.0, true, 10.0, true
		);
		P.Add_Value(
			pNode1,	"DD_FAC",	_TL("Day-Degree Factor"),
			_TL("Day-Degree Factor depends on catchment characteristics"),
			PARAMETER_TYPE_Double,
			0.7, 0.7, true, 9.2, true
		);
	}
	s.Printf(SG_T("Node6"), 6);
	pNode1 = P.Add_Node(NULL,s,SG_T("Time Delay after Start of Rainfall (INTEGER)"),_TL(""));
		
	P.Add_Value(
		pNode1,	"DELAY",	_TL("Time Delay (Rain-Runoff)"),
		_TL("The delay after the start of rainfall, before the discharge starts to rise."),
		PARAMETER_TYPE_Int,
		1, 0, true, 100, true
	);

	// End of second dialog design

	if( SG_UI_Dlg_Parameters(&P, _TL("Choose Time Range")) )
	{
		///////////////////////////////////////////////////////////////
		//
		//                ASSIGN DATA FROM SECOND DIALOG
		//
		///////////////////////////////////////////////////////////////
		date1		= P(CSG_String::Format(SG_T("FDAY"),dateField).c_str())
									->asString();
		date2		= P(CSG_String::Format(SG_T("LDAY"),dischargeField).c_str())
									->asString();
		// if observed temperature data are used
		if (bTMP) f	= P("TFac")		->asDouble();

		switch(IHAC_version)
		{
		case 0: break;
		case 1: 
			l		= P("L")			->asDouble();
			p		= P("P")			->asDouble();
			
			break;
		}

		if (bSnowModule)
		{
			T_Rain	= P("T_RAIN")		->asDouble();
			T_Melt	= P("T_MELT")		->asDouble();
			DD_FAC	= P("DD_FAC")		->asDouble();
		}
		switch (storconf)
		{
		case 0: // single storage
			a		= P("AFAC")			->asDouble();
			b		= P("BFAC")			->asDouble();
			delay	= P("DELAY")		->asInt();
			break;
		case 1: // two parallel storages
			aq		= P("AQ")			->asDouble();
			as		= P("AS")			->asDouble();
			bq		= P("BQ")			->asDouble();
			//bs		= P("BS")			->asDouble();
			bs = ihacres->Calc_Parm_BS(aq,as,bq);
			delay	= P("DELAY")		->asInt();
			break;
		case 2: // two storages in series
			break;
		} // end switch (storconf)
		return(true);
	} else {
		return(false);
	}
}