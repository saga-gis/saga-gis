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
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  shapes_generate.cpp                  //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                    Volker Wichmann                    //
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
#include "shapes_generate.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Generate::CShapes_Generate(void)
{

	Set_Name(_TL("Generate Shapes"));

	Set_Author(_TL("Volker Wichmann (c) 2012, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The module allows one to generate point, line or polygon shapes "
		"from a table with x and y coordinates and an identifier. The table "
		"must be sorted in vertex order.\n\n"
		"The identifier has different meanings:\n\n"
		"* Point Shapes: The identifier is arbitrary\n\n"
		"* Line Shapes: The identifier is unique for each line\n\n"
		"* Polygon Shapes: The identifier is unique for each polygon; "
		"the first polygon vertex may but must not be duplicated in "
		"order to close the polygon\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "INPUT"		,_TL("Input"),
		_TL("Table with coordinates."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		Parameters("INPUT"), "FIELD_ID", _TL("ID"),
		_TL("Field with identifier.")
	);
	Parameters.Add_Table_Field(
		Parameters("INPUT"), "FIELD_X", _TL("X"),
		_TL("Field with x-coordinate.")
	);
	Parameters.Add_Table_Field(
		Parameters("INPUT"), "FIELD_Y", _TL("Y"),
		_TL("Field with y-coordinate.")
	);
	/*Parameters.Add_Table_Field(
		Parameters("INPUT"), "FIELD_Z", _TL("Z"),
		_TL("Field with z-coordinate."),
		true
	);*/

	Parameters.Add_Choice(
		NULL	, "SHAPE_TYPE"	, _TL("Shape Type"),
		_TL("Shape type to generate."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Point(s)"),
			_TL("Line(s)"),
			_TL("Polygon(s)")
		), 0
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL("Generated Shapefile."),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CShapes_Generate::~CShapes_Generate(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Generate::On_Execute(void)
{
	CSG_Table		*pInput;
	CSG_Shapes		*pOutput;
	int				iFieldId, iFieldX, iFieldY; // iFieldZ
	int				iShapeType;
	CSG_String		sName;

	pInput		= Parameters("INPUT")->asTable();
	iFieldId	= Parameters("FIELD_ID")->asInt();
	iFieldX		= Parameters("FIELD_X")->asInt();
	iFieldY		= Parameters("FIELD_Y")->asInt();
	//iFieldZ	= Parameters("FIELD_Z")->asInt();
	iShapeType	= Parameters("SHAPE_TYPE")->asInt();
	pOutput		= Parameters("OUTPUT")->asShapes();
	
	
	if (pInput->Get_Count() < 1)
	{
		SG_UI_Msg_Add_Error(_TL("Input table is empty!"));
		return (false);
	}


	//-----------------------------------------------------
	// Initialize ...
	sName = CSG_String::Format(SG_T("Shapes_%s"), pInput->Get_Name());

	pOutput->Destroy();

	switch (iShapeType)
	{
	default:
	case 0:				// point
		pOutput->Create(SHAPE_TYPE_Point, sName, (CSG_Table*)0, SG_VERTEX_TYPE_XY);
		break;
	case 1:				// line
		pOutput->Create(SHAPE_TYPE_Line, sName, (CSG_Table*)0, SG_VERTEX_TYPE_XY);
		break;
	case 2:				// polygon
		pOutput->Create(SHAPE_TYPE_Polygon, sName, (CSG_Table*)0, SG_VERTEX_TYPE_XY);
		break;
	}

	pOutput->Add_Field(SG_T("ID"), SG_DATATYPE_Int);

	
	//-----------------------------------------------------
	// Generate ...

	switch (iShapeType)
	{
	default:
	case 0:				// point
		if (!Generate_Point_Shapes(pInput, pOutput, iFieldId, iFieldX, iFieldY))
			return (false);
		break;
	case 1:				// line
		if (!Generate_Line_Shapes(pInput, pOutput, iFieldId, iFieldX, iFieldY))
			return (false);
		break;
	case 2:				// polygon
		if (!Generate_Polygon_Shapes(pInput, pOutput, iFieldId, iFieldX, iFieldY))
			return (false);
		break;
	}


	//-----------------------------------------------------
	return (true);
}


//---------------------------------------------------------
bool CShapes_Generate::Generate_Point_Shapes(CSG_Table *pInput, CSG_Shapes *pOutput, int iFieldId, int iFieldX, int iFieldY)
{
	for (int i=0; i<pInput->Get_Count() && Set_Progress(i, pInput->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord = pInput->Get_Record(i);
		CSG_Shape			*pShape  = pOutput->Add_Shape();
		
		pShape->Add_Point(pRecord->asDouble(iFieldX), pRecord->asDouble(iFieldY));
		pShape->Set_Value(0, pRecord->asInt(iFieldId));
	}


	return (true);
}


//---------------------------------------------------------
bool CShapes_Generate::Generate_Line_Shapes(CSG_Table *pInput, CSG_Shapes *pOutput, int iFieldId, int iFieldX, int iFieldY)
{
	int			iVertices	= 0;
	int			iID			= pInput->Get_Record(0)->asInt(iFieldId);
	CSG_Shape	*pShape		= pOutput->Add_Shape();
	
	pShape->Set_Value(0, iID);

	for (int i=0; i<pInput->Get_Count() && Set_Progress(i, pInput->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord = pInput->Get_Record(i);

		if (pRecord->asInt(iFieldId) != iID)
		{
			if (iVertices < 2)
			{
				pOutput->Del_Record(pOutput->Get_Record_Count() - 1);
				SG_UI_Msg_Add_Error(_TL("Line with less than 2 vertices encountered!"));
				return (false);
			}

			pShape		= pOutput->Add_Shape();
			iID			= pRecord->asInt(iFieldId);

			pShape->Set_Value(0, iID);

			iVertices	= 0;
		}

		pShape->Add_Point(pRecord->asDouble(iFieldX), pRecord->asDouble(iFieldY));
		
		iVertices	+= 1;
		iID			= pRecord->asInt(iFieldId);
	}

	if (iVertices < 2)
	{
		pOutput->Del_Record(pOutput->Get_Record_Count() - 1);
		SG_UI_Msg_Add_Error(_TL("Line with less than 2 vertices encountered!"));
		return (false);
	}


	return (true);
}


//---------------------------------------------------------
bool CShapes_Generate::Generate_Polygon_Shapes(CSG_Table *pInput, CSG_Shapes *pOutput, int iFieldId, int iFieldX, int iFieldY)
{
	int			iVertices	= 0;
	int			iID			= pInput->Get_Record(0)->asInt(iFieldId);
	CSG_Shape	*pShape		= pOutput->Add_Shape();

	pShape->Set_Value(0, iID);

	TSG_Point	pointA;
	pointA.x	= pInput->Get_Record(0)->asDouble(iFieldX);
	pointA.y	= pInput->Get_Record(0)->asDouble(iFieldY);

	for (int i=0; i<pInput->Get_Count() && Set_Progress(i, pInput->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord = pInput->Get_Record(i);

		if (pRecord->asInt(iFieldId) != iID)
		{
			if (iVertices < 3)
			{
				pOutput->Del_Record(pOutput->Get_Record_Count() - 1);
				SG_UI_Msg_Add_Error(_TL("Polygon with less than 3 vertices encountered!"));
				return (false);
			}

			TSG_Point	pointB;
			pointB.x	= pInput->Get_Record(i-1)->asDouble(iFieldX);
			pointB.y	= pInput->Get_Record(i-1)->asDouble(iFieldY);

			if (pointA.x != pointB.x || pointA.y != pointB.y)
				pShape->Add_Point(pointA);

			pShape		= pOutput->Add_Shape();
			iID			= pRecord->asInt(iFieldId);

			pShape->Set_Value(0, iID);

			pointA.x	= pInput->Get_Record(i)->asDouble(iFieldX);
			pointA.y	= pInput->Get_Record(i)->asDouble(iFieldY);
			iVertices	= 0;
		}

		pShape->Add_Point(pRecord->asDouble(iFieldX), pRecord->asDouble(iFieldY));
		
		iVertices	+= 1;
		iID			= pRecord->asInt(iFieldId);
	}

	if (iVertices < 3)
	{
		pOutput->Del_Record(pOutput->Get_Record_Count() - 1);
		SG_UI_Msg_Add_Error(_TL("Polygon with less than 3 vertices encountered!"));
		return (false);
	}


	return (true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
