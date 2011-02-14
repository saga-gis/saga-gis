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

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"\n"
		"References:\n"
		"\n"
		"Riley, S.J., De Gloria, S.D., Elliot, R. (1999): "
		"A Terrain Ruggedness that Quantifies Topographic Heterogeneity. "
		"Intermountain Journal of Science, Vol.5, No.1-4, pp.23-27. "
		"<a href=\"http://www.fw.msu.edu/~rileysh2/Terrain%20Ruggedness%20Index.pdf\">pdf</a>.\n"
		"\n"
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

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	)->asParameters()->Assign(m_Cells.Get_Weighting().Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRuggedness_TRI::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	m_pDEM	= Parameters("DEM")	->asGrid();
	m_pTRI	= Parameters("TRI")	->asGrid();

	DataObject_Set_Colors(m_pTRI, 100, SG_COLORS_RED_GREY_BLUE, true);

	m_Cells.Get_Weighting().Set_Parameters(Parameters("WEIGHTING")->asParameters());

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asInt()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
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
		double	z, iz, iDistance, iWeight, n, s;

		for(i=0, n=0, s=0.0, z=m_pDEM->asDouble(x, y); i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, iDistance, iWeight, true) && iDistance > 0.0 && m_pDEM->is_InGrid(ix, iy) )
			{
				iz	 = m_pDEM->asDouble(ix, iy);
				s	+= SG_Get_Square((z - iz) * iWeight);
				n	+= iWeight;
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

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"\n"
		"References:\n"
		"Sappington, J.M., Longshore, K.M., Thompson, D.B. (2007): "
		"Quantifying Landscape Ruggedness for Animal Habitat Analysis: A Case Study Using Bighorn Sheep in the Mojave Desert. "
		"Journal of Wildlife Management 71(5):1419–1426.\n"
		"<a href=\"http://www.wildlifejournals.org/perlserv/?request=get-toc&issn=0022-541X&volume=71&issue=5\">pdf</a>.\n"
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

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	)->asParameters()->Assign(m_Cells.Get_Weighting().Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRuggedness_VRM::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	m_pDEM	= Parameters("DEM")	->asGrid();
	m_pVRM	= Parameters("VRM")	->asGrid();

	DataObject_Set_Colors(m_pVRM, 100, SG_COLORS_RED_GREY_BLUE, true);

	m_Cells.Get_Weighting().Set_Parameters(Parameters("WEIGHTING")->asParameters());

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asInt()) )
	{
		return( false );
	}

	m_X.Create(*Get_System(), SG_DATATYPE_Float);
	m_Y.Create(*Get_System(), SG_DATATYPE_Float);
	m_Z.Create(*Get_System(), SG_DATATYPE_Float);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			double	slope, aspect;

			if( m_pDEM->Get_Gradient(x, y, slope, aspect) )
			{
				m_Y.Set_Value(x, y, sin(slope) * sin(aspect));
				m_Y.Set_Value(x, y, sin(slope) * cos(aspect));
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
		double	iDistance, iWeight, n, sx, sy, sz;

		for(i=0, n=0, sx=0.0, sy=0.0, sz=0.0; i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, iDistance, iWeight, true) && iDistance > 0.0 && m_X.is_InGrid(ix, iy) )
			{
				sx	+= iWeight * m_X.asDouble(ix, iy);
				sy	+= iWeight * m_Y.asDouble(ix, iy);
				sz	+= iWeight * m_Z.asDouble(ix, iy);
				n	+= iWeight;
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
