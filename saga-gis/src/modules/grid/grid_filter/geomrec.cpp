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
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     geomrec.cpp                       //
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
#include "geomrec.h"

extern "C" {
	#include "geodesic_morph_rec/storeorg.h"
	#include "geodesic_morph_rec/geovinc.h"
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeomrec::CGeomrec(void)
{
	Set_Name		(_TL("Geodesic Morphological Reconstruction"));

	Set_Author		(SG_T("HfT Stuttgart (c) 2013"));

	Set_Description	(_TW(
		"Geodesic morphological reconstruction according to \n"
		"L. Vincent (1993): Morphological Grayscale Reconstruction in Image Analysis: "
		"Applications and Efficient Algorithms. "
		"IEEE Transactions on Image Processing, Vol. 2, No 2\n"
		"Here we use the algorithm on p. 194: Computing of Regional Maxima and Breadth-first Scanning.\n\n"
		"A marker is derived from the input image INPUT_GRID by subtracting a constant SHIFT_VALUE. Optionally "
		"the SHIFT_VALUE can be set to zero at the border of the grid (\"Preserve 1px border Yes/No\"). "
		"OUTPUT_GRID is the difference between the input image and the morphological reconstruction of "
		"the marker under the input image as mask. If the Option \"Create a binary mask\" is selected, "
		"the OUTPUT_GRID is thresholded with THRESHOLD, creating a binary image of maxima regions.\n"
	));

	CSG_Parameter	*pNode;

	Parameters.Add_Grid (NULL, "INPUT_GRID", _TL ("Input Grid"), _TL ("Input for the morphological reconstruction"), PARAMETER_INPUT);
	Parameters.Add_Grid (NULL, "OBJECT_GRID", _TL("Object Grid"), _TL("Binary object mask"), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid (NULL, "DIFFERENCE_GRID", _TL ("Difference Input - Reconstruction"), _TL ("Difference Input - Reconstruction"), PARAMETER_OUTPUT);
	Parameters.Add_Value (Parameters ("SHIFT"), "SHIFT_VALUE", _TL ("Shift value"), _TL ("Shift value"), PARAMETER_TYPE_Double, 5);
	Parameters.Add_Value (NULL, "BORDER_YES_NO", _TL ("Preserve 1px border Yes/No"), _TL ("Preserve 1px border Yes/No"), PARAMETER_TYPE_Bool, true);
	pNode = Parameters.Add_Value (NULL, "BIN_YES_NO", _TL ("Create a binary mask Yes/No"), _TL ("Create a binary mask Yes/No"), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value (pNode, "THRESHOLD", _TL ("Threshold"), _TL ("Threshold"), PARAMETER_TYPE_Double, 1);
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

bool CGeomrec::On_Execute(void)
{
	CSG_Grid *pinpgrid, *bingrid, *poutgrid;


	unsigned short numrows;
	unsigned short numcols;
	double **mask;
	double **marker;


	double h, t;
	bool pborder, bin;

	pinpgrid = Parameters ("INPUT_GRID")->asGrid ();
	bingrid = Parameters ("OBJECT_GRID")->asGrid();
	poutgrid = Parameters ("DIFFERENCE_GRID")->asGrid ();

	h = Parameters ("SHIFT_VALUE")->asDouble ();
	t = Parameters ("THRESHOLD")->asDouble ();
	pborder = Parameters ("BORDER_YES_NO")->asBool ();
	bin = Parameters ("BIN_YES_NO")->asBool ();

	numcols = Get_NY();
	numrows = Get_NX();

	mask = (double **) matrix_all_alloc (numrows, numcols, 'D', 0);
	marker = (double **) matrix_all_alloc (numrows, numcols, 'D', 0);

	for (int y = 0; y < Get_NY () && Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
       for (int x = 0; x < Get_NX (); x++)
       {
		  if (pinpgrid->is_NoData(x,y)) // check if there are no_data in input datasets
		  {
		 	 mask [x][y] = -999999.9;
		 	 marker [x][y] = -999999.9;
		  }
		  else if ((pborder) && ((x==0)||(y==0)||(x==Get_NX()-1)||(y==Get_NY()-1)))
   		  {
			 mask [x][y] = pinpgrid->asDouble(x,y);
			 marker [x][y] = pinpgrid->asDouble(x,y);
		  }
		  else
		  {
			 mask [x][y] = pinpgrid->asDouble(x,y);
			 marker [x][y] = pinpgrid->asDouble(x,y) - h;
		  }
       }
	}

   geodesic_morphological_reconstruction (numrows, numcols, mask, marker);

   for (int y = 0; y < Get_NY () && Set_Progress(y, Get_NY()); y++)
   {
	  #pragma omp parallel for
	  for (int x = 0; x < Get_NX (); x++)
	  {
		 if (pinpgrid->is_NoData(x,y))
		    poutgrid->Set_NoData(x,y);
		 else
		    poutgrid->Set_Value(x,y, mask[x][y]-marker[x][y]);
	  }
   }

   if (bin)
   {
	   for (int y = 0; y < Get_NY () && Set_Progress(y, Get_NY()); y++)
			#pragma omp parallel for
			for (int x = 0; x < Get_NX (); x++)
				if ((mask[x][y]-marker[x][y])>t)
						bingrid->Set_Value(x, y, 1);
					else
						bingrid->Set_Value(x, y, 0);
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
