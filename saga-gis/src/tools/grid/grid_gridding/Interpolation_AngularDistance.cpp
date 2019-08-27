
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
#include "Interpolation_AngularDistance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_AngularDistance::CInterpolation_AngularDistance(void)
	: CInterpolation(true, true)
{
	Set_Name		(_TL("Angular Distance Weighted"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Angular Distance Weighted (ADW) grid interpolation from irregular distributed points."
	));

	Add_Reference("Shepard, D.", "1968",
		"A Two-Dimensional Interpolation Function for Irregularly-Spaced Data",
		"Proceedings of the 1968 23rd ACM National Conference, pp.517-524, ",
		SG_T("http://champs.cecs.ucf.edu/Library/Conference_Papers/pdfs/A%20two-dimentional%20intepolation%20function%20for%20irregalarly-spaced%20data.pdf"), SG_T("online")
	);

	//-----------------------------------------------------
	m_Search_Options.Create(&Parameters, "NODE_SEARCH", 1);

	Parameters("SEARCH_POINTS_ALL")->Set_Value( 0);	// maximum number of nearest points
	Parameters("SEARCH_POINTS_MIN")->Set_Value( 4);
	Parameters("SEARCH_POINTS_MAX")->Set_Value(40);

	m_Weighting.Set_Weighting (SG_DISTWGHT_IDW);
	m_Weighting.Set_IDW_Offset(false);
	m_Weighting.Set_IDW_Power (2.);

	m_Weighting.Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CInterpolation_AngularDistance::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search_Options.On_Parameter_Changed(pParameters, pParameter);

		if( pParameter->asShapes() && pParameter->asShapes()->Get_Count() > 1 )
		{	// get a rough estimation of point density for band width suggestion
			pParameters->Set_Parameter("DW_BANDWIDTH", SG_Get_Rounded_To_SignificantFigures(
				0.5 * sqrt(pParameter->asShapes()->Get_Extent().Get_Area() / pParameter->asShapes()->Get_Count()), 1
			));
		}
	}

	return( CInterpolation::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CInterpolation_AngularDistance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search_Options.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(pParameters);

	return( CInterpolation::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_AngularDistance::On_Initialize(void)
{
	CSG_Shapes	*pPoints	= Get_Points();	int	Field	= Get_Field();

	m_Points.Create(3, pPoints->Get_Count());

	int	n	= 0;

	for(int i=0; i<pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Field) )
		{
			m_Points[n][0]	= pPoint->Get_Point(0).x;
			m_Points[n][1]	= pPoint->Get_Point(0).y;
			m_Points[n][2]	= pPoint->asDouble(Field);

			n++;
		}
	}

	m_Points.Set_Rows(n);	// resize if there are no-data values

	if( n < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !m_Search_Options.Do_Use_All(true) && !m_Search.Create(m_Points) )
	{
		Error_Set(_TL("failed to initialize search engine"));

		return( false );
	}

	return(	m_Weighting.Set_Parameters(&Parameters) );
}

//---------------------------------------------------------
bool CInterpolation_AngularDistance::On_Finalize(void)
{
	m_Search.Destroy();
	m_Points.Destroy();

	return(	true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_AngularDistance::Get_Value(double x, double y, double &z)
{
	int	nPoints;	const double	**Points;	CSG_Array_Pointer	__Points;

	if( m_Search.is_Okay() )	// local
	{
		CSG_Array_Int	Index;	CSG_Vector	Distance;

		if( m_Search.Get_Nearest_Points(x, y,
			m_Search_Options.Get_Max_Points(),
			m_Search_Options.Get_Radius(), Index, Distance
		) < m_Search_Options.Get_Min_Points() )
		{
			return( false );
		}

		nPoints	= (int)Index.Get_Size();
		Points	= (const double **)__Points.Create(Index.Get_Size());

		for(size_t i=0; i<Index.Get_Size(); i++)
		{
			Points[i]	= m_Points[Index[i]];
		}
	}
	else	// global
	{
		nPoints	= m_Points.Get_NRows();
		Points	= m_Points;
	}

	//-----------------------------------------------------
	CSG_Vector	D(nPoints), W(nPoints);

	for(int i=0; i<nPoints; i++)
	{
		D[i]	= SG_Get_Distance(x, y, Points[i][0], Points[i][1]);
		W[i]	= m_Weighting.Get_Weight(D[i]);

		if( D[i] <= 0. )
		{
			z	= Points[i][2];

			return( true );
		}
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics	s;

	for(int i=0; i<nPoints; i++)
	{
		double	xi	= Points[i][0];
		double	yi	= Points[i][1];
		double	zi	= Points[i][2];

		double	w = 0., t = 0.;

		for(int j=0; j<nPoints; j++)
		{
			if( j != i )
			{
				double	xj	= Points[j][0];
				double	yj	= Points[j][1];
				double	zj	= Points[j][2];

				t	+= W[j] * (1. - ((x - xi) * (x - xj) + (y - yi) * (y - yj)) / (D[i] * D[j]));
				w	+= W[j];
			}
		}

		s.Add_Value(zi, W[i] * (1. + t / w));
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
