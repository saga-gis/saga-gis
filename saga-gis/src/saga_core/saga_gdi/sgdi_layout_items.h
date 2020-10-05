
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  sgdi_layout_items.h                  //
//                                                       //
//                 Copyright (C) 2020 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_layout_items_H
#define HEADER_INCLUDED__SAGA_GDI_sgdi_layout_items_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>

//---------------------------------------------------------
#include "sgdi_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSGDI_Layout_Items
{
public:

	static bool						Compare				(const wxRect &r1, const wxRect &r2)
	{
		return( r1.x == r2.x && r1.y == r2.y && r1.width == r2.width && r1.height == r2.height );
	}

	static wxRect					Get_Scaled			(const wxRect &Rect, double Scale);


	//-----------------------------------------------------
	class SGDI_API_DLL_EXPORT CSGDI_Layout_Item
	{
		friend CSGDI_Layout_Items;

	public:
		CSGDI_Layout_Item(void);
		virtual ~CSGDI_Layout_Item(void);

		bool						is_Shown			(void)	const			{	return( m_bShow );	}

		bool						Set_Sizer			(bool bOn);
		bool						Set_Ratio			(double Ratio = 0.);
		bool						Fix_Ratio			(bool bOn = true);

		const wxRect &				Get_Rect			(void)	const			{	return( m_Rect );	}
		bool						Set_Rect			(const wxRect &Rect);

		bool						Refresh				(bool bErase = true);

		virtual int					Get_ID				(void)	const			{	return( 0      );	}

		virtual bool				Draw				(wxDC &dc)				{	return( true   );	}

		virtual bool				Properties			(wxWindow *pParent)		{	return( false  );	}


	protected:

		bool						m_bShow, m_bSizer;

		double						m_Ratio;

		wxRect						m_Rect;

		CSGDI_Layout_Items			*m_pOwner;


	private:

		wxRect						_Tracker_Get_Rect	(void);
		bool						_Tracker_Set_Rect	(const wxRect &Rect);
		bool						_Tracker_Contains	(const wxPoint &Point);

	};


//---------------------------------------------------------
public:
	CSGDI_Layout_Items(void);
	virtual ~CSGDI_Layout_Items(void);

	bool							Create				(wxWindow *pParent);
	bool							Destroy				(bool bDetachItems = false);

	bool							Set_Parent			(wxWindow *pParent);
	wxWindow *						Get_Parent			(void)			const	{	return( m_pParent );	}
	bool							Set_Raster			(int Raster);

	size_t							Get_Count			(void)			const	{	return( m_Items.Get_Size() );	}
	CSGDI_Layout_Item *				Get_Item			(size_t Index)	const	{	return( (CSGDI_Layout_Item *)m_Items[Index] );	}
	CSGDI_Layout_Item *				operator ()			(size_t Index)	const	{	return(  Get_Item(Index) );	}
	CSGDI_Layout_Item &				operator []			(size_t Index)			{	return( *Get_Item(Index) );	}

	bool							Add					(CSGDI_Layout_Item *pItem, const wxRect &Rect, bool bActivate = false);
	bool							Add					(CSGDI_Layout_Item *pItem, bool bActivate = false);
	bool							Del					(CSGDI_Layout_Item *pItem, bool bDetach = false);
	bool							Del					(size_t             iItem, bool bDetach = false);

	bool							Hide				(CSGDI_Layout_Item *pItem);
	bool							Show				(CSGDI_Layout_Item *pItem);

	bool							Scale				(double Scale);

	size_t							Get_Position		(CSGDI_Layout_Item *pItem)	const;
	bool							is_Top				(CSGDI_Layout_Item *pItem)	const;
	bool							is_Bottom			(CSGDI_Layout_Item *pItem)	const;
	bool							Move_Top			(CSGDI_Layout_Item *pItem);
	bool							Move_Bottom			(CSGDI_Layout_Item *pItem);
	bool							Move_Up				(CSGDI_Layout_Item *pItem);
	bool							Move_Down			(CSGDI_Layout_Item *pItem);

	CSGDI_Layout_Item *				Get_Active			(void)	const	{	return( m_pActive );	}
	size_t							Active_Get_Position	(void)	const;
	bool							Active_is_Top		(void)	const;
	bool							Active_is_Bottom	(void)	const;
	bool							Active_Move_Top		(void);
	bool							Active_Move_Bottom	(void);
	bool							Active_Move_Up		(void);
	bool							Active_Move_Down	(void);
	bool							Active_Properties	(void);

	bool							On_Key_Event		(wxKeyEvent   &event);
	bool							On_Mouse_Event		(wxMouseEvent &event);

	bool							Draw				(wxDC &dc, bool bTracker);


protected:

	int								m_Handle, m_Raster, m_MinSize;

	double							m_Scale;

	wxWindow						*m_pParent;

	CSGDI_Layout_Item				*m_pActive;

	CSG_Array_Pointer				m_Items;


	bool							Select				(const wxPoint &Point);


//---------------------------------------------------------
private:
	typedef enum EHandle
	{
		HANDLE_TOP_LEFT		= 0,
		HANDLE_TOP_CENTER,
		HANDLE_TOP_RIGHT,
		HANDLE_LEFT_CENTER,
		HANDLE_RIGHT_CENTER,
		HANDLE_BOTTOM_LEFT,
		HANDLE_BOTTOM_CENTER,
		HANDLE_BOTTOM_RIGHT,
		HANDLE_TRACKER,
		HANDLE_NONE
	}
	THandle;

	class CSGDI_Layout_Tracker
	{
	public:
		CSGDI_Layout_Tracker(void);
		virtual ~CSGDI_Layout_Tracker(void);

		wxRect						Get_Rect			(THandle Handle);

		THandle						Hit_Test			(const wxPoint &Point);

		bool						Drag_Start			(const wxPoint &Point);
		bool						Drag_Move			(const wxPoint &Point);
		bool						Drag_Stop			(const wxPoint &Point);
		wxRect						Drag_Rect			(const wxPoint &From, const wxPoint &To);
		void						Drag_Draw			(const wxRect &Rect);

		bool						Draw				(          bool bWhite = false);
		bool						Draw				(wxDC &dc, bool bWhite = false);


		THandle						m_Drag_Mode;

		wxPoint						m_Drag_Start, m_Drag_Last;

		CSGDI_Layout_Items			*m_pOwner;

	};


	CSGDI_Layout_Tracker			m_Tracker;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_layout_items_H
