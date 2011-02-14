/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Image_VI_Distance.cpp
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

#include "Image_VI_Distance.h"


CImage_VI_Distance::CImage_VI_Distance(void){

	Set_Name(_TL("Vegetation Index[distance based]"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Distance-based Vegetation Indexes"));

	Parameters.Add_Grid(NULL, 
						"NIR", 
						_TL("Near Infrared Band"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"RED", 
						_TL("Red Band"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Value(NULL, 
						"SLOPE", 
						_TL("Slope of the soil line"), 
						_TL("Slope of the soil line"), 
						PARAMETER_TYPE_Double, 
						0.5);

	Parameters.Add_Value(NULL, 
						"INTERCEPT", 
						_TL("Intercept of the soil line"), 
						_TL("Intercept of the soil line"), 
						PARAMETER_TYPE_Double, 
						0);

		
	Parameters.Add_Grid(NULL, 
						"PVI", 
						_TL("PVI (Richardson and Wiegand)"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"PVI1", 
						_TL("PVI (Perry & Lautenschlager)"), 
						_TL(""), 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						SG_DATATYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"PVI2", 
						_TL("PVI (Walther & Shabaani)"), 
						_TL(""), 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						SG_DATATYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"PVI3", 
						_TL("PVI (Qi, et al) "), 
						_TL(""), 
						PARAMETER_OUTPUT_OPTIONAL, 
						true, 
						SG_DATATYPE_Double);
}//constructor


CImage_VI_Distance::~CImage_VI_Distance(void)
{}

bool CImage_VI_Distance::On_Execute(void){
	
	double dRed, dNIR;
	
	CSG_Grid* pNIR = Parameters("NIR")->asGrid(); 
	CSG_Grid* pRed = Parameters("RED")->asGrid(); 
	CSG_Grid* pPVI = Parameters("PVI")->asGrid(); 
	CSG_Grid* pPVI1 = Parameters("PVI1")->asGrid(); 
	CSG_Grid* pPVI2 = Parameters("PVI2")->asGrid(); 
	CSG_Grid* pPVI3 = Parameters("PVI3")->asGrid();
	double dSlope = Parameters("SLOPE")->asDouble();
	double dInter = Parameters("INTERCEPT")->asDouble();
		
    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			dNIR = pNIR->asDouble(x,y);
			dRed = pRed->asDouble(x,y);
			pPVI->Set_Value(x,y,getPVI(dRed, dNIR,dSlope,dInter));
			if (pPVI1){
				pPVI1->Set_Value(x,y,getPVI1(dRed,dNIR,dSlope,dInter));
			}//if			
			if (pPVI2){
				pPVI2->Set_Value(x,y,getPVI2(dRed,dNIR,dSlope,dInter));
			}//if
			if (pPVI3){
				pPVI3->Set_Value(x,y,getPVI3(dRed,dNIR,dSlope,dInter));
			}//if
        }// for
    }// for

	return true;

}//method

double CImage_VI_Distance::getPVI(double dRed, 
							double dNIR, 
							double dSlope, 
							double dInter){

	double dA1 = dSlope;
	double dA0 = dInter;
	double dB1=1/dSlope;
	double dB0=dRed-dB1*dNIR;	
	
	//don't ask why those names... i got them from Idrisi Help File...
	double dRgg5=(dB1*dA0-dB0*dA1)/(dB1-dA1); 
	double dRgg7 = (dA0-dB0)/(dB1-dA1);
	
	return (sqrt(pow(dRgg5-dRed,2)+pow(dRgg7-dNIR,2)));

}//method


///// PVI indexing also taken from Idrisi docs...////////
double CImage_VI_Distance::getPVI1(double dRed, 
							double dNIR,
							double dSlope, 
							double dInter){

	return ((dSlope*dNIR-dRed+dInter)/sqrt(dSlope*dSlope+1));

}//method


double CImage_VI_Distance::getPVI2(double dRed, 
							double dNIR,
							double dSlope, 
							double dInter){
	
	return (((dNIR-dInter)*(dRed+dSlope))/sqrt(1+dInter));

}//method

double CImage_VI_Distance::getPVI3(double dRed, 
							double dNIR,
							double dSlope, 
							double dInter){
	
	return ((dNIR*dInter)-(dRed*dSlope));

}//method
