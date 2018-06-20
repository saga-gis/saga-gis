/**********************************************************
 * Version $Id: climate_classification.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "climate_classification.h"


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
	Peel2007
};

//---------------------------------------------------------
enum EKG
{
	KG_Af = 1, KG_Am, KG_As, KG_Aw,
	KG_BWk, KG_BWh, KG_BSk, KG_BSh,
	KG_Cfa, KG_Cfb, KG_Cfc, KG_Csa, KG_Csb, KG_Csc, KG_Cwa, KG_Cwb, KG_Cwc,
	KG_Dfa, KG_Dfb, KG_Dfc, KG_Dfd, KG_Dsa, KG_Dsb, KG_Dsc, KG_Dsd, KG_Dwa, KG_Dwb, KG_Dwc, KG_Dwd,
	KG_ET, KG_EF, KG_Count = KG_EF
};

//---------------------------------------------------------
const CClimate_Classification::TClassInfo KG_Info[KG_Count]	=
{
	{ KG_Af , SG_GET_RGB(148,   1,   1), "Af" , "equatorial, fully humid" },
	{ KG_Am , SG_GET_RGB(254,   0,   0), "Am" , "equatorial, monsoonal" },
	{ KG_As , SG_GET_RGB(255, 154, 154), "As" , "equatorial, summer dry" },
	{ KG_Aw , SG_GET_RGB(255, 207, 207), "Aw" , "equatorial, winter dry" },
	{ KG_BWk, SG_GET_RGB(255, 255, 101), "BWk", "cold desert" },
	{ KG_BWh, SG_GET_RGB(255, 207,   0), "BWh", "hot desert" },
	{ KG_BSk, SG_GET_RGB(207, 170,  85), "BSk", "cold steppe" },
	{ KG_BSh, SG_GET_RGB(207, 142,  20), "BSh", "hot steppe" },
	{ KG_Cfa, SG_GET_RGB(  0,  48,   0), "Cfa", "warm temperate, fully humid, hot summer" },
	{ KG_Cfb, SG_GET_RGB(  1,  79,   1), "Cfb", "warm temperate, fully humid, warm summer" },
	{ KG_Cfc, SG_GET_RGB(  0, 120,   0), "Cfc", "warm temperate, fully humid, cool summer" },
	{ KG_Csa, SG_GET_RGB(  0, 254,   0), "Csa", "warm temperate, summer dry, hot summer" },
	{ KG_Csb, SG_GET_RGB(149, 255,   0), "Csb", "warm temperate, summer dry, warm summer" },
	{ KG_Csc, SG_GET_RGB(203, 255,   0), "Csc", "warm temperate, summer dry, cool summer" },
	{ KG_Cwa, SG_GET_RGB(181, 101,   0), "Cwa", "warm temperate, winter dry, hot summer" },
	{ KG_Cwb, SG_GET_RGB(149, 102,   3), "Cwb", "warm temperate, winter dry, warm summer" },
	{ KG_Cwc, SG_GET_RGB( 93,  64,   2), "Cwc", "warm temperate, winter dry, cool summer" },
	{ KG_Dfa, SG_GET_RGB( 48,   0,  48), "Dfa", "snow, fully humid, hot summer" },
	{ KG_Dfb, SG_GET_RGB(101,   0, 101), "Dfb", "snow, fully humid, warm summer" },
	{ KG_Dfc, SG_GET_RGB(203,   0, 203), "Dfc", "snow, fully humid, cool summer" },
	{ KG_Dfd, SG_GET_RGB(199,  20, 135), "Dfd", "snow, fully humid, extremely continental" },
	{ KG_Dsa, SG_GET_RGB(253, 108, 253), "Dsa", "snow, summer dry, hot summer" },
	{ KG_Dsb, SG_GET_RGB(254, 182, 255), "Dsb", "snow, summer dry, warm summer" },
	{ KG_Dsc, SG_GET_RGB(231, 202, 253), "Dsc", "snow, summer dry, cool summer" },
	{ KG_Dsd, SG_GET_RGB(202, 203, 203), "Dsd", "snow, summer dry, extremely continental" },
	{ KG_Dwa, SG_GET_RGB(203, 182, 255), "Dwa", "snow, winter dry, hot summer" },
	{ KG_Dwb, SG_GET_RGB(153, 125, 178), "Dwb", "snow, winter dry, warm summer" },
	{ KG_Dwc, SG_GET_RGB(138,  89, 178), "Dwc", "snow, winter dry, cool summer" },
	{ KG_Dwd, SG_GET_RGB(109,  36, 178), "Dwd", "snow, winter dry, extremely continental" },
	{ KG_ET , SG_GET_RGB(101, 255, 255), "ET" , "polar tundra" },
	{ KG_EF , SG_GET_RGB(100, 150, 255), "EF" , "polar frost" }
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
		"This tool applies the Koeppen-Geiger climate classification "
		"scheme to monthly mean temperature and precipitation data. "
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

	Add_Reference("Willmes, C., Becker, D., Brocks, S., Huett, C., Bareth, G.", "2016",
		"High resolution Koeppen-Geiger classifications of paleo-climate simulations",
		"Trans. in GIS, DOI:10.1111/tgis.12187.",
		SG_T("https://onlinelibrary.wiley.com/doi/abs/10.1111/tgis.12187"), SG_T("Wiley Online")
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
		CSG_String::Format("%s|%s|%s",
			SG_T("Koeppen-Geiger"),
			SG_T("Koeppen-Geiger without As/Aw differentiation"),
			SG_T("Koeppen-Geiger (after Peel et al. 2007)")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClimate_Classification::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
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
	CSG_Grid	*pClasses	= Parameters("CLASSES")->asGrid();

	Set_Classified(pClasses, KG_Info, KG_Count);

	int	Method	= Parameters("METHOD")->asInt();

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
				Class	= Get_KoppenGeiger(Method, T, P);
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
	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
