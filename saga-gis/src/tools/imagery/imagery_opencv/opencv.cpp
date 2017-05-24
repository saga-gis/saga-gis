/**********************************************************
 * Version $Id: opencv.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                      opencv.cpp                       //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "opencv.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int			Get_CVMatrix_Type	(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit:
	case SG_DATATYPE_Byte:		return( CV_8U  );	// Unsigned 8-bit integer

	case SG_DATATYPE_Char:		return( CV_8S  );	// Signed 8-bit integer

	case SG_DATATYPE_Word:		return( CV_16U );	// Unsigned 16-bit integer

	case SG_DATATYPE_Short:		return( CV_16S );	// Signed 16-bit integer

	case SG_DATATYPE_Color:
	case SG_DATATYPE_DWord:
	case SG_DATATYPE_ULong:
	case SG_DATATYPE_Long:
	case SG_DATATYPE_Int:		return( CV_32S );	// Signed 32-bit integer

	default:
	case SG_DATATYPE_Float:		return( CV_32F );	// Single-precision floating point

	case SG_DATATYPE_Double:	return( CV_64F );	// Double-precision floating point
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Copy_Grid_To_CVMatrix(CSG_Grid *pGrid, cv::Mat *pMatrix, bool bCheckSize)
{
	if( pMatrix && pGrid && (!bCheckSize || (pGrid->Get_NX() == pMatrix->cols && pGrid->Get_NY() == pMatrix->rows)) )
	{
		int		nx	= pGrid->Get_NX() < pMatrix->cols ? pGrid->Get_NX() : pMatrix->cols;
		int		ny	= pGrid->Get_NY() < pMatrix->rows ? pGrid->Get_NY() : pMatrix->rows;

		#pragma omp parallel for
		for(int y=0; y<ny; y++)
		{
			CvMat	Row	= pMatrix->row(y);

			for(int x=0; x<nx; x++)
			{
				switch( pMatrix->type() )
				{
				case CV_8U :	Row.data.ptr[x]	= pGrid->asByte  (x, y);	break;
				case CV_8S :	Row.data.ptr[x]	= pGrid->asChar  (x, y);	break;
				case CV_16U:	Row.data.s  [x]	= pGrid->asShort (x, y);	break;
				case CV_16S:	Row.data.s  [x]	= pGrid->asShort (x, y);	break;
				case CV_32S:	Row.data.i  [x]	= pGrid->asInt   (x, y);	break;
				case CV_32F:	Row.data.fl [x]	= pGrid->asFloat (x, y);	break;
				case CV_64F:	Row.data.db [x]	= pGrid->asDouble(x, y);	break;
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool	Copy_CVMatrix_To_Grid(CSG_Grid *pGrid, cv::Mat *pMatrix, bool bCheckSize)
{
	if( pMatrix && pGrid && (!bCheckSize || (pGrid->Get_NX() == pMatrix->cols && pGrid->Get_NY() == pMatrix->rows)) )
	{
		int		nx	= pGrid->Get_NX() < pMatrix->cols ? pGrid->Get_NX() : pMatrix->cols;
		int		ny	= pGrid->Get_NY() < pMatrix->rows ? pGrid->Get_NY() : pMatrix->rows;

		#pragma omp parallel for
		for(int y=0; y<ny; y++)
		{
			CvMat	Row	= pMatrix->row(y);

			for(int x=0; x<nx; x++)
			{
				switch( pMatrix->type() )
				{
				case CV_8U :	pGrid->Set_Value(x, y, Row.data.ptr[x]);	break;
				case CV_8S :	pGrid->Set_Value(x, y, ((char *)Row.data.ptr)[x]);	break;
				case CV_16U:	pGrid->Set_Value(x, y, ((WORD *)Row.data.s  )[x]);	break;
				case CV_16S:	pGrid->Set_Value(x, y, Row.data.s  [x]);	break;
				case CV_32S:	pGrid->Set_Value(x, y, Row.data.i  [x]);	break;
				case CV_32F:	pGrid->Set_Value(x, y, Row.data.fl [x]);	break;
				case CV_64F:	pGrid->Set_Value(x, y, Row.data.db [x]);	break;
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Get_CVMatrix(cv::Mat &Matrix, int nx, int ny, TSG_Data_Type Type)
{
	if( nx > 0 && ny > 0 )
	{
		CvSize	Size;

		Size.width	= nx;
		Size.height	= ny;

		Matrix.create(Size, Get_CVMatrix_Type(Type));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool	Get_CVMatrix(cv::Mat &Matrix, CSG_Grid *pGrid, TSG_Data_Type Type)
{
	if( pGrid && pGrid->is_Valid() && Get_CVMatrix(Matrix, pGrid->Get_NX(), pGrid->Get_NY(), Type == SG_DATATYPE_Undefined ? pGrid->Get_Type() : Type) )
	{
		Copy_Grid_To_CVMatrix(pGrid, &Matrix);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int			Get_CVImage_Type	(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit:
	case SG_DATATYPE_Byte:		return( IPL_DEPTH_8U  );	// Unsigned 8-bit integer

	case SG_DATATYPE_Char:		return( IPL_DEPTH_8S  );	// Signed 8-bit integer

	case SG_DATATYPE_Word:		return( IPL_DEPTH_16U );	// Unsigned 16-bit integer

	case SG_DATATYPE_Short:		return( IPL_DEPTH_16S );	// Signed 16-bit integer

	case SG_DATATYPE_Color:
	case SG_DATATYPE_DWord:
	case SG_DATATYPE_ULong:
	case SG_DATATYPE_Long:
	case SG_DATATYPE_Int:		return( IPL_DEPTH_32S );	// Signed 32-bit integer

	default:
	case SG_DATATYPE_Float:		return( IPL_DEPTH_32F );	// Single-precision floating point

	case SG_DATATYPE_Double:	return( IPL_DEPTH_64F );	// Double-precision floating point
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Copy_Grid_To_CVImage(CSG_Grid *pGrid, IplImage *pImage, bool bCheckSize)
{
	if( pImage && pGrid && (!bCheckSize || (pGrid->Get_NX() == pImage->width && pGrid->Get_NY() == pImage->height)) )
	{
		int		nx	= pGrid->Get_NX() < pImage->width  ? pGrid->Get_NX() : pImage->width;
		int		ny	= pGrid->Get_NY() < pImage->height ? pGrid->Get_NY() : pImage->height;

		#pragma omp parallel for
		for(int y=0; y<ny; y++)
		{
			CvMat	Row;

			cvGetRow(pImage, &Row, y);

			for(int x=0; x<nx; x++)
			{
				switch( (unsigned int)(pImage->depth) )
				{
				case IPL_DEPTH_8U:	Row.data.ptr[x]	= pGrid->asByte  (x, y);	break;
				case IPL_DEPTH_8S:	Row.data.ptr[x]	= pGrid->asChar  (x, y);	break;
				case IPL_DEPTH_16U:	Row.data.s  [x]	= pGrid->asShort (x, y);	break;
				case IPL_DEPTH_16S:	Row.data.s  [x]	= pGrid->asShort (x, y);	break;
				case IPL_DEPTH_32S:	Row.data.i  [x]	= pGrid->asInt   (x, y);	break;
				case IPL_DEPTH_32F:	Row.data.fl [x]	= pGrid->asFloat (x, y);	break;
				case IPL_DEPTH_64F:	Row.data.db [x]	= pGrid->asDouble(x, y);	break;
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool	Copy_CVImage_To_Grid(CSG_Grid *pGrid, IplImage *pImage, bool bCheckSize)
{
	if( pImage && pGrid && (!bCheckSize || (pGrid->Get_NX() == pImage->width && pGrid->Get_NY() == pImage->height)) )
	{
		int		nx	= pGrid->Get_NX() < pImage->width  ? pGrid->Get_NX() : pImage->width;
		int		ny	= pGrid->Get_NY() < pImage->height ? pGrid->Get_NY() : pImage->height;

		#pragma omp parallel for
		for(int y=0; y<ny; y++)
		{
			CvMat	Row;

			cvGetRow(pImage, &Row, y);

			for(int x=0; x<nx; x++)
			{
				switch( (unsigned int)(pImage->depth) )
				{
				case IPL_DEPTH_8U:	pGrid->Set_Value(x, y, Row.data.ptr[x]);	break;
				case IPL_DEPTH_8S:	pGrid->Set_Value(x, y, ((char *)Row.data.ptr)[x]);	break;
				case IPL_DEPTH_16U:	pGrid->Set_Value(x, y, ((WORD *)Row.data.s  )[x]);	break;
				case IPL_DEPTH_16S:	pGrid->Set_Value(x, y, Row.data.s  [x]);	break;
				case IPL_DEPTH_32S:	pGrid->Set_Value(x, y, Row.data.i  [x]);	break;
				case IPL_DEPTH_32F:	pGrid->Set_Value(x, y, Row.data.fl [x]);	break;
				case IPL_DEPTH_64F:	pGrid->Set_Value(x, y, Row.data.db [x]);	break;
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IplImage *	Get_CVImage(int nx, int ny, TSG_Data_Type Type)
{
	if( nx > 0 && ny > 0 )
	{
		CvSize	Size;

		Size.width	= nx;
		Size.height	= ny;

		return( cvCreateImage(Size, Get_CVImage_Type(Type), 1) );
	}

	return( NULL );
}

//---------------------------------------------------------
IplImage *	Get_CVImage(CSG_Grid *pGrid, TSG_Data_Type Type)
{
	IplImage	*pImage	= NULL;

	if( pGrid && pGrid->is_Valid() && (pImage = Get_CVImage(pGrid->Get_NX(), pGrid->Get_NY(), Type == SG_DATATYPE_Undefined ? pGrid->Get_Type() : Type)) != NULL )
	{
		Copy_Grid_To_CVImage(pGrid, pImage);
	}

	return( pImage );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
