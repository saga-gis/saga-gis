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
CTWI::CTWI(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Topographic Wetness Index (TWI)"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Calculation of the slope and specific catchment area (SCA) based Topographic Wetness Index (TWI)\n"
	//	"ln(SCA / tan(Slope))\n"
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
		"Moore, I.D., Grayson, R.B., Ladson, A.R. (1991):\n"
		"'Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications'\n"
		"Hydrological Processes, Vol.5, No.1\n"
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
		NULL, "TRANS"		, _TL("Transmissivity"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "TWI"			, _TL("Topographic Wetness Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "CONV"	, _TL("Area Conversion"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("no conversion (areas already given as specific catchment area)"),
			_TL("1 / cell size (pseudo specific catchment area)")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method (TWI)"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Standard"),
			_TL("TOPMODEL")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTWI::On_Execute(void)
{
	bool		bTopmodel, bConvert;
	int			x, y;
	double		Area, Slope, Kf, Mean_Kf, Mean_TWI;
	CSG_Grid	*pArea, *pSlope, *pKf, *pTWI;

	//-----------------------------------------------------
	pSlope		= Parameters("SLOPE")	->asGrid();
	pArea		= Parameters("AREA")	->asGrid();
	pKf			= Parameters("TRANS")	->asGrid();
	pTWI		= Parameters("TWI")		->asGrid();
	bConvert	= Parameters("CONV")	->asInt() == 1;
	bTopmodel	= Parameters("METHOD")	->asInt() == 1;

	DataObject_Set_Colors(pTWI, 100, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	Kf			= 1.0;

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

							if( Kf	< M_ALMOST_ZERO )
								Kf	= M_ALMOST_ZERO;

							Kf	= log(Kf);
						}
						else
						{
							Kf	= 0.0;
						}
					}

					Slope	= tan(pSlope->asDouble(x, y));

					if( Slope	< M_ALMOST_ZERO )
						Slope	= M_ALMOST_ZERO;

					Area		++;
					Mean_TWI	+= log((pArea->asDouble(x, y) / (bConvert ? Get_Cellsize() : 1.0)) / Slope);
					Mean_Kf		+= Kf;
				}
			}
		}

		if( Area > 0.0 )
		{
			Area		*= pArea->Get_Cellarea();
			Mean_TWI	/= Area;
			Mean_Kf		/= Area;
		}
		else
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pArea->is_NoData(x, y) || pSlope->is_NoData(x, y) || (pKf && pKf->is_NoData(x, y)) )
			{
				pTWI->Set_NoData(x, y);
			}
			else
			{
				Area	= pArea->asDouble(x, y);

				if( bConvert )
					Area	/= Get_Cellsize();

				Slope	= tan(pSlope->asDouble(x, y));

				if( Slope	< M_ALMOST_ZERO )
					Slope	= M_ALMOST_ZERO;

				if( pKf )
				{
					Kf	= pKf->asDouble(x, y);

					if( Kf	< M_ALMOST_ZERO )
						Kf	= M_ALMOST_ZERO;
				}

				pTWI->Set_Value(x, y, bTopmodel
					? (log(Area / (     Slope)) - Mean_TWI) - (log(Kf) - Mean_Kf)
					:  log(Area / (Kf * Slope))
				);
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
CStream_Power::CStream_Power(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Stream Power Index"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Calculation of stream power index based on slope and specific catchment area (SCA).\n"
		"SPI = SCA * tan(Slope)\n"
		"\n"
		"References:\n"
		"\n"
		"Moore, I.D., Grayson, R.B., Ladson, A.R. (1991):\n"
		"'Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications'\n"
		"Hydrological Processes, Vol.5, No.1\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "SLOPE"	, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "AREA"	, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SPI"		, _TL("Stream Power Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "CONV"	, _TL("Area Conversion"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("no conversion (areas already given as specific catchment area)"),
			_TL("1 / cell size (pseudo specific catchment area)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CStream_Power::On_Execute(void)
{
	bool		bConvert;
	CSG_Grid	*pArea, *pSlope, *pSPI;

	//-----------------------------------------------------
	pSlope		= Parameters("SLOPE")	->asGrid();
	pArea		= Parameters("AREA")	->asGrid();
	pSPI		= Parameters("SPI")		->asGrid();
	bConvert	= Parameters("CONV")	->asInt() == 1;

	DataObject_Set_Colors(pSPI, 100, SG_COLORS_RED_GREY_GREEN, true);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pArea->is_NoData(x, y) || pSlope->is_NoData(x, y) )
			{
				pSPI->Set_NoData(x, y);
			}
			else
			{
				pSPI->Set_Value(x, y, (pArea->asDouble(x, y) / (bConvert ? Get_Cellsize() : 1.0)) * tan(pSlope->asDouble(x, y)));
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
CLS_Factor::CLS_Factor(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("LS Factor"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Calculation of slope length (LS) factor as used by the Universal Soil Loss Equation (USLE), "
		"based on slope and specific catchment area (SCA, as substitute for slope length). "
		"\n"
		"References:\n"
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
		NULL	, "SLOPE"	, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "AREA"	, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "LS"		, _TL("LS Factor"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "CONV"			, _TL("Area to Length Conversion"),
		_TL("Derivation of slope lengths from catchment areas. These are rough approximations! Applies not to Desmet & Govers' method."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("no conversion (areas already given as specific catchment area)"),
			_TL("1 / cell size (specific catchment area)"),
			_TL("square root (catchment length)")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method (LS)"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Moore et al. 1991"),
			_TL("Desmet & Govers 1996"),
			_TL("Boehner & Selige 2006")
		), 0
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_DG"		, _TL("Desmet & Govers"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "EROSIVITY"	, _TL("Rill/Interrill Erosivity"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		pNode	, "STABILITY"	, _TL("Stability"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("stable"),
			_TL("instable (thawing)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLS_Factor::On_Execute(void)
{
	int			Conversion;
	CSG_Grid	*pArea, *pSlope, *pLS;

	//-----------------------------------------------------
	pSlope		= Parameters("SLOPE")		->asGrid();
	pArea		= Parameters("AREA")		->asGrid();
	pLS			= Parameters("LS")			->asGrid();
	Conversion	= Parameters("CONV")		->asInt();
	m_Method	= Parameters("METHOD")		->asInt();
	m_Erosivity	= Parameters("EROSIVITY")	->asInt();
	m_Stability	= Parameters("STABILITY")	->asInt();

	DataObject_Set_Colors(pLS, 100, SG_COLORS_RED_GREY_GREEN, true);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pArea->is_NoData(x, y) || pSlope->is_NoData(x, y) )
			{
				pLS->Set_NoData(x, y);
			}
			else
			{
				double	Area;

				switch( Conversion )
				{
				default:	Area	= pArea->asDouble(x, y);					// no conversion...
				case 1:		Area	= pArea->asDouble(x, y) / Get_Cellsize();	// pseudo specific catchment area...
				case 2:		Area	= sqrt(pArea->asDouble(x, y));				// pseudo slope length...
				}

				pLS->Set_Value(x, y, Get_LS(pSlope->asDouble(x, y), Area));
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
double CLS_Factor::Get_LS(double Slope, double Area)
{
	double	LS, sinSlope;

	switch( m_Method )
	{
	//-----------------------------------------------------
	case 0:	default:
		{
			LS			= (0.4 + 1) * pow(Area / 22.13, 0.4) * pow(sin(Slope) / 0.0896, 1.3);
		}
		break;

	//-----------------------------------------------------
	case 1:
		{
			double	L, S, m, x, d;

			sinSlope	= sin(Slope);

			d			= Get_Cellsize();

			m			= m_Erosivity * (sinSlope / 0.0896) / (3.0 * pow(sinSlope, 0.8) + 0.56);
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
			else if( m_Stability == 0 )		// >= 9% Steigung, stabil
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
				LS		= sqrt(Area / 22.13)
						* (65.41 * sinSlope * sinSlope + 4.56 * sinSlope + 0.065);
			}
			else					// <= ca. 3 Degree
			{
				LS		= pow (Area / 22.13, 3.0 * pow(Slope, 0.6))
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
CTCI_Low::CTCI_Low(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("TCI Low"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Terrain Classification Index for Lowlands (TCI Low).\n"
		"\n"
		"Reference:\n"
		"Bock, M. / Böhner, J. / Conrad, O. / Köthe, R. / Ringeler, A. (2007): "
		"Methods for creating Functional Soil Databases and applying Digital Soil Mapping with SAGA GIS. "
		"In: Hengl, T. / Panagos, P. / Jones, A. / Toth, G. [Eds.]: "
		"Status and prospect of soil information in south-eastern Europe: soil databases, projects and applications. "
		"EUR 22646 EN Scientific and Technical Research series, Office for Official Publications of the European Communities, Luxemburg, p.149-162. "
		"<a target=\"_blank\" href==\"http://eusoils.jrc.ec.europa.eu/ESDB_Archive/eusoils_docs/esb_rr/EUR22646EN.pdf\">online</a>.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DISTANCE"	, _TL("Vertical Distance to Channel Network"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "TWI"			, _TL("Topographic Wetness Index"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "TCILOW"		, _TL("TCI Low"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTCI_Low::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pDistance	= Parameters("DISTANCE")->asGrid();
	CSG_Grid	*pTWI		= Parameters("TWI"     )->asGrid();
	CSG_Grid	*pTCI_Low	= Parameters("TCILOW"  )->asGrid();

	DataObject_Set_Colors(pTCI_Low, 100, SG_COLORS_RED_GREY_BLUE, true);

	double	dMin	= pDistance->Get_ZMin();
	double	dRange	= pDistance->Get_ZMax() - dMin;
	double	wRange	= log(1.0 + pTWI->Get_ZMax());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDistance->is_NoData(x, y) || pTWI->is_NoData(x, y) )
			{
				pTCI_Low->Set_NoData(x, y);
			}
			else
			{
				double	d	= (pDistance->asDouble(x, y) - dMin) / dRange;
				double	w	= log(1.0 + pTWI->asDouble(x, y)) / wRange;

				pTCI_Low->Set_Value(x, y, (2.0 * d + w) / 3.0);
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
