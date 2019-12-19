
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      grids_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   grids_masking.cpp                   //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "grids_masking.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Masking::CGrids_Masking(void)
{
	Set_Name		(_TL("Grid Collection Masking"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"A masking tool for grid collections. "
		"Cells of the input grid collection will be set to no-data, "
		"depending on the masking option, if their location is either "
		"between or not between the lower and upper surface. "
	));

	Parameters.Add_Grids("",
		"GRIDS"		, _TL("Grid Collection"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grids("",
		"MASKED"	, _TL("Masked Grid Collection"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid_System("",
		"SURFACES"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("SURFACES",
		"LOWER"		, _TL("Lower Surface"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("SURFACES",
		"UPPER"		, _TL("Upper Surface"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Choice("",
		"MASKING"	, _TL("Masking"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("not between upper and lower surface"),
			_TL("between upper and lower surface")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Masking::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Masking::On_Execute(void)
{
	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	CSG_Grid	*pLower	= Parameters("LOWER")->asGrid();
	CSG_Grid	*pUpper	= Parameters("UPPER")->asGrid();

	if( !Get_System().Get_Extent().Intersects(pLower->Get_Extent())
	||  !Get_System().Get_Extent().Intersects(pUpper->Get_Extent()) )
	{
		Error_Set(_TL("lower and/or upper surface layers are covering distinct areas"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("MASKED")->asGrids() && Parameters("MASKED")->asGrids() != pGrids )
	{
		CSG_Grids	*pMasked	= Parameters("MASKED")->asGrids();

		pMasked->Create(*pGrids);
		pMasked->Fmt_Name("%s [%s]", pGrids->Get_Name(), _TL("masked"));

		pGrids	= pMasked;
	}

	bool	bMaskBetween	= Parameters("MASKING")->asInt() == 1;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	px	= Get_XMin() + x * Get_Cellsize(), zMin, zMax;

			if( pLower->Get_Value(px, py, zMin)
			&&  pUpper->Get_Value(px, py, zMax) )
			{
				for(int z=0; z<pGrids->Get_NZ(); z++)
				{
					double	pz	= pGrids->Get_Z(z);

					bool	bBetween	= zMin <= pz && pz < zMax;

					if( bMaskBetween == bBetween )
					{
						pGrids->Set_NoData(x, y, z);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pGrids == Parameters("GRIDS")->asGrids() )
	{
		DataObject_Update(pGrids);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
