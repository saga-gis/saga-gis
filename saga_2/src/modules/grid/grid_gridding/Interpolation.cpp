
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Interpolation.cpp                   //
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
#include "Interpolation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation::CInterpolation(void)
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
	pParameters	= Add_Parameters("USER", _TL("User defined grid")	, "");

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
	pParameters	= Add_Parameters("SYSTEM"	, _TL("Choose Grid System")	, "");

	pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("GRID"	, _TL("Choose Grid")		, "");

	pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);
}

//---------------------------------------------------------
CInterpolation::~CInterpolation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::On_Execute(void)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	m_pShapes	= Parameters("SHAPES")	->asShapes();
	m_zField	= Parameters("FIELD")	->asInt();

	//-----------------------------------------------------
	if( _Get_Grid() )
	{
		bResult	= Interpolate();
	}

	//-----------------------------------------------------
	m_Search.Destroy();

	if( m_pShapes != Parameters("SHAPES")->asShapes() )
	{
		delete(m_pShapes);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CInterpolation::Interpolate(void)
{
	if( On_Initialize() )
	{
		int		ix, iy;
		double	 x,  y, z;

		for(iy=0, y=m_pGrid->Get_YMin(); iy<m_pGrid->Get_NY() && Set_Progress(iy, m_pGrid->Get_NY()); iy++, y+=m_pGrid->Get_Cellsize())
		{
			for(ix=0, x=m_pGrid->Get_XMin(); ix<m_pGrid->Get_NX(); ix++, x+=m_pGrid->Get_Cellsize())
			{
				if( Get_Value(x, y, z) )
				{
					m_pGrid->Set_Value(ix, iy, z);
				}
				else
				{
					m_pGrid->Set_NoData(ix, iy);
				}
			}
		}

		On_Finalize();

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
CSG_Shapes * CInterpolation::Get_Points(void)
{
	int		iShape, iPart, iPoint;
	CSG_Shape	*pShape , *pPoint;
	CSG_Shapes	*pPoints;

	m_pShapes	= Parameters("SHAPES")	->asShapes();

	if( m_pShapes->Get_Type() != SHAPE_TYPE_Point )
	{
		pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, "", &m_pShapes->Get_Table());

		for(iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
		{
			pShape	= m_pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pShape->Get_Record());
					pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));
				}
			}
		}

		m_pShapes	= pPoints;
	}

	return( m_pShapes );
}

//---------------------------------------------------------
bool CInterpolation::Set_Search_Engine(void)
{
	return( m_Search.Create(Get_Points()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::_Get_Grid(void)
{
	CSG_Shapes	*pShapes;
	
	pShapes	= Parameters("SHAPES")->asShapes();
	m_pGrid	= NULL;

	//-------------------------------------------------
	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( Dlg_Parameters("USER") )
		{
			m_pGrid	= _Get_Grid(pShapes->Get_Extent());
		}
		break;

	case 1:	// grid system...
		if( Dlg_Parameters("SYSTEM") )
		{
			m_pGrid	= SG_Create_Grid(*Get_Parameters("SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System(), GRID_TYPE_Float);
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
		m_pGrid->Set_Name(CSG_String::Format("%s (%s)", pShapes->Get_Name(), Get_Name()));
		Parameters("GRID")->Set_Value(m_pGrid);
	}

	//-----------------------------------------------------
	return( m_pGrid != NULL );
}

//---------------------------------------------------------
CSG_Grid * CInterpolation::_Get_Grid(TSG_Rect Extent)
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

	return( nx > 1 && ny > 1 ? SG_Create_Grid(GRID_TYPE_Float, nx, ny, d, Extent.xMin, Extent.yMin) : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
