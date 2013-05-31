/**********************************************************
 * Version $Id: topographic_openness.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                topographic_openness.cpp               //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "topographic_openness.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTopographic_Openness::CTopographic_Openness(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Topographic Openness"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Topographic openness expresses the dominance (positive) or enclosure (negative) "
		"of a landscape location. See Yokoyama et al. (2002) for a precise definition. "
		"Openness has been related to how wide a landscape can be viewed from any position. "
		"It has been proven to be a meaningful input for computer aided geomorphological mapping.\n"
		"\n"
		"References:\n"
		"Anders, N. S. / Seijmonsbergen, A. C. / Bouten, W. (2009): "
		"Multi-Scale and Object-Oriented Image Analysis of High-Res LiDAR Data for Geomorphological Mapping in Alpine Mountains. "
		"Proceedings of Geomorphometry 2009. "
		"<a target=\"_blank\" href=\"http://geomorphometry.org/system/files/anders2009geomorphometry.pdf\">online at geomorphometry.org</a>.\n\n"

		"Prima, O.D.A / Echigo, A. / Yokoyama, R. / Yoshida, T. (2006): "
		"Supervised landform classification of Northeast Honshu from DEM-derived thematic maps. "
		"Geomorphology, vol.78, pp.373-386.\n\n"

		"Yokoyama, R. / Shirasawa, M. / Pike, R.J. (2002): "
		"Visualizing topography by openness: A new application of image processing to digital elevation models. "
		"Photogrammetric Engineering and Remote Sensing, Vol.68, pp.251-266. "
		"<a target=\"_blank\" href=\"http://www.asprs.org/a/publications/pers/2002journal/march/2002_mar_257-265.pdf\">online at ASPRS</a>.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "POS"			, _TL("Positive Openness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "NEG"			, _TL("Negative Openness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radial Limit"),
		_TL(""),
		PARAMETER_TYPE_Double	, 10000.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("multi scale"),
			_TL("sectors")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "DLEVEL"		, _TL("Multi Scale Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 3.0, 1.25, true
	);

	Parameters.Add_Value(
		NULL	, "NDIRS"		, _TL("Number of Sectors"),
		_TL(""),
		PARAMETER_TYPE_Int		, 8.0, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::On_Execute(void)
{
	CSG_Grid	*pPos, *pNeg;

	m_pDEM		= Parameters("DEM"   )->asGrid();
	pPos		= Parameters("POS"   )->asGrid();
	pNeg		= Parameters("NEG"   )->asGrid();

	m_Radius	= Parameters("RADIUS")->asDouble();
	m_Method	= Parameters("METHOD")->asInt();

	DataObject_Set_Colors(pPos, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(pNeg, 100, SG_COLORS_RED_GREY_BLUE, false);

	//-----------------------------------------------------
	if( m_Method == 0 )	// multi scale
	{
		if( !m_Pyramid.Create(m_pDEM, Parameters("DLEVEL")->asDouble(), GRID_PYRAMID_Mean) )
		{
			return( false );
		}

		m_nLevels	= m_Pyramid.Get_Count();

		if( m_Radius > 0.0 )
		{
			while( m_nLevels > 0 && m_Pyramid.Get_Grid(m_nLevels - 1)->Get_Cellsize() > m_Radius )
			{
				m_nLevels--;
			}
		}
	}

	//-----------------------------------------------------
	bool	bResult	= Initialise(Parameters("NDIRS")->asInt());

	if( bResult )
	{
		if( m_Method != 0 && m_Radius <= 0.0 )
		{
			m_Radius	= Get_Cellsize() * M_GET_LENGTH(Get_NX(), Get_NY());
		}

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	Pos, Neg;

				if( Get_Openness(x, y, Pos, Neg) )
				{
					if( pPos )	pPos->Set_Value(x, y, Pos);
					if( pNeg )	pNeg->Set_Value(x, y, Neg);
				}
				else
				{
					if( pPos )	pPos->Set_NoData(x, y);
					if( pNeg )	pNeg->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Pyramid	.Destroy();
	m_Direction	.Clear();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Initialise(int nDirections)
{
	m_Direction.Set_Count(nDirections);

	for(int i=0; i<nDirections; i++)
	{
		m_Direction[i].z	= (M_PI_360 * i) / nDirections;
		m_Direction[i].x	= sin(m_Direction[i].z);
		m_Direction[i].y	= cos(m_Direction[i].z);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Get_Openness(int x, int y, double &Pos, double &Neg)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	Max(m_Direction.Get_Count()), Min(m_Direction.Get_Count());

	switch( m_Method )
	{
	case 0:	if( !Get_Angles_Multi_Scale(x, y, Max, Min) )	return( false );	break;
	case 1:	if( !Get_Angles_Sectoral   (x, y, Max, Min) )	return( false );	break;
	}

	//-----------------------------------------------------
	Pos	= 0.0;
	Neg	= 0.0;

	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		Pos	+= M_PI_090 - atan(Max[i]);
		Neg	+= M_PI_090 + atan(Min[i]);
	}

	Pos	/= m_Direction.Get_Count();
	Neg	/= m_Direction.Get_Count();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Get_Angles_Multi_Scale(int x, int y, CSG_Vector &Max, CSG_Vector &Min)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	double		z, d;
	TSG_Point	p, q;

	z	= m_pDEM->asDouble(x, y);
	p	= Get_System()->Get_Grid_to_World(x, y);

	//-----------------------------------------------------
	for(int iGrid=-1; iGrid<m_nLevels; iGrid++)
	{
		bool		bOkay	= false;
		CSG_Grid	*pGrid	= m_Pyramid.Get_Grid(iGrid);

		for(int i=0; i<m_Direction.Get_Count(); i++)
		{
			q.x	= p.x + pGrid->Get_Cellsize() * m_Direction[i].x;
			q.y	= p.y + pGrid->Get_Cellsize() * m_Direction[i].y;

			if( pGrid->Get_Value(q, d) )
			{
				d	= (d - z) / pGrid->Get_Cellsize();

				if( bOkay == false )
				{
					bOkay	= true;
					Max[i]	= Min[i]	= d;
				}
				else if( Max[i] < d )
				{
					Max[i]	= d;
				}
				else if( Min[i] > d )
				{
					Min[i]	= d;
				}
			}
		}

		if(0|| bOkay == false )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CTopographic_Openness::Get_Angles_Sectoral(int x, int y, CSG_Vector &Max, CSG_Vector &Min)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		if(0|| Get_Angle_Sectoral(x, y, i, Max[i], Min[i]) == false )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CTopographic_Openness::Get_Angle_Sectoral(int x, int y, int i, double &Max, double &Min)
{
	double	iDistance, dDistance, dx, dy, ix, iy, d, z;

	z			= m_pDEM->asDouble(x, y);
	dx			= m_Direction[i].x;
	dy			= m_Direction[i].y;
	ix			= x;
	iy			= y;
	iDistance	= 0.0;
	dDistance	= Get_Cellsize() * M_GET_LENGTH(dx, dy);
	Max			= 0.0;
	Min			= 0.0;

	bool	bOkay	= false;

	while( is_InGrid(x, y) && iDistance <= m_Radius )
	{
		ix	+= dx;	x	= (int)(0.5 + ix);
		iy	+= dy;	y	= (int)(0.5 + iy);
		iDistance	+= dDistance;

		if( m_pDEM->is_InGrid(x, y) )
		{
			d	= (m_pDEM->asDouble(x, y) - z) / iDistance;

			if( bOkay == false )
			{
				bOkay		= true;
				Max	= Min	= d;
			}
			else if( Max < d )
			{
				Max	= d;
			}
			else if( Min > d )
			{
				Min	= d;
			}
		}
	}

	return( bOkay );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
