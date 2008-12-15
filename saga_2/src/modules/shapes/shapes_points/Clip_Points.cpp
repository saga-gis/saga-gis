
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Clip_Points.cpp                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Clip_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CClip_Points::CClip_Points(void)
{
	Set_Name		(_TL("Clip Points with Polygons"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Add Attribute to Clipped Points"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes_List(
		NULL	, "CLIPS"		, _TL("Clipped Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Clipping Options"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("one layer for all points"),
			_TL("separate layer for each polygon")
		), 0
	);
}

//---------------------------------------------------------
CClip_Points::~CClip_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CClip_Points::On_Execute(void)
{
	int							Method, iField;
	CSG_Shapes					*pPoints, *pPolygons, *pClip;
	CSG_Parameter_Shapes_List	*pClips;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	pPolygons	= Parameters("POLYGONS")	->asShapes();
	pClips		= Parameters("CLIPS")		->asShapesList();
	Method		= Parameters("METHOD")		->asInt();
	iField		= Parameters("FIELD")		->asInt();

	//-----------------------------------------------------
	if( !pPoints->is_Valid() )
	{
		Message_Add(_TL("Invalid points layer."));

		return( false );
	}
	else if( !pPolygons->is_Valid() )
	{
		Message_Add(_TL("Invalid polygon layer."));

		return( false );
	}

	//-----------------------------------------------------
	if( iField >= pPolygons->Get_Field_Count() )
	{
		iField	= -1;
	}

	pClips->Del_Items();

	if( Method == 0 )
	{
		pClip	= SG_Create_Shapes(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), pPolygons->Get_Name()), pPoints);

		if( iField >= 0 )
		{
			pClip->Add_Field(pPolygons->Get_Field_Name(iField), pPolygons->Get_Field_Type(iField));
		}
	}

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		if( Method == 1 )
		{
			CSG_String	Name(pPoints->Get_Name());

			Name	+= iField >= 0
					? CSG_String::Format(SG_T(" [%s]"), pPolygon->asString(iField))
					: CSG_String::Format(SG_T(" [%00d]"), 1 + pClips->Get_Count());

			pClip	= SG_Create_Shapes(SHAPE_TYPE_Point, Name, pPoints);

			if( iField >= 0 )
			{
				pClip->Add_Field(pPolygons->Get_Field_Name(iField), pPolygons->Get_Field_Type(iField));
			}
		}

		for(int iPoint=0; iPoint<pPoints->Get_Count() && Process_Get_Okay(false); iPoint++)
		{
			CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

			if( pPolygon->is_Containing(pPoint->Get_Point(0)) )
			{
				pPoint	= pClip->Add_Shape(pPoint, SHAPE_COPY_ATTR);

				if( iField >= 0 )
				{
					pPoint->Set_Value(pPoints->Get_Field_Count(), pPolygon->asString(iField));
				}
			}
		}

		if( Method == 1 )
		{
			if( pClip->Get_Count() > 0 )
			{
				pClips->Add_Item(pClip);
			}
			else
			{
				delete(pClip);
			}
		}
	}

	//-----------------------------------------------------
	if( Method == 0 )
	{
		if( pClip->Get_Count() > 0 )
		{
			pClips->Add_Item(pClip);
		}
		else
		{
			delete(pClip);
		}
	}

	return( pClips->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
