
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  grid_difference.cpp                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "grid_difference.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Addition::CGrid_Addition(void)
{
	Set_Name		(_TL("Grid Addition"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid("",
		"A", CSG_String::Format("%s 1", _TL("Summand")),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"B", CSG_String::Format("%s 2", _TL("Summand")),
		_TL("The grid or values being added."),
		0.
	);

	Parameters.Add_Grid("",
		"C", _TL("Sum"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Addition::On_Execute(void)
{
	CSG_Grid *pA = Parameters("A")->asGrid  ();
	CSG_Grid *pB = Parameters("B")->asGrid  ();
	double     B = Parameters("B")->asDouble();
	CSG_Grid *pC = Parameters("C")->asGrid  ();

	DataObject_Set_Colors(pC, 11, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || (pB && pB->is_NoData(x, y)) )
			{
				pC->Set_NoData(x, y);
			}
			else
			{
				pC->Set_Value(x, y, pA->asDouble(x, y) + (pB ? pB->asDouble(x, y) : B));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Subtraction::CGrid_Subtraction(void)
{
	Set_Name		(_TL("Grid Difference"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid("",
		"A", _TL("Minuend"),
		_TL("The grid being subtracted from."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"B", _TL("Subtrahend"),
		_TL("The grid or values being subtracted."),
		0.
	);

	Parameters.Add_Grid("",
		"C", _TL("Difference"),
		_TL("The minuend less the subtrahend."),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Subtraction::On_Execute(void)
{
	CSG_Grid *pA = Parameters("A")->asGrid  ();
	CSG_Grid *pB = Parameters("B")->asGrid  ();
	double     B = Parameters("B")->asDouble();
	CSG_Grid *pC = Parameters("C")->asGrid  ();

	DataObject_Set_Colors(pC, 11, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || (pB && pB->is_NoData(x, y)) )
			{
				pC->Set_NoData(x, y);
			}
			else
			{
				pC->Set_Value(x, y, pA->asDouble(x, y) - (pB ? pB->asDouble(x, y) : B));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Multiplication::CGrid_Multiplication(void)
{
	Set_Name		(_TL("Grid Multiplication"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid("",
		"A", _TL("Multiplicand"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"B", _TL("Multiplier"),
		_TL(""),
		1.
	);

	Parameters.Add_Grid("",
		"C", _TL("Product"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Multiplication::On_Execute(void)
{
	CSG_Grid *pA = Parameters("A")->asGrid  ();
	CSG_Grid *pB = Parameters("B")->asGrid  ();
	double     B = Parameters("B")->asDouble();
	CSG_Grid *pC = Parameters("C")->asGrid  ();

	DataObject_Set_Colors(pC, 11, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || (pB && pB->is_NoData(x, y)) )
			{
				pC->Set_NoData(x, y);
			}
			else
			{
				pC->Set_Value(x, y, pA->asDouble(x, y) * (pB ? pB->asDouble(x, y) : B));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Division::CGrid_Division(void)
{
	Set_Name		(_TL("Grid Division"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid("",
		"A", _TL("Dividend"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"B", _TL("Divisor"),
		_TL(""),
		1.
	);

	Parameters.Add_Grid("",
		"C", _TL("Quotient"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Division::On_Execute(void)
{
	CSG_Grid *pA = Parameters("A")->asGrid  ();
	CSG_Grid *pB = Parameters("B")->asGrid  ();
	double     B = Parameters("B")->asDouble();
	CSG_Grid *pC = Parameters("C")->asGrid  ();

	DataObject_Set_Colors(pC, 11, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || (pB && (pB->is_NoData(x, y) || pB->asDouble(x, y) == 0.)) || (!pB && B == 0.) )
			{
				pC->Set_NoData(x, y);
			}
			else
			{
				pC->Set_Value(x, y, pA->asDouble(x, y) / (pB ? pB->asDouble(x, y) : B));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Sum::CGrids_Sum(void)
{
	Set_Name		(_TL("Grids Sum"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Calculates the sum of all input grids by cellwise addition of their grid values."
	));

	Parameters.Add_Grid_List("",
		"GRIDS" , _TL("Summands"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT", _TL("Sum"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"NODATA", _TL("Count No Data as Zero"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Sum::On_Execute(void)
{
	CSG_Parameter_Grid_List *pGrids = Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grid in list"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pSum = Parameters("RESULT")->asGrid();

	bool bNoData = Parameters("NODATA")->asBool();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int n = 0; double s = 0.;

			for(int i=0; i<pGrids->Get_Grid_Count(); i++)
			{
				if( pGrids->Get_Grid(i)->is_InGrid(x, y) )
				{
					n++; s += pGrids->Get_Grid(i)->asDouble(x, y);
				}
			}

			if( bNoData ? n > 0 : n == pGrids->Get_Grid_Count() )
			{
				pSum->Set_Value(x, y, s);
			}
			else
			{
				pSum->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Product::CGrids_Product(void)
{
	Set_Name		(_TL("Grids Product"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Calculates the product of all input grids by cellwise multiplication of their grid values."
	));

	Parameters.Add_Grid_List("",
		"GRIDS" , _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT", _TL("Product"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"NODATA", _TL("Count No Data as Zero"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Product::On_Execute(void)
{
	CSG_Parameter_Grid_List *pGrids = Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grid in list"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pProduct = Parameters("RESULT")->asGrid();

	bool bNoData = Parameters("NODATA")->asBool();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int n = 0; double p	= 1.;

			for(int i=0; i<pGrids->Get_Grid_Count(); i++)
			{
				if( pGrids->Get_Grid(i)->is_InGrid(x, y) )
				{
					n++; p += pGrids->Get_Grid(i)->asDouble(x, y);
				}
			}

			if( bNoData ? n > 0 : n == pGrids->Get_Grid_Count() )
			{
				pProduct->Set_Value(x, y, p);
			}
			else
			{
				pProduct->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
