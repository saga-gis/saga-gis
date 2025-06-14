
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
//                     WKSP_TIN.h                        //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_TIN_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_TIN_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_layer.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_TIN : public CWKSP_Layer
{
public:
	CWKSP_TIN(CSG_TIN *pTIN);
	virtual ~CWKSP_TIN(void);

	virtual TWKSP_Item			Get_Type				(void)	{	return( WKSP_ITEM_TIN );	}

	CSG_TIN *					Get_TIN					(void)	{	return( (CSG_TIN *)m_pObject );	}
	class CWKSP_Table *			Get_Table				(void)	{	return( m_pTable );	}

	virtual wxString			Get_Description			(void);

	virtual wxToolBarBase *		Get_ToolBar				(void);
	virtual wxMenu *			Get_Menu				(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	virtual wxString			Get_Value				(CSG_Point ptWorld, double Epsilon);

	int							Get_Field_Value			(void) { return( m_Stretch.Value  ); }
	int							Get_Field_Normal		(void) { return( m_Stretch.Normal ); }
	double						Get_Scale_Normal		(void) { return( m_Stretch.Scale  ); }

	virtual bool				asImage					(CSG_Grid *pImage);

	virtual TSG_Rect			Edit_Get_Extent			(void);
	virtual bool				Edit_On_Mouse_Up		(const CSG_Point &Point, double ClientToWorld, int Key);
	virtual bool				Edit_Set_Attributes		(void);


protected:

	typedef struct { int x, y; double z; } TPoint;


protected:

	class CWKSP_Table			*m_pTable;

	class CStretch				{ public: int Value = -1, Normal = -1; double Scale = 1.; } m_Stretch;


	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);
	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual void				On_Draw					(CSG_Map_DC &dc_Map, int Flags);


private:

	void						_Draw_Points			(CSG_Map_DC &dc_Map);
	void						_Draw_Edges				(CSG_Map_DC &dc_Map);
	void						_Draw_Triangles			(CSG_Map_DC &dc_Map);
	void						_Draw_Triangle			(CSG_Map_DC &dc_Map, TPoint p[3]);
	void						_Draw_Triangle_Line		(CSG_Map_DC &dc_Map, int xa, int xb, int y, double za, double zb);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_TIN_H
