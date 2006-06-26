/////////////////////////////////////////////////////////////////////////////
// Name:        wxDockWindow.cpp
// Purpose:     wxDockWindowBase implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockwindow.cpp,v 1.1.1.1 2006-06-26 15:43:34 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/dockwindow.h>
#include <wx/dockpanel.h>
#include <wx/dockhost.h>
#include <wx/gdi.h>

#include <wx/dcscreen.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>

// ----------------------------------------------------------------------------
// wxDockWindowBase constants & wx-macros
// ----------------------------------------------------------------------------

const long dockWindowStyle = (wxDEFAULT_FRAME_STYLE /*| wxFRAME_TOOL_WINDOW*/ | wxFRAME_FLOAT_ON_PARENT);

IMPLEMENT_CLASS( wxDockWindowBase, wxMiniFrame )

BEGIN_EVENT_TABLE( wxDockWindowBase, wxMiniFrame )
    EVT_MOTION( wxDockWindowBase::OnMouseMove )
    EVT_CLOSE( wxDockWindowBase::OnClose )  
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxDockWindowBase implementation
// ----------------------------------------------------------------------------

void wxDockWindowBase::Init() {
    dragging_ = false;
    haveMoved_ = false;
    startRect_.Reset();
    prevRect_.Reset();
    dragRect_.Reset();
    startPoint_.x = 0;
    startPoint_.y = 0;
    newHost_.Reset();
    prevHost_.Reset();
    pLayoutManager_ = NULL;
    pClientPanel_ = NULL;
    docked_ = false;
    flags_= 0x0000;
    disableShowOverride_ = false;
}

bool wxDockWindowBase::Create( wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& name, unsigned int flags ) {
    wxASSERT(parent);
    bool r = wxMiniFrame::Create( parent, id, title, pos, size, dockWindowStyle, name );
    flags_ = flags;
    title_ = title;
    visible_ = false;

#ifdef __WXGTK__
    // we will draw our own titlebar & close widget
    m_miniTitle = 0;
#endif
    createClient();

    return r;
}

void wxDockWindowBase::SetDockingManager( wxLayoutManager * pLayoutManager ) {
    pLayoutManager_ = pLayoutManager;
}

wxLayoutManager * wxDockWindowBase::GetLayoutManager() {
    return pLayoutManager_;
}

void wxDockWindowBase::Appear() {
    if( IsDocked() ) {
        // ignore if we are already docked
        if( pClientPanel_->IsDocked() ) {
            return;
        }
        
        // re-dock the panel from the owner window back onto the host
        if( !applyLastDock( true ) ) {
            // could not re-dock, so just show the window floating
            ActualShow();
        }
    }
    else {
        ActualShow();
    }
    visible_ = true;
}

void wxDockWindowBase::Remove() {
    if( IsDocked() ) {
       // ignore if we are already undocked
        if( !pClientPanel_->IsDocked() ) {
            return;
        }

        wxLayoutManager * pLayoutManager = GetLayoutManager();
        wxASSERT(pLayoutManager);

        // undock the panel back onto the owner window
        pLayoutManager->UndockWindow( this, true );
    }
    else {
        ActualShow( false );
    }
    visible_ = false;
}

void wxDockWindowBase::AutoFitSingleChild() {
    // tell the dock panel to auto fit
    wxASSERT(pClientPanel_);
    pClientPanel_->AutoFitSingleChild();
}

void wxDockWindowBase::SetClient( wxWindow * pClient, bool autoPane ) {
    // replace our panel's client
    pClientPanel_->SetClient( pClient, autoPane );
}

wxWindow * wxDockWindowBase::GetClient() {
    // return the actual client
    return pClientPanel_->GetClient();
}

wxWindow * wxDockWindowBase::RemoveClient( wxWindow * pNewParent ) {
    wxWindow * pClient = GetClient();

    // replace our panel's client with the stock client
    pClientPanel_->SetClient( NULL );

    // reparent old client?
    if( pClient && pNewParent ) {
        pClient->Reparent( pNewParent );
    }

    return pClient;
}

wxDockPanel * wxDockWindowBase::GetDockPanel() {
    // return the docking panel
    return pClientPanel_;
}

void wxDockWindowBase::SetDockingInfo( wxHostInfo &hi ) {
    prevHost_ = hi;
}

void wxDockWindowBase::ClearDockingInfo() {
    prevHost_.Reset();
}

wxHostInfo & wxDockWindowBase::GetDockingInfo() {
    return prevHost_;
}

bool wxDockWindowBase::ActualShow( bool show ) {
    bool r = wxMiniFrame::Show( show );
    if( show ) {
        // update the panel layout
        pClientPanel_->UpdateSize();
    }
    visible_ = true;
    return r;
}

void wxDockWindowBase::DisableShowOverride() {
    disableShowOverride_ = true;
}


bool wxDockWindowBase::Show( bool show ) {
    if( disableShowOverride_ ) {
        return ActualShow( show );
    }

    // hook into Show() to perform extended functionality
    if( show ) {
        Appear();
    }
    else {
        Remove();
    }
    return false;
}

void wxDockWindowBase::StartDragging( int x, int y, bool needMouseCapture ) {
    // removes any displayed tool-tips
    wxToolTip::Enable( false );

    dragging_ = true;
    haveMoved_ = false;
    
    // take snapshot of starting rectangle
    startRect_ = GetRect();
    
    // take account of starting mouse
    startPoint_.x = x;
    startPoint_.y = y;
    
    // draw the initial drag frame (only when window is already floating)
    if( IsShown() ) {
        wxScreenDC dc;
        g_gdi.DrawFrame( (wxDC &)dc, startRect_.rect, false );
        prevRect_ = startRect_;
    }

    // we want all mouse moves
    if( needMouseCapture ) {
        CaptureMouse();
    }
}

void wxDockWindowBase::StopDragging( bool needMouseRelease ) {
    if( !dragging_ ) return;
    dragging_ = false;
    
    // clear the last drag frame
    if( prevRect_.valid ) {
        wxScreenDC dc;
        g_gdi.DrawFrame( (wxDC &)dc, prevRect_.rect, false );
    }
    
    // move window?
    if( dragRect_.valid ) {
        if( newHost_.IsValid() ) {
            // apply host info
            pLayoutManager_->DockWindow( this, newHost_ );
        }
        else {
            // move
            Move( dragRect_.rect.x, dragRect_.rect.y );
            if( !IsShown() ) {
                // undock window
                pLayoutManager_->UndockWindow( this );
            }
        }
    }
    
    // clean-up
    startRect_.Reset();
    prevRect_.Reset();
    dragRect_.Reset();
    if( needMouseRelease ) {
        ReleaseMouse();
    }
    newHost_.Reset();
    
    // re-enables tool-tips
    wxToolTip::Enable( true );

    haveMoved_ = false;
}

bool wxDockWindowBase::BlockDocking() {
    // optional platform implementation
    return false;
}

void wxDockWindowBase::RepeatLastMouseEvent() {
    if( haveMoved_ ) {
        OnMouseMove( lastMouseEvent_ );
    }
}

void wxDockWindowBase::OnMouseMove( wxMouseEvent &e ) {
    // save a copy of the mouse event
    haveMoved_ = true;
    lastMouseEvent_ = e;

    if( dragging_ ) {
        wxScreenDC dc;

        // get mouse co-ordinates
        int mx = e.GetX();
        int my = e.GetY();
        ClientToScreen( &mx, &my );

        // update drag rectangle
        newHost_.Reset();
        if( pLayoutManager_ ) {
            newHost_ = pLayoutManager_->TestForHost( mx, my );
            newHost_.SetValid( newHost_.IsValid() & !BlockDocking());
        }
        
        if( newHost_.IsValid()  ) {
            // docking
            dragRect_ = newHost_.GetHost()->GetScreenArea( newHost_ );
        }
        else {
            // no docking 
            dragRect_ = GetRect();
            if( !IsShown() ) {
                // if dragging whilst docked, auto-adjust window position
                dragRect_ = GetClientRect();
                
                // account for offset within pane
                wxPoint point( wxPoint( startPoint_.x, startPoint_.y ) );
                dragRect_.rect.SetPosition( pLayoutManager_->PointFromScreen( point ) );
                
                // centre around cursor
                dragRect_.rect.Offset( -(dragRect_.rect.width/2), -(dragRect_.rect.height/2) );
                
                // transpose to screen
                dragRect_.rect = pLayoutManager_->RectToScreen( dragRect_.rect );
            }
            dragRect_.rect.Offset( mx-startPoint_.x, my-startPoint_.y );
        }
        
        // draw updated drag frame
        if( prevRect_.valid ) {
            wxScreenDC dc;
            g_gdi.DrawFrame( (wxDC &)dc, prevRect_.rect, false );
        }
        g_gdi.DrawFrame( (wxDC &)dc, dragRect_.rect, false );

        // update previous        
        prevRect_ = dragRect_;
    }
}

void wxDockWindowBase::OnClose( wxCloseEvent &e ) {
    // our close actually just hides
    e.Veto();
    Remove();

}

void wxDockWindowBase::createClient() {
    // make a client sizer
    pClientSizer_ = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( pClientSizer_ );
    
    // auto-create our docking client
    unsigned int flags = 0x0000;
    (flags_ & wxDWC_NO_CONTROLS) ? flags |= wxDPC_NO_CONTROLS : 0;
    pClientPanel_ = new wxDockPanel( this, 0, wxT("dockpanel"), flags );
    pClientPanel_->Show();
    pClientPanel_->SetDockWindow( this );
    
    // init. the sizer
    pClientSizer_->Add( pClientPanel_, 1, wxGROW );
    Layout();
}

bool wxDockWindowBase::applyLastDock( bool noShowOperation ) {
    // dock back into host - if possible
    if( prevHost_.IsValid() ) {
        // apply previous host info
        pLayoutManager_->DockWindow( this, prevHost_, noShowOperation );
        return true;
    }
    return false;
}

void wxDockWindowBase::SetDocked( bool state ) {
    docked_ = state;
}

bool wxDockWindowBase::IsDocked() {
    return docked_;
}

bool wxDockWindowBase::IsVisible() {
    return visible_;
}

