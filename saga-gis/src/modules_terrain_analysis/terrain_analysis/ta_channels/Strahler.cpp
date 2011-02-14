/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Strahler.cpp
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

//---------------------------------------------------------
#include "Strahler.h"

//---------------------------------------------------------
CStrahler::CStrahler(void){

	Set_Name		(_TL("Strahler Order"));
	Set_Author		(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Strahler Order Calculation"
	));

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "STRAHLER"	, _TL("Strahler Order"), 
		_TL(""), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);
}

//---------------------------------------------------------
CStrahler::~CStrahler(void)
{}

//---------------------------------------------------------
int CStrahler::getStrahlerOrder(int x, int y)
{
	int		Order	= m_pStrahler->asInt(x, y);

	if( Order == 0 )
	{
		int		i, ix, iy, n;

		for(i=0, n=0, Order=1; i<8; i++)
		{
			if( Get_System()->Get_Neighbor_Pos(i + 4, x, y, ix, iy) && m_pDEM->Get_Gradient_NeighborDir(ix, iy) == i )
			{
				int		iOrder	= getStrahlerOrder(ix, iy);

				if( Order < iOrder )
				{
					Order	= iOrder;
					n		= 1;
				}
				else if( Order == iOrder )
				{
					n++;
				}
			}
		}

		if( n > 1 )
		{
			Order++;
		}

		m_pStrahler->Set_Value(x, y, Order);
	}

	return( Order );
}

//---------------------------------------------------------
bool CStrahler::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")			->asGrid();
	m_pStrahler	= Parameters("STRAHLER")	->asGrid();

	m_pStrahler	->Set_NoData_Value(0.0);
	m_pStrahler	->Assign(0.0);

	DataObject_Set_Colors(m_pStrahler, 10, SG_COLORS_WHITE_BLUE);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				getStrahlerOrder(x, y);
			}
        }
    }

	return( true );
}

//---------------------------------------------------------
