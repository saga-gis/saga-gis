/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    AddCoordinates.cpp
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
#include "AddCoordinates.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAddCoordinates::CAddCoordinates(void)
{	
	Set_Name		(_TL("Add Coordinates to Points"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"The tool attaches the x- and y-coordinates of each point to the attribute table. "
		"For 3D shapefiles, also the z/m-coordinates are reported."
	));

	Parameters.Add_Shapes("", "INPUT" , _TL("Points"), _TL(""), PARAMETER_INPUT          , SHAPE_TYPE_Point);
	Parameters.Add_Shapes("", "OUTPUT", _TL("Output"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point);

	Parameters.Add_Bool("", "X", _TL("X"), _TL(""), true);
	Parameters.Add_Bool("", "Y", _TL("Y"), _TL(""), true);
	Parameters.Add_Bool("", "Z", _TL("Z"), _TL(""), true);
	Parameters.Add_Bool("", "M", _TL("M"), _TL(""), true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CAddCoordinates::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "INPUT") )
	{
		if( pParameter->asShapes() )
		{
			pParameters->Set_Enabled("X", true);
			pParameters->Set_Enabled("Y", true);
			pParameters->Set_Enabled("Z", pParameter->asShapes()->Get_Vertex_Type() != SG_VERTEX_TYPE_XY);
			pParameters->Set_Enabled("M", pParameter->asShapes()->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM);
		}
		else
		{
			pParameters->Set_Enabled("X", false);
			pParameters->Set_Enabled("Y", false);
			pParameters->Set_Enabled("Z", false);
			pParameters->Set_Enabled("M", false);
		}
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAddCoordinates::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("OUTPUT")->asShapes();

	if( pPoints )
	{
		pPoints->Create(*Parameters("INPUT")->asShapes());
	}
	else
	{
		pPoints	= Parameters("INPUT")->asShapes();
	}

	//-----------------------------------------------------
	int	xField = -1, yField = -1, zField = -1, mField = -1;

	if( Parameters("X")->asBool() )
	{
		xField = pPoints->Get_Field_Count(); pPoints->Add_Field("X", SG_DATATYPE_Double);
	}

	if( Parameters("Y")->asBool() )
	{
		yField = pPoints->Get_Field_Count(); pPoints->Add_Field("Y", SG_DATATYPE_Double);
	}

	if( Parameters("Z")->asBool() && pPoints->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
	{
		xField = pPoints->Get_Field_Count(); pPoints->Add_Field("Z", SG_DATATYPE_Double);
	}

	if( Parameters("M")->asBool() && pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
	{
		mField = pPoints->Get_Field_Count(); pPoints->Add_Field("M", SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( xField >= 0 ) pPoint->Set_Value(xField, pPoint->Get_Point(0).x);
		if( yField >= 0 ) pPoint->Set_Value(yField, pPoint->Get_Point(0).y);
		if( zField >= 0 ) pPoint->Set_Value(zField, pPoint->Get_Z    (0)  );
		if( mField >= 0 ) pPoint->Set_Value(mField, pPoint->Get_M    (0)  );
	}

	//-----------------------------------------------------
	DataObject_Update(pPoints);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
