/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Values_AddTo_Shapes.cpp             //
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
#include "Grid_Values_AddTo_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Values_AddTo_Shapes::CGrid_Values_AddTo_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Add Grid Values to Shapes"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Spatial Join: Retrieves information from the selected grids at the positions "
		"of the shapes of the selected shapes layer and adds it to the resulting shapes layer. "
		"For points this is similar to 'Add Grid Values to Points' tool. "
		"For lines and polygons average values will be calculated from interfering grid cells. "
		"For polygons the 'Grid Statistics for Polygons' tool offers more advanced options. "
	));


	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"		, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Values_AddTo_Shapes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("SHAPES") )
	{
		pParameters->Set_Enabled("RESAMPLING", pParameter->asShapes() && 
			(  pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Point
			|| pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Points
			|| pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Line
		));
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_AddTo_Shapes::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Shapes				*pShapes;

	//-----------------------------------------------------
	pShapes	= Parameters("RESULT")->asShapes();
	pGrids	= Parameters("GRIDS" )->asGridList();

	//-----------------------------------------------------
	if( pGrids->Get_Grid_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pShapes == NULL )
	{
		pShapes		= Parameters("SHAPES")->asShapes();
	}
	else if( pShapes != Parameters("SHAPES")->asShapes() )
	{
		pShapes->Create(*Parameters("SHAPES")->asShapes());
	}

	//-----------------------------------------------------
	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	m_Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	m_Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	m_Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	m_Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	//-----------------------------------------------------
	for(int iGrid=0; iGrid<pGrids->Get_Grid_Count(); iGrid++)
	{
		CSG_Grid	*pGrid	= pGrids->Get_Grid(iGrid);

		int	Field	= pShapes->Get_Field_Count();

		pShapes->Add_Field(pGrid->Get_Name(), SG_DATATYPE_Double);

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Simple_Statistics	Statistics;

			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( pShape->Get_Extent().Intersects(pGrid->Get_Extent()) )
			{
				switch( pShapes->Get_Type() )
				{
				case SHAPE_TYPE_Point: default:
				case SHAPE_TYPE_Points:		Get_Data_Point  (Statistics, pShape, pGrid);	break;
				case SHAPE_TYPE_Line:		Get_Data_Line   (Statistics, pShape, pGrid);	break;
				case SHAPE_TYPE_Polygon:	Get_Data_Polygon(Statistics, pShape, pGrid);	break;
				}
			}

			if( Statistics.Get_Count() > 0 )
			{
				pShape->Set_Value(Field, Statistics.Get_Mean());
			}
			else
			{
				pShape->Set_NoData(Field);
			}
		}
	}

	//-----------------------------------------------------
	if( pShapes == Parameters("SHAPES")->asShapes() )
	{
		DataObject_Update(pShapes);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Values_AddTo_Shapes::Get_Data_Point(CSG_Simple_Statistics &Statistics, CSG_Shape *pShape, CSG_Grid *pGrid)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			double	Value;

			if( pGrid->Get_Value(pShape->Get_Point(iPoint, iPart), Value, m_Resampling) )
			{
				Statistics	+= Value;
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Values_AddTo_Shapes::Get_Data_Line(CSG_Simple_Statistics &Statistics, CSG_Shape *pShape, CSG_Grid *pGrid)
{
	double	dStep	= pGrid->Get_Cellsize();

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 0 )
		{
			double		Value;

			TSG_Point	B, A	= pShape->Get_Point(0, iPart);

			if( pGrid->Get_Value(A, Value, m_Resampling) )
			{
				Statistics	+= Value;
			}

			for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				B	= A;	A	= pShape->Get_Point(iPoint, iPart);

				//-----------------------------------------
				double	Distance	= SG_Get_Distance(A, B);

				if( Distance > 0.0 )
				{
					double		dx	= (B.x - A.x) * dStep / Distance;
					double		dy	= (B.y - A.y) * dStep / Distance;

					TSG_Point	C	= A;

					for(double d=0.0; d<Distance; d+=dStep, C.x+=dx, C.y+=dy)
					{
						if( pGrid->Get_Value(C, Value, m_Resampling) )
						{
							Statistics	+= Value;
						}
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Values_AddTo_Shapes::Get_Data_Polygon(CSG_Simple_Statistics &Statistics, CSG_Shape *pShape, CSG_Grid *pGrid)
{
	int	xMin	= pGrid->Get_System().Get_xWorld_to_Grid(pShape->Get_Extent().Get_XMin());
	int	xMax	= pGrid->Get_System().Get_xWorld_to_Grid(pShape->Get_Extent().Get_XMax());
	int	yMin	= pGrid->Get_System().Get_yWorld_to_Grid(pShape->Get_Extent().Get_YMin());
	int	yMax	= pGrid->Get_System().Get_yWorld_to_Grid(pShape->Get_Extent().Get_YMax());

	if( xMin < 0 ) xMin = 0; else if( xMin >= pGrid->Get_NX() ) xMin = pGrid->Get_NX() - 1;
	if( xMax < 0 ) xMax = 0; else if( xMax >= pGrid->Get_NX() ) xMax = pGrid->Get_NX() - 1;
	if( yMin < 0 ) yMin = 0; else if( yMin >= pGrid->Get_NY() ) yMin = pGrid->Get_NY() - 1;
	if( yMax < 0 ) yMax = 0; else if( yMax >= pGrid->Get_NY() ) yMax = pGrid->Get_NY() - 1;

	//-----------------------------------------------------
	for(int y=yMin; y<=yMax; y++)
	{
		double	p_y	= pGrid->Get_System().Get_yGrid_to_World(y);

		for(int x=xMin; x<=xMax; x++)
		{
			double	p_x	= pGrid->Get_System().Get_xGrid_to_World(x);

			if( !pGrid->is_NoData(x, y) && ((CSG_Shape_Polygon *)pShape)->Contains(p_x, p_y) )
			{
				Statistics	+= pGrid->asDouble(x, y);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
