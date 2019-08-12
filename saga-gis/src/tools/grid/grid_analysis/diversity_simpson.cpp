
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 diversity_simpson.cpp                 //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "diversity_simpson.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDiversity_Simpson::CDiversity_Simpson(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Simpson Index"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Grid based analysis of diversity with the Simpson Index. "
		"The index is calculated locally for each grid cell using "
		"the specified kernel (aka 'moving window'). It is assumed "
		"that the grid cell values represent a classification. "
	));

	Add_Reference("Simpson, E.H.", "1949",
		"Measurement of diversity",
		"Nature, 163:688.",
		SG_T("https://doi.org/10.1038/163688a0"), SG_T("doi:10.1038/163688a0.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"CATEGORIES"	, _TL("Categories"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"COUNT"			, _TL("Number of Categories"),
		_TL("number of different categories (unique values) within search area"),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"INDEX"			, _TL("Simpson Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDiversity_Simpson::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Simpson::On_Execute(void)
{
	//-----------------------------------------------------
	m_pClasses	= Parameters("CATEGORIES")->asGrid();

	CSG_Grid	*pCount	= Parameters("COUNT")->asGrid();
	CSG_Grid	*pIndex	= Parameters("INDEX")->asGrid();

	if( pCount ) pCount->Fmt_Name("%s [%s]", m_pClasses->Get_Name(), _TL("Count"        ));
	if( pIndex ) pIndex->Fmt_Name("%s [%s]", m_pClasses->Get_Name(), _TL("Simpson Index"));

	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Count;	double	Index;

			if( Get_Index(x, y, Count, Index) )
			{
				if( pCount ) pCount->Set_Value(x, y, Count);
				if( pIndex ) pIndex->Set_Value(x, y, Index);
			}
			else
			{
				if( pCount ) pCount->Set_NoData(x, y);
				if( pIndex ) pIndex->Set_NoData(x, y);
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
bool CDiversity_Simpson::Get_Index(int x, int y, int &Count, double &Index)
{
	if( m_pClasses->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Unique_Number_Statistics	Classes;

	int	nTotal	= 0;

	for(int iCell=0; iCell<m_Kernel.Get_Count(); iCell++)
	{
		int	ix	= m_Kernel.Get_X(iCell, x);
		int	iy	= m_Kernel.Get_Y(iCell, y);

		if( m_pClasses->is_InGrid(ix, iy) )
		{
			Classes	+= m_pClasses->asDouble(ix, iy);

			nTotal	++;
		}
	}

	//-----------------------------------------------------
	Count	= Classes.Get_Count();

	if( Count <= 1 )
	{
		Index	= 0.;

		return( true );
	}

	//-----------------------------------------------------
	Index	= 1.;

	for(int iClass=0; iClass<Classes.Get_Count(); iClass++)
	{
		double	p	= Classes.Get_Count(iClass) / (double)nTotal;	// relative proportion of class members

		Index	-= p*p;
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
