
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 sim_ecosystems_hugget                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Hugget_02.cpp                      //
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
#include "Hugget_02.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHugget_02::CHugget_02(void)
{
	Set_Name		(_TL("Carbon Cycle Simulation for Terrestrial Biomass"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Simulation of the Carbon Cycle in Terrestrial Biomass. "
	));

	Add_Reference("Hugget, R.J.", "1993",
		"Modelling the Human Impact on Nature",
		"Oxford University Press."
	);

	//-----------------------------------------------------
	Parameters.Add_Table (""          , "TABLE"      , _TL("Terrestrial Carbon"     ), _TL(""           ), PARAMETER_OUTPUT);

	Parameters.Add_Double(""          , "TIME_SPAN"  , _TL("Time Span"              ), _TL("[yrs]"      ), 500.   , 0., true);
	Parameters.Add_Double(""          , "TIME_STEP"  , _TL("Time Interval"          ), _TL("[yrs]"      ),   0.1  , 0., true);

	Parameters.Add_Choice("",
		"PRESETS", _TL("Presets"), _TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s",
			_TL("Tropical forest"),
			_TL("Temperate forest"),
			_TL("Grassland"),
			_TL("Agricultural area"),
			_TL("Human area"),
			_TL("Tundra and semi-desert"),
			_TL("Adjust parameters manually")
		)
	);

	Parameters.Add_Double("PRESETS"   , "PRIMPROD"   , _TL("Net Primary Production" ), _TL("[Gt C / yr]"),  27.8  , 0., true);

	Parameters.Add_Node  ("PRESETS"   , "PARTITION"  , _TL("Partition Coefficients" ), _TL(""));
	Parameters.Add_Double("PARTITION" , "P_LEAV"     , _TL("Leaves"                 ), _TL("fraction"   ),   0.3  , 0., true);
	Parameters.Add_Double("PARTITION" , "P_BRAN"     , _TL("Branches"               ), _TL("fraction"   ),   0.2  , 0., true);
	Parameters.Add_Double("PARTITION" , "P_STEM"     , _TL("Stems"                  ), _TL("fraction"   ),   0.3  , 0., true);
	Parameters.Add_Double("PARTITION" , "P_ROOT"     , _TL("Roots"                  ), _TL("fraction"   ),   0.2  , 0., true);

	Parameters.Add_Node  ("PRESETS"   , "TRANSFER_C" , _TL("Transfer Coefficients"  ), _TL(""));
	Parameters.Add_Double("TRANSFER_C", "K_LEAV_LITT", _TL("Leaves to Litter"       ), _TL("[1 / yr]"   ),   1.   , 0., true);
	Parameters.Add_Double("TRANSFER_C", "K_BRAN_LITT", _TL("Branches to Litter"     ), _TL("[1 / yr]"   ),   0.1  , 0., true);
	Parameters.Add_Double("TRANSFER_C", "K_STEM_LITT", _TL("Stems to Litter"        ), _TL("[1 / yr]"   ),   0.033, 0., true);
	Parameters.Add_Double("TRANSFER_C", "K_ROOT_HUMU", _TL("Roots to Humus"         ), _TL("[1 / yr]"   ),   0.1  , 0., true);
	Parameters.Add_Double("TRANSFER_C", "K_LITT_HUMU", _TL("Litter to Humus"        ), _TL("[1 / yr]"   ),   1.   , 0., true);
	Parameters.Add_Double("TRANSFER_C", "K_HUMU_COAL", _TL("Humus to Charcoal"      ), _TL("[1 / yr]"   ),   0.1  , 0., true);
	Parameters.Add_Double("TRANSFER_C", "K_COAL_ENVI", _TL("Charcoal to Environment"), _TL("[1 / yr]"   ),   0.002, 0., true);

	Parameters.Add_Node  ("PRESETS"   , "TRANSFER_F" , _TL("Transfer Factors"       ), _TL(""));
	Parameters.Add_Double("TRANSFER_F", "CHUMIFY"    , _TL("Humification"           ), _TL("fraction"   ),   0.4  , 0., true);
	Parameters.Add_Double("TRANSFER_F", "CCARBON"    , _TL("Carbonization"          ), _TL("fraction"   ),   0.05 , 0., true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHugget_02::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PRESETS") && pParameter->asInt() < 6 )
	{
		static const double Presets[6][14] = {
			{ 27.8, 0.3, 0.2, 0.3, 0.2, 1.0, 0.1, 0.033 , 0.1, 1.0, 0.1  , 0.002, 0.4, 0.05 }, // Tropical forest
			{  8.7, 0.3, 0.2, 0.3, 0.2, 0.5, 0.1, 0.0166, 0.1, 0.5, 0.02 , 0.002, 0.6, 0.05 }, // Temperate forest
			{ 10.7, 0.6, 0.0, 0.0, 0.4, 1.0, 0.1, 0.02  , 1.0, 0.5, 0.025, 0.002, 0.6, 0.05 }, // Grassland
			{  7.5, 0.8, 0.0, 0.0, 0.2, 1.0, 0.1, 0.02  , 1.0, 1.0, 0.04 , 0.002, 0.2, 0.05 }, // Agricultural area
			{  0.2, 0.3, 0.2, 0.3, 0.2, 1.0, 0.1, 0.02  , 0.1, 0.5, 0.02 , 0.002, 0.5, 0.05 }, // Human area
			{  2.1, 0.5, 0.1, 0.1, 0.3, 1.0, 0.1, 0.02  , 0.5, 0.5, 0.02 , 0.002, 0.6, 0.05 }  // Tundra and semi-desert
		};

		const double *Preset = Presets[pParameter->asInt()];

		(*pParameters)["PRIMPROD"   ].Set_Value(Preset[ 0]);
		(*pParameters)["P_LEAV"     ].Set_Value(Preset[ 1]);
		(*pParameters)["P_BRAN"     ].Set_Value(Preset[ 2]);
		(*pParameters)["P_STEM"     ].Set_Value(Preset[ 3]);
		(*pParameters)["P_ROOT"     ].Set_Value(Preset[ 4]);
		(*pParameters)["K_LEAV_LITT"].Set_Value(Preset[ 5]);
		(*pParameters)["K_BRAN_LITT"].Set_Value(Preset[ 6]);
		(*pParameters)["K_STEM_LITT"].Set_Value(Preset[ 7]);
		(*pParameters)["K_ROOT_HUMU"].Set_Value(Preset[ 8]);
		(*pParameters)["K_LITT_HUMU"].Set_Value(Preset[ 9]);
		(*pParameters)["K_HUMU_COAL"].Set_Value(Preset[10]);
		(*pParameters)["K_COAL_ENVI"].Set_Value(Preset[11]);
		(*pParameters)["CHUMIFY"    ].Set_Value(Preset[12]);
		(*pParameters)["CCARBON"    ].Set_Value(Preset[13]);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CHugget_02::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PRESETS") )
	{
		pParameter->Set_Children_Enabled(pParameter->asInt() >= 6);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHugget_02::On_Execute(void)
{
	//-----------------------------------------------------
	#define WRITE_RECORD { int i = pTable->Get_Count(); CSG_Table_Record *pRecord = pTable->Add_Record();\
		pRecord->Set_Value(0, i + 1    );\
		pRecord->Set_Value(1, i * dTime);\
		pRecord->Set_Value(2, C_Leav   );\
		pRecord->Set_Value(3, C_Bran   );\
		pRecord->Set_Value(4, C_Stem   );\
		pRecord->Set_Value(5, C_Root   );\
		pRecord->Set_Value(6, C_Litt   );\
		pRecord->Set_Value(7, C_Humu   );\
		pRecord->Set_Value(8, C_Coal   );\
		pRecord->Set_Value(9, C_Leav + C_Bran + C_Stem + C_Root + C_Litt + C_Humu + C_Coal);\
	}

	//-----------------------------------------------------
	CSG_Table *pTable = Parameters("TABLE")->asTable(); pTable->Destroy();
	pTable->Set_Name(_TL("Terrestrial Carbon"));
	pTable->Add_Field("STEP"	, SG_DATATYPE_Int   );
	pTable->Add_Field("TIME"	, SG_DATATYPE_Double);
	pTable->Add_Field("LEAVES"	, SG_DATATYPE_Double);
	pTable->Add_Field("BRANCHES", SG_DATATYPE_Double);
	pTable->Add_Field("STEMS"	, SG_DATATYPE_Double);
	pTable->Add_Field("ROOTS"	, SG_DATATYPE_Double);
	pTable->Add_Field("LITTER"	, SG_DATATYPE_Double);
	pTable->Add_Field("HUMUS"	, SG_DATATYPE_Double);
	pTable->Add_Field("CHARCOAL", SG_DATATYPE_Double);
	pTable->Add_Field("TOTAL"   , SG_DATATYPE_Double);

	//-----------------------------------------------------
	double Time_Span   = Parameters("TIME_SPAN"  )->asDouble();
	double dTime       = Parameters("TIME_STEP"  )->asDouble();

	double PrimProd    = Parameters("PRIMPROD"   )->asDouble();
	double cHumify     = Parameters("CHUMIFY"    )->asDouble();
	double cCarbon     = Parameters("CCARBON"    )->asDouble();

	double P_Leav      = Parameters("P_LEAV"     )->asDouble();
	double P_Bran      = Parameters("P_BRAN"     )->asDouble();
	double P_Stem      = Parameters("P_STEM"     )->asDouble();
	double P_Root      = Parameters("P_ROOT"     )->asDouble();

	double K_Leav_Litt = Parameters("K_LEAV_LITT")->asDouble();
	double K_Bran_Litt = Parameters("K_BRAN_LITT")->asDouble();
	double K_Stem_Litt = Parameters("K_STEM_LITT")->asDouble();
	double K_Root_Humu = Parameters("K_ROOT_HUMU")->asDouble();
	double K_Litt_Humu = Parameters("K_LITT_HUMU")->asDouble();
	double K_Humu_Coal = Parameters("K_HUMU_COAL")->asDouble();
	double K_Coal_Envi = Parameters("K_COAL_ENVI")->asDouble();

	//-----------------------------------------------------
	double C_Leav = 0., C_Bran = 0., C_Stem = 0., C_Root = 0., C_Litt = 0., C_Humu = 0., C_Coal = 0.;

	WRITE_RECORD;

	for(double Time=0.; Time<=Time_Span && Set_Progress(Time, Time_Span); Time+=dTime)
	{
		double d_Leav_Litt = K_Leav_Litt * C_Leav;
		double d_Bran_Litt = K_Bran_Litt * C_Bran;
		double d_Stem_Litt = K_Stem_Litt * C_Stem;
		double d_Root_Humu = K_Root_Humu * C_Root;
		double d_Litt_Humu = K_Litt_Humu * C_Litt;
		double d_Humu_Coal = K_Humu_Coal * C_Humu;
		double d_Coal_Envi = K_Coal_Envi * C_Coal;

		C_Leav += dTime * (-d_Leav_Litt + P_Leav * PrimProd);
		C_Bran += dTime * (-d_Bran_Litt + P_Bran * PrimProd);
		C_Stem += dTime * (-d_Stem_Litt + P_Stem * PrimProd);
		C_Root += dTime * (-d_Root_Humu + P_Root * PrimProd);
		C_Litt += dTime * (-d_Litt_Humu + d_Leav_Litt + d_Bran_Litt + d_Stem_Litt);
		C_Humu += dTime * (-d_Humu_Coal + cHumify * (d_Root_Humu + d_Litt_Humu));
		C_Coal += dTime * (-d_Coal_Envi + cCarbon * (d_Humu_Coal));

		WRITE_RECORD;
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
