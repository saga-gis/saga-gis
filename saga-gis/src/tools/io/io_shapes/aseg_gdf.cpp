
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      io_shapes                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     aseg_gdf.cpp                    //
//                                                       //
//                 Copyright (C) 2023 by                 //
//                      Olaf Conrad                      //
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
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "aseg_gdf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CASEG_GDF_Import::CASEG_GDF_Import(void)
{
	Set_Name		(_TL("Import ASEG-GDF"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"Import a <i>General Data Format Revision 2 (GDF2)</i> file as defined by the "
		"<i>Australian Society of Exploration Geophysicists (ASEG)</i> as table. "
	));

	Add_Reference("Dampney, C.N.G., Pilkington, G. & Pratt, S.J.", "1985",
		"ASEG-GDF: The ASEG Standard for Digital Transfer of Geophysical Data",
		"Exploration Geophysics, 16:1, 123-138.",
		SG_T("https://doi.org/10.1071/EG985123"), SG_T("doi:10.1071/EG985123")
	);

	Add_Reference("https://www.aseg.org.au/technical/aseg-technical-standards",
		SG_T("ASEG Technical Standards")
	);

	Add_Reference("https://www.aseg.org.au/sites/default/files/pdf/ASEG-GDF2-REV4.pdf",
		SG_T("The ASEG-GDF2 Standard for Point Located Data")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS", _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_FilePath("",
		"FILE"  , _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.dfn)|*.dfn|%s|*.*",
			_TL("GDF Definition Files"),
			_TL("All Files")
		), NULL, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CASEG_GDF_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CASEG_GDF_Import::On_Execute(void)
{
	CSG_Table Fields; CSG_String File = Parameters("FILE")->asString();

	if( !Read_Fields(File, Fields) )
	{
		Error_Set(_TL("failed to load field definitions."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes &Points = *Parameters("POINTS")->asShapes();

	SG_File_Set_Extension(File, "dat");

	if( !Read_Data(File, Points, Fields) )
	{
		Error_Set(_TL("failed to load data."));

		return( false );
	}

	//-----------------------------------------------------
	SG_File_Set_Extension(File, "des");

	if( !Read_Description(File, Points) )
	{
		// nop - no problem
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CASEG_GDF_Import::Read_Record_Type(const CSG_String &Record, CSG_String &Type)
{
	CSG_Strings Keys = SG_String_Tokenize(Record, ",");

	if( Keys.Get_Count() >= 2 && Keys[0].Find("DEFN") >= 0 && Keys[0].Find("DEFN") < Keys[0].Find("ST=RECD") && Keys[1].Find("RT") >= 0 )
	{
		Type = Keys[1].AfterFirst('='); Type.Trim_Both();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CASEG_GDF_Import::Read_Field(const CSG_String &Record, CSG_Table_Record &Field)
{
	CSG_Strings Values = SG_String_Tokenize(Record, ":");

	if( Values.Get_Count() >= 2 )
	{
		Values[0].Trim_Both(); Values[1].Trim_Both();

		Field.Set_Value("id"    , Values[0]);
		Field.Set_Value("format", Values[1]);

		switch( Values[1][0] )
		{
		case 'A': Field.Set_Value("type", SG_DATATYPE_String   ); break;
		case 'I': Field.Set_Value("type", SG_DATATYPE_Int      ); break;
		case 'D': Field.Set_Value("type", SG_DATATYPE_Double   ); break;
		case 'E': Field.Set_Value("type", SG_DATATYPE_Double   ); break;
		case 'F': Field.Set_Value("type", SG_DATATYPE_Double   ); break;
		case 'L': Field.Set_Value("type", SG_DATATYPE_Char     ); break;
		case 'X': Field.Set_Value("type", SG_DATATYPE_Undefined); break;
		default: // error or array definition (which is not supported yet!)
			return( false );
		}

		int Width; Values[1] = Values[1].Right(Values[1].Length() - 1);

		if( !Values[1].asInt(Width) || Width < 1 )
		{
			return( false );
		}

		Field.Set_Value("width", Width);

		if( Values.Get_Count() >= 3 )
		{
			Values = SG_String_Tokenize(Values[2], ",");

			for(int i=0; i<Values.Get_Count(); i++)
			{
				Values[i].Trim_Both(); CSG_String Value = Values[i].AfterFirst('='); Value.Trim();

				if( !Value.is_Empty() )
				{
					if( Values[i].Find("UNIT") == 0 ) { Field.Set_Value("unit", Value); }
					if( Values[i].Find("NAME") == 0 ) { Field.Set_Value("name", Value); }
					if( Values[i].Find("NULL") == 0 ) { Field.Set_Value("null", Value); }
				}
				else
				{
					Field.Set_Value("comment", Values[i]);
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CASEG_GDF_Import::Read_Fields(const CSG_String &File, CSG_Table &Fields)
{
	CSG_File Stream(File, SG_FILE_R, false);

	if( !Stream.is_Open() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Fields.Destroy();
	Fields.Add_Field("id"     , SG_DATATYPE_String);
	Fields.Add_Field("type"   , SG_DATATYPE_Int   );
	Fields.Add_Field("width"  , SG_DATATYPE_Int   );
	Fields.Add_Field("format" , SG_DATATYPE_String);
	Fields.Add_Field("unit"   , SG_DATATYPE_String);
	Fields.Add_Field("name"   , SG_DATATYPE_String);
	Fields.Add_Field("null"   , SG_DATATYPE_String);
	Fields.Add_Field("comment", SG_DATATYPE_String);

	//-----------------------------------------------------
	CSG_String Line;

	while( Stream.Read_Line(Line) )
	{
		CSG_Strings Record = SG_String_Tokenize(Line, ";"); CSG_String Type;

		if( Read_Record_Type(Record[0], Type) && (Type.is_Empty() || Type.CmpNoCase("DATA") == 0) )
		{
			for(int i=1; i<Record.Get_Count(); i++)
			{
				if( !Read_Field(Record[i], *Fields.Add_Record()) )
				{
					Fields.Del_Record(Fields.Get_Count() - 1);
				}
			}
		}
	}

	return( Fields.Get_Count() > 1 );
}

//---------------------------------------------------------
bool CASEG_GDF_Import::Read_Data(const CSG_String &File, CSG_Shapes &Points, const CSG_Table &Fields)
{
	CSG_File Stream(File, SG_FILE_R, false);

	if( !Stream.is_Open() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Points.Create(SHAPE_TYPE_Point, SG_File_Get_Name(File, false)); int xField = -1, yField = -1;

	CSG_MetaData &mdFields = *Points.Get_MetaData().Add_Child("Fields");

	for(sLong i=0; i<Fields.Get_Count(); i++)
	{
		if( Fields[i].asInt("type") != SG_DATATYPE_Undefined )
		{
			CSG_String s; CSG_MetaData &mdField = *mdFields.Add_Child(CSG_String::Format("Field %d", 1 + Points.Get_Field_Count()));
			s = Fields[i].asString("id"     ); if( !s.is_Empty() ) { mdField.Add_Child("id"     , s); }
			s = Fields[i].asString("format" ); if( !s.is_Empty() ) { mdField.Add_Child("type"   , s); }
			s = Fields[i].asString("name"   ); if( !s.is_Empty() ) { mdField.Add_Child("name"   , s); }
			s = Fields[i].asString("unit"   ); if( !s.is_Empty() ) { mdField.Add_Child("unit"   , s); }
			s = Fields[i].asString("null"   ); if( !s.is_Empty() ) { mdField.Add_Child("null"   , s); }
			s = Fields[i].asString("comment"); if( !s.is_Empty() ) { mdField.Add_Child("comment", s); }

			CSG_String ID(Fields[i].asString("id"));

			if( ID.CmpNoCase("longitude") == 0 ) { xField = Points.Get_Field_Count(); }
			if( ID.CmpNoCase( "latitude") == 0 ) { yField = Points.Get_Field_Count(); }

			Points.Add_Field(ID, (TSG_Data_Type)Fields[i].asInt("type"));
		}
	}

	if( xField < 0 ) { Message_Fmt("%s: x/lon %s", _TL("Warning"), _TL("coordinate not found")); }
	if( yField < 0 ) { Message_Fmt("%s: y/lat %s", _TL("Warning"), _TL("coordinate not found")); }

	//-----------------------------------------------------
	CSG_String Line;

	while( Stream.Read_Line(Line) && Set_Progress(Stream.Tell(), Stream.Length()) )
	{
		if( Line.is_Empty() )
		{
			continue;
		}

		size_t pos = 0; CSG_Shape &Point = *Points.Add_Shape();

		for(int i=0, j=0; i<Fields.Get_Count(); pos+=Fields[i++].asInt("width"))
		{
			if( Fields[i].asInt("type") != SG_DATATYPE_Undefined )
			{
				CSG_String Value = Line.Mid(pos, Fields[i].asInt("width")); Value.Trim_Both();

				Point.Set_Value(j++, Value);
			}
		}

		if( xField >= 0 && yField >= 0 )
		{
			Point.Set_Point(Point.asDouble(xField), Point.asDouble(yField));
		}
	}

	return( Points.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CASEG_GDF_Import::Read_Description(const CSG_String &File, CSG_Shapes &Points)
{
	CSG_File Stream(File, SG_FILE_R, true);

	if( !Stream.is_Open() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String Line, Key, Value; CSG_MetaData &Description = *Points.Get_MetaData().Add_Child("Description");

	while( Stream.Read_Line(Line) && Set_Progress(Stream.Tell(), Stream.Length()) )
	{
		CSG_String s = Line.BeforeFirst(':');

		if( s.Find("COMM") != 0 )
		{
			break;
		}

		s = s.AfterFirst(' '); s.Trim_Both();

		if( Key.Cmp(s) )
		{
			if( !Key.is_Empty() )
			{
				Description.Add_Child(Key, Value);
			}

			Key = s; Value.Clear();
		}

		s = Line.AfterFirst(':'); s.Trim_Both(); Value += s;
	}

	if( !Key.is_Empty() )
	{
		Description.Add_Child(Key, Value);
	}

	if( Description("geospatial_bounds_crs") )
	{
		Points.Get_Projection().Create(Description["geospatial_bounds_crs"].Get_Content());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
