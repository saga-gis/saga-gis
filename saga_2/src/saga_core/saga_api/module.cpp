
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
#include "module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module::CSG_Module(void)
{
	m_bError_Ignore	= false;
	m_bExecutes		= false;

	m_Garbage		= NULL;
	m_nGarbage		= 0;

	m_pParameters	= NULL;
	m_npParameters	= 0;

	Parameters.Create(this, SG_T(""), SG_T(""));
	Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	Set_Managed			(false);
	Set_Show_Progress	(true);
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

	Garbage_Clear();

	if( m_bManaged )
	{
		SG_UI_Process_Set_Okay();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Set_Name(const SG_Char *String)
{
	Parameters.Set_Name(String);
}

const SG_Char * CSG_Module::Get_Name(void)
{
	return( Parameters.Get_Name() );
}

//---------------------------------------------------------
void CSG_Module::Set_Description(const SG_Char *String)
{
	Parameters.Set_Description(String);
}

const SG_Char * CSG_Module::Get_Description(void)
{
	return( Parameters.Get_Description() );
}

//---------------------------------------------------------
void CSG_Module::Set_Author(const SG_Char *String)
{
	if( String )
	{
		m_Author.Printf(String);
	}
	else
	{
		m_Author.Clear();
	}
}

const SG_Char * CSG_Module::Get_Author(void)
{
	return( m_Author.c_str() );
}

//---------------------------------------------------------
void CSG_Module::Set_Translation(CSG_Translator &Translator)
{
	Parameters.Set_Translation(Translator);

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->Set_Translation(Translator);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::Execute(void)
{
	bool	bResult	= false;

	if( m_bExecutes == false )
	{
		m_bExecutes		= true;

		if( Parameters.DataObjects_Check() )
		{
			for(int i=0; i<m_npParameters; i++)
			{
				m_pParameters[i]->DataObjects_Check();
			}

			Destroy();

			Parameters.DataObjects_Create();
			Parameters.Msg_String(false);

///////////////////////////////////////////////////////////
#if !defined(_DEBUG) && defined(_SAGA_VC)
#define _MODULE_EXCEPTION
__try
{
#endif
///////////////////////////////////////////////////////////

			if( (bResult = On_Execute()) == true )
			{
				_Set_Output_History();
			}

			if( !Process_Get_Okay(false) )
			{
				SG_UI_Msg_Add(LNG("[MSG] Execution has been stopped by user!"), true);
			}

///////////////////////////////////////////////////////////
#ifdef _MODULE_EXCEPTION
}	// try
__except(1)
{
	Message_Add(LNG("[ERR] Module caused access violation!"));
	Message_Dlg(LNG("[ERR] Module caused access violation!"));
	bResult	= false;
}	// except(1)
#endif
///////////////////////////////////////////////////////////

			Destroy();

			Parameters.DataObjects_Synchronize();
		}

		m_bExecutes		= false;
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Set_Managed(bool bOn)
{
	m_bManaged	= Parameters.m_bManaged	= bOn;

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->m_bManaged	= bOn;
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
int CSG_Module::_On_Parameter_Changed(CSG_Parameter *pParameter)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		return( ((CSG_Module *)pParameter->Get_Owner()->Get_Owner())->
			On_Parameter_Changed(pParameter->Get_Owner(), pParameter)
		);
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Module::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Extra Parameters					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CSG_Module::Add_Parameters(const SG_Char *Identifier, const SG_Char *Name, const SG_Char *Description)
{
	CSG_Parameters	*pParameters;

	m_pParameters	= (CSG_Parameters **)SG_Realloc(m_pParameters, (m_npParameters + 1) * sizeof(CSG_Parameters *));
	pParameters		= m_pParameters[m_npParameters++]	= new CSG_Parameters();

	pParameters->Create(this, Name, Description, Identifier);
	pParameters->Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	return( pParameters );
}

//---------------------------------------------------------
CSG_Parameters * CSG_Module::Get_Parameters(const SG_Char *Identifier)
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
bool CSG_Module::Dlg_Parameters(const SG_Char *Identifier)
{
	if( !m_bManaged || Dlg_Parameters(Get_Parameters(Identifier), Get_Name()) )
	{
		Get_Parameters(Identifier)->Set_History(History_Supplement);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Module::Dlg_Parameters(CSG_Parameters *pParameters, const SG_Char *Caption)
{
	return( pParameters ? SG_UI_Dlg_Parameters(pParameters, Caption) : false );
}

//---------------------------------------------------------
#ifdef _SAGA_UNICODE

CSG_Parameters *	CSG_Module::Add_Parameters(const char *Identifier, const SG_Char *Name, const SG_Char *Description)
{	return( Add_Parameters(CSG_String(Identifier), Name, Description) );	}

CSG_Parameters *	CSG_Module::Get_Parameters(const char *Identifier)
{	return( Get_Parameters(CSG_String(Identifier)) );	}

bool				CSG_Module::Dlg_Parameters(const char *Identifier)
{	return( Dlg_Parameters(CSG_String(Identifier)) );	}

#endif


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
void CSG_Module::Process_Set_Text(const SG_Char *Text)
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


///////////////////////////////////////////////////////////
//														 //
//						Message							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module::Message_Dlg(const SG_Char *Text, const SG_Char *Caption)
{
	SG_UI_Dlg_Message(Text, Caption && Caption[0] != '\0' ? Caption : Get_Name());
}

//---------------------------------------------------------
bool CSG_Module::Message_Dlg_Confirm(const SG_Char *Text, const SG_Char *Caption)
{
	return( SG_UI_Dlg_Continue(Text, Caption && Caption[0] != '\0' ? Caption : Get_Name()) );
}

//---------------------------------------------------------
void CSG_Module::Message_Add(const SG_Char *Text, bool bNewLine)
{
	SG_UI_Msg_Add_Execution(Text, bNewLine);
}

//---------------------------------------------------------
bool CSG_Module::Error_Set(TSG_Module_Error Error_ID)
{
	switch( Error_ID )
	{
	default:
		return( Error_Set(LNG("[ERR] Unknown Error")) );
	    
	case MODULE_ERROR_Calculation:
		return( Error_Set(LNG("[ERR] Calculation Error")) );
	}
}

//---------------------------------------------------------
bool CSG_Module::Error_Set(const SG_Char *Error_Text)
{
	SG_UI_Msg_Add_Error(Error_Text);

	if( SG_UI_Process_Get_Okay(false) && !m_bError_Ignore )
	{
		switch( SG_UI_Dlg_Error(Error_Text, LNG("[ERR] Error: Continue anyway ?")) )
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
//				DataObjects / Garbage					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::_Garbage_Add_Item(CSG_Data_Object *pDataObject)
{
	if( pDataObject )
	{
		for(int i=0; i<m_nGarbage; i++)
		{
			if( pDataObject == m_Garbage[i] )
			{
				return( true );
			}
		}

		m_Garbage	= (CSG_Data_Object **)SG_Realloc(m_Garbage, (m_nGarbage + 1) * sizeof(CSG_Data_Object *));
		m_Garbage[m_nGarbage++]	= pDataObject;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Data_Object * CSG_Module::Garbage_Del_Item(int i, bool bFromListOnly)
{
	CSG_Data_Object	*pDataObject	= NULL;

	if( i >= 0 && i < m_nGarbage )
	{
		if( bFromListOnly || !m_Garbage[i] )
		{
			pDataObject	= m_Garbage[i];
		}
		else if( m_Garbage[i] )
		{
			delete(m_Garbage[i]);
		}

		for(m_nGarbage--; i<m_nGarbage; i++)
		{
			m_Garbage[i]	= m_Garbage[i + 1];
		}

		m_Garbage	= (CSG_Data_Object **)SG_Realloc(m_Garbage, m_nGarbage * sizeof(CSG_Data_Object *));
	}

	return( pDataObject );
}

//---------------------------------------------------------
void CSG_Module::Garbage_Clear(void)
{
	if( m_nGarbage > 0 )
	{
		for(int i=0; i<m_nGarbage; i++)
		{
			if( m_Garbage[i] )
			{
				delete(m_Garbage[i]);
			}
		}

		SG_Free(m_Garbage);
	}

	m_nGarbage	= 0;
	m_Garbage	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//				DataObjects / GUI Interaction			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module::DataObject_Add(CSG_Data_Object *pDataObject, bool bUpdate)
{
	return( m_bManaged && SG_UI_DataObject_Add(pDataObject, bUpdate)
		? true : _Garbage_Add_Item(pDataObject)
	);
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
