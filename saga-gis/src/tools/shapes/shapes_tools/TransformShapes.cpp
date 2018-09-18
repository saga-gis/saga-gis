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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "TransformShapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTransformShapes::CTransformShapes(void)
{
	Set_Name		(_TL("Transform Shapes"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Use this tool to move, rotate and/or scale shapes."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"TRANSFORM"	, _TL("Transformed Shapes"), 
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Node("", "MOVE", _TL("Translation"), _TL("Amount in map units that vertices will be moved."));

	Parameters.Add_Double("MOVE"  , "MOVEX"  , _TL("X"), _TL(""), 0.0);
	Parameters.Add_Double("MOVE"  , "MOVEY"  , _TL("Y"), _TL(""), 0.0);
	Parameters.Add_Double("MOVE"  , "MOVEZ"  , _TL("Z"), _TL(""), 0.0);

	//-----------------------------------------------------
	Parameters.Add_Node("", "ANCHOR", _TL("Anchor Point"), _TL("Anchor point for scaling and rotation."));

	Parameters.Add_Double("ANCHOR", "ANCHORX", _TL("X"), _TL(""), 0.0);
	Parameters.Add_Double("ANCHOR", "ANCHORY", _TL("Y"), _TL(""), 0.0);
	Parameters.Add_Double("ANCHOR", "ANCHORZ", _TL("Z"), _TL(""), 0.0);

	//-----------------------------------------------------
	Parameters.Add_Node("", "SCALE", _TL("Scaling"), _TL(""));

	Parameters.Add_Double("SCALE" , "SCALEX" , _TL("X"), _TL(""), 1.0);
	Parameters.Add_Double("SCALE" , "SCALEY" , _TL("Y"), _TL(""), 1.0);
	Parameters.Add_Double("SCALE" , "SCALEZ" , _TL("Z"), _TL(""), 1.0);

	//-----------------------------------------------------
	Parameters.Add_Node("", "ROTATE", _TL("Rotation"), _TL("Rotation angles around coordinate axes in degree counting clockwise."));
	
	Parameters.Add_Double("ROTATE", "ROTATEX", _TL("X"), _TL(""), 0.0);
	Parameters.Add_Double("ROTATE", "ROTATEY", _TL("Y"), _TL(""), 0.0);
	Parameters.Add_Double("ROTATE", "ROTATEZ", _TL("Z"), _TL(""), 0.0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTransformShapes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES") && pParameter->asShapes() != NULL )
	{
		pParameters->Set_Parameter("ANCHORX",  pParameter->asShapes()->Get_Extent().Get_Center().Get_X());
		pParameters->Set_Parameter("ANCHORY",  pParameter->asShapes()->Get_Extent().Get_Center().Get_Y());
		pParameters->Set_Parameter("ANCHORZ", (pParameter->asShapes()->Get_ZMin() + pParameter->asShapes()->Get_ZMax()) / 2.0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
int CTransformShapes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES") && pParameter->asShapes() != NULL )
	{
		pParameters->Set_Enabled("MOVEZ"  , pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("SCALEZ" , pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("ANCHORZ", pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("ROTATEX", pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
		pParameters->Set_Enabled("ROTATEY", pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTransformShapes::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("TRANSFORM")->asShapes();

	if( pShapes && pShapes != Parameters("SHAPES")->asShapes() )
	{
		pShapes->Create(*Parameters("SHAPES")->asShapes());

		DataObject_Set_Parameters(pShapes, Parameters("SHAPES")->asShapes());

		pShapes->Set_Name(CSG_String::Format("%s [%s]", pShapes->Get_Name(), _TL("Transformed")));
	}
	else
	{
		pShapes	= Parameters("SHAPES")->asShapes();
	}

	//-----------------------------------------------------
	TSG_Point_Z	Move, Scale, Anchor, Rotate;

	Move.x		= Parameters("MOVEX"  )->asDouble();
	Move.y		= Parameters("MOVEY"  )->asDouble();
	Move.z		= Parameters("MOVEZ"  )->asDouble();

	Anchor.x	= Parameters("ANCHORX")->asDouble();
	Anchor.y	= Parameters("ANCHORY")->asDouble();
	Anchor.z	= Parameters("ANCHORZ")->asDouble();

	Scale.x		= Parameters("SCALEX" )->asDouble();
	Scale.y		= Parameters("SCALEY" )->asDouble();
	Scale.z		= Parameters("SCALEZ" )->asDouble();

	Rotate.x	= Parameters("ROTATEX")->asDouble() * -M_DEG_TO_RAD;
	Rotate.y	= Parameters("ROTATEY")->asDouble() * -M_DEG_TO_RAD;
	Rotate.z	= Parameters("ROTATEZ")->asDouble() * -M_DEG_TO_RAD;

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				if( pShapes->Get_Vertex_Type() < SG_VERTEX_TYPE_XYZ )
				{
					TSG_Point	Q, P	= pShape->Get_Point(iPoint, iPart);

					P.x	+= Move.x - Anchor.x;	// move first, then rotate and scale...
					P.y	+= Move.y - Anchor.y;

					Q.x	= Anchor.x + Scale.x * (P.x * cos(Rotate.z) - P.y * sin(Rotate.z));
					Q.y	= Anchor.y + Scale.y * (P.x * sin(Rotate.z) + P.y * cos(Rotate.z));

					pShape->Set_Point(Q.x, Q.y, iPoint, iPart);
				}
				else
				{
					TSG_Point_Z	P, Q;

					P.x	= pShape->Get_Point(iPoint, iPart).x;
					P.y	= pShape->Get_Point(iPoint, iPart).y;
					P.z	= pShape->Get_Z    (iPoint, iPart);

					// anchor shift
					P.x	-= Anchor.x;
					P.y	-= Anchor.y;
					P.z -= Anchor.z;

					// create rotation matrix
					double	a11 =  cos(Rotate.y) * cos(Rotate.z);
					double	a12 = -cos(Rotate.x) * sin(Rotate.z) + sin(Rotate.x) * sin(Rotate.y) * cos(Rotate.z);
					double	a13 =  sin(Rotate.x) * sin(Rotate.z) + cos(Rotate.x) * sin(Rotate.y) * cos(Rotate.z);

					double	a21 =  cos(Rotate.y) * sin(Rotate.z);
					double	a22 =  cos(Rotate.x) * cos(Rotate.z) + sin(Rotate.x) * sin(Rotate.y) * sin(Rotate.z);
					double	a23 = -sin(Rotate.x) * cos(Rotate.z) + cos(Rotate.x) * sin(Rotate.y) * sin(Rotate.z);

					double	a31 = -sin(Rotate.y);
					double	a32 =  sin(Rotate.x) * cos(Rotate.y);
					double	a33 =  cos(Rotate.x) * cos(Rotate.y);

					// transform
					Q.x = (P.x * a11 + P.y * a12 + P.z * a13) * Scale.x;
					Q.y = (P.x * a21 + P.y * a22 + P.z * a23) * Scale.y;
					Q.z = (P.x * a31 + P.y * a32 + P.z * a33) * Scale.z;

					// undo anchor shift and apply move
					Q.x	+= Anchor.x + Move.x;
					Q.y	+= Anchor.y + Move.y;
					Q.z += Anchor.z + Move.z;

					pShape->Set_Point(Q.x, Q.y, iPoint, iPart);
					pShape->Set_Z    (     Q.z, iPoint, iPart);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pShapes == Parameters("SHAPES")->asShapes() )
	{
		DataObject_Update(pShapes);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
