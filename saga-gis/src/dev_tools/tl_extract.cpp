/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    tl_extract.cpp                     //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute for Geography                //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tl_extract.h"

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTL_Extract::CTL_Extract(void)
{
	//-----------------------------------------------------
	Set_Name		(SG_T("Extract Translatable Text Elements from Sources"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(SG_T(""));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TARGET"		, SG_T("Translatable Elements"),
		SG_T(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "DIRECTORY"	, SG_T("Sources Directory"),
		SG_T(""),
		NULL, SG_T("H:/saga/saga_2/src"), false, true
	);

	Parameters.Add_Value(
		NULL	, "LOCATION"	, SG_T("Location"),
		SG_T(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "LONG"		, SG_T("Long Texts"),
		SG_T(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTL_Extract::On_Execute(void)
{
	bool		bLocation;
	CSG_String	Text, Directory;
	CSG_Table	Elements, *pTarget;

	//-----------------------------------------------------
	pTarget		= Parameters("TARGET")		->asTable();
	Directory	= Parameters("DIRECTORY")	->asString();
	bLocation	= Parameters("LOCATION")	->asBool();

	//-----------------------------------------------------
	Elements.Add_Field(SG_T("TEXT"), SG_DATATYPE_String);
	Elements.Add_Field(SG_T("FILE"), SG_DATATYPE_String);

	if( !Read_Directory(Directory, Elements) || Elements.Get_Count() <= 0 )
	{
		Error_Set(SG_T("no translatable text elements found"));

		return( false );
	}

	//-----------------------------------------------------
	Elements.Set_Index(0, TABLE_INDEX_Ascending);

	pTarget->Destroy();

	pTarget->Add_Field(SG_T("TEXT")       , SG_DATATYPE_String);
	pTarget->Add_Field(SG_T("TRANSLATION"), SG_DATATYPE_String);

	for(int i=0; i<Elements.Get_Count() && Set_Progress(i, Elements.Get_Count()); i++)
	{
		if( i == 0 || Text.Cmp(Elements.Get_Record_byIndex(i)->asString(0)) )
		{
			Text	= Elements.Get_Record_byIndex(i)->asString(0);

			CSG_Table_Record	*pRecord	= pTarget->Add_Record();
			
			pRecord->Set_Value(0, Text);

			if( bLocation )
			{
				pRecord->Set_Value(1, Elements.Get_Record_byIndex(i)->asString(1));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTL_Extract::Read_Directory(const CSG_String &Directory, CSG_Table &Elements)
{
	wxString	Name;
	wxFileName	File;
	wxDir		Dir;

	if( !Dir.Open(Directory.c_str()) )
	{
		return( false );
	}

	File.AssignDir(Directory.c_str());

	if(	Dir.GetFirst(&Name, wxT("*.cpp"), wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);
			Read_File(&File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, wxT("*.h")  , wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);
			Read_File(&File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, wxT("*")    , wxDIR_DIRS |wxDIR_HIDDEN) )
	{
		do
		{
			File.AssignDir(Directory.c_str());
			File.AppendDir(Name);
			Read_Directory(&File.GetPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	return( Elements.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTL_Extract::Read_File(const CSG_String &File, CSG_Table &Elements)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String	String, Text;

	if( !Stream.Read(String, Stream.Length()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	const SG_Char	Function[3][4]	= {	SG_T("LNG"), SG_T("_TL"), SG_T("_TW") };

	const SG_Char	*p		= String;

	bool			bLong	= Parameters("LONG")->asBool();

	while( *p != '\0' )
	{
		if(	!(p[0] == Function[0][0] && p[1] == Function[0][1] && p[2] == Function[0][2])
		&&	!(p[0] == Function[1][0] && p[1] == Function[1][1] && p[2] == Function[1][2])
		&&	!(p[0] == Function[2][0] && p[1] == Function[2][1] && p[2] == Function[2][2] && bLong) )
		{
			p	++;
		}
		else
		{
			p	+= Read_Text(p, Text);

			if( Text.Length() > 0 )
			{
				CSG_Table_Record	*pRecord	= Elements.Add_Record();
				
				pRecord->Set_Value(0, Text);
				pRecord->Set_Value(1, File);
			}
		}
	}

	return( Elements.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTL_Extract::Read_Text(const SG_Char *String, CSG_String &Text)
{
	int			n, Level;

	Text.Clear();

	for(n=0, Level=-1; String[n]!='\0' && Level<2; n++)
	{
		if( Level < 0 )
		{
			if( String[n] == '(' )
			{
				Level	= 0;
			}
		}
		else if( Level == 0 )
		{
			switch( String[n] )
			{
			case '\"':
				Level	= 1;
				break;

			case ')':
				Level	= 2;
				break;
			}
		}
		else switch( String[n] )
		{
		case '\"':
			Level	= 0;
			break;

		case '\\':
			Text.Append(String[n++]);
			Text.Append(String[n]);
			break;

		default:
			Text.Append(String[n]);
			break;
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
