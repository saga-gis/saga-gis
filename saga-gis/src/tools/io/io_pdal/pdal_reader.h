
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
#include <saga_api/saga_api.h>

#include <pdal/PointTable.hpp>


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

	bool				_Find_Class				(const CSG_Array_Int &Classes, int ID);

	CSG_PointCloud *    _Read_Points			(const CSG_String &File, const CSG_Rect &Extent, const CSG_Array_Int &Classes, bool bVar_All, bool bVar_Color, int iRGB_Range);

	void                _Init_PointCloud		(CSG_PointCloud *pPoints, pdal::BasePointTable &Table, bool bVar_All, bool bVar_Color, CSG_Array_Int &Fields, int &Field_RGB);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__pdal_reader_H
