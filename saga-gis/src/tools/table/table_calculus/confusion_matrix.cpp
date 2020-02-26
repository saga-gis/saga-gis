
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  confusion_matrix.cpp                 //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include "confusion_matrix.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CConfusion_Matrix::CConfusion_Matrix(void)
{
	Set_Name		(_TL("Confusion Matrix (Table Fields)"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"Compares record-wise the values of two table fields and "
		"creates a confusion matrix and derived coefficients. "
		"Values are expected to represent categories. "
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD_1"	, _TL("Categories 1"),
		_TL("")
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD_2"	, _TL("Categories 2"),
		_TL("")
	);

	Parameters.Add_Table("",
		"CONFUSION"	, _TL("Confusion Matrix"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"CLASSES"	, _TL("Categories"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"SUMMARY"	, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"NODATA"	, _TL("Include No-Data"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CConfusion_Matrix::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConfusion_Matrix::On_Execute(void)
{
	CSG_Table	&Table	= *Parameters("TABLE")->asTable();

	bool	bNoData	= Parameters("NODATA")->asBool();

	int	Field[2];

	Field[0]	= Parameters("FIELD_1")->asInt();
	Field[1]	= Parameters("FIELD_2")->asInt();

	//-----------------------------------------------------
	// find all categories...

	CSG_Category_Statistics	Categories;

	for(int i=0; i<Table.Get_Count() && Set_Progress(i, Table.Get_Count()); i++)
	{
		if( bNoData || !Table[i].is_NoData(Field[0]) )
		{
			Categories += Table[i].asString(Field[0]);
		}

		if( bNoData || !Table[i].is_NoData(Field[1]) )
		{
			Categories += Table[i].asString(Field[1]);
		}
	}

	//-----------------------------------------------------
	// create confusion matrix...

	CSG_Table	&Confusion	= *Parameters("CONFUSION")->asTable();

	Confusion.Destroy();
	Confusion.Fmt_Name("%s [%s - %s]", _TL("Confusion"), Table.Get_Field_Name(Field[0]), Table.Get_Field_Name(Field[1]));
	Confusion.Add_Field(_TL("Name"), SG_DATATYPE_String);

	for(int i=0; i<Categories.Get_Count(); i++)
	{
		CSG_String	Category	= Categories.asString(i);

		if( Category.is_Empty() )
		{
			Category	= "<no data>";
		}

		Confusion.Add_Field(Category, SG_DATATYPE_Int);
		Confusion.Add_Record()->Set_Value(0, Category);
	}

	//-----------------------------------------------------
	// count associations..

	for(int i=0; i<Table.Get_Count() && Set_Progress(i, Table.Get_Count()); i++)
	{
		int	c0	= Categories.Get_Category(Table[i].asString(Field[0]));
		int	c1	= Categories.Get_Category(Table[i].asString(Field[1]));

		if( c0 >= 0 && c1 >= 0 )
		{
			Confusion[c0].Add_Value(1 + c1, 1.);
		}
	}

	//-----------------------------------------------------
	Get_Quality(Confusion);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConfusion_Matrix::Get_Quality(CSG_Table &Confusion)
{
	if( Confusion.Get_Count() != Confusion.Get_Field_Count() - 1 )
	{
		Message_Add(_TL("Quality assessment is not calculated, because the number of classes in the input grids differ.")); 

		return( false );
	}

	CSG_Table	&Classes	= *Parameters("CLASSES")->asTable();
	CSG_Table	&Summary	= *Parameters("SUMMARY")->asTable();

	const SG_Char	*Name[2];

	Name[0]	= Parameters("TABLE")->asTable()->Get_Field_Name(Parameters("FIELD_1")->asInt());
	Name[1]	= Parameters("TABLE")->asTable()->Get_Field_Name(Parameters("FIELD_2")->asInt());

	//-----------------------------------------------------
	Classes.Destroy();
	Classes.Fmt_Name("%s [%s - %s]", _TL("Categories"), Name[0], Name[1]);
	Classes.Add_Field("Class"        , SG_DATATYPE_String);
	Classes.Add_Field("SumRef"       , SG_DATATYPE_Int   );
	Classes.Add_Field("AccProd"      , SG_DATATYPE_Double);
	Classes.Add_Field("SumClassified", SG_DATATYPE_Int   );
	Classes.Add_Field("AccUser"      , SG_DATATYPE_Double);
	Classes.Set_Record_Count(Confusion.Get_Count());

	Confusion.Add_Field("SumUser", SG_DATATYPE_Double);
	Confusion.Add_Field("AccUser", SG_DATATYPE_Double);

	Confusion.Add_Record()->Set_Value(0, "SumProd");
	Confusion.Add_Record()->Set_Value(0, "AccProd");

	Confusion[Confusion.Get_Count() - 1].Set_NoData(Confusion.Get_Field_Count() - 1);
	Confusion[Confusion.Get_Count() - 1].Set_NoData(Confusion.Get_Field_Count() - 2);
	Confusion[Confusion.Get_Count() - 2].Set_NoData(Confusion.Get_Field_Count() - 1);
	Confusion[Confusion.Get_Count() - 2].Set_NoData(Confusion.Get_Field_Count() - 2);

	//-----------------------------------------------------
	int	nTotal = 0, nTrue = 0, nProd = 0;

	for(int i=0; i<Classes.Get_Count(); i++)
	{
		int n[2]; n[0] = n[1] = 0; double a[2];

		for(int j=0; j<Classes.Get_Count(); j++)
		{
			n[0] += Confusion[j].asInt(1 + i);
			n[1] += Confusion[i].asInt(1 + j);
		}

		nTotal	+= n[0];
		nTrue	+= Confusion[i].asInt(1 + i);
		nProd	+= n[0] * n[1];

		a[0]	= n[0] < 1 ? -1. : Confusion[i].asDouble(1 + i) / (double)n[0];
		a[1]	= n[1] < 1 ? -1. : Confusion[i].asDouble(1 + i) / (double)n[1];

		Classes[i].Set_Value(0, Confusion[i].asString(0));
		Classes[i].Set_Value(1, n[0]); if( a[0] < 0 ) Classes[i].Set_NoData(1); else
		Classes[i].Set_Value(2, a[0]);
		Classes[i].Set_Value(3, n[1]); if( a[1] < 0 ) Classes[i].Set_NoData(3); else
		Classes[i].Set_Value(4, a[1]);

		//-------------------------------------------------
		int j;

		j	= Confusion.Get_Count() - 2;
		Confusion[j + 0].Set_Value (1 + i, n[0]); if( a[0] < 0. )
		Confusion[j + 1].Set_NoData(1 + i); else
		Confusion[j + 1].Set_Value (1 + i, a[0] * 100.);

		j	= Confusion.Get_Field_Count() - 2;
		Confusion[i    ].Set_Value (j + 0, n[1]); if( a[1] < 0. )
		Confusion[i    ].Set_NoData(j + 1); else
		Confusion[i    ].Set_Value (j + 1, a[1] * 100.);
	}

	//-----------------------------------------------------
	Summary.Destroy();
	Summary.Fmt_Name("%s [%s - %s]", _TL("Summary"), Name[0], Name[1]);
	Summary.Add_Field("NAME" , SG_DATATYPE_String);
	Summary.Add_Field("VALUE", SG_DATATYPE_Double);
	Summary.Set_Record_Count(2);

	double	d	= (double)(nTotal*nTotal - nProd);

	if( d != 0. )
	{
		double	k	= (nTotal * nTrue - nProd) / d;
		double	a	= nTrue / (double)nTotal;

		Summary[0].Set_Value(0, "Kappa"           ); Summary[0].Set_Value(1, k);
		Summary[1].Set_Value(0, "Overall Accuracy"); Summary[1].Set_Value(1, a);

		Message_Fmt("\n%s: %f", _TL("Kappa"           ), k);
		Message_Fmt("\n%s: %f", _TL("Overall Accuracy"), a);

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
