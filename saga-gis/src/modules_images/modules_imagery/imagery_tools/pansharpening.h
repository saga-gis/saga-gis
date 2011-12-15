/**********************************************************
 * Version $Id: pansharp_pca.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pansharpening.h                    //
//                                                       //
//                 Copyright (C) 2011 by                 //
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

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__pansharpening_pca_H
#define HEADER_INCLUDED__pansharpening_pca_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPanSharp_IHS : public CSG_Module_Grid
{
public:
	CPanSharp_IHS(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Image Sharpening") );	}


protected:

	virtual bool			On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPanSharp_Brovey : public CSG_Module_Grid
{
public:
	CPanSharp_Brovey(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Image Sharpening") );	}


protected:

	virtual bool			On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPanSharp_CN : public CSG_Module_Grid
{
public:
	CPanSharp_CN(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Image Sharpening") );	}


protected:

	virtual bool			On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPanSharp_PCA : public CSG_Module_Grid
{
public:
	CPanSharp_PCA(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Image Sharpening") );	}


protected:

	virtual bool			On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__pansharpening_pca_H
