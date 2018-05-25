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
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        evi.cpp                        //
//                                                       //
//                 Copyright (C) 2011 by                 //
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


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "evi.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CEnhanced_VI::CEnhanced_VI(void)
{
	Set_Name		(_TL("Enhanced Vegetation Index"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Enhanced Vegetation Index (EVI)."
	));

	Add_Reference(
		"Huete, A., Didan, K., Miura, T., Rodriguez, E. P., Gao, X., & Ferreira, L. G.", "2002",
		"Overview of the radiometric and biophysical performance of the MODIS vegetation indices",
		"Remote sensing of environment, 83(1-2), 195-213, 10.1016/S0034-4257(02)00096-2.",
		SG_T("http://www.sciencedirect.com/science/article/pii/S0034425702000962")
	);

	Parameters.Add_Grid(
		"", "BLUE"	, _TL("Blue Reflectance"),
		_TL(""), 
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "RED"	, _TL("Red Reflectance"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "NIR"	, _TL("Near Infrared Reflectance"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "EVI"	, _TL("Enhanced Vegetation Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		"", "GAIN"	, _TL("Gain"), 
		_TL(""),
		2.5, 0.0, true
	);

	Parameters.Add_Double(
		"", "L"		, _TL("Canopy Background Adjustment"), 
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "CBLUE"	, _TL("Aerosol Resistance Coefficient (Blue)"), 
		_TL(""),
		7.5, 0.0, true
	);

	Parameters.Add_Double(
		"", "CRED"	, _TL("Aerosol Resistance Coefficient (Red)"), 
		_TL(""),
		6.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CEnhanced_VI::On_Execute(void)
{
	CSG_Grid	*pBlue	= Parameters("BLUE" )->asGrid();
	CSG_Grid	*pRed	= Parameters("RED"  )->asGrid();
	CSG_Grid	*pNIR	= Parameters("NIR"  )->asGrid();

	CSG_Grid	*pEVI	= Parameters("EVI"  )->asGrid();

	double		Gain	= Parameters("GAIN" )->asDouble();
	double		L		= Parameters("L"    )->asDouble();
	double		CBlue	= Parameters("CBLUE")->asDouble();
	double		CRed	= Parameters("CRED" )->asDouble();

	DataObject_Set_Colors(pEVI, 11, SG_COLORS_RED_GREY_GREEN, false);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	d;

			if( (pBlue && pBlue->is_NoData(x, y)) || pRed->is_NoData(x, y) || pNIR->is_NoData(x, y) )
			{
				d	= 0.0;
			}
			else if( pBlue )
			{
				d	= L + pNIR->asDouble(x, y) + CRed * pRed->asDouble(x, y) + CBlue * pBlue->asDouble(x, y);
			}
			else
			{
				d	= L + pNIR->asDouble(x, y) + CRed * pRed->asDouble(x, y);
			}

			if( d )
			{
				pEVI->Set_Value(x, y, Gain * (pNIR->asDouble(x, y) - pRed->asDouble(x, y)) / d);
			}
			else
			{
				pEVI->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
