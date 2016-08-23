/**********************************************************
 * Version $Id: thiessen_polygons.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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

	Parameters.Add_Value(
		NULL	, "FRAME"		, _TL("Frame Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 0.0, true
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
		Error_Set(_TL("TIN creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	int	nNodes	= TIN.Get_Node_Count();

	CSG_Rect	r(TIN.Get_Extent());

	r.Inflate(Parameters("FRAME")->asDouble(), false);

	//-----------------------------------------------------
	CSG_Shapes	Frame(SHAPE_TYPE_Polygon);
	CSG_Shape	*pFrame	= Frame.Add_Shape();

	pFrame->Add_Point(r.Get_XMin(), r.Get_YMin());
	pFrame->Add_Point(r.Get_XMin(), r.Get_YMax());
	pFrame->Add_Point(r.Get_XMax(), r.Get_YMax());
	pFrame->Add_Point(r.Get_XMax(), r.Get_YMin());

	//-----------------------------------------------------
	r.Inflate(3 * r.Get_XRange(), 3 * r.Get_YRange(), false);

	TIN.Add_Node(CSG_Point(r.Get_XCenter(), r.Get_YMin   ()), NULL, false);
	TIN.Add_Node(CSG_Point(r.Get_XMax   (), r.Get_YCenter()), NULL, false);
	TIN.Add_Node(CSG_Point(r.Get_XCenter(), r.Get_YMax   ()), NULL, false);
	TIN.Add_Node(CSG_Point(r.Get_XMin   (), r.Get_YCenter()), NULL, false);

	TIN.Update();

	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	pPolygons->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), TIN.Get_Name(), _TL("Thiessen Polygons")), &TIN);

	//-----------------------------------------------------
	for(int iNode=0; iNode<nNodes && Set_Progress(iNode, nNodes); iNode++)
	{
		CSG_Points	Points;

		if( TIN.Get_Node(iNode)->Get_Polygon(Points) )
		{
			CSG_Shape	*pPolygon	= pPolygons->Add_Shape(TIN.Get_Record(iNode), SHAPE_COPY_ATTR);

			for(int iPoint=0; iPoint<Points.Get_Count(); iPoint++)
			{
				pPolygon->Add_Point(Points[iPoint]);
			}

			SG_Polygon_Intersection(pPolygon, pFrame);
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
