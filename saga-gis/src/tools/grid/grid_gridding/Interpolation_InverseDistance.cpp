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
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//           Interpolation_InverseDistance.cpp           //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "Interpolation_InverseDistance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_InverseDistance::CInterpolation_InverseDistance(void)
	: CInterpolation(true, false)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Inverse Distance Weighted"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Inverse distance grid interpolation from irregular distributed points."
	));

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node(NULL, "NODE_SEARCH", _TL("Search Options"), _TL("")));

	//-----------------------------------------------------
	m_Weighting.Set_Weighting (SG_DISTWGHT_IDW);
	m_Weighting.Set_IDW_Offset(false);
	m_Weighting.Set_IDW_Power (2.0);

	m_Weighting.Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CInterpolation_InverseDistance::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);

		if( pParameter->asShapes() && pParameter->asShapes()->Get_Count() > 1 )
		{	// get a rough estimation of point density for band width suggestion
			pParameters->Get_Parameter("DW_BANDWIDTH")->Set_Value(SG_Get_Rounded_To_SignificantFigures(
				0.5 * sqrt(pParameter->asShapes()->Get_Extent().Get_Area() / pParameter->asShapes()->Get_Count()), 1
			));
		}
	}

	return( CInterpolation::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CInterpolation_InverseDistance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(pParameters);

	return( CInterpolation::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_InverseDistance::On_Initialize(void)
{
	return(	m_Search.Initialize(Get_Points(), Get_Field())
		&&  m_Weighting.Set_Parameters(&Parameters) 
	);
}

//---------------------------------------------------------
bool CInterpolation_InverseDistance::On_Finalize(void)
{
	m_Search.Finalize();

	return(	true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_InverseDistance::Get_Value(double x, double y, double &z)
{
	int		n	= m_Search.Set_Location(x, y);

	if( n <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics	s;

	double	ix, iy, w;

	for(int i=0; i<n; i++)
	{
		if( m_Search.Get_Point(i, ix, iy, z) )
		{
			if( (w = m_Weighting.Get_Weight(SG_Get_Distance(x, y, ix, iy))) < 0.0 )
			{
				return( true );
			}

			s.Add_Value(z, w);
		}
	}

	z	= s.Get_Mean();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
