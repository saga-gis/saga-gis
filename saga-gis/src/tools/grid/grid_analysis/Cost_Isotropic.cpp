/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Cost_Isotropic.cpp
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
#include "Cost_Isotropic.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCost_Accumulated::CCost_Accumulated(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Accumulated Cost"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Calculation of accumulated cost, either isotropic or anisotropic, if direction of maximum cost is specified. "
	));

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"DEST_TYPE"	, _TL("Input Type of Destinations"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Point"),
			_TL("Grid")
		), 0
	);

	Parameters.Add_Shapes("",
		"DEST_POINTS"	, _TL("Destinations"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid("",
		"DEST_GRID"	, _TL("Destinations"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"COST"		, _TL("Local Cost"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"DIR_MAXCOST"	, _TL("Direction of Maximum Cost"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		"DIR_MAXCOST"	, "DIR_UNIT"	, _TL("Units of Direction"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("radians"),
			_TL("degree")
		), 0
	);

	Parameters.Add_Double(
		"DIR_MAXCOST"	, "DIR_K"		, _TL("K Factor"),
		_TL("effective friction = stated friction ^f , where f = cos(DifAngle)^k."),
		2.
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ACCUMULATED"	, _TL("Accumulated Cost"), 
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"ALLOCATION"	, _TL("Allocation"), 
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"THRESHOLD"	, _TL("Threshold for different route"),
		_TL(""),
		0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCost_Accumulated::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DIR_MAXCOST") )
	{
		pParameters->Set_Enabled("DIR_UNIT", pParameter->asPointer() != NULL);
		pParameters->Set_Enabled("DIR_K"   , pParameter->asPointer() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DEST_TYPE") )
	{
		pParameters->Set_Enabled("DEST_POINTS", pParameter->asInt() == 0);
		pParameters->Set_Enabled("DEST_GRID"  , pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCost_Accumulated::On_Execute(void)
{
	//-----------------------------------------------------
	m_pCost			= Parameters("COST"       )->asGrid();
	m_pAccumulated	= Parameters("ACCUMULATED")->asGrid();
	m_pAllocation	= Parameters("ALLOCATION" )->asGrid();
	m_pDirection	= Parameters("DIR_MAXCOST")->asGrid();
	m_bDegree		= Parameters("DIR_UNIT"   )->asInt() == 1;
	m_dK			= Parameters("DIR_K"      )->asDouble();

	//-----------------------------------------------------
	CPoints	Points;

	if( !Get_Destinations(Points) )
	{
		Error_Set(_TL("no destination points in grid area."));

		return( false );
	}

	//-----------------------------------------------------
	Get_Cost(Points);

	Get_Allocation();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCost_Accumulated::Get_Destinations(CPoints &Points)
{
	Points.Clear();

	m_pAccumulated->Set_NoData_Value(-1.0); m_pAccumulated->Assign(-1.0);
	m_pAllocation ->Set_NoData_Value(-1.0); m_pAllocation ->Assign( 0.0);

	if( Parameters("DEST_TYPE")->asInt() == 0 )	// Point
	{
		CSG_Shapes	*pDestinations	= Parameters("DEST_POINTS")->asShapes();

		for(int i=0, x, y; i<pDestinations->Get_Count(); i++)
		{
			if( Get_System()->Get_World_to_Grid(x, y, pDestinations->Get_Shape(i)->Get_Point(0)) && !m_pCost->is_NoData(x, y) )
			{
				Points.Add(x, y); m_pAllocation->Set_Value(x, y, Points.Get_Count()); m_pAccumulated->Set_Value(x, y, 0.0);
			}
		}
	}
	else										// Grid
	{
		CSG_Grid	*pDestinations	= Parameters("DEST_GRID")->asGrid();

		for(int y=0; y<Get_NY(); y++)	for(int x=0; x<Get_NX(); x++)
		{
			if( !pDestinations->is_NoData(x, y) && !m_pCost->is_NoData(x, y) )
			{
				Points.Add(x, y); m_pAllocation->Set_Value(x, y, Points.Get_Count()); m_pAccumulated->Set_Value(x, y, 0.0);
			}
		}
	}

	return( Points.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCost_Accumulated::Get_Cost(CPoints &Points)
{
	CPoints	Next;	CSG_Grid	Next_Index(*Get_System(), SG_DATATYPE_Int);

	double	Threshold	= Parameters("THRESHOLD")->asDouble();

	sLong	nProcessed	= Points.Get_Count();

	while( Points.Get_Count() > 0 && Set_Progress_NCells(nProcessed) )
	{
		Process_Set_Text(CSG_String::Format("%s: %d", _TL("cells in process"), Points.Get_Count()));

		int		iPoint;

		//-------------------------------------------------
		Next.Clear();

		for(iPoint=0; iPoint<Points.Get_Count(); iPoint++)
		{
			TSG_Point_Int	p	= Points[iPoint];

			double	Cost	= m_pCost       ->asDouble(p.x, p.y);
			double	Accu	= m_pAccumulated->asDouble(p.x, p.y);

			for(int i=0; i<8; i++)
			{
				int	ix	= Get_xTo(i, p.x);
				int	iy	= Get_yTo(i, p.y);

				if( m_pCost->is_InGrid(ix, iy) )
				{
					double	iCost	= Get_UnitLength(i);

					if( m_pDirection && m_pDirection->is_InGrid(p.x, p.y) && m_pDirection->is_InGrid(ix, iy) )
					{
						static const double	dAngles[8]	= {	0.0, M_PI_045, M_PI_090, M_PI_135, M_PI_180, M_PI_225, M_PI_270, M_PI_315 };

						double	d1	= m_pDirection->asDouble(p.x, p.y); d1 = pow(cos(fabs((m_bDegree ? M_DEG_TO_RAD * d1 : d1) - dAngles[i])), m_dK);
						double	d2	= m_pDirection->asDouble( ix,  iy); d2 = pow(cos(fabs((m_bDegree ? M_DEG_TO_RAD * d2 : d2) - dAngles[i])), m_dK);

						iCost	*= (d1 + d2) / 2.0;
					}

					iCost	= Accu + iCost * (Cost + m_pCost->asDouble(ix, iy)) / 2.0;

					//-------------------------------------
					bool	bProcessed	= !m_pAccumulated->is_NoData(ix, iy);

					if( !bProcessed || m_pAccumulated->asDouble(ix, iy) > iCost + Threshold )
					{
						if( !bProcessed )
						{
							nProcessed++;
						}

						Next_Index     .Set_Value(ix, iy, Next.Get_Count());	// remember last point (least cost!) added at position ix/iy
						Next           .Add      (ix, iy);
						m_pAccumulated->Set_Value(ix, iy, iCost);
					}
				}
			}
		}

		//-------------------------------------------------
		Points.Clear();

		for(iPoint=0; iPoint<Next.Get_Count(); iPoint++)
		{
			TSG_Point_Int	p	= Next[iPoint];

			if( Next_Index.asInt(p.x, p.y) == iPoint )
			{
				Points.Add(p.x, p.y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCost_Accumulated::Get_Allocation(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Allocation(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
int CCost_Accumulated::Get_Allocation(int x, int y)
{
	int	Allocation	= m_pAllocation->asInt(x, y);

	if( Allocation != 0 )
	{
		return( Allocation );
	}

	Allocation	= m_pAccumulated->Get_Gradient_NeighborDir(x, y, true, false);

	if( Allocation >= 0 )
	{
		Allocation	= Get_Allocation(Get_xTo(Allocation, x), Get_yTo(Allocation, y));
	}

	m_pAllocation->Set_Value(x, y, Allocation);

	return( Allocation );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
