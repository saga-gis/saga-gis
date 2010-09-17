
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Points                  //
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
#include "GSPoints_Pattern_Analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Pattern_Analysis::CGSPoints_Pattern_Analysis(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Spatial Point Pattern Analysis"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description(
		_TL("Basic measures for spatial point patterns.")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "CENTRE"		, _TL("Mean Centre"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "STDDIST"		, _TL("Standard Distance"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		pNode	, "STEP"		, _TL("Vertex Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 5.0, 0.1, true, 20.0, true
	);

	Parameters.Add_Shapes(
		NULL	, "BBOX"		, _TL("Bounding Box"),
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
bool CGSPoints_Pattern_Analysis::On_Execute(void)
{
	int						iPoint;
	double					StdDist;
	CSG_Shape				*pShape;
	CSG_Shapes				*pPoints, *pShapes;
	CSG_Simple_Statistics	X, Y, D;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")	->asShapes();

	if( pPoints->Get_Count() <= 1 )
	{
		Error_Set(_TL("not enough points to perform pattern analysis"));

		return( false );
	}

	//-----------------------------------------------------
	for(iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

		X.Add_Value(p.x);
		Y.Add_Value(p.y);
	}

	if( X.Get_Range() == 0.0 && Y.Get_Range() == 0.0 )
	{
		Error_Set(_TL("no variation in point pattern"));

		return( false );
	}

	//-----------------------------------------------------
	StdDist	= 0.0;

	for(iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

		D.Add_Value(SG_Get_Distance(X.Get_Mean(), Y.Get_Mean(), p.x, p.y));

		StdDist	+= SG_Get_Square(p.x - X.Get_Mean()) + SG_Get_Square(p.y - Y.Get_Mean());
	}

	StdDist	= sqrt(StdDist / D.Get_Count());

	//-----------------------------------------------------
	pShapes	= Parameters("CENTRE")	->asShapes();

	pShapes	->Create(SHAPE_TYPE_Point	, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Centre")));
	pShapes	->Add_Field(SG_T("X_CENTRE"), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("Y_CENTRE"), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("N_POINTS"), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("STDDIST" ), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("MEANDIST"), SG_DATATYPE_Double);

	pShape	= pShapes->Add_Shape();

	pShape	->Set_Value(0, X.Get_Mean());
	pShape	->Set_Value(1, Y.Get_Mean());
	pShape	->Set_Value(2, D.Get_Count());
	pShape	->Set_Value(3, StdDist);
	pShape	->Set_Value(4, D.Get_StdDev());

	pShape	->Add_Point(X.Get_Mean(), Y.Get_Mean());

	//-----------------------------------------------------
	pShapes	= Parameters("STDDIST")	->asShapes();

	pShapes	->Create(SHAPE_TYPE_Polygon	, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Standard Distance")));
	pShapes	->Add_Field(SG_T("X_CENTRE"), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("Y_CENTRE"), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("N_POINTS"), SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("STDDIST") , SG_DATATYPE_Double);

	pShape	= pShapes->Add_Shape();

	pShape	->Set_Value(0, X.Get_Mean());
	pShape	->Set_Value(1, Y.Get_Mean());
	pShape	->Set_Value(2, X.Get_Count());
	pShape	->Set_Value(3, StdDist);

	double	dTheta	= Parameters("STEP")->asDouble() * M_DEG_TO_RAD;

	for(double Theta=0.0; Theta<=M_PI_360; Theta+=dTheta)
	{
		pShape	->Add_Point(
			X.Get_Mean() + StdDist * cos(Theta),
			Y.Get_Mean() + StdDist * sin(Theta)
		);
	}

	//-----------------------------------------------------
	pShapes	= Parameters("BBOX")	->asShapes();

	pShapes	->Create(SHAPE_TYPE_Polygon	, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Bounding Box")));

	pShapes	->Add_Field(SG_T("XMIN")	, SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("XMAX")	, SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("YMIN")	, SG_DATATYPE_Double);
	pShapes	->Add_Field(SG_T("YMAX")	, SG_DATATYPE_Double);

	pShape	= pShapes->Add_Shape();

	pShape	->Set_Value(0, X.Get_Minimum());
	pShape	->Set_Value(1, X.Get_Maximum());
	pShape	->Set_Value(2, Y.Get_Minimum());
	pShape	->Set_Value(3, Y.Get_Maximum());

	pShape	->Add_Point(X.Get_Minimum(), Y.Get_Minimum());
	pShape	->Add_Point(X.Get_Minimum(), Y.Get_Maximum());
	pShape	->Add_Point(X.Get_Maximum(), Y.Get_Maximum());
	pShape	->Add_Point(X.Get_Maximum(), Y.Get_Minimum());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
