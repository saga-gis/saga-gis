/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pc_transform.cpp                   //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
//                                                       //
//    Based on TransformShapes.cpp, (C) Victor Olaya     //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_transform.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Transform::CPC_Transform(void)
{
	CSG_Parameter *pNode_0, *pNode_1;

	//-----------------------------------------------------
	Set_Name(_TL("Transform Point Cloud"));

	Set_Author(_TL("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The tool can be used to shift, rotate and/or scale a Point Cloud.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "IN"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "OUT"		, _TL("Output"), 
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "MOVE"	, _TL("Move"),
		_TL("")
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "DX"		, _TL("dX"), 
		_TL("dX (Map Units)"), 
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "DY"		, _TL("dY"), 
		_TL("dY (Map Units)"), 
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0, "DZ"		, _TL("dZ"),
		_TL("dZ (Map Units)"),
		PARAMETER_TYPE_Double, 0.0
		);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "ROTATE"	,
		_TL("Rotate"),
		_TL(""));
	
	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANGLEX"	, _TL("Angle X"),
		_TL("Angle in degrees, clockwise around x axis"),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANGLEY"	, _TL("Angle Y"),
		_TL("Angle in degrees, clockwise around y axis"),
		PARAMETER_TYPE_Double, 0.0
		);

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANGLEZ"	, _TL("Angle Z"),
		_TL("Angle in degrees, clockwise around z axis"),
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

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "SCALEZ"	, _TL("Scale Factor Z"),
		_TL("Scale Factor Z"),
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

	pNode_1 = Parameters.Add_Value(
		pNode_0	, "ANCHORZ"	, _TL("Z"),
		_TL("Z"),
		PARAMETER_TYPE_Double, 0.0
		);
}

//---------------------------------------------------------
CPC_Transform::~CPC_Transform(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Transform::On_Execute(void)
{
	bool			bCopy;
	double			angleX, angleY, angleZ;
	TSG_Point_Z		P, Q, Move, Scale, Anchor;
	CSG_PointCloud	*pIn, *pOut;
	double a11, a12, a13, a21, a22, a23, a31, a32, a33;

	//-----------------------------------------------------
	pIn			= Parameters("IN")		->asPointCloud();
	pOut		= Parameters("OUT")		->asPointCloud();
	Scale.x		= Parameters("SCALEX")	->asDouble();
	Scale.y		= Parameters("SCALEY")	->asDouble();
	Scale.z		= Parameters("SCALEZ")	->asDouble();
	Move.x		= Parameters("DX")		->asDouble();
	Move.y		= Parameters("DY")		->asDouble();
	Move.z		= Parameters("DZ")		->asDouble();
	Anchor.x	= Parameters("ANCHORX")	->asDouble();
	Anchor.y	= Parameters("ANCHORY")	->asDouble();
	Anchor.z	= Parameters("ANCHORZ")	->asDouble();

	angleX		= Parameters("ANGLEX")	->asDouble() * -M_DEG_TO_RAD;
	angleY		= Parameters("ANGLEY")	->asDouble() * -M_DEG_TO_RAD;
	angleZ		= Parameters("ANGLEZ")	->asDouble() * -M_DEG_TO_RAD;

	if( pIn == pOut )
	{
		bCopy	= true;
		pOut	= SG_Create_PointCloud();
	}
	else
		bCopy	= false;

	pOut->Create(pIn);

	pOut->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pIn->Get_Name(), _TL("Transformed")));


	//-----------------------------------------------------
	for (int iPoint=0; iPoint<pIn->Get_Point_Count(); iPoint++)
	{
		P	= pIn->Get_Point(iPoint);

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
		a32 =  sin(angleX) * cos(angleY);
		a33 = cos(angleX) * cos(angleY);


		//transform
		Q.x = (P.x * a11 + P.y * a12 + P.z * a13) * Scale.x;
		Q.y = (P.x * a21 + P.y * a22 + P.z * a23) * Scale.y;
		Q.z = (P.x * a31 + P.y * a32 + P.z * a33) * Scale.z;

		//undo anchor shift and apply move
		Q.x	+= Anchor.x + Move.x;
		Q.y	+= Anchor.y + Move.y;
		Q.z += Anchor.z + Move.z;

		pOut->Add_Point(Q.x, Q.y, Q.z);

		for (int j=0; j<pIn->Get_Attribute_Count(); j++)
		{
			switch (pIn->Get_Attribute_Type(j))
			{
			default:					pOut->Set_Attribute(iPoint, j, pIn->Get_Attribute(iPoint, j));		break;
			case SG_DATATYPE_Date:
			case SG_DATATYPE_String:	CSG_String sAttr; pIn->Get_Attribute(iPoint, j, sAttr); pOut->Set_Attribute(iPoint, j, sAttr);		break;
			}
		}
	}

	//-----------------------------------------------------
	if( bCopy )
	{
		pIn->Assign(pOut);
		delete(pOut);
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
