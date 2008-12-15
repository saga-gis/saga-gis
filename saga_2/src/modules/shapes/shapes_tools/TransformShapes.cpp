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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

//---------------------------------------------------------
#include "TransformShapes.h"

//---------------------------------------------------------
CTransformShapes::CTransformShapes(void)
{
	Set_Name		(_TL("Transform Shapes"));
	Set_Author		(_TL("(c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Use this module to move, rotate and/or scale shapes."
	));

	CSG_Parameter *pNode_0, *pNode_1;

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "OUT"		, _TL("Output"), 
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "IN"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "MOVE"	, _TL("Move"),
		_TL("")
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "DX"		, _TL("dX"), 
		_TL("dX(GridUnits)"), 
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "DY"		, _TL("dY"), 
		_TL("dY(GridUnits)"), 
		PARAMETER_TYPE_Double, 0.0
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "ROTATE"	,
		_TL("Rotate"),
		_TL(""));
	
	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANGLE"	, _TL("Angle"), 
		_TL("Angle in degrees, counting clockwise from north"), 
		PARAMETER_TYPE_Double, 0.0
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "SCALE"	, _TL("Scale"),
		_TL("")
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "SCALEX"	, _TL("Scale Factor X"), 
		_TL("Scale Factor X"), 
		PARAMETER_TYPE_Double, 1.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "SCALEY"	, _TL("Scale Factor Y"), 
		_TL("Scale Factor Y"), 
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "ANCHOR"	, _TL("Anchor Point"),
		_TL("")
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANCHORX"	, _TL("X"), 
		_TL("X"), 
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANCHORY"	, _TL("Y"), 
		_TL("Y"), 
		PARAMETER_TYPE_Double, 0.0
	);	
}//constructor

//---------------------------------------------------------
CTransformShapes::~CTransformShapes(void)
{}

//---------------------------------------------------------
bool CTransformShapes::On_Execute(void)
{
	bool		bCopy;
	double		Angle;
	TSG_Point	P, Q, Move, Scale, Anchor;
	CSG_Shapes	*pIn, *pOut;

	//-----------------------------------------------------
	pIn			= Parameters("IN")		->asShapes();
	pOut		= Parameters("OUT")		->asShapes();
	Scale.x		= Parameters("SCALEX")	->asDouble();
	Scale.y		= Parameters("SCALEY")	->asDouble();
	Move.x		= Parameters("DX")		->asDouble();
	Move.y		= Parameters("DY")		->asDouble();
	Anchor.x	= Parameters("ANCHORX")	->asDouble();
	Anchor.y	= Parameters("ANCHORY")	->asDouble();
	Angle		= Parameters("ANGLE")	->asDouble() * -M_DEG_TO_RAD;

	if( pIn == pOut )
	{
		bCopy = true;
		pOut	= SG_Create_Shapes();
	}//if
	else
	{
		bCopy = false;
	}//else

	pOut->Create(pIn->Get_Type(), CSG_String::Format(SG_T("%s [%s]"), pIn->Get_Name(), _TL("Transformed")), pIn);

	//-----------------------------------------------------
	for(int iShape=0; iShape<pIn->Get_Count(); iShape++)
	{
		CSG_Shape	*pShape	= pOut->Add_Shape(pIn->Get_Shape(iShape));

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				// move first, then rotate and scale...

				P	= pShape->Get_Point(iPoint, iPart);

				P.x	+= Move.x - Anchor.x;
				P.y	+= Move.y - Anchor.y;

				Q.x	= Anchor.x + Scale.x * (P.x * cos(Angle) - P.y * sin(Angle));
				Q.y	= Anchor.y + Scale.y * (P.x * sin(Angle) + P.y * cos(Angle));

				pShape->Set_Point(Q, iPoint, iPart);
			}//for
		}//for
	}//for

	//-----------------------------------------------------
	if( bCopy )
	{
		pIn->Assign(pOut);
		delete(pOut);
	}//if

	return true;

}//method
