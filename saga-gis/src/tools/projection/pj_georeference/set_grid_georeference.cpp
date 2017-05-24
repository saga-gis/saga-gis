/**********************************************************
 * Version $Id: set_grid_georeference.cpp 1834 2013-09-11 18:33:29Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    pj_georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                set_grid_georeference.cpp              //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "set_grid_georeference.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSet_Grid_Georeference::CSet_Grid_Georeference(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Define Georeference for Grids"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"This tool simply allows definition of grid's cellsize and position. "
		"It does not perform any kind of warping but might be helpful, if the "
		"grid has lost this information or is already aligned with the "
		"coordinate system. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "REFERENCED"	, _TL("Referenced Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	Parameters.Add_Choice(
		NULL	, "DEFINITION"	, _TL("Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("cellsize and lower left cell coordinates"),
			_TL("cellsize and upper left cell coordinates"),
			_TL("lower left cell coordinates and left to right range"),
			_TL("lower left cell coordinates and lower to upper range")
		), 0
	);

	pNode	= Parameters.Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Double(pNode, "SIZE", _TL("Cellsize"), _TL(""), 1.0, 0.0, true);

	Parameters.Add_Double(pNode, "XMIN", _TL("Left"    ), _TL(""));
	Parameters.Add_Double(pNode, "XMAX", _TL("Right"   ), _TL(""));
	Parameters.Add_Double(pNode, "YMIN", _TL("Lower"   ), _TL(""));
	Parameters.Add_Double(pNode, "YMAX", _TL("Upper"   ), _TL(""));

	Parameters.Add_Choice(
		pNode	, "CELL_REF"	, _TL("Cell Reference"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("center"),
			_TL("corner")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSet_Grid_Georeference::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CSet_Grid_Georeference::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Grid_System	System	= *pParameters->Get_Parameter("SYSTEM")->asGrid_System();

	if( System.is_Valid() )
	{
		pParameters->Set_Enabled("SIZE"    , false);
		pParameters->Set_Enabled("XMIN"    , false);
		pParameters->Set_Enabled("YMIN"    , false);
		pParameters->Set_Enabled("XMAX"    , false);
		pParameters->Set_Enabled("YMAX"    , false);
		pParameters->Set_Enabled("CELL_REF", false);
	}
	else
	{
		int	Definition	= pParameters->Get_Parameter("DEFINITION")->asInt();

		pParameters->Set_Enabled("SIZE"    , Definition <  2);
		pParameters->Set_Enabled("XMIN"    , true);
		pParameters->Set_Enabled("YMIN"    , Definition == 0 || Definition >= 2);
		pParameters->Set_Enabled("XMAX"    , Definition == 2);
		pParameters->Set_Enabled("YMAX"    , Definition == 1 || Definition == 3);
		pParameters->Set_Enabled("CELL_REF", true);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSet_Grid_Georeference::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System	System	= *Parameters("SYSTEM")->asGrid_System();

	double	size	= System.is_Valid() ? System.Get_Cellsize() : Parameters("SIZE")->asDouble();
	double	xMin	= System.is_Valid() ? System.Get_XMin    () : Parameters("XMIN")->asDouble();
	double	yMin	= System.is_Valid() ? System.Get_YMin    () : Parameters("YMIN")->asDouble();
	double	xMax	= System.is_Valid() ? System.Get_XMax    () : Parameters("XMAX")->asDouble();
	double	yMax	= System.is_Valid() ? System.Get_YMax    () : Parameters("YMAX")->asDouble();

	if( !System.is_Valid() && Parameters("CELL_REF")->asInt() == 1 )	// corner to center coordinates
	{
		xMin	+= 0.5 * size;
		yMin	+= 0.5 * size;
		xMax	-= 0.5 * size;
		yMax	-= 0.5 * size;
	}

	//-----------------------------------------------------
	switch( Parameters("DEFINITION")->asInt() )
	{
	case 0:	// cellsize and lower left cell coordinates
		break;

	case 1:	// cellsize and upper left cell coordinates
		yMin	= yMax - size * Get_NY();
		break;

	case 2:	// lower left cell coordinates and left to right range
		size	= (xMax - xMin) / Get_NX();
		break;

	case 3:	// lower left cell coordinates and lower to upper range
		size	= (yMax - yMin) / Get_NY();
		break;
	}

	//-----------------------------------------------------
	if( !System.Assign(size, xMin, yMin, Get_NX(), Get_NY()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Parameters("REFERENCED")->asGridList()->Del_Items();

	for(int i=0; i<pGrids->Get_Grid_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid	*pGrid	= pGrids->Get_Grid(i);
		CSG_Grid	*pReferenced	= SG_Create_Grid(System, pGrid->Get_Type());

		pReferenced->Set_Name(pGrid->Get_Name());
		pReferenced->Set_Unit(pGrid->Get_Unit());
		pReferenced->Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());
		pReferenced->Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
		pReferenced->Get_MetaData  ()	= pGrid->Get_MetaData  ();
		pReferenced->Get_Projection()	= pGrid->Get_Projection();

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				pReferenced->Set_Value(x, y, pGrid->asDouble(x, y));
			}
		}

		Parameters("REFERENCED")->asGridList()->Add_Item(pReferenced);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
