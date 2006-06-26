
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
bool			API_Directory_isValid(const char *Directory)
{
	return( Directory != NULL && *Directory != '\0' && wxFileName::DirExists(Directory) );
}

//---------------------------------------------------------
bool			API_Directory_Make(const char *Directory)
{
	if( API_Directory_isValid(Directory) )
	{
		return( true );
	}

	return( wxFileName::Mkdir(Directory) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			API_Remove_File(const char *FileName)
{
	if( FileName )
	{
		wxRemoveFile(FileName);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CAPI_String		API_Get_CWD(void)
{
	return( wxGetCwd().c_str() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_String		API_Get_Temp_File_Name(const char *Prefix, const char *Directory)
{
	if( !API_Directory_isValid(Directory) )
	{
		return( wxFileName::CreateTempFileName(Prefix).c_str() );
	}

	return( wxFileName::CreateTempFileName(API_Make_File_Path(Directory, Prefix).c_str()).c_str() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_String		API_Extract_File_Name(const char *full_Path, bool bExtension)
{
	wxFileName	fn(full_Path);
	CAPI_String	s;

	s.Printf(bExtension ? fn.GetFullName().c_str() : fn.GetName().c_str());

	return( s );
}

//---------------------------------------------------------
CAPI_String		API_Extract_File_Path(const char *full_Path)
{
	wxFileName	fn(full_Path);

	return( fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR).c_str() );
}

//---------------------------------------------------------
CAPI_String		API_Make_File_Path(const char *Directory, const char *Name, const char *Extension)
{
	wxFileName	fn;

	fn.AssignDir(API_Directory_isValid(Directory) ? Directory : API_Extract_File_Path(Name).c_str());

	if( Extension && *Extension != '\0' )
	{
		fn.SetName		(API_Extract_File_Name(Name, false).c_str());
		fn.SetExt		(Extension);
	}
	else
	{
		fn.SetFullName	(API_Extract_File_Name(Name,  true).c_str());
	}

	return( fn.GetFullPath().c_str() );
}

//---------------------------------------------------------
bool			API_Cmp_File_Extension(const char *File_Name, const char *Extension)
{
	wxFileName	fn(File_Name);

	return( fn.GetExt().CmpNoCase(Extension) == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			API_Read_Line(FILE *Stream, CAPI_String &Line)
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
int				API_Read_Int(FILE *Stream, bool bBig)
{
	int		Value;

	fread(&Value, 1, sizeof(Value), Stream);

	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			API_Write_Int(FILE *Stream, int Value, bool bBig)
{
	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	fwrite(&Value, 1, sizeof(Value), Stream);
}

//---------------------------------------------------------
double			API_Read_Double(FILE *Stream, bool bBig)
{
	double	Value;

	fread(&Value, 1, sizeof(Value), Stream);

	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			API_Write_Double(FILE *Stream, double Value, bool bBig)
{
	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	fwrite(&Value, 1, sizeof(Value), Stream);
}

//---------------------------------------------------------
int				API_Read_Int(char *Buffer, bool bBig)
{
	int		Value	= *(int *)Buffer;

	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			API_Write_Int(char *Buffer, int Value, bool bBig)
{
	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	*((int *)Buffer)	= Value;
}

//---------------------------------------------------------
double			API_Read_Double(char *Buffer, bool bBig)
{
	double	Value	= *(double *)Buffer;

	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			API_Write_Double(char *Buffer, double Value, bool bBig)
{
	if( bBig )
	{
		API_Swap_Bytes(&Value, sizeof(Value));
	}

	*(double *)Buffer	= Value;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
