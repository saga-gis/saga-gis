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
//                  connect_analysis.c                   //
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
#include "connect_analysis.h"

extern "C" {
	#include "geodesic_morph_rec/storeorg.h"
	#include "geodesic_morph_rec/combcontour.h"
}

//---------------------------------------------------------
#define RUN_TOOL(LIBRARY, TOOL, CONDITION)	{\
	bool	bResult;\
	SG_RUN_TOOL(bResult, LIBRARY, TOOL, CONDITION)\
	if( !bResult ) return( false );\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pTool->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Cconnectivity_analysis::Cconnectivity_analysis(void)
{
	Set_Name		(_TL("Connectivity Analysis"));

	Set_Author		(SG_T("HfT Stuttgart (c) 2013"));

	Set_Description	(_TW(
		"Connectivity analysis of a binary input image according to \n"
		"Burger, W., Burge, M.: Digitale Bildverarbeitung. "
		"Springer Verlag 2006, p.208.\n"
		"Output consists in a symbolic image of the connected foreground regions "
		"and a shape of the borders of the foreground regions (outer and inner "
		"borders). The shape may contain alternatively the centers or the corners "
		"of the border pixels. Optionally, the regions which have contact with "
		"the image borders can be removed together with their border shapes. \n"
		"In addition, an optional morphological filter (erosion-binary reconstruction) "
		"can be applied to the input image first. \n\n"
	));

	Parameters.Add_Grid (NULL,
		                 "INPUT_GRID",
						 _TL ("Input Binary Grid"),
						 _TL ("Binary input image for the connectivity analysis"),
						 PARAMETER_INPUT);

	Parameters.Add_Grid (NULL,
		                 "FILTERED_MASK",
						 _TL("Filtered Image"),
						 _TL("Morphologically filtered binary mask"),
						 PARAMETER_OUTPUT_OPTIONAL,
						 true,
						 SG_DATATYPE_Char);

	Parameters.Add_Value (NULL,
		                  "FILTER",
						  _TL ("Apply Filter?"),
						  _TL ("Apply a filter (erosion - binary reconstruction) to the input image "),
						  PARAMETER_TYPE_Bool,
						  true);

	Parameters.Add_Value (Parameters("FILTER"),
		                  "SIZE",
						  _TL ("Filter Size (Radius)"),
						  _TL ("Filter size (radius in grid cells)"),
						  PARAMETER_TYPE_Int,
						  3);

//	Parameters.Add_Grid_Output(NULL, "SYMBOLIC_IMAGE", _TL("Symbolic Image"), _TL("The final symbolic image"));

	Parameters.Add_Grid (NULL,
		                 "SYMBOLIC_IMAGE",
						 _TL("Symbolic Image"),
						 _TL("The final symbolic image"),
						 PARAMETER_OUTPUT);

	Parameters.Add_Shapes (NULL,
		                   "OUTLINES",
						   _TL("Outlines"),
						   _TL("Polygon outlines of object regions"),
						   PARAMETER_OUTPUT,
						   SHAPE_TYPE_Polygon);

	Parameters.Add_Value (NULL,
		                  "BORDER_PIXEL_CENTERS",
						  _TL ("Pixel Centers?"),
						  _TL ("Should the output shapes contain the centers of the border pixels instead of the corners?"),
						  PARAMETER_TYPE_Bool,
						  false);

    Parameters.Add_Value (NULL,
		                  "REMOVE_MARGINAL_REGIONS",
						  _TL ("Remove Border Regions?"),
						  _TL ("Remove regions which have contact with (are adjacent to) the image borders?"),
						  PARAMETER_TYPE_Bool,
						  false);

}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

bool Cconnectivity_analysis::On_Execute(void)
{
	CSG_Grid *pinpgrid, *bingrid, *symb_grid, *hgrid;
	CSG_Shapes *pOutlines;

	bool filter, corners_centers, remove_marginal_regions;
	unsigned short numrows;
	unsigned short numcols;
	unsigned char center;
	double xmin;
	double ymin;
	short interm_grid_created;
    simple_REGIONC_list *reg_first;
    simple_REGIONC_list *reg_last;
	simple_REGIONC_list *reg_curr;

	pinpgrid = Parameters ("INPUT_GRID")->asGrid();
	bingrid = Parameters ("FILTERED_MASK")->asGrid();
	filter = Parameters ("FILTER")->asBool();
	corners_centers = Parameters ("BORDER_PIXEL_CENTERS")->asBool();
	remove_marginal_regions = Parameters ("REMOVE_MARGINAL_REGIONS")->asBool();
	pOutlines = Parameters("OUTLINES")->asShapes();
	symb_grid = Parameters ("SYMBOLIC_IMAGE")->asGrid();
	CSG_String	sName = pOutlines->Get_Name();
	pOutlines->Destroy();
	pOutlines->Set_Name(sName);
	pOutlines->Add_Field(SG_T("ID"), SG_DATATYPE_Int);

	numrows=pinpgrid->Get_NY()+2;
	numcols=pinpgrid->Get_NX()+2;
//	xmin=pinpgrid->Get_XMin()-2*pinpgrid->Get_Cellsize();
//	ymin=pinpgrid->Get_YMin()-2*pinpgrid->Get_Cellsize();

	xmin=pinpgrid->Get_XMin();
	ymin=pinpgrid->Get_YMin();

	unsigned char **bin_image;
	long **symb_image;

	if (corners_centers)
		center = 1;
	else
		center = 0;

	bin_image = (unsigned char **) matrix_all_alloc (numrows, numcols, 'U', 0);
	symb_image = (long **) matrix_all_alloc (numrows, numcols, 'L', 0);

	interm_grid_created = 0;

	//SG_Free(bin_image);
	//CSG_Grid *pTmp = new CSG_Grid();
	//delete pTmp;

	if (filter)
	{
		if (bingrid == NULL)
		{
			SG_UI_Msg_Add(_TL("Filtered mask will be created automatically ..."), true);

			bingrid = SG_Create_Grid(SG_DATATYPE_Char,
				                     pinpgrid->Get_NX(),
									 pinpgrid->Get_NY(),
									 pinpgrid->Get_Cellsize(),
									 pinpgrid->Get_XMin(),
									 pinpgrid->Get_YMin());

			if (bingrid == NULL)
			{
				SG_UI_Msg_Add_Error(_TL("Unable to create filtered mask grid!"));
	            matrix_all_free ((void **) bin_image);
	            matrix_all_free ((void **) symb_image);

				return (false);
			}

			Parameters("FILTERED_MASK")->Set_Value(bingrid);
			interm_grid_created = 1;
		}


		//-----------------------------------------------------
		RUN_TOOL("grid_filter"			, 13,
				SET_PARAMETER("INPUT_GRID"  , pinpgrid)
			&&	SET_PARAMETER("OUTPUT_GRID"	, bingrid)
			&&	SET_PARAMETER("RADIUS"		, Parameters ("SIZE")->asInt())
		)

		hgrid = bingrid;
	}
	else
	{
	    hgrid = pinpgrid;
	}

    for (int y = 0; y < hgrid->Get_NY () && Set_Progress(y, hgrid->Get_NY()); y++)
	{
	    for (int x = 0; x < hgrid->Get_NX(); x++)
		{
            if (hgrid->is_NoData(x,y))
		        bin_image[y+1][x+1] = 0;
			else
		        bin_image[y+1][x+1] = hgrid->asChar(x,y);
		}
	}


	// Here the regions are removed which have contact with the image margins;
	// this is achieved by a region growing

	if (remove_marginal_regions)
	{
		for (int y = 1; y < numrows - 1; y++)
		{
			if (bin_image [y][1] != 0)
				background_region_growing (bin_image, numrows, numcols, y, 1);

			if (bin_image [y][numcols - 2] != 0)
				background_region_growing (bin_image, numrows, numcols, y, numcols-2);
		}

		for (int x = 1; x < numcols - 1; x++)
		{
			if (bin_image [1][x] != 0)
				background_region_growing (bin_image, numrows, numcols, 1, x);

			if (bin_image [numrows-2][x] != 0)
				background_region_growing (bin_image, numrows, numcols, numrows-2, x);
		}

		if (filter)
		{
		    for (int y = 0; y < bingrid->Get_NY (); y++)
 	        {
     		    #pragma omp parallel for
		        for (int x = 0; x < bingrid->Get_NX(); x++)
		        {
			        bingrid->Set_Value(x, y, bin_image[y+1][x+1]);
		        }
	        }
		}
	}

	if (interm_grid_created)
		bingrid->Destroy();


	// The function which does the proper work: 
	// computation of the symbolic image, construction of the border chains (shapes)
	
	comb_contour_region_marking (numrows,
                                 numcols,
                                 bin_image,
                                 symb_image,
                                 &reg_first,
                                 &reg_last,
                                 center);


	for (int y = 0; y < symb_grid->Get_NY () && Set_Progress(y, symb_grid->Get_NY()); y++)
 	{
		#pragma omp parallel for
		for (int x = 0; x < symb_grid->Get_NX(); x++)
		{
			symb_grid->Set_Value(x, y, symb_image[y+1][x+1]);
		}
	}


	// Here the shapes are generated

	int iPolygon;

	for (iPolygon = 0, reg_curr = reg_first; reg_curr != NULL; reg_curr = reg_curr -> next, iPolygon++)
	{
		CSG_Shape	*pShape = pOutlines->Add_Shape();

		pShape->Set_Value(0, iPolygon);		// set ID field (= first field in table) to polygon ID

		for (simple_PIXELC_list *pix_curr = reg_curr->first_pix; pix_curr != NULL; pix_curr = pix_curr->next)
		{
			TSG_Point point = symb_grid->Get_System().Get_Grid_to_World(pix_curr->col - 1, pix_curr->row - 1);
			pShape->Add_Point(point, 0);
		}

		int iHoles;
		simple_INNER_REGION_list *inner_curr;

		for (iHoles=0, inner_curr = reg_curr->inner_first;
			 iHoles < reg_curr->num_holes;
			 iHoles++, inner_curr = inner_curr->next)
		{
			for (simple_PIXELC_list *pix_curr = inner_curr->first_pix; pix_curr != NULL; pix_curr = pix_curr->next)
			{
				TSG_Point point = symb_grid->Get_System().Get_Grid_to_World(pix_curr->col - 1, pix_curr->row - 1);
				pShape->Add_Point(point, iHoles+1);
			}
		}

        if (!corners_centers)
            shift_shape (pShape, -Get_Cellsize()/2.0, -Get_Cellsize()/2.0);
	}

	matrix_all_free ((void **) bin_image);
	matrix_all_free ((void **) symb_image);
	free_regions (&reg_first, &reg_last);

	return( true );
}


int Cconnectivity_analysis::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if ( pParameter->Cmp_Identifier(SG_T("FILTER")) )
	{
		pParameters->Get_Parameter("SIZE")->Set_Enabled(pParameter->asBool());
		pParameters->Get_Parameter("FILTERED_MASK")->Set_Enabled(pParameter->asBool());
	}

	return (1);
}


void Cconnectivity_analysis::shift_shape (CSG_Shape *pShape, double dx, double dy)
{
	for (int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for (int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	point = pShape->Get_Point(iPoint, iPart);
			point.x		= point.x + dx;
			point.y		= point.y + dy;
			pShape->Set_Point(point, iPoint, iPart);
		} // iPoint
	} // iPart
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
