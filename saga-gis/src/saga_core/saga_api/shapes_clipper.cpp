
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
#include "shapes.h"

#include "clipper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Converter
{
public:

	CSG_Converter(void)	{}

	//-----------------------------------------------------
	static bool	Convert	(CSG_Shapes *pPolygons, Clipper2Lib::PathsD &Polygons)
	{
		Polygons.clear();

		for(int iPolygon=0, jPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape *pPolygon = pPolygons->Get_Shape(iPolygon);

			for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++, jPolygon++)
			{
				bool bAscending = pPolygon->Get_Type() != SHAPE_TYPE_Polygon
					|| (pPolygon->asPolygon()->is_Lake     (iPart)
						==  pPolygon->asPolygon()->is_Clockwise(iPart));

				Polygons.resize(1 + jPolygon);
				Polygons[jPolygon].resize(pPolygon->Get_Point_Count(iPart));

				for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point p = pPolygon->Get_Point(iPoint, iPart, bAscending);

					Polygons[jPolygon][iPoint].x = p.x;
					Polygons[jPolygon][iPoint].y = p.y;
				}
			}
		}

		return( Polygons.size() > 0 );
	}

	//-----------------------------------------------------
	static bool	Convert	(const Clipper2Lib::PathsD &Polygons, CSG_Shapes *pPolygons)
	{
		pPolygons->Del_Shapes();

		CSG_Shape *pPolygon = pPolygons->Add_Shape();

		return( Convert(Polygons, pPolygon) );
	}

	//-----------------------------------------------------
	static bool	Convert	(CSG_Shape *pPolygon, Clipper2Lib::PathsD &Polygons)
	{
		Polygons.clear();

		for(int iPart=0, iPolygon=0; iPart<pPolygon->Get_Part_Count(); iPart++, iPolygon++)
		{
			if( pPolygon->Get_Point_Count(iPart) > 0 )
			{
				bool bAscending = pPolygon->Get_Type() != SHAPE_TYPE_Polygon
					|| (pPolygon->asPolygon()->is_Lake     (iPart)
						==  pPolygon->asPolygon()->is_Clockwise(iPart));

				Polygons.resize(1 + iPolygon);

				for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point p = pPolygon->Get_Point(iPoint, iPart, bAscending);

					Clipper2Lib::PointD Point(p.x, p.y);

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

	//-----------------------------------------------------
	static bool	Convert	(const Clipper2Lib::PathsD &Polygons, CSG_Shape *pPolygon)
	{
		pPolygon->Del_Parts();

		for(size_t iPolygon=0, iPart=0; iPolygon<Polygons.size(); iPolygon++)
		{
			for(size_t iPoint=0; iPoint<Polygons[iPolygon].size(); iPoint++)
			{
				pPolygon->Add_Point(Polygons[iPolygon][iPoint].x, Polygons[iPolygon][iPoint].y, (int)iPart);
			}

			if( pPolygon->Get_Type() != SHAPE_TYPE_Polygon || pPolygon->asPolygon()->Get_Area((int)iPart) > (1.e-12) )
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

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool _SG_Shapes_Clip(Clipper2Lib::ClipType ClipType, CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	Clipper2Lib::PathsD	Subject, Clip, Solution;

	if(	CSG_Converter::Convert(pPolygon, Subject)
		&&	CSG_Converter::Convert(pClip   , Clip   ) )
	{
		Clipper2Lib::ClipperD Clipper;

		Clipper.AddClip(Clip);

		if( pPolygon->Get_Type() != SHAPE_TYPE_Line )
		{
			Clipper.AddSubject(Subject);

			if( !Clipper.Execute(ClipType, Clipper2Lib::FillRule::NonZero, Solution) )
			{
				return( false );
			}
		}
		else
		{
			Clipper.AddOpenSubject(Subject);

			if( !Clipper.Execute(ClipType, Clipper2Lib::FillRule::NonZero, Subject) )
			{
				return( false );
			}
		}

		return( CSG_Converter::Convert(Solution, pResult ? pResult : pPolygon) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Shapes_Clipper_Intersection	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_None:
		return( false );

	case INTERSECTION_Identical:
	case INTERSECTION_Contains:
		if( pResult )
		{
			pResult ->Assign(pPolygon, false);
		}
		return( true );

	case INTERSECTION_Contained:
		if( pResult )
		{
			pResult ->Assign(pClip   , false);
		}
		else
		{
			pPolygon->Assign(pClip   , false);
		}
		return( true );

	case INTERSECTION_Overlaps: default:
		return( _SG_Shapes_Clip(Clipper2Lib::ClipType::Intersection, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Shapes_Clipper_Difference	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_Contains:
	case INTERSECTION_Identical:
		return( false );

	case INTERSECTION_None:
		if( pResult )
		{
			pResult->Assign(pPolygon, false);
		}
		return( true );

	case INTERSECTION_Contained:
	case INTERSECTION_Overlaps: default:
		return( _SG_Shapes_Clip(Clipper2Lib::ClipType::Difference, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Shapes_Clipper_ExclusiveOr	(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_Identical:
		return( false );

	case INTERSECTION_None:
		if( pResult )
		{
			pResult->Assign(pPolygon, false);
		}
		else
		{
			pResult = pPolygon;
		}

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
		return( _SG_Shapes_Clip(Clipper2Lib::ClipType::Xor, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Shapes_Clipper_Union			(CSG_Shape *pPolygon, CSG_Shape *pClip, CSG_Shape *pResult)
{
	switch( pClip->Intersects(pPolygon) )
	{
	case INTERSECTION_Contained:
	case INTERSECTION_Identical:
		if( pResult )
		{
			pResult->Assign(pPolygon, false);
		}
		return( true );

	case INTERSECTION_Contains:
		if( pResult )
		{
			pResult ->Assign(pClip  , false);
		}
		else
		{
			pPolygon->Assign(pClip  , false);
		}
		return( true );

	case INTERSECTION_None:
		if( pResult )
		{
			pResult->Assign(pPolygon, false);
		}
		else
		{
			pResult	= pPolygon;
		}

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
		return( _SG_Shapes_Clip(Clipper2Lib::ClipType::Union, pPolygon, pClip, pResult) );
	}
}

//---------------------------------------------------------
bool	SG_Shapes_Clipper_Dissolve		(CSG_Shape *pPolygon, CSG_Shape *pResult)
{
	Clipper2Lib::PathsD Subject, Solution;

	if(	CSG_Converter::Convert(pPolygon, Subject) )
	{
		Clipper2Lib::ClipperD Clipper;

		Clipper.AddSubject(Subject);

		if( Clipper.Execute(Clipper2Lib::ClipType::Union, Clipper2Lib::FillRule::NonZero, Solution) )
		{
			return( CSG_Converter::Convert(Solution, pResult ? pResult : pPolygon) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Shapes_Clipper_Offset		(CSG_Shape *pPolygon, double dSize, double dArc, CSG_Shape *pResult)
{
	Clipper2Lib::PathsD Paths, Result;

	if(	CSG_Converter::Convert(pPolygon, Paths) )
	{
		Clipper2Lib::ClipperOffset Offset(2., dArc);

		if( pPolygon->Get_Type() == SHAPE_TYPE_Polygon )
		{
			Result = Clipper2Lib::InflatePaths(Paths, dSize, Clipper2Lib::JoinType::Round, Clipper2Lib::EndType::Polygon, 2., dArc);
		}
		else
		{
			Result = Clipper2Lib::InflatePaths(Paths, dSize, Clipper2Lib::JoinType::Round, Clipper2Lib::EndType::Round  , 2., dArc);
		}

		return( CSG_Converter::Convert(Result, pResult ? pResult : pPolygon) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char *	SG_Shapes_Clipper_Get_Version	(void)
{
	return( "Clipper2 - v1.0.0" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
