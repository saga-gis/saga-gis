
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   line_density.cpp                    //
//                                                       //
//                 Copyright (C) 2024 by                 //
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
#include "line_density.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Density::CLine_Density(void)
{
	Set_Name		(_TL("Line Density"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"Line density."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"     , _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Field("LINES",
		"POPULATION", _TL("Population"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"RADIUS"    , _TL("Radius"),
		_TL(""),
		1., M_FLT_EPSILON, true
	);

	Parameters.Add_Choice("",
		"OUTPUT"    , _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("absolute"),
			_TL("relative")
		), 1
	);

	Parameters.Add_Bool("",
		"NO_ZERO"   , _TL("Zero as No-Data"),
		_TL(""),
		false
	);

	m_Grid_Target.Create(&Parameters, true, "", "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Density::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("LINES") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());

		pParameters->Set_Parameter("RADIUS", 0.5 * 1.4142 * (*pParameters)("TARGET_USER_SIZE")->asDouble());
	}

	if(	pParameter->Cmp_Identifier("TARGET_USER_SIZE") )
	{
		pParameters->Set_Parameter("RADIUS", 0.5 * 1.4142 * pParameter->asDouble());
	}

	if(	pParameter->Cmp_Identifier("TARGET_SYSTEM") && pParameter->asGrid_System()->is_Valid() )
	{
		pParameters->Set_Parameter("RADIUS", 0.5 * 1.4142 * pParameter->asGrid_System()->Get_Cellsize());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CLine_Density::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Density::On_Execute(void)
{
	CSG_Grid *pGrid = m_Grid_Target.Get_Grid();

	if( pGrid == NULL )
	{
		Error_Set(_TL("Failed to request target grid."));

		return( false );
	}

	DataObject_Add(pGrid); DataObject_Set_Colors(pGrid, 5, SG_COLORS_WHITE_RED);

	pGrid->Set_NoData_Value(Parameters("NO_ZERO")->asBool() ? 0. : -1.);

	//-----------------------------------------------------
	m_pLines = Parameters("LINES")->asShapes();

	m_Population = Parameters("POPULATION")->asInt();

	if( m_Population < 0 )
	{
		pGrid->Fmt_Name("%s [%s]"   , _TL("Line Density"), m_pLines->Get_Name());
	}
	else
	{
		pGrid->Fmt_Name("%s [%s.%s]", _TL("Line Density"), m_pLines->Get_Name(), m_pLines->Get_Field_Name(m_Population));
	}

	if( !m_pLines->Get_Extent().Intersects(pGrid->Get_Extent()) )
	{
		Error_Set(_TL("Extents of lines and target grid do not intersect."));

		return( false );
	}

	//-----------------------------------------------------
	m_Radius = Parameters("RADIUS")->asDouble();

	int bAbsolute = Parameters("OUTPUT")->asInt() == 0;

	//-----------------------------------------------------
	for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		double py = pGrid->Get_YMin() + y * pGrid->Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			CSG_Point Point(pGrid->Get_XMin() + x * pGrid->Get_Cellsize(), py);

			pGrid->Set_Value(x, y, Get_Density(Point, bAbsolute));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CLine_Density::Get_Density(const CSG_Point &Point, bool bAbsolute)
{
	double Length = 0.; CSG_Rect r(Point.x - m_Radius, Point.y - m_Radius, Point.x + m_Radius, Point.y + m_Radius);

	for(sLong i=0; i<m_pLines->Get_Count(); i++)
	{
		CSG_Shape_Line *pLine = m_pLines->Get_Shape(i)->asLine();

		if( pLine->Intersects(r) )
		{
			double d = 0.;

			for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
			{
				CSG_Point A = pLine->Get_Point(0, iPart);

				for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					CSG_Point B = A; A = pLine->Get_Point(iPoint, iPart);

					d += Get_Intersection(Point, A, B);
				}
			}

			Length += m_Population < 0 ? d : d * pLine->asDouble(m_Population);
		}
	}

	return( bAbsolute ? Length : Length / (M_PI * m_Radius*m_Radius) );
}

//---------------------------------------------------------
inline double CLine_Density::Get_Intersection(const CSG_Point &Point, const CSG_Point &A, const CSG_Point &B)
{
	if( SG_Get_Distance(Point, A) <= m_Radius )
	{
		if( SG_Get_Distance(Point, B) <= m_Radius )
		{
			return( SG_Get_Distance(A, B) );
		}

		CSG_Point C; double d = SG_Get_Nearest_Point_On_Line(Point, A, B, C, false);

		d = sqrt(m_Radius*m_Radius - d*d) / SG_Get_Distance(C, B);

		C.x += d * (B.x - C.x);
		C.y += d * (B.y - C.y);

		return( SG_Get_Distance(A, C) );
	}

	//-----------------------------------------------------
	if( SG_Get_Distance(Point, B) <= m_Radius )
	{
		CSG_Point C; double d = SG_Get_Nearest_Point_On_Line(Point, A, B, C, false);

		d = sqrt(m_Radius*m_Radius - d*d) / SG_Get_Distance(C, A);

		C.x += d * (A.x - C.x);
		C.y += d * (A.y - C.y);

		return( SG_Get_Distance(B, C) );
	}

	//-----------------------------------------------------
	double d = SG_Get_Distance_To_Line(Point, A, B, false);

	if( d < m_Radius ) // no tangent! ...secant, both points are outside of circle
	{
		return( 2. * sqrt(m_Radius*m_Radius - d*d) );
	}

	return( 0. );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
