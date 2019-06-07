
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
//                    geomorphons.cpp                    //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "geomorphons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------
enum
{
	id_fl	= 1,
	id_pk,
	id_ri,
	id_sh,
	id_sp,
	id_sl,
	id_hl,
	id_fs,
	id_vl,
	id_pt,
	id_count = id_pt
};

//-----------------------------------------------------
const int id_color[id_count]	=
{
	SG_GET_RGB(220, 220, 220),	// flat
	SG_GET_RGB(100,   0,   0),	// summit
	SG_GET_RGB(200,   0,   0),	// ridge
	SG_GET_RGB(255,  80,  20),	// shoulder
	SG_GET_RGB(250, 210,  60),	// spur
	SG_GET_RGB(255, 255,  60),	// slope
	SG_GET_RGB(180, 230,  20),	// hollow
	SG_GET_RGB( 60, 250, 150),	// footslope
	SG_GET_RGB(  0,   0, 255),	// valley
	SG_GET_RGB(  0,   0, 100)	// depression
};

//-----------------------------------------------------
const CSG_String id_name[id_count]	=
{
	_TL("Flat"      ),
	_TL("Summit"    ),
	_TL("Ridge"     ),
	_TL("Shoulder"  ),
	_TL("Spur"      ),
	_TL("Slope"     ),
	_TL("Hollow"    ),
	_TL("Footslope" ),
	_TL("Valley"    ),
	_TL("Depression"),
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeomorphons::CGeomorphons(void)
{
	Set_Name		(_TL("Geomorphons"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"This tool derives so called geomorphons, which represent categories of terrain forms, "
		"from a digital elevation model using a machine vision approach. "
	));

	Add_Reference("Jasiewicz, J. / Stepinski, T.F.", "2013",
		"Geomorphons — a pattern recognition approach to classification and mapping of landforms",
		"Geomorphology, 182, 147-156.",
		SG_T("https://www.sciencedirect.com/science/article/pii/S0169555X12005028"), SG_T("ScienceDirect")
	);

	Add_Reference("Stepinski, T.F. / Jasiewicz, J.", "2011",
		"Geomorphons — a pattern recognition approach to classification and mapping of landforms",
		"In: Hengl, T. / Evans, I.S. / Wilson, J.P. / Gould, M. [Eds.]: Proceedings of Geomorphometry 2011, Redlands, 109-112.",
		SG_T("http://geomorphometry.org/system/files/StepinskiJasiewicz2011geomorphometry.pdf"), SG_T("pdf at geomorphometry.org")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "GEOMORPHONS"	, _TL("Geomorphons"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Double(
		"", "THRESHOLD"		, _TL("Threshold Angle"),
		_TL("Flatness threshold angle (degrees)."),
		1., 0., true
	);

	Parameters.Add_Double(
		"", "RADIUS"		, _TL("Radial Limit"),
		_TL(""),
		10000., 0., true
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("multi scale"),
			_TL("line tracing")
		), 1
	);

	Parameters.Add_Double(
		"", "DLEVEL"	, _TL("Multi Scale Factor"),
		_TL(""),
		3., 1.25, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeomorphons::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("DLEVEL", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeomorphons::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")->asGrid();

	m_Threshold	= Parameters("THRESHOLD")->asDouble() * M_DEG_TO_RAD;

	m_Radius	= Parameters("RADIUS")->asDouble();

	m_Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	if( m_Method == 0 )	// multi scale
	{
		if( !m_Pyramid.Create(m_pDEM, Parameters("DLEVEL")->asDouble(), GRID_PYRAMID_Mean) )
		{
			Error_Set(_TL("failed to create pyramids."));

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
	else if( m_Radius <= 0.0 )
	{
		m_Radius	= Get_Cellsize() * M_GET_LENGTH(Get_NX(), Get_NY());
	}

	//-----------------------------------------------------
	CSG_Grid	*pGeomorphons	= Parameters("GEOMORPHONS")->asGrid();

	pGeomorphons->Set_NoData_Value(0);

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pGeomorphons, "LUT");

	if( pLUT )
	{
		CSG_Table	*pTable	= pLUT->asTable();

		pLUT->asTable()->Del_Records();

		for(int i=0; i<id_count; i++)
		{
			CSG_Table_Record	*pRecord	= pLUT->asTable()->Add_Record();

			pRecord->Set_Value(0, id_color[i]);
			pRecord->Set_Value(1, id_name [i].c_str());
			pRecord->Set_Value(2, id_name [i].c_str());
			pRecord->Set_Value(3, i + 1);
			pRecord->Set_Value(4, i + 1);
		}

		DataObject_Set_Parameter(pGeomorphons, pLUT);
		DataObject_Set_Parameter(pGeomorphons, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Geomorphon;

			if( !m_pDEM->is_NoData(x, y) && Get_Geomorphon(x, y, Geomorphon) )
			{
				if( pGeomorphons )	pGeomorphons->Set_Value(x, y, Geomorphon);
			}
			else
			{
				if( pGeomorphons )	pGeomorphons->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeomorphons::Get_Geomorphon(int x, int y, int &Geomorphon)
{
	CSG_Vector	Max(8), Min(8);

	switch( m_Method )
	{
	case  0: if( !Get_Angles_Multi_Scale(x, y, Max, Min) ) return( false ); break;
	default: if( !Get_Angles_Sectoral   (x, y, Max, Min) ) return( false ); break;
	}

	//-----------------------------------------------------
	int	t[8], pos = 0, neg = 0;

	for(int i=0; i<8; i++)
	{
		double	phi	= M_PI_090 - atan(Max[i]);
		double	psi	= M_PI_090 + atan(Min[i]);

		if     ( psi - phi > m_Threshold )
		{
			t[i]	=  1;	pos++;
		}
		else if( phi - psi > m_Threshold )
		{
			t[i]	= -1;	neg++;
		}
		else
		{
			t[i]	=  0;
		}
	}

	//-----------------------------------------------------
	switch( pos )
	{
	case  0:
		Geomorphon	= neg < 3 ? id_fl : neg < 5 ? id_sh : neg < 8 ? id_ri : id_pk;
		break;

	case  1:
		Geomorphon	= neg < 2 ? id_fl : neg < 5 ? id_sh : id_ri;
		break;

	case  2:
		Geomorphon	= neg < 1 ? id_fl : neg < 2 ? id_fs : neg < 4 ? id_sl : neg < 6 ? id_sp : id_ri;
		break;

	case  3:
		Geomorphon	= neg < 2 ? id_fs : neg < 5 ? id_sl : id_sp;
		break;

	case  4:
		Geomorphon	= neg < 2 ? id_fs : neg < 3 ? id_hl : id_sl;
		break;

	case  5:
		Geomorphon	= neg < 2 ? id_vl : id_hl;
		break;

	case  6:
		Geomorphon	= id_vl;
		break;

	case  7:
		Geomorphon	= id_vl;
		break;

	case  8:
		Geomorphon	= id_pt;
		break;

	default:
		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeomorphons::Get_Angles_Multi_Scale(int x, int y, CSG_Vector &Max, CSG_Vector &Min)
{
	double		z, d;
	TSG_Point	p, q;

	z	= m_pDEM->asDouble(x, y);
	p	= Get_System().Get_Grid_to_World(x, y);

	//-----------------------------------------------------
	for(int iGrid=-1; iGrid<m_nLevels; iGrid++)
	{
		bool		bOkay	= false;
		CSG_Grid	*pGrid	= m_Pyramid.Get_Grid(iGrid);

		for(int i=0; i<8; i++)
		{
			q.x	= p.x + pGrid->Get_Cellsize() * m_dx[i];
			q.y	= p.y + pGrid->Get_Cellsize() * m_dy[i];

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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeomorphons::Get_Angles_Sectoral(int x, int y, CSG_Vector &Max, CSG_Vector &Min)
{
	for(int i=0; i<8; i++)
	{
		if(0|| Get_Angle_Sectoral(x, y, i, Max[i], Min[i]) == false )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGeomorphons::Get_Angle_Sectoral(int x, int y, int i, double &Max, double &Min)
{
	double	iDistance, dDistance, dx, dy, ix, iy, d, z;

	z			= m_pDEM->asDouble(x, y);
	dx			= m_dx[i];
	dy			= m_dy[i];
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
