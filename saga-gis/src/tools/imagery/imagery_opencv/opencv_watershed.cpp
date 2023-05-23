
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  opencv_watershed.cpp                 //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
#include "opencv_watershed.h"

#include "opencv2/imgproc.hpp"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_Watershed::COpenCV_Watershed(void)
{
	Set_Name		(_TL("Watershed Image Segmentation"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"Watershed Segmentation."
	));

	Add_Reference("https://docs.opencv.org/4.x/d3/db4/tutorial_py_watershed.html",
		SG_T("OpenCV Tutorial | Watershed Segmentation")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "BAND_1", CSG_String::Format("%s 1", _TL("Band")), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid  ("", "BAND_2", CSG_String::Format("%s 2", _TL("Band")), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "BAND_3", CSG_String::Format("%s 3", _TL("Band")), _TL(""), PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid  ("", "SEED_GRID", _TL("Seeds"), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid  ("", "SEGMENTS", _TL("Segments"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Int);
	Parameters.Add_Shapes("", "POLYGONS", _TL("Polygons"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon);

	Parameters.Add_Choice("",
		"SEEDS", _TL("Get Seeds from..."),
		_TL("Either local maxima (peaks) or minima (pits) of band 1, or provided seeds grid (enumerated values greater zero or not no-data)."),
		CSG_String::Format("%s|%s|%s", _TL("peaks"), _TL("pits"), _TL("grid")), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_Watershed::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SEEDS") )
	{
		pParameters->Set_Enabled("SEED_GRID", pParameter->asInt() == 2); // seed grid
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_Watershed::On_Execute(void)
{
	if( !Parameters("SEGMENTS")->asGrid() && !Parameters("POLYGONS")->asShapes() )
	{
		Error_Fmt("%s\n%s", _TL("No output has been selected!"),
			_TL("Activate output creation either for segments grid, polygons, or both.")
		);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pBand[3] =
	{
		Parameters("BAND_1")->asGrid(),
		Parameters("BAND_2")->asGrid(),
		Parameters("BAND_3")->asGrid()
	};

	cv::Mat Grid(Get_NY(), Get_NX(), CV_8UC3);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		cv::Vec3b *bands = Grid.ptr<cv::Vec3b>(y);

		for(int x=0; x<Get_NX(); x++)
		{
			#define GET_VALUE(i) (uchar)(!pBand[i] || pBand[i]->is_NoData(x, y) ? 0. :\
				(pBand[i]->asDouble(x, y) - pBand[i]->Get_Min()) * 255. / pBand[i]->Get_Range()\
			)

			bands[x][0] = GET_VALUE(0); bands[x][1] = GET_VALUE(1); bands[x][2] = GET_VALUE(2);
		}
	}

	//-----------------------------------------------------
	cv::Mat Marker(Get_NY(), Get_NX(), CV_32S);

	int Seeding = Parameters("SEEDS")->asInt();

	if( Seeding == 2 ) // seeds grid
	{
		CSG_Grid *pSeeds = Parameters("SEED_GRID")->asGrid();

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			int *marker = Marker.ptr<int>(y);

			for(int x=0; x<Get_NX(); x++)
			{
				marker[x] = pSeeds->is_NoData(x, y) || pSeeds->asInt(x, y) <= 0 ? 0 : pSeeds->asInt(x, y);
			}
		}
	}
	else
	{
		int n = 0;

		for(int y=0; y<Get_NY(); y++)
		{
			int *marker = Marker.ptr<int>(y);

			for(int x=0; x<Get_NX(); x++)
			{
				double z = pBand[0]->asDouble(x, y); bool bSeed = true;

				for(int i=0; bSeed && i<8; i++)
				{
					int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

					if( !pBand[0]->is_InGrid(ix, iy) )
					{
						bSeed = false;
					}
					else switch( Seeding )
					{
					case 0: bSeed = z > pBand[0]->asDouble(ix, iy); break;
					case 1: bSeed = z < pBand[0]->asDouble(ix, iy); break;
					}
				}

				marker[x] = bSeed ? ++n : 0;
			}
		}

		if( n < 1 )
		{
			Message_Fmt("\n%s\n", _TL("No segments have been detected"));

			return( false );
		}

		Message_Fmt("\n%s: %d\n", _TL("Number of detected seeds"), n);
	}

	//-----------------------------------------------------
	cv::watershed(Grid, Marker);

	//-----------------------------------------------------
	int *marker = (int *)Marker.data;

	CSG_Grid Segments, *pSegments = Parameters("SEGMENTS")->asGrid();

	if( !pSegments )
	{
		pSegments = &Segments; Segments.Create(Get_System(), SG_DATATYPE_Int);
	}

	pSegments->Fmt_Name("%s [%s]", pBand[0]->Get_Name(), _TL("Watershed"));
	pSegments->Set_NoData_Value(-1.);

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		pSegments->Set_Value(i, marker[i]);
	}

	//-----------------------------------------------------
	if( Parameters("POLYGONS")->asShapes() )
	{
		bool bResult; CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

		SG_RUN_TOOL(bResult, "shapes_grid", 19, // Boundary Cells to Polygons
			   SG_TOOL_PARAMETER_SET("GRID"    , pSegments)
			&& SG_TOOL_PARAMETER_SET("POLYGONS", pPolygons)
		);

		if( bResult )
		{
			pPolygons->Fmt_Name(pSegments->Get_Name());
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
