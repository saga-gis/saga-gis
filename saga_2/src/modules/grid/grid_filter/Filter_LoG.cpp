
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
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("standard kernel 1"),
			_TL("standard kernel 2"),
			_TL("Standard kernel 3"),
			_TL("user defined kernel")
		), 3
	);

	CSG_Parameter	*pNode	= Parameters.Add_Node(NULL, "NODE_USER", _TL("User Defined Kernel"), _TL(""));

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
			_TL("square"),
			_TL("circle")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::On_Execute(void)
{
	CSG_Grid	*pResult;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();

	//-----------------------------------------------------
	if( Initialise() )
	{
		if( !pResult || pResult == m_pInput )
		{
			pResult	= SG_Create_Grid(m_pInput);
		}
		else
		{
			pResult->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Laplace Filter")));

			pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
		}

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
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

		//-------------------------------------------------
		if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == m_pInput )
		{
			m_pInput->Assign(pResult);

			delete(pResult);

			pResult	= m_pInput;
		}

		DataObject_Set_Colors(pResult, 100, SG_COLORS_BLACK_WHITE);

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
bool CFilter_LoG::Initialise(void)
{
	int		Method		= Parameters("METHOD")	->asInt();
	bool	bCircle		= Parameters("MODE")	->asInt() == 1;
	double	Sigma		= Parameters("SIGMA")	->asDouble();

	switch( Method )
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
		m_Radius	= Parameters("RADIUS")->asInt();

		if( Sigma <= 0.0 )
		{
			return( false );
		}

		m_Kernel.Create(SG_DATATYPE_Double, 1 + 2 * m_Radius, 1 + 2 * m_Radius);

		Sigma	= SG_Get_Square(m_Radius * Sigma * 0.01);

		for(int y=-m_Radius, iy=0; y<=m_Radius; y++, iy++)
		{
			for(int x=-m_Radius, ix=0; x<=m_Radius; x++, ix++)
			{
				double	d	= x * x + y * y;

				if( bCircle && d > m_Radius*m_Radius )
				{
					m_Kernel.Set_NoData(ix, iy);
				}
				else
				{
					m_Kernel.Set_Value(ix, iy, 1.0 / (M_PI * Sigma*Sigma) * (1.0 - d / (2.0 * Sigma)) * exp(-d / (2.0 * Sigma)));
				}
			}
		}

		m_Kernel	+= -m_Kernel.Get_ArithMean();

		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_LoG::Get_Value(int x, int y)
{
	double	s	= 0.0;

	//-----------------------------------------------------
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

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
