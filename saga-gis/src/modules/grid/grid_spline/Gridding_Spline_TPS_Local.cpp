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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	: CGridding_Spline_TPS_Global()
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Thin Plate Spline (Local)"));

	Set_Author		(SG_T("O.Conrad (c) 2006"));

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
	CSG_Parameter	*pSearch	= Parameters.Add_Node(
		NULL	, "NODE_SEARCH"			, _TL("Search Options"),
		_TL("")
	);

	pNode	= Parameters.Add_Choice(
		pSearch	, "SEARCH_RANGE"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("local"),
			_TL("global")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_RADIUS"		, _TL("Maximum Search Distance"),
		_TL("local maximum search distance given in map units"),
		PARAMETER_TYPE_Double	, 1000.0, 0, true
	);

	pNode	= Parameters.Add_Choice(
		pSearch	, "SEARCH_POINTS_ALL"	, _TL("Number of Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("maximum number of nearest points"),
			_TL("all points within search distance")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_POINTS_MAX"	, _TL("Maximum Number of Points"),
		_TL("maximum number of nearest points"),
		PARAMETER_TYPE_Int, 20, 1, true
	);

	Parameters.Add_Choice(
		pNode	, "SEARCH_DIRECTION"	, _TL("Search Direction"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all directions"),
			_TL("quadrants")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_TPS_Local::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SEARCH_RANGE")) )
	{
		pParameters->Get_Parameter("SEARCH_RADIUS"    )->Set_Enabled(pParameter->asInt() == 0);	// local
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SEARCH_POINTS_ALL")) )
	{
		pParameters->Get_Parameter("SEARCH_POINTS_MAX")->Set_Enabled(pParameter->asInt() == 0);	// maximum number of points
		pParameters->Get_Parameter("SEARCH_DIRECTION" )->Set_Enabled(pParameter->asInt() == 0);	// maximum number of points per quadrant
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_TPS_Local::On_Execute(void)
{
	m_nPoints_Max	= Parameters("SEARCH_POINTS_ALL")->asInt() == 0 ? Parameters("SEARCH_POINTS_MAX")->asInt   () : 0;
	m_Radius		= Parameters("SEARCH_RANGE"     )->asInt() == 0 ? Parameters("SEARCH_RADIUS"    )->asDouble() : 0.0;
	m_Direction		= Parameters("SEARCH_DIRECTION" )->asInt();

	//-----------------------------------------------------
	if( m_nPoints_Max <= 0 && m_Radius <= 0.0 )	// global
	{
		return( CGridding_Spline_TPS_Global::On_Execute() );
	}

	if( !Initialise() )
	{
		return( false );
	}

	if( !m_Search.Create(m_pShapes, m_zField) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			x, y;
	TSG_Point	p;

	for(y=0, p.y=m_pGrid->Get_YMin(); y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, p.y+=m_pGrid->Get_Cellsize())
	{
		for(x=0, p.x=m_pGrid->Get_XMin(); x<m_pGrid->Get_NX(); x++, p.x+=m_pGrid->Get_Cellsize())
		{
			Set_Value(x, y, p);
		}
	}

	m_Search.Destroy();
	m_Spline.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_TPS_Local::Get_Points(const TSG_Point &p, int iQuadrant)
{
	double	x, y, z;

	if( m_Search.Select_Nearest_Points(p.x, p.y, m_nPoints_Max, m_Radius, iQuadrant) > 0 )
	{
		for(int i=0; i<m_Search.Get_Selected_Count(); i++)
		{
			if( m_Search.Get_Selected_Point(i, x, y, z) )
			{
				m_Spline.Add_Point(x, y, z);
			}
		}
	}

	return( m_Search.Get_Selected_Count() );
}

//---------------------------------------------------------
bool CGridding_Spline_TPS_Local::Set_Value(int x, int y, const TSG_Point &p)
{
	int		nPoints	= 0;

	m_Spline.Destroy();

	switch( m_Direction )
	{
	default:
		nPoints	+= Get_Points(p);
		break;

	case 1:
		nPoints	+= Get_Points(p, 0);
		nPoints	+= Get_Points(p, 1);
		nPoints	+= Get_Points(p, 2);
		nPoints	+= Get_Points(p, 3);
		break;
	}

	//-----------------------------------------------------
	if( nPoints >= 3 && m_Spline.Create(m_Regularisation, true) )
	{
		m_pGrid->Set_Value(x, y, m_Spline.Get_Value(p.x, p.y));

		return( true );
	}

	m_pGrid->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
