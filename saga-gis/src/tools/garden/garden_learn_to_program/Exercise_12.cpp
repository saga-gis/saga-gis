/**********************************************************
 * Version $Id: Exercise_12.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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

	Set_Name	(_TL("12: First steps with shapes"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Copy a shapes layer and move it to a new position.\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Shapes(
		NULL	, "INPUT"	, _TL("Input"),
		_TL("This must be your input data of type shapes."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type shapes."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DX"		, _TL("Translation: X"),
		_TL(""),
		PARAMETER_TYPE_Double,
		10.0
	);

	Parameters.Add_Value(
		NULL	, "DY"		, _TL("Translation: Y"),
		_TL(""),
		PARAMETER_TYPE_Double,
		10.0
	);
}

//---------------------------------------------------------
CExercise_12::~CExercise_12(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_12::On_Execute(void)
{
	int			iShape, iPart, iPoint;
	double		dx, dy;
	TSG_Point	Point;
	CSG_Shapes	*pShapes_A, *pShapes_B;
	CSG_Shape	*pShape_A, *pShape_B;


	//-----------------------------------------------------
	// Get parameter settings...

	pShapes_A	= Parameters("INPUT")	->asShapes();
	pShapes_B	= Parameters("OUTPUT")	->asShapes();

	dx			= Parameters("DX")		->asDouble();
	dy			= Parameters("DY")		->asDouble();

	pShapes_B->Create(pShapes_A->Get_Type(), _TL("Translation"), pShapes_A);


	//-----------------------------------------------------
	// Copy shapes layer A to B and translate each point's position...

	for(iShape=0; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
	{
		pShape_A	= pShapes_A->Get_Shape(iShape);
		pShape_B	= pShapes_B->Add_Shape(pShape_A, SHAPE_COPY_ATTR);

		for(iPart=0; iPart<pShape_A->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape_A->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pShape_A->Get_Point(iPoint, iPart);

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
