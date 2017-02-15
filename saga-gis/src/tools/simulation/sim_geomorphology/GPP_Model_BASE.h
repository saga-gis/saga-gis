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
//                   GPP_Model_BASE.h                    //
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
#ifndef HEADER_INCLUDED__GPP_Model_BASE_H
#define HEADER_INCLUDED__GPP_Model_BASE_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "saga_api/saga_api.h"
#include "saga_api/api_core.h"
#include "saga_api/tool.h"

#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <time.h>
#include <vector>

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const double g_g = 9.80655;

//---------------------------------------------------------
typedef enum EGPP_Model_Path_Key
{
	GPP_PATH_MAXIMUM_SLOPE	= 0,
	GPP_PATH_RANDOM_WALK,
	GPP_PATH_KEY_COUNT
}
TGPP_Model_Path_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_Path_Key_Name[GPP_PATH_KEY_COUNT]	=
{
	_TL("Maximum Slope"),
	_TL("Random Walk")
};


//---------------------------------------------------------
typedef enum EGPP_Model_Friction_Key
{
	GPP_FRICTION_NONE	= 0,
	GPP_FRICTION_GEOMETRIC_GRADIENT,
	GPP_FRICTION_FAHRBOESCHUNG,
	GPP_FRICTION_SHADOW_ANGLE,
	GPP_FRICTION_ROCKFALL_VELOCITY,
	GPP_FRICTION_PCM_MODEL,
	GPP_FRICTION_KEY_COUNT
}
TGPP_Model_Friction_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_Friction_Key_Name[GPP_FRICTION_KEY_COUNT]	=
{
	_TL("None"),
	_TL("Geometric Gradient (Heim 1932)"),
	_TL("Fahrboeschung Principle (Heim 1932)"),
	_TL("Shadow Angle (Evans & Hungr 1988)"),
	_TL("1-parameter friction model (Scheidegger 1975)"),
	_TL("PCM Model (Perla et al. 1980)")
};


//---------------------------------------------------------
typedef enum EGPP_Model_Impact_Reduction_Key
{
	GPP_IMPACT_REDUCTION_ENERGY	= 0,
	GPP_IMPACT_REDUCTION_PRESERVED_VELOCITY,
	GPP_IMPACT_REDUCTION_KEY_COUNT
}
TGPP_Model_Impact_Reduction_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_Impact_Reduction_Key_Name[GPP_IMPACT_REDUCTION_KEY_COUNT]	=
{
	_TL("Energy Reduction (Scheidegger 1975)"),
	_TL("Preserved Component of Velocity (Kirkby & Statham 1975)")
};


//---------------------------------------------------------
typedef enum EGPP_Model_Mode_Motion_Key
{
	GPP_MODE_MOTION_SLIDING	= 0,
	GPP_MODE_MOTION_ROLLING,
	GPP_MODE_MOTION_KEY_COUNT
}
TGPP_Model_Mode_Motion_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_Mode_Motion_Key_Name[GPP_MODE_MOTION_KEY_COUNT]	=
{
	_TL("Sliding"),
	_TL("Rolling")
};


//---------------------------------------------------------
typedef enum EGPP_Model_Deposition_Key
{
	GPP_DEPOSITION_NONE	= 0,
	GPP_DEPOSITION_ON_STOP,
	GPP_DEPOSITION_SLOPE_ON_STOP,
	GPP_DEPOSITION_VELOCITY_ON_STOP,
	GPP_DEPOSITION_SLOPE_VELOCITY_ON_STOP,
	GPP_DEPOSITION_KEY_COUNT
}
TGPP_Model_Deposition_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_Deposition_Key_Name[GPP_DEPOSITION_KEY_COUNT]	=
{
	_TL("None"),
	_TL("On Stop"),
	_TL("Slope & On Stop"),
	_TL("Velocity & On Stop"),
	_TL("min(Slope,Velocity) & On Stop")
};


//---------------------------------------------------------
typedef enum EGPP_Model_StartCell_Sorting_Key
{
	GPP_SORT_LOWEST_TO_HIGHEST	= 0,
	GPP_SORT_HIGHEST_TO_LOWEST,
	GPP_SORT_KEY_COUNT
}
TGPP_Model_StartCell_Sorting_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_StartCell_Sorting_Key_Name[GPP_SORT_KEY_COUNT]	=
{
	_TL("Lowest to Highest"),
	_TL("Highest to Lowest")
};


//---------------------------------------------------------
typedef enum EGPP_Model_Processing_Order_Key
{
	GPP_RELEASE_SEQUENTIAL	= 0,
	GPP_RELEASE_SEQUENTIAL_PER_ITERATION,
	GPP_RELEASE_PARALLEL_PER_ITERATION,
	GPP_PROCESSING_ORDER_KEY_Count
}
TGPP_Model_Processing_Order_Key;

//---------------------------------------------------------
const CSG_String	gGPP_Model_Processing_Order_Key_Name[GPP_PROCESSING_ORDER_KEY_Count]	=
{
	_TL("RAs in Sequence"),
	_TL("RAs in Sequence per Iteration"),
	_TL("RAs in Parallel per Iteration")
};



//---------------------------------------------------------
typedef struct	{
	int x;
	int y;
	double z;
				}GRID_CELL;

//---------------------------------------------------------
typedef struct	{
	GRID_CELL	position;
	int			exitDir;
	double		slope;		// downward, as ratio rise/run
	double		length;
	double		deposit;
				}PATH_CELL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGPP_Model_Particle
{
public:
	CGPP_Model_Particle(int iReleaseID, GRID_CELL gPosition, double dMaterial, double dTanFrictionAngle, double dFrictionMu, double dFrictionMassToDrag, double dVelocity);

	int			Get_ReleaseID(void);
	GRID_CELL	Get_Position(void);
	GRID_CELL	Get_Previous_Position(void);
	int			Get_X(void);
	int			Get_Previous_X(void);
	int			Get_Y(void);
	int			Get_Previous_Y(void);
	double		Get_Z(void);
	double		Get_Previous_Z(void);
	void		Set_Position(GRID_CELL gPosition);
	void		Set_Previous_Position(GRID_CELL gPosition, double dSlope, double dLength, int iExitDir);

	GRID_CELL	Get_Position_Start(void) const;
	void		Set_Position_Start(GRID_CELL gPosition);

	double		Get_PathLength(void);
	void		Set_PathLength(double dLength);
	double		Get_Length(void);
	double		Get_Slope(void);
	double		Get_Previous_Slope(void);
	void		Set_Slope(double dSlope);
	int			Get_Previous_Exit_Direction(void);
	int			Get_Exit_Direction(size_t iPosition);
	int			Get_Entry_Direction(void);
	size_t		Get_Count_Path_Positions(void);

	double		Get_Speed(void);
	void		Set_Speed(double dVelocity);
	double		Get_TanFrictionAngle(void);
	bool		Has_Impacted(void);
	void		Set_Impacted(void);
	double		Get_FrictionMu(void);
	void		Set_FrictionMu(double dFrictionMu);
	double		Get_FrictionMassToDrag(void);
	void		Set_FrictionMassToDrag(double dFrictionMassToDrag);

	void		Add_Cell_To_Path(CSG_Grid *pGrid, int x, int y);
	bool		Is_Cell_In_Path(CSG_Grid *pGrid, int x, int y);

	double		Get_Material(void);
	void		Set_Material(double dMaterial);
	void		Deposit_Material(CSG_Grid *pGrid, double dSlope);


protected:


private:

	int						m_iReleaseID;				// identifier of release area
	GRID_CELL				m_gPosition;				// current particle position
	GRID_CELL				m_gPosition_start;			// start position of particle
	double					m_dPathLength;				// accumulated process path length
	double					m_dMaterial;				// available material
	double					m_dTanFrictionAngle;		// tangens of friction angle
	double					m_dFrictionMu;				// friction parameter mu
	double					m_dFrictionMassToDrag;		// friction parameter M/D
	double					m_dSlope;					// current slope to exit direction (rise/run)
	bool					m_bImpacted;				// flag for impact
	double					m_dVelocity;				// current velocity
	std::vector<PATH_CELL>	m_vPath;					// grid cells of particle path
	std::set<sLong>			m_sCellsInPath;				// grid cells traversed by particle

	sLong					_Get_Cell_Number_Grid(CSG_Grid *pGrid, int x, int y);
};


//---------------------------------------------------------
class CGPP_Model_BASE
{

public:


protected:

	void		Run_GPP_Model(std::vector<class CGPP_Model_Particle> *pvProcessingList);

	bool		Update_Path(CGPP_Model_Particle *pParticle, double dMaterialRun, CGPP_Model_Particle *pParticleStartCell);
	bool		Update_Speed(CGPP_Model_Particle *pParticle, CGPP_Model_Particle *pParticleStartCell);

	bool		Update_Path_Maximum_Slope(CGPP_Model_Particle *pParticle, bool &bEdge, bool &bSink);
	bool		Update_Path_Random_Walk(CGPP_Model_Particle *pParticle, bool &bEdge, bool &bSink);

	bool		Update_Friction_Geometric_Gradient(CGPP_Model_Particle *pParticle);
	bool		Update_Friction_Fahrboeschung(CGPP_Model_Particle *pParticle);
	bool		Update_Friction_Shadow_Angle(CGPP_Model_Particle *pParticle);
	void		Update_Speed_Energy_Line(CGPP_Model_Particle *pParticle, double dHorizontal, bool bResult);
	bool		Update_Friction_Rockfall_Velocity(CGPP_Model_Particle *pParticle);
	bool		Update_Friction_PCM_Model(CGPP_Model_Particle *pParticle);

	void		Fill_Sink(CGPP_Model_Particle *pParticle);
	bool		Detect_Dir_to_Overflow_Cell(CGPP_Model_Particle *pParticle, int &iOverDir, double &dOverflowZ);
	void		Deposit_Material_On_Stop(CGPP_Model_Particle *pParticle);
	void		Update_Material_Start_Cell(CGPP_Model_Particle *pParticleStartCell, CGPP_Model_Particle *pParticle, double dMaterial);
	void		Calc_Path_Deposition(CGPP_Model_Particle *pParticle);

	void		Add_Dataset_Parameters(CSG_Parameters *pParameters);
	void		Add_Process_Path_Parameters(CSG_Parameters *pParameters);
	void		Add_Runout_Parameters(CSG_Parameters *pParameters);
	void		Add_Deposition_Parameters(CSG_Parameters *pParameters);
	void		Add_Sink_Parameters(CSG_Parameters *pParameters);

	bool		Initialize_Parameters(CSG_Parameters &Parameters);
	void		Initialize_Random_Generator(void);
	CGPP_Model_Particle Init_Particle(int iReleaseID, GRID_CELL gPosition);
	void		Finalize(CSG_Parameters *pParameters);

	CSG_Grid	*m_pDEM, *m_pReleaseAreas, *m_pMaterial, *m_pFrictionAngles, *m_pImpactAreas, *m_pFrictionMu, *m_pFrictionMassToDrag;
	CSG_Grid	*m_pProcessArea, *m_pDeposition, *m_pMaxVelocity, *m_pStopPositions;

	int			m_GPP_Path_Model;
	int			m_iIterations;
	double		m_dRW_SlopeThres;
	double		m_dRW_Exponent;
	double		m_dRW_Persistence;
	int			m_iProcessingOrder;
	unsigned int	m_iSeed;

	int			m_GPP_Friction_Model;
	int			m_GPP_Method_Impact;
	int			m_GPP_Mode_Motion;
	double		m_dTanFrictionAngle;
	double		m_dTanThresFreeFall;
	double		m_dFrictionMu;
	double		m_dReductionFactor;
	double		m_dInitVelocity;
	double		m_dFrictionMassToDrag;

	int			m_GPP_Deposition_Model;
	double		m_PercentInitialDeposit;
	double		m_dDepSlopeThres;
	double		m_dDepVelocityThres;
	double		m_PercentMaxDeposit;
	double		m_MinPathLength;

	double		m_SinkMinSlope;

	std::map<int, std::vector<GRID_CELL> >	m_mReleaseAreas;	// key = number of release area, value = C++ vector with grid cells of release area
	
	void		Add_Start_Cell(int iID, GRID_CELL startCell);
	void		SortStartCells(std::vector<GRID_CELL> *pStartCells, int iSortMode);
	void		SortParticles(std::vector<class CGPP_Model_Particle> *pvProcessingList, int iSortMode);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GPP_Model_BASE_H

