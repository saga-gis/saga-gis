/**********************************************************
 * Version $Id: set_grid_georeference.cpp 1834 2013-09-11 18:33:29Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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

	Set_Author		(SG_T("O.Conrad (c) 2013"));

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
	Parameters.Add_Choice(
		NULL	, "DEFINITION"	,_TL("Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("cellsize and lower left center coordinates"),
			_TL("cellsize and lower left corner coordinates"),
			_TL("cellsize and upper left center coordinates"),
			_TL("cellsize and upper left corner coordinates"),
			_TL("lower left and upper right center coordinates"),
			_TL("lower left and upper right corner coordinates")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "SIZE"		, _TL("Cellsize"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "XMIN"		, _TL("Left"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	Parameters.Add_Value(
		NULL	, "YMIN"		, _TL("Lower"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	Parameters.Add_Value(
		NULL	, "XMAX"		, _TL("Right"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	Parameters.Add_Value(
		NULL	, "YMAX"		, _TL("Upper"),
		_TL(""),
		PARAMETER_TYPE_Double
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSet_Grid_Georeference::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DEFINITION") )
	{
		pParameters->Get_Parameter("SIZE")->Set_Enabled(pParameter->asInt()  < 4);
		pParameters->Get_Parameter("XMAX")->Set_Enabled(pParameter->asInt() >= 4);
		pParameters->Get_Parameter("YMAX")->Set_Enabled(pParameter->asInt() >= 2);
		pParameters->Get_Parameter("YMIN")->Set_Enabled(pParameter->asInt() >= 4 || pParameter->asInt() < 2);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSet_Grid_Georeference::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	xMin, yMin, size;

	switch( Parameters("DEFINITION")->asInt() )
	{
	case 0:	// cellsize and lower left center coordinates
		size	= Parameters("SIZE")->asDouble();
		xMin	= Parameters("XMIN")->asDouble();
		yMin	= Parameters("YMIN")->asDouble();
		break;

	case 1:	// cellsize and lower left corner coordinates
		size	= Parameters("SIZE")->asDouble();
		xMin	= Parameters("XMIN")->asDouble() + size * 0.5;
		yMin	= Parameters("YMIN")->asDouble() + size * 0.5;
		break;

	case 2:	// cellsize and upper left center coordinates
		size	= Parameters("SIZE")->asDouble();
		xMin	= Parameters("XMIN")->asDouble();
		yMin	= Parameters("YMAX")->asDouble() - size * Get_NY();
		break;

	case 3:	// cellsize and upper left corner coordinates
		size	= Parameters("SIZE")->asDouble();
		xMin	= Parameters("XMIN")->asDouble() + size * 0.5;
		yMin	= Parameters("YMAX")->asDouble() - size * (0.5 + Get_NY());
		break;

	case 4:	// lower left and upper right center coordinates
		size	= (Parameters("XMAX")->asDouble() - Parameters("XMIN")->asDouble()) / Get_NX();
		xMin	= Parameters("XMIN")->asDouble();
		yMin	= Parameters("YMIN")->asDouble();
		break;

	case 5:	// lower left and upper right corner coordinates
		size	= (Parameters("XMAX")->asDouble() - Parameters("XMIN")->asDouble()) / (Get_NX() + 1);
		xMin	= Parameters("XMIN")->asDouble() + size * 0.5;
		yMin	= Parameters("YMIN")->asDouble() + size * 0.5;
		break;
	}

	//-----------------------------------------------------
	CSG_Grid_System	System;

	if( !System.Assign(size, xMin, yMin, Get_NX(), Get_NY()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Parameters("REFERENCED")->asGridList()->Del_Items();

	for(int i=0; i<pGrids->Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid	*pGrid	= pGrids->asGrid(i);
		CSG_Grid	*pReferenced	= SG_Create_Grid(System, pGrid->Get_Type());

		pReferenced->Set_Name(pGrid->Get_Name());
		pReferenced->Set_Unit(pGrid->Get_Unit());
		pReferenced->Set_ZFactor(pGrid->Get_ZFactor());
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
