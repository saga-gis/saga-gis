/**********************************************************
 * Version $Id: change_detection.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                imagery_classification                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  change_detection.cpp                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include "change_detection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChange_Detection::CChange_Detection(void)
{
	CSG_Parameter	*pNode, *pTable;

	//-----------------------------------------------------
	Set_Name		(_TL("Change Detection"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Change detection for classified grids."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid(
		NULL	, "INITIAL"		, _TL("Initial State"),
		_TL(""),
		PARAMETER_INPUT
	);

	pTable	= Parameters.Add_Table(pNode, "INI_LUT" , _TL("Look-up Table")	, _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Table_Field(pTable, "INI_LUT_MIN", _TL("Value")			, _TL(""), false);
	Parameters.Add_Table_Field(pTable, "INI_LUT_MAX", _TL("Value (Maximum)"), _TL(""), true	);
	Parameters.Add_Table_Field(pTable, "INI_LUT_NAM", _TL("Name")			, _TL(""), true);

	pNode	= Parameters.Add_Grid(
		NULL	, "FINAL"		, _TL("Final State"),
		_TL(""),
		PARAMETER_INPUT
	);

	pTable	= Parameters.Add_Table(pNode, "FIN_LUT" , _TL("Look-up Table")	, _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Table_Field(pTable, "FIN_LUT_MIN", _TL("Value")			, _TL(""), false);
	Parameters.Add_Table_Field(pTable, "FIN_LUT_MAX", _TL("Value (Maximum)"), _TL(""), true	);
	Parameters.Add_Table_Field(pTable, "FIN_LUT_NAM", _TL("Name")			, _TL(""), true);

	Parameters.Add_Grid(
		NULL	, "CHANGE"		, _TL("Changes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "NOCHANGE"	, _TL("Report Unchanged Classes"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	pNode	= Parameters.Add_Table(
		NULL	, "CHANGES"		, _TL("Changes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		pNode	, "OUTPUT"		, _TL("Output as..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("cells"),
			_TL("percent"),
			_TL("area")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CChange_Detection::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("INI_LUT")) )
	{
		pParameters->Get_Parameter("INI_LUT_MIN")->Set_Enabled(pParameter->asTable() != NULL);
		pParameters->Get_Parameter("INI_LUT_MAX")->Set_Enabled(pParameter->asTable() != NULL);
		pParameters->Get_Parameter("INI_LUT_NAM")->Set_Enabled(pParameter->asTable() != NULL);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FIN_LUT")) )
	{
		pParameters->Get_Parameter("FIN_LUT_MIN")->Set_Enabled(pParameter->asTable() != NULL);
		pParameters->Get_Parameter("FIN_LUT_MAX")->Set_Enabled(pParameter->asTable() != NULL);
		pParameters->Get_Parameter("FIN_LUT_NAM")->Set_Enabled(pParameter->asTable() != NULL);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Detection::On_Execute(void)
{
	bool		bNoChange;
	int			iInitial, iFinal;
	CSG_Matrix	Identity;
	CSG_Table	Initial, Final, *pChanges;
	CSG_Grid	*pInitial, *pFinal, *pChange;

	//-----------------------------------------------------
	pInitial	= Parameters("INITIAL")	->asGrid();
	pFinal		= Parameters("FINAL")	->asGrid();
	pChange		= Parameters("CHANGE")	->asGrid();
	pChanges	= Parameters("CHANGES")	->asTable();
	bNoChange	= Parameters("NOCHANGE")->asBool();

	if( !Get_Classes(Initial, pInitial, true) )
	{
		Error_Set(_TL("no class definitions for initial state"));

		return( false );
	}

	if( !Get_Classes(Final, pFinal, false) )
	{
		Error_Set(_TL("no class definitions for final state"));

		return( false );
	}

	if( !Get_Changes(Initial, Final, pChanges, Identity) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			iInitial	= Get_Class(Initial, pInitial->asDouble(x, y));
			iFinal		= Get_Class(Final  , pFinal  ->asDouble(x, y));

			if( bNoChange || !Identity[iInitial][iFinal] )
			{
				pChanges->Get_Record(iInitial)->Add_Value(1 + iFinal, 1);

				pChange->Set_Value(x, y, (pChanges->Get_Field_Count() - 1) * iInitial + iFinal);
			}
			else
			{
				pChange->Set_Value(x, y, -1);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pChange, P) && P("COLORS_TYPE") && P("LUT") )
	{
		CSG_Table	*pLUT	= P("LUT")->asTable();

		CSG_Colors	cRandom(pChanges->Get_Count());

		cRandom.Random();

		pLUT->Del_Records();

		for(iInitial=0; iInitial<pChanges->Get_Count(); iInitial++)
		{
			CSG_Colors	cRamp(pChanges->Get_Field_Count() - 1);

			cRamp.Set_Ramp(cRandom[iInitial], cRandom[iInitial]);
			cRamp.Set_Ramp_Brighness(225, 50);

			for(iFinal=0; iFinal<pChanges->Get_Field_Count()-1; iFinal++)
			{
				if( pChanges->Get_Record(iInitial)->asInt(1 + iFinal) > 0 )
				{
					CSG_Table_Record	*pClass	= pLUT->Add_Record();

					pClass->Set_Value(0, cRamp.Get_Color(iFinal));
					pClass->Set_Value(1, CSG_String::Format(SG_T("%s >> %s"), pChanges->Get_Record(iInitial)->asString(0), pChanges->Get_Field_Name(1 + iFinal)));
					pClass->Set_Value(3, (pChanges->Get_Field_Count() - 1) * iInitial + iFinal);
					pClass->Set_Value(4, (pChanges->Get_Field_Count() - 1) * iInitial + iFinal);
				}
			}
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pChange, P);
	}

	//-----------------------------------------------------
	double	Factor;

	switch( Parameters("OUTPUT")->asInt() )
	{
	default:	Factor	= 1.0;						break;	// cells
	case 1:		Factor	= 100.0 / Get_NCells();		break;	// percent
	case 2:		Factor	= M_SQR(Get_Cellsize());	break;	// area
	}

	if( Factor != 1.0 )
	{
		for(iInitial=0; iInitial<pChanges->Get_Count(); iInitial++)
		{
			for(iFinal=0; iFinal<pChanges->Get_Field_Count()-1; iFinal++)
			{
				pChanges->Get_Record(iInitial)->Mul_Value(1 + iFinal, Factor);
			}
		}
	}

	//-----------------------------------------------------
	pChanges	->Set_Name(CSG_String::Format(SG_T("%s [%s >> %s]"), _TL("Changes"), pInitial->Get_Name(), pFinal->Get_Name()));

	pChange		->Set_Name(CSG_String::Format(SG_T("%s [%s >> %s]"), _TL("Changes"), pInitial->Get_Name(), pFinal->Get_Name()));
	pChange		->Set_NoData_Value(-1);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASS_NAM	= 0,
	CLASS_MIN,
	CLASS_MAX
};

//---------------------------------------------------------
bool CChange_Detection::Get_Classes(CSG_Table &Classes, CSG_Grid *pGrid, bool bInitial)
{
	CSG_Table	*pClasses;

	Classes.Destroy();

	Classes.Add_Field(_TL("NAME")	, SG_DATATYPE_String);
	Classes.Add_Field(_TL("MIN")	, SG_DATATYPE_Double);
	Classes.Add_Field(_TL("MAX")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	if( (pClasses = Parameters(bInitial ? "INI_LUT" : "FIN_LUT")->asTable()) != NULL )
	{
		int	fNam	= Parameters(bInitial ? "INI_LUT_NAM" : "FIN_LUT_NAM")->asInt();
		int	fMin	= Parameters(bInitial ? "INI_LUT_MIN" : "FIN_LUT_MIN")->asInt();
		int	fMax	= Parameters(bInitial ? "INI_LUT_MAX" : "FIN_LUT_MAX")->asInt();

		if( fNam < 0 || fNam >= pClasses->Get_Field_Count() )	{	fNam	= fMin;	}
		if( fMax < 0 || fMax >= pClasses->Get_Field_Count() )	{	fMax	= fMin;	}

		for(int iClass=0; iClass<pClasses->Get_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= Classes.Add_Record();

			pClass->Set_Value(CLASS_NAM, pClasses->Get_Record(iClass)->asString(fNam));
			pClass->Set_Value(CLASS_MIN, pClasses->Get_Record(iClass)->asDouble(fMin));
			pClass->Set_Value(CLASS_MAX, pClasses->Get_Record(iClass)->asDouble(fMax));
		}
	}

	//-----------------------------------------------------
	else if( DataObject_Get_Parameter(pGrid, "LUT") )
	{
		pClasses	= DataObject_Get_Parameter(pGrid, "LUT")->asTable();

		for(int iClass=0; iClass<pClasses->Get_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= Classes.Add_Record();

			pClass->Set_Value(CLASS_NAM, pClasses->Get_Record(iClass)->asString(1));
			pClass->Set_Value(CLASS_MIN, pClasses->Get_Record(iClass)->asDouble(3));
			pClass->Set_Value(CLASS_MAX, pClasses->Get_Record(iClass)->asDouble(4));
		}
	}

	//-----------------------------------------------------
	else
	{
		double	z;

		for(long i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			double iz	= pGrid->asDouble(pGrid->Get_Sorted(i, false, false));

			if( i == 0 || iz != z )
			{
				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(CLASS_NAM, z = iz);
				pClass->Set_Value(CLASS_MIN, z);
				pClass->Set_Value(CLASS_MAX, z);
			}
		}
	}

	//-----------------------------------------------------
	return( Classes.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Detection::Get_Changes(CSG_Table &Initial, CSG_Table &Final, CSG_Table *pChanges, CSG_Matrix &Identity)
{
	int		iInitial, iFinal;

	//-----------------------------------------------------
	Identity.Create(Final.Get_Count() + 1, Initial.Get_Count() + 1);

	for(iInitial=0; iInitial<Initial.Get_Count(); iInitial++)
	{
		CSG_String	s	= Initial[iInitial].asString(CLASS_NAM);

		for(iFinal=0; iFinal<Final.Get_Count(); iFinal++)
		{
			Identity[iInitial][iFinal]	= s.Cmp(Final[iFinal].asString(CLASS_NAM)) ? 0 : 1;
		}
	}

	Identity[Initial.Get_Count()][Final.Get_Count()]	= 1;	// unclassified

	//-----------------------------------------------------
	pChanges->Destroy();

	pChanges->Add_Field(_TL("Name"), SG_DATATYPE_String);

	for(iFinal=0; iFinal<Final.Get_Count(); iFinal++)
	{
		pChanges->Add_Field(Final[iFinal].asString(CLASS_NAM), SG_DATATYPE_Double);
	}

	pChanges->Add_Field(_TL("Unclassified"), SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(iInitial=0; iInitial<Initial.Get_Count(); iInitial++)
	{
		pChanges->Add_Record()->Set_Value(0, Initial[iInitial].asString(CLASS_NAM));
	}

	pChanges->Add_Record()->Set_Value(0, _TL("Unclassified"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CChange_Detection::Cmp_Class(CSG_Table &Classes, double Value, int iClass)
{
	CSG_Table_Record	*pClass	= Classes.Get_Record_byIndex(iClass);

	double	min	= pClass->asDouble(CLASS_MIN);

	if( Value < min )
	{
		return( 1 );
	}

	double	max	= pClass->asDouble(CLASS_MAX);

	return( min < max
		?	(Value < max ?  0 : -1)
		:	(Value > min ? -1 :  0)
	);
}

//---------------------------------------------------------
int CChange_Detection::Get_Class(CSG_Table &Classes, double Value)
{
	int		a, b, i, c;

	if( Classes.Get_Count() > 0 )
	{
		if( Classes.Get_Index_Field(0) != CLASS_MIN || Classes.Get_Index_Order(0) != TABLE_INDEX_Ascending )
		{
			Classes.Set_Index(CLASS_MIN, TABLE_INDEX_Ascending);
		}

		for(a=0, b=Classes.Get_Record_Count()-1; a < b; )
		{
			i	= a + (b - a) / 2;
			c	= Cmp_Class(Classes, Value, i);

			if( c > 0 )
			{
				b	= b > i ? i : b - 1;
			}
			else if( c < 0 )
			{
				a	= a < i ? i : a + 1;
			}
			else
			{
				return( Classes.Get_Record_byIndex(i)->Get_Index() );
			}
		}

		if( Cmp_Class(Classes, Value, a) == 0 )
		{
			return( Classes.Get_Record_byIndex(a)->Get_Index() );
		}

		if( a != b && Cmp_Class(Classes, Value, b) == 0 )
		{
			return( Classes.Get_Record_byIndex(b)->Get_Index() );
		}
	}

	return( Classes.Get_Count() );	// := unclassified
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
