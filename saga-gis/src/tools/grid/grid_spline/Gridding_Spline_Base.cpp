/**********************************************************
 * Version $Id$
 *********************************************************/

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

	m_Grid_Target.Create(&Parameters, true, NULL, "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "GRID") && pParameter->asGrid() )
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Grid(void)
{
	//-----------------------------------------------------
	if( Parameters("GRID") )
	{
		CSG_Grid	*pPoints	= Parameters("GRID")->asGrid();

		if( (m_pGrid = m_Grid_Target.Get_Grid()) == NULL )
		{
			return( false );
		}

		m_pGrid->Set_Name(CSG_String::Format("%s [%s]", pPoints->Get_Name(), Get_Name().c_str()));
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shapes	*pPoints	= Parameters("SHAPES")->asShapes();

		if( (m_pGrid = m_Grid_Target.Get_Grid()) == NULL )
		{
			return( false );
		}

		m_pGrid->Set_Name(CSG_String::Format("%s.%s [%s]", pPoints->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str()));
	}

	//-----------------------------------------------------
	m_pGrid->Assign_NoData();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Points(CSG_Points_Z &Points, bool bInGridOnly)
{
	Points.Clear();

	//-----------------------------------------------------
	if( Parameters("GRID") )
	{
		CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

		TSG_Point	p; p.y	= pGrid->Get_YMin();

		for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, p.y+=pGrid->Get_Cellsize())
		{
			p.x	= pGrid->Get_XMin();

			for(int x=0; x<pGrid->Get_NX(); x++, p.x+=pGrid->Get_Cellsize())
			{
				if( !pGrid->is_NoData(x, y) && (!bInGridOnly || m_pGrid->is_InGrid_byPos(p, false)) )
				{
					Points.Add(p.x, p.y, pGrid->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

		int	Field	= Parameters("FIELD")->asInt();

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( !pShape->is_NoData(Field) )
			{
				double	z	= pShape->asDouble(Field);

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
