/**********************************************************
 * Version $Id: gcs_graticule.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   gcs_graticule.cpp                   //
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
#include "gcs_graticule.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGCS_Graticule::CGCS_Graticule(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	Set_Name		(_TL("Latitude/Longitude Graticule"));

	Set_Author		(SG_T("O. Conrad (c) 2014"));

	Set_Description	(_TW(
		"Creates a longitude/latitude graticule for the extent and projection of the input shapes layer. "
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "GRATICULE"	, _TL("Graticule"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "COORDS"		, _TL("Frame Coordinates"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	pNode_0	= Parameters.Add_Node(
		NULL	, "NODE_GRID"	, _TL("Graticule"),
		_TL("")
	);

	pNode_1 = Parameters.Add_Node(pNode_0, "NODE_X"		, _TL("X Range"), _TL(""));
	Parameters.Add_Value(pNode_1, "XMIN", _TL("Minimum"), _TL(""), PARAMETER_TYPE_Double);
	Parameters.Add_Value(pNode_1, "XMAX", _TL("Maximum"), _TL(""), PARAMETER_TYPE_Double);

	pNode_1 = Parameters.Add_Node(pNode_0, "NODE_Y"		, _TL("Y Range"), _TL(""));
	Parameters.Add_Value(pNode_1, "YMIN", _TL("Minimum"), _TL(""), PARAMETER_TYPE_Double);
	Parameters.Add_Value(pNode_1, "YMAX", _TL("Maximum"), _TL(""), PARAMETER_TYPE_Double);

	Parameters.Add_Choice(
		pNode_0	, "INTERVAL"	, _TL("Interval"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("fixed interval"),
			_TL("fitted interval")
		), 0
	);

	Parameters.Add_Value(
		pNode_0	, "FIXED"		, _TL("Fixed Interval (Degree)"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true, 20.0
	);

	Parameters.Add_Value(
		pNode_0	, "FITTED"		, _TL("Number of Intervals"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	Parameters.Add_Value(
		pNode_0	, "RESOLUTION"	, _TL("Minimum Resolution (Degree)"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.5, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGCS_Graticule::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("CRS_GRID"  ))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("CRS_SHAPES")) )
	{
		CSG_Rect	r(!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("CRS_GRID"))
			? pParameter->asParameters()->Get_Parameter("PICK")->asGrid  ()->Get_Extent()
			: pParameter->asParameters()->Get_Parameter("PICK")->asShapes()->Get_Extent()
		);

		if( r.Get_XRange() > 0.0 && r.Get_YRange() > 0.0 )
		{
			pParameters->Get_Parameter("XMIN")->Set_Value(r.Get_XMin());
			pParameters->Get_Parameter("XMAX")->Set_Value(r.Get_XMax());
			pParameters->Get_Parameter("YMIN")->Set_Value(r.Get_YMin());
			pParameters->Get_Parameter("YMAX")->Set_Value(r.Get_YMax());
		}
	}

	return( CCRS_Base::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGCS_Graticule::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("INTERVAL")) )
	{
		pParameters->Get_Parameter("FIXED" )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("FITTED")->Set_Enabled(pParameter->asInt() == 1);
	}

	return( CCRS_Base::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Graticule::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Projection	Projection;

	if( !Get_Projection(Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Projector.Set_Source(CSG_Projection("+proj=longlat +ellps=WGS84 +datum=WGS84", SG_PROJ_FMT_Proj4));

	if( !m_Projector.Set_Target(Projection) )
	{
		m_Projector.Destroy();

		return( false );
	}

	//-----------------------------------------------------
	CSG_Rect	Extent(
		Parameters("XMIN")->asDouble(),
		Parameters("YMIN")->asDouble(),
		Parameters("XMAX")->asDouble(),
		Parameters("YMAX")->asDouble()
	);

	if( !Get_Graticule(Extent) )
	{
		m_Projector.Destroy();

		return( false );
	}

	//-----------------------------------------------------
	Get_Coordinates();

	m_Projector.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Graticule::Get_Graticule(const CSG_Rect &Extent)
{
	double		x, y, Interval;
	CSG_Rect	r;

	if( !Get_Extent(Extent, r) || (Interval = Get_Interval(r)) <= 0.0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	r.m_rect.xMin	= Interval * floor(r.Get_XMin() / Interval);	if( r.Get_XMin() < -180.0 )	r.m_rect.xMin	= -180.0;
	r.m_rect.xMax	= Interval * ceil (r.Get_XMax() / Interval);	if( r.Get_XMax() >  180.0 )	r.m_rect.xMax	=  180.0;
	r.m_rect.yMin	= Interval * floor(r.Get_YMin() / Interval);	if( r.Get_YMin() <  -90.0 )	r.m_rect.yMin	=  -90.0;
	r.m_rect.yMax	= Interval * ceil (r.Get_YMax() / Interval);	if( r.Get_YMax() >   90.0 )	r.m_rect.yMax	=   90.0;

	//-----------------------------------------------------
	double	Resolution	= Parameters("RESOLUTION")->asDouble();

	if( Interval > Resolution )
	{
		Resolution	= Interval / ceil(Interval / Resolution);
	}

	//-----------------------------------------------------
	CSG_Shapes	*pGraticule	= Parameters("GRATICULE")->asShapes();

	pGraticule->Create(SHAPE_TYPE_Line);
	pGraticule->Set_Name(_TL("Graticule"));

	pGraticule->Add_Field("TYPE"  , SG_DATATYPE_String);
	pGraticule->Add_Field("LABEL" , SG_DATATYPE_String);
	pGraticule->Add_Field("DEGREE", SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Shapes	Clip(SHAPE_TYPE_Polygon);
	CSG_Shape	*pClip	= Clip.Add_Shape();

	pClip->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());
	pClip->Add_Point(Extent.Get_XMin(), Extent.Get_YMax());
	pClip->Add_Point(Extent.Get_XMax(), Extent.Get_YMax());
	pClip->Add_Point(Extent.Get_XMax(), Extent.Get_YMin());
	pClip->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());

	//-----------------------------------------------------
	for(y=r.Get_YMin(); y<=r.Get_YMax(); y+=Interval)
	{
		CSG_Shape	*pLine	= pGraticule->Add_Shape();

		pLine->Set_Value(0, "LAT");
		pLine->Set_Value(1, SG_Double_To_Degree(y));
		pLine->Set_Value(2, y);

		for(x=r.Get_XMin(); x<=r.Get_XMax(); x+=Interval)
		{
			CSG_Point	p(x, y);	m_Projector.Get_Projection(p);	pLine->Add_Point(p);

			if( Resolution < Interval && x < r.Get_XMax() )
			{
				for(double i=x+Resolution; i<x+Interval; i+=Resolution)
				{
					CSG_Point	p(i, y);	m_Projector.Get_Projection(p);	pLine->Add_Point(p);
				}
			}
		}

		if( !SG_Polygon_Intersection(pLine, pClip) )
		{
			pGraticule->Del_Shape(pLine);
		}
	}

	//-----------------------------------------------------
	for(x=r.Get_XMin(); x<=r.Get_XMax(); x+=Interval)
	{
		CSG_Shape	*pLine	= pGraticule->Add_Shape();

		pLine->Set_Value(0, "LON");
		pLine->Set_Value(1, SG_Double_To_Degree(x));
		pLine->Set_Value(2, x);

		for(y=r.Get_YMin(); y<=r.Get_YMax(); y+=Interval)
		{
			CSG_Point	p(x, y);	m_Projector.Get_Projection(p);	pLine->Add_Point(p);

			if( Resolution < Interval && y < r.Get_YMax() )
			{
				for(double i=y+Resolution; i<y+Interval; i+=Resolution)
				{
					CSG_Point	p(x, i);	m_Projector.Get_Projection(p);	pLine->Add_Point(p);
				}
			}
		}

		if( !SG_Polygon_Intersection(pLine, pClip) )
		{
			pGraticule->Del_Shape(pLine);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGCS_Graticule::Get_Interval(const CSG_Rect &Extent)
{
	if( Parameters("INTERVAL")->asInt() == 0 )
	{
		return( Parameters("FIXED")->asDouble() );
	}

	double	Interval	= Extent.Get_XRange() > Extent.Get_YRange() ? Extent.Get_XRange() : Extent.Get_YRange();

	if( Interval > 360 )
	{
		Interval	= 360;
	}

	Interval	= Interval / Parameters("FITTED")->asInt();

	double	d	= pow(10.0, (int)(log10(Interval)) - (Interval < 1.0 ? 1.0 : 0.0));

	Interval	= (int)(Interval / d) * d;

	return( Interval );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Graticule::Get_Coordinates(void)
{
	CSG_Shapes	*pCoordinates	= Parameters("COORDS")->asShapes();

	if( !pCoordinates || !m_Projector.Set_Inverse() )
	{
		return( false );
	}

	pCoordinates->Create(SHAPE_TYPE_Point);
	pCoordinates->Set_Name(_TL("Coordinates"));

	pCoordinates->Add_Field("TYPE" , SG_DATATYPE_String);
	pCoordinates->Add_Field("LABEL", SG_DATATYPE_String);

	//-----------------------------------------------------
	CSG_Shapes	*pGraticule	= Parameters("GRATICULE")->asShapes();

	for(int i=0; i<pGraticule->Get_Count(); i++)
	{
		CSG_Shape	*pPoint, *pLine	= pGraticule->Get_Shape(i);
		CSG_String	Type(pLine->asString(0)), Label(pLine->asString(1));

		//-------------------------------------------------
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( pLine->Get_Point_Count(iPart) > 0 )
			{
				pPoint	= pCoordinates->Add_Shape();
				pPoint	->Add_Point(pLine->Get_Point(0, iPart));
				pPoint	->Set_Value(0, Type + "_MIN");
				pPoint	->Set_Value(1, Label);
			}

			if( pLine->Get_Point_Count(iPart) > 1 )
			{
				pPoint	= pCoordinates->Add_Shape();
				pPoint	->Add_Point(pLine->Get_Point(pLine->Get_Point_Count(iPart) - 1));
				pPoint	->Set_Value(0, Type + "_MAX");
				pPoint	->Set_Value(1, Label);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Graticule::Get_Extent(const CSG_Rect &Extent, CSG_Rect &r)
{
	if( m_Projector.Set_Inverse() )
	{
		double		x, y, d;

		CSG_Point	p(Extent.Get_XMin(), Extent.Get_YMin());
		
		m_Projector.Get_Projection(p);	r.Assign(p, p);

		d	= Extent.Get_XRange() / 10.0;

		for(y=Extent.Get_YMin(), x=Extent.Get_XMin(); x<=Extent.Get_XMax(); x+=d)
		{
			p.Assign(x, y);	m_Projector.Get_Projection(p);	r.Union(p);
		}

		for(y=Extent.Get_YMax(), x=Extent.Get_XMin(); x<=Extent.Get_XMax(); x+=d)
		{
			p.Assign(x, y);	m_Projector.Get_Projection(p);	r.Union(p);
		}

		d	= Extent.Get_YRange() / 10.0;

		for(x=Extent.Get_XMin(), y=Extent.Get_YMin(); y<=Extent.Get_YMax(); y+=d)
		{
			p.Assign(x, y);	m_Projector.Get_Projection(p);	r.Union(p);
		}

		for(x=Extent.Get_XMax(), y=Extent.Get_YMin(); y<=Extent.Get_YMax(); y+=d)
		{
			p.Assign(x, y);	m_Projector.Get_Projection(p);	r.Union(p);
		}

		m_Projector.Set_Inverse(false);

		if( r.Get_XMin() < -180 ) r.m_rect.xMin = -180; else if( r.Get_XMax() > 180 ) r.m_rect.xMax = 180;
		if( r.Get_YMin() <  -90 ) r.m_rect.yMin =  -90; else if( r.Get_YMax() >  90 ) r.m_rect.yMax =  90;

		return( r.Get_XRange() > 0.0 && r.Get_YRange() > 0.0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
