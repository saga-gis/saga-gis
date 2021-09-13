
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_pdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pdal_reader.h                      //
//                                                       //
//               Copyrights (C) 2020-2021                //
//                     Olaf Conrad                       //
//                   Volker Wichmann                     //
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
#ifndef HEADER_INCLUDED__pdal_reader_H
#define HEADER_INCLUDED__pdal_reader_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pdal_driver.h"

#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointLayout.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/filters/StreamCallbackFilter.hpp>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPDAL_Reader : public CSG_Tool
{
public:
    CPDAL_Reader(void);

    virtual CSG_String  Get_MenuPath            (void)  { return( _TL("Import") );  }

    virtual bool        do_Sync_Projections     (void)  const { return( false );  }


protected:

    virtual int         On_Parameters_Enable    (CSG_Parameters *pParameters, CSG_Parameter *pParameter);

    virtual bool        On_Execute              (void);


private:

    CSG_PointCloud *    _Read_Points            (const CSG_String &File, bool bVar_All, bool bVar_Color, int iRGB_Range);

    void                _Init_PointCloud        (CSG_PointCloud *pPoints, pdal::PointLayoutPtr &PointLayout,
                                                 pdal::SpatialReference &SpatialRef, const CSG_String &File,
                                                 const bool &bVar_All, const bool &bVar_Color, CSG_Array_Int &Fields, int &iRGB_Field);
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__pdal_reader_H
