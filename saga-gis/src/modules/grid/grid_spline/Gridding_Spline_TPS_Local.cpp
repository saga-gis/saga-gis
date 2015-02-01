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
#include "Gridding_Spline_TPS_Local.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_TPS_Local::CGridding_Spline_TPS_Local(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Thin Plate Spline"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Creates a 'Thin Plate Spline' function for each grid point "
		"based on all of the scattered data points that are within a "
		"given distance. The number of points can be limited to a "
		"maximum number of closest points. "
		"\n\n"
		"References:\n"
		"- Donato G., Belongie S. (2002):"
		" 'Approximation Methods for Thin Plate Spline Mappings and Principal Warps',"
		" In Heyden, A., Sparr, G., Nielsen, M., Johansen, P. (Eds.):"
		" 'Computer Vision - ECCV 2002: 7th European Conference on Computer Vision, Copenhagen, Denmark, May 28-31, 2002',"
		" Proceedings, Part III, Lecture Notes in Computer Science."
		" Springer-Verlag Heidelberg; pp.21-31."
		"\n"
		"\n"
		"- Elonen, J. (2005):"
		" 'Thin Plate Spline editor - an example program in C++',"
		" <a target=\"_blank\" href=\"http://elonen.iki.fi/code/tpsdemo/index.html\">http://elonen.iki.fi/code/tpsdemo/index.html</a>."
		"\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL, "REGULARISATION"	, _TL("Regularisation"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0001, 0.0, true
	);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node(NULL, "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_TPS_Local::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES") )
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
	int			x, y;
	TSG_Point	p;

	m_Regularisation	= Parameters("REGULARISATION")->asDouble();

	//-----------------------------------------------------
	if( m_Search.Do_Use_All(true) )	// global
	{
		if( !Initialise(m_Spline.Get_Points()) || !m_Spline.Create(m_Regularisation, false) )
		{
			return(false);
		}

		for(y=0, p.y=m_pGrid->Get_YMin(); y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, p.y+=m_pGrid->Get_Cellsize())
		{
			for(x=0, p.x=m_pGrid->Get_XMin(); x<m_pGrid->Get_NX(); x++, p.x+=m_pGrid->Get_Cellsize())
			{
				m_pGrid->Set_Value(x, y, m_Spline.Get_Value(p.x, p.y));
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		if( !Initialise() || !m_Search.Initialize(Parameters("SHAPES")->asShapes(), Parameters("FIELD")->asInt()) )
		{
			return(false);
		}

		for(y=0, p.y=m_pGrid->Get_YMin(); y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, p.y+=m_pGrid->Get_Cellsize())
		{
			for(x=0, p.x=m_pGrid->Get_XMin(); x<m_pGrid->Get_NX(); x++, p.x+=m_pGrid->Get_Cellsize())
			{
				Set_Value(x, y, p);
			}
		}

		m_Search.Finalize();
	}

	//-----------------------------------------------------
	m_Spline.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_TPS_Local::Set_Value(int x, int y, const TSG_Point &p)
{
	if( m_Search.Set_Location(p) && m_Search.Get_Count() >= 3 )
	{
		m_Spline.Destroy();

		for(int i=0; i<m_Search.Get_Count(); i++)
		{
			double	ix, iy, iz;

			if( m_Search.Get_Point(i, ix, iy, iz) )
			{
				m_Spline.Add_Point(ix, iy, iz);
			}
		}

		//-------------------------------------------------
		if( m_Spline.Create(m_Regularisation, true) )
		{
			m_pGrid->Set_Value(x, y, m_Spline.Get_Value(p.x, p.y));

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
