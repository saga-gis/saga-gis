
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
//                       opencv.h                        //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#ifndef HEADER_INCLUDED__opencv_H
#define HEADER_INCLUDED__opencv_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include <opencv2/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core_c.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int			Get_CVMatrix_Type		(TSG_Data_Type Type);

bool		Copy_Grid_To_CVMatrix	(CSG_Grid *pGrid, cv::Mat &Matrix, bool bCheckSize = true);
bool		Copy_CVMatrix_To_Grid	(CSG_Grid *pGrid, cv::Mat &Matrix, bool bCheckSize = true);

bool		Get_CVMatrix			(cv::Mat &Matrix, int nx, int ny , TSG_Data_Type Type);
bool		Get_CVMatrix			(cv::Mat &Matrix, CSG_Grid *pGrid, TSG_Data_Type Type = SG_DATATYPE_Undefined);

//---------------------------------------------------------
int			Get_CVImage_Type		(TSG_Data_Type Type);

bool		Copy_Grid_To_CVImage	(CSG_Grid *pGrid, IplImage *pImage, bool bCheckSize = true);
bool		Copy_CVImage_To_Grid	(CSG_Grid *pGrid, IplImage *pImage, bool bCheckSize = true);

IplImage *	Get_CVImage				(int nx, int ny , TSG_Data_Type Type);
IplImage *	Get_CVImage				(CSG_Grid *pGrid, TSG_Data_Type Type = SG_DATATYPE_Undefined);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__opencv_H
