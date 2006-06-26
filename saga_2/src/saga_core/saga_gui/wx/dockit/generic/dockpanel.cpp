/////////////////////////////////////////////////////////////////////////////
// Name:        wxDockPanel.cpp
// Purpose:     wxDockPanel implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockpanel.cpp,v 1.1.1.1 2006-06-26 15:43:33 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/dockpanel.h>
#include <wx/dockhost.h>
#include <wx/dockwindow.h>
#include <wx/toolbutton.h>
#include <wx/gdi.h>
#include <wx/pane.h>
#include <wx/gripper.h>

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/settings.h>

// ----------------------------------------------------------------------------
// wxDockPanel constants & wx-macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxDockPanel, wxPanel )

BEGIN_EVENT_TABLE( wxDockPanel, wxPanel )
    EVT_SIZE( wxDockPanel::OnSize )
    EVT_BUTTON( 0, wxDockPanel::OnPaneClose )
    EVT_PANE_CLOSED( wxDockPanel::OnPaneClose )
    EVT_GRIP_DBLCLICK( wxDockPanel::OnGripDblClick )
    EVT_GRIP_LEFTDOWN( wxDockPanel::OnGripLeftDown )
    EVT_GRIP_LEFTUP( wxDockPanel::OnGripLeftUp )
#ifdef __WXGTK__
    EVT_LEFT_UP( wxDockPanel::OnLeftUp )  
    EVT_MOTION( wxDockPanel::OnMouseMove )
#endif    
END_EVENT_TABLE()

const unsigned int gripperSize = 14;
const unsigned int closeSize = 13;
const unsigned int closeBorder = 1;

// ----------------------------------------------------------------------------
// wxDockPanel implementation
// ----------------------------------------------------------------------------

void wxDockPanel::Init() {
    docked_ = false;
    mouseCaptured_ = false;
    pDockWindow_ = NULL;
    pDockHost_ = NULL;
    pClient_ = NULL;
    pStockClient_ = NULL;
    pPane_ = NULL;
    pClientSizer_ = NULL;
    pGripWindow_ = NULL;
    pCloseButton_ = NULL;
    area_ = 0;
    lockAreaValue_ = false;
    flags_ = 0x0000;
}

bool wxDockPanel::Create( wxWindow *parent, wxWindowID id, const wxString& name, unsigned int flags ) {
    wxASSERT(parent);
    // create ourselves
    bool r =  wxPanel::Create( parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxFULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN, name );
    flags_ = flags;
    
    // create default client area
    pClient_ = new wxPanel( this, 0, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN );
    pStockClient_ = pClient_;
    
    // create grip window
    pGripWindow_ = new wxGripWindow( this, GetOrientation(), wxGdi::wxGRIP_STYLE_HEADER );
    pGripWindow_->SetLabel( parent->GetTitle() );
    
    // create close window
    pCloseButton_ = new wxToolButton( this, 0 );
    pCloseButton_->SetDrawMargin( 1 );
    pCloseButton_->SetToolTip( wxT("Close") );

    return r;
}

void wxDockPanel::SetDockWindow( wxDockWindowBase * pOwner ) {
    pDockWindow_ = pOwner;
    
    // assign the cursor for hover over the gripper
    pGripWindow_->SetCursor( g_gdi.GetMoveCursor() );
}

wxDockWindowBase * wxDockPanel::GetDockWindow() {
    return pDockWindow_;
}

void wxDockPanel::SetDockedHost( wxDockHost * pDockHost ) {
    if( !pDockHost && pDockHost_ ) {
        // call update on host before clear
        pDockHost_->UpdateSize();
    }
    pDockHost_ = pDockHost;
    
    // internal update
    docked_ = pDockHost ? true : false;

    if( pDockHost ) {
        // call update on new host
        pDockHost->UpdateSize();
    }

    childUpdate();
}

wxDockHost * wxDockPanel::GetDockedHost() {
    return pDockHost_;
}

void wxDockPanel::OnSize( wxSizeEvent& WXUNUSED(event) ) {
    UpdateSize();
}

void wxDockPanel::OnPaneClose( wxCommandEvent& WXUNUSED(event) ) {
    wxASSERT(pDockWindow_);
    pDockWindow_->Remove();

    // Run the update to fix the menu
    wxLayoutManager * pLayoutManager = pDockWindow_->GetLayoutManager();
    wxASSERT(pLayoutManager);
    pLayoutManager->UpdateAllHosts( false );
}

void wxDockPanel::OnGripDblClick( wxMouseEvent& WXUNUSED(event) ) { 
    // in gripper area
    wxDockWindow * pDockWindow = static_cast<wxDockWindow *>(GetDockWindow());
    wxASSERT(pDockWindow);
    wxLayoutManager * pLayoutManager = pDockWindow->GetLayoutManager();
    wxASSERT(pLayoutManager);
    
    // undock the panel back onto the owner window
    pLayoutManager->UndockWindow( GetDockWindow() );
}

void wxDockPanel::OnGripLeftDown( wxMouseEvent& event ) { 
    wxDockWindow * pDockWindow = static_cast<wxDockWindow *>(GetDockWindow());
    wxASSERT(pDockWindow);

    // start dragging when docked
    wxPoint curPoint( event.GetPosition() );
    curPoint = ClientToScreen( curPoint );
#ifdef __WXGTK__
    if( docked_ ) {
        pDockWindow->StartDragging( curPoint.x, curPoint.y, false );
        CaptureMouse();
    }
    else
#endif    
    pDockWindow->StartDragging( curPoint.x, curPoint.y );
}

void wxDockPanel::OnGripLeftUp( wxMouseEvent& WXUNUSED(event) ) { 
    wxDockWindowBase * pDockWindow = GetDockWindow();
    wxASSERT(pDockWindow);

#ifdef __WXGTK__
    if( docked_ ) {
        ReleaseMouse();
        pDockWindow->StopDragging( false );
    }
    else
#endif    
    pDockWindow->StopDragging();
}

#ifdef __WXGTK__
void wxDockPanel::OnMouseMove( wxMouseEvent& event ) {
    // forward mouse move messages to owner
    ClientToScreen( &event.m_x, &event.m_y );
    pDockWindow_->ScreenToClient( &event.m_x, &event.m_y );
    pDockWindow_->OnMouseMove( event );
}

void wxDockPanel::OnLeftUp( wxMouseEvent& event ) {
    // stopping any dragging
    bool needrelease = wxWindow::GetCapture() == pDockWindow_;
    pDockWindow_->StopDragging(needrelease);
}
#endif    

void wxDockPanel::UpdateSize() {
    wxWindowList & children = GetChildren();
    if( children.GetCount() == 0 ) {
        return;
    }
   
    int tGripperSize = gripperSize;
    int tCloseSize = closeSize;

    wxOrientation orientation = GetOrientation();

    // layout
#if __WXGTK__
    if( docked_ && (pPane_ || flags_ & wxDPC_NO_CONTROLS) ) {
        // docked and pane/no controls required
#else
    if( !docked_ || pPane_ || (flags_ & wxDPC_NO_CONTROLS) ) {
#endif
        // undocked/with pane control/no controls required
        wxRect cr = GetClientRect();
        pClient_->SetSize( 0, 0, cr.width, cr.height );
        pClient_->Layout();
        pGripWindow_->SetSize( wxSize( 0,0 ) );
        pCloseButton_->SetSize( wxSize( 0,0 ) );
        }
    else {
#if __WXGTK__
        if( !docked_ ) {
            // undocked - show header
            orientation = wxVERTICAL;
        }
#endif
        // docked/or with internal controls
        wxRect cr = GetClientRect();
        if( orientation == wxHORIZONTAL ) {
            pClient_->SetSize( tGripperSize, 0, cr.width-tGripperSize, cr.height );
            pClient_->Layout();
            pGripWindow_->SetSize( 0, tCloseSize + closeBorder, tGripperSize, cr.height - tCloseSize - closeBorder );
            pGripWindow_->SetOrientation( wxHORIZONTAL );
            pCloseButton_->SetSize( 0, closeBorder, tGripperSize, tCloseSize );
        }
        else {
            pClient_->SetSize( 0, tGripperSize, cr.width, cr.height-tGripperSize );
            pClient_->Layout();
            pGripWindow_->SetSize( 0, 0, cr.width - tCloseSize - closeBorder, tGripperSize );
            pGripWindow_->SetOrientation( wxVERTICAL );
            pCloseButton_->SetSize( cr.width - tCloseSize - closeBorder, 0, tCloseSize, tGripperSize );
        }
    }
}

void wxDockPanel::SetClient( wxWindow * pClient, bool autoPane ) {
    if (pClient == pClient_) {
        // ignore if we already have the given client as our client
        return;
    }

    // if we do nothing else with it, at least hide the old client
    if( pClient_ ) {
        pClient_->Hide();
    }

    // replace current client
    if( !pClient ) {
        // remove user client
        wxASSERT(pStockClient_);
        pClient_ = pStockClient_;
    }
    else {
        wxPaneBase * pPotentialPane = (wxPane *)wxDynamicCast( pClient, wxPaneBase );
        if( autoPane && !pPotentialPane ) {
            // automatically add a pane client and then add the actual client to it (of course don't want to 'AutoPane'
            // actual pane controls)
            wxPane * pAutoPane = new wxPane( this, -1, pDockWindow_->GetTitle() );
            pClient->Reparent( pAutoPane );
            pAutoPane->SetClient( pClient );
            pClient_ = pAutoPane;
        }
        else {
            // just add the client normally
            pClient->Reparent( this );
            pClient_ = pClient;
        }
    }
    wxASSERT(pClient_);
    pClient_->Show();

    // we know about wxPane clients
    pPane_ = (wxPane *)wxDynamicCast( pClient_, wxPaneBase );
    if( pPane_ ) {
        // size the pane to our client area
        wxSize size = GetClientSize();
        pPane_->SetSize( size );
    }

    childUpdate();

    UpdateSize();
}

wxWindow * wxDockPanel::GetClient() {
    wxASSERT(pClient_);
    return pClient_;
}

void wxDockPanel::AutoFitSingleChild() {
    if( pClientSizer_ ) return;    // already done
    
    // create simple 'expand all' sizer
    pClientSizer_ = new wxBoxSizer( wxHORIZONTAL );
    pClient_->SetSizer( pClientSizer_ );
    pClient_->SetAutoLayout( true );
    
    // apply to client child
    wxWindowList & children = pClient_->GetChildren();
    if( children.GetCount() != 1 ) {
        wxASSERT_MSG( false, wxT("AutoFitSingleChild() only works when there is a single child!") );
    }
    wxWindowListNode * pChildNode = children.GetFirst();
    wxASSERT(pChildNode);
    wxWindow * pChild = pChildNode->GetData();
    wxASSERT(pChild);
    pClientSizer_->Add( pChild, 1, wxGROW|wxALL, 0 );
    pClient_->Layout();
}

wxOrientation wxDockPanel::GetOrientation() {
    // get from host
    if( pDockHost_ ) {
        return pDockHost_->GetOrientation();
    }
    
    // getOrientation() makes no sense when panel is not docked!
    return wxHORIZONTAL;
}

wxRect wxDockPanel::GetScreenArea() {
    // get the screen area for this panel
    wxASSERT(pDockHost_);
    wxRect r = GetRect();
    
    return pDockHost_->RectToScreen( r );
}

wxRect wxDockPanel::GetScreenArea( wxHostInfo &hi ) {
    wxRect screenRect = GetScreenArea();
    if( hi.GetPlacement() == wxHIP_NONE ) {
        // return all panel area
        return screenRect;
    }
    
    if( GetOrientation() == wxHORIZONTAL ) {
        // return a horizontal area
        screenRect.width /= 2;
        if( hi.GetPlacement() == wxHIP_BACK ) {
            screenRect.x += screenRect.width;
        }
    }
    else {
        // return a vertical area
        screenRect.height /= 2;
        if( hi.GetPlacement() == wxHIP_BACK ) {
            screenRect.y += screenRect.height;
        }
    }
    return screenRect;
}

wxPlacement wxDockPanel::TestForPlacement( int sx, int sy ) {
    wxPlacement placement = wxHIP_NONE;
    wxRect screenRect = GetScreenArea();
    
    // work out which half of the panel a screen point is in
    if( GetOrientation() == wxHORIZONTAL ) {
        screenRect.width /= 2;
        placement = screenRect.Inside( sx, sy ) ? wxHIP_FRONT : wxHIP_BACK;
    }
    else {
        screenRect.height /= 2;
        placement = screenRect.Inside( sx, sy ) ? wxHIP_FRONT : wxHIP_BACK;
    }
    return placement;
}

int wxDockPanel::GetArea() {
    return area_;
}

void wxDockPanel::SetArea( int area ) {
    if( lockAreaValue_ ) {
        return;
    }
    area_ = area;
}

void wxDockPanel::LockAreaValue( bool state ) {
    lockAreaValue_ = state;
}

bool wxDockPanel::IsDocked() {
    return docked_;
}

void wxDockPanel::childUpdate() {
    // we know about wxPane clients
    if( pPane_ ) {
        pPane_->ShowHeader( docked_ ? true : false );
        pPane_->SetOrientation( GetOrientation() == wxVERTICAL ? wxHORIZONTAL : wxVERTICAL );
    }
}
