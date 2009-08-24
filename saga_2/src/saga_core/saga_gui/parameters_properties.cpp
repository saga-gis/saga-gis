
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
//               Parameters_Properties.cpp               //
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
#include <saga_api/saga_api.h>

#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp_data_manager.h"

#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"

#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"

#include "parameters_properties.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_DATAOBJECT_LABEL(p)	(p->is_Option() ? wxString::Format(p->Get_Name()) : wxString::Format(wxT("%s %s"), p->is_Input() ? (p->is_Optional() ? wxT(">") : wxT(">>")) : (p->is_Optional() ? wxT("<") : wxT("<<")), p->Get_Name()))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters_PG_Choice::CParameters_PG_Choice(CSG_Parameter *pParameter)
	: wxEnumProperty(GET_DATAOBJECT_LABEL(pParameter), pParameter->Get_Identifier(), NULL)
{
	m_pParameter	= pParameter;

	_Create();
}

//---------------------------------------------------------
CParameters_PG_Choice::~CParameters_PG_Choice(void)
{
	_Destroy();
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Create(void)
{
	_Destroy();

	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:							SetIndex(0);					break;
		case PARAMETER_TYPE_Choice:			SetIndex(_Set_Choice());		break;
		case PARAMETER_TYPE_Grid_System:	SetIndex(_Set_Grid_System());	break;
		case PARAMETER_TYPE_Table_Field:	SetIndex(_Set_Table_Field());	break;
		case PARAMETER_TYPE_Grid:			SetIndex(_Set_Grid());			break;
		case PARAMETER_TYPE_Table:			SetIndex(_Set_Table());			break;
		case PARAMETER_TYPE_Shapes:			SetIndex(_Set_Shapes());		break;
		case PARAMETER_TYPE_TIN:			SetIndex(_Set_TIN());			break;
		case PARAMETER_TYPE_PointCloud:		SetIndex(_Set_PointCloud());	break;
		}
	}
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Destroy(void)
{
	m_choices		.Clear();
	m_choices_data	.Clear();

	SetIndex(0);
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Append(const wxChar *Label, long Value)
{
	m_choices		.Add(Label, Value);
	m_choices_data	.Add((void *)NULL);
}

void CParameters_PG_Choice::_Append(const wxChar *Label, void *Value)
{
	m_choices		.Add(Label, m_choices_data.Count());
	m_choices_data	.Add(Value);
}

//---------------------------------------------------------
bool CParameters_PG_Choice::Update(void)
{
	_Create();

	return( true );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Choice(void)
{
	for(int i=0; i<m_pParameter->asChoice()->Get_Count(); i++)
	{
		_Append(m_pParameter->asChoice()->Get_Item(i), i);
	}

	return( m_pParameter->asInt() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Table_Field(void)
{
	CSG_Parameter	*pParent;
	CSG_Table		*pTable;

	if(	(pParent = m_pParameter->Get_Parent()) != NULL)
	{
		switch( pParent->Get_Type() )
		{
	    default:					pTable	= NULL;					break;
		case PARAMETER_TYPE_Table:	pTable	= pParent->asTable();	break;
		case PARAMETER_TYPE_Shapes:	pTable	= pParent->asShapes();	break;
		case PARAMETER_TYPE_TIN:	pTable	= pParent->asTIN();		break;
		}

		if( pTable )
		{
			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				_Append(pTable->Get_Field_Name(i));
			}

			if( m_pParameter->is_Optional() || pTable->Get_Field_Count() == 0 )
			{
				_Append( LNG("[VAL] [not set]") );
			}

			return( m_pParameter->asInt() >= 0 ? m_pParameter->asInt() : GetItemCount() - 1);
		}
	}

	_Append( LNG("[VAL] [not set]") );

	return( GetItemCount() - 1 );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Table(void)
{
	CWKSP_Table_Manager	*pTables;

	if( (pTables = g_pData->Get_Tables()) != NULL )
	{
		for(int i=0; i<pTables->Get_Count(); i++)
		{
			_Append(pTables->Get_Table(i)->Get_Name(), pTables->Get_Table(i)->Get_Table());
		}
	}

	CWKSP_Shapes_Manager	*pManager;

	if( (pManager = g_pData->Get_Shapes()) != NULL )
	{
		for(int i=0; i<pManager->Get_Count(); i++)
		{
			CWKSP_Shapes_Type	*pShapes	= (CWKSP_Shapes_Type *)pManager->Get_Item(i);

			for(int j=0; j<pShapes->Get_Count(); j++)
			{
				_Append(pShapes->Get_Shapes(j)->Get_Name(), pShapes->Get_Shapes(j)->Get_Shapes());
			}
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Shapes(void)
{
	CWKSP_Shapes_Manager	*pManager;

	if( (pManager = g_pData->Get_Shapes()) != NULL )
	{
		int		Shape_Type	= ((CSG_Parameter_Shapes *)m_pParameter->Get_Data())->Get_Shape_Type();

		for(int i=0; i<pManager->Get_Count(); i++)
		{
			CWKSP_Shapes_Type	*pShapes	= (CWKSP_Shapes_Type *)pManager->Get_Item(i);

			if( Shape_Type == SHAPE_TYPE_Undefined || Shape_Type == pShapes->Get_Shapes_Type() )
			{
				for(int j=0; j<pShapes->Get_Count(); j++)
				{
					_Append(pShapes->Get_Shapes(j)->Get_Name(), pShapes->Get_Shapes(j)->Get_Shapes());
				}
			}
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_TIN(void)
{
	CWKSP_TIN_Manager	*pTINs;

	if( (pTINs = g_pData->Get_TINs()) != NULL )
	{
		for(int i=0; i<pTINs->Get_Count(); i++)
		{
			_Append(pTINs->Get_TIN(i)->Get_Name(), pTINs->Get_TIN(i)->Get_TIN());
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_PointCloud(void)
{
	CWKSP_PointCloud_Manager	*pManager;

	if( (pManager = g_pData->Get_PointClouds()) != NULL )
	{
		for(int i=0; i<pManager->Get_Count(); i++)
		{
			_Append(pManager->Get_PointCloud(i)->Get_Name(), pManager->Get_PointCloud(i)->Get_PointCloud());
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Grid_System(void)
{
	g_pData->Check_Parameter(m_pParameter);

	CWKSP_Grid_Manager	*pSystems	= g_pData->Get_Grids();

	if( pSystems && pSystems->Get_Count() > 0 )
	{
		int	index	= pSystems->Get_Count();

		for(int i=0; i<pSystems->Get_Count(); i++)
		{
			_Append(pSystems->Get_System(i)->Get_Name(), pSystems->Get_System(i)->Get_System());

			if( m_pParameter->asGrid_System()->is_Equal(*pSystems->Get_System(i)->Get_System()) )
			{
				index	= i;
			}
		}

		_Append(LNG("[VAL] [not set]"));

		return( index );
	}

	_Append( LNG("[VAL] [no choice available]"));

	return( 0 );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Grid(void)
{
	CSG_Parameter		*pParent	= m_pParameter->Get_Parent();
	CWKSP_Grid_Manager	*pManager	= g_pData->Get_Grids();
	CWKSP_Grid_System	*pSystem	= NULL;

	if( pManager && pParent && pParent->Get_Type() == PARAMETER_TYPE_Grid_System && pParent->asGrid_System()->is_Valid() )
	{
		for(int i=0; i<pManager->Get_Count() && !pSystem; i++)
		{
			if( pParent->asGrid_System()->is_Equal(*pManager->Get_System(i)->Get_System()) )
			{
				pSystem	= pManager->Get_System(i);
			}
		}
	}

	if( pSystem )
	{
		for(int i=0; i<pSystem->Get_Count(); i++)
		{
			_Append(pSystem->Get_Grid(i)->Get_Name(), pSystem->Get_Grid(i)->Get_Grid());
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_DataObject_Init(void)
{
	if( m_pParameter->is_Output() )
	{
		_Append(LNG("[VAL] [create]"), DATAOBJECT_CREATE);
	}

	if( !m_pParameter->is_Output() || (m_pParameter->is_Output() && m_pParameter->is_Optional()) )
	{
		_Append(LNG("[VAL] [not set]"), DATAOBJECT_NOTSET);
	}

	g_pData->Check_Parameter(m_pParameter);

	for(int i=0; i<(int)GetItemCount(); i++)
	{
		if( m_pParameter->asDataObject() == (void *)m_choices_data.Item(m_choices.GetValue(i)) )
		{
			return( i );
		}
	}

	return( GetItemCount() - 1 );
}

//---------------------------------------------------------
bool CParameters_PG_Choice::OnEvent(wxPropertyGrid *pPG, wxWindow *pPGCtrl, wxEvent &event)
{
	if( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
	{
		if( m_pParameter && m_choices.IsOk() && GetIndex() >= 0 && GetIndex() < (int)GetItemCount() )
		{
			switch( m_pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_Table_Field:
				m_pParameter->Set_Value(GetIndex());
				break;

			case PARAMETER_TYPE_Grid_System:
				m_pParameter->Set_Value((void *)m_choices_data.Item(m_choices.GetValue(GetIndex())));
				_Update_Grids(pPG);
				break;

			case PARAMETER_TYPE_Grid:
				m_pParameter->Set_Value((void *)m_choices_data.Item(m_choices.GetValue(GetIndex())));
				break;

			case PARAMETER_TYPE_Table:
			case PARAMETER_TYPE_Shapes:
			case PARAMETER_TYPE_TIN:
			case PARAMETER_TYPE_PointCloud:
				m_pParameter->Set_Value((void *)m_choices_data.Item(m_choices.GetValue(GetIndex())));
				_Update_TableFields(pPG);
				break;
			}

			pPG->EditorsValueWasModified();

			return( true );
		}
	}

	event.Skip();

	return( false );
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Update_Grids(wxPropertyGrid *pPG)
{
	if( m_pParameter && m_pParameter->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		for(int i=0; i<m_pParameter->Get_Children_Count(); i++)
		{
			wxPGProperty	*pProperty;
			CSG_Parameter	*pChild	= m_pParameter->Get_Child(i);

			switch( pChild->Get_Type() )
			{
			default:
				break;
				
			case PARAMETER_TYPE_Grid_List:
				break;

			case PARAMETER_TYPE_Grid:
				if( (pProperty = pPG->GetProperty(wxString::Format(wxT("%s.%s"), m_pParameter->Get_Identifier(), pChild->Get_Identifier()))) != NULL )
				{
					((CParameters_PG_Choice *)pProperty)->Update();
				}
			}
		}
	}
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Update_TableFields(wxPropertyGrid *pPG)
{
	if( m_pParameter )
	{
		for(int i=0; i<m_pParameter->Get_Children_Count(); i++)
		{
			wxPGProperty	*pProperty;
			CSG_Parameter	*pChild	= m_pParameter->Get_Child(i);

			if(	pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
			{
				if( (pProperty = pPG->GetPropertyByName(pChild->Get_Identifier())) != NULL )
				{
					((CParameters_PG_Choice *)pProperty)->Update();
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_VARIANT_DATA(CParameters_PG_Parameter_Variant, CParameters_PG_Parameter_Value)

//---------------------------------------------------------
bool CParameters_PG_Parameter_Value::from_String(const wxString &String)
{
	switch( m_pParameter->Get_Type() )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Text:
	case PARAMETER_TYPE_FilePath:
		m_pParameter->Set_Value(String.c_str());
		return( true );
	}
}

//---------------------------------------------------------
wxString CParameters_PG_Parameter_Value::to_String(void) const
{
	wxString	s;

	if( m_pParameter && Check() )
	{
		s	= m_pParameter->asString();
	}

	return( s );
}

//---------------------------------------------------------
bool CParameters_PG_Parameter_Value::Check(void) const
{
	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
		case PARAMETER_TYPE_Grid_List:
			CSG_Parameter_List	*pList	= (CSG_Parameter_Grid_List *)m_pParameter->Get_Data();

			for(int i=pList->Get_Count()-1; i>=0; i--)
			{
				if( !g_pData->Exists(pList->asDataObject(i)) )
				{
					pList->Del_Item(i);
				}
			}

			break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters_PG_Parameter_Value::Do_Dialog(void)
{
	bool		bModified	= false;
	long		Color;
	wxString	Text;

	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Font:
			bModified	= DLG_Font			(m_pParameter->asFont(), Color = m_pParameter->asColor());

			if( bModified )
			{
				m_pParameter->Set_Value((int)Color);

				return( true );
			}
			break;

		case PARAMETER_TYPE_Text:
			bModified	= DLG_Text			(m_pParameter->Get_Name(), Text = m_pParameter->asString());

			if( bModified )
			{
				m_pParameter->Set_Value(Text.c_str());

				return( true );
			}
			break;

		case PARAMETER_TYPE_FilePath:
			Text	= m_pParameter->asString();

			if( m_pParameter->asFilePath()->is_Directory() )
			{
				bModified	= DLG_Directory	(Text, LNG("[CAP] Choose Directory"));
			}
			else if( m_pParameter->asFilePath()->is_Save() )
			{
				bModified	= DLG_Save		(Text, LNG("[CAP] Save"), m_pParameter->asFilePath()->Get_Filter());
			}
			else if( m_pParameter->asFilePath()->is_Multiple() == false )
			{
				bModified	= DLG_Open		(Text, LNG("[CAP] Open"), m_pParameter->asFilePath()->Get_Filter());
			}
			else
			{
				wxArrayString	Files;

				bModified	= DLG_Open		(Files, LNG("[CAP] Open"), m_pParameter->asFilePath()->Get_Filter());

				if( bModified )
				{
					if( Files.GetCount() > 0 )
					{
						Text.Clear();

						for(size_t i=0; i<Files.GetCount(); i++)
						{
							Text.Append(i > 0 ? wxT(" \"") : wxT("\""));
							Text.Append(Files.Item(i));
							Text.Append(wxT("\""));
						}
					}
					else
					{
						Text	= Files.Item(0);
					}
				}
			}

			if( bModified )
			{
				m_pParameter->Set_Value(Text.c_str());

				return( true );
			}
			break;

		case PARAMETER_TYPE_FixedTable:
			bModified	= DLG_Table			(m_pParameter->Get_Name(), m_pParameter->asTable());
			break;

		case PARAMETER_TYPE_Grid_List:
			bModified	= DLG_List_Grid		(m_pParameter->Get_Name(), m_pParameter->asGridList());
			break;

		case PARAMETER_TYPE_Table_List:
			bModified	= DLG_List_Table	(m_pParameter->Get_Name(), m_pParameter->asTableList());
			break;

		case PARAMETER_TYPE_Shapes_List:
			bModified	= DLG_List_Shapes	(m_pParameter->Get_Name(), m_pParameter->asShapesList());
			break;

		case PARAMETER_TYPE_TIN_List:
			bModified	= DLG_List_TIN		(m_pParameter->Get_Name(), m_pParameter->asTINList());
			break;

		case PARAMETER_TYPE_Colors:
			bModified	= DLG_Colors		(m_pParameter->asColors());
			break;

		case PARAMETER_TYPE_Parameters:
			bModified	= DLG_Parameters	(m_pParameter->asParameters());
			break;
		}
	}

	if( bModified )
	{
		m_pParameter->has_Changed();
	}

	return( bModified );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Range, wxPGProperty, CParameters_PG_Parameter_Value, const &CParameters_PG_Parameter_Value, TextCtrl)

//---------------------------------------------------------
CParameters_PG_Range::CParameters_PG_Range(const wxString &label, const wxString &name, CSG_Parameter *pParameter)
	: wxPGProperty(label, name)
{
	if( pParameter && pParameter->Get_Type() == PARAMETER_TYPE_Range )
	{
		m_value	= CParameters_PG_Parameter_ValueToVariant(CParameters_PG_Parameter_Value(pParameter));

		AddPrivateChild( new wxFloatProperty(wxT("Minimum")	, wxPG_LABEL, pParameter->asRange()->Get_LoVal()) );
		AddPrivateChild( new wxFloatProperty(wxT("Maximum")	, wxPG_LABEL, pParameter->asRange()->Get_HiVal()) );
	}
}

//---------------------------------------------------------
void CParameters_PG_Range::RefreshChildren(void)
{
	const CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(m_value);

	if( GetCount() == 2 && value.m_pParameter && value.m_pParameter->Get_Type() == PARAMETER_TYPE_Range )
	{
		Item(0)->SetValue( value.m_pParameter->asRange()->Get_LoVal() );
		Item(1)->SetValue( value.m_pParameter->asRange()->Get_HiVal() );
	}
}

//---------------------------------------------------------
void CParameters_PG_Range::ChildChanged(wxVariant &thisValue, int childIndex, wxVariant &childValue) const
{
	CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(thisValue);

	if( GetCount() == 2 && value.m_pParameter && value.m_pParameter->Get_Type() == PARAMETER_TYPE_Range )
	{
		double	v;

		if( wxPGVariantToDouble(childValue, &v) )
		{
			switch( childIndex )
			{
				case 0:	value.m_pParameter->asRange()->Set_LoVal(v);	break;
				case 1:	value.m_pParameter->asRange()->Set_HiVal(v);	break;
			}
		}
	}
}

//---------------------------------------------------------
bool CParameters_PG_Range::Update(void)
{
	SetValue(m_value);

	return( true );

//	s.Printf( SG_T("%f; %f"), pParameter->asRange()->Get_LoVal(), pParameter->asRange()->Get_HiVal());
//	m_pPG->SetPropertyValue(pProperty, s.c_str());
//	RefreshChildren();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Degree, wxPGProperty, CParameters_PG_Parameter_Value, const &CParameters_PG_Parameter_Value, TextCtrl)

//---------------------------------------------------------
CParameters_PG_Degree::CParameters_PG_Degree(const wxString &label, const wxString &name, CSG_Parameter *pParameter)
	: wxPGProperty(label, name)
{
	if( pParameter && pParameter->Get_Type() == PARAMETER_TYPE_Degree )
	{
		m_value	= CParameters_PG_Parameter_ValueToVariant(CParameters_PG_Parameter_Value(pParameter));

		double	d, m, s;

		Decimal_To_Degree(pParameter->asDouble(), d, m, s);

		AddPrivateChild( new wxIntProperty  (wxT("Degree")	, wxPG_LABEL, (int)d) );
		AddPrivateChild( new wxIntProperty  (wxT("Minute")	, wxPG_LABEL, (int)m) );
		AddPrivateChild( new wxFloatProperty(wxT("Second")	, wxPG_LABEL,      s) );
	}
}

//---------------------------------------------------------
void CParameters_PG_Degree::RefreshChildren(void)
{
	const CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(m_value);

	if( GetCount() == 3 && value.m_pParameter && value.m_pParameter->Get_Type() == PARAMETER_TYPE_Degree )
	{
		double	d, m, s;

		Decimal_To_Degree(value.m_pParameter->asDouble(), d, m, s);

		Item(0)->SetValue((int)d);
		Item(1)->SetValue((int)m);
		Item(2)->SetValue(     s);
	}
}

//---------------------------------------------------------
void CParameters_PG_Degree::ChildChanged(wxVariant &thisValue, int childIndex, wxVariant &childValue) const
{
	CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(thisValue);

	if( GetCount() == 3 && value.m_pParameter && value.m_pParameter->Get_Type() == PARAMETER_TYPE_Degree )
	{
		double	v, d, m, s;

		if( wxPGVariantToDouble(childValue, &v) )
		{
			Decimal_To_Degree(value.m_pParameter->asDouble(), d, m, s);

			switch( childIndex )
			{
				case 0:	d	= (int)v;	break;
				case 1:	m	= (int)v;	break;
				case 2:	s	=      v;	break;
			}

			value.m_pParameter->Set_Value(Degree_To_Decimal(d, m, s));
		}
	}
}

//---------------------------------------------------------
bool CParameters_PG_Degree::Update(void)
{
	SetValue(m_value);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Dialog, wxPGProperty, CParameters_PG_Parameter_Value, const CParameters_PG_Parameter_Value &, TextCtrlAndButton)

//---------------------------------------------------------
CParameters_PG_Dialog::CParameters_PG_Dialog(const wxString &label, const wxString &name, CSG_Parameter *pParameter)
	: wxPGProperty(pParameter ? GET_DATAOBJECT_LABEL(pParameter) : label, name)
{
	m_value	= CParameters_PG_Parameter_ValueToVariant(CParameters_PG_Parameter_Value(pParameter));
}

//---------------------------------------------------------
wxString CParameters_PG_Dialog::GetValueAsString(int flags) const
{
	const CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(m_value);

	if( value.m_pParameter )
	{
		return( value.to_String() );
	}

	return( wxT("---") );
}

//---------------------------------------------------------
bool CParameters_PG_Dialog::OnEvent(wxPropertyGrid *propgrid, wxWindow *primary, wxEvent &event)
{
	if( propgrid->IsMainButtonEvent(event) )
	{
		PrepareValueForDialogEditing(propgrid);

		CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(m_value);

		if( value.m_pParameter && value.Do_Dialog() )
		{
			propgrid->EditorsValueWasModified();

			SetValueInEvent(m_value);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters_PG_Dialog::Update(void)
{
	SetValue(m_value);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Colors, wxPGProperty, CParameters_PG_Parameter_Value, const CParameters_PG_Parameter_Value &, TextCtrlAndButton)

//---------------------------------------------------------
CParameters_PG_Colors::CParameters_PG_Colors(const wxString &label, const wxString &name, CSG_Parameter *pParameter)
	: CParameters_PG_Dialog(label, name, pParameter)
{}

//---------------------------------------------------------
void CParameters_PG_Colors::OnCustomPaint(wxDC &dc, const wxRect &r, wxPGPaintData &pd)
{
	const CParameters_PG_Parameter_Value	&value	= CParameters_PG_Parameter_ValueFromVariant(m_value);

	if( value.m_pParameter && value.m_pParameter->Get_Type() == PARAMETER_TYPE_Colors )
	{
		int		i, ax, bx;

		CSG_Colors	*pColors	= value.m_pParameter->asColors();
		double		dx			= r.GetWidth() / (double)pColors->Get_Count();

		for(i=0, bx=r.GetLeft(); i<pColors->Get_Count(); i++)
		{
			ax	= bx;
			bx	= r.GetLeft() + (int)(dx * (i + 1.0));

			Draw_FillRect(dc, Get_Color_asWX(pColors->Get_Color(i)), ax, r.GetTop(), bx, r.GetBottom());
		}
	}
}

//---------------------------------------------------------
wxSize CParameters_PG_Colors::OnMeasureImage(int item = -1) const
{
	return( wxSize(-1, -1) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
