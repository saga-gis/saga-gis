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
#include "Interpolation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation::CInterpolation(void)
{
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

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
int CInterpolation::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


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
	m_pGrid		= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(m_pShapes->Get_Extent()) && Dlg_Parameters("USER") )
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

	//-----------------------------------------------------
	if( m_pGrid )
	{
		m_pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("FIELD")->asString(), Get_Name().c_str()));

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::Interpolate(void)
{
	if( On_Initialize() )
	{
		int		ix, iy;
		double	x, y, z;

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
CSG_Shapes * CInterpolation::Get_Points(bool bOnlyNonPoints)
{
	m_pShapes	= Parameters("SHAPES")	->asShapes();

	if( !bOnlyNonPoints || m_pShapes->Get_Type() != SHAPE_TYPE_Point )
	{
		CSG_Shapes	*pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point);

		pPoints->Set_NoData_Value_Range(m_pShapes->Get_NoData_Value(), m_pShapes->Get_NoData_hiValue());
		pPoints->Add_Field(SG_T("Z"), SG_DATATYPE_Double);

		for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= m_pShapes->Get_Shape(iShape);

			if( !pShape->is_NoData(m_zField) )
			{
				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						CSG_Shape	*pPoint	= pPoints->Add_Shape();

						pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));

						pPoint->Set_Value(0, pShape->asDouble(m_zField));
					}
				}
			}
		}

		m_zField	= 0;
		m_pShapes	= pPoints;
	}

	return( m_pShapes );
}

//---------------------------------------------------------
bool CInterpolation::Set_Search_Engine(void)
{
	return( m_Search.Create(m_pShapes, m_zField) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
