/**********************************************************
 * Version $Id: wksp_data_item.h 1246 2011-11-25 13:42:38Z oconrad $
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
//                   wksp_data_item.h                    //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__wksp_data_item_H
#define _HEADER_INCLUDED__SAGA_GUI__wksp_data_item_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include <wx/dynarray.h>

#include "wksp_base_item.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Item : public CWKSP_Base_Item
{
public:
	CWKSP_Data_Item(CSG_Data_Object *pObject);
	virtual ~CWKSP_Data_Item(void);

	virtual bool					On_Command				(int Cmd_ID);
	virtual bool					On_Command_UI			(wxUpdateUIEvent &event);

	virtual wxString				Get_Name				(void);

	CSG_Data_Object *				Get_Object				(void)	{	return( m_pObject );	}

	virtual CSG_Parameters *		Get_Parameters			(void)	{	return( &m_Parameters );	}

	virtual void					Parameters_Changed		(void);
	bool							DataObject_Changed		(void);
	bool							DataObject_Changed		(CSG_Parameters *pParameters);

	bool							Save					(void);
	bool							Save					(const wxString &File_Name);

	virtual bool					Show					(int Flags = 0)	= 0;
	virtual bool					Update					(CWKSP_Data_Item *pChanged)		{	return( pChanged == this );	}

	virtual bool					View_Opened				(class MDI_ChildFrame *pView);
	virtual bool					View_Closes				(class MDI_ChildFrame *pView);

	virtual bool					Update_Views			(bool bAll = true);


protected:

	CSG_Data_Object					*m_pObject;


	virtual void					On_Create_Parameters	(void);
	virtual void					On_Parameters_Changed	(void);
	virtual void					On_DataObject_Changed	(void);
	virtual bool					On_Data_Deletion		(CSG_Data_Object *pObject);
	virtual void					On_Update_Views			(bool bAll)	{}

	virtual bool					Fit_Colors				(void)	{	return( true );	}

	bool							Add_ScatterPlot			(void);


private:

	bool							m_bUpdating;

	wxArrayPtrVoid					m_Views;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__wksp_data_item_H
