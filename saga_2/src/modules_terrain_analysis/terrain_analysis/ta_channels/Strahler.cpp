/*******************************************************************************
    Strahler.cpp
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

#include "Strahler.h"
#include "Helper.h"

CStrahler::CStrahler(void){

	Set_Name("Strahler Order");
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Strahler Order Calculation"));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation Grid"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"STRAHLER", 
						_TL("Strahler Order"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Int);

}//constructor


CStrahler::~CStrahler(void)
{}


/*
void getNextCell(
		CSG_Grid *g,
		int iX,
        int iY,
		int &iNextX,
		int &iNextY) {

    float fDist;
    float fMaxSlope;
    float fSlope;

    fMaxSlope = 0;
    fSlope = 0;

    if (iX < 1 || iX >= g->Get_NX() - 1 || iY < 1 || iY >= g->Get_NY() - 1
            || g->asFloat(iX,iY) == g->Get_NoData_Value()) {
        iNextX = iX;
		iNextY = iY;
		return;
    }// if

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {                	
            if (g->asFloat(iX+i,iY+j) != g->Get_NoData_Value()){
                if (i == 0 || j == 0) {
                    fDist = 1.0f;
                }// if
                else {
                    fDist = 1.44f;
                }// else
                fSlope = (g->asFloat(iX+i,iY+j)
                         - g->asFloat(iX,iY)) / fDist;                                				
                if (fSlope <= fMaxSlope) {
                    iNextX = iX+i;
					iNextY = iY+j;                        
                    fMaxSlope = fSlope;
                }// if
            }//if                    
        }// for
    }// for

}// method
*/

int CStrahler::getStrahlerOrder(
		int iX,
		int iY){
		
	int iMaxOrder=1;
	int iNextX, iNextY;
	int iOrder = 1;
	int iMaxOrderCells=0;
	
	
	if (m_pStrahler->asInt(iX,iY) ==0 ){;
		for (int i = -1; i<2; i++){
			for (int j = -1; j<2; j++){
				if (!(i == 0) || !(j == 0)) {
					getNextCell(m_pDEM, iX + i, iY + j, iNextX, iNextY);
					if (iNextY == iY && iNextX == iX) {
						iOrder=m_pStrahler->asInt(iX+i,iY+j);
						if (iOrder==0){
							iOrder=getStrahlerOrder(iX+i,iY+j);
						}//if
						if (iOrder>iMaxOrder){
							iMaxOrder=iOrder;
							iMaxOrderCells=1;
						}//if
						else if (iOrder==iMaxOrder){
							iMaxOrderCells++;
						}//if
					}// if				
				}//if				
			}//for
		}//for
		
		if (iMaxOrderCells>1){
			iMaxOrder++;
		}//if
		
		m_pStrahler->Set_Value(iX,iY,iMaxOrder);
	
	}//if

	return iMaxOrder;
	
}//function

bool CStrahler::On_Execute(void){
	
	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pStrahler = Parameters("STRAHLER")->asGrid();
	m_pStrahler->Assign(0.0);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
            getStrahlerOrder(x,y);
        }// for
    }// for

	return( true );

}//method
