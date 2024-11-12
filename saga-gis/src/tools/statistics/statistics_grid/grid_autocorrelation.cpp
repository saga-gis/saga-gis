
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                grid_autocorrelastion.cpp              //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                     Jan Papmeier                      //
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
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_autocorrelation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Autocorrelation::CGrid_Autocorrelation(void)
{
	Set_Name		(_TL("Global Moran's I for Grids"));

	Set_Author		("Jan Papmeier (c) 2010");

	Set_Description	(_TW(
		"Global spatial autocorrelation for grids calculated as Moran's I."
	));

	Add_Reference(
		"Lloyd, C.D.", "2010", "Spatial data analysis - An introduction for GIS users", "Oxford. 206p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		"", "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "CONTIGUITY"	, _TL("Case of contiguity"),
		_TL("Choose case: Rook's case contiguity compares only cell which share an edge. Queen's case contiguity compares also cells which share just corners."),
		CSG_String::Format("%s|%s",
			_TL("Rook"),
			_TL("Queen")
		), 1
	);

	Parameters.Add_Bool(
		"", "DIALOG"		, _TL("Show Result in Dialog"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Autocorrelation::On_Execute(void)
{
	int			nContiguity, nNeighbours;
	double		Sum;
	CSG_Grid	*pGrid;
	CSG_Table	*pResult;
	
	//-----------------------------------------------------
	pGrid	= Parameters("GRID"  )->asGrid();
	pResult	= Parameters("RESULT")->asTable();

	switch( Parameters("CONTIGUITY")->asInt() )
	{
	case  0:	nContiguity	= 2;	break;	// Rook's case
	default:	nContiguity	= 1;	break;	// Queen's case
	}

	//-----------------------------------------------------
	nNeighbours	= 0;
	Sum			= 0.0;

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				double	dz	= pGrid->asDouble(x, y) - pGrid->Get_Mean();

				for(int i=0; i<8; i+=nContiguity)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( pGrid->is_InGrid(ix, iy) )
					{
						nNeighbours	++;
						Sum			+= dz * (pGrid->asDouble(ix, iy) - pGrid->Get_Mean());
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( nNeighbours <= 1 )
	{
		Message_Add(_TL("not enough neighbours"));

		return( false );
	}

	//-----------------------------------------------------
	double	Moran_I	= (pGrid->Get_NCells() * Sum) / (pGrid->Get_Variance() * pGrid->Get_NCells() * nNeighbours);

	if( pResult->Get_Field_Count() != 8 || SG_STR_CMP(_TL("Moran's I"), pResult->Get_Name()) )
	{
		pResult->Destroy();
		pResult->Set_Name(_TL("Moran's I"));

		pResult->Add_Field(_TL("GRID"      ), SG_DATATYPE_String);
		pResult->Add_Field(_TL("CONTIGUITY"), SG_DATATYPE_String);
		pResult->Add_Field(_TL("MORAN_I"   ), SG_DATATYPE_Double);
		pResult->Add_Field(_TL("NEIGHBORS" ), SG_DATATYPE_Int   );
		pResult->Add_Field(_TL("NCELLS"    ), SG_DATATYPE_Int   );
		pResult->Add_Field(_TL("MEAN"      ), SG_DATATYPE_Double);
		pResult->Add_Field(_TL("VARIATION" ), SG_DATATYPE_Double);
		pResult->Add_Field(_TL("SUM"       ), SG_DATATYPE_Double);
	}

	CSG_Table_Record	*pRecord	= pResult->Add_Record();

	pRecord->Set_Value(0, pGrid->Get_Name());
	pRecord->Set_Value(1, nContiguity == 2 ? _TL("Rook's case") : _TL("Queen's case"));
	pRecord->Set_Value(2, Moran_I);
	pRecord->Set_Value(3, nNeighbours);
	pRecord->Set_Value(4, (int)pGrid->Get_NCells());
	pRecord->Set_Value(5, pGrid->Get_Mean());
	pRecord->Set_Value(6, pGrid->Get_Variance() * pGrid->Get_NCells());
	pRecord->Set_Value(7, Sum);

	Message_Fmt("\n%s (%s): %f", _TL("Moran's I"), pGrid->Get_Name(), Moran_I);

	if( Parameters("DIALOG")->asBool() )
	{
		Message_Dlg(CSG_String::Format("%s: %f", _TL("Moran's I"), Moran_I), pGrid->Get_Name());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
