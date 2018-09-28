/**********************************************************
* Version $Id: Curvature_UpDownSlope.cpp 911 2011-11-11 11:11:11Z oconrad $
*********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Curvature_UpDownSlope.cpp              //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                    Paolo Gandelli                     //
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
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     gandelli.paolo@gmail.com               //
//                                                       //
//    contact:    Paolo Gandelli                         //
//                Engineering Geology and Geomorphology  //
//                Department of Earth Sciences           //
//                University of Firenze                  //
//                Italy                                  //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Curvature_UpDownSlope.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCurvature_UpDownSlope::CCurvature_UpDownSlope(void)
{
	Set_Name		(_TL("Upslope and Downslope Curvature"));

	Set_Author		("P.Gandelli, O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Calculates the local curvature of a cell as sum of the gradients to its neighbour cells. "
		"Upslope curvature is the distance weighted average local curvature in a cell's upslope "
		"contributing area based on multiple flow direction after Freeman 1994. "
		"\nReferences:\n"
		"- Freeman, G.T. (1991): Calculating catchment area with divergent flow based on a regular grid. "
		"  Computers and Geosciences, 17:413-22\n"
	));

	Parameters.Add_Grid(NULL, "DEM"         , _TL("Elevation"                ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid(NULL, "C_LOCAL"     , _TL("Local Curvature"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "C_UP"        , _TL("Upslope Curvature"        ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "C_UP_LOCAL"  , _TL("Local Upslope Curvature"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "C_DOWN"      , _TL("Downslope Curvature"      ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "C_DOWN_LOCAL", _TL("Local Downslope Curvature"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(
		NULL	, "WEIGHTING"	, _TL("Upslope Weighting"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.5, 0.0, true, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCurvature_UpDownSlope::On_Execute(void)
{
	m_pDEM			= Parameters("DEM"         )->asGrid();
	m_pC_Local		= Parameters("C_LOCAL"     )->asGrid();
	m_pC_Up			= Parameters("C_UP"        )->asGrid();
	m_pC_Up_Local	= Parameters("C_UP_LOCAL"  )->asGrid();
	m_pC_Down		= Parameters("C_DOWN"      )->asGrid();
	m_pC_Down_Local	= Parameters("C_DOWN_LOCAL")->asGrid();

	m_Weighting		= Parameters("WEIGHTING")->asDouble();

	m_pC_Up        ->Assign(0.0);
	m_pC_Up_Local  ->Assign(0.0);
	m_pC_Down      ->Assign(0.0);
	m_pC_Down_Local->Assign(0.0);

	DataObject_Set_Colors(m_pC_Local     , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pC_Up        , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pC_Up_Local  , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pC_Down      , 11, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pC_Down_Local, 11, SG_COLORS_RED_GREY_BLUE, true);

	if( !m_Weights.Create(Get_System()) )
	{
		Error_Set(_TL("could not allocate memory for temporary grid."));

		return( false );
	}

	//-----------------------------------------------------
	int		x, y;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pC_Local     ->Set_NoData(x, y);
				m_pC_Up        ->Set_NoData(x, y);
				m_pC_Up_Local  ->Set_NoData(x, y);
				m_pC_Down      ->Set_NoData(x, y);
				m_pC_Down_Local->Set_NoData(x, y);
			}
			else
			{
				m_pC_Local     ->Set_Value(x, y, Get_Local(x, y));
			}
		}
	}

	//-----------------------------------------------------
	for(sLong i=0; i<Get_NCells() && Set_Progress((double)i, (double)Get_NCells()); i++)
	{
		if( m_pDEM->Get_Sorted(i, x, y, true) )
		{
			Get_Upslope(x, y);
		}

		if( m_pDEM->Get_Sorted(i, x, y, false) )
		{
			Get_Downslope(x, y);
		}
	}

	//-----------------------------------------------------
	m_Weights.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCurvature_UpDownSlope::Get_Local(int x, int y)
{
	double	Sum	= 0.0;

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) )
		{
			Sum += ((m_pDEM->asDouble(x, y) - m_pDEM->asDouble(ix, iy)) / Get_Length(i));
		}
	}

	return( Sum );
}

//---------------------------------------------------------
bool CCurvature_UpDownSlope::Get_Upslope(int x, int y)
{
	double	cLocal	= m_pC_Local->asDouble(x, y);

	//-----------------------------------------------------
	double	cUp, cUp_Local, Weights;

	if( (Weights = m_Weights.asDouble(x, y)) > 0.0 )
	{
		cUp			= (m_pC_Up->asDouble(x, y) + m_Weighting * cLocal) / (Weights + m_Weighting);
		cUp_Local	= m_pC_Up_Local->asDouble(x, y) / Weights;
	}
	else
	{
		cUp			= cLocal;
		cUp_Local	= cLocal;
	}

	m_pC_Up      ->Set_Value(x, y, cUp      );
	m_pC_Up_Local->Set_Value(x, y, cUp_Local);

	//-----------------------------------------------------
	double Proportion[8];

	if( Get_Flow_Proportions(x, y, Proportion) )
	{
		for(int i=0; i<8; i++)
		{
			if( Proportion[i] > 0.0 )
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				m_pC_Up      ->Add_Value(ix, iy, Proportion[i] * cUp);
				m_pC_Up_Local->Add_Value(ix, iy, Proportion[i] * cLocal);
				m_Weights     .Add_Value(ix, iy, Proportion[i]);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CCurvature_UpDownSlope::Get_Downslope(int x, int y)
{
	//-----------------------------------------------------
	double	Proportion[8];

	if( Get_Flow_Proportions(x, y, Proportion) )
	{
		double	Weights = 0.0, cDown = 0.0, cDown_Local = 0.0;

		for(int i=0; i<8; i++)
		{
			if( Proportion[i] > 0.0 )
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				if( m_pC_Local->is_InGrid(ix, iy) )
				{
					cDown_Local	+= Proportion[i] * m_pC_Local->asDouble(ix, iy);
					cDown		+= Proportion[i] * m_pC_Down ->asDouble(ix, iy);
					Weights		+= Proportion[i];
				}
			}
		}

		if( Weights > 0.0 )
		{
			m_pC_Down_Local->Set_Value(x, y, cDown_Local / Weights);
			m_pC_Down      ->Set_Value(x, y, (m_Weighting * m_pC_Local->asDouble(x, y) + cDown) / (m_Weighting + Weights));
		}

		return( true );
	}

	m_pC_Down_Local->Set_Value(x, y, m_pC_Local->asDouble(x, y));
	m_pC_Down      ->Set_Value(x, y, m_pC_Local->asDouble(x, y));

	return( false );
}

//---------------------------------------------------------
bool CCurvature_UpDownSlope::Get_Flow_Proportions(int x, int y, double Proportion[8])
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		double Sum = 0.0;

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && m_pDEM->asDouble(x, y) > m_pDEM->asDouble(ix, iy) )
			{
				Sum	+= (Proportion[i]	= ((m_pDEM->asDouble(x, y) - m_pDEM->asDouble(ix, iy)) / Get_Length(i)));
			}
			else
			{
				Proportion[i]	= 0.0;
			}
		}

		if( Sum > 0.0 )
		{
			for(int i=0; i<8; i++)
			{
				Proportion[i]	/= Sum;
			}

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
