
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
//                   WKSP_Map_Layer.h                    //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_Layer_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_Layer_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_base_item.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Map_Layer : public CWKSP_Base_Item
{
public:
	CWKSP_Map_Layer(class CWKSP_Layer *pLayer);

	virtual TWKSP_Item			Get_Type			(void)	{	return( WKSP_ITEM_Map_Layer );	}

	virtual wxString			Get_Name			(void);
	virtual wxString			Get_Description		(void);

	virtual wxMenu *			Get_Menu			(void);

	virtual bool				On_Command			(int Cmd_ID);
	virtual bool				On_Command_UI		(wxUpdateUIEvent &event);

	virtual CSG_Parameters *	Get_Parameters		(void);
	virtual void				Parameters_Changed	(void);

	class CWKSP_Layer *			Get_Layer			(void)	{	return( m_pLayer );	}

	bool						Load_Settings		(CSG_MetaData *pEntry);
	bool						Save_Settings		(CSG_MetaData *pEntry);

	bool						do_Show				(void)	{	return( m_bShow    );	}
	bool						do_Project			(void)	{	return( m_bProject );	}

	bool						Fit_Colors			(const CSG_Rect &rWorld);

	CSG_Rect					Get_Extent			(void);

	bool						Draw				(CWKSP_Map_DC &dc_Map, int Flags);


private:

	bool						m_bShow, m_bProject, m_bResample, m_bFitColors;

	class CWKSP_Layer			*m_pLayer;


	bool						_Get_Projections	(CSG_Projection &prj_Layer, CSG_Projection &prj_Map);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_Layer_H
