/* ****************************************************************************
 * RectTracker.h                                                              *
 *                                                                            *
 * (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 *                                                                            *
 * wxEvtHandler adaptation from Troels K. (2011)                              *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */

#ifndef __RECTTRACKER_H__
#define __RECTTRACKER_H__

#include <wx/wx.h>

#ifdef __WXGTK3__
#define __TRACKER_OVERLAY__
#endif

#ifdef __TRACKER_OVERLAY__
#include <wx/overlay.h>
#endif


/// Handler position enum.
enum RT_HANDLER
{
   RT_HANDLER_OUTSIDE       = -1,
   RT_HANDLER_NONE          =  0,
   RT_HANDLER_TOP_MID       =  1,
   RT_HANDLER_MID_RIGHT     =  2,
   RT_HANDLER_BOTTOM_MID    =  4,
   RT_HANDLER_MID_LEFT      =  8,
   RT_HANDLER_TOP_LEFT      = RT_HANDLER_TOP_MID + RT_HANDLER_MID_LEFT,
   RT_HANDLER_TOP_RIGHT     = RT_HANDLER_TOP_MID + RT_HANDLER_MID_RIGHT,
   RT_HANDLER_BOTTOM_RIGHT  = RT_HANDLER_BOTTOM_MID + RT_HANDLER_MID_RIGHT,
   RT_HANDLER_BOTTOM_LEFT   = RT_HANDLER_BOTTOM_MID + RT_HANDLER_MID_LEFT
};

/// Current state of the wxRectTracker control. Currently only for internal use.
enum RT_STATE
{
   RT_STATE_NONE = 0,
   RT_STATE_DRAGGING = 1,
   RT_STATE_MOUSE_CAPTURED = 2,
   RT_STATE_DISABLED = 4,
   RT_STATE_FIRSTDRAG = 8
};

/// Mask to use with SetHandlerMask() to specify which handlers will be displayed.
enum RT_MASK
{
   /// Use this to hide handlers.
   RT_MASK_NONE         = 0x00,
   RT_MASK_TOP_LEFT     = 0x01,
   RT_MASK_TOP_MID      = 0x02,
   RT_MASK_TOP_RIGHT    = 0x04,
   RT_MASK_MID_RIGHT    = 0x08,
   RT_MASK_BOTTOM_RIGHT = 0x10,
   RT_MASK_BOTTOM_MID   = 0x20,
   RT_MASK_BOTTOM_LEFT  = 0x40,
   RT_MASK_MID_LEFT     = 0x80,
   /// Use this to show all handlers (default).
   RT_MASK_ALL          = 0xFF
};
 
/** wxRectTracker control
 * 
 * This control aims at providing same functionnalies as the MFC CRectTracker.
 * It is basically a selection rectangle with dragging capabilites, 
 *  to set its size and position.
 *
 * This control is a wxEvtHandler to not disturb Event chain and fix background issues
 * You must register it vist PushEventHandler and RemoveEventHandler
 *
 * Well nothing much to be said about this... or maybe :
 *  - The control defines two events EVT_TRACKER_CHANGED and EVT_TRACKER_CHANGING to be
 *     used in the parent hierarchy (for size and position display in a status bar, for instance)
 *  - The virtual function AdjustTrackerRect can be used to implement some 
 *     rules about the way the tracker should behave (typically, keeping a constant 
 *     width/height ratio, as in wxRectTrackerRatio)
 *  - You can set a max area with SetMaxRect()
 *  - You can get the current size/position with GetUnscrolledRect()
 *
 * To use this control, you have just to create it on the wxWindow you want ; it will register itself. 
 * Delete it to unregister it.
 *
 */ 
class wxRectTracker : public wxEvtHandler
{
	DECLARE_CLASS(wxRectTracker)
public:
    /** wxRectTracker constructor
	 *
     * @param wnd is the wxWindow accepting this widget
	 * @param frame is the frame containing this widget for debugging purpose (will display its position in the statusbar)
     */
	wxRectTracker(wxWindow* wnd, wxFrame* frame = NULL);
	virtual ~wxRectTracker();

public:
    /// Returns true if the provided coordinates are in the tracker. (Parent coo.)
	int HitTest(int x, int y) const;
	
	// Manipulation function -----------------------------------------------------
	/// Update the tracker position and size (usefull for initialisation)
	void Update();
	/// Get the coordinates of the area the tracker should not go below.
	wxRect GetMinRect() const { return m_minRect; };
	/// Set the minimum boundaries of the available space the tracker.
	void SetMinRect(const wxRect& minRect);
	/// Get the coordinates of the area the tracker should not go beyond.
	wxRect GetMaxRect() const { return m_maxRect; };
	/// Set the maximum boundaries of the available space the tracker.
	void SetMaxRect(const wxRect& maxRect);
	/// Get the list of handlers to be displayed (See RT_MASK enum)
	int GetHandlerMask() const { return m_iHandlerMask; }
	/// Set which handlers will be displayed (See RT_MASK enum)
	void SetHandlerMask(int iMask = RT_MASK_ALL) { m_iHandlerMask = iMask; }
	/// Enable the Tracker
	void Enable();
	/// Disable the Tracker
	void Disable();
	/// Get the Status
	bool IsEnabled() const { return ((m_state & RT_STATE_DISABLED) == 0); };
	/// Get Appearance Status
	bool IsShown() const;
	/// Hide the Tracker
	void Hide();
	/// Set Grey Out Colour
	void SetGreyOutColour(const wxColour & col) { m_cGreyOut = col; }
	void SetGreyOutColour(int percent, const wxColour & col = *wxBLACK) { m_cGreyOut = wxColour(col.Red(), col.Green(), col.Blue(), (percent*255/100)); }
	/// Get Grey Out Colour
	const wxColour & GetGreyOutColour() {return m_cGreyOut; }
	/// Set Handler Width
	void SetHandlesWidth(int handlesWidth) { m_iHandlerWidth = handlesWidth; Update(); }
	/// Get Handler Width
	int GetHandlesWidth() { return m_iHandlerWidth; }

	/// Get current position of the tracker
	wxRect GetTrackerRect() { return m_Rect; }
	/// Set a new position for the tracker
	void SetTrackerRect(const wxRect & rect) { m_Rect = rect; }
	
	// Scrolled Region Function --------------------------------------------------
	/** Get the current size and position of the tracker. 
     * If the tracker is on a wxScrolledWindow component, this is the absolute 
     * position (as if the control was not scrolled) 
	 *
	 * Deprecated with wxEvtHandler ; will be soon removed, use GetRect / SetRect instead.
     */
	wxRect GetUnscrolledRect() const;
	/// Set a new position for the tracker ; deprecated with wxEvtHandlet, do not use.
	virtual void SetUnscrolledRect(const wxRect& rect);
	/// Get the current position of the tracker ; deprecated with wxEvtHandler, do not use.
	wxRect GetTrackingRect() const { return m_curRect; };

	/// Callback on Draw event
	virtual void OnDraw(wxDC*);

protected:
	// Internals : Events
    DECLARE_EVENT_TABLE()
	// - Misc Events
	virtual void OnPaint(wxPaintEvent & event);
	virtual void OnKey(wxKeyEvent & event);
	virtual void OnMouseMotion(wxMouseEvent & event);
	virtual void OnMouseLeftDown(wxMouseEvent & event);
	virtual void OnMouseLeftUp(wxMouseEvent & event);

	// Helper Functions
	virtual void DrawRect(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawRect(wxDC & dc, wxRect rect);
	virtual void DrawTracker(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawTracker(wxDC & dc, const wxRect& rect);
	//void ForwardMouseEventToParent(wxMouseEvent & event);

protected:
	// Behaviour Functions
	/** Adjust the behaviour of the tracker.
     * This virtual function allow you to specify special behaviour of the tracker
     *  as maintaining a constant ratio, keeping into a special area,...
     *
     * @param curRect [in, out] the asked position
     * @param handler [in] the handler used to ask this new position
     *
     * You get the new coordinates the user ask in curRect, with the handler he 
     * used. You can now process this information, and decide a new position that
     * you indicated as output in curRect (that is why this is not a const wxRect &).
	 */
	virtual void AdjustTrackerRect(wxRect & curRect, int handler);
	/// Example of AdjustTrackerRect function
	void AdjustTrackerRectMax(wxRect & curRect, int handler);
	void AdjustTrackerRectMin(wxRect & curRect, int handler);

protected:
	int m_iHandlerWidth; /// Width of drawn handler
	int m_iHandlerMask;  /// Mask describing which handlers will be drawn
	int m_state;         /// Current state of the control (See RT_STATE)
	wxColour m_cGreyOut; /// Colour to grey out the region outside selection

#ifdef __TRACKER_OVERLAY__
	wxOverlay trackerOverlay;  // Overlay
#endif

	wxPoint m_leftClick; /// Coordinates of the last left clic
	wxPoint m_prevMove;  /// Coordinates of the previous move
	wxRect m_prevRect;   /// Coordinates of the previous calculated tracker
	wxRect m_curRect;    /// Coordinates of the current tracker
	wxRect m_Rect;       /// Coordinates of the current tracker
	wxCursor * m_cursorMove;

	/// maxRect : the tracker should not go beyond this rect (Parent Coo)
	wxRect m_minRect, m_maxRect;
	
	/// m_wnd : the window containing the widget
    wxWindow* m_wnd;

	/// m_frame : the frame containing the widget
    wxFrame* m_frame;
};

//#include "sgdi_core.h"
//extern SGDI_API_DLL_EXPORT const wxEventType wxEVT_TRACKER_CHANGING;
//extern SGDI_API_DLL_EXPORT const wxEventType wxEVT_TRACKER_CHANGED;
//
///// Event fired when the user has decided a new position for the tracker (dragging is finished)
//#define EVT_TRACKER_CHANGED(id, fn)\
// DECLARE_EVENT_TABLE_ENTRY(wxEVT_TRACKER_CHANGED, id,\
// wxID_ANY,\
// (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),
//
///// Event fired when the user is being moving or resizing the tracker (dragging in process)
//#define EVT_TRACKER_CHANGING(id, fn)\
// DECLARE_EVENT_TABLE_ENTRY(wxEVT_TRACKER_CHANGING, id,\
// wxID_ANY,\
// (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),


/* ****************************************************************************
* RectTrackerRatio.h                                                         *
*                                                                            *
* (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
*                                                                            *
* RectTracker was originally designed for RPhoto, but can be used separately *
* It is a control similar to the CRectTracker of MS MFC.                     *
*                                                                            *
* Licence : wxWindows (based on L-GPL)                                       *
*                                                                            *
* ************************************************************************** */

/** Tracker with Ratio constraint.
* 
* This class adds a width/height constraint in the tracker. The user can only
*  drag a tracker which respect this constraint. It uses basically :
* - ratio : the selected ratio (0 if none)
* - orientation : the desired orientation (0 : automatic, -1 : portrait, 1 : landscape)
* - fixed size : to set a constant size which cannot be resized
* - 4 guides line with a given radio (ex : 0 = none; 0.5 = middle ; 0.66 = third)
*
*/
class wxRectTrackerRatio : public wxRectTracker
{
	DECLARE_CLASS(wxRectTrackerRatio)
public:
	wxRectTrackerRatio(wxWindow* parent, wxFrame * frame = NULL);
	virtual ~wxRectTrackerRatio();

public:
	// Manipulation function
	double GetRatio() { return ratio; };
	double GetGuideRatio() { return guideRatio; };
	int GetOrientation() { return orientation; };
	int GetFixedWidth() { return fixedWidth; }
	int GetFixedHeight() { return fixedHeight; }
	void SetRatio(double ratio);
	void SetGuideRatio(double ratio);
	void SetOrientation(int orientation);
	void SetFixedSize(int width, int height);
	bool IsFixedSize() { return (this->fixedWidth != -1); }

	void AdjustTrackerRectFixed(wxRect & curRect, int handler);

protected:
	// Behaviour Functions
	long CalcRectDist(wxRect r1, wxRect r2);
	wxRect CalcRectNearer(wxRect rr, wxRect r1, wxRect r2);
	virtual void AdjustTrackerRect(wxRect & curRect, int handler);
	void AdjustTrackerRectRatio(wxRect & curRect, int handler, bool expand);
	// Helper Functions
	virtual void DrawRect(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawTracker(wxDC & dc, int x, int y, int w, int h);

protected:
	/// The tracker should keep this ratio (0 : don't use)
	double ratio;
	/// Ratio of the 4 guide lines (ex : 0 = none; 0.5 = middle ; 0.66 = third)
	double guideRatio;
	/// Fixed width
	int fixedWidth;
	/// Fixed height
	int fixedHeight;
	/// Orientation : 0 : automatic, -1 : portrait, 1 : landscape
	int orientation;
};


/* ****************************************************************************
* LineTracker.h                                                              *
*                                                                            *
* (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
* LineTracker was originally designed for RPhoto, but can be used separately *
*                                                                            *
* Licence : wxWindows (based on L-GPL)                                       *
*                                                                            *
* ************************************************************************** */

/// Handler position enum.
enum RT_LINE_HANDLER
{
	RT_LINE_HANDLER_OUTSIDE       = -1,
	RT_LINE_HANDLER_NONE          =  0,
	RT_LINE_HANDLER_BEGIN         =  1,
	RT_LINE_HANDLER_END           =  2,
	RT_LINE_HANDLER_ON_LINE       =  3,
};

/// Mask to use with SetHandlerMask() to specify which handlers will be displayed.
enum RT_LINE_MASK
{
	/// Use this to hide handlers.
	RT_LINE_MASK_NONE         = 0x00,
	RT_LINE_MASK_BEGIN        = 0x01,
	RT_LINE_MASK_END          = 0x02,
	/// Use this to show all handlers (default).
	RT_LINE_MASK_ALL          = 0xFF
};

/** wxLineTracker control
* 
* This control aims at providing same functionnalies as wxRectTracker, but for lines.
* It is basically a line with dragging capabilites, to set its size and position.
*
*  @see wxRectTracker
*/ 
class wxLineTracker : public wxRectTracker
{
	DECLARE_CLASS(wxLineTracker)
public:
	/** wxLineTracker constructor
	*/
	wxLineTracker(wxWindow* parent, wxFrame * frame = NULL);
	virtual ~wxLineTracker();

public:
	/// Returns true if the provided coordinates are in the tracker. (Parent coo.)
	int HitTest(int x, int y) const;

	/// Update the tracker position and size (usefull for initialisation)
	void Update();
	/// Get the list of handlers to be displayed (See RT_LINE_MASK enum)
	int GetHandlerMask() const { return m_iHandlerMask; }
	/// Set which handlers will be displayed (See RT_MASK enum)
	void SetHandlerMask(int iMask = RT_LINE_MASK_ALL) { m_iHandlerMask = iMask; }

	/// Get the current position of the tracker, without taking in account any scroll area.
	wxPoint GetPosBegin() const; 
	wxPoint GetPosEnd() const; 
	wxPoint GetPosHandler(enum RT_HANDLER handler) const;
	/// Set a new position for the tracker
	virtual void SetTrackerPosition(wxPoint begin, wxPoint end);

protected:
	// Internals : Events
	// - Misc Events
	virtual void OnPaint(wxPaintEvent & event);
	virtual void OnKey(wxKeyEvent & event);
	virtual void OnMouseMotion(wxMouseEvent & event);
	virtual void OnMouseLeftDown(wxMouseEvent & event);
	virtual void OnMouseLeftUp(wxMouseEvent & event);

	// Helper Functions
	virtual void DrawLine(wxDC & dc, wxPoint begin, wxPoint end);
	virtual void DrawTracker(wxDC & dc, wxPoint begin, wxPoint end);

protected:
	// Behaviour Functions
	virtual void AdjustLineTracker(wxPoint & begin, wxPoint & end, int handler);
	void AdjustLineTrackerMax(wxPoint & begin, wxPoint & end, int handler);

protected:
	wxPoint m_pCurBegin; /// Coordinates of the previous first point
	wxPoint m_pCurEnd;   /// Coordinates of the previous last point

	enum RT_HANDLER m_iBeginHandler;	// Handler of Begin Point
	enum RT_HANDLER m_iEndHandler;		// Handler of End Point

};

#endif // __RECTTRACKER_H__
