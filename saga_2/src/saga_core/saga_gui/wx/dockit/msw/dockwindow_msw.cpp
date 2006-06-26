/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dockwindow_msw.cpp
// Purpose:     wxDockWindow MSW implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockwindow_msw.cpp,v 1.1.1.1 2006-06-26 15:43:36 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/msw/dockwindow.h>
#include <wx/msw/private.h>

static HHOOK draghook = 0;
static BOOL fControl = FALSE;
static HWND dragHwnd = 0;

#define WM_DRAG_UPDATE (WM_USER + 0x0001)    // custom event to force an update whilst dragging

// This keyboard hook lets the user block docking by monitoring the <control> key
static LRESULT CALLBACK DragHookProc( int code, WPARAM wParam, LPARAM lParam )
{
    ULONG state = (ULONG)lParam;

    if( code < 0 ) {
        return CallNextHookEx( draghook, code, wParam, lParam );
    }

    if( wParam == VK_CONTROL ) {
        if( state & 0x80000000 ) {
            fControl = FALSE;
        }
        else {
            fControl = TRUE;
        }

        SendMessage( dragHwnd, WM_DRAG_UPDATE, 0, 0 );
        return -1;
    }

    if( wParam == VK_ESCAPE ) {
        PostMessage( dragHwnd, WM_CANCELMODE, 0, 0 );
        return 0;
    }

    return CallNextHookEx( draghook, code, wParam, lParam );
}

// ----------------------------------------------------------------------------

void wxDockWindow::StartDragging( int x, int y ) {
    // enable the keyboard hook
    dragHwnd = GetHwndOf(this);
    draghook = SetWindowsHookEx( WH_KEYBOARD, DragHookProc, GetModuleHandle(0), 0 ); 

    wxDockWindowBase::StartDragging( x, y );
}

void wxDockWindow::StopDragging() {
	// remove the keyboard hook
	if( draghook ) {
		UnhookWindowsHookEx( draghook );
		draghook = 0;
        dragHwnd = 0;
        fControl = FALSE;
	}

    wxDockWindowBase::StopDragging();
}

bool wxDockWindow::BlockDocking() {
    return fControl ? true : false;
}

long wxDockWindow::MSWWindowProc( WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam ) {
    switch( nMsg ) {
        case WM_NCLBUTTONDBLCLK:
		    // prevent standard double-click on the caption area
		    applyLastDock();
            return 0;
            
        case WM_NCLBUTTONDOWN:
        {
		    // prevent standard dragging by the caption area
		    if( wParam == HTCAPTION ) {
		        // make the window activate
		        SetWindowPos( GetHwndOf(this), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
		        
		        // start dragging
		        PPOINTS pPoint = (PPOINTS)&lParam;
		        int x = pPoint->x;
		        int y = pPoint->y;
		        StartDragging( x, y );

		        return 0;
            }
            break;
        }

        case WM_DRAG_UPDATE:
        {
            RepeatLastMouseEvent();
            break;
        }
        
        case WM_NCLBUTTONUP:
        case WM_LBUTTONUP:
        case WM_CANCELMODE:
        {
            // cancel dragging?
            if( dragging_ ) {
                StopDragging();
            }
        }
        break;
    }
    
    // as normal
    return wxDockWindowBase::MSWWindowProc( nMsg, wParam, lParam );    
}
