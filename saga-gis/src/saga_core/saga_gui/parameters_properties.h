/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Parameters_Properties.h                //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__Parameters_Properties_H
#define _HEADER_INCLUDED__SAGA_GUI__Parameters_Properties_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/extras.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_Choice : public wxEnumProperty
{
public:
	CParameters_PG_Choice(CSG_Parameter *pParameter);
	virtual ~CParameters_PG_Choice(void);

	bool						Update				(void);

	virtual bool				OnEvent				(wxPropertyGrid *pPG, wxWindow *pPGCtrl, wxEvent &event);


protected:

	class CSG_Parameter			*m_pParameter;

	wxArrayPtrVoid				m_choices_data;


	void						_Create				(void);
	void						_Destroy			(void);

	void						_Append				(const wxChar *Label, long  Value);
	void						_Append				(const wxChar *Label, void *Value = NULL);

	int							_Set_Choice			(void);
	int							_Set_Table			(void);
	int							_Set_Table_Field	(void);
	int							_Set_Shapes			(void);
	int							_Set_TIN			(void);
	int							_Set_PointCloud		(void);
	int							_Set_Grid_System	(void);
	int							_Set_Grid			(void);

	int							_DataObject_Init	(void);

	void						_Update_Grids		(wxPropertyGrid *pPG);
	void						_Update_TableFields	(wxPropertyGrid *pPG);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_Parameter_Value
{
public:
	CParameters_PG_Parameter_Value(void)							{	m_pParameter	= NULL;			}
	CParameters_PG_Parameter_Value(CSG_Parameter *pParameter)		{	m_pParameter	= pParameter;	}

	CSG_Parameter				*m_pParameter;

	bool						from_String			(const wxString &String);
	wxString					to_String			(void) const;
	bool						Check				(void) const;
	bool						Do_Dialog			(void);

};

WX_PG_DECLARE_VARIANT_DATA(CParameters_PG_Parameter_Variant, CParameters_PG_Parameter_Value, wxEMPTY_PARAMETER_VALUE)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_Range : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(CParameters_PG_Range)

public:
	CParameters_PG_Range(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	WX_PG_DECLARE_PARENTAL_METHODS()

	bool						Update				(void);

};

//---------------------------------------------------------
class CParameters_PG_Degree : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(CParameters_PG_Degree)

public:
	CParameters_PG_Degree(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	WX_PG_DECLARE_PARENTAL_METHODS()

	bool						Update				(void);

};

//---------------------------------------------------------
class CParameters_PG_Dialog : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(CParameters_PG_Dialog)

public:
	CParameters_PG_Dialog(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	virtual wxString			GetValueAsString	(int flags)	const;

	WX_PG_DECLARE_EVENT_METHODS()

	bool						Update				(void);

};

//---------------------------------------------------------
class CParameters_PG_Colors : public CParameters_PG_Dialog
{
	WX_PG_DECLARE_PROPERTY_CLASS(CParameters_PG_Colors)

public:
	CParameters_PG_Colors(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Parameters_Properties_H
