/**********************************************************
 * Version $Id: las_info.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_IO_LAS                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      las_info.h                       //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
//                                                       //
//    Implementation builds upon the lasinfo tool of     //
//         Martin Isenburg (isenburg@cs.unc.edu)         //
//                  Copyright (C) 2007                   //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__las_info_H
#define HEADER_INCLUDED__las_info_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <liblas/laspoint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/capi/las_version.h>
#include <fstream>
#include <iostream>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

const SG_Char	gLASPointClassification_Key_Name[32][32]	=
{
	SG_T("Created, never classified"),
	SG_T("Unclassified"),
	SG_T("Ground"),
	SG_T("Low Vegetation"),
	SG_T("Medium Vegetation"),
	SG_T("High Vegetation"),
	SG_T("Building"),
	SG_T("Low Point (noise)"),
	SG_T("Model Key-point (mass point)"),
	SG_T("Water"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Overlap Points"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition"),
	SG_T("Reserved for ASPRS Definition")
};


typedef struct	{
				double t;
				double x, y, z;
				liblas::uint16_t intensity;
				liblas::uint8_t cls;
				liblas::int8_t scan_angle;
				liblas::uint8_t user_data;
				liblas::uint16_t retnum;
				liblas::uint16_t numret;
				liblas::uint16_t scandir;
				liblas::uint16_t fedge;
				liblas::uint16_t red;
				liblas::uint16_t green;
				liblas::uint16_t blue;
				long rgpsum;    
				int number_of_point_records;
				int number_of_points_by_return[8];
				int number_of_returns_of_given_pulse[8];
				int classification[32];
				int classification_synthetic;
				int classification_keypoint;
				int classification_withheld;
				liblas::LASPoint pmax;
				liblas::LASPoint pmin;
				}
	LASPointSummary;


//---------------------------------------------------------
class CLAS_Info : public CSG_Module
{
public:
	CLAS_Info(void);

	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("R:Import") );	}


protected:

	virtual bool			On_Execute			(void);
	bool					Print_Header		(CSG_String fName, liblas::LASHeader header);
	bool					Print_Point_Summary	(liblas::LASHeader header, LASPointSummary *pSummary);
	bool					Summarize_Points	(liblas::LASReader *pReader, LASPointSummary *pSummary, int headerPts);


private:

	

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__las_info_H
