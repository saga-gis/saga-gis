/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                  WKSP_Data_Control.h                  //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_base_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Control : public CWKSP_Base_Control
{
	DECLARE_CLASS(CWKSP_Data_Control)

public:
	CWKSP_Data_Control(wxWindow *pParent);
	virtual ~CWKSP_Data_Control(void);

	class CWKSP_Data_Manager *	Get_Manager			(void)	{	return( (class CWKSP_Data_Manager *)m_pManager );	}

	void						Add_Item			(class CWKSP_Base_Manager *pManager, class CWKSP_Base_Item *pItem);

	bool						Close				(bool bSilent);

	virtual int					Get_Selection_Count	(void);
	virtual CWKSP_Base_Item *	Get_Item_Selected	(bool bUpdate = false);
	virtual bool				Set_Item_Selected	(class CWKSP_Base_Item *pItem, bool bKeepMultipleSelection = false);


protected:

	bool						m_bUpdate_Selection;


	virtual bool				_Del_Active			(bool bSilent);

	int							_Get_Image_ID		(class CWKSP_Base_Item *pItem);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
extern CWKSP_Data_Control		*g_pData_Ctrl;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Control_H
