
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
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/wxcrtvararg.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/zipstrm.h>
#include <wx/tarstrm.h>
#include <wx/zstream.h>
#include <wx/log.h>
#include <wx/version.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define m_pStream_Base ((wxStreamBase        *)m_pStream)
#define m_pStream_I    ((wxFFileInputStream  *)m_pStream)
#define m_pStream_O    ((wxFFileOutputStream *)m_pStream)
#define m_pStream_IO   ((wxFFileStream       *)m_pStream)


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_File::CSG_File(void)
{
	On_Construction();
}

//---------------------------------------------------------
CSG_File::CSG_File(const SG_Char *FileName, int Mode, bool bBinary, int Encoding)
{
	On_Construction();

	Open(FileName, Mode, bBinary, Encoding);
}

//---------------------------------------------------------
CSG_File::~CSG_File(void)
{
	Close();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_File::Open(const SG_Char *_FileName, int Mode, bool bBinary, int Encoding)
{
	Close();

	if( !_FileName ) { return( false ); } CSG_String FileName(_FileName);

	CSG_String Path(SG_File_Get_Path(FileName));

	if( !Path.is_Empty() && !SG_Dir_Exists(Path) )
	{
		return( false );
	}

	if( Mode == SG_FILE_R && !SG_File_Exists(FileName) )
	{
		return( false );
	}

	m_FileName = FileName; m_Mode = Mode;

	Set_Encoding(Encoding);

	switch( m_Mode )
	{
	case SG_FILE_W:
		m_pStream = new wxFFileOutputStream(FileName.c_str(), bBinary ? "wb" : "w");
		break;

	case SG_FILE_R:
		m_pStream = new wxFFileInputStream (FileName.c_str(), bBinary ? "rb" : "r");
		break;

	default: // SG_FILE_RW
		m_pStream = new wxFFileStream      (FileName.c_str(), SG_File_Exists(FileName)
			? (bBinary ? "r+b" : "r+")
			: (bBinary ? "w+b" : "w+")
		);
		break;
	}

	if( !m_pStream || !m_pStream_Base->IsOk() )
	{
		Close();

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_File::Close(void)
{
	if( m_pStream )
	{
		delete(m_pStream_Base);

		m_pStream = NULL;
	}

	Set_Encoding(SG_FILE_ENCODING_UNDEFINED);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_File::On_Construction(void)
{
	m_pStream	= NULL;
	m_pConvert	= NULL;
	m_Encoding	= SG_FILE_ENCODING_UNDEFINED;
}

//---------------------------------------------------------
bool CSG_File::Set_Encoding(int Encoding)
{
	if( m_pConvert )
	{
		if( m_pConvert != &wxConvLocal
		&&  m_pConvert != &wxConvLibc
		&&  m_pConvert != &wxConvUTF7
		&&  m_pConvert != &wxConvUTF8 )
		{
			delete((wxMBConv *)m_pConvert);
		}

		m_pConvert	= NULL;
	}

	m_Encoding	= Encoding;

	switch( Encoding )
	{
	case SG_FILE_ENCODING_ANSI   : break;
	case SG_FILE_ENCODING_UTF7   : m_pConvert = &wxConvUTF7          ; break;
	case SG_FILE_ENCODING_UTF8   : m_pConvert = &wxConvUTF8          ; break;
	case SG_FILE_ENCODING_UTF16LE: m_pConvert = new wxMBConvUTF16LE(); break;
	case SG_FILE_ENCODING_UTF16BE: m_pConvert = new wxMBConvUTF16BE(); break;
	case SG_FILE_ENCODING_UTF32LE: m_pConvert = new wxMBConvUTF32LE(); break;
	case SG_FILE_ENCODING_UTF32BE: m_pConvert = new wxMBConvUTF32BE(); break;
	default                      : break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
sLong CSG_File::Length(void) const
{
	return( m_pStream ? m_pStream_Base->GetLength() : -1 );
}

//---------------------------------------------------------
bool CSG_File::is_EOF(void)	const
{
	return( is_Reading() && (m_Mode == SG_FILE_R ? m_pStream_I->Eof() : m_pStream_IO->Eof()) );
}

//---------------------------------------------------------
bool CSG_File::Seek(sLong Offset, int Origin) const
{
	if( m_pStream )
	{
		wxSeekMode Seek = Origin == SG_FILE_CURRENT ? wxFromCurrent : Origin == SG_FILE_END ? wxFromEnd : wxFromStart;

		switch( m_Mode )
		{
		case SG_FILE_R : return( m_pStream_I ->SeekI(Offset, Seek) != wxInvalidOffset );
		case SG_FILE_W : return( m_pStream_O ->SeekO(Offset, Seek) != wxInvalidOffset );
		default        : return( m_pStream_IO->SeekI(Offset, Seek) != wxInvalidOffset
		                      && m_pStream_IO->SeekO(Offset, Seek) != wxInvalidOffset );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_File::Seek_Start(void) const	{	return( Seek(0, SEEK_SET) );	}
bool CSG_File::Seek_End  (void) const	{	return( Seek(0, SEEK_END) );	}

//---------------------------------------------------------
sLong CSG_File::Tell(void) const
{
	if( m_pStream )
	{
		switch( m_Mode )
		{
		case SG_FILE_R : return( m_pStream_I ->TellI() );
		case SG_FILE_W : return( m_pStream_O ->TellO() );
		default        : return( m_pStream_IO->TellI() );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
bool CSG_File::Flush(void)
{
	return( m_pStream_O && m_pStream_O->GetFile()->Flush() );
}

//---------------------------------------------------------
int CSG_File::Printf(const char *Format, ...)
{
	if( !is_Writing() )
	{
		return( 0 );
	}

	wxString	String;

#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int	Result	= String.PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	int	Result	= String.PrintfV(Format, argptr);
#endif
	va_end(argptr);

	Write(&String);

	return( Result );
}

//---------------------------------------------------------
int CSG_File::Printf(const wchar_t *Format, ...)
{
	if( !is_Writing() )
	{
		return( 0 );
	}

	wxString	String;

#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int	Result	= String.PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	int	Result	= String.PrintfV(Format, argptr);
#endif

	va_end(argptr);

	Write(&String);

	return( Result );
}

//---------------------------------------------------------
size_t CSG_File::Read(void *Buffer, size_t Size, size_t Count) const
{
	return( !is_Reading() || Size == 0 || Count == 0 ? 0 : m_Mode == SG_FILE_R
		? m_pStream_I ->Read(Buffer, Size * Count).LastRead() / Size
		: m_pStream_IO->Read(Buffer, Size * Count).LastRead() / Size
	);
}

size_t CSG_File::Read(CSG_String &Buffer, size_t Size) const
{
	if( is_Reading() && Size > 0 )
	{
		CSG_Buffer	s(Size + 1);

		size_t	 i	= Read(s.Get_Data(), sizeof(char), Size);

		if( i > 0 )
		{
			s[Size] = '\0';

			Buffer	= s.Get_Data();

			return( i );
		}
	}

	Buffer.Clear();

	return( 0 );
}

//---------------------------------------------------------
size_t CSG_File::Write(void *Buffer, size_t Size, size_t Count) const
{
	return( !is_Writing() || Size == 0 || Count == 0 ? 0 : m_Mode == SG_FILE_W
		? m_pStream_O ->Write(Buffer, Size * Count).LastWrite()
		: m_pStream_IO->Write(Buffer, Size * Count).LastWrite()
	);
}

size_t CSG_File::Write(const CSG_String &Buffer) const
{
	if( m_pConvert )
	{
		wxString _Buffer(Buffer.w_str());

		const wxScopedCharBuffer s(_Buffer.mb_str(*((wxMBConv *)m_pConvert)));

		return( Write((void *)s.data(), sizeof(char), s.length()) );
	}

	CSG_String s(Buffer);

	return( s.Length() > 0 ? Write((void *)s.b_str(), sizeof(char), s.Length()) : 0 );

//	CSG_Buffer s(Buffer.to_ASCII()); // returns NULL terminated char sequence, Get_Size() count includes terminating NULL!!!

//	return( s.Get_Size() > 1 ? Write((void *)s.Get_Data(), sizeof(char), s.Get_Size() - 1) : 0 );
}

//---------------------------------------------------------
bool CSG_File::Read_Line(CSG_String &sLine)	const
{
	if( !is_Reading() || is_EOF() )
	{
		return( false );
	}

	wxString s;

	if( m_pConvert )
	{
		if( m_Mode == SG_FILE_R )
		{
			wxTextInputStream Stream(*m_pStream_I , " \t", *((wxMBConv *)m_pConvert)); s = Stream.ReadLine();
		}
		else
		{
			wxTextInputStream Stream(*m_pStream_IO, " \t", *((wxMBConv *)m_pConvert)); s = Stream.ReadLine();
		}
	}
	else
	{
		if( m_Mode == SG_FILE_R )
		{
			wxTextInputStream Stream(*m_pStream_I , " \t"                           ); s = Stream.ReadLine();
		}
		else
		{
			wxTextInputStream Stream(*m_pStream_IO, " \t"                           ); s = Stream.ReadLine();
		}
	}

	sLine = CSG_String(&s);

	return( !sLine.is_Empty() || !is_EOF() );
}

//---------------------------------------------------------
int CSG_File::Read_Char(void) const
{
	return( !is_Reading() ? 0 : m_Mode == SG_FILE_R ? m_pStream_I->GetC() : m_pStream_IO->GetC() );
}

//---------------------------------------------------------
int CSG_File::Read_Int(bool bByteOrderBig) const
{
	int Value = 0;

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
	double Value = 0;

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
	if( is_Reading() )
	{
		int c; while( !is_EOF() && isspace(c = Read_Char()) ); // remove leading white space

		if( isdigit(c) || strchr("-+", c) )
		{
			CSG_String s = (char)c;

			while( !is_EOF() && isdigit(c = Read_Char()) )
			{
				s += (char)c;
			}

			return( s.asInt(Value) );
		}
	}

	return( false );
}

bool CSG_File::Scan(double &Value) const
{
	if( is_Reading() )
	{
		int c; while( !is_EOF() && isspace(c = Read_Char()) ); // remove leading white space

		if( isdigit(c) || strchr("-+.,eE", c) )
		{
			CSG_String s = (char)c;

			while( !is_EOF() && (isdigit(c = Read_Char()) || strchr(".,eE", c) || strchr("", c)) )
			{
				s += (char)c;
			}

			return( s.asDouble(Value) );
		}
	}

	return( false );
}

bool CSG_File::Scan(CSG_String &Value, SG_Char Separator) const
{
	if( is_Reading() && !is_EOF() )
	{
		Value.Clear();

		int c; while( !is_EOF() && (c = Read_Char()) != Separator && c != EOF )
		{
			Value += (char)c;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_File::Scan_Int(void) const
{
	int    Value; return( Scan(Value) ? Value : 0   );
}

double CSG_File::Scan_Double(void) const
{
	double Value; return( Scan(Value) ? Value : 0.0 );
}

CSG_String CSG_File::Scan_String(SG_Char Separator) const
{
	CSG_String Value; Scan(Value, Separator); return( Value );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Archive::CSG_Archive(void)
{
	On_Construction();
}

//---------------------------------------------------------
CSG_Archive::CSG_Archive(const SG_Char *FileName, int Mode, int Encoding)
{
	On_Construction();

	Open(FileName, Mode, Encoding);
}

//---------------------------------------------------------
CSG_Archive::~CSG_Archive(void)
{
	Close();
}

//---------------------------------------------------------
bool CSG_Archive::Open(const SG_Char *_FileName, int Mode, int Encoding)
{
	Close();

	if( !_FileName ) { return( false ); } CSG_String FileName(_FileName);

	if( SG_File_Cmp_Extension(FileName, "tar") )
	{
		m_Type = SG_FILE_TYPE_TAR;
	}
	else // if( SG_File_Cmp_Extension(FileName, "zip") )
	{
		m_Type = SG_FILE_TYPE_ZIP;
	}

	m_Archive = FileName;

	CSG_String Path(SG_File_Get_Path(m_Archive));

	if( !Path.is_Empty() && !SG_Dir_Exists(Path) )
	{
		return( false );
	}

	if( Mode == SG_FILE_R && !SG_File_Exists(m_Archive) )
	{
		return( false );
	}

	wxLogNull logNo; // suppress user notification dialog for invalid zip files

	m_Mode = Mode; Set_Encoding(Encoding);

	if( Mode == SG_FILE_W )
	{
		if( is_Zip() )
		{
			m_pStream = new wxZipOutputStream(new wxFileOutputStream(m_Archive.c_str()));
		}
		else
		{
			m_pStream = new wxTarOutputStream(new wxFileOutputStream(m_Archive.c_str()));
		}
	}
	else if( Mode == SG_FILE_R && SG_File_Exists(m_Archive) )
	{
		if( is_Zip() )
		{
			m_pStream = new wxZipInputStream (new wxFileInputStream (m_Archive.c_str()));
		}
		else
		{
			m_pStream = new wxTarInputStream (new wxFileInputStream (m_Archive.c_str()));
		}
	}

	if( !m_pStream || !m_pStream_Base->IsOk() )
	{
		Close();

		return( false );
	}

	if( is_Reading() )
	{
		wxArchiveEntry *pEntry;

		while( (pEntry = ((wxArchiveInputStream *)m_pStream)->GetNextEntry()) != NULL )
		{
			m_Files += pEntry;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Archive::Close(void)
{
	for(sLong i=0; i<m_Files.Get_Size(); i++)
	{
		if( is_Zip() )
		{
			delete((wxZipEntry *)m_Files[i]);
		}
		else
		{
			delete((wxTarEntry *)m_Files[i]);
		}
	}

	m_Files.Set_Array(0);

	m_Archive.Clear();

	return( CSG_File::Close() );
}

//---------------------------------------------------------
bool CSG_Archive::Add_Directory(const SG_Char *Name)
{
	return( is_Writing() && Name && ((wxArchiveOutputStream *)m_pStream)->PutNextDirEntry(Name) );
}

//---------------------------------------------------------
bool CSG_Archive::Add_File(const SG_Char *Name, bool bBinary)
{
	if( is_Writing() && Name )
	{
		wxArchiveEntry *pEntry = NULL;

		if( is_Zip() )
		{
			pEntry = new wxZipEntry(Name);

			((wxZipEntry *)pEntry)->SetIsText(bBinary == false);

			#if wxCHECK_VERSION(3, 1, 1)
			((wxZipOutputStream *)m_pStream)->SetFormat(wxZIP_FORMAT_ZIP64);
			#endif
		}
		else
		{
			pEntry = new wxTarEntry(Name);
		}

		if( ((wxArchiveOutputStream *)m_pStream)->PutNextEntry(pEntry) )
		{
			m_FileName = Name;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Archive::is_Directory(size_t Index)
{
	if( is_Reading() && m_Files[Index] )
	{
		return( ((wxArchiveEntry *)m_Files[Index])->IsDir() );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Archive::Get_File(size_t Index)
{
	if( is_Reading() && m_Files[Index] )
	{
		if( ((wxArchiveInputStream *)m_pStream)->OpenEntry(*(wxArchiveEntry *)m_Files[Index]) )
		{
			m_FileName = Get_File_Name(Index);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Archive::Get_File(const SG_Char *Name)
{
	if( is_Reading() && Name )
	{
		for(sLong i=0; i<m_Files.Get_Size(); i++)
		{
			if( !((wxArchiveEntry *)m_Files[i])->GetName().Cmp(Name) )
			{
				return( Get_File(i) );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CSG_Archive::Get_File_Name(size_t Index)
{
	CSG_String s;

	if( is_Reading() && m_Files[Index] )
	{
		wxString Name(((wxArchiveEntry *)m_Files[Index])->GetName()); s = &Name;
	}

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Archive::Extract_All(const SG_Char *_Directory)
{
	if( !is_Reading() )
	{
		return( false );
	}

	#ifdef _SAGA_MSW
		const char Separator = '\\';
	#else
		const char Separator = '/';
	#endif

	CSG_String Directory(_Directory ? _Directory : SG_T(""));

	if( Directory.is_Empty() )
	{
		Directory = SG_File_Get_Path(m_Archive);
	}
	else if( !SG_Dir_Exists(Directory) )
	{
		SG_Dir_Create(Directory, true);
	}

	Directory += Separator;

	//-----------------------------------------------------
	for(size_t i=0; i<Get_File_Count() && SG_UI_Process_Set_Progress((int)i, (int)Get_File_Count()); i++)
	{
		if( is_Directory(i) )
		{
			SG_Dir_Create(Directory + Get_File_Name(i));
		}
		else
		{
			Extract(Get_File_Name(i), Directory + Get_File_Name(i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Archive::Extract(const SG_Char *File, const SG_Char *_toFile)
{
	if( !is_Reading() || !Get_File(File) )
	{
		return( false );
	}

	CSG_String toFile(_toFile ? _toFile : SG_T(""));

	if( toFile.is_Empty() )
	{
		toFile = SG_File_Make_Path(SG_File_Get_Path(m_Archive), File);
	}

	CSG_File Stream(toFile, SG_FILE_W, true, m_Encoding);

	if( !Stream.is_Open() )
	{
		return( false );
	}

	//-----------------------------------------------------
	#define UNZIP_BUFFER 4096

	char *Buffer[UNZIP_BUFFER];

	while( !is_EOF() )
	{
		size_t nBytes = Read(Buffer, sizeof(char), UNZIP_BUFFER);

		Stream.Write(Buffer, 1, nBytes); Stream.Flush();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ZLib::CSG_ZLib(void)
{
	// nop
}

//---------------------------------------------------------
bool CSG_ZLib::is_GZip_Supported(void)
{
	return( wxZlibInputStream::CanHandleGZip() );
}

//---------------------------------------------------------
CSG_String CSG_ZLib::Compress(const CSG_String &File, const CSG_String &_Target)
{
	if( SG_File_Exists(File) )
	{
		wxFFileInputStream Input(File.c_str());

		if( Input.IsOk() && Input.CanRead() )
		{
			CSG_String Target(_Target);

			if( Target.is_Empty() )
			{
				Target = File + ".gz";
			}

			wxZlibOutputStream Output(new wxFFileOutputStream(Target.c_str()));

			if( Output.IsOk() )
			{
				Output.Write(Input);

				return( Target );
			}
		}
	}

	return( "" );
}

//---------------------------------------------------------
CSG_String CSG_ZLib::Uncompress(const CSG_String &File, const CSG_String &_Target)
{
	if( SG_File_Exists(File) )
	{
		wxZlibInputStream Input(new wxFFileInputStream(File.c_str()));

		if( Input.IsOk() && Input.CanRead() )
		{
			CSG_String Target(_Target);

			if( Target.is_Empty() )
			{
				Target = SG_File_Make_Path(SG_File_Get_Path(File), SG_File_Get_Name(File, false), "");
			}

			wxFFileOutputStream Output(Target.c_str());

			if( Output.IsOk() )
			{
				Output.Write(Input);

				return( Target );
			}
		}
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Dir_Exists(const CSG_String &Directory)
{
	return( wxFileName::DirExists(Directory.c_str()) );
}

//---------------------------------------------------------
bool			SG_Dir_Create(const CSG_String &Directory, bool bFullPath)
{
	if( SG_Dir_Exists(Directory) )
	{
		return( true );
	}

	return( wxFileName::Mkdir(Directory.c_str(), wxS_DIR_DEFAULT, bFullPath ? wxPATH_MKDIR_FULL : 0) );
}

//---------------------------------------------------------
bool			SG_Dir_Delete(const CSG_String &Directory, bool bRecursive)
{
	if( !SG_Dir_Exists(Directory) )
	{
		return( true );
	}

	return( wxDir::Remove(Directory.c_str(), bRecursive ? wxPATH_RMDIR_RECURSIVE : 0) );
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

//---------------------------------------------------------
bool			SG_Dir_List_Subdirectories	(CSG_Strings &List, const CSG_String &Directory, bool bRecursive)
{
	List.Clear(); wxDir Dir;

	if( Dir.Open(Directory.c_str()) )
	{
		wxString FileName;

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_DIRS) )
		{
			do
			{
				List += SG_File_Make_Path(Directory, &FileName);

				if( bRecursive )
				{
					CSG_Strings _List; SG_Dir_List_Subdirectories(_List, SG_File_Make_Path(Directory, &FileName), bRecursive); List += _List;
				}
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( List.Get_Count() > 0 );
}

//---------------------------------------------------------
bool			SG_Dir_List_Files			(CSG_Strings &List, const CSG_String &Directory, const CSG_String &Extension, bool bRecursive)
{
	List.Clear(); wxDir Dir;

	if( Dir.Open(Directory.c_str()) )
	{
		wxString FileName;

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_FILES) )
		{
			do
			{
				if( Extension.is_Empty() || SG_File_Cmp_Extension(&FileName, Extension) )
				{
					List += SG_File_Make_Path(Directory, &FileName);
				}
			}
			while( Dir.GetNext(&FileName) );
		}

		if( bRecursive && Dir.GetFirst(&FileName, wxEmptyString, wxDIR_DIRS) )
		{
			do
			{
				CSG_Strings _List; SG_Dir_List_Files(_List, SG_File_Make_Path(Directory, &FileName), Extension, bRecursive); List += _List;
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( List.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_File_Exists(const CSG_String &FileName)
{
	return( wxFileExists(FileName.c_str()) );
}

//---------------------------------------------------------
bool			SG_File_Delete(const CSG_String &FileName)
{
	return( SG_File_Exists(FileName) && wxRemoveFile(FileName.c_str()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name_Temp(const CSG_String &Prefix)
{
	return( SG_File_Get_Name_Temp(Prefix, "") );
}

CSG_String		SG_File_Get_Name_Temp(const CSG_String &Prefix, const CSG_String &Directory)
{
	if( !SG_Dir_Exists(Directory) )
	{
		return( CSG_String(wxFileName::CreateTempFileName(Prefix.c_str()).wc_str()) );
	}

	return( CSG_String(wxFileName::CreateTempFileName(SG_File_Make_Path(Directory, Prefix).w_str()).wc_str()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name(const CSG_String &full_Path, bool bExtension)
{
	wxFileName	fn(full_Path.c_str());

	if( bExtension )
	{
		wxString s(fn.GetFullName()); return( CSG_String(&s) );
	}

	wxString s(fn.GetName()); return( &s );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path(const CSG_String &full_Path)
{
	wxString	s(wxFileName(full_Path.c_str()).GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));

	return( CSG_String(&s) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path_Absolute(const CSG_String &full_Path)
{
	wxFileName	fn(full_Path.c_str());

	fn.MakeAbsolute();

	wxString s(fn.GetFullPath()); return( &s );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path_Relative(const CSG_String &Directory, const CSG_String &full_Path)
{
	wxFileName	fn(full_Path.c_str());

	fn.MakeRelativeTo(Directory.c_str());

	wxString s(fn.GetFullPath()); return( &s );
}

//---------------------------------------------------------
CSG_String		SG_File_Make_Path(const CSG_String &Directory, const CSG_String &Name)
{
	return( SG_File_Make_Path(Directory, Name, "") );
}

CSG_String		SG_File_Make_Path(const CSG_String &Directory, const CSG_String &Name, const CSG_String &Extension)
{
	wxFileName	fn;

	fn.AssignDir(!Directory.is_Empty() ? Directory.c_str() : SG_File_Get_Path(Name).c_str());

	if( !Extension.is_Empty() )
	{
		fn.SetName		(SG_File_Get_Name(Name, false).c_str());
		fn.SetExt		(Extension.c_str());
	}
	else
	{
		fn.SetFullName	(SG_File_Get_Name(Name,  true).c_str());
	}

	wxString s(fn.GetFullPath()); return( &s );
}

//---------------------------------------------------------
bool			SG_File_Cmp_Path(const CSG_String &Path1, const CSG_String &Path2)
{
	wxFileName a(wxString(Path1.c_str())), b(wxString(Path2.c_str()));

	return( a.SameAs(b) );
}

//---------------------------------------------------------
bool			SG_File_Cmp_Extension(const CSG_String &FileName, const CSG_String &Extension)
{
	return( SG_File_Get_Extension(FileName).CmpNoCase(Extension) == 0 );
}

//---------------------------------------------------------
bool			SG_File_Set_Extension(CSG_String &FileName, const CSG_String &Extension)
{
	if( FileName.Length() > 0 )
	{
		wxFileName	fn(FileName.c_str());

		fn.SetExt(Extension.c_str());

		wxString s(fn.GetFullPath());
		
		FileName	= &s;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Extension(const CSG_String &FileName)
{
	wxFileName	fn(FileName.c_str());

	wxString s(fn.GetExt()); return( &s );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Get_Environment(const CSG_String &Variable, CSG_String *Value)
{
	if( Value == NULL )
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
