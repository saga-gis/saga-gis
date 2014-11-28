/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Clip Grid with Polygon"));

	Set_Author		(_TL("copyrights (c) 2006 Stefan Liersch"));

	Set_Description	(_TW(
		"Clips the input grid with a polygon shapefile. Select "
        "polygons from the shapefile prior to module execution "
        "in case you like to use only a subset from the shapefile "
        "for clipping."
	));

	//-----------------------------------------------------
	// Parameters list...

	Parameters.Add_Grid_List(
		NULL, "OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Grid_List(
		NULL, "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL, "NODATA"		, _TL("Exclude No-Data Area"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
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
	int						x, y, ix, iy, ax, ay, nx, ny;
	CSG_Parameter_Grid_List	*pGrids_in, *pGrids_out;
	CSG_Grid				*pGrid_in, *pGrid_out, Mask;
	CSG_Shapes				*pShapes;

	//-----------------------------------------------------
	pGrids_in	= Parameters("INPUT"   )->asGridList();
	pGrids_out	= Parameters("OUTPUT"  )->asGridList();
	pShapes		= Parameters("POLYGONS")->asShapes();
	m_bNoData	= Parameters("NODATA"  )->asBool();

	//-----------------------------------------------------
	if(	pShapes->Get_Type() == SHAPE_TYPE_Polygon && pShapes->Get_Count() > 0
	&&	Get_System()->Get_Extent().Intersects(pShapes->Get_Extent()) )
	{
		// create temporary grid.
		// Cells within the shapefile get the value +1
		// Cells outside the shapefile get the value -1
		Mask.Create(*Get_System(), SG_DATATYPE_Byte);

		//-------------------------------------------------
		// Get_Mask assignes +1 values to gridcells within the shapefile
		// The function has been copied from Module: 'Grid_Statistics_AddTo_Polygon'
		// Function: Get_ShapeIDs(...)
		// and check extent of valid values in Mask to 
		// calculate GridSystem parameters pGrid_out	
		if( Get_Mask(pShapes, &Mask) && Get_Extent(ax, nx, ay, ny, &Mask, pGrids_in) )
		{
			for(int iGrid=0; iGrid<pGrids_in->Get_Count(); iGrid++)
			{
				pGrid_in	= pGrids_in->asGrid(iGrid);

				pGrid_out	= SG_Create_Grid(					// creating the output grid GridSystem
					pGrid_in->Get_Type(), nx, ny, Get_Cellsize(),
					Get_XMin() + ax * Get_Cellsize(),
					Get_YMin() + ay * Get_Cellsize()
				);

				pGrid_out	->Set_Name(pGrid_in->Get_Name());
				pGrid_out	->Set_NoData_Value(pGrid_in->Get_NoData_Value());

				pGrids_out	->Add_Item(pGrid_out);

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
bool CGrid_Polygon_Clip::Get_Extent(int &xMin, int &xCount, int &yMin, int &yCount, CSG_Grid *pMask, CSG_Parameter_Grid_List *pGrids)
{
	bool	bFound;

	for(yMin=0, bFound=false; yMin<Get_NY() && !bFound && Process_Get_Okay(true); yMin++)
	{
		for(int x=0; x<Get_NX() && !bFound; x++)
		{
			bFound	= is_InGrid(x, yMin, pMask, pGrids);
		}
	}
	yMin--;
	

	//-----------------------------------------------------
	if( yMin < Get_NY() && Process_Get_Okay() )
	{
		int		xMax, yMax;

		for(yMax=Get_NY()-1, bFound=false; yMax>=yMin && !bFound && Process_Get_Okay(true); yMax--)
		{
			for(int x=0; x<Get_NX() && !bFound; x++)
			{
				bFound	= is_InGrid(x, yMax, pMask, pGrids);
			}
		}
		yMax++;

		for(xMin=0, bFound=false; xMin<Get_NX() && !bFound && Process_Get_Okay(true); xMin++)
		{
			for(int y=yMin; y<yMax && !bFound; y++)
			{
				bFound	= is_InGrid(xMin, y, pMask, pGrids);
			}
		}
		xMin--;

		for(xMax=Get_NX()-1, bFound=false; xMax>=xMin && !bFound && Process_Get_Okay(true); xMax--)
		{
			for(int y=yMin; y<yMax && !bFound; y++)
			{
				bFound	= is_InGrid(xMax, y, pMask, pGrids);
			}
		}
		xMax++;

		xCount	= 1 + xMax - xMin;
		yCount	= 1 + yMax - yMin;

		return( xCount > 0 && yCount > 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Polygon_Clip::is_InGrid(int x, int y, CSG_Grid *pMask, CSG_Parameter_Grid_List *pGrids)
{
	if( pMask->asInt(x, y) == MASK_ON )
	{
		if( !m_bNoData )
		{
			return( true );
		}

		for(int i=0; i<pGrids->Get_Count(); i++)
		{
			if( !pGrids->asGrid(i)->is_NoData(x, y) )
			{
				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//---------------------------------------------------------
// This function has been copied from Module: 'Grid_Statistics_AddTo_Polygon'
// Function: Get_ShapeIDs(...)
// copyright by Olaf Conrad
//
// added support to clip only with selected polygons (Volker Wichmann)
//---------------------------------------------------------
bool CGrid_Polygon_Clip::Get_Mask(CSG_Shapes *pShapes, CSG_Grid *pMask)
{
	bool		bFill, *bCrossing;
    bool        bOnlySelected = false;
	int			x, y, ix, xStart, xStop, iShape, iPart, iPoint;
	double		yPos;
	TSG_Point	pLeft, pRight, pa, pb, p;
	TSG_Rect	Extent;
	CSG_Shape	*pShape;

	//-----------------------------------------------------
	pMask->Assign(MASK_OFF);

	bCrossing	= (bool *)SG_Malloc(pMask->Get_NX() * sizeof(bool));

    if (pShapes->Get_Selection_Count() > 0)
        bOnlySelected = true;

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
        if (bOnlySelected && !pShapes->Get_Shape(iShape)->is_Selected())
            continue;

		pShape		= pShapes->Get_Shape(iShape);
		Extent		= pShape->Get_Extent().m_rect;

		xStart		= Get_System()->Get_xWorld_to_Grid(Extent.xMin) - 1;	if( xStart < 0 )		xStart	= 0;
		xStop		= Get_System()->Get_xWorld_to_Grid(Extent.xMax) + 1;	if( xStop >= Get_NX() )	xStop	= Get_NX() - 1;

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
							SG_Get_Crossing(p, pa, pb, pLeft, pRight, false);

							ix	= (int)((p.x - pMask->Get_XMin()) / pMask->Get_Cellsize() + 1.0);

							if( ix < 0)
							{
								ix	= 0;
							}
							else if( ix >= pMask->Get_NX() )
							{
								continue;
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
	SG_Free(bCrossing);

	return( true );
}
