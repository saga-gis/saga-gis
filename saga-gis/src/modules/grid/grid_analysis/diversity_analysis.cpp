/**********************************************************
 * Version $Id: diversity_analysis.cpp 2476 2015-04-22 18:41:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 diversity_analysis.cpp                //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "diversity_analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDiversity_Analysis::CDiversity_Analysis(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Diversity of Categories"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Grid based analysis of diversity. It is assumed that the "
		"input grid provides a classification (i.e. not a contiuous field). "
		"For each cell it counts the number of different categories (classes) "
		"as well as the connectivity within the chosen search window. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "CATEGORIES"		, _TL("Categories"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "DIVERSITY"		, _TL("Number of Categories"),
		_TL("number of different categories (unique values) within search area"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SIZE_MEAN"		, _TL("Average Size"),
		_TL("average size of the area covered by each category that occurs within search area"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SIZE_SKEW"		, _TL("Skewness"),
		_TL("skewness of the area size per category distribution"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONNECTIVITY"	, _TL("Connectivity"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "SEARCH_MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "SEARCH_RADIUS"	, _TL("Search Distance"),
		_TL("Search distance given as number cells."),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "NB_CASE"			, _TL("Connectivity Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Rook's case"),
			_TL("Queen's case")
		), 1
	);

	m_Search.Get_Weighting().Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Search.Get_Weighting().Set_BandWidth(0.7);
	m_Search.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDiversity_Analysis::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.Get_Weighting().Enable_Parameters(pParameters);

	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Analysis::On_Execute(void)
{
	//-----------------------------------------------------
	m_pClasses		= Parameters("CATEGORIES"  )->asGrid();
	m_pDiversity	= Parameters("DIVERSITY"   )->asGrid();
	m_pSize_Mean	= Parameters("SIZE_MEAN"   )->asGrid();
	m_pSize_Skew	= Parameters("SIZE_SKEW"   )->asGrid();
	m_pConnectivity	= Parameters("CONNECTIVITY")->asGrid();

	m_pDiversity	->Set_Name(CSG_String::Format("%s [%s]", m_pClasses->Get_Name(), _TL("Count"       )));
	m_pSize_Mean	->Set_Name(CSG_String::Format("%s [%s]", m_pClasses->Get_Name(), _TL("Average"     )));
	m_pSize_Skew	->Set_Name(CSG_String::Format("%s [%s]", m_pClasses->Get_Name(), _TL("Skewness"    )));
	m_pConnectivity	->Set_Name(CSG_String::Format("%s [%s]", m_pClasses->Get_Name(), _TL("Connectivity")));

	DataObject_Set_Colors(m_pDiversity   , 11, SG_COLORS_DEFAULT, false);
	DataObject_Set_Colors(m_pSize_Mean   , 11, SG_COLORS_DEFAULT,  true);
	DataObject_Set_Colors(m_pSize_Skew   , 11, SG_COLORS_DEFAULT,  true);
	DataObject_Set_Colors(m_pConnectivity, 11, SG_COLORS_DEFAULT,  true);

	//-----------------------------------------------------
	m_Search.Get_Weighting().Set_Parameters(&Parameters);
	m_Search.Get_Weighting().Set_BandWidth(Parameters("SEARCH_RADIUS")->asDouble() * m_Search.Get_Weighting().Get_BandWidth());
	m_Search.Set_Radius(Parameters("SEARCH_RADIUS")->asInt(), Parameters("SEARCH_MODE")->asInt() == 0);

	m_bWeighted	= m_Search.Get_Weighting().Get_Weighting() != SG_DISTWGHT_None;

	m_NB_Step	= Parameters("NB_CASE")->asInt() == 0 ? 2 : 1;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pClasses->is_NoData(x, y) || !Get_Diversity(x, y) )
			{
				m_pDiversity   ->Set_NoData(x, y);
				m_pSize_Mean   ->Set_NoData(x, y);
				m_pSize_Skew   ->Set_NoData(x, y);
				m_pConnectivity->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Search.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Analysis::Get_Diversity(int x, int y)
{
	int		i, nCells = 0, nConnections = 0, nNeighbours = 0;

	CSG_Class_Statistics_Weighted	Classes;

	for(i=0; i<m_Search.Get_Count(); i++)
	{
		int	ix	= m_Search.Get_X(i, x);
		int	iy	= m_Search.Get_Y(i, y);

		if( m_pClasses->is_InGrid(ix, iy) )
		{
			double	iz	= m_pClasses->asDouble(ix, iy);

			Classes.Add_Value(iz, m_bWeighted ? m_Search.Get_Weight(i) : 1.0);

			nCells++;

			for(int j=0; j<8; j+=m_NB_Step)
			{
				int	jx	= Get_xTo(j, ix);
				int	jy	= Get_yTo(j, iy);

				if( m_pClasses->is_InGrid(jx, jy) )
				{
					nNeighbours++;

					if( m_pClasses->asDouble(jx, jy) == iz )
					{
						nConnections++;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Classes.Get_Count() > 1 )
	{
		CSG_Simple_Statistics	s(true);

		for(i=0; i<Classes.Get_Count(); i++)
		{
			s.Add_Value(Classes.Get_Class_Count(i), m_bWeighted ? Classes.Get_Class_Weight(i) / Classes.Get_Class_Count(i) : 1.0);
		}

		m_pDiversity->Set_Value(x, y, s.Get_Sum() * s.Get_Weights());
		m_pSize_Mean->Set_Value(x, y, s.Get_Mean() / (double)nCells);	// relative size !!!
		m_pSize_Skew->Set_Value(x, y, s.Get_Skewness());	//	m_pSize_Skew->Set_Value(x, y, s.Get_SkewnessPearson());
	}
	else
	{
		m_pDiversity->Set_Value(x, y, 1.0);
		m_pSize_Mean->Set_Value(x, y, 1.0);
		m_pSize_Skew->Set_Value(x, y, 0.0);
	}

	m_pConnectivity->Set_Value(x, y, nNeighbours > 0 ? nConnections / (double)nNeighbours : 0.0);

	return( nCells > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
