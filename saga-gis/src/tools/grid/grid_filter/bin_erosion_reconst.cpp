/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 bin_erosion_reconst.c                 //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     HfT Stuttgart                     //
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
//    e-mail:     johannes.engels@hft-stuttgart.de       //
//                                                       //
//    contact:    Johannes Engels                        //
//                Hochschule fuer Technik Stuttgart      //
//                Schellingstr. 24                       //
//                70174 Stuttgart                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "bin_erosion_reconst.h"

extern "C" {
	#include "geodesic_morph_rec/storeorg.h"
	#include "geodesic_morph_rec/bin_geovinc.h"
}

#define RUN_TOOL(LIBRARY, TOOL, CONDITION)	{\
	bool	bResult;\
	SG_RUN_TOOL(bResult, LIBRARY, TOOL, CONDITION)\
	if( !bResult ) return( false );\
}

#define SET_PARAMETER(IDENTIFIER, VALUE) \
	pTool->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Cbin_erosion_reconst::Cbin_erosion_reconst(void)
{
	Set_Name		(_TL("Binary Erosion-Reconstruction"));

	Set_Author		(SG_T("HfT Stuttgart (c) 2013"));

	Set_Description	(_TW(
		"Common binary Opening does not guarantee, that foreground regions which "
		"outlast the erosion step are reconstructed to their original shape in the "
		"dilation step. Depending on the application, that might be considered as a "
		"deficiency. Therefore this tool provides a combination of erosion with "
		"the binary Geodesic Morphological Reconstruction, see \n"
		"L. Vincent (1993): Morphological Grayscale Reconstruction in Image Analysis: "
		"Applications and Efficient Algorithms. "
		"IEEE Transactions on Image Processing, Vol. 2, No 2\n"
		"Here we use the algorithm on p. 194: Breadth-first Scanning.\n\n"
		"The marker is defined as the eroded INPUT_GRID, whereas the mask is just "
		"the INPUT_GRID itself. OUTPUT_GRID is the reconstruction of the marker under "
		"the mask.\n"
	));

	Parameters.Add_Grid (NULL, 
		                 "INPUT_GRID", 
						 _TL ("Input Grid"), 
						 _TL ("Grid to be filtered"), 
						 PARAMETER_INPUT);

    // Data type of the output values is signed Byte. We wish to retain NoData values 
	// unaltered. With the types Bit or unsigned Byte, however, we are not able 
	// to distinguish 0 from NoData.

	Parameters.Add_Grid (NULL, 
		                 "OUTPUT_GRID", 
						 _TL("Output Grid"), 
						 _TL("Reconstruction result"), 
						 PARAMETER_OUTPUT, 
						 true, 
						 SG_DATATYPE_Char);

	Parameters.Add_Value (NULL,
		                  "RADIUS",
						  _TL ("Filter Size (Radius)"),
						  _TL ("Filter size (radius in grid cells)"),
						  PARAMETER_TYPE_Int,
						  3);

}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

bool Cbin_erosion_reconst::On_Execute(void)
{
	CSG_Grid *pinpgrid, *bingrid, *poutgrid;

	unsigned short numrows;
	unsigned short numcols;
	char **mask;
	char **marker;

	pinpgrid = Parameters ("INPUT_GRID")->asGrid ();
	poutgrid = Parameters ("OUTPUT_GRID")->asGrid ();

	numrows=pinpgrid->Get_NY();
	numcols=pinpgrid->Get_NX();

	// bingrid is not an output grid here, so it must be created ad hoc

	bingrid = SG_Create_Grid(SG_DATATYPE_Char,
	                         pinpgrid->Get_NX(),
							 pinpgrid->Get_NY(),
							 pinpgrid->Get_Cellsize(),
							 pinpgrid->Get_XMin(),
							 pinpgrid->Get_YMin());

	if (bingrid == NULL)
	{
	    SG_UI_Msg_Add_Error(_TL("Unable to create grid for the eroded image!"));

		return (false);
	}

	RUN_TOOL("grid_filter"			, 8,
	       SET_PARAMETER("INPUT"		, pinpgrid)
	    && SET_PARAMETER("RESULT"		, bingrid)
	    && SET_PARAMETER("MODE"		, 1)
		&& SET_PARAMETER("RADIUS"		, Parameters ("RADIUS")->asInt())
		&& SET_PARAMETER("METHOD"		, 1)
    )

	mask = (char **) matrix_all_alloc (numrows, numcols, 'C', 0);
	marker = (char **) matrix_all_alloc (numrows, numcols, 'C', 0);

	for (int y = 0; y < numrows; y++)
	{
       #pragma omp parallel for
       for (int x = 0; x < numcols; x++)
       {
		  if (pinpgrid->is_NoData(x,y)) // check if there are no_data in input datasets
		  {
		 	 mask [y][x] = 0;
		 	 marker [y][x] = 0;
		  }
		  else
		  {
			 mask [y][x] = pinpgrid->asChar(x,y);
			 marker [y][x] = bingrid->asChar(x,y);
		  }
       }
	}

	bingrid->Destroy();

    binary_geodesic_morphological_reconstruction (numrows, numcols, mask, marker);

    for (int y = 0; y < Get_NY () && Set_Progress(y, Get_NY()); y++)
    {
	    #pragma omp parallel for
	    for (int x = 0; x < Get_NX (); x++)
	    {
		    if (pinpgrid->is_NoData(x,y))
		        poutgrid->Set_NoData(x,y);
		    else
		        poutgrid->Set_Value(x,y, marker[y][x]);
	    }
    }


	matrix_all_free ((void **) mask);
    matrix_all_free ((void **) marker);

    return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
