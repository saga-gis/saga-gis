
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    pj_Georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Georef_Shapes.cpp                   //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//    e-mail:     oconrad@gwdg.de                        //
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
#include "Georef_Shapes.h"
#include "Georef_Engine.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Shapes::CGeoref_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Georeferencing - Shapes"));
	Set_Author		(_TL("Copyrights (c) 2006 by Olaf Conrad"));
	Set_Description	(_TW(
		"Georeferencing of shapes layers. Either choose the attribute fields (x/y) "
		"with the projected coordinates for the reference points (origin) or supply a "
		"additional points layer with correspondend points in the target projection. "
		"\n"
		"This library uses the Minpack routines for solving the nonlinear equations and "
		"nonlinear least squares problem. You find minpack and more information "
		"about minpack at:\n"
		"  <a target=\"_blank\" href=\"http://www.netlib.org/minpack\">"
		"  http://www.netlib.org/minpack</a>\n"
		"\n"
		"or download the C source codes:\n"
		"  <a target=\"_blank\" href=\"http://www.netlib.org/minpack/cminpack.tar\">"
		"  http://www.netlib.org/minpack/cminpack.tar</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	CSG_Parameter	*pSource	= Parameters.Add_Shapes(
		NULL	, "REF_SOURCE"	, _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "REF_TARGET"	, _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field(
		pSource	, "XFIELD"		, _TL("x Position"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pSource	, "YFIELD"		, _TL("y Position"),
		_TL("")
	);
}

//---------------------------------------------------------
CGeoref_Shapes::~CGeoref_Shapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Shapes::On_Execute(void)
{
	int				iShape, iPart, iPoint, xField, yField;
	TSG_Point		Point;
	CSG_Shapes			*pShapes_A, *pShapes_B;
	CSG_Shape			*pShape_A, *pShape_B;
	CGeoref_Engine	Engine;

	//-----------------------------------------------------
	pShapes_A	= Parameters("REF_SOURCE")	->asShapes();
	pShapes_B	= Parameters("REF_TARGET")	->asShapes();
	xField		= Parameters("XFIELD")		->asInt();
	yField		= Parameters("YFIELD")		->asInt();

	//-----------------------------------------------------
	if( ( pShapes_B && Engine.Set_Engine(pShapes_A, pShapes_B))
	||	(!pShapes_B && Engine.Set_Engine(pShapes_A, xField, yField))	)
	{
		pShapes_A	= Parameters("INPUT")	->asShapes();
		pShapes_B	= Parameters("OUTPUT")	->asShapes();

		pShapes_B->Create(pShapes_A->Get_Type(), pShapes_A->Get_Name(), &pShapes_A->Get_Table());

		for(iShape=0; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
		{
			pShape_A	= pShapes_A->Get_Shape(iShape);
			pShape_B	= pShapes_B->Add_Shape(pShape_A->Get_Record());

			for(iPart=0; iPart<pShape_A->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape_A->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape_A->Get_Point(iPoint, iPart);

					if( Engine.Get_Converted(Point) )
					{
						pShape_B->Add_Point(Point);
					}
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
