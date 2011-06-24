/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             Curvature_Classification.cpp              //
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
#include "Curvature_Classification.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCurvature_Classification::CCurvature_Classification(void)
{
	Set_Name		(_TL("Curvature Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2001"));

	Set_Description	(_TW(
		"Surface curvature based terrain classification.\n"
		"Reference:\n"
		"Dikau, R. (1988):\n'Entwurf einer geomorphographisch-analytischen Systematik von Reliefeinheiten',\n"
		"Heidelberger Geographische Bausteine, Heft 5\n\n"
		"0 - V  / V\n"
		"1 - GE / V\n"
		"2 - X  / V\n"
		"3 - V  / GR\n"
		"4 - GE / GR\n"
		"5 - X  / GR\n"
		"6 - V  / X\n"
		"7 - GE / X\n"
		"8 - X  / X\n"
	));

	Parameters.Add_Grid (NULL, "CPLAN"		, _TL("Plan Curvature")				, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid (NULL, "CPROF"		, _TL("Profile Curvature")			, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Value(NULL, "THRESHOLD"	, _TL("Threshold for plane")		, _TL(""), PARAMETER_TYPE_Double, 0.001, 0.0, true);
	Parameters.Add_Grid (NULL, "CLASS"		, _TL("Curvature Classification")	, _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
}

//---------------------------------------------------------
CCurvature_Classification::~CCurvature_Classification(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCurvature_Classification::On_Execute(void)
{
	int			x, y, Class;
	double		Threshold, dPlan, dProf;
	CSG_Grid	*pPlan, *pProf, *pClasses;

	//-----------------------------------------------------
	pPlan		= Parameters("CPLAN")		->asGrid();
	pProf		= Parameters("CPROF")		->asGrid();
	Threshold	= Parameters("THRESHOLD")	->asDouble();
	pClasses	= Parameters("CLASS")		->asGrid();

	pClasses->Set_NoData_Value(-1);

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pPlan->is_NoData(x, y) || pProf->is_NoData(x, y) )
			{
				pClasses->Set_NoData(x, y);
			}
			else
			{
				dPlan	= pPlan->asDouble(x, y);
				dProf	= pProf->asDouble(x, y);

				Class	 = dPlan < -Threshold ? 0 : (dPlan <= Threshold ? 3 : 6);
				Class	+= dProf < -Threshold ? 0 : (dProf <= Threshold ? 1 : 2);

				pClasses->Set_Value(x, y, Class);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pClasses, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int Color[9]	=
		{
			SG_GET_RGB(  0,   0, 127),	// V  / V
			SG_GET_RGB(  0,  63, 200),	// GE / V
			SG_GET_RGB(  0, 127, 255),	// X  / V
			SG_GET_RGB(127, 200, 255),	// V  / GR
			SG_GET_RGB(255, 255, 255),	// GE / GR
			SG_GET_RGB(255, 200, 127),	// X  / GR
			SG_GET_RGB(255, 127,   0),	// V  / X
			SG_GET_RGB(200,  63,   0),	// GE / X
			SG_GET_RGB(127,   0,   0),	// X  / X
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL( "V / V" );	Desc	+= _TL( "V / V" );
		Name	+= _TL("GE / V" );	Desc	+= _TL("GE / V" );
		Name	+= _TL( "X / V" );	Desc	+= _TL( "X / V" );
		Name	+= _TL( "V / GR");	Desc	+= _TL( "V / GR");
		Name	+= _TL("GE / GR");	Desc	+= _TL("GE / GR");
		Name	+= _TL( "X / GR");	Desc	+= _TL( "X / GR");
		Name	+= _TL( "V / X" );	Desc	+= _TL( "V / X" );
		Name	+= _TL("GE / X" );	Desc	+= _TL("GE / X" );
		Name	+= _TL( "X / X" );	Desc	+= _TL( "X / X" );

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<9; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, Color[i]);
			pRecord->Set_Value(1, Name [i].c_str());
			pRecord->Set_Value(2, Desc [i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pClasses, P);
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
