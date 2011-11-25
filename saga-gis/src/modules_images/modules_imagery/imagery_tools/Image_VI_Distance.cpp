/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Image_VI_Distance.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Image_VI_Distance.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CImage_VI_Distance::CImage_VI_Distance(void)
{
	Set_Name		(_TL("Vegetation Index (Distance Based)"));

	Set_Author		(SG_T("V.Olaya (c) 2004, O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Distance based vegetation indices.\n"
	//	"\n<ul>"
	//	"<li>Transformed Soil Adjusted Vegetation Index (s. McCloy, 2006)\n"
	//	"    TSAVI = (NIR - Gain * R - Offset) * Gain / (Gain * NIR + R - Gain * Offset)</li>\n"
	//	"<li>Transformed Soil Adjusted Vegetation Index (s. McCloy, 2006)\n"
	//	"    ATSAVI = (NIR - Gain * R - Offset) / (Gain * NIR + R - gain * Offset + 0.8 * (1 + Gain^2))</li>\n"
	//	"</ul>(NIR = near infrared, R = red, S = soil adjustment factor)\n"
		"\n"
		"References:\n"
		"K.R. McCloy (2006): Resource Management Information Systems: Remote Sensing, GIS and Modelling. 2nd Edition, CRC Taylor & Francis, 575pp.\n"
		"\n"
		"N.G. Silleos, T.K. Alexandridis, I.Z. Gitas & K. Perakis (2006): "
		"Vegetation Indices: Advances Made in Biomass Estimation and Vegetation Monitoring in the Last 30 Years, "
		"Geocarto International, 21:4, 21-28, "
		"<a target=\"_blank\" href=\"http://dx.doi.org/10.1080/10106040608542399\">online</a>.\n"
	));

	Parameters.Add_Grid(
		NULL, "RED"			, _TL("Red Reflectance"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "NIR"			, _TL("Near Infrared Reflectance"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "PVI0"		, _TL("Perpendicular Vegetation Index (Richardson and Wiegand, 1977)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "PVI1"		, _TL("Perpendicular Vegetation Index (Perry and Lautenschlager, 1984)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "PVI2"		, _TL("Perpendicular Vegetation Index (Walther and Shabaani)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "PVI3"		, _TL("Perpendicular Vegetation Index (Qi, et al., 1994)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "TSAVI"		, _TL("Transformed Soil Adjusted Vegetation Index (Baret et al. 1989)"), 
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "ATSAVI"		, _TL("Transformed Soil Adjusted Vegetation Index (Baret and Guyot, 1991)"), 
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "INTERCEPT"	, _TL("Intercept of Soil Line"), 
		_TL(""), 
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		NULL, "SLOPE"		, _TL("Slope of Soil Line"), 
		_TL(""), 
		PARAMETER_TYPE_Double, 0.5
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CImage_VI_Distance::On_Execute(void)
{	
	CSG_Grid	*pRed		= Parameters("RED"   )->asGrid();
	CSG_Grid	*pNIR		= Parameters("NIR"   )->asGrid();

	CSG_Grid	*pPVI0		= Parameters("PVI0"  )->asGrid();
	CSG_Grid	*pPVI1		= Parameters("PVI1"  )->asGrid();
	CSG_Grid	*pPVI2		= Parameters("PVI2"  )->asGrid();
	CSG_Grid	*pPVI3		= Parameters("PVI3"  )->asGrid();
	CSG_Grid	*pTSAVI		= Parameters("TSAVI" )->asGrid();
	CSG_Grid	*pATSAVI	= Parameters("ATSAVI")->asGrid();

	DataObject_Set_Colors(pPVI0  , 100, SG_COLORS_WHITE_GREEN, false);
	DataObject_Set_Colors(pPVI1  , 100, SG_COLORS_WHITE_GREEN, false);
	DataObject_Set_Colors(pPVI2  , 100, SG_COLORS_WHITE_GREEN, false);
	DataObject_Set_Colors(pPVI3  , 100, SG_COLORS_WHITE_GREEN, false);
	DataObject_Set_Colors(pTSAVI , 100, SG_COLORS_WHITE_GREEN, false);
	DataObject_Set_Colors(pATSAVI, 100, SG_COLORS_WHITE_GREEN, false);

	m_Slope		= Parameters("SLOPE"    )->asDouble();
	m_Intercept	= Parameters("INTERCEPT")->asDouble();

	if( m_Slope <= 0.0 )
	{
		Error_Set(_TL("slope value has to be greater than zero"));

		return( false );
	}
		
    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pRed->is_NoData(x, y) || pNIR->is_NoData(x, y) )
			{
				if( pPVI0   )	pPVI0  ->Set_NoData(x, y);
				if( pPVI1   )	pPVI1  ->Set_NoData(x, y);
				if( pPVI2   )	pPVI2  ->Set_NoData(x, y);
				if( pPVI3   )	pPVI3  ->Set_NoData(x, y);
				if( pTSAVI  )	pTSAVI ->Set_NoData(x, y);
				if( pATSAVI )	pATSAVI->Set_NoData(x, y);
			}
			else
			{
				double	R, NIR, Value;

				R	= pRed->asDouble(x, y);
				NIR	= pNIR->asDouble(x, y);

				if( pPVI0   ) { if( Get_PVI0  (R, NIR, Value) ) pPVI0  ->Set_Value(x, y, Value); else pPVI0  ->Set_NoData(x, y); }
				if( pPVI1   ) { if( Get_PVI1  (R, NIR, Value) ) pPVI1  ->Set_Value(x, y, Value); else pPVI1  ->Set_NoData(x, y); }
				if( pPVI2   ) { if( Get_PVI2  (R, NIR, Value) ) pPVI2  ->Set_Value(x, y, Value); else pPVI2  ->Set_NoData(x, y); }
				if( pPVI3   ) { if( Get_PVI3  (R, NIR, Value) ) pPVI3  ->Set_Value(x, y, Value); else pPVI3  ->Set_NoData(x, y); }
				if( pTSAVI  ) { if( Get_TSAVI (R, NIR, Value) ) pTSAVI ->Set_Value(x, y, Value); else pTSAVI ->Set_NoData(x, y); }
				if( pATSAVI ) { if( Get_ATSAVI(R, NIR, Value) ) pATSAVI->Set_Value(x, y, Value); else pATSAVI->Set_NoData(x, y); }
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
inline bool CImage_VI_Distance::Get_PVI0(double R, double NIR, double &Value)	// Richardson & Wiegand, 1977
{
	double dB1	= 1.0 / m_Slope;
	double dB0	= R - NIR / m_Slope;

	double Rgg5	= (dB1 * m_Intercept - dB0 * m_Slope) / (dB1 - m_Slope);
	double Rgg7	= (m_Intercept - dB0) / (dB1 - m_Slope);
	
	Value	= sqrt(pow(Rgg5 - R, 2.0) + pow(Rgg7 - NIR, 3.0));

	return( true );
}

//---------------------------------------------------------
inline bool CImage_VI_Distance::Get_PVI1(double R, double NIR, double &Value)	// Perry & Rautenschlager, 1984
{
	if( (Value = sqrt(1.0 + m_Slope * m_Slope)) != 0.0 )
	{
		Value	= ((m_Slope * NIR - R) + m_Intercept) / Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Distance::Get_PVI2(double R, double NIR, double &Value)
{
	if( (Value = sqrt(1.0 + m_Intercept)) != 0.0 )
	{
		Value	= (NIR - m_Intercept) * (R + m_Slope) / Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Distance::Get_PVI3(double R, double NIR, double &Value)	// Qi et al., 1994
{
	Value	= (NIR * m_Intercept) - (R * m_Slope);

	return( true );
}

//---------------------------------------------------------
inline bool CImage_VI_Distance::Get_TSAVI(double R, double NIR, double &Value)	// Baret et al., 1989
{
	if( (Value = R + m_Slope * NIR - m_Slope * m_Intercept) != 0.0 )
	{
		Value	= m_Slope * (NIR - m_Slope) * (R - m_Intercept) / Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Distance::Get_ATSAVI(double R, double NIR, double &Value)	// Baret & Guyot, 1991
{
	if( (Value = R + m_Slope * NIR - m_Slope * m_Intercept + 0.08 * (1.0 + m_Slope*m_Slope)) != 0.0 )
	{
		Value	= m_Slope * (NIR - m_Slope * R - m_Intercept) / Value;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
