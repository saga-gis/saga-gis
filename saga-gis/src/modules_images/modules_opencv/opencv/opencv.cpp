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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
int			Get_CVType			(TSG_Data_Type Type)
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Copy_Grid_To_CVImage(CSG_Grid *pGrid, IplImage *pImage, bool bCheckSize)
{
	if( pImage && pGrid && (!bCheckSize || (pGrid->Get_NX() == pImage->width && pGrid->Get_NY() == pImage->height)) )
	{
		int		nx	= pGrid->Get_NX() < pImage->width  ? pGrid->Get_NX() : pImage->width;
		int		ny	= pGrid->Get_NY() < pImage->height ? pGrid->Get_NY() : pImage->height;

		for(int y=0; y<ny && SG_UI_Process_Set_Progress(y, ny); y++)
		{
			CvMat	Row;

			cvGetRow(pImage, &Row, y);

			for(int x=0; x<nx; x++)
			{
				switch( pImage->depth )
				{
				case IPL_DEPTH_8U:	Row.data.ptr[x]	= pGrid->asByte  (x, y);	break;
				case IPL_DEPTH_8S:	Row.data.ptr[x]	= pGrid->asByte  (x, y);	break;
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

		for(int y=0; y<ny && SG_UI_Process_Set_Progress(y, ny); y++)
		{
			CvMat	Row;

			cvGetRow(pImage, &Row, y);

			for(int x=0; x<nx; x++)
			{
				switch( pImage->depth )
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
//														 //
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

		return( cvCreateImage(Size, Get_CVType(Type), 1) );
	}

	return( NULL );
}

//---------------------------------------------------------
IplImage *	Get_CVImage(CSG_Grid *pGrid, TSG_Data_Type Type)
{
	IplImage	*pImage	= NULL;

	if( pGrid && (pImage = Get_CVImage(pGrid->Get_NX(), pGrid->Get_NY(), Type == SG_DATATYPE_Undefined ? pGrid->Get_Type() : Type)) != NULL )
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
