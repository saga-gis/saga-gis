/**********************************************************
 * Version $Id: idw.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    IDW.cpp
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

#include "idw.h"

CIDW::CIDW()
{

}

CIDW::~CIDW()
{

}

void CIDW::setParameters(CSG_Grid* pGrid, CSG_Shapes *pPoints, int iField){

	m_pGrid = pGrid;
	m_pPoints = pPoints;
	m_iField = iField;

}//method

void CIDW::Interpolate(){

	int x,y;

	for(y=0; y<m_pGrid->Get_NY(); y++){		
		for(x=0; x<m_pGrid->Get_NX(); x++){
			Get_Grid_Value(x,y);
        }// for
    }// for

}//method

//---------------------------------------------------------
bool CIDW::Get_Grid_Value(int x, int y)
{
	int			iPoint, nPoints;
	double		zSum, dSum, d, dx, xPos, yPos;
	TSG_Point	Point;
	CSG_Shape		*pPoint;

	nPoints = m_pPoints->Get_Count();

	xPos	= m_pGrid->Get_XMin() + x * m_pGrid->Get_Cellsize();
	yPos	= m_pGrid->Get_YMin() + y * m_pGrid->Get_Cellsize();

	for(iPoint=0, zSum=0.0, dSum=0.0; iPoint<nPoints; iPoint++){
		
		pPoint = m_pPoints->Get_Shape(iPoint);
		Point = pPoint->Get_Point(0);

		dx		= Point.x - xPos;
		d		= Point.y - yPos;
		d		= sqrt(dx*dx + d*d);

		if( d <= 0.0 ){
			m_pGrid->Set_Value(x, y, pPoint->asDouble(m_iField) );
			return( true );
		}//if

		d		= pow(d, -2);

		zSum	+= d * pPoint->asDouble(m_iField);
		dSum	+= d;
	}//for

	if( dSum > 0.0 ){
		m_pGrid->Set_Value(x, y, zSum / dSum );
		return( true );
	}//if

	m_pGrid->Set_NoData(x, y);

	return( false );

}//method