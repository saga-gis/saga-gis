
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Bifurcation.cpp                    //
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
#include "Bifurcation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBifurcation::CBifurcation(void)
{
	Set_Name(_TL("Bifurcation"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL("Feigenbaum's Bifurcation"));

	Parameters.Add_Table(	NULL, "TABLE"		, _TL("Output")			, "", PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "ITERATIONS"	, _TL("Value Count")		, "", PARAMETER_TYPE_Int, 100);
	Parameters.Add_Value(	NULL, "NVALUES"		, _TL("Value Count")		, _TL("Number of the last x Iteration to be plotted."), PARAMETER_TYPE_Int, 8);
	Parameters.Add_Value(	NULL, "SEED"		, _TL("Seed Value")		, "", PARAMETER_TYPE_Double	, 0.66, 0, true, 1.0, true);
	Parameters.Add_Range(	NULL, "RANGE"		, _TL("Range")			, "", 1.0, 4.0, 0.0, true, 4.0, true);
	Parameters.Add_Value(	NULL, "STEP"		, _TL("Number of Steps")	, "", PARAMETER_TYPE_Int		, 1000, 2, true);

}

//---------------------------------------------------------
CBifurcation::~CBifurcation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBifurcation::On_Execute(void)
{
	int					i;
	double				p, r, dr, max, min, seed, nValues, nPreIterations;
	CTable_Record		*pRecord;
	CTable				*pTable;

	nPreIterations	= Parameters("ITERATIONS")->asInt();
	nValues			= Parameters("NVALUES")->asInt();
	seed			= Parameters("SEED")->asDouble();
	min				= Parameters("RANGE")->asRange()->Get_LoVal();
	max				= Parameters("RANGE")->asRange()->Get_HiVal();
	dr				= (max - min) / 1000.0;

	pTable			= Parameters("TABLE")->asTable();
	pTable->Destroy();
	pTable->Set_Name(_TL("Feigenbaum's Bifurcation"));

	pTable->Add_Field("Growth"	, TABLE_FIELDTYPE_Double);

	for(i=0; i<nValues; i++)
	{
		pTable->Add_Field(CAPI_String::Format("VALUE_%d", i + 1), TABLE_FIELDTYPE_Double);
	}

	for(r=min; r<=max; r+=dr)
	{
		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, r);

		p		= seed;

		for(i=0; i<nPreIterations; i++)
		{
			p		= r * p * (1.0 - p);
		}

		for(i=0; i<nValues; i++)
		{
			p	= r * p * (1.0 - p);
			pRecord->Set_Value(i + 1, p);
		}
	}

	return( true );
}
