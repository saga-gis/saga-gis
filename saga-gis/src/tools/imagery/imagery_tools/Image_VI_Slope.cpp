/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Image_VI_Slope.cpp
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Image_VI_Slope.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CImage_VI_Slope::CImage_VI_Slope(void)
{
	Set_Name		(_TL("Vegetation Index (Slope Based)"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Slope based vegetation indices.\n"
		"\n<ul>"
        "<li>Difference Vegetation Index\n"
		"    DVI = NIR - R</li>\n"
		"<li>Normalized Difference Vegetation Index (Rouse et al. 1974)\n"
		"    NDVI = (NIR - R) / (NIR + R)</li>\n"
		"<li>Ratio Vegetation Index (Richardson and Wiegand, 1977)\n"
		"    RVI = R / NIR</li>\n"
		"<li>Normalized Ratio Vegetation Index (Baret and Guyot, 1991)\n"
		"    NRVI = (RVI - 1) / (RVI + 1)</li>\n"
		"<li>Transformed Vegetation Index (Deering et al., 1975)\n"
		"    TVI = [(NIR - R) / (NIR + R) + 0.5]^0.5</li>\n"
		"<li>Corrected Transformed Ratio Vegetation Index (Perry and Lautenschlager, 1984)\n"
		"    CTVI = [(NDVI + 0.5) / abs(NDVI + 0.5)] * [abs(NDVI + 0.5)]^0.5</li>\n"
		"<li>Thiam's Transformed Vegetation Index (Thiam, 1997)\n"
		"    RVI = [abs(NDVI) + 0.5]^0.5</li>\n"
		"<li>Soil Adjusted Vegetation Index (Huete, 1988)\n"
		"    SAVI = [(NIR - R) / (NIR + R)] * (1 + S)</li>\n"
		"</ul>(NIR = near infrared, R = red, S = soil adjustment factor)\n"
		"\n\n"
	));

	Add_Reference("McCloy, K.R.", "2006",
		"Resource Management Information Systems: Remote Sensing, GIS and Modelling",
		"2nd Edition, CRC Taylor & Francis, 575pp."
	);

	Add_Reference("Mroz, M., Sobieraj, A.", "2004",
		"Comparison of Several Vegetation Indices Calculated on the Basis of a Seasonal Spot XS Time Series, and their Suitability for Land Cover and Agricultural Crop Identification",
		"Technical Sciences, No. 7, 39-66.",
		SG_T("http://www.uwm.edu.pl/wnt/technicalsc/ts7_2004/4_7_2004.pdf")
	);

	Add_Reference("Silleos, N.G., Alexandridis, T.K., Gitas, I.Z., Perakis, K.", "2006",
		"Vegetation Indices: Advances Made in Biomass Estimation and Vegetation Monitoring in the Last 30 Years",
		"Geocarto International, 21:4, 21-28.",
		SG_T("http://dx.doi.org/10.1080/10106040608542399")
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
		"", "DVI"	, _TL("Difference Vegetation Index"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "NDVI"	, _TL("Normalized Difference Vegetation Index"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
	
	Parameters.Add_Grid(
		"", "RVI"	, _TL("Ratio Vegetation Index"), 
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "NRVI"	, _TL("Normalized Ratio Vegetation Index"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "TVI"	, _TL("Transformed Vegetation Index"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "CTVI"	, _TL("Corrected Transformed Vegetation Index"), 
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "TTVI"	, _TL("Thiam's Transformed Vegetation Index"), 
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "SAVI"	, _TL("Soil Adjusted Vegetation Index"), 
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(
		"SAVI", "SOIL"	, _TL("Soil Adjustment Factor"), 
		_TL("For SAVI, suggested values (after Silleos et al. 2006): 1.0 = very low, 0.5 = intermediate, 0.25 = high densitity vegetation."),
		0.5, 0.0, true, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CImage_VI_Slope::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SAVI") )
	{
		pParameters->Set_Enabled("SOIL", pParameter->asDataObject() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CImage_VI_Slope::On_Execute(void)
{
	CSG_Grid	*pRed		= Parameters("RED"  )->asGrid();
	CSG_Grid	*pNIR		= Parameters("NIR"  )->asGrid();

	CSG_Grid	*pDVI		= Parameters("DVI"  )->asGrid();
	CSG_Grid	*pNDVI		= Parameters("NDVI" )->asGrid();
	CSG_Grid	*pRVI		= Parameters("RVI"  )->asGrid();
	CSG_Grid	*pNRVI		= Parameters("NRVI" )->asGrid();
	CSG_Grid	*pTVI		= Parameters("TVI"  )->asGrid();
	CSG_Grid	*pCTVI		= Parameters("CTVI" )->asGrid();
	CSG_Grid	*pTTVI		= Parameters("TTVI" )->asGrid();
	CSG_Grid	*pSAVI		= Parameters("SAVI" )->asGrid();

	DataObject_Set_Colors(pDVI  , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pNDVI , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pRVI  , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pNRVI , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pTVI  , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pCTVI , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pTTVI , 11, SG_COLORS_RED_GREY_GREEN, false);
	DataObject_Set_Colors(pSAVI , 11, SG_COLORS_RED_GREY_GREEN, false);

	m_Soil	= Parameters("SOIL")->asDouble();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pRed->is_NoData(x, y) || pNIR->is_NoData(x, y) )
			{
				if( pDVI   )	pDVI	->Set_NoData(x, y);
				if( pNDVI  )	pNDVI	->Set_NoData(x, y);
				if( pRVI   )	pRVI	->Set_NoData(x, y);
				if( pNRVI  )	pNRVI	->Set_NoData(x, y);
				if( pTVI   )	pTVI	->Set_NoData(x, y);
				if( pTTVI  )	pTTVI	->Set_NoData(x, y);
				if( pCTVI  )	pCTVI	->Set_NoData(x, y);
				if( pSAVI  )	pSAVI	->Set_NoData(x, y);
			}
			else
			{
				double	R, NIR, Value;

				R	= pRed->asDouble(x, y);
				NIR	= pNIR->asDouble(x, y);

				if( pDVI   ) { if( Get_DVI  (R, NIR, Value) ) pDVI  ->Set_Value(x, y, Value); else pDVI  ->Set_NoData(x, y); }
				if( pNDVI  ) { if( Get_NDVI (R, NIR, Value) ) pNDVI ->Set_Value(x, y, Value); else pNDVI ->Set_NoData(x, y); }
				if( pRVI   ) { if( Get_RVI  (R, NIR, Value) ) pRVI  ->Set_Value(x, y, Value); else pRVI  ->Set_NoData(x, y); }
				if( pNRVI  ) { if( Get_NRVI (R, NIR, Value) ) pNRVI ->Set_Value(x, y, Value); else pNRVI ->Set_NoData(x, y); }
				if( pTVI   ) { if( Get_TVI  (R, NIR, Value) ) pTVI  ->Set_Value(x, y, Value); else pTVI  ->Set_NoData(x, y); }
				if( pCTVI  ) { if( Get_CTVI (R, NIR, Value) ) pCTVI ->Set_Value(x, y, Value); else pCTVI ->Set_NoData(x, y); }
				if( pTTVI  ) { if( Get_TTVI (R, NIR, Value) ) pTTVI ->Set_Value(x, y, Value); else pTTVI ->Set_NoData(x, y); }
				if( pSAVI  ) { if( Get_SAVI (R, NIR, Value) ) pSAVI ->Set_Value(x, y, Value); else pSAVI ->Set_NoData(x, y); }
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_DVI(double R, double NIR, double &Value)
{
	Value	= NIR - R;

	return( true );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_NDVI(double R, double NIR, double &Value)
{
	if( R + NIR != 0.0 )
	{
		Value	= (NIR - R) / (NIR + R);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_RVI(double R, double NIR, double &Value)
{
	if( R != 0.0 )
	{
		Value	= NIR / R;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_NRVI(double R, double NIR, double &Value)
{
	if( Get_RVI(R, NIR, Value) && Value + 1.0 != 0.0 )
	{
		Value	= (Value - 1.0) / (Value + 1.0);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_TVI(double R, double NIR, double &Value)
{
	if( Get_NDVI(R, NIR, Value) && Value + 0.5 >= 0.0 )
	{
		Value	= sqrt(Value + 0.5);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_CTVI(double R, double NIR, double &Value)
{
	if( Get_NDVI(R, NIR, Value) )
	{
		Value	+= 0.5;

		Value	= Value > 0.0 ? sqrt(fabs(Value)) : -sqrt(fabs(Value));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_TTVI(double R, double NIR, double &Value)
{
	if( Get_NDVI(R, NIR, Value) )
	{
		Value	= sqrt(fabs(Value + 0.5));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CImage_VI_Slope::Get_SAVI(double R, double NIR, double &Value)
{
	if( Get_NDVI(R, NIR, Value) )
	{
		Value	= Value * (1.0 + m_Soil);

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
