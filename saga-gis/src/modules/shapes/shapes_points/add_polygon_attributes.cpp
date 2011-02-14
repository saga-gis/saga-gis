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
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               add_polygon_attributes.cpp              //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "add_polygon_attributes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAdd_Polygon_Attributes::CAdd_Polygon_Attributes(void)
{
	Set_Name		(_TL("Add Polygon Attributes to Points"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "INPUT"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("Attribute to add. Select none to add all"),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAdd_Polygon_Attributes::On_Execute(void)
{
	int			inField, outField;
	CSG_Shapes	*pInput, *pOutput, *pPolygons;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT")		->asShapes();
	pOutput		= Parameters("OUTPUT")		->asShapes();
	pPolygons	= Parameters("POLYGONS")	->asShapes();
	inField		= Parameters("FIELD")		->asInt();

	//-----------------------------------------------------
	if( !pInput->is_Valid() )
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
	if( pOutput && pOutput != pInput )
	{
		pOutput->Create(*pInput);
	}
	else
	{
		Parameters("RESULT")->Set_Value(pOutput	= pInput);
	}

	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), pPolygons->Get_Name()));

	//-----------------------------------------------------
	outField	= pOutput->Get_Field_Count();

	if( inField >= 0 && inField < pPolygons->Get_Field_Count() )
	{	// add single attribute
		pOutput->Add_Field(pPolygons->Get_Field_Name(inField), pPolygons->Get_Field_Type(inField));
	}
	else
	{	// add all attributes
		inField	= -1;

		for(int iField=0; iField<pPolygons->Get_Field_Count(); iField++)
		{
			pOutput->Add_Field(pPolygons->Get_Field_Name(iField), pPolygons->Get_Field_Type(iField));
		}
	}

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<pOutput->Get_Count() && Set_Progress(iPoint, pOutput->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint		= pOutput	->Get_Shape(iPoint);
		CSG_Shape	*pPolygon	= pPolygons	->Get_Shape(pPoint->Get_Point(0));

		if( pPolygon )
		{
			if( inField >= 0 )
			{	// add single attribute
				pPoint->Set_Value(outField, pPolygon->asString(inField));
			}
			else
			{	// add all attributes
				for(int iField=0; iField<pPolygons->Get_Field_Count(); iField++)
				{
					switch( pPolygons->Get_Field_Type(iField) )
					{
					case SG_DATATYPE_String:
					case SG_DATATYPE_Date:
						pPoint->Set_Value(outField + iField, pPolygon->asString(iField));
						break;

					default:
						pPoint->Set_Value(outField + iField, pPolygon->asDouble(iField));
						break;
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
