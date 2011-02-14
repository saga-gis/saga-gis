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
//                  WKSP_Map_Manager.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_Manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Map_Manager : public CWKSP_Base_Manager
{
public:
	CWKSP_Map_Manager(void);
	virtual ~CWKSP_Map_Manager(void);

	virtual TWKSP_Item			Get_Type		(void)		{	return( WKSP_ITEM_Map_Manager );	}

	virtual wxString			Get_Name		(void);
	virtual wxString			Get_Description	(void);

	virtual wxMenu *			Get_Menu		(void);

	virtual CSG_Parameters *	Get_Parameters	(void)		{	return( &m_Parameters );	}
	virtual bool				On_Command		(int Cmd_ID);

	class CWKSP_Map *			Get_Map			(int i)		{	return( (class CWKSP_Map *)Get_Item(i) );	}

	bool						Exists			(class CWKSP_Map *pMap);

	bool						Close			(bool bSilent);

	bool						Add				(class CWKSP_Layer *pLayer);
	bool						Add				(class CWKSP_Layer *pLayer, class CWKSP_Map *pMap);
	bool						Del				(class CWKSP_Layer *pLayer);
	bool						Update			(class CWKSP_Layer *pLayer, bool bMapsOnly);


private:

	CSG_Parameters				m_Parameters;

};

//---------------------------------------------------------
extern CWKSP_Map_Manager		*g_pMaps;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_Manager_H
