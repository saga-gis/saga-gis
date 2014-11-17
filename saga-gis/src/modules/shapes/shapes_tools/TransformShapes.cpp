/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    TransformShapes.cpp
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

//---------------------------------------------------------
#include "TransformShapes.h"

//---------------------------------------------------------
CTransformShapes::CTransformShapes(void)
{
	Set_Name		(_TL("Transform Shapes"));
	Set_Author		(SG_T("(c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Use this module to move, rotate and/or scale shapes."
	));

	CSG_Parameter *pNode_0;

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "IN"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "OUT"		, _TL("Output"), 
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "MOVE"	, _TL("Move"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode_0	, "DX"		, _TL("dX"), 
		_TL("dX (Map Units)"), 
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode_0, "DY"		, _TL("dY"), 
		_TL("dY (Map Units)"), 
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode_0, "DZ"		, _TL("dZ"),
		_TL("dZ (Map Units)"),
		PARAMETER_TYPE_Double, 0.0
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "ROTATE"	,
		_TL("Rotate"),
		_TL(""));
	
	Parameters.Add_Value(
		pNode_0	, "ANGLE"	, _TL("Angle"),
		_TL("Angle in degrees, counting clockwise from north"),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode_0	, "ROTATEX"	, _TL("Rotation X"),
		_TL("Angle in degrees, clockwise around x axis"),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode_0	, "ROTATEY"	, _TL("Rotation Y"),
		_TL("Angle in degrees, clockwise around y axis"),
		PARAMETER_TYPE_Double, 0.0
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "SCALE"	, _TL("Scale"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode_0	, "SCALEX"	, _TL("Scale Factor X"), 
		_TL("Scale Factor X"), 
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		pNode_0	, "SCALEY"	, _TL("Scale Factor Y"), 
		_TL("Scale Factor Y"), 
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		pNode_0	, "SCALEZ"	, _TL("Scale Factor Z"),
		_TL("Scale Factor Z"),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "ANCHOR"	, _TL("Anchor Point"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode_0	, "ANCHORX"	, _TL("X"), 
		_TL("X"), 
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode_0	, "ANCHORY"	, _TL("Y"), 
		_TL("Y"), 
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode_0	, "ANCHORZ"	, _TL("Z"),
		_TL("Z"),
		PARAMETER_TYPE_Double, 0.0
	);
}//constructor

//---------------------------------------------------------
CTransformShapes::~CTransformShapes(void)
{}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTransformShapes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("IN")) && pParameter->asShapes() != NULL )
	{
		pParameters->Set_Enabled("ROTATEX",	pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("ROTATEY",	pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("DZ",		pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("SCALEZ",	pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("ANCHORZ",	pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
	}

	return( 0 );
}


//---------------------------------------------------------
bool CTransformShapes::On_Execute(void)
{
	bool		bCopy;
	double		angleX, angleY, angleZ;
	TSG_Point_Z	P, Q, Move, Scale, Anchor;
	CSG_Shapes	*pIn, *pOut;
	double		a11, a12, a13, a21, a22, a23, a31, a32, a33;

	//-----------------------------------------------------
	pIn			= Parameters("IN")		->asShapes();
	pOut		= Parameters("OUT")		->asShapes();
	Scale.x		= Parameters("SCALEX")	->asDouble();
	Scale.y		= Parameters("SCALEY")	->asDouble();
	Scale.z		= Parameters("SCALEZ")	->asDouble();
	Move.x		= Parameters("DX")		->asDouble();
	Move.y		= Parameters("DY")		->asDouble();
	Move.z		= Parameters("DZ")		->asDouble();
	Anchor.x	= Parameters("ANCHORX")	->asDouble();
	Anchor.y	= Parameters("ANCHORY")	->asDouble();
	Anchor.z	= Parameters("ANCHORZ")	->asDouble();
	angleX		= Parameters("ROTATEX")	->asDouble() * -M_DEG_TO_RAD;
	angleY		= Parameters("ROTATEY")	->asDouble() * -M_DEG_TO_RAD;
	angleZ		= Parameters("ANGLE")	->asDouble() * -M_DEG_TO_RAD;


	if( pIn == pOut )
	{
		bCopy = true;
		pOut	= SG_Create_Shapes();
	}//if
	else
	{
		bCopy = false;
	}//else

	pOut->Create(pIn->Get_Type(), CSG_String::Format(SG_T("%s [%s]"), pIn->Get_Name(), _TL("Transformed")), pIn, pIn->Get_Vertex_Type());

	//-----------------------------------------------------
	for(int iShape=0; iShape<pIn->Get_Count(); iShape++)
	{
		CSG_Shape	*pShape	= pOut->Add_Shape(pIn->Get_Shape(iShape));

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				P.x	= pShape->Get_Point(iPoint, iPart).x;
				P.y	= pShape->Get_Point(iPoint, iPart).y;

				if( pIn->Get_Vertex_Type() < SG_VERTEX_TYPE_XYZ )
				{
					// move first, then rotate and scale...

					P.x	+= Move.x - Anchor.x;
					P.y	+= Move.y - Anchor.y;

					Q.x	= Anchor.x + Scale.x * (P.x * cos(angleZ) - P.y * sin(angleZ));
					Q.y	= Anchor.y + Scale.y * (P.x * sin(angleZ) + P.y * cos(angleZ));

					pShape->Set_Point(Q.x, Q.y, iPoint, iPart);
				}
				else
				{
					P.z	= pIn->Get_Shape(iShape)->Get_Z(iPoint, iPart);

					//anchor shift
					P.x	-= Anchor.x;
					P.y	-= Anchor.y;
					P.z -= Anchor.z;

					// create rotation matrix
					a11 = cos(angleY) * cos(angleZ);
					a12 = -cos(angleX) * sin(angleZ) + sin(angleX) * sin(angleY) * cos(angleZ);
					a13 = sin(angleX) * sin(angleZ) + cos(angleX) * sin(angleY) * cos(angleZ);

					a21 = cos(angleY) * sin(angleZ);
					a22 = cos(angleX) * cos(angleZ) + sin(angleX) * sin(angleY) * sin(angleZ);
					a23 = -sin(angleX) * cos(angleZ) + cos(angleX) * sin(angleY) * sin(angleZ);

					a31 = -sin(angleY);
					a32 = sin(angleX) * cos(angleY);
					a33 = cos(angleX) * cos(angleY);

					//transform
					Q.x = (P.x * a11 + P.y * a12 + P.z * a13) * Scale.x;
					Q.y = (P.x * a21 + P.y * a22 + P.z * a23) * Scale.y;
					Q.z = (P.x * a31 + P.y * a32 + P.z * a33) * Scale.z;

					//undo anchor shift and apply move
					Q.x	+= Anchor.x + Move.x;
					Q.y	+= Anchor.y + Move.y;
					Q.z += Anchor.z + Move.z;

					pShape->Set_Point(Q.x, Q.y, iPoint, iPart);
					pShape->Set_Z(Q.z, iPoint, iPart);

					if( pIn->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pShape->Set_M(pIn->Get_Shape(iShape)->Get_M(iPoint, iPart), iPoint, iPart);
					}
				}
			}//for iPoint
		}//for iPart
	}//for iShape

	//-----------------------------------------------------
	if( bCopy )
	{
		pIn->Assign(pOut);
		delete(pOut);
	}//if

	return true;

}//method
