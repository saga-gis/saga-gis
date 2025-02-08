
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
//                 colors_from_csv.cpp                   //
//                                                       //
//                 Copyright (C) 2025 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "color_ramps_from_csv.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CColor_Ramps_From_CSV::CColor_Ramps_From_CSV(void)
{
	Set_Name		("Color Ramps from CSV");

	Set_Author		("V. Wichmann (c) 2025");

	Set_Description	(
		"The tool allows one to create SAGA color palettes from CSV files with RGB values."
	);


	Parameters.Add_FilePath(""      , "CSV_FILES" , "CSV Files"       , "", NULL, NULL, false, false, true);
	Parameters.Add_FilePath(""      , "PAL_OUTPUT", "Output Directory", "", NULL, NULL,  true,  true      );
	Parameters.Add_Choice  (""      , "METHOD"    , "Method"          , "", "Color Palette File (*.pal)|C++ Code");
	Parameters.Add_Bool    ("METHOD", "BINARY"    , "Binary Format"   , "Write color palette binary instead of ASCII", false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CColor_Ramps_From_CSV::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("BINARY", pParameter->asInt() == 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CColor_Ramps_From_CSV::On_Execute(void)
{
	#ifdef _SAGA_MSW
	const SG_Char PathSeparator = '\\';
	#else
	const SG_Char PathSeparator = '/';
	#endif

	CSG_Strings Files;
	Parameters("CSV_FILES")->asFilePath()->Get_FilePaths(Files);
	
	CSG_String	OutputPath	= Parameters("PAL_OUTPUT")->asFilePath()->asString();
	int			Method		= Parameters("METHOD")->asInt();
	bool		WriteBinary = Parameters("BINARY")->asBool();


	//-----------------------------------------------------
	for(int i=0; i<Files.Get_Count(); i++)
	{
		CSG_String Name = SG_File_Get_Name(Files[i], false);

		Process_Set_Text("file: " + Name);

		CSG_Table Content;

		if( !Content.Load(Files[i], TABLE_FILETYPE_Text, ',') )
		{
			Error_Fmt("failed to read file: %s", Files[i].c_str());
			return( false );
		}

		if( Method == 0 )
		{
			CSG_Colors Colors((int)Content.Get_Count());

			for(sLong j=0; j<Content.Get_Count(); j++)
			{
				CSG_Table_Record *pRec = Content.Get_Record(j);

				Colors.Set_Color((int)j, pRec->asInt(0), pRec->asInt(1), pRec->asInt(2));
			}

			if( !Colors.Save(CSG_String::Format("%s/%s.pal", OutputPath.c_str(), Name.c_str()), WriteBinary) )
			{
				Error_Fmt("failed to write file: %s", CSG_String::Format("%s%s.pal", OutputPath.c_str(), Name.c_str()).c_str());
				return( false );
			}
		}
		else
		{	
			CSG_String Code = CSG_String::Format("case SG_COLORS_%s:\n", Name.c_str());
			Code += CSG_String::Format("\tSet_Count(%d);\n", Content.Get_Count());

			for(sLong j=0; j<Content.Get_Count(); j++)
			{
				CSG_Table_Record *pRec = Content.Get_Record(j);

				Code += CSG_String::Format("\tSet_Color(%3d, SG_GET_RGB(%3d, %3d, %3d));\n", j, pRec->asInt(0), pRec->asInt(1), pRec->asInt(2));
			}

			Code += SG_T("\tbreak;\n");

			CSG_File Stream;

			if( Stream.Open(CSG_String::Format("%s/%s.txt", OutputPath.c_str(), Name.c_str()), SG_FILE_W) )
			{
				Stream.Write(Code);
			}
			else
			{
				Error_Fmt("failed to write file: %s", CSG_String::Format("%s%s.txt", OutputPath.c_str(), Name.c_str()).c_str());
				return( false );
			}
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
