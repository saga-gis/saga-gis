
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

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW("Relations of each grid cell to its neighborhood. "
		"Wilson & Gallant (2000) used this type of calculation in terrain analysis.\n"
		"\n"
		"Reference:\n"
		"- Wilson, J.P., Gallant, J.C., (Eds.), 2000: "
		"'Terrain analysis - principles and applications', "
		"New York, John Wiley & Sons, Inc.\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(	NULL, "INPUT"		, _TL("Grid")						, _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid(	NULL, "MEAN"		, _TL("Mean Value")					, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "DIFF"		, _TL("Difference from Mean Value")	, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "STDDEV"		, _TL("Standard Deviation")			, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "RANGE"		, _TL("Value Range")				, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "MIN"			, _TL("Minimum Value")				, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "MAX"			, _TL("Maximum Value")				, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "DEVMEAN"		, _TL("Deviation from Mean Value")	, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "PERCENTILE"	, _TL("Percentile")					, _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "RADIUS"		, _TL("Radius (Cells)")				, _TL(""), PARAMETER_TYPE_Int, 7, 1, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Residuals::On_Execute(void)
{
	m_pInput		= Parameters("INPUT")		->asGrid();
	m_pMean			= Parameters("MEAN")		->asGrid();
	m_pDiff			= Parameters("DIFF")		->asGrid();
	m_pStdDev		= Parameters("STDDEV")		->asGrid();
	m_pRange		= Parameters("RANGE")		->asGrid();
	m_pMin			= Parameters("MIN")			->asGrid();
	m_pMax			= Parameters("MAX")			->asGrid();
	m_pDevMean		= Parameters("DEVMEAN")		->asGrid();
	m_pPercentile	= Parameters("PERCENTILE")	->asGrid();

	DataObject_Set_Colors(m_pDiff		, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pStdDev		, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pRange		, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pMin		, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pMax		, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pDevMean	, 100, SG_COLORS_RED_GREY_BLUE, true);
	DataObject_Set_Colors(m_pPercentile	, 100, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	m_Radius.Create(Parameters("RADIUS")->asInt());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Value(x, y);
		}
	}

	//-----------------------------------------------------
	m_Radius.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Residuals::Get_Value(int x, int y)
{
	//-----------------------------------------------------
	if( m_pInput->is_InGrid(x, y) )
	{
		int		ix, iy, nPoints, nLower;
		double	Value, zValue, zMin, zMax, Mean, StdDev;

		nPoints	= 0;
		nLower	= 0;
		Mean	= 0.0;
		StdDev	= 0.0;
		zValue	= m_pInput->asDouble(x, y);

		//-------------------------------------------------
		for(int iPoint=0; iPoint<m_Radius.Get_nPoints(); iPoint++)
		{
			m_Radius.Get_Point(iPoint, x, y, ix, iy);

			if( m_pInput->is_InGrid(ix, iy) )
			{
				Value	 = m_pInput->asDouble(ix, iy);
				Mean	+= Value;
				StdDev	+= Value * Value;
				nPoints	++;

				if( nPoints <= 1 )
				{
					zMin	= zMax	= Value;
				}
				else if( zMin > Value )
				{
					zMin	= Value;
				}
				else if( zMax < Value )
				{
					zMax	= Value;
				}

				if( Value < zValue )
				{
					nLower++;
				}
			}
		}

		//-------------------------------------------------
		if( nPoints > 1 )
		{
			Mean	= Mean / nPoints;
			StdDev	= sqrt(StdDev / (nPoints - 1.0) - Mean * Mean);

			m_pMean			->Set_Value(x, y, Mean);
			m_pDiff			->Set_Value(x, y, zValue - Mean);
			m_pStdDev		->Set_Value(x, y, StdDev);
			m_pRange		->Set_Value(x, y, zMax - zMin);
			m_pMin			->Set_Value(x, y, zMin);
			m_pMax			->Set_Value(x, y, zMax);
			m_pDevMean		->Set_Value(x, y, (zValue - Mean) / StdDev);
			m_pPercentile	->Set_Value(x, y, 100.0 * (double)nLower / (double)(nPoints));

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pMean			->Set_NoData(x, y);
	m_pDiff			->Set_NoData(x, y);
	m_pStdDev		->Set_NoData(x, y);
	m_pRange		->Set_NoData(x, y);
	m_pMin			->Set_NoData(x, y);
	m_pMax			->Set_NoData(x, y);
	m_pDevMean		->Set_NoData(x, y);
	m_pPercentile	->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
