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

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Enhanced Vegetation Index (EVI).\n"
		"\n"
		"References:\n"
		"A Huete, K Didan, T Miura, E.P Rodriguez, X Gao, L.G Ferreira, "
		"Overview of the radiometric and biophysical performance of the MODIS vegetation indices, "
		"Remote Sensing of Environment, Volume 83, Issues 1-2, November 2002, Pages 195-213, ISSN 0034-4257, 10.1016/S0034-4257(02)00096-2. "
		"<a target=\"_blank\" href=\"http://www.sciencedirect.com/science/article/pii/S0034425702000962\">online</a>\n"
	));

	Parameters.Add_Grid(
		NULL, "BLUE"	, _TL("Blue Reflectance"),
		_TL(""), 
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "RED"		, _TL("Red Reflectance"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "NIR"		, _TL("Near Infrared Reflectance"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "EVI"		, _TL("Enhanced Vegetation Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "GAIN"	, _TL("Gain"), 
		_TL(""),
		PARAMETER_TYPE_Double, 2.5, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "L"		, _TL("Canopy Background Adjustment"), 
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "CBLUE"	, _TL("Aerosol Resistance Coefficient (Blue)"), 
		_TL(""),
		PARAMETER_TYPE_Double, 7.5, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "CRED"	, _TL("Aerosol Resistance Coefficient (Red)"), 
		_TL(""),
		PARAMETER_TYPE_Double, 6.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
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

	DataObject_Set_Colors(pEVI, 100, SG_COLORS_WHITE_GREEN, false);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
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
