/**********************************************************
 * Version $Id: citygml_import.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      io_shapes                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    citygml_import.h                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__citygml_import_H
#define HEADER_INCLUDED__citygml_import_H


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCityGML_Import : public CSG_Module
{
public: ////// public members and functions: //////////////

	CCityGML_Import(void);

	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("Import") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute				(void);


private: ///// private members and functions: /////////////

	bool				Get_Buildings			(const CSG_String &File, CSG_Shapes *pPolygons, bool bParts);
	bool				Get_Buildings			(const CSG_String &File, CSG_Shapes *pPolygons);

	bool				Add_Buildings			(CSG_Shapes *pBuildings, CSG_Shapes *pAdd);

	bool				Has_BuildingParts		(const CSG_MetaData &GML);
	bool				Add_BuildingParts		(const CSG_MetaData &GML, CSG_MetaData &GML_Parts);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__citygml_import_H
