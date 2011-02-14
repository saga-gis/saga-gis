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
//                   Grid_Georeference                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Georef_Grid.cpp                    //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

// Widely rearranged by O.Conrad April 2006 !!!

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Georef_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Grid::CGeoref_Grid(void)
{
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------

	Set_Name		(_TL("Georeferencing - Grids"));

	Set_Author		(SG_T("(c) 2004 Ringeler, (c) 2006 O.Conrad"));

	Set_Description	(_TW(
		"Georeferencing of grids. Either choose the attribute fields (x/y) "
		"with the projected coordinates for the reference points (origin) or supply a "
		"additional points layer with correspondend points in the target projection. "
		"\n"
		"This library uses the Minpack routines for solving the nonlinear equations and "
		"nonlinear least squares problem. You find minpack and more information "
		"about minpack at:\n"
		"  <a target=\"_blank\" href=\"http://www.netlib.org/minpack\">"
		"  http://www.netlib.org/minpack</a>\n"
		"\n"
		"or download the C source codes:\n"
		"  <a target=\"_blank\" href=\"http://www.netlib.org/minpack/cminpack.tar\">"
		"  http://www.netlib.org/minpack/cminpack.tar</a>\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Shapes_Output(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL("")
	);


	CSG_Parameter	*pSource	= Parameters.Add_Shapes(
		NULL	, "REF_SOURCE"	, _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "REF_TARGET"	, _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field(
		pSource	, "XFIELD"		, _TL("x Position"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pSource	, "YFIELD"		, _TL("y Position"),
		_TL("")
	);


	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"),
		"SOURCE"		, _TL("Source"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		Parameters("TARGET_NODE")	, "INTERPOLATION"	, _TL("Grid Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neigbhor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);


	//-----------------------------------------------------
	Parameters.Add_Choice(
		Parameters("TARGET_NODE"),
		"TARGET_TYPE"	, _TL("Target"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("user defined"),
			_TL("grid"),
			_TL("shapes")
		), 0
	);

	//-----------------------------------------------------
	m_Grid_Target.Add_Parameters_User(Add_Parameters("GET_USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GET_GRID", _TL("Choose Grid")			, _TL("")));

	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_SHAPES"	, _TL("Choose Shapes")		, _TL(""));

	pParameters->Add_Shapes(
		NULL, "SHAPES"		, _TL("Shapes")		, _TL(""), PARAMETER_OUTPUT	, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoref_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::On_Execute(void)
{
	int		xField, yField;
	CSG_Shapes	*pShapes_A, *pShapes_B;

	//-----------------------------------------------------
	pShapes_A	= Parameters("REF_SOURCE")	->asShapes();
	pShapes_B	= Parameters("REF_TARGET")	->asShapes();
	xField		= Parameters("XFIELD")		->asInt();
	yField		= Parameters("YFIELD")		->asInt();

	//-----------------------------------------------------
	if( ( pShapes_B && m_Engine.Set_Engine(pShapes_A, pShapes_B))
	||	(!pShapes_B && m_Engine.Set_Engine(pShapes_A, xField, yField))	)
	{
		Message_Add(m_Engine.Get_Message());

		return( Get_Conversion() );
	}

	Error_Set(m_Engine.Get_Message());

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Get_Conversion(void)
{
	int				Interpolation;
	TSG_Data_Type	Type;
	TSG_Rect		Extent;
	CSG_Grid		*pSource, *pGrid;
	CSG_Shapes		*pShapes;

	//-----------------------------------------------------
	pSource			= Parameters("SOURCE")->asGrid();

	pGrid			= NULL;
	pShapes			= NULL;

	Interpolation	= Parameters("INTERPOLATION")->asInt();
	Type			= Interpolation == 0 ? pSource->Get_Type() : SG_DATATYPE_Float;

	//-----------------------------------------------------
	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// create new user defined grid...
		if( Get_Target_Extent(pSource, Extent, true) && m_Grid_Target.Init_User(Extent, pSource->Get_NY()) && Dlg_Parameters("GET_USER") )
		{
			pGrid	= m_Grid_Target.Get_User(Type);
		}
		break;

	case 1:	// select grid...
		if( Dlg_Parameters("GET_GRID") )
		{
			pGrid	= m_Grid_Target.Get_Grid(Type);
		}
		break;

	case 2:	// shapes...
		if( Dlg_Parameters("GET_SHAPES") )
		{
			pShapes	= Get_Parameters("GET_SHAPES")->Get_Parameter("SHAPES")->asShapes();

			if( pShapes == DATAOBJECT_NOTSET || pShapes == DATAOBJECT_CREATE )
			{
				Get_Parameters("GET_SHAPES")->Get_Parameter("SHAPES")->Set_Value(pShapes = SG_Create_Shapes());
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( pShapes )
	{
		Parameters("SHAPES")->Set_Value(pShapes);

		Set_Shapes(pSource, pShapes);
	}

	if( pGrid )
	{
		Set_Grid(pSource, pGrid, Interpolation);
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
inline void CGeoref_Grid::Get_MinMax(TSG_Rect &r, double x, double y)
{
	if( m_Engine.Get_Converted(x, y) )
	{
		if( r.xMin > r.xMax )
		{
			r.xMin	= r.xMax	= x;
		}
		else if( r.xMin > x )
		{
			r.xMin	= x;
		}
		else if( r.xMax < x )
		{
			r.xMax	= x;
		}

		if( r.yMin > r.yMax )
		{
			r.yMin	= r.yMax	= y;
		}
		else if( r.yMin > y )
		{
			r.yMin	= y;
		}
		else if( r.yMax < y )
		{
			r.yMax	= y;
		}
	}
}

//---------------------------------------------------------
bool CGeoref_Grid::Get_Target_Extent(CSG_Grid *pSource, TSG_Rect &Extent, bool bEdge)
{
	if( !pSource )
	{
		return( false );
	}

	int			x, y;

	Extent.xMin	= Extent.yMin	= 1.0;
	Extent.xMax	= Extent.yMax	= 0.0;

	if( bEdge )
	{
		double		d;

		for(y=0, d=pSource->Get_YMin(); y<pSource->Get_NY(); y++, d+=pSource->Get_Cellsize())
		{
			Get_MinMax(Extent, pSource->Get_XMin(), d);
			Get_MinMax(Extent, pSource->Get_XMax(), d);
		}

		for(x=0, d=pSource->Get_XMin(); x<pSource->Get_NX(); x++, d+=pSource->Get_Cellsize())
		{
			Get_MinMax(Extent, d, pSource->Get_YMin());
			Get_MinMax(Extent, d, pSource->Get_YMax());
		}
	}
	else
	{
		TSG_Point	p;

		for(y=0, p.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, p.y+=pSource->Get_Cellsize())
		{
			for(x=0, p.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, p.x+=pSource->Get_Cellsize())
			{
				if( !pSource->is_NoData(x, y) )
				{
					Get_MinMax(Extent, p.x, p.y);
				}
			}
		}
	}

	return( is_Progress() && Extent.xMin < Extent.xMax && Extent.yMin < Extent.yMax );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Set_Grid(CSG_Grid *pSource, CSG_Grid *pTarget, int Interpolation)
{
	int			x, y;
	double		z;
	TSG_Point	Pt_Source, Pt_Target;

	if( pSource && pTarget )
	{
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_ZFactor(pSource->Get_ZFactor());
		pTarget->Set_Name	(pSource->Get_Name());
		pTarget->Set_Unit	(pSource->Get_Unit());

		pTarget->Assign_NoData();

		//-------------------------------------------------
		for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
		{
			for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( m_Engine.Get_Converted(Pt_Source, true) )
				{
					z	= pSource->Get_Value(
							pSource->Get_XMin() + Pt_Source.x * pSource->Get_Cellsize(),
							pSource->Get_YMin() + Pt_Source.y * pSource->Get_Cellsize(),
							Interpolation
						);

					if( !pSource->is_NoData_Value(z) )
					{
						pTarget->Set_Value(x, y, z);
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGeoref_Grid::Set_Shapes(CSG_Grid *pSource, CSG_Shapes *pTarget)
{
	int			x, y;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Shape		*pShape;

	if( pSource && pTarget )
	{
		pTarget->Create(SHAPE_TYPE_Point, pSource->Get_Name());
		pTarget->Add_Field("Z", SG_DATATYPE_Double);

		for(y=0, Pt_Source.y=0; y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=1)
		{
			for(x=0, Pt_Source.x=0; x<pSource->Get_NX(); x++, Pt_Source.x+=1)
			{
				if( !pSource->is_NoData(x, y) )
				{
					Pt_Target	= Pt_Source;

					if( m_Engine.Get_Converted(Pt_Target) )
					{
						pShape		= pTarget->Add_Shape();
						pShape->Add_Point(Pt_Target);
						pShape->Set_Value(0, pSource->asDouble(x, y));
					}
				}
			}
		}

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
