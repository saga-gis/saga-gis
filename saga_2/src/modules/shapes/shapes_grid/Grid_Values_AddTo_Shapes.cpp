
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
		"For points this is similar to 'Add Grid Values to Points' module. "
		"For lines and polygons average values will be calculated from interfering grid cells. "
		"For polygons the 'Grid Statistics for Polygons' module offers more advanced options. "
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
		NULL	, "INTERPOL"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_AddTo_Shapes::On_Execute(void)
{
	int						Interpolation;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Shapes				*pShapes;

	//-----------------------------------------------------
	pShapes			= Parameters("RESULT")		->asShapes();
	pGrids			= Parameters("GRIDS")		->asGridList();
	Interpolation	= Parameters("INTERPOL")	->asInt();

	//-----------------------------------------------------
	if( pGrids->Get_Count() <= 0 )
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
	for(int iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
	{
		CSG_Grid	*pGrid	= pGrids->asGrid(iGrid);

		pShapes->Add_Field(pGrid->Get_Name(), SG_DATATYPE_Double);

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( pShape->Get_Extent().Intersects(pGrid->Get_Extent()) )
			{
				bool	bResult;
				double	Value;

				switch( pShapes->Get_Type() )
				{
				default:
				case SHAPE_TYPE_Point:
				case SHAPE_TYPE_Points:
					bResult	= Get_Data_Point	(Value, pShape, pGrid, Interpolation);
					break;

				case SHAPE_TYPE_Line:
					bResult	= Get_Data_Line		(Value, pShape, pGrid);
					break;

				case SHAPE_TYPE_Polygon:
					bResult	= Get_Data_Polygon	(Value, pShape, pGrid);
					break;
				}

				if( bResult )
				{
					pShape->Set_Value(pShapes->Get_Field_Count() - 1, Value);
				}
				else
				{
					pShape->Set_NoData(pShapes->Get_Field_Count() - 1);
				}
			}
			else
			{
				pShape->Set_NoData(pShapes->Get_Field_Count() - 1);
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_AddTo_Shapes::Get_Data_Point(double &Value, CSG_Shape *pShape, CSG_Grid *pGrid, int Interpolation)
{
	Value	= 0.0;
	int	n	= 0;

	//-----------------------------------------------------
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

			if( pGrid->is_InGrid_byPos(Point) )
			{
				Value	+= pGrid->Get_Value(Point, Interpolation, true);
				n		++;
			}
		}
	}

	//-----------------------------------------------------
	if( n > 0 )
	{
		Value	/= (double)n;

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
bool CGrid_Values_AddTo_Shapes::Get_Data_Line(double &Value, CSG_Shape *pShape, CSG_Grid *pGrid)
{
	Value	= 0.0;
	int	n	= 0;

	//-----------------------------------------------------

	//-----------------------------------------------------
	if( n > 0 )
	{
		Value	/= (double)n;

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
bool CGrid_Values_AddTo_Shapes::Get_Data_Polygon(double &Value, CSG_Shape *pShape, CSG_Grid *pGrid)
{
	Value	= 0.0;
	int	n	= 0;

	//-----------------------------------------------------
	int			x, y, xMin, yMin, xMax, yMax;
	double		d;
	TSG_Point	p;

	d		= pGrid->Get_System().Get_xWorld_to_Grid(pShape->Get_Extent().Get_XMin());
	xMin	= d < -0.5 ? 0 : d >= pGrid->Get_NX() ? pGrid->Get_NX() - 1 : (int)(0.5 + d);
	d		= pGrid->Get_System().Get_xWorld_to_Grid(pShape->Get_Extent().Get_XMax());
	xMax	= d < -0.5 ? 0 : d >= pGrid->Get_NX() ? pGrid->Get_NX() - 1 : (int)(0.5 + d);
	d		= pGrid->Get_System().Get_yWorld_to_Grid(pShape->Get_Extent().Get_YMin());
	yMin	= d < -0.5 ? 0 : d >= pGrid->Get_NY() ? pGrid->Get_NY() - 1 : (int)(0.5 + d);
	d		= pGrid->Get_System().Get_yWorld_to_Grid(pShape->Get_Extent().Get_YMax());
	yMax	= d < -0.5 ? 0 : d >= pGrid->Get_NY() ? pGrid->Get_NY() - 1 : (int)(0.5 + d);

	for(y=yMin, p.y=pGrid->Get_System().Get_yGrid_to_World(yMin); y<=yMax; y++, p.y+=pGrid->Get_Cellsize())
	{
		for(x=xMin, p.x=pGrid->Get_System().Get_xGrid_to_World(xMin); x<=xMax; x++, p.x+=pGrid->Get_Cellsize())
		{
			if( pGrid->is_InGrid(x, y) && ((CSG_Shape_Polygon *)pShape)->is_Containing(p) )
			{
				Value	+= pGrid->asDouble(x, y);
				n		++;
			}
		}
	}

	//-----------------------------------------------------
	if( n > 0 )
	{
		Value	/= (double)n;

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
