
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Resample.cpp                   //
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
#include <string.h>

#include "Grid_Resample.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Resample::CGrid_Resample(void)
{
	CParameter	*pNode;
	CParameters	*pParameters;

	//-----------------------------------------------------
	Set_Name(_TL("Resampling"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Resampling of grids.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Resampled Grid"),
		""
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Target Grid"),
		"",

		_TL("Specify dimensions|Create new grid in existing project|Overwrite existing grid|")
	);

	Parameters.Add_Value(
		NULL	, "KEEP_TYPE"	, _TL("Preserve Data Type"),
		"",
		PARAMETER_TYPE_Bool		, false
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("DIMENSIONS"	, _TL("Target Grid Dimensions")	, "");

	pParameters->Add_Value(
		NULL	, "CELLSIZE"	, _TL("Cell Size"),
		"",
		PARAMETER_TYPE_Double	, 1.0, 0.0, true
	);

	pParameters->Add_Value(
		NULL	, "CELLS_NX"	, _TL("Cell Count: Columns"),
		"",
		PARAMETER_TYPE_Int		, 2, 2, true
	);

	pParameters->Add_Value(
		NULL	, "CELLS_NY"	, _TL("Cell Count: Rows"),
		"",
		PARAMETER_TYPE_Int		, 2, 2, true
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("TARGET"		, _TL("Target Grid")				, "");

	pNode	= pParameters->Add_Grid(
		NULL	, "TARGET"		, _TL("Target Grid"),
		"",
		PARAMETER_INPUT			, false
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("SCALE_UP"	, _TL("Up-Scaling")				, "");

	pNode	= pParameters->Add_Choice(
		NULL	, "METHOD"		, _TL("Interpolation Method"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"),
			_TL("Mean Value")
		), 5
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("SCALE_DOWN"	, _TL("Down-Scaling")			, "");

	pNode	= pParameters->Add_Choice(
		NULL	, "METHOD"		, _TL("Interpolation Method"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);
}

//---------------------------------------------------------
CGrid_Resample::~CGrid_Resample(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Resample::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
{
	int		nx, ny;
	double	cellsize;
	CGrid	*pGrid;

	if( !strcmp(pParameters->Get_Identifier(), "DIMENSIONS") )
	{
		pGrid		=  Parameters               ("INPUT")   ->asGrid();
		cellsize	= pParameters->Get_Parameter("CELLSIZE")->asDouble();
		nx			= pParameters->Get_Parameter("CELLS_NX")->asInt();
		ny			= pParameters->Get_Parameter("CELLS_NY")->asInt();

		if( pGrid && cellsize > 0.0 )
		{
			if(      !strcmp(pParameter->Get_Identifier(), "CELLSIZE") )
			{
				nx			= 1 + (int)(pGrid->Get_XRange() / cellsize);
				ny			= 1 + (int)(pGrid->Get_YRange() / cellsize);
			}
			else if( !strcmp(pParameter->Get_Identifier(), "CELLS_NX") )
			{
				cellsize	= pGrid->Get_XRange() / (nx - 1);
				ny			= 1 + (int)(pGrid->Get_YRange() / cellsize);
			}
			else if( !strcmp(pParameter->Get_Identifier(), "CELLS_NY") )
			{
				cellsize	= pGrid->Get_YRange() / (ny - 1);
				nx			= 1 + (int)(pGrid->Get_XRange() / cellsize);
			}

			pParameters->Get_Parameter("CELLSIZE")->Set_Value(cellsize);
			pParameters->Get_Parameter("CELLS_NX")->Set_Value(nx);
			pParameters->Get_Parameter("CELLS_NY")->Set_Value(ny);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Resample::On_Execute(void)
{
	bool				bResult;
	double				Cellsize;
	TGrid_Interpolation	Interpolation;
	CGrid				*pInput, *pOutput;
	CGrid_System		System;
	CParameters			*pParameters;

	//-----------------------------------------------------
	bResult	= false;

	pInput	= Parameters("INPUT")->asGrid();
	pOutput	= NULL;

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	// Target Dimensions...
		Get_Extra_Parameters("DIMENSIONS")->Get_Parameter("CELLSIZE")->Set_Value(pInput->Get_Cellsize());
		Get_Extra_Parameters("DIMENSIONS")->Get_Parameter("CELLS_NX")->Set_Value(pInput->Get_NX());
		Get_Extra_Parameters("DIMENSIONS")->Get_Parameter("CELLS_NY")->Set_Value(pInput->Get_NY());

		if(	Dlg_Extra_Parameters("DIMENSIONS") && (Cellsize = Get_Extra_Parameters("DIMENSIONS")->Get_Parameter("CELLSIZE")->asDouble()) > 0.0 )
		{
			System.Assign(Cellsize, pInput->Get_XMin(), pInput->Get_YMin(),
				1 + (int)(pInput->Get_XRange() / Cellsize),
				1 + (int)(pInput->Get_YRange() / Cellsize)
			);
		}
		break;

	case 1:	// Target Project...
		if( Dlg_Extra_Parameters("TARGET") )
		{
			System.Assign(Get_Extra_Parameters("TARGET")->Get_Parameter("TARGET")->asGrid()->Get_System());
		}
		break;

	case 2:	// Target Grid...
		if( Dlg_Extra_Parameters("TARGET") )
		{
			System.Assign(Get_Extra_Parameters("TARGET")->Get_Parameter("TARGET")->asGrid()->Get_System());

			pOutput	= Get_Extra_Parameters("TARGET")->Get_Parameter("TARGET")->asGrid();
		}
		break;
	}

	//-----------------------------------------------------
	if( System.is_Valid() && pInput->is_Intersecting(System.Get_Extent()) )
	{
		pParameters	= NULL;

		//-------------------------------------------------
		// Up-Scaling...

		if( pInput->Get_Cellsize() < System.Get_Cellsize() )
		{
			if( Dlg_Extra_Parameters("SCALE_UP") )
			{
				switch( Get_Extra_Parameters("SCALE_UP")->Get_Parameter("METHOD")->asInt() )
				{
				case 0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
				case 1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
				case 2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
				case 3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
				case 4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
				case 5:	Interpolation	= GRID_INTERPOLATION_Undefined;			break;
				}

				pParameters	= Get_Extra_Parameters("SCALE_UP");
			}
		}

		//-------------------------------------------------
		// Down-Scaling...

		else
		{
			if( Dlg_Extra_Parameters("SCALE_DOWN") )
			{
				switch( Get_Extra_Parameters("SCALE_DOWN")->Get_Parameter("METHOD")->asInt() )
				{
				case 0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
				case 1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
				case 2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
				case 3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
				case 4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
				}

				pParameters	= Get_Extra_Parameters("SCALE_DOWN");
			}
		}

		//-------------------------------------------------
		if( pParameters )
		{
			if( !pOutput )
			{
				pOutput	= SG_Create_Grid(System, Parameters("KEEP_TYPE")->asBool() || (Interpolation == GRID_INTERPOLATION_NearestNeighbour) ? pInput->Get_Type() : GRID_TYPE_Undefined);
			}

			pOutput->Set_Name(pInput->Get_Name());

			pOutput->Assign(pInput, Interpolation);

			Parameters("GRID")->Set_Value(pOutput);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
