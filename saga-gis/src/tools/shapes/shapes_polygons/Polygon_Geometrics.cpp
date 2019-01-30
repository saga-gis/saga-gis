/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Polygon_Geometrics.cpp
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
#include "Polygon_Geometrics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Geometrics::CPolygon_Geometrics(void)
{
	Set_Name		(_TL("Polygon Properties"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Add general and geometric properties of polygons to its atttributes."
	));

	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"OUTPUT"	, _TL("Polygons with Property Attributes"),
		_TL("If not set property attributes will be added to the orignal layer."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Fields("POLYGONS",
		"FIELDS"	, _TL("Copy Attributes"),
		_TL("Select one or more attributes to be copied to the target layer.")
	);

	Parameters.Add_Bool("",
		"BPARTS"	, _TL("Number of Parts"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"BPOINTS"	, _TL("Number of Vertices"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"BEXTENT"	, _TL("Extent"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"BCENTER"	, _TL("Centroid"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"BLENGTH"	, _TL("Perimeter"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"BAREA"		, _TL("Area"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"SCALING"	, _TL("Scaling"),
		_TL("Scaling factor for perimeter and area (squared). meter to feet = 1 / 0.3048 = 3.2808"),
		1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Geometrics::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("OUTPUT") )
	{
		pParameters->Set_Enabled("FIELDS", pParameter->asPointer() && pParameter->asPointer() != (*pParameters)("POLYGONS")->asPointer());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Geometrics::On_Execute(void)
{
	//-------------------------------------------------
	int	fParts	= Parameters("BPARTS" )->asBool() ? 0 : -1;
	int	fPoints	= Parameters("BPOINTS")->asBool() ? 0 : -1;
	int	fCenter	= Parameters("BCENTER")->asBool() ? 0 : -1;
	int	fExtent	= Parameters("BEXTENT")->asBool() ? 0 : -1;
	int	fLength	= Parameters("BLENGTH")->asBool() ? 0 : -1;
	int	fArea	= Parameters("BAREA"  )->asBool() ? 0 : -1;

	if( fParts && fPoints && fCenter && fExtent && fLength && fArea )
	{
		Error_Set(_TL("no properties selected"));

		return( false );
	}

	//-------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if(	!pPolygons->is_Valid() || pPolygons->Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid or empty polygons layer"));

		return( false );
	}

	//-------------------------------------------------
	if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pPolygons )
	{
		CSG_Shapes *pCopies	= Parameters("OUTPUT")->asShapes();

		pCopies->Create(SHAPE_TYPE_Polygon, pPolygons->Get_Name(), NULL, pPolygons->Get_Vertex_Type());

		CSG_Parameter_Table_Fields	*pFields	= Parameters("FIELDS")->asTableFields();

		for(int Field=0; Field<pFields->Get_Count(); Field++)
		{
			pCopies->Add_Field(
				pPolygons->Get_Field_Name(pFields->Get_Index(Field)),
				pPolygons->Get_Field_Type(pFields->Get_Index(Field))
			);
		}

		for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
		{
			CSG_Shape *pPolygon = pPolygons->Get_Shape(i); CSG_Shape *pCopy = pCopies->Add_Shape(pPolygon, SHAPE_COPY_GEOM);

			for(int Field=0; Field<pFields->Get_Count(); Field++)
			{
				*pCopy->Get_Value(Field)	= *pPolygon->Get_Value(pFields->Get_Index(Field));
			}
		}

		pPolygons	= pCopies;
	}

	//-------------------------------------------------
	if( !fParts  ) { fParts	 = pPolygons->Get_Field_Count(); pPolygons->Add_Field("NPARTS"   , SG_DATATYPE_Int   ); }
	if( !fPoints ) { fPoints = pPolygons->Get_Field_Count(); pPolygons->Add_Field("NPOINTS"  , SG_DATATYPE_Int   ); }
	if( !fCenter ) { fCenter = pPolygons->Get_Field_Count(); pPolygons->Add_Field("CENTER_X" , SG_DATATYPE_Double);
	                                                         pPolygons->Add_Field("CENTER_Y" , SG_DATATYPE_Double); }
	if( !fExtent ) { fExtent = pPolygons->Get_Field_Count(); pPolygons->Add_Field("MIN_X"    , SG_DATATYPE_Double);
	                                                         pPolygons->Add_Field("MAX_X"    , SG_DATATYPE_Double);
															 pPolygons->Add_Field("MIN_Y"    , SG_DATATYPE_Double);
															 pPolygons->Add_Field("MAX_Y"    , SG_DATATYPE_Double); }
	if( !fLength ) { fLength = pPolygons->Get_Field_Count(); pPolygons->Add_Field("PERIMETER", SG_DATATYPE_Double); }
	if( !fArea   ) { fArea   = pPolygons->Get_Field_Count(); pPolygons->Add_Field("AREA"     , SG_DATATYPE_Double); }

	//-------------------------------------------------
	double	Scaling	= Parameters("SCALING")->asDouble();

	//-------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(i);

		if( fParts  >= 0 ) { pPolygon->Set_Value(fParts , pPolygon->Get_Part_Count ()                  ); }
		if( fPoints >= 0 ) { pPolygon->Set_Value(fPoints, pPolygon->Get_Point_Count()                  ); }
		if( fLength >= 0 ) { pPolygon->Set_Value(fLength, pPolygon->Get_Perimeter  () * Scaling        ); }
		if( fArea   >= 0 ) { pPolygon->Set_Value(fArea  , pPolygon->Get_Area       () * Scaling*Scaling); }

		if( fCenter >= 0 )
		{
			TSG_Point	p	= pPolygon->Get_Centroid();

			pPolygon->Set_Value(fCenter + 0, p.x);
			pPolygon->Set_Value(fCenter + 1, p.y);
		}

		if( fExtent >= 0 )
		{
			pPolygon->Set_Value(fExtent + 0, pPolygon->Get_Extent().Get_XMin());
			pPolygon->Set_Value(fExtent + 1, pPolygon->Get_Extent().Get_XMax());
			pPolygon->Set_Value(fExtent + 2, pPolygon->Get_Extent().Get_YMin());
			pPolygon->Set_Value(fExtent + 3, pPolygon->Get_Extent().Get_YMax());
		}
	}

	//-------------------------------------------------
	if( pPolygons == Parameters("POLYGONS")->asShapes() )
	{
		DataObject_Update(pPolygons);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
