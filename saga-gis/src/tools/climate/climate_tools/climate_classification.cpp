
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               climate_classification.cpp              //
//                                                       //
//                 Copyright (C) 2018 by                 //
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
#include "climate_classification.h"

#include "climate_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum EMethod
{
	Koeppen1936 = 0,
	Koeppen1936_No_As,
	Peel2007,
	Wissmann,
	Thornthwaite1931,
	TrollPaffen
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CClimate_Classification::CClimate_Classification(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Climate Classification"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"This tool applies a climate classification scheme using monthly mean temperature and precipitation data. "
		"Currently implemented classification schemes are Koeppen-Geiger (1936), Thornthwaite (1931), "
		"and Troll-Paffen (1964). Because of some less precise definitions the Troll-Paffen scheme "
		"still needs some revisions. "
	));

	Add_Reference("Bluethgen, J.", "1964",
		"Allgemeine Klimageographie",
		"Lehrbuch der allgemeinen Geographie (Vol. 2)."
	);

	Add_Reference("Koeppen, W., Geiger, R.", "1936",
		"Handbuch der Klimatologie",
		"Gebrüder Borntraeger, Berlin.",
		SG_T("https://upload.wikimedia.org/wikipedia/commons/3/3c/Das_geographische_System_der_Klimate_%281936%29.pdf"), SG_T("Wikimedia")
	);

	Add_Reference("Kottek, M., Grieser, J., Beck, C., Rudolf, B., Rubel, F.", "2006",
		"World map of the Koeppen-Geiger climate classification updated",
		"Meteorologische Zeitschrift, 15(3), 259-263.",
		SG_T("https://www.schweizerbart.de/papers/metz/detail/15/55034/World_Map_of_the_Koppen_Geiger_climate_classificat"), SG_T("Free Access")
	);

	Add_Reference("Peel, M. C., Finlayson, B. L., McMahon, T. A.", "2007",
		"Updated world map of the Koeppen-Geiger climate classification",
		"Hydrology and earth system sciences discussions, 4(2), 439-473.",
		SG_T("https://hal.archives-ouvertes.fr/hal-00305098/document"), SG_T("Free Access")
	);

	Add_Reference("Thornthwaite, C. W.", "1931",
		"The climates of North America: according to a new classification",
		"Geographical review, 21(4), 633-655.",
		SG_T("https://www.jstor.org/stable/pdf/209372.pdf"), SG_T("JSTOR")
	);

	Add_Reference("Troll, C. & Paffen, K.H.", "1964",
		"Karte der Jahreszeitenklimate der Erde",
		"Erdkunde 18, p5-28",
		SG_T("https://www.erdkunde.uni-bonn.de/archive/1964/karte-der-jahreszeiten-klimate-der-erde/at_download/attachment"), SG_T("Free Access")
	);

	Add_Reference("Willmes, C., Becker, D., Brocks, S., Huett, C., Bareth, G.", "2016",
		"High resolution Koeppen-Geiger classifications of paleo-climate simulations",
		"Trans. in GIS, DOI:10.1111/tgis.12187.",
		SG_T("https://onlinelibrary.wiley.com/doi/abs/10.1111/tgis.12187"), SG_T("Wiley Online")
	);

	Add_Reference("Wissmann, H.", "1939",
		"Die Klima-und Vegetationsgebiete Eurasiens: Begleitworte zu einer Karte der Klimagebiete Eurasiens",
		"Z. Ges. Erdk. Berlin, p.81-92."
	);

	Add_Reference("http://koeppen-geiger.vu-wien.ac.at/",
		SG_T("World Maps of Koeppen-Geiger Climate Classification")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"T"			, _TL("Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"P"			, _TL("Precipitation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CLASSES"	, _TL("Climate Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Classification"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			SG_T("Koeppen-Geiger"),
			SG_T("Koeppen-Geiger without As/Aw differentiation"),
			SG_T("Koeppen-Geiger after Peel et al. (2007)"),
			SG_T("Wissmann (1939)"),
			SG_T("Thornthwaite (1931)"),
			SG_T("Troll-Paffen")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClimate_Classification::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
	//	pParameters->Set_Enabled("", pParameter->asInt() == Koeppen1936);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CClimate_Classification::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pT	= Parameters("T")->asGridList();

	if( pT->Get_Grid_Count() != 12 )
	{
		Error_Fmt("%s: %s", _TL("Temperature"), _TL("there has to be one input grid for each month"));

		return( false );
	}

	CSG_Parameter_Grid_List	*pP	= Parameters("P")->asGridList();

	if( pP->Get_Grid_Count() != 12 )
	{
		Error_Fmt("%s: %s", _TL("Precipitation"), _TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	CSG_Grid	*pClasses	= Parameters("CLASSES")->asGrid();

	Set_Classified(pClasses, Method);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Class	= 0;

			CSG_Simple_Statistics	T(true), P(true);

			if( Get_Values(x, y, pT, T) && Get_Values(x, y, pP, P) )
			{
				switch( Method )
				{
				default:
					Class	= Get_KoppenGeiger(Method, T, P);
					break;

				case Wissmann:
					Class	= Get_Wissmann    (Method, T, P);
					break;

				case Thornthwaite1931:
					Class	= Get_Thornthwaite(Method, T, P);
					break;

				case TrollPaffen:
					Class	= Get_TrollPaffen (Method, T, P);
					break;
				}
			}

			pClasses->Set_Value(x, y, Class);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum EKG
{
	KG_Af = 1, KG_Am, KG_As, KG_Aw,
	KG_BWk, KG_BWh, KG_BSk, KG_BSh,
	KG_Cfa, KG_Cfb, KG_Cfc, KG_Csa, KG_Csb, KG_Csc, KG_Cwa, KG_Cwb, KG_Cwc,
	KG_Dfa, KG_Dfb, KG_Dfc, KG_Dfd, KG_Dsa, KG_Dsb, KG_Dsc, KG_Dsd, KG_Dwa, KG_Dwb, KG_Dwc, KG_Dwd,
	KG_ET, KG_EF,
	KG_Count
};

//---------------------------------------------------------
const CClimate_Classification::TClassInfo KG_Info[KG_Count]	=
{
	{ KG_Af   , SG_GET_RGB(148,   1,   1), "Af" , "equatorial, fully humid" },
	{ KG_Am   , SG_GET_RGB(254,   0,   0), "Am" , "equatorial, monsoonal" },
	{ KG_As   , SG_GET_RGB(255, 154, 154), "As" , "equatorial, summer dry" },
	{ KG_Aw   , SG_GET_RGB(255, 207, 207), "Aw" , "equatorial, winter dry" },
	{ KG_BWk  , SG_GET_RGB(255, 255, 101), "BWk", "cold desert" },
	{ KG_BWh  , SG_GET_RGB(255, 207,   0), "BWh", "hot desert" },
	{ KG_BSk  , SG_GET_RGB(207, 170,  85), "BSk", "cold steppe" },
	{ KG_BSh  , SG_GET_RGB(207, 142,  20), "BSh", "hot steppe" },
	{ KG_Cfa  , SG_GET_RGB(  0,  48,   0), "Cfa", "warm temperate, fully humid, hot summer" },
	{ KG_Cfb  , SG_GET_RGB(  1,  79,   1), "Cfb", "warm temperate, fully humid, warm summer" },
	{ KG_Cfc  , SG_GET_RGB(  0, 120,   0), "Cfc", "warm temperate, fully humid, cool summer" },
	{ KG_Csa  , SG_GET_RGB(  0, 254,   0), "Csa", "warm temperate, summer dry, hot summer" },
	{ KG_Csb  , SG_GET_RGB(149, 255,   0), "Csb", "warm temperate, summer dry, warm summer" },
	{ KG_Csc  , SG_GET_RGB(203, 255,   0), "Csc", "warm temperate, summer dry, cool summer" },
	{ KG_Cwa  , SG_GET_RGB(181, 101,   0), "Cwa", "warm temperate, winter dry, hot summer" },
	{ KG_Cwb  , SG_GET_RGB(149, 102,   3), "Cwb", "warm temperate, winter dry, warm summer" },
	{ KG_Cwc  , SG_GET_RGB( 93,  64,   2), "Cwc", "warm temperate, winter dry, cool summer" },
	{ KG_Dfa  , SG_GET_RGB( 48,   0,  48), "Dfa", "snow, fully humid, hot summer" },
	{ KG_Dfb  , SG_GET_RGB(101,   0, 101), "Dfb", "snow, fully humid, warm summer" },
	{ KG_Dfc  , SG_GET_RGB(203,   0, 203), "Dfc", "snow, fully humid, cool summer" },
	{ KG_Dfd  , SG_GET_RGB(199,  20, 135), "Dfd", "snow, fully humid, extremely continental" },
	{ KG_Dsa  , SG_GET_RGB(253, 108, 253), "Dsa", "snow, summer dry, hot summer" },
	{ KG_Dsb  , SG_GET_RGB(254, 182, 255), "Dsb", "snow, summer dry, warm summer" },
	{ KG_Dsc  , SG_GET_RGB(231, 202, 253), "Dsc", "snow, summer dry, cool summer" },
	{ KG_Dsd  , SG_GET_RGB(202, 203, 203), "Dsd", "snow, summer dry, extremely continental" },
	{ KG_Dwa  , SG_GET_RGB(203, 182, 255), "Dwa", "snow, winter dry, hot summer" },
	{ KG_Dwb  , SG_GET_RGB(153, 125, 178), "Dwb", "snow, winter dry, warm summer" },
	{ KG_Dwc  , SG_GET_RGB(138,  89, 178), "Dwc", "snow, winter dry, cool summer" },
	{ KG_Dwd  , SG_GET_RGB(109,  36, 178), "Dwd", "snow, winter dry, extremely continental" },
	{ KG_ET   , SG_GET_RGB(101, 255, 255), "ET" , "polar tundra" },
	{ KG_EF   , SG_GET_RGB(100, 150, 255), "EF" , "polar frost" },
	{ KG_Count, SG_GET_RGB(245, 245, 245), "NA" , "NA" }
};

//---------------------------------------------------------
enum EWi
{
	Wi_I_A = 1, Wi_I_F, Wi_I_T, Wi_I_S, Wi_I_D,
	Wi_II_Fa, Wi_II_Fb, Wi_II_Tw, Wi_II_Ts, Wi_II_S, Wi_II_D,
	Wi_III_F, Wi_III_Tw, Wi_III_Ts, Wi_III_S, Wi_III_D,
	Wi_IV_F, Wi_IV_T, Wi_IV_S, Wi_IV_D,
	Wi_V, Wi_VI,
	Wi_Count
};

//---------------------------------------------------------
const CClimate_Classification::TClassInfo Wi_Info[KG_Count]	=
{
	{ Wi_I_A   , SG_GET_RGB(172,   0,   0), "I A"   , "Rainforest, equatorial" },
	{ Wi_I_F   , SG_GET_RGB(225,   0,   0), "I F"   , "Rainforest, weak dry period" },
	{ Wi_I_T   , SG_GET_RGB(255,  70,  70), "I T"   , "Savannah and monsoonal Rainforest" },
	{ Wi_I_S   , SG_GET_RGB(255, 200, 179), "I S"   , "Steppe, tropical" },
	{ Wi_I_D   , SG_GET_RGB(255, 225, 179), "I D"   , "Desert, tropical" },
	{ Wi_II_Fa , SG_GET_RGB(128,  64,   0), "II Fa" , "" },
	{ Wi_II_Fb , SG_GET_RGB(196,  92,   0), "II Fb" , "" },
	{ Wi_II_Tw , SG_GET_RGB(255, 127,   0), "II Tw" , "" },
	{ Wi_II_Ts , SG_GET_RGB(255, 156,   0), "II Ts" , "" },
	{ Wi_II_S  , SG_GET_RGB(255, 225,   0), "II S"  , "" },
	{ Wi_II_D  , SG_GET_RGB(255, 255,  64), "II D"  , "" },
	{ Wi_III_F , SG_GET_RGB(  0, 192,   0), "III F" , "" },
	{ Wi_III_Tw, SG_GET_RGB(  0, 255,   0), "III Tw", "Summer green and coniferous forest, winter dry" },
	{ Wi_III_Ts, SG_GET_RGB(127, 255,   0), "III Ts", "Summer green and coniferous forest, cool etesien" },
	{ Wi_III_S , SG_GET_RGB(156, 255,   0), "III S" , "" },
	{ Wi_III_D , SG_GET_RGB(225, 255,   0), "III D" , "" },
	{ Wi_IV_F  , SG_GET_RGB(  0, 147, 147), "IV F"  , "Humid boreal forest" },
	{ Wi_IV_T  , SG_GET_RGB(  0, 200, 200), "IV T"  , "Winter dry boreal forest" },
	{ Wi_IV_S  , SG_GET_RGB(  0, 255, 255), "IV S"  , "Boreal steppe" },
	{ Wi_IV_D  , SG_GET_RGB(127, 255, 255), "IV D"  , "Boreal desert" },
	{ Wi_V     , SG_GET_RGB(172, 172, 255), "V"     , "Polar tundra" },
	{ Wi_VI    , SG_GET_RGB(  0,   0, 255), "VI"    , "Polar frost" },
	{ Wi_Count , SG_GET_RGB(245, 245, 245), "NA"    , "NA" }
};

//---------------------------------------------------------
enum ETP
{
	TP_I_1 = 1, TP_I_2, TP_I_3, TP_I_4,
	TP_II_1, TP_II_2, TP_II_3,
	TP_III_1, TP_III_2, TP_III_3, TP_III_4, TP_III_5, TP_III_6, TP_III_7, TP_III_7a, TP_III_8,
	TP_III_9, TP_III_9a, TP_III_10, TP_III_10a, TP_III_11, TP_III_12, TP_III_12a,
	TP_IV_1, TP_IV_2, TP_IV_3, TP_IV_4, TP_IV_5, TP_IV_6, TP_IV_7,
	TP_V_1, TP_V_2, TP_V_2a, TP_V_3, TP_V_4, TP_V_4a, TP_V_5,
	TP_Count
};

//---------------------------------------------------------
const CClimate_Classification::TClassInfo TP_Info[TP_Count]	=
{
	{ TP_I_1    , SG_GET_RGB(230, 250, 250), "I.1"    , "Polar ice-deserts" },
	{ TP_I_2    , SG_GET_RGB(216, 245, 250), "I.2"    , "Polar frost-debris belt" },
	{ TP_I_3    , SG_GET_RGB(185, 224, 250), "I.3"    , "Tundra" },
	{ TP_I_4    , SG_GET_RGB(156, 205, 240), "I.4"    , "Sub-polar tussock grassland and moors" },
	{ TP_II_1   , SG_GET_RGB(190, 170, 214), "II.1"   , "Oceanic humid coniferous woods" },
	{ TP_II_2   , SG_GET_RGB(215, 201, 229), "II.2"   , "Continental coniferous woods" },
	{ TP_II_3   , SG_GET_RGB(234, 225, 238), "II.3"   , "Highly continental dry coniferous woods" },
	{ TP_III_1  , SG_GET_RGB(145, 116,  90), "III.1"  , "Evergreen broad-leaved and mixed woods" },
	{ TP_III_2  , SG_GET_RGB(170, 152, 106), "III.2"  , "Oceanic deciduous broad-leaved and mixed woods" },
	{ TP_III_3  , SG_GET_RGB(193, 164, 123), "III.3"  , "Sub-oceanic deciduous broad-leaved and mixed woods" },
	{ TP_III_4  , SG_GET_RGB(210, 180, 140), "III.4"  , "Sub-continental deciduous broad-leaved and mixed woods" },
	{ TP_III_5  , SG_GET_RGB(226, 220, 177), "III.5"  , "Continental deciduous broad-leaved and mixed woods as well as wooded steppe" },
	{ TP_III_6  , SG_GET_RGB(242, 235, 220), "III.6"  , "Highly continental deciduous broad-leaved and mixed woods as well as wooded steppe" },
	{ TP_III_7  , SG_GET_RGB(233, 226, 150), "III.7"  , "Deciduous broad-leaved and mixed wood and wooded steppe favoured by warmth, but withstanding cold and aridity in winter" },
	{ TP_III_7a , SG_GET_RGB(223, 216, 140), "III.7a" , "Thermophile dry wood and wooded stepe which withstands moderate to hard winters" },
	{ TP_III_8  , SG_GET_RGB(218, 200, 100), "III.8"  , "Humid deciduous broad-leaved and mixed wood which favours warmth" },
	{ TP_III_9  , SG_GET_RGB(234, 207,  80), "III.9"  , "High grass-steppe with perennial herbs" },
	{ TP_III_9a , SG_GET_RGB(224, 197,  70), "III.9a" , "Humid steppe with mild winters" },
	{ TP_III_10 , SG_GET_RGB(244, 236,  88), "III.10" , "Short grass-, or dwarf shrub-, or thorn-steppe" },
	{ TP_III_10a, SG_GET_RGB(234, 226,  78), "III.10a", "Steppe with short grass, dwarf shrups and thorns" },
	{ TP_III_11 , SG_GET_RGB(241, 239, 112), "III.11" , "Central and East-Asian grass and dwarf shrub steppe" },
	{ TP_III_12 , SG_GET_RGB(245, 245, 200), "III.12" , "Semi-desert and desert with cold winters" },
	{ TP_III_12a, SG_GET_RGB(235, 235, 190), "III.12a", "Semi-desert and desert with mild winters" },
	{ TP_IV_1   , SG_GET_RGB(201, 138, 110), "IV.1"   , "Sub-tropical hard-leaved and coniferous wood" },
	{ TP_IV_2   , SG_GET_RGB(227, 158, 110), "IV.2"   , "Sub-tropical grass and shrub-steppe" },
	{ TP_IV_3   , SG_GET_RGB(241, 195, 143), "IV.3"   , "Sub-tropical thorn- and succulants-steppe" },
	{ TP_IV_4   , SG_GET_RGB(235, 175,  80), "IV.4"   , "Sub-tropical steppe with short grass, hard-leaved monsoon wood and wooded-steppe" },
	{ TP_IV_5   , SG_GET_RGB(255, 219, 109), "IV.5"   , "Sub-tropical semi-deserts and deserts" },
	{ TP_IV_6   , SG_GET_RGB(251, 172, 100), "IV.6"   , "Sub-tropical high-grassland" },
	{ TP_IV_7   , SG_GET_RGB(229, 157,  90), "IV.7"   , "Sub-tropical humid forests (laurel and coniferous forests)" },
	{ TP_V_1    , SG_GET_RGB( 77, 117,  77), "V.1"    , "Evergreen tropical rain forest and half deciduous transition wood" },
	{ TP_V_2    , SG_GET_RGB(117, 152,  77), "V.2"    , "Rain-green humid forest and humid grass-savannah" },
	{ TP_V_2a   , SG_GET_RGB(107, 142,  67), "V.2a"   , "Half deciduous transition wood" },
	{ TP_V_3    , SG_GET_RGB(150, 180,  80), "V.3"    , "Rain-green dry wood and dry savannah" },
	{ TP_V_4    , SG_GET_RGB(192, 211, 106), "V.4"    , "Tropical thorn-succulent wood and savannah" },
	{ TP_V_4a   , SG_GET_RGB(182, 201,  96), "V.4a"   , "Tropical dry climates with humid months in winter" },
	{ TP_V_5    , SG_GET_RGB(212, 228, 181), "V.5"    , "Tropical semi-deserts and deserts" },
	{ TP_Count  , SG_GET_RGB(245, 245, 245), "NA"     , "NA" }
};

//---------------------------------------------------------
bool CClimate_Classification::Set_Classified(CSG_Grid *pClasses, int Method)
{
	switch( Method )
	{
	default:
		return( Set_Classified(pClasses, KG_Info, KG_Count) );

	case Wissmann:
		return( Set_Classified(pClasses, Wi_Info, Wi_Count) );

	case TrollPaffen:
		return( Set_Classified(pClasses, TP_Info, TP_Count) );

	case Thornthwaite1931:
		{
			const int	Color[6]	= {
				SG_GET_RGB(255,   0,   0),
				SG_GET_RGB(255, 127,   0),
				SG_GET_RGB(255, 255,   0),
				SG_GET_RGB(  0, 255,   0),
				SG_GET_RGB(  0, 255, 255),
				SG_GET_RGB(  0,   0, 255)
			};

			const CSG_String	pName[5]	= { "Wet", "Humid", "Subhumid", "Semiarid", "Arid" };
			const CSG_String	tName[6]	= { "Tropical", "Mesothermal", "Microthermal", "Taiga", "Tundra", "Frost" };

			const int	nClasses	= 1 + 5 * 6;

			CClimate_Classification::TClassInfo Info[nClasses];

			for(int t=0; t<6; t++)
			{
				CSG_Colors	Colors(5); Colors.Set_Ramp(Color[t], Color[t]); Colors.Set_Ramp_Brighness(64, 200);

				for(int p=0; p<5; p++)
				{
					int	i	= p + 5 * t;

					Info[i].ID		= 1 + i;
					Info[i].Color	= Colors[p];
					Info[i].Name	= pName[p] + " / " + tName[t];
				}
			}

			Info[nClasses - 1].ID		= nClasses;
			Info[nClasses - 1].Color	= SG_GET_RGB(245, 245, 245);
			Info[nClasses - 1].Name		= "NA";

			return( Set_Classified(pClasses, Info, nClasses) );
		}
	}
}

//---------------------------------------------------------
bool CClimate_Classification::Set_Classified(CSG_Grid *pClasses, const TClassInfo Info[], int nClasses)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pClasses, "LUT");

	if( !pLUT || !pLUT->asTable() )
	{
		return( false );
	}

	pLUT->asTable()->Del_Records();

	for(int i=0; i<nClasses; i++)
	{
		CSG_Table_Record	*pClass	= pLUT->asTable()->Add_Record();

		pClass->Set_Value(0, Info[i].Color      );
		pClass->Set_Value(1, Info[i].Name       );
		pClass->Set_Value(2, Info[i].Description);
		pClass->Set_Value(3, Info[i].ID         );
		pClass->Set_Value(4, Info[i].ID         );
	}

	DataObject_Set_Parameter(pClasses, pLUT);
	DataObject_Set_Parameter(pClasses, "COLORS_TYPE", 1);	// Classified

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CClimate_Classification::Get_Values(int x, int y, CSG_Parameter_Grid_List *pValues, CSG_Simple_Statistics &Values)
{
	for(int i=0; i<12; i++)
	{
		if( pValues->Get_Grid(i)->is_NoData(x, y) )
		{
			return( false );
		}

		Values	+= pValues->Get_Grid(i)->asDouble(x, y);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CClimate_Classification::is_North(double T[])
{
	double	TWinter = 0.0, TSummer = 0.0;

	for(int i=0, iWinter=9, iSummer=3; i<6; i++, iWinter++, iSummer++)
	{
		TWinter	+= T[iWinter % 12];
		TSummer	+= T[iSummer % 12];
	}

	return( TSummer > TWinter );	// Northern Hemisphere ?
}

//---------------------------------------------------------
bool CClimate_Classification::Get_PSeasonal(bool bNorth, double P[], CSG_Simple_Statistics &PWinter, CSG_Simple_Statistics &PSummer)
{
	int	iWinter	= bNorth ? 9 : 3;
	int	iSummer	= bNorth ? 3 : 9;

	PWinter.Create();
	PSummer.Create();

	for(int i=0; i<6; i++, iWinter++, iSummer++)
	{
		PWinter	+= P[iWinter % 12];
		PSummer	+= P[iSummer % 12];
	}

	return( true );
}

//---------------------------------------------------------
int CClimate_Classification::Get_GrowingDegreeDays(double T[], double Tmin)
{
	int	nDays	= 0;

	CSG_Vector	TDaily;

	if( CT_Get_Daily_Splined(TDaily, T) )
	{
		for(int i=0; i<TDaily.Get_N(); i++)
		{
			if( TDaily[i] > Tmin )
			{
				nDays++;
			}
		}
	}

	return( nDays );
}

//---------------------------------------------------------
double CClimate_Classification::Get_HumidMonths(double T[], double P[])
{
	int	nDays	= 0;

	CSG_Vector	TDaily, PDaily;

	if( CT_Get_Daily_Splined(TDaily, T) && CT_Get_Daily_Splined(PDaily, P) )
	{
		for(int i=0; i<TDaily.Get_N(); i++)
		{
			if( PDaily[i] > 2 * TDaily[i] )
			{
				nDays++;
			}
		}
	}

	return( nDays * 12. / 365. );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClimate_Classification::Get_KoppenGeiger(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P)
{
	//-----------------------------------------------------
	// E polar

	if( T.Get_Maximum() <  0 )
	{
		return( KG_EF );
	}

	if( T.Get_Maximum() < 10 )
	{
		return( KG_ET );
	}

	//-----------------------------------------------------
	// seasonal rain statistics

	CSG_Simple_Statistics	PWinter, PSummer;

	Get_PSeasonal(is_North(T.Get_Values()), P.Get_Values(), PWinter, PSummer);

	//-----------------------------------------------------
	// B arid

	if( PWinter.Get_Sum() >= 0.7 * P.Get_Sum() )
	{
		double	t	= T.Get_Mean() +  0;	// winter rain season

		if( P.Get_Sum() < 10 * t )
		{
			return( T.Get_Mean() < 18 ? KG_BWk : KG_BWh );	// desert
		}

		if( P.Get_Sum() < 20 * t )
		{
			return( T.Get_Mean() < 18 ? KG_BSk : KG_BSh );	// steppe
		}
	}
	else if( PSummer.Get_Sum() >= 0.7 * P.Get_Sum() )
	{
		double	t	= T.Get_Mean() + 14;	// summer rain season

		if( P.Get_Sum() < 10 * t )
		{
			return( T.Get_Mean() < 18 ? KG_BWk : KG_BWh );	// desert
		}

		if( P.Get_Sum() < 20 * t )
		{
			return( T.Get_Mean() < 18 ? KG_BSk : KG_BSh );	// steppe
		}
	}
	else
	{
		double	t	= T.Get_Mean() +  7;	// no rain season

		if( P.Get_Sum() < 10 * t )
		{
			return( T.Get_Mean() < 18 ? KG_BWk : KG_BWh );	// desert
		}

		if( P.Get_Sum() < 20 * t )
		{
			return( T.Get_Mean() < 18 ? KG_BSk : KG_BSh );	// steppe
		}
	}

	//-----------------------------------------------------
	// A equatorial

	if( T.Get_Minimum() >= 18 )
	{
		if( P.Get_Minimum() >= 60 )
		{
			return( KG_Af );
		}

		if( P.Get_Sum() >= 25 * (100 - P.Get_Minimum()) )
		{
			return( KG_Am );
		}

		if( Method == Peel2007 || Method == Koeppen1936_No_As || PWinter.Get_Minimum() < 60 )
		{
			return( KG_Aw );
		}

	//	if( PSummer.Get_Minimum() < 60 )
		{
			return( KG_As );
		}
	}

	//-----------------------------------------------------
	// dry season

	bool	bDryWinter	= PSummer.Get_Maximum() >= 10 * PWinter.Get_Minimum();
	bool	bDrySummer	= PWinter.Get_Maximum() >=  3 * PSummer.Get_Minimum()
					&&    PSummer.Get_Minimum() < (Method == Peel2007 ? 40 : 30);

	if( bDryWinter && bDrySummer )
	{
		if( PWinter.Get_Sum() < PSummer.Get_Sum() )
		{
			bDrySummer	= false;
		}
		else
		{
			bDryWinter	= false;
		}
	}

	//-----------------------------------------------------
	// C warm temperate

	if( T.Get_Minimum() >= (Method == Peel2007 ? 0 : -3) )
	{
		if( bDryWinter )
		{
			return( T.Get_Maximum() > 22 ? KG_Cwa : T.Get_nValues_Above(10) >= 4 ? KG_Cwb : KG_Cwc );
		}
		else if( bDrySummer )
		{
			return( T.Get_Maximum() > 22 ? KG_Csa : T.Get_nValues_Above(10) >= 4 ? KG_Csb : KG_Csc );
		}
		else	// without dry season
		{
			return( T.Get_Maximum() > 22 ? KG_Cfa : T.Get_nValues_Above(10) >= 4 ? KG_Cfb : KG_Cfc );
		}
	}

	//-----------------------------------------------------
	// D snow

//	if( T.Get_Minimum() <  (Method == Peel2007 ? 0 : -3) )
	{
		if( bDryWinter )
		{
			return( T.Get_Maximum() > 22 ? KG_Dwa : T.Get_nValues_Above(10) >= 4 ? KG_Dwb : T.Get_Minimum() > -38 ? KG_Dwc : KG_Dwd );
		}
		else if( bDrySummer )
		{
			return( T.Get_Maximum() > 22 ? KG_Dsa : T.Get_nValues_Above(10) >= 4 ? KG_Dsb : T.Get_Minimum() > -38 ? KG_Dsc : KG_Dsd );
		}
		else	// without dry season
		{
			return( T.Get_Maximum() > 22 ? KG_Dfa : T.Get_nValues_Above(10) >= 4 ? KG_Dfb : T.Get_Minimum() > -38 ? KG_Dfc : KG_Dfd );
		}
	}

	//-----------------------------------------------------
	return( KG_Count );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClimate_Classification::Get_Wissmann(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P)
{
	//-----------------------------------------------------
	if( T.Get_Maximum() < 0 )	// VI - polar frost
	{
		return( Wi_VI );
	}

	//-----------------------------------------------------
	if( T.Get_Maximum() < 10 )	// V - polar tundra
	{
		return( Wi_V );
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics	PWinter, PSummer;

	Get_PSeasonal(is_North(T.Get_Values()), P.Get_Values(), PWinter, PSummer);

	double	t	= 10 * (PWinter.Get_Sum() > PSummer.Get_Sum() ? T.Get_Mean() : T.Get_Mean() + 14.0);

	//-----------------------------------------------------
	if( T.Get_Mean() < 4 )		// IV - boreal
	{
		if( P.Get_Sum() > 2.5 * t )
		{
			return( Wi_IV_F );
		}

		if( P.Get_Sum() > 2.0 * t )
		{
			return( Wi_IV_T );
		}

		if( P.Get_Sum() > 1.0 * t )
		{
			return( Wi_IV_S );
		}

	//	if( P.Get_Sum() <= 1.0 * t )
		{
			return( Wi_IV_D );
		}
	}

	//-----------------------------------------------------
	if( T.Get_Minimum() < 2 )	// III - cool temperate
	{
		if( P.Get_Sum() > 2.5 * t )
		{
			return( Wi_III_F );
		}

		if( P.Get_Sum() > 2.0 * t )
		{
			return( PWinter.Get_Sum() < PSummer.Get_Sum() ? Wi_III_Tw : Wi_III_Ts );
		}

		if( P.Get_Sum() > 1.0 * t )
		{
			return( Wi_III_S );
		}

	//	if( P.Get_Sum() <= 1.0 * t )
		{
			return( Wi_III_D );
		}
	}

	//-----------------------------------------------------
	if( T.Get_Minimum() < 13 )	// II - warm temperate
	{
		if( P.Get_Sum() > 2.5 * t )
		{
			return( T.Get_Maximum() > 23 ? Wi_II_Fa : Wi_II_Fb );
		}

		if( P.Get_Sum() > 2.0 * t )
		{
			return( PWinter.Get_Sum() < PSummer.Get_Sum() ? Wi_II_Tw : Wi_II_Ts );
		}

		if( P.Get_Sum() > 1.0 * t )
		{
			return( Wi_I_S );
		}

	//	if( P.Get_Sum() <= 1.0 * t )
		{
			return( Wi_I_D );
		}
	}

	//-----------------------------------------------------
	if( T.Get_Minimum() >= 13 )	// I - tropical
	{
		if( P.Get_Minimum() >= 60 )	// perhumid, no dry season
		{
			return( Wi_I_A );
		}

		if( P.Get_Sum() > 2.5 * t )
		{
			return( Wi_I_F );
		}

		if( P.Get_Sum() > 2.0 * t )
		{
			return( Wi_I_T );
		}

		if( P.Get_Sum() > 1.0 * t )
		{
			return( Wi_I_S );
		}

	//	if( P.Get_Sum() <= 1.0 * t )
		{
			return( Wi_I_D );
		}
	}

	//-----------------------------------------------------
	return( Wi_Count );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define is_Between(x, min, max)	((min) <= (x) && (x) <= (max))

//---------------------------------------------------------
int CClimate_Classification::Get_TrollPaffen(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P)
{
	//-----------------------------------------------------
	// I. Polar and Subpolar Zones

	if( T.Get_Maximum() < 0 )
	{
		return( TP_I_1 );
	}

	if( T.Get_Maximum() < 6 )
	{
		return( TP_I_2 );
	}

	if( T.Get_Maximum() < 12 && T.Get_Minimum() <  -8 )
	{
		return( TP_I_3 );
	}

	if( T.Get_Maximum() < 12 && T.Get_Minimum() >= -8 && T.Get_Range() < 13 )	// annual fluctuation often < 10°C
	{
		return( TP_I_4 );
	}

	//-----------------------------------------------------
	int	nVegDays	= Get_GrowingDegreeDays(T.Get_Values(), 5.0);

	//-----------------------------------------------------
	// II. Cold-temperate Boreal Zone

	if( T.Get_Maximum() < 15 && T.Get_Minimum() >= -3 && T.Get_Minimum() < 2 && is_Between(nVegDays, 120, 180) )
	{
		return( TP_II_1 );
	}

	if( T.Get_Maximum() < 20 && T.Get_Range() < 40 && is_Between(nVegDays, 100, 150) )
	{
		return( TP_II_2 );
	}

	if( T.Get_Maximum() < 20 && T.Get_Minimum() < 25 && T.Get_Range() >= 40 )
	{
		return( TP_II_3 );
	}

	//-----------------------------------------------------
	// III. Cool-temperate Zones

	if( T.Get_Minimum() >= -3 && T.Get_Minimum() < 2 && nVegDays >= 200 )
	{
		return( TP_III_3 );
	}

	if( T.Get_Maximum() < 15 )
	{
		if( is_Between(T.Get_Minimum(), 2, 10) && T.Get_Range() < 10 )
		{
			return( TP_III_1 );
		}
	}
	else if( T.Get_Maximum() < 20 )
	{
		if( T.Get_Minimum() >= 2 && T.Get_Range() < 16 )
		{
			return( TP_III_2 );
		}

		if( is_Between(T.Get_Range(), 20, 30) && is_Between(nVegDays, 160, 210) )
		{
			return( TP_III_4 );
		}

		if( is_Between(T.Get_Minimum(), -20, -10) && is_Between(T.Get_Range(), 30, 40) && is_Between(nVegDays, 150, 180) )
		{
			return( TP_III_5 );
		}
	}
	else // if( T.Get_Maximum() >= 20 )
	{
		if( is_Between(T.Get_Minimum(), -30, -10) && T.Get_Range() > 40 )
		{
			return( TP_III_6 );
		}

		if( T.Get_Maximum() < 26 )
		{
			if( is_Between(T.Get_Minimum(), 0, 8) && SG_Is_Between(T.Get_Range(), 25, 35) )
			{
				return( TP_III_7 );
			}

			if( is_Between(T.Get_Minimum(), -6, 2) )
			{
				return( TP_III_7a );
			}

			if( is_Between(T.Get_Minimum(), -6, 2) && SG_Is_Between(T.Get_Range(), 20, 30) )
			{
				return( TP_III_8 );
			}
		}
	}

	//-----------------------------------------------------
	double	nHumid	= Get_HumidMonths(T.Get_Values(), P.Get_Values());

	bool	bNorth	= is_North(T.Get_Values());

	CSG_Simple_Statistics	PWinter, PSummer;	Get_PSeasonal(bNorth, P.Get_Values(), PWinter, PSummer);

	//-----------------------------------------------------
	// III. Steppe Climates

	if( T.Get_Minimum() < 0 )
	{
		if( nHumid >= 6 )
		{
			return( TP_III_9 );
		}
		else if( PWinter.Get_Sum() < PSummer.Get_Sum() )
		{
			return( TP_III_3 );
		}
		else
		{
			return( TP_III_10 );
		}
	}
	else if( T.Get_Minimum() < 6 )
	{
		return( TP_III_12a );
	}

	//-----------------------------------------------------
	// IV. Warm-temperate Sub-tropical Zones

	if( is_Between(T.Get_Minimum(), bNorth ? 2 : 6, 13) )
	{
		if( nHumid <= 2 )
		{
			return( TP_IV_5 );
		}

		if( nHumid >= 10 )
		{
			if( !bNorth )
			{
				return( TP_IV_6 );
			}

			return( TP_IV_7 );
		}

		if( nHumid >= 6 )
		{
			return( TP_IV_4 );
		}

		if( nHumid >= 5 )
		{
			return( TP_IV_1 );
		}

	//	if( nHumid < 5 )
		{
			if( PWinter.Get_Sum() < PSummer.Get_Sum() )
			{
				return( TP_IV_2 );
			}

			return( TP_IV_3 );
		}
	}

	//-----------------------------------------------------
	// V. Tropical Zone

	if( T.Get_Minimum() > 0 && T.Get_Mean() > 18.3 )
	{
		if( nHumid > 9.5 )
		{
			return( TP_V_1 );
		}

		if( nHumid > 7 )
		{
			if( PWinter.Get_Sum() < PSummer.Get_Sum() )
			{
				return( TP_V_2a );
			}

			return( TP_V_2 );
		}

		if( nHumid > 4.5 )
		{
			return( TP_V_3 );
		}

		if( nHumid > 2 )
		{
			if( PWinter.Get_Sum() < PSummer.Get_Sum() )
			{
				return( TP_V_4a );
			}

			return( TP_V_4 );
		}

	//	if( nHumid <= 2 )
		{
			return( TP_V_5 );
		}
	}

	//-----------------------------------------------------
	return( TP_Count );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClimate_Classification::Get_Thornthwaite(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P)
{
	double	PE	= 0.0, TE	= 0.0;

	for(int i=0; i<12; i++)
	{
		double	t	= T[i];
		double	p	= P[i];

		PE	+= 1.65 * pow(p / ((t > 0 ? t : 0) + 12.2), 10./9.);

		if( t > 0.0 )
		{
			TE	+= t * 1.8 / 4;
		}

		//double	t	= T[i] * 1.8 + 32;	// convert to Fahrenheit
		//double	p	= P[i] / 2.54;		// convert to inch

		//PE	+= 11.5 * pow(p / (t > 11 ? t - 10 : 1), 10./9.);

		//if( t - 32 > 0.0 )
		//{
		//	TE	+= (t - 32) / 4;
		//}
	}

	//-----------------------------------------------------
	int	p	= PE >= 128 ? 0
			: PE >=  64 ? 1
			: PE >=  32 ? 2
			: PE >=  16 ? 3 : 4;

	int	t	= TE >= 128 ? 0
			: TE >=  64 ? 1
			: TE >=  32 ? 2
			: TE >=  16 ? 3
			: TE >    0 ? 4 : 5;

	return( 1 + p + 5 * t );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
