
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Shapes_Cut.cpp                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>

#include "shapes_cut.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cut_Shapes(CSG_Shapes *pPolygons, int Method, CSG_Shapes *pShapes, CSG_Shapes *pCut)
{
	if( pCut && pShapes && pShapes->is_Valid() && pPolygons && pPolygons->is_Valid() && pPolygons->Get_Extent().Intersects(pShapes->Get_Extent()) )
	{
		pCut->Create(
			pShapes->Get_Type(),
			CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Cut")),
			pShapes
		);

		for(int iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			bool		bAdd;
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( Method == 2 )		// center
			{
				bAdd	= false;

				TSG_Point	Center;

				if( pShapes->Get_Type() == SHAPE_TYPE_Polygon )
					Center	= ((CSG_Shape_Polygon *)pShape)->Get_Centroid();
				else
					Center	= pShape->Get_Extent().Get_Center();

				if( pPolygons->Select(Center) )
				{
					bAdd	= true;
				}
			}
			else if( Method == 1 )	// intersects
			{
				bAdd	= false;

				for(int iPart=0; iPart<pShape->Get_Part_Count() && !bAdd; iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart) && !bAdd; iPoint++)
					{
						if( pPolygons->Select(pShape->Get_Point(iPoint, iPart)) )
						{
							bAdd	= true;
						}
					}
				}
			}
			else					// completely contained
			{
				bAdd	= true;

				for(int iPart=0; iPart<pShape->Get_Part_Count() && bAdd; iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart) && bAdd; iPoint++)
					{
						if( pPolygons->Select(pShape->Get_Point(iPoint, iPart)) == false )
						{
							bAdd	= false;
						}
					}
				}
			}

			if( bAdd )
			{
				pCut->Add_Shape(pShape);
			}
		}

		return( pCut->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool Cut_Shapes(CSG_Rect Extent, int Method, CSG_Shapes *pShapes, CSG_Shapes *pCut)
{
	if( pCut && pShapes && pShapes->is_Valid() && Extent.Intersects(pShapes->Get_Extent()) )
	{
		pCut->Create(
			pShapes->Get_Type(),
			CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Cut")),
			pShapes
		);

		for(int iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			bool		bAdd;
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( Method == 2 )	// center
			{
				bAdd	= pShapes->Get_Type() == SHAPE_TYPE_Polygon
						? Extent.Contains(((CSG_Shape_Polygon *)pShape)->Get_Centroid())
						: Extent.Contains(pShape->Get_Extent().Get_Center());
			}
			else				// completely contained, intersects
			{
				switch( pShape->Intersects(Extent) )
				{
				case INTERSECTION_Identical:
				case INTERSECTION_Contained:
					bAdd	= true;
					break;

				case INTERSECTION_Overlaps:
				case INTERSECTION_Contains:
					bAdd	= Method == 1;
					break;

				default:
					bAdd	= false;
					break;
				}
			}

			if( bAdd )
			{
				pCut->Add_Shape(pShape);
			}
		}

		return( pCut->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Shapes * Cut_Shapes(CSG_Shapes *pPolygons, int Method, CSG_Shapes *pShapes)
{
	CSG_Shapes	*pCut	= SG_Create_Shapes();

	if( Cut_Shapes(pPolygons, Method, pShapes, pCut) )
	{
		return( pCut );
	}

	delete(pCut);

	return( NULL );
}

CSG_Shapes * Cut_Shapes(CSG_Rect Extent, int Method, CSG_Shapes *pShapes)
{
	CSG_Shapes	*pCut	= SG_Create_Shapes();

	if( Cut_Shapes(Extent, Method, pShapes, pCut) )
	{
		return( pCut );
	}

	delete(pCut);

	return( NULL );
}

//---------------------------------------------------------
CSG_String Cut_Methods_Str(void)
{
	return( CSG_String::Format(SG_T("%s|%s|%s|"),
		_TL("completely contained"),
		_TL("intersects"),
		_TL("center"))
	);
}

//---------------------------------------------------------
bool Cut_Set_Extent(CSG_Rect Extent, CSG_Shapes *pExtent, bool bClear)
{
	if( pExtent )
	{
		if( bClear )
		{
			pExtent->Create(SHAPE_TYPE_Polygon, _TL("Extent [Cut]"));
			pExtent->Add_Field("ID", TABLE_FIELDTYPE_Int);
		}

		if( pExtent->Get_Type() == SHAPE_TYPE_Polygon )
		{
			CSG_Shape	*pShape	= pExtent->Add_Shape();

			pShape->Set_Value(SG_T("ID"), pExtent->Get_Count());

			pShape->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());
			pShape->Add_Point(Extent.Get_XMin(), Extent.Get_YMax());
			pShape->Add_Point(Extent.Get_XMax(), Extent.Get_YMax());
			pShape->Add_Point(Extent.Get_XMax(), Extent.Get_YMin());
			pShape->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Cut::CShapes_Cut(void)
{
	Set_Name		(_TL("Cut Shapes Layer"));

	Set_Author		(SG_T("O. Conrad (c) 2006"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes_List(
		NULL	, "CUT"			, _TL("Cut"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "EXTENT"		, _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		Cut_Methods_Str(), 0
	);

	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Extent"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("user defined"),
			_TL("grid project"),
			_TL("shapes layer extent"),
			_TL("polygons")
		), 0
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters;

	pParameters	= Add_Parameters("CUT", _TL("Extent"), _TL(""));

	pParameters->Add_Value(
		NULL, "AX"	, _TL("Left")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "BX"	, _TL("Right")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "AY"	, _TL("Bottom")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "BY"	, _TL("Top")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "DX"	, _TL("Horizontal Range")	, _TL(""), PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	pParameters->Add_Value(
		NULL, "DY"	, _TL("Vertical Range")		, _TL(""), PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	pParameters	= Add_Parameters("GRID", _TL("Extent"), _TL(""));

	pParameters->Add_Grid_System(
		NULL, "GRID", _TL("Grid Project")		, _TL("")
	);

	pParameters	= Add_Parameters("SHAPES", _TL("Extent"), _TL(""));

	pParameters->Add_Shapes(
		NULL, "SHAPES", _TL("Shapes")			, _TL(""), PARAMETER_INPUT
	);

	pParameters	= Add_Parameters("POLYGONS", _TL("Polygons"), _TL(""));

	pParameters->Add_Shapes(
		NULL, "POLYGONS", _TL("Polygons")		, _TL(""), PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut::On_Execute(void)
{
	int							Method;
	CSG_Shapes					*pExtent, *pCut;
	CSG_Parameter_Shapes_List	*pShapes, *pCuts;

	//-----------------------------------------------------
	pShapes	= Parameters("SHAPES")	->asShapesList();
	pCuts	= Parameters("CUT")		->asShapesList();
	pExtent	= Parameters("EXTENT")	->asShapes();
	Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	if( pShapes->Get_Count() > 0 )
	{
		int			iLayer;
		CSG_Rect	r(pShapes->asShapes(0)->Get_Extent());

		for(iLayer=1; iLayer<pShapes->Get_Count(); iLayer++)
		{
			r.Union(pShapes->asShapes(iLayer)->Get_Extent());
		}

		if( Get_Extent(r) )
		{
			pCuts->Del_Items();

			Cut_Set_Extent(r, pExtent, true);

			for(iLayer=0; iLayer<pShapes->Get_Count(); iLayer++)
			{
				if( m_pPolygons )
				{
					if( Cut_Shapes(m_pPolygons, Method, pShapes->asShapes(iLayer), pCut = SG_Create_Shapes()) )
						pCuts->Add_Item(pCut);
					else
						delete(pCut);
				}
				else
				{
					if( Cut_Shapes(r, Method, pShapes->asShapes(iLayer), pCut = SG_Create_Shapes()) )
						pCuts->Add_Item(pCut);
					else
						delete(pCut);
				}
			}

			return( pCuts->Get_Count() > 0 );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut::Get_Extent(CSG_Rect &r)
{
	m_pPolygons	= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// user defined
		Get_Parameters("CUT")->Get_Parameter("AX")->Set_Value(r.Get_XMin());
		Get_Parameters("CUT")->Get_Parameter("AY")->Set_Value(r.Get_YMin());
		Get_Parameters("CUT")->Get_Parameter("BX")->Set_Value(r.Get_XMax());
		Get_Parameters("CUT")->Get_Parameter("BY")->Set_Value(r.Get_YMax());
		Get_Parameters("CUT")->Get_Parameter("DX")->Set_Value(r.Get_XRange());
		Get_Parameters("CUT")->Get_Parameter("DY")->Set_Value(r.Get_YRange());

		if( Dlg_Parameters("CUT") )
		{
			r.Assign(
				Get_Parameters("CUT")->Get_Parameter("AX")->asDouble(),
				Get_Parameters("CUT")->Get_Parameter("AY")->asDouble(),
				Get_Parameters("CUT")->Get_Parameter("BX")->asDouble(),
				Get_Parameters("CUT")->Get_Parameter("BY")->asDouble()
			);

			return( true );
		}
		break;

	//-----------------------------------------------------
	case 1:	// grid project
		if( Dlg_Parameters("GRID") )
		{
			r.Assign(Get_Parameters("GRID")->Get_Parameter("GRID")->asGrid_System()->Get_Extent());

			return( true );
		}
		break;

	//-----------------------------------------------------
	case 2:	// shapes extent
		if( Dlg_Parameters("SHAPES") )
		{
			r.Assign(Get_Parameters("SHAPES")->Get_Parameter("SHAPES")->asShapes()->Get_Extent());

			return( true );
		}
		break;

	//-----------------------------------------------------
	case 3:	// polygons
		if( Dlg_Parameters("POLYGONS") )
		{
			r.Assign(Get_Parameters("POLYGONS")->Get_Parameter("POLYGONS")->asShapes()->Get_Extent());

			m_pPolygons	= Get_Parameters("POLYGONS")->Get_Parameter("POLYGONS")->asShapes();

			return( true );
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
int CShapes_Cut::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameters->Get_Identifier(), SG_T("CUT")) )
	{
		double	ax, ay, bx, by, dx, dy, d;

		ax	= pParameters->Get_Parameter("AX")->asDouble();
		ay	= pParameters->Get_Parameter("AY")->asDouble();
		bx	= pParameters->Get_Parameter("BX")->asDouble();
		by	= pParameters->Get_Parameter("BY")->asDouble();
		dx	= pParameters->Get_Parameter("DX")->asDouble();
		dy	= pParameters->Get_Parameter("DY")->asDouble();

		if( ax > bx )	{	d	= ax;	ax	= bx;	bx	= d;	}
		if( ay > by )	{	d	= ay;	ay	= by;	by	= d;	}

		if     ( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DX")) )
		{
			bx	= ax + dx;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AX"))
			||	 !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("BX")) )
		{
			dx	= bx - ax;
		}

		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DY")) )
		{
			by	= ay + dy;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AY"))
			||	 !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("BY")) )
		{
			dy	= by - ay;
		}

		pParameters->Get_Parameter("AX")->Set_Value(ax);
		pParameters->Get_Parameter("AY")->Set_Value(ay);
		pParameters->Get_Parameter("BX")->Set_Value(bx);
		pParameters->Get_Parameter("BY")->Set_Value(by);
		pParameters->Get_Parameter("DX")->Set_Value(dx);
		pParameters->Get_Parameter("DY")->Set_Value(dy);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
