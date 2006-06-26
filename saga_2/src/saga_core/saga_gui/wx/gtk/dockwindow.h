///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dockwindow_msw.h
// Purpose:     wxDockWindow GTK class
// Author:      Mark McCormack
// Modified by:
// Created:     23/10/04
// RCS-ID:      $Id: dockwindow.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCKWINDOW_H_
#define _WX_DOCKWINDOW_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/dockwindow.h>

// ----------------------------------------------------------------------------
// wxDockWindow
// ----------------------------------------------------------------------------
#include <wx/sizer.h>
class WXDLLEXPORT wxDockWindow : public wxDockWindowBase
{
public:
    wxDockWindow() {
        Init();
    }
    
    wxDockWindow( wxWindow * parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxString& name = wxT("frame"), unsigned int flags = wxDWC_DEFAULT ) {
        Init();
        Create( parent, id, title, pos, size, name, flags );
    }

    // basic interface
    bool Create( wxWindow * parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxString& name = wxT("dockwindow"), unsigned int flags = wxDWC_DEFAULT );
	
	// events
	void OnLeftDown( wxMouseEvent& event );
	void OnLeftUp( wxMouseEvent& event );
    void OnMouseMove( wxMouseEvent& event );
    void OnMouseLeave( wxMouseEvent& event );
	void OnKeyDown( wxKeyEvent& event );
	void OnKeyUp( wxKeyEvent& event );
    
    // platform
    virtual bool BlockDocking();

private:
    enum eDragHandle {
        HDL_NULL,
        HDL_TL,
        HDL_T,
        HDL_TR,
        HDL_R,
        HDL_BR,
        HDL_B,
        HDL_BL,
        HDL_L
    };

    void addBorder();
    eDragHandle findHandle( int x, int y );
    void setCursor( eDragHandle dragHandle );

    bool sizing_;
    eDragHandle sizingDragHandle_;
	wxSize startSize_;
	wxPoint startPlace_;
	wxPoint startPos_;
	wxPoint placeDiff_;
	wxRect newRect_;
	wxRect prevRect_;
	bool hSize_;
	bool vSize_;
	bool hInvert_;
	bool vInvert_;
	bool blockDocking_;
	
    DECLARE_CLASS( wxDockWindow )
    DECLARE_EVENT_TABLE()
};
#endif
    // _WX_DOCKWINDOW_H_
