
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Laplacian Filter"));

	Set_Author		(SG_T("(c) 2003 by A. Ringeler, (c) 2008 by O. Conrad"));

	Set_Description	(_TW(
		 "Other Common Names: Laplacian, Laplacian of Gaussian, LoG, Marr Filter\n"
		 "\n"
		 "Standard kernel 1 (3x3):\n"
		 " 0 | -1 |  0\n"
		 "-1 |  4 | -1\n"
		 " 0 | -1 |  0\n"
		 "\n"
		 "Standard kernel 2 (3x3):\n"
		 "-1 | -1 | -1\n"
		 "-1 |  8 | -1\n"
		 " 0 | -1 |  0\n"
		 "\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Standard Kernel 1"),
			_TL("Standard Kernel 2"),
			_TL("User defined")
		), 2
	);

	CSG_Parameter	*pNode	= Parameters.Add_Node(NULL, "NODE_USER", _TL("User defined"), _TL(""));

	Parameters.Add_Value(
		pNode	, "SIGMA"		, _TL("Standard Deviation (Percent of Radius)"),
		_TL(""),
		PARAMETER_TYPE_Double, 50.0, 0.00001, true
	);

	Parameters.Add_Value(
		pNode	, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Choice(
		pNode	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);
}

//---------------------------------------------------------
CFilter_LoG::~CFilter_LoG(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::On_Execute(void)
{
	int			Radius;
	CSG_Grid	*pResult;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();
	Radius		= Parameters("RADIUS")	->asInt();

	//-----------------------------------------------------
	if( Initialise(Parameters("METHOD")->asInt(), Radius, Parameters("SIGMA")->asDouble(), Parameters("MODE")->asInt() == 1) )
	{
		if( !pResult || pResult == m_pInput )
		{
			pResult	= SG_Create_Grid(m_pInput);
		}

		DataObject_Set_Colors(pResult, 100, SG_COLORS_BLACK_WHITE);

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( m_pInput->is_InGrid(x, y) )
				{
					pResult->Set_Value(x, y, Get_Mean(x, y, Radius));
				}
				else
				{
					pResult->Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
		if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == m_pInput )
		{
			m_pInput->Assign(pResult);

			delete(pResult);
		}

		m_Kernel.Destroy();

		return( true );
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
bool CFilter_LoG::Initialise(int Method, int &Radius, double Sigma, bool bCircle)
{
	switch( Method )
	{
	case 0:
		m_Kernel.Create(GRID_TYPE_Double, 3, 3);
		m_Kernel.Set_Value(0, 0,  0);	m_Kernel.Set_Value(0, 1, -1);	m_Kernel.Set_Value(0, 2,  0);
		m_Kernel.Set_Value(1, 0, -1);	m_Kernel.Set_Value(1, 1,  4);	m_Kernel.Set_Value(1, 2, -1);
		m_Kernel.Set_Value(2, 0,  0);	m_Kernel.Set_Value(2, 1, -1);	m_Kernel.Set_Value(2, 2,  0);
		Radius	= 1;
		return( true );

	case 1:
		m_Kernel.Create(GRID_TYPE_Double, 3, 3);
		m_Kernel.Set_Value(0, 0, -1);	m_Kernel.Set_Value(0, 1, -1);	m_Kernel.Set_Value(0, 2, -1);
		m_Kernel.Set_Value(1, 0, -1);	m_Kernel.Set_Value(1, 1,  8);	m_Kernel.Set_Value(1, 2, -1);
		m_Kernel.Set_Value(2, 0, -1);	m_Kernel.Set_Value(2, 1, -1);	m_Kernel.Set_Value(2, 2, -1);
		Radius	= 1;
		return( true );

	case 2:	default:
		if( Sigma > 0.0 )
		{
			m_Kernel.Create(GRID_TYPE_Double, 1 + 2 * Radius, 1 + 2 * Radius);

			Sigma	= SG_Get_Square(Radius * Sigma * 0.01);

		//	double	min		= 999999;
		//	double	max		= 0;

			for(int y=-Radius, iy=0; y<=Radius; y++, iy++)
			{
				for(int x=-Radius, ix=0; x<=Radius; x++, ix++)
				{
					double	d	= x * x + y * y;

					if( bCircle && d > Radius*Radius )
					{
						d	= 0.0;
					}
					else
					{
						d	= 1.0 / (M_PI * Sigma*Sigma) * (1.0 - d / (2.0 * Sigma)) * exp(-d / (2.0 * Sigma));
					}

					m_Kernel.Set_Value(ix, iy, d);

		//			if( min > k )	min	= k;
		//			if( max < k )	max	= k;
				}
			}

		//	this->DataObject_Add(SG_Create_Grid(m_Kernel));

			m_Kernel	+= -m_Kernel.Get_ArithMean();

		//	if( min / max > 0.367 / 2.0 )
		//	{
		//		Message_Add("Warning: Radius is to small for your Standard Deviation");
		//		return( false );
		//	}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_LoG::Get_Mean(int x, int y, int Radius)
{
	double	s, n, k;

	//-----------------------------------------------------
	s	= 0.0;
	n	= 0.0;

	//-----------------------------------------------------
	for(int ky=0, iy=y-Radius; ky<m_Kernel.Get_NY(); ky++, iy++)
	{
		for(int kx=0, ix=x-Radius; kx<m_Kernel.Get_NX(); kx++, ix++)
		{
			if( m_pInput->is_InGrid(ix, iy) && (k = m_Kernel.asDouble(kx, ky)) != 0.0 )
			{
				s	+= k * m_pInput->asDouble(ix, iy);
				n	+= fabs(k);
			}
		}
	}

	//-----------------------------------------------------
	return( n > 0.0 ? s / n : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
