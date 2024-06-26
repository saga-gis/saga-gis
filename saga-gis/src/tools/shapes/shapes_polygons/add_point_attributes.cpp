
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                add_point_attributes.cpp               //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#include "add_point_attributes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAdd_Point_Attributes::CAdd_Point_Attributes(void)
{
	Set_Name		(_TL("Add Point Attributes to Polygons"));

	Set_Author		("V. Wichmann (c) 2014");

	Set_Description	(_TW(
		"Spatial join for polygons. Retrieves for each polygon the selected "
		"attributes from that point, which is contained in the polygon. In case "
		"a polygon contains more than one point, the last point wins.\n"
		"Optionally, the tool allows one to attach the geometrical properties "
		"(x,y(z,m)) of each point as additional attributes.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"INPUT"		, _TL("Polygons"),
		_TL("Input polygon shapefile"),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL("Input point shapefile"),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Fields("POINTS",
		"FIELDS"	, _TL("Attributes"),
		_TL("Attributes to add. Select none to add all")
	);

	Parameters.Add_Shapes("",
		"OUTPUT"	, _TL("Result"),
		_TL("Optional output polygon shapefile"),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);
	
	Parameters.Add_Bool("",
		"ADD_LOCATION_INFO", _TL("Add Location Info"),
		_TL("Add location information from points (x,y,(z,m))"),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAdd_Point_Attributes::On_Execute(void)
{
	//-----------------------------------------------------
	bool		bAddLocInfo	= Parameters("ADD_LOCATION_INFO")->asBool();

	//-----------------------------------------------------
	CSG_Shapes	*pInput		= Parameters("INPUT")->asShapes();

	if( !pInput->is_Valid() )
	{
		Error_Set(_TL("Invalid polygon layer."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( !pPoints->is_Valid() )
	{
		Error_Set(_TL("Invalid points layer."));

		return( false );
	}

	CSG_Parameter_Table_Fields	*pFields	= Parameters("FIELDS")->asTableFields();

	if( pFields->Get_Count() == 0 )	// no fields specified, so add all
	{
		CSG_String	sFields;

		for(int iField=0; iField<pPoints->Get_Field_Count(); iField++)
		{
			sFields += CSG_String::Format("%d,", iField);
		}

		pFields->Set_Value(sFields);
	}

	//-----------------------------------------------------
	CSG_Shapes	*pOutput	= Parameters("OUTPUT")->asShapes();

	if( pOutput && pOutput != pInput )
	{
		pOutput->Create(*pInput);
	}
	else
	{
		Parameters("OUTPUT")->Set_Value(pOutput	= pInput);
	}

	pOutput->Fmt_Name("%s_%s", pInput->Get_Name(), pPoints->Get_Name());

	//-----------------------------------------------------
	int	outField	= pOutput->Get_Field_Count();
	int iXField		= -1;

	for(int iField=0; iField<pFields->Get_Count(); iField++)
	{
		int	jField	= pFields->Get_Index(iField);

		pOutput->Add_Field(pPoints->Get_Field_Name(jField), pPoints->Get_Field_Type(jField));
	}

	if( bAddLocInfo )
	{
		pOutput->Add_Field("X", SG_DATATYPE_Float);
		iXField = pOutput->Get_Field_Count() - 1;

		pOutput->Add_Field("Y", SG_DATATYPE_Float);

		if( pPoints->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
		{
			pOutput->Add_Field("Z", SG_DATATYPE_Float);

			if( pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			{
				pOutput->Add_Field("M", SG_DATATYPE_Float);
			}
		}
	}

	//-----------------------------------------------------
	for(sLong iPolygon=0; iPolygon<pOutput->Get_Count() && Set_Progress(iPolygon, pOutput->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pOutput->Get_Shape(iPolygon);

		//-------------------------------------------------
		for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Process_Get_Okay(); iPoint++)
		{
			CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

			if( pPolygon->Contains(pPoint->Get_Point()) )
			{
				for(int iField=0; iField<pFields->Get_Count(); iField++)
				{
					int	jField	= pFields->Get_Index(iField);

					switch( pPoints->Get_Field_Type(jField) )
					{
					case SG_DATATYPE_String:
					case SG_DATATYPE_Date:
						pPolygon->Set_Value(outField + iField, pPoint->asString(jField));
						break;

					default:
						pPolygon->Set_Value(outField + iField, pPoint->asDouble(jField));
						break;
					}
				}

				if( bAddLocInfo )
				{
					pPolygon->Set_Value(iXField    , pPoint->Get_Point().x);
					pPolygon->Set_Value(iXField + 1, pPoint->Get_Point().y);

					if( pPoints->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						pPolygon->Set_Value(iXField + 2, pPoint->Get_Z(0));

						if( pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							pPolygon->Set_Value(iXField + 3, pPoint->Get_M(0));
						}
					}
				}
			}
		} // iPoint
	} // iPolygon

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
