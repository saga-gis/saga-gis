/**********************************************************
 * Version $Id: thiessen_polygons.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
//                 thiessen_polygons.cpp                 //
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
#include "thiessen_polygons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CThiessen_Polygons::CThiessen_Polygons(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Thiessen Polygons"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description(_TW(
		"Creates Thiessen or Voronoi polygons for given point data set."
	));


	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CThiessen_Polygons::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_TIN	TIN;

	if( !TIN.Create(Parameters("POINTS")->asShapes()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	pPolygons->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), TIN.Get_Name(), _TL("Thiessen Polygons")), &TIN);

	//-----------------------------------------------------
	for(int iNode=0; iNode<TIN.Get_Node_Count() && Set_Progress(iNode, TIN.Get_Node_Count()); iNode++)
	{
		CSG_Points	Points;

		if( TIN.Get_Node(iNode)->Get_Polygon(Points) )
		{
			CSG_Shape	*pPolygon	= pPolygons->Add_Shape(TIN.Get_Record(iNode), SHAPE_COPY_ATTR);

			for(int iPoint=0; iPoint<Points.Get_Count(); iPoint++)
			{
				pPolygon->Add_Point(Points[iPoint]);
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
