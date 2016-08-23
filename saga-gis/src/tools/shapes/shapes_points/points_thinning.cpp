/**********************************************************
 * Version $Id: points_thinning.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Points Thinning"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "THINNED"		, _TL("Thinned Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "RESOLUTION"	, _TL("Resolution"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Thinning::On_Execute(void)
{
	int			Field;
	CSG_Shapes	*pPoints;

	//-----------------------------------------------------
	pPoints			= Parameters("POINTS")		->asShapes();
	Field			= Parameters("FIELD")		->asInt();
	m_pPoints		= Parameters("THINNED")		->asShapes();
	m_Resolution	= Parameters("RESOLUTION")	->asDouble();

	//-----------------------------------------------------
	if( m_Resolution <= 0.0 )
	{
		Error_Set(_TL("resolution has to be greater than zero"));

		return( false );
	}

	if( !pPoints->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	if( pPoints->Get_Count() < 2 )
	{
		Error_Set(_TL("not more than one point in layer"));

		return( false );
	}

	if( !Set_Search_Engine(pPoints, Field) )
	{
		Error_Set(_TL("failed to initialise search engine"));

		return( false );
	}

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), pPoints->Get_Field_Name(Field)));
	m_pPoints->Add_Field(_TL("Count")	, SG_DATATYPE_Int);
	m_pPoints->Add_Field(_TL("Mean")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Minimun")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Maximun")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("StdDev")	, SG_DATATYPE_Double);

	pPoints->Select();

	//-----------------------------------------------------
	Get_Points(m_Search.Get_Root_Pointer());

	//-----------------------------------------------------
	if( m_pPoints->Get_Count() == pPoints->Get_Count() )
	{
		Message_Add(_TL("no points removed"));
	}
	else
	{
		Message_Add(CSG_String::Format(SG_T("%d %s"), pPoints->Get_Count() - m_pPoints->Get_Count(), _TL("no points removed")));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_Thinning::Set_Search_Engine(CSG_Shapes *pPoints, int Field)
{
	CSG_Rect	r(pPoints->Get_Extent());

	r.Assign(
		r.Get_XCenter() - 0.5 * m_Resolution,
		r.Get_YCenter() - 0.5 * m_Resolution,
		r.Get_XCenter() + 0.5 * m_Resolution,
		r.Get_YCenter() + 0.5 * m_Resolution
	);

	while( r.Intersects(pPoints->Get_Extent()) != INTERSECTION_Contains )
	{
		r.Inflate(200.0);
	}

	if( !m_Search.Create(r, true) )
	{
		return( false );
	}

	for(int i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		m_Search.Add_Point(
			pPoint->Get_Point(0).x,
			pPoint->Get_Point(0).y,
			pPoint->asDouble(Field)
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_Thinning::Get_Points(CSG_PRQuadTree_Item *pItem)
{
	if( pItem )
	{
		if( pItem->is_Leaf() )
		{
			Add_Point(pItem->asLeaf());
		}
		else if( pItem->Get_Size() <= m_Resolution )
		{
			Add_Point((CSG_PRQuadTree_Node_Statistics *)pItem);
		}
		else
		{
			for(int i=0; i<4; i++)
			{
				Get_Points(pItem->asNode()->Get_Child(i));
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_Thinning::Add_Point(CSG_PRQuadTree_Leaf *pLeaf)
{
	CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

	pPoint->Add_Point(pLeaf->Get_X(), pLeaf->Get_Y());

	if( pLeaf->has_Statistics() )
	{
		CSG_PRQuadTree_Leaf_List	*pList	= (CSG_PRQuadTree_Leaf_List *)pLeaf;

		pPoint->Set_Value(0, pList->Get_Count());	// Count
		pPoint->Set_Value(1, pList->Get_Mean());	// Mean
		pPoint->Set_Value(2, pList->Get_Minimum());	// Minimun
		pPoint->Set_Value(3, pList->Get_Maximum());	// Maximun
		pPoint->Set_Value(4, pList->Get_StdDev());	// StdDev
	}
	else
	{
		pPoint->Set_Value(0, 1);					// Count
		pPoint->Set_Value(1, pLeaf->Get_Z());		// Mean
		pPoint->Set_Value(2, pLeaf->Get_Z());		// Minimun
		pPoint->Set_Value(3, pLeaf->Get_Z());		// Maximun
		pPoint->Set_Value(4, 0.0);					// StdDev
	}
}

//---------------------------------------------------------
void CPoints_Thinning::Add_Point(CSG_PRQuadTree_Node_Statistics *pNode)
{
	CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

	pPoint->Add_Point(pNode->Get_X()->Get_Mean(), pNode->Get_Y()->Get_Mean());

	pPoint->Set_Value(0, pNode->Get_Z()->Get_Count());		// Count
	pPoint->Set_Value(1, pNode->Get_Z()->Get_Mean());		// Mean
	pPoint->Set_Value(2, pNode->Get_Z()->Get_Minimum());	// Minimun
	pPoint->Set_Value(3, pNode->Get_Z()->Get_Maximum());	// Maximun
	pPoint->Set_Value(4, pNode->Get_Z()->Get_StdDev());		// StdDev
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
