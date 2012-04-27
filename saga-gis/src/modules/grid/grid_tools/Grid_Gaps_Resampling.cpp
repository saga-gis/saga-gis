/**********************************************************
 * Version $Id: Grid_Gaps_Resampling.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

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
//                Grid_Gaps_Resampling.cpp               //
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
#include "Grid_Gaps_Resampling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Gaps_Resampling::CGrid_Gaps_Resampling(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Close Gaps with Stepwise Resampling"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Close gaps of a grid data set (i.e. eliminate no data values). "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"				, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "MASK"				, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "RESULT"				, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Value(
		NULL	, "GROW"			, _TL("Grow Factor"),
		_TL(""),
		PARAMETER_TYPE_Double, 2.0, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "PYRAMIDS"		, _TL("Use Pyramids"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "START"			, _TL("Start Size"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("grid cell size"),
			_TL("user defined size")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "START_SIZE"		, _TL("User Defined Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Gaps_Resampling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("PYRAMIDS")) )
	{
		pParameters->Get_Parameter("START")->Set_Enabled(!pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("START")) )
	{
		pParameters->Get_Parameter("START_SIZE")->Set_Enabled(pParameter->asInt() == 1);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Gaps_Resampling::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("RESULT")->asGrid();
	CSG_Grid	*pMask	= Parameters("MASK"  )->asGrid();

	if( pGrid == NULL )
	{
		pGrid	= Parameters("INPUT")->asGrid();
	}
	else
	{
		pGrid->Assign(Parameters("INPUT")->asGrid());
		pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("INPUT")->asGrid()->Get_Name(), _TL("no gaps")));
	}

	TSG_Grid_Interpolation	Interpolation	= (TSG_Grid_Interpolation)Parameters("INTERPOLATION")->asInt();

	double	Grow	= Parameters("GROW")->asDouble();

	//-----------------------------------------------------
	if( !Parameters("PYRAMIDS")->asBool() )
	{
		int		nCells, nCells_0;
		double	Size;

		nCells_0	= pGrid->Get_NoData_Count();
		Size		= Parameters("START")->asInt() == 1 ? Parameters("START_SIZE")->asDouble() : Grow * Get_Cellsize();

		//-------------------------------------------------
		for(nCells=nCells_0; nCells>0 && Set_Progress(nCells_0-nCells, nCells_0); Size*=Grow)
		{
			Process_Set_Text(CSG_String::Format(SG_T("%s: %d; %s: %f"), _TL("no-data cells"), nCells, _TL("patch size"), Size));

			CSG_Grid	Patch(CSG_Grid_System(Size, Get_System()->Get_Extent()));

			SG_UI_Progress_Lock(true);
			Patch.Assign(pGrid, GRID_INTERPOLATION_BSpline);
			SG_UI_Progress_Lock(false);

			nCells	= 0;

			#pragma omp parallel for
			for(int y=0; y<Get_NY(); y++)
			{
				double	py	= Get_YMin() + y * Get_Cellsize();

				for(int x=0; x<Get_NX(); x++)
				{
					if( pGrid->is_NoData(x, y) && (!pMask || !pMask->is_NoData(x, y)) )
					{
						double	px	= Get_XMin() + x * Get_Cellsize();

						if( Patch.is_InGrid_byPos(px, py) )
						{
							pGrid->Set_Value(x, y, Patch.Get_Value(px, py, Interpolation));
						}
						else
						{
							nCells++;
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	else // if( Parameters("PYRAMIDS")->asBool() == true )
	{
		CSG_Grid_Pyramid	Pyramid;

		if( !Pyramid.Create(pGrid, Grow) )
		{
			return( false );
		}

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			double	py	= Get_YMin() + y * Get_Cellsize();

			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( pGrid->is_NoData(x, y) && (!pMask || !pMask->is_NoData(x, y)) )
				{
					double	px	= Get_XMin() + x * Get_Cellsize();

					for(int i=0; i<Pyramid.Get_Count(); i++)
					{
						CSG_Grid	*pPatch	= Pyramid.Get_Grid(i);

						if( pPatch->is_InGrid_byPos(px, py) )
						{
							pGrid->Set_Value(x, y, pPatch->Get_Value(px, py, Interpolation));

							break;
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
bool CGrid_Gaps_Resampling::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("RESULT")->asGrid();
	CSG_Grid	*pMask	= Parameters("MASK"  )->asGrid();

	if( pGrid == NULL )
	{
		pGrid	= Parameters("INPUT")->asGrid();
	}
	else
	{
		pGrid->Assign(Parameters("INPUT")->asGrid());
		pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("INPUT")->asGrid()->Get_Name(), _TL("no gaps")));
	}

	//-----------------------------------------------------
	TSG_Grid_Interpolation	Interpolation	= (TSG_Grid_Interpolation)Parameters("INTERPOLATION")->asInt();


	//-----------------------------------------------------
	if( pGrid == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pGrid);
	}

	return( true );
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
