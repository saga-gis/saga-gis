
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 DVWK_SoilMoisture.cpp                 //
//                                                       //
//                 Copyright (C) 2002 by                 //
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
#include "DVWK_SoilMoisture.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDVWK_SoilMoisture::CDVWK_SoilMoisture(void)
{
	Set_Name		(_TL("Top Soil Water Content"));

	Set_Author		("O.Conrad (c) 2002");

	Set_Description	(_TW(
		"The WEELS (Wind Erosion on European Light Soils) soil moisture model "
		"estimates the top soil water content on a daily basis using the "
		"Haude approach for evapotranspiration (cf. DVWK 1996). Input data is<ul>"
		"<li>daily weather<ul>"
		"  <li>precipitation</li>"
		"  <li>temperature</li>"
		"  <li>air humidity</li>"
		"  </ul></li>"
		"<li>soil properties<ul>"
		"  <li>field capacity</li>"
		"  <li>permanent wilting point</li>"
		"  </ul></li>"
		"<li>crop type</li>"
		"</ul>"
	));

	Add_Reference("DVWK (Deutscher Verband fuer Wasserwirtschaft und Kulturbau)", "1996",
		"Determination of evaporation and transpiration from land and water surfaces",
		"DVWK Leaflet Series for Water Management no.238, Bonn, 135p.",
		SG_T("https://webshop.dwa.de/de/merkblatt-dvwk-m-238-1996.html"), SG_T("DVWK-M 238")
	);

	Add_Reference("Boehner, J., Schaefer, W., Conrad, O., Gross, J., Ringeler, A.", "2001",
		"The WEELS Model: methods, results and limits of wind erosion modelling",
		"In: Catena, Special Issue.",
		SG_T("https://doi.org/10.1016/S0341-8162(03)00019-5"), SG_T("doi:10.1016/S0341-8162(03)00019-5")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"STA_FC"		, _TL("Field Capacity"),
		_TL("[mm]"),
		20., 0., true, 100., true
	);

	Parameters.Add_Grid_or_Const("",
		"STA_PWP"		, _TL("Permanent Wilting Point"),
		_TL("[mm]"),
		2., 0., true, 100., true
	);

	Parameters.Add_Grid("",
		"LANDUSE"		, _TL("Land Use"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Int("LANDUSE",
		"LANDUSE_DEF"	, _TL("Default"),
		_TL(""),
		-1
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DYN_W"		, _TL("Soil Moisture"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	m_pClimate   = Parameters.Add_FixedTable("",
		"DYN_CLIMATE"	, _TL("Climate Data"),
		_TL("")
	)->asTable();

	m_pClimate->Set_Name (_TL("Climate Data"         ));
	m_pClimate->Add_Field(_TL("Precipitation [mm]"   ), SG_DATATYPE_Double);
	m_pClimate->Add_Field(_TL("Temperature [2pm °C]" ), SG_DATATYPE_Double);
	m_pClimate->Add_Field(_TL("Air Humidity [2pm [%]"), SG_DATATYPE_Double);

	//-----------------------------------------------------
	m_pCropCoeff = Parameters.Add_FixedTable("",
		"STA_KC"		, _TL("Crop Coefficients"),
		_TL("")
	)->asTable();

	m_pCropCoeff->Set_Name(_TL("Crop Coefficients"));
	m_pCropCoeff->Add_Field(_TL("Land Use ID"), SG_DATATYPE_Int   );
	m_pCropCoeff->Add_Field(_TL("Name"       ), SG_DATATYPE_String);
	m_pCropCoeff->Add_Field(_TL("January"    ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("February"   ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("March"      ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("April"      ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("May"        ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("June"       ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("July"       ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("August"     ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("September"  ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("October"    ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("November"   ), SG_DATATYPE_Double);
	m_pCropCoeff->Add_Field(_TL("December"   ), SG_DATATYPE_Double);

	#define ADD_RECORD(ID, NAME, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12) { CSG_Table_Record *pRec = m_pCropCoeff->Add_Record();\
		pRec->Set_Value( 0, ID);  pRec->Set_Value( 1, NAME);\
		pRec->Set_Value( 2, m01); pRec->Set_Value( 3, m02); pRec->Set_Value( 4, m03); pRec->Set_Value( 5, m04);\
		pRec->Set_Value( 6, m05); pRec->Set_Value( 7, m06); pRec->Set_Value( 8, m07); pRec->Set_Value( 9, m08);\
		pRec->Set_Value(10, m09); pRec->Set_Value(11, m10); pRec->Set_Value(12, m11); pRec->Set_Value(13, m12);\
	}

	//         ID       NAME            Jan   Feb   Mar   Apr   Mai   Jun   Jul   Aug   Sep   Okt   Nov   Dec
	ADD_RECORD(1., _TL("Maehweide"   ), 1   , 1   , 1   , 1   , 1.05, 1.10, 1.10, 1.05, 1.05, 1   , 1   , 1   );
	ADD_RECORD(2., _TL("Winterweizen"), 0.65, 0.65, 0.80, 0.85, 1.15, 1.45, 1.40, 1   , 0.80, 0.70, 0.65, 0.65);
	ADD_RECORD(3., _TL("Wintergerste"), 1   , 1   , 0.85, 0.95, 1.30, 1.35, 1.25, 1   , 1   , 1   , 1   , 1   );
	ADD_RECORD(4., _TL("Sommergerste"), 1   , 1   , 0.80, 0.90, 1.20, 1.35, 1.20, 1   , 1   , 1   , 1   , 1   );
	ADD_RECORD(5., _TL("Winterroggen"), 0.65, 0.65, 0.85, 0.90, 1.20, 1.30, 1.25, 0.95, 0.80, 0.70, 0.65, 0.65);
	ADD_RECORD(6., _TL("Hafer"       ), 1   , 1   , 0.65, 0.70, 1.10, 1.45, 1.35, 0.95, 1   , 1   , 1   , 1   );
	ADD_RECORD(7., _TL("Zuckerrueben"), 1   , 1   , 1   , 0.50, 0.75, 1.05, 1.40, 1.30, 1.10, 0.85, 1   , 1   );
	ADD_RECORD(8., _TL("Kartoffeln"  ), 1   , 1   , 1   , 0.50, 0.90, 1.05, 1.45, 1.20, 0.90, 1   , 1   , 1   );
	ADD_RECORD(9., _TL("Winterraps"  ), 0.65, 0.65, 0.85, 1   , 1.35, 1.35, 1.10, 0.85, 1   , 1   , 0.65, 0.65);
	ADD_RECORD(0., _TL("Unknown"     ), 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   );

	#undef ADD_RECORD
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDVWK_SoilMoisture::On_Execute(void)
{
	if( m_pClimate->Get_Record_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pFK	= Parameters("STA_FC" )->asGrid  ();
	m_FK	= Parameters("STA_FC" )->asDouble();

	m_pPWP	= Parameters("STA_PWP")->asGrid  ();
	m_PWP	= Parameters("STA_PWP")->asDouble();

	m_pWi	= Parameters("DYN_W"  )->asGrid  ();
	DataObject_Set_Colors(m_pWi, 11, SG_COLORS_YELLOW_BLUE);

	//-----------------------------------------------------
	m_LandUse.Create(m_pWi, m_pCropCoeff->Get_Record_Count() < 127 ? SG_DATATYPE_Char : SG_DATATYPE_Int);
	m_LandUse.Assign(Parameters("LANDUSE_DEF")->asDouble());

	CSG_Grid	*pLandUse	= Parameters("LANDUSE")->asGrid();

	if( pLandUse != NULL )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			int	LandUseID	= pLandUse->asInt(x, y);

			for(int i=0; i<m_pCropCoeff->Get_Record_Count(); i++)
			{
				if( LandUseID == m_pCropCoeff->Get_Record(i)->asInt(0) )
				{
					m_LandUse.Set_Value(x, y, i);

					break;
				}
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pWi, 0, m_pFK ? m_pFK->Get_Max() : m_FK, true);

	for(int Day=0; Day<365 && Set_Progress(Day, 365); Day++)
	{
		Step_Day(Day);

		DataObject_Update(m_pWi, true);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDVWK_SoilMoisture::Get_Month(int Day)
{
	const int	Days[]	=
	//	Jan Feb Mar Apr Mai Jun Jul Aug Sep Okt Nov Dez
	{	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31	};

	int		iMonth, nDays;

	Day		%= 365;

	if( Day < 0 )
	{
		Day 	+= 365;
	}

	for(iMonth=0, nDays=0; iMonth<12; iMonth++)
	{
		nDays	+= Days[iMonth];

		if( Day < nDays )
		{
			return( iMonth + 1 );
		}
	}

	return( 12 );
}

//---------------------------------------------------------
// VKR 4.10:	Bestandeskoeffizient z.Ber.d.pot.Verdunstung d.Pflbest. (kc)
//
//		Bestand		: Typ (als ENUM) des Pflanzenbestandes
//		Month		: 1=Jan, 2=Feb... 0=NA;
//
double CDVWK_SoilMoisture::Get_kc(int Bestand, int Day)
{
	if( Bestand >= 0 && Bestand < m_pCropCoeff->Get_Record_Count() )
	{
		return( m_pCropCoeff->Get_Record(Bestand)->asDouble(1 + Get_Month(Day)) );
	}

	return( 1.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CDVWK_SoilMoisture::Get_ETP_Haude(int Day)
{
	const double	f[]	=
	//	---   Jan   Feb   Mar   Apr   Mai   Jun   Jul   Aug   Sep   Okt   Nov   Dez
	{	0.00, 0.22, 0.22, 0.22, 0.29, 0.29, 0.28, 0.26, 0.25, 0.23, 0.22, 0.22, 0.22	};

	double	T14, U14, es, e, ETP_Haude;

	//-----------------------------------------------------
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pClimate->Get_Record(Day)) == NULL )
	{
		return( 0 );
	}

	T14		= pRecord->asDouble(1);
	U14		= pRecord->asDouble(2);


	//-----------------------------------------------------
	// VKR 4.6:		Berechnung des Saettigungsdampfdruckes (es [hPa])
	//
	//					               17,62 * T14
	//		es(T14)		= 6,11 * exp( -------------- );		bei T14 > 0
	//					               243,12 + T14
	//
	//					               22,46 * T14
	//		es(T14)		= 6,11 * exp( -------------- );		bei T14 < 0
	//					               272,62 + T14
	//
	//		T14			: Temperatur 14 Uhr [DegreeC]
	//

	if( T14 < 0 )
	{
		es	= 6.11 * exp( (22.46 * T14) / (272.62 + T14) );
	}
	else
	{
		es	= 6.11 * exp( (17.62 * T14) / (243.12 + T14) );
	}


	//-----------------------------------------------------
	// VKR 4.7:		Berechnung des aktuellen Dampfdruckes (e [hPa])
	//
	//		e		= es(T14) * U14 / 100
	//
	//		es(T14)		: Saettigungsdampfdruck 14 Uhr [hPa]
	//		U14			: relative Luftfeuchte 14 Uhr [%] (Dampfdruck in % des Saettigungsdampfdruckes)
	//

	e	= es * U14 / 100.0;


	//-----------------------------------------------------
	// VKR 4.8:		Berechnung der potentiellen Verdunstung nach Haude (ETPhaude [mm/d])
	//
	//		ETPhaude	= f(m) * (es - e)
	//
	//		f(m)		: Monatsfaktor [mm/mbar/d]
	//		e			: aktueller Dampfdruck [hPa]
	//		es			: Saettigungsdampfdruck
	//

	ETP_Haude	= f[Get_Month(Day)] * (es - e);

	return( ETP_Haude );
}

//---------------------------------------------------------
double CDVWK_SoilMoisture::Get_Pi(int Day)
{
	const double Messfehler[]	=
	//	---    Jan    Feb    Mar    Apr    Mai    Jun    Jul    Aug    Sep    Okt    Nov    Dez
	{	0.000, 0.228, 0.236, 0.200, 0.160, 0.120, 0.103, 0.105, 0.103, 0.115, 0.136, 0.162, 0.189	};

	double	P, Pi;

	//-----------------------------------------------------
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pClimate->Get_Record(Day)) == NULL )
	{
		return( 0 );
	}

	P		= pRecord->asDouble(0);


	//-----------------------------------------------------
	// VKR 4.11:	Berechnung der korrigierten Niederschlaege (Pi [mm/d])
	//
	//		Pi	= N * kn
	//
	//		N		: Niederschlag gemessen [mm/d]
	//		kn		: Korrekturfaktor ergibt sich aus den prozentualen Meßfehlern...
	//		Month	: 1=Jan, 2=Feb... 0=NA;
	//

	Pi			= P + P * Messfehler[Get_Month(Day)];

	return( Pi );
}

//---------------------------------------------------------
double CDVWK_SoilMoisture::Get_Wi(double Wi, double Pi, double ETP, double kc, double FK, double PWP)
{
	double	Ri, d, ETPi, ETAi;


	//-----------------------------------------------------
	// VKR 4.9:		Berechnung des Reduktionsfaktors f.Verdunstung (Ri)
	//
	//			        PWP
	//			   1 - -----
	//			        Wi               ETP		Ri = 1	bei Pi > ETPi
	//		Ri	= ------------- + 0,1 * ------;		Ri = 1	bei Ri > 1,0
	//			        PWP              ETPi		Ri = 0	bei Ri < 0,0
	//			   1 - -----
	//			        FK
	//
	//		Pi			: korrigierter Niederschlag [mm/d]
	//		Wi			: Bodenwassergehalt
	//		PWP			: Permanenter Welkepunkt
	//		FK			: Feldkapazitaet
	//		kc			: Koeffizient f.d. Pflz.Bestand
	//

	ETPi	= ETP * kc;

	if( Pi > ETPi )
	{
		Ri	= 1.0;
	}
	else if( ETPi <= 0.0 || Wi <= 0.0 || FK <= 0.0 )
	{
		Ri	= 0.0;
	}
	else
	{
		d	= 1.0 - PWP / FK;

		if( d == 0.0 )
		{
			Ri	= 0.0;
		}
		else
		{
			Ri	= (1.0 - PWP / Wi) / d + 0.1 * ETP / ETPi;

			if( Ri > 1.0 )
			{
				Ri	= 1.0;
			}
			else if( Ri < 0.0 )
			{
				Ri	= 0.0;
			}
		}
	}


	//-----------------------------------------------------
	// VKR 4.13:	Berechnung der tatsaechlichen Verdunstung (ETa [mm/d])
	//
	//		ETai	= Pi - (Pi - (ETPhaude * kc)) * Ri
	//
	//		Pi			: korrigierter Niederschlag [mm/d]
	//		ETPhaude	: potentielle Verdunstung nach Haude [mm/d]
	//		kc			: Bestandeskoeffizient
	//		Ri			: Reduktionsfaktor
	//

	ETAi	= Pi - (Pi - ETPi) * Ri;


	//-----------------------------------------------------
	// VKR 4.14:	Berechnung des Bodenwasservorrats (Wi [mm/d])
	//
	//		Wi+1	= Wi + Pi - ETai - SRi + KR
	//
	//		Pi		: korrigierter Niederschlag [mm/d]
	//		ETai	: tatsaechliche Verdunstung [mm/d]
	//		SRi		: taegliche Sickerwaserrate [mm/d]
	//					SRi	= 0					bei Wi+1 <= FK * ku
	//					SRi	= Wi+1 - FK * ku	bei Wi+1 >  FK * ku;
	//		FK		: Feldkapazitaet [mm]
	//		ku		: moegliche Uebersaettigung ueber FK	:= 1 (findet keine Beruecksichtigung)
	//		KR		: kapillare Aufstiegsrate			:= 0 (findet keine Beruecksichtigung)
	//

	Wi		+= Pi - ETAi;

	if( Wi > FK )
	{
		Wi	= FK;	// -= SRi...
	}
	else if( Wi < PWP )
	{
		Wi	= PWP;
	}


	return( Wi );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDVWK_SoilMoisture::Step_Day(int Day)
{
	double	ETP	= Get_ETP_Haude(Day);
	double	Pi	= Get_Pi       (Day);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		double	kc	= Get_kc(m_LandUse.asInt(x, y), Day);

		double	FK	= m_pFK  ? m_pFK ->asDouble(x, y) : m_FK ;
		double	PWP	= m_pPWP ? m_pPWP->asDouble(x, y) : m_PWP;

		m_pWi->Set_Value(x, y,
			Get_Wi(m_pWi->asDouble(x, y), Pi, ETP, kc, FK, PWP)
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
