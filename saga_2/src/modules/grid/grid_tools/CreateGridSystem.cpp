
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   CreateGridSystem                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  CreateGridSystem.cpp                 //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at				     //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "CreateGridSystem.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

CCreateGridSystem::CCreateGridSystem(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;

	Parameters.Set_Name(_TL("Create Grid System"));

	Set_Author(_TL("Copyrights (c) 2007 by Volker Wichmann"));
	
	Parameters.Set_Description(_TW(
		"This module creates a new user specified Grid System for use with other modules.\n\n"
		"First of all, please consider the following issues before using the module:\n"
		"(a) all calculations of the module refer to the lower left corner of the grid system, i.e. "
		"the xMin and yMin values. This coordinate is fixed unless you specify an offset.\n"
		"(b) the module follows the philosophy of SAGA in that the values describing the extent refer to the "
		"cell centers. If you like to match the extent with the border of a grid, use an offset.\n\n"
		"The module provides four possibilities to set/determine the extent of the grid system:\n"
		"(1) by specifying the coordinate of the lower left cell (xMin, yMin) and the number of cells in W-E (NX) and S-N (NY) direction\n"
		"(2) by specifying the coordinates the of lower left (xMin, yMin) and the upper right (xMax, yMax) cell\n"
		"(3) by the extent of the shape(s) provided in the Data Objects section\n"
		"(4) by the extent of the grid(s) provided in the Data Objects section\n\n"
		"After selecting the appropriate method to determine the extent, the next step is to specify the "
		"cellsize of the new grid system.\n"
		"For all methods supplied to determine the extent but number (1), three possibilities are provided to "
		"adjust cellsize and grid system extent (please remember, the lower left corner is fixed!):\n"
		"(I) adjust the extent to match the cellsize\n"
		"(II) adjust the cellsize to match the extent in E-W direction\n"
		"(III) adjust the cellsize to match the extent in S-N direction\n\n"
		"Finally it is possible to apply an offset to the lower left corner of the grid system. "
		"In this case check the Use Offset option and specify the offset in W-E and S-N direction. Positive values "
		"result in a shift in E/N, negative in W/S direction.\n"
		"In order to create the grid system the module needs to create a dummy grid.")
	);


	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Dummy Grid"),
		_TL("")
	);

	pNode_0 = Parameters.Add_Choice(
		NULL, "M_EXTENT", _TL("Set extent by"),
		_TL("Please choose a method of module operation."),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("xMin, yMin, NX, NY"),
			_TL("xMin, yMin, xMax, yMax"),
			_TL("Shape(s)"),
			_TL("Grid(s)")
		), 0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "XMIN", _TL("xMin"),
		_TL("minimum x value"),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "YMIN", _TL("yMin"),
		_TL("minimum y value"),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "XMAX", _TL("xMax"),
		_TL("maximum x value"),
		PARAMETER_TYPE_Double, 100.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "YMAX", _TL("yMax"),
		_TL("maximum y value"),
		PARAMETER_TYPE_Double, 100.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "NX", _TL("NX"),
		_TL("number of cells in W-E direction"),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "NY", _TL("NY"),
		_TL("number of cells in S-N direction"),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	pNode_0 = Parameters.Add_Value(
		NULL, "CELLSIZE", _TL("Cellsize"),
		_TL("Cellsize"),
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	pNode_0 = Parameters.Add_Choice(
		NULL, "ADJUST", _TL("Adjust"),
		_TL("Choose method how to adjust input values."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Extent to CellSize"),
			_TL("CellSize to W-E Extent"),
			_TL("CellSize to S-N Extent")
		), 0
	);

	pNode_0	= Parameters.Add_Value(
		NULL, "USEOFF", _TL("Use Offset"),
		_TL("Apply offset?"),
		PARAMETER_TYPE_Bool, false
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "XOFFSET", _TL("X offset"),
		_TL("Positive values result in a shift in E direction."),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "YOFFSET", _TL("Y offset"),
		_TL("Positive values result in a shift in N direction."),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Shapes_List(
		NULL, "SHAPESLIST", _TL("Shape(s)"),
		_TL("Shape(s) to fit extent to"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_List(
		NULL, "GRIDLIST", _TL("Grid(s)"),
		_TL("Grid(s) to fit extent to"),
		PARAMETER_INPUT_OPTIONAL
	);

}

//---------------------------------------------------------
CCreateGridSystem::~CCreateGridSystem(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

bool CCreateGridSystem::On_Execute(void)		
{
	CSG_Grid					*pDummy;
	CSG_Shapes					*pShapes;
	CSG_Rect					extent;
	CSG_Parameter_Shapes_List	*pShapesList;
	CSG_Parameter_Grid_List		*pGridList;
	CSG_Grid_System				System;

	double		xMin, xMax, yMin, yMax, cellsize, offset_x, offset_y, xRange, yRange, n;
	int			NX, NY, m_extent, m_adjust, i;
	bool		useoff;


	xMin		= Parameters("XMIN")->asDouble();
	yMin		= Parameters("YMIN")->asDouble();
	xMax		= Parameters("XMAX")->asDouble();
	yMax		= Parameters("YMAX")->asDouble();
	NX			= Parameters("NX")->asInt();
	NY			= Parameters("NY")->asInt();
	cellsize	= Parameters("CELLSIZE")->asDouble();
	offset_x	= Parameters("XOFFSET")->asDouble();
	offset_y	= Parameters("YOFFSET")->asDouble();
	useoff		= Parameters("USEOFF")->asBool();
	m_extent	= Parameters("M_EXTENT")->asInt();
	m_adjust	= Parameters("ADJUST")->asInt();
	pShapesList	= Parameters("SHAPESLIST")->asShapesList();
	pGridList	= Parameters("GRIDLIST")->asGridList();


	if( useoff )
	{
		xMin += offset_x;
		yMin += offset_y;
		xMax += offset_x;
		yMax += offset_y;
	}

	switch( m_extent )
	{
	case 0:					// xMin, yMin, NX, NY

		System.Assign(cellsize, xMin, yMin, NX, NY);
		break;

	case 1:					// xMin, yMin, xMax, yMax

		if( xMin > xMax || yMin > yMax )
		{
			Message_Add(CSG_String::Format(_TL("\nError: Please verify your xMin, yMin, xMax, yMax settings!\n")));
			return false;
		}

		xRange = xMax - xMin;
		yRange = yMax - yMin;

		if( m_adjust == 0 )			// extent to cellsize
		{
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) floor(xRange / cellsize + 0.5);
				xMax = xMin + NX * cellsize;
			}
			
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) floor(yRange / cellsize + 0.5);
				yMax = yMin + NY * cellsize;
			}
		}
		else if( m_adjust == 1)		// cellsize to W-E extent
		{
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) (xRange/cellsize);
				cellsize = xRange/NX;
			}			
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) floor(yRange / cellsize + 0.5);
				yMax = yMin + NY * cellsize;
			}
		}
		else						// cellsize to S-N extent
		{
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) (yRange/cellsize);
				cellsize = yRange/NY;
			}	
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) floor(xRange / cellsize + 0.5);
				xMax = xMin + NX * cellsize;
			}
		}

		System.Assign(cellsize, xMin, yMin, xMax, yMax);
		break;

	case 2:			// Shape(s)

		if( pShapesList == NULL || pShapesList->Get_Count() == 0)
		{
			Message_Add(CSG_String::Format(_TL("\nError: the method Extent by Shape(s) requires shape(s) as input!\n")));
			return false;
		}

		for (i=0; i<pShapesList->Get_Count(); i++)
		{
			pShapes = pShapesList->asShapes(i);
			extent = pShapes->Get_Extent();

			if (i==0)
			{
				xMin = extent.Get_XMin();
				yMin = extent.Get_YMin();
				xMax = extent.Get_XMax();
				yMax = extent.Get_YMax();
			}
			else
			{
				xMin = (extent.Get_XMin() < xMin) ? extent.Get_XMin() : xMin;
				yMin = (extent.Get_YMin() < yMin) ? extent.Get_YMin() : yMin;
				xMax = (extent.Get_XMax() > xMax) ? extent.Get_XMax() : xMax;
				yMax = (extent.Get_YMax() > yMax) ? extent.Get_YMax() : yMax;
			}		
		}

		if( useoff )
		{
			xMin += offset_x;
			xMax += offset_x;
			yMin += offset_y;
			yMax += offset_y;
		}

		xRange = xMax - xMin;
		yRange = yMax - yMin;

		if( m_adjust == 0 )			// extent to cellsize
		{
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) floor(xRange / cellsize + 0.5);
				xMax = xMin + NX * cellsize;
			}
			
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) floor(yRange / cellsize + 0.5);
				yMax = yMin + NY * cellsize;
			}
		}
		else if( m_adjust == 1)		// cellsize to W-E extent
		{
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) (xRange/cellsize);
				cellsize = xRange/NX;
			}	
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) floor(yRange / cellsize + 0.5);
				yMax = yMin + NY * cellsize;
			}
		}
		else						// cellsize to S-N extent
		{
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) (yRange/cellsize);
				cellsize = yRange/NY;
			}
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) floor(xRange / cellsize + 0.5);
				xMax = xMin + NX * cellsize;
			}
		}

		System.Assign(cellsize, xMin, yMin, xMax, yMax);
		break;	

	case 3:				// Grid(s)

		if( pGridList == NULL || pGridList->Get_Count() == 0)
		{
			Message_Add(CSG_String::Format(_TL("\nError: the method Extent by Grid(s) requires grid(s) as input!\n")));
			return false;
		}
				
		for (i=0; i<pGridList->Get_Count(); i++)
		{
			pDummy = pGridList->asGrid(i);
			extent = pDummy->Get_Extent();

			if (i==0)
			{
				xMin = extent.Get_XMin();
				yMin = extent.Get_YMin();
				xMax = extent.Get_XMax();
				yMax = extent.Get_YMax();
			}
			else
			{
				xMin = (extent.Get_XMin() < xMin) ? extent.Get_XMin() : xMin;
				yMin = (extent.Get_YMin() < yMin) ? extent.Get_YMin() : yMin;
				xMax = (extent.Get_XMax() > xMax) ? extent.Get_XMax() : xMax;
				yMax = (extent.Get_YMax() > yMax) ? extent.Get_YMax() : yMax;
			}		
		}

		if( useoff )
		{
			xMin += offset_x;
			xMax += offset_x;
			yMin += offset_y;
			yMax += offset_y;
		}

		xRange = xMax - xMin;
		yRange = yMax - yMin;

		if( m_adjust == 0 )			// extent to cellsize
		{
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) floor(xRange / cellsize + 0.5);
				xMax = xMin + NX * cellsize;
			}
			
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) floor(yRange / cellsize + 0.5);
				yMax = yMin + NY * cellsize;
			}
		}
		else if( m_adjust == 1)		// cellsize to W-E extent
		{
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) (xRange/cellsize);
				cellsize = xRange/NX;
			}			
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) floor(yRange / cellsize + 0.5);
				yMax = yMin + NY * cellsize;
			}
		}
		else						// cellsize to S-N extent
		{
			if( modf((yRange/cellsize), &n) != 0.0 )
			{
				NY = (int) (yRange/cellsize);
				cellsize = yRange/NY;
			}			
			if( modf((xRange/cellsize), &n) != 0.0 )
			{
				NX = (int) floor(xRange / cellsize + 0.5);
				xMax = xMin + NX * cellsize;
			}
		}

		System.Assign(cellsize, xMin, yMin, xMax, yMax);
		break;
	}
		
	pDummy = SG_Create_Grid(System, GRID_TYPE_Int);
	pDummy->Assign(1.0);
	pDummy->Set_Name(_TL("Grid"));
	pDummy->Set_NoData_Value_Range(-99999.0, -9999.0);
	Parameters("GRID")->Set_Value(pDummy);

	return (true);

}

