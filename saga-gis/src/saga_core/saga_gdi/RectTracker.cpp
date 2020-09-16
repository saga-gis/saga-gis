/* ****************************************************************************
 * RectTracker.cpp                                                            *
 *                                                                            *
 * (c) 2004-2008 - R�mi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */

#include "RectTracker.h"

#ifdef wxUSE_GRAPHICS_CONTEXT
#include <wx/graphics.h>
#include <wx/dcgraph.h>
#endif

IMPLEMENT_CLASS(wxRectTracker, wxEvtHandler)

static const wxRect NullRect(0, 0, -1, -1);

wxRectTracker::wxRectTracker(wxWindow* wnd, wxFrame* frame) : 
    wxEvtHandler(), m_wnd(wnd), m_frame(frame), m_cGreyOut(0,0,0,0)
{
	m_iHandlerWidth = 5;
	m_cursorMove = new wxCursor(wxCURSOR_SIZING);
	m_state = 0;
	m_Rect = m_minRect = m_maxRect = NullRect;
	m_iHandlerMask = RT_MASK_ALL;
	if (wnd) wnd->PushEventHandler(this);
}

wxRectTracker::~wxRectTracker()
{
	if (m_wnd) m_wnd->RemoveEventHandler(this);
	delete m_cursorMove;
}

BEGIN_EVENT_TABLE(wxRectTracker, wxEvtHandler)
   EVT_PAINT(wxRectTracker::OnPaint)
   EVT_MOTION(wxRectTracker::OnMouseMotion)
   EVT_LEFT_DOWN(wxRectTracker::OnMouseLeftDown)
   EVT_LEFT_UP(wxRectTracker::OnMouseLeftUp)
   EVT_CHAR(wxRectTracker::OnKey)
END_EVENT_TABLE()

//const wxEventType wxEVT_TRACKER_CHANGED = wxNewEventType();
//const wxEventType wxEVT_TRACKER_CHANGING = wxNewEventType();
#include "sgdi_layout_items.h"
wxDEFINE_EVENT(wxEVT_TRACKER_CHANGED , wxCommandEvent);
wxDEFINE_EVENT(wxEVT_TRACKER_CHANGING, wxCommandEvent);

void wxRectTracker::OnPaint(wxPaintEvent& event)
{
	this->GetNextHandler()->ProcessEvent(event);

	wxClientDC dc(m_wnd);
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->DoPrepareDC(dc);
	}  

	OnDraw(&dc);
}

void wxRectTracker::OnDraw(wxDC* dc)
{
   if ((m_state & RT_STATE_DISABLED) != 0) return;
   int w = m_Rect.width, h = m_Rect.height;
   if ((w >=0) && (h >= 0)) DrawRect(*dc, m_Rect.GetLeft(), m_Rect.GetTop(), w, h);
}

// DrawRect operates with a wxPaintDC => Window coordinates
void wxRectTracker::DrawRect(wxDC & dc, int x, int y, int w, int h)
{
	
	// Rect
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawRectangle(x,y,w,h);
	dc.SetPen(wxPen(*wxWHITE,1, wxPENSTYLE_DOT));  // wxSHORT_DASH  in wxGCDC
	dc.DrawRectangle(x,y,w,h);

	// Handlers
  	int z = m_iHandlerWidth;
	if (m_iHandlerMask != RT_MASK_NONE)
	{
    	dc.SetBrush(*wxBLACK_BRUSH);
    	dc.SetPen(*wxBLACK_PEN);
    	
    	if (m_iHandlerMask & RT_MASK_TOP_LEFT) dc.DrawRectangle(x+1    , y+1    , z, z);
    	if (m_iHandlerMask & RT_MASK_TOP_RIGHT) dc.DrawRectangle(x-1+w-z, y+1    , z, z);
    	if (m_iHandlerMask & RT_MASK_BOTTOM_LEFT) dc.DrawRectangle(x+1    , y-1+h-z, z, z);
    	if (m_iHandlerMask & RT_MASK_BOTTOM_RIGHT) dc.DrawRectangle(x-1+w-z, y-1+h-z, z, z);
    	
    	if (m_iHandlerMask & RT_MASK_TOP_MID) dc.DrawRectangle(x+(w-z)/2, y+1    , z, z);
    	if (m_iHandlerMask & RT_MASK_BOTTOM_MID) dc.DrawRectangle(x+(w-z)/2, y+h-z-1, z, z);
    	if (m_iHandlerMask & RT_MASK_MID_LEFT) dc.DrawRectangle(x+1    , y+(h-z)/2, z, z);
    	if (m_iHandlerMask & RT_MASK_MID_RIGHT) dc.DrawRectangle(x+w-z-1, y+(h-z)/2, z, z);
    }


	// Grey out cutted off area
	// Use wxGraphicsContext for transparency support
#ifdef wxUSE_GRAPHICS_CONTEXT
	if ((w != 0) && (h != 0) && (m_cGreyOut.Alpha() != 0))
	{
		if (wxDynamicCast(&dc,wxWindowDC))  // If needed more casting are available to create wxGraphicsContext
		{
			int cw, ch;
			dc.GetSize(&cw,&ch);
			cw = dc.DeviceToLogicalX(cw);
			ch = dc.DeviceToLogicalY(ch);
			if (cw > GetMaxRect().GetWidth()) cw = GetMaxRect().GetWidth();
			if (ch > GetMaxRect().GetHeight()) ch = GetMaxRect().GetHeight();
			wxGraphicsContext * gc;
			gc = wxGraphicsContext::Create(*wxDynamicCast(&dc,wxWindowDC));
			gc->SetBrush(wxBrush(m_cGreyOut));
			gc->SetPen(wxPen(m_cGreyOut));
			wxGraphicsPath gp = gc->CreatePath();
			gp.AddRectangle(0  ,0  ,cw ,y );			
			gp.AddRectangle(0  ,y  ,x  ,h  );
			gp.AddRectangle(x+w,y  ,cw-x-w,h  );
			gp.AddRectangle(0  ,y+h,cw ,ch-y-h );
			gc->FillPath(gp);
			delete gc;
		}  
	}
#endif

}

void wxRectTracker::DrawRect(wxDC & dc, wxRect rect)
{
	DrawRect(dc, rect.x, rect.y, rect.width, rect.height);
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxRectTracker::DrawTracker(wxDC & dc, int x, int y, int w, int h)
{
	
	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow) != NULL)
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x, y, &x, &y);
	}
	
// Replaced invert stuff with overlay as it seems not to be compatible with GTK3
#ifndef __TRACKER_OVERLAY__
	// Inverted Rect
	dc.SetLogicalFunction(wxINVERT);
#endif
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRectangle(x,y,w,h);
#ifndef __TRACKER_OVERLAY__
	dc.SetLogicalFunction(wxCOPY);
#endif
}

void wxRectTracker::DrawTracker(wxDC & dc, const wxRect& rect)
{
	DrawTracker(dc, rect.x, rect.y, rect.width, rect.height);
}


void wxRectTracker::OnKey(wxKeyEvent & event)
{
    if ( ((m_state & RT_STATE_DISABLED) != 0) || !IsShown()) { event.Skip(); return; } 
    
	int incr = 0, dx = 0, dy = 0;
	int handler;
	wxRect tmpRect;
	tmpRect = GetUnscrolledRect();

	if ((tmpRect.width <= 0) || (tmpRect.height <= 0)) { event.Skip(); return; } 
	
	if (event.ControlDown()) incr = 10; else incr = 1;

	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE :
			if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
			{
				m_wnd->ReleaseMouse();
				m_state ^= RT_STATE_MOUSE_CAPTURED;
				Update();
			}
            else
            {
                Hide();
            }
			break;
	case WXK_LEFT	: dx -= incr; break;
	case WXK_UP		: dy -= incr; break;
	case WXK_RIGHT	: dx += incr; break;
	case WXK_DOWN	: dy += incr; break;
	default:
		event.Skip();
	}

	if ((dx != 0) || (dy != 0))
	{
		if (event.ShiftDown())
		{
			tmpRect.width += dx;
			tmpRect.height += dy;
			handler = (dx == 0)?RT_HANDLER_BOTTOM_MID:RT_HANDLER_MID_RIGHT;
		}
		else
		{
			tmpRect.x += dx;
			tmpRect.y += dy;
			handler = 0;
		}
		AdjustTrackerRect(tmpRect, handler);
		SetUnscrolledRect(tmpRect);
		Update();
	}

}

void wxRectTracker::OnMouseMotion(wxMouseEvent & event)
{
	int hit;
	int dx, dy;

    if ((m_state & RT_STATE_DISABLED) != 0) return;

	wxMouseEvent mouse(event);

	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcUnscrolledPosition(mouse.m_x, mouse.m_y, &mouse.m_x, &mouse.m_y);
	}
	
    if (m_frame && IsShown())
    {
	    m_frame->SetStatusText(wxString::Format(_("Mouse position : %d, %d / RectSize : %d,%d->%d,%d(+%d,%d)"),
				event.m_x, event.m_y, 
				m_Rect.GetLeft(), m_Rect.GetTop(), m_Rect.GetRight(), m_Rect.GetBottom(), m_Rect.GetWidth(), m_Rect.GetHeight()));
    }
	
	// Just moving ?
	if (!event.Dragging())
	{
		hit = HitTest(mouse.m_x, mouse.m_y);
		switch (hit)
		{
		case RT_HANDLER_TOP_MID:
		case RT_HANDLER_BOTTOM_MID:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZENS));
			break;
		case RT_HANDLER_MID_LEFT:
		case RT_HANDLER_MID_RIGHT:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZEWE));
			break;
		case RT_HANDLER_TOP_LEFT:
		case RT_HANDLER_BOTTOM_RIGHT:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
			break;
		case RT_HANDLER_TOP_RIGHT:
		case RT_HANDLER_BOTTOM_LEFT:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZENESW));
			break;
		case RT_HANDLER_NONE:
			m_wnd->SetCursor(wxCursor(wxCURSOR_HAND /* *m_cursorMove */));
			break;
		case RT_HANDLER_OUTSIDE:
			m_wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
			break;
		default:
			m_wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
		}
	}
	else if ((m_state & RT_STATE_DRAGGING))
	{
		// Dragging

		wxASSERT(m_wnd!=NULL);

		// Drawing Tracker Rect
	    wxClientDC dc(m_wnd);
		if (wxDynamicCast(m_wnd,wxScrolledWindow))
		{
			wxDynamicCast(m_wnd,wxScrolledWindow)->DoPrepareDC(dc);
		} 
		//m_wnd->PrepareDC(dc);

		dx = 0; dy = 0;
		dc.SetDeviceOrigin(dx, dy);
		
#ifdef __TRACKER_OVERLAY__
		wxDCOverlay odc(trackerOverlay, &dc);
#endif
		

		if ((m_state & RT_STATE_FIRSTDRAG) == 0)
		{
			m_state |= RT_STATE_FIRSTDRAG;
		}
		else
		{
			// Erase previous Tracker
#ifdef __TRACKER_OVERLAY__
			odc.Clear();
#else
			DrawTracker(dc, m_prevRect);
#endif
		}

		// Update the new position
		// - Which Tracker ?
		hit = HitTest(m_leftClick.x, m_leftClick.y);
		// - Default Rect values
		if (hit != RT_HANDLER_OUTSIDE) 
		{
			m_curRect = GetUnscrolledRect();
		}
		else
		{
			m_curRect = wxRect(m_leftClick, m_leftClick);
			hit = RT_HANDLER_BOTTOM_RIGHT;
		}
		
		dx = (mouse.m_x - m_leftClick.x);
		dy = (mouse.m_y - m_leftClick.y);

		if (hit == RT_HANDLER_OUTSIDE)
		{
			m_curRect.width += dx;
			m_curRect.height += dy;
		}
		else 	if (hit == RT_HANDLER_NONE)
		{
			m_curRect.x += dx;
			m_curRect.y += dy;
		}
		else
		{
			// Use bit combination of handler values to update position
			if ( (hit & RT_HANDLER_MID_RIGHT) != 0)
			{
				m_curRect.width += dx;
			}
			if ( (hit & RT_HANDLER_BOTTOM_MID) != 0)
			{
				m_curRect.height += dy;
			}
			if ( (hit & RT_HANDLER_MID_LEFT) != 0)
			{
				m_curRect.x += dx;
				m_curRect.width -= dx;
			}
			if ( (hit & RT_HANDLER_TOP_MID) != 0)
			{
				m_curRect.y += dy;
				m_curRect.height -= dy;
			}
		}
        if (m_frame)
        {
		    m_frame->SetStatusText(wxString::Format(_("Mouse click : %d, %d / Hit : %d / RectSize : %d,%d->%d,%d(+%d,%d)"),
				m_leftClick.x, m_leftClick.y, 
				hit,
				m_Rect.GetLeft(), m_Rect.GetTop(), m_Rect.GetRight(), m_Rect.GetBottom(), m_Rect.GetWidth(), m_Rect.GetHeight()));
        }
		// Correct Orientation (size and virtual handler)
        if (m_curRect.width < 0) 
        { 
            m_curRect.width = - m_curRect.width; 
            m_curRect.x -= m_curRect.width; 
            if ((hit & RT_HANDLER_MID_LEFT) != 0) hit = (hit ^ RT_HANDLER_MID_LEFT) | RT_HANDLER_MID_RIGHT;
            else if ((hit & RT_HANDLER_MID_RIGHT) != 0) hit = (hit ^ RT_HANDLER_MID_RIGHT) | RT_HANDLER_MID_LEFT;
        }
		if (m_curRect.height < 0) 
        { 
            m_curRect.height = - m_curRect.height; 
            m_curRect.y -= m_curRect.height; 
            if ((hit & RT_HANDLER_TOP_MID) != 0) hit = (hit ^ RT_HANDLER_TOP_MID) | RT_HANDLER_BOTTOM_MID;
            else if ((hit & RT_HANDLER_BOTTOM_MID) != 0) hit = (hit ^ RT_HANDLER_BOTTOM_MID) | RT_HANDLER_TOP_MID;
        }

		// Adjust current Tracker size
		AdjustTrackerRect(m_curRect, hit);

		// Draw current Tracker
		DrawTracker(dc, m_curRect);

		// Update Parent's Status Bar
		m_prevRect = m_curRect;
		wxCommandEvent evt(wxEVT_TRACKER_CHANGING, m_wnd->GetId());
		m_wnd->GetEventHandler()->ProcessEvent(evt);
	}
    
	// Check there is no abuse mouse capture
	if (!(event.LeftIsDown()) && ((m_state & RT_STATE_MOUSE_CAPTURED) != 0))
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
    
	// Update prev_move
	m_prevMove = mouse.GetPosition();

   event.Skip();
}

void wxRectTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	m_state |= RT_STATE_DRAGGING;
	m_leftClick = event.GetPosition();
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcUnscrolledPosition(m_leftClick.x, m_leftClick.y, &m_leftClick.x, &m_leftClick.y);
	}
	if (HitTest(m_leftClick.x, m_leftClick.y) == RT_HANDLER_OUTSIDE) 
	{
		Hide();
		Update();
	}
	
	if ((m_state & RT_STATE_MOUSE_CAPTURED) == 0)
	{
		m_wnd->CaptureMouse();
		m_state |= RT_STATE_MOUSE_CAPTURED;
	}

#ifdef __TRACKER_OVERLAY__
		trackerOverlay.Reset();
#endif

    event.Skip();
}

void wxRectTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((m_state & RT_STATE_DRAGGING) != 0)
	{
		SetUnscrolledRect(m_prevRect);
		m_state ^= RT_STATE_DRAGGING;
		m_state ^= RT_STATE_FIRSTDRAG;
		m_leftClick = wxPoint(0, 0);
		Update();
	}
   event.Skip();
}

int wxRectTracker::HitTest(int x, int y) const
{
	int w, h;
	int z = m_iHandlerWidth;
	
	w = m_Rect.GetWidth();
	h = m_Rect.GetHeight();
	x = x - m_Rect.GetLeft();
	y = y - m_Rect.GetTop();

	if ( (y < 0) || (y > h) || (x < 0) || (x > w) ) return RT_HANDLER_OUTSIDE;
	
	// Split vertically, then horizontally
	if ( (y <= h) && (y >= h-z) )
	{
		// Bottom line
		if ( (x >= w-z) && (x <= w) )
		{
			// Bottom Right
			return RT_HANDLER_BOTTOM_RIGHT;
		}
		else if ( (x >= (w-z)/2) && (x <= (w+z)/2) )
		{
			// Bottom Mid
			return RT_HANDLER_BOTTOM_MID;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Bottom left
			return RT_HANDLER_BOTTOM_LEFT;
		}
	}
	else if ( (y >= (h-z)/2 ) && (y <= (h+z)/2) )
	{
		// Mid line
		if ( (x >= w-z) && (x <= w) )
		{
			// Mid Right
			return RT_HANDLER_MID_RIGHT;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Mid left
			return RT_HANDLER_MID_LEFT;
		}
	}
	else if ( (y >= 0) && (y <= z) )
	{
		// Top line
		if ( (x >= w-z) && (x <= w) )
		{
			// Top Right
			return RT_HANDLER_TOP_RIGHT;
		}
		else if ( (x >= (w-z)/2) && (x <= (w+z)/2) )
		{
			// Top Mid
			return RT_HANDLER_TOP_MID;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Top left
			return RT_HANDLER_TOP_LEFT;
		}
	}
	return RT_HANDLER_NONE;
}

void wxRectTracker::AdjustTrackerRectMin(wxRect &m_curRect, int handler)
{
	if( handler > 0 && m_minRect.width > 0 && m_minRect.height > 0 )
	{
		if( m_curRect.width < m_minRect.width )
		{
			if( handler == RT_HANDLER_TOP_LEFT
			||  handler == RT_HANDLER_MID_LEFT
			||  handler == RT_HANDLER_BOTTOM_LEFT )
			{
				m_curRect.x	-= m_minRect.width - abs(m_curRect.width);
			}

			m_curRect.width = m_minRect.width;
		}

		if( m_curRect.height < m_minRect.height )
		{
			if( handler == RT_HANDLER_TOP_RIGHT
			||  handler == RT_HANDLER_TOP_MID
			||  handler == RT_HANDLER_TOP_LEFT )
			{
				m_curRect.y	-= m_minRect.height - m_curRect.height;
			}

			m_curRect.height = m_minRect.height;
		}
	}
}

void wxRectTracker::AdjustTrackerRectMax(wxRect &m_curRect, int handler)
{
	// Adjust m_maxRect 
	if ((m_maxRect.width < 0) || (m_maxRect.height < 0)) return;

	// - Left X
	if (m_curRect.x < m_maxRect.x)
	{
		if (handler != RT_HANDLER_NONE) m_curRect.width -= m_maxRect.x - m_curRect.x; 
		m_curRect.x = m_maxRect.x;
	}
	// - Right X
	if ((m_curRect.x + m_curRect.width) > (m_maxRect.x + m_maxRect.width))
	{
		if (handler != RT_HANDLER_NONE) m_curRect.width = m_maxRect.x + m_maxRect.width - m_curRect.x; 
		m_curRect.x = m_maxRect.x + m_maxRect.width - m_curRect.width;
	}
	// - Top Y
	if (m_curRect.y < m_maxRect.y)
	{
		if (handler != RT_HANDLER_NONE) m_curRect.height -= m_maxRect.y - m_curRect.y; 
		m_curRect.y = m_maxRect.y;
	}
	// - Bottom Y
	if ((m_curRect.y + m_curRect.height) > (m_maxRect.y + m_maxRect.height))
	{
		if (handler != RT_HANDLER_NONE) m_curRect.height = m_maxRect.y + m_maxRect.height - m_curRect.y; 
		m_curRect.y = m_maxRect.y + m_maxRect.height - m_curRect.height;
	}

}

void wxRectTracker::AdjustTrackerRect(wxRect &curRect, int handler)
{
	AdjustTrackerRectMax(curRect, handler);
	AdjustTrackerRectMin(curRect, handler);
}

void wxRectTracker::SetMinRect(const wxRect& minRect)
{
	this->m_minRect = minRect;
	if (this->m_minRect.height < 0) this->m_minRect.height = - this->m_minRect.height;
	if (this->m_minRect.width < 0)  this->m_minRect.width  = - this->m_minRect.width;
	Update();
}

void wxRectTracker::SetMaxRect(const wxRect& maxRect)
{
	this->m_maxRect = maxRect;
	if (this->m_maxRect.height < 0) this->m_maxRect.height = - this->m_maxRect.height;
	if (this->m_maxRect.width < 0)  this->m_maxRect.width  = - this->m_maxRect.width;
	Update();
}

void wxRectTracker::Update()
{
	m_curRect = GetUnscrolledRect();
	AdjustTrackerRect(m_curRect, -1);
	if (m_curRect != GetUnscrolledRect())
	{
		SetUnscrolledRect(m_curRect);
	}
	m_wnd->Refresh();
    wxCommandEvent evt(wxEVT_TRACKER_CHANGED, m_wnd->GetId());
    m_wnd->GetEventHandler()->ProcessEvent(evt);
}


// TODO : rename this function, as it is now returning the scrolled rect
wxRect wxRectTracker::GetUnscrolledRect() const
{
	return m_Rect;
}

void wxRectTracker::SetUnscrolledRect(const wxRect &rect_ref)
{
	m_Rect = rect_ref;
}


/*
wxRect wxRectTracker::GetUnscrolledRect()
{
	wxRect rect;
	rect = m_wnd->GetRect();
   //rect = m_Rect;

	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcUnscrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
	}
	return rect;
}

void wxRectTracker::SetUnscrolledRect(const wxRect& rect_ref)
{
   wxRect rect = rect_ref;
   
	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
	}
	m_wnd->SetSize(rect);
	
   //m_Rect = rect;
}
*/

void wxRectTracker::Enable()
{
	this->SetEvtHandlerEnabled(TRUE);
    m_state &= ~RT_STATE_DISABLED;
}

void wxRectTracker::Disable()
{
	this->SetEvtHandlerEnabled(FALSE);
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
    //SetSize(wxRect(0, 0, 0, 0));
//    Update();
    m_state |= RT_STATE_DISABLED;
}

bool wxRectTracker::IsShown() const
{
    return (m_Rect != NullRect);
}

void wxRectTracker::Hide()
{
    m_Rect = NullRect;
    m_prevRect = NullRect;
    m_wnd->Refresh(false);
}


/* ****************************************************************************
* RectTrackerRatio.cpp                                                       *
*                                                                            *
* (c) 2004-2008 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
*                                                                            *
* RectTracker was originally designed for RPhoto, but can be used separately *
* It is a control similar to the CRectTracker of MS MFC.                     *
*                                                                            *
* Licence : wxWindows (based on L-GPL)                                       *
*                                                                            *
* ************************************************************************** */

IMPLEMENT_CLASS(wxRectTrackerRatio, wxRectTracker)

wxRectTrackerRatio::wxRectTrackerRatio(wxWindow * parent, wxFrame * frame) 
	: wxRectTracker(parent, frame)
{
	SetRatio(0);
	SetGuideRatio(0);
	SetOrientation(0);
}

wxRectTrackerRatio::~wxRectTrackerRatio()
{
}

long wxRectTrackerRatio::CalcRectDist(wxRect r1, wxRect r2)
{
	int d;
	d = 0;
	d += (r1.x - r2.x) * (r1.x - r2.x);
	d += (r1.y - r2.y) * (r1.y - r2.y);
	d += (r1.width - r2.width) * (r1.width - r2.width);
	d += (r1.height - r2.height) * (r1.height - r2.height);
	return d;
}

wxRect wxRectTrackerRatio::CalcRectNearer(wxRect rr, wxRect r1, wxRect r2)
{
	if (CalcRectDist(rr, r2) < CalcRectDist(rr, r1))
		return r2;
	else return r1;
}

void wxRectTrackerRatio::AdjustTrackerRectRatio(wxRect &curRect, int handler, bool expand)
{
	wxRect rectPort, rectLand, rectWidth, rectHeight, rectTmp;

	if (ratio <= 0) return;

	// Calc the best two rects
	// - rectWidth
	rectPort = rectLand = curRect;
	rectPort.width = (int) (rectPort.height * ratio);
	rectLand.width = (int) (rectLand.height / ratio);
	if ((handler & RT_HANDLER_MID_LEFT) != 0)
	{
		rectPort.x += curRect.width - rectPort.width;
		rectLand.x += curRect.width - rectLand.width;
	}
	//   * Portrait or Landscape ?
	switch(orientation)
	{
	case -1:	rectTmp = rectLand; break;
	case 1:		rectTmp = rectPort; break;
	default:	rectTmp = CalcRectNearer(curRect, rectPort, rectLand); break;
	}
	rectWidth = rectTmp;
	// - rectHeight (ugly cut'n paste)
	rectPort = rectLand = curRect;
	rectPort.height = (int) (rectPort.width / ratio);
	rectLand.height = (int) (rectLand.width * ratio);
	if ((handler & RT_HANDLER_TOP_MID) != 0)
	{
		rectPort.y += curRect.height - rectPort.height;
		rectLand.y += curRect.height - rectLand.height;
	}
	//   * Portrait or Landscape ?
	switch(orientation)
	{
	case -1:	rectTmp = rectLand; break;
	case 1:		rectTmp = rectPort; break;
	default:	rectTmp = CalcRectNearer(curRect, rectPort, rectLand); break;
	}
	rectHeight = rectTmp;

	switch(handler)
	{
	case RT_HANDLER_TOP_MID:
	case RT_HANDLER_BOTTOM_MID:
		curRect = (expand)?rectWidth:rectHeight;
		break;
	case RT_HANDLER_MID_RIGHT:
	case RT_HANDLER_MID_LEFT:
		curRect = (expand)?rectHeight:rectWidth;
		break;
	default:
		if (!expand)
		{
			if (curRect.height < rectHeight.height) rectHeight.height = -12000;
			if (curRect.width < rectWidth.width) rectWidth.width = -12000;
		}
		curRect = CalcRectNearer(curRect, rectHeight, rectWidth);
	}

}

void wxRectTrackerRatio::AdjustTrackerRectFixed(wxRect &curRect, int handler)
{
	// Adjust Width
	curRect.x += (curRect.width - fixedWidth);
	curRect.width = fixedWidth;
	if (curRect.x < m_maxRect.x) curRect.x = m_maxRect.x;
	if ((curRect.x + curRect.width) > (m_maxRect.x + m_maxRect.width)) curRect.x = m_maxRect.x + m_maxRect.width - curRect.width;
	// Adjust Height
	curRect.y += (curRect.height - fixedHeight);
	curRect.height = fixedHeight;
	if (curRect.y < m_maxRect.y) curRect.y = m_maxRect.y;
	if ((curRect.y + curRect.height) > (m_maxRect.y + m_maxRect.height)) curRect.y = m_maxRect.y + m_maxRect.height - curRect.height;
}

void wxRectTrackerRatio::AdjustTrackerRect(wxRect &curRect, int handler)
{
	wxRectTracker::AdjustTrackerRect(curRect, handler);
	if ((fixedWidth > 0 ) && (fixedHeight > 0))
	{
		AdjustTrackerRectFixed(curRect, handler);
	}    
	if (ratio > 0)
	{
		// First Pass
		AdjustTrackerRectRatio(curRect, handler, true);
		// Second Pass - Adjust if new rect has been out of the max rect.
		wxRectTracker::AdjustTrackerRect(curRect, handler);
		AdjustTrackerRectRatio(curRect, handler, false);
	}
}

void wxRectTrackerRatio::SetRatio(double ratio)
{
	this->fixedWidth = this->fixedHeight = -1; // Remove fixed mode
	if ((ratio > 0) && (ratio < 1)) ratio = 1 / ratio;
	this->ratio = ratio;
	SetHandlerMask(RT_MASK_ALL);
}


void wxRectTrackerRatio::SetGuideRatio(double ratio)
{
	if (ratio > 1) ratio = 1 / ratio;
	this->guideRatio = ratio;
}

void wxRectTrackerRatio::SetOrientation(int orientation)
{
	this->orientation = orientation;
	if (this->fixedWidth != -1) SetFixedSize(this->fixedWidth, this->fixedHeight);
}

void wxRectTrackerRatio::SetFixedSize(int width, int height)
{
	int tmp;
	this->fixedWidth = width;
	this->fixedHeight = height;
	// Switch orientation if one was given
	if ( ((orientation == -1) && (this->fixedWidth > this->fixedHeight)) ||
		((orientation == 1) && (this->fixedWidth < this->fixedHeight))   )
	{
		tmp = this->fixedWidth;
		this->fixedWidth = this->fixedHeight;
		this->fixedHeight = tmp;
	}
	SetHandlerMask(RT_MASK_NONE);
}    


// DrawRect operates with a wxPaintDC => Window coordinates
void wxRectTrackerRatio::DrawRect(wxDC & dc, int x, int y, int w, int h)
{
	wxRectTracker::DrawRect(dc, x, y, w, h);
	// Rect
	if (this->guideRatio != 0)
	{
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawLine(x  ,y + h*this->guideRatio  ,x+w,y + h*this->guideRatio   );
		dc.DrawLine(x  ,y + h*(1-this->guideRatio)  ,x+w,y + h*(1-this->guideRatio));
		dc.DrawLine(x+w*this->guideRatio  ,y  ,x+w*this->guideRatio  ,y+h);
		dc.DrawLine(x+w*(1-this->guideRatio)  ,y  ,x+w*(1-this->guideRatio)  ,y+h);
	}
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxRectTrackerRatio::DrawTracker(wxDC & dc, int x, int y, int w, int h)
{
	wxRectTracker::DrawTracker(dc, x, y, w, h);
	if (this->guideRatio != 0)
	{
		// Convert coordinates if scrolled
		if (wxDynamicCast(m_wnd,wxScrolledWindow) != NULL)
		{
			wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x, y, &x, &y);
		}

		// Inverted Rect
		#ifndef __TRACKER_OVERLAY__
		// Inverted Rect
		dc.SetLogicalFunction(wxINVERT);
		#endif
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawLine(x  ,y + h*this->guideRatio  ,x+w,y + h*this->guideRatio   );
		if (this->guideRatio != 0.5) dc.DrawLine(x  ,y + h*(1-this->guideRatio)  ,x+w,y + h*(1-this->guideRatio));
		dc.DrawLine(x+w*this->guideRatio  ,y  ,x+w*this->guideRatio  ,y+h);
		if (this->guideRatio != 0.5) dc.DrawLine(x+w*(1-this->guideRatio)  ,y  ,x+w*(1-this->guideRatio)  ,y+h);
		#ifndef __TRACKER_OVERLAY__
		dc.SetLogicalFunction(wxCOPY);
		#endif
	}
}


/* ****************************************************************************
* LineTracker.cpp                                                            *
*                                                                            *
* (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
* LineTracker was originally designed for RPhoto, but can be used separately *
*                                                                            *
* Licence : wxWindows (based on L-GPL)                                       *
*                                                                            *
* ************************************************************************** */


#include <wx/dcbuffer.h>

/// Helpers

static wxRect NormalizeRect(wxRect rect)
{
	if (rect.width < 0) { rect.x += rect.width ; rect.width = -rect.width; }
	if (rect.height < 0) { rect.y += rect.height ; rect.height = -rect.height; }
	return rect;
}

/// wxLineTracker

IMPLEMENT_CLASS(wxLineTracker, wxRectTracker)

wxLineTracker::wxLineTracker(wxWindow * parent, wxFrame * frame) 
	: wxRectTracker(parent, frame)
{
}

wxLineTracker::~wxLineTracker()
{
}


void wxLineTracker::OnPaint(wxPaintEvent & event)
{
	this->GetNextHandler()->ProcessEvent(event);

	if ((m_state & RT_STATE_DISABLED) != 0) return;
	if ((m_state & RT_STATE_DRAGGING) != 0) return;
	wxClientDC dc(m_wnd);
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->DoPrepareDC(dc);
	}   
	DrawLine(dc, GetPosBegin(), GetPosEnd());
}

// DrawLine operates with a wxPaintDC => Window coordinates
void wxLineTracker::DrawLine(wxDC & dc, wxPoint begin, wxPoint end)
{
	// Rect
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawLine(begin.x, begin.y, end.x, end.y);
	dc.SetPen(wxPen(*wxWHITE,1, wxPENSTYLE_DOT));
	dc.DrawLine(begin.x, begin.y, end.x, end.y);

	// Handlers
	int z = m_iHandlerWidth;
	if (m_iHandlerMask != RT_MASK_NONE)
	{
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.SetPen(*wxBLACK_PEN);

		if (m_iHandlerMask & RT_LINE_MASK_BEGIN) dc.DrawRectangle(begin.x-(z/2), begin.y-(z/2), z, z);
		if (m_iHandlerMask & RT_LINE_MASK_END  ) dc.DrawRectangle(end.x-(z/2), end.y-(z/2), z, z);
	}   	
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxLineTracker::DrawTracker(wxDC & dc, wxPoint begin, wxPoint end)
{

	int x1, y1, x2, y2;
	x1 = begin.x; y1 = begin.y;
	x2 = end.x;   y2 = end.y;
	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow) != NULL)
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x1, y1, &x1, &y1);
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x2, y2, &x2, &y2);
	}

	// Inverted Line
	#ifndef __TRACKER_OVERLAY__
	// Inverted Rect
	dc.SetLogicalFunction(wxINVERT);
	#endif
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawLine(begin.x,begin.y,end.x,end.y);
	#ifndef __TRACKER_OVERLAY__
	dc.SetLogicalFunction(wxCOPY);
	#endif
}


void wxLineTracker::OnKey(wxKeyEvent & event)
{

	if ((m_state & RT_STATE_DISABLED) != 0) return;

	int incr = 0, dx = 0, dy = 0;
	int handler;
	wxRect tmpRect;
	tmpRect = GetUnscrolledRect();

	if (event.ControlDown()) incr = 10; else incr = 1;

	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE :
		if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
		{
			m_wnd->ReleaseMouse();
			m_state ^= RT_STATE_MOUSE_CAPTURED;
			Update();
		}
		else
		{
			Hide();
		}
		break;
	case WXK_LEFT	: dx -= incr; break;
	case WXK_UP		: dy -= incr; break;
	case WXK_RIGHT	: dx += incr; break;
	case WXK_DOWN	: dy += incr; break;
	default:
		event.Skip();
	}

	if ((dx != 0) || (dy != 0))
	{
		if (event.ShiftDown())
		{
			m_pCurEnd.x += dx;
			m_pCurEnd.y += dy;
			handler = RT_LINE_HANDLER_END;
		}
		else
		{
			m_pCurBegin.x += dx;	m_pCurEnd.x += dx;
			m_pCurBegin.y += dy;	m_pCurEnd.y += dy;
			handler = 0;
		}
		AdjustLineTracker(m_pCurBegin, m_pCurEnd, handler);
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
		Update();
	}

}

void wxLineTracker::OnMouseMotion(wxMouseEvent & event)
{
	int hit;
	int dx, dy;

	if ((m_state & RT_STATE_DISABLED) != 0) return;

	wxMouseEvent mouse(event);

	if (wxDynamicCast(m_wnd, wxScrolledWindow))
	{
		wxDynamicCast(m_wnd, wxScrolledWindow)->CalcUnscrolledPosition(mouse.m_x, mouse.m_y, &mouse.m_x, &mouse.m_y);
	}

	// Just moving ?
	if (!event.Dragging())
	{
		hit = HitTest(mouse.m_x, mouse.m_y);
		switch (hit)
		{
		case RT_LINE_HANDLER_END:
		case RT_LINE_HANDLER_BEGIN:
		case RT_LINE_HANDLER_ON_LINE:
			m_wnd->SetCursor(*m_cursorMove);
			break;
		default:
			m_wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
			break;
		}
	}
	else if (event.LeftIsDown())
	{
		// Dragging

		wxASSERT(m_wnd!=NULL);

		// Drawing Tracker Rect
		wxClientDC dc(m_wnd);
		if (wxDynamicCast(m_wnd, wxScrolledWindow))
		{
			wxDynamicCast(m_wnd, wxScrolledWindow)->DoPrepareDC(dc);
		}


		#ifdef __TRACKER_OVERLAY__
		wxDCOverlay odc(trackerOverlay, &dc);
		#endif

		if ((m_state & RT_STATE_DRAGGING) == 0)
		{
			m_state |= RT_STATE_DRAGGING;
			m_pCurBegin = GetPosBegin();
			m_pCurEnd = GetPosEnd();
		}
		else
		{
			// Erase previous Tracker
			#ifdef __TRACKER_OVERLAY__
			odc.Clear();
			#else
			DrawTracker(dc, m_pCurBegin, m_pCurEnd);
			#endif
		}

		// Update the new position
		// - Which Tracker ?
		hit = HitTest(m_leftClick.x, m_leftClick.y);
		// - Default Rect values

		dx = (mouse.m_x - m_leftClick.x);
		dy = (mouse.m_y - m_leftClick.y);

		if ( (hit & RT_LINE_HANDLER_BEGIN) != 0)
		{
			m_pCurBegin.x = GetPosBegin().x + dx;
			m_pCurBegin.y = GetPosBegin().y + dy;
		}
		if ( (hit & RT_LINE_HANDLER_END) != 0)
		{
			m_pCurEnd.x = GetPosEnd().x + dx;
			m_pCurEnd.y = GetPosEnd().y + dy;
		}

		// Adjust current Tracker size
		AdjustLineTracker(m_pCurBegin, m_pCurEnd, hit);

		// Draw current Tracker
		DrawTracker(dc, m_pCurBegin, m_pCurEnd);

		// Update Parent's Status Bar
		wxCommandEvent evt(wxEVT_TRACKER_CHANGING, m_wnd->GetId());
		m_wnd->GetEventHandler()->ProcessEvent(evt);

		//dc.EndDrawingOnTop();
	}

	// Check there is no abuse mouse capture
	if (!(event.LeftIsDown()) && ((m_state & RT_STATE_MOUSE_CAPTURED) != 0))
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}

	// Update prev_move
	m_prevMove = mouse.GetPosition();
}

void wxLineTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	wxRectTracker::OnMouseLeftDown(event);
	if (HitTest(m_leftClick.x, m_leftClick.y) == RT_LINE_HANDLER_NONE)
	{
		m_pCurBegin = wxPoint(m_leftClick.x, m_leftClick.y);
		m_pCurEnd = m_pCurBegin;
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
	}

	#ifdef __TRACKER_OVERLAY__
	trackerOverlay.Reset();
	#endif

}

void wxLineTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((m_state & RT_STATE_DRAGGING) != 0)
	{
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
		m_state ^= RT_STATE_DRAGGING;
		Update();
	}
}

int wxLineTracker::HitTest(int x, int y) const
{
	wxRect curRect;
	int z = m_iHandlerWidth;
	int hit;

	hit = wxRectTracker::HitTest(x, y);

	// if ( (y < 0) || (y > h) || (x < 0) || (x > w) ) return RT_HANDLER_OUTSIDE;

	if ( hit == m_iEndHandler)		return RT_LINE_HANDLER_END;
	if ( hit == m_iBeginHandler)	return RT_LINE_HANDLER_BEGIN;

	// Distance entre le point et le segment (a = begin ; b = end ; c = x,y )
	int xac = x - GetPosBegin().x; // + m_Rect.GetPosition().x;
	int yac = y - GetPosBegin().y; // + m_Rect.GetPosition().y;
	int xab = GetPosEnd().x - GetPosBegin().x;
	int yab = GetPosEnd().y - GetPosBegin().y;

	if ( (xab != 0) || (yab != 0) )
		if ( ( (((double) - xac * yab + yac * xab ) * ((double)  - xac * yab + yac * xab )) 
			/ ((double) (xab * xab) + (yab * yab))
			) < ((double) z * z) )
			return RT_LINE_HANDLER_ON_LINE;

	return RT_HANDLER_NONE;
}

void wxLineTracker::AdjustLineTracker(wxPoint & begin, wxPoint & end, int handler)
{
	AdjustLineTrackerMax(begin, end, handler);
}

void wxLineTracker::AdjustLineTrackerMax(wxPoint & begin, wxPoint & end, int handler)
{

	// Adjust m_maxRect 
	if ((m_maxRect.width < 0) || (m_maxRect.height < 0)) return;

	if (begin.x < m_maxRect.x) begin.x = m_maxRect.x;
	if (begin.x > m_maxRect.x + m_maxRect.width) begin.x = m_maxRect.x + m_maxRect.width;
	if (begin.y < m_maxRect.y) begin.y = m_maxRect.y;
	if (begin.y > m_maxRect.y + m_maxRect.height) begin.y = m_maxRect.x + m_maxRect.height;

	if (end.x < m_maxRect.x) end.x = m_maxRect.x;
	if (end.x > m_maxRect.x + m_maxRect.width) end.x = m_maxRect.x + m_maxRect.width;
	if (end.y < m_maxRect.y) end.y = m_maxRect.y;
	if (end.y > m_maxRect.y + m_maxRect.height) end.y = m_maxRect.x + m_maxRect.height;

}

void wxLineTracker::SetTrackerPosition(wxPoint begin, wxPoint end)
{
	if (begin.x <= end.x)
	{
		if (begin.y <= end.y)
		{
			m_iBeginHandler = RT_HANDLER_TOP_LEFT;
			m_iEndHandler = RT_HANDLER_BOTTOM_RIGHT;
		}
		else
		{
			m_iBeginHandler = RT_HANDLER_BOTTOM_LEFT;
			m_iEndHandler = RT_HANDLER_TOP_RIGHT;
		}
	}
	else
	{
		if (begin.y <= end.y)
		{
			m_iBeginHandler = RT_HANDLER_TOP_RIGHT;
			m_iEndHandler = RT_HANDLER_BOTTOM_LEFT;
		}
		else
		{
			m_iBeginHandler = RT_HANDLER_BOTTOM_RIGHT;
			m_iEndHandler = RT_HANDLER_TOP_LEFT;
		}
	}
	SetUnscrolledRect(NormalizeRect(wxRect(begin, end)));
}


void wxLineTracker::Update()
{
	m_wnd->Refresh();
	wxCommandEvent evt(wxEVT_TRACKER_CHANGED, m_wnd->GetId());
	m_wnd->GetEventHandler()->ProcessEvent(evt);
}

wxPoint wxLineTracker::GetPosBegin() const
{
	return GetPosHandler(m_iBeginHandler);
}

wxPoint wxLineTracker::GetPosEnd() const
{
	return GetPosHandler(m_iEndHandler);
}

wxPoint wxLineTracker::GetPosHandler(enum RT_HANDLER handler) const
{
	wxPoint pos;
	switch(handler)
	{
	case RT_HANDLER_TOP_MID:		pos = wxPoint(m_Rect.GetWidth() / 2, 0); break;
	case RT_HANDLER_MID_RIGHT:		pos = wxPoint(m_Rect.GetWidth() - 1, m_Rect.GetHeight() / 2); break;
	case RT_HANDLER_BOTTOM_MID:		pos = wxPoint(m_Rect.GetWidth() / 2, m_Rect.GetHeight() - 1); break;
	case RT_HANDLER_MID_LEFT:		pos = wxPoint(0, m_Rect.GetHeight() / 2); break;
	case RT_HANDLER_TOP_LEFT:		pos = wxPoint(0, 0); break;
	case RT_HANDLER_TOP_RIGHT:		pos = wxPoint(m_Rect.GetWidth() - 1, 0); break;
	case RT_HANDLER_BOTTOM_RIGHT:	pos = wxPoint(m_Rect.GetWidth() - 1, m_Rect.GetHeight() - 1); break;
	case RT_HANDLER_BOTTOM_LEFT:	pos = wxPoint(0, m_Rect.GetHeight() - 1); break;
	case RT_HANDLER_OUTSIDE:
	case RT_HANDLER_NONE:
	default:
		return wxPoint(-1,-1);
	}
	return pos + m_Rect.GetPosition();
}
