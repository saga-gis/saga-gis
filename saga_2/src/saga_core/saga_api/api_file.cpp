
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
bool			SG_Dir_isValid(const char *Directory)
{
	return( Directory != NULL && *Directory != '\0' && wxFileName::DirExists(Directory) );
}

//---------------------------------------------------------
bool			SG_Dir_Create(const char *Directory)
{
	if( SG_Dir_isValid(Directory) )
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
bool			SG_File_Exists(const char *FileName)
{
	return( FileName && *FileName != '\0' && wxFileExists(FileName) );
}

//---------------------------------------------------------
bool			SG_File_Delete(const char *FileName)
{
	return( FileName && *FileName != '\0' && wxRemoveFile(FileName) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_TmpName(const char *Prefix, const char *Directory)
{
	if( !SG_Dir_isValid(Directory) )
	{
		return( wxFileName::CreateTempFileName(Prefix).c_str() );
	}

	return( wxFileName::CreateTempFileName(SG_File_Make_Path(Directory, Prefix).c_str()).c_str() );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name(const char *full_Path, bool bExtension)
{
	wxFileName	fn(full_Path);
	CSG_String	s;

	s.Printf(bExtension ? fn.GetFullName().c_str() : fn.GetName().c_str());

	return( s );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path(const char *full_Path)
{
	wxFileName	fn(full_Path);

	return( fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR).c_str() );
}

//---------------------------------------------------------
CSG_String		SG_File_Make_Path(const char *Directory, const char *Name, const char *Extension)
{
	wxFileName	fn;

	fn.AssignDir(SG_Dir_isValid(Directory) ? Directory : SG_File_Get_Path(Name).c_str());

	if( Extension && *Extension != '\0' )
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
bool			SG_File_Cmp_Extension(const char *File_Name, const char *Extension)
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
int				SG_Read_Int(FILE *Stream, bool bBig)
{
	int		Value;

	fread(&Value, 1, sizeof(Value), Stream);

	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			SG_Write_Int(FILE *Stream, int Value, bool bBig)
{
	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	fwrite(&Value, 1, sizeof(Value), Stream);
}

//---------------------------------------------------------
double			SG_Read_Double(FILE *Stream, bool bBig)
{
	double	Value;

	fread(&Value, 1, sizeof(Value), Stream);

	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			SG_Write_Double(FILE *Stream, double Value, bool bBig)
{
	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	fwrite(&Value, 1, sizeof(Value), Stream);
}

//---------------------------------------------------------
int				SG_Read_Int(char *Buffer, bool bBig)
{
	int		Value	= *(int *)Buffer;

	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			SG_Write_Int(char *Buffer, int Value, bool bBig)
{
	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	*((int *)Buffer)	= Value;
}

//---------------------------------------------------------
double			SG_Read_Double(char *Buffer, bool bBig)
{
	double	Value	= *(double *)Buffer;

	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			SG_Write_Double(char *Buffer, double Value, bool bBig)
{
	if( bBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	*(double *)Buffer	= Value;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
