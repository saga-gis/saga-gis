
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               windeffect_correction.cpp               //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "windeffect_correction.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWindeffect_Correction::CWindeffect_Correction(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Wind Effect Correction"));

	Set_Author		("D.N.Karger, O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Wind effect correction using generalized logistic functions."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"BOUNDARY"		, _TL("Boundary Layer"),
		_TL("The absolute vertical distance to the boundary layer."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"WIND"			, _TL("Wind Effect"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"OBSERVED"		, _TL("Observations"),
		_TL("Observations used for local scaling factor calibration (e.g. precipitation, cloudiness)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"B_GRID"		, _TL("Calibrated Scaling Factor"),
		_TL("Calibrated scaling factor used in the wind effect correction."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(NULL,
		"WINDCORR"		, _TL("Corrected Wind Effect"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL,
		"B_SOURCE"		, _TL("Scaling Factor"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("constant"),
			_TL("calibrate")
		), 1
	);

	Parameters.Add_Double(Parameters("B_SOURCE"),
		"B_CONST"		, _TL("Constant Scaling Factor"),
		_TL("Constant scaling factor used in the wind effect correction."),
		0.01, 0.0, true
	);

	Parameters.Add_Double(Parameters("B_SOURCE"),
		"B_MAX"			, _TL("Maximum Scaling Factor"),
		_TL(""),
		0.05, 0.0, true
	);

	Parameters.Add_Int(Parameters("B_SOURCE"),
		"B_STEPS"		, _TL("Number of Steps"),
		_TL(""),
		10, 1, true
	);

	Parameters.Add_Choice(Parameters("B_SOURCE"),
		"KERNEL_TYPE"	, _TL("Kernel Type"),
		_TL("Kernel specification used to request observations for local scaling factor calibration."),
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Int(Parameters("B_SOURCE"),
		"KERNEL_SIZE"	, _TL("Kernel Size"),
		_TL("Kernel specification used to request observations for local scaling factor calibration."),
		2, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWindeffect_Correction::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "B_SOURCE") )
	{
		pParameters->Set_Enabled("B_CONST"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("OBSERVED"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("B_GRID"     , pParameter->asInt() == 1);
		pParameters->Set_Enabled("B_MAX"      , pParameter->asInt() == 1);
		pParameters->Set_Enabled("B_STEPS"    , pParameter->asInt() == 1);
		pParameters->Set_Enabled("KERNEL_TYPE", pParameter->asInt() == 1);
		pParameters->Set_Enabled("KERNEL_SIZE", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWindeffect_Correction::On_Execute(void)
{
	//-----------------------------------------------------
	m_pBoundary	= Parameters("BOUNDARY")->asGrid();
	m_pWind		= Parameters("WIND"    )->asGrid();
	m_pObserved	= Parameters("OBSERVED")->asGrid();

	CSG_Grid	*pWindCorr	= Parameters("WINDCORR")->asGrid();

	//-----------------------------------------------------
	if( Parameters("B_SOURCE")->asInt() == 0 )	// constant
	{
		double	B	= Parameters("B_CONST")->asDouble();

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( m_pWind->is_NoData(x, y) || m_pBoundary->is_NoData(x, y) )
				{
					pWindCorr->Set_NoData(x, y);
				}
				else
				{
					pWindCorr->Set_Value(x, y, Get_Wind_Corr(B, m_pBoundary->asDouble(x, y), m_pWind->asDouble(x, y)));
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Grid	*pB	= Parameters("B_GRID")->asGrid();

		DataObject_Set_Colors(pB, 11, SG_COLORS_WHITE_GREEN);

		double	B_min	= 0.0;
		double	B_max	= Parameters("B_MAX")->asDouble();
		double	B_Step	= (B_max - B_min) / Parameters("B_STEPS")->asDouble();

		m_Kernel.Set_Radius(Parameters("KERNEL_SIZE")->asInt(), Parameters("KERNEL_TYPE")->asInt() == 0);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int x=0; x<Get_NX(); x++)
			{
				double	B;

				if( !Fit_Scaling_Factor(x, y, B, B_min, B_max, B_Step) )
				{
					pB       ->Set_NoData(x, y);
					pWindCorr->Set_NoData(x, y);
				}
				else
				{
					pB       ->Set_Value(x, y, B);
					pWindCorr->Set_Value(x, y, Get_Wind_Corr(B, m_pBoundary->asDouble(x, y), m_pWind->asDouble(x, y)));
				}
			}
		}

		m_Kernel.Destroy();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CWindeffect_Correction::Get_Wind_Corr(double B, double Boundary, double Wind)
{
	const double K = 1.0, Q = 1.0, M = 1.0, C = 1.0, U = 6.0;

	return( Wind + (K - Wind) / (C + Q * exp(-B * Boundary + U)) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BND	0
#define WND	1

#define OBS	0
#define COR	1
#define VAL	2

//---------------------------------------------------------
bool CWindeffect_Correction::Get_Data(int x, int y, CSG_Matrix &Data, CSG_Simple_Statistics &statsObserved)
{
	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i, x);
		int	iy	= m_Kernel.Get_Y(i, y);

		if( m_pBoundary->is_InGrid(ix, iy) && m_pWind->is_InGrid(ix, iy) && m_pObserved->is_InGrid(ix, iy) )
		{
			CSG_Vector	d(2);

			d[BND]	= m_pBoundary->asDouble(ix, iy);
			d[WND]	= m_pWind    ->asDouble(ix, iy);

			Data.Add_Row(d);

			statsObserved  += m_pObserved->asDouble(ix, iy);
		}
	}

	return( statsObserved.Get_Count() >= 5 );
}

//---------------------------------------------------------
bool CWindeffect_Correction::Fit_Scaling_Factor(int x, int y, double &B, double B_min, double B_max, double B_Step)
{
	CSG_Simple_Statistics	Statistics[3];

	CSG_Matrix	Data;

	if( !Get_Data(x, y, Data, Statistics[OBS]) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	dMin	= -1.0;

	for(double iB=B_min; iB<=B_max; iB+=B_Step)
	{
		int	i;

		Statistics[COR].Create(true);	// reset

		for(i=0; i<Data.Get_NRows(); i++)
		{
			Statistics[COR]	+= Get_Wind_Corr(iB, Data[i][BND], Data[i][WND]);
		}

		Statistics[VAL].Create(false);	// reset

		for(i=0; i<Data.Get_NRows(); i++)
		{
			Statistics[VAL]	+= Statistics[OBS].Get_Mean() * Statistics[COR].Get_Value(i) / Statistics[COR].Get_Mean();
		}

		double	d	= fabs(Statistics[VAL].Get_StdDev() - Statistics[OBS].Get_StdDev());

		if( dMin < 0.0 || dMin > d )
		{
			B		= iB;
			dMin	= d;
		}
	}

	return( dMin >= 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
