
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
//                Gridding_Spline_CSA.cpp                //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "Gridding_Spline_CSA.h"

#include "csa.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_MSW
   #define isnan    _isnan
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_CSA::CGridding_Spline_CSA(void)
	: CGridding_Spline_Base()
{
	//-----------------------------------------------------
	Set_Name		(_TL("Cubic Spline Approximation"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(_TW(
		"This tool approximates irregular scalar 2D data in specified points using "
		"C1-continuous bivariate cubic spline."
		"\n"
		"Minimal Number of Points:"
		"                minimal number of points locally involved"
		"                in spline calculation (normally = 3)\n"
		"\n"
		"Maximal Number of Points:"
		"npmax:          maximal number of points locally involved"
		"                in spline calculation (required > 10,"
		"                recommended 20 < npmax < 60)"
		"\n"
		"Tolerance:"
		"                relative tolerance multiple in fitting"
		"                spline coefficients: the higher this"
		"                value, the higher degree of the locally"
		"                fitted spline (recommended 80 < k < 200)\n"
		"\n"
		"Points per square:"
		"                average number of points per square"
		"                (increase if the point distribution is strongly non-uniform"
		"                to get larger cells)\n"
		"\n"
 		"Author:         Pavel Sakov,"
 		"                CSIRO Marine Research\n"
		"\n"
 		"Purpose:        2D data approximation with bivariate C1 cubic spline."
 		"                A set of library functions + standalone utility.\n"
	));

	Add_Reference("Haber, J., Zeilfelder, F., Davydov, O., Seidel, H.-P.", "2001",
 		"Smooth approximation and rendering of large scattered data sets",
		"In Ertl, T., Joy, K., Varshney, A. [Eds.]: Proceedings of IEEE Visualization. pp.341-347, 571, IEEE Computer Society."
	);

	//-----------------------------------------------------
	Parameters.Add_Int(
		"", "NPMIN"	, _TL("Minimal Number of Points"),
		_TL(""),
		3, 0, true
	);

	Parameters.Add_Int(
		"", "NPMAX"	, _TL("Maximal Number of Points"),
		_TL(""),
		20, 11, true, 59, true
	);

	Parameters.Add_Double(
		"", "NPPC"	, _TL("Points per Square"),
		_TL(""),
		5, 1, true
	);

	Parameters.Add_Int(
		"", "K"		, _TL("Tolerance"),
		_TL("Spline sensitivity, reduce to get smoother results, recommended: 80 < Tolerance < 200"),
		140, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_CSA::On_Execute(void)
{
	//-----------------------------------------------------
	if( Initialise(m_Points, true) == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			y;
	CSG_Array	Array;
	point		*Points;

	csa			*pCSA	= csa_create();

	csa_setnpmin(pCSA, Parameters("NPMIN")->asInt   ());
	csa_setnpmax(pCSA, Parameters("NPMAX")->asInt   ());
	csa_setk	(pCSA, Parameters("K"    )->asInt   ());
	csa_setnppc	(pCSA, Parameters("NPPC" )->asDouble());

	//-----------------------------------------------------
	if( !Array.Create(sizeof(point), m_Points.Get_Count()) )
	{
		Error_Set(_TL("failed to allocate memory for input points"));

		return( false );
	}

	Points	= (point *)Array.Get_Array();

	#pragma omp parallel for
	for(int i=0; i<m_Points.Get_Count(); i++)
	{
		Points[i].x	= m_Points[i].x;
		Points[i].y	= m_Points[i].y;
		Points[i].z	= m_Points[i].z;
	}

	m_Points.Clear();

	csa_addpoints(pCSA, (int)Array.Get_Size(), Points);

	//-----------------------------------------------------
	Process_Set_Text(_TL("calculating splines..."));

	csa_calculatespline(pCSA);

	//-----------------------------------------------------
	if( !Array.Create(sizeof(point), m_pGrid->Get_NCells()) )
	{
		Error_Set(_TL("failed to allocate memory for output points"));

		return( false );
	}

	Points	= (point *)Array.Get_Array();

	#pragma omp parallel for private(y)
	for(y=0; y<m_pGrid->Get_NY(); y++)
	{
		int	i	= y * m_pGrid->Get_NX();
		double	py	= m_pGrid->Get_YMin() + y * m_pGrid->Get_Cellsize();
		double	px	= m_pGrid->Get_XMin();

		for(int x=0; x<m_pGrid->Get_NX(); x++, px+=m_pGrid->Get_Cellsize(), i++)
		{
			Points[i].x	= px;
			Points[i].y	= py;
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("approximating points..."));

	csa_approximate_points(pCSA, m_pGrid->Get_NCells(), Points);

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<m_pGrid->Get_NY(); y++)
	{
		int	i	= y * m_pGrid->Get_NX();

		for(int x=0; x<m_pGrid->Get_NX(); x++, i++)
		{
			double	z	= Points[i].z;

			if( isnan(z) )
			{
				m_pGrid->Set_NoData(x, y);
			}
			else
			{
				m_pGrid->Set_Value(x, y, z);
			}
		}
	}

	//-----------------------------------------------------
	csa_destroy(pCSA);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
