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

	Set_Author		(_TL("V.Olaya (c) 2004, O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"General and geometric properties of polygons."
	));

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Polygons with Property Attributes"),
		_TL("If not set property attributes will be added to the orignal layer."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "BPARTS"		, _TL("Number of Parts"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "BPOINTS"		, _TL("Number of Vertices"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "BLENGTH"		, _TL("Perimeter"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "BAREA"		, _TL("Area"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Geometrics::On_Execute(void)
{
	//-------------------------------------------------
	int	bParts	= Parameters("BPARTS")	->asBool() ? 0 : -1;
	int	bPoints	= Parameters("BPOINTS")	->asBool() ? 0 : -1;
	int	bLength	= Parameters("BLENGTH")	->asBool() ? 0 : -1;
	int	bArea	= Parameters("BAREA")	->asBool() ? 0 : -1;

	if( bParts && bPoints && bLength && bArea )
	{
		Error_Set(_TL("no properties selected"));

		return( false );
	}

	//-------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if(	!pPolygons->is_Valid() || pPolygons->Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid lines layer"));

		return( false );
	}

	if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pPolygons )
	{
		pPolygons	= Parameters("OUTPUT")->asShapes();
		pPolygons->Create(*Parameters("POLYGONS")->asShapes());
	}

	//-------------------------------------------------
	if( !bParts )	{	bParts	= pPolygons->Get_Field_Count();	pPolygons->Add_Field(SG_T("NPARTS")   , SG_DATATYPE_Int   );	}
	if( !bPoints )	{	bPoints	= pPolygons->Get_Field_Count();	pPolygons->Add_Field(SG_T("NPOINTS")  , SG_DATATYPE_Int   );	}
	if( !bLength )	{	bLength	= pPolygons->Get_Field_Count();	pPolygons->Add_Field(SG_T("PERIMETER"), SG_DATATYPE_Double);	}
	if( !bArea )	{	bArea	= pPolygons->Get_Field_Count();	pPolygons->Add_Field(SG_T("AREA")     , SG_DATATYPE_Double);	}

	//-------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(i);

		if( bParts  >= 0 )	pPolygon->Set_Value(bParts , pPolygon->Get_Part_Count());
		if( bPoints >= 0 )	pPolygon->Set_Value(bPoints, pPolygon->Get_Point_Count());
		if( bLength >= 0 )	pPolygon->Set_Value(bLength, ((CSG_Shape_Polygon *)pPolygon)->Get_Perimeter());
		if( bArea   >= 0 )	pPolygon->Set_Value(bArea  , ((CSG_Shape_Polygon *)pPolygon)->Get_Area());
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
