/**********************************************************
 * Version $Id: ruggedness.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                    Ruggedness.cpp                     //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "ruggedness.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRuggedness_TRI::CRuggedness_TRI(void)
{
	Set_Name		(_TL("Terrain Ruggedness Index (TRI)"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"References:\n"
		"Riley, S.J., De Gloria, S.D., Elliot, R. (1999): "
		"A Terrain Ruggedness that Quantifies Topographic Heterogeneity. "
		"Intermountain Journal of Science, Vol.5, No.1-4, pp.23-27. "
		"<a href=\"http://www.fw.msu.edu/~rileysh2/Terrain%20Ruggedness%20Index.pdf\">online</a>.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "TRI"			, _TL("Terrain Ruggedness Index (TRI)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Int(
		NULL	, "RADIUS"		, _TL("Search Radius"),
		_TL("radius in cells"),
		1, 1, true
	);

	m_Cells.Get_Weighting().Set_BandWidth(75.0);	// 75%
	m_Cells.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CRuggedness_TRI::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Cells.Get_Weighting().Enable_Parameters(pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRuggedness_TRI::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM	= Parameters("DEM")->asGrid();
	m_pTRI	= Parameters("TRI")->asGrid();

	DataObject_Set_Colors(m_pTRI, 11, SG_COLORS_RED_GREY_BLUE, true);

	m_Cells.Get_Weighting().Set_Parameters(&Parameters);
	m_Cells.Get_Weighting().Set_BandWidth(Parameters("RADIUS")->asInt() * m_Cells.Get_Weighting().Get_BandWidth() / 100.0);

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asInt(), Parameters("MODE")->asInt() == 0) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Set_Index(x, y);
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRuggedness_TRI::Set_Index(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		i, ix, iy;
		double	z, iz, Distance, Weight, n, s;

		for(i=0, n=s=0.0, z=m_pDEM->asDouble(x, y); i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, Distance, Weight, true) && Weight > 0.0 && m_pDEM->is_InGrid(ix, iy) )
			{
				iz	 = m_pDEM->asDouble(ix, iy);
				s	+= SG_Get_Square((z - iz) * Weight);
				n	+= Weight;
			}
		}

		//-------------------------------------------------
		if( n > 0.0 )
		{
			m_pTRI->Set_Value(x, y, sqrt(s / n));

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pTRI->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRuggedness_VRM::CRuggedness_VRM(void)
{
	Set_Name		(_TL("Vector Ruggedness Measure (VRM)"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"References:\n"
		"Sappington, J.M., Longshore, K.M., Thompson, D.B. (2007): "
		"Quantifying Landscape Ruggedness for Animal Habitat Analysis: A Case Study Using Bighorn Sheep in the Mojave Desert. "
		"Journal of Wildlife Management 71(5):1419–1426.\n"
		"<a href=\"http://onlinelibrary.wiley.com/doi/10.2193/2005-723/abstract\">online</a>.\n"
		"\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "VRM"			, _TL("Vector Terrain Ruggedness (VRM)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Int(
		NULL	, "RADIUS"		, _TL("Search Radius"),
		_TL("radius in cells"),
		1, 1, true
	);

	m_Cells.Get_Weighting().Set_BandWidth(75.0);	// 75%
	m_Cells.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CRuggedness_VRM::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Cells.Get_Weighting().Enable_Parameters(pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRuggedness_VRM::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM	= Parameters("DEM")->asGrid();
	m_pVRM	= Parameters("VRM")->asGrid();

	DataObject_Set_Colors(m_pVRM, 11, SG_COLORS_RED_GREY_BLUE, true);

	m_Cells.Get_Weighting().Set_Parameters(&Parameters);
	m_Cells.Get_Weighting().Set_BandWidth(Parameters("RADIUS")->asInt() * m_Cells.Get_Weighting().Get_BandWidth() / 100.0);

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asInt(), Parameters("MODE")->asInt() == 0) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		x, y;

	m_X.Create(*Get_System(), SG_DATATYPE_Float);
	m_Y.Create(*Get_System(), SG_DATATYPE_Float);
	m_Z.Create(*Get_System(), SG_DATATYPE_Float);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			double	slope, aspect;

			if( m_pDEM->Get_Gradient(x, y, slope, aspect) )
			{
				m_X.Set_Value(x, y, aspect < 0.0 ? 0.0 : sin(slope) * sin(aspect));
				m_Y.Set_Value(x, y, aspect < 0.0 ? 0.0 : sin(slope) * cos(aspect));
				m_Z.Set_Value(x, y, cos(slope));
			}
			else
			{
				m_X.Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			Set_Index(x, y);
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	m_X.Destroy();
	m_Y.Destroy();
	m_Z.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRuggedness_VRM::Set_Index(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		i, ix, iy;
		double	Distance, Weight, n, sx, sy, sz;

		for(i=0, n=sx=sy=sz=0.0; i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, Distance, Weight, true) && Weight > 0.0 && m_X.is_InGrid(ix, iy) )
			{
				sx	+= Weight * m_X.asDouble(ix, iy);
				sy	+= Weight * m_Y.asDouble(ix, iy);
				sz	+= Weight * m_Z.asDouble(ix, iy);
				n	+= Weight;
			}
		}

		//-------------------------------------------------
		if( n > 0.0 )
		{
			m_pVRM->Set_Value(x, y, 1.0 - sqrt(sx*sx + sy*sy + sz*sz) / n);

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pVRM->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
