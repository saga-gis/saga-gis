
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

	Set_Name(_TL("Residual Analysis (Grid)"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL("Relations of each grid cell to its neighborhood. "
		"Wilson & Gallant (2000) used this type of calculation in terrain analysis.\n"
		"\n"
		"Reference:\n"
		"- Wilson, J.P., Gallant, J.C., (Eds.), 2000: "
		"'Terrain analysis - principles and applications', "
		"New York, John Wiley & Sons, Inc.\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(	NULL, "INPUT"		, _TL("Grid")						, "", PARAMETER_INPUT);

	Parameters.Add_Grid(	NULL, "MEAN"		, _TL("Mean Value")					, "", PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "DIFF"		, _TL("Difference from Mean Value")	, "", PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "STDDEV"		, _TL("Standard Deviation")			, "", PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "RANGE"		, _TL("Value Range")				, "", PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "DEVMEAN"		, _TL("Deviation from Mean Value")	, "", PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, "PERCENTILE"	, _TL("Percentile")					, "", PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "RADIUS"		, _TL("Radius (Cells)")				, "", PARAMETER_TYPE_Int, 7, 1, true);
}

//---------------------------------------------------------
CGSGrid_Residuals::~CGSGrid_Residuals(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Residuals::On_Execute(void)
{
	int		x, y;
	CColors	Colors;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT")		->asGrid();

	pMean		= Parameters("MEAN")		->asGrid();
	pDiff		= Parameters("DIFF")		->asGrid();
	pStdDev		= Parameters("STDDEV")		->asGrid();
	pRange		= Parameters("RANGE")		->asGrid();
	pDevMean	= Parameters("DEVMEAN")		->asGrid();
	pPercentile	= Parameters("PERCENTILE")	->asGrid();

	Colors.Set_Palette(COLORS_PALETTE_RED_GREY_BLUE, true);

	DataObject_Set_Colors(pDiff			, Colors);
	DataObject_Set_Colors(pStdDev		, Colors);
	DataObject_Set_Colors(pRange		, Colors);
	DataObject_Set_Colors(pDevMean		, Colors);
	DataObject_Set_Colors(pPercentile	, Colors);

	//-----------------------------------------------------
	pRadius		= new CMAT_Grid_Radius(Parameters("RADIUS")->asInt() + 1);

	for(y=0, x=1; y<pRadius->Get_Maximum(); y++)
	{
		x	+= pRadius->Get_nPoints(y);
	}

	Values		= (double *)malloc(x * sizeof(double));

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Get_Value(x, y);
		}
	}

	//-----------------------------------------------------
	delete(pRadius);

	free(Values);

	return( true );
}

//---------------------------------------------------------
int CGSGrid_Residuals::Get_Value(int x, int y)
{
	int				iRadius, iPoint, ix, iy, nPoints, nLower;
	double			Value, zValue, zMin, zMax, Mean, StdDev;

	//-----------------------------------------------------
	nPoints	= 0;

	if( pInput->is_InGrid(x, y) )
	{
		nLower	= 0;
		Mean	= 0.0;
		zValue	= pInput->asDouble(x, y);

		for(iRadius=0; iRadius<pRadius->Get_Maximum(); iRadius++)
		{
			for(iPoint=0; iPoint<pRadius->Get_nPoints(iRadius); iPoint++)
			{
				pRadius->Get_Point(iRadius, iPoint, ix, iy);

				ix	+= x;
				iy	+= y;

				if( pInput->is_InGrid(ix, iy) )
				{
					Mean	+= (Values[nPoints++]	= Value	= pInput->asDouble(ix, iy));

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
		}
	}

	//-----------------------------------------------------
	if( nPoints <= 1 )
	{
		pMean		->Set_NoData(x, y);
		pDiff		->Set_NoData(x, y);
		pStdDev		->Set_NoData(x, y);
		pRange		->Set_NoData(x, y);
		pDevMean	->Set_NoData(x, y);
		pPercentile	->Set_NoData(x, y);
	}
	else
	{
		Mean		/= (double)nPoints;

		StdDev		= 0.0;

		for(iPoint=0; iPoint<nPoints; iPoint++)
		{
			Value	= Values[iPoint] - Mean;

			StdDev	+= Value * Value;
		}

		StdDev		/= (double)(nPoints - 1.0);

		pMean		->Set_Value(x, y, Mean);
		pDiff		->Set_Value(x, y, zValue - Mean);
		pStdDev		->Set_Value(x, y, StdDev);
		pRange		->Set_Value(x, y, zMax - zMin);
		pDevMean	->Set_Value(x, y, (zValue - Mean) / StdDev);
		pPercentile	->Set_Value(x, y, 100.0 * (double)nLower / (double)(nPoints - 1.0));
	}

	return( nPoints );
}
