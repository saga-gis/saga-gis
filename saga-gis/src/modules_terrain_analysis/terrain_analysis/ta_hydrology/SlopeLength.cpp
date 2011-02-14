/*******************************************************************************
    SlopeLength.cpp
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
#include "SlopeLength.h"

CSlopeLength::CSlopeLength(void){


	Parameters.Set_Name(_TL("Slope Length"));

	Parameters.Set_Description(_TL(""));

	Parameters.Add_Grid(
		NULL, "DEM", _TL("Elevation"),
		_TL(""), PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "LENGTH", _TL("Slope Length"),
		_TL(""),
		PARAMETER_OUTPUT
	);

}//constructor

CSlopeLength::~CSlopeLength(void){}


bool CSlopeLength::On_Execute(void){

	int x,y;
	int n;
	double	slope, aspect;

	m_pSlopeLengthGrid = Parameters("LENGTH")->asGrid();
	m_pDEM = Parameters("DEM")->asGrid();

	m_pSlopeLengthGrid->Assign(0.0);
	
	m_pSlopeGrid = SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);
	
	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			if( m_pDEM->Get_Gradient(x, y, slope, aspect) )	{
				m_pSlopeGrid->Set_Value(x, y, slope);				
			}//if
			else{
				m_pSlopeGrid->Set_NoData(x, y);				
			}//else
		}//for
	}//for

	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++){
		if( m_pDEM->Get_Sorted(n, x, y) )
			Set_Length(x, y);	
	}//for

	delete m_pSlopeGrid;

	DataObject_Set_Colors(m_pSlopeLengthGrid, 100, SG_COLORS_WHITE_BLUE);

	return true ;

}//method

void CSlopeLength::Set_Length(int x, int y){
	
	int	i,ix,iy;
	double dSlope, dSlope2;
	double dLength;

	if( m_pDEM->is_InGrid(x, y) && (i = m_pDEM->Get_Gradient_NeighborDir(x, y, true)) >= 0 ){
		
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) ){

			dSlope = m_pSlopeGrid->asDouble(x,y);
			dSlope2 = m_pSlopeGrid->asDouble(ix,iy);
			if (dSlope2 > 0.5 * dSlope){
				dLength = m_pSlopeLengthGrid->asDouble(x, y) + Get_Length(i);
			}//if
			else{
				dLength = 0.;
			}//else
			
			if (dLength > m_pSlopeLengthGrid->asDouble(ix, iy)){
				m_pSlopeLengthGrid->Set_Value(ix, iy, dLength);
			}//if

		}//if

	}//if

}//method


