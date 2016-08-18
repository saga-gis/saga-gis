/**********************************************************
 * Version $Id: Sinuosity.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    Sinuosity.cpp
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

#include "Sinuosity.h"
#include "Helper.h"

CSinuosity::CSinuosity(void){

	Parameters.Set_Name(_TL("Flow Sinuosity"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Flow sinuosity calculation\r\n"
		"References:\r\n 1. Olaya, V. Hidrologia computacional y modelos digitales del terreno. Alqua. 536 pp. 2004"));


	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation Grid"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"SINUOS", 
						_TL("Sinuosity"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);

}//constructor


CSinuosity::~CSinuosity(void)
{}

void CSinuosity::writeDistOut(
        int iX1,
        int iY1,
        int iX2,
        int iY2) {
	
    int iNextX, iNextY;
	double dDist = 1;
	
	if (iX1 < 0 || iX1 >= m_pDEM->Get_NX() || iY1 < 0 || iY1 >= m_pDEM->Get_NY()
            || m_pDEM->asFloat(iX1,iY1) == m_pDEM->Get_NoData_Value()) {        
    }// if
	else {				

		dDist = sqrt(pow((double)iX1-iX2,2)+pow((double)iY1-iY2,2))*m_pSinuosity->Get_Cellsize();
		m_pSinuosity->Set_Value(iX1,iY1,m_pSinuosity->asDouble(iX2,iY2) + dDist);        

		for (int i = -1; i<2; i++){
			for (int j = -1; j<2; j++){
				if (!(i == 0) || !(j == 0)) {
					getNextCell(m_pDEM, iX1 + i, iY1 + j, iNextX, iNextY);
					if (iNextY == iY1 && iNextX == iX1) {
						writeDistOut(iX1 + i, iY1 + j, iX1, iY1);
					}// if				
				}//if				
			}//for
		}//for

    }// else

}// method

void CSinuosity::ZeroToNoData(void){
		
    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
            if (m_pSinuosity->asDouble(x,y) == 0){
				m_pSinuosity->Set_Value(x,y,m_pSinuosity->Get_NoData_Value());
			}//if
        }// for
    }// for

}//method


void CSinuosity::calculateSinuosity(void){
	
	double dDist;
    
	for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
            if (m_pSinuosity->asDouble(x,y) != m_pSinuosity->Get_NoData_Value()){				
				dDist = sqrt(pow((double)x-m_iX,2)+pow((double)y-m_iY,2))*m_pSinuosity->Get_Cellsize();
				if (dDist!=0){
					m_pSinuosity->Set_Value(x,y,m_pSinuosity->asDouble(x,y)/dDist);
				}//if
			}//if
        }// for
    }// for

}//method

bool CSinuosity::On_Execute(void){

	m_pDEM			= Parameters("DEM")->asGrid(); 
	m_pSinuosity	= Parameters("SINUOS")->asGrid();

	DataObject_Update(m_pSinuosity, true);

	return true;

}//method

bool CSinuosity::On_Execute_Finish(){

	return( true );
}

bool CSinuosity::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode){	
	
	int iX, iY;

	if(	Mode != TOOL_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}

//	if( !m_pDEM->is_InGrid_byPos(ptWorld.Get_X(), ptWorld.Get_Y()) ){
//		return false;
//	}//if

//	iX	= (int) ((ptWorld.Get_X() - m_pDEM->Get_XMin()) / m_pDEM->Get_Cellsize());
//	iY	= (int) ((ptWorld.Get_Y() - m_pDEM->Get_YMin()) / m_pDEM->Get_Cellsize());

	m_iX = iX;
	m_iY = iY;

	m_pSinuosity->Assign((double)0);
	
	writeDistOut(iX, iY, iX, iY);
	ZeroToNoData();
	calculateSinuosity();

	DataObject_Update(m_pSinuosity);
    
	return true;

}//method
