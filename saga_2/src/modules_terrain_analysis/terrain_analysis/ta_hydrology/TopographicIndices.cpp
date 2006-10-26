
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
//                TopographicIndices.cpp                 //
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
#include "TopographicIndices.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTopographicIndices::CTopographicIndices(void)
{
	Set_Name(_TL("Topographic Indices"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Calculation of slope and catchment area based topographic indices (wetness, LS-factor, stream power).\n\n"
		"References:\n"
		"Moore, I.D., Grayson, R.B., Ladson, A.R. (1991): "
		"'Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications'\n"
		"Hydrological Processes, Vol.5, No.1\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "SLOPE"		, _TL("Slope"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "AREA"		, _TL("Catchment Area"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "TRANSMISS"	, _TL("Transmissivity"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "WETNESS"		, _TL("Topographic Wetness Index"),
		_TL("The Topographic Wetness Index: ln( A / tan(ß) )"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "STREAMPOW"	, _TL("Stream Power Index"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "LSFACTOR"	, _TL("LS-Factor"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
CTopographicIndices::~CTopographicIndices(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographicIndices::On_Execute(void)
{
	bool	bTopmodel	= false;
	int		x, y;
	double	Area, Slope, tan_Slope, Kf, Mean_Kf, Mean_TWI;
	CSG_Colors	Colors;
	CSG_Grid	*pArea, *pSlope, *pKf, *pTWI, *pSPI, *pLSF;

	//-----------------------------------------------------
	pSlope	= Parameters("SLOPE")		->asGrid();
	pArea	= Parameters("AREA")		->asGrid();
	pKf		= Parameters("TRANSMISS")	->asGrid();

	pTWI	= Parameters("WETNESS")		->asGrid();
	pSPI	= Parameters("STREAMPOW")	->asGrid();
	pLSF	= Parameters("LSFACTOR")	->asGrid();

	Kf		= 1.0;

	if( pTWI )
	{
		Colors.Set_Palette(SG_COLORS_RED_GREY_BLUE, false);
		DataObject_Set_Colors(pTWI, Colors);
	}

	if( pSPI )
	{
		Colors.Set_Palette(SG_COLORS_RED_GREY_GREEN, true);
		DataObject_Set_Colors(pSPI, Colors);
	}

	if( pLSF )
	{
		Colors.Set_Palette(SG_COLORS_RED_GREY_GREEN, true);
		DataObject_Set_Colors(pLSF, Colors);
	}

	//-----------------------------------------------------
	if( bTopmodel )
	{
		Mean_TWI	= 0.0;
		Mean_Kf		= 0.0;
		Area		= 0.0;

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !pArea->is_NoData(x, y) && !pSlope->is_NoData(x, y) )
				{
					if( pKf )
					{
						if( !pKf->is_NoData(x, y) )
						{
							Kf	= pKf->asDouble(x, y);
							if( Kf < M_ALMOST_ZERO )	Kf	= M_ALMOST_ZERO;
							Kf	= log(Kf);
						}
						else
						{
							Kf	= 0.0;
						}
					}

					tan_Slope	= tan(pSlope->asDouble(x, y));
					if( tan_Slope < M_ALMOST_ZERO )	tan_Slope	= M_ALMOST_ZERO;

					Mean_TWI	+= log(pArea->asDouble(x, y) / tan_Slope);
					Mean_Kf		+= Kf;
					Area++;
				}
			}
		}

		if( Area > 0.0 )
		{
			Area		*= pArea->Get_Cellarea();
			Mean_TWI	/= Area;
			Mean_Kf		/= Area;
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pArea->is_NoData(x, y) || pSlope->is_NoData(x, y) )
			{
				if( pLSF )	pLSF->Set_NoData(x, y);
				if( pSPI )	pSPI->Set_NoData(x, y);
				if( pTWI )	pTWI->Set_NoData(x, y);
			}
			else
			{
				//-----------------------------------------
				Area		= pArea	->asDouble(x, y) / pArea->Get_Cellsize();	// pseudo specific catchment area...
				Slope		= pSlope->asDouble(x, y);							// slope as radians...
				tan_Slope	= tan(Slope);
				if( tan_Slope < M_ALMOST_ZERO )	tan_Slope	= M_ALMOST_ZERO;

				if( pKf )
				{
					Kf			= pKf->asDouble(x, y);
					if( Kf < M_ALMOST_ZERO )	Kf	= M_ALMOST_ZERO;
				}

				//-----------------------------------------
				if( pLSF )
				{
					pLSF->Set_Value(x, y,
						(0.4 + 1) * pow(Area / 22.13, 0.4) * pow(sin(Slope) / 0.0896, 1.3)
					);
				}

				if( pSPI )
				{
					pSPI->Set_Value(x, y,
						Area * tan_Slope
					);
				}

				if( pTWI )
				{
					pTWI->Set_Value(x, y, bTopmodel
						? (log(Area / (     tan_Slope)) - Mean_TWI) - (log(Kf) - Mean_Kf)
						:  log(Area / (Kf * tan_Slope))
					);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
