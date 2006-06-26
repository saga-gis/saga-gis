/////////////////////////////////////////////////////////////////////////////
// Name:        barholder.cpp
// Purpose:     wxBarHolder implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     25/05/04
// RCS-ID:      $Id: barholder.cpp,v 1.1.1.1 2006-06-26 15:43:32 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/barholder.h>
#include <wx/gripper.h>
#include <wx/slidebar.h>
#include <wx/dcclient.h>

// ----------------------------------------------------------------------------
// wxBarHolder constants & wx-macros
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE( wxEVT_BARHOLDER_RIGHTDOWN )

BEGIN_EVENT_TABLE( wxBarHolder, wxWindow )
    EVT_ERASE_BACKGROUND( wxBarHolder::OnErase )
    EVT_PAINT( wxBarHolder::OnPaint )
    EVT_GRIP_LEFTDOWN( wxBarHolder::OnGripBegin )
    EVT_GRIP_LEFTUP( wxBarHolder::OnGripEnd )
    EVT_GRIP_MOTION( wxBarHolder::OnGripMotion )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS( wxBarHolder, wxWindow )

const int DEFAULT_MARGIN = 2;
const int GRIP_SIZE = 6;

// ----------------------------------------------------------------------------
// wxBarHolder implementation
// ----------------------------------------------------------------------------

void wxBarHolder::Init() {
    pSlideBar_ = NULL;
    pOurBar_ = NULL;
    pGripWindow_= NULL;
    showGripper_ = true;
    margin_ = DEFAULT_MARGIN;
    heightOverride_ = -1;
    widthOverride_ = -1;
    flags_ = wxBF_DEFAULT;
    moving_ = false;
}

bool wxBarHolder::Create( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
    wxASSERT(parent);
    // create the wxWindow control
    if( !wxWindow::Create( parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name ) ) {
        return FALSE;
    }

    // create sub-controls
    pGripWindow_ = new wxGripWindow( this, wxHORIZONTAL, wxGdi::wxGRIP_STYLE_MOVE );

    // assign the cursor for hover over the gripper
    pGripWindow_->SetCursor( g_gdi.GetMoveCursor() );

    return TRUE;
}

wxBarHolder::~wxBarHolder() {
}

void wxBarHolder::AddWindow( wxWindow * pWindow, const wxString & label, unsigned int flags ) {
    // add a window
    wxASSERT(pWindow);
    flags_ = flags;
    originalSize_ = getClientBestSize( pWindow );
    label_ = label;
    pGripWindow_->SetLabel( label );

    if( wxDynamicCast( pWindow, wxToolBar ) ) {
        // enforce the wxTB_NODIVIDER style for toolbars
        long windowStyle = pWindow->GetWindowStyle();
        windowStyle |= wxTB_NODIVIDER;
        pWindow->SetWindowStyle( windowStyle );
    }

    // take ownership
    pOurBar_ = pWindow;
    pWindow->Reparent( this );
    UpdateSize();
}

void wxBarHolder::SetSlideBar( wxSlideBar * pSlideBar ) {
    pSlideBar_ = pSlideBar;
}

void wxBarHolder::ShowGripper( bool state ) {
    showGripper_ = state;
    UpdateSize();
}

wxSize wxBarHolder::DoGetBestSize() const {
    // calc the minimum size for this holder
    wxSize barSize = originalSize_;

    // add margin
    int gripSize = showGripper_ ? GRIP_SIZE : 0;
    barSize.x += (margin_*2);
    barSize.y += (margin_*2);

    wxSize clientSize = barSize;
    clientSize.x += gripSize;

    // add our non-client area to the size
    clientSize = clientSize + getNonClientSize();

    return clientSize;
}

void wxBarHolder::UpdateSize() {
    if( !pOurBar_ ) return;

    wxSize barSize = originalSize_;
    wxSize clientSize = barSize;

    int ho = heightOverride_, wo = widthOverride_;
    // remove the non-client area from the sizes
    ho -= getNonClientSize().GetHeight();
    wo -= getNonClientSize().GetWidth();

    // fit ourselves around the owned bar
    clientSize.x += (margin_*2);
    clientSize.y += (margin_*2);
    int gripSize = showGripper_ ? GRIP_SIZE : 0;
    clientSize.x += gripSize;
    if( heightOverride_ != -1 ) clientSize.y = ho;
    if( widthOverride_ != -1 ) clientSize.x = wo;

    SetClientSize( clientSize );
    wxRect br;
    // expand in the y-axis?
    if( flags_ & wxBF_EXPAND_Y ) {
        br.y = margin_;
        br.height = clientSize.GetY() - (margin_*2);
    }
    else {
        br.y = (clientSize.GetY()/2) - (barSize.GetHeight()/2);
        br.height = barSize.GetHeight();
    }
    // expand in the x-axis?
    br.x = (margin_ + gripSize);
    if( flags_ & wxBF_EXPAND_X ) {
        br.width = clientSize.GetX() - ((margin_*2) + gripSize);
    }
    else {
        br.width = barSize.GetWidth();
    }
    pOurBar_->SetSize( br.x, br.y, br.width, br.height );

    // size the gripper
    pGripWindow_->Show( showGripper_ );
    pGripWindow_->SetSize( margin_, margin_, gripSize, clientSize.GetHeight() - (margin_*2) );

    return;
}

void wxBarHolder::SetHeightOverride( int override ) {
    heightOverride_ = override;        
}

void wxBarHolder::SetWidthOverride( int override ) {
    widthOverride_ = override;        
}

void wxBarHolder::SetFlags( unsigned int flags ) {
    flags_ = flags;
}

unsigned int wxBarHolder::GetFlags() {
    return flags_;
}

wxString wxBarHolder::GetLabel() {
	return label_;
}

void wxBarHolder::OnErase( wxEraseEvent & WXUNUSED(event) ) {
}

void wxBarHolder::OnPaint( wxPaintEvent & WXUNUSED(event) ) {
    wxPaintDC dc(this);

	// create a clipping region to exclude the child window
	dc.DestroyClippingRegion();
    wxRect cr = GetClientRect();
	wxRegion region( cr );
	wxRect sr = pOurBar_->GetRect();
	region.Subtract( sr );
	dc.SetClippingRegion( region );

	// draw background
    g_gdi.DrawBackground( dc, cr );
    g_gdi.DrawEdge( dc, cr, moving_ );
}

void wxBarHolder::OnGripBegin( wxMouseEvent & WXUNUSED(event) ) {
    // we want all mouse moves
    pGripWindow_->CaptureMouse();

    moving_ = true;
    Refresh();
}

void wxBarHolder::OnGripEnd( wxMouseEvent & WXUNUSED(event) ) {
    // release mouse
    if( moving_ ) {
        pGripWindow_->ReleaseMouse();
    }

    moving_ = false;
    Refresh();
}

void wxBarHolder::OnGripMotion( wxMouseEvent &event ) {
    if( !moving_ ) {
        return;
    }
    if( !pSlideBar_ ) {
        // can't do anything without owner
        return;
    }

    // convert from gripper local coordinates to parent local coordinates
    wxPoint pt = event.GetPosition();
    pt = pGripWindow_->ClientToScreen( pt );
    pt = pSlideBar_->ScreenToClient( pt );

    wxASSERT(pSlideBar_);

    wxBarHolder * pHoverBar = pSlideBar_->GetBarHolderAt( pt );
	if( !pHoverBar || pHoverBar == this ) {
		return;
	}
    Refresh();

	// swap this bar with the bar under the cursor
    pSlideBar_->SwapBarHolders( this, pHoverBar );

	// re-calculate what the layout would be
	BarPlacementArray &bpl = pSlideBar_->CalcBarPlacement( -1 );

	// is the cursor over where we would move to?
	if( pSlideBar_->GetBarHolderAt( bpl, pt ) == this ) {
		// accept swap
	    pSlideBar_->UpdateLayout();
		return;
	}

	// else, undo swap
    pSlideBar_->SwapBarHolders( this, pHoverBar );
}

// ----------------------------------------------------------------------------

wxSize wxBarHolder::getNonClientSize() const {
    // work out the size of the non-client border
    wxSize cs = GetClientSize();
    wxSize s = GetSize();
    return (s - cs);
}

wxSize wxBarHolder::getClientBestSize( wxWindow * pClient ) const {
    if( wxDynamicCast( pClient, wxToolBar ) ) {
        wxSize toolBarSize = pClient->GetBestSize();
#if defined(__WXMSW__)
// NOTE: that there is a bug in (wxMSW) wxToolBar::GetBestSize() which means that the returned size is
// usually be too large in the x and too small in the y
        toolBarSize.y++;
#endif
        return toolBarSize;
    }
    return pClient->GetSize();
}
