
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    Lectures_Shapes                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_12.cpp                     //
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
#include "Exercise_12.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_12::CExercise_12(void)
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name		(_TL("12: First steps with shapes"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Copy a shapes layer and move it to a new position."
	));


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Shapes("",
		"INPUT"	, _TL("Input"),
		_TL("This must be your input data of type shapes."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"OUTPUT", _TL("Output"),
		_TL("This will contain your output data of type shapes."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("", "DX", _TL("Translation: X"), _TL(""), 10.);
	Parameters.Add_Double("", "DY", _TL("Translation: Y"), _TL(""), 10.);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_12::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	CSG_Shapes	*pShapes_A	= Parameters("INPUT" )->asShapes();
	CSG_Shapes	*pShapes_B	= Parameters("OUTPUT")->asShapes();

	double	dx	= Parameters("DX")->asDouble();
	double	dy	= Parameters("DY")->asDouble();

	pShapes_B->Create(pShapes_A->Get_Type(), _TL("Translation"), pShapes_A);


	//-----------------------------------------------------
	// Copy shapes layer A to B and translate each point's position...

	for(int iShape=0; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape_A	= pShapes_A->Get_Shape(iShape);
		CSG_Shape	*pShape_B	= pShapes_B->Add_Shape(pShape_A, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pShape_A->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape_A->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	Point	= pShape_A->Get_Point(iPoint, iPart);

				Point.x	+= dx;	// perform the translation before
				Point.y	+= dy;	// you add the point to the new shape...

				pShape_B->Add_Point(Point, iPart);
			}
		}
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
