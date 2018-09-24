/**********************************************************
 * Version $Id: classification_quality.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//               classification_quality.cpp              //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "classification_quality.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CClassification_Quality::CClassification_Quality(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Confusion Matrix (Polygons / Grid)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Compares a classified polygons layer with grid classes and "
		"creates a confusion matrix and derived coefficients. "
		"Grid classes have to be defined with a look-up table and "
		"values must match those of the polygon classes for the "
		"subsequent comparison. "
		"This tool is typically used for a quality assessment of a "
		"supervised classification. "
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	pNode	= Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Classification"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		pNode	, "GRID_VALUES"	, _TL("Value Interpretation"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("values are class identifiers"),
			_TL("use look-up table")
		), 1
	);

	pNode	= Parameters.Add_Table(pNode, "GRID_LUT", _TL("Look-up Table"  ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Table_Field(pNode, "GRID_LUT_MIN", _TL("Value"          ), _TL(""), false);
	Parameters.Add_Table_Field(pNode, "GRID_LUT_MAX", _TL("Value (Maximum)"), _TL(""), true );
	Parameters.Add_Table_Field(pNode, "GRID_LUT_NAM", _TL("Name"           ), _TL(""), true );

	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Classes"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "CONFUSION"	, _TL("Confusion Matrix"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL	, "CLASSES"		, _TL("Class Values"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL	, "SUMMARY"		, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		NULL	, "NO_CLASS"	, _TL("Unclassified"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClassification_Quality::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("GRID_VALUES") )
	{
		pParameters->Set_Enabled("GRID_LUT", pParameter->asInt() == 1);
	}

	if(	pParameter->Cmp_Identifier("GRID_LUT") )
	{
		pParameters->Set_Enabled("GRID_LUT_MIN", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("GRID_LUT_MAX", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("GRID_LUT_NAM", pParameter->asTable() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CClassification_Quality::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( !pPolygons->is_Valid() )
	{
		Error_Set(_TL("invalid polygons"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	&Confusion	= *Parameters("CONFUSION")->asTable();

	bool	bUnclassified	= Parameters("NO_CLASS")->asBool();

	int		Field	= Parameters("FIELD")->asInt();

	if( !Get_Classes(pPolygons, Field, Confusion, bUnclassified) )
	{
		Error_Set(_TL("invalid polygons"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( !Get_Classes(pGrid, &Confusion) )
	{
		Error_Set(_TL("no class overlap between polygons and grid"));

		return( false );
	}

	//-----------------------------------------------------
	if( bUnclassified )
	{
		Confusion.Add_Record()->Set_Value(0, _TL("Unclassified"));
	}

	Confusion.Add_Field("SumUser", SG_DATATYPE_Double);
	Confusion.Add_Field("AccUser", SG_DATATYPE_Double);

	Confusion.Add_Record()->Set_Value(0, "SumProd");
	Confusion.Add_Record()->Set_Value(0, "AccProd");

	Confusion[Confusion.Get_Count() - 1].Set_NoData(Confusion.Get_Field_Count() - 1);
	Confusion[Confusion.Get_Count() - 1].Set_NoData(Confusion.Get_Field_Count() - 2);
	Confusion[Confusion.Get_Count() - 2].Set_NoData(Confusion.Get_Field_Count() - 1);
	Confusion[Confusion.Get_Count() - 2].Set_NoData(Confusion.Get_Field_Count() - 2);

	Confusion.Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Confusion Matrix"), pPolygons->Get_Name(), pGrid->Get_Name()));

	//-----------------------------------------------------
	TSG_Point	p;	p.y	= Get_YMin();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		p.x	= Get_XMin();

		for(int x=0; x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			int	iGrid	= Get_Class(pGrid->asDouble(x, y));

			if( bUnclassified || iGrid < m_Classes.Get_Count() )
			{
				for(int iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
				{
					CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

					if( pPolygon->Contains(p) )
					{
						int	iPolygon	= Get_Class(pPolygon->asString(Field));

						if( iPolygon >= 0 )
						{
							Confusion[iGrid].Add_Value(1 + iPolygon, 1);
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Table	&Classes	= *Parameters("CLASSES")->asTable();

	Classes.Destroy();
	Classes.Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Class Values"), pPolygons->Get_Name(), pGrid->Get_Name()));
	Classes.Add_Field("Class"        , SG_DATATYPE_String);
	Classes.Add_Field("SumRef"       , SG_DATATYPE_Int);
	Classes.Add_Field("AccProd"      , SG_DATATYPE_Double);
	Classes.Add_Field("SumClassified", SG_DATATYPE_Int);
	Classes.Add_Field("AccUser"      , SG_DATATYPE_Double);
	Classes.Set_Record_Count(m_Classes.Get_Count());

	sLong	nTotal = 0, nTrue = 0, nProd = 0;

	for(int i=0, n; i<m_Classes.Get_Count(); i++)
	{
		sLong	nPoly	= 0;
		sLong	nGrid	= 0;

		for(int j=0; j<m_Classes.Get_Count(); j++)
		{
			nPoly	+= Confusion[j].asLong(1 + i);
			nGrid	+= Confusion[i].asLong(1 + j);
		}

		nTotal	+= nPoly;
		nTrue	+= Confusion[i].asLong(1 + i);
		nProd	+= nPoly * nGrid;

		double	AccProd	= nPoly < 1 ? -1.0 : Confusion[i].asLong(1 + i) / (double)nPoly;
		double	AccUser	= nGrid < 1 ? -1.0 : Confusion[i].asLong(1 + i) / (double)nGrid;

		Classes[i].Set_Value(0, m_Classes[i].asString(0));
		Classes[i].Set_Value(1, nPoly); if( AccProd < 0.0 ) Classes[i].Set_NoData(2); else
		Classes[i].Set_Value(2, AccProd);
		Classes[i].Set_Value(3, nGrid); if( AccUser < 0.0 ) Classes[i].Set_NoData(4); else
		Classes[i].Set_Value(4, AccUser);

		n	= Confusion.Get_Count() - 2;
		Confusion[n + 0].Set_Value (1 + i, nPoly); if( AccProd < 0.0 )
		Confusion[n + 1].Set_NoData(1 + i); else
		Confusion[n + 1].Set_Value (1 + i, AccProd * 100.0);

		n	= Confusion.Get_Field_Count() - 2;
		Confusion[i    ].Set_Value (n + 0, nGrid); if( AccUser < 0.0 )
		Confusion[i    ].Set_NoData(n + 1); else
		Confusion[i    ].Set_Value (n + 1, AccUser * 100.0);
	}

	//-----------------------------------------------------
	CSG_Table	&Summary	= *Parameters("SUMMARY")->asTable();

	Summary.Destroy();
	Summary.Set_Name(CSG_String::Format("%s [%s - %s]", _TL("Summary"), pPolygons->Get_Name(), pGrid->Get_Name()));
	Summary.Add_Field("NAME" , SG_DATATYPE_String);
	Summary.Add_Field("VALUE", SG_DATATYPE_Double);
	Summary.Set_Record_Count(2);

	double	k	= nTotal*nTotal - nProd, OA = 0.0;

	if( k != 0.0 )
	{
		Summary[0].Set_Value(0, "Kappa"           ); Summary[0].Set_Value(1, k  = (nTotal * nTrue - nProd) / k);
		Summary[1].Set_Value(0, "Overall Accuracy"); Summary[1].Set_Value(1, OA = nTrue / (double)nTotal);

		Message_Fmt("\n%s: %f", _TL("Kappa"           ), k );
		Message_Fmt("\n%s: %f", _TL("Overall Accuracy"), OA);
	}

	//-----------------------------------------------------
	m_Classes.Destroy();

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
bool CClassification_Quality::Get_Classes(CSG_Shapes *pPolygons, int Field, CSG_Table &Confusion, bool bUnclassified)
{
	CSG_Category_Statistics	Classes;

	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		Classes	+= pPolygons->Get_Shape(iPolygon)->asString(Field);
	}

	if( Classes.Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Classes.Destroy();

	m_Classes.Add_Field("NAM", SG_DATATYPE_String);
	m_Classes.Add_Field("MIN", SG_DATATYPE_Double);
	m_Classes.Add_Field("MAX", SG_DATATYPE_Double);

	Confusion.Destroy();

	Confusion.Add_Field("CLASS", pPolygons->Get_Field_Type(Field));

	for(int iClass=0; iClass<Classes.Get_Count(); iClass++)
	{
		CSG_String	Class(Classes.asString(iClass));

		Confusion.Add_Field(Class, SG_DATATYPE_Double);
		Confusion.Add_Record()->Set_Value(0, Class);

		m_Classes.Add_Record()->Set_Value(0, Class);
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CClassification_Quality::Get_Classes(CSG_Grid *pGrid, CSG_Table *pConfusion)
{
	if( Parameters("GRID_VALUES")->asInt() == 0 && !pConfusion )	// values are identifiers
	{
		for(int iClass=0; iClass<m_Classes.Get_Count(); iClass++)
		{
			m_Classes[iClass][CLASS_MIN]	= m_Classes[iClass].asDouble(CLASS_NAM);
			m_Classes[iClass][CLASS_MAX]	= m_Classes[iClass].asDouble(CLASS_NAM);
		}

		return( true );
	}

	//-----------------------------------------------------
	int	fNam, fMin, fMax;

	CSG_Table	*pLUT, m_LUT;

	if( Parameters("GRID_VALUES")->asInt() == 0 )
	{
		CSG_Category_Statistics	Categories;

		for(sLong iCell=0; iCell<pGrid->Get_NCells(); iCell++)
		{
			if( !pGrid->is_NoData(iCell) )
			{
				Categories	+= SG_Get_String(pGrid->asDouble(iCell), -10);
			}
		}

		m_LUT.Add_Field("VAL", pGrid->Get_Type());	fNam = fMin = fMax = 0;

		for(int iCategory=0; iCategory<Categories.Get_Count(); iCategory++)
		{
			m_LUT.Add_Record()->Set_Value(0, Categories.asString(iCategory));
		}

		pLUT	= &m_LUT;
	}
	else if( (pLUT = Parameters("GRID_LUT")->asTable()) != NULL )
	{
		fNam	= Parameters("GRID_LUT_NAM")->asInt();
		fMin	= Parameters("GRID_LUT_MIN")->asInt();
		fMax	= Parameters("GRID_LUT_MAX")->asInt();

		if( fNam < 0 || fNam >= pLUT->Get_Field_Count() )	{	fNam	= fMin;	}
		if( fMax < 0 || fMax >= pLUT->Get_Field_Count() )	{	fMax	= fMin;	}
	}
	else if( DataObject_Get_Parameter(pGrid, "LUT") && (pLUT = DataObject_Get_Parameter(pGrid, "LUT")->asTable()) != NULL )
	{
		fNam	= 1;
		fMin	= 3;
		fMax	= 4;
	}
	else
	{
		return( false );
	}

	//-----------------------------------------------------
	int		nMatches	= 0;

	for(int iClass=0; iClass<pLUT->Get_Count(); iClass++)
	{
		CSG_String	Class(pLUT->Get_Record(iClass)->asString(fNam, -10));

		CSG_Table_Record	*pClass	= m_Classes.Get_Record(Get_Class(Class));

		if( pClass )
		{
			nMatches	++;
		}
		else if( pConfusion )
		{
			pClass	= m_Classes.Add_Record();

			pConfusion->Add_Field(Class, SG_DATATYPE_Double);
			pConfusion->Add_Record()->Set_Value(0, Class);

			pClass->Set_Value(0, Class);
		}

		if( pClass )
		{
			double	min	= pLUT->Get_Record(iClass)->asDouble(fMin);
			double	max	= pLUT->Get_Record(iClass)->asDouble(fMax);

			pClass->Set_Value(CLASS_MIN, min);
			pClass->Set_Value(CLASS_MAX, min < max ? max : min);
		}
	}

	//-----------------------------------------------------
	return( nMatches > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CClassification_Quality::Get_Class(const CSG_String &Value)
{
	for(int i=0; i<m_Classes.Get_Count(); i++)
	{
		if( !Value.Cmp(m_Classes[i].asString(CLASS_NAM)) )
		{
			return( i );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
int CClassification_Quality::Get_Class(double Value)
{
	for(int i=0; i<m_Classes.Get_Count(); i++)
	{
		double	min	= m_Classes[i].asDouble(CLASS_MIN);
		double	max	= m_Classes[i].asDouble(CLASS_MAX);

		if( min <= Value && Value <= max )
		{
			return( i );
		}
	}

	return( m_Classes.Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
