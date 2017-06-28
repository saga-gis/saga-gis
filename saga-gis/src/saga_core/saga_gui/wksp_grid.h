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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_layer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Grid : public CWKSP_Layer
{
public:
	CWKSP_Grid(CSG_Grid *pGrid);

	virtual TWKSP_Item			Get_Type				(void)	{	return( WKSP_ITEM_Grid );	}

	CSG_Grid *					Get_Grid				(void)	{	return( (CSG_Grid *)m_pObject );	}

	virtual wxString			Get_Description			(void);

	virtual wxMenu *			Get_Menu				(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	virtual wxString			Get_Value				(CSG_Point ptWorld, double Epsilon);
	virtual double				Get_Value_Minimum		(void);
	virtual double				Get_Value_Maximum		(void);
	virtual double				Get_Value_Range			(void);
	virtual double				Get_Value_Mean			(void);
	virtual double				Get_Value_StdDev		(void);

	bool						Fit_Colors				(const CSG_Rect &rWorld);

	virtual bool				asImage					(CSG_Grid *pImage);

	virtual bool				Update					(CWKSP_Layer *pChanged);

	virtual wxMenu *			Edit_Get_Menu			(void);
	virtual TSG_Rect			Edit_Get_Extent			(void);
	virtual bool				Edit_On_Key_Down		(int KeyCode);
	virtual bool				Edit_On_Mouse_Up		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				Edit_Set_Attributes		(void);


protected:

	bool						Get_Image_Grid			(wxBitmap &BMP, bool bFitSize = true);
	bool						Get_Image_Legend		(wxBitmap &BMP, double Zoom);

	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);
	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual bool				Fit_Colors				(void);

	virtual void				On_Draw					(CWKSP_Map_DC &dc_Map, int Flags);


private:

	int							m_Shade_Mode, m_xSel, m_ySel;

	double						m_Shade_Parms[6];


	bool						_Fit_Colors				(CSG_Parameters &Parameters, double &Minimum, double &Maximum);

	void						_Set_Shading			(double Shade, int &Color);
	int							_Get_Shading			(double x, double y, int Color, TSG_Grid_Resampling Resampling);
	int							_Get_Shading			(int    x, int    y, int Color);

	void						_LUT_Create				(void);

	bool						_Edit_Clr_Selection		(void);
	bool						_Edit_Del_Selection		(void);

	bool						_Save_Image				(void);
	bool						_Save_Image_Clipboard	(void);

	void						_Draw_Grid_Points		(CWKSP_Map_DC &dc_Map, TSG_Grid_Resampling Resampling);
	void						_Draw_Grid_Line			(CWKSP_Map_DC &dc_Map, TSG_Grid_Resampling Resampling, int Mode, CWKSP_Grid *pOverlay[2], int yDC, int axDC, int bxDC);
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
