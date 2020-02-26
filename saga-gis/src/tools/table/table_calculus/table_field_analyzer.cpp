
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                table_field_analyzer.cpp               //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "table_field_analyzer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Field_Extreme::CTable_Field_Extreme(void)
{
	Set_Name		(_TL("Find Field of Extreme Value"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Identifies from the selected attributes that one, "
		"which has the maximum or respectively the minimum value. "
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"INPUT"			, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields("INPUT",
		"FIELDS"		, _TL("Attributes"),
		_TL("")
	);

	Parameters.Add_Table_Field("INPUT",
		"EXTREME_ID"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field("INPUT",
		"EXTREME_VALUE"	, _TL("Value"),
		_TL(""),
		true
	);

	Parameters.Add_Table("",
		"OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"TYPE"			, _TL("TYPE"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("minimum"),
			_TL("maximum")
		), 1
	);

	Parameters.Add_Choice("",
		"IDENTIFY"		, _TL("Attribute Identification"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("name"),
			_TL("index")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Field_Extreme::On_Execute(void)
{
	//-----------------------------------------------------
	int	*Fields	= (int *)Parameters("FIELDS")->asPointer();
	int	nFields	=        Parameters("FIELDS")->asInt    ();

	if( nFields <= 1 )
	{
		Error_Set(_TL("needs at least two attributes in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("INPUT")->asTable();

	if( !pTable->is_Valid() )
	{
		Error_Set(_TL("invalid table"));

		return( false );
	}

	if( Parameters("OUTPUT")->asTable() && Parameters("OUTPUT")->asTable() != pTable )
	{
		pTable	= Parameters("OUTPUT")->asTable();
		pTable	->Create( *Parameters("INPUT")->asTable());
		pTable	->Set_Name(Parameters("INPUT")->asTable()->Get_Name());
	}

	//-----------------------------------------------------
	int		fID, fValue;

	bool	bMaximum	= Parameters("TYPE"    )->asInt() == 1;
	bool	bName		= Parameters("IDENTIFY")->asInt() == 0;

	if( (fID = Parameters("EXTREME_ID")->asInt()) < 0 )
	{
		fID	= pTable->Get_Field_Count();
		pTable->Add_Field(bMaximum ? "MAX_FIELD" : "MIN_FIELD", SG_DATATYPE_String);
	}
	else if( bName == true  && pTable->Get_Field_Type(fID) != SG_DATATYPE_String )
	{
		pTable->Set_Field_Type(fID, SG_DATATYPE_String);
	}
	else if( bName == false && pTable->Get_Field_Type(fID) != SG_DATATYPE_Int )
	{
		pTable->Set_Field_Type(fID, SG_DATATYPE_Int);
	}

	if( (fValue = Parameters("EXTREME_VALUE")->asInt()) < 0 )
	{
		fValue	= pTable->Get_Field_Count();
		pTable->Add_Field(bMaximum ? "MAX_VALUE" : "MIN_VALUE", SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		int		xField	= Fields[0];
		double	xValue	= pRecord->asDouble(xField);

		for(int iField=1; iField<nFields; iField++)
		{
			if( ( bMaximum && xValue < pRecord->asDouble(Fields[iField]))
			||  (!bMaximum && xValue > pRecord->asDouble(Fields[iField])) )
			{
				xField	= Fields[iField];
				xValue	= pRecord->asDouble(xField);
			}
		}

		if( bName )
			pRecord->Set_Value(fID, pTable->Get_Field_Name(xField));
		else
			pRecord->Set_Value(fID, xField);

		pRecord->Set_Value(fValue, xValue);
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
