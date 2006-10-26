
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

	Set_Description(_TL(
		"The Gauss Filter is a smoothing operator that is used to `blur' or 'soften' Grid Data\n"
		"and remove detail and noise.\n"
		"The degree of smoothing is determined by the standard deviation.\n"
		"For higher standard deviations you need a greater Radius\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "SIGMA"		, _TL("Standard Deviation"),
		"",
		PARAMETER_TYPE_Double, 1, 0.0001, true
	);

	Parameters.Add_Choice(
		NULL, "SEARCH_MODE"	, _TL("Search Mode"),
		"",
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Search Radius"),
		"",
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
double CFilter_Gauss::Gauss_Function(double x, double y)
{
	return exp(- (x*x+y*y) / (2*m_sigma*m_sigma)) / (2*M_PI*m_sigma*m_sigma);
}

void CFilter_Gauss::Init_Kernel(int Radius)
{
	int x,y;
	double val, min, max;

	min = 999999;
	max = 0;
	
	pKernel = (CSG_Grid *) new CSG_Grid( GRID_TYPE_Double , 1+2*Radius, 1+2*Radius);

	for(y=-Radius; y<=Radius ; y++)
	{
		for(x=-Radius; x<=Radius; x++)
		{
			val = Gauss_Function(x , y);
			pKernel->Set_Value(x+Radius, y+Radius, val);
			if (min > val)
				min =val;

			if (max < val)
				max =val;
		}
	}
	
	if (min/max > 0.367/2.0)
		Message_Add(_TL("Radius is too small for your Standard Deviation"));
}

//---------------------------------------------------------
bool CFilter_Gauss::On_Execute(void)
{
	int		x, y, Mode,  Radius;

	double	Mean;

	CSG_Grid	*pResult;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT")->asGrid();

	if( !Parameters("RESULT")->asGrid() )
	{
		Parameters("RESULT")->Set_Value(pInput);
	}

	pResult		= Parameters("RESULT")->asGrid();

	if( !pResult || pResult == pInput )
	{
		pResult	= SG_Create_Grid(pInput);
	}

	Mode		= Parameters("SEARCH_MODE")->asInt();
	m_sigma		= Parameters("SIGMA")->asDouble();
	Radius		= Parameters("RADIUS")->asInt();

	switch( Mode )
	{
	case 0:	break;
	case 1:	m_Radius.Create(1 + Radius);	break;
	}

	Init_Kernel( Radius );

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pInput->is_InGrid(x, y) )
			{
				switch( Mode )
				{
				case 0:
					Mean	= Get_Mean_Square(x, y, Radius);
					break;

				case 1:
					Mean	= Get_Mean_Circle(x, y);
					break;
				}

					pResult->Set_Value(x, y, Mean);
			}
		}
	}

	//-----------------------------------------------------
	if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == pInput )
	{
		pInput->Assign(pResult);

		delete(pResult);
	}

	m_Radius.Destroy();

	return( true );
}

//---------------------------------------------------------
double CFilter_Gauss::Get_Mean_Square(int x, int y, int Radius)
{
	int		ax, ay, bx, by, ix, iy;
	double	Result;
	double	Kernel_Sum;

	Result	= 0.0;
	Kernel_Sum	= 0.0;

	//-----------------------------------------------------
	ax		= x - Radius;
	bx		= x + Radius;

	if( ax < 0 )
	{
		ax	= 0;
	}
	else if( bx >= Get_NX() )
	{
		bx	= Get_NX() - 1;
	}

	ay		= y - Radius;
	by		= y + Radius;

	if( ay < 0 )
	{
		ay	= 0;
	}
	else if( by >= Get_NY() )
	{
		by	= Get_NY() - 1;
	}

	//-----------------------------------------------------
	for(iy=ay; iy<=by; iy++)
	{
		for(ix=ax; ix<=bx; ix++)
		{
			if( pInput->is_InGrid(ix, iy) )
			{
				Result	+= pInput->asDouble(ix, iy)* pKernel->asDouble(Radius + ix-x,Radius+ iy-y);
				Kernel_Sum += pKernel->asDouble(Radius +ix-x,Radius+ iy-y);
			}
		}
	}

	//-----------------------------------------------------
	if( Kernel_Sum > 0.0 )
	{
		Result	/=  Kernel_Sum;
	}

	return( Result );
}

//---------------------------------------------------------
double CFilter_Gauss::Get_Mean_Circle(int x, int y)
{
	int		iPoint, ix, iy, Radius;
	double	Result,Kernel_Sum;

	Radius = m_Radius.Get_Maximum()-1;
	Result	= 0.0;
	Kernel_Sum		= 0.0;

	//-----------------------------------------------------
	for(iPoint=0; iPoint<m_Radius.Get_nPoints(); iPoint++)
	{
		m_Radius.Get_Point(iPoint, x, y, ix, iy);

		if( pInput->is_InGrid(ix, iy) )
		{
			Result	+= pInput->asDouble(ix, iy)* pKernel->asDouble(Radius+ix-x,Radius+ iy-y);
			Kernel_Sum += pKernel->asDouble(Radius+ix-x,Radius+ iy-y);
		}
	}

	//-----------------------------------------------------
	if( Kernel_Sum > 0 )
	{
		Result	/= (double)Kernel_Sum;
	}

	return( Result );
}
