
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pc_from_grid.cpp                   //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "pc_from_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_From_Grid::CPC_From_Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud from Grid Points"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Z Value"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Additional Values"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_From_Grid::On_Execute(void)
{
	int						x, y, i;
	TSG_Point				p;
	CSG_Grid				*pGrid;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_PointCloud			*pPoints;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pGrids	= Parameters("GRIDS")	->asGridList();
	pPoints	= Parameters("POINTS")	->asPointCloud();

	pPoints->Create();
	pPoints->Set_Name(pGrid->Get_Name());

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		pPoints->Add_Field(pGrids->asGrid(i)->Get_Name(), SG_DATATYPE_Float);
	}

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( !pGrid->is_NoData(x, y) )
			{
				pPoints->Add_Point(p.x, p.y, pGrid->asDouble(x, y));

				for(i=0; i<pGrids->Get_Count(); i++)
				{
					pPoints->Set_Value(3 + i, pGrids->asGrid(i)->Get_Value(p));
				}
			}
		}
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
