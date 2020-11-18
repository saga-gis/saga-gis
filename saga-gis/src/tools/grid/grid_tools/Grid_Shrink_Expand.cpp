
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
//                Grid_Shrink_Expand.cpp                 //
//                                                       //
//                 Copyright (C) 2011 by                 //
//            Volker Wichmann and Olaf Conrad            //
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
#include "Grid_Shrink_Expand.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	EXPAND_MIN		= 0,
	EXPAND_MAX,
	EXPAND_MEAN,
	EXPAND_MAJORITY
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Shrink_Expand::CGrid_Shrink_Expand(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Shrink and Expand"));

	Set_Author		("V.Wichmann & O.Conrad (c) 2011");

	Set_Description	(_TW(
		"The tool allows one to shrink or expand regions with valid data by a certain distance (defined by the kernel radius). "
		"Shrinking just invalidates cells with valid data at the border to No Data regions, expanding sets No Data "
		"cells along the border of regions with valid data to a new valid value. The tool provides several options "
		"how to calculate this new value: minimum, maximum, mean or majority of the valid cells within the kernel.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL("The input grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Result Grid"),
		_TL("The output grid."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"OPERATION"	, _TL("Operation"),
		_TL("Choose the type of operation."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("shrink"           ),
			_TL("expand"           ),
			_TL("shrink and expand"),
			_TL("expand and shrink")
		), 3
	);

	Parameters.Add_Choice("",
		"CIRCLE"	, _TL("Search Mode"),
		_TL("Choose the shape of the kernel."),
		CSG_String::Format("%s|%s",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Int("",
		"RADIUS"	, _TL("Radius"),
		_TL("The kernel radius [cells]."),
		1, 1, true
	);

	Parameters.Add_Choice("",
		"EXPAND"	, _TL("Method"),
		_TL("Choose how to fill no-data cells."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("minimum" ),
			_TL("maximum" ),
			_TL("mean"    ),
			_TL("majority")
		), 3
	);

	Parameters.Add_Bool("EXPAND",
		"KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL("If false, mean value expansion results will be stored with floating point precision."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Shrink_Expand::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("OPERATION") )
	{
		pParameters->Set_Enabled("EXPAND", pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier("EXPAND") )
	{
		pParameters->Set_Enabled("KEEP_TYPE", pParameter->asInt() == 2);	// mean?!
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::On_Execute(void)
{
	if( !m_Kernel.Set_Radius(Parameters("RADIUS")->asInt(), Parameters("CIRCLE")->asInt() == 0) )
	{
		Error_Set(_TL("could not initialize search kernel"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pInput  = Parameters("INPUT" )->asGrid(), Input;
	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	if( !pResult || pResult == pInput )
	{
		Input.Create(*pInput);
		pResult = pInput;
		pInput  = &Input;
	}

	//-----------------------------------------------------
	TSG_Data_Type	Type	= pInput->Get_Type();

	if( Parameters("OPERATION")->asInt ()  > 0     // expands
	&&  Parameters("EXPAND"   )->asInt () == 2     // mean
	&&  Parameters("KEEP_TYPE")->asBool() == false // keep data type?
	&&  Type != SG_DATATYPE_Float && Type != SG_DATATYPE_Double )
	{
		Type	= SG_DATATYPE_Float;
	}

	if( pResult->Get_Type() != Type )
	{
		pResult->Create(Get_System(), Type);
		pResult->Set_Name(pInput->Get_Name());
	}

	//-----------------------------------------------------
	switch( Parameters("OPERATION")->asInt() )
	{
	case  0:	// shrink
		{
			Do_Shrink(pInput, pResult);
		}
		break;

	case  1:	// expand
		{
			Do_Expand(pInput, pResult);
		}
		break;

	case  2:	// shrink and expand
		{
			CSG_Grid TMP(pResult);

			Do_Shrink(pInput,    &TMP);
			Do_Expand(&TMP  , pResult);
		}
		break;

	default:	// expand and shrink
		{
			CSG_Grid TMP(pResult);

			Do_Expand(pInput,    &TMP);
			Do_Shrink(&TMP  , pResult);
		}
		break;
	}

	//-----------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);
	}
	else switch( Parameters("OPERATION")->asInt() )
	{
	case  0: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Shrink"           )); break;
	case  1: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Expand"           )); break;
	case  2: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Shrink and Expand")); break;
	default: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Expand and Shrink")); break;
	}

	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Do_Shrink(CSG_Grid *pInput, CSG_Grid *pResult)
{
	m_pInput	= pInput;

	Process_Set_Text("%s...", _TL("Shrink"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bShrink = m_pInput->is_NoData(x, y);

			for(int i=0; !bShrink && i<m_Kernel.Get_Count(); i++)
			{
				int	ix	= m_Kernel.Get_X(i, x);
				int	iy	= m_Kernel.Get_Y(i, y);

				if( is_InGrid(ix, iy) && m_pInput->is_NoData(ix, iy) )
				{
					bShrink	= true;
				}
			}

			if( bShrink )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				pResult->Set_Value(x, y, m_pInput->asDouble(x, y));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Do_Expand(CSG_Grid *pInput, CSG_Grid *pResult)
{
	m_pInput	= pInput;

	Process_Set_Text("%s...", _TL("Expand"));

	int		Method	= Parameters("EXPAND")->asInt();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Value;

			if( Get_Expand_Value(x, y, Method, Value) )
			{
				pResult->Set_Value(x, y, Value);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Get_Expand_Value(int x, int y, int Method, double &Value)
{
	if( !m_pInput->is_NoData(x, y) )
	{
		Value	= m_pInput->asDouble(x, y);

		return( true );
	}

	//-----------------------------------------------------
	if( Method == EXPAND_MAJORITY )
	{
		CSG_Unique_Number_Statistics	Majority;

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) )
			{
				Majority.Add_Value(m_pInput->asDouble(ix, iy));
			}
		}

		return( Majority.Get_Majority(Value) );
	}

	//-----------------------------------------------------
	{
		CSG_Simple_Statistics	Statistics;

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) )
			{
				Statistics.Add_Value(m_pInput->asDouble(ix, iy));
			}
		}

		if( Statistics.Get_Count() > 0 )
		{
			switch( Method )
			{
			default:
			case EXPAND_MEAN:	Value	= Statistics.Get_Mean   ();	break;
			case EXPAND_MIN :	Value	= Statistics.Get_Minimum();	break;
			case EXPAND_MAX :	Value	= Statistics.Get_Maximum();	break;
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
