
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
//                    opencv_canny.cpp                   //
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
#include "opencv_canny.h"

#include "opencv2/imgproc.hpp"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_Canny::COpenCV_Canny(void)
{
	Set_Name		(_TL("Canny Edge Detection"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"Canny edge detection."
	));

	Add_Reference("https://docs.opencv.org/4.7.0/da/d22/tutorial_py_canny.html",
		SG_T("OpenCV Tutorial | Canny Edge Detection")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "GRID"       , _TL("Grid"       ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid  ("", "EDGES"      , _TL("Edges"      ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SG_DATATYPE_Byte);
	Parameters.Add_Shapes("", "EDGE_LINES" , _TL("Edge Lines" ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line);

	Parameters.Add_Double("", "THRESHOLD"  , _TL("Threshold"  ), _TL(""), 1., 0., true);
	Parameters.Add_Double("", "RATIO"      , _TL("Ratio"      ), _TL(""), 3., 1., true);
	Parameters.Add_Int   ("", "KERNEL_SIZE", _TL("Kernel Size"), _TL(""), 1, 1, true, 3, true);
	Parameters.Add_Bool  ("", "L2GRADIENT" , _TL("L2 Gradient"), _TL(""), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_Canny::On_Execute(void)
{
	if( !Parameters("EDGES")->asGrid() && !Parameters("EDGE_LINES")->asShapes() )
	{
		Error_Fmt("%s\n%s", _TL("No output has been selected!"),
			_TL("Activate output creation either for edges grid, edge lines, or both.")
		);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pGrid = Parameters("GRID")->asGrid(); pGrid->Set_Max_Samples(pGrid->Get_NCells());

	cv::Mat Grid(Get_NY(), Get_NX(), CV_8U);

	for(int y=0; y<Get_NY() && Process_Get_Okay(); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Grid.at<uchar>(y, x) = (unsigned char)((pGrid->asDouble(x, y) - pGrid->Get_Min()) * 255 / pGrid->Get_Range());
		}
	}

	//-----------------------------------------------------
	double Tmin = Parameters("THRESHOLD"  )->asDouble();
	double Tmax = Parameters("RATIO"      )->asDouble() * Tmin;
	int    Size = Parameters("KERNEL_SIZE")->asInt   () * 2 + 1;
	bool   bL2G = Parameters("L2GRADIENT" )->asBool  ();

	cv::Canny(Grid, Grid, Tmin, Tmax, Size, bL2G);

	//-----------------------------------------------------
	CSG_Grid Edges, *pEdges = Parameters("EDGES")->asGrid();

	if( !pEdges )
	{
		pEdges = &Edges; Edges.Create(Get_System(), SG_DATATYPE_Byte);
	}

	pEdges->Fmt_Name("%s [%s]", pGrid->Get_Name(), Get_Name().c_str());
	pEdges->Set_NoData_Value(0.);

	unsigned char *data = (unsigned char *)Grid.data;

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		pEdges->Set_Value(i, data[i] ? 1. : 0.);
	}

	//-----------------------------------------------------
	if( Parameters("EDGE_LINES")->asShapes() )
	{
		bool bResult; CSG_Shapes *pLines = Parameters("EDGE_LINES")->asShapes();

		SG_RUN_TOOL(bResult, "imagery_segmentation", 1,
			   SG_TOOL_PARAMETER_SET("INPUT" , pEdges)
			&& SG_TOOL_PARAMETER_SET("VECTOR", pLines)
		);

		if( bResult )
		{
			pLines->Fmt_Name("%s [%s]", pGrid->Get_Name(), Get_Name().c_str());
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
