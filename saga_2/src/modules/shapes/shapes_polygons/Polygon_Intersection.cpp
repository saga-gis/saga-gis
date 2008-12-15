
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
#define MODE_FIRST		1
#define MODE_SECOND		2


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
		"Intersection of polygon layers. Uses GPC - General Polygon Clipper - version 2.31 by Alan Murta."
	));

	/*/-----------------------------------------------------
	pNode	= Parameters.Add_Shapes_List(
		NULL	, "SHAPES"		, _TL("Polygon Layers"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	/**/

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_A"	, _TL("Layer A"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_A"		, _TL("Attribute A"),
		_TL(""),
		true
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_B"	, _TL("Layer B"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_B"		, _TL("Attribute B"),
		_TL(""),
		true
	);

	/**/

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_AB"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
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

/*/---------------------------------------------------------
bool CPolygon_Intersection::On_Execute(void)
{
	CSG_Parameter_Shapes_List	*pList;

	pList			= Parameters("SHAPES")		->asShapesList();
	m_pShapes_AB	= Parameters("SHAPES_AB")	->asShapes();
	m_bSplitParts	= Parameters("SPLITPARTS")	->asBool();

	if(	pList->Get_Count() < 2 )
	{
		Message_Add(_TL("At least two polygons have to be given for intersection"));
	}
	else
	{
		m_pShapes_AB->Create(SHAPE_TYPE_Polygon);
		m_pShapes_AB->Add_Field("ID"	, TABLE_FIELDTYPE_Int);
		m_pShapes_AB->Add_Field("ID_A"	, TABLE_FIELDTYPE_Int);

		Intersect(pList->asShapes(0), pList->asShapes(1));

		//-------------------------------------------------
		if( pList->Get_Count() > 2 )
		{
			CSG_Shapes	Tmp;

			for(int i=2; i<pList->Get_Count() && Process_Get_Okay(false); i++)
			{
				Tmp.Create(*m_pShapes_AB);

				Intersect(&Tmp, pList->asShapes(i));
			}
		}

		//-------------------------------------------------
		return( m_pShapes_AB->is_Valid() );
	}

	return( false );
}

//---------------------------------------------------------
bool CPolygon_Intersection::Intersect(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B)
{
	CSG_String	sName;

	if(	pShapes_A->Get_Type() == SHAPE_TYPE_Polygon && pShapes_A->is_Valid()
	&&	pShapes_B->Get_Type() == SHAPE_TYPE_Polygon && pShapes_B->is_Valid() )
	{
		m_pShapes_AB->Del_Shapes();

		switch( Parameters("METHOD")->asInt() )
		{
		//-------------------------------------------------
		case 0:	// Complete Intersection...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Intersection"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Intersection(pShapes_A, pShapes_B, MODE_FIRST);
			Get_Difference	(pShapes_A, pShapes_B, MODE_FIRST);
			Get_Difference	(pShapes_B, pShapes_A, MODE_SECOND);

			break;

		//-------------------------------------------------
		case 1:	// Intersection...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Intersection"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Intersection(pShapes_A, pShapes_B, MODE_SINGLE);

			break;

		//-------------------------------------------------
		case 2:						// Difference A - B...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Difference"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			Get_Difference	(pShapes_A, pShapes_B, MODE_SINGLE);

			break;

		//-------------------------------------------------
		case 3:						// Difference B - A...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Difference"), pShapes_B->Get_Name(), pShapes_A->Get_Name());

			Get_Difference	(pShapes_B, pShapes_A, MODE_SINGLE);

			break;
		}

		//-------------------------------------------------
		m_pShapes_AB->Set_Name(sName);

		return( m_pShapes_AB->Get_Count() > 0 );
	}

	return( false );
}/**/

//---------------------------------------------------------
bool CPolygon_Intersection::On_Execute(void)
{
	CSG_String	sName;

	m_pShapes_A		= Parameters("SHAPES_A")	->asShapes();
	m_pShapes_B		= Parameters("SHAPES_B")	->asShapes();
	m_pShapes_AB	= Parameters("SHAPES_AB")	->asShapes();
	m_bSplitParts	= Parameters("SPLITPARTS")	->asBool();
	m_iField_A		= Parameters("FIELD_A")		->asInt();	if( m_iField_A >= m_pShapes_A->Get_Field_Count() )	{	m_iField_A	= -1;	}
	m_iField_B		= Parameters("FIELD_B")		->asInt();	if( m_iField_B >= m_pShapes_B->Get_Field_Count() )	{	m_iField_B	= -1;	}

	if(	m_pShapes_A->Get_Type() == SHAPE_TYPE_Polygon && m_pShapes_A->is_Valid()
	&&	m_pShapes_B->Get_Type() == SHAPE_TYPE_Polygon && m_pShapes_B->is_Valid() )
	{
		m_pShapes_AB->Create(SHAPE_TYPE_Polygon);
		m_pShapes_AB->Add_Field("ID"	, TABLE_FIELDTYPE_Int);
		m_pShapes_AB->Add_Field("ID_A"	, TABLE_FIELDTYPE_Int);
		m_pShapes_AB->Add_Field("ID_B"	, TABLE_FIELDTYPE_Int);
		m_pShapes_AB->Add_Field("ID_AB"	, TABLE_FIELDTYPE_Int);

		if( m_iField_A >= 0 )
		{
			m_pShapes_AB->Add_Field(m_pShapes_A->Get_Field_Name(m_iField_A), m_pShapes_A->Get_Field_Type(m_iField_A));
		}

		if( m_iField_B >= 0 )
		{
			m_pShapes_AB->Add_Field(m_pShapes_B->Get_Field_Name(m_iField_B), m_pShapes_B->Get_Field_Type(m_iField_B));
		}

		//-------------------------------------------------
		switch( Parameters("METHOD")->asInt() )
		{
		//-------------------------------------------------
		case 0:	// Complete Intersection...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Intersection"), m_pShapes_A->Get_Name(), m_pShapes_B->Get_Name());

			Get_Intersection(m_pShapes_A, m_pShapes_B, MODE_FIRST);
			Get_Difference	(m_pShapes_A, m_pShapes_B, MODE_FIRST);
			Get_Difference	(m_pShapes_B, m_pShapes_A, MODE_SECOND);

			break;

		//-------------------------------------------------
		case 1:	// Intersection...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Intersection"), m_pShapes_A->Get_Name(), m_pShapes_B->Get_Name());

			Get_Intersection(m_pShapes_A, m_pShapes_B, MODE_FIRST);

			break;

		//-------------------------------------------------
		case 2:						// Difference A - B...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Difference"), m_pShapes_A->Get_Name(), m_pShapes_B->Get_Name());

			Get_Difference	(m_pShapes_A, m_pShapes_B, MODE_FIRST);

			break;

		//-------------------------------------------------
		case 3:						// Difference B - A...
			sName.Printf(SG_T("%s [%s]-[%s]"), _TL("Difference"), m_pShapes_B->Get_Name(), m_pShapes_A->Get_Name());

			Get_Difference	(m_pShapes_B, m_pShapes_A, MODE_SECOND);

			break;
		}

		//-------------------------------------------------
		m_pShapes_AB->Set_Name(sName);

		return( m_pShapes_AB->Get_Count() > 0 );
	}

	return( false );
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Intersection(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B, int Mode)
{
	CSG_Shape	*pShape_A, *pShape_B, *pShape_AB;
	CSG_Shapes	Tmp(SHAPE_TYPE_Polygon);

	m_Mode		= Mode;

	pShape_A	= Tmp.Add_Shape();
	pShape_AB	= Tmp.Add_Shape();

	for(int iShape_A=0; iShape_A<pShapes_A->Get_Count() && Set_Progress(iShape_A, pShapes_A->Get_Count()); iShape_A++)
	{
		if( pShapes_B->Select(pShapes_A->Get_Shape(iShape_A)->Get_Extent()) )
		{
			pShape_A	= pShapes_A->Get_Shape(iShape_A);

			for(int iShape_B=0; iShape_B<pShapes_B->Get_Selection_Count(); iShape_B++)
			{
				pShape_B	= pShapes_B->Get_Selection(iShape_B);

				if( GPC_Intersection(pShape_A, pShape_B, pShape_AB) )
				{
					Add_Polygon(pShape_AB, iShape_A, pShape_B->Get_Index());
				}
			}
		}
	}

	return( m_pShapes_AB->is_Valid() );
}

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Difference(CSG_Shapes *pShapes_A, CSG_Shapes *pShapes_B, int Mode)
{
	CSG_Shape	*pShape_A;
	CSG_Shapes	Tmp(SHAPE_TYPE_Polygon);

	m_Mode		= Mode;

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CPolygon_Intersection::Get_Polygon(int iShape_A, int iShape_B)
{
	CSG_Shape	*pShape	= m_pShapes_AB->Add_Shape();

	if( pShape )
	{
		pShape->Set_Value(0, m_pShapes_AB->Get_Count());

		if( m_Mode == MODE_SECOND )
		{
			int		ID	= iShape_A;	iShape_A	= iShape_B;	iShape_B	= ID;
		}

		pShape->Set_Value(1, iShape_A);
		pShape->Set_Value(2, iShape_B);
		pShape->Set_Value(3, iShape_A >= 0 && iShape_B >= 0 ? 0 : iShape_A >= 0 ? -1 : 1);

		if( m_iField_A >= 0 && iShape_A >= 0 )
		{
			pShape->Set_Value(4, m_pShapes_A->Get_Shape(iShape_A)->asString(m_iField_A));
		}

		if( m_iField_B >= 0 && iShape_B >= 0 )
		{
			pShape->Set_Value(m_iField_A >= 0 ? 5 : 4, m_pShapes_B->Get_Shape(iShape_B)->asString(m_iField_B));
		}
	}

	return( pShape );
}

//---------------------------------------------------------
void CPolygon_Intersection::Add_Polygon(CSG_Shape *pShape, int iShape_A, int iShape_B)
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
		if( m_bSplitParts && pShape->Get_Part_Count() > 1 )
		{
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				if( !((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) && (pShape_Add = Get_Polygon(iShape_A, iShape_B)) != NULL )
				{
					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						pShape_Add->Add_Point(pShape->Get_Point(iPoint, iPart), 0);
					}

					for(jPart=0, nParts=0; jPart<pShape->Get_Part_Count(); jPart++)
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
		else if( (pShape_Add = Get_Polygon(iShape_A, iShape_B)) != NULL )
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
