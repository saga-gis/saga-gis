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
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  GSGrid_Residuals.cpp                 //
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
#include "GSGrid_Residuals.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Residuals::CGSGrid_Residuals(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Residual Analysis (Grid)"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW("Relations of each grid cell to its neighborhood. "
		"Wilson & Gallant (2000) used this type of calculation in terrain analysis.\n"
		"\n"
		"Reference:\n"
		"- Wilson, J.P., Gallant, J.C., [Eds.] (2000): "
		"'Terrain analysis - principles and applications', "
		"New York, John Wiley & Sons, Inc.\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(	NULL, "GRID"	, _TL("Grid")						, _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid(	NULL, "MEAN"	, _TL("Mean Value")					, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "DIFF"	, _TL("Difference from Mean Value")	, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "STDDEV"	, _TL("Standard Deviation")			, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "RANGE"	, _TL("Value Range")				, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "MIN"		, _TL("Minimum Value")				, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "MAX"		, _TL("Maximum Value")				, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "DEVMEAN"	, _TL("Deviation from Mean Value")	, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "PERCENT"	, _TL("Percentile")					, _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "RADIUS"	, _TL("Radius (Cells)")				, _TL(""), PARAMETER_TYPE_Int, 7, 1, true);

	Parameters.Add_Parameters(
		NULL, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	)->asParameters()->Assign(m_Cells.Get_Weighting().Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Residuals::On_Execute(void)
{
	m_pGrid		= Parameters("GRID")	->asGrid();

	m_pMean		= Parameters("MEAN")	->asGrid();
	m_pDiff		= Parameters("DIFF")	->asGrid();
	m_pStdDev	= Parameters("STDDEV")	->asGrid();
	m_pRange	= Parameters("RANGE")	->asGrid();
	m_pMin		= Parameters("MIN")		->asGrid();
	m_pMax		= Parameters("MAX")		->asGrid();
	m_pDevMean	= Parameters("DEVMEAN")	->asGrid();
	m_pPercent	= Parameters("PERCENT")	->asGrid();

	DataObject_Set_Colors(m_pDiff	, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pStdDev	, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pRange	, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pMin	, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pMax	, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pDevMean, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pPercent, 100, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	m_Cells.Get_Weighting().Set_Parameters(Parameters("WEIGHTING")->asParameters());

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asInt()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Statistics(x, y);
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Residuals::Get_Statistics(int x, int y)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		int		i, ix, iy, nLower;
		double	z, iz, id, iw;

		CSG_Simple_Statistics	Statistics;

		for(i=0, nLower=0, z=m_pGrid->asDouble(x, y); i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, id, iw, true) && id > 0.0 && m_pGrid->is_InGrid(ix, iy) )
			{
				Statistics.Add_Value(iz = m_pGrid->asDouble(ix, iy), iw);

				if( z > iz )
				{
					nLower++;
				}
			}
		}

		//-------------------------------------------------
		if( Statistics.Get_Weights() > 0.0 )
		{
			m_pMean		->Set_Value(x, y, Statistics.Get_Mean());
			m_pDiff		->Set_Value(x, y, z - Statistics.Get_Mean());
			m_pStdDev	->Set_Value(x, y, Statistics.Get_StdDev());
			m_pRange	->Set_Value(x, y, Statistics.Get_Range());
			m_pMin		->Set_Value(x, y, Statistics.Get_Minimum());
			m_pMax		->Set_Value(x, y, Statistics.Get_Maximum());
			m_pDevMean	->Set_Value(x, y, Statistics.Get_StdDev() > 0.0 ? ((z - Statistics.Get_Mean()) / Statistics.Get_StdDev()) : 0.0);
			m_pPercent	->Set_Value(x, y, 100.0 * nLower / (double)Statistics.Get_Count());

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pMean		->Set_NoData(x, y);
	m_pDiff		->Set_NoData(x, y);
	m_pStdDev	->Set_NoData(x, y);
	m_pRange	->Set_NoData(x, y);
	m_pMin		->Set_NoData(x, y);
	m_pMax		->Set_NoData(x, y);
	m_pDevMean	->Set_NoData(x, y);
	m_pPercent	->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
