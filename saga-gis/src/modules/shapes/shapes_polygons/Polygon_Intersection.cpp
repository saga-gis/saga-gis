/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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

	Set_Author		(SG_T("O.Conrad (c) 2003-13"));

	Set_Description	(_TW(
		"Uses the free and open source software library <b>Clipper</b> created by Angus Johnson.\n"
		"<a target=\"_blank\" href=\"http://www.angusj.com/delphi/clipper.php\">Clipper Homepage</a>\n"
		"<a target=\"_blank\" href=\"http://sourceforge.net/projects/polyclipping/\">Clipper at SourceForge</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "A"		, _TL("Layer A"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "B"		, _TL("Layer B"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"	, Name,
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "SPLIT"	, _TL("Split Parts"),
		_TL("Set true if you want multipart polygons to become separate polygons."),
		PARAMETER_TYPE_Bool, true
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

	m_pAB->Create(SHAPE_TYPE_Polygon, SG_T(""), *ppA);
	m_pAB->Set_Name(CSG_String::Format(SG_T("%s [%s]-[%s]"), Get_Name().c_str(), (*ppA)->Get_Name(), (*ppB)->Get_Name()));

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

	CSG_Shapes	Tmp(SHAPE_TYPE_Polygon);

	CSG_Shape	*pShape_AB	= Tmp.Add_Shape();

	//-----------------------------------------------------
	for(int id_A=0; id_A<m_pA->Get_Count() && Set_Progress(id_A, m_pA->Get_Count()); id_A++)
	{
		if( m_pB->Select(m_pA->Get_Shape(id_A)->Get_Extent()) )
		{
			CSG_Shape	*pShape_A	= m_pA->Get_Shape(id_A);

			for(int id_B=0; id_B<m_pB->Get_Selection_Count(); id_B++)
			{
				CSG_Shape	*pShape_B	= m_pB->Get_Selection(id_B);

				if( SG_Polygon_Intersection(pShape_A, pShape_B, pShape_AB) )
				{
					Add_Polygon(pShape_AB, id_A, pShape_B->Get_Index());
				}
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

	CSG_Shapes	Tmp(SHAPE_TYPE_Polygon);

	CSG_Shape	*pShape_A	= Tmp.Add_Shape();

	//-----------------------------------------------------
	for(int id_A=0; id_A<m_pA->Get_Count() && Set_Progress(id_A, m_pA->Get_Count()); id_A++)
	{
		if( m_pB->Select(m_pA->Get_Shape(id_A)->Get_Extent()) )
		{
			int		nIntersections	= 0;

			pShape_A->Assign(m_pA->Get_Shape(id_A));

			for(int id_B=0; id_B<m_pB->Get_Selection_Count(); id_B++)
			{
				if( SG_Polygon_Difference(pShape_A, m_pB->Get_Selection(id_B)) )
				{
					nIntersections++;
				}
			}

			if( nIntersections > 0 && pShape_A->is_Valid() )
			{
				Add_Polygon(pShape_A, id_A);
			}
		}
		else
		{
			Add_Polygon(m_pA->Get_Shape(id_A), id_A);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CPolygon_Overlay::Add_Polygon(int id_A, int id_B)
{
	CSG_Shape	*pOriginal, *pNew	= m_pAB->Add_Shape();

	if( pNew )
	{
		if( (pOriginal = m_pA->Get_Shape(id_A)) != NULL )
		{
			for(int i=0, j=m_bInvert ? m_pB->Get_Field_Count() : 0; i<m_pA->Get_Field_Count(); i++, j++)
			{
				if( pNew->Get_Value(j) )	*pNew->Get_Value(j)	= *pOriginal->Get_Value(i);
			}
		}

		if( (pOriginal = m_pB->Get_Shape(id_B)) != NULL )
		{
			for(int i=0, j=m_bInvert ? 0 : m_pA->Get_Field_Count(); i<m_pB->Get_Field_Count(); i++, j++)
			{
				if( pNew->Get_Value(j) )	*pNew->Get_Value(j)	= *pOriginal->Get_Value(i);
			}
		}
	}

	return( pNew );
}

//---------------------------------------------------------
void CPolygon_Overlay::Add_Polygon(CSG_Shape *pShape, int id_A, int id_B)
{
	int			iPoint, iPart, jPart, nParts;
	CSG_Shape	*pShape_Add;

	for(iPart=pShape->Get_Part_Count()-1; iPart>=0; iPart--)
	{
		if( ((CSG_Shape_Polygon *)pShape)->Get_Area(iPart) <= 0.0 )
		{
			pShape->Del_Part(iPart);
		}
		else if( pShape->Get_Point_Count(iPart) <= 3 )
		{
			CSG_Point	a(pShape->Get_Point(0, iPart)), b(pShape->Get_Point(1, iPart)), c(pShape->Get_Point(2, iPart));

			if( a == b || b == c || c == a )
			{
				pShape->Del_Part(iPart);
			}
		}
	}

	if( pShape->is_Valid() )
	{
		if( m_bSplit && pShape->Get_Part_Count() > 1 )
		{
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				if( !((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) && (pShape_Add = Add_Polygon(id_A, id_B)) != NULL )
				{
					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						pShape_Add->Add_Point(pShape->Get_Point(iPoint, iPart), 0);
					}

					for(jPart=0, nParts=0; jPart<pShape->Get_Part_Count(); jPart++)
					{
						if(	((CSG_Shape_Polygon *)pShape)->is_Lake(jPart)
						&&	((CSG_Shape_Polygon *)pShape)->Contains(pShape->Get_Point(0, jPart), iPart) )
						{
							nParts++;

							for(iPoint=0; iPoint<pShape->Get_Point_Count(jPart); iPoint++)
							{
								pShape_Add->Add_Point(pShape->Get_Point(iPoint, jPart), nParts);
							}
						}
					}
				}
			}
		}
		else if( (pShape_Add = Add_Polygon(id_A, id_B)) != NULL )
		{
			pShape_Add->Assign(pShape, false);
		}
	}
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

	return( Get_Difference(pA, pB)
		&&  Get_Difference(pB, pA, true) );
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
		&&  Get_Difference  (pA, pB)
		&&  Get_Difference  (pB, pA, true) );
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
