
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
	Set_Name		(_TL("Accumulated Cost"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Calculation of accumulated cost, either isotropic or anisotropic, if direction of maximum cost is specified. "
	));

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"DEST_TYPE"		, _TL("Destinations"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Points"),
			_TL("Grid")
		), 0
	);

	Parameters.Add_Shapes("",
		"DEST_POINTS"	, _TL("Destinations"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid("",
		"DEST_GRID"		, _TL("Destinations"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"COST"			, _TL("Local Cost"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("COST",
		"COST_MIN"		, _TL("Minimum Cost"),
		_TL("Zero cost works like a barrier. Use this option to define a minimum cost applied everywhere."),
		0.01, 0., true
	);

	Parameters.Add_Grid("",
		"DIR_MAXCOST"	, _TL("Direction of Maximum Cost"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Choice("DIR_MAXCOST",
		"DIR_UNIT"		, _TL("Units of Direction"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("radians"),
			_TL("degree")
		), 0
	);

	Parameters.Add_Double("DIR_MAXCOST",
		"DIR_K"			, _TL("K Factor"),
		_TL("effective friction = stated friction^f , where f = cos(DifAngle)^k."),
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
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCost_Accumulated::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("COST") )
	{
		pParameters->Set_Enabled("COST_MIN"   , pParameter->asGrid() && pParameter->asGrid()->Get_Min() <= 0.);
	}

	if( pParameter->Cmp_Identifier("DIR_MAXCOST") )
	{
		pParameters->Set_Enabled("DIR_UNIT"   , pParameter->asPointer() != NULL);
		pParameters->Set_Enabled("DIR_K"      , pParameter->asPointer() != NULL);
	}

	if( pParameter->Cmp_Identifier("DEST_TYPE") )
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
	m_pCost			= Parameters("COST"       )->asGrid();
	m_pAccumulated	= Parameters("ACCUMULATED")->asGrid();
	m_pAllocation	= Parameters("ALLOCATION" )->asGrid();

	//-----------------------------------------------------
	m_Cost_Min	= Parameters("COST_MIN")->asDouble();

	if( m_pCost->Get_Min() <= 0. )
	{
		Message_Fmt("\n[%s] %s"     , _TL("Warning"), _TL("Minimum cost value is zero or negative."));
		Message_Fmt("\n[%s] %s (%f)", _TL("Warning"), _TL("A minimum cost value will be used."), m_Cost_Min);
	}

	//-----------------------------------------------------
	CSG_Points_Int	Destinations;

	if( !Get_Destinations(Destinations) )
	{
		Error_Set(_TL("no destination points in grid area."));

		return( false );
	}

	//-----------------------------------------------------
	Get_Cost(Destinations);

	Get_Allocation();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCost_Accumulated::Get_Destinations(CSG_Points_Int &Destinations)
{
	Destinations.Clear();

	m_pAccumulated->Set_NoData_Value(-1.); m_pAccumulated->Assign(-1.);
	m_pAllocation ->Set_NoData_Value(-1.); m_pAllocation ->Assign( 0.);

	if( Parameters("DEST_TYPE")->asInt() == 0 )	// Point
	{
		CSG_Shapes	*pDestinations	= Parameters("DEST_POINTS")->asShapes();

		for(int i=0, x, y; i<pDestinations->Get_Count(); i++)
		{
			if( Get_System().Get_World_to_Grid(x, y, pDestinations->Get_Shape(i)->Get_Point(0)) && !m_pCost->is_NoData(x, y) )
			{
				Destinations.Add(x, y); m_pAllocation->Set_Value(x, y, Destinations.Get_Count()); m_pAccumulated->Set_Value(x, y, 0.);
			}
		}
	}
	else										// Grid
	{
		CSG_Grid	*pDestinations	= Parameters("DEST_GRID")->asGrid();

		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( !pDestinations->is_NoData(x, y) && !m_pCost->is_NoData(x, y) )
			{
				Destinations.Add(x, y); m_pAllocation->Set_Value(x, y, Destinations.Get_Count()); m_pAccumulated->Set_Value(x, y, 0.);
			}
		}
	}

	return( Destinations.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CCost_Accumulated::Get_Cost(int x, int y)
{
	double	Cost	= m_pCost->asDouble(x, y);

	return( Cost < m_Cost_Min ? m_Cost_Min : Cost );
}

//---------------------------------------------------------
bool CCost_Accumulated::Get_Cost(CSG_Points_Int &Destinations)
{
	CSG_Points_Int Next_Point; CSG_Grid Next_Index(Get_System(), SG_DATATYPE_Int);

	CSG_Grid *pDirection = Parameters("DIR_MAXCOST")->asGrid();
	double     Dir_Unit  = Parameters("DIR_UNIT"   )->asInt() == 0 ? 1. : M_DEG_TO_RAD;
	double     Dir_K     = Parameters("DIR_K"      )->asDouble();

	double	Threshold	= Parameters("THRESHOLD")->asDouble();

	sLong	nProcessed	= Destinations.Get_Count();

	//-----------------------------------------------------
	while( Destinations.Get_Count() > 0 && Set_Progress_NCells(nProcessed) )
	{
		Process_Set_Text("%s: %d", _TL("cells in process"), Destinations.Get_Count());

		Next_Point.Clear();

		for(int iPoint=0; iPoint<Destinations.Get_Count(); iPoint++)
		{
			TSG_Point_Int	p	= Destinations[iPoint];

			double	Cost	= Get_Cost(p.x, p.y);
			double	Accu	= m_pAccumulated->asDouble(p.x, p.y);

			for(int i=0; i<8; i++)
			{
				int	ix	= Get_xTo(i, p.x);
				int	iy	= Get_yTo(i, p.y);

				if( m_pCost->is_InGrid(ix, iy) )
				{
					double	dCost	= Get_UnitLength(i);

					if( pDirection )
					{
						static const double	Angle[8] = { 0., M_PI_045, M_PI_090, M_PI_135, M_PI_180, M_PI_225, M_PI_270, M_PI_315 };

						double	d1	= pDirection->is_InGrid(p.x, p.y) ? pow(cos(fabs(Dir_Unit * pDirection->asDouble(p.x, p.y) - Angle[i])), Dir_K) : -1.;
						double	d2	= pDirection->is_InGrid( ix,  iy) ? pow(cos(fabs(Dir_Unit * pDirection->asDouble( ix,  iy) - Angle[i])), Dir_K) : -1.;

						if( d1 >= 0. && d2 >= 0. )
						{
							dCost	*= (d1 + d2) / 2.;
						}
						else if( d1 >= 0. )
						{
							dCost	*= d1;
						}
						else if( d2 >= 0. )
						{
							dCost	*= d2;
						}
					}

					double	iAccu	= Accu + dCost * (Cost + Get_Cost(ix, iy)) / 2.;

					//-------------------------------------
					if( m_pAccumulated->asDouble(ix, iy) < 0.
					||  m_pAccumulated->asDouble(ix, iy) > iAccu + Threshold )
					{
						if( m_pAccumulated->asDouble(ix, iy) < 0. )
						{
							nProcessed++;
						}

						Next_Index     .Set_Value(ix, iy, Next_Point.Get_Count());	// remember last point (least cost!) added at position ix/iy
						Next_Point     .Add      (ix, iy);
						m_pAccumulated->Set_Value(ix, iy, iAccu);
					}
				}
			}
		}

		//-------------------------------------------------
		Destinations.Clear();

		for(int Index=0; Index<Next_Point.Get_Count(); Index++)
		{
			TSG_Point_Int	p	= Next_Point[Index];

			if( Next_Index.asInt(p.x, p.y) == Index )
			{
				Destinations.Add(p.x, p.y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
