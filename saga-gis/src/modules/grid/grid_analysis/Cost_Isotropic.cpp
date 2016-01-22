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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Accumulated Cost"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Calculation of accumulated cost, either isotropic or anisotropic, if direction of maximum cost is specified. "
	));

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "DEST_TYPE"	, _TL("Input Type of Destinations"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Point"),
			_TL("Grid")
		), 1
	);

	Parameters.Add_Shapes(
		NULL	, "DEST_POINTS"	, _TL("Destinations"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL	, "DEST_GRID"	, _TL("Destinations"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "COST"		, _TL("Local Cost"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "DIR_MAXCOST"	, _TL("Direction of Maximum Cost"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		pNode	, "DIR_UNIT"	, _TL("Units of Direction"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("radians"),
			_TL("degree")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "DIR_K"		, _TL("K Factor"),
		_TL("effective friction = stated friction ^f , where f = cos(DifAngle)^k."),
		PARAMETER_TYPE_Double, 2
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ACCUMULATED"	, _TL("Accumulated Cost"), 
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "ALLOCATION"	, _TL("Allocation"), 
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Threshold for different route"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
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
		pParameters->Set_Enabled("DIR_UNIT", pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("DIR_K"   , pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DEST_TYPE") )
	{
		pParameters->Set_Enabled("DEST_POINTS", pParameter->asInt() == 0);
		pParameters->Set_Enabled("DEST_GRID"  , pParameter->asInt() == 1);
	}

	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCost_Accumulated::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pCost			= Parameters("COST"       )->asGrid();
	CSG_Grid	*pAccumulated	= Parameters("ACCUMULATED")->asGrid();
	CSG_Grid	*pAllocation	= Parameters("ALLOCATION" )->asGrid();

	m_pDirection	= Parameters("DIR_MAXCOST")->asGrid();
	m_bDegree		= Parameters("DIR_UNIT"   )->asInt() == 1;
	m_dK			= Parameters("DIR_K"      )->asDouble();

	//-----------------------------------------------------
	CPoints	Points;

	pAccumulated->Set_NoData_Value(-1.0); pAccumulated->Assign_NoData();
	pAllocation ->Set_NoData_Value(-1.0); pAllocation ->Assign_NoData();

	if( Parameters("DEST_TYPE")->asInt() == 0 )	// Point
	{
		CSG_Shapes	*pDestinations	= Parameters("DEST_POINTS")->asShapes();

		for(int i=0, x, y; i<pDestinations->Get_Count(); i++)
		{
			if( Get_System()->Get_World_to_Grid(x, y, pDestinations->Get_Shape(i)->Get_Point(0)) )
			{
				Points.Add(x, y, 1 + Points.Get_Count());
				pAccumulated->Set_Value(x, y, 0.0);
				pAllocation ->Set_Value(x, y, Points.Get_Count());
			}
		}
	}
	else										// Grid
	{
		CSG_Grid	*pDestinations	= Parameters("DEST_GRID")->asGrid();

		for(int y=0; y<Get_NY(); y++)	for(int x=0; x<Get_NX(); x++)
		{
			if( !pDestinations->is_NoData(x, y) && !pCost->is_NoData(x, y) )
			{
				Points.Add(x, y, 1 + Points.Get_Count());
				pAccumulated->Set_Value(x, y, 0.0);
				pAllocation ->Set_Value(x, y, Points.Get_Count());
			}
		}
	}

	if( Points.Get_Count() < 1 )
	{
		Error_Set(_TL("no destination points in grid area."));

		return( false );
	}

	//-----------------------------------------------------
	double	Threshold	= Parameters("THRESHOLD")->asDouble();

	sLong	nProcessed	= Points.Get_Count();

	while( Points.Get_Count() > 0 && Set_Progress_NCells(nProcessed) )
	{
		Process_Set_Text(CSG_String::Format("%s: %d", _TL("cells in process"), Points.Get_Count()));

		CPoints	Next;	int	iPoint;

		for(iPoint=0; iPoint<Points.Get_Count(); iPoint++)
		{
			TSG_Point_Int	p	= Points.Get_Point(iPoint);

			double	Cost	= pCost       ->asDouble(p.x, p.y);
			double	Accu	= pAccumulated->asDouble(p.x, p.y);

			for(int i=0; i<8; i++)
			{
				int	ix	= Get_xTo(i, p.x);
				int	iy	= Get_yTo(i, p.y);

				if( pCost->is_InGrid(ix, iy) )
				{
					double	dCost	= Accu + Get_CostInDirection(p, i) * (Cost + pCost->asDouble(ix, iy)) / 2.0;

					bool	bProcessed	= !pAccumulated->is_NoData(ix, iy);

					if( !bProcessed || pAccumulated->asDouble(ix, iy) > dCost + Threshold )
					{
						if( !bProcessed )
						{
							nProcessed++;
						}

						Next.Add(ix, iy, Points.Get_Allocation(iPoint));

						pAccumulated->Set_Value(ix, iy, dCost);
						pAllocation ->Set_Value(ix, iy, Points.Get_Allocation(iPoint));
					}
				}
			}
		}

		//-------------------------------------------------
		Points.Clear();

		for(iPoint=0; iPoint<Next.Get_Count(); iPoint++)
		{
			TSG_Point_Int	p	= Next.Get_Point(iPoint);

			if( pAllocation->asInt(p.x, p.y) == Next.Get_Allocation(iPoint) )
			{
				Points.Add(p.x, p.y, Next.Get_Allocation(iPoint));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CCost_Accumulated::Get_CostInDirection(const TSG_Point_Int &p, int i)
{
	if( m_pDirection && m_pDirection->is_InGrid(p.x, p.y) )
	{
		int	ix	= Get_xTo(i, p.x);
		int	iy	= Get_yTo(i, p.y);

		if( m_pDirection->is_InGrid(ix, iy) )
		{
			static const double	dAngles[8]	= {	0.0, M_PI_045, M_PI_090, M_PI_135, M_PI_180, M_PI_225, M_PI_270, M_PI_315 };

			double	d1	= m_pDirection->asDouble(p.x, p.y);
			double	d2	= m_pDirection->asDouble( ix,  iy);

			d1	= pow(cos(fabs((m_bDegree ? M_DEG_TO_RAD * d1 : d1) - dAngles[i])), m_dK);
			d2	= pow(cos(fabs((m_bDegree ? M_DEG_TO_RAD * d2 : d2) - dAngles[i])), m_dK);

			return( Get_UnitLength(i) * (d1 + d2) / 2.0 );
		}
	}

	return( Get_UnitLength(i) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
