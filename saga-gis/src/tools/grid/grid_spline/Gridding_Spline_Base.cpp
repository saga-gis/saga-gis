
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "Gridding_Spline_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_Base::CGridding_Spline_Base(bool bGridPoints)
{
	if( bGridPoints )
	{
		Parameters.Add_Grid("",
			"GRID"	, _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT
		);
	}
	else
	{
		Parameters.Add_Shapes("",
			"SHAPES", _TL("Points"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Table_Field("SHAPES",
			"FIELD"	, _TL("Attribute"),
			_TL("")
		);
	}

	m_Grid_Target.Create(&Parameters, true, "", "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SHAPES") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	if( pParameter->Cmp_Identifier("GRID") && pParameter->asGrid() )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asGrid()->Get_System());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGridding_Spline_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::Initialize(CSG_Points_Z &Points, bool bInGridOnly, bool bDetrend)
{
	return( _Get_Grid() && _Get_Points(Points, bInGridOnly, bDetrend) );
}

//---------------------------------------------------------
bool CGridding_Spline_Base::Initialize(void)
{
	return( _Get_Grid() );
}

//---------------------------------------------------------
bool CGridding_Spline_Base::Finalize(bool bDetrend)
{
	if( bDetrend )
	{
		double	Mean	= Parameters("GRID")
			? Parameters("GRID"  )->asGrid  ()->Get_Mean()
			: Parameters("SHAPES")->asShapes()->Get_Mean(Parameters("FIELD")->asInt());

		if( Mean )
		{
			for(sLong i=0; i<m_pGrid->Get_NCells(); i++)
			{
				m_pGrid->Add_Value(i, Mean);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Grid(void)
{
	if( (m_pGrid = m_Grid_Target.Get_Grid()) == NULL )
	{
		return( false );
	}

	m_pGrid->Assign_NoData();

	//-----------------------------------------------------
	if( Parameters("GRID") )
	{
		CSG_Grid	*pPoints	= Parameters("GRID")->asGrid();

		m_pGrid->Fmt_Name("%s [%s]"   , pPoints->Get_Name(), Get_Name().c_str());
	}
	else
	{
		CSG_Shapes	*pPoints	= Parameters("SHAPES")->asShapes();

		m_pGrid->Fmt_Name("%s.%s [%s]", pPoints->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str());
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Points(CSG_Points_Z &Points, bool bInGridOnly, bool bDetrend)
{
	Points.Clear();

	//-----------------------------------------------------
	if( Parameters("GRID") )
	{
		CSG_Grid	*pPoints	= Parameters("GRID")->asGrid();

		double	Mean	= bDetrend ? pPoints->Get_Mean() : 0.;

		TSG_Point	p; p.y	= pPoints->Get_YMin();

		for(int y=0; y<pPoints->Get_NY() && Set_Progress(y, pPoints->Get_NY()); y++, p.y+=pPoints->Get_Cellsize())
		{
			p.x	= pPoints->Get_XMin();

			for(int x=0; x<pPoints->Get_NX(); x++, p.x+=pPoints->Get_Cellsize())
			{
				if( !pPoints->is_NoData(x, y) && (!bInGridOnly || m_pGrid->is_InGrid_byPos(p, false)) )
				{
					Points.Add(p.x, p.y, pPoints->asDouble(x, y) - Mean);
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shapes	*pPoints	= Parameters("SHAPES")->asShapes();

		int	Field	= Parameters("FIELD")->asInt();

		double	Mean	= bDetrend ? pPoints->Get_Mean(Field) : 0.;

		for(int i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
		{
			CSG_Shape	*pShape	= pPoints->Get_Shape(i);

			if( !pShape->is_NoData(Field) )
			{
				double	z	= pShape->asDouble(Field) - Mean;

				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

						if( !bInGridOnly || m_pGrid->is_InGrid_byPos(p, false) )
						{
							Points.Add(p.x, p.y, z);
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Points.Get_Count() >= 3 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
