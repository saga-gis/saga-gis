
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      table_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             table_change_color_format.cpp             //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "table_change_color_format.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Change_Color_Format::CTable_Change_Color_Format(void)
{
	Set_Name		(_TL("Change Color Format"));

	Set_Author		("V.Wichmann (c) 2013-2023");

	Set_Description	(_TW(
		"This tool allows one to convert table fields with SAGA RGB coded values to separate R, G, B components and vice versa. "
		"The tool can process attributes of tables, shapefiles or point clouds."
	));

	//-----------------------------------------------------
	Parameters.Add_Table     ("", "TABLE"     , _TL("Table" ), _TL("The input table."       ), PARAMETER_INPUT);
	Parameters.Add_Table     ("", "OUTPUT"    , _TL("Output"), _TL("The output table."      ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Shapes    ("", "OUTPUT_SHP", _TL("Output"), _TL("The output shapes."     ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_PointCloud("", "OUTPUT_PC" , _TL("Output"), _TL("The output point cloud."), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Table_Field("TABLE", "FIELD_RGB"  , _TL("RGB"  ), _TL("The field with RGB coded values."));

	Parameters.Add_Table_Field("TABLE", "FIELD_RED"  , _TL("Red"  ), _TL("The field with R values."        ));
	Parameters.Add_Table_Field("TABLE", "FIELD_GREEN", _TL("Green"), _TL("The field with G values."        ));
	Parameters.Add_Table_Field("TABLE", "FIELD_BLUE" , _TL("Blue" ), _TL("The field with B values."        ));

	Parameters.Add_Choice("",
		"MODE"       , _TL("Mode of Operation"),
		_TL("Choose the mode of operation."),
		CSG_String::Format("%s|%s",
			_TL("RGB to R, G, B"),
			_TL("R, G, B to RGB")
		), 0
	);

	Parameters.Add_String("",
		"ATTR_SUFFIX", _TL("Attribute Suffix"),
		_TL("Optional suffix for output attribute names."),
		SG_T("")
	);

	Parameters.Add_Choice("",
		"COLOR_DEPTH", _TL("Color Depth"),
		_TL("Choose the color depth of the red, green, blue values, either 8 bit [0-255] or 16 bit [0-65535]."),
		CSG_String::Format("%s|%s",
			_TL("8 bit"),
			_TL("16 bit")
		), 0
	);

	Parameters.Add_Choice("",
		"NORM"       , _TL("Normalization"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
			_TL("range"),
			_TL("standard deviation")
		), 0
	);

	Parameters.Add_Double("NORM",
		"NORM_STDDEV", _TL("Standard Deviation"),
		_TL(""),
		2., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Change_Color_Format::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_Data_Object *pTable = pParameter->asDataObject();

		pParameters->Set_Enabled("OUTPUT"    , pTable && pTable->asTable     ());
		pParameters->Set_Enabled("OUTPUT_SHP", pTable && pTable->asShapes    ());
		pParameters->Set_Enabled("OUTPUT_PC" , pTable && pTable->asPointCloud());
	}
	
	if(	pParameter->Cmp_Identifier("MODE") )
	{
		pParameters->Set_Enabled("FIELD_RGB"  , pParameter->asInt() == 0);
		pParameters->Set_Enabled("FIELD_RED"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("FIELD_GREEN", pParameter->asInt() == 1);
		pParameters->Set_Enabled("FIELD_BLUE" , pParameter->asInt() == 1);
		pParameters->Set_Enabled("COLOR_DEPTH", pParameter->asInt() == 1);
		pParameters->Set_Enabled("NORM"       , pParameter->asInt() == 1);
	}

	if(	pParameter->Cmp_Identifier("NORM") )
	{
		pParameters->Set_Enabled("NORM_STDDEV", pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Color_Format::On_Execute(void)
{
	CSG_Table *pTable = Parameters("TABLE")->asTable();

	switch( pTable->Get_ObjectType() )
	{
	case SG_DATAOBJECT_TYPE_Table     : {
		CSG_Table *pOutput = Parameters("OUTPUT")->asTable();

		if( pOutput && pOutput != pTable )
		{
			pOutput->Create(*pTable->asTable()); pTable = pOutput;
		}
		break; }

	case SG_DATAOBJECT_TYPE_Shapes    : {
		CSG_Shapes *pOutput = Parameters("OUTPUT_SHP")->asShapes();

		if( pOutput && pOutput != pTable )
		{
			pOutput->Create(*pTable->asShapes()); pTable = pOutput;
		}
		break; }

	case SG_DATAOBJECT_TYPE_PointCloud: {
		CSG_PointCloud *pOutput = Parameters("OUTPUT_PC")->asPointCloud();

		if( pOutput && pOutput != pTable )
		{
			pOutput->Create(*pTable->asPointCloud()); pTable = pOutput;
		}
		break; }
	}

	//-----------------------------------------------------
	CSG_String Suffix = Parameters("ATTR_SUFFIX")->asString();

	if( Suffix.Length() > 0 )
	{
		Suffix.Prepend("_");
	}

	switch( Parameters("MODE")->asInt() )
	{
	//-----------------------------------------------------
	default: { // rgb to r, g, b
		int fIn = Parameters("FIELD_RGB")->asInt(), fOut = pTable->Get_Field_Count();

		pTable->Add_Field(CSG_String::Format("R%s", Suffix.c_str()), SG_DATATYPE_Byte);
		pTable->Add_Field(CSG_String::Format("G%s", Suffix.c_str()), SG_DATATYPE_Byte);
		pTable->Add_Field(CSG_String::Format("B%s", Suffix.c_str()), SG_DATATYPE_Byte);

		for(sLong iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
		{
			CSG_Table_Record &Record = *pTable->Get_Record(iRecord);

			Record.Set_Value(fOut + 0, SG_GET_R(Record.asInt(fIn)));
			Record.Set_Value(fOut + 1, SG_GET_G(Record.asInt(fIn)));
			Record.Set_Value(fOut + 2, SG_GET_B(Record.asInt(fIn)));
		}

		break; }

	//-----------------------------------------------------
	case  1: { // r, g, b to rgb
		bool b8bit = Parameters("COLOR_DEPTH")->asInt() == 0;

		int fIn[3], fOut = pTable->Get_Field_Count();

		pTable->Add_Field(CSG_String::Format("RGB%s", Suffix.c_str()), b8bit ? SG_DATATYPE_DWord : SG_DATATYPE_ULong);

		fIn[0] = Parameters("FIELD_RED"  )->asInt();
		fIn[1] = Parameters("FIELD_GREEN")->asInt();
		fIn[2] = Parameters("FIELD_BLUE" )->asInt();

		double Norm[2][3], StdDev = Parameters("NORM_STDDEV")->asDouble();

		for(int i=0; i<3; i++)
		{
			switch( Parameters("NORM")->asInt() )
			{
			default: Norm[0][i] =                          0.; Norm[1][i] =                                                                                                 0.; break;
			case  1: Norm[0][i] = pTable->Get_Minimum(fIn[i]); Norm[1][i] = pTable->Get_Range (fIn[i]) > 0.                ? 255. / (pTable->Get_Range (fIn[i])         ) : 0.; break;
			case  2: Norm[0][i] = pTable->Get_Mean   (fIn[i]); Norm[1][i] = pTable->Get_StdDev(fIn[i]) > 0. && StdDev > 0. ? 255. / (pTable->Get_StdDev(fIn[i]) * StdDev) : 0.; break;
			}
		}

		for(sLong iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
		{
			CSG_Table_Record &Record = *pTable->Get_Record(iRecord);

			int rgb[3];

			for(int i=0; i<3; i++)
			{
				double d = Record.asDouble(fIn[i]);

				if( Norm[1][i] )
				{
					d = Norm[1][i] * (d - Norm[0][i]);
				}

				rgb[i] = (int)(d + 0.5); if( rgb[i] < 0 ) { rgb[i] = 0; } else if( rgb[i] > 255 ) { rgb[i] = 255; }
			}

			if( b8bit )
			{
				Record.Set_Value(fOut, rgb[0] +   256 * rgb[1] +      65536 * rgb[2]);
			}
			else
			{
				Record.Set_Value(fOut, rgb[0] + 65536 * rgb[1] + 4294967296 * rgb[2]);
			}
		}
		break; }
	}

	//-----------------------------------------------------
	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
