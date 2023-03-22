
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
#include "geomrec.h"

//---------------------------------------------------------
extern "C"
{
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

	Set_Author		("HfT Stuttgart (c) 2013");

	Set_Description	(_TW(
		"Geodesic morphological reconstruction according to Vincent (1993). "
		"Here we use the algorithm on p. 194: Computing of Regional Maxima and Breadth-first Scanning.\n\n"
		"A marker is derived from the input grid by subtracting a constant shift value. Optionally, "
		"the shift value can be set to zero at the border of the grid (\"Preserve 1px border Yes/No\" parameter). "
		"The final result is a grid showing the difference between the input image and the morphological reconstruction of "
		"the marker under the input image. If the Option \"Create a binary mask\" is selected, "
		"the difference grid is thresholded with provided threshold value to create a binary image of maxima regions. "
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

	Parameters.Add_Grid  ("", "INPUT_GRID"     , _TL("Input Grid"                  ), _TL("Input for the morphological reconstruction"), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "DIFFERENCE_GRID", _TL("Morphological Reconstruction"), _TL("Reconstruction, difference to input grid."), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "OBJECT_GRID"    , _TL("Objects"                     ), _TL("Binary object mask"), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char);
	Parameters.Add_Double("", "SHIFT_VALUE"    , _TL("Shift Value"                 ), _TL(""), 5.);
	Parameters.Add_Bool  ("", "BORDER_YES_NO"  , _TL("Preserve 1px border"         ), _TL(""), true);
	Parameters.Add_Double("", "THRESHOLD"      , _TL("Threshold"                   ), _TL("Threshold for object identification."), 1., 0., true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeomrec::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("OBJECT_GRID") )
	{
		pParameters->Set_Enabled("THRESHOLD", pParameter->asPointer() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeomrec::On_Execute(void)
{
	CSG_Grid *pInput = Parameters("INPUT_GRID"   )->asGrid  ();
	double     Shift = Parameters("SHIFT_VALUE"  )->asDouble();
	bool     bBorder = Parameters("BORDER_YES_NO")->asBool  ();

	unsigned short numcols = Get_NY();
	unsigned short numrows = Get_NX();

	double **mask   = (double **) matrix_all_alloc (numrows, numcols, 'D', 0);
	double **marker = (double **) matrix_all_alloc (numrows, numcols, 'D', 0);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( pInput->is_NoData(x, y) ) // check if there are no_data in input datasets
		{
		 	mask  [x][y] = -999999.9;
		 	marker[x][y] = -999999.9;
		}
		else if( bBorder && ((x==0)||(y==0)||(x==Get_NX()-1)||(y==Get_NY()-1)))
   		{
			mask  [x][y] = pInput->asDouble(x, y);
			marker[x][y] = pInput->asDouble(x, y);
		}
		else
		{
			mask  [x][y] = pInput->asDouble(x, y);
			marker[x][y] = pInput->asDouble(x, y) - Shift;
		}
	}

	//----------------------------------------------------
	geodesic_morphological_reconstruction (numrows, numcols, mask, marker);

	//----------------------------------------------------
	CSG_Grid *pDifference = Parameters("DIFFERENCE_GRID")->asGrid();

	pDifference->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Reconstruction"));
	pDifference->Set_NoData_Value(0.);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( pInput->is_NoData(x, y) )
		{
			pDifference->Set_NoData(x, y);
		}
		else
		{
			pDifference->Set_Value(x, y, mask[x][y] - marker[x][y]);
		}
	}

	//----------------------------------------------------
	CSG_Grid *pObjects = Parameters("OBJECT_GRID")->asGrid();

	if( pObjects )
	{
		pObjects->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Objects"));
		pObjects->Set_NoData_Value(0.);

		double Threshold = Parameters("THRESHOLD")->asDouble();

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( pDifference->is_NoData(x, y) || pDifference->asDouble(x, y) < Threshold )
			{
				pObjects->Set_NoData(x, y);
			}
			else
			{
				pObjects->Set_Value(x, y, 1.);
			}
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
