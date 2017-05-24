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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Value_Replace.cpp                //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Grid_Value_Replace.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_Replace::CGrid_Value_Replace(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Change Grid Values"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Changes values of a grid according to the rules of a user defined lookup table. "
		"Values or value ranges that are not listed in the lookup table remain unchanged. "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"OUTPUT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(NULL,
		"METHOD"	, _TL("Replace Condition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("identity"),
			_TL("range"),
			_TL("synchronize look-up table classification")
		)
	);

	//-----------------------------------------------------
	CSG_Table	*pLUT;

	pLUT	= Parameters.Add_FixedTable(NULL,
		"IDENTITY"	, _TL("Lookup Table"),
		_TL("")
	)->asTable();

	pLUT->Add_Field(_TL("New Value"), SG_DATATYPE_Double);
	pLUT->Add_Field(_TL("Value"    ), SG_DATATYPE_Double);
	pLUT->Add_Record();

	pLUT	= Parameters.Add_FixedTable(NULL,
		"RANGE"		, _TL("Lookup Table"),
		_TL("")
	)->asTable();

	pLUT->Add_Field(_TL("New Value"), SG_DATATYPE_Double);
	pLUT->Add_Field(_TL("Minimum"  ), SG_DATATYPE_Double);
	pLUT->Add_Field(_TL("Maximum"  ), SG_DATATYPE_Double);
	pLUT->Add_Record();

	Parameters.Add_Grid(NULL,
		"GRID"		, _TL("Grid Classification"),
		_TL("Synchronize with look-up table classification of another grid (gui only)."),
		PARAMETER_INPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Value_Replace::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("IDENTITY", pParameter->asInt() == 0);
		pParameters->Set_Enabled("RANGE"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRID"    , pParameter->asInt() == 2);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Replace::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("OUTPUT")->asGrid();

	if( !pGrid || pGrid == Parameters("INPUT")->asGrid() )
	{
		pGrid	= Parameters("INPUT")->asGrid();
	}
	else
	{
		pGrid->Assign(Parameters("INPUT")->asGrid());

		DataObject_Set_Parameters(pGrid, Parameters("INPUT")->asGrid());

		pGrid->Set_Name(CSG_String::Format("%s [%s]", Parameters("INPUT")->asGrid()->Get_Name(), _TL("Changed")));
	}

	//-----------------------------------------------------
	int		Method	= Parameters("METHOD")->asInt();

	CSG_Table	LUT;

	switch( Method )
	{
	default:	LUT.Create(*Parameters("IDENTITY")->asTable());	break;
	case  1:	LUT.Create(*Parameters("RANGE"   )->asTable());	break;
	case  2:	LUT.Create( Parameters("RANGE"   )->asTable());
		if( SG_UI_Get_Window_Main()	// gui only
		&&  DataObject_Get_Parameter(Parameters("GRID" )->asGrid(), "LUT")
		&&  DataObject_Get_Parameter(Parameters("INPUT")->asGrid(), "LUT") )
		{
			CSG_Table	LUTs[2];

			LUTs[0].Create(*DataObject_Get_Parameter(Parameters("GRID" )->asGrid(), "LUT")->asTable());
			LUTs[1].Create(*DataObject_Get_Parameter(Parameters("INPUT")->asGrid(), "LUT")->asTable());

			for(int i=0; i<LUTs[0].Get_Count(); i++)
			{
				CSG_String	Name	= LUTs[0][i].asString(1);

				for(int j=LUTs[1].Get_Count()-1; j>=0; j--)
				{
					if( !Name.Cmp(LUTs[1][j].asString(1)) )
					{
						CSG_Table_Record	*pReplace	= LUT.Add_Record();

						pReplace->Set_Value(0, LUTs[0][i].asDouble(3));
						pReplace->Set_Value(1, LUTs[1][j].asDouble(3));
						pReplace->Set_Value(2, LUTs[1][j].asDouble(4));

						LUTs[1].Del_Record(j);

						break;
					}
				}
			}

			for(int j=0; j<LUTs[1].Get_Count(); j++)
			{
				LUTs[0].Add_Record(LUTs[1].Get_Record(j));
			}

			DataObject_Add(pGrid);
			CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pGrid, "LUT");
			pLUT->asTable()->Assign_Values(&LUTs[0]);
			DataObject_Set_Parameter(pGrid, pLUT);
		}
		break;
	}

	//-----------------------------------------------------
	if( LUT.Get_Count() == 0 )
	{
		Error_Set(_TL("empty look-up table, nothing to replace"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			double	Value	= pGrid->asDouble(x, y);

			for(int i=0; i<LUT.Get_Count(); i++)
			{
				if( Method == 0 )
				{
					if( LUT[i].asDouble(1) == Value )
					{
						pGrid->Set_Value(x, y, LUT[i].asDouble(0));

						break;
					}
				}
				else
				{
					if( LUT[i].asDouble(1) <= Value && Value <= LUT[i].asDouble(2) )
					{
						pGrid->Set_Value(x, y, LUT[i].asDouble(0));

						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
