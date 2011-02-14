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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>
#include "Grid_Aggregate.h"

#define SUM 0
#define MIN 1
#define MAX 2

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Aggregate::CGrid_Aggregate(void)
{

	//-----------------------------------------------------
	Set_Name(_TL("Aggregate"));

	Set_Author(_TL("Copyrights (c) 2005 by Victor Olaya"));

	Set_Description	(_TW("Resamples a raster layer to a lower resolution, aggregating" 
     "the values of a group of cells. This should be used in any case in which and a normal"
     "resampling will result in wrong values in the resulting layer, such as, for instance,"
     "the number of elements of a given class in each cell.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "SIZE"	, _TL("Aggregation Size"),
		_TL(""),
		PARAMETER_TYPE_Int		, 2, 2, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),	_TL("Sum|Min|Max|"), 0
	);

}

//---------------------------------------------------------
CGrid_Aggregate::~CGrid_Aggregate(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


bool CGrid_Aggregate::On_Execute(void)
{
	int x,y;
	int x2,y2;
	int i,j;
	int iNX, iNY;
	int iSize = Parameters("SIZE")->asInt();
	int iMethod = Parameters("METHOD")->asInt();
	double dMin,dMax;
	double dSum;
	double dValue;

	iNX = (int) (Get_NX() / iSize);
	iNY = (int) (Get_NY() / iSize);

	CSG_Grid *pGrid = Parameters("INPUT")->asGrid();

	CSG_Grid *pOutput = SG_Create_Grid(pGrid->Get_Type(), iNX, iNY, pGrid->Get_Cellsize() * iSize, 
					pGrid->Get_XMin(), pGrid->Get_YMin());

	pOutput->Set_Name(pGrid->Get_Name());

	for (y = 0, y2 = 0; y2 < iNY; y+=iSize, y2++){
		for (x = 0, x2 = 0; x2 < iNX; x+=iSize, x2++){
			dMax = dMin = pGrid->asDouble(x,y);
			dSum = 0;
			for (i = 0; i < iSize; i++){
				for (j = 0; j < iSize; j++){
					dValue = pGrid->asDouble(x+i,y+j);
					if (dValue > dMax){
						dMax = dValue;
					}//if
					if (dValue < dMin){
						dMin = dValue;
					}//if
					dSum += dValue;
				}//for
			}//for
			switch (iMethod){
			case SUM:
				pOutput->Set_Value(x2,y2,dSum);
				break;
			case MIN:
				pOutput->Set_Value(x2,y2,dMin);
				break;
			case MAX:
				pOutput->Set_Value(x2,y2,dMax);
				break;
			default:
				break;
			}
		}//for
	}//for

	DataObject_Add(pOutput);

	return true;

}
