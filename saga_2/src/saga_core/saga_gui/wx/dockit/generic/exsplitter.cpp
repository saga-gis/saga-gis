/////////////////////////////////////////////////////////////////////////////
// Name:        wxExSplitter.cpp
// Purpose:     wxExSplitter implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: exsplitter.cpp,v 1.2 2006-08-16 18:27:30 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/exsplitter.h>
#include <wx/gdi.h>
#include <wx/dcclient.h>
#include <wx/dockhost.h>

#include <wx/dcscreen.h>

// ----------------------------------------------------------------------------
// wxExSplitter implementation
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxExSplitter, wxWindow )
DEFINE_EVENT_TYPE( wxEVT_SPLITTER_MOVED )

BEGIN_EVENT_TABLE( wxExSplitter, wxWindow )
    EVT_LEFT_DOWN( wxExSplitter::OnLeftDown )
    EVT_MOTION( wxExSplitter::OnMouseMove )
    EVT_LEFT_UP( wxExSplitter::OnLeftUp )
    EVT_ERASE_BACKGROUND( wxExSplitter::OnErase )
    EVT_PAINT( wxExSplitter::OnPaint )
END_EVENT_TABLE()

void wxExSplitter::Init() {
    memset(&pWindow_, 0, sizeof(pWindow_));
    pOwner_ = NULL;
    bounded_ = false;
    dragging_ = false;
    minSize_ = wxMINIMUM_WINDOW_SIZE;
    flags_ = 0x0000;
    startPosition_.x = startPosition_.y = 0;
}

bool wxExSplitter::Create( wxWindow * parent, wxOrientation orientation, wxWindow * win1, wxWindow * win2, unsigned int flags ) {
    wxASSERT(parent);
    bool r = wxWindow::Create( parent, -1, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE );

    // assign
    pOwner_ = parent;
    pWindow_[0] = win1;
    pWindow_[1] = win2;
    orientation_ = orientation;
    flags_ = flags;

    // set the type of cursor we need
    if( orientation_ == wxHORIZONTAL ) {
        SetCursor( g_gdi.GetHSplitCursor() );
    }
    else {
        SetCursor( g_gdi.GetVSplitCursor() );
    }

    return r;
}

void wxExSplitter::SetWindows( wxWindow * pWin1, wxWindow * pWin2 ) {
    // set windows
    pWindow_[0] = pWin1;
    pWindow_[1] = pWin2;
}

void wxExSplitter::SetFlags( unsigned int flags ) {
    flags_ = flags;
}

void wxExSplitter::OnLeftDown( wxMouseEvent& event ) {
    // take origin
    startPosition_ = ClientToScreen( event.GetPosition() );
    dragging_ = true;
    
    // take snapshots
    startRect_[0] = pWindow_[0] ? pWindow_[0]->GetRect() : wxRect();
    startRect_[1] = pWindow_[1] ? pWindow_[1]->GetRect() : wxRect();
    startRectUs_ = GetRect();
    endRectUs_ = startRectUs_;
    
    // check boundaries
    SetBoundaries();

    // draw start position
	lastDrawnRect_ = wxRect();
#ifndef __WXGTK__
	// XXX: work-around for GTK, due to GTK repainting over our initial screen XOR rectangle, it is best for
	// now to not draw the initial XOR rectangle under this platform
    draw( startRectUs_ );
#endif	
    
    // we want all further mouse events
    CaptureMouse();
}

void wxExSplitter::OnMouseMove( wxMouseEvent& event ) {
    if( dragging_ ) {
        wxPoint pos = ClientToScreen( event.GetPosition() );

        if (bounded_) {
            // check for boundary crossing
            if ((pos.x < boundary[0].x) ||
                (pos.x > boundary[1].x) ||
                (pos.y < boundary[0].y) ||
                (pos.y > boundary[1].y)) {
                return;
            }
        }

        wxRect r0 = startRect_[0], r1 = startRect_[1], r = startRectUs_;
        if( orientation_ == wxHORIZONTAL ) {
            // resize in the horizontal
            int dx = pos.x - startPosition_.x;
            if( pWindow_[0] && (r0.width + dx) < minSize_ ) dx = minSize_ - r0.width;
            if( pWindow_[1] && (r1.width - dx) < minSize_ ) dx = r1.width - minSize_;

            r0.width += dx;
            r1.width -= dx;
            r1.x += dx;
            r.x += dx;
        }
        else {
            // resize in the vertical
            int dy = pos.y - startPosition_.y;
            if( pWindow_[0] && (r0.height + dy) < minSize_ ) dy = minSize_ - r0.height;
            if( pWindow_[1] && (r1.height - dy) < minSize_ ) dy = r1.height - minSize_;

            r0.height += dy;
            r1.height -= dy;
            r1.y += dy;
            r.y += dy;
        }
        
        // perform the actual re-sizes
        if( flags_ & wxESF_LIVE_UPDATE ) {
            // update now
            if( pWindow_[0] ) pWindow_[0]->SetSize( r0 );
            if( pWindow_[1] ) pWindow_[1]->SetSize( r1 );
            if( !pWindow_[0] ) {
                // don't alter x/y for right/bottom single edge cases
                r.x = startRectUs_.x;
                r.y = startRectUs_.y;
            }
            SetSize( r );

            // generate size change event
            wxCommandEvent e( wxEVT_SPLITTER_MOVED, GetId() );
            e.SetEventObject( this );
            GetEventHandler()->ProcessEvent( e );
        }
        else {
            // draw now...
			if( lastDrawnRect_.GetWidth() && lastDrawnRect_.GetHeight() ) {
            	draw( lastDrawnRect_ );
			}
            draw( r );
            
            // ...update later
            endRect_[0] = r0;
            endRect_[1] = r1;
            endRectUs_ = r;
            if( !pWindow_[0] ) {
                // don't alter x/y for right/bottom single edge cases
                endRectUs_.x = startRectUs_.x;
                endRectUs_.y = startRectUs_.y;
            }
        }
    }
}

void wxExSplitter::OnErase( wxEraseEvent& WXUNUSED(event) ) {
    // skip erase
}

void wxExSplitter::OnPaint( wxPaintEvent& WXUNUSED(event) ) {
    wxPaintDC dc(this);
 
    wxRect cr = GetClientRect();
	
    // draw physical splitter image
	if( flags_ & wxESF_DRAW_GRIPPER ) {
		g_gdi.DrawSplitter( dc, cr, orientation_ );
	}
    else {
		g_gdi.DrawBackground( dc, cr );
    }
}

void wxExSplitter::OnLeftUp( wxMouseEvent& WXUNUSED(event) ) {
    if( !dragging_ ) return;

    // release mouse
    ReleaseMouse();
    dragging_ = false;

    // need to update here?
    if( !(flags_ & wxESF_LIVE_UPDATE) ) {
        // erase end position
        draw( lastDrawnRect_ );

        if( endRectUs_.GetWidth() || endRectUs_.GetHeight() ) {
            // update associated windows
            for( int i=0; i<wxSPLITTER_WINDOWS; i++ ) {
                // make sure window pointer valid
                if( pWindow_[i] ) {
                    // make sure new size rectangle valid
                    if( endRect_[i].GetWidth() || endRect_[i].GetHeight() ) {
                        pWindow_[i]->SetSize( endRect_[i] );
                    }
                }
            }
            SetSize( endRectUs_ );        
        }
    }

    // generate size change event
    wxCommandEvent e(wxEVT_SPLITTER_MOVED, GetId());
    e.SetEventObject(this);
    GetEventHandler()->ProcessEvent(e);
}

void wxExSplitter::draw( wxRect r ) {
    if( flags_ & wxESF_LIVE_UPDATE ) return;
    
    // draw our position onto the screen [XOR]
    wxScreenDC dc;

    wxRect screenRect = r;
    screenRect.SetPosition( pOwner_->ClientToScreen( screenRect.GetPosition() ) );
    g_gdi.DrawPattern( dc, screenRect );
    lastDrawnRect_ = r;
}

void wxExSplitter::SetBoundaries() {
    bounded_ = false;

    if ((pWindow_[0] && pWindow_[1]) || (!pWindow_[0] && !pWindow_[1])) {
        return;
    }

    wxDockHost* dh = wxDynamicCast(pOwner_, wxDockHost);

    if (!dh || (dh -> GetSizingSplitter() != this)) {
        return;
    }

    bounded_ = true;

    wxWindow* parent = dh -> GetParent();
    wxLayoutManager* pLayoutManager = dh -> GetLayoutManager();
    wxSize clientSize = parent -> GetClientSize();

    if (pLayoutManager) {
        wxRect dockArea = pLayoutManager->GetDockArea();
        boundary[0] = dockArea.GetPosition();
        clientSize = dockArea.GetSize();
    }
    else {
        boundary[0] = wxPoint(0, 0);
        clientSize = parent->GetClientSize();
    }
    boundary[0] = parent->ClientToScreen(boundary[0]);

    boundary[0].x += wxSPLITTER_SIZE;
    boundary[0].y += wxSPLITTER_SIZE;

    boundary[1] = boundary[0];

    boundary[1].x += (clientSize.GetWidth() - (wxSPLITTER_SIZE * 2));
    boundary[1].y += (clientSize.GetHeight() - (wxSPLITTER_SIZE * 2));

    if (!pLayoutManager) {
        return;
    }

    // search other dockhosts with this orientation
    wxDirection ldir = wxALL;

    switch (dh -> GetDirection()) {
    case wxLEFT   :
        ldir = wxRIGHT;  break;
    case wxRIGHT  :
        ldir = wxLEFT;   break;
    case wxTOP    :
        ldir = wxBOTTOM; break;
    case wxBOTTOM :
        ldir = wxTOP;    break;
    default       :
        ldir = wxALL;    break;
    }

    if (ldir == wxALL) {
        return;
    }

    wxDockHost* dockhost = pLayoutManager -> GetDockHost(ldir);
    if (dockhost && !dockhost -> IsEmpty()) {
        wxRect hr = dockhost -> GetScreenArea();

        switch (ldir) {
        case wxLEFT   :
            boundary[0].x = hr.GetRight() + wxSPLITTER_SIZE; break;
        case wxRIGHT  :
            boundary[1].x = hr.GetLeft() - wxSPLITTER_SIZE; break;
        case wxTOP    :
            boundary[0].y = hr.GetBottom() + wxSPLITTER_SIZE; break;
        case wxBOTTOM :
            boundary[1].y = hr.GetTop() - wxSPLITTER_SIZE; break;
        case wxALL    :
            wxASSERT(false); break;
        }
    }
}
