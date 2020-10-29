
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
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
	Set_Name		(_TL("Topographic Wetness Index (TWI)"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Calculation of the slope and specific catchment area (SCA) based Topographic Wetness Index (TWI)."
	));

	Add_Reference(
		"Beven, K.J., Kirkby, M.J.", "1979",
		"A physically-based variable contributing area model of basin hydrology",
		"Hydrology Science Bulletin 24(1), p.43-69."
	);

	Add_Reference(
		"Boehner, J., Selige, T.", "2006",
		"Spatial Prediction of Soil Attributes Using Terrain Analysis and Climate Regionalisation",
		"In: Boehner, J., McCloy, K.R., Strobl, J.: 'SAGA - Analysis and Modelling Applications', "
		"Goettinger Geographische Abhandlungen, Vol.115, p.13-27."
	);

	Add_Reference(
		"Moore, I.D., Grayson, R.B., Ladson, A.R.", "1991",
		"Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications",
		"Hydrological Processes, Vol.5, No.1."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SLOPE"	, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"AREA"	, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"TRANS"	, _TL("Transmissivity"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TWI"	, _TL("Topographic Wetness Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"CONV"	, _TL("Area Conversion"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("no conversion (areas already given as specific catchment area)"),
			_TL("1 / cell size (pseudo specific catchment area)")
		), 0
	);

	Parameters.Add_Choice("",
		"METHOD", _TL("Method (TWI)"),
		_TL(""),
		CSG_String::Format("%s|%s",
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
	pSlope		= Parameters("SLOPE" )->asGrid();
	pArea		= Parameters("AREA"  )->asGrid();
	pKf			= Parameters("TRANS" )->asGrid();
	pTWI		= Parameters("TWI"   )->asGrid();

	bConvert	= Parameters("CONV"  )->asInt() == 1;
	bTopmodel	= Parameters("METHOD")->asInt() == 1;

	DataObject_Set_Colors(pTWI, 11, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	Kf			= 1.;

	if( bTopmodel )
	{
		Mean_TWI	= 0.;
		Mean_Kf		= 0.;
		Area		= 0.;

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
							Kf	= 0.;
						}
					}

					Slope	= tan(pSlope->asDouble(x, y));

					if( Slope	< M_ALMOST_ZERO )
						Slope	= M_ALMOST_ZERO;

					Area		++;
					Mean_TWI	+= log((pArea->asDouble(x, y) / (bConvert ? Get_Cellsize() : 1.)) / Slope);
					Mean_Kf		+= Kf;
				}
			}
		}

		if( Area > 0. )
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
	Set_Name		(_TL("Stream Power Index"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Calculation of stream power index based on slope and specific catchment area (SCA).\n"
		"SPI = SCA * tan(Slope)"
	));

	Add_Reference(
		"Moore, I.D., Grayson, R.B., Ladson, A.R.", "1991",
		"Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications",
		"Hydrological Processes, Vol.5, No.1\n"
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SLOPE"	, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"AREA"	, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SPI"	, _TL("Stream Power Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"CONV"	, _TL("Area Conversion"),
		_TL(""),
		CSG_String::Format("%s|%s",
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
	CSG_Grid	*pSlope	= Parameters("SLOPE")->asGrid();
	CSG_Grid	*pArea	= Parameters("AREA" )->asGrid();
	CSG_Grid	*pSPI	= Parameters("SPI"  )->asGrid();

	bool	bConvert	= Parameters("CONV" )->asInt() == 1;

	DataObject_Set_Colors(pSPI, 11, SG_COLORS_RED_GREY_GREEN, true);

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
				pSPI->Set_Value(x, y, (pArea->asDouble(x, y) / (bConvert ? Get_Cellsize() : 1.)) * tan(pSlope->asDouble(x, y)));
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
	Set_Name		(_TL("LS Factor"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Calculation of slope length (LS) factor as used by the Universal Soil Loss Equation (USLE), "
		"based on slope and specific catchment area (SCA, as substitute for slope length)."
	));

	Add_Reference(
		"Boehner, J., Selige, T.", "2006",
		"Spatial Prediction of Soil Attributes Using Terrain Analysis and Climate Regionalisation",
		"In: Boehner, J., McCloy, K.R., Strobl, J.: 'SAGA - Analysis and Modelling Applications', "
		"Goettinger Geographische Abhandlungen, Vol.115, p.13-27."
	);

	Add_Reference(
		"Desmet & Govers", "1996",
		"A GIS Procedure for Automatically Calculating the USLE LS Factor on Topographically Complex Landscape Units",
		"Journal of Soil and Water Conservation, 51(5):427.433."
	);

	Add_Reference(
		"Kinnell, P.I.A.", "2005",
		"'Alternative Approaches for Determining the USLE-M Slope Length Factor for Grid Cells",
		"soil.scijournals.org, 69/3/674", SG_T("http://soil.scijournals.org/cgi/content/full/69/3/674")
	);

	Add_Reference(
		"Moore, I.D., Grayson, R.B., Ladson, A.R.", "1991",
		"Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications",
		"Hydrological Processes, Vol.5, No.1."
	);

	Add_Reference(
		"Wischmeier, W.H., Smith, D.D.", "1978",
		"Predicting rainfall erosion losses - A guide to conservation planning",
		"Agriculture Handbook No. 537: US Department of Agriculture, Washington DC."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"AREA"		, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"LS"		, _TL("LS Factor"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"CONV"		, _TL("Area to Length Conversion"),
		_TL("Derivation of slope lengths from catchment areas. These are rough approximations! Applies not to Desmet & Govers' method."),
		CSG_String::Format("%s|%s|%s",
			_TL("no conversion (areas already given as specific catchment area)"),
			_TL("1 / cell size (specific catchment area)"),
			_TL("square root (catchment length)")
		), 0
	);

	Parameters.Add_Bool("",
		"FEET"		, _TL("Feet Adjustment"),
		_TL("Needed if area and lengths come from coordinates measured in feet."),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method (LS)"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Moore et al. 1991"),
			_TL("Desmet & Govers 1996"),
			_TL("Boehner & Selige 2006")
		), 0
	);

	Parameters.Add_Node("",
		"NODE_DG"	, _TL("Desmet & Govers"),
		_TL("")
	);

	Parameters.Add_Double("NODE_DG",
		"EROSIVITY"	, _TL("Rill/Interrill Erosivity"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Choice("NODE_DG",
		"STABILITY"	, _TL("Stability"),
		_TL(""),
		CSG_String::Format("%s|%s",
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
	CSG_Grid	*pSlope	= Parameters("SLOPE")->asGrid();
	CSG_Grid	*pArea	= Parameters("AREA" )->asGrid();
	CSG_Grid	*pLS	= Parameters("LS"   )->asGrid();

	int	Conversion	= Parameters("CONV")->asInt();

	bool	bFeet	= Parameters("FEET")->asBool();

	m_Method	= Parameters("METHOD"   )->asInt();
	m_Erosivity	= Parameters("EROSIVITY")->asInt();
	m_Stability	= Parameters("STABILITY")->asInt();

	DataObject_Set_Colors(pLS, 11, SG_COLORS_RED_GREY_GREEN, true);

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
				double	Slope	= pSlope->asDouble(x, y);
				double	Area	= pArea ->asDouble(x, y);

				switch( Conversion )
				{
				case 1: Area /= Get_Cellsize(); break; // pseudo specific catchment area...
				case 2: Area  = sqrt(Area)    ; break; // pseudo slope length...
				}

				pLS->Set_Value(x, y, Get_LS(Slope, Area, bFeet));
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
inline double CLS_Factor::Get_LS(double Slope, double SCA, bool bFeet)
{
	if( bFeet )
	{
		SCA	*= 0.3048;
	}

	double	LS, sinSlope = sin(Slope);

	switch( m_Method )
	{
	//-----------------------------------------------------
	default: {
		LS		= (0.4 + 1) * pow(SCA / 22.13, 0.4) * pow(sinSlope / 0.0896, 1.3);
		break; }

	//-----------------------------------------------------
	case  1: {
		double	L, S, m, x, d;

		d		= (bFeet ? 0.3048 : 1.) * Get_Cellsize();

		m		= m_Erosivity * (sinSlope / 0.0896) / (3. * pow(sinSlope, 0.8) + 0.56);
		m		= m / (1. + m);

		x		= 1.;

		// x : coefficient that adjusts for width of flow at the center of the cell.
		// It has a value of 1. when the flow is toward a side and sqrt(2.) when the flow is toward a corner.
		// (Peter I. A. Kinnell: 'ALTERNATIVE APPROACHES FOR DETERMINING THE USLE-M SLOPE LENGTH FACTOR FOR GRID CELLS'
		// https://www.soils.org/publications/sssaj/abstracts/69/3/0674)

		L		= (pow(SCA + d*d, m + 1.) - pow(SCA, m + 1.))
				/ (pow(d, m + 2.) * pow(22.13, m) * pow(x, m));

		//-------------------------------------------------
		if( Slope < 0.08975817419 )		// < 9% Steigung := atan(0.09), ca. 5 Degree
		{
			S	= 10.8 * sinSlope + 0.03;	
		}
		else if( m_Stability == 0 )		// >= 9% Steigung, stabil
		{
			S	= 16.8 * sinSlope - 0.5;
		}
		else							// >= 9% Steigung, tauend u. instabil
		{
			S	= pow(sinSlope / 0.896, 0.6);
		}

		LS		= L * S;

		break; }

	//-----------------------------------------------------
	case  2: {
		if( Slope > 0.0505 )	// >  ca. 3 Degree
		{
			LS	= sqrt(SCA / 22.13)
				* (65.41 * sinSlope * sinSlope + 4.56 * sinSlope + 0.065);
		}
		else					// <= ca. 3 Degree
		{
			LS	= pow (SCA / 22.13, 3. * pow(Slope, 0.6))
				* (65.41 * sinSlope * sinSlope + 4.56 * sinSlope + 0.065);
		}
		break; }
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
	Set_Name		(_TL("TCI Low"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Terrain Classification Index for Lowlands (TCI Low)."
	));

	Add_Reference(
		"Bock, M., Boehner, J., Conrad, O., Koethe, R., Ringeler, A.", "2007",
		"Methods for creating Functional Soil Databases and applying Digital Soil Mapping with SAGA GIS",
		"In: Hengl, T., Panagos, P., Jones, A., Toth, G. [Eds.]: "
		"Status and prospect of soil information in south-eastern Europe: soil databases, projects and applications. "
		"EUR 22646 EN Scientific and Technical Research series, Office for Official Publications of the European Communities, Luxemburg, p.149-162.",
		SG_T("http://eusoils.jrc.ec.europa.eu/ESDB_Archive/eusoils_docs/esb_rr/EUR22646EN.pdf")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DISTANCE"	, _TL("Vertical Distance to Channel Network"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"TWI"		, _TL("Topographic Wetness Index"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"TCILOW"	, _TL("TCI Low"),
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

	DataObject_Set_Colors(pTCI_Low, 11, SG_COLORS_RED_GREY_BLUE, false);

	double	dMax	= pDistance->Get_Max();
	double	dRange	= pDistance->Get_Range();
	double	wMin	= pTWI->Get_Min();
	double	wRange	= log(1. + pTWI->Get_Range());

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
				double	d	= (dMax - pDistance->asDouble(x, y)) / dRange;			// inverted, normalized [0...1]
				double	w	= log(1. + (pTWI->asDouble(x, y) - wMin)) / wRange;	// natural logarithm, normalized [0...1]

				pTCI_Low->Set_Value(x, y, (2. * d + w) / 3.);
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
