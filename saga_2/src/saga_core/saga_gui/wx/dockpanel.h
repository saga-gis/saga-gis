///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dockpanel.h
// Purpose:     wxDockPanel class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockpanel.h,v 1.1.1.1 2006-06-26 15:43:05 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCKPANEL_H_
#define _WX_DOCKPANEL_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/panel.h>

#include <wx/layoutmanager.h>
#include <wx/gdi.h>

#include "wx/export.h"

class wxDockWindowBase;
class wxDockHost;
class wxBoxSizer;
class wxPaneEvent;
class wxPane;
class wxToolButton;
class wxGripWindow;

// ----------------------------------------------------------------------------
// wxDockPanel
// ----------------------------------------------------------------------------

#define wxDPC_NO_CONTROLS   0x0001
#define wxDPC_DEFAULT       0x0000

class WXDLLIMPEXP_DI wxDockPanel : public wxPanel
{
    DECLARE_DYNAMIC_CLASS( wxDockPanel )
    DECLARE_EVENT_TABLE()

public:
    wxDockPanel() {
        Init();
    }
    void Init();
    
    wxDockPanel( wxWindow *parent, wxWindowID id, 
		const wxString& name = wxT("dockpanel"), 
		unsigned int flags = wxDPC_DEFAULT ) 
	{
        Init();
        Create( parent, id, name, flags );
    }
    
    // basic interface
    bool Create( wxWindow * parent, wxWindowID id,
		const wxString& name = wxT("dockpanel"), 
		unsigned int flags = wxDPC_DEFAULT );

    void UpdateSize();
    
    void SetDockWindow( wxDockWindowBase * pOwner );
    wxDockWindowBase * GetDockWindow();
    
    void SetDockedHost( wxDockHost * pDockHost );
    wxDockHost * GetDockedHost();
    
    void SetClient( wxWindow * pClient, bool autoPane = false );
    wxWindow * GetClient();

    void AutoFitSingleChild();
    
    // access
    wxOrientation GetOrientation();
    wxRect GetScreenArea();
    wxRect GetScreenArea( wxHostInfo &hi );
    wxPlacement TestForPlacement( int sx, int sy );
    
    int GetArea();
    void SetArea( int area );
    void LockAreaValue( bool state );
    
    bool IsDocked();
    
    // event handlers
    void OnSize( wxSizeEvent& event );
    void OnPaneClose( wxCommandEvent& event );
    void OnGripDblClick( wxMouseEvent& event );
    void OnGripLeftDown( wxMouseEvent& event );
    void OnGripLeftUp( wxMouseEvent& event );
#ifdef __WXGTK__    
	void OnMouseMove( wxMouseEvent& event );
    void OnLeftUp( wxMouseEvent& event );
#endif	

private:
    void childUpdate();

private:
    unsigned int flags_;

    bool docked_;           // are we docked?
    bool mouseCaptured_;

    wxGripWindow * pGripWindow_;   // the grip window
    wxToolButton * pCloseButton_;  // the close button
    
    wxWindow * pClient_;           // the actual user area
    wxWindow * pStockClient_;      // the initial user area (default)
    wxPane * pPane_;               // pane client (only if client is a pane)
    wxBoxSizer * pClientSizer_;    // the sizer for the client panel
    
    wxDockWindowBase * pDockWindow_; // our dock window
    wxDockHost * pDockHost_;    // the host (when docked)

    int area_;
    bool lockAreaValue_;
};
#endif
    // _WX_DOCKPANEL_H_
