
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ESRI_ArcInfo.h                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//					ESRI_ArcInfo.h						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ESRI_ArcInfo_H
#define HEADER_INCLUDED__ESRI_ArcInfo_H



//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CESRI_ArcInfo_Import : public CModule
{
public:
	CESRI_ArcInfo_Import(void);
	virtual ~CESRI_ArcInfo_Import(void);

	virtual const char *	Get_MenuPath(void)				{	return( _TL("R:Import") );	}


protected:

	virtual bool			On_Execute(void);


private:

	CGrid *					Read_Header(FILE *Stream);

};

//---------------------------------------------------------
class CESRI_ArcInfo_Export : public CModule_Grid
{
public:
	CESRI_ArcInfo_Export(void);
	virtual ~CESRI_ArcInfo_Export(void);

	virtual const char *	Get_MenuPath(void)				{	return( _TL("R:Export") );	}


protected:

	virtual bool			On_Execute(void);


private:

	bool					Write_Header(FILE *Stream, CGrid *pGrid);

};

#endif // #ifndef HEADER_INCLUDED__ESRI_ArcInfo_H
