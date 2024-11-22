
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
//                  shapes_clipper.cpp                   //
//                                                       //
//                 Copyright (C) 2022 by                 //
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

#include "clipper2/clipper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Clipper
{
public:

	CSG_Clipper(void)	{}

	//-----------------------------------------------------
	static bool	to_Paths	(const CSG_Shapes *pShapes, Clipper2Lib::PathsD &Paths)
	{
		if( !pShapes )
		{
			return( false );
		}

		Paths.clear();

		for(sLong iShape=0, iPath=0; iShape<pShapes->Get_Count(); iShape++)
		{
			CSG_Shape &Shape = *pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<Shape.Get_Part_Count(); iPart++, iPath++)
			{
				bool bAscending = Shape.Get_Type() != SHAPE_TYPE_Polygon
					|| (Shape.asPolygon()->is_Lake     (iPart)
					==  Shape.asPolygon()->is_Clockwise(iPart));

				Paths.resize(1 + iPath);
				Paths[iPath].resize(Shape.Get_Point_Count(iPart));

				for(int iPoint=0; iPoint<Shape.Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point p = Shape.Get_Point(iPoint, iPart, bAscending);

					Paths[iPath][iPoint].x = p.x;
					Paths[iPath][iPoint].y = p.y;
				}
			}
		}

		return( Paths.size() > 0 );
	}

	//-----------------------------------------------------
	static bool	to_Shape	(const Clipper2Lib::PathsD &Paths, CSG_Shapes *pShapes)
	{
		return( pShapes && pShapes->Del_Shapes() && to_Shape(Paths, pShapes->Add_Shape()) );
	}

	//-----------------------------------------------------
	static bool	to_Paths	(const CSG_Shape *pShape, Clipper2Lib::PathsD &Paths, bool bCheckOrientation = true)
	{
		if( !pShape )
		{
			return( false );
		}

		Paths.clear();

		for(int iPart=0, iPath=0; iPart<pShape->Get_Part_Count(); iPart++, iPath++)
		{
			if( pShape->Get_Point_Count(iPart) > 0 )
			{
				bool bAscending = !bCheckOrientation || pShape->Get_Type() != SHAPE_TYPE_Polygon
					|| (pShape->asPolygon()->is_Lake     (iPart)
					==  pShape->asPolygon()->is_Clockwise(iPart));

				Paths.resize(1 + iPath);

				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point p = pShape->Get_Point(iPoint, iPart, bAscending);

					Clipper2Lib::PointD Point(p.x, p.y);

					if( iPoint == 0 || Paths[iPath].back() != Point )	// don't add duplicates !!!
					{
						Paths[iPath].push_back(Point);
					}
				}

				if( pShape->Get_Type() == SHAPE_TYPE_Polygon && Paths[iPath].size() > 1 && Paths[iPath][0] == Paths[iPath].back() )
				{
					Paths[iPath].pop_back();
				}
			}
		}

		return( Paths.size() > 0 );
	}

	//-----------------------------------------------------
	static bool	to_Shape	(const Clipper2Lib::PathsD &Paths, CSG_Shape *pShape)
	{
		if( !pShape )
		{
			return( false );
		}

		pShape->Del_Parts();

		for(size_t iPath=0; iPath<Paths.size(); iPath++)
		{
			for(size_t iPoint=0; iPoint<Paths[iPath].size(); iPoint++)
			{
				pShape->Add_Point(Paths[iPath][iPoint].x, Paths[iPath][iPoint].y, (int)iPath);
			}
		}

		return( pShape->is_Valid() );
	}

	//-----------------------------------------------------
	static bool	Clip		(Clipper2Lib::ClipType ClipType, CSG_Shape *pSubject, CSG_Shape_Polygon *pClip, CSG_Shape *pSolution)
	{
		Clipper2Lib::PathsD	Subject, Clip, Solution;

		if(	to_Paths(pSubject, Subject)
		&&	to_Paths(pClip   , Clip   ) )
		{
			Clipper2Lib::ClipperD Clipper(m_Precision);

			Clipper.AddClip(Clip);

			if( pSubject->Get_Type() == SHAPE_TYPE_Polygon )
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

				if( !Clipper.Execute(ClipType, Clipper2Lib::FillRule::NonZero, Solution) )
				{
					return( false );
				}
			}

			return( to_Shape(Solution, pSolution ? pSolution : pSubject) );
		}

		return( false );
	}

	//-----------------------------------------------------
	static bool	Dissolve	(CSG_Shape *pShape, CSG_Shape *pSolution)
	{
		Clipper2Lib::PathsD Subject, Solution;

		if(	to_Paths(pShape, Subject, false) )
		{
			Clipper2Lib::ClipperD Clipper(m_Precision);

			Clipper.AddSubject(Subject);

			if( Clipper.Execute(Clipper2Lib::ClipType::Union, Clipper2Lib::FillRule::NonZero, Solution) )
			{
				return( to_Shape(Solution, pSolution ? pSolution : pShape) );
			}
		}

		return( false );
	}

	//-----------------------------------------------------
	static bool	Offset		(CSG_Shape *pShape, double Delta, double dArc, CSG_Shape *pSolution)
	{
		Clipper2Lib::PathsD Paths, Solution;

		if(	to_Paths(pShape, Paths) )
		{
			Clipper2Lib::EndType EndType;

			if( pShape->Get_Type() != SHAPE_TYPE_Polygon )
			{
				EndType = Clipper2Lib::EndType::Round;
			}
			else
			{
				EndType = Clipper2Lib::EndType::Polygon;
			}

			double ArcTolerance = std::pow(10., m_Precision) * Delta * (1. - cos(dArc / 2.));

			Solution = Clipper2Lib::InflatePaths(Paths, Delta, Clipper2Lib::JoinType::Round, EndType, 2., m_Precision, ArcTolerance);

			return( to_Shape(Solution, pSolution ? pSolution : pShape) );
		}

		return( false );
	}

	//-----------------------------------------------------
	static int			Get_Precision	(void)	{ return( m_Precision ); }

	static bool			Set_Precision	(int Precision)
	{
		if( Precision < -8 || Precision > 8 )
		{
			return( false );
		}

		m_Precision = Precision;

		return( true );
	}

	private:

		static int	m_Precision;
};

//---------------------------------------------------------
int CSG_Clipper::m_Precision = 4;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Shape_Get_Intersection	(CSG_Shape *pShape, CSG_Shape_Polygon *pClip, CSG_Shape *pSolution)
{
	switch( pClip->Intersects(pShape) )
	{
	case INTERSECTION_None     :
		return( false );

	case INTERSECTION_Identical:
	case INTERSECTION_Contains :
		return( !pSolution || pSolution->Assign(pShape, false) );

	case INTERSECTION_Contained:
		return( !pSolution ? pShape->Assign(pClip, false) : pSolution->Assign(pClip, false) );

	case INTERSECTION_Overlaps : default:
		return( CSG_Clipper::Clip(Clipper2Lib::ClipType::Intersection, pShape, pClip, pSolution) );
	}
}

//---------------------------------------------------------
bool	SG_Shape_Get_Difference	(CSG_Shape *pShape, CSG_Shape_Polygon *pClip, CSG_Shape *pSolution)
{
	switch( pClip->Intersects(pShape) )
	{
	case INTERSECTION_None     :
		return( !pSolution || pSolution->Assign(pShape, false) );

	case INTERSECTION_Identical:
	case INTERSECTION_Contains :
		return( false );

	case INTERSECTION_Contained:
	case INTERSECTION_Overlaps : default:
		return( CSG_Clipper::Clip(Clipper2Lib::ClipType::Difference, pShape, pClip, pSolution) );
	}
}

//---------------------------------------------------------
bool	SG_Shape_Get_ExclusiveOr	(CSG_Shape *pShape, CSG_Shape_Polygon *pClip, CSG_Shape *pSolution)
{
	switch( pClip->Intersects(pShape) )
	{
	case INTERSECTION_None     :
		if( pSolution ) { pSolution->Assign(pShape, false); } else { pSolution = pShape; }

		for(int iPart=0; iPart<pClip->Get_Part_Count(); iPart++)
		{
			pSolution->Add_Part(pClip->Get_Part(iPart));
		}
		return( true );	

	case INTERSECTION_Identical:
		return( false );

	case INTERSECTION_Contains :
	case INTERSECTION_Contained:
	case INTERSECTION_Overlaps : default:
		return( CSG_Clipper::Clip(Clipper2Lib::ClipType::Xor, pShape, pClip, pSolution) );
	}
}

//---------------------------------------------------------
bool	SG_Shape_Get_Union			(CSG_Shape *pShape, CSG_Shape_Polygon *pClip, CSG_Shape *pSolution)
{
	switch( pClip->Intersects(pShape) )
	{
	case INTERSECTION_None     :
		if( pSolution ) { pSolution->Assign(pShape, false); } else { pSolution = pShape; }

		for(int iPart=0; iPart<pClip->Get_Part_Count(); iPart++)
		{
			pSolution->Add_Part(pClip->Get_Part(iPart));
		}
		return( true );	

	case INTERSECTION_Identical:
	case INTERSECTION_Contained:
		return( !pSolution || pSolution->Assign(pShape, false) );

	case INTERSECTION_Contains :
		return( !pSolution ? pShape->Assign(pClip, false) : pSolution->Assign(pClip, false) );

	case INTERSECTION_Overlaps: default:
		return( CSG_Clipper::Clip(Clipper2Lib::ClipType::Union, pShape, pClip, pSolution) );
	}
}

//---------------------------------------------------------
/**
* Assuming supplied shape object is of type polygon and has
* several parts, it is important that the point order of
* outer rings (i.e. not lakes) is clockwise, that of lakes
* counter-clockwise to be processed correctly by the Clipper
* dissolve function. This is in accordance with the simple
* features definition but in contrast to ESRI's Shapefile format.
*/
//---------------------------------------------------------
bool	SG_Shape_Get_Dissolve		(CSG_Shape *pShape, CSG_Shape *pSolution)
{
	return( CSG_Clipper::Dissolve(pShape, pSolution) );
}

//---------------------------------------------------------
bool	SG_Shape_Get_Offset		(CSG_Shape *pShape, double Size, double dArc, CSG_Shape *pSolution)
{
	return( CSG_Clipper::Offset(pShape, Size, dArc, pSolution) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char *	SG_Clipper_Get_Version	(void)
{
	static CSG_String Version(CSG_String("Clipper2 ") + CLIPPER2_VERSION);

	return( Version );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
