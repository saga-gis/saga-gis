
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
	Set_Name(_TL("Downslope Area"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"This interactive module allows you to specify source cells (with a left mouse click), "
		"for which the downslope area shall be identified. "
		"For the 'Deterministic Infinity' and 'Multiple Flow Direction' algorithms, "
		"which are able to simulate flow divergence, the result will give "
		"the percentage of the source cell's flow that drains through each cell.\n\n"

		"References:\n\n"

		"Deterministic 8\n"
		"- O'Callaghan, J.F. / Mark, D.M. (1984):\n"
		"    'The extraction of drainage networks from digital elevation data',\n"
		"    Computer Vision, Graphics and Image Processing, 28:323-344\n\n"

		"Rho 8:\n"
		"- Fairfield, J. / Leymarie, P. (1991):\n"
		"    'Drainage networks from grid digital elevation models',\n"
		"    Water Resources Research, 27:709-717\n\n"

		"Braunschweiger Reliefmodell:\n"
		"- Bauer, J. / Rohdenburg, H. / Bork, H.-R. (1985):\n"
		"    'Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse',\n"
		"    Landschaftsgenese und Landschaftsoekologie, H.10, Parameteraufbereitung fuer deterministische Gebiets-Wassermodelle,\n"
		"    Grundlagenarbeiten zu Analyse von Agrar-Oekosystemen, (Eds.: Bork, H.-R. / Rohdenburg, H.), p.1-15\n\n"

		"Deterministic Infinity:\n"
		"- Tarboton, D.G. (1997):\n"
		"    'A new method for the determination of flow directions and upslope areas in grid digital elevation models',\n"
		"    Water Ressources Research, Vol.33, No.2, p.309-319\n\n"

		"Multiple Flow Direction:\n"
		"- Freeman, G.T. (1991):\n"
		"    'Calculating catchment area with divergent flow based on a regular grid',\n"
		"    Computers and Geosciences, 17:413-22\n\n"

		"- Quinn, P.F. / Beven, K.J. / Chevallier, P. / Planchon, O. (1991):\n"
		"    'The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models',\n"
		"    Hydrological Processes, 5:59-79\n\n"

		"Kinematic Routing Algorithm:\n"
		"- Lea, N.L. (1992):\n"
		"    'An aspect driven kinematic routing algorithm',\n"
		"    in: Parsons, A.J., Abrahams, A.D. (Eds.), 'Overland Flow: hydraulics and erosion mechanics', London, 147-175\n\n"

		"DEMON:\n"
		"- Costa-Cabral, M. / Burges, S.J. (1994):\n"
		"    'Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas',\n"
		"    Water Resources Research, 30:1681-1692\n\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Sink Routes"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "AREA"		, _TL("Downslope Area"),
		"",
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// Method...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|",
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Braunschweiger Reliefmodell"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Kinematic Routing Algorithm"),
			_TL("DEMON")
		), 4
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "CONVERG"		, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction algorithm"),
		PARAMETER_TYPE_Double, 1.1, 0.001, true
	);


	//-----------------------------------------------------
	// Initialisations...

	pFlow	= NULL;
}

//---------------------------------------------------------
CFlow_AreaDownslope::~CFlow_AreaDownslope(void)
{
	On_Execute_Finish();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(0.0);
			break;

		case 1:	// Rho 8...
			pFlow	= new CFlow_RecursiveDown;
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(0.0);
			break;

		case 2:	// BRM...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(2);
			break;

		case 3:	// Deterministic Infinity...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(3);
			break;

		case 4:	// MFD...
			pFlow	= new CFlow_Parallel;
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(4);
			break;

		// Downward Recursive...
		case 5:	// KRA...
			pFlow	= new CFlow_RecursiveDown;
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(1);
			break;

		case 6:	// DEMON...
			pFlow	= new CFlow_RecursiveDown;
			pFlow->Get_Parameters()->Get_Parameter("Method")->Set_Value(2);
			//pFlow->Parameters(DEMON_minDQV)-Set_Value(0)	 = 0;
			break;
		}

		//-------------------------------------------------
		if( pFlow )
		{
			pParameters	= pFlow->Get_Parameters();

			pFlow->Get_System()->Assign(Parameters("ELEVATION")->asGrid()->Get_System());

			pParameters->Get_Parameter("ELEVATION")	->Set_Value(Parameters("ELEVATION")	->asGrid());
			pParameters->Get_Parameter("SINKROUTE")	->Set_Value(Parameters("SINKROUTE")	->asGrid());
			pParameters->Get_Parameter("CAREA")		->Set_Value(Parameters("AREA")		->asGrid());
		}
	}

	return( pFlow != NULL );
}

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
bool CFlow_AreaDownslope::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	if( pFlow && Mode == MODULE_INTERACTIVE_LDOWN )
	{
		pFlow->Set_Point(Get_xGrid(), Get_yGrid());

		pFlow->Execute();

		DataObject_Update(Parameters("AREA")->asGrid(), 0.0, 100.0, true);

		return( true );
	}

	return( false );
}
