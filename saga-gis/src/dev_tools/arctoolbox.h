/**********************************************************
 * Version $Id: arctoolbox.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     arctoolbox.h                      //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#ifndef HEADER_INCLUDED__arctoolbox_H
#define HEADER_INCLUDED__arctoolbox_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CArcToolBox : public CSG_Module
{
public: ////// public members and functions: //////////////

	CArcToolBox(void);


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute			(void);


private: ///// private members and functions: /////////////

	int					m_ArcVersion;


	CSG_String			Get_Formatted		(CSG_String String, int Type);
	CSG_String			Get_Description		(CSG_Module *pTool, int Type);
	CSG_String			Get_ID				(CSG_Parameter *pParameter, const CSG_String &Modifier);
	CSG_Parameter *		Get_GridTarget		(CSG_Parameters *pParameters);

	bool				Get_Parameter		(CSG_Parameter *pParameter, CSG_Strings &Info, CSG_Strings &Init, CSG_MetaData &Descs, const CSG_String &Name, const CSG_String &Identifier);
	bool				Get_Parameter		(CSG_Parameter *pParameter, CSG_Strings &Info, CSG_Strings &Init, CSG_MetaData &Descs, CSG_Parameter *pGridTarget);
	bool				Get_Tool			(CSG_Module_Library *pLibrary, int iTool, CSG_String &Code, CSG_MetaData &Description);

	bool				Save				(CSG_Module_Library *pLibrary, const CSG_String &Directory, const CSG_Strings &Names, const CSG_Strings &Codes, const CSG_Strings &Descs);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__arctoolbox_H
