
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
//                    Filter_Gauss.cpp                   //
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
#include "Filter_Gauss.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Gauss::CFilter_Gauss(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Gaussian Filter"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description	(_TW(
		"The Gauss Filter is a smoothing operator that is used to `blur' or 'soften' Grid Data\n"
		"and remove detail and noise.\n"
		"The degree of smoothing is determined by the standard deviation.\n"
		"For higher standard deviations you need a greater Radius\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "SIGMA"		, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double, 1, 0.0001, true
	);

	Parameters.Add_Choice(
		NULL, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Search Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);
}

//---------------------------------------------------------
CFilter_Gauss::~CFilter_Gauss(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Gauss::On_Execute(void)
{
	int			x, y, Mode, Radius;
	double		Sigma;
	CSG_Grid	*pResult;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();
	Sigma		= Parameters("SIGMA")	->asDouble();
	Mode		= Parameters("MODE")	->asInt();
	Radius		= Parameters("RADIUS")	->asInt();

	if( !pResult || pResult == m_pInput )
	{
		pResult	= SG_Create_Grid(m_pInput);

		Parameters("RESULT")->Set_Value(m_pInput);
	}

	//-----------------------------------------------------
	if( Initialise(Radius, Sigma, Mode) )
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( m_pInput->is_InGrid(x, y) )
				{
					pResult->Set_Value(x, y, Get_Mean(x, y, Radius));
				}
			}
		}

		//-------------------------------------------------
		if( m_pInput == Parameters("RESULT")->asGrid() )
		{
			m_pInput->Assign(pResult);

			delete(pResult);
		}

		m_Kernel.Destroy();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFilter_Gauss::Initialise(int Radius, double Sigma, int Mode)
{
	int		x, y;
	double	dx, dy, val, min, max;

	//-----------------------------------------------------
	m_Kernel.Create(GRID_TYPE_Double, 1 + 2 * Radius, 1 + 2 * Radius);

	//-----------------------------------------------------
	for(y=0, dy=-Radius, min=1.0, max=0.0; y<m_Kernel.Get_NY(); y++, dy++)
	{
		for(x=0, dx=-Radius; x<m_Kernel.Get_NX(); x++, dx++)
		{
			switch( Mode )
			{
			case 1:
				val	= sqrt(dx*dx + dy*dy) > Radius
					? 0.0
					: exp(-(dx*dx + dy*dy) / (2.0 * Sigma*Sigma)) / (M_PI * 2.0 * Sigma*Sigma);
				break;

			case 0:
				val	= exp(-(dx*dx + dy*dy) / (2.0 * Sigma*Sigma)) / (M_PI * 2.0 * Sigma*Sigma);
				break;
			}

			m_Kernel.Set_Value(x, y, val);

			if( min > max )
			{
				min	= max	= val;
			}
			else if( val < min )
			{
				min	= val;
			}
			else if( val > max )
			{
				max	= val;
			}
		}
	}

	//-----------------------------------------------------
	if( max == 0.0 )
	{
		Message_Dlg(_TL("Radius is too small"));
	}
	else if( min / max > 0.367 / 2.0 )
	{
		Message_Dlg(_TL("Radius is too small for your Standard Deviation"), Get_Name());
	}
	else
	{
		return( true );
	}

	m_Kernel.Destroy();

	return( false );
}

//---------------------------------------------------------
double CFilter_Gauss::Get_Mean(int x, int y, int Radius)
{
	int		ix, iy, jx, jy;
	double	Result, Kernel_Sum, Kernel;

	Result		= 0.0;
	Kernel_Sum	= 0.0;

	//-----------------------------------------------------
	for(jy=0, iy=y-Radius; jy<m_Kernel.Get_NY(); jy++, iy++)
	{
		for(jx=0, ix=x-Radius; jx<m_Kernel.Get_NX(); jx++, ix++)
		{
			if( (Kernel = m_Kernel.asDouble(jx, jy)) > 0.0 && m_pInput->is_InGrid(ix, iy) )
			{
				Result		+= Kernel * m_pInput->asDouble(ix, iy);
				Kernel_Sum	+= Kernel;
			}
		}
	}

	//-----------------------------------------------------
	return( Kernel_Sum > 0.0 ? Result / Kernel_Sum : m_pInput->Get_NoData_Value() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
