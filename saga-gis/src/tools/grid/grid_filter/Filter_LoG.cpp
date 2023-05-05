
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
//                 Copyright (C) 2023 by                 //
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
		"The Laplacian filter is a high-pass filter operator that is "
		"commonly used for edge detection. Also referred to as "
		"Laplacian of Gaussian (LoG) or Marr-Hildreth-Operator.\n"
		"The kernel can be defined by search radius and weighting "
		"function adjustment or be chosen from predefined standards.\n"
		"\n"
		"Standard kernel 1 (3x3):\n<table border=\"1\" align=\"center\">"
		"<tr><td> 0</td><td>-1</td><td> 0</td></tr>"
		"<tr><td>-1</td><td> 4</td><td>-1</td></tr>"
		"<tr><td> 0</td><td>-1</td><td> 0</td></tr>"
		"</table>\n"
		"\n"
		"Standard kernel 1 (3x3):\n<table border=\"1\" align=\"center\">"
		"<tr><td>-1</td><td>-1</td><td>-1</td></tr>"
		"<tr><td>-1</td><td> 8</td><td>-1</td></tr>"
		"<tr><td>-1</td><td>-1</td><td>-1</td></tr>"
		"</table>\n"
		"\n"
		"Standard kernel 1 (3x3):\n<table border=\"1\" align=\"center\">"
		"<tr><td>-1</td><td>-2</td><td>-1</td></tr>"
		"<tr><td>-2</td><td>12</td><td>-2</td></tr>"
		"<tr><td>-1</td><td>-2</td><td>-1</td></tr>"
		"</table>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("standard kernel 1"),
			_TL("standard kernel 2"),
			_TL("Standard kernel 3"),
			_TL("user defined kernel")
		), 3
	);

	Parameters.Add_Int("METHOD",
		"KERNEL_RADIUS"	, _TL("Kernel Radius"),
		_TL(""),
		2, 1, true
	);

	Parameters.Add_Double("METHOD",
		"SIGMA"			, _TL("Standard Deviation"),
		_TL("The standard deviation as percentage of the kernel radius."),
		50., 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFilter_LoG::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("KERNEL_RADIUS", pParameter->asInt() == 3);
		pParameters->Set_Enabled("SIGMA"        , pParameter->asInt() == 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::On_Execute(void)
{
	CSG_Matrix Kernel;

	if( !Get_Kernel(Kernel) )
	{
		Error_Set(_TL("Kernel initialization failed!"));

		return( false );
	}

	int Radius = (Kernel.Get_NX() - 1) / 2;

	//-----------------------------------------------------
	CSG_Grid Input, *pInput = Parameters("INPUT")->asGrid();

	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	if( !pResult || pResult == pInput )
	{
		Input.Create(*pInput); pResult = pInput; pInput = &Input;
	}
	else
	{
		DataObject_Set_Colors(pResult, 11, SG_COLORS_BLACK_WHITE);

		pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Laplacian Filter"));

		pResult->Set_NoData_Value(pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pInput->is_NoData(x, y) )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				double s = 0.;

				for(int i=0, iy=y-Radius; i<Kernel.Get_NY(); i++, iy++)
				{
					for(int j=0, ix=x-Radius; j<Kernel.Get_NX(); j++, ix++)
					{
						if( Kernel[i][j] )
						{
							s += Kernel[i][j] * (pInput->is_InGrid(ix, iy) ? pInput->asDouble(ix, iy) : pInput->asDouble(x, y));
						}
					}
				}

				pResult->Set_Value(x, y, s);
			}
		}
	}

	//-----------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG::Get_Kernel(CSG_Matrix &Kernel)
{
	const double Standard[3][9] =
	{	{  0, -1,  0,
		  -1,  4, -1,
		   0, -1,  0
		},
		{ -1, -1, -1,
		  -1,  8, -1,
		  -1, -1, -1
		},
		{ -1, -2, -1,
		  -2, 12, -2,
		  -1, -2, -1
		}
	};

	switch( Parameters("METHOD")->asInt() )
	{
	case  0: Kernel.Create(3, 3, Standard[0]); break;
	case  1: Kernel.Create(3, 3, Standard[1]); break;
	case  2: Kernel.Create(3, 3, Standard[2]); break;

	default: {
		double Sigma = Parameters("SIGMA")->asDouble() / 100.;

		if( Sigma <= 0. )
		{
			return( false );
		}

		int Radius = Parameters("KERNEL_RADIUS")->asInt();

		Kernel.Create(1 + 2 * (sLong)Radius, 1 + 2 * (sLong)Radius);

		double s2 = SG_Get_Square(Radius * Sigma);

		CSG_Simple_Statistics s;

		for(int i=0; i<Kernel.Get_NY(); i++)
		{
			for(int j=0; j<Kernel.Get_NX(); j++)
			{
				double d = SG_Get_Square((double)i - Radius) + SG_Get_Square((double)j - Radius);

				s += Kernel[i][j] = (1. / (M_PI * s2*s2)) * exp(-d / (2. * s2)) * (1. - d / (2. * s2));
			}
		}

		Kernel -= s.Get_Mean();

		break; }
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_LoG_Sharpening::CFilter_LoG_Sharpening(void)
{
	Set_Name		(_TL("Sharpening Filter"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"This Sharpening filter uses a Laplacian filter to detect "
		"the high frequencies in the supplied grid and adds it to "
		"the original values. "
	));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_LoG_Sharpening::On_Execute(void)
{
	CSG_Matrix Kernel;

	if( !Get_Kernel(Kernel) )
	{
		Error_Set(_TL("Kernel initialization failed!"));

		return( false );
	}

	int Radius = (Kernel.Get_NX() - 1) / 2;

	//-----------------------------------------------------
	CSG_Grid Input, *pInput = Parameters("INPUT")->asGrid();

	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	if( !pResult || pResult == pInput )
	{
		Input.Create(*pInput); pResult = pInput; pInput = &Input;
	}
	else
	{
		DataObject_Set_Parameters(pResult, pInput);

		pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Sharpening Filter"));

		pResult->Set_NoData_Value(pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pInput->is_NoData(x, y) )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				double s = pInput->asDouble(x, y);

				for(int i=0, iy=y-Radius; i<Kernel.Get_NY(); i++, iy++)
				{
					for(int j=0, ix=x-Radius; j<Kernel.Get_NX(); j++, ix++)
					{
						if( Kernel[i][j] )
						{
							s += Kernel[i][j] * (pInput->is_InGrid(ix, iy) ? pInput->asDouble(ix, iy) : pInput->asDouble(x, y));
						}
					}
				}

				pResult->Set_Value(x, y, s);
			}
		}
	}

	//-----------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
