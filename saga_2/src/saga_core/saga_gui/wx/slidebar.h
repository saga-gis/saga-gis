/////////////////////////////////////////////////////////////////////////////
// Name:        wx/slidebar.h
// Purpose:     wxSlideBar class
// Author:      Mark McCormack
// Modified by:
// Created:     25/05/05
// RCS-ID:      $Id: slidebar.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDEBAR_H_
#define _WX_SLIDEBAR_H_

#include <wx/defs.h>
#include <wx/barholder.h>
#include <wx/wfstream.h>
#include "wx/export.h"

enum wxSlideBarMode {
    wxSLIDE_MODE_SIMPLE,
    wxSLIDE_MODE_COMPACT
};

struct wxBarPlacement {
    wxBarHolder * pBarHolder;
    wxRect placement;
};

class wxMenu;

// ----------------------------------------------------------------------------
// wxSlideBar control
// ----------------------------------------------------------------------------

WX_DECLARE_USER_EXPORTED_LIST( wxBarHolder, BarHolderList, WXDLLIMPEXP_DI );
WX_DECLARE_USER_EXPORTED_OBJARRAY( wxBarPlacement, BarPlacementArray, WXDLLIMPEXP_DI );

class WXDLLIMPEXP_DI wxSlideBar : public wxWindow
{
public:
    // Default constructor
    wxSlideBar() 
        : barList_( wxKEY_STRING ) {
        Init();
    }
    
    // Normal constructor
    wxSlideBar( wxWindow *parent, wxWindowID id = -1, 
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, 
        const wxString& name = wxT("slidebar") )
        : barList_( wxKEY_STRING ) 
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    void Init();

    bool Create( wxWindow *parent, wxWindowID id = -1, 
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, 
        long style = wxTAB_TRAVERSAL, 
        const wxString& name = wxT("slidebar") );

    virtual ~wxSlideBar();

    // standard interface
    wxBarHolder * AddWindow( wxWindow * pWindow, 
        const wxString & label = wxT(""), unsigned int flags = wxBF_DEFAULT );
    void UpdateLayout();

    // extended interface
    void SetMode( wxSlideBarMode mode );
    wxSlideBarMode GetMode();
    void SetBarLock( bool enable );
    bool GetBarLock();

    // event handlers
    void OnQueryLayoutInfo( wxQueryLayoutInfoEvent& event );
    void OnCalculateLayout( wxCalculateLayoutEvent& event );
    void OnSize( wxSizeEvent &event );
    void OnContextMenu( wxContextMenuEvent &event );
    void OnLockBars( wxCommandEvent &event );
    void OnContextItem( wxCommandEvent &event );

    // overrides
    virtual void DoGetSize( int * x, int * y ) const;
    virtual wxSize DoGetBestSize() const;

    // load/save
    bool SaveToStream( wxOutputStream &stream );
    bool LoadFromStream( wxInputStream &stream );
#ifdef wxDI_USE_XMLLOADSAVE
    bool SaveToXML( const wxString& filename );
    bool LoadFromXML( const wxString& filename );
#endif

    // access
    wxBarHolder * GetBarHolderAt( wxPoint pt );
    wxBarHolder * GetBarHolderAt( BarPlacementArray &barPlacementArray, 
        wxPoint pt );
    void SwapBarHolders( wxBarHolder * p1, wxBarHolder * p2 );
    BarPlacementArray & CalcBarPlacement( int width, 
        int * pAreaHeight = NULL );

private:
    void applyBarPlacement( BarPlacementArray & bpl );
    void createContextMenu();
    void deleteContextMenu();
    void refreshBars();

    DECLARE_DYNAMIC_CLASS( wxSlideBar )
    DECLARE_EVENT_TABLE()

private:
    BarPlacementArray barPlacementArray_;
    int areaHeight_;
    int areaWidth_;
    wxSlideBarMode mode_;
    BarHolderList barList_;
    bool barLock_;
    wxMenu * pContextMenu_;
    
    int contextIdStart_;
    int contextIdEnd_;
};

// ----------------------------------------------------------------------------
// wxSlideBar events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_SLIDEBAR_SIZE_CHANGED, wxEVT_FIRST + 1251 )  // TODO: must change id
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_SLIDEBAR_UPDATE_LAYOUT, wxEVT_FIRST + 1252 )  // TODO: must change id
END_DECLARE_EVENT_TYPES()

// This should keep wxDockit compatible with wxWidgets 2.4.x
#ifndef WXWIN_COMPATIBILITY_2_4
#define EVT_SLIDEBAR_SIZE_CHANGED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SLIDEBAR_SIZE_CHANGED, -1, -1, \
        (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#define EVT_SLIDEBAR_UPDATE_LAYOUT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SLIDEBAR_UPDATE_LAYOUT, -1, -1, \
        (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#else
// Patch for wxWidgets-2.5.4 compat submitted by Jesus Gonzalez (2005-03-02)
typedef void (wxEvtHandler::*wxCommandEventFunction)(wxCommandEvent&);
#define EVT_SLIDEBAR_SIZE_CHANGED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SLIDEBAR_SIZE_CHANGED, -1, -1, \
        (wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, & fn ), NULL ),
#define EVT_SLIDEBAR_UPDATE_LAYOUT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SLIDEBAR_UPDATE_LAYOUT, -1, -1, \
        (wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, & fn ), NULL ),
#endif

#endif
    // _WX_SLIDEBAR_H_
