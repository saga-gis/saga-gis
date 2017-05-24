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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     kff_synth.cpp                     //
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
#include "kff_synth.h"

extern "C" {
	#include "./../grid_filter/geodesic_morph_rec/storeorg.h"
	#include "./../grid_filter/geodesic_morph_rec/spezfunc.h"
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Ckff_synthesis::Ckff_synthesis(void)
{
	Set_Name		(_TL("Spherical Harmonic Synthesis"));

	Set_Author		(SG_T("HfT Stuttgart (c) 2013"));

	Set_Description	(_TW(
		"Synthesis of a completely normalized spherical harmonic expansion. The coefficients are "
		"read from the input file (ASCII file, columns separated by space).\n\n"
	));

	Parameters.Add_Grid_Output (NULL,
		                 "OUTPUT_GRID",
						 _TL("Synthesized Grid"),
						 _TL("Synthesized Grid"));

	Parameters.Add_FilePath (NULL,
		                     "FILE",
							 _TL("File with Coefficients"),
		                     _TL("ASCII file with columns degree, order, c_lm, s_lm (separated by space)"));

	Parameters.Add_Value (NULL, "MINDEGREE",
						  _TL ("Start Degree"),
						  _TL ("Start Degree"),
						  PARAMETER_TYPE_Int,
						  0);

	Parameters.Add_Value (NULL, "MAXDEGREE",
						  _TL ("Expansion Degree"),
						  _TL ("Expansion Degree"),
						  PARAMETER_TYPE_Int,
						  180);

	Parameters.Add_Value (NULL, "LAT_START",
						  _TL ("Start Latitude"),
						  _TL ("Start Latitude"),
						  PARAMETER_TYPE_Double,
						  -90.0);

	Parameters.Add_Value (NULL, "END_LAT",
						  _TL ("End Latitude"),
						  _TL ("End Latitude"),
						  PARAMETER_TYPE_Double,
						  90.0);

	Parameters.Add_Value (NULL, "LONG_START",
						  _TL ("Start Longitude"),
						  _TL ("Start Longitude"),
						  PARAMETER_TYPE_Double,
						  -180.0);

	Parameters.Add_Value (NULL, "END_LONG",
						  _TL ("End Longitude"),
						  _TL ("End Longitude"),
						  PARAMETER_TYPE_Double,
						  180.0);

	Parameters.Add_Value (NULL, "INC",
						  _TL ("Latitude / Longitude Increment"),
						  _TL ("Latitude / Longitude Increment"),
						  PARAMETER_TYPE_Double,
						  1.0);
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

bool Ckff_synthesis::On_Execute(void)
{
	CSG_Grid *poutgrid;

	int numlat = 0;
	int numlong = 0;
	int maxdegree = 0;
	int mindegree = 0;
	int rc = 0;
	double inc = 0.0;
	double lat_start = 0.0;
	double end_lat = 0.0;
	double long_start = 0.0;
	double end_long = 0.0;
	CSG_String fileName;
    double **c_lm;
	double **s_lm;
	double **gitter;
	char *error_liste = "nix";

	//poutgrid = Parameters ("OUTPUT GRID")->asGrid ();

	fileName	= Parameters("FILE")->asString();
	inc = Parameters ("INC")->asDouble ();
	mindegree = Parameters ("MINDEGREE")->asInt ();
	maxdegree = Parameters ("MAXDEGREE")->asInt ();
	lat_start = Parameters ("LAT_START")->asDouble ();
	end_lat = Parameters ("END_LAT")->asDouble ();
	long_start = Parameters ("LONG_START")->asDouble ();
	end_long = Parameters ("END_LONG")->asDouble ();

	numlat = static_cast <int> (floor ((end_lat - lat_start) / inc) + 1);
	numlong = static_cast <int> (floor ((end_long - long_start) / inc) + 1);
	gitter = (double **) matrix_all_alloc (numlat, numlong, 'D', 0);

	read_coefficients (fileName.b_str(),
		               mindegree,
                       maxdegree,
                       &c_lm,
                       &s_lm);


	rc = kff_synthese_regel_gitter_m (inc,
                                    lat_start,
                                    end_lat,
                                    long_start,
									end_long,
									numlat,
									numlong,
                                    'A',
                                    mindegree,
                                    maxdegree,
                                    c_lm,
                                    s_lm,
                                    gitter,
                                    &error_liste);

	poutgrid	= SG_Create_Grid(SG_DATATYPE_Double, numlong, numlat, inc, long_start, lat_start);
	poutgrid	->Set_Name(_TL("Synthesized Grid"));

	for (int y = 0; y < numlat; y++)
	{
		#pragma omp parallel for
       for (int x = 0; x < numlong; x++)
       {
		    poutgrid->Set_Value(x,y, gitter[y][x]);
	   }
   }

   Parameters("OUTPUT_GRID")->Set_Value(poutgrid);

   matrix_all_free ((void **) gitter);
   matrix_all_free ((void **) c_lm);
   matrix_all_free ((void **) s_lm);

   return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
