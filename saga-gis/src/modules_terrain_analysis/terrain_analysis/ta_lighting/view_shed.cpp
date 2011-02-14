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

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"\n"
		"\n"
		"References:\n"
		"Boehner, J., Antonic, O. (2008): "
		"'Land-suface parameters specific to topo-climatology'. "
		"in: Hengl, T., Reuter, H. (Eds.): 'Geomorphometry - Concepts, Software, Applications', in press\n"
		"\n"
		"Hantzschel, J., Goldberg, V., Bernhofer, C. (2005): "
		"'GIS-based regionalisation of radiation, temperature and coupling measures in complex terrain for low mountain ranges'. "
		"Meteorological Applications, V.12:01, p.33–42, doi:10.1017/S1350482705001489\n"
		"\n"
		"Oke, T.R. (2000): "
		"'Boundary Layer Climates'. "
		"Taylor & Francis, New York. 435pp.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "VISIBLE"		, _TL("Visible Sky"),
		_TL("The unobstructed hemisphere given as percentage."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SVF"			, _TL("Sky View Factor"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SIMPLE"		, _TL("Sky View Factor (Simplified)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "TERRAIN"		, _TL("Terrain View Factor"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "MAXRADIUS"	, _TL("Maximum Search Radius"),
		_TL("This value is ignored if set to zero."),
		PARAMETER_TYPE_Double	, 10000.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("multi scale"),
			_TL("sectors")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "LEVEL_INC"	, _TL("Multi Scale Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 3.0, 1.25, true
	);

	Parameters.Add_Value(
		NULL	, "NDIRS"		, _TL("Number of Sectors"),
		_TL(""),
		PARAMETER_TYPE_Int	, 8.0, 3, true
	);
}

//---------------------------------------------------------
CView_Shed::~CView_Shed(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::On_Execute(void)
{
	bool		bResult	= false;
	int			nDirections;
	double		Visible, SVF, Simple, Terrain, Level_Inc;
	CSG_Grid	*pVisible, *pSVF, *pSimple, *pTerrain;

	m_pDEM		= Parameters("DEM")			->asGrid();

	pVisible	= Parameters("VISIBLE")		->asGrid();
	pSVF		= Parameters("SVF")			->asGrid();
	pSimple		= Parameters("SIMPLE")		->asGrid();
	pTerrain	= Parameters("TERRAIN")		->asGrid();

	m_MaxRadius	= Parameters("MAXRADIUS")	->asDouble();
	m_Method	= Parameters("METHOD")		->asInt();

	Level_Inc	= Parameters("LEVEL_INC")	->asDouble();
	nDirections	= Parameters("NDIRS")		->asInt();

	DataObject_Set_Colors(pVisible	, 100, SG_COLORS_BLACK_WHITE);
	DataObject_Set_Colors(pSVF		, 100, SG_COLORS_BLACK_WHITE);
	DataObject_Set_Colors(pSimple	, 100, SG_COLORS_BLACK_WHITE);
	DataObject_Set_Colors(pTerrain	, 100, SG_COLORS_BLACK_WHITE, true);

	//-----------------------------------------------------
	switch( m_Method )
	{
	case 0:	// multi scale
		if( m_Pyramid.Create(m_pDEM, Level_Inc, GRID_PYRAMID_Mean) )
		{
			m_MaxLevel	= m_Pyramid.Get_Count();

			if( m_MaxRadius > 0.0 )
			{
				while( m_MaxLevel > 0 && m_Pyramid.Get_Grid(m_MaxLevel - 1)->Get_Cellsize() > m_MaxRadius )
				{
					m_MaxLevel--;
				}
			}

			bResult	= Initialise(8);
		}
		break;

	case 1:	// sectors
		bResult		= Initialise(nDirections);
		break;
	}

	if( m_Method != 0 && m_MaxRadius <= 0.0 )
	{
		m_MaxRadius	= Get_Cellsize() * M_GET_LENGTH(Get_NX(), Get_NY());
	}

	//-----------------------------------------------------
	if( bResult )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( Get_View_Shed(x, y, Visible, SVF, Simple, Terrain) )
				{
					if( pVisible )	pVisible->Set_Value (x, y, Visible);
					if( pSVF )		pSVF	->Set_Value (x, y, SVF);
					if( pSimple )	pSimple	->Set_Value (x, y, Simple);
					if( pTerrain )	pTerrain->Set_Value (x, y, Terrain);
				}
				else
				{
					if( pVisible )	pVisible->Set_NoData(x, y);
					if( pSVF )		pSVF	->Set_NoData(x, y);
					if( pSimple )	pSimple	->Set_NoData(x, y);
					if( pTerrain )	pTerrain->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Pyramid	.Destroy();
	m_Angles	.Destroy();
	m_Direction	.Clear();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::Initialise(int nDirections)
{
	m_Angles	.Create		(nDirections);
	m_Direction	.Set_Count	(nDirections);

	for(int iDirection=0; iDirection<nDirections; iDirection++)
	{
		m_Direction[iDirection].z	= (M_PI_360 * iDirection) / nDirections;
		m_Direction[iDirection].x	= sin(m_Direction[iDirection].z);
		m_Direction[iDirection].y	= cos(m_Direction[iDirection].z);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::Get_View_Shed(int x, int y, double &Sky_Visible, double &Sky_Factor, double &Sky_Simple, double &Sky_Terrain)
{
	double	slope, aspect;

	if( m_pDEM->Get_Gradient(x, y, slope, aspect) )
	{
		bool	bResult;

		switch( m_Method )
		{
		case 0:		bResult	= Get_Angles_Multi_Scale(x, y);	break;
		default:	bResult	= Get_Angles_Sectoral   (x, y);	break;
		}

		if( bResult )
		{
			double	sinSlope, cosSlope, Phi, sinPhi, cosPhi;

			Sky_Visible	= 0.0;
			Sky_Factor	= 0.0;

			sinSlope	= sin(slope);
			cosSlope	= cos(slope);

			for(int iDirection=0; iDirection<m_Angles.Get_N(); iDirection++)
			{
				Phi			= atan(m_Angles[iDirection]);
				cosPhi		= cos(Phi);
				sinPhi		= sin(Phi);

				Sky_Visible	+= (M_PI_090 - Phi) * 100.0 / M_PI_090;
				Sky_Factor	+= cosSlope * cosPhi*cosPhi + sinSlope * cos(m_Direction[iDirection].z - aspect) * ((M_PI_090 - Phi) - sinPhi * cosPhi);
			}

			Sky_Visible	/= m_Angles.Get_N();
			Sky_Factor	/= m_Angles.Get_N();

			Sky_Simple	= (1.0 + cosSlope) / 2.0;
			Sky_Terrain	= Sky_Simple - Sky_Factor;

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Shed::Get_Angles_Multi_Scale(int x, int y)
{
	if( !m_pDEM->is_NoData(x, y) )
	{
		double		z, d;
		TSG_Point	p, q;

		z	= m_pDEM->asDouble(x, y);
		p	= Get_System()->Get_Grid_to_World(x, y);

		m_Angles.Assign(0.0);

		//-------------------------------------------------
		for(int iGrid=-1; iGrid<m_MaxLevel; iGrid++)
		{
			CSG_Grid	*pGrid	= m_Pyramid.Get_Grid(iGrid);

			for(int iDirection=0; iDirection<8; iDirection++)
			{
				q.x	= p.x + pGrid->Get_Cellsize() * m_Direction[iDirection].x;
				q.y	= p.y + pGrid->Get_Cellsize() * m_Direction[iDirection].y;

				if( pGrid->Get_Value(q, d) && (d = (d - z) / pGrid->Get_Cellsize()) > m_Angles[iDirection] )
				{
					m_Angles[iDirection]	= d;
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CView_Shed::Get_Angles_Sectoral(int x, int y)
{
	if( !m_pDEM->is_NoData(x, y) )
	{
		m_Angles.Assign(0.0);

		//-------------------------------------------------
		for(int iDirection=0; iDirection<m_Angles.Get_N(); iDirection++)
		{
			m_Angles[iDirection]	= Get_Angle_Sectoral(x, y, m_Direction[iDirection].x, m_Direction[iDirection].y);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CView_Shed::Get_Angle_Sectoral(int x, int y, double dx, double dy)
{
	double	Angle, Distance, dDistance, ix, iy, d, z;

	z			= m_pDEM->asDouble(x, y);
	ix			= x;
	iy			= y;
	Angle		= 0.0;
	Distance	= 0.0;
	dDistance	= Get_Cellsize() * M_GET_LENGTH(dx, dy);

	while( is_InGrid(x, y) && Distance <= m_MaxRadius )
	{
		ix	+= dx;	x	= (int)(0.5 + ix);
		iy	+= dy;	y	= (int)(0.5 + iy);
		Distance	+= dDistance;

		if( m_pDEM->is_InGrid(x, y) && (d = (m_pDEM->asDouble(x, y) - z) / Distance) > Angle )
		{
			Angle	= d;
		}
	}

	return( Angle );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
