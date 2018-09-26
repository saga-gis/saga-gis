/**********************************************************
 * Version $Id$
 *********************************************************/

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
	Set_Name(_TL("Gaussian Filter"));

	Set_Author(SG_T("A.Ringeler (c) 2003"));

	Set_Description	(_TW(
		"The Gaussian filter is a smoothing operator that is used to 'blur' or 'soften' data "
		"and to remove detail and noise. "
		"The degree of smoothing is determined by the standard deviation. "
		"For higher standard deviations you need to use a larger search radius."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(NULL,
		"SIGMA"		, _TL("Standard Deviation"),
		_TL("The standard deviation as percentage of the kernel radius, determines the degree of smoothing."),
		50.0, 0.0001, true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Gauss::On_After_Execution(void)
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
bool CFilter_Gauss::On_Execute(void)
{
	//-----------------------------------------------------
	double	Sigma	= Parameters("SIGMA")->asDouble();

	CSG_Grid_Cell_Addressor	Kernel;

	Kernel.Get_Weighting().Set_Weighting(SG_DISTWGHT_GAUSS);
	Kernel.Get_Weighting().Set_BandWidth(Sigma * Parameters("KERNEL_RADIUS")->asDouble() / 100.0);

	if( !Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pInput 	= Parameters("INPUT" )->asGrid();
	CSG_Grid	*pResult	= Parameters("RESULT")->asGrid(), Result;

	if( !pResult || pResult == pInput )
	{
		pResult	= &Result;
		
		pResult->Create(*pInput);
	}
	else
	{
		pResult->Set_Name("%s [%s]", pInput->Get_Name(), _TL("Gaussian Filter"));

		pResult->Set_NoData_Value(pInput->Get_NoData_Value());

		DataObject_Set_Parameters(pResult, pInput);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics	s;

			if( !pInput->is_NoData(x, y) )
			{
				for(int i=0; i<Kernel.Get_Count(); i++)
				{
					int	ix	= Kernel.Get_X(i, x);
					int	iy	= Kernel.Get_Y(i, y);

					if( pInput->is_InGrid(ix, iy) )
					{
						s.Add_Value(pInput->asDouble(ix, iy), Kernel.Get_Weight(i));
					}
				}
			}

			if( s.Get_Weights() > 0.0 )
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
	if( pResult == &Result )
	{
		CSG_MetaData	History	= pInput->Get_History();

		pInput->Assign(pResult);
		pInput->Get_History() = History;

		DataObject_Update(pInput);

		Parameters("RESULT")->Set_Value(pInput);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
