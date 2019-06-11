/*******************************************************************************
    OWA.cpp
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

//---------------------------------------------------------
#include "owa.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COWA::COWA(void)
{
	Set_Name		(_TL("Ordered Weighted Averaging"));

	Set_Author		("Victor Olaya (c) 2006");

	Set_Description	(_TW(
		"The ordered weighted averaging (OWA) tool calculates for each cell "
		"the weighted average from the values of the supplied grids. "
		"The weighting factor for each grid value is defined with the "
		"'Weights' table. If the 'Ordered' flag is unchecked, the order of "
		"the weights correspond to the order of the grids in the input list. "
		"If the 'Ordered' flag is checked, the grid values will be sorted "
		"and the weights will be assigned to the values in their ascending order, "
		"i.e. from the lowest to the highest value. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Input Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Output Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"ORDERED"	, _TL("Ordered"),
		_TL(""),
		true
	);

	Parameters.Add_FixedTable("", 
		"WEIGHTS"	, _TL("Weights"),
		_TL("")
	);

	CSG_Table	*pWeights	= Parameters("WEIGHTS")->asTable();

	pWeights->Add_Field(_TL("Weight"), SG_DATATYPE_Double);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COWA::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRIDS") && pParameter->asGridList()->Get_Grid_Count() > 1 )
	{
		CSG_Table	&Table	= *(*pParameters)("WEIGHTS")->asTable();

		for(int i=Table.Get_Count(); i<pParameter->asGridList()->Get_Grid_Count(); i++)
		{
			Table.Add_Record()->Set_Value(0, 1.);
		}

		Table.Set_Record_Count(pParameter->asGridList()->Get_Grid_Count());
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COWA::On_Execute(void)
{
	CSG_Grid	*pOWA = Parameters("OUTPUT")->asGrid();

	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 2 )
	{
		Error_Set(_TL("Nothing to do! There are less than two grids in the input list!"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	&Table	= *Parameters("WEIGHTS")->asTable();

	if( Table.Get_Count() < pGrids->Get_Grid_Count() )
	{
		Error_Fmt("%s [%d < %d]", _TL("Not enough entries in the weights table!"),
			Table.Get_Count(), pGrids->Get_Grid_Count()
		);

		return( false );
	}

	//-----------------------------------------------------
	int		i;	

	CSG_Vector	Weights(pGrids->Get_Grid_Count());

	double	Sum	= 0.0;

	for(i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		if( Table[i].asDouble(0) > 0.0 )
		{
			Sum	+= (Weights[i] = Table[i].asDouble(0));
		}
		else
		{
			Error_Set(_TL("All weights have to be a positive number."));

			return( false );
		}
	}

	for(i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		Weights[i]	/= Sum;	
	}

	bool	bOrdered	= Parameters("ORDERED")->asBool();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(i)
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Vector	Values(pGrids->Get_Grid_Count());

			bool	bOkay	= true;

			for(i=0; bOkay && i<pGrids->Get_Grid_Count(); i++)
			{
				if( (bOkay = !pGrids->Get_Grid(i)->is_NoData(x, y)) == true )
				{
					Values[i]	= pGrids->Get_Grid(i)->asDouble(x, y);
				}
			}

			if( bOkay == false || (bOrdered && !Values.Sort()) )
			{
				pOWA->Set_NoData(x, y);
			}
			else
			{
				double	Value	= 0.0;

				for(i=0; i<pGrids->Get_Grid_Count(); i++)
				{
					Value	+= Weights[i] * Values[i];
				}

				pOWA->Set_Value(x, y, Value);
			}
		}
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
