///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dockhost.h
// Purpose:     wxDockHost class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockhost.h,v 1.1.1.1 2006-06-26 15:43:05 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCKHOST_H_
#define _WX_DOCKHOST_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/panel.h>
#include <wx/laywin.h>

#include <wx/layoutmanager.h>

#include "wx/export.h"

class wxDockPanel;
class wxExSplitter;

// ----------------------------------------------------------------------------
// wxDockHost
// ----------------------------------------------------------------------------

WX_DECLARE_USER_EXPORTED_LIST( wxExSplitter, SplitterList, WXDLLIMPEXP_DI );

class WXDLLIMPEXP_DI wxDockHost : public wxPanel
{
    DECLARE_CLASS( wxDockHost )
    DECLARE_EVENT_TABLE()

public:

    wxDockHost() {
        Init();
    }
    ~wxDockHost();
    
    void Init();
    
    wxDockHost( wxWindow *parent, wxWindowID id, wxDirection dir, const wxString& name = wxT("dockhost") ) {
        Init();
        Create( parent, id, dir, name );
    }

    // basic interface
    bool Create( wxWindow * parent, wxWindowID id, wxDirection dir, const wxString& name = wxT("dockhost") );
    
    void SetLayoutManager( wxLayoutManager * pLayoutManager );
    wxLayoutManager * GetLayoutManager();
    
    void SetAreaSize( int size );
    int GetAreaSize();

    // access
    void SetPanelArea( int panelArea ){ panelArea_ = panelArea; }
    int GetPanelArea(){ return panelArea_; }
    void LockPanelValue( bool state ){ lockPanelValue_ = state; }

    DockWindowList & GetDockWindowList();
    
    void DockPanel( wxDockPanel * pDockPanel, wxHostInfo &hi );
    void UndockPanel( wxDockPanel * pDockPanel );
    
    // access
    wxRect GetScreenArea();
    wxRect GetScreenArea( wxHostInfo &hi );
    wxRect GetClientArea();
    wxRect GetClientArea( wxRect availableArea );
    wxOrientation GetOrientation();
    wxDirection GetDirection();
    wxExSplitter *GetSizingSplitter () const 
	{ 
		return ( pSizingSplitter_ ); 
	}

    wxRect CalcHostPlacement( bool hitTest = false );
    bool IsEmpty();
    bool TestForPanel( int sx, int sy, wxHostInfo &hi );
    wxRect RectToScreen( wxRect &rect );
    void RecalcPanelAreas();

    void UpdateSize( bool useProportions = false );

    // event handlers
    void OnSize( wxSizeEvent &event );
    void OnSplitterMoved( wxCommandEvent &event );
    void OnCalculateLayout( wxCalculateLayoutEvent &event );
    
    // internal event
    void SettingsChanged();

private:
    void calcPanelPlacement( bool useProportions = false );
    void updateSplitters();
    int getAssetCount();

private:
    enum eChildType {
        CT_NONE,
        CT_PANEL,
        CT_SPLITTER
    };

    wxDirection dir_;   // dock edge
    int areaSize_;      // docking host size in direction
    int panelArea_;     // area available to panels
    bool lockPanelValue_;
	bool internalSizeEvent_;
    
    wxPoint pos_;
    wxSize size_;
    
    wxLayoutManager * pLayoutManager_; // our layout manager

    int numPanels_;
    int numSplitters_;
    SplitterList splitters_;
    unsigned int splitterFlags_;

    wxExSplitter * pSizingSplitter_;

    DockWindowList dockWindows_;
};
#endif
    // _WX_DOCKHOST_H_
