/*******************************************************************************
    Forecasting.cpp
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

#include "../../../modules/grid/grid_tools/Grid_Gaps.h"

#include "Forecasting.h"

#include <time.h>

#define MS2FTMIN (60.0 / 0.3048)
#define FTMIN2MMIN 0.3048
#define MIN_RATIO_BURNT_AREA 2.

CForecasting::CForecasting(void){

	Parameters.Set_Name(_TL("Fire Risk Analysis"));
	Parameters.Set_Description(_TL(
		"(c) 2004 Victor Olaya. Fire Risk Analysis"));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("DEM"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"FUEL", 
						_TL("Fuel Model"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"WINDSPD", 
						_TL("Wind Speed"),
						_TL("Wind Speed (m/s)"), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"WINDDIR", 
						_TL("Wind Direction"), 
						_TL("Wind Direction (degrees clockwise from north)"),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M1H", 
						_TL("Dead Fuel Moisture 1H"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M10H", 
						_TL("Dead Fuel Moisture 10H"),
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M100H", 
						_TL("Dead Fuel Moisture 100H"),
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MHERB", 
						_TL("Herbaceous Fuel Moisture"),
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MWOOD", 
						_TL("Wood Fuel Moisture"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"VALUE", 
						_TL("Value"), 
						"", 
						PARAMETER_INPUT_OPTIONAL);
	
	Parameters.Add_Grid(NULL, 
						"BASEPROB", 
						_TL("Base Probability"), 
						"", 
						PARAMETER_INPUT_OPTIONAL);	
	
	Parameters.Add_Grid(NULL, 
						"DANGER", 
						_TL("Danger"), 
						"", 
						PARAMETER_OUTPUT);
	
	Parameters.Add_Grid(NULL, 
						"COMPPROB", 
						_TL("Compound Probability"), 
						"", 
						PARAMETER_OUTPUT);

	Parameters.Add_Grid(NULL, 
						"PRIORITY", 
						_TL("Priority Index"), 
						"", 
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL,
						"MONTECARLO",
						_TL("Number of Events"),
						_TL("Número of Monte-Carlo events"),
						PARAMETER_TYPE_Int,
						1000,
						1.,
						true);

	Parameters.Add_Value(NULL,
						"INTERVAL",
						_TL("Fire Length"),
						_TL("Fire Length (min)"),
						PARAMETER_TYPE_Double,
						100,
						1.,
						true);


}//constructor

CForecasting::~CForecasting(void)
{
}//destructor

bool CForecasting::On_Execute(void){

	AssignParameters();
	CalculateGrids();

	delete m_pSlopeGrid;
	delete m_pAspectGrid;

	return true;

}//method

bool CForecasting::AssignParameters(){

	int x,y;

	m_pDEM = Parameters("DEM")->asGrid();
	m_pFuelGrid = Parameters("FUEL")->asGrid();
	m_pWindDirGrid = Parameters("WINDDIR")->asGrid();
	m_pWindSpdGrid = Parameters("WINDSPD")->asGrid();
	m_pM1Grid = Parameters("M1H")->asGrid();
	m_pM10Grid = Parameters("M10H")->asGrid();
	m_pM100Grid = Parameters("M100H")->asGrid();
	m_pMHerbGrid = Parameters("MHERB")->asGrid();
	m_pMWoodGrid = Parameters("MWOOD")->asGrid();
	m_pDangerGrid = Parameters("DANGER")->asGrid();
	m_pValueGrid = Parameters("VALUE")->asGrid();
	m_pBaseProbabilityGrid = Parameters("BASEPROB")->asGrid();
	m_pCompoundProbabilityGrid = Parameters("COMPPROB")->asGrid();
	m_pPriorityIndexGrid = Parameters("PRIORITY")->asGrid();
	m_iInterval = Parameters("INTERVAL")->asInt();
	m_iNumEvents = Parameters("MONTECARLO")->asInt();

	/* create a standard fuel model catalog and a flame length table. */
    m_Catalog = Fire_FuelCatalogCreateStandard("Standard", 13);
    Fire_FlameLengthTable(m_Catalog, 500, 0.1);

	if (!m_pBaseProbabilityGrid){
		m_pBaseProbabilityGrid = API_Create_Grid(m_pDEM, GRID_TYPE_Double);
		m_pBaseProbabilityGrid->Assign(1);
	}//if
	if (!m_pValueGrid){
		m_pValueGrid = API_Create_Grid(m_pDEM, GRID_TYPE_Double);
		m_pValueGrid->Assign(1);
	}//if

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
			if (m_pBaseProbabilityGrid->is_NoData(x, y)){
				m_pBaseProbabilityGrid->Set_Value(x, y, 0.);
			}//if

		}//for
	}//for


	//-----------------------------------------------------
	// calculate slope and aspect using CGrid class'
	// built-in function (after Zevenbergen & Thorne)...

	m_pSlopeGrid	= API_Create_Grid(m_pDEM, GRID_TYPE_Double);
	m_pAspectGrid	= API_Create_Grid(m_pDEM, GRID_TYPE_Double);

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
	m_pTimeGrid = API_Create_Grid(m_pDEM, GRID_TYPE_Double);

	m_pTimeGrid->Assign((double)0);
	m_pDangerGrid->Assign((double)0);
	m_pCompoundProbabilityGrid->Assign((double)0);	
	
	//m_pPriorityIndexGrid = Get_SafeNew_Grid(m_pDEM, GRID_TYPE_Double);

	return true;

}//method


void CForecasting::CalculateGrids(){
	
	int x,y;
	int i;
	int iRecommendedNumFires;
	double dDanger;
	double dTotalBurntArea = 0;
	CAPI_String sMessage;

	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

	srand(time(NULL));

	Process_Set_Text(_TL("Calculating danger..."));
	for(i=0; i<m_iNumEvents && Set_Progress(i, m_iNumEvents); i++){
		x = rand() % (m_pDEM->Get_NX()-1);
		y = rand() % (m_pDEM->Get_NY()-1);
		m_CentralPoints.Clear();
		m_CentralPoints.Add(x,y);
		m_pTimeGrid->Set_Value(x,y,0.0);	
		dDanger = CalculateFireSpreading();
		dTotalBurntArea += dDanger;
		m_pDangerGrid->Set_Value(x, y, dDanger);		
	}//for

	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

	m_pDangerGrid->Set_NoData_Value(0.0);
	m_pDangerGrid->Set_Unit("m2/h");
	
	Process_Set_Text(_TL("Closing Gaps..."));

	CGrid_Gaps pGaps;
	if(	!pGaps.Get_Parameters()->Set_Parameter("INPUT", PARAMETER_TYPE_Grid, m_pDangerGrid)
	||	!pGaps.Execute() ){
		return;
	}//if

	for (y=0; y<Get_NY(); y++){
		for (x=0; x<Get_NX(); x++){
			m_pCompoundProbabilityGrid->Set_Value(x,y, 
				m_pCompoundProbabilityGrid->asFloat(x,y) / (float)m_iNumEvents);
			m_pPriorityIndexGrid->Set_Value(x, y, m_pCompoundProbabilityGrid->asFloat(x,y)*
				m_pDangerGrid->asFloat(x,y));
		}//for
	}//for

	float dRatio = (float)(dTotalBurntArea / (m_pDEM->Get_Cellsize() * m_pDEM->Get_Cellsize()));
	if (dRatio < MIN_RATIO_BURNT_AREA){
		iRecommendedNumFires =(int)((float) m_iNumEvents / dRatio * (float) MIN_RATIO_BURNT_AREA);
		sMessage = _TL("** Warning : Number of events might not be representative.\n"
					"Minimum number recommended: ") + API_Get_String(iRecommendedNumFires, 0) + "\n";
		Message_Add(sMessage.c_str());
	}//if

}//method

double CForecasting::CalculateFireSpreading(){

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
	double dBurntValue = 0;
	double dProbability;

	m_pTimeGrid->Assign(0.0);
	
    for (n=0; n<8; n++){
        nDist[n] = sqrt ( nX[n] * m_pDEM->Get_Cellsize() * nX[n] * m_pDEM->Get_Cellsize()
                        + nY[n] * m_pDEM->Get_Cellsize() * nY[n] * m_pDEM->Get_Cellsize() );
        nAzm[n] = n * 45.;
    }//for

	x = m_CentralPoints.Get_X(0);
	y = m_CentralPoints.Get_Y(0);
	dProbability = (float)(rand()) / (float)(RAND_MAX); 

	if (m_pBaseProbabilityGrid->asFloat(x,y) < dProbability){
		return 0;
	}//if

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
						Fire_SpreadAtAzimuth(m_Catalog, modelNumber, nAzm[n], FIRE_NONE);
						dSpreadRate = Fuel_SpreadAny(m_Catalog, modelNumber); // in ft/min (awkward...)					
						dSpreadRate *= FTMIN2MMIN; //a bit better...
						if (dSpreadRate > Smidgen){
							dSpreadTime = nDist[n] / dSpreadRate;
							dIgnTime = 	m_pTimeGrid->asDouble(x,y) + dSpreadTime;												
							if (dIgnTime < m_iInterval){
								if (m_pTimeGrid->asDouble(x2,y2) == 0.0 
										|| m_pTimeGrid->asDouble(x2, y2)>dIgnTime){
									if (m_pTimeGrid->asDouble(x2, y2)==0.0){
										dBurntValue += m_pValueGrid->asDouble(x2, y2);
										m_pCompoundProbabilityGrid->Set_Value(x2,y2,
											m_pCompoundProbabilityGrid->asFloat(x2,y2) + 1);
									}//if
									m_pTimeGrid->Set_Value(x2, y2, dIgnTime);
									m_AdjPoints.Add(x2,y2);									
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

	}//while

	return dBurntValue;

}//method
