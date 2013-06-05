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

	CSG_Parameter	*pNode	= Parameters.Add_Choice(
		NULL	, "FRAME"		, _TL("Add Frame"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("no"),
			_TL("four points"),
			_TL("eight points")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "FRAME_SIZE"	, _TL("Frame Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.5, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CThiessen_Polygons::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FRAME")) )
	{
		pParameters->Get_Parameter("FRAME_SIZE")->Set_Enabled(pParameter->asInt() > 0 );
	}

	return( 1 );
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
	int	nNodes	= TIN.Get_Node_Count();

	if( Parameters("FRAME")->asInt() > 0 )
	{
		CSG_Rect	Frame(TIN.Get_Extent());

		Frame.Inflate(100.0 * Parameters("FRAME_SIZE")->asDouble());

		TIN.Add_Node(CSG_Point(Frame.Get_XMin(), Frame.Get_YMin()), NULL, false);
		TIN.Add_Node(CSG_Point(Frame.Get_XMin(), Frame.Get_YMax()), NULL, false);
		TIN.Add_Node(CSG_Point(Frame.Get_XMax(), Frame.Get_YMax()), NULL, false);
		TIN.Add_Node(CSG_Point(Frame.Get_XMax(), Frame.Get_YMin()), NULL, false);

		if( Parameters("FRAME")->asInt() > 1 )
		{
			Frame.Inflate(41.42);

			TIN.Add_Node(CSG_Point(Frame.Get_XMin()   , Frame.Get_YCenter()), NULL, false);
			TIN.Add_Node(CSG_Point(Frame.Get_XMax()   , Frame.Get_YCenter()), NULL, false);
			TIN.Add_Node(CSG_Point(Frame.Get_XCenter(), Frame.Get_YMin())   , NULL, false);
			TIN.Add_Node(CSG_Point(Frame.Get_XCenter(), Frame.Get_YMax())   , NULL, false);
		}

		TIN.Update();
	}

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
