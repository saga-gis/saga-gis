/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// Free Software Foundation; version 2 of the License.   //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		"This module approximates irregular scalar 2D data in specified points using "
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
 		"\n"
 		"Description:    See J. Haber, F. Zeilfelder, O.Davydov and H.-P. Seidel,"
 		"                Smooth approximation and rendering of large scattered data"
 		"                sets, in 'Proceedings of IEEE Visualization 2001'"
 		"                (Th.Ertl, K.Joy and A.Varshney, Eds.), pp.341-347, 571,"
 		"                IEEE Computer Society, 2001.\n"
 		"<a target=\"_blank\" href=\"http://www.uni-giessen.de/www-Numerische-Mathematik/davydov/VIS2001.ps.gz\">"
		"www.uni-giessen.de/www-Numerische-Mathematik/davydov/VIS2001.ps.gz</a>\n"

 		"<a target=\"_blank\" href=\"http://www.math.uni-mannheim.de/~lsmath4/paper/VIS2001.pdf.gz\">"
		"www.math.uni-mannheim.de/~lsmath4/paper/VIS2001.pdf.gz</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "NPMIN"		, _TL("Minimal Number of Points"),
		_TL(""),
		PARAMETER_TYPE_Int		, 3, 0, true
	);

	Parameters.Add_Value(
		NULL	, "NPMAX"		, _TL("Maximal Number of Points"),
		_TL(""),
		PARAMETER_TYPE_Int		, 20, 11, true, 59, true
	);

	Parameters.Add_Value(
		NULL	, "NPPC"		, _TL("Points per Square"),
		_TL(""),
		PARAMETER_TYPE_Double	, 5, 1, true
	);

	Parameters.Add_Value(
		NULL	, "K"			, _TL("Tolerance"),
		_TL("Spline sensitivity, reduce to get smoother results, recommended: 80 < Tolerance < 200"),
		PARAMETER_TYPE_Int		, 140, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_CSA::On_Initialise(void)
{

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	int			i, x, y;
	TSG_Point	p;

	csa			*pCSA	= csa_create();

	csa_setnpmin(pCSA, Parameters("NPMIN")	->asInt());
	csa_setnpmax(pCSA, Parameters("NPMAX")	->asInt());
	csa_setk	(pCSA, Parameters("K")		->asInt());
	csa_setnppc	(pCSA, Parameters("NPPC")	->asDouble());

	//-----------------------------------------------------
	point	*pSrc	= (point *)SG_Malloc(m_Points.Get_Count() * sizeof(point));

	for(i=0; i<m_Points.Get_Count() && Set_Progress(i, m_Points.Get_Count()); i++)
	{
		pSrc[i].x	= m_Points[i].x;
		pSrc[i].y	= m_Points[i].y;
		pSrc[i].z	= m_Points[i].z;
	}

	csa_addpoints(pCSA, m_Points.Get_Count(), pSrc);

	m_Points.Clear();

	//-----------------------------------------------------
	point	*pDst	= (point *)SG_Malloc(m_pGrid->Get_NCells() * sizeof(point));

	for(y=0, i=0, p.y=m_pGrid->Get_YMin(); y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, p.y+=m_pGrid->Get_Cellsize())
	{
		for(x=0, p.x=m_pGrid->Get_XMin(); x<m_pGrid->Get_NX(); x++, p.x+=m_pGrid->Get_Cellsize(), i++)
		{
			pDst[i].x	= p.x;
			pDst[i].y	= p.y;
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("calculating splines..."));
	csa_calculatespline		(pCSA);

	Process_Set_Text(_TL("approximating points..."));
	csa_approximate_points	(pCSA, m_pGrid->Get_NCells(), pDst);

	//-----------------------------------------------------
	for(y=0, i=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
	{
		for(x=0; x<m_pGrid->Get_NX(); x++, i++)
		{
			if( isnan(pDst[i].z) )
			{
				m_pGrid->Set_NoData(x, y);
			}
			else
			{
				m_pGrid->Set_Value(x, y, pDst[i].z);
			}
		}
	}

	//-----------------------------------------------------
	csa_destroy(pCSA);

	SG_Free(pSrc);
	SG_Free(pDst);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
