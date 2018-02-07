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
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     View_Shed.cpp                     //
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
#include "view_shed.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CView_Shed::CView_Shed(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Sky View Factor"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Calculation of visible sky, sky view factor (SVF) and related parameters."
	));

	Add_Reference("Boehner, J., Antonic, O.", "2009",
		"Land-surface parameters specific to topo-climatology",
		"In: Hengl, T., Reuter, H. (Eds.): Geomorphometry - Concepts, Software, Applications. "
		"Developments in Soil Science, Volume 33, p.195-226, Elsevier."
	);

	Add_Reference("Hantzschel, J., Goldberg, V., Bernhofer, C.", "2005",
		"GIS-based regionalisation of radiation, temperature and coupling measures in complex terrain for low mountain ranges",
		"Meteorological Applications, V.12:01, p.33-42, doi:10.1017/S1350482705001489."
	);

	Add_Reference("Oke, T.R.", "2000",
		"Boundary Layer Climates",
		"Taylor & Francis, New York. 435pp."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"VISIBLE"	, _TL("Visible Sky"),
		_TL("The unobstructed hemisphere given as percentage."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"SVF"		, _TL("Sky View Factor"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"SIMPLE"	, _TL("Sky View Factor (Simplified)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TERRAIN"	, _TL("Terrain View Factor"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"DISTANCE"	, _TL("Average View Distance"),
		_TL("Average distance to horizon."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"RADIUS"	, _TL("Maximum Search Radius"),
		_TL("The maximum search radius [map units]. This value is ignored if set to zero."),
		10000.0, 0.0, true
	);

	Parameters.Add_Int("",
		"NDIRS"		, _TL("Number of Sectors"),
		_TL(""),
		8, 3, true
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("cell size"),
			_TL("multi scale")
		), 0
	);

	Parameters.Add_Double("",
		"DLEVEL"	, _TL("Multi Scale Factor"),
		_TL(""),
		3.0, 1.25, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CView_Shed::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("DLEVEL", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")->asGrid();

	CSG_Grid *pVisible	= Parameters("VISIBLE" )->asGrid(); DataObject_Set_Colors(pVisible ,  2, SG_COLORS_BLACK_WHITE);
	CSG_Grid *pSVF		= Parameters("SVF"     )->asGrid(); DataObject_Set_Colors(pSVF     ,  2, SG_COLORS_BLACK_WHITE);
	CSG_Grid *pSimple	= Parameters("SIMPLE"  )->asGrid(); DataObject_Set_Colors(pSimple  ,  2, SG_COLORS_BLACK_WHITE);
	CSG_Grid *pTerrain	= Parameters("TERRAIN" )->asGrid(); DataObject_Set_Colors(pTerrain ,  2, SG_COLORS_BLACK_WHITE, true);
	CSG_Grid *pDistance	= Parameters("DISTANCE")->asGrid(); DataObject_Set_Colors(pDistance, 11, SG_COLORS_RED_GREEN, true);

	//-----------------------------------------------------
	m_Radius	= Parameters("RADIUS")->asDouble();

	m_Method	= Parameters("METHOD")->asInt();

	switch( m_Method )
	{
	default:	// cell size
		if( m_Radius <= 0.0 )
		{
			m_Radius	= Get_Cellsize() * M_GET_LENGTH(Get_NX(), Get_NY());
		}
		break;

	case  1:	// multi scale
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
		break;
	}

	//-----------------------------------------------------
	m_Direction.Set_Count(Parameters("NDIRS")->asInt());

	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		m_Direction[i].z	= (M_PI_360 * i) / m_Direction.Get_Count();
		m_Direction[i].x	= sin(m_Direction[i].z);
		m_Direction[i].y	= cos(m_Direction[i].z);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Visible, SVF, Simple, Terrain, Distance;

			if( Get_View_Shed(x, y, Visible, SVF, Simple, Terrain, Distance) )
			{
				if( pVisible  )	pVisible ->Set_Value(x, y, Visible );
				if( pSVF      )	pSVF	 ->Set_Value(x, y, SVF     );
				if( pSimple   )	pSimple	 ->Set_Value(x, y, Simple  );
				if( pTerrain  )	pTerrain ->Set_Value(x, y, Terrain );
				if( pDistance )	pDistance->Set_Value(x, y, Distance);
			}
			else
			{
				if( pVisible  )	pVisible ->Set_NoData(x, y);
				if( pSVF      )	pSVF	 ->Set_NoData(x, y);
				if( pSimple   )	pSimple	 ->Set_NoData(x, y);
				if( pTerrain  )	pTerrain ->Set_NoData(x, y);
				if( pDistance )	pDistance->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Pyramid  .Destroy();
	m_Direction.Clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::Get_View_Shed(int x, int y, double &Sky_Visible, double &Sky_Factor, double &Sky_Simple, double &Sky_Terrain, double &Distance)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	Angles(m_Direction.Get_Count()), Distances(m_Direction.Get_Count());

	switch( m_Method )
	{
	default: if( !Get_Angles_Sectoral   (x, y, Angles, Distances) )	return( false ); break;
	case  1: if( !Get_Angles_Multi_Scale(x, y, Angles, Distances) )	return( false ); break;
	}

	//-----------------------------------------------------
	double	Slope, Aspect;

	if( !m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
	{
		Slope	= Aspect	= 0.0;
	}

	double	sinSlope	= sin(Slope);
	double	cosSlope	= cos(Slope);

	Sky_Visible	= 0.0;
	Sky_Factor	= 0.0;
	Distance	= 0.0;

	//-----------------------------------------------------
	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		double	Phi		= atan(Angles[i]);
		double	cosPhi	= cos(Phi);
		double	sinPhi	= sin(Phi);

		Sky_Visible	+= (M_PI_090 - Phi) * 100.0 / M_PI_090;
		Sky_Factor	+= cosSlope * cosPhi*cosPhi + sinSlope * cos(m_Direction[i].z - Aspect) * ((M_PI_090 - Phi) - sinPhi * cosPhi);
		Distance	+= Distances[i];
	}

	Sky_Visible	/= m_Direction.Get_Count();
	Sky_Factor	/= m_Direction.Get_Count();
	Distance	/= m_Direction.Get_Count();

	Sky_Simple	= (1.0 + cosSlope) / 2.0;
	Sky_Terrain	= Sky_Simple - Sky_Factor;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::Get_Angles_Multi_Scale(int x, int y, CSG_Vector &Angles, CSG_Vector &Distances)
{
	double	d, z	= m_pDEM->asDouble(x, y);

	TSG_Point	q, p	= Get_System()->Get_Grid_to_World(x, y);

	Distances	= 0.0;

	for(int iGrid=-1; iGrid<m_nLevels; iGrid++)
	{
		CSG_Grid	*pGrid	= m_Pyramid.Get_Grid(iGrid);

		for(int i=0; i<m_Direction.Get_Count(); i++)
		{
			q.x	= p.x + pGrid->Get_Cellsize() * m_Direction[i].x;
			q.y	= p.y + pGrid->Get_Cellsize() * m_Direction[i].y;

			if( pGrid->Get_Value(q, d) && (d = (d - z) / pGrid->Get_Cellsize()) > Angles[i] )
			{
				Angles   [i]	= d;
				Distances[i]	= pGrid->Get_Cellsize();
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::Get_Angles_Sectoral(int x, int y, CSG_Vector &Angles, CSG_Vector &Distances)
{
	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		Get_Angle_Sectoral(x, y, i, Angles[i], Distances[i]);
	}

	return( true );
}

//---------------------------------------------------------
void CView_Shed::Get_Angle_Sectoral(int x, int y, int i, double &Angle, double &Distance)
{
	double	iDistance, dDistance, dx, dy, ix, iy, d, z;

	z			= m_pDEM->asDouble(x, y);
	dx			= m_Direction[i].x;
	dy			= m_Direction[i].y;
	ix			= x;
	iy			= y;
	Angle		= 0.0;
	Distance	= m_Radius;
	iDistance	= 0.0;
	dDistance	= Get_Cellsize() * M_GET_LENGTH(dx, dy);

	while( is_InGrid(x, y) && iDistance < m_Radius )
	{
		ix	+= dx;	x	= (int)(0.5 + ix);
		iy	+= dy;	y	= (int)(0.5 + iy);
		iDistance	+= dDistance;

		if( m_pDEM->is_InGrid(x, y) && (d = (m_pDEM->asDouble(x, y) - z) / iDistance) > Angle )
		{
			Angle		= d;
			Distance	= iDistance;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
