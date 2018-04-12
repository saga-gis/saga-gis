
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                image_quality_index.cpp                //
//                                                       //
//                Marvin Ambrosi (C) 2017                //
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
//    e-mail:     marvin_ambrosi@web.de                  //
//                                                       //
//    contact:    Marvin Ambrosi                         //
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
#include "image_quality_index.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CImage_Quality_Index::CImage_Quality_Index(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Universal Image Quality Index"));

	Set_Author		("M.Ambrosi (c) 2017");

	Set_Description	(_TW(
		"The Universal Image Quality Index compares two grids (greyscale images) "
		"using the three parameters luminance, contrast and structure. This is done "
		"for each pixel using a moving window as specified by the kernel radius. "
	));

	Add_Reference(
		"Lasaponara, R. & Masini, N.", "2012",
		"Image Enhancement, Feature Extraction and Geospatial Analysis in an Archaeological Perspective",
		"In: Lasaponara, R. & Masini, N. [Eds.]: Satellite Remote Sensing - A New Tool for Archaeology, S. 17-64.",
		SG_T("https://link.springer.com/book/10.1007/978-90-481-8801-7"), SG_T("Springer Link")
	);

	Add_Reference(
		"Wang, Z. & Bovik, A.C.", "2002", 
		"A universal image quality index",
		"IEEE Signal Processing Letters, vol.9, no.3, pp.81-84.",
		SG_T("http://ieeexplore.ieee.org/document/995823/"), SG_T("IEEE-Xplore")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID_A"		, _TL("First Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"GRID_B"		, _TL("Second Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"QUALITY"		, _TL("Universal Image Quality Index"),
		_TL("The product of correlation, luminance and contrast similarity."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"CORRELATION"	, _TL("Correlation"),
		_TL("The correlation coefficient."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"LUMINANCE"		, _TL("Luminance"),
		_TL("The similarity of mean luminance."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"CONTRAST"		, _TL("Contrast"),
		_TL("The similarity of contrast."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"K1"	, _TL("k1"),
		_TL(""),
		0.01, 0., true, 1., true
	);

	Parameters.Add_Double("",
		"K2"	, _TL("k2"),
		_TL(""),
		0.03, 0., true, 1., true
	);

	Parameters.Add_Int("",
		"L"		, _TL("L"),
		_TL("The dynamic range for the image pixel, i.e. the number of different grey values."),
		255, 1, true
	);

	//-----------------------------------------------------
	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CImage_Quality_Index::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CImage_Quality_Index::On_Execute(void)
{
	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	m_pA	= Parameters("GRID_A")->asGrid();
	m_pB	= Parameters("GRID_B")->asGrid();

	m_c1	= M_SQR(Parameters("K1")->asDouble() * Parameters("L")->asInt());
	m_c2	= M_SQR(Parameters("K2")->asDouble() * Parameters("L")->asInt());

	CSG_Grid	*pQ	= Parameters("QUALITY"    )->asGrid();
	CSG_Grid	*pR	= Parameters("CORRELATION")->asGrid();
	CSG_Grid	*pL	= Parameters("LUMINANCE"  )->asGrid();
	CSG_Grid	*pC	= Parameters("CONTRAST"   )->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			double	r, l, c;

			if( Get_Quality(x, y, r, l, c) )
			{
				if( pQ ) pQ->Set_Value(x, y, r * l * c);
				if( pR ) pR->Set_Value(x, y, r);
				if( pL ) pL->Set_Value(x, y, l);
				if( pC ) pC->Set_Value(x, y, c);
			}
			else
			{
				if( pQ ) pQ->Set_NoData(x, y);
				if( pR ) pR->Set_NoData(x, y);
				if( pL ) pL->Set_NoData(x, y);
				if( pC ) pC->Set_NoData(x, y);
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
bool CImage_Quality_Index::Get_Quality(int x, int y, double &r, double &l, double &c)
{
	if( !m_pA->is_NoData(x, y) && !m_pB->is_NoData(x, y) )
	{
		int	i;	CSG_Simple_Statistics	A(true), B(true);

		for(i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pA->is_InGrid(ix, iy) && m_pB->is_InGrid(ix, iy) )
			{
				A	+= m_pA->asDouble(ix, iy);
				B	+= m_pB->asDouble(ix, iy);
			}
		}

		//-------------------------------------------------
		if( A.Get_Count() > 0 )
		{
			double	cov	= 0.0;

			for(i=0; i<A.Get_Count(); i++)
			{
				cov	+= (A.Get_Value(i) - A.Get_Mean())
					*  (B.Get_Value(i) - B.Get_Mean());
			}

			cov	/= A.Get_Count();

			r	= (                  cov               + m_c2 / 2) / (      A.Get_StdDev  ()  *       B.Get_StdDev  ()  + m_c2 / 2);
			l	= (2 * A.Get_Mean  () * B.Get_Mean  () + m_c1    ) / (M_SQR(A.Get_Mean    ()) + M_SQR(B.Get_Mean    ()) + m_c1    );
			c	= (2 * A.Get_StdDev() * B.Get_StdDev() + m_c2    ) / (      A.Get_Variance()  +       B.Get_Variance()  + m_c2    );

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
