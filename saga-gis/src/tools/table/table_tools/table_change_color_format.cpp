
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
	Parameters.Add_Table     ("", "TABLE"		, _TL("Table" )				, _TL("The input table." )		, PARAMETER_INPUT);
	Parameters.Add_Table     ("", "OUTPUT"		, _TL("Output")				, _TL("The output table.")		, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Shapes    ("", "OUTPUT_SHP"	, _TL("Output Shapes")		, _TL("The output shapes.")		, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_PointCloud("", "OUTPUT_PC"	, _TL("Output Point Cloud")	, _TL("The output point cloud."), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Table_Field("TABLE", "FIELD_SAGA_RGB", _TL("SAGA RGB"), _TL("The field with SAGA RGB coded values." ), true);
	Parameters.Add_Table_Field("TABLE", "FIELD_RED"     , _TL("Red"     ), _TL("The field with R values."), true);
	Parameters.Add_Table_Field("TABLE", "FIELD_GREEN"   , _TL("Green"   ), _TL("The field with G values."), true);
	Parameters.Add_Table_Field("TABLE", "FIELD_BLUE"    , _TL("Blue"    ), _TL("The field with B values."), true);

	Parameters.Add_Choice("",
		"MODE", _TL("Mode of Operation"),
		_TL("Choose the mode of operation."),
		CSG_String::Format("%s|%s",
			_TL("SAGA RGB to R, G, B"),
			_TL("R, G, B to SAGA RGB")
		), 0
	);

	Parameters.Add_String("",
		"ATTR_SUFFIX", _TL("Attribute Suffix"),
		_TL("Optional suffix for output attribute names."),
		SG_T("")
	);

	Parameters.Add_Choice("",
		"COLOR_DEPTH", _TL("Color Depth"),
		_TL("Choose the color depth of the R,G,B values, either 8 bit [0-255] or 16 bit [0-65535]."),
		CSG_String::Format(SG_T("%s|%s|"),
		_TL("8 bit"),
		_TL("16 bit")
		), 0
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
		if( pParameter->asDataObject() == NULL )
		{
			pParameters->Set_Enabled("OUTPUT"		, true);
			pParameters->Set_Enabled("OUTPUT_SHP"	, false);
			pParameters->Set_Enabled("OUTPUT_PC"	, false);

			Parameters("OUTPUT_SHP")	->Set_Value(DATAOBJECT_NOTSET);
			Parameters("OUTPUT_PC")		->Set_Value(DATAOBJECT_NOTSET);
		}
		else
		{
			TSG_Data_Object_Type Type = pParameter->asDataObject()->Get_ObjectType();
		
			switch( Type )
			{
			default:
			case SG_DATAOBJECT_TYPE_Table:
				pParameters->Set_Enabled("OUTPUT"		, true);
				pParameters->Set_Enabled("OUTPUT_SHP"	, false);
				pParameters->Set_Enabled("OUTPUT_PC"	, false);

				Parameters("OUTPUT_SHP")	->Set_Value(DATAOBJECT_NOTSET);
				Parameters("OUTPUT_PC")		->Set_Value(DATAOBJECT_NOTSET);
				break;

			case SG_DATAOBJECT_TYPE_Shapes:
				pParameters->Set_Enabled("OUTPUT"		, false);
				pParameters->Set_Enabled("OUTPUT_SHP"	, true);
				pParameters->Set_Enabled("OUTPUT_PC"	, false);

				Parameters("OUTPUT")		->Set_Value(DATAOBJECT_NOTSET);
				Parameters("OUTPUT_PC")		->Set_Value(DATAOBJECT_NOTSET);
				break;

			case SG_DATAOBJECT_TYPE_PointCloud:
				pParameters->Set_Enabled("OUTPUT"		, false);
				pParameters->Set_Enabled("OUTPUT_SHP"	, false);
				pParameters->Set_Enabled("OUTPUT_PC"	, true);

				Parameters("OUTPUT")		->Set_Value(DATAOBJECT_NOTSET);
				Parameters("OUTPUT_SHP")	->Set_Value(DATAOBJECT_NOTSET);
				break;
			}
		}
	}
	
	if(	pParameter->Cmp_Identifier("MODE") )
	{
		int iMode = pParameter->asInt();

		pParameters->Set_Enabled("FIELD_SAGA_RGB", iMode == 0);
		pParameters->Set_Enabled("FIELD_RED"     , iMode == 1);
		pParameters->Set_Enabled("FIELD_GREEN"   , iMode == 1);
		pParameters->Set_Enabled("FIELD_BLUE"    , iMode == 1);
		pParameters->Set_Enabled("COLOR_DEPTH"   , iMode == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Color_Format::On_Execute(void)
{
	CSG_Table		*pTable;
	CSG_Data_Object *pOutput = NULL;
	
	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asTable() != NULL )
	{
		pOutput = Parameters("OUTPUT")->asDataObject();
	}
	else if( Parameters("OUTPUT_SHP")->asShapes() != NULL )
	{
		pOutput = Parameters("OUTPUT_SHP")->asDataObject();
	}
	else if( Parameters("OUTPUT_PC")->asPointCloud() != NULL )
	{
		pOutput = Parameters("OUTPUT_PC")->asDataObject();
	}

	//-----------------------------------------------------
	CSG_Data_Object *pInput = Parameters("TABLE")->asDataObject();

	if( pOutput && pOutput != pInput )
	{
		TSG_Data_Object_Type Type = pOutput->Get_ObjectType();

		switch( Type )
		{
		default:
		case SG_DATAOBJECT_TYPE_Table:
			pTable = pOutput->asTable();
			pTable->Assign  (Parameters("TABLE")->asTable());
			pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
			break;
		case SG_DATAOBJECT_TYPE_Shapes:
			pTable = pOutput->asShapes();
			pTable->Assign  (Parameters("TABLE")->asShapes());
			pTable->Set_Name(Parameters("TABLE")->asShapes()->Get_Name());
			break;
		case SG_DATAOBJECT_TYPE_PointCloud:
			pTable = pOutput->asPointCloud();
			pTable->Assign  (Parameters("TABLE")->asPointCloud());
			pTable->Set_Name(Parameters("TABLE")->asPointCloud()->Get_Name());
			break;
		}
	}
	else
	{
		pTable = Parameters("TABLE")->asTable();
	}


	//-----------------------------------------------------
	int			iFieldRGB	= Parameters("FIELD_SAGA_RGB")->asInt();
	int			iFieldR		= Parameters("FIELD_RED"     )->asInt();
	int			iFieldG		= Parameters("FIELD_GREEN"   )->asInt();
	int			iFieldB		= Parameters("FIELD_BLUE"    )->asInt();
	int			iDepth		= Parameters("COLOR_DEPTH"   )->asInt();
	CSG_String	sAttrSuffix	= Parameters("ATTR_SUFFIX"   )->asString();

	if( sAttrSuffix.Length() > 0 )	{ sAttrSuffix.Prepend(SG_T("_")); }

	//-----------------------------------------------------
	int Mode = Parameters("MODE")->asInt();

	switch( Mode )
	{
	default:
	case 0:
		if( iFieldRGB < 0 )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid SAGA RGB field for this kind of operation!"));
			return( false );
		}

		iFieldR = pTable->Get_Field_Count();

		pTable->Add_Field(CSG_String::Format("R%s", sAttrSuffix.c_str()), SG_DATATYPE_Int);
		pTable->Add_Field(CSG_String::Format("G%s", sAttrSuffix.c_str()), SG_DATATYPE_Int);
		pTable->Add_Field(CSG_String::Format("B%s", sAttrSuffix.c_str()), SG_DATATYPE_Int);
		break;

	case  1:
		if( iFieldR < 0 || iFieldG < 0 || iFieldB < 0 )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid R,G,B fields for this kind of operation!"));

			return( false );
		}

		iFieldRGB = pTable->Get_Field_Count();

		pTable->Add_Field(CSG_String::Format("RGB%s", sAttrSuffix.c_str()), SG_DATATYPE_Int);
		break;
	}


	//-----------------------------------------------------
	for(sLong iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record *pRecord = pTable->Get_Record(iRecord);

		switch( Mode )
		{
		default:
		case 0:
			pRecord->Set_Value(iFieldR,		SG_GET_R(pRecord->asInt(iFieldRGB)));
			pRecord->Set_Value(iFieldR + 1,	SG_GET_G(pRecord->asInt(iFieldRGB)));
			pRecord->Set_Value(iFieldR + 2,	SG_GET_B(pRecord->asInt(iFieldRGB)));
			break;

		case  1:
			double r = pRecord->asDouble(iFieldR);
			double g = pRecord->asDouble(iFieldG);
			double b = pRecord->asDouble(iFieldB);

			if( iDepth == COLOR_DEPTH_16BIT )
			{
				r = r / 65535 * 255;
				g = g / 65535 * 255;
				b = b / 65535 * 255;
			}

			pRecord->Set_Value(iFieldRGB,	SG_GET_RGB(r, g, b));
			break;
		}
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
