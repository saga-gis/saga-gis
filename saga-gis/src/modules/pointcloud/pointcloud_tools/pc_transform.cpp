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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
		"The module can be used to shift, rotate and/or scale a Point Cloud.\n\n")
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
	double			Angle;
	TSG_Point_Z		P, Q, Move, Scale, Anchor;
	CSG_PointCloud	*pIn, *pOut;

	//-----------------------------------------------------
	pIn			= Parameters("IN")		->asPointCloud();
	pOut		= Parameters("OUT")		->asPointCloud();
	Scale.x		= Parameters("SCALEX")	->asDouble();
	Scale.y		= Parameters("SCALEY")	->asDouble();
	Move.x		= Parameters("DX")		->asDouble();
	Move.y		= Parameters("DY")		->asDouble();
	Anchor.x	= Parameters("ANCHORX")	->asDouble();
	Anchor.y	= Parameters("ANCHORY")	->asDouble();
	Angle		= Parameters("ANGLE")	->asDouble() * -M_DEG_TO_RAD;


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

		P.x	+= Move.x - Anchor.x;
		P.y	+= Move.y - Anchor.y;

		Q.x	= Anchor.x + Scale.x * (P.x * cos(Angle) - P.y * sin(Angle));
		Q.y	= Anchor.y + Scale.y * (P.x * sin(Angle) + P.y * cos(Angle));
		Q.z	= P.z;

		pOut->Add_Point(Q.x, Q.y, Q.z);

		for (int iField=0; iField<pIn->Get_Attribute_Count(); iField++)
			pOut->Set_Attribute(iPoint, iField, pIn->Get_Attribute(iPoint, iField));
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
