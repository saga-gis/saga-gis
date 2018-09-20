/**********************************************************
 * Version $Id: anisotropic_heating.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                 Anisotropic_Heating.cpp               //
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
//    e-mail:     conrad@geowiss.uni-hamburg.de          //
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
#include "anisotropic_heating.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAnisotropic_Heating::CAnisotropic_Heating(void)
{
	Set_Name		(_TL("Diurnal Anisotropic Heat"));

	Set_Author		("J.Boehner, O.Conrad (c) 2008");

	Set_Description	(_TW(
		"This tool calculates a rather simple approximation of the "
		"anisotropic diurnal heat (Ha) distribution using the formula:\n"
		"<b>Ha = cos(amax - a) * arctan(b)</b>\n"
		"where amax defines the aspect with the maximum total heat surplus, "
		"a is the slope aspect and b is the slope angle. For more details "
		"see Boehner & Antonic (2009)."
	));

	Add_Reference("Boehner, J., & Antonic, O.", "2009",
		"Land-surface parameters specific to topo-climatology",
		"Developments in soil science, 33, 195-226.",
		SG_T("https://www.sciencedirect.com/bookseries/developments-in-soil-science/vol/33/suppl/C"), SG_T("ScienceDirect")
	);

	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "DAH"		, _TL("Diurnal Anisotropic Heating"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		"", "ALPHA_MAX"	, _TL("Alpha Max (Degree)"),
		_TL(""),
		202.5, 0.0, true, 360.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAnisotropic_Heating::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();
	CSG_Grid	*pDAH	= Parameters("DAH")->asGrid();

	double	alpha_max	= Parameters("ALPHA_MAX")->asDouble() * M_DEG_TO_RAD;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	alpha, slope;

			if( pDEM->is_NoData(x, y) || !pDEM->Get_Gradient(x, y, slope, alpha) )
			{
				pDAH->Set_NoData(x, y);
			}
			else
			{
				pDAH->Set_Value(x, y, cos(alpha_max - alpha) * atan(slope));
			}
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
