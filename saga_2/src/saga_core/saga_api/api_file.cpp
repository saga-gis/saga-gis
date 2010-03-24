
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
#include <wx/utils.h>
#include <wx/filename.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_File::CSG_File(void)
{
	m_pStream	= NULL;
	m_bUnicode	= false;
}

//---------------------------------------------------------
CSG_File::CSG_File(const CSG_String &FileName, int Mode, bool bBinary, bool bUnicode)
{
	m_pStream	= NULL;

	Open(FileName, Mode, bBinary, bUnicode);
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
bool CSG_File::Open(const CSG_String &File_Name, int Mode, bool bBinary, bool bUnicode)
{
	Close();

	m_bUnicode	= bUnicode;

	const SG_Char *sMode;

	switch( Mode )
	{
	case SG_FILE_R:		sMode	= bBinary ? SG_T("rb" ) : SG_T("r" );	break;
	case SG_FILE_W:		sMode	= bBinary ? SG_T("wb" ) : SG_T("w" );	break;
	case SG_FILE_RW:	sMode	= bBinary ? SG_T("wb+") : SG_T("w+");	break;
	case SG_FILE_WA:	sMode	= bBinary ? SG_T("ab" ) : SG_T("a" );	break;
	case SG_FILE_RWA:	sMode	= bBinary ? SG_T("rb+") : SG_T("r+");	break;
	}

#if defined(_SAGA_LINUX) && defined(_SAGA_UNICODE)
	return( File_Name.Length() > 0 && (m_pStream = SG_FILE_OPEN( SG_STR_SGTOMB( File_Name ), SG_STR_SGTOMB( sMode ) )) != NULL );
#else
	return( File_Name.Length() > 0 && (m_pStream = SG_FILE_OPEN(File_Name, sMode)) != NULL );
#endif
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
bool CSG_File::Set_UnicodeFlag(bool bOn)
{
	if( m_bUnicode != bOn )
	{
		m_bUnicode	= bOn;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_File::Length(void)	const
{
	if( m_pStream )
	{
		long	pos, len;

		pos	= ftell(m_pStream);
		fseek(m_pStream, 0, SEEK_END);
		len	= ftell(m_pStream);
		fseek(m_pStream, pos, SEEK_SET);

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
bool CSG_File::Seek(int Offset, int Origin) const
{
	switch( Origin )
	{
	default:
	case SG_FILE_START:		Origin	= SEEK_SET;	break;
	case SG_FILE_CURRENT:	Origin	= SEEK_CUR;	break;
	case SG_FILE_END:		Origin	= SEEK_END;	break;
	}

	return( m_pStream ? !fseek(m_pStream, Offset, Origin) : false );
}

//---------------------------------------------------------
bool CSG_File::Seek_Start(void) const
{
	return( m_pStream && fseek(m_pStream, 0, SEEK_SET) == 0 );
}

//---------------------------------------------------------
bool CSG_File::Seek_End(void) const
{
	return( m_pStream && fseek(m_pStream, 0, SEEK_END) == 0 );
}

//---------------------------------------------------------
int CSG_File::Tell(void) const
{
	return( m_pStream ? ftell(m_pStream) : -1 );
}

//---------------------------------------------------------
bool CSG_File::Flush(void) const
{
	return( m_pStream ? !fflush(m_pStream) : false );
}

//---------------------------------------------------------
int CSG_File::Printf(const SG_Char *Format, ...)
{
	int		result	= 0;

	if( m_pStream )
	{
		va_list	argptr;

		va_start(argptr, Format);

		result	= wxVfprintf(m_pStream, Format, argptr);

		va_end(argptr);
	}

	return( result );
}

//---------------------------------------------------------
int CSG_File::Scanf(const SG_Char *Format, ...) const
{
	int		result	= 0;

	if( m_pStream )
	{
		va_list argptr;

		va_start(argptr, Format);

		result	= 0;	// wxVfscanf(m_pStream, Format, argptr);

		va_end(argptr);
	}

	return( result );
}

//---------------------------------------------------------
int CSG_File::Get_Character(void) const
{
	if( m_pStream )
	{
		return( getc(m_pStream) );
	}

	return( 0 );
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
		int		i	= fread(b, sizeof(char), Size, m_pStream);
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

size_t CSG_File::Write(CSG_String &Buffer) const
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

		while( !feof(m_pStream) && (c = fgetc(m_pStream)) != 0x0A && c != 0x0D && c != EOF )
		{
			sLine.Append(SG_STR_MBTOSG(c));
		}

		return( true );
	}

	return( false );
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
int CSG_File::Scan_Int(void)	const
{
	if( m_pStream )
	{
		int		Value;

		if( fscanf(m_pStream, "%d", &Value) == 1 )
		{
			return( Value );
		}
	}

	return( 0 );
}

double CSG_File::Scan_Double(void)	const
{
	if( m_pStream )
	{
		double	Value;

		if( fscanf(m_pStream, "%f", &Value) == 1 )
		{
			return( Value );
		}
	}

	return( 0.0 );
}

CSG_String CSG_File::Scan_String(SG_Char Separator)	const
{
	int			c;
	CSG_String	s;

	if( m_pStream && !feof(m_pStream) )
	{
		while( !feof(m_pStream) && (c = fgetc(m_pStream)) != Separator && c != EOF )
		{
			s	+= c;
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
	return( wxGetCwd().c_str() );
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
	return( FileName && *FileName && wxRemoveFile(FileName) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_TmpName(const SG_Char *Prefix, const SG_Char *Directory)
{
	if( !SG_Dir_Exists(Directory) )
	{
		return( wxFileName::CreateTempFileName(Prefix).c_str() );
	}

	return( wxFileName::CreateTempFileName(SG_File_Make_Path(Directory, Prefix).c_str()).c_str() );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name(const SG_Char *full_Path, bool bExtension)
{
	wxFileName	fn(full_Path);
	CSG_String	s;

	s	= bExtension ? fn.GetFullName().c_str() : fn.GetName().c_str();

	return( s );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path(const SG_Char *full_Path)
{
	wxFileName	fn(full_Path);

	return( fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR).c_str() );
}

//---------------------------------------------------------
CSG_String		SG_File_Make_Path(const SG_Char *Directory, const SG_Char *Name, const SG_Char *Extension)
{
	wxFileName	fn;

	fn.AssignDir(SG_Dir_Exists(Directory) ? Directory : SG_File_Get_Path(Name).c_str());

	if( Extension && *Extension )
	{
		fn.SetName		(SG_File_Get_Name(Name, false).c_str());
		fn.SetExt		(Extension);
	}
	else
	{
		fn.SetFullName	(SG_File_Get_Name(Name,  true).c_str());
	}

	return( fn.GetFullPath().c_str() );
}

//---------------------------------------------------------
bool			SG_File_Cmp_Extension(const SG_Char *File_Name, const SG_Char *Extension)
{
	wxFileName	fn(File_Name);

	return( fn.GetExt().CmpNoCase(Extension) == 0 );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Extension(const SG_Char *File_Name)
{
	wxFileName	fn(File_Name);

	return( fn.GetExt().c_str() );
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
