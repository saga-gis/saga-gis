/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    IsochronesConst.cpp
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

#include "Helper.h"
#include "IsochronesConst.h"

CIsochronesConst::CIsochronesConst(void){

	Set_Name(_TL("Isochrones Constant Speed"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Isochrones calculation with constant speed"));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation Grid"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"TIME", 
						_TL("Time Out (h)"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);

}//constructor


CIsochronesConst::~CIsochronesConst(void){
	On_Execute_Finish();
}


void CIsochronesConst::writeTimeOut(
        int iX1,
        int iY1,
        int iX2,
        int iY2) {
	
    int iNextX, iNextY;
	double dDist = 1;
	
	if (iX1 < 0 || iX1 >= m_pDEM->Get_NX() || iY1 < 0 || iY1 >= m_pDEM->Get_NY()
            || m_pDEM->is_NoData(iX1,iY1)) {        
    }// if
	else {				
		if (abs(iX1 - iX2 + iY1 - iY2) == 1) {
            dDist = m_pDEM->Get_Cellsize();
        }// if
        else {
            dDist = 1.414 * m_pDEM->Get_Cellsize();
        }// else
		m_pTime->Set_Value(iX1,iY1,m_pTime->asDouble(iX2,iY2) + dDist);        

		for (int i = -1; i<2; i++){
			for (int j = -1; j<2; j++){
				if (!(i == 0) || !(j == 0)) {
					getNextCell(m_pDEM, iX1 + i, iY1 + j, iNextX, iNextY);
					if (iNextY == iY1 && iNextX == iX1) {
						writeTimeOut(iX1 + i, iY1 + j, iX1, iY1);
					}// if				
				}//if				
			}//for
		}//for
    }// else

}// method

bool CIsochronesConst::On_Execute(void){
	
	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pTime = Parameters("TIME")->asGrid();
	m_pTime->Assign(0.0);

	return true;

}//method

bool CIsochronesConst::On_Execute_Finish(void)
{
	return( true );
}

bool CIsochronesConst::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{	
	int x,y;
	int iX, iY;		
	int iHighX, iHighY;

	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}

	m_pTime->Assign(0.0);
	
	writeTimeOut(iX, iY, iX, iY);
    
	double dMax = m_pTime->Get_ZMax();

    for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			double dValue = m_pTime->asDouble(x,y);
            if (dValue == dMax){
				iHighX = x;
				iHighY = y;				
			}//if
        }// for
    }// for
	
	double dMaxDist = m_pTime->Get_ZMax();
	double dH1 = m_pDEM->asDouble(iX,iY);
	double dH2 = m_pDEM->asDouble(iHighX,iHighY);
	double dConcTime = pow(0.87 * pow(dMaxDist / 1000, 3)
                / (dH2 - dH1),
                0.385);
	double dSpeed = dMaxDist / dConcTime;
	
	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			if (m_pTime->asDouble(x,y)){
				m_pTime->Set_Value(x,y, m_pTime->asDouble(x,y)/dSpeed);
			}//if
		}//for
	}//for

	m_pTime->Set_NoData_Value(0.0);

	DataObject_Update(m_pTime, true);

	return (true);

}//method
