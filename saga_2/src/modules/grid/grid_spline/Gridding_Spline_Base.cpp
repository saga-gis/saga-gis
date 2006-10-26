
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Gridding_Spline_Base.cpp               //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Gridding_Spline_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_Base::CGridding_Spline_Base(void)
{
	CSG_Parameter	*pNode;
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		""
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		""
	);

	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		"",

		CSG_String::Format("%s|%s|%s|",
			_TL("user defined"),
			_TL("grid system"),
			_TL("grid")
		), 0
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("USER", _TL("User defined grid")	, "");

	pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pParameters->Add_Range(
		pNode	, "X_EXTENT"	, _TL("X-Extent"),
		""
	);

	pParameters->Add_Range(
		pNode	, "Y_EXTENT"	, _TL("Y-Extent"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("SYSTEM"	, _TL("Choose Grid System")	, "");

	pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GRID"	, _TL("Choose Grid")		, "");

	pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);
}

//---------------------------------------------------------
CGridding_Spline_Base::~CGridding_Spline_Base(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Points(CSG_Points_3D &Points, bool bInGridOnly)
{
	int			iShape, iPart, iPoint, zField;
	double		zValue;
	TSG_Point	p;
	CSG_Shapes		*pShapes;
	CSG_Shape		*pShape;

	pShapes	= Parameters("SHAPES")	->asShapes();
	zField	= Parameters("FIELD")	->asInt();

	Points.Clear();

	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape	= pShapes->Get_Shape(iShape);
		zValue	= pShape->Get_Record()->asDouble(zField);

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				p	= pShape->Get_Point(iPoint, iPart);

				if( !bInGridOnly || m_pGrid->is_InGrid_byPos(p) )
				{
					Points.Add(p.x, p.y, zValue);
				}
			}
		}
	}

	return( Points.Get_Count() >= 3 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Grid(void)
{
	CSG_Shapes	*pShapes;
	
	pShapes	= Parameters("SHAPES")->asShapes();
	m_pGrid	= NULL;

	//-------------------------------------------------
	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( Dlg_Extra_Parameters("USER") )
		{
			m_pGrid	= _Get_Grid(pShapes->Get_Extent());
		}
		break;

	case 1:	// grid system...
		if( Dlg_Extra_Parameters("SYSTEM") )
		{
			m_pGrid	= SG_Create_Grid(*Get_Extra_Parameters("SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System(), GRID_TYPE_Float);
		}
		break;

	case 2:	// grid...
		if( Dlg_Extra_Parameters("GRID") )
		{
			m_pGrid	= Get_Extra_Parameters("GRID")->Get_Parameter("GRID")->asGrid();
		}
		break;
	}

	//-------------------------------------------------
	if( m_pGrid )
	{
		m_pGrid->Set_Name(CSG_String::Format("%s (%s)", pShapes->Get_Name(), Get_Name()));
		m_pGrid->Assign_NoData();
		Parameters("GRID")->Set_Value(m_pGrid);
	}

	//-----------------------------------------------------
	return( m_pGrid != NULL );
}

//---------------------------------------------------------
CSG_Grid * CGridding_Spline_Base::_Get_Grid(TSG_Rect Extent)
{
	CSG_Parameters	*P	= Get_Extra_Parameters("USER");

	if( !P->Get_Parameter("FIT_EXTENT")->asBool() )
	{
		Extent.xMin	= P->Get_Parameter("X_EXTENT")->asRange()->Get_LoVal();
		Extent.yMin	= P->Get_Parameter("Y_EXTENT")->asRange()->Get_LoVal();
		Extent.xMax	= P->Get_Parameter("X_EXTENT")->asRange()->Get_HiVal();
		Extent.yMax	= P->Get_Parameter("Y_EXTENT")->asRange()->Get_HiVal();
	}

	double	d	= P->Get_Parameter("CELL_SIZE")->asDouble();

	int		nx	= 1 + (int)((Extent.xMax - Extent.xMin) / d);
	int		ny	= 1 + (int)((Extent.yMax - Extent.yMin) / d);

	return( nx > 1 && ny > 1 ? SG_Create_Grid(GRID_TYPE_Float, nx, ny, d, Extent.xMin, Extent.yMin) : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::Initialise(void)
{
	m_pShapes			= Parameters("SHAPES")	->asShapes();
	m_zField			= Parameters("FIELD")	->asInt();

	return( On_Initialise() && _Get_Grid() );
}

//---------------------------------------------------------
bool CGridding_Spline_Base::Initialise(CSG_Points_3D &Points, bool bInGridOnly)
{
	return( Initialise() && _Get_Points(Points, bInGridOnly) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
