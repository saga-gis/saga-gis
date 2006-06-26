/////////////////////////////////////////////////////////////////////////////
// Name:        wxDockHost.cpp
// Purpose:     wxDockHost implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockhost.cpp,v 1.1.1.1 2006-06-26 15:43:33 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/dockhost.h>
#include <wx/dockwindow.h>
#include <wx/dockpanel.h>
#include <wx/gdi.h>
#include <wx/exsplitter.h>

#include <wx/list.h>
#include <wx/listimpl.cpp>

// ----------------------------------------------------------------------------
// wxDockHost constants & wx-macros
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxDockHost, wxPanel )

BEGIN_EVENT_TABLE( wxDockHost, wxPanel )
    EVT_SIZE( wxDockHost::OnSize )
    EVT_SPLITTER_MOVED( wxDockHost::OnSplitterMoved )
    EVT_CALCULATE_LAYOUT( wxDockHost::OnCalculateLayout )
END_EVENT_TABLE()

WX_DEFINE_LIST( DockWindowList );
WX_DEFINE_LIST( SplitterList );

// ----------------------------------------------------------------------------
// wxDockHost implementation
// ----------------------------------------------------------------------------

wxDockHost::~wxDockHost() {
    delete pSizingSplitter_;
}

void wxDockHost::Init() {
    dir_ = wxALL;
    areaSize_ = wxINITIAL_HOST_SIZE;
    panelArea_ = 0;
    lockPanelValue_ = false;
    internalSizeEvent_ = false;
    pLayoutManager_ = NULL;
    pos_.x = 0;
    pos_.y = 0;
    size_.x = 0;
    size_.y = 0;
    numPanels_ = 0;
    numSplitters_ = 0;
    splitterFlags_ = 0x0000;
    pSizingSplitter_ = NULL;
    splitters_.Clear();
    dockWindows_.Clear();
}

bool wxDockHost::Create( wxWindow *parent, wxWindowID id, wxDirection dir, const wxString& name ) {
    wxASSERT(parent);
    dir_ = dir;
    bool r = wxPanel::Create( parent, id, pos_, size_, wxTAB_TRAVERSAL | wxCLIP_CHILDREN, name );

    return r;
}

void wxDockHost::SetLayoutManager( wxLayoutManager * pLayoutManager ) {
    pLayoutManager_ = pLayoutManager;

    // create sizer splitter
    wxWindow * win1 = (dir_ == wxLEFT || dir_ == wxTOP) ? this : NULL;
    wxWindow * win2 = (dir_ == wxRIGHT || dir_ == wxBOTTOM) ? this : NULL;
    wxOrientation orientation = (GetOrientation() == wxHORIZONTAL) ? wxVERTICAL : wxHORIZONTAL;
    pSizingSplitter_ = new wxExSplitter( this, orientation, win1, win2, 0x0000 );
    SettingsChanged();
}

wxLayoutManager * wxDockHost::GetLayoutManager() {
    return pLayoutManager_;
}

void wxDockHost::SetAreaSize( int size ) {
    areaSize_ = size;
}

int wxDockHost::GetAreaSize() {
    return areaSize_;
}

DockWindowList & wxDockHost::GetDockWindowList() {
    DockWindowList dockWindows;

    wxWindowList & children = GetChildren();
    dockWindows_.Clear();
    for( int c=0; c<(int)children.GetCount(); c++ ) {
        // get child
        wxWindowListNode * pChildNode = children.Item( c );
        wxASSERT(pChildNode);
        wxWindow * pChild = pChildNode->GetData();
        wxASSERT(pChild);
    
        // is it a panel?
        wxDockPanel * pPanel = wxDynamicCast( pChild, wxDockPanel );
        if( pPanel ) {
            // add the panel's window
            dockWindows_.Append( pPanel->GetDockWindow() );
        }
    }

    return dockWindows_;
}

void wxDockHost::OnSize( wxSizeEvent &event ) {
    if( !pLayoutManager_ ) {
        // we have not beem given an owner yet, ignore event
        return;
    }

    wxSize newSize = event.GetSize();
    if( !IsEmpty() ) {
        // if host is being used then update the host's area value
        if( GetOrientation() == wxHORIZONTAL ) {
            areaSize_ = newSize.GetHeight();
        }
        else {
            areaSize_ = newSize.GetWidth();
        }
        
        if( !internalSizeEvent_ ) {
            pLayoutManager_->UpdateAllHosts( true, this );
        }
    }
    UpdateSize( true );
}

void wxDockHost::OnSplitterMoved( wxCommandEvent &event ) {
    if( event.GetEventObject() != pSizingSplitter_ ) {
        // recalc on panel splitter movement
        RecalcPanelAreas();
    }
}

void wxDockHost::UpdateSize( bool useProportions ) {
    // calculate where we should be

    wxRect hp = CalcHostPlacement();
    if( pLayoutManager_ ) {
        // ask the manager for trim with other hosts
        hp = pLayoutManager_->TrimDockArea( this, hp );
    }
    SetSize( hp );

    calcPanelPlacement( useProportions );
}

void wxDockHost::DockPanel( wxDockPanel * pDockPanel, wxHostInfo &hi ) {
    if( hi.GetPanel() == pDockPanel ) {
        // we don't need to do anything
        return;
    }

    // make the panel a child of this host
    pDockPanel->Reparent( this );

    wxWindowList & children = GetChildren();

    // if a target panel was specified, move child in relation to where we want it in the children list
    if( hi.GetPanel() ) {
        int insertIndex = children.IndexOf( hi.GetPanel() );
        if( insertIndex == wxNOT_FOUND ) {
            // panel no longer exists in this host
            hi.SetPanel( NULL );
        }
    }

    // delete original node (if we need to)
    wxWindowListNode * pNode = children.Find( pDockPanel );
    if( pNode ) {
        children.DeleteNode( pNode );

        // we may may need to remove a now unrequired splitter
        updateSplitters();

        // recalc panel areas
        RecalcPanelAreas();
    }

    int insertIndex = children.IndexOf( hi.GetPanel() );
    if( insertIndex != wxNOT_FOUND ) {
        // inserting panel is found, to the front or back?
        if( hi.GetPlacement() == wxHIP_BACK ) {
            insertIndex++;
        }
        children.Insert( insertIndex, pDockPanel );
    }
    else {
        // panel does not exist, insert at the back
        int count = getAssetCount();
        if( count > 0 ) {
            wxWindowListNode * pChildNode = children.GetLast();
            wxASSERT(pChildNode);
            hi.SetPanel( wxDynamicCast( pChildNode->GetData(), wxDockPanel ) );
            hi.SetPlacement( wxHIP_BACK );
        }
        children.Append( pDockPanel );
    }

    if( hi.GetPanel() ) {
        // split area with target dock panel
        int newArea = (hi.GetPanel()->GetArea() - wxSPLITTER_SIZE) / 2;
        hi.GetPanel()->SetArea( newArea );
        pDockPanel->SetArea( newArea );
    }
    else {
        // dock panel has all the available area
        pDockPanel->SetArea( panelArea_ );
    }

    pDockPanel->SetDockedHost( this );
}

void wxDockHost::UndockPanel( wxDockPanel * pDockPanel ) {
    // see if we have any information about how we docked
    wxWindowList & children = GetChildren();

    // find node
    wxWindowListNode * pNode = children.Find( pDockPanel );
    assert(pNode);
    wxDockPanel * pGivePanel = NULL;

    // distribute free area
    if( getAssetCount() > 1 ) {
        int lastIndex = children.GetCount()-1;
        wxWindowListNode * pLastNode = children.Item( lastIndex );
        wxASSERT(pLastNode);
        if( pNode == pLastNode ) {
            // node is the last node, give to prev
            do {
                pNode = children.Item( --lastIndex );
                pGivePanel = wxDynamicCast( pNode->GetData(), wxDockPanel );
            } while( !pGivePanel );
        }
        else {
            // give to next
            int currentIndex = children.IndexOf( pDockPanel );
            do {
                pNode = children.Item( ++currentIndex );
                pGivePanel = wxDynamicCast( pNode->GetData(), wxDockPanel );
            } while( !pGivePanel );
        }
    }

    // give the area back
    if( pGivePanel ) {
        int newArea = pDockPanel->GetArea() + pGivePanel->GetArea() + wxSPLITTER_SIZE;
        pGivePanel->SetArea( newArea );
    }

    // remove the panel from this host
    wxDockWindowBase * pDockWindow = pDockPanel->GetDockWindow();
    pDockPanel->Reparent( pDockWindow );

    pDockPanel->SetDockedHost( NULL );
}

wxRect wxDockHost::GetScreenArea() {
    // return the screen area for this host
    wxRect hp = CalcHostPlacement( true );
    if( pLayoutManager_ ) {
        hp = pLayoutManager_->TrimDockArea( this, hp );
    }

    return pLayoutManager_->RectToScreen( hp );
}

wxRect wxDockHost::GetScreenArea( wxHostInfo &hi ) {
    // return the screen area for the info
    if( !hi.GetPanel() ) {
        return GetScreenArea();
    }
    else {
        return hi.GetPanel()->GetScreenArea( hi );
    }
}

wxRect wxDockHost::GetClientArea() {
    // return the client area for this host
    return CalcHostPlacement();
}

 wxOrientation wxDockHost::GetOrientation() {
    switch( dir_ ) {
        case wxLEFT:
        case wxRIGHT:
            return wxVERTICAL;        
        
        case wxTOP:
        case wxBOTTOM:
            return wxHORIZONTAL;
            
        case wxALL:
            wxASSERT_MSG( false, wxT("wxALL is not valid") );
        break;
    }
    // failed
    return wxHORIZONTAL;
}
wxDirection wxDockHost::GetDirection() {
    return dir_;
}
 
wxRect wxDockHost::CalcHostPlacement( bool hitTest ) {
    int area = areaSize_;
    if( IsEmpty() ) {
        // host is empty
        if( !hitTest ) {
            // no testing - therefore, no size
            area = 0;
        }
    }

    // lets work out where we would like to go
    wxRect pcr = pLayoutManager_->GetDockArea();
    switch( dir_ ) {
        case wxLEFT:
            pos_.x = pcr.x;
            pos_.y = pcr.y;
            size_.x = area;
            size_.y = pcr.height;
        break;
        case wxTOP:
            pos_.x = pcr.x;
            pos_.y = pcr.y;
            size_.x = pcr.width;
            size_.y = area;
        break;
        case wxBOTTOM:
            pos_.x = pcr.x;
            pos_.y = pcr.GetBottom() - area;
            size_.x = pcr.width;
            size_.y = area;
        break;
        case wxRIGHT:
            pos_.x = pcr.GetRight() - area;
            pos_.y = pcr.y;
            size_.x = area;
            size_.y = pcr.height;
        break;        
        case wxALL:
            // n/a
        break;
    }
    wxRect t( pos_, size_ );
    return t;
}

bool wxDockHost::IsEmpty() {
    return getAssetCount() == 0;
}

bool wxDockHost::TestForPanel( int sx, int sy, wxHostInfo &hi ) {
    if( IsEmpty() ) {
        // host is empty
        wxRect hRect = GetScreenArea();
        if( hRect.Inside( sx, sy ) ) {
            hi = this;
            return true;
        }
    }
    
    wxWindowList & children = GetChildren();
    for( int c=0; c<(int)children.GetCount(); c++ ) {
        // get child
        wxWindowListNode * pChildNode = children.Item( c );
        wxASSERT(pChildNode);
        wxWindow * pChild = pChildNode->GetData();
        wxASSERT(pChild);
    
        // is it a panel?
        wxDockPanel * pPanel = wxDynamicCast( pChild, wxDockPanel );
        if( pPanel ) {
            wxRect localRect = pPanel->GetRect();
            wxRect screenRect = RectToScreen( localRect );
            if( screenRect.Inside( sx, sy ) ) {
                // hit-test success on panel
                hi = this;
                hi.SetPanel( pPanel );
                hi.SetPlacement( pPanel->TestForPlacement( sx, sy ) );
                return true;
            }
        }
    }
    return false;
}

wxRect wxDockHost::RectToScreen( wxRect &rect ) {
    // convert an child rect to screen rect
    wxRect tRect( rect );
    wxPoint pos = ClientToScreen( tRect.GetPosition() );
    tRect.SetPosition( pos );
    return tRect;
}

void wxDockHost::RecalcPanelAreas() {
    // get the client window 
    wxRect cr = GetClientRect();

    // set the available panel area
    int hostArea = GetOrientation() == wxHORIZONTAL ? cr.width : cr.height;

    int splittersArea = (numSplitters_ * wxSPLITTER_SIZE);
    if( !lockPanelValue_ ) {
        panelArea_ = (hostArea - splittersArea);
    }
 
    wxWindowList & children = GetChildren();
    int childCount = children.GetCount();

    for( int c=0; c<childCount; c++ ) {
        // get child
        wxWindowListNode * pChildNode = children.Item( c );
        wxASSERT(pChildNode);
        wxWindow * pChild = pChildNode->GetData();
        wxASSERT(pChild);

        // what is it?
        wxDockPanel * pPanel = wxDynamicCast( pChild, wxDockPanel );
        if( pPanel ) {
            // set the taken panel area
            int size = (GetOrientation() == wxHORIZONTAL) ? pPanel->GetRect().width : pPanel->GetRect().height;
            pPanel->SetArea( size ); 
        }
    }
}

void wxDockHost::OnCalculateLayout( wxCalculateLayoutEvent &event ) {
    // check for a 'query only' event
    bool queryMode = false;
    if( (event.GetFlags() & wxLAYOUT_QUERY) != 0 ) {
        queryMode = true;
    }

    wxRect areaRect = event.GetRect();

    // if we are the first host then save the area as the 'docking area'
    if( pLayoutManager_->IsPrimaryDockHost( this ) ) {
        pLayoutManager_->SetDockArea( areaRect );
    }

    if( IsEmpty() ) {
        // we have no effect when we are empty
        SetSize( 0,0,0,0 );
        return;
    }
    
    internalSizeEvent_ = true;

    // chew off a bit of area for ourselves from the layout area
    switch( GetDirection() ) {
        case wxLEFT:
            if( !queryMode ) SetSize( areaRect.GetX(), areaRect.GetY(), areaSize_, areaRect.GetHeight() );
            areaRect.SetX( areaRect.GetX() + areaSize_ );
            areaRect.SetWidth( areaRect.GetWidth() - areaSize_ );
            break;
        case wxTOP:
            if( !queryMode ) SetSize( areaRect.GetX(), areaRect.GetY(), areaRect.GetWidth(), areaSize_ );
            areaRect.SetY( areaRect.GetY() + areaSize_ );
            areaRect.SetHeight( areaRect.GetHeight() - areaSize_ );
            break;
        case wxBOTTOM:
            if( !queryMode ) SetSize( areaRect.GetX(), areaRect.GetY() + (areaRect.GetHeight() - areaSize_), areaRect.GetWidth(), areaSize_ );
            areaRect.SetHeight( areaRect.GetHeight() - areaSize_ );
            break;
        case wxRIGHT:
            if( !queryMode ) SetSize( areaRect.GetX() + (areaRect.GetWidth() - areaSize_), areaRect.GetY(), areaSize_, areaRect.GetHeight() );
            areaRect.SetWidth( areaRect.GetWidth() - areaSize_ );
            break;
        
        case wxALL:
            // n/a
        break;
    }
    event.SetRect( areaRect );

    internalSizeEvent_ = false;
}

void wxDockHost::SettingsChanged() {
    unsigned int dwFlags = pLayoutManager_->GetFlags();
    bool liveUpdate = (dwFlags & wxDWF_LIVE_UPDATE) == wxDWF_LIVE_UPDATE;
    bool splitterBorders = (dwFlags & wxDWF_SPLITTER_BORDERS) == wxDWF_SPLITTER_BORDERS;

    unsigned int flags = liveUpdate ? wxESF_LIVE_UPDATE : 0;
    flags |= splitterBorders ? wxESF_DRAW_GRIPPER : 0;
    
    // inform our splitters
    for( unsigned int i=0; i<splitters_.GetCount(); ++i ) {
        wxExSplitter * pSplitter = splitters_[i];
        pSplitter->SetFlags( flags );
    }
    pSizingSplitter_->SetFlags( flags );
    splitterFlags_ = flags;
}

void wxDockHost::calcPanelPlacement( bool useProportions ) {
    updateSplitters();

    wxWindowList & children = GetChildren();
    int childCount = children.GetCount();
    if( IsEmpty() ) {
        return;
    }
   
    // get the client window 
    wxRect cr = GetClientRect();
    
    int areaRemaining = GetOrientation() == wxHORIZONTAL ? cr.width : cr.height;
    int splittersArea = (numSplitters_ * wxSPLITTER_SIZE);
    int panelArea = (areaRemaining - splittersArea);
    int panelAllowance = panelArea;
    int areaPos = 0;
    int panelCount = 0;
    for( int c=0; c<childCount; c++ ) {
        // get child
        wxWindowListNode * pChildNode = children.Item( c );
        wxASSERT(pChildNode);
        wxWindow * pChild = pChildNode->GetData();
        wxASSERT(pChild);

        if( pChild == pSizingSplitter_ ) {
            // ignore host's own size splitter
            continue;
        }

        // what is it?
        wxDockPanel * pPanel = wxDynamicCast( pChild, wxDockPanel );

        int childChunk = 0;
        if( !pPanel ) {
            // is a splitter
            childChunk = wxSPLITTER_SIZE;
        }
        else {
            panelCount++;

            // give the panel it's required proportion of the available size
            if( panelCount < numPanels_ ) {
                if( useProportions ) {
                    // use proportions
                    float proportion = (float)pPanel->GetArea() / panelArea_;
                    childChunk = (int)((float)proportion * panelArea);
                }
                else {
                    // use absolutes
                    childChunk = pPanel->GetArea();
                }
                panelAllowance -= childChunk;
            }
            else {
                // last chunk gets whatever is left over
                childChunk = panelAllowance;
                panelAllowance = 0;
            }
        }

        // adjust size
        if( GetOrientation() == wxHORIZONTAL ) {
            int top = (dir_ == wxTOP) ? 0 : wxSPLITTER_SIZE;
            int height = cr.height - wxSPLITTER_SIZE;
            pChild->SetSize( areaPos, top, childChunk, height );
        }
        else {
            int left = (dir_ == wxLEFT) ? 0 : wxSPLITTER_SIZE;
            int width = cr.width - wxSPLITTER_SIZE;
            pChild->SetSize( left, areaPos, width, childChunk );
        }
        areaPos += childChunk + 1;  // XXX: not sure if we should have to do this extra increment?
    }

    assert(pSizingSplitter_);

    // adjust size splitter
    if( GetOrientation() == wxHORIZONTAL ) {
        int areaHeight = cr.height - wxSPLITTER_SIZE;
        int splitterTop = (dir_ == wxTOP) ? areaHeight : 0;
        pSizingSplitter_->SetSize( 0, splitterTop, cr.width, wxSPLITTER_SIZE );
    }
    else {
        int areaWidth = cr.width - wxSPLITTER_SIZE;
        int splitterLeft = (dir_ == wxLEFT) ? areaWidth : 0;
        pSizingSplitter_->SetSize( splitterLeft, 0, wxSPLITTER_SIZE, cr.height );
    }

    if( lockPanelValue_ ) {
        // recalc on load
        RecalcPanelAreas();
    }
 }

int wxDockHost::getAssetCount() {
    wxWindowList & children = GetChildren();
    int childCount = children.GetCount();

    // removes 1 for the host's own splitter
    return childCount-1;      
}

void wxDockHost::updateSplitters() {

    // adds/removes splitters dynamically as required
    wxWindowList & children = GetChildren();
    int childCount = children.GetCount();

    eChildType lastChildType = CT_NONE;
    wxWindow * pLastChild = NULL;
    numPanels_ = 0;
    numSplitters_ = 0;
    wxWindowList newChildList;
    SplitterList toDeleteSplitters;
    splitters_.Clear();
    
    int c;
    for( c=0; c<childCount; c++ ) {
        // get child
        wxWindowListNode * pChildNode = children.Item( c );
        wxASSERT(pChildNode);
        wxWindow * pChild = pChildNode->GetData();
        wxASSERT(pChild);

        if( pChild == pSizingSplitter_ ) {
            newChildList.Append( pSizingSplitter_ );

            // ignore host's own size splitter
            continue;
        }

        // what is it?
        wxDockPanel * pPanel = wxDynamicCast( pChild, wxDockPanel );
        if( pPanel ) {
            if( lastChildType == CT_PANEL ) {
                // we need to add a splitter
                wxASSERT(pLastChild);

                wxExSplitter * pSplitter = new wxExSplitter( this, GetOrientation(), pLastChild, pPanel, splitterFlags_ );
                
                newChildList.Append( pSplitter );
                numSplitters_++;
                splitters_.Append( pSplitter );
            }
            // add original panel
            newChildList.Append( pPanel );
            numPanels_++;

            // save lasts
            lastChildType = CT_PANEL;
            pLastChild = (wxWindow *)pPanel;
        }
        else {
            if( lastChildType == CT_SPLITTER || c == 1 || c == (childCount-1) ) {
                // we need to remove a splitter
                wxExSplitter * pPanelSplitter = wxDynamicCast( pChildNode->GetData(), wxExSplitter );
                toDeleteSplitters.Append( pPanelSplitter );
            }
            else {
                // add original splitter
                pLastChild = wxDynamicCast( pChildNode->GetData(), wxExSplitter );
                newChildList.Append( pLastChild );
                numSplitters_++;
                splitters_.Append( (wxExSplitter *)pLastChild );
            }
            // save lasts
            lastChildType = CT_SPLITTER;
        }
    }

    // delete unrequired splitters
    for ( SplitterList::Node *node = toDeleteSplitters.GetFirst(); node; node = node->GetNext() ) {
        delete node->GetData();
    }

    // update window list
    children = newChildList;
    
    // make sure splitters are connected to the correct panels
    int newChildCount = children.GetCount();
    for( c=0; c<newChildCount; c++ ) {
        // get child
        wxWindowListNode * pChildNode = children.Item( c );
        wxASSERT(pChildNode);

        wxExSplitter * pPanelSplitter = wxDynamicCast( pChildNode->GetData(), wxExSplitter );
        if( pPanelSplitter == pSizingSplitter_ ) {
            // ignore host's own size splitter
            continue;
        }

        if( pPanelSplitter ) {
            // get panels to either side of the splitter
            wxASSERT(c>1);
            wxASSERT(c<(newChildCount-1));
            wxDockPanel * pFrontPanel = wxDynamicCast( children.Item( c-1 )->GetData(), wxDockPanel );
            wxASSERT(pFrontPanel);
            wxDockPanel * pBackPanel = wxDynamicCast( children.Item( c+1 )->GetData(), wxDockPanel );
            wxASSERT(pBackPanel);
            pPanelSplitter->SetWindows( pFrontPanel, pBackPanel );
        }
    }
}
