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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include <wx/propgrid/property.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>


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

	bool						Update				(void)	{	_Create();	return( true );	}

	virtual bool				OnEvent				(wxPropertyGrid *pPG, wxWindow *pPGCtrl, wxEvent &event);


protected:

	class CSG_Parameter			*m_pParameter;

	wxArrayPtrVoid				m_choices_data;


	void						_Create				(void);
	void						_Destroy			(void);

	void						_Append				(const wxString &Label, long  Value);
	void						_Append				(const wxString &Label, void *Value = NULL);

	int							_Set_Choice			(void);
	int							_Set_Table			(void);
	int							_Set_Table_Field	(void);
	int							_Set_Shapes			(void);
	int							_Set_TIN			(void);
	int							_Set_PointCloud		(void);
	int							_Set_Grid_System	(void);
	int							_Set_Grid			(void);

	int							_DataObject_Init	(void);

	void						_Set_Parameter_Value(int iChoice);

	void						_Update_Grids		(void);
	void						_Update_TableFields	(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPG_Parameter_Value
{
public:
	CPG_Parameter_Value(void)							{	m_pParameter	= NULL;			}
	CPG_Parameter_Value(CSG_Parameter *pParameter)		{	m_pParameter	= pParameter;	}

	CSG_Parameter				*m_pParameter;

	bool						from_String			(const wxString &String);
	wxString					to_String			(void) const;
	bool						Check				(void) const;
	bool						Do_Dialog			(void);

};

WX_PG_DECLARE_VARIANT_DATA(CPG_Parameter_Value)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_Range : public wxPGProperty
{
public:
	CParameters_PG_Range(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	bool						Update				(void)			{	SetValue(m_value);	return( true );	}

	virtual wxVariant			ChildChanged		(wxVariant &thisValue, int childIndex, wxVariant &childValue)	const;
	virtual void				RefreshChildren		(void);
	virtual const wxPGEditor *	DoGetEditorClass	(void)	const	{	return( wxPGEditor_TextCtrl );	}

};

//---------------------------------------------------------
class CParameters_PG_Degree : public wxPGProperty
{
public:
	CParameters_PG_Degree(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	bool						Update				(void)			{	SetValue(m_value);	return( true );	}

	virtual wxVariant			ChildChanged		(wxVariant &thisValue, int childIndex, wxVariant &childValue)	const;
	virtual void				RefreshChildren		(void);
	virtual const wxPGEditor *	DoGetEditorClass	(void)	const	{	return( wxPGEditor_TextCtrl );	}
};

//---------------------------------------------------------
class CParameters_PG_Dialog : public wxPGProperty
{
public:
	CParameters_PG_Dialog(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	bool						Update				(void)			{	SetValue(m_value);	return( true );	}

	virtual bool				OnEvent				(wxPropertyGrid *pPG, wxWindow *pPGCtrl, wxEvent &event);
	virtual wxString			ValueToString		(wxVariant &value, int argFlags = 0)	const;
	virtual const wxPGEditor *	DoGetEditorClass	(void)	const	{	return( wxPGEditor_TextCtrlAndButton );	}

};

//---------------------------------------------------------
class CParameters_PG_Colors : public CParameters_PG_Dialog
{
public:
	CParameters_PG_Colors(const wxString &label = wxPG_LABEL, const wxString &name = wxPG_LABEL, CSG_Parameter *pParameter = NULL);

	virtual void				OnCustomPaint		(wxDC &dc, const wxRect &rect, wxPGPaintData &paintdata);
	virtual wxSize				OnMeasureImage		(int item = -1)	const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Parameters_Properties_H
