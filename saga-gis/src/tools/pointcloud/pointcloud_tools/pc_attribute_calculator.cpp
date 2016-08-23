/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              pc_attribute_calculator.cpp              //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_attribute_calculator.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

CPC_Attribute_Calculator::CPC_Attribute_Calculator(void)
{

	Set_Name(_TL("Point Cloud Attribute Calculator"));

	Set_Author(_TL("Volker Wichmann (c) 2010-213, LASERDATA GmbH"));

	CSG_String	s(_TW(
		"The Point Cloud Attribute Calculator calculates a new attribute "
		"based on existing attributes and a mathematical formula. "
		"Attribute fields are addressed by the character 'f' (for 'field') "
		"followed by the field number (i.e.: f1, f2, ..., fn) "
		"or by the field name in square brackets (e.g.: [Field Name]).\n"
		"Examples:\n"
		"sin(f1) * f2 + f3\n"
		"[intensity] / 1000\n\n"
		"The following operators are available for the formula definition:\n"
	));

	s	+= CSG_Formula::Get_Help_Operators();

	Set_Description(s);


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "PC_IN"		,_TL("Point Cloud"),
		_TL("Input"),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "PC_OUT"		,_TL("Result"),
		_TL("Output"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_String(
		NULL	, "FORMULA"		, _TL("Formula"),
		_TL(""),
		SG_T("f1+f2")
	);

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Output Field Name"),
		_TL(""),
		SG_T("Calculation")
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Field data type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("1 bit"),
			_TL("1 byte unsigned integer"),
			_TL("1 byte signed integer"),
			_TL("2 byte unsigned integer"),
			_TL("2 byte signed integer"),
			_TL("4 byte unsigned integer"),
			_TL("4 byte signed integer"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point")
		), 7
	);

	Parameters.Add_Value(
		NULL	, "USE_NODATA"	, _TL("Use NoData"),
		_TL("Check this in order to include NoData points in the calculation."),
		PARAMETER_TYPE_Bool, false
	);

}


//---------------------------------------------------------
CPC_Attribute_Calculator::~CPC_Attribute_Calculator(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
bool CPC_Attribute_Calculator::On_Execute(void)
{
	CSG_PointCloud	*pPC_in, *pPC_out, PC_out;
	TSG_Data_Type	Type;
	bool			bUseNoData;


	//---------------------------------------------------------
	pPC_in		= Parameters("PC_IN")->asPointCloud();
	pPC_out		= Parameters("PC_OUT")->asPointCloud();
	bUseNoData	= Parameters("USE_NODATA")->asBool();

	switch( Parameters("TYPE")->asInt() )
	{
	case 0:				Type	= SG_DATATYPE_Bit;		break;
	case 1:				Type	= SG_DATATYPE_Byte;		break;
	case 2:				Type	= SG_DATATYPE_Char;		break;
	case 3:				Type	= SG_DATATYPE_Word;		break;
	case 4:				Type	= SG_DATATYPE_Short;	break;
	case 5:				Type	= SG_DATATYPE_DWord;	break;
	case 6:				Type	= SG_DATATYPE_Int;		break;
	case 7: default:	Type	= SG_DATATYPE_Float;	break;
	case 8:				Type	= SG_DATATYPE_Double;	break;
	}


	//-----------------------------------------------------
	if( !pPC_in->is_Valid() || pPC_in->Get_Field_Count() <= 0 || pPC_in->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("invalid point cloud"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Formula	Formula;

	int		nFields		= pPC_in->Get_Field_Count();
	int		*pFields	= new int[nFields];

	if( !Formula.Set_Formula(Get_Formula(Parameters("FORMULA")->asString(), pPC_in, pFields, nFields)) )
	{
		CSG_String	Message;

		Formula.Get_Error(Message);

		Error_Set(Message);

		delete[](pFields);

		return( false );
	}

	if( nFields == 0 )
	{
		SG_UI_Msg_Add_Error(_TL("No attribute fields specified!"));
		delete[](pFields);
		return( false );
	}


	//-----------------------------------------------------
	if (!pPC_out || pPC_out == pPC_in)
	{
		pPC_out = &PC_out;
	}

	pPC_out->Create(pPC_in);

	pPC_out->Add_Field(Parameters("NAME")->asString(), Type);


	//---------------------------------------------------------
	CSG_Vector	Values(nFields);

	for( int i=0; i<pPC_in->Get_Point_Count() && Set_Progress(i, pPC_in->Get_Point_Count()); i++ )
	{
		bool	bOkay	= true;

		pPC_out->Add_Point(pPC_in->Get_X(i), pPC_in->Get_Y(i), pPC_in->Get_Z(i));

		for (int j=0; j<pPC_in->Get_Attribute_Count(); j++)
		{
			switch (pPC_in->Get_Attribute_Type(j))
			{
			default:					pPC_out->Set_Attribute(i, j, pPC_in->Get_Attribute(i, j));		break;
			case SG_DATATYPE_Date:
			case SG_DATATYPE_String:	CSG_String sAttr; pPC_in->Get_Attribute(i, j, sAttr); pPC_out->Set_Attribute(i, j, sAttr);		break;
			}
		}

		for( int iField=0; iField<nFields && bOkay; iField++ )
		{
			if( !pPC_in->is_NoData(i, pFields[iField]) || bUseNoData )
			{
				Values[iField]	= pPC_in->Get_Value(i, pFields[iField]);
			}
			else
			{
				bOkay = false;
			}
		}

		if( bOkay )
		{
			pPC_out->Set_Value(i, pPC_in->Get_Field_Count(), Formula.Get_Value(Values.Get_Data(), nFields));
		}
		else
		{
			pPC_out->Set_NoData(i, pPC_in->Get_Field_Count());
		}
	}


	delete[](pFields);


	//-----------------------------------------------------
	if (pPC_out == &PC_out)
	{
		CSG_MetaData	History	= pPC_in->Get_History();
		CSG_String		sName	= pPC_in->Get_Name();

		pPC_in->Assign(pPC_out);

		pPC_in->Get_History() = History;
		pPC_in->Set_Name(sName);

		Parameters("PC_OUT")->Set_Value(pPC_in);
	}
	else
	{
		pPC_out->Set_Name(CSG_String::Format(SG_T("%s_%s"), pPC_in->Get_Name(), Parameters("NAME")->asString()));
	}

	return (true);

}


//---------------------------------------------------------
CSG_String	CPC_Attribute_Calculator::Get_Formula(CSG_String sFormula, CSG_Table *pTable, int *Fields, int &nFields)
{
	const SG_Char	vars[27]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	int		iField;

	for(iField=pTable->Get_Field_Count()-1, nFields=0; iField>=0 && nFields<26; iField--)
	{
		bool		bUse	= false;

		CSG_String	sField;

		sField.Printf(SG_T("f%d"), iField + 1);

		if( sFormula.Find(sField) >= 0 )
		{
			sFormula.Replace(sField, CSG_String(vars[nFields]));

			bUse	= true;
		}

		sField.Printf(SG_T("[%s]"), pTable->Get_Field_Name(iField));

		if( sFormula.Find(sField) >= 0 )
		{
			sFormula.Replace(sField, CSG_String(vars[nFields]));

			bUse	= true;
		}

		if( bUse )
		{
			Fields[nFields++]	= iField;
		}
	}

	return( sFormula );
}


//---------------------------------------------------------
int CPC_Attribute_Calculator::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FORMULA")) )
		pParameters->Get_Parameter(SG_T("NAME"))->Set_Value(pParameter->asString());

    return (true);

}

//---------------------------------------------------------
bool CPC_Attribute_Calculator::On_After_Execution(void)
{
	CSG_PointCloud	*pPC_out	= Parameters("PC_OUT")->asPointCloud();

	if (pPC_out == NULL)
	{
		pPC_out = Parameters("PC_IN")->asPointCloud();
	}

	DataObject_Set_Parameter(pPC_out, "DISPLAY_VALUE_AGGREGATE", 3);
	DataObject_Set_Parameter(pPC_out, "COLORS_TYPE", 2);
	DataObject_Set_Parameter(pPC_out, "METRIC_ATTRIB", 2);
	DataObject_Set_Parameter(pPC_out, "METRIC_ZRANGE", pPC_out->Get_Minimum(2), pPC_out->Get_Maximum(2));

	CSG_Colors	Colors;
	Colors.Set_Default(255);
	DataObject_Set_Colors(pPC_out, Colors);

	if (pPC_out == Parameters("PC_IN")->asPointCloud())
	{
		Parameters("PC_OUT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return (true);
}