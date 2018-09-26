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
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Polygon_Intersection.cpp               //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "Polygon_Intersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Overlay::CPolygon_Overlay(const CSG_String &Name)
{
	//-----------------------------------------------------
	Set_Name		(Name);

	Set_Author		("O.Conrad (c) 2003-17");

	Set_Description	(_TW(
		"Uses the free and open source software library <b>Clipper</b> created by Angus Johnson.\n"
		"<a target=\"_blank\" href=\"http://www.angusj.com/delphi/clipper.php\">Clipper Homepage</a>\n"
		"<a target=\"_blank\" href=\"http://sourceforge.net/projects/polyclipping/\">Clipper at SourceForge</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"A"		, _TL("Layer A"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"B"		, _TL("Layer B"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"RESULT", Name,
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Bool("",
		"SPLIT"	, _TL("Split Parts"),
		_TL("Set true if you want multipart polygons to become separate polygons."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Overlay::Add_Description(const CSG_String &Text)
{
	Set_Description(Text + "\n" + Get_Description());

	return( true );
}

//---------------------------------------------------------
bool CPolygon_Overlay::Initialize(CSG_Shapes **ppA, CSG_Shapes **ppB, bool bBothAttributes)
{
	*ppA	= Parameters("A")->asShapes(); if( (*ppA)->Get_Type() != SHAPE_TYPE_Polygon || !(*ppA)->is_Valid() ) return( false );
	*ppB	= Parameters("B")->asShapes(); if( (*ppB)->Get_Type() != SHAPE_TYPE_Polygon || !(*ppB)->is_Valid() ) return( false );

	m_bSplit	= Parameters("SPLIT")->asBool();

	m_pA	= NULL;
	m_pB	= NULL;

	m_pAB	= Parameters("RESULT")->asShapes();

	if( m_pAB == *ppA || m_pAB == *ppB )
	{
		Error_Set(_TL("Output layer must not be one of the input layers!"));

		return( false );
	}

	m_pAB->Create(SHAPE_TYPE_Polygon, SG_T(""), *ppA);
	m_pAB->Set_Name("%s [%s]-[%s]", Get_Name().c_str(), (*ppA)->Get_Name(), (*ppB)->Get_Name());

	if( bBothAttributes )
	{
		for(int i=0; i<(*ppB)->Get_Field_Count(); i++)
		{
			m_pAB->Add_Field((*ppB)->Get_Field_Name(i), (*ppB)->Get_Field_Type(i));
		}
	}

	return(	true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Overlay::Get_Intersection(CSG_Shapes *pA, CSG_Shapes *pB)
{
	m_bInvert	= false;

	m_pA	= pA;
	m_pB	= pB;

	CSG_Shapes	Result(SHAPE_TYPE_Polygon);

	CSG_Shape_Polygon	*pResult	= (CSG_Shape_Polygon *)Result.Add_Shape();

	//-----------------------------------------------------
	for(int id_A=0; id_A<m_pA->Get_Count() && Set_Progress(id_A, m_pA->Get_Count()); id_A++)
	{
		for(int id_B=0; id_B<m_pB->Get_Count(); id_B++)
		{
			if( SG_Polygon_Intersection(m_pA->Get_Shape(id_A), m_pB->Get_Shape(id_B), pResult) )
			{
				_Add_Polygon(pResult, id_A, id_B);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Overlay::Get_Difference(CSG_Shapes *pA, CSG_Shapes *pB, bool bInvert)
{
	m_bInvert	= bInvert;

	m_pA	= pA;
	m_pB	= pB;

	CSG_Shapes	Result(SHAPE_TYPE_Polygon);

	CSG_Shape_Polygon	*pResult	= (CSG_Shape_Polygon *)Result.Add_Shape();

	//-----------------------------------------------------
	for(int id_A=0; id_A<m_pA->Get_Count() && Set_Progress(id_A, m_pA->Get_Count()); id_A++)
	{
		pResult->Assign(m_pA->Get_Shape(id_A), false);

		for(int id_B=0; id_B<m_pB->Get_Count() && pResult->is_Valid(); id_B++)
		{
			switch( pResult->Intersects(m_pB->Get_Shape(id_B)) )
			{
			case INTERSECTION_None:
				break;

			case INTERSECTION_Identical:
			case INTERSECTION_Contained:
				pResult->Del_Parts();
				break;

			case INTERSECTION_Contains:
			case INTERSECTION_Overlaps:
				SG_Polygon_Difference(pResult, m_pB->Get_Shape(id_B));
				break;
			}
		}

		if( pResult->is_Valid() )
		{
			_Add_Polygon(pResult, id_A);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Overlay::_Add_Polygon(CSG_Shape_Polygon *pPolygon, int id_A, int id_B)
{
	if( !_Fit_Polygon(pPolygon) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !m_bSplit || pPolygon->Get_Part_Count() <= 1 )
	{
		CSG_Shape_Polygon	*pNew	= _Add_Polygon(id_A, id_B);

		if( pNew )
		{
			pNew->Assign(pPolygon, false);
		}

		return( true );
	}

	//-----------------------------------------------------
	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		CSG_Shape_Polygon	*pNew	= pPolygon->is_Lake(iPart) ? NULL : _Add_Polygon(id_A, id_B);

		if( pNew )
		{
			pNew->Add_Part(pPolygon->Get_Part(iPart));

			for(int jPart=0; jPart<pPolygon->Get_Part_Count(); jPart++)
			{
				if(	pPolygon->is_Lake(jPart) && pNew->Contains(pPolygon->Get_Point(0, jPart)) )
				{
					pNew->Add_Part(pPolygon->Get_Part(jPart));
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
CSG_Shape_Polygon * CPolygon_Overlay::_Add_Polygon(int id_A, int id_B)
{
	CSG_Shape	*pOriginal, *pNew	= m_pAB->Add_Shape();

	if( pNew )
	{
		if( 1 )
		{
			for(int i=0; i<m_pAB->Get_Field_Count(); i++)
			{
				pNew->Set_NoData(i);
			}
		}

		if( (pOriginal = m_pA->Get_Shape(id_A)) != NULL )
		{
			for(int i=0, j=m_bInvert ? m_pB->Get_Field_Count() : 0; i<m_pA->Get_Field_Count() && j<m_pAB->Get_Field_Count(); i++, j++)
			{
				if( pOriginal->is_NoData(i) ) pNew->Set_NoData(i); else *pNew->Get_Value(j)	= *pOriginal->Get_Value(i);
			}
		}

		if( (pOriginal = m_pB->Get_Shape(id_B)) != NULL )
		{
			for(int i=0, j=m_bInvert ? 0 : m_pA->Get_Field_Count(); i<m_pB->Get_Field_Count() && j<m_pAB->Get_Field_Count(); i++, j++)
			{
				if( pOriginal->is_NoData(i) ) pNew->Set_NoData(i); else *pNew->Get_Value(j)	= *pOriginal->Get_Value(i);
			}
		}
	}

	return( (CSG_Shape_Polygon *)pNew );
}

//---------------------------------------------------------
bool CPolygon_Overlay::_Fit_Polygon(CSG_Shape_Polygon *pPolygon)
{
	for(int iPart=pPolygon->Get_Part_Count()-1; iPart>=0; iPart--)
	{
		if( pPolygon->Get_Area(iPart) <= 0.0 )
		{
			pPolygon->Del_Part(iPart);
		}
		else if( pPolygon->Get_Point_Count(iPart) <= 3 )
		{
			CSG_Point	a(pPolygon->Get_Point(0, iPart)), b(pPolygon->Get_Point(1, iPart)), c(pPolygon->Get_Point(2, iPart));

			if( a == b || b == c || c == a )
			{
				pPolygon->Del_Part(iPart);
			}
		}
	}

	return( pPolygon->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Intersection::CPolygon_Intersection(void)
	: CPolygon_Overlay(_TL("Intersect"))
{
	Add_Description(_TW(
		"Calculates the geometric intersection of the overlayed polygon layers, "
		"i.e. layer A and layer B."
	));
}

//---------------------------------------------------------
bool CPolygon_Intersection::On_Execute(void)
{
	CSG_Shapes	*pA, *pB;

	if( !CPolygon_Overlay::Initialize(&pA, &pB, true) )
	{
		return( false );
	}

	return( Get_Intersection(pA, pB) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Difference::CPolygon_Difference(void)
	: CPolygon_Overlay(_TL("Difference"))
{
	Add_Description(_TW(
		"Calculates the geometric difference of the overlayed polygon layers, "
		"i.e. layer A less layer B. Sometimes referred to as \'Erase\' command."
	));
}

//---------------------------------------------------------
bool CPolygon_Difference::On_Execute(void)
{
	CSG_Shapes	*pA, *pB;

	if( !CPolygon_Overlay::Initialize(&pA, &pB, false) )
	{
		return( false );
	}

	return( Get_Difference(pA, pB) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_SymDifference::CPolygon_SymDifference(void)
	: CPolygon_Overlay(_TL("Symmetrical Difference"))
{
	Add_Description(_TW(
		"Calculates the symmetrical geometric difference of the overlayed polygon layers, "
		"i.e. layer A less layer B plus layer B less layer A."
	));
}

//---------------------------------------------------------
bool CPolygon_SymDifference::On_Execute(void)
{
	CSG_Shapes	*pA, *pB;

	if( !CPolygon_Overlay::Initialize(&pA, &pB, true) )
	{
		return( false );
	}

	return( Get_Difference(pA, pB, false)
		&&  Get_Difference(pB, pA,  true) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Union::CPolygon_Union(void)
	: CPolygon_Overlay(_TL("Union"))
{
	Add_Description(_TW(
		"Calculates the geometric union of the overlayed polygon layers, "
		"i.e. the intersection plus the symmetrical difference of layers A and B."
	));
}

//---------------------------------------------------------
bool CPolygon_Union::On_Execute(void)
{
	CSG_Shapes	*pA, *pB;

	if( !CPolygon_Overlay::Initialize(&pA, &pB, true) )
	{
		return( false );
	}

	return( Get_Intersection(pA, pB)
		&&  Get_Difference  (pA, pB, false)
		&&  Get_Difference  (pB, pA,  true) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Identity::CPolygon_Identity(void)
	: CPolygon_Overlay(_TL("Identity"))
{
	Add_Description(_TW(
		"Calculates the geometric intersection between both layers "
		"and adds the difference of layer A less layer B."
	));
}

//---------------------------------------------------------
bool CPolygon_Identity::On_Execute(void)
{
	CSG_Shapes	*pA, *pB;

	if( !CPolygon_Overlay::Initialize(&pA, &pB, true) )
	{
		return( false );
	}

	return( Get_Intersection(pA, pB)
		&&  Get_Difference  (pA, pB) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Update::CPolygon_Update(void)
	: CPolygon_Overlay(_TL("Update"))
{
	Add_Description(_TW(
		"Updates features of layer A with the features of layer B, "
		"i.e. all features of layer B will be supplemented with the "
		"difference of layer A less layer B plus. It is assumed, "
		"that both input layers share the same attribute structure."
	));
}

//---------------------------------------------------------
bool CPolygon_Update::On_Execute(void)
{
	CSG_Shapes	*pA, *pB;

	if( !CPolygon_Overlay::Initialize(&pA, &pB, false) )
	{
		return( false );
	}

	if( !Get_Difference(pA, pB) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pAB	= Parameters("RESULT")->asShapes();

	for(int i=0; i<pB->Get_Count(); i++)
	{
		pAB->Add_Shape(pB->Get_Shape(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
