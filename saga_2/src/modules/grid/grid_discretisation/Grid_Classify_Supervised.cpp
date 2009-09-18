
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
#define GET_GRID_VALUE(x, y, i)	(m_bNormalise ? (m_pGrids->asGrid(i)->asDouble(x, y) - m_pGrids->asGrid(i)->Get_ArithMean()) / sqrt(m_pGrids->asGrid(i)->Get_Variance()) : m_pGrids->asGrid(i)->asDouble(x, y))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Classify_Supervised::CGrid_Classify_Supervised(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Supervised Classification"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description	(_TW(
		"Supervised Classification: Minimum Distance, Maximum Likelihood.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"		, _TL("Training Areas"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"			, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "CLASSES"			, _TL("Class Information"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"			, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid(
		NULL	, "ML_PROB"			, _TL("Distance/Probability"),
		_TL("Dependent on chosen method, these are the calculated minimum distances or the maximum probabilities."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Minimum Distance"),
			_TL("Maximum Likelihood")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "NORMALISE"		, _TL("Normalise"),
		_TL("Automatically normalise grids before classifying. Useful for minimum distance classification."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "ML_THRESHOLD"	, _TL("Probability Threshold (Percent)"),
		_TL("Let pixel stay unclassified, if maximum likelihood probability is less than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
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
	bool	bResult	= false;

	//-------------------------------------------------
	m_pClasses		= Parameters("CLASSES")		->asTable();
	m_pGrids		= Parameters("GRIDS")		->asGridList();
 	m_pResult		= Parameters("RESULT")		->asGrid();
	m_bNormalise	= Parameters("NORMALISE")	->asBool();
	m_pProbability	= Parameters("ML_PROB")		->asGrid();
	m_ML_Threshold	= Parameters("ML_THRESHOLD")->asDouble();

	//-------------------------------------------------
	if( Initialise() )
	{
		switch( Parameters("METHOD")->asInt() )
		{
		case 0:	default:	bResult	= Set_Minimum_Distance();		break;
		case 1:				bResult	= Set_Maximum_Likelihood();		break;
		}

		Finalise();
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
bool CGrid_Classify_Supervised::Initialise(void)
{
	int					x, y, iGrid, iClass, iPolygon, iField;
	double				d, n;
	TSG_Point			p;
	CSG_Table_Record	*pClass;
	CSG_Shapes			*pPolygons;
	CSG_Shape_Polygon	*pPolygon;

	//-----------------------------------------------------
	for(iGrid=m_pGrids->Get_Count()-1; iGrid>=0; iGrid--)
	{
		if( m_pGrids->asGrid(iGrid)->Get_Variance() == 0.0 )
		{
			m_pGrids->Del_Item(iGrid);
		}
	}

	//-----------------------------------------------------
	if( m_pGrids->Get_Count() > 0 )
	{
		iField		= Parameters("FIELD")		->asInt();
		pPolygons	= Parameters("POLYGONS")	->asShapes();

		m_pClasses->Destroy();
		m_pClasses->Set_Name(_TL("Class Information"));

		m_pClasses->Add_Field(_TL("NR")			, SG_DATATYPE_Int);
		m_pClasses->Add_Field(_TL("IDENTIFIER")	, SG_DATATYPE_String);
		m_pClasses->Add_Field(_TL("ELEMENTS")	, SG_DATATYPE_Int);

		for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
		{
			m_pClasses->Add_Field(CSG_String::Format(_TL("MEAN_%02d")  , iGrid + 1), SG_DATATYPE_Double);
			m_pClasses->Add_Field(CSG_String::Format(_TL("STDDEV_%02d"), iGrid + 1), SG_DATATYPE_Double);
		}

		//-------------------------------------------------
		for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
		{
			for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
			{
				bool	bNoData;

				for(iGrid=0, bNoData=false; iGrid<m_pGrids->Get_Count() && !bNoData; iGrid++)
				{
					if( m_pGrids->asGrid(iGrid)->is_NoData(x, y) )
					{
						bNoData	= true;
					}
				}

				//-----------------------------------------
				if( bNoData )
				{
					m_pResult->Set_NoData(x, y);
				}
				else
				{
					m_pResult->Set_Value(x, y, 0.0);

					for(iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
					{
						pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

						if( pPolygon->is_Containing(p) && (pClass = Get_Class(pPolygon->asString(iField))) != NULL )
						{
							pClass->Add_Value(CLASS_N, 1.0);

							for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
							{
								d	= GET_GRID_VALUE(x, y, iGrid);

								pClass->Add_Value(CLASS_M + 2 * iGrid, d);
								pClass->Add_Value(CLASS_S + 2 * iGrid, d * d);
							}
						}
					}
				}
			}
		}

		//-------------------------------------------------
		for(iClass=0; iClass<m_pClasses->Get_Record_Count(); iClass++)
		{
			pClass	= m_pClasses->Get_Record(iClass);
			n		= pClass->asDouble(CLASS_N);

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				d	= pClass->asDouble	(CLASS_M + 2 * iGrid) / n;			// arithmetic mean
				pClass->Set_Value		(CLASS_M + 2 * iGrid, d);

				d	= pClass->asDouble	(CLASS_S + 2 * iGrid) / n - d * d;	// variance
				pClass->Set_Value		(CLASS_S + 2 * iGrid, sqrt(d));		// standard deviation
			}
		}

		//-------------------------------------------------
		if( m_pClasses->Get_Record_Count() > 1 )
		{
			CSG_Parameters	Parms;

			if( DataObject_Get_Parameters(m_pResult, Parms) && Parms("COLORS_TYPE") && Parms("LUT") )
			{
				CSG_Table	*pLUT	= Parms("LUT")->asTable();

				for(iClass=0; iClass<m_pClasses->Get_Record_Count(); iClass++)
				{
					if( (pClass = pLUT->Get_Record(iClass)) == NULL )
					{
						pClass	= pLUT->Add_Record();
						pClass->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
					}

					pClass->Set_Value(1, m_pClasses->Get_Record(iClass)->asString(CLASS_ID));
					pClass->Set_Value(2, m_pClasses->Get_Record(iClass)->asString(CLASS_ID));
					pClass->Set_Value(3, iClass + 1);
					pClass->Set_Value(4, iClass + 1);
				}

				while( pLUT->Get_Record_Count() > m_pClasses->Get_Record_Count() )
				{
					pLUT->Del_Record(pLUT->Get_Record_Count() - 1);
				}

				Parms("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

				DataObject_Set_Parameters(m_pResult, Parms);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Finalise(void)
{
	if( m_bNormalise )
	{
		for(int iClass=0; iClass<m_pClasses->Get_Record_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= m_pClasses->Get_Record(iClass);

			for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				double	d,
					s	= sqrt(m_pGrids->asGrid(iGrid)->Get_Variance()),
					m	=      m_pGrids->asGrid(iGrid)->Get_ArithMean();

				d	= pClass->asDouble	(CLASS_M + 2 * iGrid);
				pClass->Set_Value		(CLASS_M + 2 * iGrid, s * d + m);

				d	= pClass->asDouble	(CLASS_S + 2 * iGrid);
				pClass->Set_Value		(CLASS_S + 2 * iGrid, s * d);
			}
		}
	}

	if( m_pProbability )
	{
		DataObject_Set_Colors(m_pProbability, 100, SG_COLORS_WHITE_GREEN);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_Record * CGrid_Classify_Supervised::Get_Class(const SG_Char *Identifier)
{
	CSG_Table_Record	*pClass	= NULL;

	if( m_pClasses && Identifier )
	{
		int		i;

		for(i=0; i<m_pClasses->Get_Record_Count(); i++)
		{
			pClass	= m_pClasses->Get_Record(i);

			if( !SG_STR_CMP(pClass->asString(CLASS_ID), Identifier) )
			{
				return( pClass );
			}
		}

		//-------------------------------------------------
		pClass	= m_pClasses->Add_Record();

		pClass->Set_Value(CLASS_NR	, m_pClasses->Get_Record_Count());
		pClass->Set_Value(CLASS_ID	, Identifier);
		pClass->Set_Value(CLASS_N	, 0.0);

		for(i=CLASS_M; i<m_pClasses->Get_Field_Count(); i++)
		{
			pClass->Set_Value(i, 0.0);
		}
	}

	return( pClass );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Minimum_Distance(void)
{
	int		x, y, iClass, iGrid, iMin;
	double	dMin, d, e, **m;

	//-----------------------------------------------------
	m		= (double **)SG_Malloc(sizeof(double *) * m_pClasses->Get_Record_Count());
	m[0]	= (double  *)SG_Malloc(sizeof(double  ) * m_pClasses->Get_Record_Count() * m_pGrids->Get_Count());

	for(iClass=0; iClass<m_pClasses->Get_Record_Count(); iClass++)
	{
		m[iClass]	= m[0] + iClass * m_pGrids->Get_Count();

		for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
		{
			m[iClass][iGrid]	= m_pClasses->Get_Record(iClass)->asDouble(CLASS_M + 2 * iGrid);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !m_pResult->is_NoData(x, y) )
			{
				for(iClass=0, dMin=-1.0; iClass<m_pClasses->Get_Record_Count(); iClass++)
				{
					for(iGrid=0, d=0.0; iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						e	= GET_GRID_VALUE(x, y, iGrid) - m[iClass][iGrid];
						d	+= e*e;
					}

					if( dMin < 0.0 || dMin > d )
					{
						dMin	= d;
						iMin	= iClass;
					}
				}

				if( dMin >= 0.0 )
				{
					m_pResult->Set_Value(x, y, iMin + 1);

					if( m_pProbability )
					{
						m_pProbability->Set_Value(x, y, sqrt(dMin));
					}
				}
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
bool CGrid_Classify_Supervised::Set_Maximum_Likelihood(void)
{
	int		x, y, iClass, iGrid, iMax;
	double	dMax, d, e, **a, **b, **m;

	//-----------------------------------------------------
	a		= (double **)SG_Malloc(sizeof(double *) * m_pClasses->Get_Record_Count());
	a[0]	= (double  *)SG_Malloc(sizeof(double  ) * m_pClasses->Get_Record_Count() * m_pGrids->Get_Count());
	b		= (double **)SG_Malloc(sizeof(double *) * m_pClasses->Get_Record_Count());
	b[0]	= (double  *)SG_Malloc(sizeof(double  ) * m_pClasses->Get_Record_Count() * m_pGrids->Get_Count());
	m		= (double **)SG_Malloc(sizeof(double *) * m_pClasses->Get_Record_Count());
	m[0]	= (double  *)SG_Malloc(sizeof(double  ) * m_pClasses->Get_Record_Count() * m_pGrids->Get_Count());

	for(iClass=0; iClass<m_pClasses->Get_Record_Count(); iClass++)
	{
		a[iClass]	= a[0] + iClass * m_pGrids->Get_Count();
		b[iClass]	= b[0] + iClass * m_pGrids->Get_Count();
		m[iClass]	= m[0] + iClass * m_pGrids->Get_Count();

		for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
		{
			d					= m_pClasses->Get_Record(iClass)->asDouble(CLASS_S + 2 * iGrid);	// standard deviation
			a[iClass][iGrid]	=  1.0 / sqrt(d*d * 2.0 * M_PI);
			b[iClass][iGrid]	= -1.0 /     (d*d * 2.0);
			m[iClass][iGrid]	= m_pClasses->Get_Record(iClass)->asDouble(CLASS_M + 2 * iGrid);	// arithmetic mean
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !m_pResult->is_NoData(x, y) )
			{
				for(iClass=0, dMax=0.0; iClass<m_pClasses->Get_Record_Count(); iClass++)
				{
					for(iGrid=0, d=1.0; iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						e	 = GET_GRID_VALUE(x, y, iGrid) - m[iClass][iGrid];
						d	*= a[iClass][iGrid] * exp(b[iClass][iGrid] * e*e);
					}

					if( dMax < d )
					{
						dMax	= d;
						iMax	= iClass;
					}
				}

				if( (dMax = 100.0 * pow(dMax, 1.0 / m_pGrids->Get_Count())) < m_ML_Threshold )
				{
					m_pResult->Set_Value(x, y, 0.0);
				}
				else
				{
					m_pResult->Set_Value(x, y, iMax + 1);
				}

				if( m_pProbability )
				{
					m_pProbability->Set_Value(x, y, dMax);
				}
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(a[0]);
	SG_Free(a);
	SG_Free(b[0]);
	SG_Free(b);
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
