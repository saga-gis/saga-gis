
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
CModule::CModule(void)
{
	m_bError_Ignore	= false;
	m_bExecutes		= false;

	m_pParameters	= NULL;
	m_npParameters	= 0;

	Parameters.Create(this, "", "");
	Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	Set_Managed			(false);
	Set_Show_Progress	(true);
}

//---------------------------------------------------------
CModule::~CModule(void)
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
void CModule::Destroy(void)
{
	m_bError_Ignore	= false;

	History_Supplement.Destroy();

	if( m_bManaged )
	{
		SG_Callback_Process_Set_Okay();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule::Set_Name(const char *String)
{
	Parameters.Set_Name(String);
}

const char * CModule::Get_Name(void)
{
	return( Parameters.Get_Name() );
}

//---------------------------------------------------------
void CModule::Set_Description(const char *String)
{
	Parameters.Set_Description(String);
}

const char * CModule::Get_Description(void)
{
	return( Parameters.Get_Description() );
}

//---------------------------------------------------------
void CModule::Set_Author(const char *String)
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

const char * CModule::Get_Author(void)
{
	return( m_Author.c_str() );
}

//---------------------------------------------------------
void CModule::Set_Translation(CSG_Translator &Translator)
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
bool CModule::Execute(void)
{
	bool	bResult	= false;

///////////////////////////////////////////////////////////
#if !defined(_DEBUG) && defined(_SAGA_VC)
#define _MODULE_EXCEPTION
_try 
{
#endif
///////////////////////////////////////////////////////////

	if( m_bExecutes == false )
	{
		m_bExecutes		= true;

		if( Parameters.DataObjects_Check() )
		{
			Destroy();

			Parameters.DataObjects_Create();
			Parameters.Msg_String(false);

			if( (bResult = On_Execute()) == true )
			{
				_Set_Output_History();
			}

			if( !Process_Get_Okay(false) )
			{
				SG_Callback_Message_Add(LNG("[MSG] Execution has been stopped by user!"), true);
			}

			Destroy();

			Parameters.DataObjects_Synchronize();
		}

		m_bExecutes		= false;
	}

///////////////////////////////////////////////////////////
#ifdef _MODULE_EXCEPTION
}
_except(1)
{
	Message_Add(Get_Name());
	Message_Add(LNG("[ERR] Access Violation"));
	Message_Dlg(LNG("[ERR] Access Violation"));
}
#endif
///////////////////////////////////////////////////////////

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule::Set_Managed(bool bOn)
{
	m_bManaged	= Parameters.m_bManaged	= bOn;

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->m_bManaged	= bOn;
	}
}

//---------------------------------------------------------
void CModule::Set_Show_Progress(bool bOn)
{
	m_bShow_Progress	= bOn;
}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule::Dlg_Parameters(CParameters *pParameters, const char *Caption)
{
	return( SG_Callback_Dlg_Parameters(pParameters, Caption) );
}

//---------------------------------------------------------
int CModule::_On_Parameter_Changed(CParameter *pParameter)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		return( ((CModule *)pParameter->Get_Owner()->Get_Owner())->
			On_Parameter_Changed(pParameter->Get_Owner(), pParameter)
		);
	}

	return( 0 );
}

//---------------------------------------------------------
int CModule::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Extra Parameters					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters * CModule::Add_Extra_Parameters(const char *Identifier, const char *Name, const char *Description)
{
	CParameters	*pParameters;

	m_pParameters	= (CParameters **)SG_Realloc(m_pParameters, (m_npParameters + 1) * sizeof(CParameters *));
	pParameters		= m_pParameters[m_npParameters++]	= new CParameters();

	pParameters->Create(this, Name, Description, Identifier);
	pParameters->Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	return( pParameters );
}

//---------------------------------------------------------
CParameters * CModule::Get_Extra_Parameters(const char *Identifier)
{
	int			i;
	CSG_String	sIdentifier(Identifier);

	for(i=0; i<m_npParameters; i++)
	{
		if( !sIdentifier.Cmp(m_pParameters[i]->Get_Identifier()) )
		{
			return( m_pParameters[i] );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
int CModule::Dlg_Extra_Parameters(const char *Identifier)
{
	if( !m_bManaged || Dlg_Parameters(Get_Extra_Parameters(Identifier), Get_Name()) )
	{
		Get_Extra_Parameters(Identifier)->Set_History(History_Supplement);

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//						Progress						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule::Process_Get_Okay(bool bBlink)
{
	return( SG_Callback_Process_Get_Okay(bBlink) );
}

//---------------------------------------------------------
void CModule::Process_Set_Text(const char *Text)
{
	SG_Callback_Process_Set_Text(Text);
}

//---------------------------------------------------------
bool CModule::Set_Progress(int Position)
{
	return( Set_Progress(Position, 100.0) );
}

//---------------------------------------------------------
bool CModule::Set_Progress(double Position, double Range)
{
	return( m_bShow_Progress ? SG_Callback_Process_Set_Progress(Position, Range) : Process_Get_Okay(false) );
}


///////////////////////////////////////////////////////////
//														 //
//						Message							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule::Message_Dlg(const char *Text, const char *Caption)
{
	if( Caption && Caption[0] != '\0' )
	{
		SG_Callback_Dlg_Message(Text, Caption);
	}
	else
	{
		SG_Callback_Dlg_Message(Text, Get_Name());
	}
}

//---------------------------------------------------------
void CModule::Message_Add(const char *Text, bool bNewLine)
{
	SG_Callback_Message_Add_Execution(Text, bNewLine);
}

//---------------------------------------------------------
bool CModule::Error_Set(TModule_Error Error_ID)
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
bool CModule::Error_Set(const char *Error_Text)
{
	SG_Callback_Message_Add_Error(Error_Text);

	if( SG_Callback_Process_Get_Okay(false) && !m_bError_Ignore )
	{
		switch( SG_Callback_Dlg_Error(Error_Text, LNG("[ERR] Error: Continue anyway ?")) )
		{
		case 0: default:
			SG_Callback_Process_Set_Okay(false);
			break;

		case 1:
			m_bError_Ignore	= true;
			break;
		}
	}

	return( SG_Callback_Process_Get_Okay(false) );
}


///////////////////////////////////////////////////////////
//														 //
//				DataObjects / GUI Interaction			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule::DataObject_Add(CDataObject *pDataObject, bool bUpdate)
{
	return( m_bManaged ? SG_Callback_DataObject_Add(pDataObject, bUpdate) : false );
}

//---------------------------------------------------------
bool CModule::DataObject_Update(CDataObject *pDataObject, bool bShow)
{
	return( SG_Callback_DataObject_Update(pDataObject, bShow, NULL) );
}

bool CModule::DataObject_Update(CDataObject *pDataObject, double Parm_1, double Parm_2, bool bShow)
{
	CParameters	Parameters;

	if( pDataObject )
	{
		switch( pDataObject->Get_ObjectType() )
		{
		default:
			break;

		case DATAOBJECT_TYPE_Grid:
			Parameters.Add_Range(NULL, "METRIC_ZRANGE"	, "", "",
				Parm_1 * ((CGrid *)pDataObject)->Get_ZFactor(),
				Parm_2 * ((CGrid *)pDataObject)->Get_ZFactor()
			);
			break;
		}

		return( SG_Callback_DataObject_Update(pDataObject, bShow, &Parameters) );
	}

	return( false );
}

//---------------------------------------------------------
void CModule::DataObject_Update_All(void)
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
bool CModule::DataObject_Get_Colors(CDataObject *pDataObject, CSG_Colors &Colors)
{
	return( SG_Callback_DataObject_Colors_Get(pDataObject, &Colors) );
}

bool CModule::DataObject_Set_Colors(CDataObject *pDataObject, CSG_Colors &Colors)
{
	return( SG_Callback_DataObject_Colors_Set(pDataObject, &Colors) );
}


///////////////////////////////////////////////////////////
//														 //
//						History							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule::_Set_Output_History(void)
{
	CSG_History	History;

	//-----------------------------------------------------
	History.Add_Entry(LNG("[HST] Created with module"), Get_Name());

	Parameters.Set_History(History);

	History.Assign(History_Supplement, true);

	//-----------------------------------------------------
	for(int i=0; i<Parameters.Get_Count(); i++)	// set output history...
	{
		CParameter	*p	= Parameters(i);

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
