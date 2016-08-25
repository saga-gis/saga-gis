
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             local_statistical_measures.cpp            //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "local_statistical_measures.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLocal_Statistical_Measures::CLocal_Statistical_Measures(void)
{
	Set_Name		(_TL("Local Statistical Measures"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"<hr><h4>References</h4><ul>"
		"<li><b>Zhang, Y. (2001):</b> Texture-integrated classification of urban treed areas in high-resolution color-infrared imagery."
		" Photogrammetric Engineering and Remote Sensing 67(12), 1359-1365."
		" <a href=\"http://web.pdx.edu/~nauna/2001_dec_1359-1365.pdf\">online</a>.</li>"
		"</ul>"
	));

	Parameters.Add_Grid(NULL, "GRID"       , _TL("Grid"       ), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Grid(NULL, "CONTRAST"   , _TL("Contrast"   ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "ENERGY"     , _TL("Energy"     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "ENTROPY"    , _TL("Entropy"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "HOMOGENEITY", _TL("Homogeneity"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "VARIANCE"   , _TL("Variance"   ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Kernel"),
		_TL("kernel radius in cells"),
		CSG_String::Format("%s|%s|",
			_TL("square"),
			_TL("circle")
		), 1
	);

	Parameters.Add_Int(
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL("kernel radius in cells"),
		PARAMETER_TYPE_Int, 0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLocal_Statistical_Measures::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLocal_Statistical_Measures::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrid	= Parameters("GRID")->asGrid();

	m_pContrast		= Parameters("CONTRAST"   )->asGrid();
	m_pEnergy		= Parameters("ENERGY"     )->asGrid();
	m_pEntropy		= Parameters("ENTROPY"    )->asGrid();
	m_pHomogeneity	= Parameters("HOMOGENEITY")->asGrid();
	m_pVariance		= Parameters("VARIANCE"   )->asGrid();

	DataObject_Set_Colors(m_pContrast   , 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pEnergy     , 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pEntropy    , 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pHomogeneity, 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pVariance   , 11, SG_COLORS_RAINBOW);

	//-----------------------------------------------------
	m_Kernel.Get_Weighting().Set_Parameters(&Parameters);
	m_Kernel.Set_Radius(m_Radius = Parameters("RADIUS")->asInt(), Parameters("TYPE")->asInt() == 0);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Measure(x, y);
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
bool CLocal_Statistical_Measures::Get_Measure(int x, int y)
{
	//-----------------------------------------------------
	CSG_Simple_Statistics	Statistics;

	double	Contrast	= 0.0;
	double	Energy		= 0.0;
	double	Entropy		= 0.0;
	double	Homogeneity	= 0.0;

	//-----------------------------------------------------
	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i);
		int	iy	= m_Kernel.Get_Y(i);

		if( m_pGrid->is_InGrid(x + ix, y + iy) )
		{
			double	z	= m_pGrid->asDouble(x + ix, y + iy);

			int	i_less_j	= (ix + m_Radius) - (iy + m_Radius);

			Statistics	+= z;
			Contrast	+= z * SG_Get_Square(i_less_j);
			Energy		+= z*z;
			Entropy		+= z * log(z > 0.0 ? z : M_ALMOST_ZERO);
			Homogeneity	+= z / (1.0 + abs(i_less_j));
		}
	}

	//-----------------------------------------------------
	if( Statistics.Get_Count() > 0 )
	{
		m_pContrast   ->Set_Value(x, y, Contrast   );
		m_pEnergy     ->Set_Value(x, y, Energy     );
		m_pEntropy    ->Set_Value(x, y, Entropy    );
		m_pHomogeneity->Set_Value(x, y, Homogeneity);
		m_pVariance   ->Set_Value(x, y, Statistics.Get_Variance());

		return( true );
	}

	m_pContrast   ->Set_NoData(x, y);
	m_pEnergy     ->Set_NoData(x, y);
	m_pEntropy    ->Set_NoData(x, y);
	m_pHomogeneity->Set_NoData(x, y);
	m_pVariance   ->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
