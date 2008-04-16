
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

#include "Polygon_Clipper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Intersection::CPolygon_Intersection(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Intersection"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Polygon_Intersection of polygon shapes. Based on the GPC (General Polygon Clipper, version 2.31) code of Alan Murta."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_A"	, _TL("Layer A"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_B"	, _TL("Layer B"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_AB"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Complete Intersection"),
			_TL("Intersection"),
			_TL("Difference (A - B)"),
			_TL("Difference (B - A)")
		), 0
	);

	pNode	= Parameters.Add_Value(
		NULL	, "SPLITPARTS"	, _TL("Split Parts"),
		_TL("Set true if you want multipart polygons to become separate polygons."),
		PARAMETER_TYPE_Bool, true
	);
}

//---------------------------------------------------------
CPolygon_Intersection::~CPolygon_Intersection(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Intersection::On_Execute(void)
{
	CSG_String	sName;
	CSG_Shapes	*pShapes_A, *pShapes_B;

	pShapes_A		= Parameters("SHAPES_A")	->asShapes();
	pShapes_B		= Parameters("SHAPES_B")	->asShapes();
	m_pShapes_AB	= Parameters("SHAPES_AB")	->asShapes();
	m_bSplitParts	= Parameters("SPLITPARTS")	->asBool();

	if(	pShapes_A->Get_Type() == SHAPE_TYPE_Polygon && pShapes_A->is_Valid()
	&&	pShapes_B->Get_Type() == SHAPE_TYPE_Polygon && pShapes_B->is_Valid() )
	{
		m_pShapes_AB->Create(SHAPE_TYPE_Polygon);
		m_pShapes_AB->Get_Table().Add_Field("ID"	, TABLE_FIELDTYPE_Int);
		m_pShapes_AB->Get_Table().Add_Field("ID_A"	, TABLE_FIELDTYPE_Int);

		//-------------------------------------------------
		switch( Parameters("METHOD")->asInt() )
		{
		//-------------------------------------------------
		case 0:	// Complete Intersection...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Intersection"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Intersection(pShapes_A, pShapes_B);

			if( Parameters("METHOD")->asInt() == 0 )	// Complete Intersection...
			{
				Get_Difference(pShapes_A, pShapes_B, 1);
				Get_Difference(pShapes_B, pShapes_A, 2);
			}

			break;

		//-------------------------------------------------
		case 1:	// Intersection...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Intersection"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Intersection(pShapes_A, pShapes_B);

			break;

		//-------------------------------------------------
		case 2:						// Difference A - B...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Difference"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Difference(pShapes_A, pShapes_B);

			break;

		//-------------------------------------------------
		case 3:						// Difference B - A...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Difference"), pShapes_B->Get_Name(), pShapes_A->Get_Name());

			Get_Difference(pShapes_B, pShapes_A);

			break;

		//-------------------------------------------------
		case 4:						// Union...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Union"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Union(pShapes_A, pShapes_B);

			break;
		}

		//-------------------------------------------------
		m_pShapes_AB->Set_Name(sName);

		return( m_pShapes_AB->Get_Count() > 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Intersection(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B)
{
	CSG_Shape	*pShape_A, *pShape_AB;
	CSG_Shapes	Tmp(SHAPE_TYPE_Polygon);

	m_pShapes_AB->Get_Table().Add_Field("ID_B"	, TABLE_FIELDTYPE_Int);

	m_ID_Mode	= 1;

	pShape_A	= Tmp.Add_Shape();
	pShape_AB	= Tmp.Add_Shape();

	for(int iShape_A=0; iShape_A<pShapes_A->Get_Count() && Set_Progress(iShape_A, pShapes_A->Get_Count()); iShape_A++)
	{
		pShapes_B->Select(pShapes_A->Get_Shape(iShape_A)->Get_Extent().m_rect);

		if( pShapes_B->Get_Selection_Count() > 0 )
		{
			pShape_A	= pShapes_A->Get_Shape(iShape_A);

			for(int iShape_B=0; iShape_B<pShapes_B->Get_Selection_Count(); iShape_B++)
			{
				if( GPC_Intersection(pShape_A, pShapes_B->Get_Selection(iShape_B), pShape_AB) )
				{
					Add_Polygon(pShape_AB, iShape_A, iShape_B);
				}
			}
		}
	}

	return( m_pShapes_AB->is_Valid() );
}

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Difference(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B, int ID_Mode)
{
	CSG_Shape	*pShape_A;
	CSG_Shapes	Tmp(SHAPE_TYPE_Polygon);

	m_ID_Mode	= ID_Mode;

	pShape_A	= Tmp.Add_Shape();

	for(int iShape_A=0; iShape_A<pShapes_A->Get_Count() && Set_Progress(iShape_A, pShapes_A->Get_Count()); iShape_A++)
	{
		if( pShapes_B->Select(pShapes_A->Get_Shape(iShape_A)->Get_Extent()) )
		{
			int		nIntersections	= 0;

			pShape_A->Assign(pShapes_A->Get_Shape(iShape_A));

			for(int iShape_B=0; iShape_B<pShapes_B->Get_Selection_Count(); iShape_B++)
			{
				if( GPC_Difference(pShape_A, pShapes_B->Get_Selection(iShape_B)) )
				{
					nIntersections++;
				}
			}

			if( nIntersections && pShape_A->is_Valid() )
			{
				Add_Polygon(pShape_A, iShape_A);
			}
		}
		else
		{
			Add_Polygon(pShapes_A->Get_Shape(iShape_A), iShape_A);
		}
	}

	return( m_pShapes_AB->is_Valid() );
}

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Union(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B)
{
	if( pShapes_A->Get_Count() > 0 )
	{
		int			iShape;
		CSG_Shape	*pShape	= Add_Polygon(0, 0);

		pShape->Assign(pShapes_A->Get_Shape(0), false);

		for(iShape=1; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
		{
			if( GPC_Union(pShape, pShapes_A->Get_Shape(iShape)) )
			{
			}
		}

		for(iShape=0; iShape<pShapes_B->Get_Count() && Set_Progress(iShape, pShapes_B->Get_Count()); iShape++)
		{
			if( GPC_Union(pShape, pShapes_B->Get_Shape(iShape)) )
			{
			}
		}
	}

	return( m_pShapes_AB->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CPolygon_Intersection::Add_Polygon(int ID_A, int ID_B)
{
	CSG_Shape	*pShape	= m_pShapes_AB->Add_Shape();

	if( pShape )
	{
		pShape->Get_Record()->Set_Value(0, m_pShapes_AB->Get_Count());

		switch( m_ID_Mode )
		{
		case 0:	// ID_A only
			pShape->Get_Record()->Set_Value(1, ID_A);
			break;

		case 1:	// ID_A first
			pShape->Get_Record()->Set_Value(1, ID_A);
			pShape->Get_Record()->Set_Value(2, ID_B);
			break;

		case 2:	// ID_B first
			pShape->Get_Record()->Set_Value(1, ID_B);
			pShape->Get_Record()->Set_Value(2, ID_A);
			break;
		}
	}

	return( pShape );
}

//---------------------------------------------------------
void CPolygon_Intersection::Add_Polygon(CSG_Shape *pShape, int ID_A, int ID_B)
{
	CSG_Shape	*pShape_Add;

	if( m_bSplitParts && pShape->Get_Part_Count() > 1 )
	{
		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			if( !((CSG_Shape_Polygon *)pShape)->is_Lake(iPart)
			&&	(pShape_Add = Add_Polygon(ID_A, ID_B)) != NULL )
			{
				int		iPoint;

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pShape_Add->Add_Point(pShape->Get_Point(iPoint, iPart), 0);
				}

				for(int jPart=0, nParts=0; jPart<pShape->Get_Part_Count(); jPart++)
				{
					if(	((CSG_Shape_Polygon *)pShape)->is_Lake(jPart)
					&&	((CSG_Shape_Polygon *)pShape)->is_Containing(pShape->Get_Point(0, jPart), iPart) )
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
	else if( (pShape_Add = Add_Polygon(ID_A, ID_B)) != NULL )
	{
		pShape_Add->Assign(pShape, false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
