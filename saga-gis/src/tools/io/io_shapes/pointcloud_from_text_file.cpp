/**********************************************************
 * Version $Id: pointcloud_from_text_file.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Shapes_IO                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             pointcloud_from_text_file.cpp             //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    LASERDATA GmbH                         //
//                Management and Analysis of             //
//                Laserscanning Data                     //
//                Technikerstr. 21a                      //
//                6020 Innsbruck                         //
//                Austria                                //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pointcloud_from_text_file.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_From_Text_File::CPointCloud_From_Text_File(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Point Cloud from Text File"));

	Set_Author		("V. Wichmann, LASERDATA GmbH (c) 2009");

	Set_Description	(_TW(
		"Creates a point cloud from a text file.\n"
		"The input file must have at least three columns holding the "
		"x, y, z coordinates of each point. You must specify the field "
		"index (i.e. the column) of these. Field index starts to count "
		"with 1. In case you like to import additional attributes, "
		"you have to provide the field indexes for those attributes with the "
		"-FIELDS option as integer numbers separated by semicolon "
		"(e.g. \"-FIELDS=4;5;8\").\n\n"
		"You have also to select the field separator that is used by the file "
		"and if the first line of the file should be skipped "
		"(in case it contains column headings).\n"
		"The columns in the input file can be in any order, and you can "
		"omit columns, but you have to provide the correct field index "
		"for those fields that you like to import.\n\n"
		"The tool usage differs slightly between SAGA GUI and SAGA CMD. "
		"With SAGA GUI you can specify names and types for additional fields in the "
		"'Specifications' sub dialog. To do this using SAGA CMD you have "
		"to use the -FIELDNAMES and -FIELDTYPES options. The first one is for the field names, "
		"the second for the data type specification (see the GUI which "
		"number equals which data type). Again entries have to be "
		"separated by semicolons, e.g. "
		"\"-FIELDNAMES=intensity;class;range -FIELDTYPES=2;2;3\".\n"
	));


	//-----------------------------------------------------
	Parameters.Add_PointCloud_Output(NULL,
		"POINTS"	, _TL("Point Cloud"),
		_TL("")
	);

	Parameters.Add_FilePath(NULL,
		"FILE"		, _TL("Text File"),
		_TL("")
	);

	Parameters.Add_Choice(NULL,
		"SEPARATOR"	, _TL("Field Separator"),
		_TL("Field Separator"),
		CSG_String::Format("%s|%s|%s|",
			_TL("tabulator"),
			_TL("space"),
			_TL("comma")
		), 0
	);

	Parameters.Add_Bool(NULL,
		"SKIP_HEADER", _TL("Skip first line"),
        _TL("Skip first line as it contains column names."),
        false
    );

    Parameters.Add_Int(NULL,
		"XFIELD"	, _TL("X is Column ..."),
		_TL("The column holding the X-coordinate."),
		1, 1, true
	);

    Parameters.Add_Int(NULL,
		"YFIELD"	, _TL("Y is Column ..."),
		_TL("The column holding the Y-coordinate."),
		2, 1, true
	);

    Parameters.Add_Int(NULL,
		"ZFIELD"	, _TL("Z is Column ..."),
		_TL("The column holding the Z-coordinate."),
		3, 1, true
	);

	Parameters.Add_String(NULL,
		"FIELDS"    , _TL("Fields"),
		_TL("The index (starting with 1) of the fields to import, separated by semicolon, e.g. \"5;6;8\""),
		""
	);

	Parameters.Add_String(NULL,
		"FIELDNAMES", _TL("Field Names"),
		_TL("The name to use for each field, separated by semicolon, e.g. \"intensity;class;range\""),
		""
	)->Set_UseInGUI(false);

	Parameters.Add_String(NULL,
		"FIELDTYPES", _TL("Field Types"),
		_TL("The datatype to use for each field, separated by semicolon, e.g. \"2;2;3;\". The number equals the choice selection, see GUI version."),
		""
	)->Set_UseInGUI(false);

	Parameters.Add_Parameters(Parameters("FIELDS"),
		"FIELDSPECS", _TL("Specifications"),
		_TL("")
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPointCloud_From_Text_File::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FIELDS") )
	{
		CSG_String_Tokenizer	tokFields(pParameter->asString(), ";");

		CSG_Parameters	&Fields	= *pParameters->Get_Parameter("FIELDSPECS")->asParameters();

		int	nCurrent	= Fields.Get_Count() / 2;
		int	nFields		= tokFields.Get_Tokens_Count();

		if( nCurrent < nFields )
		{
			for(int iField=nCurrent; iField<nFields; iField++)
			{
				CSG_Parameter	*pNode	= Fields.Add_String(NULL,
					CSG_String::Format("NAME%03d" , iField),
					CSG_String::Format("%d. %s"   , iField + 1, _TL("Field Name")), _TL(""),
					""
				);

				Fields.Add_Choice(pNode,
					CSG_String::Format("TYPE%03d" , iField),
					CSG_String::Format("%d. %s"   , iField + 1, _TL("Field Type")), _TL(""),
					CSG_String::Format("%s|%s|%s|%s|%s|%s|",
						_TL("1 byte signed integer"),
						_TL("2 byte signed integer"),
						_TL("4 byte signed integer"),
						_TL("4 byte floating point"),
						_TL("8 byte floating point"),
						_TL("string"))
				);
			}
		}
		else if( nCurrent > nFields )
		{
			for(int iField=nCurrent-1; iField>=nFields; iField--)
			{
				Fields.Del_Parameter(iField);
			}
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_Text_File::Get_Data_Type(TSG_Data_Type &Type, const CSG_String &Value)
{
	int	iType;

	if( Value.asInt(iType) )
	{
		switch( iType )
		{
		case  0: Type = SG_DATATYPE_Char  ; return( true );
		case  1: Type = SG_DATATYPE_Short ; return( true );
		case  2: Type = SG_DATATYPE_Int   ; return( true );
		case  3: Type = SG_DATATYPE_Float ; return( true );
		case  4: Type = SG_DATATYPE_Double; return( true );
		case  5: Type = SG_DATATYPE_String; return( true );
		}
	}

	if( !Value.CmpNoCase("char"  ) ) { Type = SG_DATATYPE_Char  ; return( true ); }
	if( !Value.CmpNoCase("short" ) ) { Type = SG_DATATYPE_Short ; return( true ); }
	if( !Value.CmpNoCase("int"   ) ) { Type = SG_DATATYPE_Int   ; return( true ); }
	if( !Value.CmpNoCase("float" ) ) { Type = SG_DATATYPE_Float ; return( true ); }
	if( !Value.CmpNoCase("double") ) { Type = SG_DATATYPE_Double; return( true ); }
	if( !Value.CmpNoCase("string") ) { Type = SG_DATATYPE_String; return( true ); }

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_Text_File::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_R, false) )
	{
		Error_Set(_TL("Unable to open input file!"));

		return( false );
	}

	//-----------------------------------------------------
	int	xField	= Parameters("XFIELD")->asInt() - 1;
	int	yField	= Parameters("YFIELD")->asInt() - 1;
	int	zField	= Parameters("ZFIELD")->asInt() - 1;

	char	Separator;

	switch( Parameters("SEPARATOR")->asInt() )
    {
	default:	Separator	= '\t';	break;
    case  1:	Separator	=  ' ';	break;
    case  2:	Separator	=  ',';	break;
    }

    //-----------------------------------------------------
	CSG_String	sLine;
	CSG_Strings	Values;

	if( !Stream.Read_Line(sLine) )
	{
		Error_Set(_TL("Empty file!"));

		return( false );
	}

	if( Parameters("SKIP_HEADER")->asBool() )	// header contains field names
	{
		CSG_String_Tokenizer	tokValues(sLine, Separator);	// read each field name for later use

		while( tokValues.Has_More_Tokens() )
		{
			Values	+= tokValues.Get_Next_Token();
		}
	}
	else
    {
		Stream.Seek_Start();
    }

    //-----------------------------------------------------
    CSG_PointCloud	*pPoints	= SG_Create_PointCloud();
    pPoints->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));
    Parameters("POINTS")->Set_Value(pPoints);

	CSG_Array_Int	Fields;

    //-----------------------------------------------------
    if( SG_UI_Get_Window_Main() )
    {
		CSG_Parameters	&Fields	= *Parameters("FIELDSPECS")->asParameters();

		int	nFields	= Fields.Get_Count() / 2;

		CSG_String	Names, Types;

		for(int iField=0; iField<nFields; iField++)
		{
			Names	+= CSG_String::Format("%s;", Fields(CSG_String::Format("NAME%03d", iField))->asString());
			Types	+= CSG_String::Format("%d;", Fields(CSG_String::Format("TYPE%03d", iField))->asInt   ());
		}

		Parameters("FIELDNAMES")->Set_Value(Names);
		Parameters("FIELDTYPES")->Set_Value(Types);
	}

	{
		TSG_Data_Type	Type	= SG_DATATYPE_Float;	// default

		CSG_String_Tokenizer	tokFields(Parameters("FIELDS"    )->asString(), ";");
		CSG_String_Tokenizer	tokTypes (Parameters("FIELDTYPES")->asString(), ";");
		CSG_String_Tokenizer	tokNames (Parameters("FIELDNAMES")->asString(), ";");

		while( tokFields.Has_More_Tokens() )
		{
			int	iField;

			if( !tokFields.Get_Next_Token().asInt(iField) || iField < 1 )
			{
				Error_Set(_TL("Error parsing attribute field index"));

				return( false );
			}

			Fields	+= iField - 1;

			CSG_String	Name;

			if( tokNames.Has_More_Tokens() )
			{
				Name	= tokNames.Get_Next_Token(); Name.Trim(true); Name.Trim(false);
			}

			if( Name.is_Empty() )
			{
				if( iField - 1 < Values.Get_Count() )
				{
					Name	= Values[iField - 1];
				}
				else
				{
					Name.Printf("FIELD%02d", iField);
				}
			}

			if( tokTypes.Has_More_Tokens() )
			{
				Get_Data_Type(Type, tokTypes.Get_Next_Token());
			}

			pPoints->Add_Field(Name, Type);
		}
	}

    //-----------------------------------------------------
	Process_Set_Text(_TL("Importing data ..."));

	int		nLines	= 0;
	sLong	Length	= Stream.Length();

	while( Stream.Read_Line(sLine) )
    {
		nLines++;

		if( pPoints->Get_Count() % 10000 == 0 && !Set_Progress((double)Stream.Tell(), (double)Length) )
		{
			return( true );	// user break
		}

	    //-------------------------------------------------
		CSG_String_Tokenizer	tokValues(sLine, Separator);

		Values.Clear();

		while( tokValues.Has_More_Tokens() )	// read every column in this line and fill vector
        {
			Values	+= tokValues.Get_Next_Token();
        }

	    //-------------------------------------------------
		double	x, y, z;

		if( xField >= Values.Get_Count() || !Values[xField].asDouble(x)
		||  yField >= Values.Get_Count() || !Values[yField].asDouble(y)
		||  zField >= Values.Get_Count() || !Values[zField].asDouble(z) )
		{
			Message_Add(CSG_String::Format("%s: %s [%d]", _TL("Warning"), _TL("Skipping misformatted line"), nLines));

			continue;
		}

        pPoints->Add_Point(x, y, z);

	    //-------------------------------------------------
		for(int iAttribute=0; iAttribute<pPoints->Get_Attribute_Count(); iAttribute++)
		{
			if( Fields[iAttribute] >= Values.Get_Count() )
			{
				pPoints->Set_NoData(3 + iAttribute);
			}
			else switch( pPoints->Get_Attribute_Type(iAttribute) )
			{
			case SG_DATATYPE_String:
				pPoints->Set_Attribute(iAttribute, Values[Fields[iAttribute]]);
				break;

			default:
				{
					double	Value;

					if( Values[Fields[iAttribute]].asDouble(Value) )
					{
						pPoints->Set_Attribute(iAttribute, Value);
					}
					else
					{
						pPoints->Set_NoData(3 + iAttribute);
					}
				}
				break;
			}
		}
    }

    //-----------------------------------------------------
	DataObject_Set_Parameter(pPoints, "DISPLAY_VALUE_AGGREGATE", 3);	// highest z
	DataObject_Set_Parameter(pPoints, "COLORS_TYPE"            , 3);	// graduated colors
	DataObject_Set_Parameter(pPoints, "METRIC_ATTRIB"          , 2);	// z attrib
	DataObject_Set_Parameter(pPoints, "METRIC_ZRANGE", pPoints->Get_Minimum(2), pPoints->Get_Maximum(2));

	DataObject_Update(pPoints);

    //-----------------------------------------------------
	if( nLines > pPoints->Get_Count() )
	{
		Message_Add(CSG_String::Format("%s: %d %s", _TL("Warning"), nLines - pPoints->Get_Count(), _TL("invalid points have been skipped")), true);
	}

	Message_Add(CSG_String::Format("%d %s", pPoints->Get_Count(), _TL("points have been imported with success")), true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
