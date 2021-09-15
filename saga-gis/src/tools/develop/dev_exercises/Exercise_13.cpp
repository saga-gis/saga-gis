
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
#include "Exercise_13.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_13::CExercise_13(void)
{
	Set_Name		(_TL("13: Reprojecting a shapes layer"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Copy a shapes layer and move it to a new position."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems für Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"INPUT"		, _TL("Input"),
		_TL("This must be your input data of type shapes."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type shapes."),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// Transformation parameters...

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			_TL("Translation"),
			_TL("Scaling"),
			_TL("Shearing"),
			_TL("Rotation"),
			_TL("Combination A"),
			_TL("Combination B")
		)
	);

	Parameters.Add_Node("", "TRANS", _TL("Translation"  ), _TL(""));
	Parameters.Add_Double("TRANS", "TRANS_X", "X", _TL(""), 10.);
	Parameters.Add_Double("TRANS", "TRANS_Y", "Y", _TL(""), 10.);

	Parameters.Add_Node("", "SCALE", _TL("Scale factors"), _TL(""));
	Parameters.Add_Double("SCALE", "SCALE_X", "X", _TL(""), 10.);
	Parameters.Add_Double("SCALE", "SCALE_Y", "Y", _TL(""), 10.);

	Parameters.Add_Node("", "SHEAR", _TL("Shear factors"), _TL(""));
	Parameters.Add_Double("SHEAR", "SHEAR_X", "X", _TL(""), 0.5);
	Parameters.Add_Double("SHEAR", "SHEAR_Y", "Y", _TL(""), 0.5);

	Parameters.Add_Double("",
		"ROTAT"	, _TL("Rotation angle [Degree]"),
		_TL(""),
		45.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CExercise_13::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("TRANS", pParameter->asInt() == 0 || pParameter->asInt() >= 4);
		pParameters->Set_Enabled("SCALE", pParameter->asInt() == 1 || pParameter->asInt() >= 4);
		pParameters->Set_Enabled("SHEAR", pParameter->asInt() == 2 || pParameter->asInt() >= 4);
		pParameters->Set_Enabled("ROTAT", pParameter->asInt() == 3 || pParameter->asInt() >= 4);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_13::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	CSG_Shapes	*pShapes_A	= Parameters("INPUT" )->asShapes();
	CSG_Shapes	*pShapes_B	= Parameters("OUTPUT")->asShapes();

	pShapes_B->Create(pShapes_A->Get_Type(), _TL("Transformation"), pShapes_A);

	double	trn_x = Parameters("TRANS_X")->asDouble();
	double	trn_y = Parameters("TRANS_Y")->asDouble();

	double	scl_x = Parameters("SCALE_X")->asDouble();
	double	scl_y = Parameters("SCALE_Y")->asDouble();

	double	shr_x = Parameters("SHEAR_X")->asDouble();
	double	shr_y = Parameters("SHEAR_Y")->asDouble();

	double	rot   = Parameters("ROTAT"  )->asDouble() * M_DEG_TO_RAD;


	//-----------------------------------------------------
	// Initialise transformation matrices...

	#define SET_MATRIX(M, a1, a2, a3, b1, b2, b3, c1, c2, c3)	{\
		M[0][0]=a1; M[1][0]=b1; M[2][0]=c1;\
		M[0][1]=a2; M[1][1]=b2; M[2][1]=c2;\
		M[0][2]=a3; M[1][2]=b3; M[2][2]=c3;\
	}

	double	m_trn[3][3], m_scl[3][3], m_shr[3][3], m_rot[3][3], m[3][3];

	// Translation...
	SET_MATRIX(m_trn,
		1.0      , 0.0      , trn_x,
		0.0      , 1.0      , trn_y,
		0.0      , 0.0      , 1.0
	);

	// Scaling...
	SET_MATRIX(m_scl,
		scl_x    , 0.0      , 0.0,
		0.0      , scl_y    , 0.0,
		0.0      , 0.0      , 1.0
	);

	// Shearing...
	SET_MATRIX(m_shr,
		1.0      , shr_x    , 0.0,
		shr_y    , 1.0      , 0.0,
		0.0      , 0.0      , 1.0
	);

	// Rotation...
	SET_MATRIX(m_rot,
		 cos(rot), sin(rot), 0.0,
		-sin(rot), cos(rot), 0.0,
		 0.0     , 0.0     , 1.0
	);

	// Initialise m as matrix, that does nothing...
	SET_MATRIX(m,
		1.0      , 0.0      , 0.0,
		0.0      , 1.0      , 0.0,
		0.0      , 0.0      , 1.0
	);


	//-----------------------------------------------------
	// Set final transformation matrix m...

	switch( Parameters("METHOD" )->asInt() )
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CExercise_13::Transformation(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B, double m[3][3])
{
	//-----------------------------------------------------
	// Copy shapes layer A to B and reproject each point's position using matrix m...

	for(int iShape=0; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
	{
		CSG_Shape *pShape_A	= pShapes_A->Get_Shape(iShape);
		CSG_Shape *pShape_B	= pShapes_B->Add_Shape(pShape_A, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pShape_A->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape_A->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	b, a	= pShape_A->Get_Point(iPoint, iPart);

				b.x = m[0][0] * a.x + m[0][1] * a.y + m[0][2];
				b.y = m[1][0] * a.x + m[1][1] * a.y + m[1][2];

				pShape_B->Add_Point(b, iPart);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CExercise_13::Multiply_Matrices(double A[3][3], double B[3][3])
{
	double	C[3][3];

	//-----------------------------------------------------
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			C[i][j]	= 0.;

			for(int k=0; k<3; k++)
			{				
				C[i][j]	+= A[i][k] * B[k][j];
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			A[i][j]	= C[i][j];
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
