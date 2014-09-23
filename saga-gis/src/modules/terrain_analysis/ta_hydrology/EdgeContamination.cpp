/**********************************************************
 * Version $Id: EdgeContamination.cpp 1016 2011-04-27 18:40:36Z oconrad $
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
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

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"This tool uses flow directions to estimate possible contamination "
		"effects moving from outside of the grid passing the edge into its interior. "
		"This means that derived contributing area values might be underestimated "
		"for the marked cells. Cells not contamined will be marked as no data. "
	));

	Parameters.Add_Grid(
		NULL	,  "ELEVATION"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONTAMINATION"	, _TL("Edge Contamination"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("single flow direction"),
			_TL("multiple flow direction")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	EFFECT_NO		= -1,
	EFFECT_NODATA	=  0,
	EFFECT_EDGE		=  1,
	EFFECT_YES		=  2,
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CEdgeContamination::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("ELEVATION"    )->asGrid(); 
	m_pEffect	= Parameters("CONTAMINATION")->asGrid();

	int	Method	= Parameters("METHOD")->asInt();

	m_pEffect->Assign(0.0);
	m_pEffect->Set_NoData_Value_Range(EFFECT_NO, EFFECT_NODATA);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				bool	bEdge	= false;

				for(int i=0; i<8 && !bEdge; i++)
				{
					if( !m_pDEM->is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
					{
						bEdge	= true;
					}
				}

				if( bEdge )
				{
					switch( Method )
					{
					default:	Set_D8	(x, y);	break;
					case  1:	Set_MFD	(x, y);	break;
					}
				}
				else if( m_pEffect->asInt(x, y) == EFFECT_NODATA )
				{
					m_pEffect->Set_Value(x, y, EFFECT_NO);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CEdgeContamination::Get_D8(int x, int y)
{
	int		iMax	= -1;

	if( m_pDEM->is_InGrid(x, y) )
	{
		double	z		= m_pDEM->asDouble(x, y);
		double	dzMax	= 0.0;

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) )
			{
				double	dz	= (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i);

				if( dz > dzMax )
				{
					iMax	= i;
					dzMax	= dz;
				}
			}
		}
	}

	return( iMax );
}

//---------------------------------------------------------
int CEdgeContamination::Set_D8(int x, int y)
{
	m_pEffect->Set_Value(x, y, EFFECT_EDGE);

	int	i, nContaminated	= 1;

	//-----------------------------------------------------
	while( (i = Get_D8(x, y)) >= 0
		&&  m_pDEM->is_InGrid(x = Get_xTo(i, x), y = Get_yTo(i, y))
		&&  m_pEffect->asInt(x, y) <= EFFECT_NODATA )
	{
		m_pEffect->Set_Value(x, y, EFFECT_YES);

		nContaminated++;
	}

	//-----------------------------------------------------
	return( nContaminated );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CEdgeContamination::Get_MFD(int x, int y, double dz[8])
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		i;
		double	z, d, dzSum;

		for(i=0, dzSum=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && (d = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > 0.0 )
			{
				dzSum	+= (dz[i] = d);
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		if( dzSum > 0.0 )
		{
			for(i=0; i<8; i++)
			{
				if( dz[i] > 0.0 )
				{
					dz[i]	/= dzSum;
				}
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
int CEdgeContamination::Set_MFD(int x, int y)
{
	m_pEffect->Set_Value(x, y, EFFECT_EDGE);

	CSG_Grid_Stack	Stack;	Stack.Push(x, y);

	double	dz[8];

	//-----------------------------------------------------
	while( Stack.Get_Size() > 0 && Process_Get_Okay() )
	{
		Stack.Pop(x, y);

		if( Get_MFD(x, y, dz) )
		{
			for(int i=0; i<8; i++)
			{
				if( dz[i] > 0.0 )
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( m_pEffect->asInt(ix, iy) <= EFFECT_NODATA )
					{
						m_pEffect->Set_Value(ix, iy, EFFECT_YES);

						Stack.Push(ix, iy);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
