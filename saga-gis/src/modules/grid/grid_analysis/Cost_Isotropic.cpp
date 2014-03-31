/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Cost_Isotropic.cpp
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

#include "Cost_Isotropic.h"

#define NO_DATA -1.

CCost_Isotropic::CCost_Isotropic(void)
{	 
	
	Set_Name		(_TL("Accumulated Cost (Isotropic)"));
	Set_Author		(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Calculate Accumulated Cost (Isotropic)"));

	Parameters.Add_Grid(NULL, 
						"COST", 
						_TL("Cost Grid"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"POINTS",
						_TL("Destination Points"), 
						_TL(""),
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"ACCCOST", 
						_TL("Accumulated Cost"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);

	Parameters.Add_Grid(NULL, 
						"CLOSESTPT", 
						_TL("Closest Point"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Int);

	Parameters.Add_Value(NULL,
						"THRESHOLD",
						_TL("Threshold for different route"),
						_TL(""),
						PARAMETER_TYPE_Double,
						0.);
}

CCost_Isotropic::~CCost_Isotropic(void)
{}


bool CCost_Isotropic::On_Execute(void)
{
	
	int iPoint = 1;

	m_dThreshold = Parameters("THRESHOLD")->asDouble();

	m_pAccCostGrid = Parameters("ACCCOST")->asGrid(); 
	m_pCostGrid = Parameters("COST")->asGrid(); 
	m_pClosestPtGrid = Parameters("CLOSESTPT")->asGrid(); 
	m_pPointsGrid = Parameters("POINTS")->asGrid(); 
	
	m_pAccCostGrid->Assign(NO_DATA);
	m_pClosestPtGrid->Assign(NO_DATA);

	m_pAccCostGrid->Set_NoData_Value(NO_DATA);
	m_pClosestPtGrid->Set_NoData_Value(NO_DATA);

	m_CentralPoints.Clear();
	m_AdjPoints.Clear();

	m_CentralPoints.Clear();				

	for(int y=0; y<Get_NY(); y++){		
		for(int x=0; x<Get_NX(); x++){
			if (!m_pPointsGrid->is_NoData(x,y)){				
				m_CentralPoints.Add(x,y,iPoint);
				m_pAccCostGrid->Set_Value(x,y,0.0);
				m_pClosestPtGrid->Set_Value(x,y,iPoint);
				iPoint++;
			}//if
		}//for
	}//for

	CalculateCost();

	return true;

}//method

void CCost_Isotropic::CalculateCost(){
    
    double dCost=0;
	int iPoint;
	int iX,iY;
	int iUsedCells=0;
	int iTotalCells=Get_NX()*Get_NY();
	
	while (m_CentralPoints.Get_Count()!=0){
        
		for (int iPt=0; iPt<m_CentralPoints.Get_Count();iPt++){	        
	    
			iX=m_CentralPoints.Get_X(iPt);
			iY=m_CentralPoints.Get_Y(iPt);
			iPoint = m_CentralPoints.Get_ClosestPoint(iPt);

			for (int i = -1; i < 2; i++) {        
				for (int j = -1; j < 2; j++) { 
					if (m_pCostGrid->is_InGrid(iX+i,iY+j)){							     			            		
						if (m_pCostGrid->is_NoData(iX,iY) || m_pCostGrid->is_NoData(iX+i,iY+j)){				            							            			
						}//if
						else{
							double dPartialCost=(m_pCostGrid->asDouble(iX,iY)+m_pCostGrid->asDouble(iX+i,iY+j))/2.0;
							dCost = m_pAccCostGrid->asDouble(iX,iY) + dPartialCost * M_GET_LENGTH(i, j);					
							if (m_pAccCostGrid->asDouble(iX+i,iY+j)==NO_DATA || 
									m_pAccCostGrid->asDouble(iX+i,iY+j)>dCost + m_dThreshold){								
								m_pAccCostGrid->Set_Value(iX+i,iY+j,dCost);
								m_pClosestPtGrid->Set_Value(iX+i,iY+j,iPoint);
								m_AdjPoints.Add(iX+i,iY+j,iPoint);	
								iUsedCells++;								
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
			iPoint = m_AdjPoints.Get_ClosestPoint(i);
			m_CentralPoints.Add(iX,iY, iPoint);
		}//for
		m_AdjPoints.Clear();
		
		//DataObject_Update(m_pAccCostGrid, true); 

		Set_Progress((double)iUsedCells,(double)iTotalCells);

	}//while


	
}//method


