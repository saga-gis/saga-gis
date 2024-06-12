
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
#include "pointcloud_from_text_file.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_From_Text_File::CPointCloud_From_Text_File(void)
{
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
	Parameters.Add_PointCloud("",
		"POINTS"	, _TL("Point Cloud"),
		_TL(""), PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("Text File"),
		_TL(""),
		CSG_String::Format("%s (*.txt, *.xyz)|*.txt;*.xyz|%s|*.*",
			_TL("Text Files"), _TL("All Files")
		), NULL, false, false, false
	);

	Parameters.Add_Choice("",
		"SEPARATOR"	, _TL("Field Separator"),
		_TL("Field Separator"),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("tabulator"),
			_TL("space"    ),
			_TL("comma"    ),
			_TL("semicolon")
		), 0
	);

	Parameters.Add_Bool("",
		"SKIP_HEADER", _TL("Skip first line"),
        _TL("Skip first line as it contains column names."),
        false
    );

    Parameters.Add_Int("",
		"XFIELD"	, _TL("X is Column ..."),
		_TL("The column holding the X-coordinate."),
		1, 1, true
	);

    Parameters.Add_Int("",
		"YFIELD"	, _TL("Y is Column ..."),
		_TL("The column holding the Y-coordinate."),
		2, 1, true
	);

    Parameters.Add_Int("",
		"ZFIELD"	, _TL("Z is Column ..."),
		_TL("The column holding the Z-coordinate."),
		3, 1, true
	);

	Parameters.Add_String("",
		"FIELDS"    , _TL("Fields"),
		_TL("The index (starting with 1) of the fields to import, separated by semicolon, e.g. \"5;6;8\""),
		""
	);

	Parameters.Add_String("",
		"FIELDNAMES", _TL("Field Names"),
		_TL("The name to use for each field, separated by semicolon, e.g. \"intensity;class;range\""),
		""
	)->Set_UseInGUI(false);

	Parameters.Add_String("",
		"FIELDTYPES", _TL("Field Types"),
		_TL("The datatype to use for each field, separated by semicolon, e.g. \"2;2;3;\". The number equals the choice selection, see GUI version."),
		""
	)->Set_UseInGUI(false);

	if( has_GUI() )
	{
		Parameters.Add_Parameters("FIELDS",
			"FIELDSPECS", _TL("Field Definition"),
			_TL("")
		)->Set_UseInCMD(false);
	}

	m_CRS.Create(Parameters);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_Text_File::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	if( has_GUI() )
	{
		Parameters.Set_Parameter("POINTS", DATAOBJECT_CREATE);
	}

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CPointCloud_From_Text_File::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_ID_NAME(i)	CSG_String::Format("NAME%d", i)
#define GET_ID_TYPE(i)	CSG_String::Format("TYPE%d", i)

//---------------------------------------------------------
int CPointCloud_From_Text_File::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS.On_Parameter_Changed(pParameters, pParameter);

	if( pParameter->Cmp_Identifier("FIELDS") && pParameters->Get_Parameter("FIELDSPECS") )
	{
		CSG_Array_Int Fields; CSG_Strings sFields = SG_String_Tokenize(pParameter->asString(), ";,");

		for(int i=0; i<sFields.Get_Count(); i++)
		{
			int Field; if( sFields[i].asInt(Field) && Field > 0 ) { Fields += Field; }
		}

		CSG_Parameters *pFields = pParameters->Get_Parameter("FIELDSPECS")->asParameters();

		int nFields = (int)Fields.Get_Size(), nCurrent = pFields->Get_Count() / 2;

		for(int i=0; i<nCurrent && i<nFields; i++)
		{
			pFields->Get_Parameter(GET_ID_NAME(i))->Set_Name(CSG_String::Format("%s %d", _TL("Field"), Fields[i]));
		}

		if( nCurrent < nFields )
		{
			for(int i=nCurrent; i<nFields; i++)
			{
				CSG_String Name(CSG_String::Format("%s %d", _TL("Field"), Fields[i]));

				pFields->Add_String   (""            , GET_ID_NAME(i),      Name  , _TL("Name"), Name);
				pFields->Add_Data_Type(GET_ID_NAME(i), GET_ID_TYPE(i), _TL("Type"), _TL("Type"), SG_DATATYPES_Numeric|SG_DATATYPES_String, SG_DATATYPE_Float);
			}
		}
		else if( nCurrent > nFields )
		{
			for(int i=nCurrent, j=2*nCurrent; i>nFields; i--)
			{
				pFields->Del_Parameter(--j);
				pFields->Del_Parameter(--j);
			}
		}

		pParameters->Set_Enabled("FIELDSPECS", nFields > 0);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_Text_File::On_Execute(void)
{
	CSG_File Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_R, false) )
	{
		Error_Set(_TL("Unable to open input file!"));

		return( false );
	}

    //-----------------------------------------------------
	CSG_String sLine;

	if( !Stream.Read_Line(sLine) )
	{
		Error_Set(_TL("Empty file!"));

		return( false );
	}

	//-----------------------------------------------------
	char Separator;

	switch( Parameters("SEPARATOR")->asInt() )
    {
	default: Separator = '\t'; break;
	case  1: Separator =  ' '; break;
	case  2: Separator =  ','; break;
	case  3: Separator =  ';'; break;
	}

	//-----------------------------------------------------
	CSG_Strings Values = SG_String_Tokenize(sLine, Separator); // read first line to retrieve the number of fields

	int nFields = Values.Get_Count();

	if( !Parameters("SKIP_HEADER")->asBool() )
	{
		Stream.Seek_Start(); Values.Clear();
	}

	//-----------------------------------------------------
	int xField = Parameters("XFIELD")->asInt() - 1; if( xField < 0 || xField >= nFields ) { Error_Fmt("%s\n0 < x(%d) < %d", _TL("Field index is out-of-range!"), 1 + xField, 1 + nFields); return( false ); }
	int yField = Parameters("YFIELD")->asInt() - 1; if( yField < 0 || yField >= nFields ) { Error_Fmt("%s\n0 < y(%d) < %d", _TL("Field index is out-of-range!"), 1 + yField, 1 + nFields); return( false ); }
	int zField = Parameters("ZFIELD")->asInt() - 1; if( zField < 0 || zField >= nFields ) { Error_Fmt("%s\n0 < z(%d) < %d", _TL("Field index is out-of-range!"), 1 + zField, 1 + nFields); return( false ); }

    //-----------------------------------------------------
    CSG_PointCloud *pPoints = Parameters("POINTS")->asPointCloud();

	pPoints->Create();

	pPoints->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));

	m_CRS.Get_CRS(pPoints->Get_Projection(), true);

    //-----------------------------------------------------
	if( has_GUI() )
    {
		CSG_Parameters &Fields = *Parameters("FIELDSPECS")->asParameters();

		CSG_String Names, Types; int nFields = Fields.Get_Count() / 2;

		for(int i=0; i<nFields; i++)
		{
			Names += CSG_String::Format("%s;", Fields(GET_ID_NAME(i))->asString());
			Types += CSG_String::Format("%d;", Fields(GET_ID_TYPE(i))->asString());
		}

		Parameters("FIELDNAMES")->Set_Value(Names);
		Parameters("FIELDTYPES")->Set_Value(Types);
	}

	//-----------------------------------------------------
	CSG_Array_Int Fields;

	{
		CSG_Strings Field(SG_String_Tokenize(Parameters("FIELDS"    )->asString(), ";,"));
		CSG_Strings Names(SG_String_Tokenize(Parameters("FIELDNAMES")->asString(), ";,"));
		CSG_Strings Types(SG_String_Tokenize(Parameters("FIELDTYPES")->asString(), ";,"));

		for(int i=0, Index; i<Field.Get_Count(); i++)
		{
			if( !Field[i].asInt(Index) || Index < 1 || Index > nFields )
			{
				Error_Fmt("%s\n%s\n0 < \"%s\" < %d", _TL("Error parsing attribute field index!"), _TL("Provided value is not an integer or out-of-range!"),
					Field[i].c_str(), 1 + nFields
				);

				return( false );
			}

			Fields += Index - 1;

			CSG_String Name(i < Names.Get_Count() ? Names[i] : i < nFields ? Values[i] : CSG_String("")); Name.Trim_Both();

			if( Name.is_Empty() )
			{
				Name.Printf("Field %d", Index);
			}

			int Type; if( i >= Types.Get_Count() || !Types[i].asInt(Type) ) { Type = -1; }

			switch( Type )
			{
			case  0: pPoints->Add_Field(Name, SG_DATATYPE_String); break;
			case  1: pPoints->Add_Field(Name, SG_DATATYPE_Byte  ); break;
			case  2: pPoints->Add_Field(Name, SG_DATATYPE_Char  ); break;
			case  3: pPoints->Add_Field(Name, SG_DATATYPE_Word  ); break;
			case  4: pPoints->Add_Field(Name, SG_DATATYPE_Short ); break;
			case  5: pPoints->Add_Field(Name, SG_DATATYPE_DWord ); break;
			case  6: pPoints->Add_Field(Name, SG_DATATYPE_Int   ); break;
			case  7: pPoints->Add_Field(Name, SG_DATATYPE_ULong ); break;
			case  8: pPoints->Add_Field(Name, SG_DATATYPE_Long  ); break;
			default: pPoints->Add_Field(Name, SG_DATATYPE_Float ); break;
			case 10: pPoints->Add_Field(Name, SG_DATATYPE_Double); break;
			}
		}
	}

    //-----------------------------------------------------
	Process_Set_Text(_TL("Importing data ..."));

	sLong nLines = 0, Length = Stream.Length();

	while( Stream.Read_Line(sLine) )
    {
		nLines++;

		if( pPoints->Get_Count() % 10000 == 0 && !Set_Progress(Stream.Tell(), Length) )
		{
			break; // user break
		}

	    //-------------------------------------------------
		Values = SG_String_Tokenize(sLine, Separator); // read every column in this line and fill vector

		double x, y, z;

		if( xField >= Values.Get_Count() || !Values[xField].asDouble(x)
		||  yField >= Values.Get_Count() || !Values[yField].asDouble(y)
		||  zField >= Values.Get_Count() || !Values[zField].asDouble(z) )
		{
			Message_Fmt("\n%s: %s [%d]", _TL("Warning"), _TL("Skipping misformatted line"), nLines);

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

			default: {
				double Value;

				if( Values[Fields[iAttribute]].asDouble(Value) )
				{
					pPoints->Set_Attribute(iAttribute, Value);
				}
				else
				{
					pPoints->Set_NoData(3 + iAttribute);
				}
				break; }
			}
		}
    }

    //-----------------------------------------------------
	DataObject_Set_Parameter(pPoints, "DISPLAY_VALUE_AGGREGATE", 3); // highest z
	DataObject_Set_Parameter(pPoints, "COLORS_TYPE"            , 3); // graduated colors
	DataObject_Set_Parameter(pPoints, "METRIC_ATTRIB"          , 2); // z attrib
	DataObject_Set_Parameter(pPoints, "METRIC_ZRANGE", pPoints->Get_Minimum(2), pPoints->Get_Maximum(2));

	DataObject_Update(pPoints);

    //-----------------------------------------------------
	if( nLines > pPoints->Get_Count() )
	{
		Message_Add(" ", true);
		Message_Fmt("%s: %d %s", _TL("Warning"), nLines - pPoints->Get_Count(), _TL("invalid points have been skipped"));
	}

	Message_Add(" ", true);
	Message_Fmt("%d %s", pPoints->Get_Count(), _TL("points have been imported with success"));

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
