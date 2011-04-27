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
//                     WKSP_Grid.h                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_layer.h"

class CSVG_Interactive_Map;

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Grid : public CWKSP_Layer
{
	friend class CSVG_Interactive_Map;

public:
	CWKSP_Grid(CSG_Grid *pGrid);
	virtual ~CWKSP_Grid(void);

	virtual TWKSP_Item			Get_Type				(void)	{	return( WKSP_ITEM_Grid );	}

	virtual wxString			Get_Description			(void);

	virtual wxMenu *			Get_Menu				(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	virtual wxString			Get_Value				(CSG_Point ptWorld, double Epsilon);
	virtual double				Get_Value_Range			(void);

	CSG_Grid *					Get_Grid				(void)	{	return( m_pGrid );	}

	bool						Fit_Color_Range			(CSG_Rect rWorld);

	bool						asImage					(CSG_Grid *pImage);

	virtual bool				Update					(CWKSP_Layer *pChanged)	{	return( pChanged == this || pChanged == m_pOverlay[0] || pChanged == m_pOverlay[1] );	}


protected:

	bool						Get_Image_Grid			(wxBitmap &BMP, bool bFitSize = true);
	bool						Get_Image_Legend		(wxBitmap &BMP, double Zoom);

	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);
	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual bool				On_Edit_On_Key_Down		(int KeyCode);
	virtual bool				On_Edit_On_Mouse_Up		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				On_Edit_Set_Attributes	(void);
	virtual TSG_Rect			On_Edit_Get_Extent		(void);

	virtual void				On_Draw					(CWKSP_Map_DC &dc_Map, bool bEdit);


private:

	bool						m_bOverlay;

	int							m_Sel_xOff, m_Sel_xN, m_Sel_yOff, m_Sel_yN;

	CSG_Grid					*m_pGrid;
	
	CWKSP_Grid					*m_pOverlay[2];


	void						_LUT_Create				(void);

	bool						_Edit_Del_Selection		(void);

	void						_Save_Image				(void);

	void						_Draw_Grid_Points		(CWKSP_Map_DC &dc_Map, int Interpolation);
	void						_Draw_Grid_Cells		(CWKSP_Map_DC &dc_Map);

	void						_Draw_Values			(CWKSP_Map_DC &dc_Map);
	void						_Draw_Edit				(CWKSP_Map_DC &dc_Map);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_H
