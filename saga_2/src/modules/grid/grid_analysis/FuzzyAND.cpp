
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 contrib_boggia_massei                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     FuzzyAND.cpp                      //
//                                                       //
//                 Copyright (C) 2004 by                 //
//           Antonio Boggia and Gianluca Massei          //
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
//    e-mail:     boggia@unipg.it						 //
//				  g_massa@libero.it				     	 //
//                                                       //
//    contact:    Antonio Boggia                         //
//                Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "FuzzyAND.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFuzzyAND::CFuzzyAND(void)
{
	Set_Name	(_TL("Fuzzy intersection grid"));

	Set_Author	("Copyrights (c) 2004 by Antonio Boggia and Gianluca Massei");

	Set_Description(_TL(
		"Calculates the intersection (min operator) for each grid cell of the selected grids.\n "
		"e-mail Gianluca Massei: g_massa@libero.it \n"
		"e-mail Antonio Boggia: boggia@unipg.it \n")
	);

	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "AND"		, _TL("Intersection"),
		"",
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CFuzzyAND::~CFuzzyAND(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFuzzyAND::On_Execute(void)
{
	int				iGrid, nGrids, x, y;//id Grid, cellsnumber , ,
	double			zMin, ValTemp; // variabile per costrire il grid, variabile temp di confronto
	CGrid			*pAND;
	CParameter_Grid_List	*pParm_Grids;

	//-----------------------------------------------------
	// Get user inputs from the 'Parameters' object...

	pParm_Grids	= Parameters("GRIDS")	->asGridList();
	pAND		= Parameters("AND")		->asGrid();
	nGrids		= pParm_Grids			->Get_Count();

	//-----------------------------------------------------
	if( nGrids > 1 )
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				//zMin = ValTemp	=  0;
				zMin	= pParm_Grids->asGrid(0)->asDouble(0, 0);// set initial value of zMin

				for(iGrid=0; iGrid<nGrids; iGrid++)
				{
					ValTemp	= pParm_Grids->asGrid(iGrid)->asDouble(x, y);
					
					if  (zMin > ValTemp)
					{
						zMin = ValTemp; //intersection loop
					}
					else zMin=zMin ;
				}

				pAND->Set_Value(x, y, zMin); 
			}
		}
	}
	else if( nGrids > 0 )
	{
		pAND->Assign(pParm_Grids->asGrid(0));
	}

	//-----------------------------------------------------
	return( nGrids > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
