/*******************************************************************************
    Image_VI_Slope.cpp
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

#include "Image_VI_Slope.h"


CImage_VI_Slope::CImage_VI_Slope(void){

	Set_Name(_TL("Vegetation Index[slope based]"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Slope-based Vegetation Indexes"));

	Parameters.Add_Grid(NULL, 
						"NIR", 
						_TL("Near Infrared Band"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"RED", 
						_TL("Red Band"), 
						"", 
						PARAMETER_INPUT);

		
	Parameters.Add_Grid(NULL, 
						"NDVI", 
						_TL("Normalized Difference Vegetation Index"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"RATIO", 
						_TL("Ratio Vegetation Index"), 
						"", 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"TVI", 
						_TL("Transformed Vegetation Index"), 
						"", 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"CTVI", 
						_TL("Corrected Transformed Vegetation Index"), 
						"", 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"TTVI", 
						_TL("Thiam's Transformed Vegetation Index"), 
						"", 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"NRATIO", 
						_TL("Normalized Ratio Vegetation Index"), 
						"", 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						GRID_TYPE_Double);

}//constructor


CImage_VI_Slope::~CImage_VI_Slope(void)
{}

bool CImage_VI_Slope::On_Execute(void){
	
	double dRed, dNIR;
	
	CSG_Grid* pNIR = Parameters("NIR")->asGrid(); 
	CSG_Grid* pRed = Parameters("RED")->asGrid(); 
	CSG_Grid* pNDVI = Parameters("NDVI")->asGrid(); 
	CSG_Grid* pRatio = Parameters("RATIO")->asGrid(); 
	CSG_Grid* pTVI = Parameters("TVI")->asGrid(); 
	CSG_Grid* pTTVI = Parameters("TTVI")->asGrid();
	CSG_Grid* pCTVI = Parameters("CTVI")->asGrid();
	CSG_Grid* pNRatio = Parameters("NRATIO")->asGrid();

	
    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			dNIR = pNIR->asDouble(x,y);
			dRed = pRed->asDouble(x,y);
			pNDVI->Set_Value(x,y,getNDVI(dRed, dNIR));
			if (pRatio){
				if (dRed!=0){
					pRatio->Set_Value(x,y,getRatio(dRed,dNIR));
				}//if
				else{
					pRatio->Set_Value(x,y,pRatio->Get_NoData_Value());
				}//else
			}//if
			if (pTVI){
				pTVI->Set_Value(x,y,getTVI(dRed,dNIR,pTVI));
			}//if
			if (pCTVI){
				pCTVI->Set_Value(x,y,getCTVI(dRed,dNIR));
			}//if
			if (pRatio){
				pTTVI->Set_Value(x,y,getTTVI(dRed,dNIR));
			}//if
			if (pNRatio){
				pNRatio->Set_Value(x,y,getNRatio(dRed,dNIR));
			}//if
        }// for
    }// for

	return true;

}//method

double CImage_VI_Slope::getNDVI(double dRed, double dNIR){

	return (dNIR-dRed)/(dNIR+dRed);

}//method

double CImage_VI_Slope::getRatio(double dRed, double dNIR){

	return dNIR/dRed;	

}//method

double CImage_VI_Slope::getTVI(double dRed, double dNIR, CSG_Grid *g){

	double dNDVI=getNDVI(dRed,dNIR);

	if (dNDVI>-0.5){
		return sqrt(dNDVI+0.5);
	}//if
	else{
		return g->Get_NoData_Value();
	}//else

}//method

double CImage_VI_Slope::getCTVI(double dRed, double dNIR){

	double dNDVI = getNDVI(dRed,dNIR);
	
	return (dNDVI+0.5)/fabs(dNDVI+0.5)*sqrt(fabs(dNDVI+0.5));

}//method

double CImage_VI_Slope::getTTVI(double dRed, double dNIR){

	double dNDVI = getNDVI(dRed,dNIR);
	
	return sqrt(fabs(dNDVI+0.5));

}//method

double CImage_VI_Slope::getNRatio(double dRed, double dNIR){

	return (dRed/dNIR-1)/(dRed+dNIR+1);

}//method
