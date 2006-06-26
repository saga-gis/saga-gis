/////////////////////////////////////////////////////////////////////////////
// Name:        propgrid.cpp
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "propgrid.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/button.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/choice.h"
    #include "wx/stattext.h"
    #include "wx/scrolwin.h"
    #include "wx/dirdlg.h"
    #include "wx/combobox.h"
    #include "wx/layout.h"
    #include "wx/sizer.h"
    #include "wx/textdlg.h"
    #include "wx/filedlg.h"
    #include "wx/statusbr.h"
    #include "wx/intl.h"
#endif


#include "wx/timer.h"
#include "wx/dcbuffer.h"


// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include <wx/propgrid/propgrid.h>

#include <wx/propgrid/propdev.h>

#ifdef __WXPYTHON__
    #include <wx/propgrid/advprops.h>
    #include <wx/propgrid/extras.h>
#endif

#if wxPG_USE_RENDERER_NATIVE
    #include <wx/renderer.h>
#endif

#include <wx/propgrid/odcombo.h>


// Two pics for the expand / collapse buttons.
// Files are not supplied with this project (since it is
// recommended to use either custom or native rendering).
// If you want them, get wxTreeMultiCtrl by Jorgen Bodde,
// and copy xpm files from archive to wxPropertyGrid src directory
// (and also comment/undef wxPG_ICON_WIDTH in propgrid.h
// and set wxPG_USE_RENDERER_NATIVE to 0).
#ifndef wxPG_ICON_WIDTH
  #if defined(__WXMAC__)
    #include "mac_collapse.xpm"
    #include "mac_expand.xpm"
  #elif defined(__WXGTK__)
    #include "linux_collapse.xpm"
    #include "linux_expand.xpm"
  #else
    #include "default_collapse.xpm"
    #include "default_expand.xpm"
  #endif
#endif


//#define wxPG_TEXT_INDENT                4 // For the wxComboControl
#define wxPG_ALLOW_CLIPPING             1 // If 1, GetUpdateRegion() in OnPaint event handler is not ignored
#define wxPG_GUTTER_DIV                 3 // gutter is max(iconwidth/gutter_div,gutter_min)
#define wxPG_GUTTER_MIN                 3 // gutter before and after image of [+] or [-]
#define wxPG_YSPACING_MIN               1
#define wxPG_BUTTON_SIZEDEC             0
#define wxPG_DEFAULT_VSPACING           2 // This matches .NET propertygrid's value,
                                          // but causes normal combobox to spill out under MSW

#define wxPG_OPTIMAL_WIDTH              200 // Arbitrary

#define wxPG_CAPRECTXMARGIN             2 // space between caption and selection rectangle,
#define wxPG_CAPRECTYMARGIN             1 // horizontally and vertically


#define wxPG_MIN_SCROLLBAR_WIDTH        10 // Smallest scrollbar width on any platform
                                           // Must be larger than largest control border
                                           // width * 2.


#define wxPG_DEFAULT_CURSOR             wxNullCursor
#define RedrawAllVisible                Refresh


//
// Here are some extra platform dependent defines.
//

#if defined(__WXMSW__)
    // tested

    #define wxPG_DEFAULT_SPLITTERX      110 // default splitter position

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    0 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   1  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X          0 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y          0 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY          1
    #define wxPG_NAT_BUTTON_BORDER_X            1
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    #define wxPG_TEXTCTRLYADJUST                (m_spacingy+0)

    #define wxPG_CHOICEXADJUST                  (-1) // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0 // Extra pixels above wxChoice/ComboBox.

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 0 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 1
    #define wxPG_CHECKMARK_YADJ                 (-1)
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

#elif defined(__WXGTK__)
    // tested

    #define wxPG_DEFAULT_SPLITTERX      110

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    1 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   0  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X      3 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y      3 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY      1
    #define wxPG_NAT_BUTTON_BORDER_X        1
    #define wxPG_NAT_BUTTON_BORDER_Y        1

    #define wxPG_TEXTCTRLYADJUST            0

    #define wxPG_CHOICEXADJUST                  2 // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 1 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 (-1)
    #define wxPG_CHECKMARK_HADJ                 (-1)
    #define wxPG_CHECKMARK_DEFLATE              3

#elif defined(__WXMAC__)
    // *not* tested

    #define wxPG_DEFAULT_SPLITTERX      110

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    0 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   1  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X      0 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y      0 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLYADJUST            3

    #define wxPG_CHOICEXADJUST                  0 // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 0 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

#else
    // defaults

    #define wxPG_DEFAULT_SPLITTERX      110

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    1 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   0  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X      0 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y      0 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLXADJUST            0 // position adjustment for wxTextCtrl
    #define wxPG_TEXTCTRLYADJUST            0

    #define wxPG_CHOICEXADJUST                  0 // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 1 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

#endif


#if wxPG_NO_CHILD_EVT_MOTION

    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
    #define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#else

    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
    #define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#endif


#define wxCC_CUSTOM_IMAGE_MARGIN1            4  // before image
#define wxCC_CUSTOM_IMAGE_MARGIN2            5  // after image


#if (!wxPG_NAT_TEXTCTRL_BORDER_X && !wxPG_NAT_TEXTCTRL_BORDER_Y)
    #define wxPG_ENABLE_CLIPPER_WINDOW      0
#else
    #define wxPG_ENABLE_CLIPPER_WINDOW      1
#endif


//#define wxPG_NAT_CHOICE_BORDER_ANY   0


// for odcombo
#undef wxPG_CHOICEXADJUST
#define wxPG_CHOICEXADJUST           0
#undef wxPG_CHOICEYADJUST
#define wxPG_CHOICEYADJUST           0

#define wxPG_DRAG_MARGIN                30

#define wxPG_CUSTOM_IMAGE_SPACINGY      1 // space between vertical sides of a custom image

// Use this macro to generate standard custom image height from
#define wxPG_STD_CUST_IMAGE_HEIGHT(LINEHEIGHT)  (LINEHEIGHT-3)

// How many pixels between textctrl and button (not used by custom controls)
#define wxPG_TEXTCTRL_AND_BUTTON_SPACING        2

#define wxPG_HIDER_BUTTON_HEIGHT        25

// m_expanded of wxPGPropertyWithChildren is set to this code if children should
// not be deleted in destructor.
#define wxPG_EXP_OF_COPYARRAY           127

#define wxPG_PIXELS_PER_UNIT            m_lineHeight

#ifdef wxPG_ICON_WIDTH
  #define m_iconheight m_iconwidth
#endif

#define wxPG_TOOLTIP_DELAY              1000

// Colour for the empty but visible space below last property.
#define wxPG_SLACK_BACKROUND        m_colPropBack


// Helper to decide which way is better (ie. first macro clears
// "unspecified" state of siblings of child properties as well, while the latter is
// more precise).
//#define CLEAR_PROPERTY_UNSPECIFIED_FLAG(p) wxPropertyGridState::ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED)
#define CLEAR_PROPERTY_UNSPECIFIED_FLAG(p) p->ClearFlag(wxPG_PROP_UNSPECIFIED)

#define __INTENSE_DEBUGGING__       0
#define __PAINT_DEBUGGING__         0
#define __MOUSE_DEBUGGING__         0


// -----------------------------------------------------------------------

#if wxUSE_INTL
void wxPropertyGrid::AutoGetTranslation ( bool enable )
{
    WX_PG_GLOBALS_LOCKER()

    wxPGGlobalVars->m_autoGetTranslation = enable;
}
#else
void wxPropertyGrid::AutoGetTranslation ( bool ) { }
#endif

// -----------------------------------------------------------------------

// This was needed to make quicker progress towards wxPropertyGridState
// TODO: Replace macros with real code.
#define FROM_STATE(X)       m_pState->X

// -----------------------------------------------------------------------

// DeviceContext Init Macros.

#define wxPG_CLIENT_DC_INIT() \
    wxClientDC dc(this); \
    PrepareDC(dc);

#define wxPG_CLIENT_DC_INIT_R(RETVAL) \
    wxClientDC dc(this); \
    PrepareDC(dc);

#define wxPG_PAINT_DC_INIT() \
    wxPaintDC dc(this); \
    PrepareDC(dc);

// -----------------------------------------------------------------------

// For wxMSW cursor consistency, we must do mouse capturing even
// when using custom controls.

#define BEGIN_MOUSE_CAPTURE \
    if ( !(m_iFlags & wxPG_FL_MOUSE_CAPTURED) ) \
    { \
        CaptureMouse(); \
        m_iFlags |= wxPG_FL_MOUSE_CAPTURED; \
    }

#define END_MOUSE_CAPTURE \
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED ) \
    { \
        ReleaseMouse(); \
        m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED); \
    }

// -----------------------------------------------------------------------
// NOTES
// -----------------------------------------------------------------------

//
// Long-term Plans:
// NB: No guarantees whatsoever of ever becoming into fruition.
// * Have append/insert take ref arg, to allow temporary instances
//   to be created.
//   - Maybe constructor could actually be void of any extra init,
//     to have it as lightweight as possible.
// * Rebuild the choices system.
//   Ideas:
//   - wxArrayString hashes: 2 - first is simple, second is
//     accurate. Simple is searched for first, to prevent the
//     potentially lengthy generation of accurate hash.
// * wxVariant values.
//   * Would not eliminate need for DoSetValue and GetValue in
//     all cases (but would in most).
//   * Value type would do ValueToString and ValueToInt conversion.
//     Property ptr would be given as arg, so that int value type
//     could convert itself into a choice string at value's index.
// * Consider: variant values would allow really flexible system.
// * wxVariant values for AddPropertyChoice.
//

//
// -----------------------------------------------------------------------
// TODO
// -----------------------------------------------------------------------
//

//
// For Next Release (1.1.7):
// * SetPropertyBackgroundColour instead of SetPropertyColour.
//
// Maybe later:
// * TAB should visit trigger buttons as well.
// * GetPropertyAttribute
// * WXK_RETURN to work with wxMSW wxEditEnumProperty
// * Typed client data (probably use wxClientData).
// * Fix Python SetValues Freeze-Thaw (regarding autofill=True).
// * Double-buffering to checkbox.
// * under Linux, on popups the un-focused version of currently selected
//   property flashes by.
// * Increase consistency of OnResize (need better testing code/procedure).
//   One potential solution is to use, as someone suggested, panel on
//   wxScrolledWindow.
// * If wxParentProperty is loaded from a text source, its value should
//   be set *after* all children are inserted.
// * Enable font images?
// * After 1.0.x compatibility removed, drop itemcount argument support.
//
// 1.3 Branch:
// * Replace SetAttribute with DoSetAttribute, and add SetAttribute as
//   user-usable method (currently there is only SetAttrib).
//
// High Priority but Difficult to implement or justify:
// * Put kbd focus rectangle to check box, but only when focus really comes
//   from kbd.
// * Child priority system more consistent (ie. wxPGPWC vs wxPP vs wxCP)
// * Consider: move (some of the) example properties to headers.
// * Finalize: Font property images (drop down could use them as well, and even
//   in more useful manner).
// * wxMSW: Scrollbar freeze after mode switch (did resizing before that).
// * Resolve global variable locking (caused deadlock under Linux - perhaps
//   it doesn't allow locking when one already active?).
//  - Writes to global variables are now locked, thus allowing different
//    property grids to be manipulated from different threads (one grid
//    should still only be referenced from single thread only).
//
// For Some of the Next Releases:
// * Consider: Semi-unfocus on when clicking (l+r) on empty space.
// * Consider: Match system font size in grid.
//   Problem: There is no preferred font point size detection.
// * Multiple delimiters for string tokenizers (e.g. ';' in addition to ',').
// * Temporary object file etc folder and file removal on install and uninstall.
// * Consider: Further additions to wxPropertyContainerMethods
//   (virtual Insert, Delete and DrawItemAndChildren, and then methods
//   that need them). This could reduce size of wxPropertyGridManager
//   code.
// * Consider: three-argument wxEnumProperty (choices would be added
//     with AddPropertyChoice). Already has this, but with docs too.
// * Error/Warning popups (preferably a balloon tooltip on WinNT-based OS).
// * Consider adding optional images in front of property label.
// * Improve string editor dialog (but how?).
//
// Bug Fixes:
// * wxGTK, no cc: Right-click doesn't occur if right-clicked on control.
// * wxGTK, no cc: TAB-based editor browsing ceases sometimes (try 2nd page, until
//   two disabled boolprops).
//
// Medium Priority:
// * wxPropertyGridManager's missing methods:
//   GetLastProperty.
// * Adding items when focused is no-no.
// * Properly fix DoGetBestSize (both controls). How? It is called but
//   result is probably somehow interpreted incorrectly.
//
// To Consider:
// * Simple checkbox: Background clear unaligned (see with grey colour scheme).
//   Problem: It is caused by the MSW checkmark drawing background clear.
// * Remove hover support to lighten some code (no tooltips, then).
//   Would basicly eliminate need for visibility cache (get first visible
//   instead viscache refresh and find items around it).
// * Custom image horizontal size increment in relation to line height.
//   Feb-13-2005: Only real help if people use really big ( ptsz>12 ) fonts.
//   So this is real low priority.
// * At some occasions, maybe use FindOrCreatePen and FindOrCreateBrush.
//   Feb-13-2005: More bloat for marginal gains.
// * Key event forwarding (use ex flag to mark that).
//   Feb-13-2005: Really needed?
// * Visual state saving (which items are expanded and which collapsed,
//   which mode is selected).
//   Feb-13-2005: More bloat?
// * Manager: SetCustomModeBitmaps. Call before toolbar is actually created.
//   Feb-13-2005: More bloat?
// * More drop-down editors (for wxLongStringProperty etc.) instead of modal dialogs.
//   Problem: complex, multi-sub-control popups do not work yet (will they ever work?).
//
// Low Priority:
// * wxPropertyGridManager::SetFont.
// * Basic context menu with Reset() and checkable Show Description that toggle the help box.
// * wxCustomEnumProperty - Allows user to set the image as well.
// * GetItemAtY() to use binary search.
// * Preparations for wxPGProperty direct usage.
// * Refine dialog-position auto-generator.
// * Maybe different colour lines between categories (like light grey ones in .NET).
//   (though I don't think its that elegant).
// * wxGTK1: Multichoice can't get value correctly.
// * wxGTK1: Control(s) disapper after:
//     Splitter centering.
//     Collapse expand.
//     - Refresh is not enough?
//

//
// Documentation Todo:
// * Set/GetPropertyValue: Strings work for all value types.
// * SetPropertyAttribute.
// * Having graphics glitches? Use Freeze and Thaw - they have extended functionaly in PG.
//


// -----------------------------------------------------------------------

const wxChar *wxPropertyGridNameStr = wxT("wxPropertyGrid");

const wxChar *wxPGTypeName_long = wxT("long");
const wxChar *wxPGTypeName_bool = wxT("bool");
const wxChar *wxPGTypeName_double = wxT("double");
const wxChar *wxPGTypeName_wxString = wxT("string");
const wxChar *wxPGTypeName_void = wxT("void*");
const wxChar *wxPGTypeName_wxArrayString = wxT("arrstring");

// -----------------------------------------------------------------------

static void wxPGDrawFocusRect( wxDC& dc, const wxRect& rect )
{
#if defined(__WXMSW__) && !defined(__WXWINCE__)
    /*
    RECT mswRect;
    mswRect.left = rect.x;
    mswRect.top = rect.y;
    mswRect.right = rect.x + rect.width;
    mswRect.bottom = rect.y + rect.height;
    HDC hdc = (HDC) dc.GetHDC();
    SetMapMode(hdc,MM_TEXT); // Just in case...
    DrawFocusRect(hdc,&mswRect);
    */
    // FIXME: Use DrawFocusRect code above (currently it draws solid line
    //   for caption focus but works ok for other stuff).
    //   Also, it seems that this code may not work in future wx versions.
    dc.SetLogicalFunction(wxINVERT);

    wxPen pen(*wxBLACK,1,wxDOT);
    pen.SetCap(wxCAP_BUTT);
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
#else
    dc.SetLogicalFunction(wxINVERT);

    dc.SetPen(wxPen(*wxBLACK,1,wxDOT));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
#endif
}


// -----------------------------------------------------------------------
// Choice related methods from various classes
// -----------------------------------------------------------------------

void wxPropertyContainerMethods::AddPropertyChoice( wxPGId id,
                                                    const wxString& label,
                                                    int value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);

    p->InsertChoice(label,-1,value);
}


void wxPropertyContainerMethods::InsertPropertyChoice( wxPGId id,
                                                       const wxString& label,
                                                       int index,
                                                       int value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);

    p->InsertChoice(label,index,value);
}


void wxPropertyContainerMethods::DeletePropertyChoice( wxPGId id,
                                                       int index )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);

    p->DeleteChoice(index);
}


// -----------------------------------------------------------------------
// Statics in one class for easy destruction.
// -----------------------------------------------------------------------

#include <wx/ptr_scpd.h>


// TODO: Consider converting this to wxModule.
wxDECLARE_SCOPED_PTR(wxPGGlobalVarsClass, wxPGGlobalVarsClassPtr)
wxDEFINE_SCOPED_PTR(wxPGGlobalVarsClass, wxPGGlobalVarsClassPtr)


static wxPGGlobalVarsClassPtr gs_spStaticVars;
wxPGGlobalVarsClass* wxPGGlobalVars = (wxPGGlobalVarsClass*) NULL;


wxPGGlobalVarsClass::wxPGGlobalVarsClass()
{
    wxPGGlobalVars = this;

    m_boolChoices[0] = _("False");
    m_boolChoices[1] = _("True");

    m_fontFamilyChoices = (wxPGChoices*) NULL;

    m_autoGetTranslation = false;

    m_offline = 0;
}


wxPGGlobalVarsClass::~wxPGGlobalVarsClass()
{
    size_t i;

    // This will always have one ref
    delete m_fontFamilyChoices;

#if wxUSE_VALIDATORS
    for ( i=0; i<m_arrValidators.GetCount(); i++ )
        delete ((wxValidator*)m_arrValidators[i]);
#endif

    //
    // Destroy value type class instances.
    wxPGHashMapS2P::iterator vt_it;

    for( vt_it = m_dictValueType.begin(); vt_it != m_dictValueType.end(); ++vt_it )
    {
        wxPGValueType* pcls = (wxPGValueType*) vt_it->second;
        wxASSERT ( pcls );
        delete pcls;
    }

    // Destroy editor class instances.
    // iterate over all the elements in the class
    for( vt_it = m_mapEditorClasses.begin(); vt_it != m_mapEditorClasses.end(); ++vt_it )
    {
        delete ((wxPGEditor*)vt_it->second);
    }
    /*for ( i=0; i< m_arrEditorClasses.GetCount(); i++ )
    {
        delete ((wxPGEditor*)m_arrEditorClasses.Item(i));
    }*/

}

// -----------------------------------------------------------------------
// wxPGProperty
// -----------------------------------------------------------------------

wxPGPropertyClassInfo wxBasePropertyClassInfo = {wxT("wxBaseProperty"),
                                                 (const wxPGPropertyClassInfo*) NULL,
                                                 (wxPGPropertyConstructor) NULL};

void wxPGProperty::Init()
{
    m_y = -3;
    m_arrIndex = 0xFFFF;
    m_parent = (wxPGPropertyWithChildren*) NULL;

#if wxPG_USE_CLIENT_DATA
    m_clientData = (void*) NULL;
#endif

    m_dataExt = (wxPGPropertyDataExt*) NULL;

    m_maxLen = 0; // infinite maximum length

#if wxPG_USE_VALIDATORS
    m_validator = (wxPropertyValidator*) NULL;
#endif
    m_flags = 0;

    m_depth = 1;
    m_parentingType = 0;
    m_bgColIndex = 0;
}


wxPGProperty::wxPGProperty()
#if wxPG_INCLUDE_WXOBJECT
    : wxObject()
#endif
{
    Init();
}


wxPGProperty::wxPGProperty( const wxString& label, const wxString& name )
#if wxPG_INCLUDE_WXOBJECT
    : wxObject()
#endif
{
    m_label = label;
    if ( &name != ((wxString*)NULL) )
        DoSetName ( name );
    else
        DoSetName ( label );

    Init();
}


wxPGProperty::~wxPGProperty()
{
#if wxPG_USE_VALIDATORS
    // Proper validator removal (delete if refcount reached zero)
    if ( m_validator && m_validator->UnRef() )
        delete m_validator;
#endif

    delete m_dataExt;
}


bool wxPGProperty::IsSomeParent ( wxPGProperty* candidate ) const
{
    wxPGPropertyWithChildren* parent = m_parent;
    do
    {
        if ( parent == (wxPGPropertyWithChildren*)candidate )
            return true;
        parent = parent->m_parent;
    } while ( parent );
    return false;
}


wxPropertyGridState* wxPGProperty::GetParentState() const
{
    wxASSERT ( m_parent );
    return m_parent->GetParentState();
}


size_t wxPGProperty::GetChildCount() const
{
    int cc = GetParentingType();
    if ( cc == 0 ) return 0;
    return ((wxPGPropertyWithChildren*)this)->GetCount();
}


void wxPGProperty::ShowError ( const wxString& msg )
{
    if ( !msg.length() )
        return;

#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxPropertyGrid* pg = GetParentState()->m_pPropGrid;
        wxASSERT (pg);
        wxWindow* topWnd = ::wxGetTopLevelParent(pg);
        if ( topWnd )
        {
            wxFrame* pFrame = wxDynamicCast(topWnd,wxFrame);
            if ( pFrame )
            {
                wxStatusBar* pStatusBar = pFrame->GetStatusBar();
                if ( pStatusBar )
                {
                    pStatusBar->SetStatusText(msg);
                    return;
                }
            }
        }
    }
#endif
    ::wxLogError(msg);
}


void wxPGProperty::UpdateControl( wxWindow* primary )
{
    if ( primary )
        GetEditorClass()->UpdateControl(this,primary);
}


void wxPGProperty::DoSetValue( wxPGVariant )
{
    // Actually, this should never get called
    wxFAIL_MSG( wxT("must be overridden") );
}


// wxPGRootPropertyClass, at least, should make use of this.
wxPGVariant wxPGProperty::DoGetValue() const
{
    return wxPGVariant((long)0);
}


wxString wxPGProperty::GetValueAsString( int ) const
{
    wxFAIL_MSG( wxT("must be overridden") );
    return m_name;
}


bool wxPGProperty::SetValueFromString( const wxString&, int )
{
    wxFAIL_MSG( wxT("must be overridden") );
    return false;
}


bool wxPGProperty::SetValueFromInt( long, int )
{
    wxFAIL_MSG ( wxT("must be overridden") );
    return false;
}


wxSize wxPGProperty::GetImageSize() const
{
    if ( m_dataExt && m_dataExt->m_valueBitmap )
        return wxSize(m_dataExt->m_valueBitmap->GetWidth(),-1);

    return wxSize(0,0);
}


void wxPGProperty::OnCustomPaint( wxDC& dc,
                                  const wxRect& rect,
                                  wxPGPaintData& )
{
    wxASSERT( m_dataExt );

    wxBitmap* bmp = m_dataExt->m_valueBitmap;

    wxASSERT( bmp && bmp->Ok() );

    wxASSERT_MSG( rect.x >= 0, wxT("unexpected measure call") );

    dc.DrawBitmap(*bmp,rect.x,rect.y);
}


const wxPGEditor* wxPGProperty::DoGetEditorClass() const
{
    return wxPG_EDITOR(TextCtrl);
}


// Default extra property event handling - that is, none at all.
bool wxPGProperty::OnEvent( wxPropertyGrid*, wxWindow*, wxEvent& )
{
    return false;
}


int wxPGProperty::InsertChoice( const wxString& label, int index, int value )
{
    wxPropertyGrid* pg = GetGrid();

    wxPGChoiceInfo ci;
    ci.m_choices = (wxPGChoices*) NULL;
    GetChoiceInfo(&ci);

    if ( ci.m_choices )
    {
        if ( index < 0 )
            index = ci.m_choices->GetCount();

        ci.m_choices->Insert(label, index, value);

        wxPGProperty* selected = pg->GetSelection();

        if ( this == selected )
            GetEditorClass()->InsertItem(pg->GetEditorControl(),label,index);

        return index;
    }

    return -1;
}


void wxPGProperty::DeleteChoice( int index )
{
    wxPropertyGrid* pg = GetGrid();

    wxPGChoiceInfo ci;
    ci.m_choices = (wxPGChoices*) NULL;
    GetChoiceInfo(&ci);

    if ( ci.m_choices )
    {
        ci.m_choices->RemoveAt(index);

        wxPGProperty* selected = pg->GetSelection();

        if ( this == selected )
            GetEditorClass()->DeleteItem(pg->GetEditorControl(),index);
    }
}


int wxPGProperty::GetChoiceInfo( wxPGChoiceInfo* )
{
    return 0;
}


void wxPGProperty::SetAttribute( int, wxVariant& )
{
}


#if wxUSE_VALIDATORS
wxValidator* wxPGProperty::DoGetValidator() const
{
    return (wxValidator*) NULL;
}
#endif


bool wxPGProperty::SetChoices( wxPGChoices& choices )
{
    wxPGChoiceInfo ci;
    ci.m_choices = (wxPGChoices*) NULL;

    // Unref existing
    GetChoiceInfo(&ci);
    if ( ci.m_choices )
    {
        ci.m_choices->Assign(choices);

        // This may be needed to trigger some initialization
        // (but don't do it if property is somewhat uninitialized)
        if ( m_parent )
            DoSetValue(GetValueType()->GetDefaultValue());

        return true;
    }
    return false;
}


const wxPGEditor* wxPGProperty::GetEditorClass() const
{
    if ( !m_dataExt || !m_dataExt->m_customEditor )
        return DoGetEditorClass();

    return m_dataExt->m_customEditor;
}


bool wxPGProperty::IsKindOf( wxPGPropertyClassInfo& info )
{
    const wxPGPropertyClassInfo* ownInfo = GetClassInfo();

    do
    {
        if ( ownInfo == &info )
            return true;

        ownInfo = ownInfo->m_baseInfo;
    } while ( ownInfo );

    return false;
}


// Privatizes set of choices
void wxPGProperty::SetChoicesExclusive()
{
    wxPGChoiceInfo ci;
    ci.m_choices = (wxPGChoices*) NULL;

    GetChoiceInfo(&ci);
    if ( ci.m_choices )
        ci.m_choices->SetExclusive();
}


#if wxPG_USE_VALIDATORS
void wxPGProperty::SetValidator ( wxPropertyValidator& validator )
{
    // Validator must have same data type?
    // NB: So that int validators can also be used with arrays,
    //   do not use this here:
    //   validator.AssertDataType(GetValueType()->GetTypeName());

    // Proper validator removal (delete if refcount reached zero)
    if ( m_validator && m_validator->UnRef() )
        delete m_validator;

    m_validator = validator.Ref();
}


wxPropertyValidator& wxPGProperty::GetValidator () const
{
    if ( m_validator )
    {
        return *m_validator;
    }
    wxFAIL_MSG ( wxT("Do not call GetPropertyValidator for properties that don't have one.") );
    return *((wxPropertyValidator*)NULL);
}


// Validates and sets value
bool wxPGProperty::StdValidationProcedure( wxPGVariant variant )
{
    if ( m_validator )
    {
        wxString s;
#ifdef __WXDEBUG__
        m_validator->AssertDataType(GetValueType()->GetTypeName());
#endif
        if ( !m_validator->Validate(variant,s) )
        {
            ShowError(s);
            return false;
        }
    }
    DoSetValue( variant );
    return true;
}

#endif


bool wxPGProperty::PrepareValueForDialogEditing( wxPropertyGrid* propgrid )
{
    wxWindow* primary = propgrid->GetEditorControl();
    if ( primary && propgrid->IsEditorsValueModified() )
    {
         GetEditorClass()->CopyValueFromControl( this, primary );
         return true;
    }
    else if ( m_flags & wxPG_PROP_UNSPECIFIED )
    {
        // Set default value in case it was unspecified
        DoSetValue(GetValueType()->GetDefaultValue());
    }
    return false;
}


bool wxPGProperty::RecreateEditor()
{
    wxPropertyGrid* pg = GetGrid();
    wxASSERT(pg);

    wxPGProperty* selected = pg->GetSelection();
    if ( this == selected )
    {
        pg->DoSelectProperty(this,wxPG_SEL_FORCE);
        return true;
    }
    return false;
}


bool wxPGProperty::EnsureDataExt()
{
    if ( !m_dataExt )
    {
        m_dataExt = new wxPGPropertyDataExt();
        return true;
    }
    return false;
}


void wxPGProperty::SetValueImage ( wxBitmap& bmp )
{
    EnsureDataExt();

    delete m_dataExt->m_valueBitmap;

    if ( &bmp && bmp != wxNullBitmap )
    {
        // Resize the image
        wxSize maxSz = GetGrid()->GetImageSize();
        wxSize imSz(bmp.GetWidth(),bmp.GetHeight());

        if ( imSz.x != maxSz.x || imSz.y != maxSz.y )
        {
            // Create a memory DC
            wxBitmap* bmpNew = new wxBitmap(maxSz.x,maxSz.y,bmp.GetDepth());

            wxMemoryDC dc;
            dc.SelectObject(*bmpNew);

            // Scale
            // FIXME: This is ugly - use image or wait for scaling patch.
            double scaleX = (double)maxSz.x / (double)imSz.x;
            double scaleY = (double)maxSz.y / (double)imSz.y;

            dc.SetUserScale(scaleX,scaleY);

            dc.DrawBitmap( bmp, 0, 0 );

            m_dataExt->m_valueBitmap = bmpNew;
        }
        else
            m_dataExt->m_valueBitmap = new wxBitmap(bmp);

        m_flags |= wxPG_PROP_CUSTOMIMAGE;
    }
    else
    {
        m_dataExt->m_valueBitmap = (wxBitmap*) NULL;
        m_flags &= wxPG_PROP_CUSTOMIMAGE;
    }
}


wxPGProperty* wxPGProperty::GetMainParent() const
{
    const wxPGProperty* curChild = this;
    const wxPGPropertyWithChildren* curParent = m_parent;

    while ( curParent->m_parentingType < 0 )
    {
        curChild = curParent;
        curParent = curParent->m_parent;
    }

    return (wxPGProperty*) curChild;
}


// -----------------------------------------------------------------------
// wxPGPropertyWithChildren
// -----------------------------------------------------------------------


wxPGPropertyClassInfo wxBaseParentPropertyClassInfo = {wxT("wxBaseParentProperty"),
                                                       &wxBasePropertyClassInfo,
                                                       (wxPGPropertyConstructor) NULL};


wxPGPropertyWithChildren::wxPGPropertyWithChildren()
    : wxPGProperty()
{
    m_expanded = 1;
    m_y = -2;
    m_bgColIndex = 0;
    m_parentingType = -1;
}

wxPGPropertyWithChildren::wxPGPropertyWithChildren( const wxString &label, const wxString& name )
    : wxPGProperty(label,name)
{
    m_expanded = 1;
    m_y = -2;
    m_bgColIndex = 0;
    m_parentingType = -1;
    m_parentState = (wxPropertyGridState*) NULL;
}


wxPGPropertyWithChildren::~wxPGPropertyWithChildren()
{
    Empty(); // this deletes items
}


// This is used by Insert etc.
void wxPGPropertyWithChildren::AddChild2( wxPGProperty* prop, int index, bool correct_mode )
{
    if ( index < 0 || (size_t)index >= m_children.GetCount() )
    {
        if ( correct_mode ) prop->m_arrIndex = m_children.GetCount();
        m_children.Add ( (void*)prop );
    }
    else
    {
        m_children.Insert ( (void*)prop, index );
        if ( correct_mode ) FixIndexesOfChildren( index );
    }

    prop->m_parent = this;
}

// This is used by properties that have fixed sub-properties
void wxPGPropertyWithChildren::AddChild( wxPGProperty* prop )
{
    prop->m_arrIndex = m_children.GetCount();
    m_children.Add ( (void*)prop );

    if ( prop->GetImageSize().y < 0 )
        prop->m_flags |= wxPG_PROP_CUSTOMIMAGE;

    prop->m_parent = this;

    prop->m_y = -1; // Collapsed
}


void wxPGPropertyWithChildren::FixIndexesOfChildren( size_t starthere )
{
    size_t i;
    for ( i=starthere;i<GetCount();i++)
        Item(i)->m_arrIndex = i;
}


// Returns (direct) child property with given name (or NULL if not found)
wxPGProperty* wxPGPropertyWithChildren::GetPropertyByName( const wxString& name ) const
{
    size_t i;

    for ( i=0; i<GetCount(); i++ )
    {
        wxPGProperty* p = Item(i);
        if ( p->m_name == name )
            return p;
    }

    // Does it have point, then?
    int pos = name.Find(wxT('.'));
    if ( pos <= 0 )
        return (wxPGProperty*) NULL;

    wxPGPropertyWithChildren* pwc =
        (wxPGPropertyWithChildren*) GetPropertyByName(name.substr(0,pos));

    if ( !pwc || !pwc->GetParentingType() )
        return (wxPGProperty*) NULL;

    return pwc->GetPropertyByName(name.substr(pos+1,name.length()-pos-1));
}


wxPGProperty* wxPGPropertyWithChildren::GetItemAtY( unsigned int y, unsigned int lh )
{
    // Linear search.
    unsigned int i = 0;
    unsigned int i_max = GetCount();
    unsigned long py = 0xFFFFFFFF;
    wxPGProperty* p = (wxPGProperty*) NULL;

    while ( i < i_max )
    {
        p = Item(i);
        if ( p->m_y >= 0 )
        {
            py = (unsigned long)p->m_y;
            if ( (py+lh) > y )
                break;
        }
        i++;
    }
    if ( py <= y && i < i_max )
    {
        // perfectly this item
        wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
        return p;
    }
    else
    {

        // If no visible children, we must retract our steps
        // (should not really happen, so right now we check that it
        // really doesn't).
        if ( py == 0xFFFFFFFF )
        {
            wxLogDebug(wxT("wxPropertyGrid: \"%s\" (y=%i) did not have visible children (it should)."),m_label.c_str(),(int)m_y);
            return (wxPGProperty*) NULL;
        }

        // We are about to return a child of previous' visible item.

    #ifdef __WXDEBUG__
        if ( i < 1 )
        {
            wxLogDebug ( wxT("WARNING: \"%s\"->GetItemAtY: (i <= 0)"), m_label.c_str() );
            wxLogDebug ( wxT(" \\--> y = %i, py = %i"), (int)y, (int)py );
            if ( p )
                wxLogDebug ( wxT(" \\--> p = \"%s\""), p->GetLabel().c_str() );
            else
                wxLogDebug ( wxT(" \\--> p = None") );
            return (wxPGProperty*) NULL;
        }
    #endif

        // Get previous *visible* parent.
        wxPGPropertyWithChildren* pwc;
        do
        {
            wxASSERT ( i > 0 );
            i--;
            pwc = (wxPGPropertyWithChildren*)Item(i);
        } while ( pwc->m_y < 0 );

        if ( pwc->GetParentingType() != 0 )
        {
        #ifdef __WXDEBUG__
            if ( !pwc->m_expanded || pwc->m_y < 0 )
            {
                wxLogDebug (wxT("WARNING: wxPGPropertyWithChildren::GetItemAtY: Item %s should have been visible and expanded."),pwc->m_label.c_str());
                wxLogDebug (wxT("    (%s[%i]: %s)"),pwc->m_parent->m_label.c_str(),pwc->m_arrIndex,pwc->m_label.c_str());
                //wxLogDebug (wxT("    py=%i"),(int)py);
                return (wxPGProperty*) NULL;
            }
        #endif
            return pwc->GetItemAtY(y,lh);
        }
    }
    return (wxPGProperty*) NULL;
}


void wxPGPropertyWithChildren::Empty()
{
    size_t i;
    if ( m_expanded != wxPG_EXP_OF_COPYARRAY )
        for (i=0;i<GetCount();i++)
            delete ((wxPGProperty*)Item(i));

    m_children.Empty();
}


void wxPGPropertyWithChildren::ChildChanged( wxPGProperty* WXUNUSED(p) )
{
}


wxString wxPGPropertyWithChildren::GetValueAsString( int arg_flags ) const
{
    wxASSERT_MSG( GetCount() > 0,
                  wxT("If user property does not have any children, it must override GetValueAsString.") );

    wxString text;

    int i;
    int i_max = m_children.GetCount();
    int i_max_minus_1 = i_max-1;

    wxPGProperty* curChild = (wxPGProperty*) m_children.Item(0);

    for ( i = 0; i < i_max; i++ )
    {
        wxString s;
        if ( !(curChild->m_flags & wxPG_PROP_UNSPECIFIED) )
            s = curChild->GetValueAsString(arg_flags);

        if ( curChild->GetParentingType() == 0 )
            text += s;
        else
            text += wxT("[") + s + wxT("]");

        if ( i < i_max_minus_1 )
        {
            curChild = (wxPGProperty*) m_children.Item(i+1);

            if ( curChild->GetParentingType() == 0 )
                text += wxT("; ");
            else
                text += wxT(" ");
        }
    }

    return text;
}


// Convert semicolon delimited tokens into child values.
bool wxPGPropertyWithChildren::SetValueFromString( const wxString& text, int )
{
    if ( !GetCount() )
        return false;

    unsigned int curChild = 0;

    bool changed = false;

    wxString token;
    size_t pos = 0;

    // Its best only to add non-empty group items
    bool addOnlyIfNotEmpty = false;
    const wxChar delimeter = wxT(';');
    wxChar a;

    size_t lastPos = text.length();
    size_t tokenStart = 0xFFFFFF;

    do
    {
        a = text[pos];

        if ( tokenStart != 0xFFFFFF )
        {
            // Token is running
            if ( a == delimeter || a == 0 )
            {
                token = text.substr(tokenStart,pos-tokenStart);
                token.Trim(true);
                size_t len = token.length();

                if ( !addOnlyIfNotEmpty || len > 0 )
                {
                    wxPGProperty* child = Item(curChild);

                    if ( len > 0 )
                    {
                        if ( child->SetValueFromString ( token, wxPG_REPORT_ERROR ) )
                        {
                            // If modified, set mod flag and store value back to parent
                            child->SetFlag( wxPG_PROP_MODIFIED );
                            child->ClearFlag( wxPG_PROP_UNSPECIFIED );
                            ChildChanged( child );
                            changed = true;
                        }

                    }
                    else
                    {
                        child->SetFlag( wxPG_PROP_UNSPECIFIED );
                        changed = true;
                    }

                    curChild++;
                    if ( curChild >= GetCount() )
                        break;
                }

                tokenStart = 0xFFFFFF;
            }
        }
        else
        {
            // Token is not running
            if ( a != wxT(' ') )
            {

                addOnlyIfNotEmpty = false;

                // Is this a group of tokens?
                if ( a == wxT('[') )
                {
                    int depth = 1;

                    pos++;
                    size_t startPos = pos;

                    // Group item - find end
                    do
                    {
                        a = text[pos];
                        pos++;

                        if ( a == wxT(']') )
                            depth--;
                        else if ( a == wxT('[') )
                            depth++;

                    } while ( depth > 0 && a );

                    token = text.substr(startPos,pos-startPos-1);
                    //wxLogDebug(wxT("token2: %s"),token.c_str());

                    if ( !token.length() )
                        break;

                    wxPGProperty* child = Item(curChild);

                    if ( child->SetValueFromString ( token, wxPG_REPORT_ERROR ) )
                    {
                        // If modified, set mod flag and store value back to parent
                        child->SetFlag ( wxPG_PROP_MODIFIED );
                        ChildChanged ( child );
                        changed = true;
                    }

                    curChild++;
                    if ( curChild >= GetCount() )
                        break;

                    addOnlyIfNotEmpty = true;

                    tokenStart = 0xFFFFFF;
                }
                else
                {
                    tokenStart = pos;

                    if ( a == delimeter )
                    {
                        pos--;
                    }
                }
            }

        }
        pos++;

    }
    while ( pos <= lastPos );

    // This ensures that the last item is set unspecified even
    // if the blank had no terminating delimiter.
    if ( curChild < GetCount() )
    {
        wxPGProperty* child = Item(curChild);

        child->SetFlag( wxPG_PROP_UNSPECIFIED );
        changed = true;
    }

    return changed;
}


void wxPGPropertyWithChildren::RefreshChildren ()
{
}

// -----------------------------------------------------------------------
// wxParentProperty
// -----------------------------------------------------------------------

wxPGProperty* wxParentProperty( const wxString& label, const wxString& name )
{
    return new wxParentPropertyClass (label,name);
}


WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxParentProperty,none,TextCtrl)
WX_PG_IMPLEMENT_CLASSINFO(wxParentProperty,wxBaseParentPropertyClass)


wxParentPropertyClass::wxParentPropertyClass ( const wxString& label, const wxString& name )
    : wxPGPropertyWithChildren(label,name)
{
    m_parentingType = -2;
}


wxParentPropertyClass::~wxParentPropertyClass () { }


void wxParentPropertyClass::DoSetValue ( wxPGVariant value )
{
    const wxString& str = wxPGVariantToString(value);
    m_string = str;
    SetValueFromString(str,wxPG_REPORT_ERROR);
}


wxPGVariant wxParentPropertyClass::DoGetValue () const
{
    return wxPGVariant();
}


void wxParentPropertyClass::ChildChanged ( wxPGProperty* WXUNUSED(p) )
{
}


wxString wxParentPropertyClass::GetValueAsString ( int arg_flags ) const
{
    if ( !GetCount() )
        return wxEmptyString;

    return wxPGPropertyWithChildren::GetValueAsString(arg_flags);
}


// -----------------------------------------------------------------------
// wxPGRootPropertyClass
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPGRootProperty,none,TextCtrl)
const wxPGPropertyClassInfo* wxPGRootPropertyClass::GetClassInfo() const
{
    return (const wxPGPropertyClassInfo*) NULL;
}


wxPGRootPropertyClass::wxPGRootPropertyClass()
    : wxPGPropertyWithChildren()
{
    m_parentingType = 1;
}


wxPGRootPropertyClass::~wxPGRootPropertyClass()
{
}


// -----------------------------------------------------------------------
// wxPropertyCategoryClass
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyCategory ( const wxString& label, const wxString& name )
{
    return new wxPropertyCategoryClass(label,name);
}


WX_PG_IMPLEMENT_CLASSINFO(wxPropertyCategory,wxBaseParentPropertyClass)


WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPropertyCategory,none,TextCtrl)


wxPropertyCategoryClass::wxPropertyCategoryClass()
    : wxPGPropertyWithChildren()
{
    // don't set colour - prepareadditem method should do this
    m_parentingType = 1;
}


wxPropertyCategoryClass::wxPropertyCategoryClass( const wxString &label, const wxString& name )
    : wxPGPropertyWithChildren(label,name)
{
    // don't set colour - prepareadditem method should do this
    m_parentingType = 1;
}


wxPropertyCategoryClass::~wxPropertyCategoryClass()
{
}


wxString wxPropertyCategoryClass::GetValueAsString ( int ) const
{
    return wxEmptyString;
}


void wxPropertyCategoryClass::CalculateTextExtent ( wxWindow* wnd, wxFont& font )
{
    int x = 0, y = 0;
	wnd->GetTextExtent( m_label, &x, &y, 0, 0, &font );
    m_textExtent = x;
}


// -----------------------------------------------------------------------
// wxPGEditor
// -----------------------------------------------------------------------

wxPGEditor::~wxPGEditor()
{
}


void wxPGEditor::DrawValue ( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const
{
    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        dc.DrawText( property->GetDisplayedString(), rect.x+wxPG_XBEFORETEXT, rect.y );
}


void wxPGEditor::SetControlStringValue ( wxWindow*, const wxString& ) const
{
}


void wxPGEditor::SetControlIntValue ( wxWindow*, int ) const
{
}


int wxPGEditor::InsertItem( wxWindow*, const wxString&, int ) const
{
    return -1;
}


void wxPGEditor::DeleteItem( wxWindow*, int ) const
{
    return;
}


void wxPGEditor::OnFocus( wxPGProperty*, wxWindow* ) const
{
}


bool wxPGEditor::CanContainCustomImage () const
{
    return false;
}

// -----------------------------------------------------------------------
// wxPGClipperWindow
// -----------------------------------------------------------------------


#if wxPG_ENABLE_CLIPPER_WINDOW

//
// Clipper window is used to "remove" borders from controls
// which otherwise insist on having them despite of supplied
// wxNO_BORDER window style.
//
class wxPGClipperWindow : public wxWindow
{
    DECLARE_CLASS(wxPGClipperWindow)
public:

    wxPGClipperWindow()
        : wxWindow()
    {
        wxPGClipperWindow::Init();
    }

    wxPGClipperWindow(wxWindow* parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize)
    {
        Init();
        Create(parent,id,pos,size);
    }

    void Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize);

    virtual ~wxPGClipperWindow();

    virtual bool ProcessEvent(wxEvent& event);

    inline wxWindow* GetControl () const { return m_ctrl; }

    // This is called before wxControl is constructed.
    void GetControlRect ( int xadj, int yadj, wxPoint& pt, wxSize& sz );

    // This is caleed after wxControl has been constructed.
    void SetControl ( wxWindow* ctrl );

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void SetFocus();

    virtual bool SetFont(const wxFont& font);

    inline int GetXClip () const { return m_xadj; }

    inline int GetYClip () const { return m_yadj; }

protected:
    wxWindow*       m_ctrl;

    int             m_xadj; // Horizontal border clip.

    int             m_yadj; // Vertical border clip.

private:
    void Init ()
    {
        m_ctrl = (wxWindow*) NULL;
    }
};


IMPLEMENT_CLASS(wxPGClipperWindow,wxWindow)


// This is called before wxControl is constructed.
void wxPGClipperWindow::GetControlRect ( int xadj, int yadj, wxPoint& pt, wxSize& sz )
{
    m_xadj = xadj;
    m_yadj = yadj;
    pt.x = -xadj;
    pt.y = -yadj;
    wxSize own_size = GetSize();
    sz.x = own_size.x+(xadj*2);
    sz.y = own_size.y+(yadj*2);
}


// This is caleed after wxControl has been constructed.
void wxPGClipperWindow::SetControl ( wxWindow* ctrl )
{
    m_ctrl = ctrl;

    // GTK requires this.
    ctrl->SetSizeHints(3,3);

    // Correct size of this window to match the child.
    wxSize sz = GetSize();
    wxSize chsz = ctrl->GetSize();

    int hei_adj = chsz.y - (sz.y+(m_yadj*2));
    if ( hei_adj )
        SetSize(sz.x,chsz.y-(m_yadj*2));

}


void wxPGClipperWindow::Refresh( bool eraseBackground, const wxRect *rect )
{
    wxWindow::Refresh(false,rect);
    if ( m_ctrl )
        // FIXME: Rect to sub-ctrl refresh too
        m_ctrl->Refresh(eraseBackground);
}


// Pass focus to control
void wxPGClipperWindow::SetFocus()
{
    if ( m_ctrl )
        m_ctrl->SetFocus();
    else
        wxWindow::SetFocus();
}


bool wxPGClipperWindow::SetFont(const wxFont& font)
{
    bool res = wxWindow::SetFont(font);
    if ( m_ctrl )
        return m_ctrl->SetFont(font);
    return res;
}


void wxPGClipperWindow::Create(wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size )
{
    wxWindow::Create(parent,id,pos,size);
}


wxPGClipperWindow::~wxPGClipperWindow()
{
}


bool wxPGClipperWindow::ProcessEvent(wxEvent& event)
{
    if ( event.GetEventType() == wxEVT_SIZE )
    {
        if ( m_ctrl )
        {
            // Maintain correct size relationship.
            wxSize sz = GetSize();
            m_ctrl->SetSize(sz.x+(m_xadj*2),sz.y+(m_yadj*2));
            event.Skip();
            return false;
        }
    }
    return wxWindow::ProcessEvent(event);
}

#endif // wxPG_ENABLE_CLIPPER_WINDOW

// -----------------------------------------------------------------------
// wxPGTextCtrlEditor
// -----------------------------------------------------------------------

// Clipper window support macro (depending on whether it is used
// for this editor or not)
#if wxPG_NAT_TEXTCTRL_BORDER_X || wxPG_NAT_TEXTCTRL_BORDER_Y
    #define wxPG_NAT_TEXTCTRL_BORDER_ANY    1
    #define wxPGDeclareRealTextCtrl(WND) \
        wxASSERT( WND ); \
        wxTextCtrl* tc = (wxTextCtrl*)((wxPGClipperWindow*)WND)->GetControl()
#else
    #define wxPG_NAT_TEXTCTRL_BORDER_ANY    0
    #define wxPGDeclareRealTextCtrl(WND) \
        wxASSERT( WND ); \
        wxTextCtrl* tc = (wxTextCtrl*)WND
#endif


WX_PG_IMPLEMENT_EDITOR_CLASS(TextCtrl,wxPGTextCtrlEditor,wxPGEditor)


wxWindow* wxPGTextCtrlEditor::CreateControls( wxPropertyGrid* propgrid,
                                              wxPGProperty* property,
                                              const wxPoint& pos,
                                              const wxSize& sz,
                                              wxWindow** ) const
{
    wxString text;

    // If has children and limited editing, then don't create.
    if ((property->GetFlags() & wxPG_PROP_NOEDITOR) &&
        property->GetParentingType() < 0 &&
        !property->IsKindOf(WX_PG_CLASSINFO(wxCustomProperty)))
        return (wxWindow*) NULL;

    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        text = property->GetValueAsString(0);

    int flags = 0;
    if ( (property->GetFlags() & wxPG_PROP_PASSWORD) &&
         property->IsKindOf(WX_PG_CLASSINFO(wxStringProperty)) )
        flags |= wxTE_PASSWORD;

    wxWindow* wnd = propgrid->GenerateEditorTextCtrl(pos,sz,text,(wxWindow*)NULL,flags,
                                                     property->GetMaxLength());

    return wnd;
}


void wxPGTextCtrlEditor::DrawValue( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const
{
    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
    {
        wxString drawStr = property->GetDisplayedString();

        if ( (property->GetFlags() & wxPG_PROP_PASSWORD) &&
             property->IsKindOf(WX_PG_CLASSINFO(wxStringProperty)) )
        {
            size_t a = drawStr.length();
            drawStr.Empty();
            drawStr.Append(wxT('*'),a);
        }
        dc.DrawText( drawStr, rect.x+wxPG_XBEFORETEXT, rect.y );
    }
}


void wxPGTextCtrlEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxPGDeclareRealTextCtrl(ctrl);
    tc->SetValue(property->GetDisplayedString());
}


// Provided so that, for example, ComboBox editor can use the same code
// (multiple inheritance would get way too messy).
bool wxPGTextCtrlEditor::OnTextCtrlEvent( wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          wxWindow* ctrl,
                                          wxEvent& event )
{
    if ( !ctrl )
        return false;

    //bool isTextCtrl = ctrl->IsKindOf(CLASSINFO(wxTextCtrl));

    if ( event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER )
    {
        if ( propgrid->IsEditorsValueModified() )
        {
            return true;
            /*
            if ( isTextCtrl )
            {
                if ( wxPGTextCtrlEditor::CopyTextCtrlValueFromControl(property,ctrl) )
                    return true;
            }
            else
            {
                if ( property->GetEditorClass()->CopyValueFromControl(property,ctrl) )
                    return true;
            }

            propgrid->EditorsValueWasNotModified();*/
        }
    }
    else if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED )
    {

        // If value is unspecified and character count is zero,
        // then do not set as modified.
        if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) ||
             !ctrl ||
             (ctrl->IsKindOf(CLASSINFO(wxTextCtrl)) &&
              ((wxTextCtrl*)ctrl)->GetLastPosition() > 0) )
        {

            // We must check this since an 'empty' text event
            // may be triggered when creating the property.
            if ( !(propgrid->GetInternalFlags() & wxPG_FL_IN_SELECT_PROPERTY) )
            {
                //
                // Pass this event outside wxPropertyGrid so that,
                // if necessary, program can tell when user is editing
                // a textctrl.
                // FIXME: Is it safe to change event id in the middle of event
                //        processing (seems to work, but...)?
                event.Skip();
                event.SetId(propgrid->GetId());
            }

            propgrid->EditorsValueWasModified();
        }
    }
    return false;
}


bool wxPGTextCtrlEditor::OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property,
    wxWindow* ctrl, wxEvent& event ) const
{
    return wxPGTextCtrlEditor::OnTextCtrlEvent(propgrid,property,ctrl,event);
}


bool wxPGTextCtrlEditor::CopyTextCtrlValueFromControl( wxPGProperty* property, wxWindow* ctrl )
{
#if wxPG_ENABLE_CLIPPER_WINDOW
    // Pass real control instead of clipper window
    if ( ctrl->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
    {
        ctrl = ((wxPGClipperWindow*)ctrl)->GetControl();
    }
#endif
    wxTextCtrl* tc = (wxTextCtrl*)ctrl;

    bool res = property->SetValueFromString(tc->GetValue(),0);

    // Changing unspecified always causes event (returning
    // true here should be enough to trigger it).
    if ( !res && property->IsFlagSet(wxPG_PROP_UNSPECIFIED) )
        res = true;

    return res;
}


bool wxPGTextCtrlEditor::CopyValueFromControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    return wxPGTextCtrlEditor::CopyTextCtrlValueFromControl(property,ctrl);
}


void wxPGTextCtrlEditor::SetValueToUnspecified( wxWindow* ctrl ) const
{
    wxPGDeclareRealTextCtrl(ctrl);

    tc->Remove(0,tc->GetValue().length());
}


void wxPGTextCtrlEditor::SetControlStringValue( wxWindow* ctrl, const wxString& txt ) const
{
    wxPGDeclareRealTextCtrl(ctrl);

    tc->SetValue(txt);
}


void wxPGTextCtrlEditor::OnFocus( wxPGProperty*, wxWindow* wnd ) const
{
    wxPGDeclareRealTextCtrl(wnd);

    tc->SetSelection(-1,-1);
}


wxPGTextCtrlEditor::~wxPGTextCtrlEditor() { }


// -----------------------------------------------------------------------
// wxPGChoiceEditor
// -----------------------------------------------------------------------

extern const wxChar* wxPG_ClassName_wxBoolProperty; // in props.cpp


WX_PG_IMPLEMENT_EDITOR_CLASS(Choice,wxPGChoiceEditor,wxPGEditor)


class wxPGComboBox : public wxPGOwnerDrawnComboBox
{
public:
    virtual bool OnDrawListItem( wxDC& dc, const wxRect& rect, int item, int flags )
    {
        wxPropertyGrid* pg = wxDynamicCast(GetParent(),wxPropertyGrid);
        wxASSERT(pg);
        pg->OnComboItemPaint((wxPGCustomComboControl*)this,item,dc,(wxRect&)rect,flags);
        return true;
    }
    virtual wxCoord OnMeasureListItem( int item )
    {
        wxPropertyGrid* pg = wxDynamicCast(GetParent(),wxPropertyGrid);
        wxASSERT(pg);
        wxRect rect;
        rect.x = -1;
        rect.width = 0;
        pg->OnComboItemPaint((wxPGCustomComboControl*)this,item,*((wxDC*)NULL),rect,0);
        return rect.height;
    }
    virtual wxCoord OnMeasureListItemWidth( int item )
    {
        wxPropertyGrid* pg = wxDynamicCast(GetParent(),wxPropertyGrid);
        wxASSERT(pg);
        wxRect rect;
        rect.x = -1;
        rect.width = -1;
        pg->OnComboItemPaint((wxPGCustomComboControl*)this,item,*((wxDC*)NULL),rect,0);
        return rect.width;
    }
};


void wxPropertyGrid::OnComboItemPaint( wxPGCustomComboControl* pCc,
                                       int item,
                                       wxDC& dc,
                                       wxRect& rect,
                                       int flags )
{
    wxPGOwnerDrawnComboBox* pCb = (wxPGOwnerDrawnComboBox*)pCc;

    // Sanity check
    wxASSERT( IsKindOf(CLASSINFO(wxPropertyGrid)) );

    wxPGProperty* p = m_selected;

    //
    // Decide what custom image size to use
    wxSize cis = GetImageSize(p);

    if ( rect.x < 0 &&
         !(m_iFlags & wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE) )
    {
        // Default measure behaviour (no flexible, custom paint image only)
        if ( rect.width < 0 )
        {
            wxCoord x, y;
            GetTextExtent(pCb->GetString(item), &x, &y, 0, 0, &m_font);
            rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9 + x;
        }

        rect.height = cis.y + 2;
        return;
    }

    wxPGPaintData paintdata;
    paintdata.m_parent = NULL;
    paintdata.m_choiceItem = item;

    // This is by the current (1.0.0b) spec - if painting control, item is -1
    if ( (flags & wxPGCC_PAINTING_CONTROL) )
        paintdata.m_choiceItem = -1;

    if ( &dc )
        dc.SetBrush(*wxWHITE_BRUSH);

    if ( rect.x >= 0 )
    {
        //
        // DrawItem call

        wxPoint pt(rect.x + wxPG_CONTROL_MARGIN - wxPG_CHOICEXADJUST - 1,
                   rect.y + 1);

        if ( cis.x > 0 &&
             ( !p->m_dataExt || !p->m_dataExt->m_valueBitmap || item == pCb->GetSelection() ) &&
             ( item >= 0 || (flags & wxPGCC_PAINTING_CONTROL) )
           )
        {
            pt.x += wxCC_CUSTOM_IMAGE_MARGIN1;
            wxRect r(pt.x,pt.y,cis.x,cis.y);

            if ( flags & wxPGCC_PAINTING_CONTROL )
            {
                //r.width = cis.x;
                r.height = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
            }

            if ( m_iFlags & wxPG_FL_SELECTED_IS_FULL_PAINT )
                r.width = rect.width;

            paintdata.m_drawnWidth = r.width;

            dc.SetPen(m_colPropFore);
            if ( item >= 0 )
                p->OnCustomPaint( dc, r, paintdata );
            else
                dc.DrawRectangle( r );

            if ( (m_iFlags & wxPG_FL_SELECTED_IS_FULL_PAINT) )
            {
                if ( paintdata.m_drawnWidth > 0 )
                    return;

                // Revert pt.x
                pt.x -= (wxCC_CUSTOM_IMAGE_MARGIN1+1);
            }
            else
                pt.x += paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2 - 1;
        }
        else
            // TODO: This aligns text so that it seems to be horizontally
            //       on the same line as property values. Not really
            //       sure if its needed, but seems to not cause any harm.
            pt.x -= 1;

        //
        // Draw text
        //

        pt.y += (rect.height-m_fontHeight)/2 - 1;

        wxString text;
        if ( !(flags & wxPGCC_PAINTING_CONTROL) )
            text = pCb->GetString(item);
        else
            text = p->GetValueAsString(0);

        dc.DrawText( text, pt.x + wxPG_XBEFORETEXT, pt.y );

    }
    else
    {
        //
        // MeasureItem call

        p->OnCustomPaint( dc, rect, paintdata );
        rect.height = paintdata.m_drawnHeight + 2;
        rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9;
    }
}


// CreateControls calls this with CB_READONLY in extraStyle
wxWindow* wxPGChoiceEditor::CreateControlsBase( wxPropertyGrid* propgrid,
                                                wxPGProperty* property,
                                                const wxPoint& pos,
                                                const wxSize& sz,
                                                long extraStyle ) const
{
    wxString        defstring;
    wxPGChoiceInfo  choiceinfo;

    // Get choices.
    choiceinfo.m_arrWxString = (wxString*) NULL;
    choiceinfo.m_arrWxChars = (const wxChar**) NULL;
    choiceinfo.m_itemCount = 0;

    int index = property->GetChoiceInfo( &choiceinfo );

    if ( property->GetFlags() & wxPG_PROP_UNSPECIFIED )
    {
        index = -1;
    }
    else
    {
        defstring = property->GetDisplayedString();
    }

    // SLAlloc allows fast conversion using potentially pre-allocated wxStrings
    // (and appending is out of question due to performance problems on some platforms).

    // If itemcount is < 0, fill wxArrayString using GetEntry
    if ( choiceinfo.m_itemCount < 0 )
    {
        wxBaseEnumPropertyClass* ep = (wxBaseEnumPropertyClass*) property;
        size_t i = 0;
        const wxString* entryLabel;
        int entryValue;

        wxArrayString& sl = propgrid->SLGet();

        entryLabel = ep->GetEntry(i,&entryValue);
        while ( entryLabel )
        {
            if ( sl.GetCount() > i )
                sl[i] = *entryLabel;
            else
                sl.Add(*entryLabel);
            i++;
            entryLabel = ep->GetEntry(i,&entryValue);
        }
        choiceinfo.m_itemCount = ((int)i) - 1;
    }
    else if ( !choiceinfo.m_arrWxString )
    {
        wxASSERT( choiceinfo.m_arrWxChars || !choiceinfo.m_itemCount );
        propgrid->SLAlloc( choiceinfo.m_itemCount, choiceinfo.m_arrWxChars );
        if ( choiceinfo.m_itemCount )
            choiceinfo.m_arrWxString = &propgrid->SLGet().Item(0);
    }

    //wxPGOwnerDrawnComboBox* cb;
    wxPGComboBox* cb;

    wxPoint po(pos);
    wxSize si(sz);
    po.y += wxPG_CHOICEYADJUST;
    si.y -= (wxPG_CHOICEYADJUST*2);

/*#if wxPG_NAT_CHOICE_BORDER_ANY
    po.x += (wxPG_CHOICEXADJUST+wxPG_NAT_CHOICE_BORDER_X);
    si.x -= (wxPG_CHOICEXADJUST+wxPG_NAT_CHOICE_BORDER_X);
    wxPGClipperWindow* wnd = new wxPGClipperWindow(propgrid,wxPG_SUBID1,po,si);
    wxWindow* ctrlParent = wnd;
    wnd->GetControlRect(wxPG_NAT_CHOICE_BORDER_X,wxPG_NAT_CHOICE_BORDER_Y,po,si);
#else*/
    po.x += wxPG_CHOICEXADJUST;
    si.x -= wxPG_CHOICEXADJUST;
    wxWindow* ctrlParent = propgrid;
//#endif

    int odcbFlags = extraStyle | wxNO_BORDER | wxPGCC_ALT_KEYS;

    if ( !(property->GetFlags() & wxPG_PROP_CUSTOMIMAGE) )
        odcbFlags |= wxODCB_STD_CONTROL_PAINT;

    if ( (property->GetFlags() & wxPG_PROP_USE_DCC) &&
         (property->GetClassName()==wxPG_ClassName_wxBoolProperty) )
        odcbFlags |= wxPGCC_DCLICK_CYCLES;

    cb = new wxPGComboBox();
#ifdef __WXMSW__
    cb->Hide();
#endif
    cb->Create(ctrlParent,
               wxPG_SUBID1,
               wxString(wxEmptyString),
               po,
               si,
               choiceinfo.m_itemCount,choiceinfo.m_arrWxString,
               //(wxComboPaintCallback) &wxPropertyGrid::OnComboItemPaint,
               odcbFlags);

    int extRight = propgrid->GetClientSize().x - (po.x+si.x);

    cb->SetButtonPosition(si.y,0,wxRIGHT);
    cb->SetPopupExtents( 1, extRight );
    cb->SetTextIndent(wxPG_XBEFORETEXT-2);

    if ( (property->GetFlags() & wxPG_PROP_CUSTOMIMAGE) &&
         !(propgrid->GetInternalFlags() & wxPG_FL_SELECTED_IS_FULL_PAINT) )
    {
        wxSize imageSize = propgrid->GetImageSize(property);
        cb->SetCustomPaintWidth( imageSize.x+6 );
    }

    if ( index >= 0 && index < (int)cb->GetCount() )
    {
        cb->SetSelection( index );
        if ( defstring.length() )
            cb->SetValue( defstring );
    }
    else if ( !(extraStyle & wxCB_READONLY) && defstring.length() )
        cb->SetValue( defstring );
    else
        cb->SetSelection( -1 );

#ifdef __WXMSW__
    cb->Show();
#endif

    return (wxWindow*) cb;
}


void wxPGChoiceEditor::UpdateControl ( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxASSERT ( ctrl );
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    wxASSERT ( cb->IsKindOf(CLASSINFO(wxPGOwnerDrawnComboBox)));
    int ind = property->GetChoiceInfo( (wxPGChoiceInfo*)NULL );
    cb->SetSelection(ind);
}


wxWindow* wxPGChoiceEditor::CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxWindow** ) const
{
    return CreateControlsBase(propgrid,property,pos,sz,wxCB_READONLY);
}


int wxPGChoiceEditor::InsertItem( wxWindow* ctrl, const wxString& label, int index ) const
{
    wxASSERT ( ctrl );
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    wxASSERT ( cb->IsKindOf(CLASSINFO(wxPGOwnerDrawnComboBox)));

    if (index < 0)
        index = cb->GetCount();

    return cb->Insert(label,index);
}


void wxPGChoiceEditor::DeleteItem( wxWindow* ctrl, int index ) const
{
    wxASSERT ( ctrl );
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    wxASSERT ( cb->IsKindOf(CLASSINFO(wxPGOwnerDrawnComboBox)));

    cb->Delete(index);
}


bool wxPGChoiceEditor::OnEvent( wxPropertyGrid* WXUNUSED(propgrid), wxPGProperty* WXUNUSED(property),
    wxWindow* WXUNUSED(ctrl), wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
    {
        /*if ( CopyValueFromControl( property, ctrl ) )
        {
            return true;
        }

        propgrid->EditorsValueWasNotModified();

        //wxPropertyGridState::ClearPropertyAndChildrenFlags(property,wxPG_PROP_UNSPECIFIED);
        CLEAR_PROPERTY_UNSPECIFIED_FLAG(property);*/

        return true;

    }
    return false;
}


bool wxPGChoiceEditor::CopyValueFromControl ( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;

    int index = cb->GetSelection();

    if ( index != property->GetChoiceInfo( (wxPGChoiceInfo*) NULL ) ||
        // Changing unspecified always causes event (returning
        // true here should be enough to trigger it).
         property->IsFlagSet(wxPG_PROP_UNSPECIFIED)
       )
    {
        property->SetValueFromInt(index,0);
        return true;
    }
    return false;
}


void wxPGChoiceEditor::SetControlStringValue ( wxWindow* ctrl, const wxString& txt ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    wxASSERT ( cb );
    cb->SetValue(txt);
}


void wxPGChoiceEditor::SetControlIntValue ( wxWindow* ctrl, int value ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    wxASSERT ( cb );
    cb->SetSelection(value);
}


void wxPGChoiceEditor::SetValueToUnspecified ( wxWindow* ctrl ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    cb->SetSelection(-1);
}


bool wxPGChoiceEditor::CanContainCustomImage () const
{
    return true;
}


wxPGChoiceEditor::~wxPGChoiceEditor() { }


// -----------------------------------------------------------------------
// wxPGComboBoxEditor
// -----------------------------------------------------------------------


WX_PG_IMPLEMENT_EDITOR_CLASS(ComboBox,wxPGComboBoxEditor,wxPGChoiceEditor)


void wxPGComboBoxEditor::UpdateControl ( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    cb->SetValue(property->GetDisplayedString());

    // TODO: If string matches any selection, then select that.
}


wxWindow* wxPGComboBoxEditor::CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxWindow** ) const
{
    return CreateControlsBase(propgrid,property,pos,sz,0);
}


bool wxPGComboBoxEditor::OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property,
    wxWindow* ctrl, wxEvent& event ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*) NULL;
    wxWindow* textCtrl = (wxWindow*) NULL;

    if ( ctrl )
    {
        cb = (wxPGOwnerDrawnComboBox*)ctrl;
        textCtrl = cb->GetTextCtrl();
    }

    if ( wxPGTextCtrlEditor::OnTextCtrlEvent(propgrid,property,textCtrl,event) )
        return true;

    return wxPGChoiceEditor::OnEvent(propgrid,property,ctrl,event);
}


bool wxPGComboBoxEditor::CopyValueFromControl ( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;

    bool res = property->SetValueFromString(cb->GetValue(),0);

    // Changing unspecified always causes event (returning
    // true here should be enough to trigger it).
    if ( !res && property->IsFlagSet(wxPG_PROP_UNSPECIFIED) )
        res = true;

    return res;
}


void wxPGComboBoxEditor::OnFocus ( wxPGProperty*, wxWindow* ctrl ) const
{
    wxPGOwnerDrawnComboBox* cb = (wxPGOwnerDrawnComboBox*)ctrl;
    cb->GetTextCtrl()->SetSelection(-1,-1);
}


wxPGComboBoxEditor::~wxPGComboBoxEditor() { }


// -----------------------------------------------------------------------
// wxPGChoiceAndButtonEditor
// -----------------------------------------------------------------------


// This simpler implement_editor macro doesn't define class body.
WX_PG_IMPLEMENT_EDITOR_CLASS(ChoiceAndButton,wxPGChoiceAndButtonEditor,wxPGChoiceEditor)


wxWindow* wxPGChoiceAndButtonEditor::CreateControls( wxPropertyGrid* propgrid,
                                                     wxPGProperty* property,
                                                     const wxPoint& pos,
                                                     const wxSize& sz,
                                                     wxWindow** psecondary ) const
{

    // Size of button.
    // Use one two units smaller to match size of the combo's dropbutton.
    // (normally a bigger button is used because it looks better)
    int bt_wid = sz.y;
    bt_wid -= 2;
    wxSize bt_sz(bt_wid,bt_wid);

    // Position of button.
    wxPoint bt_pos(pos.x+sz.x-bt_sz.x,pos.y);
    bt_pos.y += 1;

    wxWindow* bt = propgrid->GenerateEditorButton ( bt_pos, bt_sz );

    // Size of choice.
    wxSize ch_sz(sz.x-bt->GetSize().x,sz.y);

    wxWindow* ch = wxPG_EDITOR(Choice)->CreateControls(propgrid,property,
        pos,ch_sz,(wxWindow**)NULL);

#ifdef __WXMSW__
    bt->Show();
#endif

    *psecondary = bt;
    return ch;
}


wxPGChoiceAndButtonEditor::~wxPGChoiceAndButtonEditor() { }


// -----------------------------------------------------------------------
// wxPGTextCtrlAndButtonEditor
// -----------------------------------------------------------------------


class wxPGTextCtrlAndButtonEditor : public wxPGTextCtrlEditor
{
    WX_PG_DECLARE_EDITOR_CLASS()
public:
    virtual ~wxPGTextCtrlAndButtonEditor();
    virtual wxWindow* CreateControls( wxPropertyGrid* propgrid,
                                      wxPGProperty* property,
                                      const wxPoint& pos,
                                      const wxSize& sz,
                                      wxWindow** psecondary ) const;
};


// This simpler implement_editor macro doesn't define class body.
WX_PG_IMPLEMENT_EDITOR_CLASS(TextCtrlAndButton,wxPGTextCtrlAndButtonEditor,wxPGTextCtrlEditor)


wxWindow* wxPGTextCtrlAndButtonEditor::CreateControls( wxPropertyGrid* propgrid,
                                                       wxPGProperty* property,
                                                       const wxPoint& pos,
                                                       const wxSize& sz,
                                                       wxWindow** psecondary ) const
{
    wxWindow* wnd = propgrid->GenerateEditorTextCtrlAndButton ( pos, sz, psecondary,
        property->GetFlags() & wxPG_PROP_NOEDITOR, property);

    return wnd;
}


wxPGTextCtrlAndButtonEditor::~wxPGTextCtrlAndButtonEditor() { }


// -----------------------------------------------------------------------
// wxPGCheckBoxEditor
// -----------------------------------------------------------------------

#if wxPG_INCLUDE_CHECKBOX

WX_PG_IMPLEMENT_EDITOR_CLASS(CheckBox,wxPGCheckBoxEditor,wxPGEditor)


// state argument: 0x01 = set if checked
//                 0x02 = set if rectangle should be bold
static void DrawSimpleCheckBox( wxDC& dc, const wxRect& rect, int box_hei, int state, const wxColour& linecol )
{

    // Box rectangle.
    wxRect r(rect.x+wxPG_XBEFORETEXT,rect.y+((rect.height-box_hei)/2),box_hei,box_hei);

    // Draw check mark first because it is likely to overdraw the
    // surrounding rectangle.
    if ( state & 1 )
    {
        wxRect r2(r.x+wxPG_CHECKMARK_XADJ,
                  r.y+wxPG_CHECKMARK_YADJ,
                  r.width+wxPG_CHECKMARK_WADJ,
                  r.height+wxPG_CHECKMARK_HADJ);
    #if wxPG_CHECKMARK_DEFLATE
        r2.Deflate(wxPG_CHECKMARK_DEFLATE);
    #endif
        dc.DrawCheckMark(r2);

        // This would draw a simple cross check mark.
        // dc.DrawLine(r.x,r.y,r.x+r.width-1,r.y+r.height-1);
        // dc.DrawLine(r.x,r.y+r.height-1,r.x+r.width-1,r.y);

    }

    if ( !(state & 2) )
    {
        // Pen for thin rectangle.
        dc.SetPen(linecol);
    }
    else
    {
        // Pen for bold rectangle.
        wxPen linepen(linecol,2,wxSOLID);
        linepen.SetJoin(wxJOIN_MITER); // This prevents round edges.
        dc.SetPen(linepen);
        r.x++;
        r.y++;
        r.width--;
        r.height--;
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(r);
    dc.SetPen(*wxTRANSPARENT_PEN);
}

//
// Real simple custom-drawn checkbox-without-label class.
//
class wxSimpleCheckBox : public wxControl
{
public:

    void SetValue( int value );

    wxSimpleCheckBox( wxWindow* parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize )
        : wxControl(parent,id,pos,size,wxNO_BORDER|wxWANTS_CHARS)
    {

        // Due to SetOwnFont stuff necessary for GTK+ 1.2, we need to have this
        SetFont( parent->GetFont() );

        m_state = 0;
        m_boxHeight = ((wxPropertyGrid*)parent)->GetFontHeight();
        SetBackgroundStyle( wxBG_STYLE_COLOUR );
    }

    virtual ~wxSimpleCheckBox();

    virtual bool ProcessEvent(wxEvent& event);

    int m_state;
    int m_boxHeight;

    static wxBitmap* ms_doubleBuffer;

};

wxSimpleCheckBox::~wxSimpleCheckBox()
{
    delete ms_doubleBuffer;
    ms_doubleBuffer = NULL;
}


wxBitmap* wxSimpleCheckBox::ms_doubleBuffer = (wxBitmap*) NULL;

// value = 2 means toggle (sorry, too lazy to do constants)
void wxSimpleCheckBox::SetValue( int value )
{
    if ( value > 1 )
    {
        m_state++;
        if ( m_state > 1 ) m_state = 0;
    }
    else
    {
        m_state = value;
    }
    Refresh();

    wxCommandEvent evt(wxEVT_COMMAND_CHECKBOX_CLICKED,GetParent()->GetId());
    ((wxPropertyGrid*)GetParent())->OnCustomEditorEvent(evt);
}


bool wxSimpleCheckBox::ProcessEvent(wxEvent& event)
{
    wxPropertyGrid* propGrid = (wxPropertyGrid*) GetParent();

    if ( event.GetEventType() == wxEVT_NAVIGATION_KEY )
    {
        //wxLogDebug(wxT("wxEVT_NAVIGATION_KEY"));
        //SetFocusFromKbd();
        //event.Skip();
        //return wxControl::ProcessEvent(event);
    }
    else
    if ( ( (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK)
          && ((wxMouseEvent&)event).m_x > (wxPG_XBEFORETEXT-2)
          && ((wxMouseEvent&)event).m_x <= (wxPG_XBEFORETEXT-2+m_boxHeight) )
       )
    {
        SetValue(2);
        return true;
    }
    else if ( event.GetEventType() == wxEVT_PAINT )
    {
        wxSize clientSize = GetClientSize();
        wxPaintDC dc(this);

        /*
        // Buffered paint DC doesn't seem to do much good
        if ( !ms_doubleBuffer ||
             clientSize.x > ms_doubleBuffer->GetWidth() ||
             clientSize.y > ms_doubleBuffer->GetHeight() )
        {
            delete ms_doubleBuffer;
            ms_doubleBuffer = new wxBitmap(clientSize.x+25,clientSize.y+25);
        }

        wxBufferedPaintDC dc(this,*ms_doubleBuffer);
        */

        wxRect rect(0,0,clientSize.x,clientSize.y);
        rect.x -= 1;
        rect.width += 1;

        m_boxHeight = propGrid->GetFontHeight();

        wxColour bgcol = GetBackgroundColour();
        dc.SetBrush( bgcol );
        dc.SetPen( bgcol );
        dc.DrawRectangle( rect );

        wxColour txcol = GetForegroundColour();

        int state = m_state;
        if ( m_font.GetWeight() == wxBOLD )
            state |= 2;

        DrawSimpleCheckBox(dc,rect,m_boxHeight,state,txcol);

        // If focused, indicate it somehow.
        /*
        if ( wxWindow::FindFocus() == this )
        {
            rect.x += 1;
            rect.width -= 1;

            wxPGDrawFocusRect(dc,rect);
        }
        */

        return true;
    }
    else if ( event.GetEventType() == wxEVT_SIZE ||
              event.GetEventType() == wxEVT_SET_FOCUS ||
              event.GetEventType() == wxEVT_KILL_FOCUS
            )
    {
        Refresh();
    }
    else if ( event.GetEventType() == wxEVT_KEY_DOWN )
    {
        wxKeyEvent& keyEv = (wxKeyEvent&) event;

        if ( keyEv.GetKeyCode() == WXK_TAB )
        {
            propGrid->SendNavigationKeyEvent( keyEv.ShiftDown()?0:1 );
            return true;
        }
        else
        if ( keyEv.GetKeyCode() == WXK_SPACE )
        {
            SetValue(2);
            return true;
        }
    }
    return wxControl::ProcessEvent(event);
}


wxWindow* wxPGCheckBoxEditor::CreateControls( wxPropertyGrid* propgrid,
                                              wxPGProperty* property,
                                              const wxPoint& pos,
                                              const wxSize& size,
                                              wxWindow** ) const
{
    wxPoint pt = pos;
    pt.x -= wxPG_XBEFOREWIDGET;
    wxSize sz = size;
    sz.x += wxPG_XBEFOREWIDGET;

    wxSimpleCheckBox* cb = new wxSimpleCheckBox(propgrid,wxPG_SUBID1,pt,sz);

    cb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    cb->Connect( wxPG_SUBID1, wxEVT_LEFT_DOWN,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &wxPropertyGrid::OnCustomEditorEvent, NULL, propgrid );

    cb->Connect( wxPG_SUBID1, wxEVT_LEFT_DCLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &wxPropertyGrid::OnCustomEditorEvent, NULL, propgrid );

    if ( property->GetChoiceInfo((wxPGChoiceInfo*)NULL) &&
         !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        cb->m_state = 1;

    // If mouse cursor was on the item, toggle the value now.
    if ( propgrid->GetInternalFlags() & wxPG_FL_ACTIVATION_BY_CLICK )
    {
        wxPoint pt = propgrid->ScreenToClient(::wxGetMousePosition());
        if ( pt.x <= (cb->GetPosition().x+wxPG_XBEFORETEXT-2+cb->m_boxHeight) )
        {
            cb->m_state++;

            if ( cb->m_state > 1 )
                cb->m_state = 0;

            property->ClearFlag(wxPG_PROP_UNSPECIFIED);
            property->SetValueFromInt(cb->m_state,0);
            propgrid->PropertyWasModified(property);
        }
    }

    return cb;
}


void wxPGCheckBoxEditor::DrawValue( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const
{
    int state = 0;
    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
    {
        state = property->GetChoiceInfo((wxPGChoiceInfo*)NULL);
        if ( dc.GetFont().GetWeight() == wxBOLD ) state |= 2;
    }
    DrawSimpleCheckBox(dc,rect,dc.GetCharHeight(),state,dc.GetTextForeground());
}


void wxPGCheckBoxEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxASSERT ( ctrl );
    ((wxSimpleCheckBox*)ctrl)->m_state = property->GetChoiceInfo((wxPGChoiceInfo*)NULL);
    ctrl->Refresh();
}


bool wxPGCheckBoxEditor::OnEvent( wxPropertyGrid* WXUNUSED(propgrid), wxPGProperty* WXUNUSED(property),
    wxWindow* WXUNUSED(ctrl), wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_CHECKBOX_CLICKED )
    {
        /*if ( CopyValueFromControl( property, ctrl ) )
            return true;

        propgrid->EditorsValueWasNotModified();

        CLEAR_PROPERTY_UNSPECIFIED_FLAG(property);*/
        return true;
    }
    return false;
}


bool wxPGCheckBoxEditor::CopyValueFromControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxSimpleCheckBox* cb = (wxSimpleCheckBox*)ctrl;

    int index = cb->m_state;

    if ( index != property->GetChoiceInfo( (wxPGChoiceInfo*) NULL ) ||
         // Changing unspecified always causes event (returning
         // true here should be enough to trigger it).
         property->IsFlagSet(wxPG_PROP_UNSPECIFIED)
       )
    {
        property->SetValueFromInt(index,0);
        return true;
    }
    return false;
}


void wxPGCheckBoxEditor::SetControlIntValue( wxWindow* ctrl, int value ) const
{
    if ( value != 0 ) value = 1;
    ((wxSimpleCheckBox*)ctrl)->m_state = value;
    ctrl->Refresh();
}


void wxPGCheckBoxEditor::SetValueToUnspecified( wxWindow* ctrl ) const
{
    ((wxSimpleCheckBox*)ctrl)->m_state = 0;
    ctrl->Refresh();
}


wxPGCheckBoxEditor::~wxPGCheckBoxEditor() { }


#endif // wxPG_INCLUDE_CHECKBOX

// -----------------------------------------------------------------------
// wxPGBrush
// -----------------------------------------------------------------------

//
// This class is a wxBrush derivative used in the background colour
// brush cache. It adds wxPG-type colour-in-long to the class.
// JMS: Yes I know wxBrush doesn't actually hold the value (refcounted
//   object does), but this is simpler implementation and equally
//   effective.
//

class wxPGBrush : public wxBrush
{
public:
    wxPGBrush( const wxColour& colour );
    wxPGBrush();
    virtual ~wxPGBrush() { }
    void SetColour2( const wxColour& colour );
    inline long GetColourAsLong() const { return m_colAsLong; }
protected:
    long    m_colAsLong;
};


void wxPGBrush::SetColour2( const wxColour& colour )
{
    wxBrush::SetColour(colour);
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}


wxPGBrush::wxPGBrush() : wxBrush()
{
    m_colAsLong = 0;
}


wxPGBrush::wxPGBrush( const wxColour& colour ) : wxBrush(colour)
{
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}


// -----------------------------------------------------------------------
// wxPropertyGrid
// -----------------------------------------------------------------------

IMPLEMENT_CLASS(wxPropertyGrid, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxPropertyGrid, wxScrolledWindow)
  EVT_MOTION(wxPropertyGrid::OnMouseMove)
  EVT_LEFT_DOWN(wxPropertyGrid::OnMouseClick)
  EVT_LEFT_UP(wxPropertyGrid::OnMouseUp)
  EVT_RIGHT_UP(wxPropertyGrid::OnMouseRightClick)
  EVT_LEFT_DCLICK(wxPropertyGrid::OnMouseClick)
  EVT_PAINT(wxPropertyGrid::OnPaint)
  EVT_SIZE(wxPropertyGrid::OnResize)
  EVT_KEY_DOWN(wxPropertyGrid::OnKey)
  EVT_CHAR(wxPropertyGrid::OnKey)
  EVT_ENTER_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_LEAVE_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_MOUSE_CAPTURE_CHANGED(wxPropertyGrid::OnCaptureChange)
  EVT_SCROLLWIN(wxPropertyGrid::OnScrollEvent)
  EVT_NAVIGATION_KEY(wxPropertyGrid::OnNavigationKey)
  EVT_TEXT(wxPG_SUBID1,wxPropertyGrid::OnCustomEditorEvent)
  EVT_COMBOBOX(wxPG_SUBID1,wxPropertyGrid::OnCustomEditorEvent)
  EVT_BUTTON(wxPG_SUBID2,wxPropertyGrid::OnCustomEditorEvent)
  EVT_SET_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_KILL_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_TEXT_ENTER(wxPG_SUBID1,wxPropertyGrid::OnCustomEditorEvent)
  EVT_SYS_COLOUR_CHANGED(wxPropertyGrid::OnSysColourChanged)
END_EVENT_TABLE()


// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid()
    : wxScrolledWindow()
{
    Init1();
}

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxChar* name )
    : wxScrolledWindow()
{
    Init1();
    Create(parent,id,pos,size,style,name);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Create( wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxChar* name )
{

    if ( !(style&wxBORDER_MASK) )
        style |= wxSIMPLE_BORDER;

    style |= wxVSCROLL;

#ifdef __WXMSW__
    // This prevents crash under Win2K, but still
    // enables keyboard navigation
    if ( style & wxTAB_TRAVERSAL )
    {
        style &= ~(wxTAB_TRAVERSAL);
        style |= wxWANTS_CHARS;
    }
#else
    if ( style & wxTAB_TRAVERSAL )
        style |= wxWANTS_CHARS;
#endif

    wxScrolledWindow::Create(parent,id,pos,size,style,name);

    Init2();

    return true;
}

// -----------------------------------------------------------------------

static void wxPGRegisterStandardPropertyClasses();

//
// Initialize values to defaults
//
void wxPropertyGrid::Init1()
{

    // Init common variables.
    if ( !wxPGGlobalVars )
        gs_spStaticVars.reset( new wxPGGlobalVarsClass );

    WX_PG_GLOBALS_LOCKER()

    // Register type classes, if necessary.
    if ( wxPGGlobalVars->m_dictValueType.empty() )
        RegisterDefaultValues();

    // Register editor classes, if necessary.
    if ( wxPGGlobalVars->m_mapEditorClasses.empty() )
        RegisterDefaultEditors();

    // Register property classes, if necessary
    if ( wxPGGlobalVars->m_dictPropertyClassInfo.empty() )
        wxPGRegisterStandardPropertyClasses();

    m_iFlags = 0;

    m_pState = (wxPropertyGridState*) NULL;

    m_wndPrimary = m_wndSecondary = (wxWindow*) NULL;

    m_selected = (wxPGProperty*) NULL;

    m_propHover = (wxPGProperty*) NULL;

    m_processingEvent = 0;

    m_dragStatus = 0;

    m_mouseSide = 16;

    m_editorFocused = 0;

    m_coloursCustomized = 0;

    m_frozen = 0;

#if wxPG_DOUBLE_BUFFER
    m_doubleBuffer = (wxBitmap*) NULL;
#endif

#ifndef wxPG_ICON_WIDTH
	m_expandbmp = NULL;
	m_collbmp = NULL;
	m_iconwidth = 11;
	m_iconheight = 11;
#else
    m_iconwidth = wxPG_ICON_WIDTH;
#endif

    m_prevVY = -1;

    m_calcVisHeight = 0;

    m_gutterwidth = wxPG_GUTTER_MIN;
    m_subgroup_extramargin = 10;

    m_lineHeight = 0;

    m_width = m_height = m_fWidth = 0;

    m_bottomy = 0;

    //m_widDiffCarry = 0;

    m_splitterx = wxPG_DEFAULT_SPLITTERX;
    m_fSplitterX = (float) wxPG_DEFAULT_SPLITTERX;

#if !wxPG_HEAVY_GFX
    m_splitterpen.SetColour( *wxBLACK );
    m_splitterpen.SetWidth( 4 );

    m_splitterprevdrawnx = -1;
#endif

}

// -----------------------------------------------------------------------

//
// Initialize after parent etc. set
//
void wxPropertyGrid::Init2()
{

    wxASSERT ( !(m_iFlags & wxPG_FL_INITIALIZED ) );

    // Now create state, if one didn't exist already
    // (wxPropertyGridManager might have created it for us).
    if ( !m_pState )
    {
        m_pState = new wxPropertyGridState();
        m_pState->m_pPropGrid = this;
        m_iFlags |= wxPG_FL_CREATEDSTATE;
    }

    if ( !(m_windowStyle & wxPG_SPLITTER_AUTO_CENTER) )
        m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    if ( m_windowStyle & wxPG_HIDE_CATEGORIES )
    {
        m_pState->InitNonCatMode ();

        FROM_STATE(m_properties) = FROM_STATE(m_abcArray);
    }

    GetClientSize(&m_width,&m_height);

#if !wxPG_HEAVY_GFX
    m_splitterpen.SetColour( *wxBLACK );
    m_splitterpen.SetWidth( 4 );
#endif

#ifndef wxPG_ICON_WIDTH
    // create two bitmap nodes for drawing
	m_expandbmp = new wxBitmap(expand_xpm);
	m_collbmp = new wxBitmap(collapse_xpm);

	// calculate average font height for bitmap centering

	m_iconwidth = m_expandbmp->GetWidth();
	m_iconheight = m_expandbmp->GetHeight();
#endif

    m_curcursor = wxCURSOR_ARROW;
    m_cursor_sizewe = new wxCursor ( wxCURSOR_SIZEWE );

	// adjust bitmap icon y position so they are centered
    m_vspacing = wxPG_DEFAULT_VSPACING;

    if ( !m_font.Ok() )
    {
        wxFont useFont = wxScrolledWindow::GetFont();
        wxScrolledWindow::SetOwnFont ( useFont );
    }
    else
        // This should be otherwise called by SetOwnFont
	    CalculateFontAndBitmapStuff( wxPG_DEFAULT_VSPACING );

    // Add base brush item
    m_arrBgBrushes.Add((void*)new wxPGBrush());

    RegainColours();

    // This helps with flicker
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );

	// set virtual size to this window size
    wxSize wndsize = GetSize();
	SetVirtualSize(wndsize.GetWidth(), wndsize.GetWidth());

    m_timeCreated = ::wxGetLocalTimeMillis();

    m_iFlags |= wxPG_FL_INITIALIZED;

    // Need to call OnResize handler or size given in constructor/Create
    // will never work.
    wxSizeEvent sizeEvent(wndsize,0);
    OnResize(sizeEvent);

}

// -----------------------------------------------------------------------

wxPropertyGrid::~wxPropertyGrid()
{

    size_t i;

    // This should do prevent things from going too badly wrong
    m_iFlags &= ~(wxPG_FL_INITIALIZED);

    END_MOUSE_CAPTURE

#ifdef __WXDEBUG__
    if ( IsEditorsValueModified() )
        ::wxMessageBox(wxT("Most recent change in property editor was lost!!!\n\n(please report this, along with detailed circumstances to jmsalli79@hotmail.com)"),
                       wxT("wxPropertyGrid Debug Warning") );
#endif

#if wxPG_DOUBLE_BUFFER
    if ( m_doubleBuffer )
        delete m_doubleBuffer;
#endif

    m_selected = (wxPGProperty*) NULL;

    if ( m_iFlags & wxPG_FL_CREATEDSTATE )
        delete m_pState;

    delete m_cursor_sizewe;

#ifndef wxPG_ICON_WIDTH
	delete m_expandbmp;
	delete m_collbmp;
#endif

    // Delete cached brushes.
    for ( i=0; i<m_arrBgBrushes.GetCount(); i++ )
    {
        delete (wxPGBrush*)m_arrBgBrushes.Item(i);
    }

}

// -----------------------------------------------------------------------
// wxPropertyGrid overridden wxWindow methods
// -----------------------------------------------------------------------

void wxPropertyGrid::SetWindowStyleFlag( long style )
{
    long old_style = m_windowStyle;

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxASSERT ( m_pState );

        if ( !(style & wxPG_HIDE_CATEGORIES) && (old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Enable categories
            EnableCategories ( true );
        }
        else if ( (style & wxPG_HIDE_CATEGORIES) && !(old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Disable categories
            EnableCategories ( false );
        }
        if ( !(old_style & wxPG_AUTO_SORT) && (style & wxPG_AUTO_SORT) )
        {
            //
            // Autosort enabled
            //
            if ( !m_frozen )
                PrepareAfterItemsAdded();
            else
                FROM_STATE(m_itemsAdded) = 1;
        }
    #if wxPG_SUPPORT_TOOLTIPS
        if ( !(old_style & wxPG_TOOLTIPS) && (style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips enabled
            //
            /*wxToolTip* tooltip = new wxToolTip ( wxEmptyString );
            SetToolTip ( tooltip );
            tooltip->SetDelay ( wxPG_TOOLTIP_DELAY );*/
        }
        else if ( (old_style & wxPG_TOOLTIPS) && !(style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips disabled
            //
            wxScrolledWindow::SetToolTip ( (wxToolTip*) NULL );
        }
    #endif
    }

    wxScrolledWindow::SetWindowStyleFlag ( style );

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        if ( (old_style & wxPG_HIDE_MARGIN) != (style & wxPG_HIDE_MARGIN) )
        {
            CalculateFontAndBitmapStuff( m_vspacing );
            RedrawAllVisible();
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Freeze()
{
    m_frozen++;
    wxScrolledWindow::Freeze();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Thaw()
{
    m_frozen--;
    wxScrolledWindow::Thaw();
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    Refresh();
#endif
    // Force property re-selection
    if ( m_selected )
        DoSelectProperty(m_selected,wxPG_SEL_FORCE);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetExtraStyle( long exStyle )
{
    wxScrolledWindow::SetExtraStyle( exStyle );

    if ( exStyle & wxPG_EX_INIT_NOCAT )
        m_pState->InitNonCatMode ();

    if ( exStyle & wxPG_EX_HELP_AS_TOOLTIPS )
        m_windowStyle |= wxPG_TOOLTIPS;
}

// -----------------------------------------------------------------------

// returns the best acceptable minimal size
wxSize wxPropertyGrid::DoGetBestSize() const
{

    int hei = 15;
    if ( m_lineHeight > hei )
        hei = m_lineHeight;
    wxSize sz = wxSize( 60, hei+40 );

    //CacheBestSize(sz);

    return sz;
}

// -----------------------------------------------------------------------
// wxPropertyGrid Font and Colour Methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateFontAndBitmapStuff( int vspacing )
{
	int x = 0, y = 0;

    m_captionFont = GetFont();

	GetTextExtent(wxT("jG"), &x, &y, 0, 0, &m_captionFont);
    m_subgroup_extramargin = x + (x/2);
	m_fontHeight = y;

#if wxPG_USE_RENDERER_NATIVE
    m_iconwidth = wxPG_ICON_WIDTH;
#elif wxPG_ICON_WIDTH
    // scale icon
    m_iconwidth = (m_fontHeight * wxPG_ICON_WIDTH) / 13;
    if ( m_iconwidth < 5 ) m_iconwidth = 5;
    else if ( !(m_iconwidth & 0x01) ) m_iconwidth++; // must be odd

#endif

    m_gutterwidth = m_iconwidth / wxPG_GUTTER_DIV;
    if ( m_gutterwidth < wxPG_GUTTER_MIN )
        m_gutterwidth = wxPG_GUTTER_MIN;

    int vdiv = 6;
    if ( vspacing <= 1 ) vdiv = 12;
    else if ( vspacing >= 3 ) vdiv = 3;

    m_spacingy = m_fontHeight / vdiv;
    if ( m_spacingy < wxPG_YSPACING_MIN )
        m_spacingy = wxPG_YSPACING_MIN;

    m_marginwidth = 0;
    if ( !(m_windowStyle & wxPG_HIDE_MARGIN) )
        m_marginwidth = m_gutterwidth*2 + m_iconwidth;

    m_captionFont.SetWeight(wxBOLD);
	GetTextExtent(wxT("jG"), &x, &y, 0, 0, &m_captionFont);

    m_lineHeight = m_fontHeight+(2*m_spacingy)+1;

    // button spacing
    m_buttonSpacingY = (m_lineHeight - m_iconheight) / 2;
    if ( m_buttonSpacingY < 0 ) m_buttonSpacingY = 0;

    InvalidateBestSize();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnSysColourChanged( wxSysColourChangedEvent &WXUNUSED(event) )
{
    RegainColours();
    Refresh();
}

// -----------------------------------------------------------------------

static wxColour wxPGAdjustColour(const wxColour& src, int ra,
                                 int ga = 1000, int ba = 1000,
                                 bool forceDifferent = false)
{
    if ( ga >= 1000 )
        ga = ra;
    if ( ba >= 1000 )
        ba = ra;
#ifdef __WXDEBUG__
    // Recursion guard (allow 2 max)
    static int isinside = 0;
    isinside++;
    wxASSERT_MSG( isinside < 3,
        wxT("wxPGAdjustColour should not be recursively called more than once"));
#endif
    wxColour dst;

    int r = src.Red();
    int g = src.Green();
    int b = src.Blue();
    int r2 = r + ra;
    if ( r2>255 ) r2 = 255;
    else if ( r2<0) r2 = 0;
    int g2 = g + ga;
    if ( g2>255 ) g2 = 255;
    else if ( g2<0) g2 = 0;
    int b2 = b + ba;
    if ( b2>255 ) b2 = 255;
    else if ( b2<0) b2 = 0;

    // Make sure they are somewhat different
    if ( forceDifferent && (abs((r+g+b)-(r2+g2+b2)) < abs(ra/2)) )
        dst = wxPGAdjustColour(src,-(ra*2));
    else
        dst = wxColour(r2,g2,b2);

#ifdef __WXDEBUG__
    // Recursion guard (allow 2 max)
    isinside--;
#endif
    return dst;
}


static int wxPGGetColAvg( const wxColour& col )
{
    return (col.Red() + col.Green() + col.Blue()) / 3;
}


void wxPropertyGrid::RegainColours()
{
    //wxColour def_bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_SCROLLBAR );
    wxColour def_bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );

    if ( !(m_coloursCustomized & 0x02) )
    {
        wxColour col = wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE );

        // Make sure colour is dark enough
    #ifdef __WXGTK__
        int colDec = wxPGGetColAvg(col) - 230;
    #else
        int colDec = wxPGGetColAvg(col) - 200;
    #endif
        if ( colDec > 0 )
            m_colCapBack = wxPGAdjustColour(col,-colDec);
        else
            m_colCapBack = col;
    }

    if ( !(m_coloursCustomized & 0x01) )
        m_colMargin = m_colCapBack;

    if ( !(m_coloursCustomized & 0x04) )
    {
    #ifdef __WXGTK__
        int colDec = -90;
    #else
        int colDec = -72;
    #endif
        m_colCapFore = wxPGAdjustColour(m_colCapBack,colDec,5000,5000,true);
    }

    if ( !(m_coloursCustomized & 0x08) )
    {
        wxColour bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );
        m_colPropBack = bgcol;
        // Set the brush as well.
        ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(bgcol);
    }

    if ( !(m_coloursCustomized & 0x10) )
        m_colPropFore = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOWTEXT );

    if ( !(m_coloursCustomized & 0x20) )
        m_colSelBack = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHT );

    if ( !(m_coloursCustomized & 0x40) )
        m_colSelFore = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHTTEXT );

    if ( !(m_coloursCustomized & 0x80) )
        m_colLine = m_colCapBack;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ResetColours()
{
    m_coloursCustomized = 0;

    RegainColours();

    Refresh();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::SetFont ( const wxFont& font )
{

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::SetFont") );
#endif

    // Must disable active editor.
    if ( m_selected )
    {
        bool selRes = ClearSelection();
        wxASSERT_MSG( selRes,
                      wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    // TODO: Following code is disabled with wxMac because
    //   it is reported to fail. I (JMS) cannot debug it
    //   personally right now.
#if !defined(__WXMAC__)
    bool res = wxScrolledWindow::SetFont( font );
    if ( res )
    {

        CalculateFontAndBitmapStuff( m_vspacing );

        if ( m_pState )
        {
            // Recalculate caption text extents.
            // TODO: This should also be done to other pages of manager
            //   (so add wxPropertyGridManager::SetFont), but since font
            //   is usually set before categories are added, this is
            //   quite low priority.
            size_t i;

            for ( i=0;i<FROM_STATE(m_regularArray).GetCount();i++ )
            {
                wxPGProperty* p = FROM_STATE(m_regularArray).Item(i);

                if ( p->GetParentingType() > 0 )
                    ((wxPropertyCategoryClass*)p)->CalculateTextExtent(this,m_captionFont);
            }

            CalculateYs(NULL,-1);
        }

        Refresh();
    }

    return res;
#else
    // ** wxMAC Only **
    // TODO: Remove after SetFont crash fixed.
    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxLogDebug(wxT("WARNING: propgrid.cpp: wxPropertyGrid::SetFont has been disabled on wxMac since there has been crash reported in it. If you are willing to debug the cause, replace line '#if !defined(__WXMAC__)' with line '#if 1' in wxPropertyGrid::SetFont."));
    }
    return false;
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetLineColour ( const wxColour& col )
{
    m_colLine = col;
    m_coloursCustomized |= 0x80;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetMarginColour ( const wxColour& col )
{
    m_colMargin = col;
    m_coloursCustomized |= 0x01;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellBackgroundColour ( const wxColour& col )
{
    wxColour old_colpropback = m_colPropBack;

    m_colPropBack = col;
    m_coloursCustomized |= 0x08;

    // Set the brush as well.
    ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellTextColour ( const wxColour& col )
{
    m_colPropFore = col;
    m_coloursCustomized |= 0x10;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionBackground ( const wxColour& col )
{
    m_colSelBack = col;
    m_coloursCustomized |= 0x20;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionForeground ( const wxColour& col )
{
    m_colSelFore = col;
    m_coloursCustomized |= 0x40;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionBackgroundColour ( const wxColour& col )
{
    m_colCapBack = col;
    m_coloursCustomized |= 0x02;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionForegroundColour ( const wxColour& col )
{
    m_colCapFore = col;
    m_coloursCustomized |= 0x04;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetColourIndex ( wxPGProperty* p, int index )
{
    unsigned char ind = index;

    p->m_bgColIndex = ind;

    if ( p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
        size_t i;
        for ( i=0; i<pwc->GetCount(); i++ )
            SetColourIndex(pwc->Item(i),index);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyColour ( wxPGId id, const wxColour& colour )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    size_t i;
    int col_ind = -1;

    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    long colaslong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());

    // As it is most likely that the previous colour is used, start comparison
    // from the end.
    for ( i=(m_arrBgBrushes.GetCount()-1); i>0; i-- )
    {
        if ( ((wxPGBrush*)m_arrBgBrushes.Item(i))->GetColourAsLong() == colaslong )
        {
            col_ind = i;
            break;
        }
    }

    if ( col_ind < 0 )
    {
        col_ind = m_arrBgBrushes.GetCount();
        wxCHECK_RET ( col_ind < 256, wxT("wxPropertyGrid: Warning - Only 255 different property background colours allowed.") );
        m_arrBgBrushes.Add( (void*)new wxPGBrush(colour) );
    }

    // Set indexes
    SetColourIndex(p,col_ind);

    // If this was on a visible grid, then draw it.
    if ( p->GetParentState() == m_pState )
        DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

wxColour wxPropertyGrid::GetPropertyColour ( wxPGId id ) const
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    return ((wxPGBrush*)m_arrBgBrushes.Item(p->m_bgColIndex))->GetColour();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property adding and removal
// -----------------------------------------------------------------------

wxPGId wxPropertyGrid::Append( wxPGProperty* property )
{
    return FROM_STATE(Append(property));
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGrid::_Insert( wxPGProperty* priorthis, wxPGProperty* property )
{
    wxASSERT( priorthis );
    return FROM_STATE(DoInsert (priorthis->GetParent(), priorthis->GetArrIndex(), property ));
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::Delete( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);

    wxPropertyGridState* state = p->GetParentState();
    wxPropertyGrid* grid = state->GetGrid();

    if ( grid->GetState() == state )
    {
        bool selRes = grid->ClearSelection();
        wxASSERT_MSG( selRes,
                      wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    state->DoDelete ( p );

    if ( grid->GetState() == state && !grid->IsFrozen() )
    {
        // This should be enough to resolve even the worst
        // graphics glitch imagined.
        grid->Update();
        grid->Refresh();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::PrepareAfterItemsAdded()
{

    if ( !FROM_STATE(m_itemsAdded) ) return;

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("PrepareAfterItemsAdded( in thread 0x%lX )"),
        (unsigned long)wxThread::GetCurrentId());
#endif

    FROM_STATE(m_itemsAdded) = 0;

    if ( m_windowStyle & wxPG_AUTO_SORT )
    {
        Sort ();
    }
    else
    {
        if ( m_bottomy < 1 )
            CalculateYs( NULL, -1 );
        else
        {
            RecalculateVirtualSize();
            // Update visibles array (maybe not necessary here, but just in case)
            CalculateVisibles ( -1, true );
        }
    }

}

// -----------------------------------------------------------------------
// wxPropertyGrid property value setting and getting
// -----------------------------------------------------------------------

void wxPGGetFailed( const wxPGProperty* p, const wxChar* typestr )
{
    wxPGTypeOperationFailed(p,typestr,wxT("Get"));
}

// -----------------------------------------------------------------------

void wxPGTypeOperationFailed( const wxPGProperty* p, const wxChar* typestr,
    const wxChar* op )
{
    wxASSERT ( p != NULL );
    wxLogError( _("Type operation \"%s\" failed: Property labeled \"%s\" is of type \"%s\", NOT \"%s\"."),
        op,p->GetLabel().c_str(),p->GetValueType()->GetCustomTypeName(),typestr );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue( wxPGId id, const wxPGValueType* typeclass, wxPGVariant value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);
    if ( p && m_pState->SetPropertyValue(p,typeclass,value) )
        DrawItemAndChildren( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue( wxPGId id, const wxChar* typestring, wxPGVariant value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);
    if ( p && m_pState->SetPropertyValue(p,typestring,value) )
        DrawItemAndChildren( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValueString( wxPGId id, const wxString& value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxCHECK_RET(p,wxT("NULL property"));

    if ( m_pState->SetPropertyValueString(p,value) )
        DrawItemAndChildren( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValueWxObjectPtr( wxPGId id, wxObject* value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);
    if ( p && m_pState->SetPropertyValueWxObjectPtr(p,value) )
        DrawItemAndChildren ( p );
}

// -----------------------------------------------------------------------

#ifndef __WXPYTHON__
void wxPropertyGrid::SetPropertyValue( wxPGId id, wxVariant& value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);
    if ( p && m_pState->SetPropertyValue(p,value) )
        DrawItemAndChildren ( p );
}
#endif

// -----------------------------------------------------------------------

bool wxPropertyGrid::ClearPropertyValue ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);
    if ( p && m_pState->ClearPropertyValue(p) )
    {
        DrawItemAndChildren ( p );
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyUnspecified ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);

    if ( !p )
        return;

    m_pState->SetPropertyUnspecified(p);
    DrawItemAndChildren(p);

    wxPGPropertyWithChildren* parent = p->GetParent();
    while ( parent->GetParentingType() < 0 )
    {
        DrawItem(parent);
        parent = parent->GetParent();
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous GetPropertyXXX methods
// -----------------------------------------------------------------------

wxPropertyCategoryClass* wxPropertyGrid::_GetPropertyCategory ( wxPGProperty* p )
{
    wxPGProperty* parent = (wxPGPropertyWithChildren*)p;
    wxPGProperty* grandparent = (wxPGProperty*)parent->GetParent();
    do
    {
        parent = grandparent;
        grandparent = (wxPGProperty*)parent->GetParent();
        if ( parent->GetParentingType() > 0 && grandparent )
            return (wxPropertyCategoryClass*)parent;
    } while ( grandparent );
    return (wxPropertyCategoryClass*) NULL;
}

// -----------------------------------------------------------------------
// wxPropertyGrid property operations
// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableProperty( wxPGId id, bool enable )
{
    wxPGProperty* p = wxPGIdToPtr( id );
    if ( p )
    {
        if ( enable )
        {
            if ( !(p->m_flags & wxPG_PROP_DISABLED) )
                return false;

            // If active, Set active Editor.
            if ( p == m_selected )
                DoSelectProperty( p, wxPG_SEL_FORCE );
        }
        else
        {
            if ( p->m_flags & wxPG_PROP_DISABLED )
                return false;

            // If active, Disable as active Editor.
            if ( p == m_selected )
                DoSelectProperty( p, wxPG_SEL_FORCE );
        }

        m_pState->EnableProperty(p,enable);

        DrawItemAndChildren ( p );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::LimitPropertyEditing ( wxPGId id, bool limit )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    m_pState->LimitPropertyEditing(p,limit);
    if ( p == m_selected )
        DoSelectProperty( p, wxPG_SEL_FORCE );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::_SetPropertyLabel( wxPGProperty* p,
    const wxString& newproplabel )
{
    p->m_label = newproplabel;
    if ( m_windowStyle & wxPG_AUTO_SORT )
    {
        Sort(p->GetParent());
        Refresh();
    }
    else
        DrawItem ( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoSetPropertyName ( wxPGProperty* p,
    const wxString& newname )
{

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::SetPropertyName ( %s -> %s )"),
        p->GetName().c_str(), newname.c_str() );
#endif

    if ( p->GetName().Len() ) FROM_STATE(m_dictName).erase ( wxPGNameConv(p->GetName()) );
    if ( newname.Len() ) FROM_STATE(m_dictName)[newname] = (void*) p;

    p->DoSetName(newname);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnsureVisible ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( !p ) return false;

    Update();

    bool changed = false;

    // Is it inside collapsed section?
    if ( p->m_y < 0 )
    {
        // expand parents
        wxPGProperty* parent = p->GetParent();
        wxPGProperty* grandparent = parent->GetParent();

        if ( grandparent && grandparent != FROM_STATE(m_properties) )
            Expand ( grandparent );

        Expand ( parent );
        changed = true;
    }

    // Need to scroll?
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    if ( p->m_y < vy )
    {
        Scroll (vx, p->m_y/wxPG_PIXELS_PER_UNIT );
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = true;
    }
    else if ( (p->m_y+m_lineHeight) > (vy+m_height) )
    {
        Scroll (vx, (p->m_y-m_height+(m_lineHeight*2))/wxPG_PIXELS_PER_UNIT );
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = true;
    }

    if ( changed )
        DrawItems ( p, p );

    return changed;
}

// -----------------------------------------------------------------------
// wxPropertyGrid helper methods called by properties
// -----------------------------------------------------------------------

// Fixes position of wxTextCtrl-like control (wxSpinCtrl usually
// fits into that category as well).
void wxPropertyGrid::FixPosForTextCtrl( wxWindow* ctrl )
{
    // Center the control vertically
    wxRect finalPos = ctrl->GetRect();
    int y_adj = (m_lineHeight - finalPos.height)/2 + wxPG_TEXTCTRLYADJUST;

    // Prevent over-sized control
    int sz_dec = (y_adj + finalPos.height) - m_lineHeight;
    if ( sz_dec < 0 ) sz_dec = 0;

    finalPos.y += y_adj;
    finalPos.height -= (y_adj+sz_dec);

    // STUPID HACK: wxTextCtrl has different indentation with different
    //   fonts, so this is to solve most common case (ie. using MS Shell Dlg 2
    //   or Tahoma - which are usually the same).
#ifdef __WXMSW__
    wxString faceName = m_font.GetFaceName();
    int textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
    if ( (faceName == wxT("MS Shell Dlg 2") ||
          faceName == wxT("Tahoma")) &&
          m_font.GetWeight() != wxFONTWEIGHT_BOLD )
        textCtrlXAdjust = 0;
#else
    const int textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
#endif

    finalPos.x += textCtrlXAdjust;
    finalPos.width -= textCtrlXAdjust;

    ctrl->SetSize(finalPos);
}

// -----------------------------------------------------------------------

// Control font changer helper.
void wxPropertyGrid::SetCurControlBoldFont()
{
    wxASSERT( m_wndPrimary );

#ifdef __WXMSW__
    // For reason for this, see below for GenerateEditorTextCtrl
    const wxChar* className = m_wndPrimary->GetClassInfo()->GetClassName();
    if ( wxStrcmp(className,wxT("wxTextCtrl")) == 0 ||
         wxStrcmp(className,wxT("wxSpinCtrl")) == 0 )
    {
        wxString faceName = m_captionFont.GetFaceName();
        int textCtrlXAdjust = 0;

        if ( (faceName == wxT("MS Shell Dlg 2") ||
              faceName == wxT("Tahoma")) &&
              m_font.GetWeight() != wxFONTWEIGHT_BOLD )
        {
            textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
        }

        wxRect r = m_wndPrimary->GetRect();
        r.x += textCtrlXAdjust;
        r.width -= textCtrlXAdjust;
        m_ctrlXAdjust += textCtrlXAdjust;
        m_wndPrimary->SetSize(r);
    }
#endif

    m_wndPrimary->SetFont( m_captionFont );
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorTextCtrl( const wxPoint& pos,
                                                  const wxSize& sz,
                                                  const wxString& value,
                                                  wxWindow* secondary,
                                                  int extraStyle,
                                                  int maxLen )
{

    int tcFlags = wxTE_PROCESS_ENTER | extraStyle;

    wxPoint p(pos.x,pos.y);
    wxSize s(sz.x,sz.y);

    // Take button into acccount
    if ( secondary )
    {
        s.x -= (secondary->GetSize().x + wxPG_TEXTCTRL_AND_BUTTON_SPACING);
        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
    }

#if wxPG_NAT_TEXTCTRL_BORDER_ANY

    // Create clipper window
    wxPGClipperWindow* wnd = new wxPGClipperWindow();
#if defined(__WXMSW__)
    wnd->Hide();
#endif
    wnd->Create(this,wxPG_SUBID1,p,s);

    // This generates rect of the control inside the clipper window
    wnd->GetControlRect(wxPG_NAT_TEXTCTRL_BORDER_X,wxPG_NAT_TEXTCTRL_BORDER_Y,p,s);

    wxWindow* ctrlParent = wnd;

#else

    wxWindow* ctrlParent = this;
    tcFlags |= wxNO_BORDER;

#endif

    wxTextCtrl* tc = new wxTextCtrl();
#if defined(__WXMSW__) && !wxPG_NAT_TEXTCTRL_BORDER_ANY
    tc->Hide();
#endif
    tc->Create(ctrlParent,wxPG_SUBID1,value,p,/*wxDefaultSize*/s,tcFlags);

#if wxPG_NAT_TEXTCTRL_BORDER_ANY
    wxWindow* ed = wnd;
    wnd->SetControl(tc);
#else
    wxWindow* ed = tc;
#endif

    // Center the control vertically
    FixPosForTextCtrl(ed);

#ifdef __WXMSW__
    ed->Show();
    if ( secondary )
        secondary->Show();
#endif

    // Set maximum length
    if ( maxLen > 0 )
        tc->SetMaxLength( maxLen );

    return (wxWindow*) ed;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorButton( const wxPoint& pos, const wxSize& sz )
{

    wxSize s(sz.y-(wxPG_BUTTON_SIZEDEC*2)+(wxPG_NAT_BUTTON_BORDER_Y*2),
        sz.y-(wxPG_BUTTON_SIZEDEC*2)+(wxPG_NAT_BUTTON_BORDER_Y*2));
    wxPoint p(pos.x+sz.x-s.y,
        pos.y+wxPG_BUTTON_SIZEDEC-wxPG_NAT_BUTTON_BORDER_Y);

    wxButton* but = new wxButton();
#ifdef __WXMSW__
    but->Hide();
#endif
    but->Create(this,wxPG_SUBID2,wxT("..."),p,s,0);

    but->SetFont( m_captionFont );

    return but;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorTextCtrlAndButton( const wxPoint& pos,
    const wxSize& sz, wxWindow** psecondary, int limited_editing,
    wxPGProperty* property )
{
    wxButton* but = (wxButton*)GenerateEditorButton(pos,sz);
    *psecondary = (wxWindow*)but;

    if ( limited_editing )
    {
    #ifdef __WXMSW__
        // There is button Show in GenerateEditorTextCtrl as well
        but->Show();
    #endif
        return (wxWindow*) NULL;
    }

    wxString text;

    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        text = property->GetDisplayedString();

    return GenerateEditorTextCtrl(pos,sz,text,but,property->m_maxLen);
}

// -----------------------------------------------------------------------

wxPoint wxPropertyGrid::GetGoodEditorDialogPosition ( wxPGProperty* p,
    const wxSize& sz )
{
#if wxPG_SMALL_SCREEN
    // On small-screen devices, always show dialogs with default position and size.
    return wxDefaultPosition;
#else
    int x = m_splitterx;
    int y = p->m_y;

    wxASSERT ( y >= 0 );
    wxASSERT ( y < (int)m_bottomy );

    ImprovedClientToScreen ( &x, &y );

    int sw = wxSystemSettings::GetMetric ( ::wxSYS_SCREEN_X );
    int sh = wxSystemSettings::GetMetric ( ::wxSYS_SCREEN_Y );

    int new_x;
    int new_y;

    if ( x > (sw/2) )
        // left
        new_x = x + (m_width-m_splitterx) - sz.x;
    else
        // right
        new_x = x;

    if ( y > (sh/2) )
        // above
        new_y = y - sz.y;
    else
        // below
        new_y = y + m_lineHeight;

    return wxPoint(new_x,new_y);
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SLAlloc ( unsigned int itemcount, const wxChar** items )
{
    wxArrayString& sl = m_sl;
    unsigned int i;
    unsigned int sl_oldcount = sl.GetCount();
    if ( sl_oldcount > itemcount ) sl_oldcount = itemcount;
#if wxUSE_INTL
    if ( !wxPGGlobalVars->m_autoGetTranslation )
    {
#endif
        for ( i=0; i<sl_oldcount; i++ )
            sl.Item(i) = items[i];
        for ( i=sl_oldcount; i<itemcount; i++ )
            sl.Add ( items[i] );
#if wxUSE_INTL
    }
    else
    {
        for ( i=0; i<sl_oldcount; i++ )
            sl.Item(i) = ::wxGetTranslation ( items[i] );
        for ( i=sl_oldcount; i<itemcount; i++ )
            sl.Add ( ::wxGetTranslation ( items[i] ) );
    }
#endif
}

wxString& wxPropertyGrid::ExpandEscapeSequences( wxString& dst_str, wxString& src_str )
{
    if ( src_str.length() < 1 )
        return src_str;

    const wxChar* src = src_str.c_str();
    wxChar* dst;
    wxStringBuffer strbuf(dst_str,src_str.length());
    dst = strbuf;
    bool prev_is_slash = false;

    wxChar a = *src;
    while ( a )
    {
        src++;

        if ( a != wxT('\\') )
        {
            if ( !prev_is_slash )
            {
                *dst = a;
            }
            else
            {
                if ( a == wxT('n') )
                {
            #ifdef __WXMSW__
                    *dst = 13;
                    dst++;
                    *dst = 10;
            #else
                    *dst = 10;
            #endif
                }
                else if ( a == wxT('t') )
                    *dst = 9;
                else
                    *dst = a;
            }
            dst++;
            prev_is_slash = false;
        }
        else
        {
            if ( prev_is_slash )
            {
                *dst = wxT('\\');
                dst++;
                prev_is_slash = false;
            }
            else
            {
                prev_is_slash = true;
            }
        }

        a = *src;
    }
    *dst = 0;
    return dst_str;
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::CreateEscapeSequences ( wxString& dst_str, wxString& src_str )
{
    if ( src_str.length() < 1 )
        return src_str;

    const wxChar* src = src_str.c_str();
    wxChar* dst;
    wxStringBuffer strbuf(dst_str,src_str.length()*2);
    dst = strbuf;

    wxChar a = *src;
    while ( a )
    {
        src++;

        if ( a >= wxT(' ')
        #if !wxUSE_UNICODE
             || a < 0
        #endif
           )
        {
            // This surely is not something that requires an escape sequence.
            *dst = a;
            dst++;
        }
        else
        {
            *dst = wxT('\\');
            dst++;

            // This might need...
            if ( a == 13 && *src == 10 )
            {
                // DOS style line end.
                *dst = wxT('n');
                src++;
            }
            else if ( a == 10 )
                // UNIX style line end.
                *dst = wxT('n');
            else if ( a == 9 )
                // Tab.
                *dst = wxT('t');
            else
            {
                wxLogDebug(wxT("WARNING: Could not create escape sequence for character #%i"),(int)a);
                dst-=2; // Compensate.
            }
            dst++;
        }

        a = *src;
    }
    *dst = 0;
    return dst_str;

}

// -----------------------------------------------------------------------
// Item iteration macros
// -----------------------------------------------------------------------

#define II_INVALID_I    0x00FFFFFF

#define ITEM_ITERATION_VARIABLES \
    wxPGPropertyWithChildren* parent; \
    unsigned int i; \
    unsigned int i_max;

#define ITEM_ITERATION_DCAE_VARIABLES \
    wxPGPropertyWithChildren* parent; \
    unsigned int i; \
    unsigned int i_max;

#define ITEM_ITERATION_INIT_FROM_THE_TOP \
    parent = FROM_STATE(m_properties); \
    i = 0;

#define ITEM_ITERATION_INIT(startparent,startindex) \
    parent = startparent; \
    i = (unsigned int)startindex; \
    if ( parent == (wxPGPropertyWithChildren*) NULL ) \
    { \
        parent = FROM_STATE(m_properties); \
        i = 0; \
    }

#define ITEM_ITERATION_LOOP_BEGIN \
    unsigned char parent_expanded; \
    do \
    { \
        parent_expanded = (unsigned char)parent->m_expanded; \
        if ( parent->m_parent && !parent->m_parent->m_expanded ) \
            parent_expanded = 0; \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            int parenting = p->GetParentingType();

#define ITEM_ITERATION_LOOP_END \
            if ( parenting ) \
            { \
                i = 0; \
                parent = (wxPGPropertyWithChildren*)p; \
                if ( parent_expanded ) \
                    parent_expanded = (unsigned char)parent->m_expanded; \
                else \
                    parent_expanded = 0; \
                i_max = parent->GetCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != NULL );

// DCAE = Don't care about parent_expanded (this is the least space hungry method).
#define ITEM_ITERATION_DCAE_LOOP_BEGIN \
    do \
    { \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            int parenting = p->GetParentingType();

#define ITEM_ITERATION_DCAE_LOOP_END \
            if ( parenting ) \
            { \
                i = 0; \
                parent = (wxPGPropertyWithChildren*)p; \
                i_max = parent->GetCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != NULL );

// DCAE_ISP = Don't care about parent_expanded, Ignore sub-properties.
// Note that this treats fixed sub-properties same as sub-properties
// of wxParentProperty. Mode conversion requires this behaviour.
#define ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN \
    do \
    { \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            int parenting = p->GetParentingType();

#define ITEM_ITERATION_DCAE_ISP_LOOP_END \
            if ( parenting > 0 ) \
            { \
                i = 0; \
                parent = (wxPGPropertyWithChildren*)p; \
                i_max = parent->GetCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != (wxPGPropertyWithChildren*) NULL );


// VO = Visible only (including those outside the scrolled section).
#define ITEM_ITERATION_VO_LOOP_BEGIN \
    if ( (parent == FROM_STATE(m_properties) || parent->m_y >= 0) && parent->m_expanded ) \
    { \
    do \
    { \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            if ( p->m_y >= 0 ) \
            { \
                int parenting = p->GetParentingType();

#define ITEM_ITERATION_VO_LOOP_END \
                if ( parenting && ((wxPGPropertyWithChildren*)p)->m_expanded ) \
                { \
                    parent = (wxPGPropertyWithChildren*)p; \
                    i = 0; \
                    break; \
                } \
            } \
            i++; \
        } \
        if ( i >= i_max ) \
        { \
            i = parent->m_arrIndex + 1; \
            parent = parent->m_parent; \
        } \
    } \
    while ( parent != (wxPGPropertyWithChildren*) NULL ); \
    }

// -----------------------------------------------------------------------
// wxPropertyGrid visibility related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateYs ( wxPGPropertyWithChildren* startparent,
    int startindex )
{
    // Selection must be temporarily cleared during y-recalc
    wxPGProperty* prevSelected = m_selected;
    if ( prevSelected )
    {
        bool selRes = ClearSelection();
        wxASSERT_MSG( selRes,
                      wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    ITEM_ITERATION_VARIABLES

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("CalculateYs(startsfrom: %s[%i] ) "),
        startparent?startparent->m_label.c_str():wxT("NULL"),
            startindex);
#endif

    ITEM_ITERATION_INIT(startparent,startindex)

    wxASSERT ( !m_frozen );

    int cury = 0;
    int lh = m_lineHeight;

    if ( startparent != NULL )
        cury = parent->Item(i)->m_y;

    wxASSERT_MSG ( cury >= 0, wxT("CalculateYs first item was not visible!!!") );

    long hide_state = m_iFlags & wxPG_FL_HIDE_STATE;
    bool inside_hidden_part = false;
    //parent_expanded = (unsigned char)parent->m_expanded;
    wxPGPropertyWithChildren* nearest_expanded = (wxPGPropertyWithChildren*) NULL;

    // Find first visible and expanded parent.
    while ( !parent->IsExpanded() ||
            ( (parent->m_flags & wxPG_PROP_HIDEABLE) && hide_state )
          )
    {
        parent = parent->GetParent();
        i = 0;
    }

    wxASSERT ( parent );

    //parent = nearest_expanded;

    do
    {
        i_max = parent->GetCount();

        if ( !inside_hidden_part )
        {
            while ( i < i_max )
            {
                wxPGProperty* p = parent->Item(i);

                int parenting = p->GetParentingType();
                if ( !(p->m_flags & wxPG_PROP_HIDEABLE) || (!hide_state) )
                {
                    // item is visible (all parents are expanded, non-hideable or not in hide state)
                    p->m_y = (int)cury;
                    cury += lh;

                }
                else
                {
                    p->m_y = -1;
                }

                if ( parenting )
                {
                    wxPGPropertyWithChildren* p2 = (wxPGPropertyWithChildren*)p;

                    if ( !p2->m_expanded ||
                         ( (p2->m_flags & wxPG_PROP_HIDEABLE) && hide_state )
                       )
                    {
                       inside_hidden_part = true;
                       nearest_expanded = parent;
                    }

                    parent = p2;
                    i = 0;

                    break;
                }

                i++;
            }
        }
        else
        {
            while ( i < i_max )
            {
                wxPGProperty* p = parent->Item(i);
                int parenting = p->GetParentingType();
                p->m_y = -1;
                if ( parenting )
                {
                    parent = (wxPGPropertyWithChildren*)p;
                    i = 0;
                    break;
                }
                i++;
            }
        }
        if ( i >= i_max )
        {
            i = parent->m_arrIndex + 1;
            parent = parent->m_parent;
            if ( inside_hidden_part && parent == nearest_expanded )
            {
                inside_hidden_part = false;
            }
        }
    }
    while ( parent != (wxPGPropertyWithChildren*) NULL );

    m_bottomy = cury;

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("  \\-> m_bottomy = %i"),(int)m_bottomy);
#endif

    // Forces a new DoGetBestSize() call.
    wxScrolledWindow::InvalidateBestSize();

    // Visibles need to be recalculated *always* after y recalculation
    // (but make sure it stays here, above RecalculateVirtualSize).
    CalculateVisibles( -1, true );

    RecalculateVirtualSize();

    // Reselect
    if ( prevSelected )
        DoSelectProperty( prevSelected, wxPG_SEL_NONVISIBLE );

}

// -----------------------------------------------------------------------

// Call when scroll position changes. Do not pre-fill m_prevVY.
void wxPropertyGrid::CalculateVisibles ( int vy, bool full_recalc )
{
    //int prevVY = m_prevVY;

    if ( vy < 0 )
    {
        int vx;
        GetViewStart(&vx,&vy);
        vy *= wxPG_PIXELS_PER_UNIT;
        if ( full_recalc )
            m_prevVY = -1;
    }

    // Control not yet properly built.
    if ( vy >= (int)m_bottomy )
        return;

//    m_iFlags |= wxPG_FL_CHANGED;

    // Hide popup
    // FIXME: Delete after transient popup support added
    if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxPGOwnerDrawnComboBox)) )
    {
        ((wxPGOwnerDrawnComboBox*)m_wndPrimary)->HidePopup();
    }

    int vy2 = vy + m_height;

    if ( vy2 > (int)m_bottomy )
        vy2 = m_bottomy;

    unsigned int arr_index = 0;
    unsigned int vis_height = vy2-vy;
    unsigned int new_item_count = vis_height/m_lineHeight;
    if ( vis_height % m_lineHeight )
        new_item_count++;

    wxPGArrayProperty& arr = m_arrVisible;

    arr.SetCount ( new_item_count );

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::CalculateVisibles ( vy=%i, vy2=%i, m_height=%i, newitemcount=%i, lineheight=%i )"),
        (int)vy, (int)vy2, (int)m_height, (int)new_item_count, (int)m_lineHeight );
#endif

    //wxASSERT ( vy != m_prevVY );
    wxASSERT ( vy >= 0 );

    if ( !new_item_count )
    {
        arr.Empty();
        return;
    }

    ITEM_ITERATION_VARIABLES

    wxPGProperty* base = NULL;

    // Will simpler operation be enough?
    if ( m_prevVY >= 0 )
    {
        if ( m_calcVisHeight == m_height )
        {
            if ( m_iFlags & wxPG_FL_SCROLLED )
            {
                int diff = vy - m_prevVY;
                if ( diff == m_lineHeight )
                {
                    // Scrolled one down
                    base = DoGetItemAtY_Full( vy2 - 1 );
                    wxASSERT ( base );
                    arr_index = new_item_count - 1;
                    for ( i=0; i<arr_index; i++ )
                        arr.Item(i) = arr.Item(i+1);
                    arr.Item(arr_index) = base;
                    base = (wxPGProperty*) NULL;
                }
                else if ( diff == -m_lineHeight )
                {
                    // Scrolled one up
                    base = DoGetItemAtY_Full( vy );
                    wxASSERT ( base );
                    vy2 = vy + m_lineHeight; // update visibility
                    for ( i=(new_item_count-1); i>arr_index; i-- )
                        arr.Item(i) = arr.Item(i-1);
                    arr.Item(arr_index) = base;
                    base = (wxPGProperty*) NULL;
                }
                else
                    base = DoGetItemAtY_Full( vy );
            }
            else
                base = DoGetItemAtY_Full( vy );
        }
        else
        if ( m_prevVY == vy && !(m_iFlags & wxPG_FL_SCROLLED) )
        {
            if ( m_height > m_calcVisHeight )
            {
            // Increased height - add missing items
                arr_index = (m_calcVisHeight-1)/m_lineHeight;
                if ( arr_index >= new_item_count )
                {
                    // Now, were probably below last item here
                    //if ( (vy+m_calcVisHeight) >= (int)m_bottomy )
                        base = NULL;
                    /*else
                        arr_index = arr.GetCount()-1;*/
                }
                else
                {
                    base = (wxPGProperty*) arr.Item( arr_index );
                }
            }
            else
            {
            // Decreased height - do nothing
                //base = NULL;
            }
        }
        else
            base = DoGetItemAtY_Full( vy );
    }
    else
    {
        base = DoGetItemAtY_Full( vy );
    }

    if ( base )
    {

        ITEM_ITERATION_INIT(base->m_parent,base->m_arrIndex)

    #if __INTENSE_DEBUGGING__
        wxLogDebug ( wxT("  Starting at index %i"), (int)arr_index );
    #endif

        ITEM_ITERATION_VO_LOOP_BEGIN

            //wxASSERT ( p->m_y >= 0 );

            // update visibility limit reached?
            if ( p->m_y >= vy2 ) { parent = NULL; break; }

        #ifdef __WXDEBUG__
            if ( arr_index >= arr.GetCount() )
            {
                wxLogDebug (wxT("  wxPropertyGrid::CalculateVisibles Loop overflow (index=%i,vy+vis_height=%i,p->m_y=%i)"),
                    (int)arr_index,(int)(vy+vis_height),(int)p->m_y);
            }
        #endif

            arr.Item(arr_index) = (void*)p;
            arr_index++;

        ITEM_ITERATION_VO_LOOP_END
    }

    // Adjust controls
    /*if ( m_selected )
    {
        int adjust = prevVY - vy;
        if ( adjust )
        {
            wxPoint cp(0,adjust);

            if ( m_wndPrimary )
                m_wndPrimary->Move ( m_wndPrimary->GetPosition() + cp );

            if ( m_wndSecondary )
                m_wndSecondary->Move ( m_wndSecondary->GetPosition() + cp );
        }
    }*/

    m_iFlags &= ~(wxPG_FL_SCROLLED);

    m_prevVY = vy;

    m_calcVisHeight = m_height;

}

// -----------------------------------------------------------------------

// This version uses the visible item cache.
wxPGProperty* wxPropertyGrid::DoGetItemAtY ( int y )
{

    //wxASSERT ( m_prevVY >= 0 );

    // Outside(check 1)?
    if ( y >= (int)m_bottomy || y < 0 )
    {
        /*
    #if __PAINT_DEBUGGING__
        wxLogDebug (wxT("WARNING: DoGetItemAtY(a): y = %i"),y);
    #endif
        */
        return (wxPGProperty*) NULL;
    }

    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    // Need to recalculate visibility cache
    // Note: need to check for y < m_prevVY is a hack.
    if ( m_prevVY != vy ||y < m_prevVY ) //m_iFlags & wxPG_FL_SCROLLED ||
        CalculateVisibles( vy, true );

    // Outside(check 2)?
    if ( y >= (vy+m_height) )
    {
        /*
    #if __PAINT_DEBUGGING__
        wxLogDebug (wxT("WARNING: DoGetItemAtY(b): y = %i"),y);
    #endif
        */
        return NULL;
    }

    unsigned int index = (unsigned int)((y - vy) / m_lineHeight);

#ifdef __WXDEBUG__
    // Consistency checks
    if ( !m_arrVisible.GetCount() )
        return NULL;
    if ( index >= m_arrVisible.GetCount() )
    {
        wxLogDebug (wxT("  index = %i"),(int)index);
        wxLogDebug (wxT("  (height/lineheight+1) = %i"),(int)((m_height/m_lineHeight)+1));
        wxLogDebug (wxT("  m_arrVisible.GetCount() = %i"),(int)m_arrVisible.GetCount());

        wxCHECK_MSG( index < m_arrVisible.GetCount(), (wxPGProperty*) NULL,
            wxT("Not enough entries in m_arrVisible (y was < m_bottomy).") );
    }
#endif

    if ( index >= m_arrVisible.GetCount() )
    {
        index = m_arrVisible.GetCount()-1;
    }

    return (wxPGProperty*)m_arrVisible.Item(index);
}

// -----------------------------------------------------------------------
// wxPropertyGrid graphics related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{

    wxPG_PAINT_DC_INIT()

    // Don't paint after destruction has begun
    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

#if __PAINT_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::OnPaint()") );
#endif

    // Find out where the window is scrolled to
    int vx,vy;                     // Top left corner of client
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();

    r.y += vy;

    // Repaint this rectangle
    //if ( r.height > 0 )
        DrawItems ( dc, r.y, r.y + r.height,
    #if wxPG_ALLOW_CLIPPING
            NULL //&r
    #else
            NULL
    #endif
        );

    // We assume that the size set when grid is shown
    // is what is desired.
    m_iFlags |= wxPG_FL_GOOD_SIZE_SET;

}

// -----------------------------------------------------------------------

//
// This is the one called by OnPaint event handler and others.
// topy and bottomy are already unscrolled
// Clears any area in coordinates that doesn't have items.
//
void wxPropertyGrid::DrawItems ( wxDC& dc, unsigned int topy, unsigned int bottomy,
    const wxRect* clip_rect )
{

    if ( m_frozen || m_height < 1 || bottomy < topy ) return;

#if __PAINT_DEBUGGING__
    wxLogDebug (wxT("wxPropertyGrid::DrawItems ( %i -> %i, clip_rect = 0x%X )"),topy,bottomy,
        (unsigned int)clip_rect);
#endif

    // items added check
    if ( FROM_STATE(m_itemsAdded) ) PrepareAfterItemsAdded();

    unsigned int vx, vy;                     // Top left corner of client
    GetViewStart((int*)&vx,(int*)&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    unsigned int client_bottom = (unsigned int)m_height + vy;

    // Clip topy and bottomy
    if ( bottomy > client_bottom )
        bottomy = client_bottom;
    if ( topy < vy )
        topy = vy;

#if __PAINT_DEBUGGING__
    wxLogDebug (wxT("  \\--> ( final area %i -> %i )"),topy,bottomy);
#endif

    //
    // Determine first and last item to draw
    // (don't draw if already over the top)
    //

    if ( topy < client_bottom && topy < m_bottomy && FROM_STATE(m_properties)->GetCount() > 0 )
    {

        wxPGProperty* first_item = DoGetItemAtY(topy);

        if ( first_item == (wxPGProperty*) NULL )
        {
    #ifdef __WXDEBUG__
            wxString msg;
            msg.Printf(wxT("WARNING: wxPropertyGrid::DrawItems(): first_item == NULL!"));
            wxMessageBox(msg);
            wxLogDebug(msg);
            wxLogDebug(wxT("  More info: y: %i -> %i   visible_window: %i -> %i"),
                (int)topy,(int)bottomy,(int)vy,(int)client_bottom);
            // This is here for debugging purposes.
            DoGetItemAtY(topy);
    #endif
            return;
        }

        wxPGProperty* last_item = (wxPGProperty*) NULL;

        // last_item may be NULL on call to DoDrawItems
        // in this case last_item will truly become the last item

        if ( bottomy > topy && bottomy < m_bottomy )
        {
            last_item = DoGetItemAtY(bottomy-1);
        #if __PAINT_DEBUGGING__
            wxLogDebug ( wxT("  \\--> WARNING: last_item acquisition failed (should not)!"));
        #endif
        }

        DoDrawItems ( dc, first_item, last_item, clip_rect );
    }

    // Clear area beyond m_bottomy?
    if ( bottomy > m_bottomy )
    {
        wxColour& bgc = wxPG_SLACK_BACKROUND;
        //wxColour& bgc = wxColour(255,0,255);
        dc.SetPen ( wxPen(bgc) );
        dc.SetBrush ( wxBrush(bgc) );
        unsigned int clear_top = m_bottomy;
        if ( topy > clear_top ) clear_top = topy;
        dc.DrawRectangle ( 0, clear_top, m_width, m_height-(clear_top-vy) );
    }
}

// -----------------------------------------------------------------------

#define ITEM_ITERATION_UVC_VARIABLES \
    unsigned int ind; \
    wxPGProperty* p;

// UVC = Use Visibility Cache
// VISTART = index to first item from visibility cache to use.
// BOTTOMY = Logical y coordinate of last item to draw.
#define ITEM_ITERATION_UVC_LOOP_BEGIN(VISTART,BOTTOMY) \
    ind = VISTART; \
    do \
    { \
        p = (wxPGProperty*)m_arrVisible.Item(ind); \
        ind++; \
        int parenting = p->GetParentingType();

#define ITEM_ITERATION_UVC_LOOP_END(BOTTOMY) \
    } while ( p->m_y < BOTTOMY ); \

//
// Uses three pass approach, so it is optimized for drawing
// multiple items at once.
//
// IMPORTANT NOTES:
// - Clipping rectangle must be of physical coordinates.
//
//
void wxPropertyGrid::DoDrawItems (
#if wxPG_DOUBLE_BUFFER
        wxDC& dc_main,
#else
        wxDC& dc,
#endif
        wxPGProperty* first_item, wxPGProperty* last_item,
        const wxRect* clip_rect )
{

    if ( m_frozen || m_height < 1 )
        return;

    wxASSERT ( !FROM_STATE(m_itemsAdded) );
    wxASSERT ( first_item != NULL );
    wxASSERT ( FROM_STATE(m_properties->GetCount()) );

    int lh = m_lineHeight;

    // Make sure visibility cache is up-to-date
    int vy;
    int vx;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    if ( vy != m_prevVY )
        CalculateVisibles(vy,true);

    if ( vy != m_prevVY )
        return;

    // Determine last item, if not given (but requires clip_rect).
    if ( last_item == NULL )
    {
        if ( clip_rect != NULL )
        {

            unsigned int bottomy = /*vy + */clip_rect->y + clip_rect->height;

            if ( bottomy <= (unsigned int)first_item->m_y )
                last_item = first_item;
        }

        if ( last_item == NULL )
        {
            last_item = DoGetItemAtY(vy+m_height-1);
            if ( last_item == NULL )
                last_item = GetLastItem(true);
        }
    }

    int first_item_top_y = first_item->m_y;
    int last_item_bottom_y = last_item->m_y+lh-1;
    //unsigned int start_index = first_item->GetIndexInParent();

    int y_rel_mod = 0;

    // Entire range outside scrolled, visible area?
    if ( first_item_top_y >= (vy+m_height) || last_item_bottom_y <= vy )
        return;

    wxASSERT ( first_item_top_y < last_item_bottom_y );

#if wxPG_DOUBLE_BUFFER
    if ( !m_doubleBuffer )
        return;

    // Must fit to double-buffer
#ifdef __WXDEBUG__
    if ( (last_item_bottom_y - first_item_top_y) > m_doubleBuffer->GetHeight() )
    {
        wxString msg;
        msg.Printf ( wxT("wxPropertyGrid: DOUBLE-BUFFER TOO SMALL ( drawn %i vs db height %i vs client_height %i)!"),
            (int)(last_item_bottom_y - first_item_top_y),
            (int)(m_doubleBuffer->GetHeight()),
            (int)m_height );
        wxLogError ( msg );
        wxLogDebug ( msg );
    }
#endif

    wxMemoryDC dc;
    dc.SelectObject ( *m_doubleBuffer );

    const wxRect* blit_clip_rect = clip_rect;
    int render_height = last_item->m_y - first_item_top_y + m_lineHeight;

    //if ( m_iFlags & wxPG_FL_CHANGED ||
    //     !(m_iFlags & wxPG_FL_HANDLING_PAINT_EVENT) )
    //{
#endif

#if __PAINT_DEBUGGING__
    wxLogDebug(wxT("  -> DoDrawItems ( \"%s\" -> \"%s\", height=%i (ch=%i), clip_rect = 0x%lX )"),
        first_item->GetLabel().c_str(),
        last_item->GetLabel().c_str(),
        (int)(last_item_bottom_y - first_item_top_y),
        (int)m_height,
        (unsigned long)clip_rect );
#endif

    wxPGPaintData paintdata;
    wxRect r;

    ITEM_ITERATION_UVC_VARIABLES

    // Get first and last indexes to visibility cache
    unsigned int vi_start = (first_item_top_y - vy) / lh;
    int vi_end_y = last_item->m_y;

    if ( vi_start >= m_arrVisible.GetCount() )
    {
        wxLogDebug (wxT("WARNING: vi_start >= m_arrVisible.GetCount() ( %i >= %i )"),
            (int)vi_start, (int)m_arrVisible.GetCount() );
        return;
    }

#ifdef __WXDEBUG__
    unsigned int vi_end = (last_item->m_y - vy) / lh;
    if ( vi_end >= m_arrVisible.GetCount() )
    {
        wxLogDebug (wxT("WARNING: vi_end >= m_arrVisible.GetCount() ( %i >= %i )"),
            (int)vi_end, (int)m_arrVisible.GetCount() );
        return;
    }
#endif

    int x = m_marginwidth;
    int y;

    long window_style = m_windowStyle;

    //
    // With wxPG_DOUBLE_BUFFER, do double buffering
    // - buffer's y = 0, so align cliprect and coordinates to that
    //
#if wxPG_DOUBLE_BUFFER

    wxRect cr2;

    //y_rel_mod = first_item_top_y;
    y_rel_mod = vy;

    //
    // clip_rect conversion
    if ( clip_rect )
    {
        cr2 = *clip_rect;
        cr2.y -= y_rel_mod;
        clip_rect = &cr2;
    }
    //int render_height = last_item->m_y - first_item_top_y + m_lineHeight;
    //last_item_bottom_y -= first_item_top_y;
    //first_item_top_y = 0;
    first_item_top_y -= vy;
    last_item_bottom_y -= vy;

#endif

    wxFont& normalfont = m_font;

    bool reallyFocused = (m_iFlags & wxPG_FL_FOCUSED) ? true : false;

    //
    // Prepare some pens and brushes that are often changed to.
    //

    wxBrush marginBrush(m_colMargin);
    wxPen marginPen(m_colMargin);
    wxBrush capbgbrush(m_colCapBack,wxSOLID);
    wxPen linepen(m_colLine,1,wxSOLID);

    // pen that has same colour as text
    wxPen outlinepen(m_colPropFore,1,wxSOLID);

    if ( clip_rect )
        dc.SetClippingRegion( *clip_rect );

    //
    // Clear margin with background colour
    //
    dc.SetBrush( marginBrush );
    if ( !(window_style & wxPG_HIDE_MARGIN) )
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(-1,first_item_top_y-1,m_marginwidth+2,last_item_bottom_y-first_item_top_y+3);
    }

    /*
    // This colorizer helps to debug painting.
    bool small_draw = false;
    if ( render_height < (m_height-(lh*3)) )
    {
        if ( first_item == last_item )
        {
            bgbrush = wxBrush(wxColour(255,128,128));
            linepen = wxPen(wxColour(128,0,255));
            //boxbrush = wxBrush(wxColour(192,192,192));
        }
        else
        {
            bgbrush = wxBrush(wxColour(128,255,128));
            linepen = wxPen(wxColour(0,0,255));
            //boxbrush = wxBrush(wxColour(230,230,230));
        }
        small_draw = true;
    }
    */

    //dc.SetPen ( *wxTRANSPARENT_PEN );
    //dc.SetFont(normalfont);

    wxPGProperty* selected = m_selected;

/*#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    bool selected_painted = false;
#endif*/

    // NOTE: Clipping and pen/brush switching are main reasons for multi-pass approach.

    //
    // zero'th pass: Wireframes.
    // (this could be embedded in another loop)

    dc.SetBrush( marginBrush );

    unsigned long cur_first_ind = vi_start;
    unsigned long next_cur_first_ind = 0xFFFFFFFF;
    wxPGPropertyWithChildren* cur_category = (wxPGPropertyWithChildren*) NULL;
    int vcache_last_item_y = vy + m_height;
    if ( vcache_last_item_y > (int)m_bottomy ) vcache_last_item_y = m_bottomy;
    vcache_last_item_y -= lh;

    ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

        wxPGPropertyWithChildren* parent = p->GetParent();

        // Does this wireframe end?
        // Conditions:
        // A) This is last item within its parent, and parent is category.
        // B) Next is category.
        // C) This is collapsed category.
        // D) This is the last item drawn.

        if ( p->m_y >= vi_end_y )
        {
            // This is the last item.
            //wxLogDebug(wxT("--> last item"));
            next_cur_first_ind = ind;
        }
        else if ( p->m_arrIndex >= (parent->GetCount()-1) && parent->GetParentingType() > 0 &&
             ( parenting <= 0 /*|| ((wxPGPropertyWithChildren*)p)->GetCount() < 1*/ )
            )
        {
            // This is last item within its parent, and parent is category, but this isn't
            // an non-empty category.
            //wxLogDebug(wxT("--> category ends"));
            cur_category = (wxPropertyCategoryClass*)parent;
            next_cur_first_ind = ind;
        }
        else if ( ((wxPGProperty*)m_arrVisible.Item(ind))->GetParentingType() > 0 )
        {
            // Next item is a category.
            //wxLogDebug(wxT("--> next item is  category"));
            next_cur_first_ind = ind;
        }
        else if ( parenting > 0 &&
                  (!((wxPGPropertyWithChildren*)p)->IsExpanded() ||
                   !((wxPGPropertyWithChildren*)p)->GetCount()) )
        {
            // This is collapsed category.
            //wxLogDebug(wxT("--> collapsed category"));
            cur_category = (wxPropertyCategoryClass*)p;
            next_cur_first_ind = ind;
        }

        // When new category begins or old ends, draw wireframe for items in-between
        if ( next_cur_first_ind < 0xFFFFFF )
        {

            wxPGProperty* cur_first = (wxPGProperty*)m_arrVisible.Item(cur_first_ind);
            wxPGPropertyWithChildren* cur_last_item = (wxPGPropertyWithChildren*)p;

            if ( !cur_category )
            {
                if ( cur_first->GetParentingType() > 0 )
                {
                    cur_category = (wxPropertyCategoryClass*)cur_first;
                }
                else if ( !(m_windowStyle & wxPG_HIDE_CATEGORIES) )
                {
                    cur_category = _GetPropertyCategory(cur_first);
                    /*if ( !cur_category )
                        cur_category = (wxPropertyCategoryClass*)FROM_STATE(m_properties);*/
                }
            }

            int draw_top = cur_first->m_y - y_rel_mod;
            int draw_bottom = cur_last_item->m_y + lh - y_rel_mod;
            int frame_top = draw_top;
            int frame_bottom = draw_bottom;
            int margin_top = draw_top;
            int margin_bottom = draw_bottom;

            int ly = frame_top + lh - 1;

            if ( cur_first->GetParentingType() > 0 )
            {
                wxPropertyCategoryClass* pc = ((wxPropertyCategoryClass*)cur_first);
                frame_top += lh;
                if ( !pc->IsExpanded() )
                {
                    // Category collapsed.
                    frame_top = frame_bottom + 1;
                }
            }

            int grey_x = x;
            if ( cur_category /*!(window_style & wxPG_HIDE_CATEGORIES)*/ )
                grey_x += ((unsigned int)((cur_category->GetDepth()-1)*m_subgroup_extramargin));

            //wxLogDebug ( wxT("wireframe: %s -> %s (grey_x:%i)"), cur_first->GetLabel().c_str(),
            //    cur_last_item->GetLabel().c_str(),((int)grey_x-x));

            dc.SetPen ( *wxTRANSPARENT_PEN );

            // Clear extra margin area.
            dc.DrawRectangle ( x-1, margin_top, grey_x - x + 1, margin_bottom-margin_top );

            dc.SetPen ( linepen );

            if ( frame_bottom > frame_top )
            {

                //if ( cat_top < first_item_top_y )
                //  cat_top = first_item_top_y;


                // Margin Edge
                dc.DrawLine ( grey_x, frame_top, grey_x, frame_bottom );

                // Splitter
                dc.DrawLine ( m_splitterx, frame_top, m_splitterx, frame_bottom );

                // Horizontal Lines
                while ( ly < (frame_bottom-1) )
                {
                    dc.DrawLine ( grey_x, ly, m_width, ly );
                    ly += lh;
                }

            }

            int use_depth = grey_x; // Default is to simply tidy up this wireframe.

            // Properly draw top line of next wireframe, if adjacent.

            // Get next item.
            wxPGProperty* next_item;
            //if ( ind < m_arrVisible.GetCount() )
            if ( cur_last_item->m_y < vcache_last_item_y )
            {
                next_item = (wxPGProperty*)m_arrVisible.Item(ind);
            }
            else
            {
                // Was not in visibility cache, so use clumsier method.
                next_item = GetNeighbourItem(cur_last_item,true,1);
                if (!next_item)
                    next_item = cur_last_item; // This will serve our purpose.
            }
            //wxLogDebug(wxT("next_item: %s"),next_item->GetLabel().c_str());

            // Just take the depth and is-it-category out of it.
            int next_parenting = next_item->GetParentingType();
            int last_parenting = cur_last_item->GetParentingType();

            // A) If both are categories, draw line with background colour.
            // B) If only next is category, use its category's depth.
            // C) If only last is category, use grey_x as depth.
            // D) If neither is a category, use smaller.
            if ( next_parenting > 0 )
            {
                // Next is category.

                if ( last_parenting > 0 )
                {
                    // Last is a category too - draw complete line with background colour.
                    dc.SetPen ( marginPen );
                    use_depth = x;
                }
            }
            else
            {
                // Next is not a category.
                wxPropertyCategoryClass* next_cat = _GetPropertyCategory(next_item);
                int depth_next = x;
                if ( next_cat && /*cur_category*/ !(window_style & wxPG_HIDE_CATEGORIES) )
                {
                    //wxLogDebug(wxT("next_item_cat: %s"),next_cat->GetLabel().c_str());
                    depth_next += ((unsigned int)((next_cat->GetDepth()-1)*m_subgroup_extramargin));
                }

                if ( last_parenting <= 0 )
                {
                    // Last is not a category - use lesser depth
                    if ( depth_next < grey_x )
                        use_depth = depth_next;
                    //wxLogDebug(wxT("- neither is category"));
                }
                else
                {
                    // Last is a category
                    use_depth = depth_next;
                    //wxLogDebug(wxT("last only is category"));
                }
            }

            //wxLogDebug(wxT("last_line_use_depth: %i"),(int)use_depth);
            dc.DrawLine( use_depth, ly, m_width, ly );

            cur_first_ind = next_cur_first_ind;
            next_cur_first_ind = 0xFFFFFFFF;
            //cur_first = (wxPGPropertyWithChildren*)p;
            cur_category = (wxPGPropertyWithChildren*) NULL;
        }

        //cur_last_item = p;

    ITEM_ITERATION_UVC_LOOP_END(vi_end_y)

    //
    // First pass: Category background and text, Images, Label+value background.
    //

    //wxLogDebug (wxT("  \\--> first pass..."));
    //ITEM_ITERATION_INIT(first_item->m_parent,start_index)
    y = first_item_top_y;

    dc.SetTextForeground( m_colCapFore );
    dc.SetFont( m_captionFont );
    dc.SetPen ( *wxTRANSPARENT_PEN );

    ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

        y += m_spacingy;

        int text_x = x + ((unsigned int)((p->GetDepth()-1)*m_subgroup_extramargin));

        if ( parenting > 0 )
        {

            dc.SetBrush ( capbgbrush ); // Category label background colour.

            // Category - draw background, text and possibly selection rectangle.
            wxPropertyCategoryClass* pc = (wxPropertyCategoryClass*)p;

            // Note how next separator line is overdrawn if next item is category .
            int use_lh = lh;
            if ( ind < (m_arrVisible.GetCount()) &&
                 ( ((wxPGProperty*)m_arrVisible[ind])->GetParentingType() <= 0 ) )
                use_lh -= 1;

            //dc.DrawRectangle ( x, y-m_spacingy, m_width-x, use_lh );
            dc.DrawRectangle ( text_x, y-m_spacingy, m_width-text_x, use_lh );
            dc.DrawText( pc->GetLabel(), text_x+wxPG_XBEFORETEXT, y );

            // active caption gets nice dotted rectangle
            if ( p == selected )
            {
                wxRect focusRect(text_x+wxPG_XBEFORETEXT-wxPG_CAPRECTXMARGIN,
                                 y-wxPG_CAPRECTYMARGIN,
                                 pc->GetTextExtent()+(wxPG_CAPRECTXMARGIN*2),
                                 m_fontHeight+(wxPG_CAPRECTYMARGIN*2));
                wxPGDrawFocusRect(dc,focusRect);

                dc.SetPen ( *wxTRANSPARENT_PEN );
            }

        }
        else
        {

            // Basic background colour.
            dc.SetBrush ( *(wxPGBrush*)m_arrBgBrushes[p->m_bgColIndex] );

            //wxLogDebug(wxT("%s: %i"),p->m_label.c_str(),(int)p->m_depthBgCol);

            int grey_depth = 0;
            if ( !(window_style & wxPG_HIDE_CATEGORIES) )
                grey_depth = (((int)p->m_depthBgCol)-1) * m_subgroup_extramargin;

            // In two parts to retain splitter

            if ( p == m_selected )
            {
            // Selected get different label background.
                if ( reallyFocused )
                    dc.SetBrush ( m_colSelBack );
                else
                    dc.SetBrush ( m_colLine );

                dc.DrawRectangle ( x+grey_depth+1, y-m_spacingy, m_splitterx-grey_depth-x-1, lh-1 );

            }
            else
            {
                dc.DrawRectangle ( x+grey_depth+1, y-m_spacingy, m_splitterx-grey_depth-x-1, lh-1 );
            }

            dc.DrawRectangle ( m_splitterx+1, y-m_spacingy, m_width-m_splitterx, lh-1 );

        }

        y += m_fontHeight+m_spacingy+1;

    ITEM_ITERATION_UVC_LOOP_END(vi_end_y)

    dc.SetTextForeground( m_colPropFore );
    dc.SetFont( normalfont );

    /*
    // Expander button preparation
#if !wxPG_USE_RENDERER_NATIVE && wxPG_ICON_WIDTH
    // TODO: Move these to to class.
    wxPen whiteExpanderLine(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    wxPen blackExpanderLine(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    wxBrush whiteExpanderFace(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    //wxBrush greyExpanderFace(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    wxBrush greyExpanderFace(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
#endif
    */

    //
    // Second pass: Expander Buttons, Labels.
    //
    // Second pass happens entirely on the left side, so sometimes
    // we can just skip it.
    if ( clip_rect == NULL || clip_rect->x < m_splitterx )
    {

        //wxLogDebug (wxT("  \\--> second pass..."));

        y = first_item_top_y;

        r = wxRect(0,y,m_splitterx,last_item_bottom_y);
        dc.SetClippingRegion ( r );

        dc.SetFont(normalfont);

        ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

            //
            // Expand/collapse button image.
            if ( parenting != 0 &&
                 !(window_style & wxPG_HIDE_MARGIN) &&
                 ((wxPGPropertyWithChildren*)p)->GetChildCount() )
            {
                wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

            #ifdef wxPG_ICON_WIDTH
                int image_x = m_gutterwidth + ((unsigned int)((p->m_depth-1)*m_subgroup_extramargin));
            #endif

                y += m_buttonSpacingY;

            #if (wxPG_USE_RENDERER_NATIVE)
                // Prepare rectangle to be used
                r.x = image_x; r.y = y;
                r.width = m_iconwidth; r.height = m_iconheight;
            #elif wxPG_ICON_WIDTH
                // Drawing expand/collapse button manually
                dc.SetPen(m_colPropFore);
                if ( parenting > 0 )
                {
                    /*
                    //dc.SetPen(blackExpanderLine);
                    dc.SetPen(whiteExpanderLine);
                    //dc.SetBrush(greyExpanderFace);
                    */
                    dc.SetBrush(*wxTRANSPARENT_BRUSH);
                }
                else
                {
                    //dc.SetPen(whiteExpanderLine);
                    //dc.SetBrush(whiteExpanderFace);
                    dc.SetBrush(m_colPropBack);
                }
                dc.DrawRectangle( image_x, y, m_iconwidth, m_iconwidth );
                int _y = y+(m_iconwidth/2);
                //dc.SetPen(blackExpanderLine);
                dc.DrawLine(image_x+2,_y,image_x+m_iconwidth-2,_y);
            #else
                wxBitmap* bmp;
            #endif

                if ( pwc->m_expanded )
                {
                #if (wxPG_USE_RENDERER_NATIVE)
                    wxRendererNative::Get().DrawTreeItemButton (
                            this,
                            dc,
                            r,
                            wxCONTROL_EXPANDED
                        );
                #elif wxPG_ICON_WIDTH
                    //
                #else
                    bmp = m_collbmp;
                #endif

                }
                else
                {
                #if (wxPG_USE_RENDERER_NATIVE)
                    wxRendererNative::Get().DrawTreeItemButton (
                            this,
                            dc,
                            r,
                            0
                        );
                #elif wxPG_ICON_WIDTH
                    int _x = image_x+(m_iconwidth/2);
                    dc.DrawLine(_x,y+2,_x,y+m_iconwidth-2);
                #else
                    bmp = m_expandbmp;
                #endif
                }

            #if (wxPG_USE_RENDERER_NATIVE)
                //
            #elif wxPG_ICON_WIDTH
                //
            #else
                //dc.DrawBitmap ( *bmp, m_gutterwidth, y, false );
                dc.DrawBitmap ( *bmp, m_gutterwidth, y, true );
            #endif

                y -= m_buttonSpacingY;
            }

            y += m_spacingy;

            if ( parenting <= 0 )
            {
                // Non-categories.

                int text_x = x;
                // Use basic depth if in non-categoric mode and parent is base array.
                if ( !(window_style & wxPG_HIDE_CATEGORIES) || p->GetParent() != FROM_STATE(m_properties) )
                {
                    text_x += ((unsigned int)((p->m_depth-1)*m_subgroup_extramargin));
                }
                /*
                else
                {
                    wxLogDebug  ( wxT("%s"), p->GetLabel().c_str() );
                    text_x = x;
                }
                */

                if ( p != selected )
                {
                // background
                    dc.DrawText( p->m_label, text_x+wxPG_XBEFORETEXT, y );
                }
                else
                {
                // Selected gets different colours.
                    if ( reallyFocused )
                        dc.SetTextForeground( m_colSelFore );

                    dc.DrawText( p->m_label, text_x+wxPG_XBEFORETEXT, y );

                    dc.SetTextForeground( m_colPropFore );

                /*#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
                    selected_painted = true;
                #endif*/

                }
            }
            else
            {
                /*// switch background colour
                bgbrush.SetColour ( ((wxPropertyCategoryClass*)p)->m_colCellBg );
                dc.SetBrush ( bgbrush );*/
            }

            y += m_fontHeight+m_spacingy+1;

            //if ( y > last_item_bottom_y ) { parent = NULL; break; }

        ITEM_ITERATION_UVC_LOOP_END(vi_end_y)
    }


    //
    // Third pass: Values (including editor gfx if custom controls).
    //
    dc.DestroyClippingRegion();

    if ( clip_rect )
    {
        // third pass happens entirely on the right side, so sometimes
        // we can just skip it
        if ( (clip_rect->x + clip_rect->width) < m_splitterx )
            x = -1;
        dc.SetClippingRegion ( *clip_rect );
    }

    // This used with value drawer method.
    wxRect valueRect(0,0,
        m_width-(m_splitterx+wxPG_CONTROL_MARGIN),
        m_fontHeight);

    wxSize imageSize;

    if ( x != -1 )
    {

        r.x = m_splitterx+1+wxPG_CONTROL_MARGIN;
        r.width = m_width-m_splitterx-wxPG_CONTROL_MARGIN;
        //r.x = m_splitterx+wxPG_DIST_SPLITTER_TO_IMAGE;
        //r.width = m_width-m_splitterx-wxPG_DIST_SPLITTER_TO_IMAGE-1;
        r.height = lh-1;

    /*#if wxCC_CORRECT_CONTROL_POSITION
        const int vy2 = vy;
    #endif*/

            //wxLogDebug (wxT("  \\--> third pass..."));

        // Altough this line may seem unnecessary, it isn't
        dc.SetFont(normalfont);

        dc.SetPen( *wxTRANSPARENT_PEN );

        // Prepare paintdata.
        paintdata.m_parent = this;
        paintdata.m_choiceItem = -1; // Not drawing list item at this time.

        y = first_item_top_y;

        ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

            if ( parenting <= 0 )
            {
                r.y = y;
                y += m_spacingy;

             // background
                dc.SetBrush( *(wxPGBrush*)m_arrBgBrushes[p->m_bgColIndex] );

                // draw value string only if editor widget not open
                // (exception: no primary editor widget or it is hidden)
                if ( p != selected || !m_wndPrimary
                    // "if not primary shown" is required because
                    // primary is not usually shown during splitter
                    // movement.
                        || m_dragStatus > 0
                   )
                {

                    valueRect.x = m_splitterx+wxPG_CONTROL_MARGIN;
                    valueRect.y = y;

                    // Draw background
                    if ( p != selected )
                    {
                        dc.DrawRectangle( r );
                    }
                    else
                    {
                        if ( m_wndPrimary )
                            dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
                        else
                            dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

                        dc.DrawRectangle( r );
                    }

                    // Greyed text?
                    if ( p->m_flags & wxPG_PROP_DISABLED )
                        dc.SetTextForeground( m_colCapFore );

                    // Set bold font?
                    if ( p->m_flags & wxPG_PROP_MODIFIED && (window_style & wxPG_BOLD_MODIFIED) )
                        dc.SetFont( m_captionFont );

                    const wxPGEditor* editor = p->GetEditorClass();
                    bool fullPaint = false;

                    if ( p->m_flags & wxPG_PROP_CUSTOMIMAGE )
                    {
                        imageSize = p->GetImageSize();

                        wxRect imageRect(r.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                                         r.y+wxPG_CUSTOM_IMAGE_SPACINGY,
                                         wxPG_CUSTOM_IMAGE_WIDTH,
                                         r.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                        if ( imageSize.x == wxPG_FULL_CUSTOM_PAINT_WIDTH )
                        {
                            fullPaint = true;
                            imageRect.width = m_width - imageRect.x;
                        }
                        /*else
                        {
                            if ( imageSize.x == -1 )
                                imageRect.width = wxPG_CUSTOM_IMAGE_WIDTH;
                            else
                                imageRect.width = abs(imageSize.x);
                            imageRect.width = wxPG_CUSTOM_IMAGE_WIDTH;
                        }*/

                        dc.SetPen( outlinepen );

                        paintdata.m_drawnWidth = imageRect.width;

                        if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
                        {
                            p->OnCustomPaint( dc, imageRect, paintdata );
                        }
                        else
                        {
                            dc.SetBrush(*wxWHITE_BRUSH);
                            dc.DrawRectangle(imageRect);
                        }
                        dc.SetPen( *wxTRANSPARENT_PEN );
                    }
                    else
                        paintdata.m_drawnWidth = 0;

                    if ( paintdata.m_drawnWidth > 0 )
                        valueRect.x += paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                    else
                        fullPaint = false;

                    if ( !fullPaint )
                        editor->DrawValue(dc,p,valueRect);

                    // Return original text colour?
                    if ( p->m_flags & wxPG_PROP_DISABLED )
                        dc.SetTextForeground( m_colPropFore );

                    // Return original font?
                    if ( p->m_flags & wxPG_PROP_MODIFIED && (window_style & wxPG_BOLD_MODIFIED) )
                        dc.SetFont(normalfont);

                    /*
                    valueRect.x = m_splitterx+wxPG_CONTROL_MARGIN;
                    valueRect.y = y;

                    if ( p->m_flags & wxPG_PROP_CUSTOMIMAGE )
                    {
                        imageSize = GetImageSize(p);
                        valueRect.x += imageSize.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                    }

                    // Greyed text?
                    if ( p->m_flags & wxPG_PROP_DISABLED )
                        dc.SetTextForeground( m_colCapFore );

                    // Set bold font?
                    if ( p->m_flags & wxPG_PROP_MODIFIED && (window_style & wxPG_BOLD_MODIFIED) )
                        dc.SetFont( m_captionFont );

                    const wxPGEditor* editor = p->GetEditorClass();

                    if ( p != selected )
                    {
                        dc.DrawRectangle( r );
                        //wxLogDebug(wxT("out_combo: %i"),(int)(paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2));
                        editor->DrawValue(dc,p,valueRect);
                    }
                    else
                    {
                        if ( m_wndPrimary )
                            dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
                        else
                            dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

                        dc.DrawRectangle( r );
                        editor->DrawValue(dc,p,valueRect);
                    }

                    // Return original text colour?
                    if ( p->m_flags & wxPG_PROP_DISABLED )
                        dc.SetTextForeground( m_colPropFore );

                    // Return original font?
                    if ( p->m_flags & wxPG_PROP_MODIFIED && (window_style & wxPG_BOLD_MODIFIED) )
                        dc.SetFont(normalfont);

                    if ( p->m_flags & wxPG_PROP_CUSTOMIMAGE )
                    {
                        dc.SetPen ( outlinepen );
                        if ( !(m_iFlags & wxPG_FL_SELECTED_IS_FULL_PAINT) )
                        {
                            wxRect imageRect(r.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                                r.y+wxPG_CUSTOM_IMAGE_SPACINGY,wxPG_CUSTOM_IMAGE_WIDTH,
                                r.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                            paintdata.m_drawnWidth = r.width;

                            if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
                            {
                                p->OnCustomPaint( dc, imageRect, paintdata );
                            }
                            else
                            {
                                dc.SetBrush(*wxWHITE_BRUSH);
                                dc.DrawRectangle(imagerect);
                            }
                        }
                        else
                        {
                            wxRect imageRect(r.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                                r.y+wxPG_CUSTOM_IMAGE_SPACINGY,wxPG_CUSTOM_IMAGE_WIDTH,
                                r.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                            paintdata.m_drawnWidth = r.width;

                            if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
                                p->OnCustomPaint( dc, imageRect, paintdata );
                        }
                        dc.SetPen( *wxTRANSPARENT_PEN );
                    }
                    */

                }
                else
                {

                    if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) ||
                         m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE )
                    {
                        //wxLogDebug(wxT("Primary doesn't fill entire"));
                        //dc.SetBrush ( m_wndPrimary->GetBackgroundColour() );
                        dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
                        dc.DrawRectangle( r );
                    }
                    if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE )
                    {
                        wxRect imagerect(r.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                            r.y+wxPG_CUSTOM_IMAGE_SPACINGY,wxPG_CUSTOM_IMAGE_WIDTH,
                            r.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                        dc.SetPen ( outlinepen );
                        if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
                        {
                            p->OnCustomPaint( dc, imagerect, paintdata );
                        }
                        else
                        {
                            dc.SetBrush(*wxWHITE_BRUSH);
                            dc.DrawRectangle(imagerect);
                        }
                    }
                    dc.SetPen( *wxTRANSPARENT_PEN );
                }

                y += m_fontHeight+m_spacingy + 1;
            }
            else
            {
                // caption item
                //dc.DrawRectangle ( m_splitterx, y, width-m_splitterx, lh );
                y += lh;
            }

            //if ( y > last_item_bottom_y ) { parent = NULL; break; }

        ITEM_ITERATION_UVC_LOOP_END(vi_end_y)

    }

    dc.DestroyClippingRegion(); // Is this really necessary?

#if wxPG_DOUBLE_BUFFER
    //}
    //else wxLogDebug(wxT("Used Cache"));

    //if ( m_iFlags & wxPG_FL_HANDLING_PAINT_EVENT )
    //    m_iFlags &= ~(wxPG_FL_CHANGED);

    if ( blit_clip_rect )
        dc_main.SetClippingRegion( *blit_clip_rect );
    //wxLogDebug (wxT("  \\--> (0,%i)"),(int)final_y);
    dc_main.Blit ( 0, first_item->m_y, m_width, render_height,
        &dc, 0, first_item->m_y-vy, wxCOPY );
    //dc_main.Blit ( 0, 0, m_width, m_height,
    //    &dc, 0, 0, wxCOPY );
    dc_main.DestroyClippingRegion(); // Is this really necessary?

#endif

#if __PAINT_DEBUGGING__
    wxLogDebug (wxT("  \\--> ends..."));
#endif

}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItems ( wxPGProperty* p1, wxPGProperty* p2 )
{
    if ( m_frozen )
        return;
    if ( FROM_STATE(m_itemsAdded) )
        PrepareAfterItemsAdded();
    if ( m_width < 10 || m_height < 10 )
        return;
    if ( !FROM_STATE(m_properties)->GetCount() )
        return;

    int vx,vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;


    if ( p1 == (wxPGProperty*) NULL ) return;

    int visTop = p1->m_y;
    int visBottom = m_bottomy;
    if ( p2 )
        visBottom = p2->m_y + m_lineHeight;

    wxRect r(0,visTop-vy,m_width,visBottom-visTop);
    RefreshRect(r);
}

// -----------------------------------------------------------------------

// In addition to calling DoDrawItems directly, this is the
// only alternative for using wxClientDC - others just call
// RefreshRect.
void wxPropertyGrid::DrawItem ( wxDC& dc, wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    // do not draw a single item if multiple pending
    if ( FROM_STATE(m_itemsAdded) )
        return;

    if ( p->m_y < 0 )
        return;

#if __PAINT_DEBUGGING__
    wxLogDebug(wxT("wxPropertyGrid::DrawItem( %s )"), p->GetLabel().c_str() );
#endif

    DoDrawItems ( dc, p, p, NULL );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::RefreshProperty( wxPGProperty* p )
{
    if ( p == m_selected )
        DoSelectProperty(p,wxPG_SEL_FORCE);

    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItemAndChildren ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    // do not draw a single item if multiple pending
    if ( FROM_STATE(m_itemsAdded) || p->m_y < 0 || m_frozen )
        return;

#if __PAINT_DEBUGGING__
    wxLogDebug(wxT("wxPropertyGrid::DrawItemAndChildren( %s )"), p->GetLabel().c_str() );
#endif

    // Enable commented lines (and disable immediately following ones)
    // to use client-drawing instead of onPaint drawing.

    //wxClientDC dc(this);
    //PrepareDC(dc);

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

    // Update child control.
    if ( m_selected && m_selected->GetParent() == pwc )
        m_selected->UpdateControl(m_wndPrimary);

    wxPGProperty* lastDrawn = pwc;

    if ( pwc->GetParentingType() != 0 &&
         pwc->GetCount() &&
         pwc->m_expanded )
    {
        //last_drawn = GetNearestPaintVisible ( pwc->Last() );
        lastDrawn = pwc->Last();
    }

    DrawItems(pwc,lastDrawn);

    /*
    int vx,vy;
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    //DoDrawItems ( dc, pwc, last_drawn, NULL );
    wxRect r(0,pwc->m_y-vy,m_width,last_drawn->m_y-pwc->m_y+m_lineHeight);
    RefreshRect(r);
    */

}

// -----------------------------------------------------------------------

void wxPropertyGrid::Refresh( bool WXUNUSED(eraseBackground),
                              const wxRect *rect )
{
    // Refresh implies forced redraw
    //m_iFlags |= wxPG_FL_CHANGED;

    wxWindow::Refresh(false,rect);
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    // I think this really helps only GTK+1.2
    if ( m_wndPrimary ) m_wndPrimary->Refresh();
    if ( m_wndSecondary ) m_wndSecondary->Refresh();
#endif
}

// -----------------------------------------------------------------------

/*
void wxPropertyGrid::RedrawAllVisible ()
{
    // TODO: Is this safe?
    //Update();

    if ( m_frozen || !IsShown() )
        return;

    wxPG_CLIENT_DC_INIT()

#if __PAINT_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::RedrawAllVisible()") );
#endif

    int vx,vy;                     // Top left corner of client
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    int y1 = vy;
    int y2 = y1 + m_height;

    // Repaint this rectangle
    DrawItems ( dc, y1, y2, (wxRect*) NULL );

}
*/

// -----------------------------------------------------------------------

#if wxPG_HEAVY_GFX
void wxPropertyGrid::DrawSplitterDragColumn ( wxDC&, int ) { }
#else
void wxPropertyGrid::DrawSplitterDragColumn ( wxDC& dc, int x )
{
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    dc.SetLogicalFunction(wxINVERT);
    dc.DestroyClippingRegion();

    dc.SetPen( m_splitterpen );
    dc.DrawLine(x,vy,x,vy+m_height);

}
#endif

// -----------------------------------------------------------------------
// wxPropertyGrid global operations
// -----------------------------------------------------------------------

void wxPropertyGrid::Clear()
{
    //wxASSERT_MSG ( wxStrcmp(GetParent()->GetClassInfo()->GetClassName(),wxT("wxPropertyGridManager")) != 0,
    //    wxT("Don't call this in wxPropertyGridManager."));

    if ( m_selected )
    {
        bool selRes = ClearSelection();  // This must be before state clear
        wxASSERT_MSG( selRes,
                      wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    FROM_STATE(Clear());

    m_propHover = NULL;
    m_bottomy = 0;

    m_prevVY = 0;
    m_arrVisible.Empty();

    RecalculateVirtualSize();

    // Need to clear some area at the end
    if ( !m_frozen )
    {
        wxColour& bgc = wxPG_SLACK_BACKROUND;
        wxPG_CLIENT_DC_INIT()
        dc.SetPen( bgc );
        dc.SetBrush( bgc );
        dc.DrawRectangle( 0, 0, m_width, m_height );
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableCategories ( bool enable )
{
    if ( !ClearSelection() )
        return false;

    if ( enable )
    {
        //
        // Enable categories
        //

        m_windowStyle &= ~(wxPG_HIDE_CATEGORIES);
    }
    else
    {
        //
        // Disable categories
        //
        m_windowStyle |= wxPG_HIDE_CATEGORIES;
    }

    if ( !m_pState->EnableCategories(enable) )
        return false;

    if ( !m_frozen )
    {
        if ( m_windowStyle & wxPG_AUTO_SORT )
        {
            FROM_STATE(m_itemsAdded) = 1; // force
            PrepareAfterItemsAdded();
        }
        else
        {
            CalculateYs(NULL,-1);
            //CalculateVisibles( -1 );
        }
    }
    else
        FROM_STATE(m_itemsAdded) = 1;

    Refresh();

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SwitchState ( wxPropertyGridState* pNewState )
{
    wxASSERT ( pNewState );

    wxPGProperty* oldSelection = m_selected;

    // Deselect.
    if ( m_selected )
    {
        bool selRes = ClearSelection();
        wxASSERT_MSG( selRes,
                      wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    m_pState->m_selected = oldSelection;

    bool orig_mode = m_pState->IsInNonCatMode();
    bool new_state_mode = pNewState->IsInNonCatMode();

    m_pState = pNewState;

    m_bottomy = 0; // This is necessary or y's won't get updated.

    // If necessary, convert state to correct mode.
    if ( orig_mode != new_state_mode )
    {
        // This should refresh as well.
        EnableCategories ( orig_mode?false:true );
    }
    else if ( !m_frozen )
    {
        // Refresh, if not frozen.
        if ( FROM_STATE(m_itemsAdded) )
            PrepareAfterItemsAdded();
        else
            CalculateYs(NULL,-1);

        // Reselect
        if ( FROM_STATE(m_selected) )
            DoSelectProperty( FROM_STATE(m_selected) );

        //RedrawAllVisible();
        Refresh();
    }
    else
        m_pState->m_itemsAdded = 1;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Sort ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    m_pState->Sort( p );

    // Because order changed, Y's need to be changed as well
    if ( p->GetParentState() == m_pState )
        CalculateYs ( p->m_parent, p->m_arrIndex );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Sort ()
{
    bool selRes = ClearSelection();  // This must be before state clear
    wxASSERT_MSG( selRes,
                  wxT("failed to deselect a property (editor probably had invalid value)") );

    m_pState->Sort();

    CalculateYs( NULL, -1 );
}

// -----------------------------------------------------------------------

// Call to SetSplitterPosition will always disable splitter auto-centering
// if parent window is shown.
void wxPropertyGrid::DoSetSplitterPosition ( int newxpos, bool refresh )
{
    if ( ( newxpos < wxPG_DRAG_MARGIN ) )
        return;
    // ( m_width > wxPG_DRAG_MARGIN && newxpos > (m_width-wxPG_DRAG_MARGIN) )

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::DoSetSplitterPosition ( %i )"), newxpos );
#endif

#if wxPG_HEAVY_GFX
    m_splitterx = newxpos;
    m_fSplitterX = (float) newxpos;

    if ( refresh )
    {
        if ( m_selected )
            CorrectEditorWidgetSizeX( m_splitterx, m_width );

        Refresh();
        //RedrawAllVisible(); // no flicker
    }
#else
    if ( !m_dragStatus )
    {
        // Only do this if this was not a call from HandleMouseUp
        m_startingSplitterX = m_splitterx;
        m_splitterx = newxpos;
        m_fSplitterX = (float) newxpos;
    }

    // Clear old
    if ( m_splitterprevdrawnx != -1 )
    {
        wxPG_CLIENT_DC_INIT()

        DrawSplitterDragColumn( dc, m_splitterprevdrawnx );
        m_splitterprevdrawnx = -1;
    }

    // Redraw only if drag really moved
    if ( m_splitterx != m_startingSplitterX && refresh )
    {
        if ( m_selected)
            CorrectEditorWidgetSizeX( m_splitterx, m_width );

        Update(); // This fixes a graphics-mess in wxMSW

        Refresh();
        //RedrawAllVisible(); // no flicker
    }
#endif

    // Don't allow initial splitter auto-positioning after this.
    m_iFlags |= wxPG_FL_SPLITTER_PRE_SET;

}

// -----------------------------------------------------------------------

void wxPropertyGrid::CenterSplitter ( bool enable_auto_centering )
{
    SetSplitterPosition ( m_width/2, true );
    if ( enable_auto_centering && ( m_windowStyle & wxPG_SPLITTER_AUTO_CENTER ) )
        m_iFlags &= ~(wxPG_FL_DONT_CENTER_SPLITTER);
}

// -----------------------------------------------------------------------

// Moves splitter so that all labels are visible, but just.
void wxPropertyGrid::SetSplitterLeft( bool subProps )
{
    wxClientDC dc(this);
    dc.SetFont(m_font);

    int maxW = m_pState->GetLeftSplitterPos(dc, m_pState->m_properties, subProps );

    if ( maxW > 0 )
        SetSplitterPosition( maxW );

    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;
}


// -----------------------------------------------------------------------
// wxPropertyGrid item iteration (GetNextProperty etc.) methods
// -----------------------------------------------------------------------

// Returns nearest paint visible property (such that will be painted unless
// window is scrolled or resized). If given property is paint visible, then
// it itself will be returned
wxPGProperty* wxPropertyGrid::GetNearestPaintVisible ( wxPGProperty* p )
{
    int vx,vy1;// Top left corner of client
    GetViewStart(&vx,&vy1);
    vy1 *= wxPG_PIXELS_PER_UNIT;

    int vy2 = vy1 + m_height;

    if ( (p->m_y + m_lineHeight) < vy1 )
    {
    // Too high
        return DoGetItemAtY( vy1 );
    }
    else if ( p->m_y > vy2 )
    {
    // Too low
        return DoGetItemAtY( vy2 );
    }

    // Itself paint visible
    return p;

}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGrid::GetNeighbourItem ( wxPGProperty* item,
    bool need_visible, int dir ) const
{
    wxPGPropertyWithChildren* parent = item->m_parent;
    unsigned int indinparent = item->GetIndexInParent();

    if ( dir > 0 )
    {
        if ( item->GetChildCount() == 0 ||
             (!((wxPGPropertyWithChildren*)item)->m_expanded && need_visible) )
        {
            // current item did not have any expanded children
            if ( indinparent < (parent->GetCount()-1) )
            {
                // take next in parent's array
                item = parent->Item(indinparent+1);
            }
            else
            {
                // no more in parent's array; move up until found;
                wxPGPropertyWithChildren* p2 = parent;
                parent = parent->m_parent;
                item = (wxPGProperty*) NULL;
                while ( parent )
                {
                    if ( p2->m_arrIndex < (parent->GetCount()-1) )
                    {
                        item = parent->Item(p2->m_arrIndex+1);
                        break;
                    }
                    p2 = parent;
                    parent = parent->m_parent;
                }
            }
        }
        else
        {
            // take first of current item's children
            wxPGPropertyWithChildren* p2 = (wxPGPropertyWithChildren*)item;
            item = p2->Item(0);
            //indinparent = 0;
        }
    }
    else
    {

        // items in array left?
        if ( indinparent > 0 )
        {
            // take prev in parent's array
            item = parent->Item(indinparent-1);
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)item;

            // Recurse to it's last child
            while ( item->GetParentingType() != 0 && pwc->GetCount() &&
                    ( pwc->m_expanded || !need_visible )
                  )
            {
                item = pwc->Last();
                pwc = (wxPGPropertyWithChildren*)item;
            }

        }
        else
        {
            // If we were at first, go to parent
            item = parent;
        }
    }

    if ( item == FROM_STATE(m_properties) )
        return (wxPGProperty*) NULL;

    // If item was hidden and need_visible, get next.
    if ( (m_iFlags & wxPG_FL_HIDE_STATE) && need_visible && item )
    {
        if ( item->m_flags & wxPG_PROP_HIDEABLE )
        {
            // Speed-up: If parent is hidden as well, then skip to last child or to itself
            if ( parent->m_flags & wxPG_PROP_HIDEABLE )
            {
                item = parent; // if dir up
                if ( dir > 0 )
                    item = parent->Last(); // if dir down
            }

            return GetNeighbourItem ( item, need_visible, dir );
        }
    }

    return item;
}

// -----------------------------------------------------------------------

// This is used in DoDrawItems.
wxPGProperty* wxPropertyGrid::GetLastItem ( bool need_visible, bool allow_subprops )
{
    if ( FROM_STATE(m_properties)->GetCount() < 1 )
        return (wxPGProperty*) NULL;

    wxPGProperty* p = FROM_STATE(m_properties)->Last();

    int parenting = p->GetParentingType();

    while ( parenting != 0 && ( allow_subprops || parenting > 0 ) )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        parenting = 0;
        if ( pwc->GetCount() )
        {

            if ( (!need_visible || pwc->m_expanded) )
            {
                p = pwc->Last();
                parenting = p->GetParentingType();
            }
            else
                parenting = 0;
        }
    }

    // If item was hidden and need_visible, get previous.
    if ( (m_iFlags & wxPG_FL_HIDE_STATE) && need_visible &&
         p && ( p->m_flags & wxPG_PROP_HIDEABLE )
       )
        return GetNeighbourItem ( p, need_visible, -1 );

    return p;
}

// -----------------------------------------------------------------------
// Methods related to change in value, value modification and sending events
// -----------------------------------------------------------------------

// commits any changes in editor of selected property
// return true if validation did not fail
// flags are same as with DoSelectProperty
bool wxPropertyGrid::CommitChangesFromEditor( wxUint32 flags )
{
    if ( m_wndPrimary && IsEditorsValueModified() &&
         (m_iFlags & wxPG_FL_INITIALIZED) )
    {
        wxASSERT( m_selected );

        if ( !(flags & (wxPG_SEL_NOVALIDATE|wxPG_SEL_FORCE)) &&
             !DoEditorValidate() )
        {
            return false;
        }

        // Save value (only if truly modified).
        if ( !m_selected->GetEditorClass()->CopyValueFromControl( m_selected, m_wndPrimary ) )
            EditorsValueWasNotModified();

        DoPropertyChanged( m_selected );

        return true;
    }

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoPropertyChanged( wxPGProperty* p )
{

    if ( m_processingEvent )
        return;

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("wxPropertyGrid::DoPropertyChanged( %s )"),p->GetLabel().c_str());
#endif

    m_pState->m_anyModified = 1;

    wxPropertyGridEvent evt( wxEVT_PG_CHANGED, GetId() );
    evt.SetPropertyGrid(this);
    evt.SetEventObject(this);

    m_processingEvent = 1;

    // No longer unspecified
    //m_pState->ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
    CLEAR_PROPERTY_UNSPECIFIED_FLAG(p);

    if ( m_iFlags & wxPG_FL_VALUE_MODIFIED )
    {
        m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);

        // Set as Modified (not if dragging just began)
        if ( !(p->m_flags & wxPG_PROP_MODIFIED) )
        {
            p->m_flags |= wxPG_PROP_MODIFIED;
            if ( p == m_selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
            {
                if ( m_wndPrimary )
                    SetCurControlBoldFont();
            }
        }

        wxPGProperty* curChild = p;
        wxPGPropertyWithChildren* curParent = p->m_parent;

        // Also update parent(s), if any
        // (but not if its wxCustomProperty)

        while ( curParent &&
                curParent->GetParentingType() < 0 /*&&
                wxStrcmp(curParent->GetClassName(),wxT("wxCustomProperty")) != 0*/ )
        {
            // Set as Modified
            if ( !(curParent->m_flags & wxPG_PROP_MODIFIED) )
            {
                curParent->m_flags |= wxPG_PROP_MODIFIED;
                if ( curParent == m_selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
                {
                    if ( m_wndPrimary )
                        SetCurControlBoldFont();
                }
            }

            curParent->ChildChanged ( curChild );

            DrawItem ( curParent );

            curChild = curParent;
            curParent = curParent->GetParent();
        }

        // Draw the actual property
        if ( ( p != m_selected ) || !m_wndPrimary ||
             ( p->GetParentingType() < 0 ) ||
             ( p->m_flags & wxPG_PROP_CUSTOMIMAGE ) )
        {
            DrawItemAndChildren( p );
        }

        if ( curChild != p )
            //m_pState->ClearPropertyAndChildrenFlags(curChild,wxPG_PROP_UNSPECIFIED);
            CLEAR_PROPERTY_UNSPECIFIED_FLAG(curChild);

        // Call wx event handler for property (or its topmost parent, but only
        // when dealign with legitemate sub-properties - see above).
        if ( curChild->GetParentingType() != -2 )
            evt.SetProperty( curChild );
        else
            evt.SetProperty( p );

        // Maybe need to update control
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        if ( m_wndPrimary ) m_wndPrimary->Refresh();
        if ( m_wndSecondary ) m_wndSecondary->Refresh();
#endif

        GetEventHandler()->AddPendingEvent(evt);

    }

    m_processingEvent = 0;

}

// -----------------------------------------------------------------------

// Runs wxValidator for the selected property
bool wxPropertyGrid::DoEditorValidate()
{
#if wxUSE_VALIDATORS
    if ( m_iFlags & wxPG_FL_VALIDATION_FAILED )
    {
        return false;
    }

    wxWindow* wnd = m_wndPrimary;
#if wxPG_ENABLE_CLIPPER_WINDOW
    if ( wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
        wnd = ((wxPGClipperWindow*)wnd)->GetControl();
#endif

    wxValidator* validator = m_selected->GetValidator();
    if ( validator && wnd )
    {
        validator->SetWindow(wnd);

        // Instead setting the flag after the failure, we set
        // it before checking and then clear afterwards if things
        // went fine. This trick is necessary since focus events
        // may be triggered while in Validate.
        m_iFlags |= wxPG_FL_VALIDATION_FAILED;
        if ( !validator->Validate(this) )
        {
            //wxPrintf(wxT("  validation failed\n"));
            return false;
        }
        m_iFlags &= ~(wxPG_FL_VALIDATION_FAILED);
    }
#endif
    //wxPrintf(wxT("  validation success\n"));
    return true;
}

// -----------------------------------------------------------------------

// NB: It may really not be wxCommandEvent - must check if necessary
//     (usually not).
void wxPropertyGrid::OnCustomEditorEvent( wxCommandEvent &event )
{
    wxPGProperty* selected = m_selected;
    wxASSERT( selected );
    wxWindow* wnd = m_wndPrimary;
    bool res1, res2;

    m_iFlags &= ~(wxPG_FL_VALIDATION_FAILED);

    // First call editor class' event handler.
    const wxPGEditor* editor = selected->GetEditorClass();
    res1 = editor->OnEvent( this, selected, wnd, event );

    if ( res1 )
    {
        // If changes, validate them
        if ( DoEditorValidate() )
        {
            if ( editor->CopyValueFromControl( selected, wnd ) )
            {
            }
            else
            {
                // False alarm
                res1 = false;

                EditorsValueWasNotModified();

                // However, even moot editing will clear the unspecified status
                CLEAR_PROPERTY_UNSPECIFIED_FLAG(selected);
            }
        }
        else
        {
            res1 = false;
            EditorsValueWasNotModified();
            CLEAR_PROPERTY_UNSPECIFIED_FLAG(selected);
            return;
        }
    }


    // Then the property's custom handler (must be always called).
    res2 = selected->OnEvent( this, wnd, event );

    if ( res1 || res2 )
    {
        // Setting this is not required if res was true, so we do it now.
        m_iFlags |= wxPG_FL_VALUE_MODIFIED;

        //wxPG_CLIENT_DC_INIT()
        //DoPropertyChanged(dc,m_selected);
        DoPropertyChanged(selected);

    }
    else
        // Let unhandled button click events go to the parent
        if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
        {
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED,GetId());
            GetEventHandler()->AddPendingEvent(evt);
        }

}

// -----------------------------------------------------------------------

// When a property's value was modified internally (using SetValueFromString
// or SetValueFromInt, for example), then this should be called afterwards.
void wxPropertyGrid::PropertyWasModified( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    EditorsValueWasModified();
    //wxPG_CLIENT_DC_INIT()
    //DoPropertyChanged(dc,p);
    DoPropertyChanged(p);
}

// -----------------------------------------------------------------------
// wxPropertyGrid editor control helper methods
// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GetEditorControl() const
{
    wxWindow* ctrl = m_wndPrimary;

    // If it's clipper window, return its child instead
#if wxPG_ENABLE_CLIPPER_WINDOW
    if ( ctrl->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
    {
        return ((wxPGClipperWindow*)ctrl)->GetControl();
    }
#endif

    return ctrl;
}

// -----------------------------------------------------------------------

// inline because it is used exactly once in the code
inline wxRect wxPropertyGrid::GetEditorWidgetRect( wxPGProperty* p )
{
    //wxASSERT ( p->m_y >= 0 ); // item is not visible

    int itemy = p->m_y;
    int vx,vy;// Top left corner of client
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;
    int cust_img_space = 0;

    //m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);

    // TODO: If custom image detection changes from current, change this.
    if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE /*p->m_flags & wxPG_PROP_CUSTOMIMAGE*/ )
    {
        //m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;
        int imwid = p->GetImageSize().x;
        if ( imwid < 1 ) imwid = wxPG_CUSTOM_IMAGE_WIDTH;
        cust_img_space = imwid + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
    }

    return wxRect
      (
        m_splitterx+cust_img_space+wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1,
        itemy-vy,
        m_width-m_splitterx-wxPG_XBEFOREWIDGET-wxPG_CONTROL_MARGIN-cust_img_space-1,
        m_lineHeight-1
      );
}

// -----------------------------------------------------------------------

// return size of custom paint image
wxSize wxPropertyGrid::GetImageSize( wxPGId id ) const
{
    if ( wxPGIdIsOk(id) )
    {
        wxSize cis = wxPGIdToPtr(id)->GetImageSize();

        if ( cis.x < 0 )
        {
            if ( cis.x <= -1 )
                cis.x = wxPG_CUSTOM_IMAGE_WIDTH;
        }
        if ( cis.y <= 0 )
        {
            if ( cis.y >= -1 )
                cis.y = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
            else
                cis.y = -cis.y;
        }
        return cis;
    }
    return wxSize(wxPG_CUSTOM_IMAGE_WIDTH,wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight));
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CorrectEditorWidgetSizeX( int newSplitterx, int newWidth )
{
    wxASSERT ( m_selected );

    int secWid = 0;

    if ( m_wndSecondary )
    {
        // if width change occurred, move secondary wnd by that amount
        wxRect r = m_wndSecondary->GetRect();
        secWid = r.width;
        r.x = newWidth - secWid;
        //r.y += yAdj;

        m_wndSecondary->SetSize ( r );

        // if primary is textctrl, then we have to add some extra space
        if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxTextCtrl)) )
            secWid += wxPG_TEXTCTRL_AND_BUTTON_SPACING;
    }

    if ( m_wndPrimary )
    {
        wxRect r = m_wndPrimary->GetRect();

        r.x = newSplitterx+m_ctrlXAdjust;
        //r.y += yAdj;
        r.width = newWidth - r.x - secWid;

        m_wndPrimary->SetSize(r);
    }

/*
    int sec_wid = 0;

    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    int propY = m_selected->m_y - vy;

    if ( m_wndSecondary )
    {
        // if width change occurred, move secondary wnd by that amount
        wxRect r = m_wndSecondary->GetRect();
        int adjust = r.y % wxPG_PIXELS_PER_UNIT;
        if ( adjust > (wxPG_PIXELS_PER_UNIT/2) )
            adjust = adjust - wxPG_PIXELS_PER_UNIT;
        int y = propY + adjust;
        sec_wid = r.width;

        m_wndSecondary->Move ( new_width-r.width,y );

        // if primary is textctrl, then we have to add some extra space
        if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxTextCtrl)) )
            sec_wid += wxPG_TEXTCTRL_AND_BUTTON_SPACING;
    }

    if ( m_wndPrimary )
    {
        wxRect r = m_wndPrimary->GetRect();
        int adjust = r.y % wxPG_PIXELS_PER_UNIT;
        if ( adjust > (wxPG_PIXELS_PER_UNIT/2) )
            adjust = adjust - wxPG_PIXELS_PER_UNIT;
        wxLogDebug(wxT("adjust: %i"),adjust);
        int y = propY + adjust;

        m_wndPrimary->SetSize(
            new_splitterx+m_ctrlXAdjust,
            y,
            new_width-(new_splitterx+m_ctrlXAdjust)-sec_wid,
            r.height
        );
    }
*/

    if ( m_wndSecondary )
        m_wndSecondary->Refresh();

}

// -----------------------------------------------------------------------

/*void wxPropertyGrid::CorrectEditorWidgetSizeY( int cy )
{
    if ( m_selected )
    {
        wxPoint cp(0,cy);

        if ( m_wndPrimary )
            m_wndPrimary->Move ( m_wndPrimary->GetPosition() + cp );

        if ( m_wndSecondary )
            m_wndSecondary->Move ( m_wndSecondary->GetPosition() + cp );
    }
}*/

// -----------------------------------------------------------------------

// takes scrolling into account
void wxPropertyGrid::ImprovedClientToScreen( int* px, int* py )
{
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    vx*=wxPG_PIXELS_PER_UNIT;
    *px -= vx;
    *py -= vy;
    ClientToScreen ( px, py );
}

// -----------------------------------------------------------------------

// custom set cursor
void wxPropertyGrid::CustomSetCursor( int type, bool override )
{
    if ( type == m_curcursor && !override ) return;

    wxCursor* cursor = &wxPG_DEFAULT_CURSOR;

    if ( type == wxCURSOR_SIZEWE )
        cursor = m_cursor_sizewe;

    SetCursor ( *cursor );

    //if ( m_wndPrimary ) m_wndPrimary->SetCursor(wxNullCursor);

    m_curcursor = type;
}

// -----------------------------------------------------------------------
// wxPropertyGrid property selection
// -----------------------------------------------------------------------

#define CONNECT_CHILD(EVT,FUNCTYPE,FUNC) \
    wnd->Connect(id, EVT, \
        (wxObjectEventFunction) (wxEventFunction)  \
        FUNCTYPE (&wxPropertyGrid::FUNC), \
        NULL, this );

// Setups event handling for child control
void wxPropertyGrid::SetupEventHandling( wxWindow* argWnd, int id )
{
    wxWindow* wnd = argWnd;

#if wxPG_ENABLE_CLIPPER_WINDOW
    // Pass real control instead of clipper window
    if ( wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
    {
        wnd = ((wxPGClipperWindow*)argWnd)->GetControl();
    }
#endif

    if ( argWnd == m_wndPrimary )
    {
        CONNECT_CHILD(wxEVT_MOTION,(wxMouseEventFunction),OnMouseMoveChild)
        CONNECT_CHILD(wxEVT_LEFT_UP,(wxMouseEventFunction),OnMouseUpChild)
        CONNECT_CHILD(wxEVT_LEFT_DOWN,(wxMouseEventFunction),OnMouseClickChild)
        CONNECT_CHILD(wxEVT_RIGHT_UP,(wxMouseEventFunction),OnMouseRightClickChild)
        CONNECT_CHILD(wxEVT_ENTER_WINDOW,(wxMouseEventFunction),OnMouseEntry)
        CONNECT_CHILD(wxEVT_LEAVE_WINDOW,(wxMouseEventFunction),OnMouseEntry)
        CONNECT_CHILD(wxEVT_KEY_DOWN,(wxCharEventFunction),OnKeyChild)
    }
    CONNECT_CHILD(wxEVT_SET_FOCUS,(wxFocusEventFunction),OnFocusEvent)
    CONNECT_CHILD(wxEVT_KILL_FOCUS,(wxFocusEventFunction),OnFocusEvent)
}

// Call with NULL to de-select property
bool wxPropertyGrid::DoSelectProperty( wxPGProperty* p, unsigned int flags )
{

#if __INTENSE_DEBUGGING__
    if (p)
        wxLogDebug(wxT("SelectProperty( %s (%s[%i]) )"),p->m_label.c_str(),
            p->m_parent->m_label.c_str(),p->GetIndexInParent());
    else
        wxLogDebug(wxT("SelectProperty( NULL, -1 )"));
#endif

    wxPGProperty* prev = m_selected;

    //
    // If we are frozen, then just set the values.
    if ( m_frozen )
    {
        m_editorFocused = 0;
        m_selected = p;
        FROM_STATE(m_selected) = p;

        // If frozen, always free controls. But don't worry, as Thaw will
        // recall SelectProperty to recreate them.
        if ( m_wndSecondary )
        {
            m_wndSecondary->Destroy();
            m_wndSecondary = (wxWindow*) NULL;
        }

        if ( m_wndPrimary )
        {
            m_wndPrimary->Destroy();
            m_wndPrimary = (wxWindow*) NULL;
        }

        // Prevent any further selection measures in this call
        p = (wxPGProperty*) NULL;

    }
    else
    {
        // Is it the same?
        if ( m_selected == p && !(flags & wxPG_SEL_FORCE) )
        {
            // Only set focus if not deselecting
            if ( p )
            {
                if ( flags & wxPG_SEL_FOCUS )
                {
                    if ( m_wndPrimary )
                    {
                        m_wndPrimary->SetFocus();
                        m_editorFocused = 1;
                    }
                }
                else
                {
                    wxScrolledWindow::SetFocus();
                    m_editorFocused = 0;
                }
            }

            return true;
        }

        wxClientDC dc(this);
        PrepareDC(dc);

        // Don't put this earlier, due to return statements
        m_iFlags |= wxPG_FL_IN_SELECT_PROPERTY;

        //
        // First, deactivate previous
        if ( m_selected )
        {

    #if __INTENSE_DEBUGGING__
            wxLogDebug (wxT("  (closing previous (%s))"), m_selected->m_label.c_str() );
    #endif

            // Must double-check if this is an selected in case of forceswitch
            if ( p != prev )
            {
                if ( !CommitChangesFromEditor(flags) )
                {
                    // Validation has failed, so we can't exit the previous editor
                    //::wxMessageBox(_("Please correct the value or press ESC to cancel the edit."),
                    //               _("Invalid Value"),wxOK|wxICON_ERROR);
                    return false;
                }
            }

            if ( m_wndSecondary )
            {
                m_wndSecondary->Destroy();
                m_wndSecondary = (wxWindow*) NULL;
            }

            if ( m_wndPrimary )
            {
                m_wndPrimary->Destroy();
                m_wndPrimary = (wxWindow*) NULL;
            }

            m_iFlags &= ~(wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE|wxPG_FL_SELECTED_IS_FULL_PAINT);
            m_selected = (wxPGProperty*) NULL;
            FROM_STATE(m_selected) = (wxPGProperty*) NULL;

            // Make sure the previous selection is refreshed
            if ( prev->m_y < (int)m_bottomy )
                DoDrawItems ( dc, prev, prev, NULL );

            m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);
        }

        //
        // Then, activate the one given.
        if ( p )
        {

            m_editorFocused = 0;
            m_selected = p;
            FROM_STATE(m_selected) = p;
            m_iFlags |= wxPG_FL_PRIMARY_FILLS_ENTIRE;
            if ( p != prev )
                m_iFlags &= ~(wxPG_FL_VALIDATION_FAILED);

            //m_wndPrimary = (wxWindow*) NULL;
            wxASSERT( m_wndPrimary == (wxWindow*) NULL );


            // Do we need OnMeasureCalls?
            wxSize imsz = p->GetImageSize();
            if ( imsz.y < -1 )
                m_iFlags |= wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE;

            // Is the entire cell/row custom painted?
            if ( imsz.x == wxPG_FULL_CUSTOM_PAINT_WIDTH )
                m_iFlags |= wxPG_FL_SELECTED_IS_FULL_PAINT;


            //
            // Only create editor for non-disabled non-caption
            if ( p->GetParentingType() <= 0 && !(p->m_flags & wxPG_PROP_DISABLED) )
            {
            // do this for non-caption items

                // Do we need to paint the custom image, if any?
                m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);
                if ( (p->m_flags & wxPG_PROP_CUSTOMIMAGE) &&
                     !p->GetEditorClass()->CanContainCustomImage()
                   )
                    m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;

                wxRect grect = GetEditorWidgetRect(p);
                wxPoint good_pos = grect.GetPosition();
            #if wxPG_CREATE_CONTROLS_HIDDEN
                int coord_adjust = m_height - good_pos.y;
                good_pos.y += coord_adjust;
            #endif

                const wxPGEditor* editor = p->GetEditorClass();
                wxASSERT_MSG(editor,
                    wxT("NULL editor class not allowed"));

                m_wndPrimary = editor->CreateControls(this,
                                                      p,
                                                      good_pos,
                                                      grect.GetSize(),
                                                      &m_wndSecondary);

                // NOTE: It is allowed for m_wndPrimary to be NULL - in this case
                //       value is drawn as normal, and m_wndSecondary is assumed
                //       to be a right-aligned button that triggers a separate editor
                //       window.

                if ( m_wndPrimary )
                {

                    // Set validator, if any
                /*#if wxUSE_VALIDATORS
                    if ( validator ) m_wndPrimary->SetValidator(*validator);
                #endif*/

                    // If it has modified status, use bold font
                    // (must be done before capturing m_ctrlXAdjust)
                    if ( (p->m_flags & wxPG_PROP_MODIFIED) && (m_windowStyle & wxPG_BOLD_MODIFIED) )
                        SetCurControlBoldFont();

                    // Store x relative to splitter (we'll need it).
                    m_ctrlXAdjust = m_wndPrimary->GetPosition().x - m_splitterx;

                    // Check if background clear is not necessary
                    // FIXME: Reverse conditional (as would be logical)
                    wxPoint pos = m_wndPrimary->GetPosition();
                    //if ( pos.x <= (m_splitterx+1) && pos.y <= p->m_y )
                    //else
                    if ( pos.x > (m_splitterx+1) || pos.y > p->m_y )
                    {
                        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
                    }

                    m_wndPrimary->SetSizeHints(3,3);

                #if wxPG_CREATE_CONTROLS_HIDDEN
                    m_wndPrimary->Show(false);
                    m_wndPrimary->Freeze();

                    good_pos = m_wndPrimary->GetPosition();
                    good_pos.y -= coord_adjust;
                    m_wndPrimary->Move( good_pos );
                #endif

                    SetupEventHandling(m_wndPrimary,wxPG_SUBID1);

                    // Focus and select all (wxTextCtrl, wxComboBox etc)
                    if ( flags & wxPG_SEL_FOCUS )
                    {
                        wxWindow* ctrl = m_wndPrimary;
                        ctrl->SetFocus();

                    #if wxPG_NAT_TEXTCTRL_BORDER_ANY
                        // Take into account textctrl in clipper window
                        if ( ctrl->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
                            ctrl = ((wxPGClipperWindow*)ctrl)->GetControl();
                    #endif

                        p->GetEditorClass()->OnFocus(p,m_wndPrimary);
                    }
                }

                if ( m_wndSecondary )
                {

                    m_wndSecondary->SetSizeHints(3,3);

                #if wxPG_CREATE_CONTROLS_HIDDEN
                    wxRect sec_rect = m_wndSecondary->GetRect();
                    sec_rect.y -= coord_adjust;

                    // Fine tuning required to fix "oversized"
                    // button disappearance bug.
                    if ( sec_rect.y < 0 )
                    {
                        sec_rect.height += sec_rect.y;
                        sec_rect.y = 0;
                    }
                    m_wndSecondary->SetSize( sec_rect );
                #endif

                    SetupEventHandling(m_wndSecondary,wxPG_SUBID2);

                    // If no primary editor, focus to button to allow
                    // it to interprete ENTER etc.
                    if ( (flags & wxPG_SEL_FOCUS) && !m_wndPrimary )
                        m_wndSecondary->SetFocus();

                }

                if ( flags & wxPG_SEL_FOCUS )
                    m_editorFocused = 1;

            }
            else
            {
                // wxGTK atleast seems to need this (wxMSW not)
                SetFocus();
            }

            m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);

            //Update();

            // If it's inside collapsed section, expand parent, scroll, etc.
            // Also, if it was partially visible, scroll it into view.
            int vx, vy;
            GetViewStart(&vx,&vy);
            vy*=wxPG_PIXELS_PER_UNIT;
            int vy2 = vy + m_height;

            if ( (p->m_y < vy ||
                  (p->m_y <= vy2 &&
                   (p->m_y+m_lineHeight) > vy2)) &&
                 !(flags & wxPG_SEL_NONVISIBLE) )
                EnsureVisible ( wxPGIdGen(p) );

            if ( m_wndPrimary )
            {
                // Clear its background
                // (why can't this be optimized by some other drawing?)
                if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) )
                {
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) );
                    dc.DrawRectangle(m_splitterx+1,p->m_y,
                                     m_width-m_splitterx,m_lineHeight-1);
                }

            #if wxPG_CREATE_CONTROLS_HIDDEN
                m_wndPrimary->Thaw();
                m_wndPrimary->Show(true);
            #endif
            }

            DoDrawItems ( dc, p, p, (const wxRect*) NULL );

        }
    }

#if wxUSE_STATUSBAR

    //
    // Show help text in status bar
    //   (if found and grid not embedded in manager with help box).
    //

    wxStatusBar* statusbar = (wxStatusBar*) NULL;
    if ( !(m_iFlags & wxPG_FL_NOSTATUSBARHELP) )
    {
        wxFrame* frame = wxDynamicCast(::wxGetTopLevelParent(this),wxFrame);
        if ( frame )
            statusbar = frame->GetStatusBar();
    }

    if ( statusbar )
    {
        const wxString* pHelpString = (const wxString*) NULL;

        if ( p && p->m_dataExt )
        {
            pHelpString = &p->m_dataExt->m_helpString;
            if ( pHelpString->length() )
            {
                // Set help box text.
                statusbar->SetStatusText ( *pHelpString );
                m_iFlags |= wxPG_FL_STRING_IN_STATUSBAR;
            }
        }

        if ( (!pHelpString || !pHelpString->length()) &&
             (m_iFlags & wxPG_FL_STRING_IN_STATUSBAR) )
        {
            // Clear help box - but only if it was written
            // by us at previous time.
            statusbar->SetStatusText ( wxEmptyString );
            m_iFlags &= ~(wxPG_FL_STRING_IN_STATUSBAR);
        }
    }
#endif

    // call wx event handler (here so that deselecting gets processed as well)
    wxPropertyGridEvent evt( wxEVT_PG_SELECTED, GetId() );
    evt.SetPropertyGrid (this);
    evt.SetEventObject (this);
    evt.SetProperty (m_selected);
    GetEventHandler()->AddPendingEvent(evt);

    m_iFlags &= ~(wxPG_FL_IN_SELECT_PROPERTY);

    return true;
}

// -----------------------------------------------------------------------

// This method is not inline because it called dozens of times
// (i.e. two-arg function calls create smaller code size).
bool wxPropertyGrid::ClearSelection()
{
    return DoSelectProperty((wxPGProperty*)NULL);
}

// -----------------------------------------------------------------------
// wxPropertyGrid expand/collapse state and priority (compact mode) related
// -----------------------------------------------------------------------

bool wxPropertyGrid::_Collapse( wxPGProperty* p, bool sendEvents )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return false;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return false;

    if ( !pwc->m_expanded ) return false;

    // If active editor was inside collapsed section, then disable it
    if ( m_selected && m_selected->IsSomeParent (p) )
    {
        if ( !ClearSelection() )
            return false;
    }

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    wxUint32 old_flag = m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER;
    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 0;

    // Redraw etc. only if collapsed was visible.
    if (pwc->m_y >= 0 &&
        !m_frozen &&
        ( pwc->GetParentingType() != 1 || !(m_windowStyle & wxPG_HIDE_CATEGORIES) ) )
    {
        /*int y_adjust = 0;

        if ( m_selected && m_selected->m_y > pwc->m_y )
        {
            wxPGProperty* next_vis = GetNeighbourItem(pwc,true,1);
            wxASSERT ( next_vis );

            y_adjust = next_vis->m_y - pwc->m_y - m_lineHeight;
        }*/

        CalculateYs( pwc->m_parent, pwc->m_arrIndex );

        // Fix control position.
        /*if ( y_adjust )
            CorrectEditorWidgetSizeY ( -y_adjust );*/

        // When item is collapsed so that scrollbar would move,
        // graphics mess is about (unless we redraw everything).
        Refresh();
    }

    // Clear dont-center-splitter flag if it wasn't set
    m_iFlags = m_iFlags & ~(wxPG_FL_DONT_CENTER_SPLITTER) | old_flag;

    if ( sendEvents )
    {
        wxPropertyGridEvent evt( wxEVT_PG_ITEM_COLLAPSED, GetId() );
        evt.SetPropertyGrid (this);
        evt.SetEventObject (this);
        evt.SetProperty (p);
        GetEventHandler()->AddPendingEvent(evt);
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::_Expand( wxPGProperty* p, bool sendEvents )
{
    wxASSERT_MSG( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return false;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return false;

    if ( pwc->m_expanded ) return false;

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    wxUint32 old_flag = m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER;
    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 1;

    // Redraw etc. only if expanded was visible.
    if ( pwc->m_y >= 0 && !m_frozen &&
         ( pwc->GetParentingType() != 1 || !(m_windowStyle & wxPG_HIDE_CATEGORIES) )
       )
    {
        CalculateYs( pwc->m_parent, pwc->m_arrIndex );

        /*int y_adjust = pwc->GetCount()*m_lineHeight;

        // Fix widget position as well
        if ( m_selected && m_selected->m_y > pwc->m_y )
            CorrectEditorWidgetSizeY ( y_adjust );*/

        // Redraw
    #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        Refresh();
    #else
        //wxPG_CLIENT_DC_INIT_R(true)
        //DrawItems( dc, pwc->m_y, m_bottomy );
        DrawItems(pwc,(wxPGProperty*) NULL);
    #endif
    }

    // Clear dont-center-splitter flag if it wasn't set
    m_iFlags = m_iFlags & ~(wxPG_FL_DONT_CENTER_SPLITTER) | old_flag;

    if ( sendEvents )
    {
        wxPropertyGridEvent evt( wxEVT_PG_ITEM_EXPANDED, GetId() );
        evt.SetPropertyGrid (this);
        evt.SetEventObject (this);
        evt.SetProperty (p);
        GetEventHandler()->AddPendingEvent(evt);
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Compact( bool compact )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::Compact()") );
#endif
    if ( compact )
    {
        if ( !(m_iFlags & wxPG_FL_HIDE_STATE) )
        {
            // Deselect selected if it was hideable
            if ( m_selected && ( m_selected->m_flags & wxPG_PROP_HIDEABLE ) )
            {
                if ( !ClearSelection() )
                    return false;
            }

            m_iFlags |= wxPG_FL_HIDE_STATE;

            if ( !m_frozen )
            {
                CalculateYs( NULL, -1 );
                RedrawAllVisible();
            }
        }
    }
    else
    {
        if ( m_iFlags & wxPG_FL_HIDE_STATE )
        {

            m_iFlags &= ~(wxPG_FL_HIDE_STATE);

            if ( !m_frozen )
            {
                CalculateYs( NULL, -1 );
                RedrawAllVisible();
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------

// Used by HideProperty as well
bool wxPropertyGrid::SetPropertyPriority( wxPGProperty* p, int priority )
{
    if ( m_frozen )
        return m_pState->SetPropertyPriority(p,priority);

    if ( (m_iFlags & wxPG_FL_HIDE_STATE) && m_selected &&
         ( m_selected == p || m_selected->IsSomeParent(p) )
       )
        {
            if ( !ClearSelection() )
                return false;
        }

    m_pState->SetPropertyPriority(p,priority);

    if ( m_iFlags & wxPG_FL_HIDE_STATE )
    {
        CalculateYs(NULL,-1);
        RedrawAllVisible();
    }

    return true;
}


// -----------------------------------------------------------------------
// wxPropertyGrid size related methods
// -----------------------------------------------------------------------

// This is called by CalculateYs (so those calling it won't need to call this)
void wxPropertyGrid::RecalculateVirtualSize()
{

    int x = m_width;
    int y = m_bottomy;

    //SetClientSize(x,y);

    // Now adjust virtual size.
	SetVirtualSize(x, y);

	PGAdjustScrollbars(y);

    //
    // FIXME: Is this really needed? I mean, can't OnResize handle this?
    int width, height;
    GetClientSize(&width,&height);

    if ( m_selected && width != m_width )
    {
        CorrectEditorWidgetSizeX( m_splitterx, width );
    }

    m_width = width;
    m_height = height;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::PGAdjustScrollbars( int y )
{
    // Adjust scrollbars.

	y += wxPG_PIXELS_PER_UNIT+2; // One more scrollbar unit + 2 pixels.
    int y_amount = y/wxPG_PIXELS_PER_UNIT;

    int y_pos = GetScrollPos( wxVERTICAL );
    SetScrollbars( 0, wxPG_PIXELS_PER_UNIT, 0,
		           y_amount, 0, y_pos, true );
}

// -----------------------------------------------------------------------

/*
bool wxPropertyGrid::DetectScrollbar()
{
    // Call at every time scrollbar may have appeared/disappeared
    // Returns true if scrollbar was toggled

    bool toggled = false;

    // Use functions instead of m_width for total independence
    wxCoord width = GetSize().x;
    wxCoord cwidth = GetClientSize().x;

    if ( abs(width-cwidth) >= wxPG_MIN_SCROLLBAR_WIDTH )
    {
        // There is a scrollbar.
        if ( !(m_iFlags & wxPG_FL_SCROLLBAR_DETECTED) )
        {
            //wxLogDebug(wxT("Scrollbar Appeared"));
            toggled = true;
            m_iFlags |= wxPG_FL_SCROLLBAR_DETECTED;
        }
    }
    else if ( m_iFlags & wxPG_FL_SCROLLBAR_DETECTED )
    {
        //wxLogDebug(wxT("Scrollbar Disappeared"));
        toggled = true;
        m_iFlags &= ~(wxPG_FL_SCROLLBAR_DETECTED);
    }

    return toggled;
}
*/

void wxPropertyGrid::OnResize( wxSizeEvent& event )
{

    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

    if ( FROM_STATE(m_itemsAdded) && !m_frozen )
        PrepareAfterItemsAdded();

    int width, height;
    GetClientSize(&width,&height);

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("wxPropertyGrid::OnResize ( %i, %i )"),width,height);
#endif

    //int old_width = m_width;
    //int old_height = m_height;
    int old_fwidth = m_fWidth; // non-client width
    int old_splitterx = m_splitterx;

    int fwidth = event.GetSize().x;
    m_fWidth = fwidth;
    m_width = width;
    m_height = height;
    //bool sb_really_toggled = false;
    //bool sb_vis_toggled = false;

    //sb_really_toggled = DetectScrollbar();

    int widthDiff = fwidth - old_fwidth;

    // Determine if scrollbar appeared or disappeared.
    /*if ( width > 150 && old_width > 150 &&
        height > 120 && old_height > 120 )*/
    /*{
        sb_vis_toggled = sb_really_toggled;
    }*/

#if wxPG_DOUBLE_BUFFER
    int dblh = (m_lineHeight*2);
    if ( !m_doubleBuffer )
    {
        // Create double buffer bitmap to draw on, if none
        int w = (width>250)?width:250;
        int h = height + dblh;
        h = (h>400)?h:400;
        m_doubleBuffer = new wxBitmap ( w, h );
    }
    else
    {
        int w = m_doubleBuffer->GetWidth();
        int h = m_doubleBuffer->GetHeight();

        // Double buffer must be large enough
        if ( w < width || h < (height+dblh) )
        {
            if ( w < width ) w = width;
            if ( h < (height+dblh) ) h = height + dblh;
            delete m_doubleBuffer;
            m_doubleBuffer = new wxBitmap ( w, h );
        }
    }

    // Consider full update on every resize
    //m_iFlags |= wxPG_FL_CHANGED;

#endif

    //
    // Center splitter when...
    // * always when propgrid not shown yet or its full size is not realized yet
    //   and then only if splitter's position was not pre-set
    // * auto-centering is enabled and scrollbar was not toggled
    //

    // Need to center splitter?
    //if ( width!=old_width )
    {
        bool needSplitterCheck = true;

        //if ( !sb_vis_toggled )
        {
            if ( m_windowStyle & wxPG_SPLITTER_AUTO_CENTER )
            {
                float centerX = float(width) * 0.5;

                float splitterX = m_fSplitterX + (float(widthDiff) * 0.5);

                float deviation = fabs(centerX - splitterX);
                //wxLogDebug(wxT("deviation: %.1f"),deviation);

                // If deviated too far from the center, reset it
                if ( deviation > 30.0 )
                    splitterX = centerX;

                DoSetSplitterPosition( (int)splitterX, false );

                m_fSplitterX = splitterX; // needed to retain accuracy

                needSplitterCheck = false;
            }
            else if ( !(m_iFlags & wxPG_FL_SPLITTER_PRE_SET) )
            {
                long timeSinceCreation = (::wxGetLocalTimeMillis() - m_timeCreated).ToLong();

                if ( m_pState->m_properties->GetCount() || timeSinceCreation > 750 )
                {
                    SetSplitterLeft( false );
                    needSplitterCheck = false;
                }
                else
                {
                    DoSetSplitterPosition( width / 2, false );
                    m_iFlags &= ~(wxPG_FL_SPLITTER_PRE_SET);
                    needSplitterCheck = false;
                }
            }
        }

        if ( needSplitterCheck && (m_splitterx + wxPG_DRAG_MARGIN) > width )
        {
            long timeSinceCreation = (::wxGetLocalTimeMillis() - m_timeCreated).ToLong();

            if ( timeSinceCreation >= 750 )
            {
                DoSetSplitterPosition( width - wxPG_DRAG_MARGIN - 1, false );
            }
        }
    }

    // Need to correct widget position?
    if ( m_selected /*&& (width != old_width || sb_vis_toggled)*/ )
    {
        // Take splitter position change into account
        CorrectEditorWidgetSizeX( m_splitterx, width );
    }

    if ( !m_frozen )
    {

        // Need to recalculate visibles array?
        //if ( height != old_height )
        if ( height > m_calcVisHeight )
            CalculateVisibles( -1, false );

        /*if ( sb_vis_toggled )
        {
            Refresh();
        }
        else*/
        if ( m_splitterx != old_splitterx )
        {
            Refresh();
            /*if ( abs(height-old_height) < 100 )
            {
                Update(); // Necessary, atleast on wxMSW
                RedrawAllVisible();
            }
            else
            {
                Refresh();
            }*/
        }
    }

    // Without this, virtual size (atleast under wxGTK) will be skewed
    RecalculateVirtualSize();

}

// -----------------------------------------------------------------------
// wxPropertyGrid mouse event handling
// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseClick( int x, unsigned int y, wxMouseEvent &event )
{
    bool res = true;

#if __MOUSE_DEBUGGING__
    wxLogDebug ( wxT("  \\--> HandleMouseClick") );
#endif

    // Need to set focus?
    if ( !(m_iFlags & wxPG_FL_FOCUSED) )
    {
        //m_parent->SetFocus (this);
        SetFocus();
        //OnSetFocus ( *((wxFocusEvent*)NULL) );
    }

    if ( y < m_bottomy )
    {

        wxPGProperty* p = DoGetItemAtY(y);

        if ( p )
        {
            int parenting = p->GetParentingType();
            int margin_ends = m_marginwidth + ( ( (int)p->GetDepth()-1 ) * m_subgroup_extramargin );
            if ( x >= margin_ends )
            {
                // Outside margin.

                if ( parenting > 0 )
                {
                    // This is category.
                    wxPropertyCategoryClass* pwc = (wxPropertyCategoryClass*)p;

                    int text_x = m_marginwidth + ((unsigned int)((pwc->m_depth-1)*m_subgroup_extramargin));

                    // Expand, collapse, activate etc. if click on text or left of splitter.
                    if ( x >= text_x
                         &&
                         ( x < (text_x+pwc->GetTextExtent()+(wxPG_CAPRECTXMARGIN*2))
                           ||
                           x < m_splitterx
                         )
                        )
                    {
                        if ( !DoSelectProperty( p ) )
                            return res;

                        // On double-click, expand/collapse.
                        if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                        {
                            if ( pwc->m_expanded ) _Collapse ( p, true );
                            else _Expand ( p, true );
                        }
                    }
                }
                else if ( x > (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
                          x < (m_splitterx - wxPG_SPLITTERX_DETECTMARGIN1) )
                {
                // Click on value.
                    unsigned int selFlag = 0;
                    if ( x > m_splitterx )
                    {
                        m_iFlags |= wxPG_FL_ACTIVATION_BY_CLICK;
                        selFlag = wxPG_SEL_FOCUS;
                    }
                    if ( !DoSelectProperty( p, selFlag ) )
                        return res;

                    m_iFlags &= ~(wxPG_FL_ACTIVATION_BY_CLICK);

                    if ( p->GetParentingType() < 0 )
                        // On double-click, expand/collapse.
                        if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                        {
                            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
                            if ( pwc->m_expanded ) _Collapse ( p, true );
                            else _Expand ( p, true );
                        }

                    res = false;
                }
                else
                {
                // click on splitter
                    if ( !(m_windowStyle & wxPG_STATIC_SPLITTER) )
                    {

                        if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
                        {
                            // Double-clicking the splitter causes auto-centering
                            CenterSplitter ( true );
                            // TODO: Would this be more natural?
                            //   .NET grid doesn't do it but maybe we should.
                            //CustomSetCursor ( wxCURSOR_ARROW );
                        }
                        else if ( m_dragStatus == 0 )
                        {
                        //
                        // Begin draggin the splitter
                        //
                        #if __MOUSE_DEBUGGING__
                            wxLogDebug ( wxT("       dragging begins at splitter + %i"),
                                (int)(x - m_splitterx) );
                        #endif

                            if ( m_wndPrimary )
                            {
                                // Changes must be committed here or the
                                // value won't be drawn correctly
                                if ( !CommitChangesFromEditor() )
                                    return res;

                                m_wndPrimary->Show ( false );
                            }

                            BEGIN_MOUSE_CAPTURE

                            m_dragStatus = 1;

                            m_dragOffset = x - m_splitterx;

                            wxPG_CLIENT_DC_INIT()

                        #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
                            // Fixes button disappearance bug
                            if ( m_wndSecondary )
                                m_wndSecondary->Show ( false );
                        #endif

                            m_startingSplitterX = m_splitterx;
                        #if wxPG_HEAVY_GFX
                        #else
                            Update(); // clear graphics mess
                            DrawSplitterDragColumn( dc, m_splitterx );
                            m_splitterprevdrawnx = m_splitterx;
                        #endif

                        }
                    }
                }
            }
            else
            {
            // Click on margin.
                if ( parenting != 0 )
                {
                    int nx = x + m_marginwidth - margin_ends; // Normalize x.

                    if ( (nx >= m_gutterwidth && nx < (m_gutterwidth+m_iconwidth)) )
                    {
                        int y2 = y - p->m_y;
                        if ( (y2 >= m_buttonSpacingY && y2 < (m_buttonSpacingY+m_iconheight)) )
                        {
                            // On click on expander button, expand/collapse
                            if ( ((wxPGPropertyWithChildren*)p)->m_expanded )
                                _Collapse ( p, true );
                            else
                                _Expand ( p, true );
                        }
                    }
                }
            }
        }
    }
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseRightClick( int WXUNUSED(x), unsigned int y,
                                            wxMouseEvent& WXUNUSED(event) )
{
    if ( y < m_bottomy )
    {
        // Select property here as well
        wxPGProperty* p = m_propHover;
        if ( p != m_selected )
            DoSelectProperty( p );

        // Send right click event.
        wxPropertyGridEvent evt( wxEVT_PG_RIGHT_CLICK, GetId() );
        evt.SetPropertyGrid (this);
        evt.SetEventObject (this);
        evt.SetProperty (p);
        GetEventHandler()->AddPendingEvent(evt);
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

/*
// Splits text into lines so that each will have width less than arg maxWidth.
// * Returns string with line breaks inserted into appropriate positions.
// * Keeps words together.
// * Useful in conjunction with wxWindow::SetToolTip and wxDC::DrawLabel.
static wxString SplitTextByPixelWidth(wxDC& dc, const wxString& text, int lineWidth)
{
    if ( !text.length() )
        return text;

    wxString resultLine;
    wxArrayInt extents;

    unsigned int index = 0;
    unsigned int maxIndex = text.length() - 1;
    unsigned int prevSplitIndex = 0;
    unsigned int prevCanSplitIndex = 0;
    int lineCheckWidth = lineWidth;
    wxChar prevA = wxT('\0');

    dc.GetPartialTextExtents(text,extents);

    wxASSERT( text.length() == extents.GetCount() );

    while ( index <= maxIndex )
    {
        const wxChar A = text[index];

        if ( !wxIsalnum(prevA) )
        {
            // Can split here
            prevCanSplitIndex = index;
        }
        else
        {
            // Can't split here
        }

        if ( ( (extents[index] >= lineCheckWidth || A == wxT('\n')) &&
               index > prevCanSplitIndex ) ||
             index == maxIndex )
        {
            // Need to split now

            unsigned int useSplit = prevCanSplitIndex;
            if ( useSplit <= prevSplitIndex ||
                 index >= maxIndex )
                useSplit = index;

            resultLine << text.Mid(prevSplitIndex,useSplit-prevSplitIndex);

            if ( index >= maxIndex )
                break;
            else
            if ( A != wxT('\n') )
            {
                resultLine.Append(_T("\n"));
                //resultLine.Append(text.Mid(useSplit,text.length()-useSplit));
                //break;
            }
            prevSplitIndex = useSplit;
            lineCheckWidth = extents[useSplit] + lineWidth;
            //widSum = 0;
            index = useSplit;
            prevA = wxT('\0');
        }
        else
        {
            index++;
            prevA = A;
        }
    }

    return resultLine;
}
*/
// -----------------------------------------------------------------------

#if wxPG_SUPPORT_TOOLTIPS

void wxPropertyGrid::SetToolTip( const wxString& tipString )
{
    if ( tipString.length() )
    {
        //wxClientDC dc(this);
        //wxString finalString = SplitTextByPixelWidth(dc,tipString,350);
        //wxScrolledWindow::SetToolTip(finalString);
        wxScrolledWindow::SetToolTip(tipString);
    }
    else
    {
    #if wxPG_ALLOW_EMPTY_TOOLTIPS
        wxScrolledWindow::SetToolTip( wxEmptyString );
    #else
        wxScrolledWindow::SetToolTip( NULL );
    #endif
    }
}

#endif // #if wxPG_SUPPORT_TOOLTIPS

// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseMove( int x, unsigned int y, wxMouseEvent &event )
{
    // Safety check (needed because mouse capturing may
    // otherwise freeze the control)
    if ( m_dragStatus > 0 && !event.Dragging() )
    {
        //wxLogDebug(wxT("MOUSE CAPTURE SAFETY RELEASE TRIGGERED"));
        HandleMouseUp(x,y,event);
    }

    if ( m_dragStatus > 0 )
    {

        if ( x > (m_marginwidth + wxPG_DRAG_MARGIN) &&
             x < (m_width - wxPG_DRAG_MARGIN) )
        {

        #if wxPG_HEAVY_GFX

            int new_splitterx = x - m_dragOffset;

            // Splitter redraw required?
            if ( new_splitterx != m_splitterx )
            {

                if ( m_selected )
                    CorrectEditorWidgetSizeX( new_splitterx, m_width );

                // Move everything
                m_splitterx = new_splitterx;
                m_fSplitterX = (float) new_splitterx;

                Update();
                RedrawAllVisible();

            }

        #else

            if ( x != m_splitterx )
            {
                wxPG_CLIENT_DC_INIT_R(false)

                if ( m_splitterprevdrawnx != -1 )
                    DrawSplitterDragColumn( dc, m_splitterprevdrawnx );

                m_splitterx = x;
                m_fSplitterX = (float) x;

                DrawSplitterDragColumn( dc, x );

                m_splitterprevdrawnx = x;
            }

        #endif

            m_dragStatus = 2;

        }

        return false;
    }
    else
    {

        int ih = m_lineHeight;
        int sy = y;

    #if wxPG_SUPPORT_TOOLTIPS
        wxPGProperty* prev_hover = m_propHover;
        unsigned char prev_side = m_mouseSide;
    #endif

        // On which item it hovers
        if ( ( !m_propHover && y < m_bottomy)
             ||
             ( m_propHover && ( sy < m_propHover->m_y || sy >= (m_propHover->m_y+ih) ) )
           )
        {
            // Mouse moves on another property

            m_propHover = DoGetItemAtY(y);

            // Send hover event
            wxPropertyGridEvent evt( wxEVT_PG_HIGHLIGHTED, GetId() );
            evt.SetPropertyGrid (this);
            evt.SetEventObject (this);
            evt.SetProperty (m_propHover);
            GetEventHandler()->AddPendingEvent(evt);

        }

    #if wxPG_SUPPORT_TOOLTIPS
        // Store which side we are on
        m_mouseSide = 0;
        if ( x >= m_splitterx )
            m_mouseSide = 2;
        else if ( x >= m_marginwidth )
            m_mouseSide = 1;

        //
        // If tooltips are enabled, show label or value as a tip
        // in case it doesn't otherwise show in full length.
        //
        if ( m_windowStyle & wxPG_TOOLTIPS )
        {
            wxToolTip* tooltip = GetToolTip();

            if ( m_propHover != prev_hover || prev_side != m_mouseSide )
            {
                if ( m_propHover && m_propHover->GetParentingType() <= 0 )
                {

                    if ( GetExtraStyle() & wxPG_EX_HELP_AS_TOOLTIPS )
                    {
                        // Show help string as a tooltip
                        wxString tipString = m_propHover->GetHelpString();

                        SetToolTip(tipString);
                    }
                    else
                    {
                        // Show cropped value string as a tooltip
                        wxString tipString;
                        int space = 0;

                        if ( m_mouseSide == 1 )
                        {
                            tipString = m_propHover->m_label;
                            space = m_splitterx-m_marginwidth-3;
                        }
                        else if ( m_mouseSide == 2 )
                        {
                            tipString = m_propHover->GetDisplayedString();

                            space = m_width - m_splitterx;
                            if ( m_propHover->m_flags & wxPG_PROP_CUSTOMIMAGE )
                                space -= wxPG_CUSTOM_IMAGE_WIDTH + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                        }

                        if ( space )
                        {
                            int tw, th;
    	                    GetTextExtent( tipString, &tw, &th, 0, 0, &m_font );
                            if ( tw > space )
                            {
                                SetToolTip( tipString );
                            }
                        }
                        else
                        {
                            if ( tooltip )
                            {
                            #if wxPG_ALLOW_EMPTY_TOOLTIPS
                                wxScrolledWindow::SetToolTip( wxEmptyString );
                            #else
                                wxScrolledWindow::SetToolTip( NULL );
                            #endif
                            }
                        }

                    }
                }
                else
                {
                    if ( tooltip )
                    {
                    #if wxPG_ALLOW_EMPTY_TOOLTIPS
                        wxScrolledWindow::SetToolTip( wxEmptyString );
                    #else
                        wxScrolledWindow::SetToolTip( NULL );
                    #endif
                    }
                }
            }
        }
    #endif

        if ( x > (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
             x < (m_splitterx - wxPG_SPLITTERX_DETECTMARGIN1) ||
             y >= m_bottomy ||
             (m_windowStyle & wxPG_STATIC_SPLITTER) )
        {
            // hovering on something else
            if ( m_curcursor != wxCURSOR_ARROW )
                CustomSetCursor( wxCURSOR_ARROW );
        }
        else
        {
            // Do not allow splitter cursor on caption items.
            // (also not if we were dragging and its started
            // outside the splitter region)

            if ( m_propHover &&
                 m_propHover->GetParentingType() <= 0 &&
                 !event.Dragging() )
            {

                // hovering on splitter

                // NB: Condition disabled since MouseLeave event (from the editor control) cannot be
                //     reliably detected.
                //if ( m_curcursor != wxCURSOR_SIZEWE )
                CustomSetCursor( wxCURSOR_SIZEWE, true );

                return false;
            }
            else
            {
                // hovering on something else
                if ( m_curcursor != wxCURSOR_ARROW )
                    CustomSetCursor( wxCURSOR_ARROW );
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------

// Also handles Leaving event
bool wxPropertyGrid::HandleMouseUp( int x, unsigned int y, wxMouseEvent &WXUNUSED(event) )
{
    bool res = false;

#if __MOUSE_DEBUGGING__
    wxLogDebug ( wxT("  \\--> HandleMouseUp") );
#endif

    // No event type check - basicly calling this method should
    // just stop dragging.
    //if( event.LeftUp() || event.Leaving() )
	//{
        // Left up after dragged?
        if ( m_dragStatus >= 1 )
        {
        //
        // End Splitter Dragging
        //
        #if __MOUSE_DEBUGGING__
            wxLogDebug ( wxT("       dragging ends") );
        #endif

            // DO NOT ENABLE FOLLOWING LINE!
            // (it is only here as a reminder to not to do it)
            //m_splitterx = x;

        #if wxPG_HEAVY_GFX
            //Refresh();
        #else
            DoSetSplitterPosition( -1 ); // -1 tells not to make change

            // Hack to clear-up editor graphics mess (on wxMSW, atleast)
            if ( m_selected )
                DrawItem ( m_selected );

        #endif
            // Disable splitter auto-centering
            m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

            // This is necessary to return cursor
            END_MOUSE_CAPTURE

            // Set back the default cursor, if necessary
            if ( x > (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
                 x < (m_splitterx - wxPG_SPLITTERX_DETECTMARGIN1) ||
                 y >= m_bottomy )
            {
                CustomSetCursor( wxCURSOR_ARROW );
            }

            m_dragStatus = 0;

            #if wxPG_HEAVY_GFX
            // Control background needs to be cleared
            if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) && m_selected )
                DrawItem ( m_selected );
            #endif

            if ( m_wndPrimary )
            {
                m_wndPrimary->Show ( true );
            }

        #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
            // Fixes button disappearance bug
            if ( m_wndSecondary )
                m_wndSecondary->Show ( true );
        #endif

            // This clears the focus.
            m_editorFocused = 0;

        }
    //}
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::OnMouseCommon( wxMouseEvent& event, int* px, int* py )
{
    int ux, uy;
    CalcUnscrolledPosition( event.m_x, event.m_y, &ux, &uy );

    // Hide popup on clicks
    // FIXME: Not necessary after transient window implemented
    if ( event.GetEventType() != wxEVT_MOTION )
        if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxPGOwnerDrawnComboBox)) )
        {
            ((wxPGOwnerDrawnComboBox*)m_wndPrimary)->HidePopup();
        }

    //if (printmsg) wxLogDebug ( wxT("On") wxT(#func) wxT("( %i, %i )"),(int)ux,(int)uy );
    wxRect r;
    wxWindow* wnd = m_wndPrimary;
    if ( wnd )
        r = wnd->GetRect();
    if ( wnd == (wxWindow*) NULL || m_dragStatus ||
         (
           ux <= (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
           event.m_y < r.y ||
           event.m_y >= (r.y+r.height)
         )
       )
    {
        *px = ux;
        *py = uy;
        return true;
    }
    else
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseClick( wxMouseEvent &event )
{
    //HANDLE_ON_MOUSE_EV(MouseClick,OMPMR)

    /*if ( event.GetEventType() != wxEVT_LEFT_DOWN )
    {
        wxLogDebug(wxT("%i"),(int)event.GetEventType());
        event.Skip();
    }*/

    int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseClick(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseRightClick( wxMouseEvent &event )
{
    /*int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseRightClick(x,y,event);
    }
    event.Skip();*/
    int x, y;
    CalcUnscrolledPosition( event.m_x, event.m_y, &x, &y );
    HandleMouseRightClick(x,y,event);
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseMove( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseMove(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseUp( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseUp(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseEntry( wxMouseEvent &event )
{
    // This may get called from child control as well, so event's
    // mouse position cannot be relied on.
    //int x = event.m_x;
    //int y = event.m_y;

    if ( event.Entering() )
    {
        if ( !(m_iFlags & wxPG_FL_MOUSE_INSIDE) )
        {
        #if __MOUSE_DEBUGGING__
            wxLogDebug (wxT("Mouse Enters Window"));
        #endif
            //SetCursor ( *wxSTANDARD_CURSOR );
            // TODO: Fix this (detect parent and only do
            //   cursor trick if it is a manager).
            wxASSERT ( GetParent() );
            GetParent()->SetCursor(wxNullCursor);

            m_iFlags |= wxPG_FL_MOUSE_INSIDE;
            //if ( m_wndPrimary ) m_wndPrimary->Show ( true );
        }
        else
            GetParent()->SetCursor(wxNullCursor);
    }
    else if ( event.Leaving() )
    {
        // Without this, wxSpinCtrl editor will sometimes have wrong cursor
        SetCursor( wxNullCursor );

        // Get real cursor position
        wxPoint pt = ScreenToClient(::wxGetMousePosition());

        if ( ( pt.x <= 0 || pt.y <= 0 || pt.x >= m_width || pt.y >= m_height ) )
        {
            if ( CommitChangesFromEditor() )
            {

                if ( (m_iFlags & wxPG_FL_MOUSE_INSIDE) )
                {
                #if __MOUSE_DEBUGGING__
                    wxLogDebug (wxT("Mouse Leaves Window"));
                #endif
                    m_iFlags &= ~(wxPG_FL_MOUSE_INSIDE);
                    //if ( m_wndPrimary ) m_wndPrimary->Show ( false );
                }

                if ( m_dragStatus )
                    wxPropertyGrid::HandleMouseUp ( -1, 10000, event );

            }
        }
        else
        {
        /*#if wxPG_NO_CHILD_EVT_MOTION
            // cursor must be reset because EVT_MOTION handler is not there to do it
            if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
        #endif*/
        }
    }

    event.Skip();
}

// -----------------------------------------------------------------------

//    if (printmsg) wxLogDebug ( wxT("On") wxT(#func) wxT("Child ( %i, %i )"),(int)event.m_x,(int)event.m_y );

// Common code used by various OnMouseXXXChild methods.
bool wxPropertyGrid::OnMouseChildCommon( wxMouseEvent &event, int* px, int *py )
{
    wxWindow* top_ctrl_wnd = (wxWindow*)event.GetEventObject();
    wxASSERT ( top_ctrl_wnd );
    int x, y;
    event.GetPosition(&x,&y);

#if wxPG_ENABLE_CLIPPER_WINDOW
    // Take clipper window into account
    if (top_ctrl_wnd->GetPosition().x < 1 &&
        !top_ctrl_wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)))
    {
        top_ctrl_wnd = top_ctrl_wnd->GetParent();
        wxASSERT( top_ctrl_wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)) );
        x -= ((wxPGClipperWindow*)top_ctrl_wnd)->GetXClip();
        y -= ((wxPGClipperWindow*)top_ctrl_wnd)->GetYClip();
    }
#endif

    wxRect r = top_ctrl_wnd->GetRect();
    if ( !m_dragStatus &&
         x > (m_splitterx-r.x+wxPG_SPLITTERX_DETECTMARGIN2) &&
         y >= 0 && y < r.height \
       )
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
        event.Skip();
    }
    else
    {
        CalcUnscrolledPosition( event.m_x + r.x, event.m_y + r.y, \
            px, py );
        return true;
    }
    return false;
}

/*void wxPropertyGrid::OnMouseEntryChild ( wxMouseEvent &event )
{
    wxLogDebug(wxT("Entering/Leaving Child..."));
    event.Skip();
}*/

void wxPropertyGrid::OnMouseClickChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseClick(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseRightClickChild( wxMouseEvent &event )
{
    int x,y;
    wxASSERT ( m_wndPrimary );
    // These coords may not be exact (about +-2),
    // but that should not matter (right click is about item, not position).
    wxPoint pt = m_wndPrimary->GetPosition();
    CalcUnscrolledPosition( event.m_x + pt.x, event.m_y + pt.y, &x, &y );
    wxASSERT ( m_selected );
    m_propHover = m_selected;
    bool res = HandleMouseRightClick(x,y,event);
    if ( !res ) event.Skip();
}

void wxPropertyGrid::OnMouseMoveChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseMove(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseUpChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseUp(x,y,event);
        if ( !res ) event.Skip();
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid keyboard event handling
// -----------------------------------------------------------------------

void wxPropertyGrid::SendNavigationKeyEvent( int dir )
{
    wxNavigationKeyEvent evt;
    evt.SetFlags(wxNavigationKeyEvent::FromTab|
                 (dir?wxNavigationKeyEvent::IsForward:
                      wxNavigationKeyEvent::IsBackward));
    evt.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(evt);
}

void wxPropertyGrid::HandleKeyEvent(wxKeyEvent &event)
{

    //
    // Handles key event when editor control is not focused.
    //

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::HandleKeyEvent(%i)"),(int)event.GetKeyCode() );
#endif

    wxASSERT ( !m_frozen );
    if ( m_frozen )
        return;

    // Travelsal between items, collapsing/expanding, etc.
    int keycode = event.GetKeyCode();

    if ( keycode == WXK_TAB )
    {
        SendNavigationKeyEvent( event.ShiftDown()?0:1 );
        return;
    }

    // Must update changes if special key was used
    // (because it may trigger closing of this app)
    if ( event.AltDown() ||
         event.ControlDown() ||
         event.MetaDown() )
    {
        CommitChangesFromEditor();
    }

    if ( m_selected )
    {
        wxPGProperty* p = m_selected;

        //bool focus_selected = false;
        int select_dir = -2;

        if ( p->GetParentingType() != 0 &&
             !(p->m_flags & wxPG_PROP_DISABLED)
           )
        {
            //wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
            if ( keycode == WXK_LEFT )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || Collapse ( p ) )
                    keycode = 0;
            }
            else if ( keycode == WXK_RIGHT )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || Expand ( p ) )
                    keycode = 0;
            }
        }

        if ( keycode )
        {
            if ( keycode == WXK_UP || keycode == WXK_LEFT )
            {
                select_dir = 0;
            }
            else if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
            {
                select_dir = 1;
            }
            else
            {
                event.Skip();
            }

        }

        if ( select_dir >= -1 )
        {
            p = GetNeighbourItem ( p, true, select_dir );
            if ( p )
                DoSelectProperty(p/*,focus_selected*/);
        }

    }
    else
    {
        // If nothing was selected, select the first item now
        // (or navigate out of tab).
        if ( keycode != WXK_ESCAPE )
        {
            wxPGProperty* p = GetFirst();
            if ( p ) DoSelectProperty(p);
        }
    }
}

// -----------------------------------------------------------------------

// Potentially handles a keyboard event for editor controls.
// Returns false if event should *not* be skipped (on true it can
// be optionally skipped).
// Basicly, false means that SelectProperty was called (or was about
// to be called, if canDestroy was false).
bool wxPropertyGrid::HandleChildKey ( wxKeyEvent& event, bool canDestroy )
{
    int keycode = event.GetKeyCode();
    bool res = true;

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::HandleChildKey(%i)"),(int)event.GetKeyCode() );
#endif

    wxASSERT ( m_wndPrimary );

    // Unfocus?
    if ( keycode == WXK_ESCAPE )
    {
        // Esc cancels any changes
        EditorsValueWasNotModified();

        wxPGProperty* p = m_selected;

        res = false;

        if ( canDestroy )
        {
            DoSelectProperty( (wxPGProperty*)NULL, wxPG_SEL_NOVALIDATE );
            DoSelectProperty( p );
        }

        //wxKeyEvent keyEvt(wxEVT_KEY_DOWN);
        //keyEvt.m_keyCode = WXK_ESCAPE;
        //keyEvt.SetEventObject(GetParent());
        //GetParent()->GetEventHandler()->AddPendingEvent(keyEvt);

    }
    else
    {
        // event.Skip();
    }

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKey ( wxKeyEvent &event )
{

    //
    // Events to editor controls should get relayed here.
    //

    wxWindow* focused = wxWindow::FindFocus();

    /*
    if ( focused )
        wxLogDebug(focused->GetClassInfo()->GetClassName());
    else
        wxLogDebug(wxT("<no focus>"));
    */

    if ( m_wndPrimary &&
         (focused==m_wndPrimary
          || m_editorFocused
    #if wxPG_ENABLE_CLIPPER_WINDOW
          || ((m_wndPrimary->IsKindOf(CLASSINFO(wxPGClipperWindow))) &&
              ((wxPGClipperWindow*)m_wndPrimary)->GetControl() == focused)
    #endif
          ) )
    {
        // Child key must be processed here, since it can
        // destroy the control which is referred by its own
        // event handling.
        HandleChildKey ( event, true );
    }
    else
        HandleKeyEvent ( event );

}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnNavigationKey( wxNavigationKeyEvent& event )
{

    // Ignore events that occur very close to focus set
    if ( m_iFlags & wxPG_FL_IGNORE_NEXT_NAVKEY )
    {
        m_iFlags &= ~(wxPG_FL_IGNORE_NEXT_NAVKEY);
        event.Skip();
        return;
    }

    wxPGProperty* next = (wxPGProperty*) NULL;

    int dir = event.GetDirection()?1:0;

    if ( m_selected )
    {
        if ( dir == 1 && (m_wndPrimary || m_wndSecondary) )
        {
            wxWindow* focused = wxWindow::FindFocus();
            //if ( focused ) wxLogDebug(wxT("  focused: %s"),focused->GetName().c_str());
            //else wxLogDebug(wxT("  focused: <NULL>"));

            wxWindow* wndToCheck = m_wndPrimary;

            // If no primary editor control, focus to button (if any)
            if ( !wndToCheck )
                wndToCheck = m_wndSecondary;

        #if wxPG_ENABLE_CLIPPER_WINDOW
            if ( wndToCheck->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
                wndToCheck = ((wxPGClipperWindow*)wndToCheck)->GetControl();
        #endif

            // Added by JACS
            // ODComboBox focus goes to its text ctrl, so we need to use it instead
            if ( m_wndPrimary->IsKindOf(CLASSINFO(wxPGOwnerDrawnComboBox)) )
            {
                wxTextCtrl* comboTextCtrl = ((wxPGOwnerDrawnComboBox*)wndToCheck)->GetTextCtrl();
                if ( comboTextCtrl )
                    wndToCheck = comboTextCtrl;
            }

            if ( focused != wndToCheck )
            {
                wndToCheck->SetFocus();

                // Select all text in wxTextCtrl etc.
                if ( m_wndPrimary )
                    m_selected->GetEditorClass()->OnFocus(m_selected,m_wndPrimary);

                m_editorFocused = 1;
                next = m_selected;
            }
        }

        if ( !next )
        {
            next = GetNeighbourItem(m_selected,true,dir);

            if ( next )
            {
                // This allows preventing NavigateOut to occur
                DoSelectProperty( next, wxPG_SEL_FOCUS );
            }
        }
    }

    if ( !next )
        event.Skip();

}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKeyChild( wxKeyEvent &event )
{
    // Must update changes if special key was used
    // (because it may trigger closing of this app)
    if ( event.AltDown() ||
         event.ControlDown() ||
         event.MetaDown() )
    {
        CommitChangesFromEditor();
    }

    // Since event handling may destroy the control which
    // triggered this event, we need to send it separately
    // to the wxPropertyGrid itself.
    if ( HandleChildKey(event,false) == false )
        AddPendingEvent( event );
    else
        event.Skip();

}

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous event handling
// -----------------------------------------------------------------------

void wxPropertyGrid::OnFocusEvent( wxFocusEvent& event )
{
    unsigned int oldFlags = m_iFlags;

    if ( event.GetEventType() == wxEVT_SET_FOCUS )
    {
        m_iFlags |= wxPG_FL_FOCUSED;
    }
    else
    {
        wxWindow* nextFocus = event.GetWindow();

        m_iFlags &= ~(wxPG_FL_FOCUSED);

        wxWindow* parent = nextFocus;

        // This must be one of nextFocus' parents.
        while ( parent )
        {
            if ( parent == this )
            {
                m_iFlags |= wxPG_FL_FOCUSED;
                break;
            }
            parent = parent->GetParent();
        }
    }

    if ( (m_iFlags & wxPG_FL_FOCUSED) !=
         (oldFlags & wxPG_FL_FOCUSED) )
    {
        // On each focus kill, mark the next nav key event
        // to be ignored (can't do on set focus since the
        // event would occur before it).
        if ( !(m_iFlags & wxPG_FL_FOCUSED) )
        {
            m_iFlags |= wxPG_FL_IGNORE_NEXT_NAVKEY;

            // Need to store changed value
            CommitChangesFromEditor();

        }
        else
        {
            /*
            //
            // Preliminary code for tab-order respecting
            // tab-traversal (but should be moved to
            // OnNav handler)
            //
            wxWindow* prevFocus = event.GetWindow();
            wxWindow* useThis = this;
            if ( m_iFlags & wxPG_FL_IN_MANAGER )
                useThis = GetParent();

            if ( prevFocus &&
                 prevFocus->GetParent() == useThis->GetParent() )
            {
                wxList& children = useThis->GetParent()->GetChildren();

                wxNode* node = children.Find(prevFocus);

                if ( node->GetNext() &&
                     useThis == node->GetNext()->GetData() )
                    DoSelectProperty(GetFirst());
                else if ( node->GetPrevious () &&
                          useThis == node->GetPrevious()->GetData() )
                    DoSelectProperty(GetLastProperty());

            }
            */

            m_iFlags &= ~(wxPG_FL_IGNORE_NEXT_NAVKEY);
        }

        // Redraw selected
        if ( m_selected && (m_iFlags & wxPG_FL_INITIALIZED) )
            DrawItem( m_selected );
    }

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnScrollEvent( wxScrollWinEvent &event )
{
    m_iFlags |= wxPG_FL_SCROLLED;

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnCaptureChange( wxMouseCaptureChangedEvent& WXUNUSED(event) )
{
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
    {
    #if __MOUSE_DEBUGGING__
        wxLogDebug ( wxT("wxPropertyGrid: mouse capture lost") );
    #endif
        m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED);
    }
}

// -----------------------------------------------------------------------
// Property text-based storage
// -----------------------------------------------------------------------

#define wxPG_PROPERTY_FLAGS_COUNT   8

// property-flag-to-text array
static const wxChar* gs_property_flag_to_string[wxPG_PROPERTY_FLAGS_COUNT] =
{
    wxT("Modified"),
    wxT("Disabled"),
    wxT("LowPriority"),
    (const wxChar*) NULL, // wxPG_PROP_CUSTOMIMAGE is auto-generated flag
    wxT("LimitedEditing"),
    wxT("Unspecified"),
    (const wxChar*) NULL, // Special flags cannot be stored as-is
    (const wxChar*) NULL  //
};


wxString wxPGProperty::GetAttributes( unsigned int flagmask )
{
    wxASSERT(this);

    wxString s;
    unsigned int i;
    unsigned int flags = ((unsigned int)m_flags) &
                         flagmask &
                         ~(wxPG_PROP_CUSTOMIMAGE |
                           wxPG_PROP_CLASS_SPECIFIC_1 |
                           wxPG_PROP_CLASS_SPECIFIC_2);

    if ( !flags )
        return wxEmptyString;

    for ( i=0; i<wxPG_PROPERTY_FLAGS_COUNT; i++ )
    {
        if ( flags & (1<<i) )
        {
            s.append( gs_property_flag_to_string[i] );
            flags &= ~(1<<i);
            if ( !flags )
                break;
            s.append(wxT(", "));
        }
    }

    return s;
}

// -----------------------------------------------------------------------

void wxPGProperty::SetAttributes( const wxString& attributes )
{
    wxASSERT(this);
    size_t i;

    WX_PG_TOKENIZER1_BEGIN(attributes,wxT(','))

        for (i=0;i<wxPG_PROPERTY_FLAGS_COUNT;i++)
        {
            const wxChar* flagText = gs_property_flag_to_string[i];
            if ( flagText && token == flagText )
            {
                m_flags |= ( 1<<i );
                break;
            }
        }

    WX_PG_TOKENIZER1_END()

}

// -----------------------------------------------------------------------

// Returns name of property without 'Property' at the end, and 'wx'
// in the beginning (if any).
wxString wxPropertyContainerMethods::GetPropertyShortClassName( wxPGId id )
{
    wxPGProperty* p = id;
    if ( p->GetParentingType() != 1 )
    {
        const wxChar* src = p->GetClassName();
        wxString s;
        if ( src[0] == wxT('w') && src[1] == wxT('x') )
            s = &src[2];
        else
            s = src;
        wxASSERT ( (((int)s.length())-8) > 0 );
        s.Truncate(s.length()-8);
        //s.LowerCase();
        return s;
    }
    return wxT("Category");
}

// -----------------------------------------------------------------------
// Value type related methods (should all be pretty much static).

wxPGValueType::~wxPGValueType()
{
}

const wxChar* wxPGValueType::GetCustomTypeName() const
{
    return GetTypeName();
}

// Implement default types.
WX_PG_IMPLEMENT_VALUE_TYPE(wxString,wxStringProperty,wxPGTypeName_wxString,GetString,wxEmptyString)
WX_PG_IMPLEMENT_VALUE_TYPE(long,wxIntProperty,wxPGTypeName_long,GetLong,(long)0)
WX_PG_IMPLEMENT_VALUE_TYPE(double,wxFloatProperty,wxPGTypeName_double,GetDouble,0.0)
WX_PG_IMPLEMENT_VALUE_TYPE(wxArrayString,wxArrayStringProperty,wxPGTypeName_wxArrayString,GetArrayString,wxArrayString())

// Bool is a special case... thanks to the C++'s bool vs int vs long inconsistency issues.
const wxPGValueType *wxPGValueType_bool = (wxPGValueType *) NULL;
class wxPGValueTypeboolClass : public wxPGValueType
{
public:
    virtual const wxChar* GetTypeName() const { return wxPGTypeName_long; }
    virtual const wxChar* GetCustomTypeName() const { return wxPGTypeName_bool; }
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant(0); }
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const
    { return wxVariant ( value.GetBool(), name ); }
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const
    {
        return wxPG_NEWPROPERTY(Bool,label,name,false);
    }
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const
    {
        wxASSERT_MSG( wxStrcmp(wxPGTypeName_bool,value.GetType().c_str()) == 0,
            wxT("SetValueFromVariant: wxVariant type mismatch.") );
        property->DoSetValue(value.GetBool()?1:0);
    }
};

// Implement nonetype.
const wxPGValueType *wxPGValueType_none = (wxPGValueType*) NULL;
class wxPGValueTypenoneClass : public wxPGValueType
{
public:
    virtual const wxChar* GetTypeName() const { return wxT("null"); }
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant((long)0); }
    virtual wxVariant GenerateVariant ( wxPGVariant, const wxString& name ) const
    { return wxVariant ( (long)0, name ); }
    virtual wxPGProperty* GenerateProperty ( const wxString&, const wxString& ) const
    { return (wxPGProperty*) NULL; }
    virtual void SetValueFromVariant ( wxPGProperty*, wxVariant& ) const
    { }
};

// Implement void* type.
const wxPGValueType *wxPGValueType_void = (wxPGValueType*) NULL;
class wxPGValueTypevoidClass : public wxPGValueType
{
public:
    virtual const wxChar* GetTypeName() const { return wxPGTypeName_void; }
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant((void*)NULL); }
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const
    { return wxVariant ( value.GetRawPtr(), name ); }
    virtual wxPGProperty* GenerateProperty ( const wxString&, const wxString& ) const
    { return (wxPGProperty*) NULL; }
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const
    {
        wxASSERT_MSG( wxStrcmp(GetTypeName(),value.GetType().c_str()) == 0,
            wxT("SetValueFromVariant: wxVariant type mismatch.") );
        property->DoSetValue(value.GetVoidPtr());
    }
};

// Registers all default value types
void wxPropertyGrid::RegisterDefaultValues()
{
    wxPGRegisterDefaultValueType( none );
    wxPGRegisterDefaultValueType( wxString );
    wxPGRegisterDefaultValueType( long );
    wxPGRegisterDefaultValueType( bool );
    wxPGRegisterDefaultValueType( double );
    wxPGRegisterDefaultValueType( void );
    wxPGRegisterDefaultValueType( wxArrayString );
}

// no_def_check = true prevents infinite recursion.
wxPGValueType* wxPropertyGrid::RegisterValueType ( wxPGValueType* valueclass, bool no_def_check )
{
    wxASSERT ( valueclass );

    WX_PG_GLOBALS_LOCKER()

    if ( !no_def_check && wxPGGlobalVars->m_dictValueType.empty() )
        RegisterDefaultValues();

    wxString temp_str;
    const wxChar* name = valueclass->GetType();

    wxPGValueType* p_at_slot = (wxPGValueType*) wxPGGlobalVars->m_dictValueType[name];

    if ( !p_at_slot )
    {
        wxPGGlobalVars->m_dictValueType[name] = (void*) valueclass;
        return valueclass;
    }

    // Delete given object instance, but only if it wasn't the same as in the hashmap.
    if ( p_at_slot != valueclass )
        delete valueclass;

    return p_at_slot;
}


/*
 * wxPGVariantDataWxObj
 */

//IMPLEMENT_DYNAMIC_CLASS(wxPGVariantDataWxObj, wxVariantData)

#if wxUSE_STD_IOSTREAM
bool wxPGVariantDataWxObj::Write(wxSTD ostream&) const
{
    // Not implemented
    return true;
}
#endif

bool wxPGVariantDataWxObj::Write(wxString&) const
{
    // Not implemented
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxPGVariantDataWxObj::Read(wxSTD istream& WXUNUSED(str))
{
    // Not implemented
    return false;
}
#endif

bool wxPGVariantDataWxObj::Read(wxString& WXUNUSED(str))
{
    // Not implemented
    return false;
}

// -----------------------------------------------------------------------
// Editor class specific.

// no_def_check = true prevents infinite recursion.
wxPGEditor* wxPropertyGrid::RegisterEditorClass( wxPGEditor* editorclass, const wxString& name, bool no_def_check )
{
    wxASSERT ( editorclass );

    WX_PG_GLOBALS_LOCKER()

    if ( !no_def_check && wxPGGlobalVars->m_mapEditorClasses.empty() )
        RegisterDefaultEditors();

    //wxPGGlobalVars->m_arrEditorClasses.Add((void*)editorclass);
    wxPGGlobalVars->m_mapEditorClasses[name] = (void*)editorclass;

    return editorclass;
}

// Registers all default editor classes
void wxPropertyGrid::RegisterDefaultEditors()
{
    wxPGRegisterDefaultEditorClass( TextCtrl );
    wxPGRegisterDefaultEditorClass( Choice );
    wxPGRegisterDefaultEditorClass( ComboBox );
    wxPGRegisterDefaultEditorClass( TextCtrlAndButton );
#if wxPG_INCLUDE_CHECKBOX
    wxPGRegisterDefaultEditorClass( CheckBox );
#endif
    wxPGRegisterDefaultEditorClass( ChoiceAndButton );

#ifdef __WXPYTHON__
    // Register SpinCtrl editor before use
    wxPGRegisterDefaultEditorClass( SpinCtrl );
#endif
}

wxPGEditor* wxPropertyContainerMethods::GetEditorByName( const wxString& editor_name )
{
    wxPGEditor* editor = (wxPGEditor*) wxPGGlobalVars->m_mapEditorClasses[editor_name];
    wxASSERT_MSG( editor,
                  wxT("unregistered editor name") );
    return editor;
}

/*
#if wxPG_USE_VALIDATORS

// -----------------------------------------------------------------------
// wxPropertyValidator
// -----------------------------------------------------------------------

wxPropertyValidator::wxPropertyValidator()
{
    m_refCount = 0;
    m_refObject = (wxPropertyValidator*) NULL;
}

wxPropertyValidator::~wxPropertyValidator()
{
}

wxPropertyValidator* wxPropertyValidator::Ref()
{
    if ( m_refCount )
    {
        // If object given to SetPropertyValidator was one got
        // from GetPropertyValidator, we go here.
        m_refCount++;
        return this;
    }

    wxPropertyValidator* refObj = m_refObject;

    if ( !refObj )
    {
        refObj = Clone();
        m_refObject = refObj;
    }
    refObj->m_refCount++;

    return refObj;
}

// Return true if need to delete
bool wxPropertyValidator::UnRef()
{
    m_refCount--;
    return ( m_refCount < 1 );
}

#ifdef __WXDEBUG__
void wxPropertyValidator::AssertDataType ( const wxChar* ) const
{
}
#endif

// -----------------------------------------------------------------------
// wxStringPropertyValidator
// -----------------------------------------------------------------------

wxStringPropertyValidator::wxStringPropertyValidator( const wxString& excludeList )
    : wxPropertyValidator()
{
    m_excludeList = excludeList;
}

wxStringPropertyValidator::~wxStringPropertyValidator()
{
}

wxPropertyValidator* wxStringPropertyValidator::Clone() const
{
    return new wxStringPropertyValidator(m_excludeList);
}

bool wxStringPropertyValidator::Validate ( wxPGVariant& value, wxString& showmsg ) const
{
    const wxString& str = value.GetString();
    size_t i;

    // Check exclude list
    for ( i=0; i<m_excludeList.length(); i++ )
    {
        if ( str.Index(m_excludeList[i]) != wxNOT_FOUND )
        {
            showmsg.Printf(_("String cannot contain any of these: %s"),m_excludeList.c_str());
            return false;
        }
    }

    return true;
}

#ifdef __WXDEBUG__
void wxStringPropertyValidator::AssertDataType ( const wxChar* typestr ) const
{
    wxASSERT ( wxStrcmp(typestr,wxT("string")) == 0  );
}
#endif

// -----------------------------------------------------------------------
// wxIntPropertyValidator
// -----------------------------------------------------------------------

wxIntPropertyValidator::wxIntPropertyValidator( long min, long max )
    : wxPropertyValidator()
{
    wxASSERT_MSG ( min < max, wxT("Invalid value range given for property validator") );
    m_min = min;
    m_max = max;
}

wxIntPropertyValidator::~wxIntPropertyValidator()
{
}

wxPropertyValidator* wxIntPropertyValidator::Clone() const
{
    return new wxIntPropertyValidator(m_min,m_max);
}

bool wxIntPropertyValidator::Validate ( wxPGVariant& value, wxString& showmsg ) const
{
    long val = value.GetLong();
    long ret_val = val;

    if ( val < m_min )
        ret_val = m_min;
    else if ( val > m_max )
        ret_val = m_max;

    if ( ret_val != val )
    {
        value.m_v.m_long = ret_val;
        showmsg.Printf(_("Value must be between %i and %i"),(int)m_min,(int)m_max);
        return false;
    }

    return true;
}

#ifdef __WXDEBUG__
void wxIntPropertyValidator::AssertDataType ( const wxChar* typestr ) const
{
    wxASSERT ( wxStrcmp(typestr,wxT("long")) == 0  );
}
#endif

// -----------------------------------------------------------------------
// wxFloatPropertyValidator
// -----------------------------------------------------------------------

wxFloatPropertyValidator::wxFloatPropertyValidator( double min, double max )
    : wxPropertyValidator()
{
    wxASSERT_MSG ( min < max, wxT("Invalid value range given for property validator") );
    m_min = min;
    m_max = max;
}

wxFloatPropertyValidator::~wxFloatPropertyValidator()
{
}

wxPropertyValidator* wxFloatPropertyValidator::Clone() const
{
    return new wxFloatPropertyValidator(m_min,m_max);
}

bool wxFloatPropertyValidator::Validate ( wxPGVariant& value, wxString& showmsg ) const
{
    double* pval = value.GetDoublePtr();
    double val = *pval;

    if ( val < m_min || val > m_max )
    {
        if ( val < m_min )
            *pval = m_min;
        else if ( val > m_max )
            *pval = m_max;

        showmsg.Printf(_("Value must be between %.3f and %.3f"),m_min,m_max);
        return false;
    }

    return true;
}

#ifdef __WXDEBUG__
void wxFloatPropertyValidator::AssertDataType ( const wxChar* typestr ) const
{
    wxASSERT ( wxStrcmp(typestr,wxT("double")) == 0  );
}
#endif

#endif
*/

// -----------------------------------------------------------------------
// wxPGStringTokenizer
//   Needed to handle C-style string lists (e.g. "str1" "str2")
// -----------------------------------------------------------------------

wxPGStringTokenizer::wxPGStringTokenizer( const wxString& str, wxChar delimeter )
{
    m_str = &str;
    m_curPos = str.c_str();
    m_delimeter = delimeter;
}

wxPGStringTokenizer::~wxPGStringTokenizer()
{
}

bool wxPGStringTokenizer::HasMoreTokens()
{
    wxASSERT_MSG ( m_curPos, wxT("Do not call wxPGStringTokenizer methods after HasMoreTokens has returned false."));

    const wxChar* ptr = m_curPos;
    const wxChar* ptr_end = &m_str->c_str()[m_str->length()];

    size_t store_index = 0xFFFFFFFF;

#if !wxUSE_STL
    wxChar* store_ptr_base = (wxChar*) NULL;
#endif

    wxChar delim = m_delimeter;
    wxChar a = *ptr;
    wxChar prev_a = 0;

    while ( a )
    {
        if ( store_index == 0xFFFFFFFF )
        {
            if ( a == delim )
            {
                size_t req_len = ptr_end-ptr+1;
            #if wxUSE_STL
                if ( m_readyToken.length() < req_len )
                    m_readyToken.resize( req_len, wxT(' ') );
            #else
                store_ptr_base = m_readyToken.GetWriteBuf ( req_len );
            #endif
                store_index = 0;
                prev_a = 0;
            }
        }
        else
        {
            if ( prev_a != wxT('\\') )
            {
                if ( a != delim )
                {
                    if ( a != wxT('\\') )
                    {
                    #if wxUSE_STL
                        m_readyToken[store_index] = a;
                    #else
                        store_ptr_base[store_index] = a;
                    #endif
                        store_index++;
                    }
                }
                else
                {
                #if wxUSE_STL
                    m_readyToken[store_index] = 0;
                    m_readyToken.resize(store_index,wxT(' '));
                #else
                    store_ptr_base[store_index] = 0;
                    m_readyToken.UngetWriteBuf ( store_index );
                #endif
                    m_curPos = ptr+1;
                    return true;
                }
                prev_a = a;
            }
            else
            {
            #if wxUSE_STL
                m_readyToken[store_index] = a;
            #else
                store_ptr_base[store_index] = a;
            #endif
                store_index++;
                prev_a = 0;
            }
        }
        ptr++;
        a = *ptr;
    }
#if !wxUSE_STL
    if ( store_index != 0xFFFFFFFF )
        m_readyToken.UngetWriteBuf ( store_index );
#endif
    m_curPos = (const wxChar*) NULL;
    return false;
}

wxString wxPGStringTokenizer::GetNextToken()
{
    wxASSERT_MSG ( m_curPos, wxT("Do not call wxPGStringTokenizer methods after HasMoreTokens has returned false."));
    return m_readyToken;
}

// -----------------------------------------------------------------------
// wxPGChoicesData
// -----------------------------------------------------------------------

wxPGChoicesData::wxPGChoicesData()
{
    m_refCount = 1;
}

wxPGChoicesData::~wxPGChoicesData()
{
}

// -----------------------------------------------------------------------
// wxPGChoices
// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxChar* label, int value )
{
    EnsureData();

    if ( value != wxPG_INVALID_VALUE && m_data->m_arrLabels.GetCount() == m_data->m_arrValues.GetCount() )
        m_data->m_arrValues.Add( value );
    else if ( m_data->m_arrValues.GetCount() > 0 )
        m_data->m_arrValues.Add( 0 );

    m_data->m_arrLabels.Add ( label );
}

// -----------------------------------------------------------------------

void wxPGChoices::Insert( const wxChar* label, int index, int value )
{
    EnsureData();

    if ( value != wxPG_INVALID_VALUE && m_data->m_arrLabels.GetCount() == m_data->m_arrValues.GetCount() )
        m_data->m_arrValues.Insert( value, index );
    else if ( m_data->m_arrValues.GetCount() > 0 )
        m_data->m_arrValues.Insert( 0, index );

    m_data->m_arrLabels.Insert( label, index );
}

// -----------------------------------------------------------------------

void wxPGChoices::AddAsSorted( const wxString& label, int value )
{
    //wxASSERT_MSG ( IsOk(),
    //    wxT("do not add items to invalid wxPGChoices") );
    EnsureData();

    size_t index = 0;

    wxArrayString& labels = m_data->m_arrLabels;
    wxArrayInt& values = m_data->m_arrValues;

    while ( index < labels.GetCount() )
    {
        int cmpRes = labels[index].Cmp(label);
        if ( cmpRes > 0 )
            break;
        index++;
    }

    if ( value != wxPG_INVALID_VALUE &&
         labels.GetCount() == values.GetCount() )
        values.Insert ( value, index );

    labels.Insert ( label, index );
}

// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxChar** labels, const long* values )
{
    //wxASSERT_MSG ( IsOk(),
    //    wxT("do not add items to invalid wxPGChoices") );
    EnsureData();

    unsigned int itemcount = 0;
    const wxChar** p = &labels[0];
    while ( *p ) { p++; itemcount++; }

    wxArrayString& i_labels = m_data->m_arrLabels;
    wxArrayInt& i_values = m_data->m_arrValues;

    unsigned int i;
    for ( i = 0; i < itemcount; i++ )
    {
        i_labels.Add ( labels[i] );
    }
    if ( values )
    {
        for ( i = 0; i < itemcount; i++ )
        {
            i_values.Add ( values[i] );
        }
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Add ( const wxArrayString& arr, const long* values )
{
    //wxASSERT_MSG ( IsOk(),
    //    wxT("do not add items to invalid wxPGChoices") );
    EnsureData();

    wxArrayString& labels = m_data->m_arrLabels;
    wxArrayInt& i_values = m_data->m_arrValues;

    unsigned int i;
    unsigned int itemcount = arr.GetCount();

    for ( i = 0; i < itemcount; i++ )
    {
        labels.Add ( arr[i] );
    }
    if ( values )
    {
        for ( i = 0; i < itemcount; i++ )
            i_values.Add ( values[i] );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Add ( const wxArrayString& arr, const wxArrayInt& arrint )
{
    //wxASSERT_MSG ( IsOk(),
    //    wxT("do not add items to invalid wxPGChoices") );
    EnsureData();

    wxArrayString& labels = m_data->m_arrLabels;
    wxArrayInt& values = m_data->m_arrValues;

    unsigned int i;
    unsigned int itemcount = arr.GetCount();

    for ( i = 0; i < itemcount; i++ )
    {
        labels.Add ( arr[i] );
    }

    if ( &arrint && arrint.GetCount() )
        for ( i = 0; i < itemcount; i++ )
        {
            values.Add ( arrint[i] );
        }
}

// -----------------------------------------------------------------------

void wxPGChoices::AssignData( wxPGChoicesData* data )
{
    Free();

    if ( data != wxPGChoicesEmptyData )
    {
        m_data = data;
        data->m_refCount++;
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Init()
{
    m_data = wxPGChoicesEmptyData;
}

// -----------------------------------------------------------------------

void wxPGChoices::Free()
{
    if ( m_data != wxPGChoicesEmptyData )
    {
        m_data->m_refCount--;
        if ( m_data->m_refCount < 1 )
            delete m_data;
        m_data = wxPGChoicesEmptyData;
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridEvent
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyGridEvent, wxCommandEvent)


DEFINE_EVENT_TYPE( wxEVT_PG_SELECTED )
DEFINE_EVENT_TYPE( wxEVT_PG_CHANGED )
DEFINE_EVENT_TYPE( wxEVT_PG_HIGHLIGHTED )
DEFINE_EVENT_TYPE( wxEVT_PG_RIGHT_CLICK )
DEFINE_EVENT_TYPE( wxEVT_PG_PAGE_CHANGED )
DEFINE_EVENT_TYPE( wxEVT_PG_ITEM_EXPANDED )
DEFINE_EVENT_TYPE( wxEVT_PG_ITEM_COLLAPSED )


wxPropertyGridEvent::wxPropertyGridEvent(wxEventType commandType, int id)
    : wxCommandEvent(commandType,id)
{
    m_property = NULL;
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::wxPropertyGridEvent(const wxPropertyGridEvent& event)
    : wxCommandEvent(event)
{
    m_eventType = event.GetEventType();
    m_eventObject = event.m_eventObject;
    m_pg = event.m_pg;
    m_property = event.m_property;
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::~wxPropertyGridEvent()
{
}

// -----------------------------------------------------------------------

wxEvent* wxPropertyGridEvent::Clone() const
{
    return new wxPropertyGridEvent ( *this );
}

// -----------------------------------------------------------------------
// wxPropertyContainerMethods
// - common methods for wxPropertyGrid and wxPropertyGridManager -
// -----------------------------------------------------------------------

void wxPropertyContainerMethods::DoSetPropertyAttribute (wxPGId id, int attrid,
                                                         wxVariant& value, long arg_flags )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT( p );

    p->SetAttribute(attrid,value);

    if ( ( arg_flags & wxPG_RECURSE ) && p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            DoSetPropertyAttribute(pwc->Item(i),attrid,value,arg_flags);
    }
}


// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyAttributeAll ( int attrid, wxVariant value )
{
    DoSetPropertyAttribute(GetRoot(),attrid,value,wxPG_RECURSE);
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::SetBoolChoices (const wxChar* true_choice,
                                                 const wxChar* false_choice)
{
    WX_PG_GLOBALS_LOCKER()
    wxPGGlobalVars->m_boolChoices[0] = false_choice;
    wxPGGlobalVars->m_boolChoices[1] = true_choice;
}

// -----------------------------------------------------------------------

static wxPGChoices gs_emptyChoices;

wxPGChoices& wxPropertyContainerMethods::GetPropertyChoices( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT( p );

    wxPGChoiceInfo ci;
    ci.m_choices = (wxPGChoices*) NULL;

    p->GetChoiceInfo(&ci);

    if ( !ci.m_choices )
        return gs_emptyChoices;

    return *ci.m_choices;
}

// -----------------------------------------------------------------------

wxPGId wxPropertyContainerMethods::DoGetPropertyByName( wxPGPropNameStr name ) const
{
    return m_pState->BaseGetPropertyByName(name);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyContainerMethods::GetPropertyByName( wxPGPropNameStr name,
                                                      wxPGPropNameStr subname ) const
{
    wxPGId id = DoGetPropertyByName(name);
    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*) wxPGIdToPtr(id);
    if ( !pwc || !pwc->GetParentingType() )
        return wxNullProperty;

    return wxPGIdGen(pwc->GetPropertyByName(subname));
}

// -----------------------------------------------------------------------

// Since GetPropertyByName is used *a lot*, this makes sense
// since non-virtual method can be called with less code.
wxPGId wxPropertyContainerMethods::GetPropertyByName( wxPGPropNameStr name ) const
{
    wxPGId id = DoGetPropertyByName(name);
    if ( wxPGIdIsOk(id) )
        return id;

#if wxPG_NAME_IS_WXSTRING
    // Check if its "Property.SubProperty" format
    int pos = name.Find(wxT('.'));
    if ( pos <= 0 )
        return id;

    return GetPropertyByName(name.substr(0,pos),
                             name.substr(pos+1,name.length()-pos-1));
#else
    const wxChar* ptr = wxStrchr(name,wxT('.'));
    if ( !ptr )
        return id;

    size_t slen = wxStrlen(name) + 1;
    wxChar* chars = new wxChar[slen];
    wxTmemcpy(chars,name,slen);

    size_t pos = ptr-name;
    chars[pos] = 0;

    wxPGId retId = GetPropertyByName(chars,
                                     &chars[pos+1]);

    delete [] chars;

    return retId;
#endif
}

// -----------------------------------------------------------------------

bool wxPropertyContainerMethods::HideProperty( wxPGId id, bool hide )
{
    // Hiding properties requires that we are always in the compact mode
    m_pState->GetGrid()->Compact(true);
    return SetPropertyPriority(id,hide?wxPG_LOW:wxPG_HIGH);
}

// -----------------------------------------------------------------------

// Used by HideProperty as well
bool wxPropertyContainerMethods::SetPropertyPriority( wxPGId id, int priority )
{
    wxPropertyGrid* pg = m_pState->GetGrid();

    if ( !wxPGIdIsOk(id) )
        return true;

    wxPGProperty* p = wxPGIdToPtr(id);

    if ( pg == p->GetGrid() )
        return pg->SetPropertyPriority(p,priority);
    else
        m_pState->SetPropertyPriority(p,priority);

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyContainerMethods::SetPropertyMaxLength( wxPGId id, int maxLen )
{
    wxPGProperty* p = wxPGIdToPtr(id);

    wxASSERT_MSG( p, wxT("Cannot call this method with invalid property id") );

    wxPropertyGrid* pg = m_pState->GetGrid();

    p->m_maxLen = (short) maxLen;

    // Adjust control if selected currently
    if ( pg == p->GetGrid() && p == m_pState->GetSelection() )
    {
        wxWindow* wnd = pg->GetEditorControl();
        wxTextCtrl* tc = wxDynamicCast(wnd,wxTextCtrl);
        if ( tc )
            tc->SetMaxLength( maxLen );
        else
        // Not a text ctrl
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------
// GetPropertyValueAsXXX methods

#define IMPLEMENT_GET_VALUE(T,TRET,BIGNAME,DEFRETVAL) \
TRET wxPropertyContainerMethods::GetPropertyValueAs##BIGNAME ( wxPGId id ) wxPG_GETVALUE_CONST \
{ \
    wxPGProperty* p = wxPGIdToPtr(id); \
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") ); \
    if ( !p ) return (TRET)DEFRETVAL; \
    if ( p->GetValueType()->GetTypeName() != wxPGTypeName_##T ) \
    { \
        wxPGGetFailed(p,wxPGTypeName_##T); \
        return (TRET)DEFRETVAL; \
    } \
    return (TRET)wxPGVariantTo##BIGNAME(p->DoGetValue()); \
}

// String is different than others.
wxString wxPropertyContainerMethods::GetPropertyValueAsString ( wxPGId id ) wxPG_GETVALUE_CONST
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return wxEmptyString;
    wxString str;
    return p->GetValueAsString(wxPG_FULL_VALUE);
}

IMPLEMENT_GET_VALUE(long,long,Long,0)
IMPLEMENT_GET_VALUE(long,bool,Bool,false)
IMPLEMENT_GET_VALUE(double,double,Double,0.0)
IMPLEMENT_GET_VALUE(void,void*,VoidPtr,NULL)
IMPLEMENT_GET_VALUE(wxArrayString,const wxArrayString&,ArrayString,*((wxArrayString*)NULL))

// wxObject is different than others.
const wxObject* wxPropertyContainerMethods::GetPropertyValueAsWxObjectPtr ( wxPGId id ) wxPG_GETVALUE_CONST
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return (const wxObject*) NULL;
    const wxChar* typestr = p->GetValueType()->GetTypeName();
    if ( typestr[0] != wxT('w') || typestr[1] != wxT('x') )
    {
        wxPGGetFailed(p,wxT("wxObject"));
        return (const wxObject*) NULL;
    }
    return (const wxObject*)(p->DoGetValue().GetRawPtr());
}

// -----------------------------------------------------------------------

bool wxPropertyContainerMethods::IsPropertyExpanded ( wxPGId id )
{
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*)wxPGIdToPtr( id );
    if ( p->GetParentingType() == 0 )
        return false;
    return p->IsExpanded();
}

// -----------------------------------------------------------------------

// returns value type class for type name
wxPGValueType* wxPropertyContainerMethods::GetValueType(const wxString &type)
{
    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictValueType.find(type);

    if ( it != wxPGGlobalVars->m_dictValueType.end() )
        return (wxPGValueType*) it->second;

    return (wxPGValueType*) NULL;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyContainerMethods::CreatePropertyByType(const wxString &valuetype,
                                                               const wxString &label,
                                                               const wxString &name)
{
    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictValueType.find(valuetype);

    if ( it != wxPGGlobalVars->m_dictValueType.end() )
    {
        wxPGValueType* vt = (wxPGValueType*) it->second;
        wxPGProperty* p = vt->GenerateProperty(label,name);
    #ifdef __WXDEBUG__
        if ( !p )
        {
            wxLogDebug (wxT("WARNING: CreatePropertyByValueType generated NULL property for ValueType \"%s\""),valuetype.c_str());
            return (wxPGProperty*) NULL;
        }
    #endif
        return p;
    }

    wxLogDebug(wxT("WARNING: No value type registered with name \"%s\""),valuetype.c_str());
    return (wxPGProperty*) NULL;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyContainerMethods::CreatePropertyByClass(const wxString &classname,
                                                                const wxString &label,
                                                                const wxString &name)
{
    wxPGHashMapS2P* cis =
        (wxPGHashMapS2P*) &wxPGGlobalVars->m_dictPropertyClassInfo;

    const wxString* pClassname = &classname;
    wxString s;

    // Translate to long name, if necessary
    if ( (pClassname->GetChar(0) != wxT('w') || pClassname->GetChar(1) != wxT('x')) &&
          pClassname->Find(wxT("Property")) < 0 )
    {
        if ( classname != wxT("Category") )
            s.Printf(wxT("wx%sProperty"),pClassname->c_str());
        else
            s = wxT("wxPropertyCategory");
        pClassname = &s;
    }

    wxPGHashMapS2P::iterator it;
    it = cis->find(*pClassname);

    if ( it != cis->end() )
    {
        wxPGPropertyClassInfo* pci = (wxPGPropertyClassInfo*) it->second;
        wxPGProperty* p = pci->m_constructor(label,name);
        return p;
    }
    wxLogError(wxT("No such property class: %s"),pClassname->c_str());
    return (wxPGProperty*) NULL;
}

/*
// create property from text
wxPGProperty* wxPropertyContainerMethods::CreateProperty(const wxString &label,
                                                         const wxString &name,
                                                         wxPGValueType* valuetype,
                                                         const wxString &value)
{
    wxPGProperty* p = valuetype->GenerateProperty(label,name);
#ifdef __WXDEBUG__
    if ( !p )
    {
        wxLogDebug (wxT("WARNING: wxPropertyGridMethods::CreateProperty generated NULL property for ValueType \"%s\""),valuetype->GetTypeName());
        return (wxPGProperty*) NULL;
    }
#endif
    if ( value.length() )
        p->SetValueFromString(value,wxPG_FULL_VALUE);

    return p;
}
*/

// -----------------------------------------------------------------------

// lazy way to prevent RegisterPropertyClass infinite recursion
static int gs_registering_standard_props = 0;

bool wxPropertyContainerMethods::RegisterPropertyClass (const wxChar* name,
                                                        wxPGPropertyClassInfo* classinfo)
{

    WX_PG_GLOBALS_LOCKER()

    // Standard classes must be registered first!
    if ( !gs_registering_standard_props &&
         wxPGGlobalVars->m_dictPropertyClassInfo.empty()
       )
        wxPGRegisterStandardPropertyClasses();

    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictPropertyClassInfo.find(name);

    // only register if not registered already
    if ( it == wxPGGlobalVars->m_dictPropertyClassInfo.end() )
    {
        wxPGGlobalVars->m_dictPropertyClassInfo[name] = classinfo;
        return true;
    }

    wxLogDebug(wxT("WARNING: Property with name \"%s\" was already registered."),name);

    return false;
}

// -----------------------------------------------------------------------

static void wxPGRegisterStandardPropertyClasses()
{

    if ( gs_registering_standard_props )
        return;

    gs_registering_standard_props = 1; // no need to reset this

    wxPGRegisterPropertyClass(wxStringProperty);
    wxPGRegisterPropertyClass(wxIntProperty);
    wxPGRegisterPropertyClass(wxUIntProperty);
    wxPGRegisterPropertyClass(wxFloatProperty);
    wxPGRegisterPropertyClass(wxBoolProperty);
    wxPGRegisterPropertyClass(wxEnumProperty);
    wxPGRegisterPropertyClass(wxFlagsProperty);
    wxPGRegisterPropertyClass(wxLongStringProperty);

    wxPGRegisterPropertyClass(wxPropertyCategory);
    wxPGRegisterPropertyClass(wxParentProperty);
    wxPGRegisterPropertyClass(wxCustomProperty);

    // TODO: Are these really "standard" ?
    wxPGRegisterPropertyClass(wxArrayStringProperty);
    wxPGRegisterPropertyClass(wxFileProperty);
    wxPGRegisterPropertyClass(wxDirProperty);

#ifdef __WXPYTHON__
    wxPropertyContainerMethods::RegisterAdvancedPropertyClasses();
#endif
}

// -----------------------------------------------------------------------
// wxPropertyGridState
// -----------------------------------------------------------------------

// reset helper macro
#undef FROM_STATE
#define FROM_STATE(A) A

// -----------------------------------------------------------------------
// wxPropertyGridState item iteration methods
// -----------------------------------------------------------------------

// Skips categories and sub-properties (unless in wxCustomProperty/wxParentProperty).
wxPGId wxPropertyGridState::GetFirstProperty() const
{
    if ( !m_properties->GetCount() ) return wxPGIdGen((wxPGProperty*)NULL);
    wxPGProperty* p = m_properties->Item(0);
    int parenting = p->GetParentingType();
    if ( parenting > 0 )
        return GetNextProperty ( wxPGIdGen(p) );
    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

// Skips categories and sub-properties (unless in wxParentProperty).
wxPGId wxPropertyGridState::GetNextProperty ( wxPGId id ) const
{
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*)wxPGIdToPtr(id);

    // Go with first child?
    int parenting = p->GetParentingType();
    if ( parenting == 0 || parenting == -1 || !p->GetCount() )
    {
        // No...

        wxPGPropertyWithChildren* parent = p->m_parent;

        // As long as last item, go up and get parent' sibling
        while ( p->m_arrIndex >= (parent->GetCount()-1) )
        {
            p = parent;
            if ( p == m_properties ) return wxPGIdGen((wxPGProperty*)NULL);
            parent = parent->m_parent;
        }

        p = (wxPGPropertyWithChildren*)parent->Item(p->m_arrIndex+1);

        // Go with the next sibling of parent's parent?
    }
    else
    {
        // Yes...
        p = (wxPGPropertyWithChildren*)p->Item(0);
    }

    // If it's category or parentproperty, then go recursive
    parenting = p->GetParentingType();
    if ( parenting > 0 /*|| parenting == -2*/ )
        return GetNextProperty ( wxPGIdGen (p) );

    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetNextSibling ( wxPGId id )
{
    wxASSERT ( wxPGIdIsOk(id) );
    wxPGProperty* p = wxPGIdToPtr(id);
    wxPGPropertyWithChildren* parent = p->m_parent;
    size_t next_ind = p->m_arrIndex + 1;
    if ( next_ind >= parent->GetCount() ) return wxPGIdGen((wxPGProperty*)NULL);
    return wxPGIdGen(parent->Item(next_ind));
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetPrevSibling ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    size_t ind = p->m_arrIndex;
    if ( ind < 1 ) return wxPGIdGen((wxPGProperty*)NULL);
    return wxPGIdGen(p->m_parent->Item(ind-1));
}

// -----------------------------------------------------------------------

// Skips categories and sub-properties (unless in wxParentProperty).
wxPGId wxPropertyGridState::GetPrevProperty ( wxPGId id ) const
{
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*)wxPGIdToPtr(id);
    wxPGPropertyWithChildren* parent = p->m_parent;

    // Is there a previous sibling?
    if ( p->m_arrIndex > 0 )
    {
        // There is!
        p = (wxPGPropertyWithChildren*)parent->Item ( p->m_arrIndex-1 );
        int parenting = p->GetParentingType();

        // Do we return it's last child?
        while ( (parenting > 0 || parenting == -2) && p->GetCount() )
        {
            p = (wxPGPropertyWithChildren*)p->Last();
            parenting = p->GetParentingType();
        }
    }
    else if ( parent != m_properties )
    // Return parent if it isnt' the root
        p = parent;
    else
        return wxPGIdGen((wxPGProperty*)NULL);

    // Skip category and parentproperty.
    int parenting = p->GetParentingType();
    if ( parenting > 0 /*|| parenting == -2*/ )
        return GetPrevProperty ( wxPGIdGen(p) );

    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetFirstCategory () const
{
    //if ( IsInNonCatMode() )
    //    return wxPGIdGen((wxPGProperty*)NULL);

    wxPGProperty* found = (wxPGProperty*)NULL;

    size_t i;
    for ( i=0; i<m_regularArray.GetCount(); i++ )
    {
        wxPGProperty* p = m_regularArray.Item(i);
        if ( p->GetParentingType() > 0 )
        {
            found = p;
            break;
        }
    }
    return wxPGIdGen(found);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetNextCategory ( wxPGId id ) const
{
    wxPGPropertyWithChildren* current = (wxPGPropertyWithChildren*)wxPGIdToPtr(id);

    wxCHECK_MSG( !IsInNonCatMode() || current->GetParentingType() == 1, wxPGIdGen((wxPGProperty*)NULL),
        wxT("GetNextCategory should not be called with non-category argument in non-categoric mode.") );

    wxPGPropertyWithChildren* parent = current->m_parent;
    wxPGProperty* found = (wxPGProperty*) NULL;
    size_t i;

    // Find sub-category, if any.
    if ( current->GetParentingType() > 0 )
    {
        // Find first sub-category in current's array.
        for ( i = 0; i<current->GetCount(); i++ )
        {
            wxPGProperty* p = current->Item(i);
            if ( p->GetParentingType() > 0 )
            {
                found = p;
                break;
            }
        }
        if ( found )
            return wxPGIdGen(found);
    }

    // Find next category in parent's array.
    // (and go up in hierarchy until one found or
    // top is reached).
    do
    {
        for ( i = current->m_arrIndex+1; i<parent->GetCount(); i++ )
        {
            wxPGProperty* p = parent->Item(i);
            if ( p->GetParentingType() > 0 )
            {
                found = p;
                break;
            }
        }
        current = parent;
        parent = parent->m_parent;
    } while ( !found && parent );

    return wxPGIdGen(found);
}

// -----------------------------------------------------------------------
// wxPropertyGridState GetPropertyXXX methods
// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetPropertyByLabel ( const wxString& label, wxPGPropertyWithChildren* parent ) const
{

    size_t i;

    if ( !parent ) parent = (wxPGPropertyWithChildren*) &m_regularArray;

    for ( i=0; i<parent->GetCount(); i++ )
    {
        wxPGProperty* p = parent->Item(i);
        if ( p->m_label == label )
            return wxPGIdGen(p);
        // Check children recursively.
        if ( p->GetParentingType() != 0 )
        {
            p = wxPGIdToPtr(GetPropertyByLabel(label,(wxPGPropertyWithChildren*)p));
            if ( p )
                return wxPGIdGen(p);
        }
    }

    return wxPGIdGen((wxPGProperty*) NULL);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::BaseGetPropertyByName ( wxPGPropNameStr name ) const
{
    wxPGHashMapS2P::const_iterator it;
    it = m_dictName.find(name);
    if ( it != m_dictName.end() )
        return wxPGIdGen( (wxPGProperty*) it->second );
    return wxPGIdGen( (wxPGProperty*) NULL );
}

// -----------------------------------------------------------------------
// wxPropertyGridState global operations
// -----------------------------------------------------------------------

bool wxPropertyGridState::EnableCategories ( bool enable )
{
    ITEM_ITERATION_VARIABLES

    if ( enable )
    {
        //
        // Enable categories
        //

        if ( !IsInNonCatMode() )
            return false;

        m_properties = &m_regularArray;

        // fix parents and indexes
        ITEM_ITERATION_INIT_FROM_THE_TOP

        ITEM_ITERATION_LOOP_BEGIN

            p->m_arrIndex = i;

            p->m_parent = parent;

        ITEM_ITERATION_LOOP_END

    }
    else
    {
        //
        // Disable categories
        //

        if ( IsInNonCatMode() )
            return false;

        // Create array, if necessary.
        if ( !m_abcArray )
            InitNonCatMode ();

        m_properties = m_abcArray;

        // fix parents and indexes
        ITEM_ITERATION_INIT_FROM_THE_TOP

        ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN

            p->m_arrIndex = i;

            p->m_parent = parent;

        ITEM_ITERATION_DCAE_ISP_LOOP_END

    }

    return true;
}

// -----------------------------------------------------------------------

static int wxPG_SortFunc(void **p1, void **p2)
{
    wxPGProperty *pp1 = *((wxPGProperty**)p1);
    wxPGProperty *pp2 = *((wxPGProperty**)p2);
    return pp1->GetLabel().Cmp ( pp2->GetLabel().c_str() );
}

void wxPropertyGridState::Sort ( wxPGProperty* p )
{
    if ( !p )
        p = (wxPGProperty*)m_properties;

    wxASSERT( p->GetParentingType() != 0 );

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

    // Can only sort items with children
    if ( pwc->m_children.GetCount() < 1 )
        return;

    pwc->m_children.Sort ( wxPG_SortFunc );

    // Fix indexes
    pwc->FixIndexesOfChildren();

}

// -----------------------------------------------------------------------

void wxPropertyGridState::Sort()
{
    Sort ( m_properties );

    // Sort categories as well
    if ( !IsInNonCatMode() )
    {
        size_t i;
        for ( i=0;i<m_properties->GetCount();i++)
        {
            wxPGProperty* p = m_properties->Item(i);
            if ( p->GetParentingType() > 0 )
                Sort ( p );
        }
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::ExpandAll( unsigned char do_expand )
{
    ITEM_ITERATION_DCAE_VARIABLES

    bool isGrid = m_pPropGrid->GetState() == this;

    if ( isGrid &&
         m_selected &&
         m_selected->GetParent() != m_properties )
    {
        if ( !m_pPropGrid->ClearSelection() )
            return false;
    }

    if ( !do_expand )
    {
        if ( isGrid )
        {
            if ( !m_pPropGrid->ClearSelection() )
                return false;
        }
        else m_selected = (wxPGProperty*) NULL;
    }

    ITEM_ITERATION_INIT_FROM_THE_TOP

    ITEM_ITERATION_DCAE_LOOP_BEGIN

        if ( parenting != 0 )
            ((wxPGPropertyWithChildren*)p)->m_expanded = do_expand;

    ITEM_ITERATION_DCAE_LOOP_END

    if ( m_pPropGrid->GetState() == this )
    {
        m_pPropGrid->CalculateYs((wxPGPropertyWithChildren*)NULL,-1);

        m_pPropGrid->RedrawAllVisible();
    }

    return true;
}

// -----------------------------------------------------------------------

// Used by SetSplitterLeft
int wxPropertyGridState::GetLeftSplitterPos(wxClientDC& dc,
                                            wxPGPropertyWithChildren* pwc,
                                            bool subProps)
{
    wxPropertyGrid* pg = m_pPropGrid;
    size_t i;
    int maxW = 0;
    int w, h;

    for ( i=0; i<pwc->GetCount(); i++ )
    {
        wxPGProperty* p = pwc->Item(i);
        if ( p->GetParentingType() <= 0 )
        {
            dc.GetTextExtent( p->GetLabel(), &w, &h );

            w += pg->m_marginwidth + ( ((int)p->m_depth-1) * pg->m_subgroup_extramargin ) + (wxPG_XBEFORETEXT*2);

            if ( w > maxW )
                maxW = w;
        }

        if ( p->GetParentingType() &&
             ( subProps || p->GetParentingType() > 0 ) )
        {
            w = GetLeftSplitterPos( dc, (wxPGPropertyWithChildren*) p, subProps );

            if ( w > maxW )
                maxW = w;
        }
    }

    return maxW;
}

// -----------------------------------------------------------------------
// wxPropertyGridState property value setting and getting
// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropVal( wxPGProperty* p, wxPGVariant value )
{
    p->DoSetValue(value);
    if ( m_selected==p && this==m_pPropGrid->GetState() )
        p->UpdateControl(m_pPropGrid->m_wndPrimary);
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::ClearPropertyValue ( wxPGProperty* p )
{
    if ( p )
    {
        const wxPGValueType* valueclass = p->GetValueType();

        if ( valueclass != wxPG_VALUETYPE(none) )
        {
            // wnd_primary has to be given so the editor control can be updated as well.
            SetPropVal(p,valueclass->GetDefaultValue());

            return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue( wxPGProperty* p, const wxPGValueType* typeclass, wxPGVariant value )
{
    if ( p )
    {
        //wxPrintf(wxT("SetPropertyValue: '%s', %s = %s?"),p->GetName().c_str(),p->GetValueType()->GetTypeName(),typeclass->GetTypeName());
        if ( p->GetValueType()->GetTypeName() == typeclass->GetTypeName() )
        {
            //ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
            CLEAR_PROPERTY_UNSPECIFIED_FLAG(p);

            SetPropVal(p,value);

            return true;
        }
        wxPGTypeOperationFailed ( p, typeclass->GetTypeName(), wxT("Set") );
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue( wxPGProperty* p, const wxChar* typestring, wxPGVariant value )
{
    if ( p )
    {
        if ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),typestring) == 0 )
        {
            // wnd_primary has to be given so the control can be updated as well.

            SetPropVal(p,value);
            return true;
        }
        wxPGTypeOperationFailed ( p, typestring, wxT("Set") );
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValueString( wxPGProperty* p, const wxString& value )
{
    if ( p )
    {
        int flags = wxPG_REPORT_ERROR|wxPG_FULL_VALUE;
        CLEAR_PROPERTY_UNSPECIFIED_FLAG(p);

        if ( p->GetMaxLength() <= 0 )
            p->SetValueFromString( value, flags );
        else
            p->SetValueFromString( value.Mid(0,p->GetMaxLength()), flags );

        if ( m_selected==p && this==m_pPropGrid->GetState() )
            p->UpdateControl(m_pPropGrid->m_wndPrimary);

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue( wxPGProperty* p, wxVariant& value )
{
    if ( p )
    {
        //ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
        CLEAR_PROPERTY_UNSPECIFIED_FLAG(p);
        p->GetValueType()->SetValueFromVariant(p,value);
        if ( m_selected==p && this==m_pPropGrid->GetState() )
            p->UpdateControl(m_pPropGrid->m_wndPrimary);

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValueWxObjectPtr( wxPGProperty* p, wxObject* value )
{
    if ( p )
    {
        if ( wxStrcmp (p->GetValueType()->GetTypeName(),
                       value->GetClassInfo()->GetClassName()
                      ) == 0
           )
        {
            //ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
            CLEAR_PROPERTY_UNSPECIFIED_FLAG(p);
            // wnd_primary has to be given so the control can be updated as well.
            SetPropVal(p,wxPGVariantFromWxObject(value));
            return true;
        }
        wxPGTypeOperationFailed ( p, wxT("wxObject"), wxT("Set") );
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyUnspecified ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
    {
        // Flag should be set first - editor class methods may need it
        p->m_flags |= wxPG_PROP_UNSPECIFIED;

        wxASSERT ( m_pPropGrid );

        if ( m_pPropGrid->GetState() == this )
        {
            if ( m_pPropGrid->m_selected == p && m_pPropGrid->m_wndPrimary )
            {
                p->GetEditorClass()->SetValueToUnspecified(m_pPropGrid->m_wndPrimary);
            }
        }

        if ( p->GetParentingType() != 0 )
        {
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

            size_t i;
            for ( i = 0; i < pwc->GetCount(); i++ )
                SetPropertyUnspecified ( pwc->Item(i) );
        }
    }

}

// -----------------------------------------------------------------------
// wxPropertyGridState property operations
// -----------------------------------------------------------------------

void wxPropertyGridState::LimitPropertyEditing( wxPGProperty* p, bool limit )
{
    if ( p )
    {
        if ( limit )
            p->m_flags |= wxPG_PROP_NOEDITOR;
        else
            p->m_flags &= ~(wxPG_PROP_NOEDITOR);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridState::ClearModifiedStatus( wxPGProperty* p )
{
    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

    if ( p->m_flags & wxPG_PROP_MODIFIED )
    {
        p->m_flags &= ~(wxPG_PROP_MODIFIED);

        if ( m_pPropGrid->GetState() == this )
        {
            // Clear active editor bold
            if ( p == m_selected && m_pPropGrid->m_wndPrimary )
                m_pPropGrid->m_wndPrimary->SetFont ( m_pPropGrid->GetFont() );

            m_pPropGrid->DrawItem ( p );
        }
    }

    if ( pwc->GetParentingType() != 0 )
    {
        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            ClearModifiedStatus ( pwc->Item(i) );
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::Collapse( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return false;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return false;

    if ( !pwc->m_expanded ) return false;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 0;

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::Expand( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return false;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return false;

    if ( pwc->m_expanded ) return false;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 1;

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::DoSelectProperty( wxPGProperty* p, bool focus )
{
    if ( this == m_pPropGrid->GetState() )
        return m_pPropGrid->DoSelectProperty( p, focus?wxPG_SEL_FOCUS:0 );

    m_selected = p;
    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyLabel( wxPGProperty* p, const wxString& newlabel )
{
    wxASSERT(p);
    p->SetLabel(newlabel);
    if ( m_pPropGrid->GetWindowStyleFlag() & wxPG_AUTO_SORT )
        Sort(p->GetParent());
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyPriority( wxPGProperty* p, int priority )
{
    int parenting = p->GetParentingType();

    if ( priority == wxPG_HIGH ) p->ClearFlag( wxPG_PROP_HIDEABLE );
    else p->SetFlag( wxPG_PROP_HIDEABLE );

    if ( parenting != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            SetPropertyPriority(pwc->Item(i),priority);
    }

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyAndChildrenFlags( wxPGProperty* p, long flags )
{
    p->m_flags |= flags;

    if ( p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            ClearPropertyAndChildrenFlags ( pwc->Item(i), flags );
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridState::ClearPropertyAndChildrenFlags ( wxPGProperty* p, long flags )
{
    p->m_flags &= ~(flags);

    if ( p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            ClearPropertyAndChildrenFlags ( pwc->Item(i), flags );
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::EnableProperty ( wxPGProperty* p, bool enable )
{
    if ( p )
    {
        if ( enable )
        {
            if ( !(p->m_flags & wxPG_PROP_DISABLED) )
                return false;

            // Enabling

            p->m_flags &= ~(wxPG_PROP_DISABLED);
        }
        else
        {
            if ( p->m_flags & wxPG_PROP_DISABLED )
                return false;

            // Disabling

            p->m_flags |= wxPG_PROP_DISABLED;

        }

        if ( p->GetParentingType() == 0 )
            return true;

        // Apply same to sub-properties as well
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            EnableProperty ( pwc->Item(i), enable );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
// wxPropertyGridState wxVariant related routines
// -----------------------------------------------------------------------

// Returns list of wxVariant objects (non-categories and non-sub-properties only).
// Never includes sub-properties (unless they are parented by wxParentProperty).
wxVariant wxPropertyGridState::GetPropertyValues ( const wxString& listname, wxPGId baseparent, long flags ) const
{
    ITEM_ITERATION_DCAE_VARIABLES

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)wxPGIdToPtr(baseparent);

    // Root is the default base-parent.
    if ( !pwc )
        pwc = m_properties;

    wxList temp_list;
    wxVariant v( temp_list, listname );

    if ( flags & wxPG_KEEP_STRUCTURE )
    {
        wxASSERT ( (pwc->GetParentingType() < -1) || (pwc->GetParentingType() > 0) );

        size_t i;
        for ( i=0; i<pwc->GetCount(); i++ )
        {
            wxPGProperty* p = pwc->Item(i);
            int parenting = p->GetParentingType();
            if ( parenting == 0 || parenting == -1 )
            {
                v.Append ( p->GetValueAsVariant() );
            }
            else
                v.Append ( GetPropertyValues(p->m_name,wxPGIdGen(p),wxPG_KEEP_STRUCTURE) );
        }
    }
    else
    {
        ITEM_ITERATION_INIT((wxPGPropertyWithChildren*)wxPGIdToPtr(baseparent),0)
        ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN

            // Use a trick to ignore wxParentProperty itself, but not its sub-properties.
            if ( parenting == -2 )
            {
                parenting = 1;
            }
            else if ( parenting <= 0 )
            {
                v.Append ( p->GetValueAsVariant() );
            }

        ITEM_ITERATION_DCAE_ISP_LOOP_END
    }

    return v;
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyValues( const wxList& list, wxPGId default_category )
{

    unsigned char orig_frozen = 1;

    if ( m_pPropGrid->GetState() == this )
    {
        orig_frozen = m_pPropGrid->m_frozen;
        if ( !orig_frozen ) m_pPropGrid->Freeze();
    }

    wxPropertyCategoryClass* use_category = (wxPropertyCategoryClass*)wxPGIdToPtr(default_category);

    if ( !use_category )
        use_category = (wxPropertyCategoryClass*)m_properties;

    // Let's iterate over the list of variants.
    wxList::const_iterator node;

    //for ( wxList::Node *node = list.GetFirst(); node; node = node->GetNext() )
    for ( node = list.begin(); node != list.end(); node++ )
    {
        //wxVariant *current = (wxVariant*)node->GetData();
        wxVariant *current = (wxVariant*)*node;

        // Make sure it is wxVariant.
        wxASSERT ( current );
        wxASSERT ( wxStrcmp(current->GetClassInfo()->GetClassName(),wxT("wxVariant")) == 0 );

        if ( current->GetName().length() > 0 )
        {
            wxPGId found_prop = BaseGetPropertyByName(current->GetName());
            if ( wxPGIdIsOk(found_prop) )
            {
                wxPGProperty* p = wxPGIdToPtr(found_prop);

                const wxPGValueType* vtype = p->GetValueType();

                // If it was a list, we still have to go through it.
                if ( current->GetType() == wxT("list") )
                {
                    SetPropertyValues( current->GetList(),
                        wxPGIdGen(
                            p->GetParentingType()>0?p:((wxPGProperty*)NULL)
                        ) );
                }
                else //if ( current->GetType() == vtype->GetTypeName() )
                {
            #ifdef __WXDEBUG__
                    if ( current->GetType() != vtype->GetTypeName() &&
                         current->GetType() != vtype->GetCustomTypeName() )
                    {
                        wxLogDebug(wxT("wxPropertyGridState::SetPropertyValues Warning: Setting value of property \"%s\" from variant"),
                            p->m_name.c_str());
                        wxLogDebug(wxT("    but variant's type name (%s) doesn't match either base type name (%s) nor custom type name (%s)."),
                            current->GetType().c_str(),vtype->GetTypeName(),
                            vtype->GetCustomTypeName());
                    }
            #endif

                    vtype->SetValueFromVariant(p,*current);
                }
            }
            else
            {
                // Is it list?
                if ( current->GetType() != wxT("list") )
                {
                    // Not.
                    AppendIn(use_category,current->GetName(),wxPG_LABEL,(wxVariant&)*current);
                }
                else
                {
                    // Yes, it is; create a sub category and append contents there.
                    wxPGId new_cat = DoInsert(use_category,-1,new wxPropertyCategoryClass(current->GetName(),wxPG_LABEL));
                    SetPropertyValues( current->GetList(), new_cat );
                }
            }
        }
    }

    if ( !orig_frozen )
    {
        m_pPropGrid->Thaw();

        if ( this == m_pPropGrid->GetState() )
        {
            m_selected->UpdateControl(m_pPropGrid->m_wndPrimary);
        }
    }

}

// -----------------------------------------------------------------------
// wxPropertyGridState property adding and removal
// -----------------------------------------------------------------------

// Call for after sub-properties added with AddChild
void wxPGPropertyWithChildren::PrepareSubProperties()
{
        // TODO: When in 1.0.5, move extra stuff from AddChild to here.
    wxPropertyGridState* state = GetParentState();

    wxASSERT(state);

    if ( !GetCount() )
        return;

    wxByte depth = m_depth + 1;
    wxByte depthBgCol = m_depthBgCol;

    wxByte inheritFlags = m_flags & wxPG_INHERITED_PROPFLAGS;

    wxByte bgColIndex = m_bgColIndex;

    //
    // Set some values to the children
    //
    size_t i = 0;
    wxPGPropertyWithChildren* nparent = this;

    while ( i < nparent->GetCount() )
    {
        wxPGProperty* np = nparent->Item(i);

        np->m_flags |= inheritFlags; // Hideable also if parent.
        np->m_depth = depth;
        np->m_depthBgCol = depthBgCol;
        np->m_bgColIndex = bgColIndex;

        // Also handle children of children
        if ( np->GetParentingType() != 0 &&
             ((wxPGPropertyWithChildren*)np)->GetCount() > 0 )
        {
            nparent = (wxPGPropertyWithChildren*) np;
            i = 0;

            // Init
            nparent->m_expanded = 0;
            nparent->m_parentState = state;
            depth++;
        }
        else
        {
            // Next sibling
            i++;
        }

        // After reaching last sibling, go back to processing
        // siblings of the parent
        while ( i >= nparent->GetCount() )
        {
            // Exit the loop when top parent hit
            if ( nparent == this )
                break;

            depth--;

            i = nparent->GetArrIndex() + 1;
            nparent = nparent->GetParent();
        }
    }
}

// -----------------------------------------------------------------------

// Call after fixed sub-properties added/removed after creation.
// if oldSelInd >= 0 and < new max items, then selection is
// moved to it. Note: oldSelInd -2 indicates that this property
// should be selected.
void wxPGPropertyWithChildren::SubPropsChanged( int oldSelInd )
{
    wxPropertyGridState* state = GetParentState();
    wxPropertyGrid* grid = state->GetGrid();

    PrepareSubProperties();

    wxPGProperty* sel = (wxPGProperty*) NULL;
    if ( oldSelInd >= (int)m_children.GetCount() )
        oldSelInd = (int)m_children.GetCount() - 1;

    if ( oldSelInd >= 0 )
        sel = (wxPGProperty*) m_children[oldSelInd];
    else if ( oldSelInd == -2 )
        sel = this;

    if ( sel )
        state->DoSelectProperty(sel);

    if ( state == grid->GetState() )
    {
        if ( m_expanded )
            grid->CalculateYs( GetParent(), m_arrIndex );
        grid->Refresh();
    }
}

// -----------------------------------------------------------------------

int wxPropertyGridState::PrepareToAddItem( wxPGProperty* property,
                                           wxPGPropertyWithChildren* scheduled_parent )
{
    wxPropertyGrid* propgrid = m_pPropGrid;
    wxASSERT ( propgrid );

    int parenting = property->GetParentingType();

    // This will allow better behaviour.
    if ( scheduled_parent == m_properties )
        scheduled_parent = (wxPGPropertyWithChildren*) NULL;

    if ( parenting > 0 )
    {
        /*
        if ( scheduled_parent )
            wxLogDebug(wxT("scheduled_parent= %s, %i"),
                scheduled_parent->GetName().c_str(), (int)scheduled_parent->GetParentingType());
        */

        // Parent of a category must be either root or another category
        // (otherwise Bad Things might happen).
        wxASSERT_MSG ( scheduled_parent == (wxPGPropertyWithChildren*) NULL ||
                       scheduled_parent == m_properties ||
                       scheduled_parent->GetParentingType() > 0,
                 wxT("Parent of a category must be either root or another category."));

        /*
        wxASSERT_MSG ( m_properties == &m_regularArray,
                wxT("Do not add categories in non-categoric mode!"));
        */

        // If we already have category with same name, delete given property
        // and use it instead as most recent caption item.
        wxPGId found_id = BaseGetPropertyByName ( property->GetName() );
        if ( wxPGIdIsOk(found_id) )
        {
            wxPropertyCategoryClass* pwc = (wxPropertyCategoryClass*)wxPGIdToPtr(found_id);
            if ( pwc->GetParentingType() > 0 ) // Must be a category.
            {
                delete property;
                m_currentCategory = pwc;
                return 2; // Tells the caller what we did.
            }
        }
    }

#ifdef __WXDEBUG__
    // Warn for identical names in debug mode.
    if ( property->GetName().length() &&
         wxPGIdIsOk(BaseGetPropertyByName(property->GetName())) &&
         (!scheduled_parent || scheduled_parent->GetParentingType() >= 1) )
        wxLogError(wxT("wxPropertyGrid: Warning - item with name \"%s\" already exists."),
            property->GetName().c_str());
#endif

    // Make sure nothing is selected.
    if ( propgrid && propgrid->m_selected )
    {
        bool selRes = propgrid->ClearSelection();
        wxASSERT_MSG( selRes,
                      wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    property->m_y = -1;

    if ( scheduled_parent )
    {
        // Use parent's background colour.
        property->m_bgColIndex = scheduled_parent->m_bgColIndex;
    }

    // If in hideable adding mode, or if assigned parent is hideable, then
    // make this one hideable.
    if (
         ( scheduled_parent && (scheduled_parent->m_flags & wxPG_PROP_HIDEABLE) ) ||
         ( propgrid && (propgrid->m_iFlags & wxPG_FL_ADDING_HIDEABLES) )
       )
        property->SetFlag ( wxPG_PROP_HIDEABLE );

    // Set custom image flag.
    if ( property->GetImageSize().y < 0 )
    {
        property->m_flags |= wxPG_PROP_CUSTOMIMAGE;
    }

    if ( propgrid->GetWindowStyleFlag() & wxPG_LIMITED_EDITING )
        property->m_flags |= wxPG_PROP_NOEDITOR;

    if ( parenting < 1 )
    {
        // This is not a category.

        wxASSERT_MSG ( property->GetEditorClass(), wxT("Editor class not initialized!") );

        // Depth.
        unsigned char depth = 1;
        if ( scheduled_parent )
        {
            depth = scheduled_parent->m_depth;
            if ( scheduled_parent->GetParentingType() < 0 )
                depth++;
        }
        property->m_depth = depth;
        unsigned char grey_depth = depth;

        if ( scheduled_parent )
        {
            wxPropertyCategoryClass* pc;

            if ( scheduled_parent->GetParentingType() > 0 )
                pc = (wxPropertyCategoryClass*)scheduled_parent;
            else
                // This conditional compile is necessary because
                // otherwise things get too complex for some compilers.
                pc = wxPropertyGrid::_GetPropertyCategory(scheduled_parent);

            if ( pc )
                grey_depth = pc->GetDepth();
            else
                grey_depth = scheduled_parent->m_depthBgCol;
        }

        property->m_depthBgCol = grey_depth;

        // Add children to propertywithchildren.
        if ( parenting < 0 )
        {
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)property;

            pwc->m_parentState = this;

            pwc->m_expanded = 0; // Properties with children are not expanded by default.
            if ( propgrid && propgrid->GetWindowStyleFlag() & wxPG_HIDE_MARGIN )
                pwc->m_expanded = 1; // ...unless it cannot not be expanded.

            //
            // If children were added prior to append, then this is considered
            // a "fixed" parent (otherwise the -2 is set, see below, to mark
            // its customizable).
            if ( pwc->GetCount() )
            {
                pwc->PrepareSubProperties();
                /*

                depth++;

                unsigned char inheritFlags =
                    property->m_flags & wxPG_INHERITED_PROPFLAGS;

                unsigned char bgcolind = property->m_bgColIndex;

                //
                // Set some values to the children
                //
                size_t i = 0;
                wxPGPropertyWithChildren* nparent = pwc;

                while ( i < nparent->GetCount() )
                {
                    wxPGProperty* np = nparent->Item(i);

                    np->m_flags |= inheritFlags; // Hideable also if parent.
                    np->m_depth = depth;
                    np->m_depthBgCol = grey_depth;
                    np->m_bgColIndex = bgcolind;

                    // Also handle children of children
                    if ( np->GetParentingType() != 0 &&
                         ((wxPGPropertyWithChildren*)np)->GetCount() > 0 )
                    {
                        nparent = (wxPGPropertyWithChildren*) np;
                        i = 0;

                        // Init
                        nparent->m_expanded = 0;
                        nparent->m_parentState = this;
                        depth++;
                    }
                    else
                    {
                        // Next sibling
                        i++;
                    }

                    // After reaching last sibling, go back to processing
                    // siblings of the parent
                    while ( i >= nparent->GetCount() )
                    {
                        // Exit the loop when top parent hit
                        if ( nparent == pwc )
                            break;

                        depth--;

                        i = nparent->GetArrIndex() + 1;
                        nparent = nparent->GetParent();
                    }
                }
                */

            }
            else
            {
                pwc->m_parentingType = -2;
            }

        }

    }
    else
    {
        // This is a category.

        // depth
        unsigned char depth = 1;
        if ( scheduled_parent )
        {
            depth = scheduled_parent->m_depth + 1;
        }
        property->m_depth = depth;
        property->m_depthBgCol = depth;

        m_currentCategory = (wxPropertyCategoryClass*)property;

        wxPropertyCategoryClass* pc = (wxPropertyCategoryClass*)property;
        pc->m_parentState = this;

        // Calculate text extent for caption item.
        pc->CalculateTextExtent(propgrid,propgrid->GetCaptionFont());

    }

    return parenting;
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::Append( wxPGProperty* property )
{
    wxPropertyCategoryClass* cur_cat = m_currentCategory;
    if ( property->GetParentingType() > 0 )
        cur_cat = (wxPropertyCategoryClass*) NULL;

    return DoInsert( cur_cat, -1, property );
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::DoInsert( wxPGPropertyWithChildren* parent, int index, wxPGProperty* property )
{
    wxASSERT( property );

    if ( !parent )
        parent = m_properties;

    wxPropertyGrid* propgrid = m_pPropGrid;

    wxASSERT_MSG( parent->GetParentingType() != 0,
        wxT("wxPropertyGrid: Invalid parent") );

    wxASSERT_MSG( (parent->GetParentingType() != -1),
        wxT("wxPropertyGrid: Do not attempt to insert items under fixed parents.") );

    int parenting = PrepareToAddItem ( property, (wxPropertyCategoryClass*)parent );

    // This type of invalid parenting value indicates we should exit now, returning
    // id of most recent category.
    if ( parenting > 1 )
        return wxPGIdGen(m_currentCategory);

    // Note that item must be added into current mode later.

    // If parent is wxParentProperty, just stick it in...
    // If parent is root (m_properties), then...
    //   In categoric mode: Add as last item in m_abcArray (if not category).
    //                      Add to given index in m_regularArray.
    //   In non-cat mode:   Add as last item in m_regularArray.
    //                      Add to given index in m_abcArray.
    // If parent is category, then...
    //   1) Add to given category in given index.
    //   2) Add as last item in m_abcArray.

    int parents_parenting = parent->GetParentingType();
    if ( parents_parenting < 0 )
    {
        // Parent is wxParentingProperty: Just stick it in...
        parent->AddChild2( property, index );
    }
    else
    {
        // Parent is Category or Root.

        if ( m_properties == &m_regularArray )
        {
            // Categorized mode

            // Only add non-categories to m_abcArray.
            if ( m_abcArray && parenting <= 0 )
                m_abcArray->AddChild2 ( property, -1, false );

            // Add to current mode.
            parent->AddChild2 ( property, index );

        }
        else
        {
            // Non-categorized mode.

            if ( parent != m_properties )
                // Parent is category.
                parent->AddChild2 ( property, index, false );
            else
                // Parent is root.
                m_regularArray.AddChild2 ( property, -1, false );

            // Add to current mode (no categories).
            if ( parenting <= 0 )
                m_abcArray->AddChild2 ( property, index );
        }
    }

    // category stuff
    if ( parenting > 0 )
    {
    // This is a category caption item.

        // Last caption is not the bottom one (this info required by append)
        m_lastCaptionBottomnest = 0;
    }

    // Only add name to hashmap if parent is root or category
    if ( parent->GetParentingType() >= 1 && property->m_name.length() )
        m_dictName[property->m_name] = (void*) property;

    m_itemsAdded = 1;

    if ( propgrid )
        propgrid->m_bottomy = 0; // this signals y recalculation

    return wxPGIdGen(property);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::AppendIn( wxPGPropertyWithChildren* pwc,
                                      const wxString& label,
                                      const wxString& propname,
                                      wxVariant& value )
{
    wxPGProperty* p = wxPropertyContainerMethods::
        CreatePropertyByType(value.GetType(),label,propname);

    if ( p )
    {
        p->GetValueType()->SetValueFromVariant(p,value);
        return DoInsert(pwc,-1,p);
    }
    return wxPGIdGen((wxPGProperty*)NULL);
}

// -----------------------------------------------------------------------

void wxPropertyGridState::DoDelete( wxPGProperty* item )
{

    wxASSERT_MSG( item != &m_regularArray && item != m_abcArray,
        wxT("wxPropertyGrid: Do not attempt to remove the root item.") );

    size_t i;
    int parenting = item->GetParentingType();
    unsigned int indinparent = item->GetIndexInParent();

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)item;

    wxASSERT_MSG( item->GetParent()->GetParentingType() != -1,
        wxT("wxPropertyGrid: Do not attempt to remove sub-properties.") );

    if ( parenting > 0 )
    {
        // deleting a category

        // erase category entries from the hash table
        for ( i=0; i<pwc->GetCount(); i++ )
        {
            wxPGProperty* sp = pwc->Item( i );
            if ( sp->GetName().Len() ) m_dictName.erase( wxPGNameConv(sp->GetName()) );
        }

        if ( pwc == m_currentCategory )
            m_currentCategory = (wxPropertyCategoryClass*) NULL;

        if ( m_abcArray )
        {
        // Remove children from non-categorized array.
            for ( i=0; i<pwc->GetCount(); i++ )
            {
                wxPGProperty * p = pwc->Item( i );
                wxASSERT ( p != NULL );
                m_abcArray->m_children.Remove( (void*)p );
            }

            if ( IsInNonCatMode() )
                m_abcArray->FixIndexesOfChildren();
        }

    }

    if ( !IsInNonCatMode() )
    {
        // categorized mode - non-categorized array
        if ( parenting <= 0 )
        {
            if ( m_abcArray )
            {
                m_abcArray->m_children.Remove( item );
                m_abcArray->FixIndexesOfChildren();
            }
        }

        // categorized mode - categorized array
        item->m_parent->m_children.RemoveAt(indinparent);
        item->m_parent->FixIndexesOfChildren(/*indinparent*/);
    }
    else
    {
        // non-categorized mode - categorized array

        // We need to find location of item.
        wxPGPropertyWithChildren* cat_parent = &m_regularArray;
        int cat_index = m_regularArray.GetCount();
        size_t i;
        for ( i = 0; i < m_regularArray.GetCount(); i++ )
        {
            wxPGProperty* p = m_regularArray.Item(i);
            if ( p == item ) { cat_index = i; break; }
            if ( p->GetParentingType() > 0 )
            {
                int subind = ((wxPGPropertyWithChildren*)p)->Index(item);
                if ( subind != wxNOT_FOUND )
                {
                    cat_parent = ((wxPGPropertyWithChildren*)p);
                    cat_index = subind;
                    break;
                }
            }
        }
        cat_parent->m_children.RemoveAt(cat_index);

        // non-categorized mode - non-categorized array
        if ( parenting <= 0 )
        {
            wxASSERT( item->m_parent == m_abcArray );
            item->m_parent->m_children.RemoveAt(indinparent);
            item->m_parent->FixIndexesOfChildren(indinparent);
        }
    }

    if ( item->GetName().Len() ) m_dictName.erase( wxPGNameConv(item->GetName()) );

    // We can actually delete it now
    delete item;

    m_itemsAdded = 1; // Not a logical assignment (but required nonetheless).

    if ( this == m_pPropGrid->GetState() )
    {
        //m_pPropGrid->m_clearThisMany = 1;
        m_pPropGrid->m_bottomy = 0; // this signals y recalculation
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridState init etc.
// -----------------------------------------------------------------------

void wxPropertyGridState::InitNonCatMode ()
{
    ITEM_ITERATION_DCAE_VARIABLES

    if ( !m_abcArray )
    {
        m_abcArray = new wxPGRootPropertyClass();
        m_abcArray->SetParentState(this);
        m_abcArray->m_expanded = wxPG_EXP_OF_COPYARRAY;
    }

    // Must be called when FROM_STATE(m_properties) still points to regularArray.
    //wxASSERT ( FROM_STATE(m_properties) == &FROM_STATE(m_regularArray) );
    wxPGPropertyWithChildren* old_properties = m_properties;

    // Must use temp value in FROM_STATE(m_properties) for item iteration loop
    // to run as expected.
    m_properties = &m_regularArray;

    // Copy items.
    ITEM_ITERATION_INIT_FROM_THE_TOP

    ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN

    if ( parenting < 1 &&
        ( parent == m_properties || parent->GetParentingType() > 0 ) )
    {

        m_abcArray->AddChild2 ( p );
        p->m_parent = &FROM_STATE(m_regularArray);
    }
    //else wxLogDebug("OUT: %s",p->m_label.c_str());

    ITEM_ITERATION_DCAE_ISP_LOOP_END

    m_properties = old_properties;

}

// -----------------------------------------------------------------------

void wxPropertyGridState::Clear()
{
    m_regularArray.Empty();
    if ( m_abcArray )
        m_abcArray->Empty();

    m_dictName.clear();

    m_currentCategory = (wxPropertyCategoryClass*) NULL;
    m_lastCaptionBottomnest = 1;
    m_itemsAdded = 0;

    m_selected = (wxPGProperty*) NULL;
}

// -----------------------------------------------------------------------

wxPropertyGridState::wxPropertyGridState()
{
    m_pPropGrid = (wxPropertyGrid*) NULL;
    m_regularArray.SetParentState(this);
    m_properties = &m_regularArray;
    m_abcArray = (wxPGRootPropertyClass*) NULL;
    m_currentCategory = (wxPropertyCategoryClass*) NULL;
    m_selected = (wxPGProperty*) NULL;
    m_lastCaptionBottomnest = 1;
    m_itemsAdded = 0;
    m_anyModified = 0;
}

// -----------------------------------------------------------------------

wxPropertyGridState::~wxPropertyGridState()
{
    if ( m_abcArray )
        delete m_abcArray;
}

// -----------------------------------------------------------------------
// wxPropertyGridPopulator
// -----------------------------------------------------------------------

void wxPropertyGridPopulator::Init( wxPropertyGrid* pg, wxPGId popRoot )
{
    WX_PG_GLOBALS_LOCKER()

    m_propGrid = pg;
    m_popRoot = popRoot;
    wxPGGlobalVars->m_offline++;
}

// -----------------------------------------------------------------------

wxPropertyGridPopulator::~wxPropertyGridPopulator()
{

    //
    // Free unused sets of choices
    wxPGHashMapP2P::iterator it;

    for( it = m_dictIdChoices.begin(); it != m_dictIdChoices.end(); ++it )
    {
        wxPGChoicesData* data = (wxPGChoicesData*) it->second;
        data->m_refCount--;
        if ( data->m_refCount < 1 )
            delete data;
    }

    wxPGGlobalVars->m_offline--;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::HasChoices( wxPGChoicesId id ) const
{
    wxPGHashMapP2P::const_iterator it = m_dictIdChoices.find(id);
    return ( it != m_dictIdChoices.end() );
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::BeginChildren()
{
    if ( wxPGIdIsOk(m_lastProperty) &&
         wxPGIdToPtr(m_lastProperty)->CanHaveExtraChildren() )
    {
        wxLogDebug(wxT("New Parent: %s"),wxPGIdToPtr(m_lastProperty)->GetLabel().c_str());
        m_curParent = m_lastProperty;
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::AddChoices(wxPGChoicesId choicesId,
                                         const wxArrayString& choiceLabels,
                                         const wxArrayInt& choiceValues)
{
#ifdef __WXDEBUG__
    // Make sure the id is not used yet
    wxPGHashMapP2P::iterator it = m_dictIdChoices.find(choicesId);
    wxASSERT_MSG( it == m_dictIdChoices.end(),
        wxT("added set of choices to same id twice (use HasChoices if necessary)") );
#endif

    wxASSERT_MSG( choicesId != (wxPGChoicesId)0,
        wxT("choicesId must not be 0/NULL"));

    wxPGChoices chs(choiceLabels,choiceValues);
    wxPGChoicesData* data = chs.ExtractData();
    m_dictIdChoices[choicesId] = (void*) data;

    // Artifically reduce refcount to 0 (since nothing uses it yet)
    //data->m_refCount = 0;

}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridPopulator::DoAppend(wxPGProperty* p,
                                         const wxString& value,
                                         const wxString& attributes,
                                         wxPGChoicesId choicesId,
                                         const wxArrayString& choiceLabels,
                                         const wxArrayInt& choiceValues)
{
    wxASSERT( m_propGrid );

    // Make sure m_curParent is ok
    if ( !wxPGIdIsOk(m_curParent) )
    {
        if ( !wxPGIdIsOk(m_popRoot) )
            m_popRoot = m_propGrid->GetRoot();
        m_curParent = m_popRoot;
    }

    if ( p )
    {

        // Set choices
        if ( choicesId )
        {
            wxPGHashMapP2P::iterator it = m_dictIdChoices.find(choicesId);

            wxPGChoices chs;

            if ( it != m_dictIdChoices.end() )
            {
                // Already found
                wxPGChoicesData* foundData = (wxPGChoicesData*) it->second;
                chs.AssignData(foundData);
            }
            else
            {
                chs.Set(choiceLabels,choiceValues);
                m_dictIdChoices[choicesId] = (void*) chs.GetData();
            }

            p->SetChoices(chs);

        }

        // Value setter must be before append
        if ( value.length() )
        {
            p->SetValueFromString(value,wxPG_FULL_VALUE);
        }

        // Set attributes
        if ( attributes.length() )
            wxPropertyGrid::SetPropertyAttributes(p,attributes);

        // Append to grid
        m_propGrid->AppendIn(m_curParent,p);

        m_lastProperty = p;
    }
    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridPopulator::AppendByClass(const wxString& classname,
                                              const wxString& label,
                                              const wxString& name,
                                              const wxString& value,
                                              const wxString& attributes,
                                              wxPGChoicesId choicesId,
                                              const wxArrayString& choiceLabels,
                                              const wxArrayInt& choiceValues)
{
    wxPGProperty* p = m_propGrid->CreatePropertyByClass(classname,label,name);
    return DoAppend(p,value,attributes,choicesId,choiceLabels,choiceValues);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridPopulator::AppendByType(const wxString& valuetype,
                                             const wxString& label,
                                             const wxString& name,
                                             const wxString& value,
                                             const wxString& attributes,
                                             wxPGChoicesId choicesId,
                                             const wxArrayString& choiceLabels,
                                             const wxArrayInt& choiceValues)
{
    wxPGProperty* p = m_propGrid->CreatePropertyByType(valuetype,label,name);
    return DoAppend(p,value,attributes,choicesId,choiceLabels,choiceValues);
}

// -----------------------------------------------------------------------
