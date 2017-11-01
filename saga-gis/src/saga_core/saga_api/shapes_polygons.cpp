/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  shapes_polygons.cpp                  //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "shapes.h"

//---------------------------------------------------------
#include "clipper.hpp"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Converter_WorldToInt
{
public:

	CSG_Converter_WorldToInt		(void)															{	Create(0.0, 1.0, 0.0, 1.0);					}
	CSG_Converter_WorldToInt		(const CSG_Converter_WorldToInt &Converter)						{	Create(Converter);							}
	CSG_Converter_WorldToInt		(double xOffset, double xScale, double yOffset, double yScale)	{	Create(xOffset, xScale, yOffset, yScale);	}
	CSG_Converter_WorldToInt		(const CSG_Rect &Extent, bool bAspectRatio = false)				{	Create(Extent, bAspectRatio);				}

	bool			Create			(const CSG_Converter_WorldToInt &Converter)
	{
		return( Create(Converter.m_xOffset, Converter.m_xScale, Converter.m_yOffset, Converter.m_yScale) );
	}

	bool			Create			(double xOffset, double xScale, double yOffset, double yScale)
	{
		if( xScale != 0.0 && yScale != 0.0 )
		{
			m_xOffset	= xOffset;
			m_xScale	= xScale;
			m_yOffset	= yOffset;
			m_yScale	= yScale;

			return( true );
		}

		return( false );
	}

	bool			Create			(const CSG_Rect &Extent, bool bAspectRatio = false)
	{
		double	xRange	= Extent.Get_XRange();
		double	yRange	= Extent.Get_YRange();
		double	xMin	= Extent.Get_XMin  ();
		double	yMin	= Extent.Get_YMin  ();

		if( bAspectRatio )
		{
			if( xRange < yRange )
			{
				xRange	 =  yRange;
			}
			else if( yRange < xRange )
			{
				yRange	 =  xRange;
			}
		}

		return( xRange > 0 && yRange > 0 ? Create(
			xMin, (0x3FFFFFFFFFFFFFF) / xRange,
			yMin, (0x3FFFFFFFFFFFFFF) / yRange
		) : false);
	}

	static ClipperLib::cInt		Round			(double Value)	{	return( (ClipperLib::cInt)(Value < 0.0 ? Value - 0.5 : Value + 0.5) );	}

	ClipperLib::cInt			Get_X_asInt		(double Value)	const	{	return( Round((Value - m_xOffset) * m_xScale) );	}
	ClipperLib::cInt			Get_Y_asInt		(double Value)	const	{	return( Round((Value - m_yOffset) * m_yScale) );	}

	double						Get_X_asWorld	(ClipperLib::cInt Value)	const	{	return( m_xOffset + Value / m_xScale );	}
	double						Get_Y_asWorld	(ClipperLib::cInt Value)	const	{	return( m_yOffset + Value / m_yScale );	}

	bool						Convert			(CSG_Shapes *pPolygons     , ClipperLib::Paths &P )	const;
	bool						Convert			(const ClipperLib::Paths &P, CSG_Shapes *pPolygons)	const;

	bool						Convert			(CSG_Shape *pPolygon       , ClipperLib::Paths &P)	const;
	bool						Convert			(const ClipperLib::Paths &P, CSG_Shape *pPolygon )	const;

	double						Get_xScale		(void)	const	{	return( m_xScale );	}
	double						Get_yScale		(void)	const	{	return( m_yScale );	}


private:

	double						m_xOffset, m_xScale, m_yOffset, m_yScale;


};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(CSG_Shapes *pPolygons, ClipperLib::Paths &Polygons) const
{
	Polygons.clear();

	for(int iPolygon=0, jPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(iPolygon);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++, jPolygon++)
		{
			bool	bAscending	= pPolygon->Get_Type() != SHAPE_TYPE_Polygon
			|| (((CSG_Shape_Polygon *)pPolygon)->is_Lake(iPart)
			==  ((CSG_Shape_Polygon *)pPolygon)->is_Clockwise(iPart));

			Polygons.resize(1 + jPolygon);
			Polygons[jPolygon].resize(pPolygon->Get_Point_Count(iPart));

			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	p	= pPolygon->Get_Point(iPoint, iPart, bAscending);

				Polygons[jPolygon][iPoint].X	= Get_X_asInt(p.x);
				Polygons[jPolygon][iPoint].Y	= Get_Y_asInt(p.y);
			}
		}
	}

	return( Polygons.size() > 0 );
}

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(const ClipperLib::Paths &Polygons, CSG_Shapes *pPolygons) const
{
	pPolygons->Del_Shapes();

	CSG_Shape	*pPolygon	= pPolygons->Add_Shape();

	return( Convert(Polygons, pPolygon) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(CSG_Shape *pPolygon, ClipperLib::Paths &Polygons) const
{
	Polygons.clear();

	for(int iPart=0, iPolygon=0; iPart<pPolygon->Get_Part_Count(); iPart++, iPolygon++)
	{
		if( pPolygon->Get_Point_Count(iPart) > 0 )
		{
			bool	bAscending	= pPolygon->Get_Type() != SHAPE_TYPE_Polygon
			|| (((CSG_Shape_Polygon *)pPolygon)->is_Lake(iPart)
			==  ((CSG_Shape_Polygon *)pPolygon)->is_Clockwise(iPart));

			Polygons.resize(1 + iPolygon);

			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	p	= pPolygon->Get_Point(iPoint, iPart, bAscending);

				ClipperLib::IntPoint	Point(Get_X_asInt(p.x), Get_Y_asInt(p.y));

				if( iPoint == 0 || Polygons[iPolygon].back() != Point )	// don't add duplicates !!!
				{
					Polygons[iPolygon].push_back(Point);
				}
			}

			if( pPolygon->Get_Type() == SHAPE_TYPE_Polygon && Polygons[iPolygon].size() > 1 && Polygons[iPolygon][0] == Polygons[iPolygon].back() )
			{
				Polygons[iPolygon].pop_back();
			}
		}
	}

	return( Polygons.size() > 0 );
}

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(const ClipperLib::Paths &Polygons, CSG_Shape *pPolygon) const
{
	pPolygon->Del_Parts();

	for(size_t iPolygon=0, iPart=0; iPolygon<Polygons.size(); iPolygon++)
	{
		for(size_t iPoint=0; iPoint<Polygons[iPolygon].size(); iPoint++)
		{
			pPolygon->Add_Point(
				Get_X_asWorld(Polygons[iPolygon][iPoint].X),
				Get_Y_asWorld(Polygons[iPolygon][iPoint].Y),
				(int)iPart
			);
		}

		if( pPolygon->Get_Type() != SHAPE_TYPE_Polygon || ((CSG_Shape_Polygon *)pPolygon)->Get_Area((int)iPart) > (1.0e-12) )
		{
			iPart++;
		}
		else
		{
			pPolygon->Del_Part((int)iPart);
		}
	}

	return( pPolygon->Get_Part_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool _SG_Polygon_Clip(ClipperLib::ClipType ClipType, CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	CSG_Rect	r(pPolygon->Get_Extent());	r.Union(pClip->Get_Extent());

	CSG_Converter_WorldToInt	Converter(r);

	ClipperLib::Paths	Polygon, Clip, Result;

	if(	Converter.Convert(pPolygon, Polygon)
	&&	Converter.Convert(pClip   , Clip   ) )
	{
		ClipperLib::Clipper	Clipper;

		Clipper.AddPaths(Clip, ClipperLib::ptClip, true);

		if( pPolygon->Get_Type() != SHAPE_TYPE_Line )
		{
			Clipper.AddPaths(Polygon, ClipperLib::ptSubject, true);

			if( !Clipper.Execute(ClipType, Result) )
			{
				return( false );
			}
		}
		else
		{
			Clipper.AddPaths(Polygon, ClipperLib::ptSubject, false);

			ClipperLib::PolyTree	PolyTree;

			if( !Clipper.Execute(ClipType, PolyTree) )
			{
				return( false );
			}

			ClipperLib::PolyTreeToPaths(PolyTree, Result);
		}

		return( Converter.Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Polygon_Intersection	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_None:
		return( false );

	case INTERSECTION_Identical:
	case INTERSECTION_Contains:
		if( pResult )	pResult->Assign(pPolygon, false);
		return( true );

	case INTERSECTION_Contained:
		if( pResult )	pResult ->Assign(pClip  , false);
		else			pPolygon->Assign(pClip  , false);
		return( true );

	case INTERSECTION_Overlaps: default:
		return( _SG_Polygon_Clip(ClipperLib::ctIntersection	, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Polygon_Difference	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_Contains:
	case INTERSECTION_Identical:
		return( false );

	case INTERSECTION_None:
		if( pResult )	pResult->Assign(pPolygon, false);
		return( true );

	case INTERSECTION_Contained:
	case INTERSECTION_Overlaps: default:
		return( _SG_Polygon_Clip(ClipperLib::ctDifference	, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Polygon_ExclusiveOr	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_Identical:
		return( false );

	case INTERSECTION_None:
		if( pResult )	pResult->Assign(pPolygon, false);
		else			pResult	= pPolygon;

		{	for(int iPart=0, jPart=pResult->Get_Part_Count(); iPart<pClip->Get_Part_Count(); iPart++, jPart++)
			{
				for(int iPoint=0; iPoint<pClip->Get_Point_Count(iPart); iPoint++)
				{
					pResult->Add_Point(pClip->Get_Point(iPoint, iPart), jPart);
				}
			}
		}

		return( true );	

	case INTERSECTION_Contained:
	case INTERSECTION_Contains:
	case INTERSECTION_Overlaps: default:
		return( _SG_Polygon_Clip(ClipperLib::ctXor			, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Polygon_Union		(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_Contained:
	case INTERSECTION_Identical:
		if( pResult )	pResult->Assign(pPolygon, false);
		return( true );

	case INTERSECTION_Contains:
		if( pResult )	pResult ->Assign(pClip  , false);
		else			pPolygon->Assign(pClip  , false);
		return( true );

	case INTERSECTION_None:
		if( pResult )	pResult->Assign(pPolygon, false);
		else			pResult	= pPolygon;

		{	for(int iPart=0, jPart=pResult->Get_Part_Count(); iPart<pClip->Get_Part_Count(); iPart++, jPart++)
			{
				for(int iPoint=0; iPoint<pClip->Get_Point_Count(iPart); iPoint++)
				{
					pResult->Add_Point(pClip->Get_Point(iPoint, iPart), jPart);
				}
			}
		}

		return( true );	

	case INTERSECTION_Overlaps: default:
		return( _SG_Polygon_Clip(ClipperLib::ctUnion		, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Polygon_Dissolve		(CSG_Shape *pPolygon, CSG_Shape *pResult)
{
	CSG_Converter_WorldToInt	Converter(pPolygon->Get_Extent());

	ClipperLib::Paths			Polygon, Result;

	if(	Converter.Convert(pPolygon, Polygon) )
	{
		ClipperLib::Clipper	Clipper;

		Clipper.AddPaths(Polygon, ClipperLib::ptSubject, true);

		Clipper.Execute(ClipperLib::ctUnion, Result);

		return( Converter.Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Polygon_Simplify		(CSG_Shape *pPolygon, CSG_Shape *pResult)
{
	CSG_Converter_WorldToInt	Converter(pPolygon->Get_Extent());

	ClipperLib::Paths			Polygon, Result;

	if(	Converter.Convert(pPolygon, Polygon) )
	{
		ClipperLib::SimplifyPolygons(Polygon, Result);

		return( Converter.Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Polygon_Offset		(CSG_Shape *pPolygon, double dSize, double dArc, CSG_Shape *pResult)
{
	CSG_Rect					r(pPolygon->Get_Extent());	if( dSize > 0.0 )	r.Inflate(5.0 * dSize, false);

	CSG_Converter_WorldToInt	Converter(r, true);

	ClipperLib::Paths			Paths, Result;

	if(	Converter.Convert(pPolygon, Paths) )
	{
		ClipperLib::ClipperOffset	Offset(2.0, dArc * Converter.Get_xScale());

		if( pPolygon->Get_Type() == SHAPE_TYPE_Polygon )
		{
			Offset.AddPaths(Paths, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
		}
		else
		{
			Offset.AddPaths(Paths, ClipperLib::jtRound, ClipperLib::etOpenRound);
		}

		Offset.Execute(Result, dSize * Converter.Get_xScale());

		return( Converter.Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
