///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Strahler.cpp                      //
//                                                       //
//               Copyright (C) 2004-17 by                //
//             Victor Olaya, Volker Wichmann             //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Strahler.h"

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------------
CStrahler::CStrahler(void){

	Set_Name		(_TL("Strahler Order"));
	Set_Author		(_TL("Victor Olaya, Volker Wichmann (c) 2004-17"));
	Set_Description	(_TW("This tool allows one to calculate the Strahler stream order on basis of a DEM "
						 "and the steepest descent (D8) algorithm.\n"
	));

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"), 
		_TL("The input elevation data set."), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "STRAHLER"	, _TL("Strahler Order"), 
		_TL("The output data set with encoded Strahler stream order."), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);
}

//---------------------------------------------------------
CStrahler::~CStrahler(void)
{}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CStrahler::On_Execute(void)
{
	CSG_Grid	*pDEM, *pOrder;

	pDEM	= Parameters("DEM")->asGrid();
	pOrder	= Parameters("STRAHLER")->asGrid();

	pOrder->Set_NoData_Value(0.0);
	pOrder->Assign(0.0);

	CSG_Grid Count(pOrder);
	Count.Assign(0.0);

	DataObject_Set_Colors(pOrder, 10, SG_COLORS_WHITE_BLUE);


	//---------------------------------------------------------
	int x, y, i, ix, iy;

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( pDEM->Get_Sorted(n, x, y) )
		{
			if( Count.asDouble(x, y) > 1 )
			{
				pOrder->Set_Value(x, y, pOrder->asDouble(x, y) + 1);
			}

			i = pDEM->Get_Gradient_NeighborDir(x, y);

			if( i >= 0 )
			{
				ix	= Get_xTo(i, x);
				iy	= Get_yTo(i, y);

				if( pOrder->asDouble(x, y) > pOrder->asDouble(ix, iy) )
				{
					pOrder->Set_Value(ix, iy, pOrder->asDouble(x, y));
					Count.Set_Value(ix, iy, 1.0);
				}
				else if( pOrder->asDouble(x, y) == pOrder->asDouble(ix, iy) )
				{
					Count.Set_Value(ix, iy, Count.asDouble(x, y) + 1);
				}
			}
		}
	}

	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
