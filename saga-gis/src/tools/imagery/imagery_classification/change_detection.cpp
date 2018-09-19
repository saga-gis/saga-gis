/**********************************************************
 * Version $Id: change_detection.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "change_detection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChange_Detection::CChange_Detection(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Confusion Matrix (Two Grids)"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Compares two classified grids and creates a confusion matrix "
		"and derived coefficients as well as the combinations of both "
		"classifications as new grid. "
		"Grid classes have to be defined with a look-up table and "
		"values of both grids must match each other for the "
		"subsequent comparison. "
		"A typical application is a change detection analysis "
		"based on land cover classification of satellite imagery. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ONE"		, _TL("Classification 1"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("ONE", "ONE_LUT", _TL("Look-up Table"), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Table_Field( "ONE_LUT", "ONE_LUT_MIN", _TL("Value"          ), _TL(""), false);
	Parameters.Add_Table_Field( "ONE_LUT", "ONE_LUT_MAX", _TL("Value (Maximum)"), _TL(""),  true);
	Parameters.Add_Table_Field( "ONE_LUT", "ONE_LUT_NAM", _TL("Name"           ), _TL(""),  true);

	Parameters.Add_Grid("",
		"TWO"		, _TL("Classification 2"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("TWO", "TWO_LUT", _TL("Look-up Table"), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Table_Field( "TWO_LUT", "TWO_LUT_MIN", _TL("Value"          ), _TL(""), false);
	Parameters.Add_Table_Field( "TWO_LUT", "TWO_LUT_MAX", _TL("Value (Maximum)"), _TL(""),  true);
	Parameters.Add_Table_Field( "TWO_LUT", "TWO_LUT_NAM", _TL("Name"           ), _TL(""),  true);

	Parameters.Add_Grid("",
		"COMBINED"	, _TL("Combined Classes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"NOCHANGE"	, _TL("Report Unchanged Classes"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"NODATA"	, _TL("Include Unclassified Cells"),
		_TL(""),
		true
	);

	Parameters.Add_Table("",
		"CONFUSION"	, _TL("Confusion Matrix"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("CONFUSION",
		"OUTPUT"	, _TL("Output as..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("cells"),
			_TL("percent"),
			_TL("area")
		), 0
	);

	Parameters.Add_Table("",
		"CLASSES"	, _TL("Class Values"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"SUMMARY"	, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CChange_Detection::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("ONE_LUT") )
	{
		pParameters->Set_Enabled("ONE_LUT_MIN", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("ONE_LUT_MAX", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("ONE_LUT_NAM", pParameter->asTable() != NULL);
	}

	if(	pParameter->Cmp_Identifier("TWO_LUT") )
	{
		pParameters->Set_Enabled("TWO_LUT_MIN", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("TWO_LUT_MAX", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("TWO_LUT_NAM", pParameter->asTable() != NULL);
	}

	if(	pParameter->Cmp_Identifier("NOCHANGE") )
	{
		pParameters->Set_Enabled("CLASSES"    , pParameter->asBool());
		pParameters->Set_Enabled("SUMMARY"    , pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Detection::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	 One;

	CSG_Grid	*pOne	= Parameters("ONE")->asGrid();

	if( !Get_Classes(One, pOne, true) )
	{
		Error_Set(_TL("no class definitions for initial state"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	Two;

	CSG_Grid	*pTwo	= Parameters("TWO")->asGrid();

	if( !Get_Classes(Two, pTwo, false) )
	{
		Error_Set(_TL("no class definitions for final state"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pConfusion		= Parameters("CONFUSION")->asTable();

	bool	bUnclassified	= Parameters("NODATA")->asBool();

	int	nOne	= One.Get_Count(); if( bUnclassified ) nOne++;
	int	nTwo	= Two.Get_Count(); if( bUnclassified ) nTwo++;

	CSG_Matrix	Identity;

	if( !Get_Changes(One, Two, pConfusion, Identity, bUnclassified) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pCombined	= Parameters("COMBINED")->asGrid();

	bool	bNoChange	= Parameters("NOCHANGE")->asBool();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Value	= -1;

			int	iOne	= Get_Class(One, pOne->asDouble(x, y));
			int	iTwo	= Get_Class(Two, pTwo->asDouble(x, y));

			if( bUnclassified || (iOne < One.Get_Count() && iTwo < Two.Get_Count()) )
			{
				if( bNoChange || !Identity[iOne][iTwo] )
				{
					pConfusion->Get_Record(iOne)->Add_Value(1 + iTwo, 1);

					Value	= nTwo * iOne + iTwo;
				}
			}

			pCombined->Set_Value(x, y, Value);
		}
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pCombined, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		CSG_Colors	cRandom(nOne);	cRandom.Random();

		for(int iOne=0; iOne<nOne; iOne++)
		{
			CSG_Colors	cRamp(nTwo);

			cRamp.Set_Ramp(cRandom[iOne], cRandom[iOne]);
			cRamp.Set_Ramp_Brighness(225, 50);

			for(int iTwo=0; iTwo<nTwo; iTwo++)
			{
				if( pConfusion->Get_Record(iOne)->asInt(1 + iTwo) > 0 )
				{
					CSG_Table_Record	*pClass	= pLUT->asTable()->Add_Record();

					pClass->Set_Value(0, cRamp.Get_Color(iTwo));
					pClass->Set_Value(1, CSG_String::Format("%s >> %s", pConfusion->Get_Record(iOne)->asString(0), pConfusion->Get_Field_Name(1 + iTwo)));
					pClass->Set_Value(3, nTwo * iOne + iTwo);
					pClass->Set_Value(4, nTwo * iOne + iTwo);
				}
			}
		}

		DataObject_Set_Parameter(pCombined, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(pCombined, pLUT);
	}

	//-----------------------------------------------------
	double	Factor;

	switch( Parameters("OUTPUT")->asInt() )
	{
	default:	Factor	= 1.0;					break;	// cells
	case  1:	Factor	= 100.0 / Get_NCells();	break;	// percent
	case  2:	Factor	= Get_Cellarea();		break;	// area
	}

	if( Factor != 1.0 )
	{
		for(int iOne=0; iOne<nOne; iOne++)
		{
			for(int iTwo=0; iTwo<nTwo; iTwo++)
			{
				pConfusion->Get_Record(iOne)->Mul_Value(1 + iTwo, Factor);
			}
		}
	}

	//-----------------------------------------------------
	if( bNoChange )
	{
		CSG_Table	*pClasses	= Parameters("CLASSES")->asTable();
		CSG_Table	*pSummary	= Parameters("SUMMARY")->asTable();

		Get_Quality(*pConfusion, *pClasses, *pSummary);

		pClasses->Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Class Values"), pOne->Get_Name(), pTwo->Get_Name()));
		pSummary->Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Summary"     ), pOne->Get_Name(), pTwo->Get_Name()));
	}

	//-----------------------------------------------------
	pConfusion->Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Confusion"   ), pOne->Get_Name(), pTwo->Get_Name()));
	pCombined ->Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Combination" ), pOne->Get_Name(), pTwo->Get_Name()));
	pCombined ->Set_NoData_Value(-1);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Detection::Get_Quality(CSG_Table &Confusion, CSG_Table &Classes, CSG_Table &Summary)
{
	if( Confusion.Get_Count() != Confusion.Get_Field_Count() - 1 )
	{
		Message_Add(_TL("Quality assessment is not calculated, because the number of classes in the input grids differ.")); 

		return( false );
	}

	int	nClasses	= Confusion.Get_Count();

	//-----------------------------------------------------
	Classes.Destroy();
	Classes.Add_Field("Class"        , SG_DATATYPE_String);
	Classes.Add_Field("SumRef"       , SG_DATATYPE_Int);
	Classes.Add_Field("AccProd"      , SG_DATATYPE_Double);
	Classes.Add_Field("SumClassified", SG_DATATYPE_Int);
	Classes.Add_Field("AccUser"      , SG_DATATYPE_Double);
	Classes.Set_Record_Count(nClasses);

	Confusion.Add_Field("SumUser", SG_DATATYPE_Double);
	Confusion.Add_Field("AccUser", SG_DATATYPE_Double);

	Confusion.Add_Record()->Set_Value(0, "SumProd");
	Confusion.Add_Record()->Set_Value(0, "AccProd");

	Confusion[Confusion.Get_Count() - 1].Set_NoData(Confusion.Get_Field_Count() - 1);
	Confusion[Confusion.Get_Count() - 1].Set_NoData(Confusion.Get_Field_Count() - 2);
	Confusion[Confusion.Get_Count() - 2].Set_NoData(Confusion.Get_Field_Count() - 1);
	Confusion[Confusion.Get_Count() - 2].Set_NoData(Confusion.Get_Field_Count() - 2);

	//-----------------------------------------------------
	sLong	nTotal = 0, nTrue = 0, nProd = 0;

	for(int i=0, n; i<Classes.Get_Count(); i++)
	{
		sLong	nOne	= 0;
		sLong	nTwo	= 0;

		for(int j=0; j<Classes.Get_Count(); j++)
		{
			nOne	+= Confusion[j].asLong(1 + i);
			nTwo	+= Confusion[i].asLong(1 + j);
		}

		nTotal	+= nOne;
		nTrue	+= Confusion[i].asLong(1 + i);
		nProd	+= nOne * nTwo;

		double	AccOne	= nOne < 1 ? -1.0 : Confusion[i].asLong(1 + i) / (double)nOne;
		double	AccTwo	= nTwo < 1 ? -1.0 : Confusion[i].asLong(1 + i) / (double)nTwo;

		Classes[i].Set_Value(0, Confusion[i].asString(0));
		Classes[i].Set_Value(1, nOne); if( AccOne < 0 ) Classes[i].Set_NoData(1); else
		Classes[i].Set_Value(2, AccOne);
		Classes[i].Set_Value(3, nTwo); if( AccTwo < 0 ) Classes[i].Set_NoData(3); else
		Classes[i].Set_Value(4, AccTwo);

		n	= Confusion.Get_Count() - 2;
		Confusion[n + 0].Set_Value (1 + i, nOne); if( AccOne < 0.0 )
		Confusion[n + 1].Set_NoData(1 + i); else
		Confusion[n + 1].Set_Value (1 + i, AccOne * 100.0);

		n	= Confusion.Get_Field_Count() - 2;
		Confusion[i    ].Set_Value (n + 0, nTwo); if( AccTwo < 0.0 )
		Confusion[i    ].Set_NoData(n + 1); else
		Confusion[i    ].Set_Value (n + 1, AccTwo * 100.0);
	}

	//-----------------------------------------------------
	Summary.Destroy();
	Summary.Add_Field("NAME" , SG_DATATYPE_String);
	Summary.Add_Field("VALUE", SG_DATATYPE_Double);
	Summary.Set_Record_Count(2);

	double	k	= nTotal*nTotal - nProd, OA = 0.0;

	if( k != 0.0 )
	{
		Summary[0].Set_Value(0, "Kappa"           ); Summary[0].Set_Value(1, k  = (nTotal * nTrue - nProd) / k);
		Summary[1].Set_Value(0, "Overall Accuracy"); Summary[1].Set_Value(1, OA = nTrue / (double)nTotal);

		Message_Add(CSG_String::Format("\n%s: %f", _TL("Kappa"           ), k ), false);
		Message_Add(CSG_String::Format("\n%s: %f", _TL("Overall Accuracy"), OA), false);

		return( true );
	}

	return( false );
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

	Classes.Add_Field(_TL("NAME"), SG_DATATYPE_String);
	Classes.Add_Field(_TL("MIN" ), SG_DATATYPE_Double);
	Classes.Add_Field(_TL("MAX" ), SG_DATATYPE_Double);

	//-----------------------------------------------------
	if( (pClasses = Parameters(bInitial ? "ONE_LUT" : "TWO_LUT")->asTable()) != NULL )
	{
		int	fNam	= Parameters(bInitial ? "ONE_LUT_NAM" : "TWO_LUT_NAM")->asInt();
		int	fMin	= Parameters(bInitial ? "ONE_LUT_MIN" : "TWO_LUT_MIN")->asInt();
		int	fMax	= Parameters(bInitial ? "ONE_LUT_MAX" : "TWO_LUT_MAX")->asInt();

		if( fNam < 0 || fNam >= pClasses->Get_Field_Count() )	{	fNam	= fMin;	}
		if( fMax < 0 || fMax >= pClasses->Get_Field_Count() )	{	fMax	= fMin;	}

		pClasses->Set_Index(fMin, TABLE_INDEX_Ascending);

		for(int iClass=0; iClass<pClasses->Get_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= Classes.Add_Record();

			pClass->Set_Value(CLASS_NAM, (*pClasses)[iClass].asString(fNam));
			pClass->Set_Value(CLASS_MIN, (*pClasses)[iClass].asDouble(fMin));
			pClass->Set_Value(CLASS_MAX, (*pClasses)[iClass].asDouble(fMax));
		}

		pClasses->Del_Index();
	}

	//-----------------------------------------------------
	else if( DataObject_Get_Parameter(pGrid, "LUT") && (pClasses = DataObject_Get_Parameter(pGrid, "LUT")->asTable()) != NULL )
	{
		pClasses->Set_Index(3, TABLE_INDEX_Ascending);

		for(int iClass=0; iClass<pClasses->Get_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= Classes.Add_Record();

			pClass->Set_Value(CLASS_NAM, (*pClasses)[iClass].asString(1));
			pClass->Set_Value(CLASS_MIN, (*pClasses)[iClass].asDouble(3));
			pClass->Set_Value(CLASS_MAX, (*pClasses)[iClass].asDouble(4));
		}

		pClasses->Del_Index();
	}

	//-----------------------------------------------------
	else
	{
		if( !pGrid->Set_Index() )
		{
			Error_Set(_TL("index creation failed"));

			return( false );
		}

		double	z;

		for(sLong i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
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
	if( Classes.Get_Index_Field(0) != CLASS_MIN || Classes.Get_Index_Order(0) != TABLE_INDEX_Ascending )
	{
		Classes.Set_Index(CLASS_MIN, TABLE_INDEX_Ascending);
	}

	return( Classes.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Detection::Get_Changes(CSG_Table &One, CSG_Table &Two, CSG_Table *pConfusion, CSG_Matrix &Identity, bool bUnclassified)
{
	int		iOne, iTwo;

	//-----------------------------------------------------
	Identity.Create(Two.Get_Count() + 1, One.Get_Count() + 1);

	for(iOne=0; iOne<One.Get_Count(); iOne++)
	{
		CSG_String	s	= One[iOne].asString(CLASS_NAM);

		for(iTwo=0; iTwo<Two.Get_Count(); iTwo++)
		{
			Identity[iOne][iTwo]	= s.Cmp(Two[iTwo].asString(CLASS_NAM)) ? 0 : 1;
		}
	}

	Identity[One.Get_Count()][Two.Get_Count()]	= 1;	// unclassified

	//-----------------------------------------------------
	pConfusion->Destroy();

	pConfusion->Add_Field(_TL("Name"), SG_DATATYPE_String);

	for(iTwo=0; iTwo<Two.Get_Count(); iTwo++)
	{
		pConfusion->Add_Field(Two[iTwo].asString(CLASS_NAM), SG_DATATYPE_Double);
	}

	if( bUnclassified )
	{
		pConfusion->Add_Field(_TL("Unclassified"), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(iOne=0; iOne<One.Get_Count(); iOne++)
	{
		pConfusion->Add_Record()->Set_Value(0, One[iOne].asString(CLASS_NAM));
	}

	if( bUnclassified )
	{
		pConfusion->Add_Record()->Set_Value(0, _TL("Unclassified"));
	}

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
