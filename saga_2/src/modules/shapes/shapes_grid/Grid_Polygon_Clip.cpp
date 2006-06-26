
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
//                 Grid_Polygon_Clip.cpp                 //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                     Stefan Liersch                    //
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
//    e-mail:     stefan.liersch@ufz.de                  //
//                stliersch@freenet.de                   //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <memory.h>

#include "Grid_Polygon_Clip.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MASK_OFF	-1
#define MASK_ON		 1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// constructor
//---------------------------------------------------------
CGrid_Polygon_Clip::CGrid_Polygon_Clip(void)
{
	//-----------------------------------------------------
	// 1. Info...
	Set_Name("Clip Grid with Polygon");

	Set_Author("copyrights (c) 2006 Stefan Liersch");

	Set_Description("Clip Grid with Polygon");

	//-----------------------------------------------------
	// Parameters list...

	Parameters.Add_Grid_Output(
		NULL, "GRIDout"	, _TL("Output"),
		""
	);

	Parameters.Add_Grid(
		NULL, "GRID"	, _TL("Input"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "POLY"	, _TL("Polygons"),
		"",
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

}

//---------------------------------------------------------
// destructor
//---------------------------------------------------------
CGrid_Polygon_Clip::~CGrid_Polygon_Clip(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// This function is executed if the user is pressing the OK button
//---------------------------------------------------------
bool CGrid_Polygon_Clip::On_Execute(void)
{
	int		x, y, ix, iy, ax, ay, nx, ny;
	CGrid	*pGrid_in, *pGrid_out, Mask;
	CShapes	*pShapes;
	
	//-----------------------------------------------------
	pGrid_in	= Parameters("GRID")->asGrid();
	pShapes		= Parameters("POLY")->asShapes();
	
	//-----------------------------------------------------
	if(	pShapes->Get_Type() == SHAPE_TYPE_Polygon && pShapes->Get_Count() > 0
	&&	pGrid_in->is_Intersecting(pShapes->Get_Extent()) )
	{
		// create temporary grid.
		// Cells within the shapefile get the value +1
		// Cells outside the shapefile get the value -1
		Mask.Create(pGrid_in, GRID_TYPE_Byte);

		//-------------------------------------------------
		// Get_Mask assignes +1 values to gridcells within the shapefile
		// The function has been copied from Module: 'Grid_Statistics_AddTo_Polygon'
		// Function: Get_ShapeIDs(...)
		// and check extent of valid values in Mask to 
		// calculate GridSystem parameters pGrid_out	
		if( Get_Mask(pShapes, &Mask) && Get_Extent(ax, nx, ay, ny, &Mask) )
		{
			pGrid_out	= API_Create_Grid(					// creating the output grid GridSystem
				pGrid_in->Get_Type(), nx, ny, pGrid_in->Get_Cellsize(),
				pGrid_in->Get_XMin() + ax * pGrid_in->Get_Cellsize(),
				pGrid_in->Get_YMin() + ay * pGrid_in->Get_Cellsize()
			);

			pGrid_out->Set_Name(pGrid_in->Get_Name());
			Parameters("GRIDout")->Set_Value(pGrid_out);	// adding the grid in workspace "Data"

			// Assign valid values from input grid to the cells of the
			// output grid that are within the borders of the shapefile
			// Assign NoData values to the cells outside the shapefile borders
			for(y=0, iy=ay; y<ny && Set_Progress(y, ny); y++, iy++)
			{
				for(x=0, ix=ax; x<nx; x++, ix++)
				{
					if( Mask.asDouble(ix, iy) == MASK_ON )	// -1 = NoData_Value
					{
						pGrid_out->Set_Value(x, y, pGrid_in->asDouble(ix, iy));
					}
					else
					{
						pGrid_out->Set_NoData(x, y);
					}
				}
			}

			return( true );
		}
	}

	return( false );
}

///////////////////////////////////////////////////////////
//---------------------------------------------------------
// This function modifies the incoming integer variables!!!
//---------------------------------------------------------
bool CGrid_Polygon_Clip::Get_Extent(int &xMin, int &xMax, int &yMin, int &yMax, CGrid *pMask)
{
	int		x, y;

	for(y=0, yMin=-1; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pMask->asInt(x, y) == MASK_ON )
			{
				if( yMin < 0 )
				{
					yMin	= yMax	= y;
					xMin	= xMax	= x;
				}
				else
				{
					yMax	= y;

					if( xMin > x )
					{
						xMin	= x;
					}
					else if( xMax < x )
					{
						xMax	= x;
					}
				}
			}
		}
	}

	if( yMin >= 0 )
	{
		xMax	-= xMin - 1;
		yMax	-= yMin - 1;

		return( true );
	}

	return( false );
}

///////////////////////////////////////////////////////////
//---------------------------------------------------------
// This function has been copied from Module: 'Grid_Statistics_AddTo_Polygon'
// Function: Get_ShapeIDs(...)
// copyright by Olaf Conrad
//---------------------------------------------------------
bool CGrid_Polygon_Clip::Get_Mask(CShapes *pShapes, CGrid *pMask)
{
	bool		bFill, *bCrossing;
	int			x, y, ix, xStart, xStop, iShape, iPart, iPoint;
	double		yPos;
	TGEO_Point	pLeft, pRight, pa, pb, p;
	TGEO_Rect	Extent;
	CShape		*pShape;

	//-----------------------------------------------------
	pMask->Assign(MASK_OFF);

	bCrossing	= (bool *)API_Malloc(pMask->Get_NX() * sizeof(bool));

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape		= pShapes->Get_Shape(iShape);
		Extent		= pShape->Get_Extent().m_rect;

		xStart		= (int)((Extent.xMin - pMask->Get_XMin()) / pMask->Get_Cellsize()) - 1;
		if( xStart < 0 )
			xStart	= 0;

		xStop		= (int)((Extent.xMax - pMask->Get_XMin()) / pMask->Get_Cellsize()) + 1;
		if( xStop >= pMask->Get_NX() )
			xStop	= pMask->Get_NX() - 1;

		pLeft.x		= pMask->Get_XMin() - 1.0;
		pRight.x	= pMask->Get_XMax() + 1.0;


		//-------------------------------------------------
		for(y=0, yPos=pMask->Get_YMin(); y<pMask->Get_NY(); y++, yPos+=pMask->Get_Cellsize())
		{
			if( yPos >= Extent.yMin && yPos <= Extent.yMax )
			{
				memset(bCrossing, 0, pMask->Get_NX() * sizeof(bool));

				pLeft.y	= pRight.y	= yPos;

				//-----------------------------------------
				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					pb		= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);

					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						pa	= pb;
						pb	= pShape->Get_Point(iPoint, iPart);

						if(	(	(pa.y <= yPos && yPos < pb.y)
							||	(pa.y > yPos && yPos >= pb.y)	)	)
						{
							GEO_Get_Crossing(p, pa, pb, pLeft, pRight, false);

							ix	= (int)((p.x - pMask->Get_XMin()) / pMask->Get_Cellsize() + 1.0);

							if( ix < 0)
							{
								ix	= 0;
							}
							else if( ix >= pMask->Get_NX() )
							{
								ix	= pMask->Get_NX() - 1;
							}

							bCrossing[ix]	= !bCrossing[ix];
						}
					}
				}

				//-----------------------------------------
				for(x=xStart, bFill=false; x<=xStop; x++)
				{
					if( bCrossing[x] )
					{
						bFill	= !bFill;
					}

					if( bFill )
					{
						pMask->Set_Value(x, y, MASK_ON);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	API_Free(bCrossing);

	return( true );
}
