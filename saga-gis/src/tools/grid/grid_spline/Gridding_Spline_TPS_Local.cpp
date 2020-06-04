
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Gridding_Spline_TPS_Local.cpp            //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Gridding_Spline_TPS_Local.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_TPS_Local::CGridding_Spline_TPS_Local(void)
{
	Set_Name		(_TL("Thin Plate Spline"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Creates a 'Thin Plate Spline' function for each grid point "
		"based on all of the scattered data points that are within a "
		"given distance. The number of points can be limited to a "
		"maximum number of closest points."
	));

	Add_Reference("Donato G., Belongie S.", "2002",
		"Approximation Methods for Thin Plate Spline Mappings and Principal Warps",
		"In Heyden, A., Sparr, G., Nielsen, M., Johansen, P. (Eds.): Computer Vision - ECCV 2002: 7th European Conference on Computer Vision, Copenhagen, Denmark, May 28-31, 2002, "
		"Proceedings, Part III, Lecture Notes in Computer Science., Springer-Verlag Heidelberg; pp.21-31."
	);

	Add_Reference("Elonen, J.", "2005",
		"Thin Plate Spline editor - an example program in C++",
		"",
		SG_T("http://elonen.iki.fi/code/tpsdemo/index.html"
	));

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "REGULARISATION"	, _TL("Regularisation"),
		_TL(""),
		0.0001, 0., true
	);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node("", "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_TPS_Local::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SHAPES") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);
	}

	return( CGridding_Spline_Base::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGridding_Spline_TPS_Local::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	return( CGridding_Spline_Base::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_TPS_Local::On_Execute(void)
{
	double	Regularization	= Parameters("REGULARISATION")->asDouble();

	//-----------------------------------------------------
	if( m_Search.Do_Use_All(true) )	// global
	{
		CSG_Thin_Plate_Spline	Spline;

		if( !Initialize(Spline.Get_Points()) || !Spline.Create(Regularization, false) )
		{
			return( false );
		}

		for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
		{
			double	yWorld	= m_pGrid->Get_YMin() + y * m_pGrid->Get_Cellsize();

			#pragma omp parallel for
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				double	xWorld	= m_pGrid->Get_XMin() + x * m_pGrid->Get_Cellsize();

				m_pGrid->Set_Value(x, y, Spline.Get_Value(xWorld, yWorld));
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		if( !Initialize() || !m_Search.Initialize(Parameters("SHAPES")->asShapes(), Parameters("FIELD")->asInt()) )
		{
			return( false );
		}

		for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
		{
			double	yWorld	= m_pGrid->Get_YMin() + y * m_pGrid->Get_Cellsize();

			#pragma omp parallel for
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				double	xWorld	= m_pGrid->Get_XMin() + x * m_pGrid->Get_Cellsize();

				Set_Value(x, y, xWorld, yWorld, Regularization);
			}
		}

		m_Search.Finalize();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_TPS_Local::Set_Value(int x, int y, double xWorld, double yWorld, double Regularization)
{
	CSG_Points_Z	Points;

	if( m_Search.Get_Points(xWorld, yWorld, Points) && Points.Get_Count() > 2 )
	{
		CSG_Thin_Plate_Spline	Spline;

		for(int i=0; i<Points.Get_Count(); i++)
		{
			Spline.Add_Point(Points[i].x, Points[i].y, Points[i].z);
		}

		if( Spline.Create(Regularization, true) )
		{
			m_pGrid->Set_Value(x, y, Spline.Get_Value(xWorld, yWorld));

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pGrid->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
