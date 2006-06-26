///////////////////////////////////////////////////////////////////////////////
// Name:        wx/layoutmanager.h
// Purpose:     wxLayoutManager class
// Author:      Mark McCormack
// Modified by: Francesco Montorsi
// Created:     23/02/04
// RCS-ID:      $Id: layoutmanager.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LAYOUTMANAGER_H_
#define _WX_LAYOUTMANAGER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/object.h>
#include <wx/gdicmn.h>
#include <wx/event.h>
#include <wx/wfstream.h>
#include "wx/export.h"

class wxFrame;
class wxDockHost;
class wxDockPanel;
class wxDockWindowBase;
class wxConfigBase;

#define wxDEFAULT_LEFT_HOST   wxT("LeftHost")
#define wxDEFAULT_RIGHT_HOST  wxT("RightHost")
#define wxDEFAULT_TOP_HOST    wxT("TopHost")
#define wxDEFAULT_BOTTOM_HOST wxT("BottomHost")

#define wxINITIAL_HOST_SIZE 64

// ----------------------------------------------------------------------------
// wxOwnerEventHandler
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DI wxOwnerEventHandler : public wxEvtHandler 
{
    DECLARE_CLASS( wxOwnerEventHandler )
    DECLARE_EVENT_TABLE()
public:
    wxOwnerEventHandler() {
        pOwner_ = NULL;
    }
    void SetOwner( class wxLayoutManager * pOwner ) {
        pOwner_ = pOwner;
    }

    void OnSize( wxSizeEvent &WXUNUSED(event) );
    void OnMove( wxMoveEvent &WXUNUSED(event) );
    void OnUpdateLayout( wxCommandEvent &WXUNUSED(event) );
    void OnMenu( wxCommandEvent &event );

private:
    class wxLayoutManager * pOwner_;
};

// ----------------------------------------------------------------------------
// wxHostInfo
// ----------------------------------------------------------------------------

enum wxPlacement {
    wxHIP_NONE,
    wxHIP_FRONT,
    wxHIP_BACK
};

struct WXDLLIMPEXP_DI wxHostInfo {

    wxHostInfo() {
        Reset();
    }
    wxHostInfo(wxDockHost * pDockHost, wxPlacement placement = wxHIP_NONE) {
        wxASSERT(pDockHost);
        Reset();
        pDockHost_ = pDockHost;
        placement_ = placement;
        valid_ = true;
    }

    void Reset() {
        pDockHost_ = NULL;
        pPanel_ = NULL;
        placement_ = wxHIP_NONE;
        valid_ = false;
    }

    bool IsValid() const {
        return valid_;
    }
    wxDockHost * GetHost() const {
        return pDockHost_;
    }
    wxDockPanel * GetPanel() const {
        return pPanel_;
    }
    wxPlacement GetPlacement() const {
        return placement_;
    }

    void SetValid( bool valid ) {
        valid_ = valid;
    }
    void SetPanel( wxDockPanel * pPanel ) {
        pPanel_ = pPanel;
    }
    void SetPlacement( wxPlacement placement ) {
        placement_ = placement;
    }

private:
    bool valid_;
    wxDockHost * pDockHost_;
    wxDockPanel * pPanel_;
    wxPlacement placement_;
};

// ----------------------------------------------------------------------------
// wxLayoutManager
// ----------------------------------------------------------------------------

WX_DECLARE_USER_EXPORTED_LIST( wxDockHost, DockHostList, WXDLLIMPEXP_DI );
WX_DECLARE_USER_EXPORTED_LIST( wxDockWindowBase, DockWindowList, WXDLLIMPEXP_DI );

#define wxDWF_LIVE_UPDATE       0x01
#define wxDWF_SPLITTER_BORDERS  0x02


//! One of the main classes of wxDockIt.
//! It handles all the XXXX
class WXDLLIMPEXP_DI wxLayoutManager : public wxObject
{
    DECLARE_DYNAMIC_CLASS( wxLayoutManager )

public:
    wxLayoutManager( wxWindow * pOwnerWindow );
    ~wxLayoutManager();
    
    void Init();
    void SetLayout( unsigned int flags, 
        wxWindow * pAutoLayoutClientWindow = NULL );
    void SetWindowMenu( wxMenu * menu ) 
		{ pWindowMenu_ = menu; }

    // dockhost
    void AddDefaultHosts();
    void AddDockHost( wxDirection dir, int initialSize = wxINITIAL_HOST_SIZE, 
        const wxString& name = wxT("guessname") );
    wxDockHost * GetDockHost( const wxString& name );
    wxDockHost * GetDockHost( const wxDirection  &_dir );

    // dockwindow
    void AddDockWindow( wxDockWindowBase * pDockWindow );
    void DockWindow( wxDockWindowBase * pDockWindow, wxHostInfo &hi, 
        bool noHideOperation = false );
    void UndockWindow( wxDockWindowBase * pDockWindow, 
        bool noShowOperation = false );

    // load/save
    bool SaveToStream( wxOutputStream &stream );
    bool LoadFromStream( wxInputStream &stream );
#ifdef wxDI_USE_XMLLOADSAVE
    bool SaveToXML( wxOutputStream &stream );
    bool LoadFromXML( wxInputStream &stream );
#endif
	bool SaveToConfig(wxConfigBase *, const wxString &key) const;
	bool LoadFromConfig(wxConfigBase *, const wxString &key);
    
    // access
    wxHostInfo TestForHost( int sx, int sy );
    wxRect TrimDockArea( wxDockHost * pDockHost, wxRect &dockArea );
    bool IsPrimaryDockHost( wxDockHost * pDockHost );
    void SetDockArea( wxRect &rect );
    wxRect GetDockArea();
    wxRect RectToScreen( wxRect &rect );
    wxPoint PointFromScreen( wxPoint &point );
    unsigned int GetFlags();

    void UpdateAllHosts( bool sizeChange, wxDockHost * pIgnoreHost = NULL );
    
    // event handers
    void OnSize();
    void OnMove();
    void OnUpdateLayout();
    void OnMenuToggle( int entryId );
    
private:
    wxDockHost * findDockHost( const wxString& name );
    wxDockWindowBase * findDockWindow( const wxString& name );

    void settingsChanged();

private:
    DockHostList dockHosts_;
    DockWindowList dockWindows_;
    
    wxWindow * pOwnerWindow_;
    wxOwnerEventHandler frameEventHandler_;
    
    unsigned int flags_;
    wxWindow * pAutoLayoutClientWindow_;
    wxRect dockArea_;
    wxMenu * pWindowMenu_;
};

// ----------------------------------------------------------------------------
// wxLayoutManager events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_LAYOUT_CHANGED, wxEVT_FIRST + 1211 )   // TODO: must change ids
END_DECLARE_EVENT_TYPES()

#define EVT_LAYOUT_CHANGED( fn ) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_LAYOUT_CHANGED, -1, -1, \
        (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),

#endif
    // _WX_LAYOUTMANAGER_H_
