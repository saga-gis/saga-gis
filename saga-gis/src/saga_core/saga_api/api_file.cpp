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
//                    api_file.cpp                       //
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
#include <wx/utils.h>
#include <wx/filename.h>

#include "api_core.h"

#if defined(_SAGA_VC)
	#define SG_FILE_TELL	_ftelli64
	#define SG_FILE_SEEK	_fseeki64
	#define SG_FILE_SIZE	__int64
#else
	#define SG_FILE_TELL	ftell
	#define SG_FILE_SEEK	fseek
	#define SG_FILE_SIZE	long
#endif

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_File::CSG_File(void)
{
	m_pStream	= NULL;
	m_Encoding	= SG_FILE_ENCODING_CHAR;
}

//---------------------------------------------------------
CSG_File::CSG_File(const CSG_String &FileName, int Mode, bool bBinary, int Encoding)
{
	m_pStream	= NULL;

	Open(FileName, Mode, bBinary, Encoding);
}

//---------------------------------------------------------
CSG_File::~CSG_File(void)
{
	Close();
}

//---------------------------------------------------------
bool CSG_File::Attach(FILE *Stream)
{
	Close();

	m_pStream	= Stream;

	return( true );
}

//---------------------------------------------------------
bool CSG_File::Detach(void)
{
	m_pStream	= NULL;

	return( true );
}

//---------------------------------------------------------
bool CSG_File::Open(const CSG_String &File_Name, int Mode, bool bBinary, int Encoding)
{
	Close();

	m_Encoding	= Encoding;

	CSG_String	sMode;

	switch( Mode )
	{
	default:	return( false );
	case SG_FILE_R:		sMode	= bBinary ? SG_T("rb" ) : SG_T("r" );	break;
	case SG_FILE_W:		sMode	= bBinary ? SG_T("wb" ) : SG_T("w" );	break;
	case SG_FILE_RW:	sMode	= bBinary ? SG_T("wb+") : SG_T("w+");	break;
	case SG_FILE_WA:	sMode	= bBinary ? SG_T("ab" ) : SG_T("a" );	break;
	case SG_FILE_RWA:	sMode	= bBinary ? SG_T("rb+") : SG_T("r+");	break;
	}

	switch( Encoding )
	{
	case SG_FILE_ENCODING_CHAR:		default:	break;
	case SG_FILE_ENCODING_UNICODE:	sMode	+= SG_T(", ccs=UNICODE");	break;
	case SG_FILE_ENCODING_UTF8:		sMode	+= SG_T(", ccs=UTF-8"  );	break;
	case SG_FILE_ENCODING_UTF16:	sMode	+= SG_T(", ccs=UTF-16" );	break;
	}

	if( File_Name.Length() > 0 )
	{
		m_pStream	= fopen(File_Name, sMode);
	}

	return( m_pStream != NULL );
}

//---------------------------------------------------------
bool CSG_File::Close(void)
{
	if( m_pStream )
	{
		fclose(m_pStream);

		m_pStream	= NULL;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
sLong CSG_File::Length(void)	const
{
	if( m_pStream )
	{
		SG_FILE_SIZE	pos	= SG_FILE_TELL(m_pStream);	SG_FILE_SEEK(m_pStream,   0, SEEK_END);
		SG_FILE_SIZE	len	= SG_FILE_TELL(m_pStream);	SG_FILE_SEEK(m_pStream, pos, SEEK_SET);

		return( len );
	}

	return( -1 );
}

//---------------------------------------------------------
bool CSG_File::is_EOF(void)	const
{
	return( m_pStream == NULL || feof(m_pStream) != 0 );
}

//---------------------------------------------------------
bool CSG_File::Seek(sLong Offset, int Origin) const
{
	switch( Origin )
	{
	default:
	case SG_FILE_START:		Origin	= SEEK_SET;	break;
	case SG_FILE_CURRENT:	Origin	= SEEK_CUR;	break;
	case SG_FILE_END:		Origin	= SEEK_END;	break;
	}

	return( m_pStream ? !SG_FILE_SEEK(m_pStream, Offset, Origin) : false );
}

//---------------------------------------------------------
bool CSG_File::Seek_Start(void) const
{
	return( m_pStream && SG_FILE_SEEK(m_pStream, 0, SEEK_SET) == 0 );
}

//---------------------------------------------------------
bool CSG_File::Seek_End(void) const
{
	return( m_pStream && SG_FILE_SEEK(m_pStream, 0, SEEK_END) == 0 );
}

//---------------------------------------------------------
sLong CSG_File::Tell(void) const
{
	return( m_pStream ? SG_FILE_TELL(m_pStream) : -1 );
}

//---------------------------------------------------------
bool CSG_File::Flush(void) const
{
	return( m_pStream ? !fflush(m_pStream) : false );
}

//---------------------------------------------------------
int CSG_File::Printf(const char *Format, ...)
{
	if( !m_pStream )
	{
		return( 0 );
	}

#ifdef _SAGA_LINUX
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int	result	= wxVfprintf(m_pStream, _Format, argptr);
#else
	va_list	argptr;	va_start(argptr, Format);

	int	result	= wxVfprintf(m_pStream, Format, argptr);
#endif

	va_end(argptr);

	return( result );
}

//---------------------------------------------------------
int CSG_File::Printf(const wchar_t *Format, ...)
{
	if( !m_pStream )
	{
		return( 0 );
	}

#ifdef _SAGA_LINUX
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int	result	= wxVfprintf(m_pStream, _Format, argptr);
#else
	va_list	argptr;	va_start(argptr, Format);

	int	result	= wxVfprintf(m_pStream, Format, argptr);
#endif

	va_end(argptr);

	return( result );
}

//---------------------------------------------------------
size_t CSG_File::Read(void *Buffer, size_t Size, size_t Count) const
{
	return( m_pStream ? fread(Buffer, Size, Count, m_pStream) : 0 );
}

size_t CSG_File::Read(CSG_String &Buffer, size_t Size) const
{
	if( m_pStream )
	{
		char	*b	= (char *)SG_Calloc(Size + 1, sizeof(char));
		size_t	 i	= fread(b, sizeof(char), Size, m_pStream);
		Buffer		= b;
		SG_Free(b);

		return( i );
	}

	return( 0 );
}

//---------------------------------------------------------
size_t CSG_File::Write(void *Buffer, size_t Size, size_t Count) const
{
	return( m_pStream && Size > 0 && Count > 0 ? fwrite(Buffer, Size, Count, m_pStream) : 0 );
}

size_t CSG_File::Write(const CSG_String &Buffer) const
{
	return( Write((void *)Buffer.b_str(), sizeof(char), strlen(Buffer.b_str())) );
}

//---------------------------------------------------------
bool CSG_File::Read_Line(CSG_String &sLine)	const
{
	int		c;

	if( m_pStream && !feof(m_pStream) )
	{
		sLine.Clear();

		while( !feof(m_pStream) && (c = fgetc(m_pStream)) != 0x0A && c != EOF )
		{
			if( c != 0x0D )
			{
				sLine.Append((char)c);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_File::Read_Char(void) const
{
	if( m_pStream )
	{
		return( getc(m_pStream) );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_File::Read_Int(bool bByteOrderBig) const
{
	int		Value	= 0;

	if( Read(&Value, sizeof(Value)) == 1 )
	{
		if( bByteOrderBig )
		{
			SG_Swap_Bytes(&Value, sizeof(Value));
		}
	}

	return( Value );
}

bool CSG_File::Write_Int(int Value, bool bByteOrderBig)
{
	if( bByteOrderBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Write(&Value, sizeof(Value)) == sizeof(Value) );
}

//---------------------------------------------------------
double CSG_File::Read_Double(bool bByteOrderBig) const
{
	double	Value	= 0;

	if( Read(&Value, sizeof(Value)) == 1 )
	{
		if( bByteOrderBig )
		{
			SG_Swap_Bytes(&Value, sizeof(Value));
		}
	}

	return( Value );
}

bool CSG_File::Write_Double(double Value, bool bByteOrderBig)
{
	if( bByteOrderBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Write(&Value, sizeof(Value)) == sizeof(Value) );
}

//---------------------------------------------------------
bool CSG_File::Scan(int &Value) const
{
	return( m_pStream && fscanf(m_pStream, "%d" , &Value) == 1 );
}

bool CSG_File::Scan(double &Value) const
{
	return( m_pStream && fscanf(m_pStream, "%lf", &Value) == 1 );
}

bool CSG_File::Scan(CSG_String &Value, SG_Char Separator) const
{
	if( m_pStream && !feof(m_pStream) )
	{
		int		c;

		Value.Clear();

		while( !feof(m_pStream) && (c = fgetc(m_pStream)) != Separator && c != EOF )
		{
			Value	+= (char)c;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_File::Scan_Int(void) const
{
	int		Value;

	return( Scan(Value) ? Value : 0 );
}

double CSG_File::Scan_Double(void) const
{
	double	Value;

	return( Scan(Value) ? Value : 0.0 );
}

CSG_String CSG_File::Scan_String(SG_Char Separator) const
{
	CSG_String	Value;

	Scan(Value, Separator);

	return( Value );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Dir_Exists(const SG_Char *Directory)
{
	return( Directory && *Directory && wxFileName::DirExists(Directory) );
}

//---------------------------------------------------------
bool			SG_Dir_Create(const SG_Char *Directory)
{
	if( SG_Dir_Exists(Directory) )
	{
		return( true );
	}

	return( wxFileName::Mkdir(Directory) );
}

//---------------------------------------------------------
CSG_String		SG_Dir_Get_Current(void)
{
	wxString cwd = wxFileName::GetCwd();
	return( CSG_String(&cwd) );
}

//---------------------------------------------------------
CSG_String		SG_Dir_Get_Temp(void)
{
        wxString fname = wxFileName::GetTempDir();
	return( CSG_String(&fname) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_File_Exists(const SG_Char *FileName)
{
	return( FileName && *FileName && wxFileExists(FileName) );
}

//---------------------------------------------------------
bool			SG_File_Delete(const SG_Char *FileName)
{
	return( SG_File_Exists(FileName) && wxRemoveFile(FileName) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name_Temp(const SG_Char *Prefix, const SG_Char *Directory)
{
	if( !SG_Dir_Exists(Directory) )
	{
		return( CSG_String(wxFileName::CreateTempFileName(Prefix).wc_str()) );
	}

	return( CSG_String(wxFileName::CreateTempFileName(SG_File_Make_Path(Directory, Prefix).w_str()).wc_str()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name(const SG_Char *full_Path, bool bExtension)
{
	wxFileName	fn(full_Path);
	CSG_String	s(fn.GetFullName().wc_str());

	return( !bExtension && s.Find(SG_T(".")) >= 0 ? s.BeforeLast(SG_T('.')) : s );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path(const SG_Char *full_Path)
{
	if( full_Path && *full_Path )
	{
		wxFileName	fn(full_Path);

		return( CSG_String(fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR).wc_str()) );
	}

	return( SG_T("") );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path_Absolute	(const SG_Char *full_Path)
{
	wxString	Path;

	if( full_Path && *full_Path )
	{
		wxFileName	fn(full_Path);

		fn.MakeAbsolute();

		Path	= fn.GetFullPath();
	}

	return( &Path );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path_Relative	(const SG_Char *Directory, const SG_Char *full_Path)
{
	wxFileName	fn(full_Path);

	fn.MakeRelativeTo(Directory);

	return( CSG_String(fn.GetFullPath().wc_str()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Make_Path(const SG_Char *Directory, const SG_Char *Name, const SG_Char *Extension)
{
	wxFileName	fn;

	fn.AssignDir(Directory && *Directory ? Directory : SG_File_Get_Path(Name).c_str());

	if( Extension && *Extension )
	{
		fn.SetName		(SG_File_Get_Name(Name, false).c_str());
		fn.SetExt		(Extension);
	}
	else
	{
		fn.SetFullName	(SG_File_Get_Name(Name,  true).c_str());
	}

	return( CSG_String(fn.GetFullPath().wc_str()) );
}

//---------------------------------------------------------
bool			SG_File_Cmp_Extension(const SG_Char *File_Name, const SG_Char *Extension)
{
	wxFileName	fn(File_Name);

	return( fn.GetExt().CmpNoCase(Extension) == 0 );
}

//---------------------------------------------------------
bool			SG_File_Set_Extension(CSG_String &File_Name, const CSG_String &Extension)
{
	if( File_Name.Length() > 0 && Extension.Length() > 0 )
	{
		wxFileName	fn(File_Name.w_str());

		fn.SetExt(Extension.w_str());

		File_Name	= fn.GetFullPath().wc_str();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Extension(const SG_Char *File_Name)
{
	wxFileName	fn(File_Name);

	return( CSG_String(fn.GetExt().wc_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Read_Line(FILE *Stream, CSG_String &Line)
{
	char	c;

	if( Stream && !feof(Stream) )
	{
		Line.Clear();

		while( !feof(Stream) && (c = fgetc(Stream)) != 0x0A && c != 0x0D )
		{
			Line.Append(c);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Get_Environment(const CSG_String &Variable, CSG_String *Value)
{
	if( Value == NULL)
	{
		return( wxGetEnv(Variable.w_str(), NULL) );
	}

	wxString	s;

	if( wxGetEnv(Variable.w_str(), &s) )
	{
		*Value	= s.wc_str();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool			SG_Set_Environment(const CSG_String &Variable, const CSG_String &Value)
{
	return( wxSetEnv(Variable.w_str(), Value.w_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
