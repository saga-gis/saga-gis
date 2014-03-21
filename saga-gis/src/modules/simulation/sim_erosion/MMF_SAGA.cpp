
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      sim_erosion                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     MMF_SAGA.cpp                      //
//                                                       //
//                Copyright (C) 2009-12 by               //
//                    Volker Wichmann                    //
//                   Muhammad Setiawan                   //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//  Implementation of the CMMF_SAGA class.               //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MMF_SAGA.h"



///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMMF_SAGA::CMMF_SAGA(void)
{
//	CSG_Parameter	*pNodeTerrain, *pNodeVegetation, *pNodeSoil, *pNodeClimate, *pNodeOptions, *pNodeOutput;

	Set_Name	(_TL("MMF-SAGA Soil Erosion Model"));

	Set_Author	(SG_T("(c) 2009-2012 V. Wichmann, M. Setiawan"));
	
	Set_Description(_TW("Soil erosion modelling with a modified MMF (Morgan-Morgan-Finney) model "
						"(Morgan & Duzant 2008).<br/>"
						"This module is called MMF-SAGA because some things have been implemented differently "
						"compared to the original publication. The most important are:<br/><br/>"
						"<ul>"
						"<li>the extension of the model to support spatially distributed modelling</li>"
						"<li>the introduction of a \"Channel Network\" layer</li>"
						"<li>the introduction of a \"rainfall duration\" (time span) parameter</li>"
						"<li>the exposure of the flow depth parameter</li>"
						"</ul>"
						"<br/><br/>"
						"A more detailed description of the model, its modifications, and "
						"model application is provided by Setiawan (2012), chapter 6.<br/>"
						"Currently, a number of additional grid datasets are outputted to facilitate "
						"model evaluation. This can be easily changed within the source code.<br/>"
						"<br/><br/>"
						"References:<br/>"
						"<b>Morgan, R.P.C. (2001)</b>: A simple approach to soil loss prediction: a revised "
						"Morgan-Morgan-Finney model. Catena 44: 305-322.<br/><br/>"
						"<b>Morgan, R.P.C., Duzant, J.H. (2008)</b>: Modified MMF (Morgan-Morgan-Finney) model "
						"for evaluating effects of crops and vegetation cover on soil erosion. Earth "
						"Surf. Process. Landforms 32: 90-106.<br/><br/>"
						"<a href=\"http://sourceforge.net/projects/saga-gis/files/SAGA%20-%20Documentation/Modules/MMF-SAGA_Setiawan.pdf\">"
						"<b>Setiawan, M. A. (2012)</b>: Integrated Soil Erosion Management in the upper Serayu "
						"Watershed, Wonosobo District, Central Java Province, Indonesia. Dissertation at "
						"the Faculty of Geo- and Atmospheric Sciences of the University of Innsbruck, Austria.</a>"
						"<br/><br/>"
	));

	// comments on equation numbers refer to Morgan & Duzant (2008)

	
	//pNodeTerrain = Parameters.Add_Node(NULL, "TERRAIN", _TL("Terrain"), _TL("Terrain parameters"));
	Parameters.Add_Grid(
		NULL, "DTM", _TL("Digital Terrain Model"), 
		_TL("DTM, digital terrain model [m]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(
		NULL, "S", _TL("Slope"), 
		_TL("S, slope [rad]"), 
		PARAMETER_INPUT
	);
    Parameters.Add_Grid(
		NULL, "CHANNEL", _TL("Channel Network"), 
		_TL("Channel network, all other cells NoData"), 
		PARAMETER_INPUT_OPTIONAL
	);

	//pNodeVegetation = Parameters.Add_Node(NULL, "VEGETATION", _TL("Vegetation"), _TL("Vegetation parameters"));
	Parameters.Add_Grid(	
		NULL, "PI", _TL("Permament Interception"), 
		_TL("PI, permanent interception expressed as the proportion [between 0-1] of rainfall"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "CC", _TL("Canopy Cover"), 
		_TL("CC, canopy cover expressed as a portion [between 0-1] of the soil surface protected by vegetation or crop"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "PH", _TL("Plant Height"), 
		_TL("PH, plant height [m], representing the effective height from which raindrops fall from the crop or vegetation"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "EtEo", _TL("Ratio Evapotranspiration"), 
		_TL("Et/Eo, ratio of actual to potential evapotranspiration"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "GC", _TL("Ground cover"), 
		_TL("GC, Ground cover expressed as a portion [between 0-1] of the soil surface protected by vegetation or crop cover on the ground"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "D", _TL("Diameter plant elements"), 
		_TL("D, Average diameter [m] of the individual plants elements (stem, leaves) at the ground surface"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "NV", _TL("Number plant elements"), 
		_TL("NV, Number of plant elements per unit area [number/unit area] at the ground surface"), 
		PARAMETER_INPUT
	);

	//pNodeSoil = Parameters.Add_Node(NULL, "SOIL", _TL("Soil"), _TL("Soil parameters"));
	Parameters.Add_Grid(	
		NULL, "MS", _TL("Soil moisture (at FC)"), 
		_TL("MS, Soil moisture at field capacity [% w/w]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "BD", _TL("Bulk density top layer"), 
		_TL("BD, Bulk density of the top layer [Mg/m3]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "EHD", _TL("Effective hydrological depth"), 
		_TL("EHD, Effective hydrological depth of the soil [m]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "LP", _TL("Sat. lateral permeability"), 
		_TL("LP, Saturated lateral permeability of the soil [m/day]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "PER_C", _TL("Percentage clays"), 
		_TL("c, Percentage clays [%]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "PER_Z", _TL("Percentage silt"), 
		_TL("z, Percentage silt [%]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "PER_S", _TL("Percentage sand"), 
		_TL("s, Percentage sand [%]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "ST", _TL("Percentage rock fragments"), 
		_TL("ST, Percentage rock fragments on the soil surface [%]"), 
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "RFR", _TL("Surface roughness"), 
		_TL("RFR, Surface roughness [cm/m]"), 
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL, "TAB_METEO", _TL("Meteorological data"),
		_TL("Meteorological data for multiple timestep modelling [model step (day); temperature (Celsius); rainfall (mm), rainfall intensity (mm/h); rainfall duration (day); timespan (days)]"),
		PARAMETER_INPUT_OPTIONAL
	);
	Parameters.Add_FilePath( 
		Parameters("TAB_METEO"), "OUT_PATH", _TL("Output file path"), 
		_TL("Full path to the directory for the output grids of each model step"), 
		SG_T("\0*.*\0\0*.*\0"), _TL(""), true, true
	);

	//pNodeClimate = Parameters.Add_Node(NULL, "CLIMATE", _TL("Climate"), _TL("Climate parameters"));
	Parameters.Add_Value(
		NULL, "INTERFLOW", _TL("Simulate Interflow"),
		_TL("Simulate interflow"),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value( 
		Parameters("INTERFLOW"), "T", _TL("Mean temperature"), 
		_TL("T, mean temperature [degree C]"), 
		PARAMETER_TYPE_Double, 
		18.0
	);

	Parameters.Add_Value( 
		Parameters("INTERFLOW"), "TIMESPAN", _TL("Timespan (days)"), 
		_TL("The number of days to model."), 
		PARAMETER_TYPE_Int, 30.0,
        1.0, true, 365, true
	);

	Parameters.Add_Grid(	
		Parameters("INTERFLOW"), "IF", _TL("Interflow"), 
		_TL("IF"), 
		PARAMETER_OUTPUT_OPTIONAL
	);
	Parameters.Add_Value( 
		NULL, "R", _TL("Rainfall"), 
		_TL("R, height of precipitation in timespan [mm]"), 
		PARAMETER_TYPE_Double, 
		200.0
	);
	Parameters.Add_Value( 
		NULL, "I", _TL("Rainfall intensity"), 
		_TL("I, rainfall intensity [mm/h]"), 
		PARAMETER_TYPE_Double, 
		20.0
	);
	Parameters.Add_Value( 
		NULL, "Rn", _TL("Rainfall Duration"), 
		_TL("Rn, number of rain days in timespan [-]"), 
		PARAMETER_TYPE_Double, 
		20.0
	);

	CSG_String	choices;

	for (int i=0; i<MMF_KE_I_Count; i++)
		choices += CSG_String::Format(SG_T("%s|"), gMMF_KE_I_Key_Name[i]);

	Parameters.Add_Choice(
		NULL, "KE_I_METHOD", _TL("Relationship KE - I"),
		_TL("Relationship between kinetic energy (KE) and rainfall intensity (I)"),
		choices,
		0
	);

    //pNodeOptions = Parameters.Add_Node(NULL, "OPTIONS", _TL("Model Options"), _TL("Model parameters"));
    Parameters.Add_Value( 
		NULL, "FLOWD_VA", _TL("Flow Depth (actual flow velocity)"), 
		_TL("The flow depth used to calculate the actual flow velocity [m] (e.g. 0.005 unchannelled flow, 0.01 shallow rills, 0.25 deeper rills."), 
		PARAMETER_TYPE_Double, 0.005,
        0.00000001, true
	);
    Parameters.Add_Value(
        NULL, "CHANNELTRANSPORT", _TL("Route Soil along Channel Network"),
        _TL("Route soil loss along channel network to outlet"),
        PARAMETER_TYPE_Bool, false
    );


	//pNodeOutput = Parameters.Add_Node(NULL, "OUTPUT", _TL("Output"), _TL("Output parameters"));
	Parameters.Add_Grid(	
		NULL, "Q", _TL("Mean runoff"), 
		_TL("Q, estimation of mean runoff [mm]"), 
		PARAMETER_OUTPUT
	);
	Parameters.Add_Grid(	
		NULL, "SL", _TL("Mean soil loss"), 
		_TL("SL, estimation of mean soil loss [kg]"), 
		PARAMETER_OUTPUT
	);

	#ifdef _TMP_OUT			// define this preprocessor variable to get intermediate grids as output
		Parameters.Add_Grid(	
			NULL, "Rf", _TL("Effective Rainfall"), 
			_TL("Rf"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "KE", _TL("Total Kinetic Energy"), 
			_TL("KE"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
				NULL, "Rc", _TL("Soil moisture storage capacity"), 
				_TL("Rc"), 
				PARAMETER_OUTPUT
			);
		Parameters.Add_Grid(	
			NULL, "TCc", _TL("Transport Capacity Clay"), 
			_TL("TCc"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "TCz", _TL("Transport Capacity Silt"), 
			_TL("TCz"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "TCs", _TL("Transport Capacity Sand"), 
			_TL("SLs"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "Gc", _TL("Available Clay"), 
			_TL("Gc"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "Gz", _TL("Available Silt"), 
			_TL("Gz"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "Gs", _TL("Available Sand"), 
			_TL("Gs"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "SLc", _TL("Sediment Balance Clay"), 
			_TL("SLc"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "SLz", _TL("Sediment Balance Silt"), 
			_TL("SLz"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "SLs", _TL("Sediment Balance Sand"), 
			_TL("SLs"), 
			PARAMETER_OUTPUT
		);
		Parameters.Add_Grid(	
			NULL, "TCONDc", _TL("Transport Condition Clay"), 
			CSG_String::Format(_TL("Sediment Limited [%d], Transport Limited (SL = TC) [%d], Transport Limited (SL = G) [%d]"), TCOND_SED_LIMITED, TCOND_TRANS_LIMITED_TC, TCONF_TRANS_LIMITED_G), 
			PARAMETER_OUTPUT, true, SG_DATATYPE_Int
		);
		Parameters.Add_Grid(	
			NULL, "TCONDz", _TL("Transport Condition Silt"), 
			CSG_String::Format(_TL("Sediment Limited [%d], Transport Limited (SL = TC) [%d], Transport Limited (SL = G) [%d]"), TCOND_SED_LIMITED, TCOND_TRANS_LIMITED_TC, TCONF_TRANS_LIMITED_G), 
			PARAMETER_OUTPUT, true, SG_DATATYPE_Int
		);
		Parameters.Add_Grid(	
			NULL, "TCONDs", _TL("Transport Condition Sand"), 
			CSG_String::Format(_TL("Sediment Limited [%d], Transport Limited (SL = TC) [%d], Transport Limited (SL = G) [%d]"), TCOND_SED_LIMITED, TCOND_TRANS_LIMITED_TC, TCONF_TRANS_LIMITED_G), 
			PARAMETER_OUTPUT, true, SG_DATATYPE_Int
		);
		Parameters.Add_Grid(	
			NULL, "W_up", _TL("Upslope Flow Width"), 
			_TL("W_up"), 
			PARAMETER_OUTPUT
		);
	#endif
}

//---------------------------------------------------------
CMMF_SAGA::~CMMF_SAGA(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMMF_SAGA::On_Execute(void)
{
	// input grids
	CSG_Grid	*pDTM, *pS, *pChannel, *pPI, *pCC, *pPH, *pMS, *pBD, *pEHD, *pEtEo, *pLP, *pGC, *pPer_c, *pPer_z, *pPer_s, *pST, *pD, *pNV, *pRFR;
	// temporary grids
	CSG_Grid	*pRf, *pKE, *pRc, *pSLc, *pSLz, *pSLs, *pW_up;
	#ifdef _TMP_OUT
	CSG_Grid	*pTCc, *pTCz, *pTCs, *pGc, *pGz, *pGs, *pTCondc, *pTCondz, *pTConds;
	#endif
	// optional meteo table
	CSG_Table	*pMeteoTab;
	CSG_String	sOutPath;

	// interflow
	bool		bInterflow, bFreeIF = false;
	CSG_Grid	*pIF = NULL;
	double		T = 0.0;
	double		Rmod = 0.0, IF = 0.0, Z = 0.0, E = 0.0, Rc = 0.0, d_IF = 0.0;
	// output grids
	CSG_Grid	 *pQ, *pSL;

	// input parameters
	double		R, I, Rn;
	int			KE_I_method;
	
	// temporary variables
	double		    LD, DT, KE_DT, KE_LD, L, Q, Ro;
	double		    per_c, per_z, per_s, ST, KE, GC, sin_S;
	double		    n_manning, d_flow, v_flow_a, v_flow_b, v_flow_v, v_flow_t, v_flow;
	double		    DEPc, DEPz, DEPs, Gc, Gz, Gs, TCc, TCz, TCs, SLc, SLz, SLs;

    // variables
	//double			slopeDeg;
	double			slopeFract;
	double		    timespan, flowd_va;
	int			    x, y, ix, iy;
	double		    z, d, dz[8], dzSum, d_Q, d_SLc, d_SLz, d_SLs;
	double		    W_up, W_down, f_w;
	const double	Convergence	= 1.1;  // convergence factor MFD after FREEMAN (1991)
	int			    steepestN;
	CSG_String	    InvalidGrid;
    bool            bChannelT;
	int				iRuns, iMstep;

    // change this to false to only calculate runoff
	bool		    bCalcSoil = true;


	// detachability of the soil by raindrop impact
	double		Kc = 0.1;		// [g/J]
	double		Kz = 0.5;		// [g/J]
	double		Ks = 0.3;		// [g/J]
	double		Fc, Fz, Fs, F;

	// detachability of the soil by runoff
	double		DRc = 1.0;		// [-]
	double		DRz = 1.6;		// [-]
	double		DRs = 1.5;		// [-]
	double		Hc, Hz, Hs, H;

	// particle fall number
	double	vs_c = 0.000002;	// fall velocity [m/s] clay
	double	vs_z = 0.002;		// fall velocity [m/s] silt
	double	vs_s = 0.02;		// fall velocity [m/s] sand
	double	Nf_c, Nf_z, Nf_s;
	// recalculation of particle fall number for sediment balance
	double	vs_c1 = 0.00002;	// fall velocity [m/s] clay
	double	vs_z1 = 0.02;		// fall velocity [m/s] silt
	double	vs_s1 = 0.2;		// fall velocity [m/s] sand



	pDTM		= Parameters("DTM")->asGrid();
	pS			= Parameters("S")->asGrid();
    pChannel    = Parameters("CHANNEL")->asGrid();
	pPI			= Parameters("PI")->asGrid();
	pCC			= Parameters("CC")->asGrid();
	pPH			= Parameters("PH")->asGrid();
	pMS			= Parameters("MS")->asGrid();
	pBD			= Parameters("BD")->asGrid();
	pEHD		= Parameters("EHD")->asGrid();
	pEtEo		= Parameters("EtEo")->asGrid();
	pLP			= Parameters("LP")->asGrid();
	pGC			= Parameters("GC")->asGrid();
	pPer_c		= Parameters("PER_C")->asGrid();
	pPer_z		= Parameters("PER_Z")->asGrid();
	pPer_s		= Parameters("PER_S")->asGrid();
	pST			= Parameters("ST")->asGrid();
	pD			= Parameters("D")->asGrid();
	pNV			= Parameters("NV")->asGrid();
	pRFR		= Parameters("RFR")->asGrid();
	
	pMeteoTab	= Parameters("TAB_METEO")->asTable();
	sOutPath	= Parameters("OUT_PATH")->asString();

	if (pMeteoTab != NULL)
	{
		if (sOutPath.Length() == 0)
		{
			SG_UI_Msg_Add_Error(_TL("No path for output files specified!"));
			SG_UI_Msg_Add(_TL("Please provide a path for the output files."), true);
			return (false);
		}

		iRuns = pMeteoTab->Get_Count();
	}
	else
		iRuns = 1;


	// check if all input grids are valid within the bounds of the DTM
	//----------------------------------------------------------------
	for (y=0; y<Get_NY(); y++)
	{
		for (x=0; x<Get_NX(); x++)
		{
			if (!pDTM->is_NoData(x, y))
			{
				InvalidGrid = SG_T("");

				if (pS->is_NoData(x, y))
					InvalidGrid = SG_T("S");
				if (pPI->is_NoData(x, y))
					InvalidGrid = SG_T("PI");
				if (pCC->is_NoData(x, y))
					InvalidGrid = SG_T("CC");
				if (pPH->is_NoData(x, y))
					InvalidGrid = SG_T("PH");
				if (pMS->is_NoData(x, y))
					InvalidGrid = SG_T("MS");
				if (pBD->is_NoData(x, y))
					InvalidGrid = SG_T("BD");
				if (pEHD->is_NoData(x, y))
					InvalidGrid = SG_T("EHD");
				if (pEtEo->is_NoData(x, y))
					InvalidGrid = SG_T("EtEo");
				if (pLP->is_NoData(x, y))
					InvalidGrid = SG_T("LP");
				if (pGC->is_NoData(x, y))
					InvalidGrid = SG_T("GC");
				if (pPer_c->is_NoData(x, y))
					InvalidGrid = SG_T("PER_C");
				if (pPer_z->is_NoData(x, y))
					InvalidGrid = SG_T("PER_Z");
				if (pPer_s->is_NoData(x, y))
					InvalidGrid = SG_T("PER_S");
				if (pST->is_NoData(x, y))
					InvalidGrid = SG_T("ST");
				if (pD->is_NoData(x, y))
					InvalidGrid = SG_T("D");
				if (pNV->is_NoData(x, y))
					InvalidGrid = SG_T("NV");
				if (pRFR->is_NoData(x, y))
					InvalidGrid = SG_T("RFR");

				if (InvalidGrid.Length() > 0)
				{
					Error_Set(CSG_String::Format(_TL("NoData encountered in input grid %s: x %d (%.2f), y %d (%.2f)"),
						InvalidGrid.c_str(), x, pDTM->Get_System().Get_xGrid_to_World(x), y, pDTM->Get_System().Get_yGrid_to_World(y)));
					return (false);
				}
			}
		}
	}
	
	#ifdef _TMP_OUT								
		pRf		= Parameters("Rf")->asGrid();
		pKE		= Parameters("KE")->asGrid();
		pRc		= Parameters("Rc")->asGrid();
		pSLc	= Parameters("SLc")->asGrid();
		pSLz	= Parameters("SLz")->asGrid();
		pSLs	= Parameters("SLs")->asGrid();
		pW_up	= Parameters("W_up")->asGrid();

		pTCc	= Parameters("TCc")->asGrid();
		pTCz	= Parameters("TCz")->asGrid();
		pTCs	= Parameters("TCs")->asGrid();
		pGc		= Parameters("Gc")->asGrid();
		pGz		= Parameters("Gz")->asGrid();
		pGs		= Parameters("Gs")->asGrid();
		pTCondc = Parameters("TCONDc")->asGrid();
		pTCondz = Parameters("TCONDz")->asGrid();
		pTConds = Parameters("TCONDs")->asGrid();
		pTCc->Assign_NoData();
		pTCz->Assign_NoData();
		pTCs->Assign_NoData();
		pGc->Assign_NoData();
		pGz->Assign_NoData();
		pGs->Assign_NoData();
		pTCondc->Assign_NoData();
		pTCondz->Assign_NoData();
		pTConds->Assign_NoData();
	#endif

	pIF			= Parameters("IF")->asGrid();
	pQ			= Parameters("Q")->asGrid();
	pSL			= Parameters("SL")->asGrid();

	R		    = Parameters("R")->asDouble();
	I		    = Parameters("I")->asDouble();
	Rn		    = Parameters("Rn")->asDouble();
	KE_I_method	= Parameters("KE_I_METHOD")->asInt();
    timespan    = Parameters("TIMESPAN")->asInt();
    flowd_va    = Parameters("FLOWD_VA")->asDouble();
    bChannelT   = Parameters("CHANNELTRANSPORT")->asBool();
	bInterflow	= Parameters("INTERFLOW")->asBool();
	T			= Parameters("T")->asDouble();

	if (bInterflow && pIF == NULL)
	{
		pIF		= SG_Create_Grid(pDTM);
		bFreeIF = true;
	}


	// create temporary grids
	#ifndef _TMP_OUT
		pRf		= SG_Create_Grid(pDTM->Get_System());
		pKE		= SG_Create_Grid(pDTM->Get_System());
		pRc		= SG_Create_Grid(pDTM->Get_System());
		pSLc	= SG_Create_Grid(pDTM->Get_System());
		pSLz	= SG_Create_Grid(pDTM->Get_System());
		pSLs	= SG_Create_Grid(pDTM->Get_System());
		pW_up	= SG_Create_Grid(pDTM->Get_System());
	#endif


	for (int iRun=0; iRun<iRuns; iRun++)
	{
		SG_UI_Process_Set_Text(CSG_String::Format(_TL("Model step %d/%d ..."), iRun + 1, iRuns));

		if (pMeteoTab != NULL)
		{
			iMstep		= pMeteoTab->Get_Record(iRun)->asInt(0);
			T			= pMeteoTab->Get_Record(iRun)->asDouble(1);
			R			= pMeteoTab->Get_Record(iRun)->asDouble(2);
			I			= pMeteoTab->Get_Record(iRun)->asDouble(3);
			Rn			= pMeteoTab->Get_Record(iRun)->asDouble(4);
			timespan	= pMeteoTab->Get_Record(iRun)->asInt(5);
		}

		if (bInterflow && Rn > timespan)
		{
			Error_Set(CSG_String::Format(_TL("Number of raindays is greater than model timespan!")));
			return (false);
		}

		// Initialize Grids
		pSLc->Assign(0.0);
		pSLz->Assign(0.0);
		pSLs->Assign(0.0);
		pQ->Assign(0.0);
		pW_up->Assign(0.0);
		if (bInterflow)
			pIF->Assign(0.0);


		// Estimation of rainfall energy (effective rainfall, leaf drainage, direct throughfall, kinetic energy)
		//------------------------------------------------------------------------------------------------------
		for (y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for (x=0; x<Get_NX(); x++)
			{
				if (!pS->is_NoData(x, y) && !pPI->is_NoData(x, y) && !pCC->is_NoData(x, y) && !pPH->is_NoData(x, y))
				{
					pRf->Set_Value(x, y, (R * (1-pPI->asDouble(x, y))) / cos(pS->asDouble(x, y)));	// Equ. (1)
					LD = pRf->asDouble(x, y) * pCC->asDouble(x, y);									// Equ. (2)
					DT = pRf->asDouble(x, y) - LD;													// Equ. (3)
				
					switch (KE_I_method)		// Equ. (4), see Table 2 in Morgan (2001)
					{
					case MMF_KE_I_WISCHMEIER_SMITH_1978:
					default:
						KE_DT = DT * (11.87 + 8.73 * log10(I));				break;
					case MMF_KE_I_MARSHALL_PALMER:
						KE_DT = DT * (8.95 + 8.44 * log10(I));				break;
					case MMF_KE_I_ZANCHI_TORRI_1980:
						KE_DT = DT * (9.81 + 11.25 * log10(I));				break;
					case MMF_KE_I_COUTINHO_TOMAS_1995:
						KE_DT = DT * (35.9 * (1 - 0.56 * exp(-0.034 * I)));	break;
					case MMF_KE_I_HUDSON_1965:
						KE_DT = DT * (29.8 - (127.5 / I));					break;
					case MMF_KE_I_ONEGA_ET_AL_1998:
						KE_DT = DT * (9.81 + 10.6 * log10(I));				break;
					case MMF_KE_I_ROSEWELL_1986:
						KE_DT = DT * (29.0 * (1 - 0.6 * exp(-0.04 * I)));	break;
					case MMF_KE_I_MCISAAC_1990:
						KE_DT = DT * (26.8 * (1 - 0.29 * exp(-0.049 * I))); break;
					}
				
					if (pPH->asDouble(x, y) < 0.15)
						KE_LD = 0.0;																// Equ. (5)
					else
						KE_LD = LD * (15.8 * pow(pPH->asDouble(x, y), 0.5) - 5.87);					// Equ. (6)

					pKE->Set_Value(x, y, KE_DT + KE_LD);											// Equ. (7)
				}
				else
				{
					pRf->Set_NoData(x, y);
					pKE->Set_NoData(x, y);
				}
			}
		}

		// estimation of runoff
		//-----------------------

		if (bInterflow)
		{
			Z		= 300 + (25 * T) + (0.05 * T * T * T);		// Equ. (12)
			Rmod	= R * (365/timespan);						// scale up from timespan to annual values
			E		= Rmod / sqrt(0.9 + (Rmod*Rmod)/(Z*Z));		// Equ. (11)
			E		= E / (365/timespan);                       // calculate E for timespan
		}

		Ro	= R / Rn;											// Equ. (9) mean rain per rain day


		for (sLong n=0; n<Get_NCells() && Set_Progress(n); n++)
		{
			pDTM->Get_Sorted(n, x, y, true, false);

			if (pDTM->is_NoData(x, y))	// pMS, pBD, pEHD, pEtEo, pRf, pLP, pS
			{
				pQ->Set_NoData(x, y);
				pSL->Set_NoData(x, y);
				pRc->Set_NoData(x, y);
				if (bInterflow)
					pIF->Set_NoData(x, y);
			}
			else
			{
				if (bInterflow)
				{
					Rc = (1000 * pMS->asDouble(x, y) * pBD->asDouble(x, y) * pEHD->asDouble(x, y) * pow(pEtEo->asDouble(x, y), 0.5)) - pIF->asDouble(x, y);	// Equ. (8)
					if (Rc < 0.0)
						Rc = 0.0;
					pRc->Set_Value(x, y, Rc);
				}
				else
					pRc->Set_Value(x, y, 1000 * pMS->asDouble(x, y) * pBD->asDouble(x, y) * pEHD->asDouble(x, y) * pow(pEtEo->asDouble(x, y), 0.5));		// Equ. (8), modified

				L = Get_Cellsize() / cos(pS->asDouble(x, y));	// slope length, approx. from cellsize and slope

				if (pChannel != NULL && !pChannel->is_NoData(x, y))
					Q = pRf->asDouble(x, y) + pQ->asDouble(x, y);
				else
					Q = (pRf->asDouble(x, y) + pQ->asDouble(x, y)) * exp((-1.0 * pRc->asDouble(x, y)) / Ro) * pow(L/10,0.1);		// Equ. (9)
            
				//double Qe = pRf->asDouble(x, y) * exp((-1.0 * pRc->asDouble(x, y)) / Ro);
			
				if (bInterflow)
				{
					IF = ((R - E - Q) * pLP->asDouble(x, y) * sin(pS->asDouble(x, y))) / timespan;		// Equ. (13)
					if (IF < 0)
						IF = 0.0;
				}


				z		= pDTM->asDouble(x, y);
				dzSum	= 0.0;
				W_down	= 0.0;


				// uncomment this to just route number of cells to verify flow routing
				/*if (pChannel != NULL && !pChannel->is_NoData(x, y))
					Q = 1.0 + pQ->asDouble(x, y);
				else
					Q = 0.0;
				if (bInterflow)
					IF = 0.0;
				*/

				if (pChannel != NULL && !pChannel->is_NoData(x, y))
				{
					steepestN   = pDTM->Get_Gradient_NeighborDir(x, y);
					dzSum       = 1.0;
				}
				else	// MFD Freeman 1991
				{
					for (int i=0; i<8; i++)
					{
						ix	= Get_xTo(i, x);			
						iy	= Get_yTo(i, y);			

						if( pDTM->is_InGrid(ix, iy) )
						{
							d		= z - pDTM->asDouble(ix, iy);
						}
						else
						{
							ix		= Get_xTo(i + 4, x);
							iy		= Get_yTo(i + 4, y);

							if( pDTM->is_InGrid(ix, iy) )
							{
								d		= pDTM->asDouble(ix, iy) - z;
							}
							else
							{
								d		= 0.0;
							}
						}

						if( d > 0.0 )
						{
							dzSum	+= (dz[i]	= pow(d / Get_Length(i), Convergence));
						}
						else
						{
							dz[i]	= 0.0;
						}
					}
				}
			
				// flow routing
				//-----------------------------------------------------
				if (dzSum > 0.0)
				{
					if (pChannel != NULL && !pChannel->is_NoData(x, y))
					{
						if (steepestN != -1)
						{
							ix	= Get_xTo(steepestN, x);
							iy	= Get_yTo(steepestN, y);

							pQ->Add_Value(ix, iy, Q);								// distribute				
							pW_up->Add_Value(ix, iy, Get_Length(steepestN));		// upslope CL of ix,iy
							W_down = Get_Length(steepestN);							// downslope CL of x,y
							if (bInterflow)
								pIF->Add_Value(ix, iy, IF);
						}
					}
					else
					{
						d_Q		=  Q / dzSum;
						if (bInterflow)
							d_IF =  IF / dzSum;

						for (int i=0; i<8; i++)
						{
							if (dz[i] > 0.0)
							{
								ix	= Get_xTo(i, x);
								iy	= Get_yTo(i, y);

								if( pDTM->is_InGrid(ix, iy) )
								{
									pQ->Add_Value(ix, iy, d_Q * dz[i]);		// distribute
									f_w = 0.5 * Get_Cellsize() / Get_System()->Get_UnitLength(i);
									pW_up->Add_Value(ix, iy, f_w);			// upslope CL of ix,iy
									W_down += f_w;							// downslope CL of x,y
									if (bInterflow)
										pIF->Add_Value(ix, iy, d_IF * dz[i]);
								}
							}
						}
					}
				}

				pQ->Set_Value(x, y, Q);		// write local values
				if (bInterflow)
					pIF->Set_Value(x, y, IF);

				if (bCalcSoil)
				{
					// detachment of soil particles ...
					per_c	= pPer_c->asDouble(x, y);
					per_z	= pPer_z->asDouble(x, y);
					per_s	= pPer_s->asDouble(x, y);
					ST		= pST->asDouble(x, y);
					KE		= pKE->asDouble(x, y);
					GC		= pGC->asDouble(x, y);
					sin_S	= sin(pS->asDouble(x, y));

					// ... by raindrop impact
					Fc	= Kc * (per_c / 100.0) * (1.0 - ST) * KE * 0.001;		// Equ. (14)
					Fz	= Kz * (per_z / 100.0) * (1.0 - ST) * KE * 0.001;		// Equ.	(15)
					Fs	= Ks * (per_s / 100.0) * (1.0 - ST) * KE * 0.001;		// Equ. (16)
					F	= Fc + Fz + Fs;											// Equ. (17)
					// ... by runoff
					Hc	= DRc * (per_c / 100.0) * pow(Q, 1.5) * (1.0 - (GC + ST)) * pow(sin_S, 0.3) * 0.001;		// Equ. (18)
					Hz	= DRz * (per_z / 100.0) * pow(Q, 1.5) * (1.0 - (GC + ST)) * pow(sin_S, 0.3) * 0.001;		// Equ. (19)
					Hs	= DRs * (per_s / 100.0) * pow(Q, 1.5) * (1.0 - (GC + ST)) * pow(sin_S, 0.3) * 0.001;		// Equ. (20)
					H	= Hc + Hz +Hs;																				// Equ. (21)

					// flow velocity ...				/// input map with 3 classes
					// ... for standard bare soil
					n_manning = 0.015;
					d_flow = 0.005;
	
					slopeFract = tan(pS->asDouble(x, y));
					if (slopeFract == 0.0)
						slopeFract = 0.001;	// workaround for velocity calculations, otherwise v_flow and thus TC may become NaN!

					v_flow_b = 1.0 / n_manning * pow(d_flow, 0.67) * pow(slopeFract, 0.5);	// Equ. (22)
				
					// ... actual flow velocity
					n_manning = 0.015;
					// if ....		     // global method							
					//d_flow = 0.005;	// unchannelled flow
					//d_flow = 0.01;	// shallow rills
					//d_flow = 0.25;	// deeper rills
					d_flow = flowd_va;  // user supplied value
					v_flow_a = 1.0 / n_manning * pow(d_flow, 0.67) * pow(slopeFract, 0.5) * pow(M_EULER, (-0.018*ST));	// Equ. (23)

					// ... for vegetated conditions
					if (pGC->asDouble(x, y) > 0.01)			// assuming bare soil as no ground cover
						v_flow_v = pow(((2 * 9.81) / (pD->asDouble(x, y) * pNV->asDouble(x, y))), 0.5) * pow(slopeFract, 0.5);	// Equ. (24)
					else
						v_flow_v = 1.0;						// bare soil

					// ... for the effect of tillage
					d_flow = 0.005;
					//n_manning = pow(M_EULER, log(-2.1132 + 0.0349 * pRFR->asDouble(x, y)));	// Equ. (27)
					n_manning = pow(M_EULER, -2.1132 + 0.0349 * pRFR->asDouble(x, y));	// Equ. (27)
					v_flow_t = 1.0 / n_manning * pow(d_flow, 0.67) * pow(slopeFract, 0.5);
					// v_flow_t = 1.0 if not under arable cultivation and natural soil surface roughness is not accounted for

					// particle fall number
					//d_flow = 0.005;
					d_flow = flowd_va;  // user supplied value

					if (pGC->asDouble(x, y) > 0.01)						// assuming bare soil as no ground cover
					{
						Nf_c = (L * vs_c) / (v_flow_v * d_flow);		// Equ. (28)
						Nf_z = (L * vs_z) / (v_flow_v * d_flow);		// Equ. (29)
						Nf_s = (L * vs_s) / (v_flow_v * d_flow);		// Equ. (30)
					}
					else												// bare soil
					{
						Nf_c = (L * vs_c) / (v_flow_b * d_flow);		// Equ. (28)
						Nf_z = (L * vs_z) / (v_flow_b * d_flow);		// Equ. (29)
						Nf_s = (L * vs_s) / (v_flow_b * d_flow);		// Equ. (30)
					}

					// percentage of detached sediment deposited
					DEPc = 44.1 * pow(Nf_c, 0.29);						// Equ. (31)
					if (DEPc > 100.0)
						DEPc = 100.0;
					DEPz = 44.1 * pow(Nf_z, 0.29);						// Equ. (32)
					if (DEPz > 100.0)
						DEPz = 100.0;
					DEPs = 44.1 * pow(Nf_s, 0.29);						// Equ. (33)
					if (DEPs > 100.0)
						DEPs = 100.0;

					// delivery of detached particles to runoff

					if (W_down <= 0.0)
						W_down = 0.5 * Get_Cellsize();

					if (pW_up->asDouble(x, y) == 0)
						W_up = W_down;
				

					Gc	= (Fc + Hc) * (1.0 - DEPc / 100.0) + (pSLc->asDouble(x, y) * W_up/W_down);	// Equ. (35)
					Gz	= (Fz + Hz) * (1.0 - DEPz / 100.0) + (pSLz->asDouble(x, y) * W_up/W_down);	// Equ. (36)
					Gs	= (Fs + Hs) * (1.0 - DEPs / 100.0) + (pSLs->asDouble(x, y) * W_up/W_down);	// Equ. (37)
					//G	= Gc + Gz + Gs;																		// Equ. (38)


					// transport capacity of runoff
					v_flow = (v_flow_a * v_flow_v * v_flow_t) / v_flow_b;
					TCc = v_flow * (per_c / 100.0) * pow(Q, 2) * sin_S * 0.001;	// Equ. (39)
					TCz = v_flow * (per_z / 100.0) * pow(Q, 2) * sin_S * 0.001;	// Equ. (40)
					TCs = v_flow * (per_s / 100.0) * pow(Q, 2) * sin_S * 0.001;	// Equ. (41)

					// sediment balance
					if (TCc >= Gc)
					{
						SLc = Gc;
						#ifdef _TMP_OUT
						pTCondc->Set_Value(x, y, TCOND_SED_LIMITED);
						#endif
					}
					else	// recalculation
					{
						if (pGC->asDouble(x, y) > 0.01)								// assuming bare soil as no ground cover
							Nf_c = (L * vs_c1) / (v_flow_v * d_flow);				// Equ. (28)
						else														// bare soil
							Nf_c = (L * vs_c1) / (v_flow_b * d_flow);				// Equ. (28)

						DEPc = 44.1 * pow(Nf_c, 0.29);								// Equ. (31)
						if (DEPc > 100.0)
							DEPc = 100.0;
						Gc	= Gc * (1.0 - DEPc / 100.0);							// Equ. (45)
						if (TCc >= Gc)
						{
							SLc = TCc;
							#ifdef _TMP_OUT
							pTCondc->Set_Value(x, y, TCOND_TRANS_LIMITED_TC);
							#endif
						}
						else
						{
							SLc = Gc;
							#ifdef _TMP_OUT
							pTCondc->Set_Value(x, y, TCONF_TRANS_LIMITED_G);
							#endif
						}
					}

					if (TCz >= Gz)
					{
						SLz = Gz;
						#ifdef _TMP_OUT
						pTCondz->Set_Value(x, y, 0);
						#endif
					}
					else	// recalculation
					{
						if (pGC->asDouble(x, y) > 0.01)								// assuming bare soil as no ground cover
							Nf_z = (L * vs_z1) / (v_flow_v * d_flow);				// Equ. (28)
						else														// bare soil
							Nf_z = (L * vs_z1) / (v_flow_b * d_flow);				// Equ. (28)

						DEPz = 44.1 * pow(Nf_z, 0.29);								// Equ. (31)
						if (DEPz > 100.0)
							DEPz = 100.0;
						Gz	= Gz * (1.0 - DEPz / 100.0);							// Equ. (46)
						if (TCz >= Gz)
						{
							SLz = TCz;
							#ifdef _TMP_OUT
							pTCondz->Set_Value(x, y, 1);
							#endif
						}
						else
						{
							SLz = Gz;
							#ifdef _TMP_OUT
							pTCondz->Set_Value(x, y, 2);
							#endif
						}
					}

					if (TCs >= Gs)
					{
						SLs = Gs;
						#ifdef _TMP_OUT
						pTConds->Set_Value(x, y, 0);
						#endif
					}
					else	// recalculation
					{
						if (pGC->asDouble(x, y) > 0.01)								// assuming bare soil as no ground cover
							Nf_s = (L * vs_s1) / (v_flow_v * d_flow);				// Equ. (28)
						else														// bare soil
							Nf_s = (L * vs_s1) / (v_flow_b * d_flow);				// Equ. (28)

						DEPs = 44.1 * pow(Nf_s, 0.29);								// Equ. (31)
						if (DEPs > 100.0)
							DEPs = 100.0;
						Gs	= Gs * (1.0 - DEPs / 100.0);							// Equ. (47)
						if (TCs >= Gs)
						{
							SLs = TCs;
							#ifdef _TMP_OUT
							pTConds->Set_Value(x, y, 1);
							#endif
						}
						else
						{
							SLs = Gs;
							#ifdef _TMP_OUT
							pTConds->Set_Value(x, y, 2);
							#endif
						}
					}

					SLc *= pDTM->Get_Cellarea();
					SLz *= pDTM->Get_Cellarea();
					SLs *= pDTM->Get_Cellarea();

					// sediment routing
					//-----------------------------------------------------
					if (dzSum > 0.0)
					{
						if (pChannel != NULL && !pChannel->is_NoData(x, y) && bChannelT)		// Transport along channel, ignore (new) sediment balance within channel
						{
							if (steepestN != -1)
							{					
								ix	= Get_xTo(steepestN, x);
								iy	= Get_yTo(steepestN, y);

								pSLc->Add_Value(ix, iy, pSLc->asDouble(x, y));
								pSLz->Add_Value(ix, iy, pSLz->asDouble(x, y));
								pSLs->Add_Value(ix, iy, pSLs->asDouble(x, y));
							}
						}
						else if (pChannel == NULL || (pChannel != NULL && pChannel->is_NoData(x, y)))		// no channel grid used or no channel cell
						{
							d_SLc	=  SLc / dzSum;
							d_SLz	=  SLz / dzSum;
							d_SLs	=  SLs / dzSum;

							for (int i=0; i<8; i++)
							{
								if (dz[i] > 0.0)
								{
									ix	= Get_xTo(i, x);
									iy	= Get_yTo(i, y);

									if( pDTM->is_InGrid(ix, iy) )
									{
										pSLc->Add_Value(ix, iy, d_SLc * dz[i]);		// distribute
										pSLz->Add_Value(ix, iy, d_SLz * dz[i]);
										pSLs->Add_Value(ix, iy, d_SLs * dz[i]);
									}
								}
							}
						}
					}

					if (pChannel != NULL && !pChannel->is_NoData(x, y))			// channel grid and channel cell: only write incoming material, ignore (new) sediment balance
					{
						pSL->Set_Value(x, y, pSLc->asDouble(x, y) + pSLz->asDouble(x, y) + pSLs->asDouble(x, y));
						pSLc->Set_Value(x, y, pSLc->asDouble(x, y));
						pSLz->Set_Value(x, y, pSLz->asDouble(x, y));
						pSLs->Set_Value(x, y, pSLs->asDouble(x, y));
					}
					else  // calc mean annual soil loss
					{
						pSL->Set_Value(x, y, SLc + SLz + SLs);
						pSLc->Set_Value(x, y, SLc);
						pSLz->Set_Value(x, y, SLz);
						pSLs->Set_Value(x, y, SLs);
					}

					#ifdef _TMP_OUT
					pTCc->Set_Value(x, y, TCc * pDTM->Get_Cellarea());
					pTCz->Set_Value(x, y, TCz * pDTM->Get_Cellarea());
					pTCs->Set_Value(x, y, TCs * pDTM->Get_Cellarea());

					pGc->Set_Value(x, y, Gc * pDTM->Get_Cellarea());
					pGz->Set_Value(x, y, Gz * pDTM->Get_Cellarea());
					pGs->Set_Value(x, y, Gs * pDTM->Get_Cellarea());
					#endif
				}// bCalcSoil
			}// not NoData
		}// for

		if (pMeteoTab != NULL)
		{
			pQ->Save(CSG_String::Format(SG_T("%s\\%03d_Runoff"), sOutPath.c_str(), iRun + 1));
			pSL->Save(CSG_String::Format(SG_T("%s\\%03d_SoilLoss"), sOutPath.c_str(), iRun + 1));
		}
	}// for model step

	//-----------------------------------------------------
	// delete temporary grids
	#ifndef _TMP_OUT
		delete(pRf);
		delete(pKE);
		delete(pRc);
		delete(pSLc);
		delete(pSLz);
		delete(pSLs);
		delete(pW_up);
	#endif

	if (bFreeIF)
		delete(pIF);

	return( true );
}




///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
