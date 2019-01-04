/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    Shapes_Polygon                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Shape_Index.cpp                     //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "shape_index.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShape_Index::CShape_Index(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Shape Indices"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"The tool calculates various indices describing the shape of polygons, mostly based on "
		"area, perimeter and maximum diameter. If the optional output 'Shape Indices' "
		"is not created, the tool attaches the attributes to the input dataset. "
		"Otherwise a new dataset is created and attributes existing in the input "
		"dataset are dropped."
		"<ul>"
		"<li><b>A</b> area</li>"
		"<li><b>P</b> perimeter</li>"
		"<li><b>P/A</b> interior edge ratio</li>"
		"<li><b>P/sqrt(A)</b></li>"
		"<li><b>Deqpc</b> equivalent projected circle diameter (=2*sqrt(A/pi))</li>"
		"<li><b>Sphericity</b> the ratio of the perimeter of the equivalent circle to the real perimeter (=P/(2*sqrt(A*pi)))</li>"
		"<li><b>Shape Index</b> the inverse of the spericity</li>"
		"<li><b>Dmax</b> maximum diameter calculated as maximum distance between two polygon part's vertices</li>"
		"<li><b>DmaxDir</b> direction of maximum diameter</li>"
		"<li><b>Dmax/A</b></li>"
		"<li><b>Dmax/sqrt(A)</b></li>"
		"<li><b>Dgyros</b> diameter of gyration, calculated as twice the maximum vertex distance to its polygon part's centroid</li>"
		"<li><b>Fmax</b> maximum Feret diameter</li>"
		"<li><b>FmaxDir</b> direction of the maximum Feret diameter</li>"
		"<li><b>Fmin</b> minimum Feret diameter</li>"
		"<li><b>FminDir</b> direction of the minimum Feret diameter</li>"
		"<li><b>Fmean</b> mean Feret diameter</li>"
		"<li><b>Fmax90</b> the Feret diameter measured at an angle of 90 degrees to that of the Fmax direction</li>"
		"<li><b>Fmin90</b> the Feret diameter measured at an angle of 90 degrees to that of the Fmin direction</li>"
		"<li><b>Fvol</b> the diameter of a sphere having the same volume as the cylinder constructed by Fmin as the cylinder diameter and Fmax as its length</li>"
		"</ul>"
	));

	Add_Reference("Lang, S. & Blaschke, T.", "2007",
		"Landschaftsanalyse mit GIS",
		"Stuttgart."
	);

	Add_Reference("Forman, R.T.T. & Godron, M.", "1986",
		"Landscape Ecology",
		"Cambridge."
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES", _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"INDEX"	, _TL("Shape Indices"),
		_TL("Polygon shapefile with the calculated indices."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"DMAX"	, _TL("Maximum Diameter"),
		_TL("Line shapefile showing the maximum diameter."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Bool("",
		"GYROS"	, _TL("Diameter of Gyration"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"FERET"	, _TL("Feret Diameters"),
		_TL(""),
		false
	);

	Parameters.Add_Int("FERET",
		"FERET_DIRS"	, _TL("Number of Directions"),
		_TL("Number of directions (0-90) to be analyzed."),
		18, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShape_Index::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FERET") )
	{
		pParameters->Set_Enabled("FERET_DIRS", pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShape_Index::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons = Parameters("SHAPES")->asShapes();

	if( !pPolygons->is_Valid() )
	{
		Error_Set(_TL("invalid polygons layer"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("INDEX")->asShapes() && Parameters("INDEX")->asShapes() != pPolygons )
	{
		CSG_Shapes	*pTarget   = Parameters("INDEX")->asShapes();

		pTarget->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Shape Indices")));

		pTarget->Add_Field("ID", SG_DATATYPE_Int);

		for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
		{
			pTarget->Add_Shape(pPolygons->Get_Shape(iPolygon), SHAPE_COPY)->Set_Value(0, iPolygon);
		}

		pPolygons	= pTarget;
	}

	//-----------------------------------------------------
	int	offIndices	= pPolygons->Get_Field_Count();

	pPolygons->Add_Field(_TL("A"           ), SG_DATATYPE_Double);	//  0
	pPolygons->Add_Field(_TL("P"           ), SG_DATATYPE_Double);	//  1
	pPolygons->Add_Field(_TL("P/A"         ), SG_DATATYPE_Double);	//  2
	pPolygons->Add_Field(_TL("P/sqrt(A)"   ), SG_DATATYPE_Double);	//  3
	pPolygons->Add_Field(_TL("Depqc"       ), SG_DATATYPE_Double);	//  4
	pPolygons->Add_Field(_TL("Sphericity"  ), SG_DATATYPE_Double);	//  5
	pPolygons->Add_Field(_TL("Shape Index" ), SG_DATATYPE_Double);	//  6
	pPolygons->Add_Field(_TL("Dmax"        ), SG_DATATYPE_Double);	//  7
	pPolygons->Add_Field(_TL("DmaxDir"     ), SG_DATATYPE_Double);	//  8
	pPolygons->Add_Field(_TL("Dmax/A"      ), SG_DATATYPE_Double);	//  9
	pPolygons->Add_Field(_TL("Dmax/sqrt(A)"), SG_DATATYPE_Double);	// 10

	bool	bGyros	= Parameters("GYROS")->asBool();

	if( bGyros )
	{
		pPolygons->Add_Field(_TL("Dgyros"  ), SG_DATATYPE_Double);	// 11
	}

	int	offFeret	= pPolygons->Get_Field_Count();

	double	dFeret	= 0.0;

	if( Parameters("FERET")->asBool() )
	{
		dFeret	= M_DEG_TO_RAD * (180. / (1. + Parameters("FERET_DIRS")->asInt()));

		pPolygons->Add_Field(_TL("Fmax"    ), SG_DATATYPE_Double);	//  0
		pPolygons->Add_Field(_TL("FmaxDir" ), SG_DATATYPE_Double);	//  1
		pPolygons->Add_Field(_TL("Fmin"    ), SG_DATATYPE_Double);	//  2
		pPolygons->Add_Field(_TL("FminDir" ), SG_DATATYPE_Double);	//  3
		pPolygons->Add_Field(_TL("Fmean"   ), SG_DATATYPE_Double);	//  4
		pPolygons->Add_Field(_TL("Fmax90"  ), SG_DATATYPE_Double);	//  5
		pPolygons->Add_Field(_TL("Fmin90"  ), SG_DATATYPE_Double);	//  6
		pPolygons->Add_Field(_TL("Fvol"    ), SG_DATATYPE_Double);	//  7
	}

	//-----------------------------------------------------
	CSG_Shapes	*pDmax	= Parameters("DMAX")->asShapes();

	if( pDmax )
	{
		pDmax->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Maximum Diameter")));

		pDmax->Add_Field("ID", SG_DATATYPE_Int   );
		pDmax->Add_Field("D" , SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		double	A	= pPolygon->Get_Area     ();
		double	P	= pPolygon->Get_Perimeter();

		if( A > 0.0 && P > 0.0 )
		{
			pPolygon->Set_Value(offIndices + 0, A);
			pPolygon->Set_Value(offIndices + 1, P);
			pPolygon->Set_Value(offIndices + 2, P / A);
			pPolygon->Set_Value(offIndices + 3, P / sqrt(A));
			pPolygon->Set_Value(offIndices + 4, 2 * sqrt(A / M_PI));
			pPolygon->Set_Value(offIndices + 5, (2 * sqrt(A * M_PI)) / P);
			pPolygon->Set_Value(offIndices + 6, P / (2 * sqrt(A * M_PI)));

			double	Dmax;	TSG_Point	Pmax[2];

			if( Get_Diameter_Max(pPolygon, Dmax, Pmax) )
			{
				double	DmaxDir	= SG_Get_Angle_Of_Direction(Pmax[0], Pmax[1]); if( DmaxDir > M_PI_180 ) DmaxDir -= M_PI_180;

				pPolygon->Set_Value(offIndices +  7, Dmax);
				pPolygon->Set_Value(offIndices +  8, DmaxDir * M_RAD_TO_DEG);
				pPolygon->Set_Value(offIndices +  9, Dmax / A);
				pPolygon->Set_Value(offIndices + 10, Dmax / sqrt(A));

				if( pDmax )
				{
					CSG_Shape	*pLine	= pDmax->Add_Shape();

					pLine->Add_Point(Pmax[0]);
					pLine->Add_Point(Pmax[1]);

					pLine->Set_Value(0, iPolygon);
					pLine->Set_Value(1, Dmax);
				}

				if( bGyros )
				{
					Get_Diameter_Gyros(pPolygon, offIndices + 11);
				}

				if( dFeret > 0.0 )
				{
					Get_Diameters_Feret(pPolygon, offFeret, dFeret);
				}
			}
			else
			{
				for(int iField=offIndices+7; iField<pPolygons->Get_Field_Count(); iField++)
				{
					pPolygon->Set_NoData(iField);
				}
			}
		}
		else
		{
			for(int iField=offIndices; iField<pPolygons->Get_Field_Count(); iField++)
			{
				pPolygon->Set_NoData(iField);
			}
		}
	}

	//-----------------------------------------------------
	if( pPolygons == Parameters("SHAPES")->asShapes() )
	{	// output is always updated automatically - but if input has been modified, this needs a manual update!
		DataObject_Update(pPolygons);
	}

	return( pPolygons->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShape_Index::Get_Diameter_Max(CSG_Shape_Polygon *pPolygon, double &Dmax, TSG_Point Pmax[2])
{
	Dmax	= 0.0;

	for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(); iPoint++)
	{
		TSG_Point	P	= pPolygon->Get_Point(iPoint);

		for(int jPoint=iPoint+1; jPoint<pPolygon->Get_Point_Count(); jPoint++)
		{
			double	d	= SG_Get_Distance(P, pPolygon->Get_Point(jPoint));

			if( Dmax < d )
			{
				Dmax	= d;
				Pmax[0]	= P;
				Pmax[1]	= pPolygon->Get_Point(jPoint);
			}
		}
	}

	return( Dmax > 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShape_Index::Get_Diameter_Gyros(CSG_Shape_Polygon *pPolygon, int Field)
{
	double	Dmax	= 0.0;

	TSG_Point	C	= pPolygon->Get_Centroid();

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			double	d	= SG_Get_Distance(C, pPolygon->Get_Point(iPoint, iPart));

			if( d > Dmax )
			{
				Dmax	= d;
			}
		}
	}

	if( Dmax > 0.0 )
	{
		pPolygon->Set_Value(Field, 2 * Dmax);

		return( true );
	}

	pPolygon->Set_NoData(Field);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShape_Index::Get_Diameters_Feret(CSG_Shape_Polygon *pPolygon, int Field, double dAngle)
{
	CSG_Simple_Statistics	F;

	double	maxDir, maxF90, minDir, minF90;

	TSG_Point	C	= pPolygon->Get_Centroid();	// not really necessary

	for(double Direction=0.0; Direction<M_PI_090; Direction+=dAngle)
	{
		double	sin_a	= sin(Direction);
		double	cos_a	= cos(Direction);

		CSG_Simple_Statistics	sx, sy;

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	P	= pPolygon->Get_Point(iPoint, iPart);

				P.x	-= C.x;	P.y	-= C.y;	// not really necessary

				sx	+= P.x * cos_a - P.y * sin_a;
				sy	+= P.x * sin_a + P.y * cos_a;
			}
		}

		#define CHECK_DIR(s, s90, d)	{ if( !F.Get_Count() ) { minDir = maxDir = d; minF90 = maxF90 = s90.Get_Range(); }\
			else if( F.Get_Minimum() > s.Get_Range() ) { minDir = d; minF90 = s90.Get_Range(); }\
			else if( F.Get_Maximum() < s.Get_Range() ) { maxDir = d; maxF90 = s90.Get_Range(); } F += s.Get_Range(); }

		CHECK_DIR(sx, sy, Direction + M_PI_090);
		CHECK_DIR(sy, sx, Direction           );
	}

	pPolygon->Set_Value(Field + 0, F.Get_Maximum());
	pPolygon->Set_Value(Field + 1, maxDir * M_RAD_TO_DEG);
	pPolygon->Set_Value(Field + 2, F.Get_Minimum());
	pPolygon->Set_Value(Field + 3, minDir * M_RAD_TO_DEG);
	pPolygon->Set_Value(Field + 4, F.Get_Mean());
	pPolygon->Set_Value(Field + 5, maxF90);
	pPolygon->Set_Value(Field + 6, minF90);
	pPolygon->Set_Value(Field + 7, pow(3 * F.Get_Minimum() * F.Get_Maximum(), 1. / 3.));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
