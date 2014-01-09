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
	m_bGridPoints	= bGridPoints;

	//-----------------------------------------------------
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
		CSG_Parameter	*pNode	= Parameters.Add_Shapes(
			NULL	, "SHAPES"		, _TL("Points"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Table_Field(
			pNode	, "FIELD"		, _TL("Attribute"),
			_TL("")
		);
	}

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
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
	CSG_Grid	*pGrid		=  m_bGridPoints ? Parameters("GRIDPOINTS")->asGrid  () : NULL;
	CSG_Shapes	*pShapes	= !m_bGridPoints ? Parameters("SHAPES")    ->asShapes() : NULL;

	//-----------------------------------------------------
	m_pGrid		= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(m_bGridPoints ? pGrid->Get_Extent() : pShapes->Get_Extent()) && Dlg_Parameters("USER") )
		{
			m_pGrid	= m_Grid_Target.Get_User();
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pGrid	= m_Grid_Target.Get_Grid();
		}
		break;
	}

	//-------------------------------------------------
	if( m_pGrid )
	{
		m_pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_bGridPoints ? pGrid->Get_Name() : Parameters("FIELD")->asString(), Get_Name().c_str()));
		m_pGrid->Assign_NoData();
	}

	//-----------------------------------------------------
	return( m_pGrid != NULL );
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

	if( m_bGridPoints )
	{
		int			x, y;
		TSG_Point	p;
		CSG_Grid	*pGrid	= Parameters("GRIDPOINTS")	->asGrid();

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
	else
	{
		CSG_Shapes	*pShapes	= Parameters("SHAPES")	->asShapes();
		int			zField		= Parameters("FIELD")	->asInt();

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
