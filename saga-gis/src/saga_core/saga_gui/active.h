
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
//                      ACTIVE.h                         //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_H
#define _HEADER_INCLUDED__SAGA_GUI__ACTIVE_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/notebook.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CActive : public wxNotebook
{
public:
	CActive(wxWindow *pParent);
	virtual ~CActive(void);

	void							Add_Pages				(void);

	class CActive_Parameters *		Get_Parameters			(void)	{	return( m_pParameters  );	}
	class CActive_Description *		Get_Description			(void)	{	return( m_pDescription );	}
	class CActive_History *			Get_History				(void)	{	return( m_pHistory     );	}
	class CActive_Legend *			Get_Legend				(void)	{	return( m_pLegend      );	}
	class CActive_Attributes *		Get_Attributes			(void)	{	return( m_pAttributes  );	}
	class CActive_Info *			Get_Info				(void)	{	return( m_pInfo        );	}

	bool							Set_Active				(class CWKSP_Base_Item *pItem);
	class CWKSP_Base_Item *			Get_Active				(void)	{	return( m_pItem );			}
	class CWKSP_Data_Item *			Get_Active_Data_Item	(void);
	class CWKSP_Layer *				Get_Active_Layer		(void);
	class CWKSP_Shapes *			Get_Active_Shapes		(bool bWithInfo = false);
	class CWKSP_Map *				Get_Active_Map			(void);

	bool							Update					(class CWKSP_Base_Item *pItem, bool bSave);
	bool							Update_DataObjects		(void);
	bool							Update_Description		(void);
	bool							Update_Attributes		(bool bSave = false);
	bool							Update_Info				(void);


private:

	class CActive_Parameters		*m_pParameters;

	class CActive_Description		*m_pDescription;

	class CActive_History			*m_pHistory;

	class CActive_Legend			*m_pLegend;

	class CActive_Attributes		*m_pAttributes;

	class CActive_Info				*m_pInfo;

	class CWKSP_Base_Item			*m_pItem;


	bool							_Show_Page				(wxWindow *pPage, bool bShow);
	bool							_Show_Page				(wxWindow *pPage);
	bool							_Hide_Page				(wxWindow *pPage);


	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
extern CActive						*g_pActive;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_H
