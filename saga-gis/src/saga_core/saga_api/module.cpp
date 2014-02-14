/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      module.cpp                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "module.h"

#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module::CSG_Module(void)
{
	m_ID			= -1;

	m_bError_Ignore	= false;
	m_bExecutes		= false;

	m_pParameters	= NULL;
	m_npParameters	= 0;

	Parameters.Create(this, SG_T(""), SG_T(""));
	Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	Set_Show_Progress(true);
}

//---------------------------------------------------------
CSG_Module::~CSG_Module(void)
{
	if( m_pParameters )
	{
		for(int i=0; i<m_npParameters; i++)
		{
			delete(m_pParameters[i]);
		}

		SG_Free(m_pParameters);
	}

	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Destroy(void)
{
	m_bError_Ignore	= false;

	History_Supplement.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Set_Name(const CSG_String &String)
{
	Parameters.Set_Name(String);
}

const CSG_String & CSG_Module::Get_Name(void)
{
	return( Parameters.Get_Name() );
}

//---------------------------------------------------------
void CSG_Module::Set_Description(const CSG_String &String)
{
	Parameters.Set_Description(String);
}

const CSG_String & CSG_Module::Get_Description(void)
{
	return( Parameters.Get_Description() );
}

//---------------------------------------------------------
void CSG_Module::Set_Author(const CSG_String &String)
{
	m_Author	= String;
}

const CSG_String & CSG_Module::Get_Author(void)
{
	return( m_Author );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::Execute(void)
{
	if( m_bExecutes )
	{
		return( false );
	}

	m_bExecutes	= true;

	Destroy();

	bool	bResult	= false;

	Update_Parameter_States();

	//-----------------------------------------------------
	if( !Parameters.DataObjects_Create() )
	{
		Message_Dlg(_TL("could not initialize data objects"));
	}
	else
	{
		Parameters.Msg_String(false);

///////////////////////////////////////////////////////////
#if !defined(_DEBUG) && defined(_SAGA_VC) && defined(WXWIN_28)
#define _MODULE_EXCEPTION
		__try
		{
#endif
///////////////////////////////////////////////////////////

			bResult	= On_Execute();

///////////////////////////////////////////////////////////
#ifdef _MODULE_EXCEPTION
		}	// try
		__except(1)
		{
			Message_Dlg(SG_T("Tool caused access violation!"));
		}	// except(1)
#endif
///////////////////////////////////////////////////////////

		if( bResult )
		{
			_Set_Output_History();
		}

		if( !Process_Get_Okay(false) )
		{
			SG_UI_Msg_Add(_TL("Execution has been stopped by user!"), true);

			bResult	= false;
		}

		_Synchronize_DataObjects();
	}

	//-----------------------------------------------------
	Destroy();

	SG_UI_Process_Set_Ready();
	SG_UI_Process_Set_Okay();

	m_bExecutes	= false;

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::_Synchronize_DataObjects(void)
{
	int				i;
	CSG_Projection	Projection;

	Parameters.DataObjects_Synchronize();

	for(i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->DataObjects_Synchronize();
	}

	if( do_Sync_Projections() && Get_Projection(Projection) )
	{
		Parameters.DataObjects_Set_Projection(Projection);

		for(i=0; i<m_npParameters; i++)
		{
			m_pParameters[i]->DataObjects_Set_Projection(Projection);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Module::Get_Projection(CSG_Projection &Projection)	const
{
	Projection.Destroy();

	if( !Parameters.DataObjects_Get_Projection(Projection) )
	{
		return( false );
	}

	for(int i=0; i<m_npParameters; i++)
	{
		CSG_Projection	P;

		if( !m_pParameters[i]->DataObjects_Get_Projection(P) )
		{
			return( false );
		}
		else if( P.is_Okay() )
		{
			if( !Projection.is_Okay() )
			{
				Projection	= P;
			}
			else if( Projection != P )
			{
				return( false );
			}
		}
	}

	return( Projection.is_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Set_Manager(class CSG_Data_Manager *pManager)
{
	Parameters.Set_Manager(pManager);

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->Set_Manager(pManager);
	}
}

//---------------------------------------------------------
void CSG_Module::Set_Show_Progress(bool bOn)
{
	m_bShow_Progress	= bOn;
}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Module::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		if( Flags & PARAMETER_CHECK_VALUES )
		{
			((CSG_Module *)pParameter->Get_Owner()->Get_Owner())->
				On_Parameter_Changed(pParameter->Get_Owner(), pParameter);
		}

		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			((CSG_Module *)pParameter->Get_Owner()->Get_Owner())->
				On_Parameters_Enable(pParameter->Get_Owner(), pParameter);
		}

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Module::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( true );
}

//---------------------------------------------------------
int CSG_Module::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Extra Parameters					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CSG_Module::Add_Parameters(const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameters	*pParameters;

	m_pParameters	= (CSG_Parameters **)SG_Realloc(m_pParameters, (m_npParameters + 1) * sizeof(CSG_Parameters *));
	pParameters		= m_pParameters[m_npParameters++]	= new CSG_Parameters();

	pParameters->Create(this, Name, Description, Identifier);
	pParameters->Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	return( pParameters );
}

//---------------------------------------------------------
CSG_Parameters * CSG_Module::Get_Parameters(const CSG_String &Identifier)
{
	CSG_String	sIdentifier(Identifier);

	for(int i=0; i<m_npParameters; i++)
	{
		if( !sIdentifier.Cmp(m_pParameters[i]->Get_Identifier()) )
		{
			return( m_pParameters[i] );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Module::Dlg_Parameters(const CSG_String &Identifier)
{
	CSG_Parameters	*pParameters	= Get_Parameters(Identifier);

	if( pParameters && (!pParameters->is_Managed() || Dlg_Parameters(pParameters, Get_Name())) )
	{
		pParameters->Set_History(History_Supplement);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Module::Dlg_Parameters(CSG_Parameters *pParameters, const CSG_String &Caption)
{
	return( pParameters ? SG_UI_Dlg_Parameters(pParameters, Caption) : false );
}


///////////////////////////////////////////////////////////
//														 //
//						Progress						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::Process_Get_Okay(bool bBlink)
{
	return( SG_UI_Process_Get_Okay(bBlink) );
}

//---------------------------------------------------------
void CSG_Module::Process_Set_Text(const CSG_String &Text)
{
	SG_UI_Process_Set_Text(Text);
}

//---------------------------------------------------------
bool CSG_Module::Set_Progress(int Position)
{
	return( Set_Progress(Position, 100.0) );
}

//---------------------------------------------------------
bool CSG_Module::Set_Progress(double Position, double Range)
{
	return( m_bShow_Progress ? SG_UI_Process_Set_Progress(Position, Range) : Process_Get_Okay(false) );
}

//---------------------------------------------------------
bool CSG_Module::Stop_Execution(bool bDialog)
{
	return( SG_UI_Stop_Execution(bDialog) );
}


///////////////////////////////////////////////////////////
//														 //
//						Message							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Message_Dlg(const CSG_String &Text, const SG_Char *Caption)
{
	SG_UI_Dlg_Message(Text, Caption && Caption[0] != '\0' ? Caption : Get_Name().c_str());
}

//---------------------------------------------------------
bool CSG_Module::Message_Dlg_Confirm(const CSG_String &Text, const SG_Char *Caption)
{
	return( SG_UI_Dlg_Continue(Text, Caption && Caption[0] != '\0' ? Caption : Get_Name().c_str()) );
}

//---------------------------------------------------------
void CSG_Module::Message_Add(const CSG_String &Text, bool bNewLine)
{
	SG_UI_Msg_Add_Execution(Text, bNewLine);
}

//---------------------------------------------------------
bool CSG_Module::Error_Set(TSG_Module_Error Error_ID)
{
	switch( Error_ID )
	{
	default:
		return( Error_Set(_TL("Unknown Error")) );
	    
	case MODULE_ERROR_Calculation:
		return( Error_Set(_TL("Calculation Error")) );
	}
}

//---------------------------------------------------------
bool CSG_Module::Error_Set(const CSG_String &Error_Text)
{
	SG_UI_Msg_Add_Error(Error_Text);

	if( SG_UI_Process_Get_Okay(false) && !m_bError_Ignore )
	{
		switch( SG_UI_Dlg_Error(Error_Text, _TL("Error: Continue anyway ?")) )
		{
		case 0: default:
			SG_UI_Process_Set_Okay(false);
			break;

		case 1:
			m_bError_Ignore	= true;
			break;
		}
	}

	return( SG_UI_Process_Get_Okay(false) );
}


///////////////////////////////////////////////////////////
//														 //
//			DataObjects / GUI Interaction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::DataObject_Add(CSG_Data_Object *pDataObject, bool bUpdate)
{
	if( Parameters.Get_Manager() )
	{
		Parameters.Get_Manager()->Add(pDataObject);
	}

	return( SG_UI_DataObject_Add(pDataObject, bUpdate) );
}

//---------------------------------------------------------
bool CSG_Module::DataObject_Update(CSG_Data_Object *pDataObject, int Show)
{
	return( SG_UI_DataObject_Update(pDataObject, Show, NULL) );
}

bool CSG_Module::DataObject_Update(CSG_Data_Object *pDataObject, double Parm_1, double Parm_2, int Show)
{
	CSG_Parameters	Parameters;

	if( pDataObject )
	{
		switch( pDataObject->Get_ObjectType() )
		{
		default:
			break;

		case DATAOBJECT_TYPE_Grid:
			Parameters.Add_Range(NULL, SG_T("METRIC_ZRANGE"), SG_T(""), SG_T(""),
				Parm_1 * ((CSG_Grid *)pDataObject)->Get_ZFactor(),
				Parm_2 * ((CSG_Grid *)pDataObject)->Get_ZFactor()
			);
			break;
		}

		return( SG_UI_DataObject_Update(pDataObject, Show, &Parameters) );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Module::DataObject_Update_All(void)
{
	for(int i=0; i<Parameters.Get_Count(); i++)
	{
		if( Parameters(i)->is_Output() )
		{
			if( Parameters(i)->is_DataObject() )
			{
				DataObject_Update(Parameters(i)->asDataObject(), false);
			}
			else if( Parameters(i)->is_DataObject_List() )
			{
				for(int j=0; j<Parameters(i)->asList()->Get_Count(); j++)
				{
					DataObject_Update(Parameters(i)->asList()->asDataObject(j), false);
				}
			}
		}
	}
}

//---------------------------------------------------------
bool CSG_Module::DataObject_Get_Colors(CSG_Data_Object *pDataObject, CSG_Colors &Colors)
{
	return( SG_UI_DataObject_Colors_Get(pDataObject, &Colors) );
}

bool CSG_Module::DataObject_Set_Colors(CSG_Data_Object *pDataObject, const CSG_Colors &Colors)
{
	CSG_Colors	c(Colors);

	return( SG_UI_DataObject_Colors_Set(pDataObject, &c) );
}

bool CSG_Module::DataObject_Set_Colors(CSG_Data_Object *pDataObject, int nColors, int Palette, bool bRevert)
{
	CSG_Colors	c(nColors, Palette, bRevert);

	return( SG_UI_DataObject_Colors_Set(pDataObject, &c) );
}

//---------------------------------------------------------
bool CSG_Module::DataObject_Get_Parameters(CSG_Data_Object *pDataObject, CSG_Parameters &Parameters)
{
	return( SG_UI_DataObject_Params_Get(pDataObject, &Parameters) );
}

bool CSG_Module::DataObject_Set_Parameters(CSG_Data_Object *pDataObject, CSG_Parameters &Parameters)
{
	return( SG_UI_DataObject_Params_Set(pDataObject, &Parameters) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Module::DataObject_Get_Parameter(CSG_Data_Object *pDataObject, const CSG_String &ID)
{
	static CSG_Parameters	sParameters;

	return( DataObject_Get_Parameters(pDataObject, sParameters) ? sParameters(ID) : NULL );
}

bool CSG_Module::DataObject_Set_Parameter(CSG_Data_Object *pDataObject, CSG_Parameter *pParameter)
{
	CSG_Parameters	P;

	P._Add(pParameter);

	return( DataObject_Set_Parameters(pDataObject, P) );
}

bool CSG_Module::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, int            Value)
{
	CSG_Parameters	Parameters;

	if( DataObject_Get_Parameters(pDataObject, Parameters) && Parameters(ID) )
	{
		return( Parameters(ID)->Set_Value(Value) && DataObject_Set_Parameters(pDataObject, Parameters) );
	}

	return( false );
}

bool CSG_Module::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, double         Value)
{
	CSG_Parameters	Parameters;

	if( DataObject_Get_Parameters(pDataObject, Parameters) && Parameters(ID) )
	{
		return( Parameters(ID)->Set_Value(Value) && DataObject_Set_Parameters(pDataObject, Parameters) );
	}

	return( false );
}

bool CSG_Module::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, void          *Value)
{
	CSG_Parameters	Parameters;

	if( DataObject_Get_Parameters(pDataObject, Parameters) && Parameters(ID) )
	{
		return( Parameters(ID)->Set_Value(Value) && DataObject_Set_Parameters(pDataObject, Parameters) );
	}

	return( false );
}

bool CSG_Module::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, const SG_Char *Value)
{
	CSG_Parameters	Parameters;

	if( DataObject_Get_Parameters(pDataObject, Parameters) && Parameters(ID) )
	{
		return( Parameters(ID)->Set_Value(Value) && DataObject_Set_Parameters(pDataObject, Parameters) );
	}

	return( false );
}

bool CSG_Module::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, double loVal, double hiVal)	// Range Parameter
{
	CSG_Parameters	Parameters;

	if( DataObject_Get_Parameters(pDataObject, Parameters) && Parameters(ID) && Parameters(ID)->Get_Type() == PARAMETER_TYPE_Range )
	{
		return( Parameters(ID)->asRange()->Set_Range(loVal, hiVal) && DataObject_Set_Parameters(pDataObject, Parameters) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::Update_Parameter_States(void)
{
	_Update_Parameter_States(&Parameters);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		_Update_Parameter_States(Get_Parameters(i));
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Module::_Update_Parameter_States(CSG_Parameters *pParameters)
{
	if( pParameters )
	{
		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

			if( pParameter->Get_Type() == PARAMETER_TYPE_Parameters )
			{
				_Update_Parameter_States(pParameter->asParameters());
			}
			else
			{
				On_Parameters_Enable(pParameters, pParameter);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//						History							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::_Set_Output_History(void)
{
	CSG_MetaData	History;

	//-----------------------------------------------------
	History.Set_Name(SG_META_HST);

	History.Add_Child(SG_T("MODULE")	, Get_Name());
//	History.Add_Child(SG_T("LIBRARY")	, Get_Library());

	Parameters.Set_History(History);

	History.Assign(History_Supplement, true);

	History.Del_Children(SG_Get_History_Depth());

	//-----------------------------------------------------
	for(int j=-1; j<Get_Parameters_Count(); j++)
	{
		CSG_Parameters	*pParameters	= j < 0 ? &Parameters : Get_Parameters(j);

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter	*p	= pParameters->Get_Parameter(i);

			if( p->is_Output() && p->is_DataObject() && p->asDataObject() )
			{
				p->asDataObject()->Get_History().Assign(History);
			}

			if( p->is_Output() && p->is_DataObject_List() )
			{
				for(int j=0; j<p->asList()->Get_Count(); j++)
				{
					p->asList()->asDataObject(j)->Get_History().Assign(History);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Summary							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void _Add_XML(CSG_MetaData *pParent, CSG_Parameter *pParameter, CSG_String ID = "")
{
	if( !ID.is_Empty() )	ID	+= "_";	ID	+= pParameter->Get_Identifier();

	CSG_MetaData	*pItem	= pParent->Add_Child(SG_XML_PARAM);

	pItem->Add_Property(SG_XML_PARAM_ATT_NAME , pParameter->Get_Name());
	pItem->Add_Property(SG_XML_PARAM_ATT_CLASS,	pParameter->is_Input() ? "input" : pParameter->is_Output() ? "output" : "option");

	pItem->Add_Child(SG_XML_PARAM_IDENT, ID);
	pItem->Add_Child(SG_XML_PARAM_TYPE , pParameter->Get_Type_Name().Make_Lower());
	pItem->Add_Child(SG_XML_PARAM_MAND , pParameter->is_Optional() ? SG_T("false") : SG_T("true"));
	pItem->Add_Child(SG_XML_DESCRIPTION, pParameter->Get_Description());

	switch( pParameter->Get_Type() )
	{
	//-----------------------------------------------------
	case PARAMETER_TYPE_Choice:
		{
			CSG_MetaData	*pChild	= pItem->Add_Child(SG_XML_PARAM_LIST);

			for(int i=0; i<pParameter->asChoice()->Get_Count(); i++)
			{
				pChild->Add_Child(SG_XML_PARAM_ITEM, pParameter->asChoice()->Get_Item(i));
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_FixedTable:
		{
			CSG_MetaData	*pChild	= pItem->Add_Child(SG_XML_PARAM_TABLE);

			for(int i=0; i<pParameter->asTable()->Get_Field_Count(); i++)
			{
				CSG_MetaData	*pField	= pChild->Add_Child(SG_XML_PARAM_FIELD);

				pField->Add_Property(SG_XML_PARAM_FIELD_ATT_NAME, pParameter->asTable()->Get_Field_Name(i));
				pField->Add_Property(SG_XML_PARAM_FIELD_ATT_TYPE, SG_Data_Type_Get_Name(pParameter->asTable()->Get_Field_Type(i)));
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Int:
	case PARAMETER_TYPE_Double:
	case PARAMETER_TYPE_Degree:
		if( pParameter->asValue()->has_Minimum() )	pItem->Add_Child(SG_XML_PARAM_MIN, pParameter->asValue()->Get_Minimum());
		if( pParameter->asValue()->has_Maximum() )	pItem->Add_Child(SG_XML_PARAM_MAX, pParameter->asValue()->Get_Maximum());
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Parameters:
		{
			for(int i=0; i<pParameter->asParameters()->Get_Count(); i++)
			{
				_Add_XML(pItem, pParameter->asParameters()->Get_Parameter(i), ID);
			}
		}
	default:
		break;
	}
}

//---------------------------------------------------------
#define SUMMARY_ADD_STR(label, value)	s += CSG_String::Format(SG_T("<tr><td valign=\"top\">%s</td><td valign=\"top\">%s</td></tr>"), label, value)
#define SUMMARY_ADD_INT(label, value)	s += CSG_String::Format(SG_T("<tr><td valign=\"top\">%s</td><td valign=\"top\">%d</td></tr>"), label, value)

//---------------------------------------------------------
CSG_String CSG_Module::Get_Summary(bool bParameters, const CSG_String &Menu, const CSG_String &Description, bool bXML)
{
	int			i;
	CSG_String	s;

	//-----------------------------------------------------
	if( bXML )
	{
		CSG_MetaData	m;

		m.Set_Name    (SG_XML_MODULE);
		m.Add_Property(SG_XML_MODULE_ATT_NAME  , Get_Name       ());
		m.Add_Property(SG_XML_MODULE_ATT_ID    , Get_ID         ());
		m.Add_Property(SG_XML_MODULE_ATT_AUTHOR, Get_Author     ());
		m.Add_Child   (SG_XML_DESCRIPTION      , Get_Description());
		m.Add_Child   (SG_XML_MENU             , Get_MenuPath   ());
		m.Add_Child   (SG_XML_SPEC_ATT_GRID    , is_Grid        () ? SG_T("true") : SG_T("false"));
		m.Add_Child   (SG_XML_SPEC_ATT_GRID    , is_Interactive () ? SG_T("true") : SG_T("false"));

	//	CSG_MetaData	*pChild	= m.Add_Child(SG_XML_SPECIFICATION);
	//	pChild->Add_Property(SG_XML_SPEC_ATT_GRID    , is_Grid        () ? SG_T("true") : SG_T("false"));
	//	pChild->Add_Property(SG_XML_SPEC_ATT_INTERA  , is_Interactive () ? SG_T("true") : SG_T("false"));

		if( bParameters )
		{
			for(i=0; i<Parameters.Get_Count(); i++)
			{
				if( Parameters(i)->is_Input() )
 				{
					_Add_XML(&m, Parameters(i));
				}
			}

			for(i=0; i<Parameters.Get_Count(); i++)
			{
				if( Parameters(i)->is_Output() )
 				{
					_Add_XML(&m, Parameters(i));
				}
			}

			for(i=0; i<Parameters.Get_Count(); i++)
			{
	 			if( Parameters(i)->is_Option()
				&&  Parameters(i)->Get_Type() != PARAMETER_TYPE_Node
				&&  Parameters(i)->Get_Type() != PARAMETER_TYPE_Grid_System )
 				{
					_Add_XML(&m, Parameters(i));
				}
			}
		}

		s	= m.asText(1);
	}

	//-----------------------------------------------------
	else
	{
		s	+= CSG_String::Format(SG_T("<b>%s</b><table border=\"0\">"), _TL("Tool"));

		SUMMARY_ADD_STR(_TL("Name"  ), Get_Name  ().c_str());
		SUMMARY_ADD_INT(_TL("ID"    ), Get_ID    ());
		SUMMARY_ADD_STR(_TL("Author"), Get_Author().c_str());

		if( is_Interactive() && is_Grid() )
		{
			SUMMARY_ADD_STR(_TL("Specification"), CSG_String::Format(SG_T("%s, %s"), _TL("grid"), _TL("interactive")).c_str() );
		}
		else if( is_Interactive() )
		{
			SUMMARY_ADD_STR(_TL("Specification"), _TL("interactive"));
		}
		else if( is_Grid() )
		{
			SUMMARY_ADD_STR(_TL("Specification"), _TL("grid"));
		}

		if( Menu.Length() > 0 )
		{
			CSG_String	sMenu(Menu);

			sMenu.Replace(SG_T("|"), SG_T(" <b>></b> "));

			SUMMARY_ADD_STR(_TL("Menu"  ), sMenu.c_str());
		}

		s	+= SG_T("</table><hr>");

		//-------------------------------------------------
		s	+= CSG_String::Format(SG_T("<b>%s</b><br>"), _TL("Description"));

		s	+= Description.Length() > 0 ? Description : Get_Description();

		//-------------------------------------------------
		if( bParameters )
		{
			bool	bFirst, bOptionals	= false;

			s	+= CSG_String::Format(SG_T("<hr><b>%s</b><br>"), _TL("Parameters"));
			s	+= CSG_String::Format(SG_T("<table border=\"1\" width=\"100%%\" valign=\"top\" cellpadding=\"5\" rules=\"all\"><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n"),
					_TL("Name"), _TL("Type"), _TL("Identifier"), _TL("Description"), _TL("Constraints")
				);

			//---------------------------------------------
			for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
			{
				CSG_Parameter	*pParameter	= Parameters(i);

				if( pParameter->is_Input() )
				{
					if( bFirst )
					{
						bFirst	= false;
						s	+= CSG_String::Format(SG_T("<tr><th colspan=\"5\">%s</th></tr>"), _TL("Input"));
					}

					s	+= CSG_String::Format(SG_T("<tr><td>%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"),
						pParameter->Get_Name(),
						pParameter->is_Optional() ? SG_T(" (*)") : SG_T(" "),
						pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
						pParameter->Get_Identifier(),
						pParameter->Get_Description(),
						pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
					);
				}
			}

			//---------------------------------------------
			for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
			{
				CSG_Parameter	*pParameter	= Parameters(i);

				if( pParameter->is_Output() )
				{
					if( bFirst )
					{
						bFirst	= false;
						s	+= CSG_String::Format(SG_T("<tr><th colspan=\"5\">%s</th></tr>"), _TL("Output"));
					}

					s	+= CSG_String::Format(SG_T("<tr><td>%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"),
						pParameter->Get_Name(),
						pParameter->is_Optional() ? SG_T(" (*)") : SG_T(""),
						pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
						pParameter->Get_Identifier(),
						pParameter->Get_Description(),
						pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
					);
				}
			}

			//---------------------------------------------
			for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
			{
				CSG_Parameter	*pParameter	= Parameters(i);

				if( pParameter->is_Option() && pParameter->Get_Type() != PARAMETER_TYPE_Grid_System )
				{
					if( bFirst )
					{
						bFirst	= false;
						s	+= CSG_String::Format(SG_T("<tr><th colspan=\"5\">%s</th></tr>"), _TL("Options"));
					}

					s	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"),
						pParameter->Get_Name(),
						pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
						pParameter->Get_Identifier(),
						pParameter->Get_Description(),
						pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
					);
				}
				else if( pParameter->is_Optional() )
				{
					bOptionals	= true;
				}
			}

			//---------------------------------------------
			s	+= SG_T("</table>");

			if( bOptionals )
			{
				s	+= CSG_String::Format(SG_T("(*) <i>%s</i>"), _TL("optional"));
			}
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
