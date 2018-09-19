/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Fuzzify.cpp
    Copyright (DecMin) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR IncMin PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Fuzzify.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFuzzify::CFuzzify(void)
{
	Set_Name		(_TL("Fuzzify"));

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"Translates grid values into fuzzy set membership as preparation for fuzzy set analysis. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Fuzzified Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Node("", "INCREASE", _TL("Increase"), _TL(""));
	Parameters.Add_Double("INCREASE", "INC_MIN", _TL("From"), _TL(""), 0.0);
	Parameters.Add_Double("INCREASE", "INC_MAX", _TL("To"  ), _TL(""), 0.3);

	Parameters.Add_Node("", "DECREASE", _TL("Decrease"), _TL(""));
	Parameters.Add_Double("DECREASE", "DEC_MIN", _TL("From"), _TL(""), 0.7);
	Parameters.Add_Double("DECREASE", "DEC_MAX", _TL("To"  ), _TL(""), 1.0);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Increase"),
			_TL("Decrease"),
			_TL("Increase and Decrease")
		), 0
	);

	Parameters.Add_Choice("",
		"TRANSITION", _TL("Transition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("linear"),
			_TL("sigmoidal"),
			_TL("j-shaped")
		), 0
	);

	Parameters.Add_Bool("",
		"INVERT"	, _TL("Invert"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"AUTOFIT"	, _TL("Adjust"),
		_TL("Automatically adjust control points to grid's data range"),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFuzzify::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("INPUT"  )
	||	pParameter->Cmp_Identifier("AUTOFIT")
	||	pParameter->Cmp_Identifier("METHOD" ) )
	{
		if( pParameters->Get_Parameter("AUTOFIT")->asBool() && pParameters->Get_Parameter("INPUT")->asGrid() )
		{
			CSG_Grid	*pGrid	= pParameters->Get_Parameter("INPUT")->asGrid();

			switch( pParameters->Get_Parameter("METHOD")->asInt() )
			{
			case  0:	// Increase
				pParameters->Get_Parameter("INC_MIN")->Set_Value(pGrid->Get_Min());
				pParameters->Get_Parameter("INC_MAX")->Set_Value(pGrid->Get_Max());
				break;

			case  1:	// Decrease
				pParameters->Get_Parameter("DEC_MIN")->Set_Value(pGrid->Get_Min());
				pParameters->Get_Parameter("DEC_MAX")->Set_Value(pGrid->Get_Max());
				break;

			default:	// Increase and Decrease
				pParameters->Get_Parameter("INC_MIN")->Set_Value(pGrid->Get_Min());
				pParameters->Get_Parameter("INC_MAX")->Set_Value(pGrid->Get_Min() + 0.3 * pGrid->Get_Range());
				pParameters->Get_Parameter("DEC_MIN")->Set_Value(pGrid->Get_Max() - 0.3 * pGrid->Get_Range());
				pParameters->Get_Parameter("DEC_MAX")->Set_Value(pGrid->Get_Max());
				break;
			}
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CFuzzify::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("INCREASE", pParameter->asInt() != 1);
		pParameters->Set_Enabled("DECREASE", pParameter->asInt() != 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFuzzify::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pInput	= Parameters("INPUT" )->asGrid();
	CSG_Grid	*pFuzzy	= Parameters("OUTPUT")->asGrid();

	pFuzzy->Set_Name(CSG_String::Format("%s [%s]", pInput->Get_Name(), _TL("Fuzzified")));

	//-----------------------------------------------------
	bool	bInvert	= Parameters("INVERT")->asBool();

	double	IncMin	= Parameters("INC_MIN")->asDouble();
	double	IncMax	= Parameters("INC_MAX")->asDouble();
	double	DecMin	= Parameters("DEC_MIN")->asDouble();
	double	DecMax	= Parameters("DEC_MAX")->asDouble();

	switch( Parameters("METHOD")->asInt() )
	{
	case  0:	// Increase
		DecMin = DecMax = pInput->Get_Max();
		break;

	case  1:	// Decrease
		IncMin = IncMax = pInput->Get_Min();
		break;

	default:	// Increase and Decrease
		break;
	}

	if( IncMin > IncMax || DecMin > DecMax || IncMax > DecMin )
	{
		Error_Set(_TL("invalid control points"));

		return( false );
	}

	if( IncMax > DecMin )	// overlap !
	{
		IncMax	= (DecMin = DecMin + (IncMax - DecMin) / 2.0);
	}

	//-----------------------------------------------------
	int	Type	= Parameters("TRANSITION")->asInt();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pInput->is_InGrid(x, y) )
			{
				double	Value	= pInput->asDouble(x, y);

				if( Value <= IncMin || Value >= DecMax )
				{
					Value	= 0.0;
				}
				else if( Value >= IncMax && Value <= DecMin )
				{
					Value	= 1.0;
				}
				else
				{
					double	dX, dW;

					if( Value < IncMax )
					{
						dX	= Value  - IncMin;
						dW	= IncMax - IncMin;
					}
					else
					{
						dX	= DecMax - Value ;
						dW	= DecMax - DecMin;
					}

					switch( Type )
					{
					default: Value = dX / dW;									break;
					case  1: Value = pow(sin(dX / dW * M_PI_090), 2.0);			break;
					case  2: Value = 1.0 / (1.0 + pow((dW - dX) / dW, 2.0));	break;
					}
				}

				pFuzzy->Set_Value(x, y, bInvert ? 1.0 - Value : Value);
			}
			else
			{
				pFuzzy->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
