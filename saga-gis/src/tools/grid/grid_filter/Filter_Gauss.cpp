
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
//                    Filter_Gauss.cpp                   //
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
#include "Filter_Gauss.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Gauss::CFilter_Gauss(void)
{
	Set_Name		(_TL("Gaussian Filter"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"The Gaussian filter is a low-pass filter operator that is used to 'blur' "
		"or 'soften' data and to remove detail and noise.\n"
		"The degree of smoothing is determined by the kernel size specified as "
		"radius and the weighting of each raster cell within the kernel. The "
		"weighting scheme uses the Gaussian bell curve function and can be adjusted "
		"to the kernel size with the 'Standard Deviation' option. "
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

	Parameters.Add_Int("",
		"KERNEL_RADIUS"	, _TL("Kernel Radius"),
		_TL(""),
		2, 1, true
	);

	Parameters.Add_Double("",
		"SIGMA"			, _TL("Standard Deviation"),
		_TL("The standard deviation as percentage of the kernel radius."),
		50., 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Gauss::On_Execute(void)
{
	int Radius = Parameters("KERNEL_RADIUS")->asInt();

	CSG_Matrix Kernel;

	if( !Kernel.Create(1 + 2 * (sLong)Radius, 1 + 2 * (sLong)Radius) )
	{
		Error_Set(_TL("Kernel initialization failed!"));

		return( false );
	}

	//-----------------------------------------------------
	double Bandwidth = Radius * Parameters("SIGMA")->asDouble() / 100.;

	for(int i=0; i<Kernel.Get_NY(); i++)
	{
		for(int j=0; j<Kernel.Get_NX(); j++)
		{
			double d = SG_Get_Square((double)i - Radius) + SG_Get_Square((double)j - Radius) / Bandwidth;

			Kernel[i][j] = exp(-0.5 * d*d);
		}
	}

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

		pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Gaussian Filter"));

		pResult->Set_NoData_Value(pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics s;

			if( !pInput->is_NoData(x, y) )
			{
				for(int i=0, iy=y-Radius; i<Kernel.Get_NY(); i++, iy++)
				{
					for(int j=0, ix=x-Radius; j<Kernel.Get_NX(); j++, ix++)
					{
						if( pInput->is_InGrid(ix, iy) )
						{
							s.Add_Value(pInput->asDouble(ix, iy), Kernel[i][j]);
						}
					}
				}
			}

			if( s.Get_Weights() > 0. )
			{
				pResult->Set_Value(x, y, s.Get_Mean());
			}
			else
			{
				pResult->Set_NoData(x, y);
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
