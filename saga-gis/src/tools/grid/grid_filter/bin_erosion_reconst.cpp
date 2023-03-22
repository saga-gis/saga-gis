
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
#include "bin_erosion_reconst.h"

//---------------------------------------------------------
extern "C"
{
	#include "geodesic_morph_rec/storeorg.h"
	#include "geodesic_morph_rec/bin_geovinc.h"
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Cbin_erosion_reconst::Cbin_erosion_reconst(void)
{
	Set_Name		(_TL("Binary Erosion-Reconstruction"));

	Set_Author		("HfT Stuttgart (c) 2013");

	Set_Description	(_TW(
		"Common binary opening does not guarantee, that foreground regions which "
		"outlast the erosion step are reconstructed to their original shape in the "
		"dilation step. Depending on the application, that might be considered as a "
		"deficiency. Therefore this tool provides a combination of erosion with "
		"the binary Geodesic Morphological Reconstruction, see Vincent (1993). "
		"Here we use the algorithm on p. 194: Breadth-first Scanning.\n\n"
		"The marker is defined as the eroded input grid, whereas the mask is just "
		"the input grid itself. The output grid is the reconstruction of the marker under "
		"the mask. "
	));

	Add_Reference(
		"Arefi, H., Hahn, M.", "2005",
		"A Morphological Reconstruction Algorithm for Separating Off-Terrain Points from Terrain Points in Laser Scanning Data",
		"Proceedings of the ISPRS Workshop Laser Scanning 2005, Enschede, the Netherlands, September 12-14, 2005",
		SG_T("https://www.isprs.org/proceedings/xxxvi/3-W19/papers/120.pdf"), SG_T("PDF")
	);

	Add_Reference("Vincent, L.", "1993",
		"Morphological Grayscale Reconstruction in Image Analysis: Applications and Efficient Algorithms",
		"IEEE Transactions on Image Processing, Vol. 2, No 2.",
		SG_T("https://doi.org/10.1109/83.217222"), SG_T("doi: 10.1109/83.217222")
	);

	Parameters.Add_Grid("", "INPUT_GRID", _TL("Input Grid"), _TL("Grid to be filtered"), 
		PARAMETER_INPUT
	);

    // Data type of the output values is signed Byte. We wish to retain NoData values 
	// unaltered. With the types Bit or unsigned Byte, however, we are not able 
	// to distinguish 0 from NoData.
	Parameters.Add_Grid("", "OUTPUT_GRID", _TL("Output Grid"), 
		_TL("Reconstruction result"), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Int("", "RADIUS", _TL("Filter Size (Radius)"),
		_TL ("Filter size (radius in grid cells)"),
		3
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cbin_erosion_reconst::On_Execute(void)
{
	CSG_Grid *pInput = Parameters("INPUT_GRID")->asGrid();

	CSG_Grid Eroded(Get_System(), pInput->Get_Type()); // bingrid is not an output grid here, so it must be created ad hoc

	if( !Eroded.is_Valid() )
	{
	    SG_UI_Msg_Add_Error(_TL("Unable to create grid for the eroded image!"));

		return( false );
	}

	SG_RUN_TOOL_ExitOnError("grid_filter", 8, // Morphological Filter
		   SG_TOOL_PARAMETER_SET("INPUT"        , pInput)
		&& SG_TOOL_PARAMETER_SET("RESULT"       , &Eroded)
		&& SG_TOOL_PARAMETER_SET("METHOD"       , 1) // Erosion
		&& SG_TOOL_PARAMETER_SET("KERNEL_TYPE"  , 1) // Circle
		&& SG_TOOL_PARAMETER_SET("KERNEL_RADIUS", Parameters("RADIUS")->asInt())
    );

	//----------------------------------------------------
	double Offset = pInput->Get_Min(), Scale = pInput->Get_Range(); Scale = Scale ? 127. / Scale : 1.;

	unsigned short numrows = Get_NY();
	unsigned short numcols = Get_NX();

	char **mask   = (char **) matrix_all_alloc (numrows, numcols, 'C', 0);
	char **marker = (char **) matrix_all_alloc (numrows, numcols, 'C', 0);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( pInput->is_NoData(x, y) ) // check if there are no_data in input datasets
		{
			mask  [y][x] = 0;
			marker[y][x] = 0;
		}
		else
		{
			mask  [y][x] = (char)((pInput->asDouble(x, y) - Offset) * Scale);
			marker[y][x] = (char)((Eroded .asDouble(x, y) - Offset) * Scale);
		}
	}

	//----------------------------------------------------
	binary_geodesic_morphological_reconstruction(numrows, numcols, mask, marker);

	//----------------------------------------------------
	CSG_Grid *pOutput = Parameters("OUTPUT_GRID")->asGrid();

	if( pOutput->Get_Type() != pInput->Get_Type() )
	{
		pOutput->Create(Get_System(), pInput->Get_Type());
	}

	pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), Get_Name().c_str());
	pOutput->Set_NoData_Value(pInput->Get_NoData_Value());

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( pInput->is_NoData(x, y) )
		{
			pOutput->Set_NoData(x, y);
		}
		else
		{
			pOutput->Set_Value(x, y, Offset + marker[y][x] / Scale);
		}
	}

	//----------------------------------------------------
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
