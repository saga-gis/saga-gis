
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                       io_pdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pdal_driver.h                      //
//                                                       //
//                 Copyright (C) 2021 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__pdal_driver_H
#define HEADER_INCLUDED__pdal_driver_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


//---------------------------------------------------------
// list of drivers that are available with the currently used
// windows build of PDAL (2.0.1) but which do not work and
// thus need to be skipped / disabled by us
// 
// test data can be found at
// https://github.com/PDAL/PDAL/tree/master/test/data


//---------------------------------------------------------
struct NON_WORKING_DRIVERS
{
    CSG_String	Name;
};

const struct NON_WORKING_DRIVERS g_Non_Working_Drivers[]	=
{
    {	"readers.bpf"           },
    {	"readers.ept"           },
    {	"readers.faux"          },
    {	"readers.gdal"          },
    {	"readers.memoryview"    },
    {	"readers.optech"        },
    {	""                      }
};



///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_PDAL_Drivers
{
public:
    CSG_PDAL_Drivers(void);
    virtual ~CSG_PDAL_Drivers(void);

    CSG_String                  Get_Version             (void)                  const;
    int                         Get_Count               (void)                  const;
    CSG_String                  Get_Driver_Name         (int Index)             const;
    CSG_String                  Get_Driver_Description  (int Index)             const;
    CSG_Strings                 Get_Driver_Extensions   (int Index)             const;
    bool                        is_Reader               (int Index)             const;
    bool                        is_Writer               (int Index)             const;
    bool                        is_Filter               (int Index)             const;


private:

};


//---------------------------------------------------------
const CSG_PDAL_Drivers &  SG_Get_PDAL_Drivers (void);


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__pdal_driver_H
