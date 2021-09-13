
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     tl_merge.cpp                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "tl_merge.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTL_Merge::CTL_Merge(void)
{
	//-----------------------------------------------------
	Set_Name		("Merge Translation Files");

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	("");

	//-----------------------------------------------------
	CSG_String	Filter;
	
	Filter.Printf(SG_T("%s|*.lng;*.txt|%s|*.lng|%s|*.txt|%s|*.*"),
		SG_T("All recognized Files"),
		SG_T("SAGA Translation File (*.lng)"),
		SG_T("Text Files (*.txt)"),
		SG_T("All Files")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		"", "TARGET", "Target",
		"",
		Filter, NULL, true
	);

	Parameters.Add_FilePath(
		"", "FILES"	, "Files",
		"",
		Filter, NULL, false, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTL_Merge::On_Execute(void)
{
	CSG_String	Target	= Parameters("TARGET")->asString();
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) || Files.Get_Count() <= 1 )
	{
		Error_Set("no files to merge");

		return( false );
	}

	//-----------------------------------------------------
	int			i, j;
	CSG_Table	Merge;

	if( !Merge.Create(Files[0], TABLE_FILETYPE_Text, '\t', SG_FILE_ENCODING_UTF8) )
	{
		Merge.Destroy();

		Merge.Add_Field("ORIGINAL"   , SG_DATATYPE_String);
		Merge.Add_Field("TRANSLATION", SG_DATATYPE_String);

	}

	Merge.Save(Target, TABLE_FILETYPE_Text, '\t', SG_FILE_ENCODING_UTF8);

	//-----------------------------------------------------
	for(i=1; i<Files.Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Translator	Translator(Target, false);
		CSG_Table		Add(Target, TABLE_FILETYPE_Text, '\t', SG_FILE_ENCODING_UTF8);

		if( Merge.Create(Files[i], TABLE_FILETYPE_Text, '\t', SG_FILE_ENCODING_UTF8) )
		{
			for(j=0; j<Merge.Get_Count() && Set_Progress(j, Merge.Get_Count()); j++)
			{
				if( !Translator.Get_Translation(Merge[j].asString(0), true) )
				{
					Add.Add_Record(Merge.Get_Record(j));
				}
			}

			if( Add.Get_Count() > Translator.Get_Count() )
			{
				Add.Save(Target, TABLE_FILETYPE_Text, '\t', SG_FILE_ENCODING_UTF8);
			}
		}
	}

	//-----------------------------------------------------
	for(i=Merge.Get_Count()-1; i>0 && Process_Get_Okay(); i--)
	{
		if( !SG_STR_CMP(Merge[i].asString(0), Merge[i - 1].asString(0)) )
		{
			Merge.Del_Record(i);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
