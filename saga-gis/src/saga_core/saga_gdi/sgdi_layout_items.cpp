
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
//                 sgdi_layout_items.cpp                 //
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
#include "sgdi_layout_items.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Item::CSGDI_Layout_Item(void)
{
	m_pOwner	= NULL;
	m_bShow		= true;
	m_bSizer	= true;
	m_Ratio		= 0.;
}

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Item::~CSGDI_Layout_Item(void)
{}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Sizer(bool bOn)
{
	if( m_bSizer != bOn )
	{
		m_bSizer	= bOn;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Ratio(double Ratio)
{
	if( m_Ratio != Ratio )
	{
		m_Ratio	= Ratio;

		return( true );
	}

	return( false );
}

bool CSGDI_Layout_Items::CSGDI_Layout_Item::Fix_Ratio(bool bOn)
{
	if( bOn )
	{
		return( m_Rect.width && m_Rect.height && Set_Ratio(m_Rect.height / (double)m_Rect.width) );
	}

	return( Set_Ratio(0.) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Rect(const wxRect &Rect)
{
	if( !Rect.IsEmpty() && !Compare(Rect, m_Rect) )
	{
		m_Rect	= Rect;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Refresh(bool bErase)
{
	if( m_pOwner && m_pOwner->m_pParent )
	{
		wxRect	rTracker(Get_Scaled(m_Rect, m_pOwner->m_Scale));

		rTracker.Inflate(5);

		if( wxDynamicCast(m_pOwner->m_pParent, wxScrolledWindow) )
		{
			wxPoint	Offset((wxDynamicCast(m_pOwner->m_pParent, wxScrolledWindow)->CalcUnscrolledPosition(wxPoint(0, 0))));

			rTracker.x	-= Offset.x;
			rTracker.y	-= Offset.y;
		}

		m_pOwner->m_pParent->Refresh(bErase, &rTracker);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Contains(const wxPoint &Point)
{
	return( Get_Scaled(m_Rect, m_pOwner->m_Scale).Contains(Point) );
}

//---------------------------------------------------------
wxRect CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Get_Rect(void)
{
	return( Get_Scaled(m_Rect, m_pOwner ? m_pOwner->m_Scale : 1.) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Set_Rect(const wxRect &newRect)
{
	if( m_pOwner && m_pOwner->m_Scale > 0. )
	{
		wxRect	Rect(Get_Scaled(newRect, 1. / m_pOwner->m_Scale));

		if( !Rect.IsEmpty() )
		{
			if( m_pOwner->m_Raster > 1 )
			{
				if( Rect.x != m_Rect.x )
				{
					Rect.x	= m_pOwner->m_Raster * (int)(0.5 + Rect.x / (double)m_pOwner->m_Raster);
				}
				else if( Rect.width != m_Rect.width )
				{
					int	w = m_pOwner->m_Raster * (int)(0.5 + (Rect.x + Rect.width) / (double)m_pOwner->m_Raster);

					Rect.width  = w - Rect.x;
				}

				if( Rect.y != m_Rect.y )
				{
					Rect.y	= m_pOwner->m_Raster * (int)(0.5 + Rect.y / (double)m_pOwner->m_Raster);
				}
				else if( Rect.height != m_Rect.height )
				{
					int	h = m_pOwner->m_Raster * (int)(0.5 + (Rect.y + Rect.height) / (double)m_pOwner->m_Raster);

					Rect.height  = h - Rect.y;
				}
			}

			if( m_Ratio > 0. )
			{
			}

			if( !Rect.IsEmpty() && !Compare(Rect, m_Rect) )
			{
				Refresh(true);

				m_Rect	= Rect;

				Refresh(false);

				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Items(void)
{
	m_pParent	= NULL;
	m_pActive	= NULL;

	m_Scale		= 1.;
	m_Raster	= 0;
	m_Handle	= 7;

	m_Tracker.m_pOwner	= this;
}

//---------------------------------------------------------
CSGDI_Layout_Items::~CSGDI_Layout_Items(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Create(wxWindow *pParent)
{
	Destroy();

	m_pParent	= pParent;

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Destroy(bool bDetachItems)
{
	m_pActive	= NULL;

	if( bDetachItems == false )
	{
		for(size_t i=0; i<m_Items.Get_Size(); i++)
		{
			delete(Get_Item(i));
		}
	}

	m_Items.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxRect CSGDI_Layout_Items::Get_Scaled(const wxRect &Rect, double Scale)
{
	return( Scale == 1. ? Rect : wxRect(
		(int)(0.5 + Scale * Rect.x     ),
		(int)(0.5 + Scale * Rect.y     ),
		(int)(0.5 + Scale * Rect.width ),
		(int)(0.5 + Scale * Rect.height))
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Set_Parent(wxWindow *pParent)
{
	if( m_pParent != pParent )
	{
		m_pParent	= pParent;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Set_Raster(int Raster)
{
	if( Raster >= 0 )
	{
		m_Raster	= Raster;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Add(CSGDI_Layout_Item *pItem, const wxRect &Rect, bool bActivate)
{
	if( pItem )
	{
		pItem->m_pOwner = this;
		pItem->m_Rect   = Rect;

		m_Items.Add(pItem);

		if( bActivate )
		{
			if( m_pActive )
			{
				m_pActive->Refresh();
			}

			m_pActive	= pItem;
		}

		pItem->Refresh(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Add(CSGDI_Layout_Item *pItem, bool bActivate)
{
	return( pItem && Add(pItem, pItem->m_Rect, bActivate) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Del(CSGDI_Layout_Item *pItem, bool bDetachOnly)
{
	for(size_t i=0; i<m_Items.Get_Size(); i++)
	{
		if( pItem == m_Items[i] )
		{
			return( Del(i, bDetachOnly) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Del(size_t Index, bool bDetachOnly)
{
	if( Index < m_Items.Get_Size() )
	{
		CSGDI_Layout_Item	*pItem	= Get_Item(Index);

		if( m_pActive == pItem )
		{
			m_pActive	= NULL;
		}

		m_Items.Del(Index);

		pItem->Refresh();

		pItem->m_pOwner	= NULL;

		if( !bDetachOnly )
		{
			delete(pItem);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Scale(double Scale)
{
	if( Scale > 0. && Scale != m_Scale )
	{
		m_Scale	= Scale;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSGDI_Layout_Items::Get_Position(CSGDI_Layout_Item *pItem)	const
{
	if( pItem )
	{
		for(size_t i=0; i<m_Items.Get_Size(); i++)
		{
			if( pItem == m_Items[i] )
			{
				return( i );
			}
		}
	}

	return( m_Items.Get_Size() );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::is_Top(CSGDI_Layout_Item *pItem)	const
{
	return( Get_Position(pItem) == m_Items.Get_Size() - 1 );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::is_Bottom(CSGDI_Layout_Item *pItem)	const
{
	return( Get_Position(pItem) == 0 );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Top(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position < m_Items.Get_Size() - 1 )
	{
		for(size_t i=Position; i<m_Items.Get_Size() - 1; i++)
		{
			m_Items[i]	= m_Items[i + 1];
		}

		m_Items[m_Items.Get_Size() - 1]	= pItem;

		pItem->Refresh();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Bottom(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position > 0 )
	{
		for(size_t i=Position; i>0; i--)
		{
			m_Items[i]	= m_Items[i - 1];
		}

		m_Items[0]	= pItem;

		pItem->Refresh();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Up(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position < m_Items.Get_Size() - 1 )
	{
		m_Items[Position    ]	= m_Items[Position + 1];
		m_Items[Position + 1]	= pItem;

		pItem->Refresh();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Down(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position > 0 )
	{
		m_Items[Position    ]	= m_Items[Position - 1];
		m_Items[Position - 1]	= pItem;

		pItem->Refresh();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSGDI_Layout_Items::Active_Get_Position(void)	const
{
	return( Get_Position(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_is_Top(void)	const
{
	return( is_Top(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_is_Bottom(void)	const
{
	return( is_Bottom(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Top(void)
{
	return( Move_Top(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Bottom(void)
{
	return( Move_Bottom(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Up(void)
{
	return( Move_Up(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Down(void)
{
	return( Move_Up(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Properties(void)
{
	if( m_pActive && m_pActive->Properties(m_pParent) )
	{
		if( m_pParent )
		{
			m_pParent->Refresh();
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Hide(CSGDI_Layout_Item *pItem)
{
	if( pItem->is_Shown() )
	{
		if( m_pActive == pItem )
		{
			m_pActive	= NULL;
		}

		pItem->Refresh();

		pItem->m_bShow	= false;
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Show(CSGDI_Layout_Item *pItem)
{
	if( !pItem->is_Shown() )
	{
		pItem->Refresh();

		pItem->m_bShow	= true;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Draw(wxDC &dc, bool bTracker)
{
	for(size_t i=0; i<m_Items.Get_Size(); i++)
	{
		if( Get_Item(i)->is_Shown() )
		{
			Get_Item(i)->Draw(dc);
		}
	}

	if( bTracker )
	{
		m_Tracker.Draw(dc);
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::On_Key_Event(wxKeyEvent &event)
{
	if( m_pActive )
	{
		if( event.GetKeyCode() == WXK_RETURN )
		{
			if( Active_Properties() )
			{
				m_pActive->Refresh();
			}

			return( true );
		}

		//-------------------------------------------------
		wxRect	r(m_pActive->m_Rect); int Move = event.ControlDown() ? 1 : !event.ShiftDown() ? 5 : 20;

		switch( event.GetKeyCode() )
		{
		case WXK_LEFT : r.Offset(-Move,     0); return( m_pActive->_Tracker_Set_Rect(Get_Scaled(r, m_Scale)) );
		case WXK_RIGHT: r.Offset( Move,     0); return( m_pActive->_Tracker_Set_Rect(Get_Scaled(r, m_Scale)) );
		case WXK_UP   : r.Offset(    0, -Move); return( m_pActive->_Tracker_Set_Rect(Get_Scaled(r, m_Scale)) );
		case WXK_DOWN : r.Offset(    0,  Move); return( m_pActive->_Tracker_Set_Rect(Get_Scaled(r, m_Scale)) );
		}
	}

	//-----------------------------------------------------
	event.Skip();

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::On_Mouse_Event(wxMouseEvent &event)
{
	wxPoint	p(event.GetPosition());

	if( wxDynamicCast(m_pParent, wxScrolledWindow) )
	{
		p	= wxDynamicCast(m_pParent, wxScrolledWindow)->CalcUnscrolledPosition(p);
	}

	//-----------------------------------------------------
	if     ( event.Moving    () )
	{
		switch( m_Tracker.Hit_Test(p) )
		{
		case HANDLE_TOP_LEFT     : m_pParent->SetCursor(wxCURSOR_SIZENWSE); break;
		case HANDLE_TOP_CENTER   : m_pParent->SetCursor(wxCURSOR_SIZENS  ); break;
		case HANDLE_TOP_RIGHT    : m_pParent->SetCursor(wxCURSOR_SIZENESW); break;
		case HANDLE_LEFT_CENTER  : m_pParent->SetCursor(wxCURSOR_SIZEWE  ); break;
		case HANDLE_RIGHT_CENTER : m_pParent->SetCursor(wxCURSOR_SIZEWE  ); break;
		case HANDLE_BOTTOM_LEFT  : m_pParent->SetCursor(wxCURSOR_SIZENESW); break;
		case HANDLE_BOTTOM_CENTER: m_pParent->SetCursor(wxCURSOR_SIZENS  ); break;
		case HANDLE_BOTTOM_RIGHT : m_pParent->SetCursor(wxCURSOR_SIZENWSE); break;
		case HANDLE_TRACKER      : m_pParent->SetCursor(wxCURSOR_HAND    ); break;
		case HANDLE_NONE         : m_pParent->SetCursor(wxNullCursor     ); break;
		}
	}

	//-----------------------------------------------------
	else if( event.LeftDown  () )
	{
		if( m_pParent->GetParent() )
		{
			m_pParent->GetParent()->SetFocus();
		}

		Select(p);

		m_Tracker.Drag_Start(p);

		m_pParent->CaptureMouse();
	}
	else if( event.Dragging  () )
	{
		m_Tracker.Drag_Move (p);
	}
	else if( event.LeftUp    () )
	{
		m_Tracker.Drag_Stop (p);

		m_pParent->ReleaseMouse();
	}

	//-----------------------------------------------------
	else if( event.RightDown () )
	{
		if( m_pParent->GetParent() )
		{
			m_pParent->GetParent()->SetFocus();
		}

		Select(p);
	}

	//-----------------------------------------------------
	else if( event.LeftDClick() )
	{
		Select(p);

		if( m_pActive && Active_Properties() )
		{
			m_pActive->Refresh();
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Select(const wxPoint &Point)
{
	if( m_pActive )
	{
		switch( m_Tracker.Hit_Test(Point) )
		{
		default:
			return( true );

		case HANDLE_TRACKER:
		case HANDLE_NONE:
			break;
		}
	}

	//-----------------------------------------------------
	CSGDI_Layout_Item	*pActivate	= NULL;

	for(size_t i=0, j=m_Items.Get_Size()-1; !pActivate && i<m_Items.Get_Size(); i++, j--)
	{
		if( Get_Item(j)->is_Shown() && Get_Item(j)->_Tracker_Contains(Point) )
		{
			pActivate	= Get_Item(j);
		}
	}

	//-----------------------------------------------------
	if( m_pActive != pActivate )
	{
		if( m_pActive )
		{
			m_pActive->Refresh(false);

			m_Tracker.Draw(true);
		}

		m_pActive	= pActivate;

		m_Tracker.Draw();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Tracker::CSGDI_Layout_Tracker(void)
{
	m_pOwner	= NULL;
}

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Tracker::~CSGDI_Layout_Tracker(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline wxRect CSGDI_Layout_Items::CSGDI_Layout_Tracker::Get_Rect(THandle Handle)
{
	if( m_pOwner->m_pActive && m_pOwner->m_Scale > 0. && Handle != HANDLE_NONE )
	{
		wxRect	r(Get_Scaled(m_pOwner->m_pActive->m_Rect, m_pOwner->m_Scale));	int	w	= m_pOwner->m_Handle;

		switch( Handle )
		{
		case HANDLE_TOP_LEFT     : return( wxRect(r.x                    , r.y                     , w, w) );
		case HANDLE_TOP_CENTER   : return( wxRect(r.x + (r.width - w) / 2, r.y                     , w, w) );
		case HANDLE_TOP_RIGHT    : return( wxRect(r.x + (r.width - w)    , r.y                     , w, w) );
		case HANDLE_LEFT_CENTER  : return( wxRect(r.x                    , r.y + (r.height - w) / 2, w, w) );
		case HANDLE_RIGHT_CENTER : return( wxRect(r.x + (r.width - w)    , r.y + (r.height - w) / 2, w, w) );
		case HANDLE_BOTTOM_LEFT  : return( wxRect(r.x                    , r.y + (r.height - w)    , w, w) );
		case HANDLE_BOTTOM_CENTER: return( wxRect(r.x + (r.width - w) / 2, r.y + (r.height - w)    , w, w) );
		case HANDLE_BOTTOM_RIGHT : return( wxRect(r.x + (r.width - w)    , r.y + (r.height - w)    , w, w) );
		default                  : return( r );
		}
	}

	return( wxRect(0, 0, 0, 0) );
}

//---------------------------------------------------------
inline CSGDI_Layout_Items::THandle CSGDI_Layout_Items::CSGDI_Layout_Tracker::Hit_Test(const wxPoint &Point)
{
	if( m_pOwner->m_pActive && Get_Rect(HANDLE_TRACKER).Contains(Point) )
	{
		if( m_pOwner->m_pActive->m_bSizer )
		{
			if( Get_Rect(HANDLE_TOP_LEFT     ).Contains(Point) ) { return( HANDLE_TOP_LEFT     ); }
			if( Get_Rect(HANDLE_TOP_CENTER   ).Contains(Point) ) { return( HANDLE_TOP_CENTER   ); }
			if( Get_Rect(HANDLE_TOP_RIGHT    ).Contains(Point) ) { return( HANDLE_TOP_RIGHT    ); }
			if( Get_Rect(HANDLE_LEFT_CENTER  ).Contains(Point) ) { return( HANDLE_LEFT_CENTER  ); }
			if( Get_Rect(HANDLE_RIGHT_CENTER ).Contains(Point) ) { return( HANDLE_RIGHT_CENTER ); }
			if( Get_Rect(HANDLE_BOTTOM_LEFT  ).Contains(Point) ) { return( HANDLE_BOTTOM_LEFT  ); }
			if( Get_Rect(HANDLE_BOTTOM_CENTER).Contains(Point) ) { return( HANDLE_BOTTOM_CENTER); }
			if( Get_Rect(HANDLE_BOTTOM_RIGHT ).Contains(Point) ) { return( HANDLE_BOTTOM_RIGHT ); }
		}

		return( HANDLE_TRACKER );
	}

	return( HANDLE_NONE );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Tracker::Drag_Start(const wxPoint &Point)
{
	if( !m_pOwner->m_pActive )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Drag_Mode		= Hit_Test(Point);
	m_Drag_Start	= Point;
	m_Drag_Last		= Point;

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Tracker::Drag_Move(const wxPoint &Point)
{
	if( m_Drag_Mode == HANDLE_NONE )
	{
		return( false );
	}

	//-----------------------------------------------------
	Drag_Draw(Drag_Rect(m_Drag_Start, m_Drag_Last        ));
	Drag_Draw(Drag_Rect(m_Drag_Start, m_Drag_Last = Point));

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Tracker::Drag_Stop(const wxPoint &Point)
{
	if( !m_pOwner->m_pActive || m_Drag_Mode == HANDLE_NONE )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( m_Drag_Start != m_Drag_Last )
	{
		Drag_Draw(Drag_Rect(m_Drag_Start, m_Drag_Last));
	}

	m_pOwner->m_pActive->_Tracker_Set_Rect(Drag_Rect(m_Drag_Start, Point));

	m_Drag_Mode	= HANDLE_NONE;

	return( true );
}

//---------------------------------------------------------
wxRect CSGDI_Layout_Items::CSGDI_Layout_Tracker::Drag_Rect(const wxPoint &From, const wxPoint &To)
{
	wxRect	r(Get_Scaled(m_pOwner->m_pActive->m_Rect, m_pOwner->m_Scale));

	if( From != To )
	{
		int	dx	= To.x - From.x;
		int	dy	= To.y - From.y;

		switch( m_Drag_Mode )
		{
		case HANDLE_TOP_LEFT     : r.x += dx; r.width -= dx; r.y += dy; r.height -= dy; break;
		case HANDLE_TOP_CENTER   :                           r.y += dy; r.height -= dy; break;
		case HANDLE_TOP_RIGHT    :            r.width += dx; r.y += dy; r.height -= dy; break;
		case HANDLE_LEFT_CENTER  : r.x += dx; r.width -= dx;                            break;
		case HANDLE_RIGHT_CENTER :            r.width += dx;                            break;
		case HANDLE_BOTTOM_LEFT  : r.x += dx; r.width -= dx;            r.height += dy; break;
		case HANDLE_BOTTOM_CENTER:                                      r.height += dy; break;
		case HANDLE_BOTTOM_RIGHT :            r.width += dx;            r.height += dy; break;
		case HANDLE_TRACKER      : r.x += dx;                r.y += dy;                 break;
		default                  :                                                      break;
		}

		if( abs(r.width) < 25 || abs(r.height) < 25 )
		{
			wxRect	rOriginal(Get_Scaled(m_pOwner->m_pActive->m_Rect, m_pOwner->m_Scale));

			if( abs(r.width ) < 25 ) { r.x = rOriginal.x; r.width  = 25; }
			if( abs(r.height) < 25 ) { r.y = rOriginal.y; r.height = 25; }
		}

		if( m_pOwner->m_pActive && m_pOwner->m_pActive->m_Ratio > 0. )
		{
			int	b	= r.width && m_pOwner->m_pActive->m_Ratio < (r.height / (double)r.width) ? 1 : 0;
			int	rx	= (int)(0.5 + r.height / m_pOwner->m_pActive->m_Ratio);
			int	ry	= (int)(0.5 + r.width  * m_pOwner->m_pActive->m_Ratio);

			switch( m_Drag_Mode )
			{
			case HANDLE_TOP_LEFT     : if( b ) { r.x += r.width - rx; r.width = rx; } else { r.y += r.height - ry; r.height = ry; } break;
			case HANDLE_TOP_CENTER   :         {                      r.width = rx; }                                               break;
			case HANDLE_TOP_RIGHT    : if( b ) {                      r.width = rx; } else { r.y += r.height - ry; r.height = ry; } break;
			case HANDLE_LEFT_CENTER  :                                                     {                       r.height = ry; } break;
			case HANDLE_RIGHT_CENTER :                                                     {                       r.height = ry; } break;
			case HANDLE_BOTTOM_LEFT  : if( b ) { r.x += r.width - rx; r.width = rx; } else {                       r.height = ry; } break;
			case HANDLE_BOTTOM_CENTER:         {                      r.width = rx; }                                               break;
			case HANDLE_BOTTOM_RIGHT : if( b ) {                      r.width = rx; } else {                       r.height = ry; } break;
			default                  :                                                                                              break;
			}
		}
	}

	return( r );
}

//---------------------------------------------------------
void CSGDI_Layout_Items::CSGDI_Layout_Tracker::Drag_Draw(const wxRect &Rect)
{
	if( m_pOwner->m_pParent )
	{
		wxClientDC	dc(m_pOwner->m_pParent);

		if( wxDynamicCast(m_pOwner->m_pParent, wxScrolledWindow) )
		{
			wxDynamicCast(m_pOwner->m_pParent, wxScrolledWindow)->DoPrepareDC(dc);
		}

		dc.SetLogicalFunction(wxINVERT);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
	//	dc.SetPen(*wxWHITE_PEN);
		dc.DrawRectangle(Rect);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Tracker::Draw(bool bWhite)
{
	if( m_pOwner->m_pParent && m_pOwner->m_pActive )
	{
		wxClientDC	dc(m_pOwner->m_pParent);

		if( wxDynamicCast(m_pOwner->m_pParent, wxScrolledWindow) )
		{
			wxDynamicCast(m_pOwner->m_pParent, wxScrolledWindow)->DoPrepareDC(dc);
		}

		return( Draw(dc, bWhite) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Tracker::Draw(wxDC &dc, bool bWhite)
{
	if( !m_pOwner->m_pParent || !m_pOwner->m_pActive )
	{
		return( false );
	}

	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	dc.SetPen(*wxWHITE_PEN);
	dc.DrawRectangle(Get_Rect(HANDLE_TRACKER));

	if( !bWhite )
	{
		dc.SetPen  (*wxBLACK_DASHED_PEN );
		dc.DrawRectangle(Get_Rect(HANDLE_TRACKER));
	}

	if( m_pOwner->m_pActive->m_bSizer )
	{
		dc.SetBrush(bWhite ? *wxWHITE_BRUSH : *wxBLACK_BRUSH);
		dc.SetPen(*wxTRANSPARENT_PEN);

		dc.DrawRectangle(Get_Rect(HANDLE_TOP_LEFT     ));
		dc.DrawRectangle(Get_Rect(HANDLE_TOP_CENTER   ));
		dc.DrawRectangle(Get_Rect(HANDLE_TOP_RIGHT    ));
		dc.DrawRectangle(Get_Rect(HANDLE_LEFT_CENTER  ));
		dc.DrawRectangle(Get_Rect(HANDLE_RIGHT_CENTER ));
		dc.DrawRectangle(Get_Rect(HANDLE_BOTTOM_LEFT  ));
		dc.DrawRectangle(Get_Rect(HANDLE_BOTTOM_CENTER));
		dc.DrawRectangle(Get_Rect(HANDLE_BOTTOM_RIGHT ));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
