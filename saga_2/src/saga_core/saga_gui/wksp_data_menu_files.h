
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
//                WKSP_Data_Menu_Files.h                 //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Menu_Files_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Menu_Files_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Menu_Files
{
public:
	CWKSP_Data_Menu_Files(void);
	virtual ~CWKSP_Data_Menu_Files(void);

	void							Add				(class wxMenu *pMenu);
	void							Del				(class wxMenu *pMenu);

	void							Set_Update		(bool bUpdate)	{	m_bUpdate	= bUpdate;	}

	bool							Recent_Open		(int Cmd_ID);
	void							Recent_Add		(int DataType, const wxChar *FileName);
	void							Recent_Del		(int DataType, const wxChar *FileName);


private:

	bool							m_bUpdate;

	int								m_nMenus;

	class wxMenu					**m_Menus;

	class CWKSP_Data_Menu_File		*m_pFMProjects, *m_pFMTables, *m_pFMShapes, *m_pFMTINs, *m_pFMGrids;


	class CWKSP_Data_Menu_File *	_Get_Menu		(int DataType);

	void							_Update			(void);
	void							_Update			(class wxMenu *pMenu);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Menu_Files_H
