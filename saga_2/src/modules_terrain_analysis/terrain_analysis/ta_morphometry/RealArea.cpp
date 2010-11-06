/*******************************************************************************
    RealArea.cpp
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

#include "RealArea.h"
#include "Morphometry.h"

CRealArea::CRealArea(void){

	Parameters.Set_Name(_TL("Real Area Calculation"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Calculates real (not projected) cell area"));

	Parameters.Add_Grid(NULL, 
						"DEM",
						_TL("Elevation"), 						
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"AREA", 
						_TL("Real Area Grid"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Float);

}//constructor

CRealArea::~CRealArea(void)
{}

bool CRealArea::On_Execute(void){
	
	CSG_Grid* pDEM = Parameters("DEM")->asGrid(); 
	CSG_Grid* pArea = Parameters("AREA")->asGrid();
	CSG_Grid pSlope(pDEM);
	CSG_Grid pAspect(pDEM);
	double fArea;
	double fCellArea = pDEM->Get_Cellsize() * pDEM->Get_Cellsize();

	CMorphometry	Morphometry;

	if(	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, PARAMETER_TYPE_Grid, pDEM)
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("SLOPE")		, PARAMETER_TYPE_Grid, &pSlope)
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("ASPECT")		, PARAMETER_TYPE_Grid, &pAspect)
	||	!Morphometry.Execute() )
	{
		return( false );
	}
	

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
			if (!pSlope.is_NoData(x,y)){
				fArea = fCellArea / cos(pSlope.asFloat(x,y,false));
				pArea->Set_Value(x,y,fArea);
			}//if
			else{
				pArea->Set_NoData(x,y);
			}//else
		}//for
	}//for	
	
	return true;

}//method
