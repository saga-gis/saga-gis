
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
//                  svg_converter.cpp                    //
//                                                       //
//                 Copyright (C) 2022 by                 //
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
#include "svg_converter.h"

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSVG_Converter::CSVG_Converter(void)
{
	Set_Name		("SVG Converter");

	Set_Author		("O. Conrad (c) 2022");

	Set_Description	(
		""
	);

	//-----------------------------------------------------
	CSG_String Headers;

	#ifdef _SAGA_MSW
	if( SG_Get_Environment("SAGA", &Headers) )
	{
		Headers += "\\src\\saga_core\\saga_gui\\res\\svg";
	}
	#endif

	Parameters.Add_Choice  ("", "DIRECTION" , "Direction"       , "", "SVG to Header|Header to SVG");

	Parameters.Add_FilePath("", "SVG_FILES" , "SVG Files"       , "", NULL,    NULL, false, false, true);
	Parameters.Add_FilePath("", "HDR_OUTPUT", "Output Directory", "", NULL, Headers,  true,  true      );

	Parameters.Add_FilePath("", "HDR_FILES" , "Header Files"    , "", NULL, Headers, false, false, true);
	Parameters.Add_FilePath("", "SVG_OUTPUT", "Output Directory", "", NULL,    NULL,  true,  true      );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSVG_Converter::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DIRECTION") )
	{
		pParameters->Set_Enabled("SVG_FILES" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("HDR_OUTPUT", pParameter->asInt() == 0);

		pParameters->Set_Enabled("HDR_FILES" , pParameter->asInt() == 1);
		pParameters->Set_Enabled("SVG_OUTPUT", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVG_Converter::On_Execute(void)
{
	#ifdef _SAGA_MSW
	const SG_Char Separator = '\\';
	#else
	const SG_Char Separator = '/';
	#endif

	CSG_Strings Files; CSG_String OutputFile;

	if( Parameters("DIRECTION")->asInt() == 0 )
	{
		Parameters("SVG_FILES")->asFilePath()->Get_FilePaths(Files);
		OutputFile.Printf("%s%c%%s.svg.h", Parameters("HDR_OUTPUT")->asString(), Separator);
	}
	else
	{
		Parameters("HDR_FILES")->asFilePath()->Get_FilePaths(Files);
		OutputFile.Printf("%s%c%%s"      , Parameters("SVG_OUTPUT")->asString(), Separator);
	}

	//-----------------------------------------------------
	int nConverted = 0;
    const int MaxCharPerLine = 16000;

	for(int i=0; i<Files.Get_Count(); i++)
	{
		CSG_String Content, Name = SG_File_Get_Name(Files[i], false);

		Process_Set_Text("file: " + Name);

		CSG_File Stream;

		if( Stream.Open(Files[i], SG_FILE_R) && Stream.Read(Content, Stream.Length()) )
		{
			if( Parameters("DIRECTION")->asInt() == 0 )
			{
				Content.Replace("\n", ""    );
				Content.Replace("\"", "\\\"");
				Content.Prepend(CSG_String::Format("static const char svg_%s[] = \"", Name.c_str()));
				Content.Append ("\";");

                if( Stream.Open(CSG_String::Format(OutputFile.c_str(), Name.c_str()), SG_FILE_W) )
                {
                    while( Content.Length() > 0 )
                    {
                        if( Content.Length() > MaxCharPerLine )
                        {
                            CSG_String Part = Content.Left(MaxCharPerLine) + "\"\n";

                            Stream.Write(Part);
                        }
                        else
                        {
                            Stream.Write(Content);
                        }

                        Content = Content.Mid(MaxCharPerLine);

                        if( Content.Length() > 0 )
                        {
                            Content = Content.Prepend("\"");
                        }
                    }

                    nConverted++;
                }
			}
			else
			{
                Content.Replace("\"\n\"", "");
				Content = Content.AfterFirst('\"').BeforeLast('\"');
				Content.Replace("\\\"", "\"");

                if( Stream.Open(CSG_String::Format(OutputFile.c_str(), Name.c_str()), SG_FILE_W) )
                {
                    Stream.Write(Content);

                    nConverted++;
                }
			}
		}
	}

	//-----------------------------------------------------
	return( nConverted > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
