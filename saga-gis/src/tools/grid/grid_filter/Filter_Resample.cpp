/**********************************************************
 * Version $Id: Filter_Resample.cpp 1086 2011-06-08 10:12:02Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Filter_Resample.cpp                  //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#include "Filter_Resample.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Resample::CFilter_Resample(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Resampling Filter"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Resampling filter for grids. Resamples in a first step the "
		"given grid to desired resampling cell size, expressed as "
		"multiple of the original cell size (scale factor). This is an up-scaling "
		"through which cell values are aggregated as cell area weighted "
		"means. Second step is the down-scaling to original cell size "
		"using spline interpolation. Specially for larger search distances "
		"this is a comparably fast alternative for simple low and high "
		"pass filter operations. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"LOPASS"	, _TL("Low Pass Filter"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"HIPASS"	, _TL("High Pass Filter"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"SCALE"		, _TL("Scale Factor"),
		_TL(""),
		10.0, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Resample::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid		= Parameters("GRID"  )->asGrid();
	CSG_Grid	*pLoPass	= Parameters("LOPASS")->asGrid();
	CSG_Grid	*pHiPass	= Parameters("HIPASS")->asGrid();

	double	Cellsize	= Parameters("SCALE" )->asDouble() * Get_Cellsize();

	//-----------------------------------------------------
	if( Cellsize > 0.5 * SG_Get_Length(Get_System()->Get_XRange(), Get_System()->Get_YRange()) )
	{
		Error_Set(_TL("resampling cell size is too large"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	Grid(CSG_Grid_System(Cellsize, Get_XMin(), Get_YMin(), Get_XMax(), Get_YMax()), SG_DATATYPE_Float);

	Grid.Assign(pGrid, GRID_RESAMPLING_Mean_Cells);

	//-----------------------------------------------------
	pLoPass->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Low Pass" ));
	pHiPass->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("High Pass"));

	CSG_Colors	Colors;

	DataObject_Get_Colors(pGrid  , Colors);
	DataObject_Set_Colors(pLoPass, Colors);
	DataObject_Set_Colors(pHiPass, 11, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	z, px	= Get_XMin() + x * Get_Cellsize();

			if( !pGrid->is_NoData(x, y) && Grid.Get_Value(px, py, z) )
			{
				pLoPass->Set_Value(x, y, z);
				pHiPass->Set_Value(x, y, pGrid->asDouble(x, y) - z);
			}
			else
			{
				pLoPass->Set_NoData(x, y);
				pHiPass->Set_NoData(x, y);
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
