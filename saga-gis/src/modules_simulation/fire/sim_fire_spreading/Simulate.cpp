/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Simulate.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

#include "Simulate.h"

#define MS2FTMIN (60.0 / 0.3048)
#define FTMIN2MMIN 0.3048
#define BTU2KCAL 0.252164401
#define FT2M 0.3048
#define NO_TIME_LIMIT -1
#define THRESHOLD_FOR_DIFFERENCE 0.1

CSimulate::CSimulate(void){
	
	Set_Name		(_TL("Simulation"));
	Set_Author		(SG_T("(c) 2004 Victor Olaya"));
	Set_Description	(_TW(
		"Fire simulation based on the BEHAVE fire modeling system "
		"supported by the U.S. Forest Service, Fire and Aviation Management. "
		"Find more information on BEHAVE at the <i>Public Domain Software for the Wildland Fire Community</i> at "
		"<a target=\"_blank\" href=\"http://fire.org\">http://fire.org</a>\n"
		"\n"
		"Reference:\n"
		"Andrews, P.L. (1986): BEHAVE: Fire Behavior Prediction and Fuel Modeling System - "
		"Burn Subsystem, Part 1. U.S. Department of Agriculture, Forest Service General, Technical Report INT-194. "
	));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("DEM"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"FUEL", 
						_TL("Fuel Model"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"WINDSPD", 
						_TL("Wind Speed"),
						_TL("Wind speed (m/s)"), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"WINDDIR", 
						_TL("Wind Direction"), 
						_TL("Wind direction (degrees clockwise from north)"),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M1H", 
						_TL("Dead Fuel Moisture 1H"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M10H", 
						_TL("Dead Fuel Moisture 10H"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M100H", 
						_TL("Dead Fuel Moisture 100H"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MHERB", 
						_TL("Herbaceous Fuel Moisture"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MWOOD", 
						_TL("Wood Fuel Moisture"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"IGNITION", 
						_TL("Ignition Points"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"TIME", 
						_TL("Time"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Grid(NULL, 
						"FLAME", 
						_TL("Flame Length"), 
						_TL("Flame Length (m)"), 
						PARAMETER_OUTPUT);

	Parameters.Add_Grid(NULL, 
						"INTENSITY", 
						_TL("Intensity"), 
						_TL("Intensity (Kcal/m)"), 
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL, 
						"UPDATEVIEW", 
						_TL("Update View"),
						_TL("Update view during simulation."),
						PARAMETER_TYPE_Bool, 
						true);

}//constructor

CSimulate::~CSimulate(void){}

bool CSimulate::On_Execute(void){

	AssignParameters();
	CalculateFire();
	
	DeleteObjects();
	return true;

}//method

void CSimulate::DeleteObjects(){

	delete m_pAspectGrid;
	delete m_pSlopeGrid;

	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

}//method

bool CSimulate::AssignParameters(){

	int x,y;

	m_pDEM = Parameters("DEM")->asGrid();
	m_pFuelGrid = Parameters("FUEL")->asGrid();
	m_pIgnGrid = Parameters("IGNITION")->asGrid();
	m_pWindDirGrid = Parameters("WINDDIR")->asGrid();
	m_pWindSpdGrid = Parameters("WINDSPD")->asGrid();
	m_pM1Grid = Parameters("M1H")->asGrid();
	m_pM10Grid = Parameters("M10H")->asGrid();
	m_pM100Grid = Parameters("M100H")->asGrid();
	m_pMHerbGrid = Parameters("MHERB")->asGrid();
	m_pMWoodGrid = Parameters("MWOOD")->asGrid();
	m_pTimeGrid = Parameters("TIME")->asGrid();
	m_pFlameGrid = Parameters("FLAME")->asGrid();
	m_pIntensityGrid = Parameters("INTENSITY")->asGrid();

    m_Catalog = Fire_FuelCatalogCreateStandard("Standard", 13);
    Fire_FlameLengthTable(m_Catalog, 500, 0.1);

	//substitute no-data values
	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){

			if (m_pWindSpdGrid->is_NoData(x, y)){
				m_pWindSpdGrid->Set_Value(x, y, 0.);
			}//if
			if (m_pWindDirGrid->is_NoData(x, y)){
				m_pWindDirGrid->Set_Value(x, y, 0.);
			}//if
			if (m_pM1Grid->is_NoData(x, y)){
				m_pM1Grid->Set_Value(x, y, 0.);
			}//if
			if (m_pM10Grid->is_NoData(x, y)){
				m_pM10Grid->Set_Value(x, y, 0.);
			}//if
			if (m_pM100Grid->is_NoData(x, y)){
				m_pM100Grid->Set_Value(x, y, 0.);
			}//if
			if (m_pMHerbGrid->is_NoData(x, y)){
				m_pMHerbGrid->Set_Value(x, y, 0.);
			}//if
			if (m_pMWoodGrid->is_NoData(x, y)){
				m_pMWoodGrid->Set_Value(x, y, 0.);
			}//if

		}//for
	}//for


	//-----------------------------------------------------
	// calculate slope and aspect using CSG_Grid class'
	// built-in function (after Zevenbergen & Thorne)...

	m_pSlopeGrid	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);
	m_pAspectGrid	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{		
		for(x=0; x<Get_NX(); x++)
		{
			double	slope, aspect;

			if( m_pDEM->Get_Gradient(x, y, slope, aspect) )
			{
				m_pSlopeGrid	->Set_Value(x, y, slope);
				m_pAspectGrid	->Set_Value(x, y, aspect);
			}
			else
			{
				m_pSlopeGrid	->Set_NoData(x, y);
				m_pAspectGrid	->Set_NoData(x, y);
			}
		}
	}


	//-----------------------------------------------------
	m_pTimeGrid->Assign((double)0);

	return true;

}//method

void CSimulate::CalculateFire(){
	
	int x,y;

	Process_Set_Text(_TL("Simulating..."));
	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

	m_CentralPoints.Clear();
	for(y=0; y<Get_NY(); y++){
		for(x=0; x<Get_NX(); x++){
			if (!m_pIgnGrid->is_NoData(x,y)){
				m_CentralPoints.Add(x,y);
				m_pTimeGrid->Set_Value(x,y,0.0);				
			}//if
		}//for
	}//for
	CalculateFireSpreading(NO_TIME_LIMIT);

	m_pTimeGrid->Set_NoData_Value(0.);


}//method


int CSimulate::CalculateFireSpreading(float fTimeLimit){

	int x,y;
	int x2,y2;
	int n;
	bool bReturn = false;
	/* neighbor's address*/   /* N  NE   E  SE   S  SW   W  NW */
	static int nX[8] =        {  0,  1,  1,  1,  0, -1, -1, -1};
    static int nY[8] =        {  1,  1,  0, -1, -1, -1,  0,  1};
	double nDist[8];          /* distance to each neighbor */
    double nAzm[8];           /* compass azimuth to each neighbor (0=N) */	
	size_t modelNumber;       /* fuel model number at current cell */
    double moisture[6];       /* fuel moisture content at current cell */
    double dSpreadRate;       /* spread rate in direction of neighbor */
    double dSpreadTime;       /* time to spread from cell to neighbor */
    double dIgnTime;          /* time neighbor is ignited by current cell */
	double dWindSpd;
	int iBurntCells = 0;

	bool bUpdate = Parameters("UPDATEVIEW")->asBool();

    for (n=0; n<8; n++){
        nDist[n] = sqrt ( nX[n] * m_pDEM->Get_Cellsize() * nX[n] * m_pDEM->Get_Cellsize()
                        + nY[n] * m_pDEM->Get_Cellsize() * nY[n] * m_pDEM->Get_Cellsize() );
        nAzm[n] = n * 45.;
    }//for

	while (m_CentralPoints.Get_Count()!=0){

		for (int iPt=0; iPt<m_CentralPoints.Get_Count();iPt++){

			x = m_CentralPoints.Get_X(iPt);
			y = m_CentralPoints.Get_Y(iPt);

			if (!m_pDEM->is_NoData(x,y) && !m_pFuelGrid->is_NoData(x,y)){

				modelNumber = (size_t) m_pFuelGrid->asInt(x, y);
				moisture[0] = m_pM1Grid->asFloat(x, y);
				moisture[1] = m_pM10Grid->asFloat(x, y);
				moisture[2] = m_pM100Grid->asFloat(x, y);
				moisture[3] = m_pM100Grid->asFloat(x, y);
				moisture[4] = m_pMHerbGrid->asFloat(x, y);;
				moisture[5] = m_pMWoodGrid->asFloat(x, y);
				dWindSpd = m_pWindSpdGrid->asFloat(x,y)  * MS2FTMIN; 
				Fire_SpreadNoWindNoSlope(m_Catalog, modelNumber, moisture);
				Fire_SpreadWindSlopeMax(m_Catalog, modelNumber, dWindSpd,
										 m_pWindDirGrid->asFloat(x,y), tan(m_pSlopeGrid->asFloat(x,y)),
										 m_pAspectGrid->asFloat(x,y, true));

				for (n=0; n<8; n++){
					x2 = x + nX[n];
					y2 = y + nY[n];
					if (m_pTimeGrid->is_InGrid(x2,y2,false)){
						Fire_SpreadAtAzimuth(m_Catalog, modelNumber, nAzm[n], FIRE_BYRAMS);
						dSpreadRate = Fuel_SpreadAny(m_Catalog, modelNumber); // in ft/min (awkward...)					
						dSpreadRate *= FTMIN2MMIN; //a bit better...
						if (dSpreadRate > Smidgen){
							dSpreadTime = nDist[n] / dSpreadRate;							
							if (fTimeLimit == NO_TIME_LIMIT){
								dIgnTime = 	m_pTimeGrid->asDouble(x,y) + dSpreadTime;
								if (m_pTimeGrid->asDouble(x2,y2) == 0.0 
										|| m_pTimeGrid->asDouble(x2, y2) > dIgnTime + THRESHOLD_FOR_DIFFERENCE ){
									m_pTimeGrid->Set_Value(x2, y2, dIgnTime);
									m_AdjPoints.Add(x2,y2);
									Fire_FlameScorch(m_Catalog, modelNumber, FIRE_FLAME);
									m_pFlameGrid->Set_Value(x2, y2, Fuel_FlameLength(m_Catalog, modelNumber) * FT2M);									
									m_pIntensityGrid->Set_Value(x2, y2, Fuel_ByramsIntensity(m_Catalog, modelNumber)
																* BTU2KCAL / FT2M );									
								}//if
							}//if
						}//if					
					}//if
				}//for
			}//if
		}//for

		m_CentralPoints.Clear();
		for (int i=0; i<m_AdjPoints.Get_Count(); i++){
			x= m_AdjPoints.Get_X(i);
			y = m_AdjPoints.Get_Y(i);
			m_CentralPoints.Add(x, y);
		}//for
		m_AdjPoints.Clear();

		if (fTimeLimit == NO_TIME_LIMIT){
			Process_Get_Okay(true);
		}//if	
		
		if (bUpdate){
			DataObject_Update(m_pTimeGrid, true);
		}

	}//while

	return iBurntCells;

}//method

