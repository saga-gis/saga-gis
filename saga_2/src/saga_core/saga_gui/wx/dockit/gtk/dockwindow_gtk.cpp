/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/dockwindow_msw.cpp
// Purpose:     wxDockWindow GTK implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/10/04
// RCS-ID:      $Id: dockwindow_gtk.cpp,v 1.1.1.1 2006-06-26 15:43:36 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/gtk/dockwindow.h>
#include <wx/dockpanel.h>

#include <wx/sizer.h>
#include <wx/dcscreen.h>

IMPLEMENT_CLASS( wxDockWindow, wxDockWindowBase )

BEGIN_EVENT_TABLE( wxDockWindow, wxDockWindowBase )
    EVT_LEFT_DOWN( wxDockWindow::OnLeftDown )
    EVT_LEFT_UP( wxDockWindow::OnLeftUp )  
    EVT_MOTION( wxDockWindow::OnMouseMove )
    EVT_LEAVE_WINDOW( wxDockWindow::OnMouseLeave )
	EVT_KEY_DOWN( wxDockWindow::OnKeyDown )
	EVT_KEY_UP( wxDockWindow::OnKeyUp )
END_EVENT_TABLE()

const int BORDER_SIZE = 4;
const int CORNER_SIZE = 8;

bool wxDockWindow::Create( wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& name, unsigned int flags ) {
    wxASSERT(parent);
    if( !wxDockWindowBase::Create( parent, id, title, pos, size, name, flags ) ) {
        return false;
    }
    SetBackgroundColour(GetBackgroundColour()); // XXX: unsure why we have to do this?

    // init
	hInvert_ = false;
	vInvert_ = false;
	hSize_ = false;
	vSize_ = false;
    sizing_= false;
	blockDocking_ = false;
    sizingDragHandle_ = HDL_NULL;

    // force normal cursor on client
    wxDockPanel * pDockPanel = GetDockPanel();
    wxASSERT(pDockPanel);
    pDockPanel->SetCursor(*wxSTANDARD_CURSOR);

    addBorder();

    return true;
}

void wxDockWindow::OnLeftDown( wxMouseEvent& event ) {
	if( dragging_ ) {
		event.Skip();
		return;
	}

    // start sizing
	startSize_ = GetSize();
	startPlace_ = GetPosition();
	startPos_ = event.GetPosition();
	newRect_ = wxRect(0,0,0,0);
	prevRect_ = wxRect(0,0,0,0);
    sizingDragHandle_ = findHandle( event.GetX(), event.GetY() );
	setCursor( sizingDragHandle_ );
    sizing_ = true;
    CaptureMouse();

	// draw initial frame	
	wxScreenDC dc;
	prevRect_ = GetRect();
	g_gdi.DrawFrame( dc, prevRect_, false );
}

void wxDockWindow::OnLeftUp( wxMouseEvent& event ) {
	if( dragging_ ) {
		StopDragging();
		return;
	}

    // stop sizing
    ReleaseMouse();
    sizing_ = false;
	startSize_ = wxSize(0,0);
	startPlace_ = wxPoint(0,0);
	startPos_ = wxPoint(0,0);
    sizingDragHandle_ = HDL_NULL;
	setCursor( sizingDragHandle_ );

	// undraw frame	
	wxScreenDC dc;
	g_gdi.DrawFrame( dc, prevRect_, false );

	// set new placement (if required)
	if( newRect_.GetWidth() && newRect_.GetHeight() ) {
		SetSize( newRect_ );
	}

	// finish
	newRect_ = wxRect(0,0,0,0);
	prevRect_ = wxRect(0,0,0,0);
}

void wxDockWindow::OnMouseMove( wxMouseEvent& event ) {
    if( !dragging_ && !sizing_ ) {
        // when not dragging, set hover cursor
        eDragHandle dragHandle = findHandle( event.GetX(), event.GetY() );
        setCursor( dragHandle );
    }
	
	if( sizing_ ) {
		int x = event.GetX();
		int y = event.GetY();
		
		// perform resizing
		wxPoint newPlace( startPlace_ );
		wxSize newSize( startSize_ );
		if( hSize_ ) {
			if( hInvert_ ) {
				int amount = (x - startPos_.x);
				newSize.x -= amount;
				newPlace.x += amount;
			}
			else {
				newSize.x += (x - startPos_.x);
			}
		}
		if( vSize_ ) {
			if( vInvert_ ) {
				int amount = (y - startPos_.y);
				newSize.y -= amount;
				newPlace.y += amount;
			}
			else {
				newSize.y += (y - startPos_.y);
			}
		}
		
		// undraw/draw new frame	
		wxScreenDC dc;
		g_gdi.DrawFrame( dc, prevRect_, false );
		g_gdi.DrawFrame( dc, newRect_, false );
		prevRect_ = newRect_;
		
		// set new placement
		newRect_ = wxRect( newPlace.x, newPlace.y, newSize.x, newSize.y );
	}
	
	event.Skip();
}

void wxDockWindow::OnMouseLeave( wxMouseEvent& WXUNUSED(event) ) {
    if( !dragging_ && !sizing_ ) {
        // when not dragging, reset hover cursor
        setCursor( HDL_NULL );
    }
}

void wxDockWindow::OnKeyDown( wxKeyEvent& event ) {
	if( event.GetKeyCode() == WXK_CONTROL ) {
		blockDocking_ = true;
	}
	RepeatLastMouseEvent();
	event.Skip();
}

void wxDockWindow::OnKeyUp( wxKeyEvent& event ) {
	// XXX: not sure why this event does not appear to be getting generated?
	if( event.GetKeyCode() == WXK_CONTROL ) {
		blockDocking_ = false;
	}
	RepeatLastMouseEvent();
	event.Skip();
}

bool wxDockWindow::BlockDocking() {
    // TODO: gtk implementation
    return blockDocking_;
}

void wxDockWindow::addBorder() {
    // move the windows contents in by an amount
    wxSizer * sizer = GetSizer();
    wxSizerItemList& sizerList = sizer->GetChildren();
    wxwxSizerItemListNode * pNode = sizerList.GetFirst();
    wxASSERT(pNode);
    wxSizerItem * pSizerItem = pNode->GetData();
    wxASSERT(pSizerItem);
    pSizerItem->SetBorder( BORDER_SIZE );
    pSizerItem->SetFlag( pSizerItem->GetFlag() | wxALL );
    Layout();
}

wxDockWindow::eDragHandle wxDockWindow::findHandle( int x, int y ) {
    wxSize size = GetClientSize();
    int halfX = size.x/2;
    int halfY = size.y/2;

    // work out quad area
    bool top, left;
    top = false;
    left = false;
    if( x < halfX ) {
        left = true;
    }
    if( y < halfY ) {
        top = true;
    }

    // work out corners
    int clx = CORNER_SIZE;
    int crx = size.x - CORNER_SIZE;
    int cty = CORNER_SIZE;
    int cby = size.y - CORNER_SIZE;
	hSize_ = false;
	vSize_ = false;
	hInvert_ = false;
	vInvert_ = false;
	
    if( left ) {
        // left half
        if( top ) {
            // top quad
            if( x < clx ) {
				hSize_ = true;
				hInvert_ = true;
                if( y < cty ) {
					vSize_ = true;
					vInvert_ = true;
                    return HDL_TL;
                }
                else {
                    return HDL_L;
                }
            }
            else {
				vInvert_ = true;
				vSize_ = true;
                return HDL_T;
            }
        }
        else {
            // bottom quad
            if( x < clx ) {
				hSize_ = true;
				hInvert_ = true;
                if( y > cby ) {
					vSize_ = true;
                    return HDL_BL;
                }
                else {
                    return HDL_L;
                }
            }
            else {
				vSize_ = true;
                return HDL_B;
            }
        }
    }
    else {
        // right half
        if( top ) {
            // top quad
            if( x > crx ) {
				hSize_ = true;
                if( y < cty ) {
					vSize_ = true;
					vInvert_ = true;
                    return HDL_TR;
                }
                else {
                    return HDL_R;
                }
            }
            else {
				vSize_ = true;
				vInvert_ = true;
                return HDL_T;
            }
        }
        else {
            // bottom quad
            if( x > crx ) {
				hSize_ = true;
                if( y > cby ) {
					vSize_ = true;
                    return HDL_BR;
                }
                else {
                    return HDL_R;
                }
            }
            else {
				vSize_ = true;
                return HDL_B;
            }
        }
    }
}

void wxDockWindow::setCursor( eDragHandle dragHandle ) {
    // set cursor depending on drag-handle
    int cursorId = wxCURSOR_ARROW;
    switch( dragHandle ) {
        case HDL_TL: cursorId = wxCURSOR_SIZENWSE; break;
        case HDL_L: cursorId = wxCURSOR_SIZEWE; break;
        case HDL_T: cursorId = wxCURSOR_SIZENS; break;
        case HDL_BL: cursorId = wxCURSOR_SIZENESW; break;
        case HDL_TR: cursorId = wxCURSOR_SIZENESW; break;
        case HDL_R: cursorId = wxCURSOR_SIZEWE; break;
        case HDL_B: cursorId = wxCURSOR_SIZENS; break;
        case HDL_BR: cursorId = wxCURSOR_SIZENWSE; break;
		case HDL_NULL: break;
    }
    wxCursor cursor(cursorId);
    SetCursor(cursor);
}
