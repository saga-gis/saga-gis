/*******************************************************************************
    Cost_Anisotropic.cpp
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

#include "Cost_Anisotropic.h"

#define NO_DATA -1.

CCost_Anisotropic::CCost_Anisotropic(void)
{	 

	Set_Name(_TL("Accumulated Cost (Anisotropic)"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Calculate Accumulated Cost (Anisotropic)"));

	Parameters.Add_Grid(NULL, 
						"COST", 
						_TL("Cost Grid"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"DIRECTION", 
						_TL("Direction of max cost"), 
						_TL("Direction of max cost for each cell. In degrees"), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"POINTS",
						_TL("Destination Points"), 
						"",
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"ACCCOST", 
						_TL("Accumulated Cost"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Value(NULL, 
						"K", 
						_TL("k factor"), 
						_TL("Efective friction = stated friction ^f , where f = cos(DifAngle)^k."), 
						PARAMETER_TYPE_Double, 
						2);

	Parameters.Add_Value(NULL,
						"THRESHOLD",
						_TL("Threshold for different route"),
						"",
						PARAMETER_TYPE_Double,
						0.);

}

CCost_Anisotropic::~CCost_Anisotropic(void)
{}


bool CCost_Anisotropic::On_Execute(void)
{
	
	m_pAccCostGrid = Parameters("ACCCOST")->asGrid(); 
	m_pCostGrid = Parameters("COST")->asGrid(); 
	m_pPointsGrid = Parameters("POINTS")->asGrid(); 
	m_pDirectionGrid = Parameters("DIRECTION")->asGrid();
	m_dK = Parameters("K")->asDouble();
	m_dThreshold = Parameters("THRESHOLD")->asDouble();
	
	m_pAccCostGrid->Assign(NO_DATA);

	m_CentralPoints.Clear();
	m_AdjPoints.Clear();				
	
	m_CentralPoints.Clear();

	for(int y=0; y<Get_NY(); y++){		
		for(int x=0; x<Get_NX(); x++){
			if (!m_pPointsGrid->is_NoData(x,y)){
				m_CentralPoints.Add(x,y);
				m_pAccCostGrid->Set_Value(x,y,0.0);
			}//if
		}//for
	}//for
	
	CalculateCost();

	m_pAccCostGrid->Set_NoData_Value(NO_DATA);

	return true;

}//method

void CCost_Anisotropic::CalculateCost(){
    
    double dCost=0;;
	int iX,iY;
	int iUsedCells=0;
	int iTotalCells=Get_NX()*Get_NY();
	
	while (m_CentralPoints.Get_Count()!=0){
        
		for (int iPt=0; iPt<m_CentralPoints.Get_Count();iPt++){	        
	    
			iX=m_CentralPoints.Get_X(iPt);
			iY=m_CentralPoints.Get_Y(iPt);
			
			for (int i = -1; i < 2; i++) {        
				for (int j = -1; j < 2; j++) { 
					if (m_pCostGrid->is_InGrid(iX+i,iY+j)){							     			            		
						if (m_pCostGrid->is_NoData(iX,iY) || m_pCostGrid->is_NoData(iX+i,iY+j)){				            							            			
						}//if
						else{
							double dPartialCost=CalculateCostInDirection(iX,iY,i,j);
							dCost = m_pAccCostGrid->asDouble(iX,iY) + dPartialCost* M_GET_LENGTH(i, j);
							if (m_pAccCostGrid->asDouble(iX+i,iY+j)==NO_DATA){
								iUsedCells++;
								m_pAccCostGrid->Set_Value(iX+i,iY+j,dCost);
								m_AdjPoints.Add(iX+i,iY+j);									
							}//if
							else if (m_pAccCostGrid->asDouble(iX+i,iY+j)> dCost + m_dThreshold){
								m_pAccCostGrid->Set_Value(iX+i,iY+j,dCost);
								m_AdjPoints.Add(iX+i,iY+j);		            			
							}//if
						}//else			            	       		
					}//if           	                
				}// for
			}// for
		}//for


		m_CentralPoints.Clear();
		for (int i=0; i<m_AdjPoints.Get_Count(); i++){
			iX = m_AdjPoints.Get_X(i);
			iY = m_AdjPoints.Get_Y(i);
			m_CentralPoints.Add(iX,iY);
		}//for
		m_AdjPoints.Clear();
		DataObject_Update(m_pAccCostGrid, true);

		Set_Progress((double)iUsedCells,(double)iTotalCells);

	}//while

}//method


double CCost_Anisotropic::CalculateCostInDirection(int iX,
											  int iY,
											  int iH,
											  int iV){

	double dAngles[3][3] = {{315,0,45},{270,0,90},{225,180,135}};
	double PI=3.14159;
	
	double dAngle = dAngles[iV+1][iH+1];

	double dDifAngle1 = fabs(m_pDirectionGrid->asDouble(iX,iY)-dAngle);
	double dDifAngle2 = fabs(m_pDirectionGrid->asDouble(iX+iH,iY+iV)-dAngle);

	dDifAngle1 = dDifAngle1/360.0 * 2.0 * PI;
	dDifAngle2 = dDifAngle2/360.0 * 2.0 * PI;
	
	double dCost1 = pow(cos(dDifAngle1),m_dK)/2;
	double dCost2 = pow(cos(dDifAngle2),m_dK)/2;

	return dCost1+dCost2;

}//method
