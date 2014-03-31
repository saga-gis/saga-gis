/**********************************************************
 * Version $Id: ProtectionIndex.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    ProtectionIndex.cpp
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

#include "ProtectionIndex.h"

#define NO_DATA -1

CProtectionIndex::CProtectionIndex(void){

	Set_Name(_TL("Morphometric Protection Index"));
    Set_Author(SG_T("(c) 2005 by Victor Olaya"));
	Set_Description(_TW(
        "This algorithm analyses the immediate surrounding of each cell up to an given distance and evaluates how the relief protects it.\n"
         "It is equivalent to the positive openness described in: Visualizing Topography by Openness: A New Application of Image Processing to Digital Elevation Models, Photogrammetric Engineering and Remote Sensing(68), No. 3, March 2002, pp. 257-266."));

	Parameters.Add_Grid(NULL, 
						"DEM",
						_TL("Elevation"), 						
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"PROTECTION", 
						_TL("Protection Index"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Float);

	Parameters.Add_Value(NULL, 
						"RADIUS", 
						_TL("Radius"), 
						_TL(""), 
						PARAMETER_TYPE_Double, 2000, 
						0.0, 
						true);

}//constructor

CProtectionIndex::~CProtectionIndex(void)
{}

bool CProtectionIndex::On_Execute(void){
	
	int x,y;
	double dProtectionIndex;
	CSG_Grid* pProtectionIndex = Parameters("PROTECTION")->asGrid();
	
	m_dRadius = Parameters("RADIUS")->asDouble();
	m_pDEM = Parameters("DEM")->asGrid(); 

    for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			dProtectionIndex = getProtectionIndex(x,y);
			if (dProtectionIndex == NO_DATA){
				pProtectionIndex->Set_NoData(x,y);
			}//if
			else{
				pProtectionIndex->Set_Value(x,y, dProtectionIndex);
			}//else
		}//for
	}//for	
	
	return true;

}//method

double CProtectionIndex::getProtectionIndex(int x, int y){

	int i,j;
	int iDifX[] = {0,1,1,1,0,-1,-1,-1};
	int iDifY[] = {1,1,0,-1,-1,-1,0,1};
	double dDifHeight;
	double dDist;
	double dAngle;
	double dProtectionIndex = 0;
	double aAngle[8];

	for (i = 0; i < 8; i++){
		j = 1;
		aAngle[i] = 0;
		dDist = M_GET_LENGTH(iDifX[i], iDifY[i]) * j * m_pDEM->Get_Cellsize();
		while (dDist < m_dRadius){
			if (m_pDEM->is_InGrid(x + iDifX[i] * j, y + iDifY[i] * j)){
				dDifHeight = m_pDEM->asDouble(x,y);
			}//if
			else{
				return NO_DATA;
			}
			dDifHeight = m_pDEM->asDouble(x + iDifX[i] * j, y + iDifY[i] * j) 
						 - m_pDEM->asDouble(x,y);
			dAngle = atan (dDifHeight / dDist);
			if (dAngle > aAngle[i]){
				aAngle[i] = dAngle;
			}//if
			j++;
			dDist = M_GET_LENGTH(iDifX[i], iDifY[i]) * j * m_pDEM->Get_Cellsize();
		}//while
		dProtectionIndex+=aAngle[i];
	}//while

	return (dProtectionIndex / 8.);

}//method
