/*******************************************************************************
    Grid_Buffer.cpp
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

#include "Grid_Buffer.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
	
int BUFFER = 1;
int FEATURE = 2;

CGrid_Buffer::CGrid_Buffer(void){

	Set_Name(_TL("Grid Buffer"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Grid Buffer Creation"));

	Parameters.Add_Grid(NULL, 
						"FEATURES",
						_TL("Features Grid"), 						
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"BUFFER", 
						_TL("Buffer Grid"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Value(NULL, 
						"DIST", 
						_TL("Distance"), 
						_TL("Distance (Grid Units)."), 
						PARAMETER_TYPE_Double, 
						1000);
						
	Parameters.Add_Choice(NULL, 
						"BUFFERTYPE", 
						_TL("Buffer Distance"),
						_TL(""),
						_TL("Fixed|Cell value|"),
						0);
}//constructor


CGrid_Buffer::~CGrid_Buffer(void)
{}

bool CGrid_Buffer::On_Execute(void){
	
	CSG_Grid* pFeatures = Parameters("FEATURES")->asGrid(); 
	CSG_Grid* pGrid_Buffer = Parameters("BUFFER")->asGrid();
	int iBufferType = Parameters("BUFFERTYPE")->asInt();
	double dBufDist = Parameters("DIST")->asDouble() / pFeatures->Get_Cellsize();
	int iBufFixedDist = (int) (dBufDist + 2.0);
	double dDist = 0;
	int iBufDist;
	double dValue = 0;
	int x2=0, y2=0;

	pGrid_Buffer->Set_NoData_Value(0.0);
	pGrid_Buffer->Assign(0.0);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
            dValue = pFeatures->asDouble(x,y);
			if (dValue != 0 && !pFeatures->is_NoData(x,y)){
				if (iBufferType==1){
					dBufDist = dValue / pFeatures->Get_Cellsize();
					iBufDist = (int) (dBufDist + 2.0);
				}//if
				else{
					iBufDist = iBufFixedDist;
				}//else
				for	(int i=-iBufDist ; i<iBufDist ; i++){
					for (int j=-iBufDist ; j<iBufDist ; j++){
						x2 = max(min(Get_NX()-1,x+i),0);
						y2 = max(min(Get_NY()-1,y+j),0);
						dDist= M_GET_LENGTH(x-x2, y-y2);
						if (dDist<dBufDist){
							dValue = pFeatures->asDouble(x2,y2);
							if (dValue != 0 && dValue!= pFeatures->Get_NoData_Value()){
								pGrid_Buffer->Set_Value(x2,y2,FEATURE);
							}//if
							else{
								pGrid_Buffer->Set_Value(x2,y2,BUFFER);	
							}//else
						}//if
					}//for
				}//for
			}//if
		}//for
	}//for						

	return( true );

}//method
