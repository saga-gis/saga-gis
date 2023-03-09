
/*******************************************************************************
    GridsFromTableAndGrid.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GridsFromTableAndGrid.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridsFromTableAndGrid::CGridsFromTableAndGrid(void)
{
    Set_Name		(_TL("Grids from Classified Grid and Table"));

	Set_Author		("Victor Olaya (c) 2004");

    Set_Version     ("2.0");

	Set_Description	(_TW(
		"The tool allows one to create grids from a classified grid and a corresponding lookup table. The "
        "table must provide an attribute with the class identifiers used in the grid, which is used to link "
        "the table and the grid. A grid is created for each additional attribute field found in the table."
	));

	Parameters.Add_Table(
		""	, "TABLE"		, _TL("Table"),
		_TL("The table with the (numeric) data values for each class. The tool creates a grid for each table column (besides the ID)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		"TABLE"	, "ID_FIELD"	, _TL("Attribute"),
		_TL("The attribute with the class IDs, used to link the table and the grid.")
	);

	Parameters.Add_Grid(
		""	, "CLASSES"		, _TL("Classes"),
		_TL("The grid encoded with the class IDs."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		""	, "GRIDS"		, _TL("Grids"),
		_TL("The output grids, one grid for each table column."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridsFromTableAndGrid::On_Execute(void)
{
	//-----------------------------------------------------
    CSG_Grid                *pClasses	= Parameters("CLASSES" )->asGrid();
    CSG_Parameter_Grid_List *pGrids		= Parameters("GRIDS"   )->asGridList();
    CSG_Table               *pTable		= Parameters("TABLE"   )->asTable();
	int                     iField		= Parameters("ID_FIELD")->asInt();

	//-----------------------------------------------------
	if( pTable->Get_Field_Count() == 0 || pTable->Get_Count() == 0 )
	{
		Message_Add(_TL("selected table contains no valid records"));

		return( false );
	}

  	//-----------------------------------------------------
	int *Attribute	= new int[pTable->Get_Field_Count()];
    int nAttributes = 0;

    pGrids->Del_Items();

	for(int i=0; i<pTable->Get_Field_Count(); i++)
	{
		if( i != iField && pTable->Get_Field_Type(i) != SG_DATATYPE_String )
		{
			Attribute[nAttributes++]	= i;

			CSG_Grid	*pGrid	= SG_Create_Grid(Get_System());

			pGrid->Fmt_Name("%s [%s]", pClasses->Get_Name(), pTable->Get_Field_Name(i));

			pGrids->Add_Item(pGrid);
		}
	}

	if( nAttributes == 0 )
	{
		delete[](Attribute);

		Message_Add(_TL("selected table does not have numeric attributes"));

		return( false );
	}

    //-----------------------------------------------------
    std::map<double, sLong>   mapKeys;

    for(sLong i=0; i<pTable->Get_Count(); i++)
    {
        mapKeys.insert(std::pair<double, sLong>(pTable->Get_Record(i)->asDouble(iField), i));
    }

	//-----------------------------------------------------
    for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
    {
        #pragma omp parallel for
        for(int x=0; x<Get_NX(); x++)
        {
            std::map<double, sLong>::iterator it = mapKeys.find(pClasses->asDouble(x, y));

            if( it == mapKeys.end())
            {
                for(int i=0; i<nAttributes; i++)
                {
                    pGrids->Get_Grid(i)->Set_NoData(x, y);
                }
            }
            else
            {
                for(int i=0; i<nAttributes; i++)
                {
                    pGrids->Get_Grid(i)->Set_Value(x, y, pTable->Get_Record(it->second)->asDouble(Attribute[i]));
                }
            }
        }
    }

	//-----------------------------------------------------
	delete[](Attribute);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
