
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Plotter.cpp                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Plotter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Plotter::CGrid_Plotter(void)
{
	Set_Name	(_TL("Function Plotter"));

	Set_Author	("A.Ringeler (c) 2003");

	CSG_String	s(_TW(
		"Generate a grid based on a functional expression. "
		"The function interpreter uses an formula expression "
		"parser that offers the following operators:\n"
	));

	s	+= CSG_Formula::Get_Help_Operators(true);

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_String("",
		"FORMULA"	, _TL("Formula"),
		_TL(""),
		"sin(x*x + y*y)"
	);

	Parameters.Add_Range("FORMULA",
		"X_RANGE"	, _TL("X Range"),
		_TL(""),
		0.0, 10.0
	);

	Parameters.Add_Range("FORMULA",
		"Y_RANGE"	, _TL("Y Range"),
		_TL(""),
		0., 10.
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");

	m_Grid_Target.Add_Grid("FUNCTION", _TL("Function"), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Plotter::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Plotter::On_Execute(void)
{
	CSG_Formula	Formula;
	
	if( !Formula.Set_Formula(Parameters("FORMULA")->asString()) )
	{
		CSG_String	Message;

		if( !Formula.Get_Error(Message) )
		{
			Message	= _TL("unknown errror parsing formula");
		}

		Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pFunction	= m_Grid_Target.Get_Grid("FUNCTION");

	if( !pFunction )
	{
		Error_Set(_TL("could not create target grid"));

		return( false );
	}

	//-----------------------------------------------------
	double xMin		= Parameters("X_RANGE.MIN")->asDouble();
	double xRange	= Parameters("X_RANGE.MAX")->asDouble() - xMin;

	double yMin		= Parameters("Y_RANGE.MIN")->asDouble();
	double yRange	= Parameters("Y_RANGE.MAX")->asDouble() - yMin;

	//-----------------------------------------------------
	for(int y=0; y<pFunction->Get_NY() && Set_Progress(y); y++)
	{
		Formula.Set_Variable('y', yMin + yRange * (y / (double)pFunction->Get_NY()));

		#pragma omp parallel for
		for(int x=0; x<pFunction->Get_NX(); x++)
		{
			pFunction->Set_Value(x, y, Formula.Get_Value(xMin + xRange * (x / (double)pFunction->Get_NX())));
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
