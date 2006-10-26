
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
#include "Hugget_02.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHugget_02::CHugget_02(void)
{
	CSG_Parameter	*pNode;

	Set_Name	(_TL("02: Carbon Cycle Simulation for Terrestrial Biomes"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Simulation of the Carbon Cycle in Terrestrial Biomes. "

		"\nReference:"
		"\nHugget, R.J. (1993): 'Modelling the Human Impact on Nature', Oxford University Press.\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Results"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "TIME_SPAN"	, _TL("Time Span"),
		"",
		PARAMETER_TYPE_Double	, 100.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "TIME_STEP"	, _TL("Time Interval [a]"),
		"",
		PARAMETER_TYPE_Double	, 0.1, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "PRIMPROD"	, _TL("Net Primary Production [Gt Carbon / a]"),
		"",
		PARAMETER_TYPE_Double	, 27.8, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "CHUMIFY"		, _TL("Humification Factor"),
		"",
		PARAMETER_TYPE_Double	, 0.4, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "CCARBON"		, _TL("Carbonization Factor"),
		"",
		PARAMETER_TYPE_Double	, 0.05, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_1", _TL("Partition Coefficients"), "");

	Parameters.Add_Value(
		pNode	, "P_LEAV"		, _TL("Leaves"),
		"",
		PARAMETER_TYPE_Double	, 0.3, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "P_BRAN"		, _TL("Branches"),
		"",
		PARAMETER_TYPE_Double	, 0.2, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "P_STEM"		, _TL("Stems"),
		"",
		PARAMETER_TYPE_Double	, 0.3, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "P_ROOT"		, _TL("Roots"),
		"",
		PARAMETER_TYPE_Double	, 0.2, 0.0, true
	);


	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_2", _TL("Transfer Coefficients"), "");

	Parameters.Add_Value(
		pNode	, "K_LEAV_LITT"		, _TL("Leaves to Litter"),
		"",
		PARAMETER_TYPE_Double	, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "K_BRAN_LITT"		, _TL("Branches to Litter"),
		"",
		PARAMETER_TYPE_Double	, 0.1, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "K_STEM_LITT"		, _TL("Stems to Litter"),
		"",
		PARAMETER_TYPE_Double	, 0.033, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "K_ROOT_HUMU"		, _TL("Roots to Humus"),
		"",
		PARAMETER_TYPE_Double	, 0.1, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "K_LITT_HUMU"		, _TL("Litter to Humus"),
		"",
		PARAMETER_TYPE_Double	, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "K_HUMU_COAL"		, _TL("Humus to Charcoal"),
		"",
		PARAMETER_TYPE_Double	, 0.1, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "K_COAL_ENVI"		, _TL("Charcoal to Environment"),
		"",
		PARAMETER_TYPE_Double	, 0.002, 0.0, true
	);

}

//---------------------------------------------------------
CHugget_02::~CHugget_02(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define WRITE_RECORD	pRecord	= pTable->Add_Record();\
						pRecord->Set_Value(0, iStep);\
						pRecord->Set_Value(1, iStep * dTime);\
						pRecord->Set_Value(2, C_Leav);\
						pRecord->Set_Value(3, C_Bran);\
						pRecord->Set_Value(4, C_Stem);\
						pRecord->Set_Value(5, C_Root);\
						pRecord->Set_Value(6, C_Litt);\
						pRecord->Set_Value(7, C_Humu);\
						pRecord->Set_Value(8, C_Coal);

//---------------------------------------------------------
bool CHugget_02::On_Execute(void)
{
	int				iStep, nSteps;
	double			sTime, dTime, PrimProd, cHumify, cCarbon,
					C_Leav, C_Bran, C_Stem, C_Root, C_Litt, C_Humu, C_Coal,
					P_Leav, P_Bran, P_Stem, P_Root,
					K_Leav_Litt, K_Bran_Litt, K_Stem_Litt,
					K_Root_Humu, K_Litt_Humu, K_Humu_Coal, K_Coal_Envi,
					d_Leav_Litt, d_Bran_Litt, d_Stem_Litt,
					d_Root_Humu, d_Litt_Humu, d_Humu_Coal, d_Coal_Envi;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;

	//-----------------------------------------------------
	sTime	= Parameters("TIME_SPAN")		->asDouble();
	dTime	= Parameters("TIME_STEP")		->asDouble();
	nSteps	= (int)(sTime / dTime);

	PrimProd	= Parameters("PRIMPROD")	->asDouble();
	cHumify		= Parameters("CHUMIFY")		->asDouble();
	cCarbon		= Parameters("CCARBON")		->asDouble();

	P_Leav		= Parameters("P_LEAV")		->asDouble();
	P_Bran		= Parameters("P_BRAN")		->asDouble();
	P_Stem		= Parameters("P_STEM")		->asDouble();
	P_Root		= Parameters("P_ROOT")		->asDouble();

	K_Leav_Litt	= Parameters("K_LEAV_LITT")	->asDouble();
	K_Bran_Litt	= Parameters("K_BRAN_LITT")	->asDouble();
	K_Stem_Litt	= Parameters("K_STEM_LITT")	->asDouble();
	K_Root_Humu	= Parameters("K_ROOT_HUMU")	->asDouble();
	K_Litt_Humu	= Parameters("K_LITT_HUMU")	->asDouble();
	K_Humu_Coal	= Parameters("K_HUMU_COAL")	->asDouble();
	K_Coal_Envi	= Parameters("K_COAL_ENVI")	->asDouble();

	pTable		= Parameters("TABLE")		->asTable();
	pTable->Destroy();
	pTable->Set_Name(_TL("Carbon Cycle Simulation"));
	pTable->Add_Field("STEP"	, TABLE_FIELDTYPE_Int);
	pTable->Add_Field("TIME"	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field("LEAVES"	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field("BRANCHES", TABLE_FIELDTYPE_Double);
	pTable->Add_Field("STEMS"	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field("ROOTS"	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field("LITTER"	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field("HUMUS"	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field("CHARCOAL", TABLE_FIELDTYPE_Double);

	//-----------------------------------------------------
	C_Leav		= 0.0;
	C_Bran		= 0.0;
	C_Stem		= 0.0;
	C_Root		= 0.0;
	C_Litt		= 0.0;
	C_Humu		= 0.0;
	C_Coal		= 0.0;

	//-----------------------------------------------------
	for(iStep=0; iStep<nSteps && Set_Progress(iStep, nSteps); iStep++)
	{
		WRITE_RECORD;

		d_Leav_Litt	= K_Leav_Litt * C_Leav;
		d_Bran_Litt	= K_Bran_Litt * C_Bran;
		d_Stem_Litt	= K_Stem_Litt * C_Stem;
		d_Root_Humu	= K_Root_Humu * C_Root;
		d_Litt_Humu	= K_Litt_Humu * C_Litt;
		d_Humu_Coal	= K_Humu_Coal * C_Humu;
		d_Coal_Envi	= K_Coal_Envi * C_Coal;

		C_Leav	= C_Leav + dTime * (-d_Leav_Litt + P_Leav * PrimProd);
		C_Bran	= C_Bran + dTime * (-d_Bran_Litt + P_Bran * PrimProd);
		C_Stem	= C_Stem + dTime * (-d_Stem_Litt + P_Stem * PrimProd);
		C_Root	= C_Root + dTime * (-d_Root_Humu + P_Root * PrimProd);
		C_Litt	= C_Litt + dTime * (-d_Litt_Humu + d_Leav_Litt + d_Bran_Litt + d_Stem_Litt);
		C_Humu	= C_Humu + dTime * (-d_Humu_Coal + cHumify * (d_Root_Humu + d_Litt_Humu));
		C_Coal	= C_Coal + dTime * (-d_Coal_Envi + cCarbon * (d_Humu_Coal));
	}

	WRITE_RECORD;

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
