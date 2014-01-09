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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Difference.cpp                  //
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
#include "grid_difference.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Difference::CGrid_Difference(void)
{
	Set_Name		(_TL("Grid Difference"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	, "A"	, _TL("A"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "B"	, _TL("B"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "C"	, _TL("Difference (A - B)"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Difference::On_Execute(void)
{
	CSG_Grid	*pA, *pB, *pC;

	//-----------------------------------------------------
	pA	= Parameters("A")->asGrid();
	pB	= Parameters("B")->asGrid();
	pC	= Parameters("C")->asGrid();

	DataObject_Set_Colors(pC, 100, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || pB->is_NoData(x, y) )
			{
				pC->Set_NoData(x, y);
			}
			else
			{
				pC->Set_Value(x, y, pA->asDouble(x, y) - pB->asDouble(x, y));
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
CGrid_Division::CGrid_Division(void)
{
	Set_Name		(_TL("Grid Division"));

	Set_Author		(SG_T("O. Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	, "A"	, _TL("Dividend"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "B"	, _TL("Divisor"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "C"	, _TL("Quotient"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Division::On_Execute(void)
{
	CSG_Grid	*pA, *pB, *pC;

	//-----------------------------------------------------
	pA	= Parameters("A")->asGrid();
	pB	= Parameters("B")->asGrid();
	pC	= Parameters("C")->asGrid();

	DataObject_Set_Colors(pC, 100, SG_COLORS_RED_GREY_BLUE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || pB->is_NoData(x, y) || pB->asDouble(x, y) == 0.0 )
			{
				pC->Set_NoData(x, y);
			}
			else
			{
				pC->Set_Value(x, y, pA->asDouble(x, y) / pB->asDouble(x, y));
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
CGrids_Sum::CGrids_Sum(void)
{
	Set_Name		(_TL("Grids Sum"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"Cellwise addition of grid values."
	));

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"	, _TL("Sum"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Sum::On_Execute(void)
{
	CSG_Grid				*pResult;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids	= Parameters("GRIDS")	->asGridList();
	pResult	= Parameters("RESULT")	->asGrid();

	//-----------------------------------------------------
	if( pGrids->Get_Count() < 1 )
	{
		Error_Set(_TL("no grid in list"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int		n	= 0;
			double	d	= 0.0;

			for(int i=0; i<pGrids->Get_Count(); i++)
			{
				if( pGrids->asGrid(i)->is_InGrid(x, y) )
				{
					n	++;
					d	+= pGrids->asGrid(i)->asDouble(x, y);
				}
			}

			if( n == pGrids->Get_Count() )
			{
				pResult->Set_Value(x, y, d);
			}
			else
			{
				pResult->Set_NoData(x, y);
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
CGrids_Product::CGrids_Product(void)
{
	Set_Name		(_TL("Grids Product"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"Cellwise multiplication of grid values."
	));

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"	, _TL("Product"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Product::On_Execute(void)
{
	CSG_Grid				*pResult;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids	= Parameters("GRIDS")	->asGridList();
	pResult	= Parameters("RESULT")	->asGrid();

	//-----------------------------------------------------
	if( pGrids->Get_Count() < 1 )
	{
		Error_Set(_TL("no grid in list"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int		n	= 0;
			double	d	= 0.0;

			for(int i=0; i<pGrids->Get_Count(); i++)
			{
				if( pGrids->asGrid(i)->is_InGrid(x, y) )
				{
					if( n++ < 1 )
					{
						d 	 = pGrids->asGrid(i)->asDouble(x, y);
					}
					else
					{
						d	*= pGrids->asGrid(i)->asDouble(x, y);
					}
				}
			}

			if( n == pGrids->Get_Count() )
			{
				pResult->Set_Value(x, y, d);
			}
			else
			{
				pResult->Set_NoData(x, y);
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
