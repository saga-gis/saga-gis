/////////////////////////////////////////////////////////////////////////////
// Name:        slidebar.cpp
// Purpose:     wxSlideBar implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     25/05/04
// RCS-ID:      $Id: slidebar.cpp,v 1.1.1.1 2006-06-26 15:43:35 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/slidebar.h>
#include <wx/barholder.h>
#include <wx/util.h>

#include <wx/menu.h>
#include <wx/list.h>
#include <wx/listimpl.cpp>
#include <wx/dynarray.h>
#include <wx/arrimpl.cpp>

using namespace wxUtil;

enum eContextMenu {
    IDM_LOCK_BARS = 200,    // TODO: check ids?
    IDM_ITEMS,
};

#define MINIMUM_HEIGHT 4    // if we are empty, this is our minimum presence

#define STREAM_VERSION  wxT("wxSlideBar-Stream-v1.0")

// ----------------------------------------------------------------------------
// wxSlideBar constants & wx-macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( wxSlideBar, wxWindow )
    // these two events make the control 'layout aware'
    EVT_CALCULATE_LAYOUT( wxSlideBar::OnCalculateLayout )
    EVT_QUERY_LAYOUT_INFO( wxSlideBar::OnQueryLayoutInfo )
    EVT_CONTEXT_MENU( wxSlideBar::OnContextMenu )
    EVT_MENU( IDM_LOCK_BARS, wxSlideBar::OnLockBars )
    EVT_SIZE( wxSlideBar::OnSize )
END_EVENT_TABLE()

DEFINE_EVENT_TYPE( wxEVT_SLIDEBAR_SIZE_CHANGED )
DEFINE_EVENT_TYPE( wxEVT_SLIDEBAR_UPDATE_LAYOUT )

IMPLEMENT_DYNAMIC_CLASS( wxSlideBar, wxWindow )

WX_DEFINE_LIST( BarHolderList );
WX_DEFINE_OBJARRAY( BarPlacementArray );

// ----------------------------------------------------------------------------
// wxSlideBar implementation
// ----------------------------------------------------------------------------

void wxSlideBar::Init() {
    areaHeight_ = 0;
    areaWidth_ = 0;
    mode_ = wxSLIDE_MODE_SIMPLE;
    barList_.Clear();
    barLock_ = false;
    pContextMenu_ = NULL;
}

bool wxSlideBar::Create( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
    wxASSERT(parent);
    // create the wxWindows controls
    if( !wxWindow::Create( parent, id, pos, size, style | wxCLIP_CHILDREN, name ) ) {
        return FALSE;
    }
    SetBackgroundColour(GetBackgroundColour()); // XXX: unsure why we have to do this?

    return TRUE;
}

wxSlideBar::~wxSlideBar() {
    // delete context menu
    if( pContextMenu_ ) {
        delete pContextMenu_;
        pContextMenu_ = NULL;
    }
}

void wxSlideBar::OnQueryLayoutInfo( wxQueryLayoutInfoEvent& WXUNUSED(event) ) {
    // this is the function that wxLayoutAlgorithm calls to ascertain the window dimensions
}

void wxSlideBar::OnCalculateLayout( wxCalculateLayoutEvent& event ) {
    // check for a 'query only' event
    bool queryMode = false;
    if( (event.GetFlags() & wxLAYOUT_QUERY) != 0 ) {
        queryMode = true;
    }

    wxRect areaRect = event.GetRect();
    
    // position ourselves
    int width = areaRect.GetWidth();
    int height = 0;
    BarPlacementArray & bpl = CalcBarPlacement( width, &height );
    if( !queryMode ) {
        applyBarPlacement( bpl );
        SetSize( areaRect.GetX(), areaRect.GetY(), areaRect.GetWidth(), height );
    }

    // chew off a bit of area for ourselves from the layout area
    areaRect.SetY( areaRect.GetY() + height );
    areaRect.SetHeight( areaRect.GetHeight() - height );
    event.SetRect( areaRect );
}

void wxSlideBar::SetMode( wxSlideBarMode mode ) {
    // change mode
    mode_ = mode;
    UpdateLayout();
}

wxSlideBarMode wxSlideBar::GetMode() {
    return mode_;
}

void wxSlideBar::SetBarLock( bool enable ) {
    // enable/disable bar lock 
    barLock_ = enable;

    // for all bars we own
    for( BarHolderList::Node *node = barList_.GetFirst(); node; node = node->GetNext() ) {
        // get holder & placement
        wxBarHolder * pBarHolder = node->GetData();
        wxASSERT(pBarHolder);
        pBarHolder->ShowGripper( !enable );
    }

    UpdateLayout();
}

bool wxSlideBar::GetBarLock() {
    return barLock_;
}

wxBarHolder * wxSlideBar::AddWindow( wxWindow * pWindow, const wxString & label, unsigned int flags ) {
    // add a window as a new bar
    wxASSERT(pWindow);

    // create a holder for the window
    wxBarHolder * pBarHolder = new wxBarHolder( this, 0 );
    pBarHolder->AddWindow( pWindow, label, flags );
    pBarHolder->SetSlideBar( this );
    wxString key = pBarHolder->GetLabel();
    barList_.Append( key, pBarHolder );
    UpdateLayout();

    return pBarHolder;
}

void wxSlideBar::UpdateLayout() {
    // create an update event
    wxCommandEvent e( wxEVT_SLIDEBAR_UPDATE_LAYOUT );
    e.SetEventObject( this );
    GetEventHandler()->AddPendingEvent( e );
/*
    wxFrame * pFrame = wxDynamicCast( GetParent(), wxFrame );
    if( pFrame ) {
        pFrame->SendSizeEvent();
    }
    refreshBars();
*/    
}

void wxSlideBar::DoGetSize( int * x, int * y ) const {
    if( x ) *x = areaWidth_;
    if( y ) *y = areaHeight_;
}

wxSize wxSlideBar::DoGetBestSize() const {
    return GetSize();
}

void wxSlideBar::OnSize( wxSizeEvent &event ) {
    int oldHeight = areaHeight_;

    // update our height & width
    wxSize size = event.GetSize();
    areaWidth_ = size.GetWidth();
    areaHeight_ = size.GetHeight();

    // generate a size change event?
    if( oldHeight != areaHeight_ ) {
        // create a size event
        wxCommandEvent e( wxEVT_SLIDEBAR_SIZE_CHANGED );
        e.SetEventObject( this );
        GetEventHandler()->AddPendingEvent( e );
    }
}

void wxSlideBar::OnContextMenu( wxContextMenuEvent &event ) {
    // show the context menu
    createContextMenu();
    wxPoint mp = event.GetPosition();
    mp = ScreenToClient( mp );
    PopupMenu( pContextMenu_, mp );
    deleteContextMenu();
}

void wxSlideBar::OnLockBars( wxCommandEvent & WXUNUSED(event) ) {
    // toggle locking
    SetBarLock( GetBarLock() ^ true );
}

void wxSlideBar::OnContextItem( wxCommandEvent &event ) {
    int id = event.GetId();

    // toggle visibility for item
    BarHolderList::Node * node = barList_.Item( id - IDM_ITEMS );
    wxASSERT(node);
    wxBarHolder * pBarHolder = node->GetData();
    wxASSERT(pBarHolder);

    bool visible = pBarHolder->IsShown();
    pBarHolder->Show( visible ^ true );
    
    UpdateLayout();
}

wxBarHolder * wxSlideBar::GetBarHolderAt( BarPlacementArray &barPlacementArray, wxPoint pt ) {
    // for all bars in the provided array
    for( unsigned int i=0; i<barPlacementArray.GetCount(); i++ ) {
        // get holder & placement
        wxBarPlacement &pl = barPlacementArray.Item( i );
        wxBarHolder * pBarHolder = pl.pBarHolder;
        wxASSERT(pBarHolder);
        wxRect r = pl.placement;
        if( r.Inside( pt ) ) {
            return pBarHolder;
        }
    }

    return NULL;
}

wxBarHolder * wxSlideBar::GetBarHolderAt( wxPoint pt ) {
    // for all bars we own
    for( BarHolderList::Node *node = barList_.GetFirst(); node; node = node->GetNext() ) {
        // get holder & placement
        wxBarHolder * pBarHolder = node->GetData();
        wxASSERT(pBarHolder);
        if( !pBarHolder->IsShown() ) {
            // ignore bar if hidden
            continue;
        }

        wxRect r = pBarHolder->GetRect();
        if( r.Inside( pt ) ) {
            return pBarHolder;
        }
    }

    return NULL;
}

void wxSlideBar::SwapBarHolders( wxBarHolder * p1, wxBarHolder * p2 ) {
    // swaps the position of two bar holders in the holder list
    wxASSERT(p1 && p2);
    BarHolderList::Node *p1Node = barList_.Find( p1 );
    BarHolderList::Node *p2Node = barList_.Find( p2 );
    wxASSERT(p1Node && p1Node);
    p1Node->SetData( p2 );
    p2Node->SetData( p1 );
}

BarPlacementArray & wxSlideBar::CalcBarPlacement( int width, int * pAreaHeight ) {
    barPlacementArray_.Clear();
    if( width == -1 ) {
        width = GetClientSize().x;
    }
    if( width == 0 ) {
        return barPlacementArray_;
    }

    // layout the contained bars
    bool finished = false;
    int x = 0, y = 0, rowHeight = 0, rowWidth = width;
    BarPlacementArray rowHolders;
    for( BarHolderList::Node *node = barList_.GetFirst(); node;  ) {
        // get holder
        wxBarHolder * pBarHolder = node->GetData();
        wxASSERT(pBarHolder);

        // is hidden?
        if( !pBarHolder->IsShown() ) {
            // skip
            node = node->GetNext();
            if( !node ) {
                finished = true;
            }
            else
                continue;
        }

        // get size
        wxSize barSize = pBarHolder->GetBestSize();

        wxBarPlacement barPlacement;
        barPlacement.pBarHolder = pBarHolder;
        wxRect & r = barPlacement.placement;

        // can we fit the bar on the current row?
        bool noFit = false;
        if( !finished && (x + barSize.GetWidth() <= width || x == 0) ) {
            // yes
            r.SetPosition( wxPoint( x, y ) );
            r.SetSize( barSize );
            rowHolders.Add( barPlacement );
            x += barSize.GetWidth();
            node = node->GetNext();
            if( !node ) {
                finished = true;
            }
            if( rowHeight < barSize.GetHeight() ) {
                // make row taller
                rowHeight = barSize.GetHeight();
            }
        }
        else {
            // no
            noFit = true;
        }
        
        // do a 'end-of-row'
        if( finished || noFit || (mode_ == wxSLIDE_MODE_SIMPLE)  ) {
            for( unsigned int c = 0; c<rowHolders.GetCount(); c++ ) {
                // update bar position for this row
                wxBarPlacement &bp = rowHolders[c];
                wxRect & r = bp.placement;
                int width = (c == (rowHolders.GetCount()-1) ? (rowWidth - r.GetX()) : r.GetWidth() );
                int height = rowHeight;
                r.SetSize( wxSize( width, height ) );

                // add to final array
                barPlacementArray_.Add( bp );
            }

            // start a new row
            y += rowHeight;
            x = 0;
            rowHeight = 0;
            rowWidth = width;
            rowHolders.Clear();
        }
    }

    // don't allow zero height
    if( y == 0 ) y = MINIMUM_HEIGHT;

    // return new height also?
    if( pAreaHeight ) *pAreaHeight = y;

    return barPlacementArray_;
}

bool wxSlideBar::SaveToStream( wxOutputStream &stream ) {

    // version
    WriteString( stream, STREAM_VERSION );

    // attributes
    stream.Write( &mode_, sizeof( mode_ ) );
    stream.Write( &barLock_, sizeof( barLock_ ) );

    // bars
    WriteString( stream, wxT("<layout>") );

    int barCount = barList_.GetCount();
    stream.Write( &barCount, sizeof( barCount ) );
    for( BarHolderList::Node *node = barList_.GetFirst(); node; node = node->GetNext() ) {
        wxBarHolder * pBarHolder = node->GetData();

        // name
        WriteString( stream, pBarHolder->GetLabel() );  // TODO: really should be non-visual i.e GetName()

        // visiblity
        bool visible = pBarHolder->IsShown();
        stream.Write( &visible, sizeof( visible ) );
    }

    return true;
}

bool wxSlideBar::LoadFromStream( wxInputStream &stream ) {

    // version
    wxString version = ReadString( stream );
    if( version != STREAM_VERSION ) {
        return false;
    }

    // attributes
    stream.Read( &mode_, sizeof( mode_ ) );
    bool barLock = false;
    stream.Read( &barLock, sizeof( barLock ) );
    SetBarLock( barLock );

    wxString layoutTag = ReadString( stream );
    if( layoutTag == wxT("<layout>") ) {
        // create a copy of the bar list
        BarHolderList tmpBarList( wxKEY_STRING );
        for( BarHolderList::Node *node = barList_.GetFirst(); node; node = node->GetNext() ) {
            wxBarHolder * pBarHolder = node->GetData();
            tmpBarList.Append( pBarHolder->GetLabel(), pBarHolder );
        }
        barList_.Clear();

        // read in windows
        int barCount = 0;
        stream.Read( &barCount, sizeof( barCount ) );
        for( int i=0; i<barCount; i++ ) {
            // name
            wxString name = ReadString( stream );

            // find bar
            BarHolderList::Node * node = tmpBarList.Find( name );
            if( !node ) {
                // bar no longer exists
                continue;
            }
            wxBarHolder * pBarHolder = node->GetData();

            // visibility
            bool visible = false;
            stream.Read( &visible, sizeof( visible ) );
            pBarHolder->Show( visible );

            // add to new list
            barList_.Append( name, pBarHolder );
        }

    }

    UpdateLayout();

    return true;
}

bool wxSlideBar::SaveToXML( const wxString& filename ) {
    (filename);
    return true;
}

bool wxSlideBar::LoadFromXML( const wxString& filename ) {
    (filename);
    return true;
}

// ----------------------------------------------------------------------------

void wxSlideBar::applyBarPlacement( BarPlacementArray & bpl ) {
    // apply a list of bar placements
    for( unsigned int i=0; i<bpl.GetCount(); i++ ) {
        // apply holder
        wxBarPlacement &pl = bpl.Item( i );
        wxBarHolder * pBarHolder = pl.pBarHolder;
        wxASSERT(pBarHolder);
        pBarHolder->Move( pl.placement.GetPosition() );
        pBarHolder->SetHeightOverride( pl.placement.GetHeight() );
        pBarHolder->SetWidthOverride( pl.placement.GetWidth() );
        pBarHolder->UpdateSize();
    }
}

void wxSlideBar::createContextMenu() {
    wxASSERT(!pContextMenu_);

    // create the context menu
    pContextMenu_ = new wxMenu;

    // holders
    int c = 0;
    for( BarHolderList::Node *node = barList_.GetFirst(); node; node = node->GetNext() ) {
        // get holder
        wxBarHolder * pBarHolder = node->GetData();
        wxASSERT(pBarHolder);
        wxString label = pBarHolder->GetLabel();

        // add to list of view toggles?
        if( label.Length() > 0 ) {
            // add item
            int itemId = IDM_ITEMS + c;
            pContextMenu_->AppendCheckItem( itemId, label );
            
            // set current state
            bool check = pBarHolder->IsShown();
            pContextMenu_->Check( itemId, check );
            c++;

            // connect event handler
            GetEventHandler()->Connect( itemId, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &wxSlideBar::OnContextItem );
            contextIdEnd_ = itemId;
        }
    }
    contextIdStart_ = IDM_ITEMS;

    // standard items
    if( c ) {
        pContextMenu_->AppendSeparator();
    }
    pContextMenu_->AppendCheckItem( IDM_LOCK_BARS, wxT("&Lock the bars") );
    pContextMenu_->Check( IDM_LOCK_BARS, barLock_ );
}

void wxSlideBar::deleteContextMenu() {
    wxASSERT(pContextMenu_);
    // disconnect dynamic events
    for( int i=contextIdStart_; i<=contextIdEnd_; i++ ) {
        GetEventHandler()->Disconnect( i );
    }
    delete pContextMenu_;
    pContextMenu_ = NULL;
}

void wxSlideBar::refreshBars() {
    for( BarHolderList::Node *node = barList_.GetFirst(); node; node = node->GetNext() ) {
        wxBarHolder * pBarHolder = node->GetData();
        pBarHolder->Refresh();
    }
}
