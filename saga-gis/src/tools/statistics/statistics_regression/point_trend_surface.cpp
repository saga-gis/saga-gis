/**********************************************************
 * Version $Id: point_trend_surface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                point_trend_surface.cpp                //
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
#include "point_trend_surface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_Trend_Surface::CPoint_Trend_Surface(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Polynomial Regression"));

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

	Parameters.Add_Shapes(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "POLYNOM"		, _TL("Polynom"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("simple planar surface"),	// a + bx + cy
			_TL("bi-linear saddle"),		// a + bx + cy + dxy
			_TL("quadratic surface"),		// a + bx + cy + dxy + ex2 + fy2
			_TL("cubic surface"),			// a + bx + cy + dxy + ex2 + fy2 + gx2y + hxy2 + ix3 + iy3
			_TL("user defined")
		), 0
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_USER"	, _TL("User Defined Polynomial"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "XORDER"		, _TL("Maximum X Order"),
		_TL(""),
		PARAMETER_TYPE_Int, 4, 1, true
	);

	Parameters.Add_Value(
		pNode	, "YORDER"		, _TL("Maximum Y Order"),
		_TL(""),
		PARAMETER_TYPE_Int, 4, 1, true
	);

	Parameters.Add_Value(
		pNode	, "TORDER"		, _TL("Maximum Total Order"),
		_TL(""),
		PARAMETER_TYPE_Int, 4, 0, true
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, true, NULL, "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPoint_Trend_Surface::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CPoint_Trend_Surface::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POLYNOM") )
	{
		pParameters->Set_Enabled("NODE_USER", pParameter->asInt() == 4);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Trend_Surface::On_Execute(void)
{
	int			iAttribute;
	CSG_Shapes	*pPoints, *pResiduals;
	CSG_Grid	*pRegression;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS"   )->asShapes();
	pResiduals	= Parameters("RESIDUALS")->asShapes();
	iAttribute	= Parameters("ATTRIBUTE")->asInt();

	switch( Parameters("POLYNOM")->asInt() )
	{
	case 0:	m_xOrder = 1; m_yOrder = 1; m_tOrder = 1;	break;	// simple planar surface	// a + bx + cy
	case 1:	m_xOrder = 1; m_yOrder = 1; m_tOrder = 2;	break;	// bi-linear saddle"),		// a + bx + cy + dxy
	case 2:	m_xOrder = 2; m_yOrder = 2; m_tOrder = 2;	break;	// quadratic surface"),		// a + bx + cy + dxy + ex2 + fy2
	case 3:	m_xOrder = 3; m_yOrder = 3; m_tOrder = 3;	break;	// cubic surface"),			// a + bx + cy + dxy + ex2 + fy2 + gx2y + hxy2 + ix3 + iy3
	case 4:
		m_xOrder	= Parameters("XORDER")->asInt();
		m_yOrder	= Parameters("YORDER")->asInt();
		m_tOrder	= Parameters("TORDER")->asInt();
		break;
	}

	//-----------------------------------------------------
	if( !Get_Regression(pPoints, iAttribute) )
	{
		return( false );
	}

	Set_Message();

	//-----------------------------------------------------
	if( (pRegression = m_Grid_Target.Get_Grid()) == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	pRegression->Set_Name(CSG_String::Format("%s [%s]", Parameters("ATTRIBUTE")->asString(), _TL("Trend Surface")));

	Set_Regression(pRegression);

	Set_Residuals(pPoints, iAttribute, pResiduals, pRegression);

	m_Coefficients.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CPoint_Trend_Surface::Get_Power(const SG_Char *Value, int Power)
{
	if( Power > 0 )
	{
		if( Power > 1 )
		{
			return( CSG_String::Format(SG_T("%s%d"), Value, Power) );
		}

		return( Value );
	}

	return( SG_T("") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Trend_Surface::Get_Regression(CSG_Shapes *pPoints, int iAttribute)
{
	//-----------------------------------------------------
	int		i, j, Field;

	m_Names.Clear();

	m_Names	+= pPoints->Get_Name();

	for(i=1; i<=m_xOrder; i++)
	{
		m_Names	+= Get_Power(SG_T("x"), i);
	}

	for(i=1; i<=m_yOrder; i++)
	{
		m_Names	+= Get_Power(SG_T("y"), i);

		for(j=1; j<=m_xOrder && i<m_tOrder && j<m_tOrder; j++)
		{
			m_Names	+= Get_Power(SG_T("x"), j) + Get_Power(SG_T("y"), i);
		}
	}

	//-----------------------------------------------------
	CSG_Vector	Y, xPow, yPow;
	CSG_Matrix	X;

	Y.Create(pPoints->Get_Count());
	X.Create(m_Names.Get_Count(), pPoints->Get_Count());
	
	xPow.Create(m_xOrder + 1);
	yPow.Create(m_yOrder + 1);

	xPow[0]	= 1.0;
	yPow[0]	= 1.0;

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape(iShape);

		if( !pShape->is_NoData(iAttribute) )
		{
			double		zShape	= pShape->asDouble(iAttribute);
			TSG_Point	Point	= pShape->Get_Point(0);

			Y[iShape]		= zShape;
			X[iShape][0]	= 1.0;

			for(i=1, Field=1; i<=m_xOrder; i++)
			{
				X[iShape][Field++]	= xPow[i]	= xPow[i - 1] * Point.x;
			}

			for(i=1; i<=m_yOrder; i++)
			{
				X[iShape][Field++]	= yPow[i]	= yPow[i - 1] * Point.y;

				for(j=1; j<=m_xOrder && i<m_tOrder && j<m_tOrder; j++)
				{
					X[iShape][Field++]	= xPow[j] * yPow[i];
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Matrix	Xt, XtXinv;

	Xt		= X;
	Xt		.Set_Transpose();

	XtXinv	= Xt * X;
	XtXinv	.Set_Inverse();

	m_Coefficients	= XtXinv * Xt * Y;

	return( true );
}

//---------------------------------------------------------
bool CPoint_Trend_Surface::Set_Regression(CSG_Grid *pRegression)
{
	int			x, y, i, j, Field;
	double		z;
	TSG_Point	Point;
	CSG_Vector	xPow(m_xOrder + 1), yPow(m_yOrder + 1);

	xPow[0]	= 1.0;
	yPow[0]	= 1.0;

	for(y=0, Point.y=pRegression->Get_YMin(); y<pRegression->Get_NY() && Set_Progress(y, pRegression->Get_NY()); y++, Point.y+=pRegression->Get_Cellsize())
	{
		for(x=0, Point.x=pRegression->Get_XMin(); x<pRegression->Get_NX(); x++, Point.x+=pRegression->Get_Cellsize())
		{
			z	 = m_Coefficients[0];

			for(i=1, Field=1; i<=m_xOrder; i++)
			{
				z	+= m_Coefficients[Field++] * (xPow[i] = xPow[i - 1] * Point.x);
			}

			for(i=1; i<=m_yOrder; i++)
			{
				z	+= m_Coefficients[Field++] * (yPow[i] = yPow[i - 1] * Point.y);

				for(j=1; j<=m_xOrder && i<m_tOrder && j<m_tOrder; j++)
				{
					z	+= m_Coefficients[Field++] * xPow[j] * yPow[i];
				}
			}

			pRegression->Set_Value(x, y, z);
		}
	}

	DataObject_Update(pRegression);

	return( true );
}

//---------------------------------------------------------
bool CPoint_Trend_Surface::Set_Residuals(CSG_Shapes *pPoints, int iAttribute, CSG_Shapes *pResiduals, CSG_Grid *pRegression)
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
		pResiduals->Add_Field("POLYNOM" , SG_DATATYPE_Double);
		pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

		//-------------------------------------------------
		for(iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
		{
			pShape	= pPoints->Get_Shape(iShape);

			if( !pShape->is_NoData(iAttribute) )
			{
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
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
void CPoint_Trend_Surface::Set_Message(void)
{
	int			i;
	CSG_String	s;

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("\n%s:"), _TL("Regression"));

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("\n z = A"));

	for(i=1; i<m_Coefficients.Get_N(); i++)
	{
		s	+= CSG_String::Format(SG_T(" + %c%s"), 'A' + i, m_Names[i].c_str());
	}

	s	+= SG_T("\n");

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("\n z = %f"), m_Coefficients[0]);

	for(i=1; i<m_Coefficients.Get_N(); i++)
	{
		s	+= CSG_String::Format(SG_T(" %+f*%s"), m_Coefficients[i], m_Names[i].c_str());
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
