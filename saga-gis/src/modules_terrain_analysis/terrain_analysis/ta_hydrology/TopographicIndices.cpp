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
	CSG_Parameter	*pNode, *pSNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Topographic Indices"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Calculation of slope and catchment area based topographic indices "
		"Topograhic Wetness Index (TWI), USLE LS factor, Stream Power Index.\n"
		"\n"
		"References:\n"
		"\n"
		"Beven, K.J., Kirkby, M.J. (1979):\n"
		"A physically-based variable contributing area model of basin hydrology'\n"
		"Hydrology Science Bulletin 24(1), p.43-69\n"
		"\n"
		"Boehner, J., Selige, T. (2006):\n"
		"Spatial Prediction of Soil Attributes Using Terrain Analysis and Climate Regionalisation'\n"
		"In: Boehner, J., McCloy, K.R., Strobl, J.: 'SAGA – Analysis and Modelling Applications', "
		"Goettinger Geographische Abhandlungen, Vol.115, p.13-27\n"
		"\n"
		"Desmet & Govers (1996):\n"
		"'A GIS Procedure for Automatically Calculating the USLE LS Factor on Topographically Complex Landscape Units'\n"
		"Journal of Soil and Water Conservation, 51(5):427.433\n"
		"\n"
		"Kinnell, P.I.A. (2005):\n"
		"'Alternative Approaches for Determining the USLE-M Slope Length Factor for Grid Cells.'\n"
		"<a href=\"http://soil.scijournals.org/cgi/content/full/69/3/674\">http://soil.scijournals.org/cgi/content/full/69/3/674</a>\n"
		"\n"
		"Moore, I.D., Grayson, R.B., Ladson, A.R. (1991):\n"
		"'Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications'\n"
		"Hydrological Processes, Vol.5, No.1\n"
		"\n"
		"Wischmeier, W.H., Smith, D.D. (1978):\n"
		"'Predicting rainfall erosion losses – A guide to conservation planning'\n"
		"Agriculture Handbook No. 537: US Department of Agriculture, Washington DC.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "AREA"		, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "TRANSMISS"	, _TL("Transmissivity"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "WETNESS"		, _TL("Topographic Wetness Index"),
		_TL("The Topographic Wetness Index: ln( Area / tan(Slope) )"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STREAMPOW"	, _TL("Stream Power Index"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "LSFACTOR"	, _TL("LS-Factor"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	/*/-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "TWI", _TL("Topgraphic Wetness Index"), _TL(""));

	Parameters.Add_Choice(
		pNode	, "TWI_METHOD"	, _TL("Method (TWI)"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Standard"),
			_TL("TOPMODEL")
		), 0
	);/**/

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "LS", _TL("LS-Factor"), _TL(""));

	Parameters.Add_Choice(
		pNode	, "LS_METHOD"		, _TL("Method (LS)"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Moore et al. 1991"),
			_TL("Desmet & Govers 1996"),
			_TL("Boehner & Selige 2006")
		), 0
	);

	Parameters.Add_Choice(
		pNode	, "LS_AREA"			, _TL("Area to Length Conversion"),
		_TL("Derivation of slope lengths from catchment areas. These are rough approximations! Applies not to Desmet & Govers' method."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("no conversion (areas already given as specific catchment area)"),
			_TL("1 / cell size (specific catchment area)"),
			_TL("square root (catchment length)")
		), 0
	);

	pSNode	= Parameters.Add_Node(pNode, "LS_DG", _TL("Desmet & Govers"), _TL(""));

	Parameters.Add_Value(
		pSNode	, "DG_EROSIVITY"	, _TL("Rill/Interrill Erosivity"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		pSNode	, "DG_STABILITY"	, _TL("Stability"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("stable"),
			_TL("instable (thawing)")
		), 0
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
	bool		bTopmodel;
	int			x, y;
	double		Area, Slope, tan_Slope, Kf, Mean_Kf, Mean_TWI;
	CSG_Grid	*pArea, *pSlope, *pKf, *pTWI, *pSPI, *pLSF;

	//-----------------------------------------------------
	pSlope			= Parameters("SLOPE")		->asGrid();
	pArea			= Parameters("AREA")		->asGrid();
	pKf				= Parameters("TRANSMISS")	->asGrid();

	pTWI			= Parameters("WETNESS")		->asGrid();
	pSPI			= Parameters("STREAMPOW")	->asGrid();
	pLSF			= Parameters("LSFACTOR")	->asGrid();

	bTopmodel		= false; // Parameters("TWI_METHOD")	->asInt() == 1;

	m_Method_LS		= Parameters("LS_METHOD")	->asInt();
	m_DG_Erosivity	= Parameters("DG_EROSIVITY")->asInt();
	m_DG_Stability	= Parameters("DG_STABILITY")->asInt();

	m_Method_Area	= Parameters("LS_AREA")		->asInt();

	//-----------------------------------------------------
	Kf		= 1.0;

	if( pTWI )
	{
		DataObject_Set_Colors(pTWI, 100, SG_COLORS_RED_GREY_BLUE);
	}

	if( pSPI )
	{
		DataObject_Set_Colors(pSPI, 100, SG_COLORS_RED_GREY_GREEN, true);
	}

	if( pLSF )
	{
		DataObject_Set_Colors(pLSF, 100, SG_COLORS_RED_GREY_GREEN, true);
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
			if( pArea->is_NoData(x, y) || pSlope->is_NoData(x, y) || (pKf && pKf->is_NoData(x, y)) )
			{
				if( pLSF )	pLSF->Set_NoData(x, y);
				if( pSPI )	pSPI->Set_NoData(x, y);
				if( pTWI )	pTWI->Set_NoData(x, y);
			}
			else
			{
				//-----------------------------------------
				Area		= _Get_Area(pArea->asDouble(x, y));	// specific catchment area...
				Slope		= pSlope->asDouble(x, y);			// slope as radians...
				tan_Slope	= tan(Slope);
				if( tan_Slope < M_ALMOST_ZERO )	tan_Slope	= M_ALMOST_ZERO;

				if( pKf )
				{
					Kf		= pKf->asDouble(x, y);
					if( Kf < M_ALMOST_ZERO )	Kf	= M_ALMOST_ZERO;
				}

				//-----------------------------------------
				if( pLSF )
				{
					pLSF->Set_Value(x, y,
						_Get_LS(Slope, pArea->asDouble(x, y))
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

//---------------------------------------------------------
inline double CTopographicIndices::_Get_Area(double Area)
{
	switch( m_Method_Area )
	{
	default:	return( Area );						// no conversion...
	case 1:		return( Area / Get_Cellsize() );	// pseudo specific catchment area...
	case 2:		return( sqrt(Area) );				// pseudo slope length...
	}
}

//---------------------------------------------------------
double CTopographicIndices::_Get_LS(double Slope, double Area)
{
	double	LS, sinSlope;

	switch( m_Method_LS )
	{
	//-----------------------------------------------------
	case 0:	default:
		{
			LS			= (0.4 + 1) * pow(_Get_Area(Area) / 22.13, 0.4) * pow(sin(Slope) / 0.0896, 1.3);
		}
		break;

	//-----------------------------------------------------
	case 1:
		{
			double	L, S, m, x, d;

			sinSlope	= sin(Slope);

			d			= Get_Cellsize();

			m			= m_DG_Erosivity * (sinSlope / 0.0896) / (3.0 * pow(sinSlope, 0.8) + 0.56);
			m			= m / (1.0 + m);

		//	x			= Aspect < 0.0 ? 0.0 : fmod(Aspect, M_PI_090);
		//	x			= sin(x) + cos(x);
			x			= 1.0;

			// x : coefficient that adjusts for width of flow at the center of the cell.
			//     It has a value of 1.0 when the flow is toward a side and sqrt(2.0) when
			//     the flow is toward a corner.
			//     (Peter I. A. Kinnell: 'ALTERNATIVE APPROACHES FOR DETERMINING THE
			//     USLE-M SLOPE LENGTH FACTOR FOR GRID CELLS'
			//     http://soil.scijournals.org/cgi/content/full/69/3/674)

			L			= (pow(Area + d*d, m + 1.0) - pow(Area, m + 1.0))
						/ (pow(d, m + 2.0) * pow(22.13, m) * pow(x, m));

			//-----------------------------------------------------
			if( Slope < 0.08975817419 )		// < 9% Steigung := atan(0.09), ca. 5 Degree
			{
				S		= 10.8 * sinSlope + 0.03;	
			}
			else if( m_DG_Stability == 0 )	// >= 9% Steigung, stabil
			{
				S		= 16.8 * sinSlope - 0.5;
			}
			else							// >= 9% Steigung, tauend u. instabil
			{
				S		= pow(sinSlope / 0.896, 0.6);
			}

			LS			= L * S;
		}
		break;

	//-----------------------------------------------------
	case 2:
		{
			sinSlope	= sin(Slope);

			if( Slope > 0.0505 )	// >  ca. 3 Degree
			{
				LS		= sqrt(_Get_Area(Area) / 22.13)
						* (65.41 * sinSlope * sinSlope + 4.56 * sinSlope + 0.065);
			}
			else					// <= ca. 3 Degree
			{
				LS		= pow (_Get_Area(Area) / 22.13, 3.0 * pow(Slope, 0.6))
						* (65.41 * sinSlope * sinSlope + 4.56 * sinSlope + 0.065);
			}
		}
		break;
	}

	return( LS );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
