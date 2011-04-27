/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    EdgeContamination.cpp
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "EdgeContamination.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define NOT_VISITED -1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CEdgeContamination::CEdgeContamination(void)
{
	Set_Name		(_TL("Edge Contamination"));

	Set_Author		(SG_T("V.Olaya (c) 2004"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	,  "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONTAMINATION"	, _TL("Edge Contamination"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CEdgeContamination::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	m_pDEM				= Parameters("DEM")				->asGrid(); 
	m_pContamination	= Parameters("CONTAMINATION")	->asGrid();

	//-----------------------------------------------------
	m_pContamination->Set_NoData_Value(-2);

	m_Edge.Create(*Get_System(), SG_DATATYPE_Byte);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_InGrid(x, y) )
			{
				for(int i=0; i<8; i++)
				{
					if( !m_pDEM->is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
					{
						m_Edge.Set_Value(x, y, 1);

						break;
					}
				}

				m_pContamination->Set_Value(x, y, -1);
			}
			else
			{
				m_pContamination->Set_NoData(x, y);
			}
		}
	}

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_InGrid(x, y) && !m_Edge.asInt(x, y) )
			{
				for(int i=0; i<8; i++)
				{
					if( m_Edge.asInt(Get_xTo(i, x), Get_yTo(i, y)) == 1 )
					{
						m_Edge.Set_Value(x, y, 2);

						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Get_Contamination(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Edge.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CEdgeContamination::Get_Contamination(int x, int y)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( 0 );
	}

	if( m_pContamination->asInt(x, y) >= 0 )
	{
		return( m_pContamination->asInt(x, y) );
	}

	//-----------------------------------------------------
	int	Contamination	= m_Edge.asInt(x, y) ? 1 : 0;

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xFrom(i, x);
		int	iy	= Get_yFrom(i, y);

		if( m_pDEM->Get_Gradient_NeighborDir(ix, iy) == i )
		{
			Contamination	+= Get_Contamination(ix, iy);
		}
	}

	m_pContamination->Set_Value(x, y, Contamination);

	return( Contamination );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
