/////////////////////////////////////////////////////////////////////////////
// Name:        wxLayoutManager.cpp
// Purpose:     wxLayoutManager implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: layoutmanager.cpp,v 1.1.1.1 2006-06-26 15:43:35 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/layoutmanager.h>
#include <wx/dockhost.h>
#include <wx/dockpanel.h>
#include <wx/dockwindow.h>
#include <wx/slidebar.h>
#include <wx/util.h>
#include <wx/mdi.h>

#include <wx/xml/xml.h>

#include <wx/gdicmn.h>

#include <wx/list.h>
#include <wx/listimpl.cpp>

using namespace wxUtil;

// ----------------------------------------------------------------------------
// wxOwnerEventHandler implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxOwnerEventHandler, wxEvtHandler )

BEGIN_EVENT_TABLE( wxOwnerEventHandler, wxEvtHandler )
    EVT_SIZE(wxOwnerEventHandler::OnSize)
    EVT_MOVE(wxOwnerEventHandler::OnMove)
    EVT_SLIDEBAR_UPDATE_LAYOUT(wxOwnerEventHandler::OnUpdateLayout)
    EVT_MENU( -1, wxOwnerEventHandler::OnMenu)
END_EVENT_TABLE()

void wxOwnerEventHandler::OnSize( wxSizeEvent &event ) {
    if( pOwner_ ) {
        pOwner_->OnSize();
    }
    event.Skip();
}

void wxOwnerEventHandler::OnMove( wxMoveEvent &event ) {
    if( pOwner_ ) {
        pOwner_->OnMove();
    }
    event.Skip();
}

void wxOwnerEventHandler::OnUpdateLayout( wxCommandEvent& WXUNUSED(event) ) {
    if( pOwner_ ) {
        pOwner_->OnUpdateLayout();
    }
}

void wxOwnerEventHandler::OnMenu( wxCommandEvent& event ) {
    if( pOwner_ ) {
        pOwner_->OnMenuToggle( event.GetId() );
    }
    event.Skip(true);
}

// ----------------------------------------------------------------------------
// wxLayoutManager constants & wx-macros
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxLayoutManager, wxObject )

DEFINE_EVENT_TYPE( wxEVT_LAYOUT_CHANGED )

WX_DECLARE_LIST( wxDockPanel, DockPanelList );
WX_DEFINE_LIST( DockPanelList );

WX_DEFINE_LIST( DockHostList );

#define STREAM_VERSION wxT("wxDocking-Stream-v1.0")

// ----------------------------------------------------------------------------
// wxLayoutManager implementation
// ----------------------------------------------------------------------------
wxLayoutManager::wxLayoutManager( wxWindow * pOwnerFrame )
    : pOwnerWindow_( pOwnerFrame ) {
    Init();
    
    // install event handler
    frameEventHandler_.SetOwner( this );
    pOwnerWindow_->PushEventHandler( &frameEventHandler_ );
}

wxLayoutManager::~wxLayoutManager() {
    // uninstall event handler
    pOwnerWindow_->RemoveEventHandler( &frameEventHandler_ );

    // make sure all dockwindows extended Show() handling is turned off
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();
        wxASSERT(pDockWindow);
        pDockWindow->DisableShowOverride();
    }
}

void wxLayoutManager::Init() {
    dockHosts_.Clear();
    dockWindows_.Clear();
    flags_ = wxDWF_SPLITTER_BORDERS;
    pAutoLayoutClientWindow_ = NULL;
    pWindowMenu_ = NULL;
}

void wxLayoutManager::SetLayout( unsigned int flags, wxWindow * pAutoLayoutClientWindow ) {
    flags_ = flags;
    pAutoLayoutClientWindow_ = pAutoLayoutClientWindow;

    // generate internal event    
    settingsChanged();
}

void wxLayoutManager::AddDefaultHosts() {
    // adds the standard four hosts - in the standard priority order
    AddDockHost( wxTOP );
    AddDockHost( wxBOTTOM );
    AddDockHost( wxLEFT );
    AddDockHost( wxRIGHT );
}

void wxLayoutManager::AddDockHost( wxDirection dir, int initialSize, const wxString& name ) {
    // fill in name?
    wxString dockName = name;
    if( dockName == wxT("guessname") ) {
        switch( dir ) {
        case wxLEFT:
            dockName = wxDEFAULT_LEFT_HOST;
        break;
        case wxRIGHT:
            dockName = wxDEFAULT_RIGHT_HOST;
        break;
        case wxTOP:
            dockName = wxDEFAULT_TOP_HOST;
        break;
        case wxBOTTOM:
            dockName = wxDEFAULT_BOTTOM_HOST;
        break;
        default:
            wxASSERT_MSG( false, wxT("AddDockHost() - direction parameter not recognised") );
        break;
        }
    }

    // check for duplicate
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( dockName == pDockHost->GetName() || dir == pDockHost->GetDirection() ) {
            wxASSERT_MSG( false, wxT("AddDockHost() - direction or name already used") );
        }
    }

    // create host
    wxDockHost * pDockHost = new wxDockHost( pOwnerWindow_, 0, dir, dockName );
    pDockHost->SetLayoutManager( this );
    pDockHost->SetAreaSize( initialSize );
    
    // add a host
    dockHosts_.Append( pDockHost );
}

wxDockHost * wxLayoutManager::GetDockHost( const wxString& name ) {
    // find dock host
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( name == pDockHost->GetName()  ) {
            return pDockHost;
        }
    }
    
    return NULL;
}

wxDockHost  *wxLayoutManager :: GetDockHost ( const wxDirection  &_dir )
{
    // find dock host
    wxDockHost   *pDockHost = 0;

    for ( DockHostList :: Node  *node = dockHosts_.GetFirst(); node; node = node -> GetNext () )
    {
        pDockHost = node -> GetData ();

        if( pDockHost && ( _dir == pDockHost -> GetDirection () ) )
            return ( pDockHost );
    }

    return NULL;
}

void wxLayoutManager::AddDockWindow( wxDockWindowBase * pDockWindow ) {
    // check for duplicate
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pKnownDockWindow = node->GetData();
        if( pDockWindow->GetName() == pKnownDockWindow->GetName() ) {
            wxASSERT_MSG( false, wxT("AddDockWindow() - name already used") );
        }
    }
    
    // add a window
    dockWindows_.Append( pDockWindow );
    pDockWindow->SetDockingManager( this );
}

wxHostInfo wxLayoutManager::TestForHost( int sx, int sy ) {
    wxHostInfo tHostInfo;
    // test host for screen co-ordinate inside of
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( pDockHost->TestForPanel( sx, sy, tHostInfo ) ) {
            break;
        }
    }
    return tHostInfo;
}

wxRect wxLayoutManager::TrimDockArea( wxDockHost * pDockHost, wxRect &dockArea ) {
    wxRect tDockArea( dockArea );

    // test host for screen co-ordinate inside of
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pKnownDockHost = node->GetData();

        if( pKnownDockHost == pDockHost ) {
            // ignore all hosts after us
            return tDockArea;
        }
        if( pKnownDockHost->IsEmpty() ) {
            // ignore empty dock hosts
            continue;
        }

        // chew of another bit
        wxRect chewArea = pKnownDockHost->GetClientArea();
        if( pKnownDockHost->GetDirection() == wxLEFT && pDockHost->GetDirection() != wxRIGHT) {
            if( chewArea.width ) {
                tDockArea.x += chewArea.width;
                tDockArea.width -= chewArea.width;
            }
        }
        if( pKnownDockHost->GetDirection() == wxRIGHT && pDockHost->GetDirection() != wxLEFT ) {
            if( chewArea.width ) {
                tDockArea.width -= chewArea.width + 1;
            }
        }
        if( pKnownDockHost->GetDirection() == wxTOP && pDockHost->GetDirection() != wxBOTTOM ) {
            if( chewArea.height ) {
                tDockArea.y += chewArea.height;
                tDockArea.height -= chewArea.height;
            }
        }
        if( pKnownDockHost->GetDirection() == wxBOTTOM && pDockHost->GetDirection() != wxTOP ) {
            if( chewArea.height ) {
                tDockArea.height -= chewArea.height + 1;
            }
        }
    }
    return tDockArea;
}

wxRect wxLayoutManager::RectToScreen( wxRect &rect ) {
    // convert owner (frame) rect to screen rect
    wxASSERT(pOwnerWindow_);
    wxRect tRect( rect );
    wxPoint pos = pOwnerWindow_->ClientToScreen( tRect.GetPosition() );
    tRect.SetPosition( pos );
    return tRect;
}

wxPoint wxLayoutManager::PointFromScreen( wxPoint &point ) {
    wxASSERT(pOwnerWindow_);
    return pOwnerWindow_->ScreenToClient( point );
}

unsigned int wxLayoutManager::GetFlags() {
    return flags_;
}

bool wxLayoutManager::IsPrimaryDockHost( wxDockHost * pDockHost ) {
    // are we the first dock host in the list?
    if( dockHosts_.IndexOf( pDockHost ) == 0 ) {
        return true;
    }
    else {
        return false;
    }
}

void wxLayoutManager::SetDockArea( wxRect &rect ) {
    dockArea_ = rect;
}

wxRect wxLayoutManager::GetDockArea() {
    return dockArea_;
}

void wxLayoutManager::DockWindow( wxDockWindowBase * pDockWindow, wxHostInfo &hi, bool noHideOperation ) {
    wxASSERT(hi.IsValid());
    wxASSERT(pDockWindow);
    
    wxDockPanel * pClient = pDockWindow->GetDockPanel();
    wxASSERT(pClient);

    // can't dock back into the same panel
    if( hi.GetPanel() == pClient ) {
        return;
    }

    // undock first if in a host
    if( pClient->GetDockedHost() ) {
        UndockWindow( pDockWindow, true );
    }
    // dock a window into a host
    wxDockHost * pDockHost = hi.GetHost();
    pDockHost->DockPanel( pClient, hi );
    if( !noHideOperation ) {
        pDockWindow->ActualShow( false );
        pDockWindow->SetDocked( true );
    }
    pDockWindow->SetDockingInfo( hi );
    UpdateAllHosts( true );
}

void wxLayoutManager::UndockWindow( wxDockWindowBase * pDockWindow, bool noShowOperation ) {
    wxASSERT(pDockWindow);

    // already undocked?
    if( !pDockWindow->IsDocked() ) {
        return;
    }

    // undock a window
    wxDockPanel * pClient = pDockWindow->GetDockPanel();
    wxASSERT(pClient);
    wxDockHost * pDockHost = pClient->GetDockedHost();
    wxASSERT(pDockHost);
    pDockHost->UndockPanel( pClient );
    pDockWindow->Layout();
    if( !noShowOperation ) {
        pDockWindow->ActualShow( true );
        pDockWindow->SetDocked( false );
    }
    UpdateAllHosts( true );
}

void wxLayoutManager::OnSize() {
    // callback event generated by the a host size change
    UpdateAllHosts( true );
}

void wxLayoutManager::OnMove() {
    // XXX: no longer used
}

void wxLayoutManager::OnUpdateLayout() {
    UpdateAllHosts( false );
}

void wxLayoutManager::OnMenuToggle( int entryId ) {
    if ( entryId == -1 )
        return;
    if ( !pWindowMenu_ )
        return;

    wxMenuItem * mEntry = pWindowMenu_->FindItem(entryId);

    // If we don't own this menu ID then do nothing
    if ( mEntry == NULL )
        return;

    wxString mTitle = mEntry->GetLabel();
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();
        if (mTitle == pDockWindow->GetTitle()) {
            pDockWindow->Show(mEntry->IsChecked());
        }
    }
    UpdateAllHosts( false );
}

void wxLayoutManager::UpdateAllHosts( bool WXUNUSED(sizeChange), wxDockHost * WXUNUSED(pIgnoreHost) ) {
    // generate an event
    wxCommandEvent e( wxEVT_LAYOUT_CHANGED );
    e.SetEventObject( this );
    wxEvtHandler * pFrameEventHandler = pOwnerWindow_->GetEventHandler();
    wxASSERT( pFrameEventHandler );
    pFrameEventHandler->ProcessEvent( e );

    wxLayoutAlgorithm layout;
    wxMDIParentFrame * pMDIFrame = wxDynamicCast( pOwnerWindow_, wxMDIParentFrame );
    if( pMDIFrame ) {
        // layout within an MDI frame
        layout.LayoutMDIFrame( pMDIFrame );
    }
    else {
        // layout within an normal frame/or standard wxWindow
        layout.LayoutWindow( pOwnerWindow_, pAutoLayoutClientWindow_ );
    }
    if ( pWindowMenu_ )    {
        wxString mTitle;
        wxMenuItem * mEntry;
        int mEntryId;
        for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
            wxDockWindowBase * pDockWindow = node->GetData();
            mTitle = pDockWindow->GetTitle();
            mEntryId = pWindowMenu_->FindItem(mTitle);
            if ( mEntryId == wxNOT_FOUND ) {
                mEntry = pWindowMenu_->AppendCheckItem(-1, mTitle);
            } 
            else {
                mEntry = pWindowMenu_->FindItem(mEntryId);
            }
            mEntry->Check(pDockWindow->IsVisible());
        }
    }

}

wxDockHost * wxLayoutManager::findDockHost( const wxString& name ) {
    // look for a particular host
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( pDockHost->GetName() == name ) {
            // found
            return pDockHost;
        }
    }
    // not found
    return NULL;
}

wxDockWindowBase * wxLayoutManager::findDockWindow( const wxString& name ) {
    // look for a particular window
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();
        if( pDockWindow->GetName() == name ) {
            // found
            return pDockWindow;
        }
    }
    // not found
    return NULL;
}

void wxLayoutManager::settingsChanged() {
    // update size of all hosts
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        pDockHost->SettingsChanged();
    }
}

bool wxLayoutManager::SaveToStream( wxOutputStream &stream ) {

    // version
    WriteString( stream, STREAM_VERSION );

    // windows
    WriteString( stream, wxT("<layout>") );

    int winCount = dockWindows_.GetCount();
    stream.Write( &winCount, sizeof( winCount ) );
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();

        // name
        WriteString( stream, pDockWindow->GetName() );

        // undocked size
        wxRect size = pDockWindow->GetRect();
        stream.Write( &size.x, sizeof( size.x ) );
        stream.Write( &size.y, sizeof( size.y ) );
        stream.Write( &size.width, sizeof( size.width ) );
        stream.Write( &size.height, sizeof( size.height ) );

        // attributes
        bool isShown = pDockWindow->IsShown();
        stream.Write( &isShown, sizeof( isShown ) );
        bool isDocked = pDockWindow->IsDocked();
        stream.Write( &isDocked, sizeof( isDocked ) );
        
        // area taken
        wxDockPanel * pDockPanel = pDockWindow->GetDockPanel();
        wxASSERT(pDockPanel);
        int area = pDockPanel->GetArea();
        stream.Write( &area, sizeof( area ) );

        // docking info
        wxHostInfo &hi = pDockWindow->GetDockingInfo();
        if( hi.GetHost() ) {
            WriteString( stream, hi.GetHost()->GetName() );
        }
        else {
            WriteString( stream, wxT("<nohost>") );
        }
    }

    // write out hosts
    int hostCount = dockHosts_.GetCount();
    stream.Write( &hostCount, sizeof( hostCount ) );
    for( DockHostList::Node *hnode = dockHosts_.GetFirst(); hnode; hnode = hnode->GetNext() ) {
        wxDockHost * pDockHost = hnode->GetData();

        // name
        WriteString( stream, pDockHost->GetName() );

        // area size
        int areaSize = pDockHost->GetAreaSize();
        stream.Write( &areaSize, sizeof( areaSize ) );

        // panel size
        int panelArea = pDockHost->GetPanelArea();
        stream.Write( &panelArea, sizeof( panelArea ) );

        // docked windows
        const DockWindowList & dwl = pDockHost->GetDockWindowList();
        int winCount = dwl.GetCount();
        stream.Write( &winCount, sizeof( winCount ) );
        for( DockWindowList::Node *lnode = dwl.GetFirst(); lnode; lnode = lnode->GetNext() ) {
            wxDockWindowBase * pDockWindow = lnode->GetData();

            // name
            WriteString( stream, pDockWindow->GetName() );
        }
    }    

    return true;
}

bool wxLayoutManager::LoadFromStream( wxInputStream &stream ) {
    int i;
    // version
    wxString version = ReadString( stream );
    if( version != STREAM_VERSION ) {
        return false;
    }

    wxString layoutTag = ReadString( stream );
    if( layoutTag == wxT("<layout>") ) {
        DockPanelList lockedPanels;
        
        // undock all windows
        for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
            wxDockWindowBase * pDockWindow = node->GetData();
            wxASSERT( pDockWindow );
            pDockWindow->Remove();
        }

        // read in windows
        int winCount = 0;
        stream.Read( &winCount, sizeof( winCount ) );
        for( i=0; i<winCount; i++ ) {
            // name
            wxString name = ReadString( stream );

            // undocked size
            wxRect size;
            stream.Read( &size.x, sizeof( size.x ) );
            stream.Read( &size.y, sizeof( size.y ) );
            stream.Read( &size.width, sizeof( size.width ) );
            stream.Read( &size.height, sizeof( size.height ) );

            // attributes
            bool isShown = false;
            stream.Read( &isShown, sizeof( isShown ) );
            bool isDocked = false;
            stream.Read( &isDocked, sizeof( isDocked ) );
            
            // area taken
            int area = 0;
            stream.Read( &area, sizeof( area ) );

            // docking info
            wxString host = ReadString( stream );

            // find window and apply
            wxDockWindowBase * pDockWindow = findDockWindow( name );
            if( pDockWindow ) {
                pDockWindow->SetSize( size );
                pDockWindow->SetDocked( isDocked );

                wxDockPanel * pDockPanel = pDockWindow->GetDockPanel();
                wxASSERT( pDockPanel );

                // panel area
                pDockPanel->SetArea( area );
                pDockPanel->LockAreaValue( true );
                lockedPanels.Append( pDockPanel );

                // docking info
                wxDockHost * pDockHost = findDockHost( host );
                if( pDockHost && isDocked ) {
                    // was docked
                    wxHostInfo hi;
                    hi = pDockHost;
                    pDockWindow->SetDockingInfo( hi );
                }
                else {
                    // was not docked
                    if( isShown ) {
                        pDockWindow->Appear();
                    }
                    pDockWindow->ClearDockingInfo();
                }
            }
            else {
                // could not find window
            }
        }

        DockHostList lockedHosts;

        // read in hosts
        int hostCount = 0;
        stream.Read( &hostCount, sizeof( hostCount ) );
        for( i=0; i<hostCount; i++ ) {
            // name
            wxString name = ReadString( stream );

            // area size
            int areaSize = 0;
            stream.Read( &areaSize, sizeof( areaSize ) );

            // panel area
            int panelArea = 0;
            stream.Read( &panelArea, sizeof( panelArea ) );

            // find host and apply
            wxDockHost * pDockHost = findDockHost( name );
            if( pDockHost ) {
                // areas
                pDockHost->SetAreaSize( areaSize );
                pDockHost->SetPanelArea( panelArea );
                pDockHost->LockPanelValue( true );
                lockedHosts.Append( pDockHost );

                // docked windows
                int winCount = 0;
                stream.Read( &winCount, sizeof( winCount ) );
                for( int i=0; i<winCount; i++ ) {
                    // name
                    wxString name = ReadString( stream );

                    // find window and dock
                    wxDockWindowBase * pDockWindow = findDockWindow( name );
                    if( pDockWindow ) {
                        wxHostInfo hi;
                        hi = pDockHost;
                        DockWindow( pDockWindow, hi ); 
                    }
                    else {
                        // could not find window
                    }
                }
            }
            else {
                // could not find host
            }
        }
        UpdateAllHosts( true );

        // unlock all panels
        for( DockPanelList::Node *pnode = lockedPanels.GetFirst(); pnode; pnode = pnode->GetNext() ) {
            wxDockPanel * pDockPanel = pnode->GetData();
            wxASSERT( pDockPanel );
            pDockPanel->LockAreaValue( false );
        }

        // unlock all hosts
        for( DockHostList::Node *hnode = lockedHosts.GetFirst(); hnode; hnode = hnode->GetNext() ) {
            wxDockHost * pDockHost = hnode->GetData();
            wxASSERT( pDockHost );
            pDockHost->LockPanelValue( false );
        }

    }   // end <layout>

    return true;
}

#ifdef wxDI_USE_XMLLOADSAVE

/**
 * This function creates a new XML_ELEMENT_NODE and then creates a XML_TEXT_NODE
 * that is then assigned the content passed to the function.  The pointer to the 
 * new element node is returned to the caller.
 *
 * TODO: move this to the wxDockUtil namespace so it is shared by wxSlideBar
 *
 * @author Landon Bradshaw <landon@phazeforward.com>
 * 
 * @param const wxString nodename  The physical name given for the new element node
 * @param const wxString content   The string to assign to the text node
 *
 * @return wxXmlNode *
 */
wxXmlNode *eNode(const wxString nodename, const wxString content) {
    wxXmlNode *child = new wxXmlNode(wxXML_TEXT_NODE, wxT("value")),
        *node = new wxXmlNode(wxXML_ELEMENT_NODE, nodename);
    child->SetContent(content);
    node->AddChild(child);
    return node;
}

/**
 * Used in the SaveToXML() method for either changing existing XML node content or
 * to create a new XML node if the existing node does not exist.
 *
 * TODO: move this to the wxDockUtil namespace so it is shared by wxSlideBar
 *
 * @author Landon Bradshaw <landon@phazeforward.com>
 *
 * @param wxXmlNode *root   
 * @param const wxString nodePath  The physical name given for the new element node
 * @param const wxString content   The string to assign to the text node
 *
 */
wxXmlNode *changeNode(wxXmlNode *root, const wxString nodePath, const wxString content) {
    (root);
    return NULL;
}

/**
 * Searches the XML document tree starting from the given node, the argument
 * "nodePath" denotes the tree structure to follow (node names separated by '/')
 * if getText is true the returned node pointer will point to the XML_TEXT_NODE
 * otherwise it will return the final XML_ELEMENT_NODE
 *
 * TODO: move this to the wxDockUtil namespace so it is shared by wxSlideBar
 *
 * @author Landon Bradshaw <landon@phazeforward.com>
 * 
 * @param wxXmlNode *root   
 * @param const wxString nodePath  The physical name given for the new element node
 * @param bool getText             true = return the XML_TEXT_NODE
 *                                 false = return the final XML_ELEMENT_NODE
 *
 * @return wxXmlNode *
 */
wxXmlNode *xmlFindNode(wxXmlNode *root, const wxString nodePath, bool getText = true) {
    wxXmlNode *node = root;
    wxString pathPart = nodePath;
    wxString findPart;
    bool nFound;

    while(pathPart.Length() > 0) {
        node = node->GetChildren();
        if (pathPart.Find(wxT("/")) == wxNOT_FOUND) {
            findPart = pathPart;
            pathPart.Empty();
        }
        else {
            findPart = pathPart.Left(pathPart.Find(wxT("/")));
            pathPart.Remove(0,pathPart.Find(wxT("/"))+1);
        }
        nFound = false;
        while((!nFound) && (node)) {
            if (findPart.Cmp(node->GetName()) == 0) {
                nFound = true;
                continue;
            }
            node = node->GetNext();
        }
        if (!node) {
            pathPart = wxEmptyString;
        }
    }
    if (!node)
        return NULL;
    if (getText) {
        node = node->GetChildren();
    }
    return node;
}

/**
 * SaveToXML allows the layout manager to save the current window layout and host setup
 * into an XML document for reloading later.  This XML tree will contain three main sections,
 * windows, hosts and slidebars, though the slidebars have a similiar function to save their 
 * state separately.
 *
 * TODO: only change existing nodes, currently conflicts with wxSlideBar::SaveToXML()
 *
 * @author Landon Bradshaw <landon@phazeforward.com>
 * 
 * @param const wxString& filename Filename to use for the XML file
 * 
 * @return bool true for success, false on error
 */
bool wxLayoutManager::SaveToXML( wxOutputStream &stream ) {
    wxXmlDocument xmlDoc;
    wxXmlNode *xroot = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("wxDockit"));
    xmlDoc.SetRoot(xroot);

    xroot->AddChild(eNode(wxT("stream_version"), STREAM_VERSION));

    int winCount = dockWindows_.GetCount();
    xroot->AddChild(eNode(wxT("window_count"), wxString::Format(wxT("%d"), winCount)));
    int hostCount = dockHosts_.GetCount();
    xroot->AddChild(eNode(wxT("host_count"), wxString::Format(wxT("%d"), hostCount)));

    // windows
    wxXmlNode *xnode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("windows"));
    xroot->AddChild(xnode);

    xroot = xnode;

    wxString wnStr;
    int wn = 0;

    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();

        wnStr.Printf(wxT("window_%d"),wn++);

        xnode = new wxXmlNode(wxXML_ELEMENT_NODE, wnStr);
        xroot->AddChild(xnode);

        // name
        xnode->AddChild(eNode(wxT("name"),pDockWindow->GetName()));

        // undocked size
        wxRect size = pDockWindow->GetRect();
        xnode->AddChild(eNode(wxT("corner_x"), wxString::Format(wxT("%d"), size.x)));
        xnode->AddChild(eNode(wxT("corner_y"), wxString::Format(wxT("%d"), size.y)));
        xnode->AddChild(eNode(wxT("width"), wxString::Format(wxT("%d"), size.width)));
        xnode->AddChild(eNode(wxT("height"), wxString::Format(wxT("%d"), size.height)));

        // attributes
        xnode->AddChild(eNode(wxT("shown"), wxString::Format(wxT("%d"), pDockWindow->IsShown())));
        xnode->AddChild(eNode(wxT("docked"), wxString::Format(wxT("%d"), pDockWindow->IsDocked())));

        // area taken
        wxDockPanel * pDockPanel = pDockWindow->GetDockPanel();
        wxASSERT(pDockPanel);
        int area = pDockPanel->GetArea();
        xnode->AddChild(eNode(wxT("dockarea"), wxString::Format(wxT("%d"), area)));

        // docking info
        wxHostInfo &hi = pDockWindow->GetDockingInfo();
        if( hi.GetHost() ) {
            xnode->AddChild(eNode(wxT("dockhost"), hi.GetHost()->GetName()));
        }
        else {
            xnode->AddChild(eNode(wxT("dockhost"), wxT("none")));
        }
    }

    xroot = xroot->GetParent();

    // write out hosts
    xnode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("hosts"));
    xroot->AddChild(xnode);

    xroot = xnode;

    wxString hnStr;
    int hn = 0;

    for( DockHostList::Node *hnode = dockHosts_.GetFirst(); hnode; hnode = hnode->GetNext() ) {
        wxDockHost * pDockHost = hnode->GetData();

        // name
        hnStr.Printf(wxT("host_%d"),hn++);
        xnode = new wxXmlNode(wxXML_ELEMENT_NODE, hnStr);
        xroot->AddChild(xnode);

        xnode->AddChild(eNode(wxT("name"), pDockHost->GetName()));

        // area size
        int areaSize = pDockHost->GetAreaSize();
        xnode->AddChild(eNode(wxT("host_area"), wxString::Format(wxT("%d"),areaSize)));

        // panel size
        int panelArea = pDockHost->GetPanelArea();
        xnode->AddChild(eNode(wxT("panel_area"), wxString::Format(wxT("%d"),panelArea)));

        // docked windows
        const DockWindowList & dwl = pDockHost->GetDockWindowList();
        int winCount = dwl.GetCount();
        xnode->AddChild(eNode(wxT("window_count"), wxString::Format(wxT("%d"),winCount)));
        wn = 0;
        for( DockWindowList::Node *lnode = dwl.GetFirst(); lnode; lnode = lnode->GetNext() ) {
            wxDockWindowBase * pDockWindow = lnode->GetData();
            wnStr.Printf(wxT("dockwin_%d"),wn++);
            xnode->AddChild(eNode(wnStr,pDockWindow->GetName()));
            // name
        }
    }    

    return xmlDoc.Save(stream);
}

/**
 * LoadFromXML will retrieve the layout from the specified XML document and apply it to the
 * current dock windows and hosts.
 *
 * TODO: need to error-check the xmlFindNode() return value properly
 * TODO: figure out a way to handle existing windows that do not exist in the saved layout
 *
 * @author Landon Bradshaw <landon@phazeforward.com>
 * 
 * @param const wxString& filename The XML file to load the layout from
 *
 * @return bool true means there were no errors, false means the load failed
 */
bool wxLayoutManager::LoadFromXML( wxInputStream &stream ) {
    wxXmlDocument xmlDoc;

    if (!xmlDoc.Load(stream))
        return false;

    wxXmlNode *xroot = xmlDoc.GetRoot();

    int i;
    // version
    wxString version = xmlFindNode(xroot, wxT("stream_version"))->GetContent();
    if( version != STREAM_VERSION ) {
        return false;
    }

    DockPanelList lockedPanels;
    
    // undock all windows
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();
        wxASSERT( pDockWindow );
        pDockWindow->Remove();
    }

    // read in windows
    wxString wnStr,wPath;
    long winCount = 0,longConvert;
    wxString strNumber = xmlFindNode(xroot, wxT("window_count"))->GetContent();
    strNumber.ToLong(&winCount);
    for( i=0; i<(int)winCount; i++ ) {
        // name
        wxString name = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/name"),i))->GetContent();

        // undocked size
        wxRect size;
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/corner_x"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        size.x = (int)longConvert;
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/corner_y"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        size.y = (int)longConvert;
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/width"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        size.width = (int)longConvert;
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/height"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        size.height = (int)longConvert;

        // attributes
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/shown"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        bool isShown = (longConvert == 1);
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/docked"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        bool isDocked = (longConvert == 1);
        
        // area taken
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/dockarea"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        int area = (int)longConvert;

        // docking info
        wxString host = xmlFindNode(xroot,wxString::Format(wxT("windows/window_%d/dockhost"),i))->GetContent();

        // find window and apply
        wxDockWindowBase * pDockWindow = findDockWindow( name );
        if( pDockWindow ) {
            pDockWindow->SetSize( size );
            pDockWindow->SetDocked( isDocked );

            wxDockPanel * pDockPanel = pDockWindow->GetDockPanel();
            wxASSERT( pDockPanel );

            // panel area
            pDockPanel->SetArea( area );
            pDockPanel->LockAreaValue( true );
            lockedPanels.Append( pDockPanel );

            // docking info
            wxDockHost * pDockHost = findDockHost( host );
            if( pDockHost && isDocked ) {
                // was docked
                wxHostInfo hi;
                hi = pDockHost;
                pDockWindow->SetDockingInfo( hi );
            }
            else {
                // was not docked
                if( isShown ) {
                    pDockWindow->Appear();
                }
                pDockWindow->ClearDockingInfo();
            }
        }
        else {
            // could not find window
        }
    }

    DockHostList lockedHosts;

    // read in hosts
    long hostCount = 0;
    strNumber = xmlFindNode(xroot, wxT("host_count"))->GetContent();
    strNumber.ToLong(&hostCount);

    for( i=0; i<hostCount; i++ ) {
        // name
        wxString name = xmlFindNode(xroot,wxString::Format(wxT("hosts/host_%d/name"),i))->GetContent();

        // area size
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("hosts/host_%d/host_area"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        int areaSize = (int)longConvert;

        // panel area
        strNumber = xmlFindNode(xroot,wxString::Format(wxT("hosts/host_%d/panel_area"),i))->GetContent();
        strNumber.ToLong(&longConvert);
        int panelArea = (int)longConvert;

        // find host and apply
        wxDockHost * pDockHost = findDockHost( name );
        if( pDockHost ) {
            // areas
            pDockHost->SetAreaSize( areaSize );
            pDockHost->SetPanelArea( panelArea );
            pDockHost->LockPanelValue( true );
            lockedHosts.Append( pDockHost );

            // docked windows
            strNumber = xmlFindNode(xroot,wxString::Format(wxT("hosts/host_%d/window_count"),i))->GetContent();
            strNumber.ToLong(&longConvert);
            int winCount = (int)longConvert;
            int j;
            for( j=0; j<winCount; j++ ) {
                // name
                wxString name = xmlFindNode(xroot,wxString::Format(wxT("hosts/host_%d/dockwin_%d"),i,j))->GetContent();

                // find window and dock
                wxDockWindowBase * pDockWindow = findDockWindow( name );
                if( pDockWindow ) {
                    wxHostInfo hi;
                    hi = pDockHost;
                    DockWindow( pDockWindow, hi ); 
                }
                else {
                    // could not find window
                }
            }
        }
        else {
            // could not find host
        }
    }
    UpdateAllHosts( true );

    // unlock all panels
    for( DockPanelList::Node *pnode = lockedPanels.GetFirst(); pnode; pnode = pnode->GetNext() ) {
        wxDockPanel * pDockPanel = pnode->GetData();
        wxASSERT( pDockPanel );
        pDockPanel->LockAreaValue( false );
    }

    // unlock all hosts
    for( DockHostList::Node *hnode = lockedHosts.GetFirst(); hnode; hnode = hnode->GetNext() ) {
        wxDockHost * pDockHost = hnode->GetData();
        wxASSERT( pDockHost );
        pDockHost->LockPanelValue( false );
    }

    return true;
}
#endif		// wxDI_USE_XMLLOADSAVE

bool wxLayoutManager::SaveToConfig(wxConfigBase *pcfg, const wxString &key) const
{
    (pcfg);
    (key);
	return FALSE;
}

bool wxLayoutManager::LoadFromConfig(wxConfigBase *pcfg, const wxString &key)
{
    (pcfg);
    (key);
	return FALSE;
}
