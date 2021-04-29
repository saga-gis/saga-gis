
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
//                  points_thinning.cpp                  //
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
#include "points_thinning.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_Thinning::CPoints_Thinning(void)
{
	Set_Name		(_TL("Point Thinning"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"The Points Thinning tool aggregates points at a level "
		"that fits the specified resolution. The information of "
		"those points that become aggregated is based on basic "
		"statistics, i.e. mean values for coordinates and mean, "
		"minimum, maximum, standard deviation for the selected "
		"attribute. Due to the underlying spatial structure "
		"the quadtree and the raster method lead to differing, "
		"though comparable results. "

	));

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Bool("",
		"OUTPUT_PC"	, _TL("Output to Point Cloud"),
		_TL(""),
		false
	);

	Parameters.Add_Shapes("",
		"THINNED"	, _TL("Thinned Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_PointCloud("",
		"THINNED_PC", _TL("Thinned Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"RESOLUTION", _TL("Resolution"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("quadtree"),
			_TL("raster")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPoints_Thinning::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("OUTPUT_PC") )
	{
		pParameters->Set_Enabled("THINNED"   , pParameter->asBool() == false);
		pParameters->Set_Enabled("THINNED_PC", pParameter->asBool() ==  true);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Thinning::On_Execute(void)
{
	m_pPoints	= Parameters("POINTS")->asShapes();

	if( !m_pPoints->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	if( m_pPoints->Get_Count() < 2 )
	{
		Error_Set(_TL("nothing to do, there are less than two points in layer"));

		return( false );
	}

	//-----------------------------------------------------
	m_Resolution	= Parameters("RESOLUTION")->asDouble();

	if( m_Resolution <= 0. )
	{
		Error_Set(_TL("resolution has to be greater than zero"));

		return( false );
	}

	if( m_Resolution >= m_pPoints->Get_Extent().Get_XRange()
	&&  m_Resolution >= m_pPoints->Get_Extent().Get_YRange() )
	{
		Error_Set(_TL("nothing to do, resolution needs to be set smaller than the points' extent"));

		return( false );
	}

	//-----------------------------------------------------
	m_pPoints->Select();

	if( Parameters("OUTPUT_PC")->asBool() )
	{
		m_pThinned	= Parameters("THINNED_PC")->asShapes();
		m_pThinned->asPointCloud()->Create();
	}
	else
	{
		m_pThinned	= Parameters("THINNED"   )->asShapes();
		m_pThinned->asShapes    ()->Create(SHAPE_TYPE_Point);
	}

	m_Field		= Parameters("FIELD")->asInt();

	m_pThinned->Fmt_Name("%s [%s]", m_pPoints->Get_Name(), m_pPoints->Get_Field_Name(m_Field));

	m_pThinned->Add_Field("Count"  , SG_DATATYPE_Int   );
	m_pThinned->Add_Field("Mean"   , SG_DATATYPE_Double);
	m_pThinned->Add_Field("Minimun", SG_DATATYPE_Double);
	m_pThinned->Add_Field("Maximun", SG_DATATYPE_Double);
	m_pThinned->Add_Field("StdDev" , SG_DATATYPE_Double);

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	default: if( !QuadTree_Execute(Get_Extent(0)) ) { return( false ); } break;
	case  1: if( !  Raster_Execute(Get_Extent(1)) ) { return( false ); } break;
	}

	//-----------------------------------------------------
	if( m_pThinned->Get_Count() == m_pPoints->Get_Count() )
	{
		Message_Add(_TL("no points removed"));
	}
	else
	{
		Message_Fmt("\n%d %s", m_pPoints->Get_Count() - m_pThinned->Get_Count(), _TL("points removed"));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect CPoints_Thinning::Get_Extent(int Method)
{
	if( Method == 0 )
	{
		CSG_Rect	Extent(m_pPoints->Get_Extent());

		Extent.Assign(
			Extent.Get_XCenter() - 0.5 * m_Resolution, Extent.Get_YCenter() - 0.5 * m_Resolution,
			Extent.Get_XCenter() + 0.5 * m_Resolution, Extent.Get_YCenter() + 0.5 * m_Resolution
		);

		while( Extent.Intersects(m_pPoints->Get_Extent()) != INTERSECTION_Contains )
		{
			Extent.Inflate(200.);
		}

		return( Extent );
	}

	//-----------------------------------------------------
	CSG_Rect	Extent(m_pPoints->Get_Extent());

	double	dx = 0.5 * m_Resolution * (1 + (int)(Extent.Get_XRange() / m_Resolution));
	double	dy = 0.5 * m_Resolution * (1 + (int)(Extent.Get_YRange() / m_Resolution));

	Extent.Assign(
		Extent.Get_XCenter() - dx, Extent.Get_YCenter() - dy,
		Extent.Get_XCenter() + dx, Extent.Get_YCenter() + dy
	);

	return( Extent );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CPoints_Thinning::Add_Point(double x, double y, int Count, double Mean, double Minimum, double Maximum, double StdDev)
{
	if( m_pThinned->asPointCloud() )
	{
		m_pThinned->asPointCloud()->Add_Point    (x, y, Mean);
		m_pThinned->asPointCloud()->Set_Attribute(0, Count  );
		m_pThinned->asPointCloud()->Set_Attribute(1, Mean   );
		m_pThinned->asPointCloud()->Set_Attribute(2, Minimum);
		m_pThinned->asPointCloud()->Set_Attribute(3, Maximum);
		m_pThinned->asPointCloud()->Set_Attribute(4, StdDev );
	}
	else if( m_pThinned->asShapes() )
	{
		CSG_Shape	*pPoint	= m_pThinned->Add_Shape();

		pPoint->Add_Point(x, y);

		pPoint->Set_Value(0, Count  );
		pPoint->Set_Value(1, Mean   );
		pPoint->Set_Value(2, Minimum);
		pPoint->Set_Value(3, Maximum);
		pPoint->Set_Value(4, StdDev );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Thinning::QuadTree_Execute(const CSG_Rect &Extent)
{
	Process_Set_Text(_TL("initializing..."));

	if( !m_QuadTree.Create(Extent, true) )
	{
		Error_Set(_TL("initialization failed"));

		return( false );
	}

	for(int i=0; i<m_pPoints->Get_Count() && Set_Progress(i, m_pPoints->Get_Count()); i++)
	{
		CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

		m_QuadTree.Add_Point(pPoint->Get_Point(0), pPoint->asDouble(m_Field));
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("evaluating..."));

	QuadTree_Get_Points(m_QuadTree.Get_Root_Pointer());

	m_QuadTree.Destroy();

	return( true );
}

//---------------------------------------------------------
void CPoints_Thinning::QuadTree_Get_Points(CSG_PRQuadTree_Item *pItem)
{
	if( pItem )
	{
		if( pItem->is_Leaf() )
		{
			QuadTree_Add_Point(pItem->asLeaf());
		}
		else if( pItem->Get_Size() <= m_Resolution )
		{
			QuadTree_Add_Point((CSG_PRQuadTree_Node_Statistics *)pItem);
		}
		else for(int i=0; i<4; i++)
		{
			QuadTree_Get_Points(pItem->asNode()->Get_Child(i));
		}
	}
}

//---------------------------------------------------------
inline void CPoints_Thinning::QuadTree_Add_Point(CSG_PRQuadTree_Leaf *pLeaf)
{
	if( pLeaf->has_Statistics() )
	{
		CSG_PRQuadTree_Leaf_List	*pList	= (CSG_PRQuadTree_Leaf_List *)pLeaf;

		Add_Point(pLeaf->Get_X(), pLeaf->Get_Y(),
	   (int)pList->Get_Count  (), // Count
			pList->Get_Mean   (), // Mean
			pList->Get_Minimum(), // Minimun
			pList->Get_Maximum(), // Maximun
			pList->Get_StdDev ()  // StdDev
		);
	}
	else
	{
		Add_Point(pLeaf->Get_X(), pLeaf->Get_Y(),
			1             , // Count
			pLeaf->Get_Z(), // Mean
			pLeaf->Get_Z(), // Minimun
			pLeaf->Get_Z(), // Maximun
			0.              // StdDev
		);
	}
}

//---------------------------------------------------------
inline void CPoints_Thinning::QuadTree_Add_Point(CSG_PRQuadTree_Node_Statistics *pNode)
{
	Add_Point(pNode->Get_X()->Get_Mean(), pNode->Get_Y()->Get_Mean(),
   (int)pNode->Get_Z()->Get_Count  (), // Count
		pNode->Get_Z()->Get_Mean   (), // Mean
		pNode->Get_Z()->Get_Minimum(), // Minimun
		pNode->Get_Z()->Get_Maximum(), // Maximun
		pNode->Get_Z()->Get_StdDev ()  // StdDev
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Thinning::Raster_Execute(const CSG_Rect &Extent)
{
	Process_Set_Text(_TL("initializing..."));

	CSG_Grid_System System(m_Resolution, Extent);

	CSG_Grid X   (System, SG_DATATYPE_Double                );
	CSG_Grid Y   (System, SG_DATATYPE_Double                );
	CSG_Grid N   (System, SG_DATATYPE_Word                  );
	CSG_Grid Sum (System, SG_DATATYPE_Double                );
	CSG_Grid Sum2(System, SG_DATATYPE_Double                );
	CSG_Grid Min (System, m_pPoints->Get_Field_Type(m_Field));
	CSG_Grid Max (System, m_pPoints->Get_Field_Type(m_Field));

	if( !X.is_Valid() || !Y.is_Valid() || !N.is_Valid() || !Sum.is_Valid() || !Sum2.is_Valid() || !Min.is_Valid() || !Max.is_Valid() )
	{
		Error_Set(_TL("initialization failed"));

		return( false );
	}

	//---------------------------------------------------------
	for(int i=0; i<m_pPoints->Get_Count() && Set_Progress(i, m_pPoints->Get_Count()); i++)
	{
		int	x, y;	CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

		if( System.Get_World_to_Grid(x, y, pPoint->Get_Point(0)) )
		{
			double	Value	= pPoint->asDouble(m_Field);

			X   .Add_Value(x, y, pPoint->Get_Point(0).x);
			Y   .Add_Value(x, y, pPoint->Get_Point(0).y);
			N   .Add_Value(x, y, 1.);
			Sum .Add_Value(x, y, Value);
			Sum2.Add_Value(x, y, Value * Value);

			if( N.asInt(x, y) <= 1 )
			{
				Max.Set_Value(x, y, Value);
				Min.Set_Value(x, y, Value);
			}
			else
			{
				if( Max.asDouble(x, y) < Value ) { Max.Set_Value(x, y, Value); }
				if( Min.asDouble(x, y) > Value ) { Min.Set_Value(x, y, Value); }
			}
		}
	}

	//---------------------------------------------------------
	Process_Set_Text(_TL("evaluating..."));

	for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
	{
		for(int x=0; x<System.Get_NX(); x++)
		{
			int	n	= N.asInt(x, y);

			if( n > 0 )
			{
				double	Mean	= Sum.asDouble(x, y) / n;

				Add_Point(X.asDouble(x, y) / n, Y.asDouble(x, y) / n,
					n,
					Mean,
					Min.asDouble(x, y),
					Max.asDouble(x, y),
					sqrt(Sum2.asDouble(x, y) / n - Mean*Mean)
				);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
