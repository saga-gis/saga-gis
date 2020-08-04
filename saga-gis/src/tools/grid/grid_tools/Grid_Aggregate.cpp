
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      grid_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Aggregate.cpp                  //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Victor Olaya                     //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Aggregate.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Aggregate::CGrid_Aggregate(void)
{
	Set_Name		(_TL("Aggregate"));

	Set_Author		("V.Olaya (c) 2005");

	Set_Description	(_TW(
		"Resamples a raster layer to a lower resolution, aggregating" 
	    "the values of a group of cells. This should be used in any case in which a normal"
		"resampling will result in wrong values in the resulting layer, such as, for instance,"
		"the number of elements of a given class in each cell."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_Output("",
		"OUTPUT"	, _TL("Aggregated Grid"),
		_TL("")
	);

	Parameters.Add_Int("",
		"SIZE"		, _TL("Aggregation Size"),
		_TL(""),
		2, 2, true
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			_TL("Sum"),
			_TL("Minimum"),
			_TL("Maximum"),
			_TL("Median"),
			_TL("Mean"),
			_TL("Mode")
		), 4
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Aggregate::On_Execute(void)
{
	int	Size	= Parameters("SIZE")->asInt();

	CSG_Grid_System	System(Get_Cellsize() * Size, Get_XMin(), Get_YMin(), Get_NX() / Size, Get_NY() / Size);

	CSG_Grid	*pOutput, *pGrid	= Parameters("INPUT")->asGrid();

	Parameters("OUTPUT")->Set_Value(pOutput = SG_Create_Grid(System, pGrid->Get_Type()));

	pOutput->Set_Name(pGrid->Get_Name());

	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<System.Get_NY(); y++)
	{
		int	yy	= -Size/2 + y * Size;

		for(int x=0, xx=-Size/2; x<System.Get_NX(); xx+=Size, x++)
		{
			if( Method == 5 )	// Mode
			{
				CSG_Unique_Number_Statistics	s;

				for(int iy=yy; iy<yy+Size; iy++)
				{
					for(int ix=xx; ix<xx+Size; ix++)
					{
						if( pGrid->is_InGrid(ix, iy) )
						{
							s	+= pGrid->asDouble(ix, iy);
						}
					}
				}

				double	Value;

				if( s.Get_Majority(Value) == false )
				{
					pOutput->Set_NoData(x, y);
				}
				else
				{
					pOutput->Set_Value(x, y, Value);
				}
			}

			//---------------------------------------------
			else
			{
				CSG_Simple_Statistics	s(Method == 3);

				for(int iy=yy; iy<yy+Size; iy++)
				{
					for(int ix=xx; ix<xx+Size; ix++)
					{
						if( pGrid->is_InGrid(ix, iy) )
						{
							s	+= pGrid->asDouble(ix, iy);
						}
					}
				}

				if( s.Get_Count() == 0 )
				{
					pOutput->Set_NoData(x, y);
				}
				else switch( Method )
				{
				default: pOutput->Set_Value(x, y, s.Get_Sum    ()); break;
				case  1: pOutput->Set_Value(x, y, s.Get_Minimum()); break;
				case  2: pOutput->Set_Value(x, y, s.Get_Maximum()); break;
				case  3: pOutput->Set_Value(x, y, s.Get_Median ()); break;
				case  4: pOutput->Set_Value(x, y, s.Get_Mean   ()); break;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
