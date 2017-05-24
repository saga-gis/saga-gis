
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   grid_pca_focal.cpp                  //
//                                                       //
//                 Copyright (C) 2016 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_pca_focal.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_PCA_Focal::CGrid_PCA_Focal(void)
{
	Set_Name		(_TL("Focal PCA on a Grid"));

	Set_Author		("A.Thomas, O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool uses the difference in cell values of a center cell "
		"and its neighbours (as specified by the kernel) as features for "
		"a Principle Components Analysis (PCA). "
		"This method has been used by Thomas and Herzfeld (2004) to parameterize "
		"the topography for a subsequent regionalization of climate variables "
		"with the principle components as predictors in a regression model. "
	));

	Add_Reference("Benichou, P., Lebreton, O.", "1987",
		"Prise en compte de la topographie pour la cartographie des champs pluviometriques statistiques",
		"Meteorologie 7. Serie, no. 19."
	);

	Add_Reference("Thomas, A., Herzfeld, U.C.", "2004",
		"REGEOTOP: New Climatic Data Fields for East Asia Based on Localized Relief Information and Geostatistical Methods",
		"International Journal of Climatology, 24(10), 1283-1306. DOI:10.1002/joc.1058.",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1002/joc.1058/full"), SG_T("Wiley Online Library")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(NULL,
		"PCA"			, _TL("Principal Components"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(NULL,
		"EIGEN"			, _TL("Eigen Vectors"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Int(NULL,
		"COMPONENTS"	, _TL("Number of Components"),
		_TL("number of first components in the output; set to zero to get all"),
		7, 1, true
	);

	Parameters.Add_Bool(NULL,
		"OVERWRITE"		, _TL("Overwrite Previous Results"),
		_TL(""),
		true
	);

	Parameters.Add_Choice(NULL,
		"KERNEL_TYPE"	, _TL("Kernel Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Int(NULL,
		"KERNEL_RADIUS"	, _TL("Kernel Radius"),
		_TL("Kernel radius in cells."),
		2, 1, true
	);

	Parameters.Add_Choice(NULL,
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_PCA_Focal::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA_Focal::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	CSG_Grid_Cell_Addressor	Kernel;

	Kernel.Set_Radius(
		Parameters("KERNEL_RADIUS")->asInt(),
		Parameters("KERNEL_TYPE"  )->asInt() == 0
	);

	//-----------------------------------------------------
	CSG_Parameters	PCA_Parms;

	int	i, nGrids	= Kernel.Get_Count() - 1;

	CSG_Grid	*Grids	= new CSG_Grid[nGrids];

	CSG_Parameter_Grid_List	*pPCA, *pGrids	= PCA_Parms.Add_Grid_List(NULL, "GRIDS", "", "", PARAMETER_INPUT)->asGridList();

	for(i=0; i<nGrids; i++)
	{
		if( !Grids[i].Create(*Get_System()) )
		{
			delete[](Grids);

			Error_Set(_TL("failed to allocate memory"));

			return( false );
		}

		Grids[i].Set_Name(CSG_String::Format("x(%d)-y(%d)", Kernel.Get_X(i + 1), Kernel.Get_Y(i + 1)));

		pGrids->Add_Item(&Grids[i]);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(i)
		for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
			{
				for(i=0; i<nGrids; i++)
				{
					Grids[i].Set_NoData(x, y);
				}
			}
			else
			{
				double	z	= pGrid->asDouble(x, y);

				for(i=0; i<nGrids; i++)
				{
					int	ix	= Kernel.Get_X(i + 1, x);
					int	iy	= Kernel.Get_Y(i + 1, y);

					Grids[i].Set_Value(x, y, pGrid->is_InGrid(ix, iy) ? z - pGrid->asDouble(ix, iy) : 0.0);
				}
			}
		}
	}

	//-----------------------------------------------------
	bool	bResult;

	SG_RUN_TOOL_KEEP_PARMS(bResult, "statistics_grid", 8, PCA_Parms,	// pca analysis for grids
			SG_TOOL_PARAMETER_SET("GRIDS"     , PCA_Parms ("GRIDS"     ))
		&&	SG_TOOL_PARAMETER_SET("METHOD"    , Parameters("METHOD"    ))
		&&	SG_TOOL_PARAMETER_SET("EIGEN"     , Parameters("EIGEN"     ))
		&&	SG_TOOL_PARAMETER_SET("COMPONENTS", Parameters("COMPONENTS")->asInt())
	);

	delete[](Grids);

	//-----------------------------------------------------
	pGrids	= Parameters("PCA")->asGridList();
	pPCA	= PCA_Parms ("PCA")->asGridList();

	if( !Parameters("OVERWRITE")->asBool() )
	{
		pGrids->Del_Items();
	}

	for(i=0; i<pPCA->Get_Grid_Count(); i++)
	{
		if( pGrids->Get_Grid(i) )
		{
			pGrids->Get_Grid(i)->Assign(pPCA->Get_Grid(i));

			delete(pPCA->Get_Grid(i));
		}
		else
		{
			pGrids->Add_Item(pPCA->Get_Grid(i));
		}

		pGrids->Get_Grid(i)->Set_Name(CSG_String::Format("%s [PC%0*d]", pGrid->Get_Name(), pPCA->Get_Grid_Count() < 10 ? 1 : 2, i + 1));
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
