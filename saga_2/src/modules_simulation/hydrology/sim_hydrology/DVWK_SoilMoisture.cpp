
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
#include "DVWK_SoilMoisture.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDVWK_SoilMoisture::CDVWK_SoilMoisture(void)
{
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name	(_TL("DVWK - Distributed Soil Moisture Model"));

	Set_Author	(_TL("Copyrights (c) 2002 by Olaf Conrad"));

	Set_Description(_TL(
		"The WEELS (Wind Erosion on European Light Soils) distributed soil moisture "
		"model dynamically calculates the soil moisture based on data about:\n"
		"- soil properties (grids: field capacity and permanent wilting point)\n"
		"- land use (grid: crop types)\n"
		"- climate (table: daily values of precipitation, temperature, air humidity)\n\n"

		"References:\n"

		"- Deutscher Verband fuer Wasserwirtschaft und Kulturbau e.V. (1996): "
		"'Ermittlung der Verdunstung von Land- und Wasserflaechen', "
		"DVWK Merkblaetter 238/1996, Bonn, 135p.\n"

		"- Boehner, J., Schaefer, W., Conrad, O., Gross, J., Ringeler, A. (2001): "
		"'The WEELS Model: methods, results and limits of wind erosion modelling', "
		"In: Catena, Special Issue\n")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid(
		NULL	, "STA_FC"		, _TL("Field Capacity [mm]"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "STA_FC_DEF"	, _TL("Default"),
		"",
		PARAMETER_TYPE_Double	, 20.0	, 0.0, true
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "STA_PWP"		, _TL("Permanent Wilting Point [mm]"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "STA_PWP_DEF"	, _TL("Default"),
		"",
		PARAMETER_TYPE_Double	, 2.0	, 0.0, true
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "LANDUSE"		, _TL("Land Use"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "LANDUSE_DEF"	, _TL("Default"),
		"",
		PARAMETER_TYPE_Int		, -1.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid(
		NULL	, "DYN_W"		, _TL("Soil Moisture"),
		"",
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_FixedTable(
		NULL	, "DYN_CLIMATE"	, _TL("Climate Data"),
		""
	);

	pClimate	= pNode->asTable();
	pClimate->Set_Name(_TL("Climate Data"));
	pClimate->Add_Field(_TL("Precipitation [mm]")	, TABLE_FIELDTYPE_Double);
	pClimate->Add_Field(_TL("Temperature (2pm) [DegreeC]"), TABLE_FIELDTYPE_Double);
	pClimate->Add_Field(_TL("Air Humidity (2pm) [%%]"), TABLE_FIELDTYPE_Double);

	//-----------------------------------------------------
	pNode	= Parameters.Add_FixedTable(
		NULL	, "STA_KC"		, _TL("Crop Coefficients"),
		""
	);

	pCropCoeff	= pNode->asTable();
	pCropCoeff->Set_Name(_TL("Crop Coefficients"));
	pCropCoeff->Add_Field(_TL("Land Use ID")	, TABLE_FIELDTYPE_Int);
	pCropCoeff->Add_Field(_TL("Name")		, TABLE_FIELDTYPE_String);
	pCropCoeff->Add_Field(_TL("January")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("February")	, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("March")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("April")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("May")			, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("June")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("July")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("August")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("September")	, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("October")		, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("November")	, TABLE_FIELDTYPE_Double);
	pCropCoeff->Add_Field(_TL("December")	, TABLE_FIELDTYPE_Double);

	CTable_Record	*pRec;

#define ADD_RECORD(ID, NAME, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12)	pRec = pCropCoeff->Add_Record();\
	pRec->Set_Value( 0, ID);  pRec->Set_Value( 1, NAME);\
	pRec->Set_Value( 2, m01); pRec->Set_Value( 3, m02); pRec->Set_Value( 4, m03); pRec->Set_Value( 5, m04);\
	pRec->Set_Value( 6, m05); pRec->Set_Value( 7, m06); pRec->Set_Value( 8, m07); pRec->Set_Value( 9, m08);\
	pRec->Set_Value(10, m09); pRec->Set_Value(11, m10); pRec->Set_Value(12, m11); pRec->Set_Value(13, m12);

	//         ID   NAME              Jan   Feb   Mar   Apr   Mai   Jun   Jul   Aug   Sep   Okt   Nov   Dec
	ADD_RECORD(1.0, _TL("Maehweide")		, 1   , 1   , 1   , 1   , 1.05, 1.10, 1.10, 1.05, 1.05, 1   , 1   , 1   );
	ADD_RECORD(2.0, _TL("Winterweizen")	, 0.65, 0.65, 0.80, 0.85, 1.15, 1.45, 1.40, 1   , 0.80, 0.70, 0.65, 0.65);
	ADD_RECORD(3.0, _TL("Wintergerste")	, 1   , 1   , 0.85, 0.95, 1.30, 1.35, 1.25, 1   , 1   , 1   , 1   , 1   );
	ADD_RECORD(4.0, _TL("Sommergerste")	, 1   , 1   , 0.80, 0.90, 1.20, 1.35, 1.20, 1   , 1   , 1   , 1   , 1   );
	ADD_RECORD(5.0, _TL("Winterroggen")	, 0.65, 0.65, 0.85, 0.90, 1.20, 1.30, 1.25, 0.95, 0.80, 0.70, 0.65, 0.65);
	ADD_RECORD(6.0, _TL("Hafer"	)		, 1   , 1   , 0.65, 0.70, 1.10, 1.45, 1.35, 0.95, 1   , 1   , 1   , 1   );
	ADD_RECORD(7.0, _TL("Zuckerrueben")	, 1   , 1   , 1   , 0.50, 0.75, 1.05, 1.40, 1.30, 1.10, 0.85, 1   , 1   );
	ADD_RECORD(8.0, _TL("Kartoffeln")	, 1   , 1   , 1   , 0.50, 0.90, 1.05, 1.45, 1.20, 0.90, 1   , 1   , 1   );
	ADD_RECORD(9.0, _TL("Winterraps")	, 0.65, 0.65, 0.85, 1   , 1.35, 1.35, 1.10, 0.85, 1   , 1   , 0.65, 0.65);
	ADD_RECORD(0.0, _TL("Unknown")		, 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   );
#undef ADD_RECORD
}

//---------------------------------------------------------
CDVWK_SoilMoisture::~CDVWK_SoilMoisture(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDVWK_SoilMoisture::On_Execute(void)
{
	int		Day, x, y, i, LandUseID;
	CGrid	*pGrid;
	CColors	Colors;

	//-----------------------------------------------------
	if( pClimate->Get_Record_Count() > 0 )
	{
		pFK_mm		= Parameters("STA_FC")		->asGrid();
		FK_mm_Def	= Parameters("STA_FC_DEF")	->asDouble();

		pPWP_mm		= Parameters("STA_PWP")		->asGrid();
		PWP_mm_Def	= Parameters("STA_PWP_DEF")	->asDouble();

		pWi_mm		= Parameters("DYN_W")		->asGrid();
		Colors.Set_Ramp(COLOR_GET_RGB(255, 255, 100), COLOR_GET_RGB(0, 0, 100));
		DataObject_Set_Colors(pWi_mm, Colors);

		//-------------------------------------------------
		pLandUse	= API_Create_Grid(pWi_mm, pCropCoeff->Get_Record_Count() < 127 ? GRID_TYPE_Char : GRID_TYPE_Int);
		pLandUse->Assign(Parameters("LANDUSE_DEF")->asInt());

		if( (pGrid = Parameters("LANDUSE")->asGrid()) != NULL )
		{
			for(y=0; y<Get_NY(); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					LandUseID	= pGrid->asInt(x, y);

					for(i=0; i<pCropCoeff->Get_Record_Count(); i++)
					{
						if( LandUseID == pCropCoeff->Get_Record(i)->asInt(0) )
						{
							pLandUse->Set_Value(x, y, i);
							break;
						}
					}
				}
			}
		}

		//-------------------------------------------------
		DataObject_Update(pWi_mm, 0, pFK_mm ? pFK_mm->Get_ZMax() : FK_mm_Def, true);

		for(Day=0; Day<365 && Set_Progress(Day, 365); Day++)
		{
			Step_Day(Day);

			DataObject_Update(pWi_mm, true);
		}

		//-------------------------------------------------
		delete(pLandUse);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	if( Bestand >= 0 && Bestand < pCropCoeff->Get_Record_Count() )
	{
		return( pCropCoeff->Get_Record(Bestand)->asDouble(1 + Get_Month(Day)) );
	}

	return( 1.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	CTable_Record	*pRecord;

	if( (pRecord = pClimate->Get_Record(Day)) == NULL )
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
	CTable_Record	*pRecord;

	if( (pRecord = pClimate->Get_Record(Day)) == NULL )
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDVWK_SoilMoisture::Step_Day(int Day)
{
	int		x, y;
	double	Wi, Pi, ETP, kc, FK, PWP;

	ETP		= Get_ETP_Haude(Day);
	Pi		= Get_Pi(Day);

	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			kc		= Get_kc(pLandUse->asInt(x, y), Day);

			FK		= pFK_mm	? pFK_mm	->asDouble(x, y) : FK_mm_Def;
			PWP		= pPWP_mm	? pPWP_mm	->asDouble(x, y) : PWP_mm_Def;

			Wi		= pWi_mm->asDouble(x, y);
			Wi		= Get_Wi(Wi, Pi, ETP, kc, FK, PWP);
			pWi_mm->Set_Value(x, y, Wi);
		}
	}
}
