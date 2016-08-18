/**********************************************************
 * Version $Id: FlowDepth.cpp 1016 2011-04-27 18:40:36Z oconrad $
 *********************************************************/
/*******************************************************************************
    FlowDepth.cpp
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

#include "FlowDepth.h"
#include "Helper.h"

#define NO_DEPTH -1

CFlowDepth::CFlowDepth(void){

	Parameters.Set_Name(_TL("Flow Depth"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Flow Depth Calculation \r\n"
		"References:\r\n 1. Olaya, V. Hidrologia computacional y modelos digitales del terreno. Alqua. 536 pp. 2004"));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation Grid"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"FLOWDEPTH", 
						_TL("Flow Depth Grid (m)"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Float);

	Parameters.Add_Value(NULL,
						"THRESHOLD",
						_TL("Channel definition threshold"),
						_TL("Channel definition threshold"),
						PARAMETER_TYPE_Double,
						10000.);

	Parameters.Add_Value(NULL,
						"FLOW",
						_TL("Peak Flow (m3/s)"),
						_TL("Peak Flow (m3/s) at selected outlet cell"),
						PARAMETER_TYPE_Double,
						100.);

}//constructor


CFlowDepth::~CFlowDepth(void){
}

bool CFlowDepth::On_Execute(void){
		
	m_pDEM			= Parameters("DEM")			->asGrid(); 
	m_pFlowDepth	= Parameters("FLOWDEPTH")	->asGrid();
	m_dThreshold	= Parameters("THRESHOLD")	->asDouble();
	m_dFlow			= Parameters("FLOW")		->asDouble();

	m_pCatchArea	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Float);
	m_pBasinGrid	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Int);
	m_pSlope		= SG_Create_Grid(m_pDEM, SG_DATATYPE_Float);
	m_pAspect		= SG_Create_Grid(m_pDEM, SG_DATATYPE_Float);

	m_pFlowDepth->Set_NoData_Value(0.);

	Process_Set_Text(_TL("Calculating Catchment Area..."));
	CalculateFlowAccGrid(m_pCatchArea, m_pDEM);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{		
		for(int x=0; x<Get_NX(); x++)
		{
			double	slope, aspect;

			if( m_pDEM->Get_Gradient(x, y, slope, aspect) )
			{
				m_pSlope	->Set_Value(x, y, slope);
				m_pAspect	->Set_Value(x, y, aspect);
			}
			else
			{
				m_pSlope	->Set_NoData(x, y);
				m_pAspect	->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pFlowDepth, true);

	return true;

}//method

bool CFlowDepth::On_Execute_Finish(){

	delete m_pSlope;
	delete m_pAspect;
	delete m_pBasinGrid;
	delete m_pCatchArea;

	return true;

}//method

bool CFlowDepth::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode){	

	int iX, iY;	
	int iNextX, iNextY;
	int x,y;
	int iOutletX, iOutletY;
	double fArea;
	double fDepth, fPreviousDepth = 0;

	if(	Mode != TOOL_INTERACTIVE_LDOWN || !Get_Grid_Pos(iOutletX, iOutletY) )
	{
		return( false );
	}

	m_pFlowDepth->Assign((double)0);

	fArea = m_pCatchArea->asFloat(iOutletX, iOutletY);
	
	if (fArea < m_dThreshold * 2.){ //check outlet point
		iNextX = iOutletX;
		iNextY = iOutletY;
		do{
			iOutletX = iNextX;
			iOutletY = iNextY;
			getNextCell(m_pDEM, iOutletX, iOutletY, iNextX, iNextY);
		}while (m_pCatchArea->asFloat(iOutletX, iOutletY) < m_dThreshold * 2. &&
				(iOutletX != iNextX || iOutletY != iNextY));
			
		if (m_pCatchArea->asFloat(iOutletX, iOutletY) < m_dThreshold * 2.){
			Message_Add(_TL("** Error : Wrong outlet point selected **"));
			return false;
		}//if
		Message_Add(_TL("** Warning : Outlet point was modified **"));
    }//if

	CalculateBasinGrid(m_pBasinGrid, m_pDEM, iOutletX, iOutletY);

	m_fMaxFlowAcc = m_pCatchArea->asFloat(iOutletX, iOutletY);		
    
    for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			if (m_pCatchArea->asFloat(x,y) > m_dThreshold){
				if (isHeader(x,y)){					
					iNextX = x;
					iNextY = y;
					do {
						iX = iNextX;
						iY = iNextY;
						if (m_pFlowDepth->asFloat(iX,iY) == 0. && m_pBasinGrid->asInt(iX, iY) != 0){
							getNextCell(m_pDEM, iX, iY, iNextX, iNextY);
							fDepth = CalculateFlowDepth(iX,iY);
							if (fDepth == NO_DEPTH){
								m_pFlowDepth->Set_Value(iX,iY, fPreviousDepth);
							}//if
							else{
								fPreviousDepth = fDepth;
							}//else
						}//if
					}while ((iX != iOutletX || iY != iOutletY)
							&& (iX != iNextX || iY != iNextY));
				}//if
			}//if
		}//for
	}// for

	DataObject_Update(m_pFlowDepth);

	return true;

}//method

bool CFlowDepth::isHeader(int iX, int iY){

    int iNextX, iNextY;
	
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
			if (m_pCatchArea->is_InGrid(iX+i,iY+j)){
				if (m_pCatchArea->asFloat(iX+i,iY+j) > m_dThreshold && (i!=0 || j!=0)){
					getNextCell(m_pDEM, iX + i, iY + j, iNextX, iNextY);
					if (iNextX == iX && iNextY == iY) {
						return false;
					}//if
				}// if
			}//if
        }// for
    }// for

    return true;
		
}// method

double CFlowDepth::CalculateFlowDepth(int iX, int iY){

	int iIter=0;
	double fSup, fInf;
	double fH;
	double fDif;
	double fArea;
	double fPerim;
	double fFlow = m_dFlow / m_fMaxFlowAcc * m_pCatchArea->asFloat(iX,iY);
	double fSlope = tan(m_pSlope->asFloat(iX,iY));
	bool bReturn = false;
		
	fSup = 100;
	fInf = 0;
	fH = 1;
	
	while (!bReturn  && fH >= 0.00001){
		bReturn = getWetAreaAndPerimeter(iX, iY, fH, fArea, fPerim);
		fH /= 2.0;
	}		
	
	if (bReturn){
		fDif = (sqrt(fSlope) * pow(fArea, 5.0 / 3.0)
				/ pow(fPerim, 2.0 / 3.0) / 0.035) - fFlow;
		do {
			iIter++;
			if (fDif > 0) {
				fSup = fH;
				fH = (fInf + fH) / 2.0;
			}// if
			else if (fDif < 0) {
				fInf = fH;
				fH = (fSup + fH) / 2.0;
			}// else if
			bReturn = getWetAreaAndPerimeter(iX, iY, fH, fArea, fPerim);		
			if (!bReturn){
				return NO_DEPTH;
			}//if
			if (iIter>20){
				return NO_DEPTH;
			}//if
			fDif = (sqrt(fSlope) * pow(fArea, 5.0 / 3.0)
				/ pow(fPerim, 2.0 / 3.0) / 0.035) - fFlow;
		}while (fabs(fDif) > 0.1); 
	}//if
	else{
		return NO_DEPTH;
	}//else
	
	m_pFlowDepth->Set_Value(iX,iY,fH);

	return fH;

}//method

bool CFlowDepth::getWetAreaAndPerimeter(int iX, 
									 int iY, 
									 double fH, 
									 double &fArea, 
									 double &fPerim){

	int iWidth = 0;
	int iX2, iY2;
	int iX3, iY3;
	int i = 0;	
	int iStepX, iStepY;
	int pStepX[4] = {1,1,0,1};
	int pStepY[4] = {0,-1,1,1};
	int iDir = (m_pAspect->asInt(iX, iY) / 45) % 4;
	double fDist;
	double fLocalDist;
	double fBaseHeight = m_pDEM->asFloat(iX, iY);	
	double fPerimLeft = 0;
	double fAreaLeft = 0;
	double fPerimRight = 0;
	double fAreaRight = 0;
	double fHeightDif;
	double fLocalHeightDif;
	
	iStepY = pStepX[iDir]; 
	iStepX = pStepY[iDir]; 
	
	fDist = M_GET_LENGTH(iStepX, iStepY) * m_pDEM->Get_Cellsize();
	
	do{
		iX2 = iX + iStepX * i;
		iY2 = iY + iStepY * i;
		iX3 = iX + iStepX * (i+1);
		iY3 = iY + iStepY * (i+1);	
		if (m_pDEM->is_InGrid(iX2,iY2) && m_pDEM->is_InGrid(iX3,iY3)){
			fHeightDif = m_pDEM->asFloat(iX3,iY3) - m_pDEM->asFloat(iX,iY);
			if (fHeightDif >= fH){
				fLocalHeightDif = fabs(m_pDEM->asFloat(iX,iY) + fH - m_pDEM->asFloat(iX2,iY2));
				fLocalDist = fabs(fDist * fLocalHeightDif 
							/ (m_pDEM->asFloat(iX3,iY3) - m_pDEM->asFloat(iX2,iY2)));
			}//if
			else{
				fLocalHeightDif = fabs(m_pDEM->asFloat(iX3,iY3) - m_pDEM->asFloat(iX2,iY2));
				fLocalDist = fDist;
			}//else
			fPerimLeft += sqrt(fLocalDist * fLocalDist + fLocalHeightDif * fLocalHeightDif);
			fAreaLeft += (i * fLocalHeightDif + fLocalHeightDif * fLocalDist / 2.);
		}//if
		else{
			return false;
		}//else
		i++;
	}while(fHeightDif < fH);
	
	i = 0;
	do{
		iX2 = iX - iStepX * i;
		iY2 = iY - iStepY * i;
		iX3 = iX - iStepX * (i+1);
		iY3 = iY - iStepY * (i+1);	
		if (m_pDEM->is_InGrid(iX2,iY2) && m_pDEM->is_InGrid(iX3,iY3)){
			fHeightDif = m_pDEM->asFloat(iX3,iY3) - m_pDEM->asFloat(iX,iY);
			if (fHeightDif >= fH){
				fLocalHeightDif = fabs(m_pDEM->asFloat(iX,iY) + fH - m_pDEM->asFloat(iX2,iY2));
				fLocalDist = fabs(fDist * fLocalHeightDif 
							/ (m_pDEM->asFloat(iX3,iY3) - m_pDEM->asFloat(iX2,iY2)));
			}//if
			else{
				fLocalHeightDif = fabs(m_pDEM->asFloat(iX3,iY3) - m_pDEM->asFloat(iX2,iY2));
				fLocalDist = fDist;
			}//else
			fPerimLeft += sqrt(fLocalDist * fLocalDist + fLocalHeightDif * fLocalHeightDif);
			fAreaLeft += (i * fLocalHeightDif + fLocalHeightDif * fLocalDist / 2.);
		}//if
		else{
			return false;
		}//else
		i++;
	}while(fHeightDif < fH);
	
	fArea = fAreaLeft + fAreaRight;
	fPerim = fPerimLeft +fPerimRight; 

	return true;

}//method



