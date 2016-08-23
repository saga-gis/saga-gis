/**********************************************************
 * Version $Id: RealArea.cpp 1348 2012-03-12 16:17:14Z oconrad $
 *********************************************************/
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

#include "RealArea.h"

CRealArea::CRealArea(void)
{
	Set_Name		(_TL("Real Surface Area"));
	Set_Author		(SG_T("V. Olaya (c) 2004"));
	Set_Description	(_TW(
		"Calculates real (not projected) cell area"
	));

	Parameters.Add_Grid(NULL, "DEM" , _TL("Elevation"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "AREA", _TL("Surface Area"), _TL(""), PARAMETER_OUTPUT);
}

bool CRealArea::On_Execute(void)
{
	CSG_Grid	*pDEM	= Parameters("DEM" )->asGrid(); 
	CSG_Grid	*pArea	= Parameters("AREA")->asGrid();

    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	s, a;

			if( pDEM->Get_Gradient(x, y, s, a) )
			{
				pArea->Set_Value(x, y, Get_System()->Get_Cellarea() / cos(s));
			}
			else
			{
				pArea->Set_NoData(x,y);
			}
		}
	}
	
	return( true );
}
