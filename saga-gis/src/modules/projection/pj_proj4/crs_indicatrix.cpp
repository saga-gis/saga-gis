/**********************************************************
 * Version $Id: crs_indicatrix.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                  crs_indicatrix.cpp                   //
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
#include "crs_indicatrix.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Indicatrix::CCRS_Indicatrix(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Tissot's Indicatrix"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"Creates a shapes layer with Tissot's indicatrices for chosen projection."
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "TARGET"	, _TL("Indicatrix"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "NY"		, _TL("Number in Latitudinal Direction"),
		_TL(""),
		PARAMETER_TYPE_Int, 11, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NX"		, _TL("Number in Meridional Direction"),
		_TL(""),
		PARAMETER_TYPE_Int, 20, 1, true
	);

	Parameters.Add_Value(
		NULL	, "SIZE"	, _TL("Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 50.0, 1.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FIELD_LON	= 0,
	FIELD_LAT,
	FIELD_h,
	FIELD_k,
	FIELD_a,
	FIELD_b,
	FIELD_w,
	FIELD_PHI
};

//---------------------------------------------------------
bool CCRS_Indicatrix::On_Execute_Transformation(void)
{
	m_Projector.Set_Source(CSG_Projection("+proj=longlat +ellps=WGS84 +datum=WGS84", SG_PROJ_FMT_Proj4));

	//-----------------------------------------------------
	double	yStep	=  90.0 / Parameters("NY")->asDouble();
	double	xStep	= 180.0 / Parameters("NX")->asDouble();

	m_Scale	= 0.0001;
	m_Size	= Parameters("SIZE")->asDouble() / m_Scale;

	//-----------------------------------------------------
	for(double a=0.0; a<M_PI_360; a+=5.0*M_DEG_TO_RAD)
	{
		m_Circle.Add(m_Scale * sin(a), m_Scale * cos(a));
	}

	//-----------------------------------------------------
	CSG_Shapes	*pTarget	= Parameters("TARGET")->asShapes();

	pTarget->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), _TL("Indicatrix"), m_Projector.Get_Target().Get_Name().c_str()));

	pTarget->Get_Projection() = m_Projector.Get_Target();

	pTarget->Add_Field("LON", SG_DATATYPE_Double);
	pTarget->Add_Field("LAT", SG_DATATYPE_Double);
	pTarget->Add_Field("h"  , SG_DATATYPE_Double);
	pTarget->Add_Field("k"  , SG_DATATYPE_Double);
	pTarget->Add_Field("a"  , SG_DATATYPE_Double);
	pTarget->Add_Field("b"  , SG_DATATYPE_Double);
	pTarget->Add_Field("w"  , SG_DATATYPE_Double);
	pTarget->Add_Field("PHI", SG_DATATYPE_Double);

	//-----------------------------------------------------
	int		nDropped	= 0;

	for(double lat=-90.0+yStep/2.0; lat<90.0; lat+=yStep)
	{
		for(double lon=-180.0+xStep/2.0; lon<180.0; lon+=xStep)
		{
			CSG_Shape	*pIndicatrix	= pTarget->Add_Shape();

			if( !Get_Indicatrix(lon, lat, pIndicatrix) )
			{
				nDropped++;

				pTarget->Del_Shape(pIndicatrix);
			}
		}
	}

	//-----------------------------------------------------
	if( nDropped > 0 )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %d %s"), pTarget->Get_Name(), nDropped, _TL("shapes have been dropped")));
	}

	m_Circle.Clear();

	return( pTarget->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Indicatrix::Get_Indicatrix(double lon, double lat, CSG_Shape *pIndicatrix)
{
	TSG_Point	Center, Point;

	Center.x	= lon;
	Center.y	= lat;

	if( !m_Projector.Get_Projection(Center) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Point.x		= lon + m_Scale;
	Point.y		= lat;

	if( !m_Projector.Get_Projection(Point) )
	{
		return( false );
	}

	double	a	= SG_Get_Distance(Center, Point);

	//-----------------------------------------------------
	Point.x		= lon;
	Point.y		= lat + m_Scale;

	if( !m_Projector.Get_Projection(Point) )
	{
		return( false );
	}

	double	b	= SG_Get_Distance(Center, Point);

	//-----------------------------------------------------
	pIndicatrix->Set_Value(FIELD_LON, lon);
	pIndicatrix->Set_Value(FIELD_LAT, lat);
	pIndicatrix->Set_Value(FIELD_a  , a);
	pIndicatrix->Set_Value(FIELD_b  , b);
	pIndicatrix->Set_Value(FIELD_h  , a / m_Scale);
	pIndicatrix->Set_Value(FIELD_k  , b / m_Scale);
	pIndicatrix->Set_Value(FIELD_w  , 2.0 * asin((a - b) / (a + b)));
	pIndicatrix->Set_Value(FIELD_PHI, a * b);

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<m_Circle.Get_Count(); iPoint++)
	{
		Point.x	= lon + m_Circle[iPoint].x;
		Point.y	= lat + m_Circle[iPoint].y;

		if( !m_Projector.Get_Projection(Point) )
		{
			return( false );
		}

		Point.x	= Center.x + m_Size * (Point.x - Center.x);
		Point.y	= Center.y + m_Size * (Point.y - Center.y);

		pIndicatrix->Add_Point(Point);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
