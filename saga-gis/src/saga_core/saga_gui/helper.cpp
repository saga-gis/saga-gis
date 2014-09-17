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
#include <wx/config.h>
#include <wx/cursor.h>
#include <wx/utils.h>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/protocol/http.h>

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

	s.Printf(wxT("%.*f"), SG_Get_Significant_Decimals(Value, maxDecimals), Value);

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
	bool	bNegative = false;

	if( Value < 0 )
	{
		Value		= fabs(Value);
		bNegative	= true;
	}

	Value	= fmod(Value, 360.0);
	Deg		= (int)Value;
	Value	= 60.0 * (Value - Deg);
	Min		= (int)Value;
	Value	= 60.0 * (Value - Min);
	Sec		= Value;

	if( bNegative )
	{
		Deg	*= -1.0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString	Get_nBytes_asString(double nBytes, int Precision)
{
	if( nBytes < 1024 )
	{
		return( wxString::Format(wxT("%.0f %s"), nBytes, wxT("bytes")) );
	}

	double	dSize	= nBytes / 1024.0;

	if( dSize < 1024 )
	{
		return( wxString::Format(wxT("%.*f %s"), Precision < 0 ? SG_Get_Significant_Decimals(dSize, 20) : Precision, dSize, wxT("kB")) );
	}

	dSize	/= 1024.0;

	if( dSize < 1024 )
	{
		return( wxString::Format(wxT("%.*f %s"), Precision < 0 ? SG_Get_Significant_Decimals(dSize, 20) : Precision, dSize, wxT("MB")) );
	}

	dSize	/= 1024.0;

	return( wxString::Format(wxT("%.*f %s"), Precision < 0 ? SG_Get_Significant_Decimals(dSize, 20) : Precision, dSize, wxT("GB")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		Get_Random(double loValue, double hiValue)
{
	return( loValue + (hiValue - loValue) * (double)rand() / (double)RAND_MAX );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString	Get_FilePath_Relative(const wxString &Directory, const wxString &FileName)
{
	wxFileName	fn(FileName);

	if( fn.GetPath().Find(Directory) == 0 )
	{
		fn.MakeRelativeTo(Directory);
	}

	return( fn.GetFullPath() );
}

//---------------------------------------------------------
wxString	Get_FilePath_Absolute(const wxString &Directory, const wxString &FileName)
{
	wxFileName	fn(FileName);

	if( !fn.IsAbsolute() )
	{
		fn.MakeAbsolute(Directory);
	}

	return( fn.GetFullPath() );
}

//---------------------------------------------------------
wxString		Get_TableInfo_asHTML(CSG_Table *pTable)
{
	wxString	s;

	if( pTable && pTable->is_Valid() )
	{
		s	+= wxString::Format(wxT("<hr><b>%s</b>"), _TL("Table Description"));

		s	+= wxString::Format(wxT("<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th>"),
			_TL("Field"),
			_TL("Name"),
			_TL("Type"),
			_TL("Minimum"),
			_TL("Maximum"),
			_TL("Mean"),
			_TL("Standard Deviation")
		);

		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			s	+= wxString::Format(wxT("<tr><td>%d</td><td>%s</td><td>%s</td>"),
				i + 1,
				pTable->Get_Field_Name(i),
				SG_Data_Type_Get_Name(pTable->Get_Field_Type(i)).c_str()
			);

			if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
			{
				s	+= wxString::Format(wxT("<td align=\"right\">%s</td><td align=\"right\">%s</td><td align=\"right\">%s</td><td align=\"right\">%s</td></tr>"),
					SG_Get_String(pTable->Get_Minimum(i), -6).c_str(),
					SG_Get_String(pTable->Get_Maximum(i), -6).c_str(),
					SG_Get_String(pTable->Get_Mean   (i), -6).c_str(),
					SG_Get_String(pTable->Get_StdDev (i), -6).c_str()
				);
			}
			else
			{
				s	+= wxString::Format(wxT("<td></td><td></td><td></td><td></td></tr>"));
			}
		}

		s	+= wxT("</table>");
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Set_Font(CSG_Parameter *pFont, wxFont &Font, wxColour &Colour)
{
	if( !pFont )
	{
		return( false );
	}

	Colour.Set(
		SG_GET_R(pFont->asColor()),
		SG_GET_G(pFont->asColor()),
		SG_GET_B(pFont->asColor())
	);

	Font.SetNativeFontInfo(pFont->asFont());

	return( true );
}

//---------------------------------------------------------
bool	Set_Font(const wxFont &Font, wxColour Colour, CSG_Parameter *pFont)
{
	if( !pFont )
	{
		return( false );
	}

	pFont->Set_Value((int)SG_GET_RGB(Colour.Red(), Colour.Green(), Colour.Blue()));
	pFont->Set_Value(Font.GetNativeFontInfoDesc().wx_str());

	return( true );
}

//---------------------------------------------------------
wxFont	Get_Font(CSG_Parameter *pFont)
{
	wxColour	Colour;
	wxFont		Font;

	Set_Font(pFont, Font, Colour);

	return( Font );
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
wxColour	Get_Color_Random(int rLo, int rHi, int gLo, int gHi, int bLo, int bHi)
{
	return( wxColour(
		(int)(0.5 + Get_Random(rLo, rHi)),
		(int)(0.5 + Get_Random(gLo, gHi)),
		(int)(0.5 + Get_Random(bLo, bHi))
	));
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

//---------------------------------------------------------
void		MDI_Top_Window_Push(wxWindow *pWindow)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->Top_Window_Push(pWindow);
	}
}

//---------------------------------------------------------
void		MDI_Top_Window_Pop(wxWindow *pWindow)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->Top_Window_Pop(pWindow);
	}
}

//---------------------------------------------------------
wxWindow *	MDI_Get_Top_Window(void)
{
	return( g_pSAGA_Frame ? g_pSAGA_Frame->Top_Window_Get() : NULL );
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
//		wxBell();
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
void		MSG_General_Add(const wxString &Message, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( g_pINFO )
	{
		g_pINFO->Get_General()->Add_String(Message, bNewLine, bTime, Style);
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
void		MSG_Error_Add(const wxString &Message, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Errors()->Add_String(Message, bNewLine, bTime, Style);
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
void		MSG_Execution_Add(const wxString &Message, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Execution()->Add_String(Message, bNewLine, bTime, Style);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		g_CONFIG_bSave	= true;

//---------------------------------------------------------
bool		CONFIG_Do_Save(bool bOn)
{
	if( g_CONFIG_bSave != bOn )
	{
		g_CONFIG_bSave	= bOn;

		if( g_CONFIG_bSave )
		{
			wxConfigBase::Get()->Flush();
		}
	}

	return( g_CONFIG_bSave );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, wxString &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, long &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, double &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, bool &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, const wxString &Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

bool		CONFIG_Write(const wxString &Group, const wxString &Entry, const char *Value)
{
	return( CONFIG_Write(Group, Entry, wxString(Value)) );
}

bool		CONFIG_Write(const wxString &Group, const wxString &Entry, const wchar_t *Value)
{
	return( CONFIG_Write(Group, Entry, wxString(Value)) );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, long Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, double Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, bool Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Delete(const wxString &Group)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		return( pConfig->DeleteGroup(Group) ? pConfig->Flush() : false );
	}

	return( true );
}

bool		CONFIG_Delete(const wxString &Group, const wxString &Entry)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

		return( pConfig->DeleteEntry(Entry) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Read(wxConfigBase *pConfig, CSG_Parameter *pParameter)
{
	long		l;
	double		d;
	wxString	s, Entry(pParameter->Get_Identifier());

	switch( pParameter->Get_Type() )
	{
	default:	return( false );

	case PARAMETER_TYPE_Bool    :
	case PARAMETER_TYPE_Int     :
	case PARAMETER_TYPE_Choice  :
	case PARAMETER_TYPE_Color   :
		return( pConfig->Read(Entry, &l) && pParameter->Set_Value((int)l) );

	case PARAMETER_TYPE_Double  :
	case PARAMETER_TYPE_Degree  :
		return( pConfig->Read(Entry, &d) && pParameter->Set_Value(d) );

	case PARAMETER_TYPE_String  :
	case PARAMETER_TYPE_Text    :
	case PARAMETER_TYPE_FilePath:
		return( pConfig->Read(Entry, &s) && pParameter->Set_Value((const SG_Char *)s) );

	case PARAMETER_TYPE_Range   :
		return(
			pConfig->Read(Entry + "_LO", &d) && pParameter->asRange()->Set_LoVal(d)
		&&	pConfig->Read(Entry + "_HI", &d) && pParameter->asRange()->Set_HiVal(d)
		);

	case PARAMETER_TYPE_Font    :
		return(
			pConfig->Read(Entry + "_FONT" , &s) && pParameter->Set_Value((const SG_Char *)s)
		&&	pConfig->Read(Entry + "_COLOR", &l) && pParameter->Set_Value((int)l)
		);

	case PARAMETER_TYPE_Parameters:
		return( CONFIG_Read(Entry + "/" + pParameter->Get_Identifier(), pParameter->asParameters()) );
	}
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, CSG_Parameters *pParameters)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CONFIG_Read(pConfig, pParameters->Get_Parameter(i));
	}

	return( pConfig->Flush() );
}

//---------------------------------------------------------
bool		CONFIG_Write(wxConfigBase *pConfig, CSG_Parameter *pParameter)
{
	if( g_CONFIG_bSave )
	{
		wxString	Entry(pParameter->Get_Identifier());

		switch( pParameter->Get_Type() )
		{
		default:	return( false );

		case PARAMETER_TYPE_Bool    :
		case PARAMETER_TYPE_Int     :
		case PARAMETER_TYPE_Choice  :
		case PARAMETER_TYPE_Color   :
			return( pConfig->Write(Entry, pParameter->asInt()) );

		case PARAMETER_TYPE_Double  :
		case PARAMETER_TYPE_Degree  :
			return( pConfig->Write(Entry, pParameter->asDouble()) );

		case PARAMETER_TYPE_String  :
		case PARAMETER_TYPE_Text    :
		case PARAMETER_TYPE_FilePath:
			return( pConfig->Write(Entry, pParameter->asString()) );

		case PARAMETER_TYPE_Range   :
			return(
				pConfig->Write(Entry + "_LO", pParameter->asRange()->Get_LoVal())
			&&	pConfig->Write(Entry + "_HI", pParameter->asRange()->Get_HiVal())
			);

		case PARAMETER_TYPE_Font    :
			return(
				pConfig->Write(Entry + "_FONT" , (const SG_Char *)pParameter->asPointer())
			&&	pConfig->Write(Entry + "_COLOR", pParameter->asInt())
			);

		case PARAMETER_TYPE_Parameters:
			return( CONFIG_Write(Entry + "/" + pParameter->Get_Identifier(), pParameter->asParameters()) );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, CSG_Parameters *pParameters)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format(wxT("/%s"), Group));

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CONFIG_Write(pConfig, pParameters->Get_Parameter(i));
		}

		return( pConfig->Flush() );
	}

	return( true );
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
void		STATUSBAR_Set_Text(const wxString &Text, int iPane)
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
bool		Open_Application(const wxString &Reference, const wxString &Mime_Extension)
{
	bool		bResult		= false;

	if( Reference.Length() )
	{
		wxString	Extension, sReference(Reference);
		wxFileType	*pFileType;

		if( Mime_Extension.Length() )
		{
			Extension	= Mime_Extension;
		}
		else if( sReference.Find(SG_T("ftp:")) == 0 || sReference.Find(SG_T("http:")) == 0 || sReference.Find(SG_T("https:")) == 0 )
		{
			Extension	= wxT("html");
		}
		else
		{
			wxFileName	FileName(Reference);

			Extension	= FileName.GetExt();
		}

		if( Extension.Length() > 0 && (pFileType = wxTheMimeTypesManager->GetFileTypeFromExtension(Extension)) != NULL )
		{
			wxString	Command;

			if( pFileType->GetOpenCommand(&Command, wxFileType::MessageParameters(Reference, wxT(""))) )
			{
				MSG_Execution_Add_Line();
				MSG_Execution_Add(Command, true, true);

				bResult	= wxExecute(Command) == 0;
			}

			delete(pFileType);
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool		Open_WebBrowser(const wxString &Reference)
{
	return( Open_Application(Reference, wxT("html")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString Get_Online_Module_Description(const wxString &Library, const wxString &ID)
{
	static bool	bBuisy	= false;

	wxString	Description;

	if( !bBuisy )
	{
		bBuisy	= true;

		wxInputStream	*pStream;
		wxHTTP			Server;

		wxString	sServer	= wxT("sourceforge.net");
		wxString	sPath	= wxT("/apps/trac/saga-gis/wiki/");
		wxString	sRoot	= wxT("/apps/trac/saga-gis/");

		wxString	sPage	= SG_File_Get_Name(Library, false).w_str();

		if( sPage.Length() > 3 && sPage[0] == wxT('l') && sPage[1] == wxT('i') && sPage[2] == wxT('b') )	// remove linux prefix 'lib'
		{
			sPage.Remove(0, 3);
		}

		if( !ID.IsEmpty() )
		{
			sPage	+= "_" + ID;
		}

		if( Server.Connect(sServer) && (pStream = Server.GetInputStream(sPath + sPage)) != NULL )
		{
			while( !pStream->Eof() )
			{
				Description	+= pStream->GetC();
			}

			if( Description.Find(wxT("Trac Error")) >= 0 )
			{
				Description.Clear();
			}
			else
			{
				int		n;

				if( (n = Description.Find(wxT("<div class=\"wikipage searchable\">"))) > 0 )
				{
					Description.Remove(0, n);

					if( (n = Description.Find(wxT("</div>"))) > 0 )
					{
						Description.RemoveLast(Description.Length() - (n - 6));
					}
				}

				Description.Replace(sRoot, wxT("http://") + sServer + sRoot);
				Description.Replace(wxT("\n"), wxT(""));
			}
		}

		bBuisy	= false;
	}

	return( Description );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
