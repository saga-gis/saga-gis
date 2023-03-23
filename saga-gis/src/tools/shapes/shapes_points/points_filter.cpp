
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   points_filter.cpp                   //
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
#include "points_filter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_Filter::CPoints_Filter(void)
{
	Set_Name		(_TL("Points Filter"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Shapes("",
		"FILTER"	, _TL("Filtered Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Double("",
		"RADIUS"	, _TL("Radius"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Int("",
		"MINNUM"	, _TL("Minimum Number of Points"),
		_TL("only points with given minimum number of points in search radius will be processed"),
		0, 0, true
	);

	Parameters.Add_Int("",
		"MAXNUM"	, _TL("Maximum Number of Points"),
		_TL("Number of nearest points, which will be evaluated for filtering. Set to zero to investigate all points in search radius."),
		0, 0, true
	);

	Parameters.Add_Bool("",
		"QUADRANTS"	, _TL("Quadrants"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Filter Criterion"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			_TL("keep maxima (with tolerance)"),
			_TL("keep minima (with tolerance)"),
			_TL("remove maxima (with tolerance)"),
			_TL("remove minima (with tolerance)"),
			_TL("remove below percentile"),
			_TL("remove above percentile")
		), 0
	);

	Parameters.Add_Double("",
		"TOLERANCE"	, _TL("Tolerance"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Double("",
		"PERCENT"		, _TL("Percentile"),
		_TL(""),
		50., 0., true, 100., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Filter::On_Execute(void)
{
	CSG_Shapes *pPoints = Parameters("POINTS")->asShapes();

	if( !pPoints->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	if( pPoints->Get_Count() <= 0 )
	{
		Error_Set(_TL("no points in layer"));

		return( false );
	}

	int zField = Parameters("FIELD")->asInt();

	if( !m_Search.Create(pPoints, zField) )
	{
		Error_Set(_TL("failed to initialise search engine"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pFilter = Parameters("FILTER")->asShapes();

	if( pFilter )
	{
		pFilter->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Filtered")), pPoints);
	}
	else
	{
		pPoints->Select();
	}

	//-----------------------------------------------------
	m_Method     = Parameters("METHOD"   )->asInt();
	m_nMinPoints = Parameters("MINNUM"   )->asInt();
	m_nMaxPoints = Parameters("MAXNUM"   )->asInt();
	m_Radius     = Parameters("RADIUS"   )->asDouble();
	m_Tolerance  = Parameters("TOLERANCE")->asDouble();
	m_Percentile = Parameters("PERCENT"  )->asDouble();

	bool bQuadrants = Parameters("QUADRANTS")->asBool();

	//-----------------------------------------------------
	sLong nFiltered = 0;

	for(sLong i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(i);

		if( pPoint )
		{
			bool bFilter = bQuadrants
				?  Do_Filter(pPoint->Get_Point(), pPoint->asDouble(zField), 0)
				|| Do_Filter(pPoint->Get_Point(), pPoint->asDouble(zField), 1)
				|| Do_Filter(pPoint->Get_Point(), pPoint->asDouble(zField), 2)
				|| Do_Filter(pPoint->Get_Point(), pPoint->asDouble(zField), 3)
				:  Do_Filter(pPoint->Get_Point(), pPoint->asDouble(zField)   );

			if( bFilter )
			{
				nFiltered++;

				if( !pFilter )
				{
					pPoints->Select(i, true);
				}
			}
			else if( pFilter )
			{
				pFilter->Add_Shape(pPoint);
			}
		}
	}

	//-----------------------------------------------------
	if( !pFilter )
	{
		pPoints->Del_Selection();

		DataObject_Update(pPoints);
	}

	Message_Fmt("\n%lld %s", nFiltered, _TL("points have been filtered"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Filter::Do_Filter(TSG_Point Point, double zPoint, int Quadrant)
{
	if( !m_Search.Select_Nearest_Points(Point.x, Point.y, m_nMaxPoints, m_Radius, Quadrant) )
	{
		return( false );
	}

	if( m_Search.Get_Selected_Count() <= m_nMinPoints )
	{
		return( true );
	}

	switch( m_Method )
	{
	//-----------------------------------------------------
	case 0:	// keep maxima
	case 1:	// keep minima
	case 2:	// remove maxima
	case 3:	// remove minima
		{
			for(size_t i=0; i<m_Search.Get_Selected_Count(); i++)
			{
				CSG_PRQuadTree_Leaf	*pLeaf	= m_Search.Get_Selected_Leaf(i);

				if( pLeaf )
				{
					double	z;

					if( pLeaf->has_Statistics() )
					{
						switch( m_Method )
						{
						case 0:	z	= ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Maximum();	break;	// keep maxima
						case 1:	z	= ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Minimum();	break;	// keep minima
						case 2:	z	= ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Maximum();	break;	// remove maxima
						case 3:	z	= ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Minimum();	break;	// remove minima
						}
					}
					else
					{
						z	= pLeaf->Get_Z();
					}

					switch( m_Method )
					{
					case 0:	if( zPoint < (z - m_Tolerance) )	return(  true );	break;	// keep maxima
					case 1:	if( zPoint > (z + m_Tolerance) )	return(  true );	break;	// keep minima
					case 2:	if( zPoint < (z - m_Tolerance) )	return( false );	break;	// remove maxima
					case 3:	if( zPoint > (z + m_Tolerance) )	return( false );	break;	// remove minima
					}
				}
			}

			return( m_Method <= 1 ? false : true );
		}

	//-----------------------------------------------------
	case 4:	// remove below percentile
	case 5:	// remove above percentile
		{
			double	n	= 0.0;

			for(size_t i=0; i<m_Search.Get_Selected_Count(); i++)
			{
				CSG_PRQuadTree_Leaf	*pLeaf	= m_Search.Get_Selected_Leaf(i);

				if( pLeaf )
				{
					if( pLeaf->has_Statistics() )
					{
						for(int j=0; j<((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Count(); j++)
						{
							if( zPoint > ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Value(j) )
							{
								n++;
							}
						}
					}
					else if( zPoint > pLeaf->Get_Z() )
					{
						n++;
					}
				}
			}

			n	*= 100.0 / m_Search.Get_Selected_Count();

			return( m_Method == 4 ? n < m_Percentile : n > m_Percentile );
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
