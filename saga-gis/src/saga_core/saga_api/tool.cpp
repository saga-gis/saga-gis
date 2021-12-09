
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
//                       tool.cpp                        //
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "saga_api.h"
#include "data_manager.h"
#include "tool.h"
#include "tool_chain.h"

#include <wx/string.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_MESSAGE_EXECUTION(Text, Style)	{ SG_UI_Msg_Add(Text, true, Style); if( has_GUI() ) { SG_UI_Msg_Add_Execution(Text, true, Style); } }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool::CSG_Tool(void)
{
	m_ID			= "-1";
	m_Version		= "1.0";

	m_bError_Ignore	= false;
	m_bExecutes		= false;
	m_bWithGUI		= true;

	m_pParameters	= NULL;
	m_npParameters	= 0;

	Parameters.Create(this, SG_T("Tool"));
	Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);
	Parameters.m_pTool	= this;

	Set_Show_Progress(true);
}

//---------------------------------------------------------
CSG_Tool::~CSG_Tool(void)
{
	if( m_Settings_Stack.Get_Size() > 0 )
	{
		for(size_t i=0; i<m_Settings_Stack.Get_Size(); i++)
		{
			delete(((CSG_Parameters **)m_Settings_Stack.Get_Array())[i]);
		}
	}

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Destroy(void)
{
	m_bError_Ignore	= false;

	History_Supplement.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_String & CSG_Tool::Get_Library(void) const
{
	return( m_Library );
}

//---------------------------------------------------------
const CSG_String & CSG_Tool::Get_File_Name(void) const
{
	return( m_File_Name );
}

//---------------------------------------------------------
void CSG_Tool::Set_Name(const CSG_String &String)
{
	Parameters.Set_Name(String);
}

const CSG_String & CSG_Tool::Get_Name(void) const
{
	return( Parameters.Get_Name() );
}

//---------------------------------------------------------
void CSG_Tool::Set_Author(const CSG_String &String)
{
	m_Author	= String;
}

const CSG_String & CSG_Tool::Get_Author(void) const
{
	return( m_Author );
}

//---------------------------------------------------------
void CSG_Tool::Set_Version(const CSG_String &String)
{
	m_Version	= String;
}

const CSG_String & CSG_Tool::Get_Version(void) const
{
	return( m_Version );
}

//---------------------------------------------------------
void CSG_Tool::Set_Description(const CSG_String &String)
{
	Parameters.Set_Description(String);
}

const CSG_String & CSG_Tool::Get_Description(void) const
{
	return( Parameters.Get_Description().is_Empty() ? Get_Name() : Parameters.Get_Description() );
}

//---------------------------------------------------------
void CSG_Tool::Add_Reference(const CSG_String &Authors, const CSG_String &Year, const CSG_String &Title, const CSG_String &Where, const SG_Char *Link, const SG_Char *Link_Text)
{
	Parameters.Add_Reference(Authors, Year, Title, Where, Link, Link_Text);
}

void CSG_Tool::Add_Reference(const CSG_String &Link, const SG_Char *Link_Text)
{
	Parameters.Add_Reference(Link, Link_Text);
}

const CSG_Strings & CSG_Tool::Get_References(void) const
{
	return( Parameters.Get_References() );
}

//---------------------------------------------------------
CSG_String CSG_Tool::Get_MenuPath(bool bSolved)
{
	if( !bSolved )
	{
		return( Get_MenuPath() );
	}

	CSG_String	Menu	= Get_MenuPath();

	if( Menu.Length() > 1 && Menu[1] == ':' )
	{
		if( Menu[0] == 'A' || Menu[0] == 'a' )	// absolute menu path, overwrites library's default menu path
		{
			return( Menu.AfterFirst(':') );
		}

		Menu	= Menu.AfterFirst(':');	// Menu[0] == 'R' || Menu[0] == 'r'	// menu path explicitly declared as relative to library's default menu path
	}

	if( m_Library_Menu.is_Empty() )
	{
		return( Menu );
	}

	if( Menu.is_Empty() )
	{
		return( m_Library_Menu );
	}

	return( m_Library_Menu + "|" + Menu );
}

//---------------------------------------------------------
bool CSG_Tool::has_GUI(void) const
{
	return( m_bWithGUI && SG_UI_Get_Window_Main() != NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Execute(bool bAddHistory)
{
	if( m_bExecutes )
	{
		return( false );
	}

	m_bExecutes	= true;

	//-----------------------------------------------------
	if( Parameters.Get_Manager() == &SG_Get_Data_Manager() )
	{
		ADD_MESSAGE_EXECUTION(CSG_String::Format("[%s] %s...", Get_Name().c_str(), _TL("Execution started")), SG_UI_MSG_STYLE_SUCCESS);
	}

	Destroy();

	m_Execution_Info.Clear();

	Update_Parameter_States();

	if( !Parameters.DataObjects_Create() )
	{
		Message_Dlg(_TL("could not initialize data objects"));

		Destroy();

		m_bExecutes	= false;

		return( false );
	}

	Parameters.Msg_String(false);

///////////////////////////////////////////////////////////
#if !defined(_DEBUG) && !defined(_OPENMP) && defined(_SAGA_MSW)
#define _TOOL_EXCEPTION
	__try
	{
#endif
///////////////////////////////////////////////////////////

	CSG_DateTime Started(CSG_DateTime::Now());
	bool bResult = On_Execute();
	CSG_TimeSpan Span = CSG_DateTime::Now() - Started;

///////////////////////////////////////////////////////////
#ifdef _TOOL_EXCEPTION
	}	// try
	__except(1)
	{
		Message_Dlg("Tool caused access violation!");
	}	// except(1)
#endif
///////////////////////////////////////////////////////////

	if( !Process_Get_Okay(false) )
	{
		SG_UI_Process_Set_Okay();

		SG_UI_Msg_Add(_TL("Execution has been stopped by user!"), true, SG_UI_MSG_STYLE_BOLD);

		bResult	= false;
	}

	if( bResult && bAddHistory )
	{
		_Set_Output_History();
	}

	_Synchronize_DataObjects();

	//-----------------------------------------------------
	Destroy();

	m_bExecutes	= false;

	SG_UI_Process_Set_Okay(); SG_UI_Process_Set_Ready();

	//---------------------------------------------------------
	if( is_Interactive() )
	{
		if( bResult )
		{
			CSG_String Text(CSG_String::Format("\n%s...", _TL("Interactive tool started and is waiting for user input.")));

			SG_UI_Msg_Add          (Text, false, SG_UI_MSG_STYLE_BOLD);
			SG_UI_Msg_Add_Execution(Text, false, SG_UI_MSG_STYLE_BOLD);
		}
		else
		{
			ADD_MESSAGE_EXECUTION(_TL("Interactive tool initialization failed."), SG_UI_MSG_STYLE_FAILURE);
		}
	}
	else
	{
		CSG_String Time =
		  Span.Get_Hours       () >= 1 ? (Span.Format("%Hh %Mm %Ss"))
		: Span.Get_Minutes     () >= 1 ? (Span.Format(    "%Mm %Ss"))
		: Span.Get_Seconds     () >= 1 ? (Span.Format(        "%Ss"))
		: Span.Get_Milliseconds() >= 1 ? (Span.Format("%l ") + _TL("milliseconds"))
		: CSG_String(_TL("less than 1 millisecond"));

		if( Parameters.Get_Manager() != &SG_Get_Data_Manager() )
		{
			SG_UI_Msg_Add_Execution(CSG_String::Format("\n[%s] %s: %s", Get_Name().c_str(),
				_TL("execution time"), Time.c_str()),
				false, SG_UI_MSG_STYLE_NORMAL
			);
		}
		else
		{
			SG_UI_Msg_Add_Execution(CSG_String::Format("\n__________\n%s %s: %ld %s (%s)\n", _TL("total"),
				_TL("execution time"), Span.Get_Milliseconds(), _TL("milliseconds"), Time.c_str()),
				false, SG_UI_MSG_STYLE_BOLD
			);

			ADD_MESSAGE_EXECUTION(CSG_String::Format("[%s] %s (%s)", Get_Name().c_str(),
				bResult ? _TL("Execution succeeded") : _TL("Execution failed"), Time.c_str()),
				bResult ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE
			);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::_Synchronize_DataObjects(void)
{
	Parameters.DataObjects_Synchronize();

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->DataObjects_Synchronize();
	}

	CSG_Projection	Projection;

	if( do_Sync_Projections() && Get_Projection(Projection) )
	{
		Parameters.DataObjects_Set_Projection(Projection);

		for(int j=0; j<m_npParameters; j++)
		{
			m_pParameters[j]->DataObjects_Set_Projection(Projection);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool::Get_Projection(CSG_Projection &Projection)	const
{
	Projection.Destroy();

	Parameters.DataObjects_Get_Projection(Projection);

	for(int i=0; i<m_npParameters && !Projection.is_Okay(); i++)
	{
		m_pParameters[i]->DataObjects_Get_Projection(Projection);
	}

	return( Projection.is_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Tool::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	CSG_Parameters *pParameters = pParameter  ? pParameter ->Get_Parameters() : NULL;
	CSG_Tool       *pTool       = pParameters ? pParameters->Get_Tool      () : NULL;

	if( pTool )
	{
		if( Flags & PARAMETER_CHECK_VALUES )
		{
			pTool->On_Parameter_Changed(pParameters, pParameter);
		}

		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			pTool->On_Parameters_Enable(pParameters, pParameter);
		}

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Tool::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
}

//---------------------------------------------------------
int CSG_Tool::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//					Extra Parameters					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CSG_Tool::Add_Parameters(const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameters	*pParameters;

	m_pParameters	= (CSG_Parameters **)SG_Realloc(m_pParameters, (m_npParameters + 1) * sizeof(CSG_Parameters *));
	pParameters		= m_pParameters[m_npParameters++]	= new CSG_Parameters();

	pParameters->Create(this, Name, Description, Identifier);
	pParameters->Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);
	pParameters->m_pTool	= this;

	return( pParameters );
}

//---------------------------------------------------------
CSG_Parameters * CSG_Tool::Get_Parameters(const CSG_String &Identifier)
{
	for(int i=0; i<m_npParameters; i++)
	{
		if( m_pParameters[i]->Cmp_Identifier(Identifier) )
		{
			return( m_pParameters[i] );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Tool::Dlg_Parameters(const CSG_String &Identifier)
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
bool CSG_Tool::Dlg_Parameters(CSG_Parameters *pParameters, const CSG_String &Caption)
{
	return( pParameters ? SG_UI_Dlg_Parameters(pParameters, Caption) : false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Set_Callback(bool bActive)
{
	Parameters.Set_Callback(bActive);

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->Set_Callback(bActive);
	}
}

//---------------------------------------------------------
bool CSG_Tool::Set_Manager(class CSG_Data_Manager *pManager)
{
	Parameters.Set_Manager(pManager);

	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->Set_Manager(pManager);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Settings_Push(CSG_Data_Manager *pManager)
{
	if( m_Settings_Stack.Get_Value_Size() != sizeof(CSG_Parameters *) )
	{
		m_Settings_Stack.Create(sizeof(CSG_Parameters *));
	}

	size_t	n	= m_Settings_Stack.Get_Size();

	CSG_Parameters	**pP	= (CSG_Parameters **)m_Settings_Stack.Get_Array(n + 1 + m_npParameters);

	if( pP )
	{
		pP[n++]	= new CSG_Parameters(Parameters); Parameters.Restore_Defaults(true); Parameters.Set_Manager(pManager);

		for(int i=0; i<m_npParameters; i++)
		{
			pP[n++]	= new CSG_Parameters(*m_pParameters[i]); m_pParameters[i]->Restore_Defaults(true); m_pParameters[i]->Set_Manager(pManager);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool::Settings_Pop(void)
{
	CSG_Parameters	**pP	= (CSG_Parameters **)m_Settings_Stack.Get_Array();

	if( pP && (int)m_Settings_Stack.Get_Size() >= 1 + m_npParameters )
	{
		size_t	n	= m_Settings_Stack.Get_Size() - 1;

		for(int i=m_npParameters-1; i>=0; i--, n--)
		{
			m_pParameters[i]->Assign_Values(pP[n]); m_pParameters[i]->Set_Manager(pP[n]->Get_Manager()); delete(pP[n]);
		}

		Parameters.Assign_Values(pP[n]); Parameters.Set_Manager(pP[n]->Get_Manager()); delete(pP[n]);

		m_Settings_Stack.Set_Array(n);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Progress						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Set_Show_Progress(bool bOn)
{
	m_bShow_Progress	= bOn;
}

//---------------------------------------------------------
bool CSG_Tool::Set_Progress(double Percent)	const
{
	return( Set_Progress(Percent, 100.0) );
}

//---------------------------------------------------------
bool CSG_Tool::Set_Progress(double Position, double Range)	const
{
	return( m_bShow_Progress ? SG_UI_Process_Set_Progress(Position, Range) : Process_Get_Okay(false) );
}

//---------------------------------------------------------
bool CSG_Tool::Stop_Execution(bool bDialog)
{
	m_bExecutes	= false;

	return( SG_UI_Stop_Execution(bDialog) );
}


///////////////////////////////////////////////////////////
//														 //
//						Message							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Message_Dlg(const CSG_String &Text, const SG_Char *Caption)
{
	SG_UI_Dlg_Message(Text, Caption && Caption[0] != '\0' ? Caption : Get_Name().c_str());
}

//---------------------------------------------------------
bool CSG_Tool::Message_Dlg_Confirm(const CSG_String &Text, const SG_Char *Caption)
{
	return( SG_UI_Dlg_Continue(Text, Caption && Caption[0] != '\0' ? Caption : Get_Name().c_str()) );
}

//---------------------------------------------------------
bool CSG_Tool::Error_Set(TSG_Tool_Error Error_ID)
{
	switch( Error_ID )
	{
	default:
		return( Error_Set(_TL("Unknown Error")) );

	case TOOL_ERROR_Calculation:
		return( Error_Set(_TL("Calculation Error")) );
	}
}

//---------------------------------------------------------
bool CSG_Tool::Error_Set(const CSG_String &Text)
{
	SG_UI_Msg_Add_Error(Text);

	m_Execution_Info += "\n____\n" + Text;

	if( SG_UI_Process_Get_Okay(false) && !m_bError_Ignore )
	{
		switch( SG_UI_Dlg_Error(Text, CSG_String::Format("%s: %s?", _TL("Error"), _TL("Ignore"))) )
		{
		default:
			SG_UI_Process_Set_Okay(false);
			break;

		case  1:
			m_bError_Ignore	= true;
			break;
		}
	}

	return( SG_UI_Process_Get_Okay(false) );
}

//---------------------------------------------------------
bool CSG_Tool::Error_Fmt(const char *Format, ...)
{
	wxString	_s;

	va_list	argptr;
	
#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");
	va_start(argptr, _Format);
	_s.PrintfV(_Format, argptr);
#else
	va_start(argptr, Format);
	_s.PrintfV(Format, argptr);
#endif

	va_end(argptr);

	CSG_String	s(&_s);

	return( Error_Set(s) );
}

//---------------------------------------------------------
bool CSG_Tool::Error_Fmt(const wchar_t *Format, ...)
{
	wxString	_s;

	va_list	argptr;
	
#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");
	va_start(argptr, _Format);
	_s.PrintfV(_Format, argptr);
#else
	va_start(argptr, Format);
	_s.PrintfV(Format, argptr);
#endif

	va_end(argptr);

	CSG_String	s(&_s);

	return( Error_Set(s) );
}


///////////////////////////////////////////////////////////
//														 //
//			DataObjects / GUI Interaction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::DataObject_Add(CSG_Data_Object *pDataObject, bool bUpdate)
{
	if( Parameters.Get_Manager() )
	{
		Parameters.Get_Manager()->Add(pDataObject);
	}

	if( Parameters.Get_Manager() == &SG_Get_Data_Manager() )	// prevent that local data manager send their data objects to gui
	{
		return( SG_UI_DataObject_Add(pDataObject, bUpdate) );
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Tool::DataObject_Update_All(void)
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
				for(int j=0; j<Parameters(i)->asList()->Get_Item_Count(); j++)
				{
					DataObject_Update(Parameters(i)->asList()->Get_Item(j), false);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//		Static Data Message/Progress Functions			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Process_Get_Okay(bool bBlink)
{
	return( SG_UI_Process_Get_Okay(bBlink) );
}

//---------------------------------------------------------
void CSG_Tool::Process_Set_Text(const CSG_String &Text)
{
	SG_UI_Process_Set_Text(Text);
}

//---------------------------------------------------------
void CSG_Tool::Process_Set_Text(const char    *Format, ...)
{
	wxString	_s;

	va_list	argptr;

	#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");
	va_start(argptr, _Format);
	_s.PrintfV(_Format, argptr);
	#else
	va_start(argptr, Format);
	_s.PrintfV(Format, argptr);
	#endif

	va_end(argptr);

	CSG_String	s(&_s);

	SG_UI_Process_Set_Text(s);
}

//---------------------------------------------------------
void CSG_Tool::Process_Set_Text(const wchar_t *Format, ...)
{
	wxString	_s;

	va_list	argptr;

	#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");
	va_start(argptr, _Format);
	_s.PrintfV(_Format, argptr);
	#else
	va_start(argptr, Format);
	_s.PrintfV(Format, argptr);
	#endif

	va_end(argptr);

	CSG_String	s(&_s);

	SG_UI_Process_Set_Text(s);
}

//---------------------------------------------------------
void CSG_Tool::Message_Add(const CSG_String &Text, bool bNewLine)
{
	SG_UI_Msg_Add_Execution(Text, bNewLine);

	if( bNewLine )
	{
		m_Execution_Info += "\n";
	}

	m_Execution_Info += Text;
}

//---------------------------------------------------------
void CSG_Tool::Message_Fmt(const char *Format, ...)
{
	wxString	_s;

	va_list	argptr;

	#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");
	va_start(argptr, _Format);
	_s.PrintfV(_Format, argptr);
	#else
	va_start(argptr, Format);
	_s.PrintfV(Format, argptr);
	#endif

	va_end(argptr);

	CSG_String	s(&_s);

	Message_Add(s, false);
}

//---------------------------------------------------------
void CSG_Tool::Message_Fmt(const wchar_t *Format, ...)
{
	wxString	_s;

	va_list	argptr;

	#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");
	va_start(argptr, _Format);
	_s.PrintfV(_Format, argptr);
	#else
	va_start(argptr, Format);
	_s.PrintfV(Format, argptr);
	#endif

	va_end(argptr);

	CSG_String	s(&_s);

	Message_Add(s, false);
}


///////////////////////////////////////////////////////////
//														 //
//		Static Data Object Property Functions			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::DataObject_Update(CSG_Data_Object *pDataObject, int Show)
{
	return( SG_UI_DataObject_Update(pDataObject, Show, NULL) );
}

bool CSG_Tool::DataObject_Update(CSG_Data_Object *pDataObject, double Minimum, double Maximum, int Show)
{
	CSG_Parameters	P;

	return( DataObject_Get_Parameters(pDataObject, P)
		&&  P.Set_Parameter("STRETCH_UPDATE"   , false  )	// internal update flag
		&&  P.Set_Parameter("STRETCH_DEFAULT"  , 3      )	// manual
		&&  P.Set_Parameter("METRIC_ZRANGE.MIN", Minimum)
		&&  P.Set_Parameter("METRIC_ZRANGE.MAX", Maximum)
		&&  SG_UI_DataObject_Update(pDataObject, Show, &P)
		);
}

//---------------------------------------------------------
bool CSG_Tool::DataObject_Get_Colors(CSG_Data_Object *pDataObject, CSG_Colors &Colors)
{
	return( SG_UI_DataObject_Colors_Get(pDataObject, &Colors) );
}

bool CSG_Tool::DataObject_Set_Colors(CSG_Data_Object *pDataObject, const CSG_Colors &Colors)
{
	CSG_Colors	c(Colors);

	return( SG_UI_DataObject_Colors_Set(pDataObject, &c) );
}

bool CSG_Tool::DataObject_Set_Colors(CSG_Data_Object *pDataObject, int nColors, int Palette, bool bRevert)
{
	CSG_Colors	c(nColors, Palette, bRevert);

	return( SG_UI_DataObject_Colors_Set(pDataObject, &c) );
}

//---------------------------------------------------------
bool CSG_Tool::DataObject_Get_Parameters(CSG_Data_Object *pDataObject, CSG_Parameters &Parameters)
{
	return( SG_UI_DataObject_Params_Get(pDataObject, &Parameters) );
}

bool CSG_Tool::DataObject_Set_Parameters(CSG_Data_Object *pDataObject, CSG_Parameters &Parameters)
{
	return( SG_UI_DataObject_Params_Set(pDataObject, &Parameters) );
}

bool CSG_Tool::DataObject_Set_Parameters(CSG_Data_Object *pDataObject, CSG_Data_Object *pCopy)
{
	if( pDataObject == pCopy )
	{
		return( true );
	}

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pCopy, P) )
	{
		P.Del_Parameter("OBJECT_NODATA"  );
		P.Del_Parameter("OBJECT_Z_FACTOR");
		P.Del_Parameter("OBJECT_Z_OFFSET");

		return( DataObject_Set_Parameters(pDataObject, P) );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Tool::DataObject_Get_Parameter(CSG_Data_Object *pDataObject, const CSG_String &ID)
{
	static CSG_Parameters	P;

	return( DataObject_Get_Parameters(pDataObject, P) ? P(ID) : NULL );
}

bool CSG_Tool::DataObject_Set_Parameter(CSG_Data_Object *pDataObject, CSG_Parameter *pParameter)
{
	CSG_Parameters	P;

	return( P._Add(pParameter) != NULL && DataObject_Set_Parameters(pDataObject, P) );
}

bool CSG_Tool::DataObject_Set_Parameter(CSG_Data_Object *pDataObject, CSG_Data_Object *pCopy, const CSG_String &ID)
{
	CSG_Parameter	*pParameter	= DataObject_Get_Parameter(pCopy, ID);

	return( DataObject_Set_Parameter(pDataObject, pParameter) );
}

bool CSG_Tool::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, int            Value)
{
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pDataObject, P) && P(ID) )
	{
		return( P(ID)->Set_Value(Value) && DataObject_Set_Parameter(pDataObject, P(ID)) );
	}

	return( false );
}

bool CSG_Tool::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, double         Value)
{
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pDataObject, P) && P(ID) )
	{
		return( P(ID)->Set_Value(Value) && DataObject_Set_Parameter(pDataObject, P(ID)) );
	}

	return( false );
}

bool CSG_Tool::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, void          *Value)
{
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pDataObject, P) && P(ID) )
	{
		return( P(ID)->Set_Value(Value) && DataObject_Set_Parameter(pDataObject, P(ID)) );
	}

	return( false );
}

bool CSG_Tool::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, const SG_Char *Value)
{
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pDataObject, P) && P(ID) )
	{
		return( P(ID)->Set_Value(Value) && DataObject_Set_Parameter(pDataObject, P(ID)) );
	}

	return( false );
}

bool CSG_Tool::DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, double loVal, double hiVal)	// Range Parameter
{
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pDataObject, P) && P(ID) && P(ID)->Get_Type() == PARAMETER_TYPE_Range )
	{
		return( P(ID)->asRange()->Set_Range(loVal, hiVal) && DataObject_Set_Parameter(pDataObject, P(ID)) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, CSG_Parameter *pValue) { return( Parameters.Set_Parameter(ID, pValue) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, CSG_Parameter *pValue) { return( Parameters.Set_Parameter(ID, pValue) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, CSG_Parameter *pValue) { return( Parameters.Set_Parameter(ID, pValue) ); }

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, int               Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, int               Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, int               Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, double            Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, double            Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, double            Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, void             *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, void             *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, void             *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, const CSG_String &Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, const CSG_String &Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, const CSG_String &Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, const char       *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, const char       *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, const char       *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }

//---------------------------------------------------------
/**
  * Direct 'set a value' access to this tool's default parameters list.
*/
//---------------------------------------------------------
bool CSG_Tool::Set_Parameter(const CSG_String &ID, const wchar_t    *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const char       *ID, const wchar_t    *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }
bool CSG_Tool::Set_Parameter(const wchar_t    *ID, const wchar_t    *Value, int Type) { return( Parameters.Set_Parameter(ID, Value, Type) ); }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Resets the tools' parameters list. All input and output
* data objects and object lists are cleared and parameter
* defaults are restored.
*/
bool CSG_Tool::Reset(void)
{
	for(int i=0; i<m_npParameters; i++)
	{
		m_pParameters[i]->Restore_Defaults(true);
		m_pParameters[i]->Reset_Grid_System();
	}

	return( Parameters.Restore_Defaults(true)
		&&  Parameters.Reset_Grid_System()
	);
}

//---------------------------------------------------------
/**
* Resets the tools' grid system, if it has one, which is
* typically the case for all derivatives of CSG_Tool_Grid.
*/
bool CSG_Tool::Reset_Grid_System(void)
{
	return( Parameters.Reset_Grid_System() );
}

//---------------------------------------------------------
/**
* Sets the tools' grid system, if it has one, which is
* typically the case for all derivatives of CSG_Tool_Grid.
*/
bool CSG_Tool::Set_Grid_System(const CSG_Grid_System &System)
{
	return( Parameters.Set_Grid_System(System) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Update_Parameter_States(void)
{
	_Update_Parameter_States(&Parameters);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		_Update_Parameter_States(Get_Parameters(i));
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Tool::_Update_Parameter_States(CSG_Parameters *pParameters)
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
//						Script							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::Get_Script(TSG_Tool_Script_Type Type, bool bHeader, bool bAllParameters)
{
	switch( Type )
	{
	case TOOL_SCRIPT_CMD_SHELL: return( _Get_Script_CMD           (      bHeader, bAllParameters, Type) );
	case TOOL_SCRIPT_CMD_BATCH: return( _Get_Script_CMD           (      bHeader, bAllParameters, Type) );
	case TOOL_SCRIPT_PYTHON   : return( _Get_Script_Python        (      bHeader, bAllParameters      ) );
	case TOOL_SCRIPT_CHAIN    : return( CSG_Tool_Chain::Get_Script(this, bHeader, bAllParameters      ) );
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::_Get_Script_CMD(bool bHeader, bool bAllParameters, TSG_Tool_Script_Type Type)
{
	CSG_String	Script;

	if( bHeader )
	{
		switch( Type )
		{
		case TOOL_SCRIPT_CMD_BATCH:	// DOS/Windows Batch Script
			Script	+= "@ECHO OFF\n\n";
			Script	+= "PUSHD %~dp0\n\n";
			Script	+= "REM SET SAGA_TLB=C:\\MyTools\n\n";
			Script	+= "SET SAGA_CMD=" + SG_UI_Get_Application_Path(true) + "saga_cmd.exe\n\n";
			Script	+= "REM Tool: " + Get_Name() + "\n\n";
			Script	+= "%SAGA_CMD%";
			break;

		default                   :	// Bash Shell Script
			Script	+= "#!/bin/bash\n\n";
			Script	+= "# export SAGA_TLB=/home/myhome/mytools\n\n";
			Script	+= "# tool: " + Get_Name() + "\n\n";
			Script	+= "saga_cmd";
			break;
		}
	}
	else
	{
		Script	+= "saga_cmd";
	}

	//-----------------------------------------------------
	Script	+= Get_Library().Contains(" ")	// white space? use quotation marks!
		? " \"" + Get_Library() + "\""
		: " "   + Get_Library();

	Script	+= Get_ID().Contains     (" ")	// white space? use quotation marks!
		? " \"" + Get_ID     () + "\""
		: " "   + Get_ID     ();

	_Get_Script_CMD(Script, Get_Parameters(), bAllParameters);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		_Get_Script_CMD(Script, Get_Parameters(i), bAllParameters);
	}

	//-----------------------------------------------------
	if( bHeader && Type == TOOL_SCRIPT_CMD_BATCH )
	{
		Script	+= "\n\nPAUSE\n";
	}

	return( Script );
}

//---------------------------------------------------------
void CSG_Tool::_Get_Script_CMD(CSG_String &Script, CSG_Parameters *pParameters, bool bAllParameters)
{
	#define GET_ID1(p)		(p->Get_Parameters()->Get_Identifier().Length() > 0 \
		? CSG_String::Format("%s_%s", p->Get_Parameters()->Get_Identifier().c_str(), p->Get_Identifier()) \
		: CSG_String::Format(p->Get_Identifier())).c_str()

	#define GET_ID2(p, s)	CSG_String::Format("%s_%s", GET_ID1(p), s).c_str()

#ifdef _SAGA_MSW
	CSG_String Prefix = " ^\n -";
#else
	CSG_String Prefix = " \\\n -";
#endif

	//-----------------------------------------------------
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !bAllParameters && (!p->is_Enabled(false) || p->is_Information() || !p->do_UseInCMD()) )
		{
			continue;
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool             :
			Script	+= Prefix + CSG_String::Format("%s=%d", GET_ID1(p), p->asBool() ? 1 : 0);
			break;

		case PARAMETER_TYPE_Int              :
		case PARAMETER_TYPE_Choice           :
		case PARAMETER_TYPE_Table_Field      :
			Script	+= Prefix + CSG_String::Format("%s=%d", GET_ID1(p), p->asInt());
			break;

		case PARAMETER_TYPE_Choices          :
		case PARAMETER_TYPE_Table_Fields:
			if( p->asString() && *p->asString() )
				Script	+= Prefix + CSG_String::Format("%s=%s", GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Double           :
		case PARAMETER_TYPE_Degree           :
			Script	+= Prefix + CSG_String::Format("%s=%g", GET_ID1(p), p->asDouble());
			break;

		case PARAMETER_TYPE_Range            :
			Script	+= Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T("MIN")), p->asRange()->Get_Min());
			Script	+= Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T("MAX")), p->asRange()->Get_Max());
			break;

		case PARAMETER_TYPE_Date             :
		case PARAMETER_TYPE_String           :
		case PARAMETER_TYPE_Text             :
		case PARAMETER_TYPE_FilePath         :
			Script	+= Prefix + CSG_String::Format("%s=\"%s\"", GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable       :
			Script	+= Prefix + CSG_String::Format("%s=%s", GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Grid_System      :
			if( p->Get_Children_Count() == 0 )
			{
				Script	+= Prefix + CSG_String::Format("%s=%d", GET_ID2(p, SG_T("NX")), p->asGrid_System()->Get_NX());
				Script	+= Prefix + CSG_String::Format("%s=%d", GET_ID2(p, SG_T("NY")), p->asGrid_System()->Get_NY());
				Script	+= Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T( "X")), p->asGrid_System()->Get_XMin());
				Script	+= Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T( "Y")), p->asGrid_System()->Get_YMin());
				Script	+= Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T( "D")), p->asGrid_System()->Get_Cellsize());
			}
			break;

		case PARAMETER_TYPE_DataObject_Output:
		case PARAMETER_TYPE_Grid             :
		case PARAMETER_TYPE_Grids            :
		case PARAMETER_TYPE_Table            :
		case PARAMETER_TYPE_Shapes           :
		case PARAMETER_TYPE_TIN              :
		case PARAMETER_TYPE_PointCloud       :
			if( p->is_Input() )
			{
				Script	+= Prefix + CSG_String::Format("%s=\"%s\"", GET_ID1(p), SG_Get_Data_Manager().Exists(p->asDataObject()) && p->asDataObject()->Get_File_Name() ? p->asDataObject()->Get_File_Name() : SG_T("input file"));
			}
			else
			{
				CSG_String	ext;

				switch( p->Get_DataObject_Type() )
				{
				case SG_DATAOBJECT_TYPE_Grid      : ext = "sg-grd-z"; break;
				case SG_DATAOBJECT_TYPE_Grids     : ext = "sg-gds-z"; break;
				case SG_DATAOBJECT_TYPE_Table     : ext = "txt"     ; break;
				case SG_DATAOBJECT_TYPE_Shapes    : ext = "geojson" ; break;
				case SG_DATAOBJECT_TYPE_PointCloud: ext = "sg-pts-z"; break;
				case SG_DATAOBJECT_TYPE_TIN       : ext = "geojson" ; break;
				default                           : ext = "dat"     ; break;
				}

				Script	+= Prefix + CSG_String::Format("%s=\"%s.%s\"", GET_ID1(p), p->Get_Name(), ext.c_str());
			}
			break;

		case PARAMETER_TYPE_Grid_List        :
		case PARAMETER_TYPE_Grids_List       :
		case PARAMETER_TYPE_Table_List       :
		case PARAMETER_TYPE_Shapes_List      :
		case PARAMETER_TYPE_TIN_List         :
		case PARAMETER_TYPE_PointCloud_List  :
			if( p->is_Input() )
			{
				Script	+= Prefix + CSG_String::Format("%s=", GET_ID1(p));

				if( p->asList()->Get_Item_Count() == 0 )
				{
					Script	+= "file(s)";
				}
				else
				{
					Script	+= SG_File_Exists(p->asList()->Get_Item(0)->Get_File_Name())
						? p->asList()->Get_Item(0)->Get_File_Name() : _TL("memory");

					for(int iObject=1; iObject<p->asList()->Get_Item_Count(); iObject++)
					{
						Script	+= ";";
						Script	+= SG_File_Exists(p->asList()->Get_Item(iObject)->Get_File_Name())
							? p->asList()->Get_Item(iObject)->Get_File_Name() : _TL("memory");
					}
				}
			}
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::_Get_Script_Python(bool bHeader, bool bAllParameters)
{
	CSG_String	Script, Name(Get_Name()); Name.Replace(" ", "_"); Name.Replace("(", ""); Name.Replace(")", ""); Name.Replace("[", ""); Name.Replace("]", ""); Name.Replace(".", "");

	//-----------------------------------------------------
	if( bHeader )
	{
		Script += "#! /usr/bin/env python\n";
		Script += "\n";
		Script += "#_________________________________________\n";
		Script += "##########################################\n";
		Script += "# Initialize the environment...\n";
		Script += "\n";
#ifdef _SAGA_MSW
		CSG_String AppPath = SG_UI_Get_Application_Path(true); AppPath.Replace("\\", "/");
		Script += "# Windows: set/adjust the 'SAGA_PATH' environment variable before importing saga_helper\n";
		Script += "import os\n";
		Script += "if os.name == 'nt' and os.getenv('SAGA_PATH') is None:\n";
		Script += "    os.environ['SAGA_PATH'] = '" + AppPath + "'\n";
		Script += "import saga_helper, saga_api\n";
#else
		Script += "import os, saga_helper, saga_api\n";
#endif // _SAGA_MSW
		Script += "\n";
		Script += "saga_helper.Initialize(True)\n";
		Script += "\n";
		Script += "\n";
		Script += "#_________________________________________\n";
		Script += "##########################################\n";
	}

	//-----------------------------------------------------
	Script += "def Run_" + Name + "(File):\n";
	Script += "    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('" + Get_Library() + "', '" + Get_ID() + "')\n";
	Script += "    if Tool == None:\n";
    Script += "        print('Failed to create tool: " + Get_Name() + "')\n";
	Script += "        return False\n";
	Script += "\n";
	Script += "    Tool.Reset()\n";
	Script += "\n";

	//-------------------------------------------------
	_Get_Script_Python(Script, Get_Parameters(), bAllParameters);

	for(int iParameters=0; iParameters<Get_Parameters_Count(); iParameters++)
	{
		_Get_Script_Python(Script, Get_Parameters(iParameters), bAllParameters, Get_Parameters(iParameters)->Get_Identifier());
	}

	//-------------------------------------------------
	Script += "\n";
	Script += "    if Tool.Execute() == False:\n";
    Script += "        print('failed to execute tool: ' + Tool.Get_Name().c_str())\n";
	Script += "        return False\n";
	Script += "\n";
	Script += "    #_____________________________________\n";
	Script += "    # Save results to file:\n";
	Script += "    Path = os.path.split(File)[0] + os.sep\n";

	for(int iParameter=0; iParameter<Get_Parameters()->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= Get_Parameters()->Get_Parameter(iParameter);

		if( p->is_Output() )
		{
			CSG_String	id(p->Get_Identifier()), ext;

			switch( p->Get_DataObject_Type() )
			{
			case SG_DATAOBJECT_TYPE_Grid      : ext = " + '.sg-grd-z'"; break;
			case SG_DATAOBJECT_TYPE_Grids     : ext = " + '.sg-gds-z'"; break;
			case SG_DATAOBJECT_TYPE_Table     : ext = " + '.txt'"     ; break;
			case SG_DATAOBJECT_TYPE_Shapes    : ext = " + '.geojson'" ; break;
			case SG_DATAOBJECT_TYPE_PointCloud: ext = " + '.sg-pts-z'"; break;
			case SG_DATAOBJECT_TYPE_TIN       : ext = " + '.geojson'" ; break;
			default                           : ext = ""              ; break;
			}

			Script += "\n";

			if( p->is_DataObject() )
			{
				Script += "    Data = Tool.Get_Parameter('" +  id + "').asDataObject()\n";
				Script += "    Data.Save(Path + Data.Get_Name()" + ext + ")\n";
			}
			else if( p->is_DataObject_List() )
			{
				Script += "    List = Tool.Get_Parameter('" +  id + "').asList()\n";
				Script += "    Name = Path + List.Get_Name()\n";
				Script += "    for i in range(0, List.Get_Data_Count()):\n";
				Script += "        List.Get_Data(i).Save(Name + str(i)" + ext + ")\n";
			}
		}
	}

	Script += "\n";
	Script += "    #_____________________________________\n";
	Script += "    saga_api.SG_Get_Data_Manager().Delete_All() # job is done, free memory resources\n";
	Script += "\n";
	Script += "    return True\n";
	Script += "\n";

	//-----------------------------------------------------
	if( bHeader )
	{
		Script += "\n";
		Script += "#_________________________________________\n";
		Script += "##########################################\n";
		Script += "if __name__ == '__main__':\n";
        Script += "    print('This is a simple template for using a SAGA tool through Python.')\n";
        Script += "    print('Please edit the script to make it work properly before using it!')\n";
		Script += "    import sys\n";
		Script += "    sys.exit()\n";
		Script += "\n";
		Script += "    # For a single file based input it might look like following:\n";
		Script += "    File = sys.argv[1]\n";
		Script += "    Run_" + Name + "(File)\n";
	}

	return( Script );
}

//---------------------------------------------------------
void CSG_Tool::_Get_Script_Python(CSG_String &Script, CSG_Parameters *pParameters, bool bAllParameters, const CSG_String &Prefix)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !bAllParameters && (!p->is_Enabled(false) || p->is_Information() || !p->do_UseInCMD()) )
		{
			continue;
		}

		CSG_String	ID(p->Get_Identifier());

		if( !Prefix.is_Empty() )
		{
			ID.Prepend(Prefix + ".");
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool           :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', %s)\n", ID.c_str(), p->asBool() ? SG_T("True") : SG_T("False"));
			break;

		case PARAMETER_TYPE_Int            :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', %d)\n", ID.c_str(), p->asInt());
			break;

		case PARAMETER_TYPE_Choice         :
		case PARAMETER_TYPE_Choices        :
		case PARAMETER_TYPE_Table_Field    :
		case PARAMETER_TYPE_Table_Fields   :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', '%s')\n", ID.c_str(), p->asString());
			break;

		case PARAMETER_TYPE_Double         :
		case PARAMETER_TYPE_Degree         :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', %g)\n", ID.c_str(), p->asDouble());
			break;

		case PARAMETER_TYPE_Range          :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s.MIN', %g)\n", ID.c_str(), p->asRange()->Get_Min());
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s.MAX', %g)\n", ID.c_str(), p->asRange()->Get_Max());
			break;

		case PARAMETER_TYPE_Date           :
		case PARAMETER_TYPE_String         :
		case PARAMETER_TYPE_Text           :
		case PARAMETER_TYPE_FilePath       :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', '%s')\n", ID.c_str(), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable     :
			Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.SG_Create_Table('table.txt'))\n", ID.c_str());
			break;

		case PARAMETER_TYPE_Grid_System    :
			if( p->Get_Children_Count() == 0 )
			{
				Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.CSG_Grid_System(%g, %g, %g, %d, %d))\n", ID.c_str(),
					p->asGrid_System()->Get_Cellsize(),
					p->asGrid_System()->Get_XMin(), p->asGrid_System()->Get_YMin(),
					p->asGrid_System()->Get_NX  (), p->asGrid_System()->Get_NY  ()
				);
			}
			break;

		case PARAMETER_TYPE_Grid           :
		case PARAMETER_TYPE_Grids          :
		case PARAMETER_TYPE_Table          :
		case PARAMETER_TYPE_Shapes         :
		case PARAMETER_TYPE_TIN            :
		case PARAMETER_TYPE_PointCloud     :
			if( p->is_Input() )
			{
				Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.SG_Get_Data_Manager().Add('%s input file%s'))\n", ID.c_str(),
					SG_Get_DataObject_Name(p->Get_DataObject_Type()).c_str(), p->is_Optional() ? SG_T(", optional") : SG_T("")
				);
			}
			else if( p->is_Output() && p->is_Optional() )
			{
				Script	+= CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.SG_Get_Create_Pointer()) # optional output, remove this line, if you don't want to create it\n", ID.c_str());
			}
			break;

		case PARAMETER_TYPE_Grid_List      :
		case PARAMETER_TYPE_Grids_List     :
		case PARAMETER_TYPE_Table_List     :
		case PARAMETER_TYPE_Shapes_List    :
		case PARAMETER_TYPE_TIN_List       :
		case PARAMETER_TYPE_PointCloud_List:
			if( p->is_Input() )
			{
				Script	+= CSG_String::Format("    Tool.Get_Parameter('%s').asList().Add_Item('%s input list%s')\n", ID.c_str(),
					SG_Get_DataObject_Name(p->Get_DataObject_Type()).c_str(), p->is_Optional() ? SG_T(", optional") : SG_T("")
				);
			}
			break;

		case PARAMETER_TYPE_Parameters     :
			_Get_Script_Python(Script, p->asParameters(), bAllParameters, ID);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//						History							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData CSG_Tool::_Get_Output_History(void)
{
	CSG_MetaData	History;

	History.Set_Name(SG_META_HISTORY);
	History.Add_Property("saga-version", SAGA_VERSION);

	if( SG_Get_History_Depth() )
	{
		CSG_MetaData	*pTool	= History.Add_Child("TOOL");

		pTool->Add_Property("library", Get_Library());
		pTool->Add_Property("id"     , Get_ID     ());
		pTool->Add_Property("name"   , Get_Name   ());

		Parameters.Set_History(*pTool);

		pTool->Add_Children(History_Supplement);

		CSG_MetaData	*pOutput	= pTool->Add_Child("OUTPUT");
		pOutput->Add_Property("type", "");
		pOutput->Add_Property("id"  , "");
		pOutput->Add_Property("name", "");

		pTool->Del_Children(SG_Get_History_Depth(), SG_T("TOOL"));
	}

	return( History );
}

//---------------------------------------------------------
void CSG_Tool::_Set_Output_History(void)
{
	CSG_MetaData	History(_Get_Output_History());

	//-----------------------------------------------------
	for(int j=-1; j<Get_Parameters_Count(); j++)
	{
		CSG_Parameters	*pParameters	= j < 0 ? &Parameters : Get_Parameters(j);

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

			if( pParameter->is_Output() )//&& (pParameter->is_Enabled() || !has_GUI()) )
			{
				DataObject_Set_History(pParameter, &History);
			}
		}
	}
}

//---------------------------------------------------------
bool CSG_Tool::DataObject_Set_History(CSG_Parameter *pParameter, CSG_MetaData *pHistory)
{
	if( !pParameter )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	History;

	if( !pHistory )
	{
		History		= _Get_Output_History();

		pHistory	= &History;
	}

	//-----------------------------------------------------
	CSG_MetaData	*pOutput	= pHistory->Get_Child("TOOL") ? pHistory->Get_Child("TOOL")->Get_Child("OUTPUT") : NULL;

	if( pOutput )
	{
		pOutput->Set_Property("type", pParameter->Get_Type_Identifier());
		pOutput->Set_Property("id"  , pParameter->Get_Identifier     ());
		pOutput->Set_Property("name", pParameter->Get_Name           ());
	}

	//-----------------------------------------------------
	if( pParameter->is_DataObject() )
	{
		if( pParameter->asDataObject() )
		{
			if( pOutput )
			{
				pOutput->Set_Content(pParameter->asDataObject()->Get_Name());
			}

			pParameter->asDataObject()->Get_History().Assign(*pHistory);

			return( true );
		}
	}

	//-----------------------------------------------------
	else if( pParameter->is_DataObject_List() )
	{
		for(int j=0; j<pParameter->asList()->Get_Item_Count(); j++)
		{
			if( pOutput )
			{
				pOutput->Set_Content(pParameter->asList()->Get_Item(j)->Get_Name());
			}

			pParameter->asList()->Get_Item(j)->Get_History().Assign(*pHistory);
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
