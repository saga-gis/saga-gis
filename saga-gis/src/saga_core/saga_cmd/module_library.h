/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                Command Line Interface                 //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   module_library.h                    //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_CMD__Module_Library_H
#define _HEADER_INCLUDED__SAGA_CMD__Module_Library_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/cmdline.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCMD_Data_Objects
{
public:
	CCMD_Data_Objects(void);
	virtual ~CCMD_Data_Objects(void);

	void						Clear					(bool bDelete = true);

	void						Add						(class CSG_Data_Object *pObject);


private:

	int							m_nObjects;

	class CSG_Data_Object		**m_pObjects;

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCMD_Module
{
public:
	CCMD_Module(void);
	CCMD_Module(CSG_Module *pModule);
	virtual ~CCMD_Module(void);

	bool						Create					(CSG_Module *pModule);
	void						Destroy					(void);

	bool						Execute					(int argc, char *argv[]);

	bool						Get_Parameters			(class CSG_Parameters *pParameters);
	bool						Add_DataObject			(class CSG_Data_Object *pObject);


private:

	CSG_Module					*m_pModule;

	CCMD_Data_Objects			m_Data_Objects;

	wxCmdLineParser				m_CMD;


	wxString					_Get_ID					(CSG_Parameter  *pParameter, const wxString &Modifier = SG_T(""));

	void						_Set_CMD				(CSG_Parameters *pParameters, bool bExtra);
	bool						_Get_CMD				(CSG_Parameters *pParameters, bool bNoDataObjects);

	bool						_Create_DataObjects		(CSG_Parameters *pParameters);
	bool						_Create_DataObject		(CSG_Parameter  *pParameter, const wxString &FileName);
	bool						_Create_DataObject_List	(CSG_Parameter  *pParameter, wxString sList);

	bool						_Destroy_DataObjects	(bool bSave);
	bool						_Destroy_DataObjects	(bool bSave, CSG_Parameters *pParameters);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_CMD__Module_Library_H
