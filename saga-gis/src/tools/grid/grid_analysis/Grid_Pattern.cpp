
/*******************************************************************************
    Grid_Pattern.cpp
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
#include "Grid_Pattern.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Pattern::CGrid_Pattern(void)
{
	Set_Name		(_TL("Pattern Analysis"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Pattern Analysis"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"			, _TL("Input Grid"),
		_TL(""),
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid("",
		"NDC"			, _TL("Number of Classes"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);

	Parameters.Add_Grid("",
		"RELATIVE"		, _TL("Relative Richness"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"FRAGMENTATION"	, _TL("Fragmentation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"CVN"			, _TL("Center vs. Neighbours"),
		_TL(""), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);

	Parameters.Add_Grid("",
		"DIVERSITY"		, _TL("Diversity"),
		_TL(""),
		PARAMETER_OUTPUT 
	);

	Parameters.Add_Grid("",
		"DOMINANCE"		, _TL("Dominance"),
		_TL(""),
		PARAMETER_OUTPUT 
	);

	Parameters.Add_Int("",
		"MAXNUMCLASS"	, _TL("Max. Number of Classes"),
		_TL("Maximum number of classes in entire grid."),
		10
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Pattern::On_Execute(void)
{	
	m_pInput = Parameters("INPUT")->asGrid(); 	

	CSG_Grid	*pnClasses      = Parameters("NDC"          )->asGrid();
	CSG_Grid	*pDiversity     = Parameters("DIVERSITY"    )->asGrid();
	CSG_Grid	*pRelative	    = Parameters("RELATIVE"     )->asGrid();
	CSG_Grid	*pDominance     = Parameters("DOMINANCE"    )->asGrid();
	CSG_Grid	*pFragmentation = Parameters("FRAGMENTATION")->asGrid();
	CSG_Grid	*pCVN           = Parameters("CVN"          )->asGrid();

	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	int	maxClasses	= Parameters("MAXNUMCLASS")->asInt();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			int	nCells, nClasses, nCVN;	double Diversity;

			if( Get_Pattern(x, y, nCells, nClasses, nCVN, Diversity) )
			{
				SG_GRID_PTR_SAFE_SET_VALUE (pnClasses     , x, y, nClasses);
				SG_GRID_PTR_SAFE_SET_VALUE (pRelative     , x, y, 100. * nClasses / (double)maxClasses);
				SG_GRID_PTR_SAFE_SET_VALUE (pFragmentation, x, y,        nClasses / (double)nCells);
				SG_GRID_PTR_SAFE_SET_VALUE (pCVN          , x, y, nCVN);
				SG_GRID_PTR_SAFE_SET_VALUE (pDiversity    , x, y, Diversity);
				SG_GRID_PTR_SAFE_SET_VALUE (pDominance    , x, y, log((double)nClasses) - Diversity);
			}
			else
			{
				SG_GRID_PTR_SAFE_SET_NODATA(pnClasses     , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pRelative     , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pDiversity    , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pDominance    , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pFragmentation, x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pCVN          , x, y);
			}
        }
    }

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Pattern::Get_Pattern(int x, int y, int &nCells, int &nClasses, int &nCVN, double &Diversity)
{
	if( m_pInput->is_NoData(x, y) )
	{
		return( false );
	}

	nCells = nCVN = 0;

	CSG_Unique_Number_Statistics	Classes;

	double	iz, z	= m_pInput->asDouble(x, y);

	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i, x);
		int	iy	= m_Kernel.Get_Y(i, y);

		if( m_pInput->is_InGrid(ix, iy) )
		{
			nCells++;

			Classes	+= (iz = m_pInput->asDouble(ix, iy));

			if( z != iz )
			{
				nCVN++;
			}
		}
	}

	nClasses	= Classes.Get_Count();

	Diversity	= 0.;	// the Shannon index !!!

	for(int i=0; i<Classes.Get_Count(); i++)
	{
		double	p	= Classes.Get_Count(i) / (double)nCells;

		Diversity	-= p * log(p);
	}

	return( true );
}


 ///////////////////////////////////////////////////////////
 //														 //
 //														 //
 //														 //
 ///////////////////////////////////////////////////////////

 //---------------------------------------------------------
