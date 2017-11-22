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
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             shapes_convert_vertex_type.cpp            //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                    Volker Wichmann                    //
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
#include "shapes_convert_vertex_type.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Convert_Vertex_Type::CShapes_Convert_Vertex_Type(void)
{
	Set_Name		(_TL("Convert Vertex Type (2D/3D)"));

	Set_Author		("Volker Wichmann (c) 2013, LASERDATA GmbH");

	Set_Description	(_TW(
		"The tool allows one to convert the vertex type of shapes from "
		"'XY' (2D) to 'XYZ/M' (3D) and vice versa. The conversion from "
		"3D to 2D is not lossless for lines and polygons, as only the "
		"Z/M value of one vertex can be retained (currently that of the "
		"last vertex)."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"INPUT"		, _TL("Input"),
		_TL("The shapefile to convert."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("INPUT",
		"FIELD_Z"	, _TL("Z"),
		_TL("Field with z-coordinate information."),
		true
	);

	Parameters.Add_Table_Field("INPUT",
		"FIELD_M"	, _TL("M"),
		_TL("Field with measure information."),
		true
	);

	Parameters.Add_Shapes("",
		"OUTPUT"	, _TL("Output"),
		_TL("The converted shapefile."),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Convert_Vertex_Type::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "INPUT") && pParameter->asShapes() != NULL )
	{
		pParameters->Set_Enabled("FIELD_Z", pParameters->Get("INPUT")->asShapes()->Get_Vertex_Type() == SG_VERTEX_TYPE_XY);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "FIELD_Z") )
	{
		pParameters->Set_Enabled("FIELD_M", pParameter->asInt() >= 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Convert_Vertex_Type::On_Execute(void)
{
	CSG_Shapes		*pInput, *pOutput;
	int				iFieldZ, iFieldM;

	pInput		= Parameters("INPUT"  )->asShapes();
	iFieldZ		= Parameters("FIELD_Z")->asInt();
	iFieldM		= Parameters("FIELD_M")->asInt();
	pOutput		= Parameters("OUTPUT" )->asShapes();	
	
	if( pInput->Get_Count() < 1 )
	{
		SG_UI_Msg_Add_Error(_TL("Input shape is empty!"));

		return (false);
	}

	//-----------------------------------------------------
	if( pInput->Get_Vertex_Type() == SG_VERTEX_TYPE_XY )
	{
		if( iFieldZ < 0 )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide an attribute field with z-information!"));

			return( false );
		}

		if( iFieldM < 0 )
		{
			pOutput->Create(pInput->Get_Type(), CSG_String::Format("%s_Z", pInput->Get_Name()), pInput, SG_VERTEX_TYPE_XYZ);
		}
		else
		{
			pOutput->Create(pInput->Get_Type(), CSG_String::Format("%s_ZM", pInput->Get_Name()), pInput, SG_VERTEX_TYPE_XYZM);
		}
	}
	else
	{
		pOutput->Create(pInput->Get_Type(), CSG_String::Format("%s_XY", pInput->Get_Name()), pInput, SG_VERTEX_TYPE_XY);

		pOutput->Add_Field("Z", SG_DATATYPE_Double);

		if( pInput->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
		{
			pOutput->Add_Field("M", SG_DATATYPE_Double);
		}
	}


	//-----------------------------------------------------
	for(int iShape=0; iShape<pInput->Get_Count(); iShape++)
	{
		CSG_Shape	*pShapeIn	= pInput	->Get_Shape(iShape);
		CSG_Shape	*pShapeOut	= pOutput	->Add_Shape(pShapeIn, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pShapeIn->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShapeIn->Get_Point_Count(iPart); iPoint++)
			{
				pShapeOut->Add_Point(pShapeIn->Get_Point(iPoint, iPart), iPart);

				if( pInput->Get_Vertex_Type() == SG_VERTEX_TYPE_XY )
				{
					pShapeOut->Set_Z(pShapeIn->asDouble(iFieldZ), iPoint, iPart);

					if( pOutput->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pShapeOut->Set_M(pShapeIn->asDouble(iFieldM), iPoint, iPart);
					}
				}
				else
				{
					if( pInput->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pShapeOut->Set_Value(pOutput->Get_Field_Count() - 1, pShapeIn->Get_M(iPoint, iPart));
						pShapeOut->Set_Value(pOutput->Get_Field_Count() - 2, pShapeIn->Get_Z(iPoint, iPart));
					}
					else
					{
						pShapeOut->Set_Value(pOutput->Get_Field_Count() - 1, pShapeIn->Get_Z(iPoint, iPart));
					}
				}
			}
		}
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
