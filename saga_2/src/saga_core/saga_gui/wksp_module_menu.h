
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
//                  WKSP_Module_Menu.h                   //
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
// $Id: wksp_module_menu.h,v 1.3 2007-07-06 10:07:54 tschorr Exp $

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Menu_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Menu_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Module_Menu
{
public:
	CWKSP_Module_Menu(void);
	virtual ~CWKSP_Module_Menu(void);

	void						Destroy				(void);

	void						Add					(class wxMenu *pMenu);
	void						Del					(class wxMenu *pMenu);

	void						Update				(void);

	void						Set_Recent			(class CWKSP_Module *pModule);

	int							Get_ID_Translated	(int ID);


private:

	int							m_nMenus;

	class wxMenu				**m_Menus;

	class CWKSP_Module			**m_Recent;


	void						_Update				(class wxMenu *pMenu);
	class wxMenu *				_Get_SubMenu		(class wxMenu *pMenu, wxString Menu_Path);
	class wxMenu* 				_Find_SubMenu_For_Token( wxMenu* menu, wxString token );
	void						_Set_Recent			(class wxMenu *pMenu);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Menu_H
