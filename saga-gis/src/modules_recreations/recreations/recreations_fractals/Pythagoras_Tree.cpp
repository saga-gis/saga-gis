/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Pythagoras_Tree.cpp                  //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "Pythagoras_Tree.h"
#include <time.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPythagoras_Tree::CPythagoras_Tree(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Pythagoras' Tree"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW("Pythagoras' Tree."));


	//-----------------------------------------------------
	// 2. Grids...

	pNode	= Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Pythagoras' Tree"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		pNode	, "TYPE"		, _TL("Shape Type"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Lines"),
			_TL("Polygons")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "ANGLE"		, _TL("Angle [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 55.0, 0.0, true, 90.0, true
	);

	Parameters.Add_Value(
		NULL	, "MINSIZE"		, _TL("Minimum Size [%]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 2.0, 0.001, true, 100.0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Fixed angle"),
			_TL("Constant variation range"),
			_TL("Low variation for low sizes"),
			_TL("High variation for low sizes")
		), 0
	);

	Parameters.Add_Range(
		pNode	, "VARRANGE"	, _TL("Constant Variation Range [Degree]"),
		_TL(""),
		0.0, 90.0, 0.0, true, 90.0, true
	);
}

//---------------------------------------------------------
CPythagoras_Tree::~CPythagoras_Tree(void)
{}


///////////////////////////////////////////////////////////
//														 //
//	Run													 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPythagoras_Tree::On_Execute(void)
{
	TSG_Point	pt_A, pt_B;

	pShapes		= Parameters("RESULT")	->asShapes();
	pShapes->Create(Parameters("TYPE")->asInt() == 0 ? SHAPE_TYPE_Line : SHAPE_TYPE_Polygon, _TL("Pythagoras' Tree"));
	pShapes->Add_Field(_TL("Iteration")	, SG_DATATYPE_Int);
	pShapes->Add_Field(_TL("Size")		, SG_DATATYPE_Double);

	Min_Size	= Parameters("MINSIZE")	->asDouble() / 100.0;

	switch( (Method = Parameters("METHOD")->asInt()) )
	{
	case 0:	// Standard...
		sin_Angle	= sin(Parameters("ANGLE")->asDouble() * M_DEG_TO_RAD);
		cos_Angle	= cos(Parameters("ANGLE")->asDouble() * M_DEG_TO_RAD);
		break;

	case 1:	// Variation Range...
		var_Min		= Parameters("VARRANGE")->asRange()->Get_LoVal() * M_DEG_TO_RAD;
		var_Range	= Parameters("VARRANGE")->asRange()->Get_HiVal() * M_DEG_TO_RAD;
		var_Range	= (var_Range - var_Min) / (double)RAND_MAX;
		break;
	}

	srand((unsigned)time(NULL));

	Iteration	= 0;

	pt_A.x		= 0.0;
	pt_B.x		= 1.0;
	pt_A.y		= pt_B.y	= 0.0;

	Set_Quadrat(pt_A, pt_B);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPythagoras_Tree::Set_Quadrat(TSG_Point pt_A0, TSG_Point pt_B0)
{
	double		dx, dy;
	TSG_Point	pt_A1, pt_B1;

	if( Process_Get_Okay(false) )
	{
		Iteration++;

		dx		= pt_B0.x - pt_A0.x;
		dy		= pt_B0.y - pt_A0.y;
		Size	= sqrt(dx*dx + dy*dy);

		pt_A1.x	= pt_A0.x - dy;
		pt_A1.y	= pt_A0.y + dx;

		pt_B1.x	= pt_B0.x - dy;
		pt_B1.y	= pt_B0.y + dx;

		Add_Shape(pt_A0, pt_B0, pt_B1, pt_A1);

		if( Size > Min_Size )
		{
			switch( Method )
			{
			case 1:	// Variation Range...
				var_Angle	= var_Min + (double)rand() * var_Range;
				sin_Angle	= sin(var_Angle);
				cos_Angle	= cos(var_Angle);
				break;

			case 2:	// High Variation for low sizes...
				var_Range	= Size * M_PI_090;
				var_Angle	= (M_PI_045 - var_Range / 2.0) + (double)rand() * var_Range / (double)RAND_MAX;
				sin_Angle	= sin(var_Angle);
				cos_Angle	= cos(var_Angle);
				break;

			case 3:	// Low Variation for low sizes...
				var_Range	= (1.0 - Size) * M_PI_090;
				var_Angle	= (M_PI_045 - var_Range / 2.0) + (double)rand() * var_Range / (double)RAND_MAX;
				sin_Angle	= sin(var_Angle);
				cos_Angle	= cos(var_Angle);
				break;
			}

			pt_A0.x	= pt_A1.x + cos_Angle * (cos_Angle * dx - sin_Angle * dy);
			pt_A0.y	= pt_A1.y + cos_Angle * (sin_Angle * dx + cos_Angle * dy);

			Add_Shape(pt_A1, pt_B1, pt_A0);

			Set_Quadrat(pt_A1, pt_A0);
			Set_Quadrat(pt_A0, pt_B1);
		}

		Iteration--;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPythagoras_Tree::Add_Shape(TSG_Point pt_A, TSG_Point pt_B, TSG_Point pt_C, TSG_Point pt_D)
{
	CSG_Shape	*pShape;

	pShape	= pShapes->Add_Shape();
	pShape->Set_Value(0, Iteration);
	pShape->Set_Value(1, Size);

	pShape->Add_Point(pt_A);
	pShape->Add_Point(pt_B);
	pShape->Add_Point(pt_C);
	pShape->Add_Point(pt_D);

	if( pShapes->Get_Type() == SHAPE_TYPE_Line )
	{
		pShape->Add_Point(pt_A);
	}
}

//---------------------------------------------------------
void CPythagoras_Tree::Add_Shape(TSG_Point pt_A, TSG_Point pt_B, TSG_Point pt_C)
{
	CSG_Shape	*pShape;

	pShape	= pShapes->Add_Shape();
	pShape->Set_Value(0, Iteration);
	pShape->Set_Value(1, Size);

	pShape->Add_Point(pt_A);
	pShape->Add_Point(pt_B);
	pShape->Add_Point(pt_C);

	if( pShapes->Get_Type() == SHAPE_TYPE_Line )
	{
		pShape->Add_Point(pt_A);
	}
}
