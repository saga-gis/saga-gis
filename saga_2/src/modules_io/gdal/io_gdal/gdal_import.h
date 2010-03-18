
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
//                    gdal_import.h                      //
//                                                       //
//            Copyright (C) 2007 O. Conrad               //
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
#ifndef HEADER_INCLUDED__gdal_import_H
#define HEADER_INCLUDED__gdal_import_H

//---------------------------------------------------------
#include "gdal_driver.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGDAL_Import : public CSG_Module
{
public:
	CGDAL_Import(void);


protected:

	virtual bool				On_Execute			(void);


private:

	CSG_Parameter_Grid_List		*m_pGrids;


	bool						Load_Sub			(CGDAL_System &System, const CSG_String &Name);
	bool						Load				(CGDAL_System &System, const CSG_String &Name);

	void						Set_Transformation	(CSG_Grid **ppGrid, const CSG_Vector &A, const CSG_Matrix &B);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gdal_import_H
