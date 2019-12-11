
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
//                    Filter_LoG.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Filter_LoG.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_LoG::CFilter_LoG(void)
{
	Set_Name		(_TL("Laplacian Filter"));

	Set_Author		("A.Ringeler (c) 2003, O.Conrad (c) 2008");

	Set_Description	(_TW(
		 "Other Common Names: Laplacian, Laplacian of Gaussian, LoG, Marr Filter\n"
		 "\n"
		 "Standard kernel 1 (3x3):\n"
		 " 0 | -1 |  0\n"
		 "-- + -- + --\n"
		 "-1 |  4 | -1\n"
		 "-- + -- + --\n"
		 " 0 | -1 |  0\n"
		 "\n"
		 "Standard kernel 2 (3x3):\n"
		 "-1 | -1 | -1\n"
		 "-- + -- + --\n"
		 "-1 |  8 | -1\n"
		 "-- + -- + --\n"
		 "-1 | -1 | -1\n"
		 "\n"
		 "Standard kernel 3 (3x3):\n"
		 "-1 | -2 | -1\n"
		 "-- + -- + --\n"
		 "-2 | 12 | -2\n"
		 "-- + -- + --\n"
		 "-1 | -2 | -1\n"
		 "\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("standard kernel 1"),
			_TL("standard kernel 2"),
			_TL("Standard kernel 3"),
			_TL("user defined kernel")
		), 3
	);

	Parameters.Add_Double("",
		"SIGMA"		, _TL("Standard Deviation"),
		_TL("The standard deviation, expressed as a percentage of the radius."),
		50., 0.00001, true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFilter_LoG::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("SIGMA"        , pParameter->asInt() == 3);
		pParameters->Set_Enabled("KERNEL_RADIUS", pParameter->asInt() == 3);
		pParameters->Set_Enabled("KERNEL_TYPE"  , pParameter->asInt() == 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::On_After_Execution(void)
{
	if( Parameters("RESULT")->asGrid() == Parameters("INPUT")->asGrid() )
	{
		Parameters("RESULT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::On_Execute(void)
{
	if( !Initialise() )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")->asGrid();

	CSG_Grid	Result, *pResult	= Parameters("RESULT")->asGrid();

	if( !pResult || pResult == m_pInput )
	{
		pResult	= &Result;
		
		pResult->Create(m_pInput);
	}
	else
	{
		pResult->Fmt_Name("%s [%s]", m_pInput->Get_Name(), _TL("Laplace Filter"));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_InGrid(x, y) )
			{
				pResult->Set_Value(x, y, Get_Value(x, y));
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( pResult == &Result )
	{
		CSG_MetaData	History	= m_pInput->Get_History();

		m_pInput->Assign(pResult);
		m_pInput->Get_History() = History;

		DataObject_Update(m_pInput);

		Parameters("RESULT")->Set_Value(m_pInput);
	}

	DataObject_Set_Colors(pResult, 11, SG_COLORS_BLACK_WHITE);

	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::Initialise(void)
{
	bool	bCircle	= Parameters("KERNEL_TYPE")->asInt() == 1;

	double	Sigma	= Parameters("SIGMA")->asDouble();

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:
		m_Radius	= 1;
		m_Kernel.Create(SG_DATATYPE_Double, 3, 3);
		m_Kernel.Set_Value(0, 0,  0);	m_Kernel.Set_Value(0, 1, -1);	m_Kernel.Set_Value(0, 2,  0);
		m_Kernel.Set_Value(1, 0, -1);	m_Kernel.Set_Value(1, 1,  4);	m_Kernel.Set_Value(1, 2, -1);
		m_Kernel.Set_Value(2, 0,  0);	m_Kernel.Set_Value(2, 1, -1);	m_Kernel.Set_Value(2, 2,  0);
		break;

	case 1:
		m_Radius	= 1;
		m_Kernel.Create(SG_DATATYPE_Double, 3, 3);
		m_Kernel.Set_Value(0, 0, -1);	m_Kernel.Set_Value(0, 1, -1);	m_Kernel.Set_Value(0, 2, -1);
		m_Kernel.Set_Value(1, 0, -1);	m_Kernel.Set_Value(1, 1,  8);	m_Kernel.Set_Value(1, 2, -1);
		m_Kernel.Set_Value(2, 0, -1);	m_Kernel.Set_Value(2, 1, -1);	m_Kernel.Set_Value(2, 2, -1);
		break;

	case 2:
		m_Radius	= 1;
		m_Kernel.Create(SG_DATATYPE_Double, 3, 3);
		m_Kernel.Set_Value(0, 0, -1);	m_Kernel.Set_Value(0, 1, -2);	m_Kernel.Set_Value(0, 2, -1);
		m_Kernel.Set_Value(1, 0, -2);	m_Kernel.Set_Value(1, 1, 12);	m_Kernel.Set_Value(1, 2, -2);
		m_Kernel.Set_Value(2, 0, -1);	m_Kernel.Set_Value(2, 1, -2);	m_Kernel.Set_Value(2, 2, -1);
		break;

	case 3:	default:
		m_Radius	= Parameters("KERNEL_RADIUS")->asInt();

		if( Sigma <= 0. )
		{
			return( false );
		}

		m_Kernel.Create(SG_DATATYPE_Double, 1 + 2 * m_Radius, 1 + 2 * m_Radius);

		Sigma	= SG_Get_Square(m_Radius * Sigma * 0.01);

		for(int y=-m_Radius, iy=0; y<=m_Radius; y++, iy++)
		{
			for(int x=-m_Radius, ix=0; x<=m_Radius; x++, ix++)
			{
				double	d	= x*x + y*y;

				if( bCircle && d > m_Radius*m_Radius )
				{
					m_Kernel.Set_NoData(ix, iy);
				}
				else
				{
					m_Kernel.Set_Value(ix, iy, 1. / (M_PI * Sigma*Sigma) * (1. - d / (2. * Sigma)) * exp(-d / (2. * Sigma)));
				}
			}
		}

		m_Kernel	+= -m_Kernel.Get_Mean();

		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_LoG::Get_Value(int x, int y)
{
	double	s	= 0.;

	for(int ky=0, iy=y-m_Radius; ky<m_Kernel.Get_NY(); ky++, iy++)
 	{
		for(int kx=0, ix=x-m_Radius; kx<m_Kernel.Get_NX(); kx++, ix++)
		{
			if( !m_Kernel.is_NoData(kx, ky) )
			{
				s	+= m_Kernel.asDouble(kx, ky) * (m_pInput->is_InGrid(ix, iy) ? m_pInput->asDouble(ix, iy) : m_pInput->asDouble(x, y));
			}
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
