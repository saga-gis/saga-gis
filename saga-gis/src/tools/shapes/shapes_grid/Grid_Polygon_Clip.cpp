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
CGrid_Polygon_Clip::CGrid_Polygon_Clip(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Clip Grid with Polygon"));

	Set_Author		("Stefan Liersch (c) 2006");

	Set_Description	(_TW(
		"Clips the input grid with a polygon shapefile. Select "
        "polygons from the shapefile prior to tool execution "
        "in case you like to use only a subset from the shapefile "
        "for clipping."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(NULL,
		"INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(NULL,
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Shapes(NULL,
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(NULL,
		"EXTENT"	, _TL("Target Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("original"),
			_TL("polygons"),
			_TL("crop to data")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Polygon_Clip::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	Mask;

	if( !Get_Mask(Mask) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pInput	= Parameters("INPUT")->asGridList();

	if( pInput->Get_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pOutput	= Parameters("OUTPUT")->asGridList();

	CSG_Grid_System	System;

	if( !Get_Output(Mask, pOutput, System) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	ax	= (int)((System.Get_XMin() - Get_XMin()) / Get_Cellsize());
	int	ay	= (int)((System.Get_YMin() - Get_YMin()) / Get_Cellsize());

	//-----------------------------------------------------
	for(int y=0, iy=ay; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++, iy++)
	{
		for(int x=0, ix=ax; x<System.Get_NX(); x++, ix++)
		{
			if( Mask.asByte(ix, iy) )
			{
				for(int i=0; i<pInput->Get_Count(); i++)
				{
					pOutput->asGrid(i)->Set_Value(x, y, pInput->asGrid(i)->asDouble(ix, iy));
				}
			}
			else
			{
				for(int i=0; i<pInput->Get_Count(); i++)
				{
					pOutput->asGrid(i)->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Polygon_Clip::Get_Output(const CSG_Grid &Mask, CSG_Parameter_Grid_List *pOutput, CSG_Grid_System &System)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pInput	= Parameters("INPUT")->asGridList();

	int	Crop	= Parameters("EXTENT")->asInt();

	if( Crop == 0 )
	{
		System	= *Get_System();
	}
	else
	{
		int		xMin, yMin, xMax, yMax = -1;

		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( Mask.asByte(x, y) && (Crop == 1 || Has_Data(x, y, pInput)) )
				{
					if( yMax < 0 )
					{
						xMin = xMax = x;
						yMin = yMax = y;
					}
					else
					{
						if( xMin > x ) xMin = x; else if( xMax < x ) xMax = x;
						if( yMin > y ) yMin = y; else if( yMax < y ) yMax = y;
					}
				}
			}
		}

		if( yMax >= 0 )
		{
			System.Assign(Get_Cellsize(),
				Get_XMin() + xMin * Get_Cellsize(),
				Get_YMin() + yMin * Get_Cellsize(),
				1 + xMax - xMin,
				1 + yMax - yMin
			);
		}
	}

	//-----------------------------------------------------
	if( System.is_Valid() )
	{
		pOutput->Del_Items();

		for(int i=0; i<pInput->Get_Count(); i++)
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(System, pInput->asGrid(i)->Get_Type());

			pGrid->Set_Name        (pInput->asGrid(i)->Get_Name        ());
			pGrid->Set_NoData_Value(pInput->asGrid(i)->Get_NoData_Value());

			pOutput->Add_Item(pGrid);

			DataObject_Add(pGrid);
			DataObject_Set_Parameters(pGrid, pInput->asGrid(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Polygon_Clip::Has_Data(int x, int y, CSG_Parameter_Grid_List *pInput)
{
	for(int i=0; i<pInput->Get_Count(); i++)
	{
		if( !pInput->asGrid(i)->is_NoData(x, y) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Polygon_Clip::Get_Mask(CSG_Grid &Mask)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if(	pPolygons->Get_Type() != SHAPE_TYPE_Polygon || pPolygons->Get_Count() <= 0 )
	{
		return( false );
	}

	if( !Get_System()->Get_Extent().Intersects(pPolygons->Get_Extent()) )
	{
		return( false );
	}

	Mask.Create(*Get_System(), SG_DATATYPE_Byte); // Mask.Assign(0.0);

	//-----------------------------------------------------
	bool	*bCrossing	= new bool[Get_NX()];

	bool	bSelection	= pPolygons->Get_Selection_Count() > 0;

	TSG_Point	Line[2];

	Line[0].x	= Get_XMin() - 1.0;
	Line[1].x	= Get_XMax() + 1.0;

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		if( bSelection && !pPolygon->is_Selected() )
		{
			continue;
		}

		int	xStart	= Get_System()->Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin()) - 1; if( xStart < 0        ) xStart	= 0;
		int	xStop	= Get_System()->Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax()) + 1; if( xStop >= Get_NX() ) xStop	= Get_NX() - 1;

		//-------------------------------------------------
		for(int y=0; y<Get_NY(); y++)
		{
			double	dy	= Get_YMin() + y * Get_Cellsize();

			if( dy >= pPolygon->Get_Extent().Get_YMin()
			&&  dy <= pPolygon->Get_Extent().Get_YMax() )
			{
				Line[0].y = Line[1].y = dy;

				memset(bCrossing, 0, Mask.Get_NX() * sizeof(bool));

				//-----------------------------------------
				for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
				{
					TSG_Point	C, A, B	= pPolygon->Get_Point(0, iPart, false);

					for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
					{
						A	= B; B = pPolygon->Get_Point(iPoint, iPart);

						if(	(A.y <= dy && dy <  B.y)
						||  (A.y >  dy && dy >= B.y) )
						{
							SG_Get_Crossing(C, A, B, Line[0], Line[1], false);

							int	ix	= (int)(1 + (C.x - Get_XMin()) / Get_Cellsize());

							if( ix < 0 )
							{
								ix	= 0;
							}
							else if( ix >= Mask.Get_NX() )
							{
								continue;
							}

							bCrossing[ix]	= !bCrossing[ix];
						}
					}
				}

				//-----------------------------------------
				for(int x=xStart, bFill=false; x<=xStop; x++)
				{
					if( bCrossing[x] )
					{
						bFill	= !bFill;
					}

					if( bFill )
					{
						Mask.Set_Value(x, y, 1.0);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	delete[](bCrossing);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
