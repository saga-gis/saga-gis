
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  line_properties.cpp                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "line_properties.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Properties::CLine_Properties(void)
{
	Set_Name		(_TL("Line Properties"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Line properties: length, number of vertices."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"  , _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"OUTPUT" , _TL("Lines with Property Attributes"),
		_TL("If not set property attributes will be added to the original layer."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Fields("LINES",
		"FIELDS" , _TL("Copy Attributes"),
		_TL("Select one or more attributes to be copied to the target layer.")
	);

	Parameters.Add_Bool("",
		"BPARTS" , _TL("Number of Parts"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"BPOINTS", _TL("Number of Vertices"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"BLENGTH", _TL("Length"),
		_TL(""),
		true
	);

	Parameters.Add_Double("BLENGTH",
		"SCALING", _TL("Scaling"),
		_TL("Scaling factor for length property. meter to feet = 1 / 0.3048 = 3.2808"),
		1., 0., true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Properties::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("OUTPUT") )
	{
		pParameters->Set_Enabled("FIELDS", pParameter->asPointer() && pParameter->asPointer() != (*pParameters)("LINES")->asPointer());
	}

	if( pParameter->Cmp_Identifier("BLENGTH") )
	{
		pParameters->Set_Enabled("SCALING", pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Properties::On_Execute(void)
{
	int	bParts  = Parameters("BPARTS" )->asBool() ? 0 : -1;
	int	bPoints = Parameters("BPOINTS")->asBool() ? 0 : -1;
	int	bLength = Parameters("BLENGTH")->asBool() ? 0 : -1;

	if( bParts && bPoints && bLength )
	{
		Error_Set(_TL("no properties selected"));

		return( false );
	}

	//-------------------------------------------------
	CSG_Shapes *pLines = Parameters("LINES")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() < 1 )
	{
		Error_Set(_TL("invalid lines layer"));

		return( false );
	}

	if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pLines )
	{
		CSG_Shapes *pCopies = Parameters("OUTPUT")->asShapes();

		pCopies->Create(SHAPE_TYPE_Line, pLines->Get_Name(), NULL, pLines->Get_Vertex_Type());

		CSG_Parameter_Table_Fields *pFields = Parameters("FIELDS")->asTableFields();

		for(int Field=0; Field<pFields->Get_Count(); Field++)
		{
			pCopies->Add_Field(
				pLines->Get_Field_Name(pFields->Get_Index(Field)),
				pLines->Get_Field_Type(pFields->Get_Index(Field))
			);
		}

		for(sLong i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++)
		{
			CSG_Shape *pLine = pLines->Get_Shape(i); CSG_Shape *pCopy = pCopies->Add_Shape(pLine, SHAPE_COPY_GEOM);

			for(int Field=0; Field<pFields->Get_Count(); Field++)
			{
				*pCopy->Get_Value(Field) = *pLine->Get_Value(pFields->Get_Index(Field));
			}
		}

		pLines = pCopies;
	}

	//-------------------------------------------------
	if( !bParts  ) { bParts  = pLines->Get_Field_Count(); pLines->Add_Field("NPARTS" , SG_DATATYPE_Int   ); }
	if( !bPoints ) { bPoints = pLines->Get_Field_Count(); pLines->Add_Field("NPOINTS", SG_DATATYPE_Int   ); }
	if( !bLength ) { bLength = pLines->Get_Field_Count(); pLines->Add_Field("LENGTH" , SG_DATATYPE_Double); }

	double Scaling = Parameters("SCALING")->asDouble();

	//-------------------------------------------------
	for(sLong i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++)
	{
		CSG_Shape *pLine = pLines->Get_Shape(i);

		if( bParts  >= 0 ) { pLine->Set_Value(bParts , pLine->Get_Part_Count ()); }
		if( bPoints >= 0 ) { pLine->Set_Value(bPoints, pLine->Get_Point_Count()); }
		if( bLength >= 0 ) { pLine->Set_Value(bLength, Scaling * ((CSG_Shape_Line *)pLine)->Get_Length()); }
	}

	//-------------------------------------------------
	if( pLines == Parameters("LINES")->asShapes() )
	{
		DataObject_Update(pLines);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
