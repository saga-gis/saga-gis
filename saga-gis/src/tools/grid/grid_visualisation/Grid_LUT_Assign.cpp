
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  grid_lut_assign.cpp                  //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "Grid_LUT_Assign.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_LUT_Assign::CGrid_LUT_Assign(void)
{
	Set_Name		(_TL("Select Look-up Table for Grid Visualization"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Select a look-up table for visual classification of a grid. "
		"Useful in combination with tool chains. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		"", "LUT"	, _TL("Look-up Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("LUT", "NAME"       , _TL("Name"                 ), _TL(""), false);
	Parameters.Add_Table_Field("LUT", "VALUE"      , _TL("Value"                ), _TL(""), false);
	Parameters.Add_Table_Field("LUT", "VALUE_MAX"  , _TL("Value (Range Maximum)"), _TL(""),  true);
	Parameters.Add_Table_Field("LUT", "DESCRIPTION", _TL("Description"          ), _TL(""),  true);
	Parameters.Add_Table_Field("LUT", "COLOR"      , _TL("Color"                ), _TL(""),  true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_LUT_Assign::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("LUT") )
	{
		if( pParameter->asTable() )
		{
			if( pParameter->asTable()->Get_Field_Count() < 5 )
			{
				pParameters->Set_Parameter("VALUE"      , 0);
				pParameters->Set_Parameter("NAME"       , 1);
			}
			else // if( pParameter->asTable()->Get_Field_Count() >= 5 )
			{
				pParameters->Set_Parameter("COLOR"      , 0);
				pParameters->Set_Parameter("NAME"       , 1);
				pParameters->Set_Parameter("DESCRIPTION", 2);
				pParameters->Set_Parameter("VALUE"      , 3);
				pParameters->Set_Parameter("VALUE_MAX"  , 4);
			}
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_LUT_Assign::On_Execute(void)
{
	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pGrid, "LUT");

	if( !pLUT || !pLUT->asTable() || pLUT->asTable()->Get_Field_Count() < 5 )
	{
		return( false );
	}

	CSG_Table &LUT = *Parameters("LUT")->asTable();

	if( !LUT.is_Valid() || LUT.Get_Field_Count() < 2 || LUT.Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int        fName = Parameters("NAME"       )->asInt();
	int       fValue = Parameters("VALUE"      )->asInt();
	int   fValue_Max = Parameters("VALUE_MAX"  )->asInt();
	int fDescription = Parameters("DESCRIPTION")->asInt();
	int       fColor = Parameters("COLOR"      )->asInt();

	for(int i=0; i<LUT.Get_Count(); i++)
	{
		CSG_Table_Record &Source = *LUT.Get_Record(i), *pTarget = pLUT->asTable()->Get_Record(i);

		if( !pTarget )
		{
			pTarget = pLUT->asTable()->Add_Record();

			pTarget->Set_Value(0, fColor >= 0 ? Source.asInt(fColor) : SG_Color_Get_Random());
		}

		pTarget->Set_Value(1,                     Source.asString(fName       ));
		pTarget->Set_Value(2, fDescription >= 0 ? Source.asString(fDescription) : SG_T(""));
		pTarget->Set_Value(3,                     Source.asDouble(fValue      ));
		pTarget->Set_Value(4, fValue_Max   >= 0 ? Source.asDouble(fValue_Max  ) : Source.asDouble(fValue));
	}

	pLUT->asTable()->Set_Count(LUT.Get_Count());

	//-----------------------------------------------------
	DataObject_Set_Parameter(pGrid, pLUT);
	DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table

	DataObject_Update(pGrid);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLUT_Create::CLUT_Create(void)
{
	Set_Name		(_TL("Create a Table from Look-up Table"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Creates a table object from a look-up table for visual data object classifications. "
		"Useful in combination with tool chains. "
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		"", "TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FixedTable(
		"", "LUT"	, _TL("Look-up Table"),
		_TL("")
	);

	//-----------------------------------------------------
	CSG_Table	*pLUT	= Parameters("LUT")->asTable();

	pLUT->Add_Field("COLOR"      , SG_DATATYPE_Color );
	pLUT->Add_Field("NAME"       , SG_DATATYPE_String);
	pLUT->Add_Field("DESCRIPTION", SG_DATATYPE_String);
	pLUT->Add_Field("MINIMUM"    , SG_DATATYPE_Double);
	pLUT->Add_Field("MAXIMUM"    , SG_DATATYPE_Double);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLUT_Create::On_Execute(void)
{
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	return( pTable->Create(*Parameters("LUT")->asTable()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
