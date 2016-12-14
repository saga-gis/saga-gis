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
//                     WKSP_Map.h                        //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_H


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
class CWKSP_Map_Extents : public CSG_Rects
{
public:
	CWKSP_Map_Extents(void);

	bool						is_First				(void)		{	return( m_iExtent <= 0 );				}
	bool						is_Last					(void)		{	return( m_iExtent >= m_nExtents - 1 );	}

	CSG_Rect					Set_Back				(void);
	CSG_Rect					Set_Forward				(void);

	bool						Add_Extent				(const CSG_Rect &Extent, bool bReset = false);
	const CSG_Rect &			Get_Extent				(void)		{	return( m_iExtent >= 0 ? Get_Rect(m_iExtent) : m_Dummy );	}


private:

	int							m_iExtent, m_nExtents;

	static CSG_Rect				m_Dummy;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Map : public CWKSP_Base_Manager
{
public:
	CWKSP_Map(void);
	virtual ~CWKSP_Map(void);

	virtual TWKSP_Item			Get_Type				(void)		{	return( WKSP_ITEM_Map );	}

	virtual wxString			Get_Name				(void);
	virtual wxString			Get_Description			(void);

	virtual wxMenu *			Get_Menu(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	virtual void				Parameters_Changed		(void);

	const CSG_Rect &			Get_Extent				(void)		{	return( m_Extents.Get_Extent() );	}
	void						Set_Extent				(const CSG_Rect &Extent, bool bReset = false);
	void						Set_Extent				(void);
	void						Set_Extent_Full			(void);
	void						Set_Extent_Active		(void);
	void						Set_Extent_Selection	(void);
	bool						Set_Extent_Back			(bool bCheck_Only = false);
	bool						Set_Extent_Forward		(bool bCheck_Only = false);

	bool						is_North_Arrow			(void);
	void						Set_North_Arrow			(bool bOn);

	bool						is_ScaleBar				(void);
	void						Set_ScaleBar			(bool bOn);

	bool						is_Synchronising		(void);
	void						Set_Synchronising		(bool bOn);

	bool						is_Image_Save_Mode		(void)	{	return( m_Img_bSave );		}

	bool						Update					(class CWKSP_Layer *pLayer, bool bMapOnly);
	int							Get_Layer				(class CWKSP_Layer *pLayer);
	class CWKSP_Map_Layer *		Find_Layer				(class CWKSP_Layer *pLayer);
	class CWKSP_Map_Layer *		Add_Layer				(class CWKSP_Layer *pLayer);
	class CWKSP_Map_Graticule *	Add_Graticule			(CSG_MetaData *pEntry = NULL);
	class CWKSP_Map_BaseMap *	Add_BaseMap				(CSG_MetaData *pEntry = NULL);
	class CWKSP_Base_Item *		Add_Copy				(CWKSP_Base_Item *pItem);

    bool                        View_Opened             (class MDI_ChildFrame *pView);
	void						View_Closes				(class MDI_ChildFrame *pView);
	void						View_Refresh			(bool bMapOnly);
	class CVIEW_Map *			View_Get				(void)		{	return( m_pView );		}
	void						View_Show				(bool bShow);
	void						View_Toggle				(void);
	class CVIEW_Map_3D *		View_3D_Get				(void)		{	return( m_pView_3D );	}
	void						View_3D_Show			(bool bShow);
	void						View_3D_Toggle			(void);
	class CVIEW_Layout *		View_Layout_Get			(void)		{	return( m_pLayout );	}
	void						View_Layout_Show		(bool bShow);
	void						View_Layout_Toggle		(void);

	CSG_Rect					Get_World				(wxRect rClient);
	CSG_Point					Get_World				(wxRect rClient, wxPoint ptClient);

	bool						Get_Image				(wxImage &Image, CSG_Rect &rWorld);
	void						SaveAs_Image			(void);
	void						SaveAs_Image_Clipboard	(int nx, int ny, int frame);
	void						SaveAs_Image_Clipboard	(bool bLegend = false);
	void						SaveAs_Image_To_KMZ		(int nx, int ny);
	void						SaveAs_Image_To_Memory	(int nx, int ny);
	void						SaveAs_Image_On_Change	(void);
	void						SaveAs_Image_To_Grid	(CSG_Grid &Grid, int Size);

	void						Draw_Map				(wxDC &dc                        , double Zoom, const wxRect &rClient, int Flags = 0, int Background = -1);
	void						Draw_Map				(wxDC &dc, const CSG_Rect &rWorld, double Zoom, const wxRect &rClient, int Flags = 0, int Background = -1);
	void						Draw_Frame				(wxDC &dc, wxRect rMap, int Width);
	void						Draw_Frame				(wxDC &dc, const CSG_Rect &rWorld, wxRect rMap, int Width);
	bool						Draw_Legend				(wxDC &dc, double Zoom_Map, double Zoom, wxPoint Position, wxSize *pSize = NULL);

	bool						Draw_North_Arrow		(class CWKSP_Map_DC &dc_Map, const CSG_Rect &rWorld, const wxRect &rClient);
	bool						Draw_ScaleBar			(class CWKSP_Map_DC &dc_Map, const CSG_Rect &rWorld, const wxRect &rClient);
	bool						Draw_Extent				(class CWKSP_Map_DC &dc_Map, const CSG_Rect &rWorld, const wxRect &rClient);

	const wxBitmap &			Get_Thumbnail			(int dx, int dy);

	bool						Get_Legend_Size			(wxSize &Size, double Zoom_Map = 1.0, double Zoom = 1.0);

	int							Get_Frame_Width			(void);
	int							Get_Print_Resolution	(void);
	int							Get_Print_Frame			(void);
	int							Get_Print_Legend		(void);

	void						Set_Projection			(void);
	CSG_Projection &			Get_Projection			(void)	{	return( m_Projection );	}


protected:

	virtual void				On_Create_Parameters	(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);


private:

	bool						m_Img_bSave;

	int							m_Img_Type, m_Img_Count;

	wxString					m_Name, m_Img_File;

	wxBitmap					m_Thumbnail;

	CSG_Parameters				m_Img_Parms;

	CSG_Projection				m_Projection;

	CWKSP_Map_Extents			m_Extents;

	class CVIEW_Map				*m_pView;

	class CVIEW_Map_3D			*m_pView_3D;

	class CVIEW_Layout			*m_pLayout;

	class CVIEW_Layout_Info		*m_pLayout_Info;


	void						_Set_Extent				(const CSG_Rect &Extent);
	void						_Synchronise_Extents	(void);

	void						_Img_Save				(wxString file, int type);
	void						_Img_Save_On_Change		(void);

	bool						_Set_Thumbnail			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_H
