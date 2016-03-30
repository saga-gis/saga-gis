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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	//-----------------------------------------------------
	if( bGridPoints )
	{
		Parameters.Add_Grid(
			NULL	, "GRID"	, _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT
		);
	}

	//-----------------------------------------------------
	else
	{
		CSG_Parameter	*pNode	= Parameters.Add_Shapes(
			NULL	, "SHAPES"	, _TL("Points"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Table_Field(
			pNode	, "FIELD"	, _TL("Attribute"),
			_TL("")
		);
	}

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, true, NULL, "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	return( CSG_Module::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGridding_Spline_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
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

		m_pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), Get_Name().c_str()));
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shapes	*pPoints	= Parameters("SHAPES")->asShapes();

		if( (m_pGrid = m_Grid_Target.Get_Grid()) == NULL )
		{
			return( false );
		}

		m_pGrid->Set_Name(CSG_String::Format(SG_T("%s.%s [%s]"), pPoints->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str()));
	}

	//-----------------------------------------------------
	m_pGrid->Assign_NoData();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_Base::_Get_Points(CSG_Points_Z &Points, bool bInGridOnly)
{
	Points.Clear();

	//-----------------------------------------------------
	if( Parameters("GRID") )
	{
		int			x, y;
		TSG_Point	p;
		CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

		for(y=0, p.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, p.y+=pGrid->Get_Cellsize())
		{
			for(x=0, p.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, p.x+=pGrid->Get_Cellsize())
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
		int			zField		= Parameters("FIELD" )->asInt   ();

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( !pShape->is_NoData(zField) )
			{
				double		zValue	= pShape->asDouble(zField);

				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

						if( !bInGridOnly || m_pGrid->is_InGrid_byPos(p, false) )
						{
							Points.Add(p.x, p.y, zValue);
						}
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
