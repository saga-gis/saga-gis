/**********************************************************
 * Version $Id: GSPoints_Pattern_Analysis.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   statistics_points                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSPoints_Pattern_Analysis.cpp            //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                      Olaf Conrad                      //
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
#include "GSPoints_Pattern_Analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Pattern_Analysis::CGSPoints_Pattern_Analysis(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Spatial Point Pattern Analysis"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description(
		_TL("Basic measures for spatial point patterns.")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"WEIGHT"	, _TL("Weight"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes("",
		"CENTRE"	, _TL("Mean Centre"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"STDDIST"	, _TL("Standard Distance"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double("STDDIST",
		"STEP"		, _TL("Vertex Distance [Degree]"),
		_TL(""),
		5.0, 0.1, true, 20.0, true
	);

	Parameters.Add_Shapes("",
		"BBOX"		, _TL("Bounding Box"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSPoints_Pattern_Analysis::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( pPoints->Get_Count() <= 1 )
	{
		Error_Set(_TL("not enough points to perform pattern analysis"));

		return( false );
	}

	int	iPoint, Weight	= Parameters("WEIGHT")->asInt();

	CSG_Simple_Statistics	X, Y, D;

	//-----------------------------------------------------
	for(iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

		double	w	= Weight < 0 ? 1.0 : pPoints->Get_Shape(iPoint)->asDouble(Weight);

		X.Add_Value(p.x, w);
		Y.Add_Value(p.y, w);
	}

	if( X.Get_Range() == 0.0 && Y.Get_Range() == 0.0 )
	{
		Error_Set(_TL("no variation in point pattern"));

		return( false );
	}

	//-----------------------------------------------------
	double	StdDist	= 0.0;

	for(iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

		double	w	= Weight < 0 ? 1.0 : pPoints->Get_Shape(iPoint)->asDouble(Weight);

		D.Add_Value(SG_Get_Distance(X.Get_Mean(), Y.Get_Mean(), p.x, p.y), w);

		StdDist	+= w * (SG_Get_Square(p.x - X.Get_Mean()) + SG_Get_Square(p.y - Y.Get_Mean()));
	}

	if( D.Get_Weights() == 0.0 )
	{
		Error_Set(_TL("number of valid points or sum of weights equals zero"));

		return( false );
	}

	StdDist	= sqrt(StdDist / D.Get_Weights());

	//-----------------------------------------------------
	CSG_Shapes	*pShapes;	CSG_Shape	*pShape;

	pShapes	= Parameters("CENTRE")->asShapes();

	pShapes	->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("Centre")));
	pShapes	->Add_Field("X_CENTRE", SG_DATATYPE_Double);
	pShapes	->Add_Field("Y_CENTRE", SG_DATATYPE_Double);
	pShapes	->Add_Field("N_POINTS", SG_DATATYPE_Double);
	pShapes	->Add_Field("STDDIST" , SG_DATATYPE_Double);
	pShapes	->Add_Field("MEANDIST", SG_DATATYPE_Double);

	pShape	= pShapes->Add_Shape();

	pShape	->Set_Value(0, X.Get_Mean  ());
	pShape	->Set_Value(1, Y.Get_Mean  ());
	pShape	->Set_Value(2, D.Get_Count ());
	pShape	->Set_Value(3, StdDist       );
	pShape	->Set_Value(4, D.Get_StdDev());

	pShape	->Add_Point(X.Get_Mean(), Y.Get_Mean());

	//-----------------------------------------------------
	pShapes	= Parameters("STDDIST")->asShapes();

	pShapes	->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("Standard Distance")));
	pShapes	->Add_Field("X_CENTRE", SG_DATATYPE_Double);
	pShapes	->Add_Field("Y_CENTRE", SG_DATATYPE_Double);
	pShapes	->Add_Field("N_POINTS", SG_DATATYPE_Double);
	pShapes	->Add_Field("STDDIST" , SG_DATATYPE_Double);

	pShape	= pShapes->Add_Shape();

	pShape	->Set_Value(0, X.Get_Mean ());
	pShape	->Set_Value(1, Y.Get_Mean ());
	pShape	->Set_Value(2, X.Get_Count());
	pShape	->Set_Value(3, StdDist      );

	double	dTheta	= Parameters("STEP")->asDouble() * M_DEG_TO_RAD;

	for(double Theta=0.0; Theta<=M_PI_360; Theta+=dTheta)
	{
		pShape->Add_Point(
			X.Get_Mean() + StdDist * cos(Theta),
			Y.Get_Mean() + StdDist * sin(Theta)
		);
	}

	//-----------------------------------------------------
	pShapes	= Parameters("BBOX")->asShapes();

	pShapes	->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("Bounding Box")));

	pShapes	->Add_Field("XMIN", SG_DATATYPE_Double);
	pShapes	->Add_Field("XMAX", SG_DATATYPE_Double);
	pShapes	->Add_Field("YMIN", SG_DATATYPE_Double);
	pShapes	->Add_Field("YMAX", SG_DATATYPE_Double);

	pShape	= pShapes->Add_Shape();

	pShape	->Set_Value(0, X.Get_Minimum());
	pShape	->Set_Value(1, X.Get_Maximum());
	pShape	->Set_Value(2, Y.Get_Minimum());
	pShape	->Set_Value(3, Y.Get_Maximum());

	pShape	->Add_Point(X.Get_Minimum(), Y.Get_Minimum());
	pShape	->Add_Point(X.Get_Minimum(), Y.Get_Maximum());
	pShape	->Add_Point(X.Get_Maximum(), Y.Get_Maximum());
	pShape	->Add_Point(X.Get_Maximum(), Y.Get_Minimum());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
