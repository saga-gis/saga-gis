/**********************************************************
 * Version $Id: Interpolation_AngularDistance.cpp 1482 2012-10-08 16:15:45Z oconrad $
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
//           Interpolation_AngularDistance.cpp           //
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
#include "Interpolation_AngularDistance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_AngularDistance::CInterpolation_AngularDistance(void)
	: CInterpolation(true, false)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Angular Distance Weighted"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Angular Distance Weighted (ADW) grid interpolation from irregular distributed points.\n"
		"\n"
		"References:\n"
		"Shepard, D. (1968): A Two-Dimensional Interpolation Function for Irregularly-Spaced Data. "
		"Proceedings of the 1968 23rd ACM National Conference, pp.517-524, "
		"<a target=\"_blank\" href=\"http://champs.cecs.ucf.edu/Library/Conference_Papers/pdfs/A%20two-dimentional%20intepolation%20function%20for%20irregalarly-spaced%20data.pdf\">online</a>.\n"
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
int CInterpolation_AngularDistance::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") )
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
int CInterpolation_AngularDistance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(pParameters);

	return( CInterpolation::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_AngularDistance::On_Initialize(void)
{
	return(	m_Search.Initialize(Get_Points(), Get_Field())
		&&  m_Weighting.Set_Parameters(&Parameters) 
	);
}

//---------------------------------------------------------
bool CInterpolation_AngularDistance::On_Finalize(void)
{
	m_Search.Finalize();

	return(	true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_AngularDistance::Get_Value(double x, double y, double &z)
{
	int		i, j, n;

	if( (n = m_Search.Set_Location(x, y)) <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	X(n), Y(n), D(n), W(n), Z(n);

	for(i=0; i<n; i++)
	{
		m_Search.Get_Point(i, X[i], Y[i], Z[i]);

		D[i]	= SG_Get_Distance(x, y, X[i], Y[i]);
		W[i]	= m_Weighting.Get_Weight(D[i]);

		if( D[i] <= 0.0 )
		{
			z	= Z[i];

			return( true );
		}
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics	s;

	for(i=0; i<n; i++)
	{
		double	w	= 0.0, t	= 0.0;

		for(j=0; j<n; j++)
		{
			if( j != i )
			{
				t	+= W[j] * (1.0 - ((x - X[i]) * (x - X[j]) + (y - Y[i]) * (y - Y[j])) / (D[i] * D[j]));
				w	+= W[j];
			}
		}

		s.Add_Value(Z[i], W[i] * (1.0 + t / w));
	}

	//-----------------------------------------------------
	z	= s.Get_Mean();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
