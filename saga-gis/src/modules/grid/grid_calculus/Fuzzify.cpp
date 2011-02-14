/*******************************************************************************
    Fuzzify.cpp
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

	Set_Author		(SG_T("Victor Olaya (c) 2004"));

	Set_Description	(_TW(
		"Translates grid values into fuzzy set membership as preparation for fuzzy logic analysis."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Fuzzified Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	CSG_Parameter	*pNode	= Parameters.Add_Node(
		NULL	, "NODE_PTS", _TL("Control Points"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "A"		, _TL("A"),
		_TL("Values lower than A will be set to 0."),
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Value(
		pNode	, "B"		, _TL("B"),
		_TL("Values between A and B increase from 0 to 1, values between B and C will be set to 1."),
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Value(
		pNode	, "C"		, _TL("C"),
		_TL("Values between B and C will be set to 1, values between C and D decrease from 1 to 0."),
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Value(
		pNode	, "D"		, _TL("D"),
		_TL("Values greater than D will be set to 0."),
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"	, _TL("Membership Function Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("linear"),
			_TL("sigmoidal"),
			_TL("j-shaped")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "AUTOFIT"	, _TL("Adjust to Grid"),
		_TL("Automatically adjust control points to grid's data range"),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFuzzify::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("INPUT"))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AUTOFIT")) )
	{
		if( pParameters->Get_Parameter("AUTOFIT")->asBool() )
		{
			CSG_Grid	*pGrid	= pParameters->Get_Parameter("INPUT")->asGrid();

			pParameters->Get_Parameter("A")->Set_Value(pGrid->Get_ZMin());
			pParameters->Get_Parameter("B")->Set_Value(pGrid->Get_ZMin() + 0.3 * pGrid->Get_ZRange());
			pParameters->Get_Parameter("C")->Set_Value(pGrid->Get_ZMax() - 0.3 * pGrid->Get_ZRange());
			pParameters->Get_Parameter("D")->Set_Value(pGrid->Get_ZMax());
		}
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("A")) )
	{
		if( pParameter->asDouble() > pParameters->Get_Parameter("B")->asDouble() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("B")->asDouble());
		}
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("B")) )
	{
		if( pParameter->asDouble() < pParameters->Get_Parameter("A")->asDouble() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("A")->asDouble());
		}
		else if( pParameter->asDouble() > pParameters->Get_Parameter("C")->asDouble() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("C")->asDouble());
		}
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("C")) )
	{
		if( pParameter->asDouble() < pParameters->Get_Parameter("B")->asDouble() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("B")->asDouble());
		}
		else if( pParameter->asDouble() > pParameters->Get_Parameter("D")->asDouble() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("D")->asDouble());
		}
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("D")) )
	{
		if( pParameter->asDouble() < pParameters->Get_Parameter("C")->asDouble() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("C")->asDouble());
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFuzzify::On_Execute(void)
{
	int			Type;
	double		A, B, C, D;
	CSG_Grid	*pInput, *pOutput;

	//-----------------------------------------------------
	pInput	= Parameters("INPUT")	->asGrid();
	pOutput	= Parameters("OUTPUT")	->asGrid();
	Type	= Parameters("TYPE")	->asInt();
	A		= Parameters("A")		->asDouble();
	B		= Parameters("B")		->asDouble();
	C		= Parameters("C")		->asDouble();
	D		= Parameters("D")		->asDouble();

	if( A > B || B > C || C > D )
	{
		Error_Set(_TL("invalid control points"));

		return( false );
	}

	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("Fuzzified")));

	DataObject_Set_Colors(pOutput, 100, SG_COLORS_BLACK_WHITE);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pInput->is_InGrid(x, y) )
			{
				double	Value	= pInput->asDouble(x, y);

				if( Value <= A || Value >= D )
				{
					Value	= 0.0;
				}
				else if( Value >= B && Value <= C )
				{
					Value	= 1.0;
				}
				else
				{
					double	dX, dW;

					if( Value < B )
					{
						dX	= Value - A;
						dW	= B     - A;
					}
					else
					{
						dX	= D - Value;
						dW	= D - C;
					}

					switch( Type )
					{
					case 0:	Value	= dX / dW;									break;
					case 1:	Value	= pow(sin(dX / dW * M_PI_090), 2.0);		break;
					case 2:	Value	= 1.0 / (1.0 + pow((dW - dX) / dW, 2.0));	break;
					}
				}

				pOutput->Set_Value(x, y, Value);
			}
			else
			{
				pOutput->Set_NoData(x, y);
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
