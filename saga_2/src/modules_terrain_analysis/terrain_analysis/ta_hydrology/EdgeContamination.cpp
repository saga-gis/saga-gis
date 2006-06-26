/*******************************************************************************
    EdgeContamination.cpp
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

#include "EdgeContamination.h"
#include "Helper.h"

#define NOT_VISITED -1

CEdgeContamination::CEdgeContamination(void){

	Parameters.Set_Name(_TL("Edge Contamination"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. "));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"CONTAMINATION", 
						_TL("Edge Contamination"), 
						"", 
						PARAMETER_OUTPUT);
}//constructor

CEdgeContamination::~CEdgeContamination(void)
{}

bool CEdgeContamination::On_Execute(void){
	
	m_pEdgeContamination = Parameters("CONTAMINATION")->asGrid();;

	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pEdgeContamination->Assign(NOT_VISITED);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
            m_pEdgeContamination->Set_Value(x,y,getEdgeContamination(x,y));
        }// for
    }// for

	return( true );

}//method

int CEdgeContamination::getEdgeContamination(int x, int y){

    int iNextX, iNextY;
	int iEdgeContamination;

	if (x <= 1 || y <= 1 || x >= Get_NX() - 2 || y >= Get_NY() - 2){
		iEdgeContamination = 1;
	}//if
	else{
		iEdgeContamination = 0;
	}//else
			
	for (int i = -1; i<2; i++){
		for (int j = -1; j<2; j++){
			if (!(i == 0) || !(j == 0)) {
				getNextCell(m_pDEM, x + i, y + j, iNextX, iNextY);
				if (iNextY == y && iNextX == x) {
					if (m_pEdgeContamination->asInt(x+i,y+j)!=NOT_VISITED){
						iEdgeContamination += m_pEdgeContamination->asInt(x+i,y+j);
					}//if
					else{
						iEdgeContamination += getEdgeContamination(x+i,y+j);
					}//else
				}// if				
			}//if				
		}//for
	}//for

	m_pEdgeContamination->Set_Value(x, y, iEdgeContamination);

	return iEdgeContamination;

}//method