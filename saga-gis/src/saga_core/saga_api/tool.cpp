
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
#include <wx/cmdline.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool::CSG_Tool(void)
{
	m_ID      = "-1";
	m_Version = "1.0";

	m_bError_Ignore = false;
	m_bExecutes     = false;

	m_bGUI = SG_UI_Get_Application_Name().Cmp("saga_gui") == 0;
	m_bCMD = SG_UI_Get_Application_Name().Cmp("saga_cmd") == 0;

	Parameters.Create(this, SG_T("Tool"));
	Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);
	Parameters.Set_Tool(this);

	Set_Show_Progress(true);
}

//---------------------------------------------------------
CSG_Tool::~CSG_Tool(void)
{
	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		delete(Get_Parameters(i));
	}

	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Destroy(void)
{
	m_bError_Ignore	= false;

	History_Supplement.Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
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

	CSG_Strings Menu = SG_String_Tokenize(Get_MenuPath(), ";"); if( !Menu.Get_Count() ) { Menu += ""; }

	for(int i=0; i<Menu.Get_Count(); i++)
	{
		if( Menu[i].Length() > 1 && Menu[i][1] == ':' )
		{
			if( Menu[i][0] == 'A' || Menu[i][0] == 'a' ) // absolute menu path, overwrites library's default menu path
			{
				Menu[i] = Menu[i].AfterFirst(':');

				continue;
			}

			Menu[i] = Menu[i].AfterFirst(':'); // Menu[0] == 'R' || Menu[0] == 'r' // menu path explicitly declared as relative to library's default menu path
		}

		if( !m_Library_Menu.is_Empty() )
		{
			if( Menu[i].is_Empty() )
			{
				Menu[i] = m_Library_Menu;
			}
			else
			{
				Menu[i] = m_Library_Menu + "|" + Menu[i];
			}
		}
	}

	CSG_String Menus; 

	for(int i=0; i<Menu.Get_Count(); i++)
	{
		if( i > 0 )
		{
			Menus += ";";
		}

		Menus += Menu[i];
	}

	return( Menus );
}

//---------------------------------------------------------
bool CSG_Tool::has_GUI(void) const
{
	return( m_bGUI );
}

//---------------------------------------------------------
bool CSG_Tool::has_CMD(void) const
{
	return( m_bCMD );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Execute(bool bAddHistory)
{
	#define ADD_MESSAGE_EXECUTION(Text, Style) { SG_UI_Msg_Add(Text, true, Style); if( has_GUI() ) { SG_UI_Msg_Add_Execution(Text, true, Style); } }

	//-----------------------------------------------------
	if( m_bExecutes )
	{
		return( false );
	}

	m_bExecutes     = true;

	m_bError_Ignore	= false;

	bool bResult    = false;

	m_Execution_Info.Clear();

	History_Supplement.Destroy();

	//-----------------------------------------------------
	if( Parameters.Get_Manager() == &SG_Get_Data_Manager() )
	{
		ADD_MESSAGE_EXECUTION(CSG_String::Format("[%s] %s...", Get_Name().c_str(), _TL("Execution started")), SG_UI_MSG_STYLE_SUCCESS);
	}

	Update_Parameter_States();

	if( Parameters.DataObjects_Create() == false )
	{
		_Synchronize_DataObjects();	// not all, but some objects might have been created!
	}
	else
	{
		Parameters.Msg_String(false);

	//	SG_UI_Process_Set_Busy(true, CSG_String::Format("%s: %s...", _TL("Executing"), Get_Name().c_str()));
		CSG_DateTime Started(CSG_DateTime::Now());

///////////////////////////////////////////////////////////
//#if !defined(_DEBUG)
#define _TOOL_EXCEPTION
	try
	{
//#endif
///////////////////////////////////////////////////////////

		bResult = On_Execute();

///////////////////////////////////////////////////////////
#ifdef _TOOL_EXCEPTION
	}	// try
	catch(const std::exception &Exception)
	{
		Message_Dlg(Exception.what()            , CSG_String::Format("%s | %s", Get_Name().c_str(), _TL("Access violation!")));
	}
	catch(...)
	{
		Message_Dlg(_TL("unspecified exception"), CSG_String::Format("%s | %s", Get_Name().c_str(), _TL("Access violation!")));
	}
	#endif
///////////////////////////////////////////////////////////

		CSG_TimeSpan Span = CSG_DateTime::Now() - Started;
	//	SG_UI_Process_Set_Busy(false);

		_Synchronize_DataObjects();

		if( !Process_Get_Okay(false) )
		{
			SG_UI_Msg_Add(_TL("Execution has been stopped by user!"), true, SG_UI_MSG_STYLE_BOLD);

			bResult	= false;
		}

		if( bResult && bAddHistory )
		{
			_Set_Output_History();
		}

		//-------------------------------------------------
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
				SG_UI_Msg_Add_Execution(CSG_String::Format("\n__________\n%s %s: %lld %s (%s)\n", _TL("total"),
					_TL("execution time"), Span.Get_Milliseconds(), _TL("milliseconds"), Time.c_str()),
					false, SG_UI_MSG_STYLE_BOLD
				);

				ADD_MESSAGE_EXECUTION(CSG_String::Format("[%s] %s (%s)", Get_Name().c_str(),
					bResult ? _TL("Execution succeeded") : _TL("Execution failed"), Time.c_str()),
					bResult ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE
				);
			}
		}
	}

	//-----------------------------------------------------
	History_Supplement.Destroy();

	m_bExecutes	= false;

	SG_UI_Process_Set_Okay ();
	SG_UI_Process_Set_Ready();

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::_Synchronize_DataObjects(void)
{
	Parameters.DataObjects_Synchronize();

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->DataObjects_Synchronize();
	}

	CSG_Projection Projection;

	if( do_Sync_Projections() && Get_Projection(Projection) )
	{
		Parameters.DataObjects_Set_Projection(Projection);

		for(int i=0; i<Get_Parameters_Count(); i++)
		{
			Get_Parameters(i)->DataObjects_Set_Projection(Projection);
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

	for(int i=0; i<Get_Parameters_Count() && !Projection.is_Okay(); i++)
	{
		Get_Parameters(i)->DataObjects_Get_Projection(Projection);
	}

	return( Projection.is_Okay() );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Parameters						 //
//                                                       //
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
//                                                       //
//					Extra Parameters					 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CSG_Tool::Add_Parameters(const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameters *pParameters = new CSG_Parameters(); // (Name, Description, Identifier);

	pParameters->Create(this, Name, Description, Identifier);
	pParameters->Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);
	pParameters->m_pTool = this;

	m_pParameters.Add(pParameters);

	return( pParameters );
}

//---------------------------------------------------------
CSG_Parameters * CSG_Tool::Get_Parameters(const CSG_String &Identifier) const
{
	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		if( Get_Parameters(i)->Cmp_Identifier(Identifier) )
		{
			return( Get_Parameters(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Tool::Dlg_Parameters(const CSG_String &Identifier)
{
	CSG_Parameters *pParameters = Get_Parameters(Identifier);

	if( pParameters && (!pParameters->is_Managed() || Dlg_Parameters(pParameters, Get_Name())) )
	{
		pParameters->Set_History(History_Supplement);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Dlg_Parameters(CSG_Parameters *pParameters, const CSG_String &Caption)
{
	return( pParameters ? Dlg_Parameters(*pParameters, Caption) : false );
}

bool CSG_Tool::Dlg_Parameters(CSG_Parameters &Parameters, const CSG_String &Caption)
{
	return( SG_UI_Dlg_Parameters(&Parameters, Caption.is_Empty() ? Get_Name() : Caption) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Set_Callback(bool bActive)
{
	Parameters.Set_Callback(bActive);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->Set_Callback(bActive);
	}
}

//---------------------------------------------------------
bool CSG_Tool::Set_Manager(CSG_Data_Manager *pManager)
{
	Parameters.Set_Manager(pManager);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->Set_Manager(pManager);
	}

	return( true );
}

//---------------------------------------------------------
CSG_Data_Manager * CSG_Tool::Get_Manager(void)	const
{
	return( Parameters.Get_Manager() );
}

//---------------------------------------------------------
CSG_Data_Manager * CSG_Tool::Create_Manager(void)
{
	Set_Manager(new CSG_Data_Manager());

	return( Get_Manager() );
}

//---------------------------------------------------------
bool CSG_Tool::Delete_Manager(bool bDetachData, bool bReset)
{
	CSG_Data_Manager *pManager = Get_Manager();

	if( pManager && pManager != &SG_Get_Data_Manager() )
	{
		pManager->Delete(bDetachData);

		delete(pManager);

		return( bReset ? Set_Manager(&SG_Get_Data_Manager()) : Set_Manager(NULL) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::Settings_Push(CSG_Data_Manager *pManager)
{
	Parameters.Push(pManager);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->Push(pManager);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool::Settings_Pop(void)
{
	Parameters.Pop();

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->Pop();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Progress						 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool::Set_Show_Progress(bool bOn)
{
	m_bShow_Progress = bOn;
}

//---------------------------------------------------------
bool CSG_Tool::Set_Progress(int Position, int Range)	const
{
	return( Set_Progress((double)Position / (double)Range) );
}

//---------------------------------------------------------
bool CSG_Tool::Set_Progress(sLong Position, sLong Range)	const
{
	return( Set_Progress((double)Position / (double)Range) );
}

//---------------------------------------------------------
bool CSG_Tool::Set_Progress(double Position, double Range)	const
{
	return( m_bShow_Progress ? SG_UI_Process_Set_Progress(Position, Range) : Process_Get_Okay(false) );
}

//---------------------------------------------------------
bool CSG_Tool::Stop_Execution(bool bDialog)
{
	m_bExecutes = false;

	return( SG_UI_Stop_Execution(bDialog) );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Message							 //
//                                                       //
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
//                                                       //
//			DataObjects / GUI Interaction				 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool::DataObject_Add(CSG_Data_Object *pDataObject, bool bShow)
{
	if( Parameters.Get_Manager() )
	{
		Parameters.Get_Manager()->Add(pDataObject);
	}

	if( Parameters.Get_Manager() == &SG_Get_Data_Manager() ) // prevent that local data manager send their data objects to gui
	{
		return( SG_UI_DataObject_Add(pDataObject, bShow ? SG_UI_DATAOBJECT_SHOW_MAP : SG_UI_DATAOBJECT_UPDATE) );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool::DataObject_Update_All(void)
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

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//		Static Data Message/Progress Functions			 //
//                                                       //
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
//                                                       //
//		Static Data Object Property Functions			 //
//                                                       //
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
//                                                       //
//                                                       //
//                                                       //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Resets the tools' parameters list. All input and output
* data objects and object lists are cleared and parameter
* defaults are restored.
*/
bool CSG_Tool::Reset(bool bManager)
{
	Reset_Grid_System();

	if( bManager )
	{
		Reset_Manager();
	}

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->Restore_Defaults(true);
	}

	return( Parameters.Restore_Defaults(true) );
}

//---------------------------------------------------------
/**
* Resets the tools' data manager so it will be the SAGA API's
* default manager as can be requested by SG_Get_Data_Manager().
*/
bool CSG_Tool::Reset_Manager(void)
{
	return( Set_Manager(&SG_Get_Data_Manager()) );
}

//---------------------------------------------------------
/**
* Resets the tools' grid system, if it has one, which is
* typically the case for all derivatives of CSG_Tool_Grid.
*/
bool CSG_Tool::Reset_Grid_System(void)
{
	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		Get_Parameters(i)->Reset_Grid_System();
	}

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

//---------------------------------------------------------
/**
* Gets the tools' grid system, if it has one, which is
* typically the case for all derivatives of CSG_Tool_Grid.
*/
CSG_Grid_System * CSG_Tool::Get_Grid_System(void) const
{
	return( Parameters.Get_Grid_System() );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
//                                                       //
//						Script							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::Get_Script(Script_Format Format, bool bHeader, int Arguments, bool bWrapArgs)
{
	switch( Format )
	{
	case Script_Format::CMD_Shell                    : return( _Get_Script_CMD           (      bHeader, Arguments > 0, bWrapArgs, false    ) );
	case Script_Format::CMD_Batch                    : return( _Get_Script_CMD           (      bHeader, Arguments > 0, bWrapArgs, true     ) );
	case Script_Format::CMD_Usage                    : return( _Get_Script_CMD_Usage     (                                                  ) );

	case Script_Format::Toolchain                    : return( CSG_Tool_Chain::Get_Script(this, bHeader, Arguments > 0                      ) );

	case Script_Format::Python                       : return( _Get_Script_Python        (      bHeader, Arguments > 0                      ) );

	case Script_Format::Python_Wrapper_Func_Name     : return( _Get_Script_Python_Wrap   (      bHeader, true , false,         1, bWrapArgs ) );
	case Script_Format::Python_Wrapper_Func_ID       : return( _Get_Script_Python_Wrap   (      bHeader, false, false,         1, bWrapArgs ) );

	case Script_Format::Python_Wrapper_Call_Name     : return( _Get_Script_Python_Wrap   (      bHeader, true , true , Arguments, bWrapArgs ) );
	case Script_Format::Python_Wrapper_Call_ID       : return( _Get_Script_Python_Wrap   (      bHeader, false, true , Arguments, bWrapArgs ) );
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::_Get_Script_CMD(bool bHeader, bool bAllArguments, bool bWrapArgs, bool bBatch)
{
	CSG_String Script;

	if( bHeader )
	{
		if( bBatch ) // DOS/Windows Batch Script
		{
			Script += "@ECHO OFF\n\n";
			Script += "PUSHD %~dp0\n\n";
			Script += "REM SET SAGA_TLB=C:\\MyTools\n\n";
			Script += "SET SAGA_CMD=" + SG_UI_Get_Application_Path(true) + "saga_cmd.exe\n\n";
			Script += "REM Tool: " + Get_Name() + "\n\n";
			Script += "%SAGA_CMD%";
		}
		else         // Bash Shell Script
		{
			Script += "#!/bin/bash\n\n";
			Script += "# export SAGA_TLB=~/mytools\n\n";
			Script += "# tool: " + Get_Name() + "\n\n";
			Script += "saga_cmd";
		}
	}
	else
	{
		Script += "saga_cmd";
	}

	//-----------------------------------------------------
	Script += Get_Library().Contains(" ")	// white space? use quotation marks!
		? " \"" + Get_Library() + "\""
		: " "   + Get_Library();

	Script += Get_ID().Contains     (" ")	// white space? use quotation marks!
		? " \"" + Get_ID     () + "\""
		: " "   + Get_ID     ();

	_Get_Script_CMD(Script, Get_Parameters(), bAllArguments, bWrapArgs, bBatch);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		_Get_Script_CMD(Script, Get_Parameters(i), bAllArguments, bWrapArgs, bBatch);
	}

	//-----------------------------------------------------
	if( bHeader )
	{
		Script += bBatch ? "\n\nPAUSE\n" : "\n";
	}

	return( Script );
}

//---------------------------------------------------------
void CSG_Tool::_Get_Script_CMD(CSG_String &Script, CSG_Parameters *pParameters, bool bAllArguments, bool bWrapArgs, bool bBatch)
{
	#define GET_ID1(p) (p->Get_Parameters()->Get_Identifier().Length() > 0 \
		? CSG_String::Format("%s_%s", p->Get_Parameters()->Get_Identifier().c_str(), p->Get_Identifier()) \
		: CSG_String::Format(p->Get_Identifier())).c_str()

	#define GET_ID2(p, s) CSG_String::Format("%s_%s", GET_ID1(p), s).c_str()

	const char *Prefix = !bWrapArgs ? " -" : bBatch ? " ^\n -" : " \\\n -";
	const char *StrFmt = bBatch ? "%s=\"%s\"" : "%s=\\\"\"%s\"\\\"";

	//-----------------------------------------------------
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter *p = pParameters->Get_Parameter(iParameter);

		if( !bAllArguments && (!p->is_Enabled(false) || p->is_Information() || !p->do_UseInCMD()) )
		{
			continue;
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool             :
			Script += Prefix + CSG_String::Format("%s=%d", GET_ID1(p), p->asBool() ? 1 : 0);
			break;

		case PARAMETER_TYPE_Int              :
		case PARAMETER_TYPE_Data_Type        :
		case PARAMETER_TYPE_Choice           :
		case PARAMETER_TYPE_Table_Field      :
			Script += Prefix + CSG_String::Format("%s=%d", GET_ID1(p), p->asInt());
			break;

		case PARAMETER_TYPE_Choices          :
		case PARAMETER_TYPE_Table_Fields     :
			if( p->asString() && *p->asString() )
			{
				Script += Prefix + CSG_String::Format(StrFmt, GET_ID1(p), p->asString());
			}
			break;

        case PARAMETER_TYPE_Color            :
            Script += Prefix + CSG_String::Format("%s=\"%s\"", GET_ID1(p), SG_Color_To_Text(p->asColor()).c_str());
            break;

		case PARAMETER_TYPE_Double           :
		case PARAMETER_TYPE_Degree           :
			Script += Prefix + CSG_String::Format("%s=%g", GET_ID1(p), p->asDouble());
			break;

		case PARAMETER_TYPE_Range            :
			Script += Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T("MIN")), p->asRange()->Get_Min());
			Script += Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T("MAX")), p->asRange()->Get_Max());
			break;

		case PARAMETER_TYPE_String           :
			if( ((CSG_Parameter_String *)p)->is_Password() )
			{
				Script += Prefix + CSG_String::Format("%s=\"***\"", GET_ID1(p));
				break;
			}

		case PARAMETER_TYPE_Date             :
		case PARAMETER_TYPE_Text             :
		case PARAMETER_TYPE_FilePath         :
			Script += Prefix + CSG_String::Format(StrFmt, GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable       :
			Script += Prefix + CSG_String::Format(StrFmt, GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Grid_System      :
			if( p->Get_Children_Count() == 0 )
			{
				Script += Prefix + CSG_String::Format("%s=%d", GET_ID2(p, SG_T("NX")), p->asGrid_System()->Get_NX      ());
				Script += Prefix + CSG_String::Format("%s=%d", GET_ID2(p, SG_T("NY")), p->asGrid_System()->Get_NY      ());
				Script += Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T( "X")), p->asGrid_System()->Get_XMin    ());
				Script += Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T( "Y")), p->asGrid_System()->Get_YMin    ());
				Script += Prefix + CSG_String::Format("%s=%g", GET_ID2(p, SG_T( "D")), p->asGrid_System()->Get_Cellsize());
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
				Script += Prefix + CSG_String::Format(StrFmt, GET_ID1(p), SG_Get_Data_Manager().Exists(p->asDataObject()) && p->asDataObject()->Get_File_Name() ? p->asDataObject()->Get_File_Name() : SG_T("input file"));
			}
			else
			{
				CSG_String File(p->Get_Name());

				switch( p->Get_DataObject_Type() )
				{
				case SG_DATAOBJECT_TYPE_Grid      : File += ".sg-grd-z"; break;
				case SG_DATAOBJECT_TYPE_Grids     : File += ".sg-gds-z"; break;
				case SG_DATAOBJECT_TYPE_Table     : File += ".txt"     ; break;
				case SG_DATAOBJECT_TYPE_Shapes    : File += ".geojson" ; break;
				case SG_DATAOBJECT_TYPE_PointCloud: File += ".sg-pts-z"; break;
				case SG_DATAOBJECT_TYPE_TIN       : File += ".geojson" ; break;
				default                           : File += ".dat"     ; break;
				}

				Script += Prefix + CSG_String::Format(StrFmt, GET_ID1(p), File.c_str());
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
				Script += Prefix + CSG_String::Format("%s=", GET_ID1(p));

				if( p->asList()->Get_Item_Count() == 0 )
				{
					Script += "file(s)";
				}
				else
				{
					Script += SG_File_Exists(p->asList()->Get_Item(0)->Get_File_Name())
						? p->asList()->Get_Item(0)->Get_File_Name() : _TL("memory");

					for(int iObject=1; iObject<p->asList()->Get_Item_Count(); iObject++)
					{
						Script += ";";
						Script += SG_File_Exists(p->asList()->Get_Item(iObject)->Get_File_Name())
							? p->asList()->Get_Item(iObject)->Get_File_Name() : _TL("memory");
					}
				}
			}
			else
			{
				Script += Prefix + CSG_String::Format(StrFmt, GET_ID1(p), p->Get_Name());
			}
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::_Get_Script_CMD_Usage(void)
{
	wxCmdLineParser Parser; Parser.SetSwitchChars("-");

	_Get_Script_CMD_Usage(Get_Parameters(), Parser);

	for(int i=0; i<Get_Parameters_Count(); i++)
	{
		_Get_Script_CMD_Usage(Get_Parameters(i), Parser);
	}

	wxString Usage = wxString::Format("\nUsage: saga_cmd %s %s %s", Get_Library().c_str(), Get_ID().c_str(),
		Parser.GetUsageString().AfterFirst(' ').AfterFirst(' ')
	);

	CSG_String _Usage(&Usage);

	return( _Usage );
}

//---------------------------------------------------------
void CSG_Tool::_Get_Script_CMD_Usage(CSG_Parameters *pParameters, wxCmdLineParser &Parser)
{
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter *pParameter = pParameters->Get_Parameter(i);

		//-------------------------------------------------
		if( pParameter->is_DataObject() )	// reset data object parameters, avoids problems when tool is called more than once without un-/reloading
		{
			pParameter->Set_Value(DATAOBJECT_NOTSET);
		}
		else if( pParameter->is_DataObject_List() )
		{
			pParameter->asList()->Del_Items();
		}

		//-------------------------------------------------
		if( pParameter->do_UseInCMD() == false )
		{
			continue;
		}

		wxString Description = pParameter->Get_Description(
			PARAMETER_DESCRIPTION_NAME|PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, SG_T("\n\t")
		).c_str();

		Description.Replace("\xb", "");	// unicode problem: quick'n'dirty bug fix, to be replaced

		wxString ID(pParameter->Get_CmdID().c_str());

		if( pParameter->is_Input() || pParameter->is_Output() )
		{
			Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR|wxCMD_LINE_PARAM_OPTIONAL);
		}

		//-------------------------------------------------
		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			switch( pParameter->Get_Type() )
			{
			case PARAMETER_TYPE_Parameters  :
				_Get_Script_CMD_Usage(pParameter->asParameters(), Parser);
				break;

			case PARAMETER_TYPE_Bool        :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Int         :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Data_Type   :
			case PARAMETER_TYPE_Choice      :
			case PARAMETER_TYPE_Choices     :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Date        :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_DATE  , wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Range       :
				Parser.AddOption(ID + "_MIN", wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				Parser.AddOption(ID + "_MAX", wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Color       :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Colors      :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_FixedTable  :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Grid_System :
				if( pParameter->Get_Children_Count() == 0 )
				{
					Parser.AddOption(ID + "_D"   , wxEmptyString, _TL("Cell Size"                          ), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_X"   , wxEmptyString, _TL("Lower Left Center Cell X-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_Y"   , wxEmptyString, _TL("Lower Left Center Cell Y-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_NX"  , wxEmptyString, _TL("Number of Columns"                  ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_NY"  , wxEmptyString, _TL("Number of Rows"                     ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_FILE", wxEmptyString, _TL("Grid File"                          ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				}
				break;

			default:
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::_Get_Script_Python(bool bHeader, bool bAllArguments)
{
	CSG_String	Script, Name(Get_Name());

	Name.Replace(" ", "_");
	Name.Replace("(", "");
	Name.Replace(")", "");
	Name.Replace("[", "");
	Name.Replace("]", "");
	Name.Replace(".", "");
	Name.Replace(",", "");
	Name.Replace("/", "");
	Name.Replace("-", "");

	//-----------------------------------------------------
	if( bHeader )
	{
		Script += "#! /usr/bin/env python\n";
		Script += "\n";
		Script += "#_________________________________________\n";
		Script += "##########################################\n";
		Script += "\n";
#ifdef _SAGA_MSW
		CSG_String AppPath = SG_UI_Get_Application_Path(true); AppPath.Replace("\\", "/");
		Script += "# Initialize the environment...\n";
		Script += "\n";
		Script += "# Windows: Let the 'SAGA_PATH' environment variable point to\n";
		Script += "# the SAGA installation folder before importing 'saga_api'!\n";
		Script += "# This can be defined globally in the Windows system or\n";
		Script += "# user environment variable settings, in the 'PySAGA/__init__.py'\n";
		Script += "# file, or in the individual Python script itself. To do the latter\n";
		Script += "# just uncomment the following line and adjust the path accordingly:\n";
		Script += "###import os; os.environ['SAGA_PATH'] = '" + AppPath + "'\n";
		Script += "\n";
		Script += "# Windows: The most convenient way to make PySAGA available to all your\n";
		Script += "# Python scripts is to copy the PySAGA folder to the 'Lib/site-packages/'\n";
		Script += "# folder of your Python installation. If don't want to do this or if you\n";
		Script += "# don't have the rights to do so, you can also copy it to the folder with\n";
		Script += "# the Python scripts in which you want to use PySAGA, or alternatively\n";
		Script += "# you can add the path containing the PySAGA folder (e.g. the path to your\n";
		Script += "# SAGA installation) to the PYTHONPATH environment variable. To do this\n";
		Script += "# from within your script you can also take the following command (just\n";
		Script += "# uncomment the following line and adjust the path accordingly):\n";
		Script += "###import sys; sys.path.insert(1, '" + AppPath + "')\n";
		Script += "\n";
#endif // _SAGA_MSW
		Script += "# Import saga_api from PySAGA:\n";
		Script += "from PySAGA import saga_api\n";
		Script += "\n";
		Script += "\n";
		Script += "#_________________________________________\n";
		Script += "##########################################\n";
		Script += "def Run_" + Name + "(Results):\n";
	}

	//-----------------------------------------------------
	if( bHeader ) Script += "    # Get the tool:\n";
	Script += "    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('" + Get_Library() + "', '" + Get_ID() + "')\n";
	Script += "    if not Tool:\n";
	Script += "        saga_api.SG_UI_Msg_Add_Error('Failed to request tool: " + Get_Name() + "')\n";
	Script += "        return False\n";
	Script += "\n";
	if( bHeader ) Script += "    # Set the parameter interface:\n";
	Script += "    Tool.Reset()\n";

	//-------------------------------------------------
	_Get_Script_Python(Script, Get_Parameters(), bAllArguments);

	for(int iParameters=0; iParameters<Get_Parameters_Count(); iParameters++)
	{
		_Get_Script_Python(Script, Get_Parameters(iParameters), bAllArguments, Get_Parameters(iParameters)->Get_Identifier());
	}

	//-------------------------------------------------
	Script += "\n";
	if( bHeader ) Script += "    # Execute the tool:\n";
	Script += "    if not Tool.Execute():\n";
	Script += "        saga_api.SG_UI_Msg_Add_Error('failed to execute tool: ' + Tool.Get_Name().c_str())\n";
	Script += "        return False\n";
	Script += "\n";
	if( bHeader ) Script += "    # Request the results:\n";

	for(int iParameter=0; iParameter<Get_Parameters()->Get_Count(); iParameter++)
	{
		CSG_Parameter *p = Get_Parameters()->Get_Parameter(iParameter);

		if( p->is_Output() )
		{
			CSG_String id(p->Get_Identifier()), type, ext;

			switch( p->Get_DataObject_Type() )
			{
			case SG_DATAOBJECT_TYPE_Grid      : type = "Grid"      ; ext = "sg-grd-z"; break;
			case SG_DATAOBJECT_TYPE_Grids     : type = "Grids"     ; ext = "sg-gds-z"; break;
			case SG_DATAOBJECT_TYPE_Table     : type = "Table"     ; ext = "txt"     ; break;
			case SG_DATAOBJECT_TYPE_Shapes    : type = "Shapes"    ; ext = "geojson" ; break;
			case SG_DATAOBJECT_TYPE_PointCloud: type = "PointCloud"; ext = "sg-pts-z"; break;
			case SG_DATAOBJECT_TYPE_TIN       : type = "TIN"       ; ext = "geojson" ; break;
			default                           : type = ""          ; ext = ""        ; break;
			}

			if( p->is_DataObject() )
			{
				Script += "    Data = Tool.Get_Parameter('" +  id + "').as" + type + "()\n";

				if( bHeader )
				{
					Script += "    Data.Save('{:s}/{:s}.{:s}'.format(Results, Data.Get_Name(), '" + ext + "'))\n\n";
				}
			}
			else if( p->is_DataObject_List() )
			{
				Script += "    List = Tool.Get_Parameter('" +  id + "').as" + type + "List()\n";
				Script += "    for i in range(0, List.Get_Item_Count()):\n";

				if( bHeader )
				{
					Script += "        List.Get_Item(i).Save('{:s}/{:s}_{:d}.{:s}'.format(Results, List.Get_Name(), i, '" + ext + "'))\n\n";
				}
				else
				{
					Script += "        Data = List.Get_Item(i)\n";
				}
			}
		}
	}

	//-----------------------------------------------------
	if( bHeader )
	{
		Script += "    # job is done, free memory resources:\n";
		Script += "    saga_api.SG_Get_Data_Manager().Delete_All()\n";
		Script += "\n";
		Script += "    return True\n";
		Script += "\n";
		Script += "\n";
		Script += "#_________________________________________\n";
		Script += "##########################################\n";
        Script += "print('This is a simple template for using a SAGA tool through Python.')\n";
        Script += "print('Please edit the script to make it work properly before using it!')\n";
		Script += "\n";
		Script += "# Run_" + Name + "('.')\n";
	}

	return( Script );
}

//---------------------------------------------------------
void CSG_Tool::_Get_Script_Python(CSG_String &Script, CSG_Parameters *pParameters, bool bAllArguments, const CSG_String &Prefix)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter *p = pParameters->Get_Parameter(iParameter);

		if( !bAllArguments && (!p->is_Enabled(false) || p->is_Information() || !p->do_UseInCMD()) )
		{
			continue;
		}

		CSG_String ID(p->Get_Identifier());

		if( !Prefix.is_Empty() )
		{
			ID.Prepend(Prefix + ".");
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool           :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', %s)\n", ID.c_str(), p->asBool() ? SG_T("True") : SG_T("False"));
			break;

		case PARAMETER_TYPE_Int            :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', %d)\n", ID.c_str(), p->asInt());
			break;

		case PARAMETER_TYPE_Data_Type      :
		case PARAMETER_TYPE_Choice         :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', %d) # '%s'\n", ID.c_str(), p->asInt(), p->asString());
			break;

		case PARAMETER_TYPE_Choices        :
		case PARAMETER_TYPE_Table_Field    :
		case PARAMETER_TYPE_Table_Fields   :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', '%s')\n", ID.c_str(), p->asString());
			break;

        case PARAMETER_TYPE_Color          :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', '%s')\n", ID.c_str(), SG_Color_To_Text(p->asColor()).c_str());
            break;

		case PARAMETER_TYPE_Double         :
		case PARAMETER_TYPE_Degree         :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', %g)\n", ID.c_str(), p->asDouble());
			break;

		case PARAMETER_TYPE_Range          :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s.MIN', %g)\n", ID.c_str(), p->asRange()->Get_Min());
			Script += CSG_String::Format("    Tool.Set_Parameter('%s.MAX', %g)\n", ID.c_str(), p->asRange()->Get_Max());
			break;

		case PARAMETER_TYPE_String         :
			if( ((CSG_Parameter_String *)p)->is_Password() )
			{
				Script += CSG_String::Format("    Tool.Set_Parameter('%s', '***')\n", ID.c_str());
				break;
			}

		case PARAMETER_TYPE_Date           :
		case PARAMETER_TYPE_Text           :
		case PARAMETER_TYPE_FilePath       :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', '%s')\n", ID.c_str(), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable     :
			Script += CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.SG_Create_Table('table.txt'))\n", ID.c_str());
			break;

		case PARAMETER_TYPE_Grid_System    :
			if( p->Get_Children_Count() == 0 )
			{
				Script += CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.CSG_Grid_System(%g, %g, %g, %d, %d))\n", ID.c_str(),
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
				Script += CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.SG_Get_Data_Manager().Add('%s input file%s'))\n", ID.c_str(),
					SG_Get_DataObject_Name(p->Get_DataObject_Type()).c_str(), p->is_Optional() ? SG_T(", optional") : SG_T("")
				);
			}
			else if( p->is_Output() && p->is_Optional() )
			{
				Script += CSG_String::Format("    Tool.Set_Parameter('%s', saga_api.SG_Get_Create_Pointer()) # optional output, remove this line, if you don't want to create it\n", ID.c_str());
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
				Script += CSG_String::Format("    Tool.Get_Parameter('%s').asList().Add_Item('%s input list%s')\n", ID.c_str(),
					SG_Get_DataObject_Name(p->Get_DataObject_Type()).c_str(), p->is_Optional() ? SG_T(", optional") : SG_T("")
				);
			}
			break;

		case PARAMETER_TYPE_Parameters     :
			_Get_Script_Python(Script, p->asParameters(), bAllArguments, ID);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool::_Get_Script_Python_Wrap(bool bHeader, bool bName, bool bCall, int AllArguments, bool bWrapArgs)
{
	CSG_String Arguments, Description, Code;

	if( AllArguments >= 0 ) // if( AllArguments < 0 ) => skip arguments list
	{
		for(int i=0; i<Parameters.Get_Count(); i++)	// add input that is not optional in 1st place
		{
			_Get_Script_Python_Wrap(Parameters[i], PARAMETER_INPUT         , Arguments, Description, Code, bCall && bHeader, AllArguments == 0, bWrapArgs);
		}

		for(int i=0; i<Parameters.Get_Count(); i++) // add optional input in 2nd place
		{
			_Get_Script_Python_Wrap(Parameters[i], PARAMETER_INPUT_OPTIONAL, Arguments, Description, Code, bCall && bHeader, AllArguments == 0, bWrapArgs);
		}

		for(int i=0; i<Parameters.Get_Count(); i++) // add output
		{
			_Get_Script_Python_Wrap(Parameters[i], PARAMETER_OUTPUT        , Arguments, Description, Code, bCall && bHeader, AllArguments == 0, bWrapArgs);
		}

		for(int i=0; i<Parameters.Get_Count(); i++) // add options
		{
			_Get_Script_Python_Wrap(Parameters[i], 0                       , Arguments, Description, Code, bCall           , AllArguments == 0, bWrapArgs);
		}
	}

	//---------------------------------------------------------
	CSG_String Name, Expected(Get_Name()); Expected.Replace("'", "\\'");
	
	if( bName )
	{
		Name = Get_Name();

		if( isdigit(Name[0]) ) // ...in case first letter is a numeric character, what is not allowed for Python function names!
		{
			Name.Prepend("_");
		}
	
		Name.Replace(" ", "_");
		Name.Replace("(", "");
		Name.Replace(")", "");
		Name.Replace("[", "");
		Name.Replace("]", "");
		Name.Replace(".", "");
		Name.Replace(",", "");
		Name.Replace("/", "");
		Name.Replace("-", "");
		Name.Replace("'", "");
		Name.Replace("&", "and");
	}
	else
	{
		Name = "run_tool_" + Get_Library() + "_" + Get_ID();

		Name.Replace(" ", "_");
	}

	//---------------------------------------------------------
	CSG_String Script;

	if( bCall )
	{
		if( bHeader )
		{
			Script += "from PySAGA.tools import " + Get_Library() + "\n\n";

			if( !Code.is_Empty() )
			{
				Script += Code + "\n";
			}
		}

		Script += Get_Library() + '.' + Name + "(" + Arguments + ")\n";
	}
	else
	{
		CSG_Strings _Description = SG_String_Tokenize(SG_HTML_Tag_Replacer(Get_Description()), "\n");

		if( bHeader )
		{
			Script += "#! /usr/bin/env python\n";
			Script += "from PySAGA.helper import Tool_Wrapper\n\n";
		}

		Script += "def " + Name + "(" + Arguments + ", Verbose=2):\n";
		Script += "    '''\n";
		Script += "    " + Get_Name() + "\n";
		Script += "    ----------\n";
		Script += "    [" + Get_Library() + "." + Get_ID() + "]\\n\n";
		for(int i=0; i<_Description.Get_Count(); i++)
		{
			_Description[i].Trim_Both(); Script += "    " + _Description[i] + "\\n\n";
		}
		Script += "    Arguments\n";
		Script += "    ----------\n";
		Script += Description + "\n";
		Script += "    - Verbose [`integer number`] : Verbosity level, 0=silent, 1=tool name and success notification, 2=complete tool output.\\n\n";
		Script += "    Returns\n";
		Script += "    ----------\n";
		Script += "    `boolean` : `True` on success, `False` on failure.\n";
		Script += "    '''\n";
		Script += "    Tool = Tool_Wrapper('" + Get_Library() + "', '" + Get_ID() + "', '" + Expected + "')\n";
		Script += "    if Tool.is_Okay():\n";
		Script += Code;
		Script += "        return Tool.Execute(Verbose)\n";
		Script += "    return False\n\n";
	}

	return( Script );
}

//---------------------------------------------------------
bool CSG_Tool::_Get_Script_Python_Wrap(const CSG_Parameter &Parameter, int Constraint, CSG_String &Arguments, CSG_String &Description, CSG_String &Code, bool bCall, bool bOnlyNonDefaults, bool bWrapArgs, const CSG_String &Prefix)
{
	if( Parameter.do_UseInCMD() == false
	||  Parameter.is_Information()
	||  Parameter.Get_Type() == PARAMETER_TYPE_Node
	||  Parameter.Cmp_Identifier("PARAMETERS_GRID_SYSTEM") )
	{
		return( false );
	}

	if( bCall && !Parameter.is_Enabled() && !(Parameter.is_Output() && Parameter.Get_Parent() && Parameter.Get_Parent()->Cmp_Identifier("TARGET_SYSTEM")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String ID(Parameter.Get_Identifier());

	if( !Prefix.is_Empty() )
	{
		ID.Prepend(Prefix + ".");
	}

	CSG_String Argument(ID);

	if( Argument[0] >= '0' && Argument[0] <= '9' )
	{
		Argument.Prepend('_');
	}

	Argument.Replace(".", "_");
	Argument.Replace("|", "_");
	Argument.Replace(" ", "_");

	if( Argument.Length() > 2 )
	{
		Argument.Make_Upper();
	}

	//-----------------------------------------------------
	if( Parameter.asParameters() ) // PARAMETER_TYPE_Parameters
	{
		bool bResult = false;

		for(int i=0; i<(*Parameter.asParameters()).Get_Count(); i++)
		{
			if( _Get_Script_Python_Wrap((*Parameter.asParameters())[i], Constraint, Arguments, Description, Code, bCall, bOnlyNonDefaults, bWrapArgs, ID) )
			{
				bResult = true;
			}
		}

		return( bResult );
	}

	//-----------------------------------------------------
	if( Parameter.is_Input () && !Parameter.is_Optional() && Constraint != PARAMETER_INPUT          ) { return( false ); }
	if( Parameter.is_Input () &&  Parameter.is_Optional() && Constraint != PARAMETER_INPUT_OPTIONAL ) { return( false ); }
	if( Parameter.is_Output() &&                             Constraint != PARAMETER_OUTPUT         ) { return( false ); }
	if( Parameter.is_Option() &&                             Constraint != 0                        ) { return( false ); }

	//-----------------------------------------------------
	if( bCall )
	{
		CSG_String Value;

		if( Parameter.is_DataObject() )
		{
			Value = Parameter.Get_Identifier(); Value.Make_Lower();

			if( Parameter.is_Input() )
			{
				if( (bOnlyNonDefaults && !Parameter.asDataObject()) && Parameter.is_Optional() ) // don't add optional input that has not been set
				{
					return( false );
				}

				CSG_String File(Parameter.asDataObject() && Parameter.asDataObject()->Get_File_Name(false)
					? Parameter.asDataObject()->Get_File_Name(false) : SG_T("data object file")
				); File.Replace("\\", "/");

				Code += Value + " = saga_api.SG_Get_Data_Manager().Add_" + SG_Get_DataObject_Class_Name(Parameter.Get_DataObject_Type()).AfterFirst('_') + "('" + File + "') # input data object\n";
			}
			else // if( Parameter.is_Output() )
			{
				if( (bOnlyNonDefaults && !Parameter.asDataObject()) && Parameter.is_Optional() ) // don't add optional output that has not been requested
				{
					return( false );
				}

				Code += Value + " = saga_api.SG_Get_Data_Manager().Add_" + SG_Get_DataObject_Class_Name(Parameter.Get_DataObject_Type()).AfterFirst('_') + "() # output data object\n";
			}
		}
		else if( Parameter.is_DataObject_List() )
		{
			Value = Parameter.Get_Identifier(); Value.Make_Lower();

			if( Parameter.is_Input() )
			{
				if( (bOnlyNonDefaults && Parameter.asList()->Get_Item_Count() < 1) && Parameter.is_Optional() ) // don't add optional input that has not been set
				{
					return( false );
				}

				Code += Value + " = [] # Python list with input data objects of type 'saga_api." + SG_Get_DataObject_Class_Name(Parameter.Get_DataObject_Type()) + "'\n";
			}
			else // if( Parameter.is_Output() )
			{
				Code += Value + " = [] # Python list, will become filled after successful execution with output data objects of type 'saga_api." + SG_Get_DataObject_Class_Name(Parameter.Get_DataObject_Type()) + "'\n";
			}
		}
		else if( Parameter.is_Option() )
		{
			if( bOnlyNonDefaults && Parameter.is_Default() )
			{
				return( false );
			}

			switch( Parameter.Get_Type() )
			{
			case PARAMETER_TYPE_Bool        : Value = Parameter.asBool() ? "True" : "False"; break;

			case PARAMETER_TYPE_Int         : Value.Printf("%d", Parameter.asInt()); break;

			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      : Value.Printf("%f", Parameter.asDouble()); break;

			case PARAMETER_TYPE_Color       : Value = SG_Color_To_Text(Parameter.asColor()); break;

			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_Date        :
			case PARAMETER_TYPE_Range       :
			case PARAMETER_TYPE_Data_Type   :
			case PARAMETER_TYPE_Choice      :
			case PARAMETER_TYPE_Choices     :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
			case PARAMETER_TYPE_FilePath    : Value.Printf("'%s'", Parameter.asString()); break;

			case PARAMETER_TYPE_Grid_System :
				if( !Parameter.Cmp_Identifier("TARGET_SYSTEM") )
				{
					return( false );
				}

				Value.Printf("'%s'", Parameter.asGrid_System()->asString());
				break;

			default                         : return( false );
			}
		}

		if( !Value.is_Empty() )
		{
			if( bWrapArgs )
			{
				Arguments += !Arguments.is_Empty() ? ",\n    " : "\n    ";
			}
			else if( !Arguments.is_Empty() )
			{
				Arguments += ", ";
			}

			Arguments += Argument + "=" + Value;
		}

		return( true );
	}

	//-----------------------------------------------------
	if( bWrapArgs )
	{
		Arguments += !Arguments.is_Empty() ? ",\n    " : "\n    ";
	}
	else if( !Arguments.is_Empty() )
	{
		Arguments += ", ";
	}

	Arguments += Argument + "=None";

	//-----------------------------------------------------
	Code += "        ";

	if( Parameter.is_Input () ) { Code += "Tool.Set_Input "; }
	if( Parameter.is_Output() ) { Code += "Tool.Set_Output"; }
	if( Parameter.is_Option() ) { Code += "Tool.Set_Option"; }

	Code += CSG_String::Format("('%s', %s)\n", ID.c_str(), Argument.c_str());

	//-----------------------------------------------------
	Description += "    - " + Argument + " [`";

	if( Parameter.is_Input() )
	{
		Description += Parameter.is_Optional() ? "optional input " : "input ";
	}
	else if( Parameter.is_Output() )
	{
		Description += "output ";
	}

	Description += Parameter.Get_Type_Name() + "`] : " + Parameter.Get_Name();

	CSG_String s(Parameter.Get_Description(PARAMETER_DESCRIPTION_PROPERTIES|PARAMETER_DESCRIPTION_TEXT, SG_T(" ")));

	if( !s.is_Empty() )
	{
		Description += ". " + s;
	}

	Description += "\n";

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData CSG_Tool::Get_History(int Depth)
{
	CSG_MetaData History;

	History.Set_Name(SG_META_HISTORY);
	History.Add_Property("saga-version", SAGA_VERSION);

	if( Depth )
	{
		CSG_MetaData *pTool = History.Add_Child("TOOL");

		pTool->Add_Property("library", Get_Library());
		pTool->Add_Property("id"     , Get_ID     ());
		pTool->Add_Property("name"   , Get_Name   ());

		Parameters.Set_History(*pTool);

		pTool->Add_Children(History_Supplement);

		CSG_MetaData *pOutput = pTool->Add_Child("OUTPUT");
		pOutput->Add_Property("type", "");
		pOutput->Add_Property("id"  , "");
		pOutput->Add_Property("name", "");

		pTool->Del_Children(Depth, SG_T("TOOL"));
	}

	return( History );
}

//---------------------------------------------------------
bool CSG_Tool::Set_History(CSG_Data_Object *pDataObject, int Depth)
{
	if( pDataObject )
	{
		pDataObject->Get_History() = Get_History(Depth);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Tool::_Set_Output_History(void)
{
	CSG_MetaData History(Get_History(SG_Get_History_Depth()));

	for(int j=-1; j<Get_Parameters_Count(); j++)
	{
		CSG_Parameters *pParameters = j < 0 ? &Parameters : Get_Parameters(j);

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter *pParameter = pParameters->Get_Parameter(i);

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
	if( !pParameter && !((pParameter->is_DataObject() && pParameter->asDataObject()) || !(pParameter->is_DataObject_List() && pParameter->asList()->Get_Item_Count() > 0)) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData History;

	if( !pHistory )
	{
		History = Get_History(SG_Get_History_Depth()); pHistory = &History;
	}

	//-----------------------------------------------------
	CSG_MetaData *pOutput = pHistory->Get_Child("TOOL") ? pHistory->Get_Child("TOOL")->Get_Child("OUTPUT") : NULL;

	if( pOutput )
	{
		pOutput->Set_Property("type", pParameter->Get_Type_Identifier());
		pOutput->Set_Property("id"  , pParameter->Get_Identifier     ());
		pOutput->Set_Property("name", pParameter->Get_Name           ());
	}

	//-----------------------------------------------------
	if( pParameter->is_DataObject() && pParameter->asDataObject() )
	{
		if( pOutput )
		{
			pOutput->Set_Content(pParameter->asDataObject()->Get_Name());
		}

		pParameter->asDataObject()->Get_History().Assign(*pHistory);
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
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
