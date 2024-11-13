
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
		SG_T("https://doi.org/10.1029/93WR03512"), SG_T("doi:10.1029/93WR03512")
	);

	Add_Reference("Fairfield, J. & Leymarie, P.", "1991",
		"Drainage networks from grid digital elevation models",
		"Water Resources Research, 27:709-717.",
		SG_T("https://doi.org/10.1029/90WR02658"), SG_T("doi:10.1029/90WR02658")
	);

	Add_Reference("Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22.",
		SG_T("https://doi.org/10.1016/0098-3004(91)90048-I"), SG_T("doi:10.1016/0098-3004(91)90048-I")
	);

	Add_Reference("Lea, N.L.", "1992",
		"An aspect driven kinematic routing algorithm",
		"In: Parsons, A.J. & Abrahams, A.D. [Eds.], 'Overland Flow: hydraulics and erosion mechanics', London, 147-175.",
		SG_T("https://doi.org/10.1201/b12648"), SG_T("doi:10.1201/b12648")
	);

	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344.",
		SG_T("https://doi.org/10.1016/S0734-189X(84)80011-0"), SG_T("doi:10.1016/S0734-189X(84)80011-0")
	);

	Add_Reference("Quinn, P.F., Beven, K.J., Chevallier, P. & Planchon, O.", "1991",
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models",
		"Hydrological Processes, 5:59-79.",
		SG_T("https://doi.org/10.1002/hyp.3360050106"), SG_T("doi:10.1002/hyp.3360050106")
	);

	Add_Reference("Tarboton, D.G.", "1997",
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",
		"Water Resources Research, Vol.33, No.2, p.309-319.",
		SG_T("https://doi.org/10.1029/96WR03137"), SG_T("doi:10.1029/96WR03137")
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
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Braunschweiger Reliefmodell"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Multiple Triangular Flow Direction"),
			_TL("Multiple Maximum Downslope Gradient Based Flow Direction"),
			_TL("Kinematic Routing Algorithm"),
			_TL("DEMON")
		), 4
	);

	Parameters.Add_Double("",
		"CONVERGENCE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction Algorithm (Freeman 1991).\nApplies also to the Multiple Triangular Flow Direction Algorithm."),
		1.1, 0.001, true
	);

	Parameters.Add_Bool("",
		"MFD_CONTOUR"	, _TL("Contour Length"),
		_TL("Include (pseudo) contour length as additional weighting factor in multiple flow direction routing, reduces flow to diagonal neighbour cells by a factor of 0.71 (s. Quinn et al. 1991 for details)."),
		false
	);

	//-----------------------------------------------------
	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);

	m_pTool	= NULL;
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
int CFlow_AreaDownslope::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("CONVERGENCE", pParameter->asInt() == 4 || pParameter->asInt() == 5);
		pParameters->Set_Enabled("MFD_CONTOUR", pParameter->asInt() == 4 || pParameter->asInt() == 5);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaDownslope::On_Execute(void)
{
	On_Execute_Finish();

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	// Deterministic 8...
		m_pTool	= new CFlow_Parallel;
		m_pTool->Set_Parameter("METHOD", 0.);
		break;

	case 1:	// Rho 8...
		m_pTool	= new CFlow_RecursiveDown;
		m_pTool->Set_Parameter("METHOD", 0.);
		break;

	case 2:	// BRM...
		m_pTool	= new CFlow_Parallel;
		m_pTool->Set_Parameter("METHOD", 2);
		break;

	case 3:	// Deterministic Infinity...
		m_pTool	= new CFlow_Parallel;
		m_pTool->Set_Parameter("METHOD", 3);
		break;

	case 4:	// MFD...
		m_pTool	= new CFlow_Parallel;
		m_pTool->Set_Parameter("METHOD", 4);
		break;

	case 5:	// Triangular MFD...
		m_pTool	= new CFlow_Parallel;
		m_pTool->Set_Parameter("METHOD", 5);
		break;

	case 6:	// Multiple Maximum Downslope Gradient Based Flow Direction...
		m_pTool	= new CFlow_Parallel;
		m_pTool->Set_Parameter("METHOD", 6);
		break;

	case 7:	// KRA...
		m_pTool	= new CFlow_RecursiveDown;
		m_pTool->Set_Parameter("METHOD", 1);
		break;

	case 8:	// DEMON...
		m_pTool	= new CFlow_RecursiveDown;
		m_pTool->Set_Parameter("METHOD", 2);
		break;
	}

	//-----------------------------------------------------
	if( m_pTool )
	{
		m_pTool->Set_Manager(NULL);

		m_pTool->Set_System(Parameters("ELEVATION")->asGrid()->Get_System());

		m_Weights.Create(m_pTool->Get_System(), SG_DATATYPE_Byte);

		m_pTool->Set_Parameter("WEIGHTS"    , &m_Weights);
		m_pTool->Set_Parameter("ELEVATION"  , Parameters("ELEVATION"  )->asGrid  ());
		m_pTool->Set_Parameter("SINKROUTE"  , Parameters("SINKROUTE"  )->asGrid  ());
		m_pTool->Set_Parameter("FLOW"       , Parameters("AREA"       )->asGrid  ());
		m_pTool->Set_Parameter("CONVERGENCE", Parameters("CONVERGENCE")->asDouble());
		m_pTool->Set_Parameter("MFD_CONTOUR", Parameters("MFD_CONTOUR")->asBool  ());

		DataObject_Set_Colors(Parameters("AREA")->asGrid(), 11, SG_COLORS_WHITE_BLUE);
		Parameters("AREA")->asGrid()->Set_NoData_Value(0.);
		DataObject_Update    (Parameters("AREA")->asGrid(), SG_UI_DATAOBJECT_SHOW_MAP);
	}

	return( m_pTool != NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaDownslope::On_Execute_Finish(void)
{
	m_Weights.Destroy();

	if( m_pTool )
	{
		delete(m_pTool);

		m_pTool	= NULL;
	}

	return( true );
}

//---------------------------------------------------------
bool CFlow_AreaDownslope::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( m_pTool && Get_System().Get_Extent().Contains(ptWorld) )
	{
		switch( Mode )
		{
		case TOOL_INTERACTIVE_LDOWN:
			m_Weights.Assign(0.);
			m_Weights.Set_Value(Get_xGrid(), Get_yGrid(), 1.);
			break;

		case TOOL_INTERACTIVE_MOVE_LDOWN:
			m_Weights.Set_Value(Get_xGrid(), Get_yGrid(), 1.);
			break;

		case TOOL_INTERACTIVE_LUP:
			SG_UI_ProgressAndMsg_Lock(true);
			m_Weights.Set_Value(Get_xGrid(), Get_yGrid(), 1.);
			m_pTool->Execute();
			DataObject_Update(Parameters("AREA")->asGrid());
			SG_UI_ProgressAndMsg_Lock(false);
			break;
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
