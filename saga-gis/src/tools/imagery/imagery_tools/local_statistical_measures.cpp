
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

	Parameters.Add_Grid("", "GRID"    , _TL("Grid"    ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid("", "CONTRAST", _TL("Contrast"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "ENERGY"  , _TL("Energy"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "ENTROPY" , _TL("Entropy" ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "VARIANCE", _TL("Variance"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Kernel"),
		_TL("kernel radius in cells"),
		CSG_String::Format("%s|%s|",
			_TL("square"),
			_TL("circle")
		), 1
	);

	Parameters.Add_Int("",
		"RADIUS"	, _TL("Radius"),
		_TL("kernel radius in cells"),
		1, 1, true
	);

	Parameters.Add_Choice("",
		"NORMALIZE"	, _TL("Normalization"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("no"),
			_TL("scale to range")
		), 1
	);

	Parameters.Add_Double("NORMALIZE",
		"NORM_MIN"	, _TL("Minimum"),
		_TL(""),
		1.0
	);

	Parameters.Add_Double("NORMALIZE",
		"NORM_MAX"	, _TL("Maximum"),
		_TL(""),
		255.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLocal_Statistical_Measures::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NORMALIZE") )
	{
		pParameters->Set_Enabled("NORM_MIN", pParameter->asInt() == 1);
		pParameters->Set_Enabled("NORM_MAX", pParameter->asInt() == 1);
	}

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

	if( m_pGrid->Get_Range() <= 0.0 )
	{
		Error_Set(_TL("nothing to do, input data has no variation."));

		return( false );
	}

	//-----------------------------------------------------
	m_pContrast		= Parameters("CONTRAST"   )->asGrid();
	m_pEnergy		= Parameters("ENERGY"     )->asGrid();
	m_pEntropy		= Parameters("ENTROPY"    )->asGrid();
	m_pVariance		= Parameters("VARIANCE"   )->asGrid();

	DataObject_Set_Colors(m_pContrast   , 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pEnergy     , 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pEntropy    , 11, SG_COLORS_RAINBOW);
	DataObject_Set_Colors(m_pVariance   , 11, SG_COLORS_RAINBOW);

	//-----------------------------------------------------
	m_Kernel.Get_Weighting().Set_Parameters(&Parameters);
	m_Kernel.Set_Radius(m_Radius = Parameters("RADIUS")->asInt(), Parameters("TYPE")->asInt() == 0);

	//-----------------------------------------------------
	m_Normalize	=  Parameters("NORMALIZE")->asInt();
	m_Minimum	=  Parameters("NORM_MIN" )->asDouble();
	m_Scale		= (Parameters("NORM_MAX" )->asDouble() - m_Minimum) / m_pGrid->Get_Range();

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
inline bool CLocal_Statistical_Measures::Get_Value(int x, int y, double &z)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		switch( m_Normalize )
		{
		default:
			z	= m_pGrid->asDouble(x, y);
			break;

		case  1:
			z	= m_Minimum + m_Scale * (m_pGrid->asDouble(x, y) - m_pGrid->Get_Min());
			break;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLocal_Statistical_Measures::Get_Measure(int x, int y)
{
	//-----------------------------------------------------
	CSG_Simple_Statistics	s;

	double	Energy		= 0.0;
	double	Entropy		= 0.0;

	double	z;

	//-----------------------------------------------------
	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i);
		int	iy	= m_Kernel.Get_Y(i);

		if( Get_Value(x + ix, y + iy, z) )
		{
			s			+= z;
			Energy		+= z*z;
			Entropy		+= z * log(z > 0.0 ? z : M_ALMOST_ZERO);
		}
	}

	//-----------------------------------------------------
	if( s.Get_Count() > 0 )
	{
		Get_Value(x, y, z);

		m_pContrast   ->Set_Value(x, y, s.Get_Mean() ? (z - s.Get_Mean()) / s.Get_Mean() : 0.0);	// Weber
	//	m_pContrast   ->Set_Value(x, y, s.Get_Range() ? s.Get_Range() / (s.Get_Minimum() + s.Get_Maximum()) : 0.0);	// Michelson
		m_pEnergy     ->Set_Value(x, y, Energy     );
		m_pEntropy    ->Set_Value(x, y, Entropy    );
		m_pVariance   ->Set_Value(x, y, s.Get_Variance());

		return( true );
	}

	m_pContrast   ->Set_NoData(x, y);
	m_pEnergy     ->Set_NoData(x, y);
	m_pEntropy    ->Set_NoData(x, y);
	m_pVariance   ->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
