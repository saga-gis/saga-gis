/**********************************************************
 * Version $Id: Forecasting.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

//#include "../../../modules/grid/grid_tools/Grid_Gaps.h"

#include "Forecasting.h"

#include <time.h>

#define MS2FTMIN (60.0 / 0.3048)
#define FTMIN2MMIN 0.3048
#define MIN_RATIO_BURNT_AREA 2.

CForecasting::CForecasting(void){

	Set_Name		(_TL("Fire Risk Analysis"));
	Set_Author		(SG_T("(c) 2004 Victor Olaya"));
	Set_Description	(_TW(
		"Fire risk analysis based on the BEHAVE fire modeling system "
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
						"VALUE", 
						_TL("Value"), 
						_TL(""), 
						PARAMETER_INPUT_OPTIONAL);
	
	Parameters.Add_Grid(NULL, 
						"BASEPROB", 
						_TL("Base Probability"), 
						_TL(""), 
						PARAMETER_INPUT_OPTIONAL);	
	
	Parameters.Add_Grid(NULL, 
						"DANGER", 
						_TL("Danger"), 
						_TL(""), 
						PARAMETER_OUTPUT);
	
	Parameters.Add_Grid(NULL, 
						"COMPPROB", 
						_TL("Compound Probability"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Grid(NULL, 
						"PRIORITY", 
						_TL("Priority Index"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL,
						"MONTECARLO",
						_TL("Number of Events"),
						_TL("N\xc3\xbamero of Monte-Carlo events"),
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
		m_pBaseProbabilityGrid = SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);
		m_pBaseProbabilityGrid->Assign(1);
	}//if
	if (!m_pValueGrid){
		m_pValueGrid = SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);
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
	m_pTimeGrid = SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);

	m_pTimeGrid->Assign((double)0);
	m_pDangerGrid->Assign((double)0);
	m_pCompoundProbabilityGrid->Assign((double)0);	
	
	//m_pPriorityIndexGrid = Get_SafeNew_Grid(m_pDEM, SG_DATATYPE_Double);

	return true;

}//method


void CForecasting::CalculateGrids(){
	
	int x,y;
	int i;
	int iRecommendedNumFires;
	double dDanger;
	double dTotalBurntArea = 0;
	CSG_String sMessage;

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
	m_pDangerGrid->Set_Unit(_TL("m2/h"));
	
	Process_Set_Text(_TL("Closing Gaps..."));

	if(	!Gaps_Close(m_pDangerGrid) )
	{
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
		sMessage.Printf(
			_TL("** Warning : Number of events might not be representative.\nMinimum number recommended: ")
		);
		sMessage	+= SG_Get_String(iRecommendedNumFires, 0);
		sMessage	+= SG_T("\n");
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CForecasting::Gaps_Close(CSG_Grid *pInput)
{
	int			iStep, iStart, n;
	double		max, Threshold;
	CSG_Grid	*pResult, *pTension_Keep, *pTension_Temp;

	//-----------------------------------------------------
	pResult		= pInput;
	pInput		= SG_Create_Grid(pInput);
	pInput->Assign(pResult);

	//-----------------------------------------------------
	Threshold	= 0.1;

	n			= Get_NX() > Get_NY() ? Get_NX() : Get_NY();
	iStep		= 0;
	do	{	iStep++;	}	while( pow(2.0, iStep + 1) < n );
	iStart		= (int)pow(2.0, iStep);

	pTension_Keep		= new CSG_Grid(pResult, SG_DATATYPE_Byte);
	pTension_Temp		= new CSG_Grid(pResult);

	pResult->Assign_NoData();

	for(iStep=iStart; iStep>=1; iStep/=2)
	{
		Gaps_Tension_Init(iStep, pTension_Temp, pTension_Keep, pResult, pInput);

		do
		{
			max		= Gaps_Tension_Step(iStep, pTension_Temp, pTension_Keep, pResult);
		}
		while( max > Threshold && Process_Get_Okay(true) );
	}

	//-----------------------------------------------------
	delete(pTension_Keep);
	delete(pTension_Temp);
	delete(pInput);

	return( true );
}

//---------------------------------------------------------
void CForecasting::Gaps_Tension_Init(int iStep, CSG_Grid *pTension_Temp, CSG_Grid *pTension_Keep, CSG_Grid *pResult, CSG_Grid *pInput)
{
	int		x, y, i, ix, iy, nx, ny, nz;
	double	z;

	//-----------------------------------------------------
	// 1. Channels...

	pTension_Temp->Assign_NoData();
	pTension_Keep->Assign();

	for(y=0; y<Get_NY(); y+=iStep)
	{
		ny	= y + iStep < Get_NY() ? y + iStep : Get_NY();

		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( !pInput->is_NoData(x, y) )
			{
				pTension_Temp->Set_Value(x, y, pInput->asDouble(x, y) );
				pTension_Keep->Set_Value(x, y, 1.0);
			}
			else
			{
				nx	= x + iStep < Get_NX() ? x + iStep : Get_NX();
				nz	= 0;
				z	= 0.0;

				for(iy=y; iy<ny; iy++)
				{
					for(ix=x; ix<nx; ix++)
					{
						if( pInput->is_InGrid(ix, iy) )
						{
							z	+= pInput->asDouble(ix, iy);
							nz++;
						}
					}
				}

				if( nz > 0 )
				{
					pTension_Temp->Set_Value(x, y, z / (double)nz );
					pTension_Keep->Set_Value(x, y, 1.0);
				}
			}
		}
	}

	//-----------------------------------------------------
	// 2. Previous Iteration...

	for(y=0; y<Get_NY(); y+=iStep)
	{
		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( pTension_Keep->asByte(x, y) == false )
			{
				if( !pResult->is_NoData(x, y) )
				{
					pTension_Temp->Set_Value(x, y, pResult->asDouble(x, y));
				}
				else
				{
					nz	= 0;
					z	= 0.0;

					for(i=0; i<8; i++)
					{
						ix	= x + iStep * Get_System()->Get_xTo(i);
						iy	= y + iStep * Get_System()->Get_yTo(i);

						if( pResult->is_InGrid(ix, iy) )
						{
							z	+= pResult->asDouble(ix, iy);
							nz++;
						}
					}

					if( nz > 0.0 )
					{
						pTension_Temp->Set_Value(x, y, z / (double)nz);
					}
					else
					{
						pTension_Temp->Set_Value(x, y, pInput->asDouble(x, y));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	// 3. ...

	pResult->Assign(pTension_Temp);
}

//---------------------------------------------------------
double CForecasting::Gaps_Tension_Step(int iStep, CSG_Grid *pTension_Temp, CSG_Grid *pTension_Keep, CSG_Grid *pResult)
{
	int		x, y;
	double	d, dMax;

	dMax	= 0.0;

	for(y=0; y<Get_NY(); y+=iStep)
	{
		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( pTension_Keep->asByte(x, y) == false )
			{
				d	= Gaps_Tension_Change(x, y, iStep, pResult);

				pTension_Temp->Set_Value(x, y, d);

				d	= fabs(d - pResult->asDouble(x, y));

				if( d > dMax )
				{
					dMax	= d;
				}
			}
		}
	}

	for(y=0; y<Get_NY(); y+=iStep)
	{
		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( pTension_Keep->asByte(x, y) == false )
			{
				pResult->Set_Value(x, y, pTension_Temp->asDouble(x, y));
			}
		}
	}

	return( dMax );
}

//---------------------------------------------------------
double CForecasting::Gaps_Tension_Change(int x, int y, int iStep, CSG_Grid *pResult)
{
	int		i, ix, iy;
	double	n, d, dz;

	for(i=0, d=0.0, n=0.0; i<8; i++)
	{
		ix	= x + iStep * Get_System()->Get_xTo(i);
		iy	= y + iStep * Get_System()->Get_yTo(i);

		if( pResult->is_InGrid(ix, iy) )
		{
			dz	= 1.0 / Get_System()->Get_UnitLength(i);
			d	+= dz * pResult->asDouble(ix, iy);
			n	+= dz;
		}
	}

	if( n > 0.0 )
	{
		d	/= n;

		return( d );
	}

	return( pResult->asDouble(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
