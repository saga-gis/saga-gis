/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    BasinGrid.cpp
    Copyright (C) Victor Olaya . Proyecto Sextante (http://sextante.sf.net)
    
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
#include  "BasinGrid.h"
#include "Helper.h"

double AccFlow(CSG_Grid *pGrid, CSG_Grid *pDEM, int iX, int iY){
	
    int iNextX, iNextY;
	double dAccFlow = pGrid->Get_Cellsize()*pGrid->Get_Cellsize();
			
	for (int i = -1; i<2; i++){
		for (int j = -1; j<2; j++){
			if (!(i == 0) || !(j == 0)) {
				getNextCell(pDEM, iX + i, iY + j, iNextX, iNextY);
				if (iNextY == iY && iNextX == iX) {
					if (pGrid->asDouble(iX+i,iY+j)!=0){
						dAccFlow += pGrid->asDouble(iX+i,iY+j);
					}//if
					else{
						dAccFlow += AccFlow (pGrid,pDEM,iX+i,iY+j);
					}//else
				}// if				
			}//if				
		}//for
	}//for

	pGrid->Set_Value(iX,iY,dAccFlow);

	return dAccFlow;

}// function

double CalculateBasinGrid(CSG_Grid *pBasinGrid, 
						CSG_Grid *pDEM, 			
						int iOutletX, 
						int iOutletY){

	pBasinGrid->Assign((double)0);
	double dAccFlow = AccFlow(pBasinGrid,pDEM,iOutletX,iOutletY);

	return dAccFlow;

}//function

void CalculateFlowAccGrid(CSG_Grid *pFlowAccGrid, 
						CSG_Grid *pDEM){

	int x,y;
	
	pFlowAccGrid->Assign((double)0);
	for(y=0; y<pDEM->Get_NY(); y++){
		for(x=0; x<pDEM->Get_NX(); x++){
			AccFlow(pFlowAccGrid,pDEM,x,y);
		}//for
	}//for

	pFlowAccGrid->Set_Description(_TL("Acc. Area"));
	pFlowAccGrid->Set_Unit(_TL("m2"));
	
}//function


