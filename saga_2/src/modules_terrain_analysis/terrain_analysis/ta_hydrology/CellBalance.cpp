/*******************************************************************************
    CellBalance.cpp
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

#include "CellBalance.h"
#include "Helper.h"

CCellBalance::CCellBalance(void){

	Parameters.Set_Name(_TL("Cell Balance"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Cell Balance Calculation\r\n"
		"References:\r\n 1. Olaya, V. Hidrologia computacional y modelos digitales del terreno. Alqua. 536 pp. 2004"));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation Grid"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"PARAM", 
						_TL("Parameter"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"BALANCE", 
						_TL("Cell Balance"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

}//constructor


CCellBalance::~CCellBalance(void)
{}


double CCellBalance::getCellBalance(
		int iX,
		int iY){
		
	int iNextX, iNextY;
	double dIn=0, dOut=0, dBalance;
	
	for (int i = -1; i<2; i++){
		for (int j = -1; j<2; j++){
			if (!(i == 0) || !(j == 0)) {
				getNextCell(m_pDEM, iX + i, iY + j, iNextX, iNextY);
                if (iNextY == iY && iNextX == iX) {
                    dIn+=m_pParameter->asDouble(iX+i,iY+j);                                        
				}// if				
			}//if				
		}//for
	}//for
	
	dOut = m_pParameter->asDouble(iX,iY);
	dBalance = dIn-dOut;
	
	return dBalance;
	
}//function

bool CCellBalance::On_Execute(void){
	
	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pParameter = Parameters("PARAM")->asGrid(); 
	m_pCellBalance = Parameters("BALANCE")->asGrid();
	m_pCellBalance->Assign((double)0);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
            m_pCellBalance->Set_Value(x,y,getCellBalance(x,y));
        }// for
    }// for

	return( true );

}//method
