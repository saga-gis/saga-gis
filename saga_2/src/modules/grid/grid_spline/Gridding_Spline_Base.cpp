
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
CGridding_Spline_Base::CGridding_Spline_Base(bool bGridPoints)
{
	CSG_Parameter	*pNode;
	CSG_Parameters	*pParameters;

	m_bGridPoints	= bGridPoints;

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		_TL("")
	);

	if( m_bGridPoints )
	{
		Parameters.Add_Grid(
			NULL	, "GRIDPOINTS"	, _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT
		);
	}
	else
	{
		pNode	= Parameters.Add_Shapes(
			NULL	, "SHAPES"		, _TL("Points"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Table_Field(
			pNode	, "FIELD"		, _TL("Attribute"),
			_TL("")
		);
	}

	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("user defined"),
			_TL("grid system"),
			_TL("grid")
		), 0
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("USER", _TL("User defined grid")	, _TL(""));

	pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pParameters->Add_Range(
		pNode	, "X_EXTENT"	, _TL("X-Extent"),
		_TL("")
	);

	pParameters->Add_Range(
		pNode	, "Y_EXTENT"	, _TL("Y-Extent"),
		_TL("")
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("SYSTEM"	, _TL("Choose Grid System")	, _TL(""));

	pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		_TL("")
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("GRID"	, _TL("Choose Grid")		, _TL(""));

	pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
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
bool CGridding_Spline_Base::_Get_Points(CSG_Points_Z &Points, bool bInGridOnly)
{
	Points.Clear();

	if( m_bGridPoints )
	{
		int			x, y;
		TSG_Point	p;
		CSG_Grid	*pGrid	= Parameters("GRIDPOINTS")	->asGrid();

		for(y=0, p.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, p.y+=pGrid->Get_Cellsize())
		{
			for(x=0, p.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, p.x+=pGrid->Get_Cellsize())
			{
				if( !pGrid->is_NoData(x, y) && (!bInGridOnly || m_pGrid->is_InGrid_byPos(p)) )
				{
					Points.Add(p.x, p.y, pGrid->asDouble(x, y));
				}
			}
		}
	}
	else
	{
		CSG_Shapes	*pShapes	= Parameters("SHAPES")	->asShapes();
		int			zField		= Parameters("FIELD")	->asInt();

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);
			double		zValue	= pShape->asDouble(zField);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

					if( !bInGridOnly || m_pGrid->is_InGrid_byPos(p) )
					{
						Points.Add(p.x, p.y, zValue);
					}
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
	CSG_Grid	*pGrid;
	CSG_Shapes	*pShapes;

	if( m_bGridPoints )
	{
		pGrid	= Parameters("GRIDPOINTS")	->asGrid();
	}
	else
	{
		pShapes	= Parameters("SHAPES")		->asShapes();
	}

	m_pGrid	= NULL;

	//-------------------------------------------------
	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( Dlg_Parameters("USER") )
		{
			m_pGrid	= _Get_Grid(m_bGridPoints ? pGrid->Get_Extent() : pShapes->Get_Extent());
		}
		break;

	case 1:	// grid system...
		if( Dlg_Parameters("SYSTEM") )
		{
			m_pGrid	= SG_Create_Grid(*Get_Parameters("SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System(), SG_DATATYPE_Float);
		}
		break;

	case 2:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pGrid	= Get_Parameters("GRID")->Get_Parameter("GRID")->asGrid();
		}
		break;
	}

	//-------------------------------------------------
	if( m_pGrid )
	{
		m_pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_bGridPoints ? pGrid->Get_Name() : pShapes->Get_Name(), Get_Name()));
		m_pGrid->Assign_NoData();
		Parameters("GRID")->Set_Value(m_pGrid);
	}

	//-----------------------------------------------------
	return( m_pGrid != NULL );
}

//---------------------------------------------------------
CSG_Grid * CGridding_Spline_Base::_Get_Grid(TSG_Rect Extent)
{
	CSG_Parameters	*P	= Get_Parameters("USER");

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

	return( nx > 1 && ny > 1 ? SG_Create_Grid(SG_DATATYPE_Float, nx, ny, d, Extent.xMin, Extent.yMin) : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::Initialise(void)
{
	return( On_Initialise() && _Get_Grid() );
}

//---------------------------------------------------------
bool CGridding_Spline_Base::Initialise(CSG_Points_Z &Points, bool bInGridOnly)
{
	return( Initialise() && _Get_Points(Points, bInGridOnly) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
