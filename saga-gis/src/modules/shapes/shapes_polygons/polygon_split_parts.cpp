/**********************************************************
 * Version $Id: polygon_split_parts.cpp 915 2011-02-15 08:43:36Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                polygon_split_parts.cpp                //
//                                                       //
//                 Copyright (C) 2011 by                 //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "polygon_split_parts.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Split_Parts::CPolygon_Split_Parts(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Parts to Separate Polygons"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Splits parts of multipart polygons into separate polygons. "
		"This can be done only for islands (outer rings) or for all "
		"parts (inner and outer rings) by checking the 'ignore lakes' "
		"option."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "PARTS"		, _TL("Polygon Parts"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "LAKES"		, _TL("Ignore Lakes"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Split_Parts::On_Execute(void)
{
	bool		bIgnoreLakes;
	CSG_Shapes	*pPolygons, *pParts;

	pPolygons		= Parameters("POLYGONS")	->asShapes();
	pParts			= Parameters("PARTS")		->asShapes();
	bIgnoreLakes	= Parameters("LAKES")		->asBool();

	pParts->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pPolygons->Get_Name(), _TL("Parts")), pPolygons);

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPolygons->Get_Count() && Set_Progress(iShape, pPolygons->Get_Count()); iShape++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(iShape);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count() && Process_Get_Okay(); iPart++)
		{
			if( bIgnoreLakes || !((CSG_Shape_Polygon *)pPolygon)->is_Lake(iPart) )
			{
				CSG_Shape	*pPart	= pParts->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

				for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					pPart->Add_Point(pPolygon->Get_Point(iPoint, iPart));
				}

				if( !bIgnoreLakes )
				{
					for(int jPart=0; jPart<pPolygon->Get_Part_Count(); jPart++)
					{
						if(	((CSG_Shape_Polygon *)pPolygon)->is_Lake(jPart)
						&&	((CSG_Shape_Polygon *)pPart)->Contains(pPolygon->Get_Point(0, jPart)) )
						{
							for(int jPoint=0, nPart=pPart->Get_Part_Count(); jPoint<pPolygon->Get_Point_Count(jPart); jPoint++)
							{
								pPart->Add_Point(pPolygon->Get_Point(jPoint, jPart), nPart);
							}
						}
					}
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
