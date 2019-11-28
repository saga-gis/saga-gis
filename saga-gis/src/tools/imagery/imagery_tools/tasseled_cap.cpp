
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
//                   tasseled_cap.cpp                    //
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
#include "tasseled_cap.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTasseled_Cap::CTasseled_Cap(void)
{
	Set_Name		(_TL("Tasseled Cap Transformation"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Tasseled Cap Transformation as proposed for Landsat Thematic Mapper."
	));

	Add_Reference(
		"Kauth, R.J., Thomas, G.S.", "1976",
		"The Tasseled Cap - A Graphic Description of the Spectral-Temporal Development of Agricultural Crops as Seen by LANDSAT",
		"Proceedings of the Symposium on Machine Processing of Remotely Sensed Data.",
		SG_T("http://docs.lib.purdue.edu/cgi/viewcontent.cgi?article=1160&context=lars_symp")
	);

	Add_Reference(
		"Huang, C., Wylie, B., Yang, L., Homer, C., Zylstra, G.", "2002",
		"Derivation of a tasselled cap transformation based on Landsat 7 at-satellite reflectance",
		"International Journal of Remote Sensing, 23(8), 1741-1748.",
		SG_T("http://www.tandfonline.com/doi/pdf/10.1080/01431160110106113")
	);

	Parameters.Add_Grid("", "BLUE"      , _TL("Blue (TM 1)"         ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "GREEN"     , _TL("Green (TM 2)"        ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "RED"       , _TL("Red (TM 3)"          ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "NIR"       , _TL("Near Infrared (TM 4)"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "MIR1"      , _TL("Mid Infrared (TM 5)" ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "MIR2"      , _TL("Mid Infrared (TM 7)" ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid("", "BRIGHTNESS", _TL("Brightness"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "GREENNESS" , _TL("Greenness"           ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "WETNESS"   , _TL("Wetness"             ), _TL(""), PARAMETER_OUTPUT);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTasseled_Cap::On_Execute(void)
{
	CSG_Grid	*pBand[6], *pBright, *pGreen, *pWet;

	pBand[0]	= Parameters("BLUE"      )->asGrid();
	pBand[1]	= Parameters("GREEN"     )->asGrid();
	pBand[2]	= Parameters("RED"       )->asGrid();
	pBand[3]	= Parameters("NIR"       )->asGrid();
	pBand[4]	= Parameters("MIR1"      )->asGrid();
	pBand[5]	= Parameters("MIR2"      )->asGrid();

	pBright		= Parameters("BRIGHTNESS")->asGrid();
	pGreen		= Parameters("GREENNESS" )->asGrid();
	pWet		= Parameters("WETNESS"   )->asGrid();

	DataObject_Set_Colors(pBright, 11, SG_COLORS_BLACK_WHITE   , false);
	DataObject_Set_Colors(pGreen , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pWet   , 11, SG_COLORS_RED_GREY_BLUE , false);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if(	pBand[0]->is_NoData(x, y)
			||	pBand[1]->is_NoData(x, y)
			||	pBand[2]->is_NoData(x, y)
			||	pBand[3]->is_NoData(x, y)
			||	pBand[4]->is_NoData(x, y)
			||	pBand[5]->is_NoData(x, y)
			)
			{
				pBright	->Set_NoData(x, y);
				pGreen	->Set_NoData(x, y);
				pWet	->Set_NoData(x, y);
			}
			else
			{
				double	b[6];

				b[0]	= pBand[0]->asDouble(x, y);
				b[1]	= pBand[1]->asDouble(x, y);
				b[2]	= pBand[2]->asDouble(x, y);
				b[3]	= pBand[3]->asDouble(x, y);
				b[4]	= pBand[4]->asDouble(x, y);
				b[5]	= pBand[5]->asDouble(x, y);

			    pBright->Set_Value(x, y,
					 0.3037 * b[0] + 0.2793 * b[1] + 0.4743 * b[2] + 0.5585 * b[3] + 0.5082 * b[4] + 0.1863 * b[5]
				);

				pGreen ->Set_Value(x, y,
					-0.2848 * b[0] - 0.2435 * b[1] - 0.5436 * b[2] + 0.7243 * b[3] + 0.0840 * b[4] - 0.1800 * b[5]
				);

				pWet   ->Set_Value(x, y,
					 0.1509 * b[0] + 0.1973 * b[1] + 0.3279 * b[2] + 0.3406 * b[3] - 0.7112 * b[4] - 0.4572 * b[5]
				);
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
