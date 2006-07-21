
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
#include "Polygon_Intersection_GPC.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Intersection::CPolygon_Intersection(void)
{
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Polygon Intersection"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Polygon_Intersection of polygon shapes. Based on the GPC (General Polygon Clipper, version 2.31) code of Alan Murta.")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_A"	, _TL("Layer A"),
		"",
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES_B"	, _TL("Layer B"),
		"",
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Polygon Intersection"),
		"",
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		"", _TL(
		"Complete Intersection|"
		"Intersection|"
		"Difference (A - B)|")	, 0
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
	CShapes		*pShapes_A, *pShapes_B, *pShapes_AB;

	pShapes_A	= Parameters("SHAPES_A")->asShapes();
	pShapes_B	= Parameters("SHAPES_B")->asShapes();

	if(	pShapes_A->Get_Type() == SHAPE_TYPE_Polygon
	&&	pShapes_B->Get_Type() == SHAPE_TYPE_Polygon )
	{
		bSplitParts	= Parameters("SPLITPARTS")->asBool();

		//-------------------------------------------------
		switch( Parameters("METHOD")->asInt() )
		{
		//-------------------------------------------------
		case 0: case 1: default:	// Intersection...
			sName.Printf("%s [%s / %s]", _TL("Intersection"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			pShapes_AB	= Parameters("RESULT")->asShapes();
			pShapes_AB->Create(SHAPE_TYPE_Polygon, sName);
			pShapes_AB->Get_Table().Add_Field("ID"		, TABLE_FIELDTYPE_Int);
			pShapes_AB->Get_Table().Add_Field("ID_A"	, TABLE_FIELDTYPE_Int);
			pShapes_AB->Get_Table().Add_Field("ID_B"	, TABLE_FIELDTYPE_Int);

			Get_Polygon_Intersection(pShapes_A, pShapes_B, pShapes_AB);

			if( Parameters("METHOD")->asInt() == 0 )	// Complete Intersection...
			{
				Get_Difference	(pShapes_A, pShapes_B, pShapes_AB, 1);
				Get_Difference	(pShapes_B, pShapes_A, pShapes_AB, 2);
			}
			break;

		//-------------------------------------------------
		case 2:						// Difference...
			sName.Printf("%s [%s / %s]", _TL("Difference"), pShapes_A->Get_Name(), pShapes_B->Get_Name());

			pShapes_AB	= Parameters("RESULT")->asShapes();
			pShapes_AB->Create(SHAPE_TYPE_Polygon, sName);
			pShapes_AB->Get_Table().Add_Field("ID"		, TABLE_FIELDTYPE_Int);
			pShapes_AB->Get_Table().Add_Field("ID_A"	, TABLE_FIELDTYPE_Int);

			Get_Difference	(pShapes_A, pShapes_B, pShapes_AB, 0);
			break;
		}

		//-------------------------------------------------
		return( pShapes_AB->Get_Count() > 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Polygon_Intersection(CShapes *pShapes_A, CShapes *pShapes_B, CShapes *pShapes_AB)
{
	int		iShape_A, iShape_B, nResult;
	CShape	*pShape_A, *pShape_B, *pShape_AB;
	CShapes	Intersect;

	Intersect.Create(SHAPE_TYPE_Polygon);
	pShape_A	= Intersect.Add_Shape();
	pShape_B	= Intersect.Add_Shape();
	pShape_AB	= Intersect.Add_Shape();

	nResult		= 0;

	for(iShape_A=0; iShape_A<pShapes_A->Get_Count() && Set_Progress(iShape_A, pShapes_A->Get_Count()); iShape_A++)
	{
		pShapes_B->Select(pShapes_A->Get_Shape(iShape_A)->Get_Extent().m_rect);

		if( pShapes_B->Get_Selection_Count() > 0 )
		{
			pShape_A->Assign(pShapes_A->Get_Shape(iShape_A));

			for(iShape_B=0; iShape_B<pShapes_B->Get_Selection_Count(); iShape_B++)
			{
				pShape_B->Assign(pShapes_B->Get_Selection(iShape_B));

				if( GPC_Polygon_Intersection(pShape_A, pShape_B, pShape_AB) )
				{
					nResult++;

					Add_Polygon(pShapes_AB, pShape_AB, iShape_A + 1, iShape_B + 1);
				}
			}
		}
	}

	return( nResult > 0 );
}

//---------------------------------------------------------
bool CPolygon_Intersection::Get_Difference(CShapes *pShapes_A, CShapes *pShapes_B, CShapes *pShapes_AB, int bTargetID)
{
	int		iShape_A, iShape_B, nResult;
	CShape	*pShape_A, *pShape_B, *pShape_AB;
	CShapes	Intersect;

	Intersect.Create(SHAPE_TYPE_Polygon);
	pShape_A	= Intersect.Add_Shape();
	pShape_B	= Intersect.Add_Shape();
	pShape_AB	= Intersect.Add_Shape();

	nResult		= 0;

	for(iShape_A=0; iShape_A<pShapes_A->Get_Count() && Set_Progress(iShape_A, pShapes_A->Get_Count()); iShape_A++)
	{
		pShape_A->Assign(pShapes_A->Get_Shape(iShape_A));

		pShapes_B->Select(pShapes_A->Get_Shape(iShape_A)->Get_Extent().m_rect);

		if( pShapes_B->Get_Selection_Count() > 0 )
		{
			for(iShape_B=0; iShape_B<pShapes_B->Get_Selection_Count(); iShape_B++)
			{
				pShape_B->Assign(pShapes_B->Get_Selection(iShape_B));

				if( GPC_Difference(pShape_A, pShape_B, pShape_AB) )
				{
					pShape_A->Assign(pShape_AB, false);
				}
			}
		}

		if( pShape_A->is_Valid() )
		{
			nResult++;

			switch( bTargetID )
			{
			case 0:
				Add_Polygon(pShapes_AB, pShape_A, iShape_A + 1);
				break;

			case 1:
				Add_Polygon(pShapes_AB, pShape_A, iShape_A + 1, 0);
				break;

			case 2:
				Add_Polygon(pShapes_AB, pShape_A, 0, iShape_A + 1);
				break;
			}
		}
	}

	return( nResult > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_POLYGON(POLY)	if( (pShape_Add = pShapes->Add_Shape()) != NULL )\
	{\
		pShape_Add->Assign(POLY, false);\
		pShape_Add->Get_Record()->Set_Value(0, pShapes->Get_Count());\
		pShape_Add->Get_Record()->Set_Value(1, ID_A);\
		pShape_Add->Get_Record()->Set_Value(2, ID_B);\
	}

void CPolygon_Intersection::Add_Polygon(CShapes *pShapes, CShape *pShape, int ID_A, int ID_B)
{
	int		iPart, jPart, iPoint, nParts;
	CShape	*pShape_Add, *pShape_Part;
	CShapes	Shapes_Part;

	if( bSplitParts && pShape->Get_Part_Count() > 1 )
	{
		Shapes_Part.Create(SHAPE_TYPE_Polygon);
		pShape_Part	= Shapes_Part.Add_Shape();

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			if( !((CShape_Polygon *)pShape)->is_Lake(iPart) )
			{
				pShape_Part->Del_AllParts();
				nParts	= 0;

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pShape_Part->Add_Point(pShape->Get_Point(iPoint, iPart), nParts);
				}

				for(jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
				{
					if(	((CShape_Polygon *)pShape)->is_Lake(jPart)
					&&	((CShape_Polygon *)pShape)->is_Containing(pShape->Get_Point(0, jPart), iPart) )
					{
						nParts++;

						for(iPoint=0; iPoint<pShape->Get_Point_Count(jPart); iPoint++)
						{
							pShape_Part->Add_Point(pShape->Get_Point(iPoint, jPart), nParts);
						}
					}
				}

				ADD_POLYGON(pShape_Part);
			}
		}
	}
	else
	{
		ADD_POLYGON(pShape);
	}
}

#undef ADD_POLYGON

//---------------------------------------------------------
#define ADD_POLYGON(POLY)	if( (pShape_Add = pShapes->Add_Shape()) != NULL )\
	{\
		pShape_Add->Assign(POLY, false);\
		pShape_Add->Get_Record()->Set_Value(0, pShapes->Get_Count());\
		pShape_Add->Get_Record()->Set_Value(1, ID);\
	}

void CPolygon_Intersection::Add_Polygon(CShapes *pShapes, CShape *pShape, int ID)
{
	int		iPart, jPart, iPoint, nParts;
	CShape	*pShape_Add, *pShape_Part;
	CShapes	Shapes_Part;

	if( bSplitParts && pShape->Get_Part_Count() > 1 )
	{
		Shapes_Part.Create(SHAPE_TYPE_Polygon);
		pShape_Part	= Shapes_Part.Add_Shape();

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			if( !((CShape_Polygon *)pShape)->is_Lake(iPart) )
			{
				pShape_Part->Del_AllParts();
				nParts	= 0;

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pShape_Part->Add_Point(pShape->Get_Point(iPoint, iPart), nParts);
				}

				for(jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
				{
					if(	((CShape_Polygon *)pShape)->is_Lake(jPart)
					&&	((CShape_Polygon *)pShape)->is_Containing(pShape->Get_Point(0, jPart), iPart) )
					{
						nParts++;

						for(iPoint=0; iPoint<pShape->Get_Point_Count(jPart); iPoint++)
						{
							pShape_Part->Add_Point(pShape->Get_Point(iPoint, jPart), nParts);
						}
					}
				}

				ADD_POLYGON(pShape_Part);
			}
		}
	}
	else
	{
		ADD_POLYGON(pShape);
	}
}

#undef ADD_POLYGON


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Intersection::GPC_Polygon_Intersection(CShape *pShape_A, CShape *pShape_B, CShape *pShape_AB)
{
	bool		bResult;
	int			iPoint, nPoints, iPart;
	gpc_polygon	poly_A, poly_B, poly_AB;
	gpc_vertex	*Contour;

	bResult	= false;

	if(	GPC_Create_Polygon(pShape_A, &poly_A)
	&&	GPC_Create_Polygon(pShape_B, &poly_B) )
	{
		gpc_polygon_clip(GPC_INT, &poly_A, &poly_B, &poly_AB);

		if( poly_AB.num_contours > 0 )
		{
			pShape_AB->Del_AllParts();

			for(iPart=0; iPart<poly_AB.num_contours; iPart++)
			{
				Contour	= poly_AB.contour[iPart].vertex;
				nPoints	= poly_AB.contour[iPart].num_vertices;

				for(iPoint=0; iPoint<nPoints; iPoint++)
				{
					pShape_AB->Add_Point(Contour[iPoint].x, Contour[iPoint].y, iPart);
				}
			}

			bResult	= true;
		}

		gpc_free_polygon(&poly_AB);
	}

	gpc_free_polygon(&poly_A);
	gpc_free_polygon(&poly_B);

	return( bResult );
}

//---------------------------------------------------------
bool CPolygon_Intersection::GPC_Difference(CShape *pShape_A, CShape *pShape_B, CShape *pShape_AB)
{
	bool		bResult;
	int			iPoint, nPoints, iPart;
	gpc_polygon	poly_A, poly_B, poly_AB;
	gpc_vertex	*Contour;

	bResult	= false;

	if(	GPC_Create_Polygon(pShape_A, &poly_A)
	&&	GPC_Create_Polygon(pShape_B, &poly_B) )
	{
		pShape_AB->Del_AllParts();

		gpc_polygon_clip(GPC_DIFF, &poly_A, &poly_B, &poly_AB);

		if( poly_AB.num_contours > 0 )
		{
			for(iPart=0; iPart<poly_AB.num_contours; iPart++)
			{
				Contour	= poly_AB.contour[iPart].vertex;
				nPoints	= poly_AB.contour[iPart].num_vertices;

				for(iPoint=0; iPoint<nPoints; iPoint++)
				{
					pShape_AB->Add_Point(Contour[iPoint].x, Contour[iPoint].y, iPart);
				}
			}
		}

		gpc_free_polygon(&poly_AB);

		bResult	= true;
	}

	gpc_free_polygon(&poly_A);
	gpc_free_polygon(&poly_B);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Intersection::GPC_Create_Polygon(CShape *pShape, gpc_polygon *pPolygon)
{
	int				iPoint, iPart;
	TSG_Point		Point;
	gpc_vertex		*Contour;
	gpc_vertex_list	vList;

	pPolygon->contour		= NULL;
	pPolygon->hole			= NULL;
	pPolygon->num_contours	= 0;

	//-----------------------------------------------------
	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 0 )
		{
			Contour	= (gpc_vertex *)malloc(pShape->Get_Point_Count(iPart) * sizeof(gpc_vertex));

			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pShape->Get_Point(iPoint, iPart);
				Contour[iPoint].x	= Point.x;
				Contour[iPoint].y	= Point.y;
			}

			vList.num_vertices	= pShape->Get_Point_Count(iPart);
			vList.vertex		= Contour;
			gpc_add_contour(pPolygon, &vList, ((CShape_Polygon *)pShape)->is_Lake(iPart) ? 1 : 0);

			free(Contour);
		}
	}

	return( pPolygon->num_contours > 0 );
}
