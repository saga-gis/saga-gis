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
//                    GPP_Model.cpp                      //
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
#include "GPP_Model.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGPP_Model::CGPP_Model(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Gravitational Process Path Model"));

	Set_Author		(SG_T("V. Wichmann (c) 2016-2022"));

    Set_Version     ("1.1");

	Set_Description	(_TW("The Gravitational Process Path (GPP) model can be used to simulate the process path and "
                         "run-out area of gravitational processes based on a digital terrain model (DTM). The "
                         "conceptual model combines several components (process path, run-out length, sink filling "
                         "and material deposition) to simulate the movement of a mass point from an initiation site "
                         "to the deposition area. For each component several modeling approaches are provided, "
                         "which makes the tool configurable for different processes such as rockfall, debris flows "
                         "or snow avalanches.<br/><br/>"
                         "The tool can be applied to regional-scale studies such as natural hazard susceptibility "
                         "mapping but also contains components for scenario-based modeling of single events. Both "
                         "the modeling approaches and precursor implementations of the tool have proven their "
                         "applicability in numerous studies, also including geomorphological research questions "
                         "such as the delineation of sediment cascades or the study of process connectivity.<br/><br/>"
						 "Please provide the reference cited below in your work if you are using the GPP model.<br/><br/>"
                         "<b>Addendum:</b><br/>"
                         "The article is not clear about the way the impact on the slope is exactly modelled when the 'Shadow Angle' "
                         "or '1-parameter' friction model is used. Besides the 'Threshold Angle Free Fall' criterion to determine the "
                         "location of the first impact, it is also assumed that the particle must leave its own release "
                         "area (given by its ID) in order to impact. This is actually a conceptual design, taking into "
                         "account that free fall usually occurs in steep rock faces (release areas), and the fact, that "
                         "such rockfaces are not characterised very well in a 2.5D elevation model. You can work around that "
                         "conceptual design by providing a grid describing the 'slope impact areas' as input. Using such a "
                         "grid disables the 'Threshold Angle Free Fall' parameter.<br/><br/>"
                         "<b>New in version 1.1:</b><br/>"
                         "Since version 1.1 the model supports the monitoring of potentially endangered objects like infrastructure "
                         "and reports from which process paths and release areas objects might be hit. In order to enable this "
                         "backtracking, the user must provide an 'Objects' grid as input. The grid can be used to store different types "
                         "or classes of objects, using one-hot categorical data encoding for each object class, i.e. powers "
                         "of ten: 1, 10, 100, 1000, etc. (all other cells NoData). The 'Endangered Objects' output grid will "
                         "contain combinations of these numbers if several different classes were hit from a grid cell, allowing "
                         "to analyse which object classes might be hit from which location."
    ));

	Add_Reference("Wichmann, V.", "2017",
		"The Gravitational Process Path (GPP) model (v1.0) - a GIS-based simulation framework for gravitational processes",
		"Geosci. Model Dev., 10, 3309-3327, doi:10.5194/gmd-10-3309-2017",
		SG_T("https://doi.org/10.5194/gmd-10-3309-2017")
	);

	Add_Dataset_Parameters(Get_Parameters());

	Add_Process_Path_Parameters(Get_Parameters());

	Add_Runout_Parameters(Get_Parameters());

	Add_Deposition_Parameters(Get_Parameters());

	Add_Sink_Parameters(Get_Parameters());
}


//---------------------------------------------------------
CGPP_Model::~CGPP_Model(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


bool CGPP_Model::On_Execute(void)
{
	//---------------------------------------------------------
	SG_UI_Process_Set_Text(_TL("Initializing ..."));

	if( !Initialize_Parameters(*Get_Parameters()) )
	{
		return( false );
	}

	Initialize_Random_Generator();


	//---------------------------------------------------------
	// initialize release areas / start cells

	m_mReleaseAreas.clear();

	for (int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for (int x=0; x<Get_NX(); x++)
		{
			if( !m_pReleaseAreas->is_NoData(x, y) )
			{
				int			iID;
				GRID_CELL	startCell;

				startCell.x	= x;
				startCell.y	= y;
				startCell.z	= m_pDEM->asDouble(x, y);
				iID			= m_pReleaseAreas->asInt(x, y);

				Add_Start_Cell(iID, startCell);
			}
		}
	}


	//---------------------------------------------------------
	switch( m_iProcessingOrder )
	{
	case GPP_RELEASE_SEQUENTIAL:
	case GPP_RELEASE_SEQUENTIAL_PER_ITERATION:
		// sort start cells of each release area by elevation

		for (std::map<int, std::vector<GRID_CELL> >::iterator it=m_mReleaseAreas.begin(); it != m_mReleaseAreas.end(); ++it)
		{
			SortStartCells(&(*it).second, GPP_SORT_LOWEST_TO_HIGHEST);
		}
		break;

	default:		break;
	}


    //---------------------------------------------------------
    if( m_pObjects != NULL )
    {
        m_pEndangered->Assign_NoData();

        m_pObjectClasses = new CSG_Grid(m_pObjects, SG_DATATYPE_Int);

        #pragma omp parallel for
        for(int y=0; y<m_pObjects->Get_NY(); y++)
        {
            for(int x=0; x<m_pObjects->Get_NX(); x++)
            {
                if( m_pObjects->is_NoData(x, y) )
                {
                    m_pObjectClasses->Set_NoData(x, y);
                }
                else
                {
                    sLong n = m_pObjects->asLong(x, y);

                    int decimal = 0, i = 0, remainder;

                    while( n != 0 )
                    {
                        remainder = n%10;
                        n /= 10;
                        decimal += (int)(remainder * pow(2, i));
                        ++i;
                    }

                    m_pObjectClasses->Set_Value(x, y, decimal);
                }
            }
        }
    }


	//---------------------------------------------------------
	SG_UI_Process_Set_Text(_TL("Processing ..."));

	switch( m_iProcessingOrder )
	{
	case GPP_RELEASE_SEQUENTIAL:
	default:

		for (std::map<int, std::vector<GRID_CELL> >::iterator it=m_mReleaseAreas.begin(); it != m_mReleaseAreas.end(); ++it)
		{
			int	iReleaseID = (*it).first;

			std::vector<class CGPP_Model_Particle>	vProcessingList;

			for (size_t i=0; i<m_mReleaseAreas.at(iReleaseID).size(); i++)
			{
				vProcessingList.push_back(Init_Particle(iReleaseID, m_mReleaseAreas.at(iReleaseID).at(i)));
			}

			Run_GPP_Model(&vProcessingList);
		}

		break;

	case GPP_RELEASE_SEQUENTIAL_PER_ITERATION:
	case GPP_RELEASE_PARALLEL_PER_ITERATION:

		std::vector<class CGPP_Model_Particle>	vProcessingList;

		for (std::map<int, std::vector<GRID_CELL> >::iterator it=m_mReleaseAreas.begin(); it != m_mReleaseAreas.end(); ++it)
		{
			int	iReleaseID = (*it).first;

			for (size_t i=0; i<m_mReleaseAreas.at(iReleaseID).size(); i++)
			{
				vProcessingList.push_back(Init_Particle(iReleaseID, m_mReleaseAreas.at(iReleaseID).at(i)));
			}
		}

		if( m_iProcessingOrder == GPP_RELEASE_PARALLEL_PER_ITERATION )
		{
			// sort start cells of all release areas by elevation
			SortParticles(&vProcessingList, GPP_SORT_LOWEST_TO_HIGHEST);
		}

		Run_GPP_Model(&vProcessingList);

		break;
	}


	//---------------------------------------------------------
	Finalize(Get_Parameters());

	return( true );
}


//---------------------------------------------------------
int CGPP_Model::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier(SG_T("MATERIAL")) )
	{
		pParameters->Get_Parameter("SINK_MIN_SLOPE"				)->Set_Enabled( pParameter->asGrid() != NULL );
		pParameters->Get_Parameter("DEPOSITION"					)->Set_Enabled( pParameter->asGrid() != NULL );
	}

    //-----------------------------------------------------
    if(	pParameter->Cmp_Identifier(SG_T("OBJECTS")) )
    {
        pParameters->Get_Parameter("ENDANGERED" 		        )->Set_Enabled( pParameter->asGrid() != NULL );
    }

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier(SG_T("PROCESS_PATH_MODEL")) )
	{
		pParameters->Get_Parameter("RW_SLOPE_THRES"				)->Set_Enabled( pParameter->asInt() == GPP_PATH_RANDOM_WALK );
		pParameters->Get_Parameter("RW_EXPONENT"				)->Set_Enabled( pParameter->asInt() == GPP_PATH_RANDOM_WALK );
		pParameters->Get_Parameter("RW_PERSISTENCE"				)->Set_Enabled( pParameter->asInt() == GPP_PATH_RANDOM_WALK );
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier(SG_T("FRICTION_MODEL")) )
	{
		pParameters->Get_Parameter("MAX_VELOCITY"				)->Set_Enabled( pParameter->asInt() > 0 );
		pParameters->Get_Parameter("FRICTION_THRES_FREE_FALL"	)->Set_Enabled((pParameter->asInt() == GPP_FRICTION_SHADOW_ANGLE
																				|| pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY)
																				&& pParameters->Get_Parameter("SLOPE_IMPACT_GRID")->asGrid() == NULL );
		pParameters->Get_Parameter("FRICTION_METHOD_IMPACT"		)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY );
		pParameters->Get_Parameter("FRICTION_IMPACT_REDUCTION"	)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY);
		pParameters->Get_Parameter("FRICTION_ANGLE"				)->Set_Enabled((pParameter->asInt() == GPP_FRICTION_GEOMETRIC_GRADIENT
																				|| pParameter->asInt() == GPP_FRICTION_FAHRBOESCHUNG
																				|| pParameter->asInt() == GPP_FRICTION_SHADOW_ANGLE)
																				&& pParameters->Get_Parameter("FRICTION_ANGLE_GRID")->asGrid() == NULL );
		pParameters->Get_Parameter("FRICTION_ANGLE_GRID"		)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_GEOMETRIC_GRADIENT
																				|| pParameter->asInt() == GPP_FRICTION_FAHRBOESCHUNG
																				|| pParameter->asInt() == GPP_FRICTION_SHADOW_ANGLE );
		pParameters->Get_Parameter("SLOPE_IMPACT_GRID"			)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_SHADOW_ANGLE
																				|| pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY );
		pParameters->Get_Parameter("FRICTION_MU"				)->Set_Enabled((pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY
																				|| pParameter->asInt() == GPP_FRICTION_PCM_MODEL)
																				&& pParameters->Get_Parameter("FRICTION_MU_GRID")->asGrid() == NULL );
		pParameters->Get_Parameter("FRICTION_MU_GRID"			)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY
																				|| pParameter->asInt() == GPP_FRICTION_PCM_MODEL );
		pParameters->Get_Parameter("FRICTION_MODE_OF_MOTION"	)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY );
		pParameters->Get_Parameter("FRICTION_MASS_TO_DRAG"		)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_PCM_MODEL
																				&& pParameters->Get_Parameter("FRICTION_MASS_TO_DRAG_GRID")->asGrid() == NULL );
		pParameters->Get_Parameter("FRICTION_MASS_TO_DRAG_GRID"	)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_PCM_MODEL );
		pParameters->Get_Parameter("FRICTION_INIT_VELOCITY"		)->Set_Enabled( pParameter->asInt() == GPP_FRICTION_PCM_MODEL );
	}

	if(	pParameter->Cmp_Identifier(SG_T("FRICTION_ANGLE_GRID")) )
	{
		pParameters->Get_Parameter("FRICTION_ANGLE"				)->Set_Enabled( pParameter->asGrid() == NULL
																				&& (pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_GEOMETRIC_GRADIENT
																				|| pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_FAHRBOESCHUNG
																				|| pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_SHADOW_ANGLE) );
	}

	if(	pParameter->Cmp_Identifier(SG_T("FRICTION_METHOD_IMPACT")) )
	{
		pParameters->Get_Parameter("FRICTION_IMPACT_REDUCTION"	)->Set_Enabled( pParameter->asInt() == GPP_IMPACT_REDUCTION_ENERGY
																				&& pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY );
	}

	if(	pParameter->Cmp_Identifier(SG_T("SLOPE_IMPACT_GRID")) )
	{
		pParameters->Get_Parameter("FRICTION_THRES_FREE_FALL"	)->Set_Enabled( pParameter->asGrid() == NULL
																				&& (pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_SHADOW_ANGLE
																				|| pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY) );
	}

	if(	pParameter->Cmp_Identifier(SG_T("FRICTION_MU_GRID")) )
	{
		pParameters->Get_Parameter("FRICTION_MU"				)->Set_Enabled( pParameter->asGrid() == NULL
																				&& (pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_ROCKFALL_VELOCITY
																				|| pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_PCM_MODEL) );
	}

	if(	pParameter->Cmp_Identifier(SG_T("FRICTION_MASS_TO_DRAG_GRID")) )
	{
		pParameters->Get_Parameter("FRICTION_MASS_TO_DRAG"		)->Set_Enabled( pParameter->asGrid() == NULL
																				&& pParameters->Get_Parameter("FRICTION_MODEL")->asInt() == GPP_FRICTION_PCM_MODEL );
	}


	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier(SG_T("DEPOSITION_MODEL")) )
	{
		pParameters->Get_Parameter("DEPOSITION_INITIAL"			)->Set_Enabled( pParameter->asInt() >= GPP_DEPOSITION_ON_STOP );
		pParameters->Get_Parameter("DEPOSITION_SLOPE_THRES"		)->Set_Enabled( pParameter->asInt() == GPP_DEPOSITION_SLOPE_ON_STOP
																				|| pParameter->asInt() == GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP );
		pParameters->Get_Parameter("DEPOSITION_VELOCITY_THRES"	)->Set_Enabled( pParameter->asInt() == GPP_DEPOSITION_VELOCITY_ON_STOP
																				|| pParameter->asInt() == GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP );
		pParameters->Get_Parameter("DEPOSITION_MAX"				)->Set_Enabled( pParameter->asInt() > GPP_DEPOSITION_ON_STOP );
		pParameters->Get_Parameter("DEPOSITION_MIN_PATH"		)->Set_Enabled( pParameter->asInt() > GPP_DEPOSITION_ON_STOP );
	}


	//-----------------------------------------------------
	return (1);
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
