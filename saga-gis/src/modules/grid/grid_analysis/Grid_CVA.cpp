/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Grid_CVA.cpp
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
#include "Grid_CVA.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_CVA::CGrid_CVA(void)
{
	Set_Name		(_TL("Change Vector Analysis"));

	Set_Author		(SG_T("V.Olaya (c) 2004, O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"This module performs a change vector analysis (CVA) for the given input features. "
		"Input features are supplied as grid lists for initial and final state. "
		"In both lists features have to be given in the same order. "
		"Distance is measured as Euclidean distance in features space. When analyzing two features "
		"direction is calculated as angle (radians) by default. Otherwise direction is coded as "
		"the quadrant it points to in terms of feature space. "
	));

	Parameters.Add_Grid_List(
		NULL	, "A"		, _TL("Initial State"), 
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "B"		, _TL("Final State"),
		_TL(""), 
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid(
		NULL	, "DIST"	, _TL("Distance"), 
		_TL(""), 
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DIR"		, _TL("Direction"), 
		_TL(""), 
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "ANGLE"	, _TL("Angle Calculation"), 
		_TL("angle calculation is only available when exact two features are compared"), 
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "C_OUT"	, _TL("Output of Change Vector"), 
		_TL(""), 
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid_List(
		NULL	, "C"		, _TL("Change Vector"),
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_CVA::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("A"))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("B")) )
	{
		pParameters->Get_Parameter("ANGLE")->Set_Enabled(
				pParameters->Get_Parameter("A")->asInt() == 2
			&&	pParameters->Get_Parameter("B")->asInt() == 2
		);
	}

	//-----------------------------------------------------
	return( 1 );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_CVA::On_Execute(void)
{	
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pA	= Parameters("A")->asGridList();
	CSG_Parameter_Grid_List	*pB	= Parameters("B")->asGridList();
	CSG_Parameter_Grid_List	*pC	= Parameters("C")->asGridList();

	if( pA->Get_Count() != pB->Get_Count() )
	{
		Error_Set(_TL("number of initial and final state grids differs"));

		return( false );
	}

	if( pA->Get_Count() == 0 )
	{
		Error_Set(_TL("no grids in list"));

		return( false );
	}

	//-----------------------------------------------------
	int	n	= pA->Get_Count();

	bool		bAngle	= Parameters("ANGLE")->asBool() && n == 2;
	bool		bC_Out	= Parameters("C_OUT")->asBool();

	CSG_Grid	*pDist	= Parameters("DIST")->asGrid();
	CSG_Grid	*pDir	= Parameters("DIR" )->asGrid();

	//-----------------------------------------------------
	pC->Del_Items();

	if( bC_Out )
	{
		for(int i=0; i<n; i++)
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(*Get_System());
			pGrid->Set_Name(CSG_String::Format(SG_T("%s %01d"), _TL("Change Vector"), i + 1));
			pC->Add_Item(pGrid);
		}
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT;
	CSG_Colors		Colors;

	Colors.Set_Count(100);
	Colors.Set_Ramp(SG_GET_RGB(255, 255, 255), SG_GET_RGB(  0, 127, 127), 0, Colors.Get_Count() / 2);
	Colors.Set_Ramp(SG_GET_RGB(  0, 127, 127), SG_GET_RGB(255,   0,   0),    Colors.Get_Count() / 2, Colors.Get_Count());
	DataObject_Set_Colors(pDist, Colors);

	if( (pLUT = DataObject_Get_Parameter(pDir, "LUT")) == NULL || pLUT->asTable() == NULL || bAngle )
	{
		Colors.Set_Default(100);
		Colors.Set_Ramp_Brighness(255,   0, 0, Colors.Get_Count() / 2);
		Colors.Set_Ramp_Brighness(  0, 255,    Colors.Get_Count() / 2, Colors.Get_Count());
		DataObject_Set_Colors(pDir, Colors);

		DataObject_Set_Parameter(pDir, "COLORS_TYPE", 2);
	}
	else
	{
		pLUT->asTable()->Del_Records();

		for(int i=0, nClasses=(int)pow(2.0, n); i<nClasses; i++)
		{
			CSG_String	s;

			for(int j=0; j<n; j++)
			{
				s	+= i & (int)pow(2.0, j) ? '+' : '-';
			}

			CSG_Table_Record	*pClass	= pLUT->asTable()->Add_Record();
			pClass->Set_Value(1, s);
			pClass->Set_Value(3, i);
			pClass->Set_Value(4, i);
		}

		Colors.Set_Count(pLUT->asTable()->Get_Count());
		Colors.Random();

		for(int c=0; c<pLUT->asTable()->Get_Count(); c++)
		{
			pLUT->asTable()->Get_Record(c)->Set_Value(0, Colors.Get_Color(c));
		}

		DataObject_Set_Parameter(pDir, pLUT);
		DataObject_Set_Parameter(pDir, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool		bOkay;
			int			i, j;
			double		d;
			CSG_Vector	v(n);

			for(i=0, bOkay=true; i<n && bOkay; i++)
			{
				if( pA->asGrid(i)->is_NoData(x, y) || pB->asGrid(i)->is_NoData(x, y) )
				{
					bOkay	= false;
				}
				else
				{
					v[i]	= pB->asGrid(i)->asDouble(x, y) - pA->asGrid(i)->asDouble(x, y);
				}
			}

			//---------------------------------------------
			if( bOkay )
			{
				if( bAngle )
				{
					d	= atan2(v[0], v[1]);
				}
				else for(i=0, j=1, d=0.0; i<n; i++, j*=2)
				{
					if( v[i] >= 0.0 )
					{
						d	+= j;
					}
				}

				pDist->Set_Value(x, y, v.Get_Length());
				pDir ->Set_Value(x, y, d);

				for(i=0; i<n && bC_Out; i++)
				{
					pC->asGrid(i)->Set_Value(x, y, v[i]);
				}
			}

			//---------------------------------------------
			else
			{
				pDist->Set_NoData(x, y);
				pDir ->Set_NoData(x, y);

				for(i=0; i<n && bC_Out; i++)
				{
					pC->asGrid(i)->Set_NoData(x, y);
				}
			}
        }
    }

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
