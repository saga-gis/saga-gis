/**********************************************************
 * Version $Id: table_change_field_format.cpp 911 2011-11-11 11:11:11Z oconrad $
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
				xMin	-= (yRange - xRange) / 2.0;
				xRange	 =  yRange;
			}
			else if( yRange < xRange )
			{
				yMin	-= (xRange - yRange) / 2.0;
				yRange	 =  xRange;
			}
		}

		return( xRange > 0 && yRange > 0 ? Create(
			xMin, (1000000000000000000LL) / xRange,
			yMin, (1000000000000000000LL) / yRange
		) : false);
	}

	static ClipperLib::long64	Round			(double Value)	{	return( (ClipperLib::long64)(Value < 0.0 ? Value - 0.5 : Value + 0.5) );	}

	ClipperLib::long64			Get_X_asInt		(double Value)	const	{	return( Round((Value - m_xOffset) * m_xScale) );	}
	ClipperLib::long64			Get_Y_asInt		(double Value)	const	{	return( Round((Value - m_yOffset) * m_yScale) );	}

	double						Get_X_asWorld	(ClipperLib::long64 Value)	const	{	return( m_xOffset + Value / m_xScale );	}
	double						Get_Y_asWorld	(ClipperLib::long64 Value)	const	{	return( m_yOffset + Value / m_yScale );	}

	bool						Convert			(CSG_Shapes *pPolygons, ClipperLib::Polygons &P)		const;
	bool						Convert			(const ClipperLib::Polygons &P, CSG_Shapes *pPolygons)	const;

	bool						Convert			(CSG_Shape_Polygon *pPolygon, ClipperLib::Polygons &P)	const;
	bool						Convert			(const ClipperLib::Polygons &P, CSG_Shape *pPolygon)	const;

	double						Get_xScale		(void)	const	{	return( m_xScale );	}
	double						Get_yScale		(void)	const	{	return( m_yScale );	}


private:

	double						m_xOffset, m_xScale, m_yOffset, m_yScale;


};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(CSG_Shapes *pPolygons, ClipperLib::Polygons &Polygons) const
{
	Polygons.clear();

	for(int iPolygon=0, jPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++, jPolygon++)
		{
			bool	bAscending	= pPolygon->is_Lake(iPart) == pPolygon->is_Clockwise(iPart);

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
bool CSG_Converter_WorldToInt::Convert(const ClipperLib::Polygons &Polygons, CSG_Shapes *pPolygons) const
{
	pPolygons->Del_Shapes();

	CSG_Shape	*pPolygon	= pPolygons->Add_Shape();

	return( Convert(Polygons, pPolygon) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(CSG_Shape_Polygon *pPolygon, ClipperLib::Polygons &Polygons) const
{
	Polygons.clear();

	for(int iPart=0, iPolygon=0; iPart<pPolygon->Get_Part_Count(); iPart++, iPolygon++)
	{
		bool	bAscending	= pPolygon->is_Lake(iPart) == pPolygon->is_Clockwise(iPart);

		Polygons.resize(1 + iPolygon);
		Polygons[iPolygon].resize(pPolygon->Get_Point_Count(iPart));

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	p	= pPolygon->Get_Point(iPoint, iPart, bAscending);

			Polygons[iPolygon][iPoint].X	= Get_X_asInt(p.x);
			Polygons[iPolygon][iPoint].Y	= Get_Y_asInt(p.y);
		}
	}

	return( Polygons.size() > 0 );
}

//---------------------------------------------------------
bool CSG_Converter_WorldToInt::Convert(const ClipperLib::Polygons &Polygons, CSG_Shape *pPolygon) const
{
	pPolygon->Del_Parts();

	for(size_t iPolygon=0, iPart=0; iPolygon<Polygons.size(); iPolygon++)
	{
		for(size_t iPoint=0; iPoint<Polygons[iPolygon].size(); iPoint++)
		{
			pPolygon->Add_Point(
				Get_X_asWorld(Polygons[iPolygon][iPoint].X),
				Get_Y_asWorld(Polygons[iPolygon][iPoint].Y),
				iPart
			);
		}

		if( ((CSG_Shape_Polygon *)pPolygon)->Get_Area(iPart) > (1.0e-12) )
		{
			iPart++;
		}
		else
		{
			pPolygon->Del_Part(iPart);
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

	ClipperLib::Polygons		Polygon, Clip, Result;

	if(	Converter.Convert((CSG_Shape_Polygon *)pPolygon, Polygon)
	&&	Converter.Convert((CSG_Shape_Polygon *)pClip   , Clip   ) )
	{
		ClipperLib::Clipper	Clipper;

		Clipper.AddPolygons(Polygon, ClipperLib::ptSubject);
		Clipper.AddPolygons(Clip   , ClipperLib::ptClip);

		Clipper.Execute(ClipType, Result);

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
	return( _SG_Polygon_Clip(ClipperLib::ctIntersection	, pPolygon, pClip, pResult) );
}

//---------------------------------------------------------
bool	SG_Polygon_Difference	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	return( _SG_Polygon_Clip(ClipperLib::ctDifference	, pPolygon, pClip, pResult) );
}

//---------------------------------------------------------
bool	SG_Polygon_ExclusiveOr	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	return( _SG_Polygon_Clip(ClipperLib::ctXor			, pPolygon, pClip, pResult) );
}

//---------------------------------------------------------
bool	SG_Polygon_Union		(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	return( _SG_Polygon_Clip(ClipperLib::ctUnion		, pPolygon, pClip, pResult) );
}

//---------------------------------------------------------
bool	SG_Polygon_Dissolve		(CSG_Shape *pPolygon, CSG_Shape *pResult)
{
	CSG_Converter_WorldToInt	Converter(pPolygon->Get_Extent());

	ClipperLib::Polygons		Polygon, Result;

	if(	Converter.Convert((CSG_Shape_Polygon *)pPolygon, Polygon) )
	{
		ClipperLib::Clipper	Clipper;

		Clipper.AddPolygons(Polygon, ClipperLib::ptSubject);

		Clipper.Execute(ClipperLib::ctUnion, Result);

		return( Converter.Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Polygon_Simplify		(CSG_Shape *pPolygon, CSG_Shape *pResult)
{
	CSG_Converter_WorldToInt	Converter(pPolygon->Get_Extent());

	ClipperLib::Polygons		Polygon, Result;

	if(	Converter.Convert((CSG_Shape_Polygon *)pPolygon, Polygon) )
	{
		ClipperLib::SimplifyPolygons(Polygon, Result);

		return( Converter.Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Polygon_Offset		(CSG_Shape *pPolygon, double dSize, double dArc, CSG_Shape *pResult)
{
	CSG_Rect					r(pPolygon->Get_Extent());	if( dSize > 0.0 )	r.Inflate(2.1 * dSize, false);

	CSG_Converter_WorldToInt	Converter(r, true);

	ClipperLib::Polygons		Polygon, Result;

	if(	Converter.Convert((CSG_Shape_Polygon *)pPolygon, Polygon) )
	{
		ClipperLib::OffsetPolygons(Polygon, Result, dSize * Converter.Get_xScale(), ClipperLib::jtRound, dArc);

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
