
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TIN_From_Grid.cpp                   //
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
#include "TIN_From_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_From_Grid::CTIN_From_Grid(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Grid to TIN"));

	Set_Author(_TL("Copyrights (c) 2004 by Olaf Conrad"));

	Set_Description(
		_TL("Creates a TIN from grid points. No data values will be ignored.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "VALUES"		, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_TIN(
		NULL	, "TIN"			, _TL("TIN"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CTIN_From_Grid::~CTIN_From_Grid(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_From_Grid::On_Execute(void)
{
	int						x, y, i;
	CSG_TIN					*pTIN;
	CSG_Grid					*pGrid;
	CSG_Parameter_Grid_List	*pValues;
	CSG_Shape					*pPoint;
	CSG_Shapes					Points;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pValues	= Parameters("VALUES")	->asGridList();

	Points.Create(SHAPE_TYPE_Point);
	Points.Get_Table().Add_Field("VALUE", TABLE_FIELDTYPE_Double);

	for(i=0; i<pValues->Get_Count(); i++)
	{
		Points.Get_Table().Add_Field(pValues->asGrid(i)->Get_Name(), TABLE_FIELDTYPE_Double);
	}

	//-----------------------------------------------------
	for(y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(x=0; x<pGrid->Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				pPoint	= Points.Add_Shape();

				pPoint->Add_Point(
					pGrid->Get_XMin() + pGrid->Get_Cellsize() * x,
					pGrid->Get_YMin() + pGrid->Get_Cellsize() * y
				);

				pPoint->Get_Record()->Set_Value(0, pGrid->asDouble(x, y));

				for(i=0; i<pValues->Get_Count(); i++)
				{
					pPoint->Get_Record()->Set_Value(1 + i, pValues->asGrid(i)->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Points.Get_Count() >= 3 )
	{
		pTIN	= Parameters("TIN")->asTIN();
		pTIN->Create(&Points);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
