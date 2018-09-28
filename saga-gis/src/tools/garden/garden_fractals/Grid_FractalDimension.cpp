/**********************************************************
 * Version $Id: Grid_FractalDimension.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_FractalDimension.cpp               //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_FractalDimension.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_FractalDimension::CGrid_FractalDimension(void)
{
	Set_Name		(_TL("Fractal Dimension of Grid Surface"));

	Set_Author		(SG_T("O.Conrad (c) 2001"));

	Set_Description	(_TW(
		"Calculates surface areas for increasing mesh sizes.")
	);

	Parameters.Add_Grid (NULL, "INPUT" , _TL("Input") , _TL(""), PARAMETER_INPUT);
	Parameters.Add_Table(NULL, "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Value(NULL, "DSIZE" , _TL("Scale" ), _TL(""), PARAMETER_TYPE_Double, 1.5, 1.0001, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_FractalDimension::On_Execute(void)
{
	double		maxSize, dSize;
	CSG_Grid	*pGrid;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pGrid	= Parameters("INPUT" )->asGrid();
	pTable	= Parameters("RESULT")->asTable();

	pTable->Destroy();
	pTable->Set_Name(_TL("Fractal Dimension"));

	pTable->Add_Field(SG_T("CLASS"  ), SG_DATATYPE_Int);
	pTable->Add_Field(SG_T("SCALE"  ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("BASAL"  ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("SURFACE"), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("RATIO"  ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("CHANGE" ), SG_DATATYPE_Double);

	//-----------------------------------------------------
	Get_Area(pGrid, pTable);

	maxSize	= 0.5 * (pGrid->Get_XRange() > pGrid->Get_YRange() ? pGrid->Get_XRange() : pGrid->Get_YRange());
	dSize	= Parameters("DSIZE")->asDouble();

	for(double Cellsize=dSize*pGrid->Get_Cellsize(); Cellsize<maxSize && Set_Progress(Cellsize, maxSize); Cellsize*=dSize)
	{
		Set_Show_Progress(false);

		CSG_Grid	g(CSG_Grid_System(Cellsize,
			pGrid->Get_XMin(true), pGrid->Get_YMin(true),
			pGrid->Get_XMax(true), pGrid->Get_YMax(true)
		));

		g.Assign(pGrid, GRID_RESAMPLING_BSpline);

		Get_Area(&g, pTable);

		Set_Show_Progress(true);
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
void CGrid_FractalDimension::Get_Area(CSG_Grid *pGrid, CSG_Table *pTable)
{
	double		scale	= pGrid->Get_Cellsize();
	CSG_Grid	g;

	if( !Get_System().is_Equal(pGrid->Get_System()) )
	{
		g.Create(Get_System());
		g.Assign(pGrid, GRID_RESAMPLING_BSpline);
		pGrid	= &g;
	}

	double	basal	= 0.0;
	double	surface	= 0.0;

	for(int y=0; y<pGrid->Get_NY() && Process_Get_Okay(); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			double	s, a;

			if( pGrid->Get_Gradient(x, y, s, a) )
			{
				basal	+= pGrid->Get_Cellarea();
				surface	+= pGrid->Get_Cellarea() / cos(s);
			}
		}
	}

	//-----------------------------------------------------
	if( basal > 0.0 )
	{
		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(0, pTable->Get_Count());	// CLASS
		pRecord->Set_Value(1, scale);				// SCALE
		pRecord->Set_Value(2, basal);				// BASAL
		pRecord->Set_Value(3, surface);				// SURFACE
		pRecord->Set_Value(4, surface / basal);		// RATIO

		if( (pRecord = pTable->Get_Record(pTable->Get_Count() - 2)) != NULL )
		{
			pRecord->Set_Value(5, pRecord->asDouble(3) - surface);	// CHANGE
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
