///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   sim_geomorphology                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  GPP_Model_BASE.cpp                   //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@alps-gmbh.com                 //
//                wichmann@laserdata.at                  //
//                                                       //
//    contact:    alpS GmbH                              //
//                Grabenweg 68                           //
//                A-6020 Innsbruck                       //
//                www.alps-gmbh.com                      //
//                                                       //
//                LASERDATA GmbH                         //
//                Technikerstr. 21a                      //
//                A-6020 Innsbruck                       //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GPP_Model_BASE.h"


//---------------------------------------------------------
CGPP_Model_Particle::CGPP_Model_Particle(int iReleaseID, GRID_CELL gPosition, double dMaterial, double dTanFrictionAngle, double dFrictionMu, double dFrictionMassToDrag, double dVelocity)
{
	m_iReleaseID			= iReleaseID;
	m_gPosition				= gPosition;
	m_gPosition_start		= gPosition;
	m_dPathLength			= 0.0;
	m_dMaterialRelease		= dMaterial;
	m_dMaterial				= dMaterial;
	m_dMaterialFlux			= 0.0;
	m_dTanFrictionAngle		= dTanFrictionAngle;
	m_dFrictionMu			= dFrictionMu;
	m_dFrictionMassToDrag	= dFrictionMassToDrag;
	m_bImpacted				= false;
	m_dSlope				= 0.0;
	m_dVelocity				= dVelocity;

	m_vPath.clear();
	m_sCellsInPath.clear();
}


//---------------------------------------------------------
int CGPP_Model_Particle::Get_ReleaseID(void)
{
	return( m_iReleaseID );
}

//---------------------------------------------------------
GRID_CELL CGPP_Model_Particle::Get_Position(void)
{
	return( m_gPosition );
}

//---------------------------------------------------------
GRID_CELL CGPP_Model_Particle::Get_Previous_Position(void)
{
	return( m_vPath.at(m_vPath.size() - 1).position );
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_X(void)
{
	return( m_gPosition.x );
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_Previous_X(void)
{
	return( m_vPath.at(m_vPath.size() - 1).position.x );
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_Y(void)
{
	return( m_gPosition.y );
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_Previous_Y(void)
{
	return( m_vPath.at(m_vPath.size() - 1).position.y );
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Z(void)
{
	return( m_gPosition.z );
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Previous_Z(void)
{
	return( m_vPath.at(m_vPath.size() - 1).position.z );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Position(GRID_CELL gPosition)
{
	m_gPosition = gPosition;
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Previous_Position(GRID_CELL gPosition, double dSlope, double dLength, int iExitDir, double dMaterialFlux)
{
	PATH_CELL	cell;

	cell.position		= gPosition;
	cell.slope			= dSlope;
	cell.length			= dLength;
	cell.exitDir		= iExitDir;
	cell.deposit		= 0.0;
	cell.materialflux	= dMaterialFlux;

	m_vPath.push_back(cell);
}

//---------------------------------------------------------
GRID_CELL CGPP_Model_Particle::Get_Position_Start(void) const
{
	return( m_gPosition_start );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Position_Start(GRID_CELL gPosition)
{
	m_gPosition_start = gPosition;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_PathLength(void)
{
	return( m_dPathLength );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_PathLength(double dLength)
{
	m_dPathLength = dLength;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Length(void)
{
	return( m_vPath.at(m_vPath.size() - 1).length );
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Slope(void)
{
	return( m_dSlope );
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Previous_Slope(void)
{
	return( m_vPath.at(m_vPath.size() - 1).slope );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Slope(double dSlope)
{
	m_dSlope = dSlope;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Material(void)
{
	return( m_dMaterial );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Material(double dMaterial)
{
	m_dMaterial = dMaterial;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Material_Release(void)
{
	return( m_dMaterialRelease );
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Material_Flux(void)
{
	return( m_dMaterialFlux );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Material_Flux(double dMaterialFlux)
{
	m_dMaterialFlux = dMaterialFlux;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_Speed(void)
{
	return( m_dVelocity );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Speed(double dVelocity)
{
	m_dVelocity = dVelocity;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_TanFrictionAngle(void)
{
	return( m_dTanFrictionAngle );
}

//---------------------------------------------------------
bool CGPP_Model_Particle::Has_Impacted(void)
{
	return( m_bImpacted );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_Impacted(void)
{
	m_bImpacted = true;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_FrictionMu(void)
{
	return( m_dFrictionMu );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_FrictionMu(double dFrictionMu)
{
	m_dFrictionMu = dFrictionMu;
}

//---------------------------------------------------------
double CGPP_Model_Particle::Get_FrictionMassToDrag(void)
{
	return( m_dFrictionMassToDrag );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Set_FrictionMassToDrag(double dFrictionMassToDrag)
{
	m_dFrictionMassToDrag = dFrictionMassToDrag;
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_Previous_Exit_Direction(void)
{
	if( m_vPath.size() == 0 )
	{
		return( -1 );
	}
	else
	{
		return( m_vPath.at(m_vPath.size() - 1).exitDir );
	}
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_Entry_Direction(void)
{
	if( m_vPath.size() == 0 )
	{
		return( -1 );
	}
	
	int iEntryDir = m_vPath.at(m_vPath.size() - 1).exitDir + 4;

	if( iEntryDir >= 8 )
	{
		iEntryDir -= 8;
	}

	return( iEntryDir );
}

//---------------------------------------------------------
int CGPP_Model_Particle::Get_Exit_Direction(size_t iPosition)
{
	if( m_vPath.size() == 0 || iPosition >= m_vPath.size() )
	{
		return( -1 );
	}
	
	return( m_vPath.at(iPosition).exitDir );
}

//---------------------------------------------------------
size_t CGPP_Model_Particle::Get_Count_Path_Positions(void)
{
	return( m_vPath.size() );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Add_Cell_To_Path(CSG_Grid *pGrid, int x, int y)
{
	m_sCellsInPath.insert(_Get_Cell_Number_Grid(pGrid, x, y));

	return;
}

//---------------------------------------------------------
bool CGPP_Model_Particle::Is_Cell_In_Path(CSG_Grid *pGrid, int x, int y)
{
	std::set<sLong>::iterator	it_set = m_sCellsInPath.find(_Get_Cell_Number_Grid(pGrid, x, y));

	if( it_set != m_sCellsInPath.end() )
		return( true );
	else
		return( false );
}

//---------------------------------------------------------
sLong CGPP_Model_Particle::_Get_Cell_Number_Grid(CSG_Grid *pGrid, int x, int y)
{
	return( y * pGrid->Get_NX() + x );
}

//---------------------------------------------------------
void CGPP_Model_Particle::Deposit_Material(CSG_Grid *pGrid, double dSlope)
{
	double	dBaseLevel	= pGrid->asDouble(Get_X(), Get_Y());
	double	dAlpha		= 0.0;
	double	dPathLength	= 0.0;
	int		iCells		= 0;
	double	dPercent	= 1.0;
	double	dReduction	= 0.05;		// we use 5% steps to reduce the slope angle in case there is not enough material available
	double	dMaterial	= 0.0;
	double	dFill		= 0.0;
	bool	bRestart	= false;
	sLong	trials		= 0;


	while( true )
	{
		dAlpha		= dSlope * dPercent;
		dPathLength	= 0.0;
		dMaterial	= Get_Material();
		iCells		= 0;
		bRestart	= false;
		trials++;

		if( (sLong)this->Get_Count_Path_Positions() > pGrid->Get_NCells() )
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: particle of release area %d terminated in sink filling procedure in order to prevent endless loop!"), this->Get_ReleaseID()), true);
			break;
		}

		for(std::vector<PATH_CELL>::reverse_iterator rit=m_vPath.rbegin()+1; rit!=m_vPath.rend(); ++rit)	// rbegin()+1 because we start with the first upslope cell from the sink
		{
			dPathLength	+= (*rit).length;
			dFill		= dBaseLevel - (*rit).position.z + (dAlpha * dPathLength);

			if( dFill <= 0 )			// next upslope cell is higher than slope preserving fill, no further filling required
				break;

			if( dFill > dMaterial )		// not enough material available, we need to reduce the slope angle and re-start
			{
				bRestart = true;
				break;
			}

			(*rit).deposit	= dFill;	// remember fill of this trial
			dMaterial		-= dFill;
			iCells++;
		}

		if( bRestart || iCells == (int)(m_vPath.size() - 1) )	// also check that we don't have reached the start cell
		{
			dPercent -= dReduction;
			continue;
		}
		else													// actually fill the path slope preserving
		{
			std::vector<PATH_CELL>::reverse_iterator rit=m_vPath.rbegin()+1;

			dMaterial = 0.0;

			for(int i=0; i<iCells; i++)
			{
				pGrid->Add_Value((*rit).position.x, (*rit).position.y, (*rit).deposit);
				dMaterial += (*rit).deposit;
				rit++;
			}

			Set_Material(Get_Material() - dMaterial);

			break;
		}
	}

	return;
}

//---------------------------------------------------------
void CGPP_Model_Particle::Evaluate_Damage_Potential(CSG_Grid *pObjectClasses, CSG_Grid *pHazardPaths, CSG_Grid *pHazardSources, CSG_Grid *pHazardSourceMatl)
{
    int		iObjectClasses   = 0;
	double	dMaxMaterialFlux = 0.0;

    for(std::vector<PATH_CELL>::reverse_iterator rit=m_vPath.rbegin(); rit!=m_vPath.rend(); ++rit)
    {
        int x = (*rit).position.x;
        int y = (*rit).position.y;

        if( !pObjectClasses->is_NoData(x, y) )
        {
            int iClass = pObjectClasses->asInt(x, y);

            iObjectClasses |= iClass;

			if( pHazardSourceMatl != NULL && iObjectClasses > 0)
			{
				if( dMaxMaterialFlux < (*rit).materialflux )
				{
					dMaxMaterialFlux = (*rit).materialflux;
				}
			}
        }

        if( pHazardPaths != NULL && iObjectClasses > 0 )
        {
            if( pHazardPaths->is_NoData(x, y) )
            {
                pHazardPaths->Set_Value(x, y, iObjectClasses);
            }
            else
            {
                pHazardPaths->Set_Value(x, y, pHazardPaths->asInt(x, y) | iObjectClasses);
            }
        }
    }

	if( pHazardSources != NULL && iObjectClasses > 0 )
	{
		GRID_CELL s = Get_Position_Start();

		if( pHazardSources->is_NoData(s.x, s.y) )
        {
            pHazardSources->Set_Value(s.x, s.y, iObjectClasses);
        }
        else
        {
            pHazardSources->Set_Value(s.x, s.y, pHazardSources->asInt(s.x, s.y) | iObjectClasses);
        }
	}

	if( pHazardSourceMatl != NULL && dMaxMaterialFlux > 0.0 )
	{
		GRID_CELL s = Get_Position_Start();

		if( pHazardSourceMatl->is_NoData(s.x, s.y) )
        {
            pHazardSourceMatl->Set_Value(s.x, s.y, dMaxMaterialFlux / pHazardSourceMatl->Get_Cellsize());
        }
        else
        {
            pHazardSourceMatl->Add_Value(s.x, s.y, dMaxMaterialFlux / pHazardSourceMatl->Get_Cellsize());
        }
	}

    return;
}


//---------------------------------------------------------
void CGPP_Model_BASE::Add_Dataset_Parameters(CSG_Parameters *pParameters)
{
	pParameters->Add_Grid(
		NULL, "DEM", _TL("DEM"), 
		_TL("Digital elevation model [m]."), 
		PARAMETER_INPUT
	);
	
	pParameters->Add_Grid(	
		NULL, "RELEASE_AREAS", _TL("Release Areas"), 
		_TL("Release areas encoded by unique integer IDs, all other cells NoData [-]."), 
		PARAMETER_INPUT
	);

	pParameters->Add_Grid(
		NULL, "MATERIAL", _TL("Material"), 
		_TL("Height of material available in each start cell [m]."), 
		PARAMETER_INPUT_OPTIONAL
	);

	pParameters->Add_Grid(	
		NULL, "FRICTION_ANGLE_GRID", _TL("Friction Angle"), 
		_TL("Spatially distributed friction angles [degree]. Optionally used with the Geometric Gradient, Fahrboeschung's angle or Shadow Angle friction model."), 
		PARAMETER_INPUT_OPTIONAL
	);

	pParameters->Add_Grid(	
		NULL, "SLOPE_IMPACT_GRID", _TL("Slope Impact Areas"), 
		_TL("Slope impact grid, impact areas encoded with valid values, all other NoData. Optionally used with the Shadow Angle or the 1-parameter friction model."), 
		PARAMETER_INPUT_OPTIONAL
	);

	pParameters->Add_Grid(
		NULL, "FRICTION_MU_GRID", _TL("Friction Parameter Mu"),
		_TL("Spatially distributed friction parameter mu [-], optionally used with the 1-parameter friction model or the PCM Model."),
		PARAMETER_INPUT_OPTIONAL
	);
	
	pParameters->Add_Grid(
		NULL, "FRICTION_MASS_TO_DRAG_GRID", _TL("Mass to Drag Ratio"),
		_TL("Spatially distributed mass to drag ratio [m], optionally used with the PCM Model."),
		PARAMETER_INPUT_OPTIONAL
	);

    pParameters->Add_Grid(
        NULL, "OBJECTS", _TL("Objects"),
        _TL("Potentially endangered objects (like infrastructure) to monitor, using one-hot categorical data encoding for each object class [1, 10, 100, 1000, ...]."),
        PARAMETER_INPUT_OPTIONAL
    );

	pParameters->Add_Grid(	
		NULL, "PROCESS_AREA", _TL("Process Area"), 
		_TL("Delineated process area with encoded transition frequencies [count]."), 
		PARAMETER_OUTPUT
	);

	pParameters->Add_Grid(	
		NULL, "DEPOSITION", _TL("Deposition"), 
		_TL("Height of the material deposited in each cell [m]. Optional output in case a grid with material amounts is provided as input."), 
		PARAMETER_OUTPUT_OPTIONAL
	);

	pParameters->Add_Grid(	
		NULL, "MAX_VELOCITY", _TL("Maximum Velocity"), 
		_TL("Maximum velocity observed in each cell [m/s]. Optional output of the 1-parameter friction model and the PCM Model."), 
		PARAMETER_OUTPUT_OPTIONAL
	);

	pParameters->Add_Grid(	
		NULL, "STOP_POSITIONS", _TL("Stopping Positions"), 
		_TL("Stopping positions, showing cells in which the run-out length has been reached [count]."), 
		PARAMETER_OUTPUT_OPTIONAL
	);

    pParameters->Add_Grid(	
        NULL, "HAZARD_PATHS", _TL("Hazard Paths"), 
        _TL("Process path cells from which objects were hit. Cell values indicate which object classes were hit [combination of object classes]. Optional output in case a grid with potentially endangered objects is provided as input."), 
        PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Long
    );

	pParameters->Add_Grid(	
        NULL, "HAZARD_SOURCES", _TL("Hazard Sources"), 
        _TL("Source (release area) cells from which objects were hit. Cell values indicate which object classes were hit [combination of object classes]. Optional output in case a grid with potentially endangered objects is provided as input."), 
        PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Long
    );

	pParameters->Add_Grid(	
        NULL, "HAZARD_SOURCES_MATERIAL", _TL("Hazard Sources Material"), 
        _TL("Source (release area) cells from which objects were hit. Cell values indicate the material amount [m/cell] that has hit objects from that source cell. Optional output in case grids with material amounts and potentially endangered objects are provided as input."), 
        PARAMETER_OUTPUT_OPTIONAL
    );

	pParameters->Add_Grid(	
		NULL, "MATERIAL_FLUX", _TL("Material Flux"), 
		_TL("Amount of material that has passed through each cell [m]. Optional output in case a grid with material amounts is provided as input."), 
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
void CGPP_Model_BASE::Add_Process_Path_Parameters(CSG_Parameters *pParameters)
{
	CSG_Parameter *pNode = pParameters->Add_Node(NULL, "PROCESS_PATH_NODE", _TL("Process Path"), _TL(""));

	CSG_String	sChoices = SG_T("");

	for (int i=0; i<GPP_PATH_KEY_COUNT; i++)
	{
		sChoices += CSG_String::Format(SG_T("%s|"), gGPP_Model_Path_Key_Name[i].c_str());
	}

	pParameters->Add_Choice( 
		pNode, "PROCESS_PATH_MODEL", _TL("Model"), _TL("Choose a process path model."),
		sChoices,
		1
	);

	pParameters->Add_Value( 
		pNode, "RW_SLOPE_THRES", _TL("Slope Threshold"), 
		_TL("In case the local slope is greater as this threshold [degree], no lateral spreading is modeled."), 
		PARAMETER_TYPE_Double, 
		40.0, 0.001, true, 90.0, true
	);
	pParameters->Add_Value( 
		pNode, "RW_EXPONENT", _TL("Exponent"), 
		_TL("The exponent [-] is controlling the amount of lateral spreading in case the local slope is in between zero and the slope threshold."), 
		PARAMETER_TYPE_Double, 
		2.0, 1.0, true
	);
    pParameters->Add_Value(
		pNode, "RW_PERSISTENCE", _TL("Persistence Factor"),
		_TL("Factor [-] used as weight for the current flow direction. A higher factor reduces abrupt changes in flow direction."),
		PARAMETER_TYPE_Double,
		1.5, 1.0, true
	);

	pParameters->Add_Value( 
		pNode, "GPP_ITERATIONS", _TL("Iterations"), 
		_TL("The number of model runs from each start cell [-]."), 
		PARAMETER_TYPE_Int, 
		1000, 1, true
	);

	sChoices = SG_T("");

	for (int i=0; i<GPP_PROCESSING_ORDER_KEY_Count; i++)
	{
		sChoices += CSG_String::Format(SG_T("%s|"), gGPP_Model_Processing_Order_Key_Name[i].c_str());
	}

	pParameters->Add_Choice( 
		pNode, "GPP_PROCESSING_ORDER", _TL("Processing Order"), _TL("Choose the processing order."),
		sChoices,
		2
	);

	pParameters->Add_Value( 
		pNode, "GPP_SEED", _TL("Seed Value"), 
		_TW("The seed value used to initialize the pseudo-random number generator. A value of 1 will "
		"initialize the generator with the current time, higher numbers will always produce the same "
		"succession of values for each seed value [-]."), 
		PARAMETER_TYPE_Int, 
		1, 1, true
	);
}

//---------------------------------------------------------
void CGPP_Model_BASE::Add_Runout_Parameters(CSG_Parameters *pParameters)
{
	CSG_Parameter *pNode = pParameters->Add_Node(NULL, "RUNOUT_NODE", _TL("Run-out"), _TL(""));

	CSG_String	sChoices = SG_T("");

	for (int i=0; i<GPP_FRICTION_KEY_COUNT; i++)
	{
		sChoices += CSG_String::Format(SG_T("%s|"), gGPP_Model_Friction_Key_Name[i].c_str());
	}

	pParameters->Add_Choice( 
		pNode, "FRICTION_MODEL", _TL("Model"), _TL("Choose a friction model."),
		sChoices,
		0 
	);

	pParameters->Add_Value( 
		pNode, "FRICTION_THRES_FREE_FALL", _TL("Threshold Angle Free Fall"), 
		_TL("The minimum slope angle [degree] between start cell and current cell for modeling free fall with the Shadow Angle or the 1-parameter friction model."),
		PARAMETER_TYPE_Double, 
		60.0, 0.0, true
	);

	sChoices = SG_T("");

	for (int i=0; i<GPP_IMPACT_REDUCTION_KEY_COUNT; i++)
	{
		sChoices += CSG_String::Format(SG_T("%s|"), gGPP_Model_Impact_Reduction_Key_Name[i].c_str());
	}

	pParameters->Add_Choice( 
		pNode, "FRICTION_METHOD_IMPACT", _TL("Method Impact"), _TL("Choose the velocity calculation on slope impact with the 1-parameter friction model."),
		sChoices,
		0
	);
	
	pParameters->Add_Value( 
		pNode, "FRICTION_IMPACT_REDUCTION", _TL("Reduction"), 
		_TL("The energy reduction [%] on slope impact with the 1-parameter friction model."), 
		PARAMETER_TYPE_Double, 
		75.0, 0.0, true, 100.0, true
	);

	pParameters->Add_Value( 
		pNode, "FRICTION_ANGLE", _TL("Friction Angle"), 
		_TL("Friction angle [degree] used as Geometric Gradient, Fahrboeschung's angle or Shadow Angle."), 
		PARAMETER_TYPE_Double, 
		30.0, 0.0, true, 90.0, true
	);

	pParameters->Add_Value(
		pNode, "FRICTION_MU", _TL("Mu"),
		_TL("The (constant) friction parameter mu [-] used with the 1-parameter friction model or the PCM Model."),
		PARAMETER_TYPE_Double,
		0.25, 0.0, true
	);

	sChoices = SG_T("");

	for (int i=0; i<GPP_MODE_MOTION_KEY_COUNT; i++)
	{
		sChoices += CSG_String::Format(SG_T("%s|"), gGPP_Model_Mode_Motion_Key_Name[i].c_str());
	}

	pParameters->Add_Choice( 
		pNode, "FRICTION_MODE_OF_MOTION", _TL("Mode of Motion"), _TL("Choose the mode of motion on hillslope with the 1-parameter friction model."),
		sChoices,
		0
	);

	pParameters->Add_Value(
		pNode, "FRICTION_MASS_TO_DRAG", _TL("Mass to Drag Ratio"),
		_TL("The (constant) mass to drag ratio [m] used with the PCM Model."),
		PARAMETER_TYPE_Double,
		200, 0.0, true
	);

	pParameters->Add_Value(
		pNode, "FRICTION_INIT_VELOCITY", _TL("Initial Velocity"),
		_TL("The initial velocity [m/s] used with the PCM Model."),
		PARAMETER_TYPE_Double,
		1.0, 0.0, true
	);
}

//---------------------------------------------------------
void CGPP_Model_BASE::Add_Deposition_Parameters(CSG_Parameters *pParameters)
{
	CSG_Parameter *pNode = pParameters->Add_Node(NULL, "DEPOSITION_NODE", _TL("Deposition"), _TL(""));

	CSG_String	sChoices = SG_T("");

	for (int i=0; i<GPP_DEPOSITION_KEY_COUNT; i++)
	{
		sChoices += CSG_String::Format(SG_T("%s|"), gGPP_Model_Deposition_Key_Name[i].c_str());
	}

	pParameters->Add_Choice( 
		pNode, "DEPOSITION_MODEL", _TL("Model"), _TL("Choose a deposition model."),
		sChoices,
		0
	);
	
	pParameters->Add_Value(
		pNode, "DEPOSITION_INITIAL", _TL("Initial Deposition on Stop"),
		_TL("The percentage of available material (per run) initially deposited at the stopping position [%]."),
		PARAMETER_TYPE_Double,
		20.0, 0.0, true, 100.0, true
	);

	pParameters->Add_Value(
		pNode, "DEPOSITION_SLOPE_THRES", _TL("Slope Threshold"),
		_TL("The slope angle below which the deposition of material is starting [degree]."),
		PARAMETER_TYPE_Double,
		20.0, 0.0, true, 90.0, true
	);
	
	pParameters->Add_Value(
		pNode, "DEPOSITION_VELOCITY_THRES", _TL("Velocity Threshold"),
		_TL("The velocity below which the deposition of material is starting [m/s]."),
		PARAMETER_TYPE_Double,
		15.0, 0.0, true
	);

	pParameters->Add_Value(
		pNode, "DEPOSITION_MAX", _TL("Maximum Deposition along Path"),
		_TL("The percentage of available material (per run) which is deposited at most (slope or velocity equal zero) [%]."),
		PARAMETER_TYPE_Double,
		20.0, 0.0, true, 100.0, true
	);

	pParameters->Add_Value(
		pNode, "DEPOSITION_MIN_PATH", _TL("Minimum Path Length"),
		_TL("The minimum path length which has to be reached before material deposition is enabled [m]."),
		PARAMETER_TYPE_Double,
		100.0, 0.0, true
	);
}

//---------------------------------------------------------
void CGPP_Model_BASE::Add_Sink_Parameters(CSG_Parameters *pParameters)
{
	CSG_Parameter *pNode = pParameters->Add_Node(NULL, "SINK_NODE", _TL("Sink Filling"), _TL(""));
	
	pParameters->Add_Value(
		pNode, "SINK_MIN_SLOPE", _TL("Minimum Slope"),
		_TL("The minimum slope to preserve on sink filling [degree]."),
		PARAMETER_TYPE_Double,
		2.5, 0.0, true, 90.0, true
	);
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Initialize_Parameters(CSG_Parameters &Parameters)
{
	m_pDEM					= SG_Create_Grid(Parameters("DEM")->asGrid());		m_pDEM->Assign(Parameters("DEM")->asGrid());
	m_pReleaseAreas			= Parameters("RELEASE_AREAS")->asGrid();
	m_pMaterial				= Parameters("MATERIAL")->asGrid();
	m_pFrictionAngles		= Parameters("FRICTION_ANGLE_GRID")->asGrid();
	m_pImpactAreas			= Parameters("SLOPE_IMPACT_GRID")->asGrid();
	m_pFrictionMu			= Parameters("FRICTION_MU_GRID")->asGrid();
	m_pFrictionMassToDrag	= Parameters("FRICTION_MASS_TO_DRAG_GRID")->asGrid();
    m_pObjects				= Parameters("OBJECTS")->asGrid();

	m_GPP_Path_Model		= Parameters("PROCESS_PATH_MODEL")->asInt();
	m_iIterations			= Parameters("GPP_ITERATIONS")->asInt();
	m_iProcessingOrder		= Parameters("GPP_PROCESSING_ORDER")->asInt();
	m_iSeed					= Parameters("GPP_SEED")->asInt();

	m_dRW_SlopeThres		= tan(Parameters("RW_SLOPE_THRES")->asDouble() * M_DEG_TO_RAD);
	m_dRW_Exponent			= Parameters("RW_EXPONENT")->asDouble();
	m_dRW_Persistence		= Parameters("RW_PERSISTENCE")->asDouble();

	m_GPP_Friction_Model	= Parameters("FRICTION_MODEL")->asInt();
	m_dTanFrictionAngle		= tan(Parameters("FRICTION_ANGLE")->asDouble() * M_DEG_TO_RAD);
	m_dTanThresFreeFall		= tan(Parameters("FRICTION_THRES_FREE_FALL")->asDouble() * M_DEG_TO_RAD);
	m_dFrictionMu			= Parameters("FRICTION_MU")->asDouble();
	m_dReductionFactor		= Parameters("FRICTION_IMPACT_REDUCTION")->asDouble() / 100.0;
	m_GPP_Method_Impact		= Parameters("FRICTION_METHOD_IMPACT")->asInt();
	m_GPP_Mode_Motion		= Parameters("FRICTION_MODE_OF_MOTION")->asInt();
	m_dFrictionMassToDrag	= Parameters("FRICTION_MASS_TO_DRAG")->asDouble();
	if( m_GPP_Friction_Model == GPP_FRICTION_PCM_MODEL )
		m_dInitVelocity		= Parameters("FRICTION_INIT_VELOCITY")->asDouble();
	else
		m_dInitVelocity		= 0.0;
	
	m_pProcessArea			= Parameters("PROCESS_AREA")->asGrid();				m_pProcessArea->Assign(0.0);
	m_pDeposition			= Parameters("DEPOSITION")->asGrid();
	m_pMaxVelocity			= Parameters("MAX_VELOCITY")->asGrid();				if( m_pMaxVelocity != NULL )		m_pMaxVelocity->Assign_NoData();
	m_pStopPositions		= Parameters("STOP_POSITIONS")->asGrid();			if( m_pStopPositions != NULL )		m_pStopPositions->Assign(0.0);
    m_pHazardPaths			= Parameters("HAZARD_PATHS")->asGrid();				if( m_pHazardPaths != NULL )		m_pHazardPaths->Assign_NoData();
	m_pHazardSources		= Parameters("HAZARD_SOURCES")->asGrid();			if( m_pHazardSources != NULL )		m_pHazardSources->Assign_NoData();
	m_pHazardSourcesMatl	= Parameters("HAZARD_SOURCES_MATERIAL")->asGrid();	if( m_pHazardSourcesMatl != NULL )	m_pHazardSourcesMatl->Assign_NoData();
	m_pMaterialFlux			= Parameters("MATERIAL_FLUX")->asGrid();			if( m_pMaterialFlux != NULL )		m_pMaterialFlux->Assign(0.0);
	
	m_GPP_Deposition_Model	= Parameters("DEPOSITION_MODEL")->asInt();
	m_PercentInitialDeposit	= Parameters("DEPOSITION_INITIAL")->asDouble() / 100.0;
	m_dDepSlopeThres		= tan(Parameters("DEPOSITION_SLOPE_THRES")->asDouble() * M_DEG_TO_RAD);
	m_dDepVelocityThres		= Parameters("DEPOSITION_VELOCITY_THRES")->asDouble();
	m_PercentMaxDeposit		= Parameters("DEPOSITION_MAX")->asDouble() / 100.0;
	m_MinPathLength			= Parameters("DEPOSITION_MIN_PATH")->asDouble();

	m_SinkMinSlope			= tan(Parameters("SINK_MIN_SLOPE")->asDouble() * M_DEG_TO_RAD);

	//---------------------------------------------------------
	if( (m_GPP_Deposition_Model == GPP_DEPOSITION_VELOCITY_ON_STOP || m_GPP_Deposition_Model == GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP)
		&&
		(m_GPP_Friction_Model != GPP_FRICTION_ROCKFALL_VELOCITY && m_GPP_Friction_Model != GPP_FRICTION_PCM_MODEL) )
	{
		SG_UI_Msg_Add_Error(_TL("Deposition modelling based on velocity requires an appropriate friction model!"));
		return( false );
	}

	//---------------------------------------------------------
	if( m_GPP_Deposition_Model > GPP_DEPOSITION_NONE && m_pMaterial == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("Deposition modelling requires a material grid as input!"));
		return( false );
	}

	//---------------------------------------------------------
    if( m_pMaterialFlux != NULL && m_pMaterial == NULL )
    {
		SG_UI_Msg_Add_Error(_TL("Material flux output requires a material grid as input!"));
		return( false );
    }

	//---------------------------------------------------------
    if( m_pHazardSourcesMatl != NULL && (m_pMaterial == NULL || m_pObjects == NULL) )
    {
		SG_UI_Msg_Add_Error(_TL("Hazard sources material output requires a material and an objects grid as input!"));
		return( false );
    }

    //---------------------------------------------------------
    if( m_pObjects != NULL && (m_pHazardPaths == NULL && m_pHazardSources == NULL && m_pHazardSourcesMatl == NULL ) )
    {
		SG_UI_Msg_Add_Error(_TL("An 'Objects' input grid is provided but none of the 'hazard' grids (paths, sources or material) is selected as output!"));
		return( false );
    }

    //---------------------------------------------------------
	return( true );;
}

//---------------------------------------------------------
void CGPP_Model_BASE::Initialize_Random_Generator(void)
{
	if( m_iSeed == 1 )
	{
		srand( (unsigned)time(NULL) );
	}
	else
	{
		srand( m_iSeed );
	}

	return;
}

//---------------------------------------------------------
void CGPP_Model_BASE::Finalize(CSG_Parameters *pParameters)
{
    //---------------------------------------------------------
	if( m_pDeposition != NULL )
	{
        #pragma omp parallel for
		for (int y=0; y<m_pDEM->Get_NY(); y++)
		{	
			for (int x=0; x<m_pDEM->Get_NX(); x++)
			{
				if( m_pDEM->is_NoData(x, y) )
					m_pDeposition->Set_NoData(x, y);
				else
					m_pDeposition->Set_Value(x, y, m_pDEM->asDouble(x, y) - pParameters->Get_Parameter("DEM")->asGrid()->asDouble(x, y));
			}
		}

		m_pDeposition->Set_NoData_Value(0.0);
	}

    //---------------------------------------------------------
	delete( m_pDEM );

	m_pProcessArea->Set_NoData_Value(0.0);

	if( m_pMaterialFlux != NULL )
	{
		m_pMaterialFlux->Set_NoData_Value(0.0);
	}

	if( m_pStopPositions != NULL )
	{
		m_pStopPositions->Set_NoData_Value(0.0);
	}

    //---------------------------------------------------------
    if( m_pObjects != NULL )
    {
        #pragma omp parallel for
        for(int y=0; y<m_pObjects->Get_NY(); y++)
        {
            for(int x=0; x<m_pObjects->Get_NX(); x++)
            {
                if( m_pHazardPaths != NULL && !m_pHazardPaths->is_NoData(x, y) )
                {
                    m_pHazardPaths->Set_Value(x, y, _Get_ObjectClass_Binary(m_pHazardPaths->asInt(x, y)));
                }

                if( m_pHazardSources != NULL && !m_pHazardSources->is_NoData(x, y) )
                {
                    m_pHazardSources->Set_Value(x, y, _Get_ObjectClass_Binary(m_pHazardSources->asInt(x, y)));
                }
            }
        }

        delete (m_pObjectClasses);
    }

    //---------------------------------------------------------
    return;
}

//---------------------------------------------------------
void CGPP_Model_BASE::Add_Start_Cell(int iID, GRID_CELL startCell)
{
	std::map<int, std::vector<GRID_CELL> >::iterator it;

	it = m_mReleaseAreas.find(iID);
				
	if( it == m_mReleaseAreas.end() )
	{
		std::vector<GRID_CELL>	vCell(1, startCell);
		m_mReleaseAreas.insert(std::pair<int, std::vector<GRID_CELL> >(iID, vCell));
	}
	else
	{
		(*it).second.push_back(startCell);
	}

	return;
}

//---------------------------------------------------------
bool StartCellSortPredicateAscending(const GRID_CELL& c1, const GRID_CELL& c2)
{
	if( c2.z != c1.z )
		return c1.z < c2.z;
    if( c2.x != c1.x)
		return c1.x < c2.x;

	return c1.y < c2.y;
}

//---------------------------------------------------------
bool StartCellSortPredicateDescending(const GRID_CELL& c1, const GRID_CELL& c2)
{
	if( c2.z != c1.z )
		return c1.z > c2.z;
    if( c2.x != c1.x)
		return c1.x > c2.x;

	return c1.y > c2.y;
}

//---------------------------------------------------------
void CGPP_Model_BASE::SortStartCells(std::vector<GRID_CELL> *pStartCells, int iSortMode)
{
	switch( iSortMode )
	{
	default:
	case GPP_SORT_LOWEST_TO_HIGHEST:
		std::sort(pStartCells->begin(), pStartCells->end(), StartCellSortPredicateAscending);
		break;
	case GPP_SORT_HIGHEST_TO_LOWEST:
		std::sort(pStartCells->begin(), pStartCells->end(), StartCellSortPredicateDescending);
		break;
	}

	return;
}

//---------------------------------------------------------
bool ParticleSortPredicateAscending(const class CGPP_Model_Particle& p1, const class CGPP_Model_Particle& p2)
{
	if( p2.Get_Position_Start().z != p1.Get_Position_Start().z )
		return p1.Get_Position_Start().z < p2.Get_Position_Start().z;
    if( p2.Get_Position_Start().x != p1.Get_Position_Start().x)
		return p1.Get_Position_Start().x < p2.Get_Position_Start().x;

	return p1.Get_Position_Start().y < p2.Get_Position_Start().y;
}

//---------------------------------------------------------
bool ParticleSortPredicateDescending(const class CGPP_Model_Particle& p1, const class CGPP_Model_Particle& p2)
{
	if( p2.Get_Position_Start().z != p1.Get_Position_Start().z )
		return p1.Get_Position_Start().z > p2.Get_Position_Start().z;
    if( p2.Get_Position_Start().x != p1.Get_Position_Start().x)
		return p1.Get_Position_Start().x > p2.Get_Position_Start().x;

	return p1.Get_Position_Start().y > p2.Get_Position_Start().y;
}

//---------------------------------------------------------
void CGPP_Model_BASE::SortParticles(std::vector<class CGPP_Model_Particle> *pvProcessingList, int iSortMode)
{
	switch( iSortMode )
	{
	default:
	case GPP_SORT_LOWEST_TO_HIGHEST:
		std::sort(pvProcessingList->begin(), pvProcessingList->end(), ParticleSortPredicateAscending);
		break;
	case GPP_SORT_HIGHEST_TO_LOWEST:
		std::sort(pvProcessingList->begin(), pvProcessingList->end(), ParticleSortPredicateDescending);
		break;
	}

	return;
}

//---------------------------------------------------------
CGPP_Model_Particle CGPP_Model_BASE::Init_Particle(int iReleaseID, GRID_CELL gPosition)
{
	double	dMaterial = 0.0;

	if( m_pMaterial != NULL )
	{
		if( !m_pMaterial->is_NoData(gPosition.x, gPosition.y) )
		{
			dMaterial = m_pMaterial->asDouble(gPosition.x, gPosition.y);	// total amount available for this start cell
		}
	}


	switch (m_GPP_Friction_Model)
	{
	default:
	case GPP_FRICTION_NONE:
	case GPP_FRICTION_ROCKFALL_VELOCITY:
	case GPP_FRICTION_PCM_MODEL:			
		m_dTanFrictionAngle = 0.0;
		if( m_pFrictionMu != NULL )
			m_dFrictionMu = m_pFrictionMu->asDouble(gPosition.x, gPosition.y);
		if( m_pFrictionMassToDrag != NULL )
			m_dFrictionMassToDrag = m_pFrictionMassToDrag->asDouble(gPosition.x, gPosition.y);
		break;

	case GPP_FRICTION_GEOMETRIC_GRADIENT:
	case GPP_FRICTION_FAHRBOESCHUNG:
	case GPP_FRICTION_SHADOW_ANGLE:
		if( m_pFrictionAngles != NULL )
			m_dTanFrictionAngle = tan(m_pFrictionAngles->asDouble(gPosition.x, gPosition.y) * M_DEG_TO_RAD);
		m_dFrictionMu = 0.0;
		m_dFrictionMassToDrag = 0.0;
		break;
	}
				
	CGPP_Model_Particle		particle(iReleaseID, gPosition, dMaterial, m_dTanFrictionAngle, m_dFrictionMu, m_dFrictionMassToDrag, m_dInitVelocity);

	particle.Add_Cell_To_Path(m_pDEM, gPosition.x, gPosition.y);

	return( particle );
}

//---------------------------------------------------------
void CGPP_Model_BASE::Run_GPP_Model(std::vector<class CGPP_Model_Particle> *pvProcessingList)
{
	for (int iIter=0; iIter<m_iIterations && SG_UI_Process_Set_Progress(iIter, m_iIterations); iIter++)
	{
		for (size_t iParticle=0; iParticle<pvProcessingList->size(); iParticle++)
		{
			CGPP_Model_Particle	Particle = pvProcessingList->at(iParticle);

			m_pProcessArea->Add_Value(Particle.Get_X(), Particle.Get_Y(), 1);

			if( m_pMaxVelocity != NULL )
			{
				if( Particle.Get_Speed() > m_pMaxVelocity->asDouble(Particle.Get_X(), Particle.Get_Y()) )
					m_pMaxVelocity->Set_Value(Particle.Get_X(), Particle.Get_Y(), Particle.Get_Speed());
			}

			double	dTotalMaterialLeft	= Particle.Get_Material();
			double	dMaterialRun		= dTotalMaterialLeft / (m_iIterations - iIter);

			Particle.Set_Material(dMaterialRun);

			while( true )
			{
				if( !Update_Path(&Particle, dMaterialRun, &pvProcessingList->at(iParticle)) )
				{
					break;
				}

				bool bContinue = Update_Speed(&Particle, &pvProcessingList->at(iParticle));

				if( bContinue )
				{
					if( m_GPP_Deposition_Model > GPP_DEPOSITION_ON_STOP && Particle.Get_PathLength() > m_MinPathLength )
					{
						Calc_Path_Deposition(&Particle);
					}
				}

				if( (m_pMaterialFlux != NULL || m_pHazardSourcesMatl != NULL) && Particle.Get_Material() > 0.0 )
				{
					double dMaterial = Particle.Get_Material_Release() / m_iIterations;

					if( Particle.Get_Material() < dMaterial )
					{
						dMaterial = Particle.Get_Material();
					}

					Particle.Set_Material_Flux(dMaterial * m_pDEM->Get_Cellsize());

					if( m_pMaterialFlux != NULL )
					{
						m_pMaterialFlux->Add_Value(Particle.Get_X(), Particle.Get_Y(), Particle.Get_Material_Flux());
					}
				}

				if( !bContinue )
				{
					break;
				}

				if( (sLong)Particle.Get_Count_Path_Positions() > m_pDEM->Get_NCells() )
				{
					SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: particle %zu of release area %d terminated in iteration %d in order to prevent endless loop!"), iParticle, Particle.Get_ReleaseID(), iIter), true);

                    if( m_pObjects != NULL )
                    {
                        Particle.Evaluate_Damage_Potential(m_pObjectClasses, m_pHazardPaths, m_pHazardSources, m_pHazardSourcesMatl);
                    }

					break;
				}
			} // while( true )

			if( m_GPP_Deposition_Model > GPP_DEPOSITION_NONE && dMaterialRun > Particle.Get_Material() )
			{
				// update material in start cell (amount available for subsequent runs) 
				pvProcessingList->at(iParticle).Set_Material(dTotalMaterialLeft - (dMaterialRun - Particle.Get_Material()));
			}
		} // for iParticle
	} // for iIteration

	return;
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Path(CGPP_Model_Particle *pParticle, double dMaterialRun, CGPP_Model_Particle *pParticleStartCell)
{
	bool	bResult, bEdge, bSink;

	switch (m_GPP_Path_Model)
	{
	default:
	case GPP_PATH_MAXIMUM_SLOPE:	bResult = Update_Path_Maximum_Slope(pParticle, bEdge, bSink);		break;

	case GPP_PATH_RANDOM_WALK:		bResult = Update_Path_Random_Walk(pParticle, bEdge, bSink);			break;
	}

	if( !bResult )
	{
		if( !bEdge )
		{
			if( m_pStopPositions != NULL )
			{
				m_pStopPositions->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), 1);
			}
		}

		if( bSink )
		{
			Fill_Sink(pParticle);
		}

		if( !bEdge && !bSink )
		{
			if( m_GPP_Deposition_Model > GPP_DEPOSITION_NONE )
			{
				Deposit_Material_On_Stop(pParticle);
			}
		}

        if( m_pObjects != NULL )
        {
            pParticle->Evaluate_Damage_Potential(m_pObjectClasses, m_pHazardPaths, m_pHazardSources, m_pHazardSourcesMatl);
        }
	}

	return( bResult );
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Path_Maximum_Slope(CGPP_Model_Particle *pParticle, bool &bEdge, bool &bSink)
{
	int		iExitDir	= -1;
	double	z			= m_pDEM->asDouble(pParticle->Get_X(), pParticle->Get_Y());
	double	dSlopeMax	= -1.0;
	double	dSlope[8];
	bool	bIsInPath[8];

	bEdge	= false;
	bSink	= false;

	//---------------------------------------------------------
	// get maximum slope

	for(int i=0; i<8; i++)
	{
		int ix	= m_pDEM->Get_System().Get_xTo(i, pParticle->Get_X());
		int iy	= m_pDEM->Get_System().Get_yTo(i, pParticle->Get_Y());

		bIsInPath[i] = pParticle->Is_Cell_In_Path(m_pDEM, ix, iy);

		if( !m_pDEM->is_InGrid(ix, iy, false) )
		{
			bEdge		= true;		// we do not perform any backfilling of sinks in this case -> material flows out of grid bounds
			dSlope[i]	= -1.0;
			continue;
		}
		else if( !m_pDEM->is_NoData(ix, iy) && !bIsInPath[i] )
		{
			dSlope[i]	= (z - m_pDEM->asDouble(ix, iy)) / m_pDEM->Get_System().Get_Length(i);

			if( dSlope[i] >= 0.0 && (iExitDir < 0 || dSlopeMax < dSlope[i]) )
			{
				dSlopeMax	= dSlope[i];
				iExitDir	= i;
			}
		}
		else
		{
			dSlope[i]	= -1.0;
		}
	}

	//---------------------------------------------------------
	if( iExitDir == -1 )
	{
		if( !bEdge )
		{
			bSink = true;
		}

		return( false );
	}


	//---------------------------------------------------------
	// check for valid neighbors and update iDir in case of a single valid NN

	int	iCount = 0;

	for(int i=0; i<8; i++)
	{
		if( dSlope[i] == dSlopeMax )
		{
			iExitDir	= i;
			iCount		+= 1;
		}
	}


	if( iCount > 1 )	// in case of multiple valid neighbors, choose one by random
	{
		double	dProp, dProbCum[8], dProbSum = 0.0, dRandom;

		dProp	= 1.0 / iCount;

		int		iLastIndex = -1;

		for(int i=0; i<8; i++)
		{
			if( dSlope[i] == dSlopeMax && !bIsInPath[i] )
			{
				dProbCum[i]	= dProp + dProbSum;
				dProbSum	= dProbCum[i];

				iLastIndex	= i;
			}
		}

		dProbCum[iLastIndex] = 1.0;		// we require this because upper boundary can be lower than 1 because of floating point precision problems
				
		dRandom = (double)rand() / RAND_MAX;
		
		for(int i=0; i<8; i++)
		{
			if( dSlope[i] == dSlopeMax && !bIsInPath[i] )
			{
				if( dRandom <= dProbCum[i] )
				{
					iExitDir = i;
					break;
				}
			}
		}
	}


	//---------------------------------------------------------
	// update previous position
	pParticle->Set_Previous_Position(pParticle->Get_Position(), pParticle->Get_Slope(), m_pDEM->Get_System().Get_Length(iExitDir), iExitDir, pParticle->Get_Material_Flux());

	// update current position
	GRID_CELL	neighbor;
	neighbor.x	= m_pDEM->Get_System().Get_xTo(iExitDir, pParticle->Get_X());
	neighbor.y	= m_pDEM->Get_System().Get_yTo(iExitDir, pParticle->Get_Y());
	neighbor.z	= m_pDEM->asDouble(neighbor.x, neighbor.y);

	pParticle->Set_Position(neighbor);
	pParticle->Set_Slope((pParticle->Get_Previous_Z() - pParticle->Get_Z()) / m_pDEM->Get_System().Get_Length(iExitDir));
	pParticle->Set_PathLength(pParticle->Get_PathLength() + m_pDEM->Get_System().Get_Length(iExitDir));
	
	pParticle->Add_Cell_To_Path(m_pDEM, pParticle->Get_X(), pParticle->Get_Y());

	m_pProcessArea->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), 1);

	return( true );
}


//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Path_Random_Walk(CGPP_Model_Particle *pParticle, bool &bEdge, bool &bSink)
{
	double	z = m_pDEM->asDouble(pParticle->Get_X(), pParticle->Get_Y());

	double	iz[8], dSlope[8], dThresSlope[8];
	double	dThresSlopeSum = 0.0, dThresSlopeMax = -1.0;
	bool	bIsInPath[8];

	bEdge	= false;
	bSink	= false;

	//---------------------------------------------------------
	for(int i=0; i<8; i++)
	{
		int ix	= m_pDEM->Get_System().Get_xTo(i, pParticle->Get_X());
		int iy	= m_pDEM->Get_System().Get_yTo(i, pParticle->Get_Y());

		bIsInPath[i] = pParticle->Is_Cell_In_Path(m_pDEM, ix, iy);

		if( !m_pDEM->is_InGrid(ix, iy, false) )
		{
			bEdge			= true;		// we do not perform any backfilling of sinks in this case -> material flows out of grid bounds
			dThresSlope[i]	= -1.0;
			continue;
		}
		else if( !m_pDEM->is_NoData(ix, iy) && (iz[i] = m_pDEM->asDouble(ix, iy)) <= z && !bIsInPath[i] )
		{
			dSlope[i]		= (z - iz[i]) / m_pDEM->Get_System().Get_Length(i);
			dThresSlope[i]	= dSlope[i] / m_dRW_SlopeThres;
			dThresSlopeSum	+= dThresSlope[i];

			if( dThresSlope[i] > dThresSlopeMax )
				dThresSlopeMax = dThresSlope[i];
		}
		else
		{
			dThresSlope[i] = -1.0;
		}
	}

	//---------------------------------------------------------
	if( dThresSlopeSum == 0.0 && dThresSlopeMax == -1.0 )		// no neighbors which have an equal or lower elevation
	{
		if( !bEdge )
		{
			bSink = true;
		}

		return( false );
	}


	//---------------------------------------------------------
	double	dProb[8], dProbCum[8];
	double	dProbSum = 0.0;
	int		iExitDir = -1;

	if( dThresSlopeMax > 1 || dThresSlopeSum == 0.0 )	// single flow direction because of slope threshold or only neighbor(s) with equal slope
	{													// check if there are several potential and if so, choose one by random
		int	iNeighbors = 0;

		for(int i=0; i<8; i++)
		{
			if( dThresSlope[i] == dThresSlopeMax && !bIsInPath[i] )
			{
				iExitDir = i;
				iNeighbors++;
			}
		}

		if( iNeighbors == 0 )
		{
			return( false );
		}

		if( iNeighbors > 1 )
		{
			double dProb_pers[8];
			double dPersSum = 0.0;

			for(int i=0; i<8; i++)
			{
				if( dThresSlope[i] == dThresSlopeMax && !bIsInPath[i] )
				{
					if( pParticle->Get_Previous_Exit_Direction() == i )
						dProb_pers[i] = 1.0 * m_dRW_Persistence;
					else
						dProb_pers[i] = 1.0;

					dPersSum += dProb_pers[i];
				}
			}

			int		iLastIndex = -1;

			for(int i=0; i<8; i++)
			{
				if( dThresSlope[i] == dThresSlopeMax && !bIsInPath[i] )
				{
					dProb[i]	= dProb_pers[i] / dPersSum;
					dProbCum[i]	= dProb[i] + dProbSum;
					dProbSum	= dProbCum[i];

					iLastIndex	= i;
				}
			}
			
			dProbCum[iLastIndex] = 1.0;		// we require this because upper boundary can be lower than 1 because of floating point precision problems

			double dRandNum = (double)rand() / RAND_MAX;
				
			for(int i=0; i<8; i++)
			{
				if( dThresSlope[i] == dThresSlopeMax && !bIsInPath[i] )
				{
					if( dRandNum <= dProbCum[i] )
					{
						iExitDir = i;
						break;
					}
				}
			}
		}
	} 
	else	// multiple potential flow directions
	{
		double dCriterion = pow(dThresSlopeMax, m_dRW_Exponent);

		double	dSlope_pers[8];
		double	dPersSum = 0.0;
		int		iNeighbors = 0;

		for(int i=0; i<8; i++)
		{
			if( dThresSlope[i] >= dCriterion && !bIsInPath[i] )
			{
				if( pParticle->Get_Previous_Exit_Direction() == i )
					dSlope_pers[i] = dSlope[i] * m_dRW_Persistence;
				else
					dSlope_pers[i] = dSlope[i];

				dPersSum += dSlope_pers[i];
				iNeighbors++;
			}
		}

		if( iNeighbors == 0 )
		{
			return( false );
		}

		double dRandNum = (double)rand() / RAND_MAX;
		
		int		iLastIndex = -1;

		for(int i=0; i<8; i++)
		{
			if( dThresSlope[i] >= dCriterion && !bIsInPath[i] )
			{
				dProb[i]	= dSlope_pers[i] / dPersSum;
				dProbCum[i]	= dProb[i] + dProbSum;
				dProbSum	= dProbCum[i];

				iLastIndex	= i;
			}
		}			

		dProbCum[iLastIndex] = 1.0;		// we require this because upper boundary can be lower than 1 because of floating point precision problems
			
		for(int i=0; i<8; i++)
		{
			if( dThresSlope[i] >= dCriterion && !bIsInPath[i] )
			{
				if( dRandNum <= dProbCum[i] )
				{
					iExitDir = i;
					break;
				}
			}
		}
	}


	//---------------------------------------------------------
	// update previous position
	pParticle->Set_Previous_Position(pParticle->Get_Position(), pParticle->Get_Slope(), m_pDEM->Get_System().Get_Length(iExitDir), iExitDir, pParticle->Get_Material_Flux());

	// update current position
	GRID_CELL neighbor;
	neighbor.x = m_pDEM->Get_System().Get_xTo(iExitDir, pParticle->Get_X());
	neighbor.y = m_pDEM->Get_System().Get_yTo(iExitDir, pParticle->Get_Y());
	neighbor.z = iz[iExitDir];

	pParticle->Set_Position(neighbor);
	pParticle->Set_Slope(dSlope[iExitDir]);
	pParticle->Set_PathLength(pParticle->Get_PathLength() + m_pDEM->Get_System().Get_Length(iExitDir));

	pParticle->Add_Cell_To_Path(m_pDEM, pParticle->Get_X(), pParticle->Get_Y());

	m_pProcessArea->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), 1);

	return( true );
}


//---------------------------------------------------------
void  CGPP_Model_BASE::Fill_Sink(CGPP_Model_Particle *pParticle)
{
	//---------------------------------------------------------
	// no filling in case the start cell itself is already a sink

	if( pParticle->Get_Count_Path_Positions() == 0 )
	{
		return;
	}


	//---------------------------------------------------------
	int		iOverDir;
	double	dOverflowZ;

	if( !Detect_Dir_to_Overflow_Cell(pParticle, iOverDir, dOverflowZ) )
	{
		return;
	}


	//---------------------------------------------------------
	// add sink cell to path

	pParticle->Set_Previous_Position(pParticle->Get_Position(), 0.0, 0.0, -1, 0.0);
	
	
	//---------------------------------------------------------
	// determine material required to fill sink and preserve slope to overflow

	double dSinkDepth	= dOverflowZ - pParticle->Get_Z();
	double dFill		= (m_SinkMinSlope * m_pDEM->Get_System().Get_Length(iOverDir)) + dSinkDepth;

	if( dFill >= pParticle->Get_Material() )
	{
		m_pDEM->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), pParticle->Get_Material());
		pParticle->Set_Material(0.0);

		return;
	}
	else
	{
		m_pDEM->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), dFill);	// preserves slope to overflow
		pParticle->Set_Material(pParticle->Get_Material() - dFill);
	}


	//---------------------------------------------------------
	// continue to fill sink upwards with minimum slope

	pParticle->Deposit_Material(m_pDEM, m_SinkMinSlope);


	return;
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Detect_Dir_to_Overflow_Cell(CGPP_Model_Particle *pParticle, int &iOverDir, double &dOverflowZ)
{
	// detect direction to overflow cell; exclude the cell from which the particle entered the sink

	iOverDir	= -1;
	dOverflowZ	= std::numeric_limits<double>::max();
	int	iInDir	= pParticle->Get_Entry_Direction();
	int	x		= pParticle->Get_X();
	int	y		= pParticle->Get_Y();

	for(int i=0; i<8; i++)
	{
		if( i == iInDir )
			continue;

		int ix	= m_pDEM->Get_System().Get_xTo(i, x);
		int iy	= m_pDEM->Get_System().Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy, true) )
		{
			if( m_pDEM->asDouble(ix, iy) < dOverflowZ )
			{
				dOverflowZ	= m_pDEM->asDouble(ix, iy);
				iOverDir	= i;
			}
		}
	}

	if( iOverDir < 0 )
		return( false );
	else
		return( true );
}

//---------------------------------------------------------
void CGPP_Model_BASE::Deposit_Material_On_Stop(CGPP_Model_Particle *pParticle)
{
	//---------------------------------------------------------
	// no deposition on the start cell itself or if no material is left over

	if( pParticle->Get_Count_Path_Positions() == 0 || pParticle->Get_Material() <= 0.0 )
	{
		return;
	}


	//---------------------------------------------------------
	// add stop cell to path
	pParticle->Set_Previous_Position(pParticle->Get_Position(), 0.0, 0.0, -1, pParticle->Get_Material_Flux());
	

	//---------------------------------------------------------
	// deposit initial percentage of available material

	double	dDeposit = m_PercentInitialDeposit * pParticle->Get_Material();

	m_pDEM->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), dDeposit);
	pParticle->Set_Material(pParticle->Get_Material() - dDeposit);


	//---------------------------------------------------------
	// continue to deposit material upwards, starting with a slope of almost 90 degree

	double	dSlope = tan(89.99 * M_DEG_TO_RAD);
	
	pParticle->Deposit_Material(m_pDEM, dSlope);


	return;
}


//---------------------------------------------------------
void CGPP_Model_BASE::Calc_Path_Deposition(CGPP_Model_Particle *pParticle)
{
	//---------------------------------------------------------
	// no deposition on the start cell itself or if no material is left over

	if( pParticle->Get_Count_Path_Positions() == 0 || pParticle->Get_Material() <= 0.0 )
	{
		return;
	}

	//---------------------------------------------------------
	double	dMaxDeposit			= m_PercentMaxDeposit * pParticle->Get_Material();
	double	dSlopeDeposit		= 0.0;
	double	dVelocityDeposit	= 0.0;
	double	dDeposit			= 0.0;

	if( m_GPP_Deposition_Model == GPP_DEPOSITION_SLOPE_ON_STOP || m_GPP_Deposition_Model == GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP )
	{
		if( pParticle->Get_Slope() < m_dDepSlopeThres )
		{
			dSlopeDeposit = (-dMaxDeposit / m_dDepSlopeThres) * pParticle->Get_Slope() + dMaxDeposit;
		}
	}

	if( m_GPP_Deposition_Model == GPP_DEPOSITION_VELOCITY_ON_STOP || m_GPP_Deposition_Model == GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP )
	{
		if( pParticle->Get_Speed() < m_dDepVelocityThres )
		{
			dVelocityDeposit = (-dMaxDeposit / m_dDepVelocityThres) * pParticle->Get_Speed() + dMaxDeposit;
		}
	}

	
	if( m_GPP_Deposition_Model == GPP_DEPOSITION_SLOPE_ON_STOP )
	{
		dDeposit = dSlopeDeposit;
	}
	else if( m_GPP_Deposition_Model == GPP_DEPOSITION_VELOCITY_ON_STOP )
	{
		dDeposit = dVelocityDeposit;
	}
	else // if( m_GPP_Deposition_Model == GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP )
	{
		dDeposit = M_GET_MIN(dSlopeDeposit, dVelocityDeposit);
	}

	if( dDeposit > pParticle->Get_Material() )
	{
		dDeposit = pParticle->Get_Material();
	}

	m_pDEM->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), dDeposit);
	pParticle->Set_Material(pParticle->Get_Material() - dDeposit);

	return;
}


//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Speed(CGPP_Model_Particle *pParticle, CGPP_Model_Particle *pParticleStartCell)
{
	bool	bResult;

	switch (m_GPP_Friction_Model)
	{
	default:
	case GPP_FRICTION_NONE:					bResult = true;												break;

	case GPP_FRICTION_GEOMETRIC_GRADIENT:	bResult = Update_Friction_Geometric_Gradient(pParticle);	break;

	case GPP_FRICTION_FAHRBOESCHUNG:		bResult = Update_Friction_Fahrboeschung(pParticle);			break;

	case GPP_FRICTION_SHADOW_ANGLE:			bResult = Update_Friction_Shadow_Angle(pParticle);			break;

	case GPP_FRICTION_ROCKFALL_VELOCITY:	bResult = Update_Friction_Rockfall_Velocity(pParticle);		break;

	case GPP_FRICTION_PCM_MODEL:			bResult = Update_Friction_PCM_Model(pParticle);				break;
	}

	if( !bResult )
	{
		if( m_pStopPositions != NULL )
		{
			m_pStopPositions->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), 1);
		}

		if( m_GPP_Deposition_Model > GPP_DEPOSITION_NONE )
		{
			if( (m_pMaterialFlux != NULL || m_pHazardSourcesMatl != NULL) && pParticle->Get_Material() > 0.0 )
			{
				double dMaterial = pParticle->Get_Material_Release() / m_iIterations;

				if( pParticle->Get_Material() < dMaterial )
				{
					dMaterial = pParticle->Get_Material();
				}

				pParticle->Set_Material_Flux(dMaterial * m_pDEM->Get_Cellsize());

				if( m_pMaterialFlux != NULL )
				{
					m_pMaterialFlux->Add_Value(pParticle->Get_X(), pParticle->Get_Y(), pParticle->Get_Material_Flux());
				}
			}

			Deposit_Material_On_Stop(pParticle);
		}

        if( m_pObjects != NULL )
        {
            pParticle->Evaluate_Damage_Potential(m_pObjectClasses, m_pHazardPaths, m_pHazardSources, m_pHazardSourcesMatl);
        }
	}

	return( bResult );
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Friction_Geometric_Gradient(CGPP_Model_Particle *pParticle)
{
	double dVertical	= pParticle->Get_Position_Start().z - pParticle->Get_Z();
	double dHorizontal	= sqrt(
							pow((pParticle->Get_Position_Start().x - pParticle->Get_X()) * m_pDEM->Get_Cellsize(), 2) +
							pow((pParticle->Get_Position_Start().y - pParticle->Get_Y()) * m_pDEM->Get_Cellsize(), 2)
							);

	bool bResult		= false;
	
	if( (dVertical / dHorizontal) >= pParticle->Get_TanFrictionAngle() )
	{
		bResult = true;
	}
	
	if( m_pMaxVelocity != NULL )
	{
		Update_Speed_Energy_Line(pParticle, dHorizontal, bResult);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Friction_Fahrboeschung(CGPP_Model_Particle *pParticle)
{
	double dVertical	= pParticle->Get_Position_Start().z - pParticle->Get_Z();

	bool bResult		= false;
	
	if( (dVertical / pParticle->Get_PathLength()) >= pParticle->Get_TanFrictionAngle() )
	{
		bResult = true;
	}
	
	if( m_pMaxVelocity != NULL )
	{
		Update_Speed_Energy_Line(pParticle, pParticle->Get_PathLength(), bResult);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Friction_Shadow_Angle(CGPP_Model_Particle *pParticle)
{
	if( !pParticle->Has_Impacted() )
	{
		if( m_pImpactAreas != NULL )
		{
			if( !m_pImpactAreas->is_NoData(pParticle->Get_X(), pParticle->Get_Y()) )
			{
				pParticle->Set_Impacted();
				pParticle->Set_Position_Start(pParticle->Get_Position());
			}
		}
		else
		{
			double dVertical	= pParticle->Get_Previous_Z() - pParticle->Get_Z();
			double dHorizontal	= pParticle->Get_Length();

			if( (dVertical / dHorizontal) < m_dTanThresFreeFall )
			{
				if( m_pReleaseAreas->asInt(pParticle->Get_X(), pParticle->Get_Y()) != pParticle->Get_ReleaseID() )
				{
					pParticle->Set_Impacted();
					pParticle->Set_Position_Start(pParticle->Get_Position());
				}
			}
		}

		if( m_pMaxVelocity != NULL )
		{
			double dVelocity = sqrt(2 * g_g * (pParticle->Get_Position_Start().z - pParticle->Get_Z()));

			if( dVelocity > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
			{
				m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), dVelocity);
			}
		}

		return( true );
	}
	else
	{
		return( Update_Friction_Geometric_Gradient(pParticle) );
	}
}

//---------------------------------------------------------
void CGPP_Model_BASE::Update_Speed_Energy_Line(CGPP_Model_Particle *pParticle, double dHorizontal, bool bResult)
{
	double dVelocity	= 0.0;

	if( bResult )
	{
		double dEnergyLineZ	= pParticle->Get_Position_Start().z - (pParticle->Get_TanFrictionAngle() * dHorizontal);
		double dZ			= dEnergyLineZ - pParticle->Get_Z();

		if( dZ >= 0.0)
		{
			dVelocity	= sqrt(2 * g_g * dZ);
		}
	}

	if( dVelocity > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
	{
		m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), dVelocity);
	}

	return;
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Friction_Rockfall_Velocity(CGPP_Model_Particle *pParticle)
{
	double			dVelocity = 0.0;

	if( !pParticle->Has_Impacted() )
	{
		if( m_pImpactAreas != NULL )
		{
			if( !m_pImpactAreas->is_NoData(pParticle->Get_X(), pParticle->Get_Y()) )
			{
				pParticle->Set_Impacted();

				dVelocity = sqrt(2 * g_g * (pParticle->Get_Position_Start().z - pParticle->Get_Z()));
			}
		}
		else
		{
			double dVertical	= pParticle->Get_Previous_Z() - pParticle->Get_Z();
			double dHorizontal	= pParticle->Get_Length();

			if( (dVertical / dHorizontal) < m_dTanThresFreeFall )
			{
				if( m_pReleaseAreas->asInt(pParticle->Get_X(), pParticle->Get_Y()) != pParticle->Get_ReleaseID() )
				{
					pParticle->Set_Impacted();
				}
			}
		}

		if( !pParticle->Has_Impacted() )
		{
			dVelocity = sqrt(2 * g_g * (pParticle->Get_Position_Start().z - pParticle->Get_Z()));
		}
		else
		{
			switch (m_GPP_Method_Impact)
			{
			default:
			case GPP_IMPACT_REDUCTION_ENERGY:
				dVelocity = sqrt(2 * g_g * (pParticle->Get_Position_Start().z - pParticle->Get_Z()) * m_dReductionFactor);
				break;
			case GPP_IMPACT_REDUCTION_PRESERVED_VELOCITY:
				dVelocity = sqrt(2 * g_g * (pParticle->Get_Position_Start().z - pParticle->Get_Z()) * sin(atan(pParticle->Get_Slope())));
				break;
			}
		}

		pParticle->Set_Speed(dVelocity);

		if( m_pMaxVelocity != NULL )
		{
			if( dVelocity > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
				m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), dVelocity);
		}

		return( true );
	}
	else
	{
		double dVertical	= pParticle->Get_Previous_Z() - pParticle->Get_Z();
		double dHorizontal	= pParticle->Get_Length();
		double dMu			= pParticle->Get_FrictionMu();


		if( m_pFrictionMu != NULL && !m_pFrictionMu->is_NoData(pParticle->Get_X(), pParticle->Get_Y()) )
		{
			pParticle->Set_FrictionMu(m_pFrictionMu->asDouble(pParticle->Get_X(), pParticle->Get_Y()));
		}
		else
		{
			pParticle->Set_FrictionMu(m_dFrictionMu);
		}

		dMu = (dMu + pParticle->Get_FrictionMu()) / 2.0;

		switch (m_GPP_Mode_Motion)
		{
		default:
		case GPP_MODE_MOTION_SLIDING:
			dVelocity = pow(pParticle->Get_Speed(), 2) + 2 * g_g * (dVertical - dMu * dHorizontal);
			break;
		case GPP_MODE_MOTION_ROLLING:
			dVelocity = pow(pParticle->Get_Speed(), 2) + 1.428571 * g_g * (dVertical - dMu * dHorizontal);
			break;
		}

		if( dVelocity <= 0.0 )
		{
			pParticle->Set_Speed(0.0);

			if( m_pMaxVelocity != NULL )
			{
				if( 0.0 > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
					m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), 0.0);
			}

			return( false );
		}

		dVelocity = sqrt(dVelocity);

		pParticle->Set_Speed(dVelocity);

		if( m_pMaxVelocity != NULL )
		{
			if( dVelocity > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
				m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), dVelocity);
		}

		return( true );
	}
}

//---------------------------------------------------------
bool CGPP_Model_BASE::Update_Friction_PCM_Model(CGPP_Model_Particle *pParticle)
{
	double dVertical	= pParticle->Get_Previous_Z() - pParticle->Get_Z();
	double dHorizontal	= pParticle->Get_Length();
	double dLength3D	= sqrt(pow(dHorizontal, 2) + pow(dVertical, 2));
	double dMu			= pParticle->Get_FrictionMu();
	double dMassToDrag	= pParticle->Get_FrictionMassToDrag();

	if( m_pFrictionMu != NULL && !m_pFrictionMu->is_NoData(pParticle->Get_X(), pParticle->Get_Y()) )
	{
		pParticle->Set_FrictionMu(m_pFrictionMu->asDouble(pParticle->Get_X(), pParticle->Get_Y()));
	}
	else
	{
		pParticle->Set_FrictionMu(m_dFrictionMu);
	}

	dMu = (dMu + pParticle->Get_FrictionMu()) / 2.0;

	if( m_pFrictionMassToDrag != NULL && !m_pFrictionMassToDrag->is_NoData(pParticle->Get_X(), pParticle->Get_Y()) )
	{
		pParticle->Set_FrictionMassToDrag(m_pFrictionMassToDrag->asDouble(pParticle->Get_X(), pParticle->Get_Y()));
	}
	else
	{
		pParticle->Set_FrictionMassToDrag(m_dFrictionMassToDrag);
	}
	
	dMassToDrag = (dMassToDrag + pParticle->Get_FrictionMassToDrag()) / 2.0;

	double dSlope		= atan(pParticle->Get_Slope());
	double dDeltaSlope	= 0.0;

	if( atan(pParticle->Get_Previous_Slope()) > dSlope )
		dDeltaSlope = atan(pParticle->Get_Previous_Slope()) - dSlope;

	double dAlpha		= g_g * (sin(dSlope) - dMu * cos(dSlope));
	double dBeta		= (-2.0 * dLength3D) / dMassToDrag;
	double dVelocity	= dAlpha * dMassToDrag * (1 - pow(M_EULER, dBeta)) + pow(pParticle->Get_Speed(), 2) * pow(M_EULER, dBeta) * cos(dDeltaSlope);

	if( dVelocity < 0 )
	{
		pParticle->Set_Speed(0.0);

		if( m_pMaxVelocity != NULL )
		{
			if( 0.0 > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
				m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), 0.0);
		}

		return( false );
	}

	dVelocity = sqrt(dVelocity);

	pParticle->Set_Speed(dVelocity);

	if( m_pMaxVelocity != NULL )
	{
		if( dVelocity > m_pMaxVelocity->asDouble(pParticle->Get_X(), pParticle->Get_Y()) )
			m_pMaxVelocity->Set_Value(pParticle->Get_X(), pParticle->Get_Y(), dVelocity);
	}

	return( true );
}


//---------------------------------------------------------
int CGPP_Model_BASE::_Get_ObjectClass_Decimal(sLong n)
{
    int decimal = 0, i = 0, remainder;

    while( n != 0 )
    {
        remainder = n%10;
        n /= 10;
        decimal += (int)(remainder * pow(2, i));
        ++i;
    }

    return( decimal );
}


//---------------------------------------------------------
double CGPP_Model_BASE::_Get_ObjectClass_Binary(int n)
{
    sLong binary = 0;
    int remainder, i = 1;

    while( n != 0 )
    {
        remainder = n%2;
        n /= 2;
        binary += remainder * i;
        i *= 10;
    }

    return( (double)binary );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
