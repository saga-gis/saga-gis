/*******************************************************************************
    FilterClumps.cpp
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

#include "FilterClumps.h"

CFilterClumps::CFilterClumps(void){

	Set_Name		(_TL("Filter Clumps"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Filter Clumps"
	));

	Parameters.Add_Grid(NULL, 
						"GRID",
						_TL("Input Grid"), 						
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"OUTPUT", 
						_TL("Filtered Grid"), 
						_TL(""), 
						PARAMETER_OUTPUT);
	
	Parameters.Add_Value(NULL,
						"THRESHOLD",
						_TL("Min. Size"),
						_TL("Min. Size (Cells)"),
						PARAMETER_TYPE_Int,
						10);	
						
}//constructor

CFilterClumps::~CFilterClumps(void)
{}

bool CFilterClumps::On_Execute(void){

	int x,y;
	int iArea;
	m_pInputGrid = Parameters("GRID")->asGrid();
	m_pOutputGrid = Parameters("OUTPUT")->asGrid();
	m_pMaskGrid = SG_Create_Grid(m_pInputGrid);
	m_pMaskGridB = SG_Create_Grid(m_pInputGrid);
	int iThreshold = Parameters("THRESHOLD")->asInt();

	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

	m_pMaskGrid->Assign((double)0);
	m_pMaskGridB->Assign(1);

	for (x = 1; x < Get_NX()-1; x++){
		for (y = 1; y < Get_NY()-1; y++){
			if (!m_pInputGrid->is_NoData(x,y) && m_pMaskGrid->asInt(x,y) == 0){
				m_CentralPoints.Clear();
				m_CentralPoints.Add(x,y);	
				m_pMaskGrid->Set_Value(x,y,1);
				iArea = CalculateCellBlockArea();
				if (iArea < iThreshold){
					m_CentralPoints.Clear();
					m_CentralPoints.Add(x,y);
					m_pMaskGridB->Set_NoData(x,y);
					EliminateClump();
				}//if
			}//if
		}//for
	}//for
	
	for (x = 0; x < Get_NX(); x++){
		for (y = 0; y < Get_NY(); y++){
			if (m_pMaskGridB->is_NoData(x,y)){
				m_pOutputGrid->Set_NoData(x,y);
			}//if
			else{
				m_pOutputGrid->Set_Value(x,y,m_pInputGrid->asDouble(x,y));
			}//else
		}//for
	}//for

	return true;

}//method

int CFilterClumps::CalculateCellBlockArea(){
		
	int iX,iY;	
	int iArea = 1;
	int iClass;

	while (m_CentralPoints.Get_Count()!=0){

		for (int iPt=0; iPt<m_CentralPoints.Get_Count();iPt++){

			iX=m_CentralPoints.Get_X(iPt);
			iY=m_CentralPoints.Get_Y(iPt);
			iClass = m_pInputGrid->asInt(iX,iY);

			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (i!=0 || j!=0){
						if (m_pInputGrid->is_InGrid(iX+i,iY+j)){
							if (!m_pInputGrid->is_NoData(iX,iY)){							
								if (m_pInputGrid->asInt(iX+i,iY+j) == iClass
										&& m_pMaskGrid->asInt(iX+i, iY+j) == 0){
									iArea++;
									m_pMaskGrid->Set_Value(iX+i,iY+j,1);									
									m_AdjPoints.Add(iX+i,iY+j);
								}//if
							}//if
						}//if
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
	
	}//while

	return iArea;

}//method

void CFilterClumps::EliminateClump(){
		
	int iX,iY;	
	int iClass;

	while (m_CentralPoints.Get_Count()!=0){

		for (int iPt=0; iPt<m_CentralPoints.Get_Count();iPt++){

			iX=m_CentralPoints.Get_X(iPt);
			iY=m_CentralPoints.Get_Y(iPt);
			iClass = m_pInputGrid->asInt(iX,iY);

			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (i!=0 || j!=0){
						if (m_pInputGrid->is_InGrid(iX+i,iY+j)){
							if (!m_pInputGrid->is_NoData(iX,iY)){							
								if (m_pInputGrid->asInt(iX+i,iY+j) == iClass
										&& m_pMaskGridB->asInt(iX+i, iY+j) == 1){
									m_pMaskGridB->Set_NoData(iX+i,iY+j);									
									m_AdjPoints.Add(iX+i,iY+j);
								}//if
							}//if
						}//if
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
	
	}//while

}//method
