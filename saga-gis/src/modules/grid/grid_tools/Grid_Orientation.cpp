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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Orientation.cpp                  //
//                                                       //
//                Copyright (C) 2016 by                  //
//                     Olaf Conrad                       //
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
#include "Grid_Orientation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Copy::CGrid_Copy(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Copy Grid"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Copy a grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "COPY"	, _TL("Copy"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Copy::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();
	CSG_Grid	*pCopy	= Parameters("COPY")->asGrid();

	return( pCopy->Create(*pGrid) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Invert::CGrid_Invert(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Invert Grid"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Invert a grid, i.e. the highest value becomes the lowest and vice versa. "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "INVERSE"	, _TL("Inverse Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Invert::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("INVERSE")->asGrid();

	if( pGrid == NULL )
	{
		pGrid	= Parameters("GRID")->asGrid();
	}
	else if( pGrid != Parameters("GRID")->asGrid() )
	{
		pGrid->Create(*Parameters("GRID")->asGrid());

		pGrid->Set_Name(CSG_String::Format("%s [%s]", pGrid->Get_Name(), _TL("Inverse")));
	}

	//-----------------------------------------------------
	double	zMin	= pGrid->Get_ZMin();
	double	zMax	= pGrid->Get_ZMax();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				pGrid->Set_Value(x, y, zMax - (pGrid->asDouble(x, y) - zMin));
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("GRID")->asGrid() )
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

//---------------------------------------------------------
CGrid_Mirror::CGrid_Mirror(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Mirror Grid"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Mirror a grid at its center axes', either vertically, horizontally or both. "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "MIRROR"	, _TL("Mirror Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("horizontally"),
			_TL("vertically"),
			_TL("both")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Mirror::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("MIRROR")->asGrid();

	if( pGrid == NULL )
	{
		pGrid	= Parameters("GRID")->asGrid();
	}
	else if( pGrid != Parameters("GRID")->asGrid() )
	{
		pGrid->Create(*Parameters("GRID")->asGrid());

		pGrid->Set_Name(CSG_String::Format("%s [%s %s]", pGrid->Get_Name(), _TL("mirrored"), Parameters("METHOD")->asString()));
	}

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	//-----------------------------------------------------
	case  0:	// vertically
		{
			for(int xa=0, xb=Get_NX()-1; xa<xb && SG_UI_Process_Set_Progress(xa, Get_NX()/2); xa++, xb--)
			{
				#pragma omp parallel for
				for(int y=0; y<Get_NY(); y++)
				{
					double	d             = pGrid->asDouble(xa, y);
					pGrid->Set_Value(xa, y, pGrid->asDouble(xb, y));
					pGrid->Set_Value(xb, y, d);
				}
			}
		}
		break;

	//-----------------------------------------------------
	case  1:	// horizontally
		{
			for(int ya=0, yb=Get_NY()-1; ya<yb && SG_UI_Process_Set_Progress(ya, Get_NY()/2); ya++, yb--)
			{
				#pragma omp parallel for
				for(int x=0; x<Get_NX(); x++)
				{
					double	d             = pGrid->asDouble(x, ya);
					pGrid->Set_Value(x, ya, pGrid->asDouble(x, yb));
					pGrid->Set_Value(x, yb, d);
				}
			}
		}
		break;

	//-----------------------------------------------------
	default:	// both
		{
			for(int ya=0, yb=Get_NY()-1; ya<=yb && SG_UI_Process_Set_Progress(ya, Get_NY()/2); ya++, yb--)
			{
				#pragma omp parallel for
				for(int xa=0; xa<=Get_NX()/2; xa++)
				{
					int	xb	= Get_NX() - xa - 1;

					if( ya < yb && xa < xb )
					{
						double	d              = pGrid->asDouble(xa, ya);
						pGrid->Set_Value(xa, ya, pGrid->asDouble(xb, yb));
						pGrid->Set_Value(xb, yb, d);

						d                      = pGrid->asDouble(xa, yb);
						pGrid->Set_Value(xa, yb, pGrid->asDouble(xb, ya));
						pGrid->Set_Value(xb, ya, d);
					}
					else if( xa < xb )
					{
						double	d              = pGrid->asDouble(xa, ya);
						pGrid->Set_Value(xa, ya, pGrid->asDouble(xb, ya));
						pGrid->Set_Value(xb, ya, d);
					}
					else if( ya < yb )
					{
						double	d              = pGrid->asDouble(xa, ya);
						pGrid->Set_Value(xa, ya, pGrid->asDouble(xa, yb));
						pGrid->Set_Value(xa, yb, d);
					}
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("GRID")->asGrid() )
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

//---------------------------------------------------------
