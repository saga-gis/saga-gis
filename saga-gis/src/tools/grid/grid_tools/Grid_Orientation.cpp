/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
		""	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		""	, "COPY"	, _TL("Copy"),
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

		pGrid->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Inverse"));
	}

	//-----------------------------------------------------
	double	zMin	= pGrid->Get_Min();
	double	zMax	= pGrid->Get_Max();

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

		pGrid->Fmt_Name("%s [%s %s]", pGrid->Get_Name(), _TL("mirrored"), Parameters("METHOD")->asString());
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
				for(int xa=0, xb=Get_NX()-1; xa<=xb; xa++, xb--)
				{
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
CChange_Grid_System::CChange_Grid_System(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Change Grid System"));

	Set_Author		("J. Spitzmueller (c) 2023");

	Set_Description(_TW(
		"This tool changes the grid system by assigning new origin coordinates (lower left corner)."
		"<ul>"
		"<li>Set Origin: Defines the new lower left corner.</li>"
		"<li>Shift Origin: Moves the origin in the given direction.</li>"
		"</ul>"

	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "OUT"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Set Origin"),
			_TL("Shift Origin")
		)
	);

	Parameters.Add_Double(
		NULL 	, "X"	, _TL("X"),
		_TL(""), 
		0.0
	);

	Parameters.Add_Double(
		NULL 	, "Y"	, _TL("Y"),
		_TL(""), 
		0.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Grid_System::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	CSG_Grid_System NewSystem = pGrid->Get_System(); 

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{	
	case 0:

		NewSystem.Assign( pGrid->Get_Cellsize(), Parameters("X")->asDouble(),
			Parameters("Y")->asDouble(), pGrid->Get_NX(), pGrid->Get_NY() );

		break;

	case 1:

		NewSystem.Assign( pGrid->Get_Cellsize(), pGrid->Get_XMin() + Parameters("X")->asDouble(),
			pGrid->Get_YMin() + Parameters("Y")->asDouble(), pGrid->Get_NX(), pGrid->Get_NY() );

		break;
	}

	CSG_Grid *pOut = SG_Create_Grid( NewSystem, pGrid->Get_Type() );

	if( pOut )
	{
		pOut->Set_Name   (pGrid->Get_Name());
		pOut->Set_Unit   (pGrid->Get_Unit());
		pOut->Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());

		for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
		{
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				pOut->Set_Value(x, y, pGrid->asDouble(x, y));
			}
		}

		return( Parameters("OUT")->Set_Value(pOut) );
	}
		
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//---------------------------------------------------------
