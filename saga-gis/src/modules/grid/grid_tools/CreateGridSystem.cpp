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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     reklovw@web.de                         //
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

//---------------------------------------------------------
CCreateGridSystem::CCreateGridSystem(void)
{
	Set_Name		(_TL("Create Grid System"));

	Set_Author		("Volker Wichmann (c) 2007");
	
	Set_Description(_TW(
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
		"Cellsize of the new grid system.\n"
		"For all methods supplied to determine the extent but number (1), three possibilities are provided to "
		"adjust Cellsize and grid system extent (please remember, the lower left corner is fixed!):\n"
		"(I) adjust the extent to match the Cellsize\n"
		"(II) adjust the Cellsize to match the extent in E-W direction\n"
		"(III) adjust the Cellsize to match the extent in S-N direction\n\n"
		"Finally it is possible to apply an offset to the lower left corner of the grid system. "
		"In this case check the Use Offset option and specify the offset in W-E and S-N direction. Positive values "
		"result in a shift in E/N, negative in W/S direction.\n"
		"In order to create the grid system the module needs to create a dummy grid."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Dummy Grid"),
		_TL("")
	);

	Parameters.Add_Double(
		NULL	, "INIT"		, _TL("Initialization Value"),
		_TL("Value which is assigned to the dummy grid.")
	);

	Parameters.Add_Double(
		NULL	, "CELLSIZE"	, _TL("Cellsize"),
		_TL(""),
		10.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "M_EXTENT"	, _TL("Extent Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("lower left coordinate and number of rows and columns"),
			_TL("lower left and upper right coordinates"),
			_TL("one or more shapes layers"),
			_TL("one or more grids")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "ADJUST"		, _TL("Adjust"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("extent to cell size"),
			_TL("cell size to left-right extent"),
			_TL("cell size to bottom-top extent")
		), 0
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	pNode	= Parameters.Add_Node(NULL, "X_NODE", _TL("Left-Right"), _TL(""));
	Parameters.Add_Double(pNode, "XMIN", _TL("Left"   ), _TL(""),   0.0);
	Parameters.Add_Double(pNode, "XMAX", _TL("Right"  ), _TL(""), 100.0);
	Parameters.Add_Int   (pNode, "NX"  , _TL("Columns"), _TL(""), 10, 1, true);

	pNode	= Parameters.Add_Node(NULL, "Y_NODE", _TL("Bottom-Top"), _TL(""));
	Parameters.Add_Double(pNode, "YMIN", _TL("Bottom" ), _TL(""),   0.0);
	Parameters.Add_Double(pNode, "YMAX", _TL("Top"    ), _TL(""), 100.0);
	Parameters.Add_Int   (pNode, "NY"  , _TL("Rows"   ), _TL(""), 10, 1, true);

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL	, "SHAPESLIST"	, _TL("Shapes Layers"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDLIST"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Bool(
		NULL	, "USEOFF"		, _TL("Use Offset"),
		_TL(""),
		false
	);

	Parameters.Add_Double(pNode, "XOFFSET", _TL("X Offset"), _TL("Positive values result in a shift in E direction."));
	Parameters.Add_Double(pNode, "YOFFSET", _TL("Y Offset"), _TL("Positive values result in a shift in N direction."));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCreateGridSystem::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Module::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CCreateGridSystem::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "M_EXTENT") )
	{
		pParameters->Set_Enabled("NX"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("NY"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("XMAX"      , pParameter->asInt() == 1);
		pParameters->Set_Enabled("YMAX"      , pParameter->asInt() == 1);
		pParameters->Set_Enabled("XMIN"      , pParameter->asInt() <= 1);
		pParameters->Set_Enabled("YMIN"      , pParameter->asInt() <= 1);
		pParameters->Set_Enabled("X_NODE"    , pParameter->asInt() <= 1);
		pParameters->Set_Enabled("Y_NODE"    , pParameter->asInt() <= 1);
		pParameters->Set_Enabled("ADJUST"    , pParameter->asInt() >= 1);
		pParameters->Set_Enabled("SHAPESLIST", pParameter->asInt() == 2);
		pParameters->Set_Enabled("GRIDLIST"  , pParameter->asInt() == 3);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "USEOFF") )
	{
		pParameters->Set_Enabled("XOFFSET", pParameter->asBool());
		pParameters->Set_Enabled("YOFFSET", pParameter->asBool());
	}

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCreateGridSystem::On_Execute(void)
{
	CSG_Grid_System		System;

	switch( Parameters("M_EXTENT")->asInt() )
	{
	//-----------------------------------------------------
	default:	// lower left coordinate and number of rows and columns
		{
			System.Assign(Parameters("CELLSIZE")->asDouble(),
				Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
				Parameters("NX"  )->asInt   (), Parameters("NY"  )->asInt   ()
			);
		}
		break;

	//-----------------------------------------------------
	case  1:	// lower left and upper right coordinates
		{
			CSG_Rect	Extent(
				Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
				Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
			);

			System	= Get_Adjusted(Parameters("CELLSIZE")->asDouble(), Extent);
		}
		break;

	//-----------------------------------------------------
	case  2:	// one or more shapes layers
		{
			CSG_Parameter_Shapes_List	*pList	= Parameters("SHAPESLIST")->asShapesList();

			if( pList->Get_Count() > 0 )
			{
				CSG_Rect	Extent(pList->asShapes(0)->Get_Extent());

				for(int i=1; i<pList->Get_Count(); i++)
				{
					Extent.Union(pList->asShapes(i)->Get_Extent());
				}

				System	= Get_Adjusted(Parameters("CELLSIZE")->asDouble(), Extent);
			}
		}
		break;

	//-----------------------------------------------------
	case  3:	// one or more grids
		{
			CSG_Parameter_Grid_List	*pList	= Parameters("GRIDLIST")->asGridList();

			if( pList->Get_Count() > 0 )
			{
				CSG_Rect	Extent(pList->asGrid(0)->Get_Extent());

				for(int i=1; i<pList->Get_Count(); i++)
				{
					Extent.Union(pList->asGrid(i)->Get_Extent());
				}

				System	= Get_Adjusted(Parameters("CELLSIZE")->asDouble(), Extent);
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( !System.is_Valid() )
	{
		Error_Set(_TL("invalid grid system"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("USEOFF")->asBool() )
	{
		CSG_Rect	Extent	= System.Get_Extent();

		Extent.Move(
			Parameters("XOFFSET")->asDouble(),
			Parameters("YOFFSET")->asDouble()
		);

		System.Assign(System.Get_Cellsize(), Extent);
	}
		
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= SG_Create_Grid(System);

	if( pGrid )
	{
		pGrid->Set_Name(_TL("Dummy Grid"));
		pGrid->Assign(Parameters("INIT")->asDouble());
		Parameters("GRID")->Set_Value(pGrid);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System CCreateGridSystem::Get_Adjusted(double Cellsize, TSG_Rect Extent)
{
	CSG_Grid_System	System;

	if( Cellsize > 0.0 )
	{
		double	xRange	= Extent.xMax - Extent.xMin;
		double	yRange	= Extent.yMax - Extent.yMin;
		double	n;

		switch( Parameters("ADJUST")->asInt() )
		{
		case 0:	// extent to cell size
			if( modf(xRange / Cellsize, &n) != 0.0 )
			{
				Extent.xMax	= Extent.xMin + Cellsize * floor(0.5 + xRange / Cellsize);
			}
			
			if( modf(yRange / Cellsize, &n) != 0.0 )
			{
				Extent.yMax = Extent.yMin + Cellsize * floor(0.5 + yRange / Cellsize);
			}
			break;

		case 1:	// cell size to left-right extent
			if( modf(xRange / Cellsize, &n) != 0.0 )
			{
				Cellsize	= xRange / floor(xRange / Cellsize);
			}

			if( modf(yRange / Cellsize, &n) != 0.0 )
			{
				Extent.yMax = Extent.yMin + Cellsize * floor(0.5 + yRange / Cellsize);
			}
			break;

		case 2:	// cell size to bottom-top extent
			if( modf(yRange / Cellsize, &n) != 0.0 )
			{
				Cellsize	= yRange / floor(yRange / Cellsize);
			}

			if( modf(xRange / Cellsize, &n) != 0.0 )
			{
				Extent.xMax = Extent.xMin + Cellsize * floor(0.5 + xRange / Cellsize);
			}
			break;
		}

		System.Assign(Cellsize, Extent);
	}

	return( System );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
