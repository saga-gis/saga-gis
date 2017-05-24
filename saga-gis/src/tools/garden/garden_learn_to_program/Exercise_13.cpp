/**********************************************************
 * Version $Id: Exercise_13.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   Exercise_13.cpp                     //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Exercise_13.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_13::CExercise_13(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name	(_TL("13: Reprojecting a shapes layer"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Copy a shapes layer and move it to a new position.\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Shapes input and output...

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


	//-----------------------------------------------------
	// Transformation parameters...

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Translation"),
			_TL("Scaling"),
			_TL("Shearing"),
			_TL("Rotation"),
			_TL("Combination A"),
			_TL("Combination B")
		)
	);

	//-----------------------------------------------------
	// Transformation parameters...

	pNode	= Parameters.Add_Node(NULL, "NODE_TRANS", _TL("Translation"), _TL(""));
	Parameters.Add_Value(
		pNode	, "TRANS_X"	, SG_T("X"),
		_TL(""),
		PARAMETER_TYPE_Double,
		10.0
	);
	Parameters.Add_Value(
		pNode	, "TRANS_Y"	, SG_T("Y"),
		_TL(""),
		PARAMETER_TYPE_Double,
		10.0
	);

	pNode	= Parameters.Add_Node(NULL, "NODE_SCALE", _TL("Scale factors")	, _TL(""));
	Parameters.Add_Value(
		pNode	, "SCALE_X"	, SG_T("X"),
		_TL(""),
		PARAMETER_TYPE_Double,
		10.0
	);
	Parameters.Add_Value(
		pNode	, "SCALE_Y"	, SG_T("Y"),
		_TL(""),
		PARAMETER_TYPE_Double,
		10.0
	);

	pNode	= Parameters.Add_Node(NULL, "NODE_SHEAR", _TL("Shear factors")	, _TL(""));
	Parameters.Add_Value(
		pNode	, "SHEAR_X"	, SG_T("X"),
		_TL(""),
		PARAMETER_TYPE_Double,
		0.5
	);
	Parameters.Add_Value(
		pNode	, "SHEAR_Y"	, SG_T("Y"),
		_TL(""),
		PARAMETER_TYPE_Double,
		0.5
	);

	Parameters.Add_Value(
		NULL	, "ROTAT"	, _TL("Rotation angle [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double,
		45.0
	);
}

//---------------------------------------------------------
CExercise_13::~CExercise_13(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SET_MATRIX(M, a1, a2, a3, b1, b2, b3, c1, c2, c3)	M[0][0]=a1; M[1][0]=b1; M[2][0]=c1;\
															M[0][1]=a2; M[1][1]=b2; M[2][1]=c2;\
															M[0][2]=a3; M[1][2]=b3; M[2][2]=c3;

//---------------------------------------------------------
bool CExercise_13::On_Execute(void)
{
	int			Method;
	double		trn_x, trn_y, scl_x, scl_y, shr_x, shr_y, rot,
				m_trn[3][3], m_scl[3][3], m_shr[3][3], m_rot[3][3], m[3][3];
	CSG_Shapes		*pShapes_A, *pShapes_B;


	//-----------------------------------------------------
	// Get parameter settings...

	pShapes_A	= Parameters("INPUT")	->asShapes();
	pShapes_B	= Parameters("OUTPUT")	->asShapes();

	pShapes_B->Create(pShapes_A->Get_Type(), _TL("Transformation"), pShapes_A);

	Method		= Parameters("METHOD")	->asInt();

	trn_x		= Parameters("TRANS_X")	->asDouble();
	trn_y		= Parameters("TRANS_Y")	->asDouble();

	scl_x		= Parameters("SCALE_X")	->asDouble();
	scl_y		= Parameters("SCALE_Y")	->asDouble();

	shr_x		= Parameters("SHEAR_X")	->asDouble();
	shr_y		= Parameters("SHEAR_Y")	->asDouble();

	rot			= Parameters("ROTAT")	->asDouble() * M_DEG_TO_RAD;


	//-----------------------------------------------------
	// Initialise transformation matrices...

	// Translation...
	SET_MATRIX(m_trn,
		1.0			, 0.0		, trn_x	,
		0.0			, 1.0		, trn_y	,
		0.0			, 0.0		, 1.0	);

	// Scaling...
	SET_MATRIX(m_scl,
		scl_x		, 0.0		, 0.0	,
		0.0			, scl_y		, 0.0	,
		0.0			, 0.0		, 1.0	);

	// Shearing...
	SET_MATRIX(m_shr,
		1.0			, shr_x		, 0.0	,
		shr_y		, 1.0		, 0.0	,
		0.0			, 0.0		, 1.0	);

	// Rotation...
	SET_MATRIX(m_rot,
		 cos(rot)	, sin(rot)	, 0.0	,
		-sin(rot)	, cos(rot)	, 0.0	,
		 0.0		, 0.0		, 1.0	);

	// Initialise m as matrix, that does nothing...
	SET_MATRIX(m,
		1.0			, 0.0		, 0.0	,
		0.0			, 1.0		, 0.0	,
		0.0			, 0.0		, 1.0	);


	//-----------------------------------------------------
	// Set final transformation matrix m...

	switch( Method )
	{
	case 0:	// Translation...
		Multiply_Matrices(m, m_trn);
		break;

	case 1:	// Scaling...
		Multiply_Matrices(m, m_scl);
		break;

	case 2:	// Shearing...
		Multiply_Matrices(m, m_shr);
		break;

	case 3:	// Rotation...
		Multiply_Matrices(m, m_rot);
		break;

	case 4:	// Combination A...
		Multiply_Matrices(m, m_trn);
		Multiply_Matrices(m, m_scl);
		Multiply_Matrices(m, m_shr);
		Multiply_Matrices(m, m_rot);
		break;

	case 5:	// Combination B...
		Multiply_Matrices(m, m_rot);
		Multiply_Matrices(m, m_shr);
		Multiply_Matrices(m, m_scl);
		Multiply_Matrices(m, m_trn);
		break;
	}


	//-----------------------------------------------------
	// Execute transformation...

	Transformation(pShapes_A, pShapes_B, m);


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
void CExercise_13::Transformation(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B, double m[3][3])
{
	int			iShape, iPart, iPoint;
	double		x;
	TSG_Point	p;
	CSG_Shape		*pShape_A, *pShape_B;


	//-----------------------------------------------------
	// Copy shapes layer A to B and reproject each point's position using matrix m...

	for(iShape=0; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
	{
		pShape_A	= pShapes_A->Get_Shape(iShape);
		pShape_B	= pShapes_B->Add_Shape(pShape_A, SHAPE_COPY_ATTR);

		for(iPart=0; iPart<pShape_A->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape_A->Get_Point_Count(iPart); iPoint++)
			{
				p	= pShape_A->Get_Point(iPoint, iPart);

				x	= m[0][0] * p.x + m[0][1] * p.y + m[0][2];
				p.y	= m[1][0] * p.x + m[1][1] * p.y + m[1][2];
				p.x	= x;

				pShape_B->Add_Point(p, iPart);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CExercise_13::Multiply_Matrices(double A[3][3], double B[3][3])
{
	int		x, y, k;
	double	C[3][3];

	//-----------------------------------------------------
	for(x=0; x<3; x++)
	{
		for(y=0; y<3; y++)
		{
			C[x][y]	= 0.0;

			for(k=0; k<3; k++)
			{				
				C[x][y]	+= A[x][k] * B[k][y];
			}
		}
	}

	//-----------------------------------------------------
	for(x=0; x<3; x++)
	{
		for(y=0; y<3; y++)
		{
			A[x][y]	= C[x][y];
		}
	}
}
