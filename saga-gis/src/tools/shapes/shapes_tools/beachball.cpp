
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    beachball.cpp                      //
//                                                       //
//                  Olaf Conrad (C) 2018                 //
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
#include "beachball.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBeachball::CBeachball(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Focal Mechanism (Beachball Plots)"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"This tool creates a new polygon layer with beachball plots as representation of focal mechanism solution data. "
		"Focal mechanism data of earthquakes have to be supplied as strike and dip angles of the fault plane "
		"and rake angle (slip vector) as deviation from the strike angle. "
	));

	Add_Reference(
		"Cronin, V.", "2010",
		"A Primer on Focal Mechanism Solutions for Geologists",
		"Baylor University.",
		SG_T("http://serc.carleton.edu/files/NAGTWorkshops/structure04/Focal_mechanism_primer.pdf"), SG_T("pdf")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"STRIKE"	, _TL("Strike"),
		_TL("")
	);

	Parameters.Add_Table_Field("POINTS",
		"DIP"		, _TL("Dip"),
		_TL("")
	);

	Parameters.Add_Table_Field("POINTS",
		"RAKE"		, _TL("Rake"),
		_TL("")
	);

	Parameters.Add_Table_Field("POINTS",
		"SIZE"		, _TL("Size"),
		_TL(""),
		true
	);

	Parameters.Add_Double("SIZE",
		"SIZE_DEF"	, _TL("Default"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Range("SIZE",
		"SIZE_RANGE", _TL("Scale to..."),
		_TL(""),
		1.0, 10.0, 0.0, true
	);

	Parameters.Add_Shapes("",
		"PLOTS"		, _TL("Focal Mechanism Beachballs"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double("",
		"DARC"		, _TL("Arc Vertex Distance [Degree]"),
		_TL(""),
		5.0, 0.1, true, 10.0, true
	);

	Parameters.Add_Choice("",
		"STYLE"		, _TL("Style"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("one"),
			_TL("two")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CBeachball::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SIZE") )
	{
		if( (*pParameters)("POINTS")->asShapes() )
		{
			bool	None	= pParameter->asInt() < 0;

			pParameters->Set_Enabled("SIZE_DEF"  , None ==  true);
			pParameters->Set_Enabled("SIZE_RANGE", None == false);
		}
		else
		{
			pParameters->Set_Enabled("SIZE_DEF"  , false);
			pParameters->Set_Enabled("SIZE_RANGE", false);
		}
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBeachball::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();
	CSG_Shapes	*pPlots		= Parameters("PLOTS" )->asShapes();

	pPlots->Create(SHAPE_TYPE_Polygon, _TL("Focal Mechanism Plots"), pPoints, pPoints->Get_Vertex_Type());

	int	fStrike	= Parameters("STRIKE")->asInt();
	int	fDip	= Parameters("DIP"   )->asInt();
	int	fRake	= Parameters("RAKE"  )->asInt();
	int	fSize	= Parameters("SIZE"  )->asInt();

	double	Scale_Min, Scale_Range;

	if( fSize < 0 || pPoints->Get_Range(fSize) <= 0.0 )
	{
		Scale_Min	= Parameters("SIZE_DEF")->asDouble();
		Scale_Range	= 0.0;
	}
	else
	{
		Scale_Min	=  Parameters("SIZE_RANGE")->asRange()->Get_LoVal();
		Scale_Range	= (Parameters("SIZE_RANGE")->asRange()->Get_HiVal() - Scale_Min) / pPoints->Get_Range(fSize);
	}

	m_dArc	= Parameters("DARC" )->asDouble();
	m_Style	= Parameters("STYLE")->asInt();

	//-----------------------------------------------------
	CSG_Shapes	Circle(SHAPE_TYPE_Polygon);

	m_pCircle	= (CSG_Shape_Polygon *)Circle.Add_Shape();

	for(double a=0.0; a<M_PI_360; a+=m_dArc*M_DEG_TO_RAD)
	{
		m_pCircle->Add_Point(sin(a), cos(a));
	}

	//-----------------------------------------------------
	for(int i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		double	Strike	= pPoint->asDouble(fStrike) * M_DEG_TO_RAD;
		double	Dip		= pPoint->asDouble(fDip   ) * M_DEG_TO_RAD;
		double	Rake	= pPoint->asDouble(fRake  ) * M_DEG_TO_RAD;

		double	Size	= Scale_Range <= 0.0 ? Scale_Min : Scale_Min + Scale_Range * (pPoint->asDouble(fSize) - pPoints->Get_Minimum(fSize));

		Set_Plot(pPlots->Add_Shape(pPoint, SHAPE_COPY_ATTR), pPoint->Get_Point(0), Size, Strike, Dip, Rake);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBeachball::Set_Plot(CSG_Shape *pPlot, const TSG_Point &Center, double Scale, double Strike, double Dip, double Rake)
{
	//-----------------------------------------------------
	CSG_Shapes	Plot(SHAPE_TYPE_Polygon);

	CSG_Vector	N(3);

	N[0] = 0; N[1] = 0; N[2] = 1;

	SG_VectorR3_Rotate(N, 1, Dip);
	SG_VectorR3_Rotate(N, 2, Strike);

	Get_Plane(Plot.Add_Shape(), N);

	N[0] = 0; N[1] = -1; N[2] = 0;

	Rake = fmod(Rake, M_PI_360); if( Rake < -M_PI_180 ) Rake += M_PI_360; else if( Rake > M_PI_180 ) Rake -= M_PI_360;

	SG_VectorR3_Rotate(N, 2, -Rake);
	SG_VectorR3_Rotate(N, 1, Dip);
	SG_VectorR3_Rotate(N, 2, Strike);

	Get_Plane(Plot.Add_Shape(), N);

	//-----------------------------------------------------
	SG_Polygon_Intersection(m_pCircle, Plot.Get_Shape(0), Plot.Add_Shape());
	SG_Polygon_Difference  (m_pCircle, Plot.Get_Shape(0), Plot.Add_Shape());

	SG_Polygon_Intersection(Plot.Get_Shape(2), Plot.Get_Shape(1), Plot.Add_Shape());
	SG_Polygon_Difference  (Plot.Get_Shape(2), Plot.Get_Shape(1), Plot.Add_Shape());

	SG_Polygon_Intersection(Plot.Get_Shape(3), Plot.Get_Shape(1), Plot.Add_Shape());
	SG_Polygon_Difference  (Plot.Get_Shape(3), Plot.Get_Shape(1), Plot.Add_Shape());

	//-----------------------------------------------------
	int	p1	= Rake < 0.0 ? 5 : 4;
	int	p2	= Rake < 0.0 ? 6 : 7;

	switch( m_Style )
	{
	default:
		pPlot->Add_Part(m_pCircle->Get_Part(0));

		SG_Polygon_Offset(Plot.Get_Shape(p1), -0.01, m_dArc); pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(p1))->Get_Part(0));
		SG_Polygon_Offset(Plot.Get_Shape(p2), -0.01, m_dArc); pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(p2))->Get_Part(0));
		break;

	case  1:
		pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(4))->Get_Part(0));
		pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(5))->Get_Part(0));
		pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(6))->Get_Part(0));
		pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(7))->Get_Part(0));

		SG_Polygon_Offset(Plot.Get_Shape(p1), -0.01, m_dArc); pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(p1))->Get_Part(0));
		SG_Polygon_Offset(Plot.Get_Shape(p2), -0.01, m_dArc); pPlot->Add_Part(((CSG_Shape_Polygon *)Plot.Get_Shape(p2))->Get_Part(0));
		break;
	}

	//-----------------------------------------------------
	return( Get_Scaled(pPlot, Center, Scale) );
}

//---------------------------------------------------------
bool CBeachball::Get_Plane(CSG_Shape *pPlane, const CSG_Vector &Normal)
{
	CSG_Vector	Ez(3);	Ez[2]	=  1.0;

	double	Slope	= Normal.Get_Angle(Ez);
	double	Azimuth	= M_PI_090 + atan2(Normal[1], Normal[0]);

	if( Slope > M_PI_090 )
	{
		Azimuth	+= M_PI_180;
		Slope	 = M_PI_090 - (Slope - M_PI_090);
	}

	TSG_Point	A, C;

	A.x	= sin(Azimuth - M_PI_090);	// strike
	A.y	= cos(Azimuth - M_PI_090);

	if( Slope < M_PI_090 )
	{
		double	d	= -2. * tan(Slope / 2.);	// stereographic projection

		C.x	= d * sin(Azimuth);
		C.y	= d * cos(Azimuth);

		pPlane->Add_Part(m_pCircle->Get_Part(0));

		Get_Scaled(pPlane, C, SG_Get_Distance(A, C));
	}
	else
	{
		A.x	*= 1.1;	A.y	*= 1.1;

		pPlane->Add_Point(-A.x, -A.y);
		pPlane->Add_Point( A.x,  A.y);
		pPlane->Add_Point( A.x - A.y,  A.y + A.x);
		pPlane->Add_Point(-A.x - A.y, -A.y + A.x);
	}

	return( true );
}

//---------------------------------------------------------
bool CBeachball::Get_Scaled(CSG_Shape *pShape, const TSG_Point &Center, double Scale)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

			Point.x	= Center.x + Scale * Point.x;
			Point.y	= Center.y + Scale * Point.y;

			pShape->Set_Point(Point, iPoint, iPart);
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
