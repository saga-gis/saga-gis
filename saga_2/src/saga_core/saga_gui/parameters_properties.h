
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_Choice : public wxEnumPropertyClass
{
public:
	CParameters_PG_Choice(class CSG_Parameter *pParameter);
	virtual ~CParameters_PG_Choice(void);

	bool						Update				(void);
 
	virtual bool				OnEvent				(wxPropertyGrid *pPG, wxWindow *pPGCtrl, wxEvent &event);

	virtual const wxChar *		GetClassName		(void) const;


protected:

	class CSG_Parameter			*m_pParameter;

	wxArrayPtrVoid				m_choices_data;


	void						_Create				(void);
	void						_Destroy			(void);
	void						_Append				(const wxChar *Label, long  Value);
	void						_Append				(const wxChar *Label, void *Value);
	void						_Append				(const wxChar *Label);

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
class CParameters_PG_GridSystem : public wxCustomPropertyClass
{
public:
	CParameters_PG_GridSystem(class CSG_Parameter *pParameter);
	virtual ~CParameters_PG_GridSystem(void);

	bool						Update				(void);
 
	virtual wxString			GetValueAsString	(int arg_flags = 0) const;
	virtual bool				SetValueFromInt		(long value, int arg_flags = 0);


protected:

	int							m_index;

	class CSG_Parameter			*m_pParameter;

	wxArrayPtrVoid				m_choices_data;


	void						_Create				(void);
	void						_Destroy			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_DoublesValue
{
public:
	CParameters_PG_DoublesValue(void);
	CParameters_PG_DoublesValue(class CSG_Parameter *pParameter);
	~CParameters_PG_DoublesValue(void);

	bool						operator ==			(const CParameters_PG_DoublesValue &r) const	{	return( false );	}

	bool						Assign				(const CParameters_PG_DoublesValue &Value);

	bool						Update_Parameter	(void);
	bool						Update_Values		(void);

	int							m_nValues;

	double						*m_Values;

	wxString					*m_Labels;


protected:

	class CSG_Parameter			*m_pParameter;


	bool						_Create				(class CSG_Parameter *pParameter);
	void						_Destroy			(void);

};

//---------------------------------------------------------
WX_PG_DECLARE_VALUE_TYPE_VOIDP(CParameters_PG_DoublesValue)

//---------------------------------------------------------
WX_PG_DECLARE_PROPERTY(CParameters_PG_Doubles, const CParameters_PG_DoublesValue &, CParameters_PG_DoublesValue(NULL))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_DialogedValue
{
public:
	CParameters_PG_DialogedValue(void)								{	m_pParameter	= NULL;			}
	CParameters_PG_DialogedValue(class CSG_Parameter *pParameter)	{	m_pParameter	= pParameter;	}
	virtual ~CParameters_PG_DialogedValue(void)						{}

	bool						operator ==		(const CParameters_PG_DialogedValue &r) const	{	return( false );	}

	bool						fromString		(wxString String);
	wxString					asString		(void) const;
	bool						Check			(void) const;
	bool						Do_Dialog		(void);


	class CSG_Parameter			*m_pParameter;

};

//---------------------------------------------------------
WX_PG_DECLARE_VALUE_TYPE_VOIDP(CParameters_PG_DialogedValue)

//---------------------------------------------------------
WX_PG_DECLARE_PROPERTY(CParameters_PG_Dialoged, const CParameters_PG_DialogedValue &, CParameters_PG_DialogedValue(NULL))

//---------------------------------------------------------
WX_PG_DECLARE_PROPERTY(CParameters_PG_Colors  , const CParameters_PG_DialogedValue &, CParameters_PG_DialogedValue(NULL))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Parameters_Properties_H
