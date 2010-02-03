
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                GSGrid_Trend_Surface.cpp               //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "GSGrid_Trend_Surface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Trend_Surface::CGSGrid_Trend_Surface(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Trend Surface"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Reference:\n"
		" - Lloyd, C. (2010): Spatial Data Analysis - An Introduction for GIS Users. Oxford, 206p.\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Details"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "CELLSIZE"	, _TL("Cellsize"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Trend Surface Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("first order polynom"),
			_TL("second order polynom")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Trend_Surface::On_Execute(void)
{
	int			iAttribute, Type;
	double		Cellsize;
	CSG_Table	*pTable;
	CSG_Shapes	*pPoints, *pResiduals;
	CSG_Grid	*pRegression;

	//-----------------------------------------------------
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pTable			= Parameters("TABLE")		->asTable();
	pPoints			= Parameters("POINTS")		->asShapes();
	pResiduals		= Parameters("RESIDUALS")	->asShapes();
	iAttribute		= Parameters("ATTRIBUTE")	->asInt();
	Type			= Parameters("TYPE")		->asInt();
	Cellsize		= Parameters("CELLSIZE")	->asDouble();

	//-----------------------------------------------------
	if( Get_Regression(pPoints, iAttribute, Type) )
	{
		Set_Message(Type);

		if( pRegression == NULL )
		{
			pRegression	= SG_Create_Grid(SG_DATATYPE_Float,
				1 + (int)(pPoints->Get_Extent().Get_XRange() / Cellsize),
				1 + (int)(pPoints->Get_Extent().Get_XRange() / Cellsize),
				Cellsize,
				pPoints->Get_Extent().Get_XMin(),
				pPoints->Get_Extent().Get_YMin()
			);

			DataObject_Add(pRegression);

			Parameters("REGRESSION")->Set_Value(pRegression);
		}

		pRegression->Set_Name(CSG_String::Format(SG_T("%s (%s)"), pPoints->Get_Name(), Get_Name()));

		Set_Regression(pRegression, Type);

		Set_Residuals(pPoints, iAttribute, pResiduals, pRegression);

		if( pTable )
		{
			pTable->Assign(m_Regression.Get_Result());

			pTable->Set_Name(_TL("Trend Surface Analysis"));
		}

		m_Regression.Destroy();

		return( true );
	}

	//-----------------------------------------------------
	m_Regression.Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Trend_Surface::Get_Regression(CSG_Shapes *pPoints, int iAttribute, int Type)
{
	//-----------------------------------------------------
	CSG_Table	Table;

	Table.Add_Field(pPoints->Get_Name(), SG_DATATYPE_Double);

	Table.Add_Field(SG_T("X"), SG_DATATYPE_Double);
	Table.Add_Field(SG_T("Y"), SG_DATATYPE_Double);

	if( Type == 1 )
	{
		Table.Add_Field(SG_T("XY"), SG_DATATYPE_Double);
		Table.Add_Field(SG_T("X2"), SG_DATATYPE_Double);
		Table.Add_Field(SG_T("Y2"), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape(iShape);
		double		zShape	= pShape->asDouble(iAttribute);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point			Point		= pShape->Get_Point(iPoint, iPart);
				CSG_Table_Record	*pRecord	= Table.Add_Record();

				pRecord->Set_Value(0, zShape);

				pRecord->Set_Value(1, Point.x);
				pRecord->Set_Value(2, Point.y);

				if( Type == 1 )
				{
					pRecord->Set_Value(3, Point.x * Point.y);
					pRecord->Set_Value(4, Point.x * Point.x);
					pRecord->Set_Value(5, Point.y * Point.y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Regression.Calculate(Table) );
}

//---------------------------------------------------------
bool CGSGrid_Trend_Surface::Set_Regression(CSG_Grid *pRegression, int Type)
{
	int			x, y;
	double		z;
	TSG_Point	p;

	for(y=0, p.y=pRegression->Get_YMin(); y<pRegression->Get_NY() && Set_Progress(y, pRegression->Get_NY()); y++, p.y+=pRegression->Get_Cellsize())
	{
		for(x=0, p.x=pRegression->Get_XMin(); x<pRegression->Get_NX(); x++, p.x+=pRegression->Get_Cellsize())
		{
			z	 = m_Regression.Get_RConst();

			z	+= m_Regression.Get_RCoeff(0) * p.x;
			z	+= m_Regression.Get_RCoeff(1) * p.y;

			if( Type == 1 )
			{
				z	+= m_Regression.Get_RCoeff(2) * p.x * p.y;
				z	+= m_Regression.Get_RCoeff(3) * p.x * p.x;
				z	+= m_Regression.Get_RCoeff(4) * p.y * p.y;
			}

			pRegression->Set_Value (x, y, z);
		}
	}

	DataObject_Update(pRegression);

	return( true );
}

//---------------------------------------------------------
bool CGSGrid_Trend_Surface::Set_Residuals(CSG_Shapes *pPoints, int iAttribute, CSG_Shapes *pResiduals, CSG_Grid *pRegression)
{
	int			iPoint, iPart, iShape;
	double		zShape, zGrid;
	TSG_Point	Point;
	CSG_Shape	*pShape, *pResidual;

	//-----------------------------------------------------
	if( pResiduals )
	{
		pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Residuals")));
		pResiduals->Add_Field(pPoints->Get_Field_Name(iAttribute), SG_DATATYPE_Double);
		pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
		pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

		//-------------------------------------------------
		for(iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
		{
			pShape	= pPoints->Get_Shape(iShape);
			zShape	= pShape->asDouble(iAttribute);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape->Get_Point(iPoint, iPart);

					if( pRegression->Get_Value(Point, zGrid) )
					{
						pResidual	= pResiduals->Add_Shape();
						pResidual->Add_Point(Point);
						pResidual->Set_Value(0, zShape);
						pResidual->Set_Value(1, zGrid);
						pResidual->Set_Value(2, zShape - zGrid);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
void CGSGrid_Trend_Surface::Set_Message(int Type)
{
	CSG_String	s;

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("\n%s:"), _TL("Regression"));
	s	+= CSG_String::Format(SG_T("\n Y = %f"), m_Regression.Get_RConst( ));
	s	+= CSG_String::Format(SG_T(" %+f*x")   , m_Regression.Get_RCoeff(0));
	s	+= CSG_String::Format(SG_T(" %+f*y")   , m_Regression.Get_RCoeff(1));

	if( Type == 1 )
	{
		s	+= CSG_String::Format(SG_T(" %+f*xy"), m_Regression.Get_RCoeff(2));
		s	+= CSG_String::Format(SG_T(" %+f*xx"), m_Regression.Get_RCoeff(3));
		s	+= CSG_String::Format(SG_T(" %+f*yy"), m_Regression.Get_RCoeff(4));
	}

	s	+= SG_T("\n");

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("\n%s:"), _TL("Correlation"));

	s	+= CSG_String::Format(SG_T("\nR\xc2\xb2 = %2.2f%% [%02.2f%%] -> x"), 100.0 * m_Regression.Get_R2(0), 100.0 * m_Regression.Get_R2_Change(0));
	s	+= CSG_String::Format(SG_T("\nR\xc2\xb2 = %2.2f%% [%02.2f%%] -> y"), 100.0 * m_Regression.Get_R2(1), 100.0 * m_Regression.Get_R2_Change(1));

	if( Type == 1 )
	{
		s	+= CSG_String::Format(SG_T("\nR\xc2\xb2 = %2.2f%% [%02.2f%%] -> xy"), 100.0 * m_Regression.Get_R2(2), 100.0 * m_Regression.Get_R2_Change(2));
		s	+= CSG_String::Format(SG_T("\nR\xc2\xb2 = %2.2f%% [%02.2f%%] -> xx"), 100.0 * m_Regression.Get_R2(3), 100.0 * m_Regression.Get_R2_Change(3));
		s	+= CSG_String::Format(SG_T("\nR\xc2\xb2 = %2.2f%% [%02.2f%%] -> yy"), 100.0 * m_Regression.Get_R2(4), 100.0 * m_Regression.Get_R2_Change(4));
	}

	s	+= SG_T("\n");

	//-----------------------------------------------------
	Message_Add(s, false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
