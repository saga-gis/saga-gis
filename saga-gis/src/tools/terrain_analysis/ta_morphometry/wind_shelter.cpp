
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
//                   wind_shelter.cpp                    //
//                                                       //
//                  Copyright (C) 2019                   //
//                     Olaf Conrad                       //
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
#include "wind_shelter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWind_Shelter::CWind_Shelter(void)
{
	Set_Name		(_TL("Wind Shelter Index"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"This tool reimplements the Wind Shelter Index proposed by Plattner et al. (2004), "
		"that has originally been implemented within the RSAGA package. "
	));

	Add_Reference("Plattner, C., Braun, L.N., Brenning, A.", "2004",
		"Spatial variability of snow accumulation on Vernagtferner, Austrian Alps, in winter 2003/2004",
		"Zeitschrift fuer Gletscherkunde und Glazialgeologie, 39: 43-57.",
		SG_T("http://www.academia.edu/download/30283083/Plattner-et-al-2006-ZGletscherk.pdf"), SG_T("[PDF] academia.edu")
	);

	Add_Reference("Winstral, A., Elder, K., Davis, R.E.", "2002",
		"Spatial snow modeling of wind-redistributed snow using terrain-based parameters",
		"Journal of Hydrometeorology, 3: 524-538.",
		SG_T("https://doi.org/10.1175/1525-7541(2002)003%3C0524:SSMOWR%3E2.0.CO;2"), SG_T("[PDF] ametsoc.org")
	);

	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SHELTER"	, _TL("Wind Shelter Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"DIRECTION"	, _TL("Wind Direction"),
		_TL("The direction [degree] into which the wind blows."),
		135.//, -180., true, 360., true
	);

	Parameters.Add_Double("",
		"TOLERANCE"	, _TL("Tolerance"),
		_TL("The tolerance angle [degree] to the left and right of the direction angle."),
		5., 0., true, 90., true
	);

	Parameters.Add_Double("",
		"DISTANCE"	, _TL("Distance"),
		_TL("[cells]"),
		5., 1., true
	);

	Parameters.Add_Double("",
		"QUANTILE"	, _TL("Quantile"),
		_TL("Set quantile to one to get the maximum slope angle."),
		1., 0., true, 1., true
	);

	Parameters.Add_Bool("",
		"NEGATIVES"	, _TL("Include Negative Slopes"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("windward"),
			_TL("leeward")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWind_Shelter::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Kernel.Enable_Parameters(*pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWind_Shelter::On_Execute(void)
{
	m_pDEM	= Parameters("ELEVATION")->asGrid();

	CSG_Grid	*pShelter	= Parameters("SHELTER")->asGrid();

	DataObject_Set_Colors(pShelter, 11, SG_COLORS_YELLOW_BLUE);

	double	Direction	= Parameters("DIRECTION")->asDouble();
	double	Tolerance	= Parameters("TOLERANCE")->asDouble();
	double	Distance	= Parameters("DISTANCE" )->asDouble();

	m_Quantile	= Parameters("QUANTILE")->asDouble();

	m_Negatives	= Parameters("NEGATIVES")->asBool();

	m_Method	= Parameters("METHOD")->asInt();

	if( m_Method == 0 )
	{
		Direction	+= 180.;
	}

	if( Distance < 1. )
	{
		Distance	= 1. + SG_Get_Length(Get_NX(), Get_NY());
	}

	if( !m_Kernel.Set_Sector(Distance, M_DEG_TO_RAD * Direction, M_DEG_TO_RAD * Tolerance) )
	{
		Error_Set(_TL("failed to initialize kernel"));

		return( false );
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	Index;

			if( !Get_Index(x, y, Index) )
			{
				pShelter->Set_NoData(x, y);
			}
			else
			{
				pShelter->Set_Value(x, y, 90. - Index);
			}
		}
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWind_Shelter::Get_Index(int x, int y, double &Index)
{
	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	CSG_Simple_Statistics	s(m_Quantile < 1.);

	double	z	= m_pDEM->asDouble(x, y);

	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i, x);
		int	iy	= m_Kernel.Get_Y(i, y);

		if( m_pDEM->is_InGrid(ix, iy) )
		{
			double	dz	= z - m_pDEM->asDouble(ix, iy);

			if( m_Method == 1 )
			{
				dz	= -dz;
			}
			
			if( m_Negatives || dz > 0. )
			{
				s	+= dz / m_Kernel.Get_Distance(i);
			}
		}
	}

	Index	= s.Get_Count() < 1 ? 0. : M_RAD_TO_DEG * atan(
		m_Quantile < 1. ? s.Get_Quantile(m_Quantile) : s.Get_Maximum()
	);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
