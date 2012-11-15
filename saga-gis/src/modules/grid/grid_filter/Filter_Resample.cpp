/**********************************************************
 * Version $Id: Filter_Resample.cpp 1086 2011-06-08 10:12:02Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
	// 1. Info...

	Set_Name		(_TL("Resampling Filter"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

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
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "LOPASS"		, _TL("Low Pass Filter"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "HIPASS"		, _TL("High Pass Filter"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "SCALE"		, _TL("Scale Factor"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Resample::On_Execute(void)
{
	double		Cellsize;
	CSG_Grid	*pGrid, *pLoPass, *pHiPass;

	//-----------------------------------------------------
	pGrid		= Parameters("GRID"  )->asGrid();
	pLoPass		= Parameters("LOPASS")->asGrid();
	pHiPass		= Parameters("HIPASS")->asGrid();
	Cellsize	= Parameters("SCALE" )->asDouble() * Get_Cellsize();

	//-----------------------------------------------------
	if( Cellsize > 0.5 * SG_Get_Length(Get_System()->Get_XRange(), Get_System()->Get_YRange()) )
	{
		Error_Set(_TL("resampling cell size is too large"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	Grid(CSG_Grid_System(Cellsize, Get_XMin(), Get_YMin(), Get_XMax(), Get_YMax()), SG_DATATYPE_Float);

	Grid.Assign(pGrid, GRID_INTERPOLATION_Mean_Cells);

	//-----------------------------------------------------
	pLoPass->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("Low Pass")));
	pHiPass->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("High Pass")));

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	z, px	= Get_XMin() + x * Get_Cellsize();

			if( Grid.Get_Value(px, py, z) )
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
