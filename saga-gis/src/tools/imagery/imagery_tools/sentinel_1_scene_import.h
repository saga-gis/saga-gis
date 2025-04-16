
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               sentinel_1_scene_import.h               //
//                                                       //
//                 Olaf Conrad (C) 2025                  //
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
#ifndef HEADER_INCLUDED__sentinel_1_scene_import_H
#define HEADER_INCLUDED__sentinel_1_scene_import_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSentinel_1_Scene_Import : public CSG_Tool
{
public:
	CSentinel_1_Scene_Import(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:File|Satellite Imagery") );	}

	virtual bool				On_Before_Execution		(void);
	virtual bool				On_After_Execution		(void);


protected:

	virtual int                 On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CSG_Parameters_CRSPicker	m_CRS;

	CSG_Parameters_Grid_Target	m_Grid_Target;


	bool						Get_Files				(const CSG_String &File, CSG_Strings Files[2]);
	bool						Get_MetaData			(const CSG_String &File, CSG_Grid &Band);
	static bool					Get_Geolocations		(const CSG_String &File, CSG_Shapes &Points, const CSG_Projection &CRS, CSG_String &Error);
	static CSG_Grid_System		Get_System				(const CSG_String &File, const CSG_Projection &CRS);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__sentinel_1_scene_import_H
