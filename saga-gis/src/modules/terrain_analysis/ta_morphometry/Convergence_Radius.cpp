/**********************************************************
 * Version $Id: Convergence_Radius.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                 Convergence_Radius.cpp                //
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
#include <memory.h>

#include "Convergence_Radius.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CConvergence_Radius::CConvergence_Radius(void)
{
	Set_Name	(_TL("Convergence Index (Search Radius)"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Reference:\n"
		"Koethe, R. & Lehmeier, F. (1996): SARA - System zur Automatischen Relief-Analyse. "
		"User Manual, 2. Edition [Dept. of Geography, University of Goettingen, unpublished]\n\n"
	));

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONVERGENCE"	, _TL("Convergence Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius [Cells]"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 1.0, true
	);

	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	)->asParameters()->Assign(m_Cells.Get_Weighting().Get_Parameters());

	Parameters.Add_Value(
		NULL	, "SLOPE"		, _TL("Gradient"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Choice(
		NULL	, "DIFFERENCE"	, _TL("Difference"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("direction to the center cell"),
			_TL("center cell's aspect direction")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvergence_Radius::On_Execute(void)
{
	int			y;
	CSG_Grid	*pConvergence;

	//-----------------------------------------------------
	m_pDTM			= Parameters("ELEVATION")	->asGrid();
	pConvergence	= Parameters("CONVERGENCE")	->asGrid();
	m_bSlope		= Parameters("SLOPE")		->asBool();
	m_bDifference	= Parameters("DIFFERENCE")	->asInt() == 0;

	//-----------------------------------------------------
	m_Cells.Get_Weighting().Set_Parameters(Parameters("WEIGHTING")->asParameters());

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asDouble()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	DataObject_Set_Colors(pConvergence, 100, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	m_Direction.Create(m_Cells.Get_Count());

	for(int i=0; i<m_Cells.Get_Count(); i++)
	{
		int		x;
		double	d;

		if( m_Cells.Get_Values(i, x, y, d, d, false) )
		{
			m_Direction[i]	= SG_Get_Angle_Of_Direction(0.0, 0.0, x, y);
			m_Direction[i]	= y != 0 ? (M_PI_180 + atan2((double)x, (double)y)) : (x > 0 ? M_PI_270 : M_PI_090);
		}
	}

	//-----------------------------------------------------
	m_Slope	.Create(*Get_System(), SG_DATATYPE_Float);
	m_Aspect.Create(*Get_System(), SG_DATATYPE_Float);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( m_pDTM->is_InGrid(x, y) && m_pDTM->Get_Gradient(x, y, Slope, Aspect) && Aspect >= 0.0 )
			{
				m_Slope	.Set_Value(x, y, Slope);
				m_Aspect.Set_Value(x, y, Aspect);
			}
			else
			{
				m_Slope	.Set_NoData(x, y);
				m_Aspect.Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	d;

			if( Get_Convergence(x, y, d) )
			{
				pConvergence->Set_Value(x, y, d);
			}
			else
			{
				pConvergence->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Cells		.Destroy();
	m_Direction	.Destroy();
	m_Slope		.Destroy();
	m_Aspect	.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvergence_Radius::Get_Convergence(int x, int y, double &Convergence)
{
	//-----------------------------------------------------
	if( !m_pDTM->is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		i, ix, iy;
	double	iDistance, iWeight, z;

	CSG_Simple_Statistics	s;

	//-----------------------------------------------------
	for(i=0, z=m_pDTM->asDouble(x, y); i<m_Cells.Get_Count(); i++)
	{
		if( m_Cells.Get_Values(i, ix = x, iy = y, iDistance, iWeight, true) && iDistance > 0.0 && m_Aspect.is_InGrid(ix, iy) )
		{
			double	d, Direction	= m_bDifference ? m_Direction[i] : m_Aspect.asDouble(x, y);

			d		= fmod(m_Aspect.asDouble(ix, iy) - Direction, M_PI_360);

			if( d < -M_PI_180 )
			{
				d	+= M_PI_360;
			}
			else if( d > M_PI_180 )
			{
				d	-= M_PI_360;
			}

		//	s.Add_Value(m_bSlope ? fabs(d) * tan(m_Slope(ix, iy)) : fabs(d), iWeight);
			s.Add_Value(fabs(d), m_bSlope ? iWeight * tan(m_Slope(ix, iy)) : iWeight);
		}
	}

	//-----------------------------------------------------
	if( s.Get_Count() > 0 )
	{
		Convergence	= (s.Get_Mean() - M_PI_090) * 100.0 / M_PI_090;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
