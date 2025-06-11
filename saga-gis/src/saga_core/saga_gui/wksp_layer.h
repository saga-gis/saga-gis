
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
//                     WKSP_Layer.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_data_item.h"

#include "wksp_layer_classify.h"

#include <saga_gdi/map_dc.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define LAYER_DRAW_FLAG_THUMBNAIL 0x01
#define LAYER_DRAW_FLAG_SELECTION 0x02
#define LAYER_DRAW_FLAG_HIGHLIGHT 0x04
#define LAYER_DRAW_FLAG_NOEDITS   0x08
#define LAYER_DRAW_FLAG_NOLABELS  0x10


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Layer : public CWKSP_Data_Item
{
public:
	CWKSP_Layer(CSG_Data_Object *pObject);
	virtual ~CWKSP_Layer(void);

	virtual bool					On_Command				(int Cmd_ID);
	virtual bool					On_Command_UI			(wxUpdateUIEvent &event);

	CSG_Data_Object *				Get_Object				(void)	{	return( m_pObject );	}
	CSG_Rect						Get_Extent				(void);

	CSG_Colors *					Get_Colors				(void);
	bool							Get_Colors				(CSG_Colors *pColors);
	bool							Set_Colors				(CSG_Colors *pColors);

	virtual wxString				Get_Value				(CSG_Point ptWorld, double Epsilon)	= 0;

	wxString						Get_Field_Naming		(void) const;
	bool							Get_Field_Value			(sLong Index, int Field, int Normalize, double Scale, wxString &Value) const;
	bool							Get_Field_Value			(sLong Index, int Field, int Normalize, double Scale, double   &Value) const;
	
	double							Get_Stretch_Minimum		(void) const;
	double							Get_Stretch_Maximum		(void) const;
	double							Get_Stretch_Range		(void) const;
	bool							Set_Stretch_Range		(double Minimum, double Maximum);
	bool							Set_Stretch_FullRange	(void);

	bool							Classify				(const CSG_MetaData &Options);

	bool							Draw					(CSG_Map_DC &dc_Map, int Flags = 0, CSG_Data_Object *pObject = NULL);

	class CWKSP_Layer_Classify *	Get_Classifier			(void)	{	return( m_pClassify );	}

	class CWKSP_Layer_Legend *		Get_Legend				(void)	{	return( m_pLegend );	}
	bool							do_Legend				(void);
	bool							do_Show					(CSG_Rect const &Map_Extent, bool bIntersects = true);

	virtual bool					Show					(class CWKSP_Map *pMap);
	virtual bool					Show					(int Flags = 0);
	virtual bool					View_Closes				(class MDI_ChildFrame *pView);
	virtual bool					asImage					(CSG_Grid *pImage)		{	return( false );	}


	const wxBitmap &				Get_Thumbnail			(int dx, int dy);

	void							Histogram_Show			(bool bShow);
	void							Histogram_Toggle		(void);

	virtual wxMenu *				Edit_Get_Menu			(void);
	virtual TSG_Rect				Edit_Get_Extent			(void)	= 0;
	virtual bool					Edit_On_Key_Down		(int KeyCode);
	virtual bool					Edit_On_Mouse_Down		(const CSG_Point &Point, double ClientToWorld, int Key);
	virtual bool					Edit_On_Mouse_Up		(const CSG_Point &Point, double ClientToWorld, int Key);
	virtual bool					Edit_On_Mouse_Move		(wxWindow *pMap, const CSG_Rect &rWorld, const wxPoint &Point, const wxPoint &Last, int Key);
	virtual bool					Edit_On_Mouse_Move_Draw	(wxDC &dc      , const CSG_Rect &rWorld, const wxPoint &Point);
	virtual bool					Edit_Do_Mouse_Move_Draw	(bool bMouseDown);
	virtual bool					Edit_Set_Index			(int Index);
	virtual bool					Edit_Set_Attributes		(void)	{	return( false );	}
	virtual int						Edit_Get_Index			(void)	{	return( 0     );	}
	CSG_Table *						Edit_Get_Attributes		(void);


protected:

	CSG_Point						m_Edit_Mouse_Down;

	CSG_Table						m_Edit_Attributes, m_Normalization;

	class CWKSP_Layer_Classify		*m_pClassify;

	class CWKSP_Layer_Legend		*m_pLegend;

	class CVIEW_Histogram			*m_pHistogram;

	wxBitmap						m_Thumbnail;


	static bool						Add_ToolBar_Defaults	(class wxToolBarBase *pToolBar);

	virtual void					On_Create_Parameters	(void);
	virtual void					On_DataObject_Changed	(void);
	virtual void					On_Parameters_Changed	(void);

	virtual int						On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual void					On_Update_Views			(bool bAll);
	virtual void					On_Update_Views			(void)			{}

	virtual void					On_Draw					(CSG_Map_DC &dc_Map, int Flags)	= 0;

	bool							Set_Stretch				(CSG_Parameters &Parameters, CSG_Data_Object *pObject = NULL, const CSG_String &Suffix = "");

	bool							Set_Normalization		(int Field_Value, int Field_Normalize, double Scale_Normalize, sLong maxSamples = 0);

	bool							Set_Fields_Choice		(CSG_Parameter *pChoice, bool bNumeric, bool bAddNone, bool bSelectNone = false);
	static int						Get_Fields_Choice		(CSG_Parameter *pChoice);


private:

	bool							_Set_Thumbnail			(bool bRefresh);

	void							_Set_Projection			(void);

	CSG_Table *						_Get_Field_Table		(int &Field, const CSG_Parameters &Parameters);


	//-----------------------------------------------------
	CSG_Parameters					m_Classify;

	static int						_Classify_Callback		(CSG_Parameter *pParameter, int Flags);

	bool							_Classify				(void);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer *						Get_Active_Layer		(void);


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_H
