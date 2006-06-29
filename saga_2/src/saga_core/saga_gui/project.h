
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Project.h                        //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__Project_H
#define _HEADER_INCLUDED__SAGA_GUI__Project_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Project
{
public:
	CWKSP_Project(void);
	virtual ~CWKSP_Project(void);

	const char *			Get_File_Name		(void)	{	return( m_File_Name );	}
	bool					Has_File_Name		(void);

	bool					Load				(bool bAdd);
	bool					Load				(const char *FileName, bool bAdd, bool bUpdateMenu);

	bool					Save				(void);
	bool					Save				(bool bSaveAsOnError);
	bool					Save				(const char *FileName, bool bSaveModified);

	bool					Save_Modified		(class CWKSP_Base_Item *pItem);


private:

	wxString				m_File_Name;

	bool					_Load				(const char *FileName, bool bAdd, bool bUpdateMenu);
	bool					_Save				(const char *FileName, bool bSaveModified, bool bUpdateMenu);

	bool					_Load_Data			(FILE *Stream, const char *ProjectDir);
	bool					_Save_Data			(FILE *Stream, const char *ProjectDir, class CDataObject *pDataObject, class CParameters *pParameters);

	bool					_Load_Map			(FILE *Stream, const char *ProjectDir);
	bool					_Save_Map			(FILE *Stream, const char *ProjectDir, class CWKSP_Map *pMap);

	class CWKSP_Base_Item *	_Get_byFileName		(wxString FileName);

	bool					_Modified_Get		(class CParameters *pParameters, class CWKSP_Base_Item *pItem);
	bool					_Modified_Get		(class CParameters *pParameters, class CWKSP_Base_Item *pItem, class CDataObject *pObject);
	bool					_Modified_Save		(class CParameters *pParameters);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Project_H
