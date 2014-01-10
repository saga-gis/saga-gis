/**********************************************************
 * Version $Id: gdal_import_netcdf.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 gdal_import_netcdf.h                  //
//                                                       //
//            Copyright (C) 2012 O. Conrad               //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__gdal_import_netcdf_H
#define HEADER_INCLUDED__gdal_import_netcdf_H

//---------------------------------------------------------
#include "gdal_driver.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class io_gdal_EXPORT CGDAL_Import_NetCDF : public CSG_Module
{
public:
	CGDAL_Import_NetCDF(void);


protected:

	virtual bool				On_Execute				(void);

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	bool						m_bSaveFile;

	CSG_String					m_SavePath;

	CSG_Parameter_Grid_List		*m_pGrids;


	bool						Load					(CSG_GDAL_DataSet &DataSet, const CSG_String &Description);

	const char *				Get_Variable			(CSG_GDAL_DataSet &DataSet, int iBand);
	const char *				Get_Time				(CSG_GDAL_DataSet &DataSet, int iBand);
	const char *				Get_Level				(CSG_GDAL_DataSet &DataSet, int iBand);

	CSG_String					Get_Time_String			(const CSG_String &Time, int Format);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gdal_import_netcdf_H
