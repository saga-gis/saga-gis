
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Classify_Supervised.cpp             //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include <string.h>

#include "Grid_Classify_Supervised.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASS_NR			= 0,
	CLASS_ID,
	CLASS_N,
	CLASS_M,
	CLASS_S
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Classify_Supervised::CGrid_Classify_Supervised(void)
{
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Supervised Classification"));

	Set_Author(_TL("Copyrights (c) 2005 by Olaf Conrad"));

	Set_Description(_TL("Supervised Classification: Minimum Distance, Maximum Likelihood.\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"		, _TL("Training Areas"),
		"",
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"			, _TL("Class Identifier"),
		""
	);

	Parameters.Add_Table(
		NULL	, "CLASSES"			, _TL("Class Information"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL	, "LUT"				, _TL("Lookup Table"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"			, _TL("Classification"),
		"",
		PARAMETER_OUTPUT, true, GRID_TYPE_Char
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		"",
		CSG_String::Format("%s|%s|",
			_TL("Minimum Distance"),
			_TL("Maximum Likelihood")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "NORMALISE"		, _TL("Normalise"),
		_TL("Automatically normalise grids before classifying."),
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
CGrid_Classify_Supervised::~CGrid_Classify_Supervised(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::On_Execute(void)
{
	bool					bResult	= false;
	int						iField;
	CTable					*pClasses, *pLUT;
	CShapes					*pPolygons;
	CGrid					*pGrid, *pResult;
	CParameter_Grid_List	*pGrids;
	CParameters				P;

	//-----------------------------------------------------
	iField		= Parameters("FIELD")		->asInt();
	pClasses	= Parameters("CLASSES")		->asTable();
	pLUT		= Parameters("LUT")			->asTable();
	pPolygons	= Parameters("POLYGONS")	->asShapes();
 	pResult		= Parameters("RESULT")		->asGrid();
	pGrids		= Parameters("GRIDS")		->asGridList();

	//-------------------------------------------------
	if( Parameters("NORMALISE")->asBool() )
	{
		P.Add_Grid_List(NULL, "GRIDS", "", "", PARAMETER_INPUT, false);

		for(int i=0; i<pGrids->Get_Count(); i++)
		{
			pGrid	= SG_Create_Grid(pGrids->asGrid(i), GRID_TYPE_Float);
			pGrid->Assign(pGrids->asGrid(i));
			pGrid->Normalise();
			P("GRIDS")->asGridList()->Add_Item(pGrid);
		}

		pGrids	= P("GRIDS")->asGridList();
	}

	//-----------------------------------------------------
	if( Get_Class_Information(pGrids, pPolygons, iField, pClasses, pLUT) )
	{
		switch( Parameters("METHOD")->asInt() )
		{
		case 0:	default:
			bResult	= Do_Minimum_Distance	(pGrids, pClasses, pResult);
			break;

		case 1:
			bResult	= Do_Maximum_Likelihood	(pGrids, pClasses, pResult);
			break;
		}
	}

	//-------------------------------------------------
	if( Parameters("NORMALISE")->asBool() )
	{
		for(int i=0; i<pGrids->Get_Count(); i++)
		{
			delete(pGrids->asGrid(i));
		}
	}

	//-------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Get_Class_Information(CParameter_Grid_List *pGrids, CShapes *pPolygons, int iField, CTable *pClasses, CTable *pLUT)
{
	int				x, y, iGrid, iClass, iPolygon;
	double			d, n;
	TSG_Point		p;
	CTable_Record	*pClass;
	CShape_Polygon	*pPolygon;

	//-----------------------------------------------------
	if( pPolygons->Get_Type() == SHAPE_TYPE_Polygon )
	{
		pClasses->Destroy();
		pClasses->Set_Name(_TL("Class Information"));

		pClasses->Add_Field(_TL("NR")			, TABLE_FIELDTYPE_Int);
		pClasses->Add_Field(_TL("IDENTIFIER")	, TABLE_FIELDTYPE_String);
		pClasses->Add_Field(_TL("ELEMENTS")		, TABLE_FIELDTYPE_Int);

		for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			pClasses->Add_Field(CSG_String::Format(_TL("MEAN_%02d")  , iGrid + 1), TABLE_FIELDTYPE_Double);
			pClasses->Add_Field(CSG_String::Format(_TL("STDDEV_%02d"), iGrid + 1), TABLE_FIELDTYPE_Double);
		}

		//-------------------------------------------------
		for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
		{
			for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
			{
				for(iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
				{
					pPolygon	= (CShape_Polygon *)pPolygons->Get_Shape(iPolygon);

					if( pPolygon->is_Containing(p) && (pClass = Get_Class(pClasses, pPolygon->Get_Record()->asString(iField))) != NULL )
					{
						pClass->Add_Value(CLASS_N, 1.0);

						for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
						{
							d	= pGrids->asGrid(iGrid)->asDouble(x, y);

							pClass->Add_Value(CLASS_M + 2 * iGrid, d);
							pClass->Add_Value(CLASS_S + 2 * iGrid, d * d);
						}
					}
				}
			}
		}

		//-------------------------------------------------
		for(iClass=0; iClass<pClasses->Get_Record_Count(); iClass++)
		{
			pClass	= pClasses->Get_Record(iClass);
			n		= pClass->asDouble(CLASS_N);

			for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
			{
				d	= pClass->asDouble(CLASS_M + 2 * iGrid) / n;			// arithmetic mean
				pClass->Set_Value(CLASS_M + 2 * iGrid, d);

				d	= pClass->asDouble(CLASS_S + 2 * iGrid) / n - d * d;	// variance
				pClass->Set_Value(CLASS_S + 2 * iGrid, sqrt(d));			// standard deviation
			}
		}

		//-------------------------------------------------
		if( pClasses->Get_Record_Count() > 1 )
		{
			if( pLUT )
			{
				pLUT->Destroy();
				pLUT->Set_Name(_TL("Lookup Table"));

				pLUT->Add_Field(_TL("COLOR")			, TABLE_FIELDTYPE_Color);
				pLUT->Add_Field(_TL("NAME")			, TABLE_FIELDTYPE_String);
				pLUT->Add_Field(_TL("DESCRIPTION")	, TABLE_FIELDTYPE_String);
				pLUT->Add_Field(_TL("MINIMUM")		, TABLE_FIELDTYPE_Double);
				pLUT->Add_Field(_TL("MAXIMUM")		, TABLE_FIELDTYPE_Double);

				for(iClass=0; iClass<pClasses->Get_Record_Count(); iClass++)
				{
					pClass	= pLUT->Add_Record();

					pClass->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
					pClass->Set_Value(1, pClasses->Get_Record(iClass)->asString(CLASS_ID));
					pClass->Set_Value(2, pClasses->Get_Record(iClass)->asString(CLASS_ID));
					pClass->Set_Value(3, iClass + 1);
					pClass->Set_Value(4, iClass + 1);
				}
			}

			//---------------------------------------------
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Record * CGrid_Classify_Supervised::Get_Class(CTable *pClasses, const char *Identifier)
{
	int				i;
	CTable_Record	*pClass;

	if( pClasses && Identifier )
	{
		for(i=0; i<pClasses->Get_Record_Count(); i++)
		{
			pClass	= pClasses->Get_Record(i);

			if( !strcmp(pClass->asString(CLASS_ID), Identifier) )
			{
				return( pClass );
			}
		}

		//-------------------------------------------------
		pClass	= pClasses->Add_Record();

		pClass->Set_Value(CLASS_NR	, pClasses->Get_Record_Count());
		pClass->Set_Value(CLASS_ID	, Identifier);
		pClass->Set_Value(CLASS_N	, 0.0);

		for(i=CLASS_M; i<pClasses->Get_Field_Count(); i++)
		{
			pClass->Set_Value(i, 0.0);
		}

		return( pClass );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Do_Minimum_Distance(CParameter_Grid_List *pGrids, CTable *pClasses, CGrid *pResult)
{
	int		x, y, iClass, iGrid, iMin;
	double	dMin, d, e, **m;

	//-----------------------------------------------------
	m		= (double **)SG_Malloc(sizeof(double *) * pClasses->Get_Record_Count());
	m[0]	= (double  *)SG_Malloc(sizeof(double  ) * pClasses->Get_Record_Count() * pGrids->Get_Count());

	for(iClass=0; iClass<pClasses->Get_Record_Count(); iClass++)
	{
		m[iClass]	= m[0] + iClass * pGrids->Get_Count();

		for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			m[iClass][iGrid]	= pClasses->Get_Record(iClass)->asDouble(CLASS_M + 2 * iGrid);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			for(iClass=0, dMin=-1.0; iClass<pClasses->Get_Record_Count(); iClass++)
			{
				for(iGrid=0, d=0.0; iGrid<pGrids->Get_Count(); iGrid++)
				{
					e	= pGrids->asGrid(iGrid)->asDouble(x, y) - m[iClass][iGrid];
					d	+= e * e;
				}

				if( dMin < 0.0 || dMin > d )
				{
					dMin	= d;
					iMin	= iClass;
				}
			}

			if( dMin >= 0.0 )
			{
				pResult->Set_Value(x, y, iMin + 1);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(m[0]);
	SG_Free(m);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Do_Maximum_Likelihood(CParameter_Grid_List *pGrids, CTable *pClasses, CGrid *pResult)
{
	int		x, y, iClass, iGrid, iMax;
	double	dMax, d, e, **m, **s, **k;

	//-----------------------------------------------------
	m		= (double **)SG_Malloc(sizeof(double *) * pClasses->Get_Record_Count());
	m[0]	= (double  *)SG_Malloc(sizeof(double  ) * pClasses->Get_Record_Count() * pGrids->Get_Count());
	s		= (double **)SG_Malloc(sizeof(double *) * pClasses->Get_Record_Count());
	s[0]	= (double  *)SG_Malloc(sizeof(double  ) * pClasses->Get_Record_Count() * pGrids->Get_Count());
	k		= (double **)SG_Malloc(sizeof(double *) * pClasses->Get_Record_Count());
	k[0]	= (double  *)SG_Malloc(sizeof(double  ) * pClasses->Get_Record_Count() * pGrids->Get_Count());

	for(iClass=0; iClass<pClasses->Get_Record_Count(); iClass++)
	{
		m[iClass]	= m[0] + iClass * pGrids->Get_Count();
		s[iClass]	= s[0] + iClass * pGrids->Get_Count();
		k[iClass]	= k[0] + iClass * pGrids->Get_Count();

		for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			m[iClass][iGrid]	= pClasses->Get_Record(iClass)->asDouble(CLASS_M + 2 * iGrid);	// arithmetic mean
			s[iClass][iGrid]	= pClasses->Get_Record(iClass)->asDouble(CLASS_S + 2 * iGrid);	// standard deviation
			k[iClass][iGrid]	= 1.0 / (s[iClass][iGrid] * sqrt(2.0 * M_PI));
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			for(iClass=0, dMax=0.0; iClass<pClasses->Get_Record_Count(); iClass++)
			{
				for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
				{
					e	= (pGrids->asGrid(iGrid)->asDouble(x, y) - m[iClass][iGrid]) / s[iClass][iGrid];
					e	= k[iClass][iGrid] * exp(-0.5 * e * e);
					d	= iGrid == 0 ? e : d * e;
				}

				if( dMax < d )
				{
					dMax	= d;
					iMax	= iClass;
				}
			}

			if( dMax > 0.0 )
			{
				pResult->Set_Value(x, y, iMax + 1);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(m[0]);
	SG_Free(m);
	SG_Free(s[0]);
	SG_Free(s);
	SG_Free(k[0]);
	SG_Free(k);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
