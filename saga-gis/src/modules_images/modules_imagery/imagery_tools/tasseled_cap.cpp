/**********************************************************
 * Version $Id: evi.cpp 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
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

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Tasseled Cap Transformation as proposed for Landsat Thematic Mapper.\n"
		"\n"
		"References:\n"
		"Kauth R. J. und G. S. Thomas (1976): "
		"The Tasseled Cap - A Graphic Description of the Spectral-Temporal Development of Agricultural Crops as Seen by LANDSAT. "
		"Proceedings of the Symposium on Machine Processing of Remotely Sensed Data. "
		"<a target=\"_blank\" href=\"http://docs.lib.purdue.edu/cgi/viewcontent.cgi?article=1160&context=lars_symp&sei-redir=1&referer=http%3A%2F%2Fwww.google.de%2Furl%3Fsa%3Dt%26rct%3Dj%26q%3Dthe%2520tasseled%2520cap%2520--%2520a%2520graphic%2520description%2520of%2520the%2520spectral-temporal%2520development%2520of%2520agricultural%2520crops%26source%3Dweb%26cd%3D1%26ved%3D0CCEQFjAA%26url%3Dhttp%253A%252F%252Fdocs.lib.purdue.edu%252Fcgi%252Fviewcontent.cgi%253Farticle%253D1160%2526context%253Dlars_symp%26ei%3D1-jcTvq2NpGPsAb4tK2ODA%26usg%3DAFQjCNFLCISdiKdt2njGl6Dj1FC4Bac0ag#search=%22tasseled%20cap%20--%20graphic%20description%20spectral-temporal%20development%20agricultural%20crops%22\">online at Purdue University</a>\n"
		"\n"
		"Huang, C., B. Wylie, L. Yang, C. Homer, and G. Zylstra. "
		"Derivation of a Tasseled Cap Transformation Based on Landsat 7 At-Satellite Reflectance. "
		"USGS EROS Data Center White Paper. "
		"<a target=\"_blank\" href=\"http://landcover.usgs.gov/pdf/tasseled.pdf\">online at USGS</a>\n"
	));

	Parameters.Add_Grid(
		NULL, "BLUE"		, _TL("Blue (TM 1)"),
		_TL(""), 
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "RED"			, _TL("Red (TM 2)"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "GREEN"		, _TL("Green (TM 3)"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "NIR"			, _TL("Near Infrared (TM 4)"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "MIR1"		, _TL("Mid Infrared (TM 5)"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "MIR2"		, _TL("Mid Infrared (TM 7)"), 
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "BRIGHTNESS"	, _TL("Brightness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "GREENNESS"	, _TL("Greenness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "WETNESS"		, _TL("Wetness"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
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

	DataObject_Set_Colors(pBright, 100, SG_COLORS_BLACK_WHITE  , false);
	DataObject_Set_Colors(pGreen , 100, SG_COLORS_WHITE_GREEN  , false);
	DataObject_Set_Colors(pWet   , 100, SG_COLORS_RED_GREY_BLUE, false);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
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

			    pBright	->Set_Value(x, y,
					 0.3037 * b[0] + 0.2793 * b[1] + 0.4743 * b[2] + 0.5585 * b[3] + 0.5082 * b[4] + 0.1863 * b[5]
				);

				pGreen	->Set_Value(x, y,
					-0.2848 * b[0] - 0.2435 * b[1] - 0.5436 * b[2] + 0.7243 * b[3] + 0.0840 * b[4] - 0.1800 * b[5]
				);

				pWet	->Set_Value(x, y,
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
