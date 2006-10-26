
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
//                     FuzzyOR.cpp                       //
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
#include "FuzzyOR.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFuzzyOR::CFuzzyOR(void)
{
	Set_Name	(_TL("Fuzzy union grid"));

	Set_Author	("Copyrights (c) 2004 by Antonio Boggia and Gianluca Massei");

	Set_Description(_TL(
		"Calculates the union (max operator) for each grid cell of the selected grids.\n "
		"e-mail Gianluca Massei: g_massa@libero.it \n"
		"e-mail Antonio Boggia: boggia@unipg.it \n")
	);

	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "OR"		, _TL("Union"),
		"",
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CFuzzyOR::~CFuzzyOR(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFuzzyOR::On_Execute(void)
{
	int				iGrid, nGrids, x, y;//id Grid, cellsnumber , ,
	double			zMax, ValTemp; // variabile per costrire il grid, variabile temp di confronto
	CSG_Grid			*pOR;
	CSG_Parameter_Grid_List	*pParm_Grids;

	//-----------------------------------------------------
	// Get user inputs from the 'Parameters' object...

	pParm_Grids	= (CSG_Parameter_Grid_List *)Parameters("GRIDS")->Get_Data();
	pOR			= Parameters("OR")->asGrid();
	nGrids		= pParm_Grids->Get_Count();

	//-----------------------------------------------------
	if( nGrids > 1 )
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				//zMax = ValTemp	=  0;
				zMax	= pParm_Grids->asGrid(0)->asDouble(0, 0);// set initial value of zMax

				for(iGrid=0; iGrid<nGrids; iGrid++)
				{
					ValTemp	= pParm_Grids->asGrid(iGrid)->asDouble(x, y);
					
					if  (zMax < ValTemp)
					{
						zMax = ValTemp; //union loop
					}
					else zMax=zMax ;
				}

				pOR->Set_Value(x, y, zMax);
			}
		}
	}
	else if( nGrids > 0 )
	{
		pOR->Assign(pParm_Grids->asGrid(0));
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
