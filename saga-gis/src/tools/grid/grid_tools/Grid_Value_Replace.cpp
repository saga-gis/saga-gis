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
	// 1. Info...

	Set_Name		(_TL("Change Grid Values"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"Changes values of a grid according to the rules of a user defined lookup table. "
		"Values or value ranges that are not listed in the lookup table remain unchanged. "
		"If the target is not set, the changes will be stored to the original grid. ")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "GRID_IN"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRID_OUT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Replace Condition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Grid value equals low value"),
			_TL("Low value < grid value < high value"),
			_TL("Low value <= grid value < high value")
		)
	);

	//-----------------------------------------------------
	CSG_Table			*pLookup;
	CSG_Table_Record	*pRecord;

	pLookup	= Parameters.Add_FixedTable(
		NULL	, "LOOKUP"		, _TL("Lookup Table"),
		_TL("")
	)->asTable();

	pLookup->Add_Field(_TL("Low Value")		, SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("High Value")		, SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("Replace with")	, SG_DATATYPE_Double);

	pRecord	= pLookup->Add_Record();	pRecord->Set_Value(0, 0.0);	pRecord->Set_Value(1, 0.0);	pRecord->Set_Value(2, 10.0);
	pRecord	= pLookup->Add_Record();	pRecord->Set_Value(0, 1.0);	pRecord->Set_Value(1, 4.0);	pRecord->Set_Value(2, 11.0);
}

//---------------------------------------------------------
CGrid_Value_Replace::~CGrid_Value_Replace(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Replace::On_Execute(void)
{
	bool			bReplace;
	int				x, y, iRecord, Method;
	double			Value;
	CSG_Grid			*pGrid;
	CSG_Table			*pLookup;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	pLookup	= Parameters("LOOKUP")->asTable();

	if( pLookup->Get_Record_Count() > 0 )
	{
		if( Parameters("GRID_OUT")->asGrid() == NULL || Parameters("GRID_IN")->asGrid() == Parameters("GRID_OUT")->asGrid() )
		{
			pGrid	= Parameters("GRID_IN")	->asGrid();
		}
		else
		{
			pGrid	= Parameters("GRID_OUT")->asGrid();
			pGrid->Assign(Parameters("GRID_IN")->asGrid());
		}

		Method	= Parameters("METHOD")->asInt();

		//-------------------------------------------------
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				Value	= pGrid->asDouble(x, y);

				for(iRecord=0, bReplace=false; iRecord<pLookup->Get_Record_Count() && !bReplace; iRecord++)
				{
					pRecord	= pLookup->Get_Record(iRecord);

					switch( Method )
					{
					case 0:	// grid value equals low value...
						bReplace	= Value == pRecord->asDouble(0);
						break;

					case 1:	// low value < grid value < high value...
						bReplace	= pRecord->asDouble(0) < Value && Value < pRecord->asDouble(1);
						break;

					case 2:	// low value <= grid value <= high value...
						bReplace	= pRecord->asDouble(0) <= Value && Value <= pRecord->asDouble(1);
						break;
					}

					if( bReplace )
					{
						pGrid->Set_Value(x, y, pRecord->asDouble(2));
					}
				}
			}
		}

		//-------------------------------------------------
		DataObject_Update(pGrid, pGrid->Get_ZMin(), pGrid->Get_ZMax());

		return( true );
	}

	return( false );
}
