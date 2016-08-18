/**********************************************************
 * Version $Id: line_dissolve.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   line_dissolve.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
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
#include "line_dissolve.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Dissolve::CLine_Dissolve(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Line Dissolve"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Dissolves line shapes, which share the same attribute value(s)."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_1"		, _TL("1. Attribute"),
		_TL(""),
		false
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_2"		, _TL("2. Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_3"		, _TL("3. Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes(
		NULL	, "DISSOLVED"	, _TL("Dissolved Lines"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "ALL"			, _TL("Dissolve..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("lines with same attribute value(s)"),
			_TL("all lines")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Dissolve::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LINES")) && pParameters->Get_Parameter("LINES")->asShapes() != NULL )
	{
		int	nFields	= pParameters->Get_Parameter("LINES")->asShapes()->Get_Field_Count();

		pParameters->Get_Parameter("FIELD_2")->Set_Value(nFields);
		pParameters->Get_Parameter("FIELD_3")->Set_Value(nFields);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Dissolve::On_Execute(void)
{
	bool		bAll;
	int			Field_1, Field_2, Field_3, iLine;
	CSG_String	Value;
	CSG_Shape	*pLine , *pUnion;
	CSG_Shapes	*pLines, *pUnions;

	//-----------------------------------------------------
	pLines	= Parameters("LINES")		->asShapes();
	pUnions		= Parameters("DISSOLVED")	->asShapes();
	Field_1		= Parameters("FIELD_1")		->asInt();
	Field_2		= Parameters("FIELD_2")		->asInt();
	Field_3		= Parameters("FIELD_3")		->asInt();
	bAll		= Parameters("ALL")			->asInt() == 1;

	//-----------------------------------------------------
	if(	pLines->is_Valid() )
	{
		pUnions->Create(SHAPE_TYPE_Line);

		//-------------------------------------------------
		if( bAll || Field_1 >= pLines->Get_Field_Count() )
		{
			pUnions->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pLines->Get_Name(), _TL("Dissolved")));
			pUnions->Add_Field(_TL("ID"), SG_DATATYPE_Int);

			pUnion	= pUnions->Add_Shape();

			for(iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
			{
				Add_Line(pUnion, pLines->Get_Shape(iLine));
			}
		}

		//-------------------------------------------------
		else
		{
			Value	= pLines->Get_Field_Name(Field_1);
			pUnions	->Add_Field(pLines->Get_Field_Name(Field_1), pLines->Get_Field_Type(Field_1));

			if( Field_2 >= 0 )
			{
				Value	+= CSG_String::Format(SG_T(", %s"), pLines->Get_Field_Name(Field_2));
				pUnions	->Add_Field(pLines->Get_Field_Name(Field_2), pLines->Get_Field_Type(Field_2));
			}

			if( Field_3 >= 0 )
			{
				Value	+= CSG_String::Format(SG_T(", %s"), pLines->Get_Field_Name(Field_3));
				pUnions	->Add_Field(pLines->Get_Field_Name(Field_3), pLines->Get_Field_Type(Field_3));
			}

			pLines->Set_Index(Field_1, TABLE_INDEX_Ascending, Field_2, TABLE_INDEX_Ascending, Field_3, TABLE_INDEX_Ascending);

			pUnions->Set_Name(CSG_String::Format(SG_T("%s [%s: %s]"), pLines->Get_Name(), _TL("Dissolved"), Value.c_str()));

			for(iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
			{
				pLine	= pLines->Get_Shape(pLines->Get_Record_byIndex(iLine)->Get_Index());

				CSG_String	s(pLine->asString(Field_1));

				if( Field_2 >= 0 )	s	+= pLine->asString(Field_2);
				if( Field_3 >= 0 )	s	+= pLine->asString(Field_3);

				if( iLine == 0 || Value.Cmp(s) )
				{
					Value	= s;

					pUnion	= pUnions->Add_Shape(pLine, SHAPE_COPY_GEOM);

					pUnion->Set_Value(0, pLine->asString(Field_1));

					if( Field_2 >= 0 )	pUnion->Set_Value(1, pLine->asString(Field_2));
					if( Field_3 >= 0 )	pUnion->Set_Value(2, pLine->asString(Field_3));
				}
				else
				{
					Add_Line(pUnion, pLine);
				}
			}
		}

		//-------------------------------------------------
		return( pUnions->is_Valid() );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Dissolve::Add_Line(CSG_Shape *pLine, CSG_Shape *pAdd)
{
	if( pLine && pAdd )
	{
		int	nParts	= pLine->Get_Part_Count();

		for(int iPart=0; iPart<pAdd->Get_Part_Count(); iPart++)
		{
			if( pAdd->Get_Point_Count(iPart) > 1 )
			{
				int	jPart	= pLine->Get_Part_Count();

				for(int iPoint=0; iPoint<pAdd->Get_Point_Count(iPart); iPoint++)
				{
					pLine->Add_Point(pAdd->Get_Point(iPoint, iPart), jPart);
				}
			}
		}

		return( nParts < pLine->Get_Part_Count() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
