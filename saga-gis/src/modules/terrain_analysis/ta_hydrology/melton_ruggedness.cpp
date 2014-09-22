/**********************************************************
 * Version $Id: melton_ruggedness.cpp 1091 2011-06-16 15:50:52Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 melton_ruggedness.cpp                 //
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
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "melton_ruggedness.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMelton_Ruggedness::CMelton_Ruggedness(void)
{
	Set_Name		(_TL("Melton Ruggedness Number"));

	Set_Author		(SG_T("O. Conrad (c) 2012"));

	Set_Description	(_TW(
		"Melton ruggedness number (MNR) is a simple flow accumulation related index, "
		"calculated as difference between maximum and minimum elevation "
		"in catchment area divided by square root of catchment area size. "
		"The calculation is performed for each grid cell, therefore minimum elevation "
		"is same as elevation at cell's position. "
		"Due to the discrete character of a single maximum elevation, flow calculation "
		"is simply done with Deterministic 8. "
		"\n\n"
		"References:\n"
		"Marchi, L. &  Fontana, G.D. (2005): "
		"GIS morphometric indicators for the analysis of sediment dynamics in mountain basins. "
		"Environ. Geol. 48:218-228, DOI 10.1007/s00254-005-1292-4.\n"
		"\n"
		"Melton M.A. (1965): "
		"The geomorphic and paleoclimatic significance of alluvial deposits in Southern Arizona. "
		"J. Geol. 73:1-38.\n"
		"\n"
		"O'Callaghan, J.F. / Mark, D.M. (1984): "
		"The extraction of drainage networks from digital elevation data. "
		"Computer Vision, Graphics and Image Processing 28:323-344.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "AREA"	, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "ZMAX"	, _TL("Maximum Height"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "MRN"		, _TL("Melton Ruggedness Number"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMelton_Ruggedness::On_Execute(void)
{
	CSG_Grid	*pDEM, *pArea, *pMRN, *pZMax;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM" )->asGrid();
	pArea		= Parameters("AREA")->asGrid();
	pZMax		= Parameters("ZMAX")->asGrid();
	pMRN		= Parameters("MRN" )->asGrid();

	if( !pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	pArea->Set_NoData_Value(0.0);
	pArea->Assign_NoData();
	pZMax->Assign_NoData();
	pMRN ->Assign_NoData();

	//-------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y, i, ix, iy;

		if( pDEM->Get_Sorted(n, x, y, true, true) )
		{
			pArea->Add_Value(x, y, Get_Cellsize());

			if( pZMax->is_NoData(x, y) )
			{
				pZMax->Set_Value(x, y, pDEM->asDouble(x, y));
			}

			if( (i = pDEM->Get_Gradient_NeighborDir(x, y, true)) >= 0 && Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy) )
			{
				pArea->Add_Value(ix, iy, pArea->asDouble(x, y));

				if( pZMax->is_NoData(ix, iy) || pZMax->asDouble(ix, iy) < pZMax->asDouble(x, y) )
				{
					pZMax->Set_Value(ix, iy, pZMax->asDouble(x, y));
				}
			}

			pMRN->Set_Value(x, y, (pZMax->asDouble(x, y) - pDEM->asDouble(x, y)) / sqrt(pArea->asDouble(x, y)));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
