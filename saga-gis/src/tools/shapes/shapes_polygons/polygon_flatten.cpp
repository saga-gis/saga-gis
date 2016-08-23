/**********************************************************
 * Version $Id: polygon_flatten.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygon                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  polygon_flatten.cpp                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "polygon_flatten.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Flatten::CPolygon_Flatten(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Flatten Polygon Layer"));

	Set_Author	("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Removes invalid polygons, i.e. polygons with less than three vertices, "
		"and merges polygons belonging spatially together, i.e. forming outer "
		"and inner rings. Inner rings are not preserved as separate polygon, "
		"but become new part of the polygon forming the outer ring. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Flatten::On_Execute(void)
{
	CSG_Shapes	*pShapes	= Parameters("INPUT")->asShapes();

	if( !pShapes->is_Valid() || pShapes->Get_Count() <= 1 )
	{
		Message_Add(_TL("less than two polygons in layer, nothing to do!"));

		return( false );
	}

	if( Parameters("OUTPUT")->asShapes() != NULL && pShapes != Parameters("OUTPUT")->asShapes() )
	{
		pShapes	= Parameters("OUTPUT")->asShapes();
		pShapes->Create(*Parameters("INPUT")->asShapes());
		pShapes->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("dissolved inner rings")));
	}

	//-----------------------------------------------------
	int		i, j, k, n;

	int		*Container	= (int *)SG_Malloc(pShapes->Get_Count() * sizeof(int));

	//-----------------------------------------------------
	Process_Set_Text(_TL("find inner rings"));

	for(i=0, n=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(i);

		if( !pShape->is_Valid() )
		{
			Container[i]	= -2;
		}
		else
		{
			for(j=0, k=-1; k<0 && j<pShapes->Get_Count(); j++)
			{
				if( j > i || (j < i && Container[j] != i) )
				{
					CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pShapes->Get_Shape(j);

					if( pPolygon->Intersects(pShape) == INTERSECTION_Contains )
					{
						k	= j;
						n	++;
					}
				}
			}

			Container[i]	= k;
		}
	}

	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("number of inner rings"), n));

	if( n == 0 )
	{
		SG_Free(Container);

		return( true );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("solve inner rings"));

	for(i=0, n=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		if( Container[i] >= 0 )
		{
			j	= Container[i];

			while( Container[j] >= 0 )
			{
				j	= Container[j];
			}

			CSG_Shape	*pOuter	= pShapes->Get_Shape(j);
			CSG_Shape	*pInner	= pShapes->Get_Shape(i);

			for(int iPart=0; iPart<pInner->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0, jPart=pOuter->Get_Part_Count(); iPoint<pInner->Get_Point_Count(iPart); iPoint++)
				{
					pOuter->Add_Point(pInner->Get_Point(iPoint, iPart), jPart);
				}
			}

			pInner->Del_Parts();	// save memory

			n++;
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("clean inner rings"));

	for(i=0, j=pShapes->Get_Count()-1; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++, j--)
	{
		if( Container[j] != -1 )	// valid outer ring
		{
			pShapes->Del_Shape(j);
		}
	}

	//-----------------------------------------------------
	SG_Free(Container);

	if( pShapes == Parameters("INPUT")->asShapes() )
	{
		DataObject_Update(pShapes);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
