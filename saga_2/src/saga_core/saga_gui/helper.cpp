
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
//                      Helper.cpp                       //
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
#include <wx/config.h>
#include <wx/cursor.h>
#include <wx/utils.h>
#include <wx/mimetype.h>

#include <saga_api/saga_api.h>

#include "helper.h"

#include "saga.h"
#include "saga_frame.h"

#include "info.h"
#include "info_messages.h"

#include "wksp_module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString	Get_SignificantDecimals_String(double Value, int maxDecimals)
{
	wxString	s;

	s.Printf("%.*f", SG_Get_Significant_Decimals(Value, maxDecimals), Value);

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		Degree_To_Decimal(double Deg, double Min, double Sec)
{
	return( Deg > 0.0
		? (Deg + Min / 60.0 + Sec / (60.0 * 60.0))
		: (Deg - Min / 60.0 - Sec / (60.0 * 60.0))
	);
}

//---------------------------------------------------------
void		Decimal_To_Degree(double Value, double &Deg, double &Min, double &Sec)
{
	Value	= fmod(Value, 360.0);
	Deg		= (int)Value;
	Value	= 60.0 * (Value - Deg);
	Min		= (int)Value;
	Value	= 60.0 * (Value - Min);
	Sec		= Value;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString	Get_FilePath_Relative(const char *Directory, const char *FileName)
{
	int		i, n;

	if( Directory && FileName && (n = strlen(Directory)) < (int)strlen(FileName) )
	{
		for(i=0; i<n; i++)
		{
			if( Directory[i] != FileName[i] )
			{
				return( FileName );
			}
		}

		return( FileName + n );
	}

	return( FileName );
}

//---------------------------------------------------------
wxString	Get_FilePath_Absolute(const char *Directory, const char *FileName)
{
	if( wxIsAbsolutePath(FileName) )
	{
		return( FileName );
	}

	return( wxString::Format("%s%s", Directory, FileName) );
//	return( SG_File_Make_Path(Directory, FileName, NULL).c_str() );
}

//---------------------------------------------------------
wxString		Get_TableInfo_asHTML(CTable *pTable)
{
	wxString	s;

	if( pTable && pTable->is_Valid() )
	{
		s.Append(wxString::Format("<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th>",
			LNG("[CAP] Field"), LNG("[CAP] Name"), LNG("[CAP] Type")
		));

		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			s.Append(wxString::Format("<tr><td>%d</td><td>%s</td><td>%s</td></tr>",
				i + 1, pTable->Get_Field_Name(i), TABLE_FieldType_Names[pTable->Get_Field_Type(i)]
			));
		}

		s.Append("</table>");
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxColour	Get_Color_asWX(int Color)
{
	return( wxColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)) );
}

//---------------------------------------------------------
int			Get_Color_asInt(wxColour Color)
{
	return( SG_GET_RGB(Color.Red(), Color.Green(), Color.Blue()) );
}

//---------------------------------------------------------
wxColour	SYS_Get_Color(wxSystemColour index)
{
	return( wxSystemSettings::GetColour(index) );
}

//---------------------------------------------------------
void		SYS_Set_Color_BG(wxWindow *pWindow, wxSystemColour index)
{
	if( pWindow )
	{
		pWindow->SetBackgroundColour(SYS_Get_Color(index));
	}
}

//---------------------------------------------------------
void		SYS_Set_Color_BG_Window(wxWindow *pWindow)
{
	if( pWindow )
	{
#if defined(__WXMSW__)
		pWindow->SetBackgroundColour(SYS_Get_Color(wxSYS_COLOUR_WINDOW));
#else
		pWindow->SetBackgroundColour(*wxWHITE);
#endif
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxWindow *	MDI_Get_Frame(void)
{
	return( g_pSAGA_Frame );
}

//---------------------------------------------------------
wxPoint		MDI_Get_Def_Position(void)
{
	static int	n	= 0;

	int		Height	= wxSystemSettings::GetMetric(wxSYS_CAPTION_Y);

	wxPoint	p(n * Height, n * Height);

	n	= n < 10 ? n + 1 : 0;

	return( p );
}

//---------------------------------------------------------
wxSize		MDI_Get_Def_Size(void)
{
	return( wxSize(400, 300) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Set_Buisy_Cursor(bool bOn)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->SetCursor(bOn ? *wxHOURGLASS_CURSOR : *wxSTANDARD_CURSOR);
	}
}

//---------------------------------------------------------
void		Do_Beep(int Style)
{
	switch( Style )
	{
	case -1:	// no noise...
		break;

	case 0:	default:
#if defined(__WXMSW__)
		Beep(330, 100);
		Beep(440, 100);
		Beep(550, 100);
#else
		wxBell();
#endif
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		MSG_General_Add_Line(void)
{
	if( g_pINFO )
	{
		g_pINFO->Get_General()->Add_Line();
	}
}

//---------------------------------------------------------
void		MSG_General_Add(const char *Message, bool bNewLine, bool bTime)
{
	if( g_pINFO )
	{
		g_pINFO->Get_General()->Add_String(Message, bNewLine, bTime);
	}
}

//---------------------------------------------------------
void		MSG_Error_Add_Line(void)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Errors()->Add_Line();
	}
}

//---------------------------------------------------------
void		MSG_Error_Add(const char *Message, bool bNewLine, bool bTime)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Errors()->Add_String(Message, bNewLine, bTime);
	}
}

//---------------------------------------------------------
void		MSG_Execution_Add_Line(void)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Execution()->Add_Line();
	}
}

//---------------------------------------------------------
void		MSG_Execution_Add(const char *Message, bool bNewLine, bool bTime)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Execution()->Add_String(Message, bNewLine, bTime);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		CONFIG_Read(const char *Group, const char *Entry, wxString &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const char *Group, const char *Entry, long &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const char *Group, const char *Entry, double &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const char *Group, const char *Entry, bool &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Write(const char *Group, const char *Entry, const char *Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
}

//---------------------------------------------------------
bool		CONFIG_Write(const char *Group, const char *Entry, long Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
}

//---------------------------------------------------------
bool		CONFIG_Write(const char *Group, const char *Entry, double Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
}

//---------------------------------------------------------
bool		CONFIG_Write(const char *Group, const char *Entry, bool Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
}

//---------------------------------------------------------
bool		CONFIG_Delete(const char *Group)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	return( pConfig->DeleteGroup(Group) ? pConfig->Flush() : false );
}

bool		CONFIG_Delete(const char *Group, const char *Entry)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->DeleteEntry(Entry) ? pConfig->Flush() : false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		PROCESS_is_Executing(void)
{
	return( g_pModule != NULL );
}

//---------------------------------------------------------
bool		PROCESS_Wait(void)
{
	if( g_pSAGA )
	{
		return( g_pSAGA->Process_Wait() );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROCESS_Get_Okay(bool bBlink)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->Process_Get_Okay(bBlink) );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROCESS_Set_Okay(bool bOkay)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->Process_Set_Okay(bOkay) );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROGRESSBAR_Set_Position(int Position)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->ProgressBar_Set_Position(Position) );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROGRESSBAR_Set_Position(double Position, double Range)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->ProgressBar_Set_Position(Position, Range) );
	}

	return( true );
}

//---------------------------------------------------------
void		STATUSBAR_Set_Text(const char *Text, int iPane)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->StatusBar_Set_Text(Text, iPane);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Open_WebBrowser(const char *HRef)
{
	bool		bResult	= false;
	wxString	Command;
	wxFileType	*pFileType;

	if( (pFileType = wxTheMimeTypesManager->GetFileTypeFromExtension("html")) != NULL )
	{
		if( pFileType->GetOpenCommand(&Command, wxFileType::MessageParameters(HRef, _T(""))) )
		{
			bResult	= wxExecute(Command) == 0;
		}

		delete(pFileType);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
