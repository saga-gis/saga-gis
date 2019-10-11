
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     saga_api.h                        //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef SWIG
#if defined(_SAGA_PYTHON)
%include <typemaps.i>
%include <python/cwstring.i>
#endif

%module saga_api
%{
#include "api_core.h"
#include "dataobject.h"
#include "data_manager.h"
#include "geo_tools.h"
#include "grid.h"
#include "grids.h"
#include "grid_pyramid.h"
#include "mat_tools.h"
#include "metadata.h"
#include "tool.h"
#include "tool_library.h"
#include "parameters.h"
#include "pointcloud.h"
#include "saga_api.h"
#include "shapes.h"
#include "table.h"
#include "table_value.h"
#include "tin.h"
%}
#endif	// #ifdef SWIG
//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__saga_api_H
#define HEADER_INCLUDED__SAGA_API__saga_api_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file saga_api.h
* This is the central header to include the complete SAGA API.
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool_library.h"
#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SAGA_MAJOR_VERSION		7
#define SAGA_MINOR_VERSION		5
#define SAGA_RELEASE_NUMBER		0
#define SAGA_VERSION			SG_T("7.5.0")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const SG_Char *	SAGA_API_Get_Version	(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT int	SG_Compare_Version	(const CSG_String &Version, int Major, int Minor, int Release);
SAGA_API_DLL_EXPORT int	SG_Compare_Version	(const CSG_String &Version, const CSG_String &Release);

SAGA_API_DLL_EXPORT int	SG_Compare_SAGA_Version	(int Major, int Minor, int Release);
SAGA_API_DLL_EXPORT int	SG_Compare_SAGA_Version	(const CSG_String &Version);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__saga_api_H
