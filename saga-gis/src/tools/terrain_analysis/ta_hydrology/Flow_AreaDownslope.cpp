/**********************************************************
 * Version $Id: Flow_AreaDownslope.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Flow_AreaDownslope.cpp                 //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Flow_AreaDownslope.h"
#include "Flow_Parallel.h"
#include "Flow_RecursiveDown.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_AreaDownslope::CFlow_AreaDownslope(void)
{
	Set_Name		(_TL("Downslope Area"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"This interactive tool allows you to specify source cells (with a left mouse click), "
		"for which the downslope area shall be identified. "
		"For the 'Deterministic Infinity' and 'Multiple Flow Direction' algorithms, "
		"which are able to simulate flow divergence, the result will give "
		"the percentage of the source cell's flow that drains through each cell."
	));

	Add_Reference("Bauer, J., Rohdenburg, H. & Bork, H.-R.", "1985",
		"Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse",
		"Landschaftsgenese und Landschaftsoekologie, H.10, p.1-15."
	);

	Add_Reference("Costa-Cabral, M. & Burges, S.J.", "1994",
		"Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas",
		"Water Resources Research, 30:1681-1692.",
		SG_T("https://www.researchgate.net/profile/Mariza_Costa-Cabral/publication/233756725_Digital_Elevation_Model_Networks_DEMON_A_model_of_flow_over_hillslopes_for_computation_of_contributing_and_dispersal_areas/links/0912f50b3c13976e7d000000.pdf"),
		SG_T("ResearchGate")
	);

	Add_Reference("Fairfield, J. & Leymarie, P.", "1991",
		"Drainage networks from grid digital elevation models",
		"Water Resources Research, 27:709-717."
	);

	Add_Reference("Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22."
	);

	Add_Reference("Lea, N.L.", "1992",
		"An aspect driven kinematic routing algorithm",
		"In: Parsons, A.J. & Abrahams, A.D. [Eds.], 'Overland Flow: hydraulics and erosion mechanics', London, 147-175."
	);

	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344."
	);

	Add_Reference("Quinn, P.F., Beven, K.J., Chevallier, P. & Planchon, O.", "1991",
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models",
		"Hydrological Processes, 5:59-79.",
		SG_T("https://www.researchgate.net/profile/Olivier_Planchon/publication/32978462_The_Prediction_of_Hillslope_Flow_Paths_for_Distributed_Hydrological_Modeling_Using_Digital_Terrain_Model/links/0912f5130c356c86e6000000.pdf"),
		SG_T("ResearchGate")
	);

	Add_Reference("Tarboton, D.G.", "1997",
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",
		"Water Resources Research, Vol.33, No.2, p.309-319.",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1029/96WR03137/pdf"),
		SG_T("Wiley")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SINKROUTE"	, _TL("Sink Routes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"AREA"		, _TL("Downslope Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Braunschweiger Reliefmodell"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Multiple Triangular Flow Directon"),
			_TL("Multiple Maximum Downslope Gradient Based Flow Directon"),
			_TL("Kinematic Routing Algorithm"),
			_TL("DEMON")
		), 4
	);

	Parameters.Add_Double("",
		"CONVERG"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction algorithm"),
		1.1, 0.001, true
	);

	//-----------------------------------------------------
	pFlow	= NULL;
}

//---------------------------------------------------------
CFlow_AreaDownslope::~CFlow_AreaDownslope(void)
{
	On_Execute_Finish();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaDownslope::On_Execute(void)
{
	CSG_Parameters	*pParameters;

	if( On_Execute_Finish() )
	{
		switch( Parameters("METHOD")->asInt() )
		{
		// Parallel...
		case 0:	// Deterministic 8...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(0.0);
			break;

		case 1:	// Rho 8...
			pFlow	= new CFlow_RecursiveDown;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(1);
			break;

		case 2:	// BRM...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(2);
			break;

		case 3:	// Deterministic Infinity...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(3);
			break;

		case 4:	// MFD...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(4);
			break;

		case 5:	// Triangular MFD...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(5);
			break;

		case 6:	// Multiple Maximum Downslope Gradient Based Flow Directon...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(6);
			break;

		// Downward Recursive...
		case 7:	// KRA...
			pFlow	= new CFlow_RecursiveDown;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(1);
			break;

		case 8:	// DEMON...
			pFlow	= new CFlow_RecursiveDown;
			pFlow->Get_Parameters()->Get_Parameter("METHOD")->Set_Value(2);
			//pFlow->Parameters(DEMON_minDQV)-Set_Value(0)	 = 0;
			break;
		}

		//-------------------------------------------------
		if( pFlow )
		{
			pParameters	= pFlow->Get_Parameters();

			pFlow->Set_System(Parameters("ELEVATION")->asGrid()->Get_System());

			pParameters->Get_Parameter("ELEVATION")->Set_Value(Parameters("ELEVATION")->asGrid());
			pParameters->Get_Parameter("SINKROUTE")->Set_Value(Parameters("SINKROUTE")->asGrid());
			pParameters->Get_Parameter("FLOW"     )->Set_Value(Parameters("AREA"     )->asGrid());
		}
	}

	return( pFlow != NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaDownslope::On_Execute_Finish(void)
{
	if( pFlow )
	{
		delete( pFlow );

		pFlow	= NULL;
	}

	return( pFlow == NULL );
}

//---------------------------------------------------------
bool CFlow_AreaDownslope::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( pFlow && Mode == TOOL_INTERACTIVE_LDOWN )
	{
		pFlow->Set_Point(Get_xGrid(), Get_yGrid());

		pFlow->Execute();

		DataObject_Update(Parameters("AREA")->asGrid(), 0.0, 100.0, true);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
