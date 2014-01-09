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
//                   kernel_density.cpp                  //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
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
#include "kernel_density.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define X_WORLD_TO_GRID(X)	(((X) - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize())
#define Y_WORLD_TO_GRID(Y)	(((Y) - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize())


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKernel_Density::CKernel_Density(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Kernel Density Estimation"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Kernel density estimation. If any point is currently in selection only selected points are taken into account.\n"
		"\n"
		"References:\n"
		"- Fotheringham, A.S., Brunsdon, C., Charlton, M. (2000): Quantitative Geography. Sage. 270p.\n"
		"- Lloyd, C.D. (2010): Spatial data analysis - An introduction for GIS users. Oxford. 206p.\n"
	));


	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "POPULATION"	, _TL("Population"),
		_TL(""),
		true
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "KERNEL"		, _TL("Kernel"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("quartic kernel"),
			_TL("gaussian kernel")
		), 0
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
int CKernel_Density::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKernel_Density::On_Execute(void)
{
	int			Population;
	double		Radius;
	CSG_Shapes	*pPoints;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	Population	= Parameters("POPULATION")	->asInt();
	Radius		= Parameters("RADIUS")		->asDouble();
	m_Kernel	= Parameters("KERNEL")		->asInt();

	if( Population < 0 || Population >= pPoints->Get_Field_Count() || pPoints->Get_Field_Type(Population) == SG_DATATYPE_String )
	{
		Population	= -1;
	}

	//-----------------------------------------------------
	m_pGrid		= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		{
			CSG_Rect	Extent(pPoints->Get_Extent());

			Extent.Inflate(Radius, false);

			if( m_Grid_Target.Init_User(Extent) && Dlg_Parameters("USER") )
			{
				m_pGrid	= m_Grid_Target.Get_User();
			}
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pGrid	= m_Grid_Target.Get_Grid();
		}
		break;
	}

	if( m_pGrid == NULL )
	{
		return( false );
	}

	//-------------------------------------------------
	m_pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Kernel Density")));
	m_pGrid->Set_NoData_Value(0.0);
	m_pGrid->Assign(0.0);

	DataObject_Set_Colors(m_pGrid, 100, SG_COLORS_BLACK_WHITE, true);

	m_dRadius	= Radius / m_pGrid->Get_Cellsize();
	m_iRadius	= 1 + (int)m_dRadius;

	//-----------------------------------------------------
	if( pPoints->Get_Selection_Count() > 0 )
	{
		for(int iPoint=0; iPoint<pPoints->Get_Selection_Count() && Set_Progress(iPoint, pPoints->Get_Selection_Count()); iPoint++)
		{
			CSG_Shape	*pPoint	= pPoints->Get_Selection(iPoint);

			Set_Kernel(pPoint->Get_Point(0), Population < 0 ? 1.0 : pPoint->asDouble(Population));
		}
	}
	else
	{
		for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
		{
			CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

			Set_Kernel(pPoint->Get_Point(0), Population < 0 ? 1.0 : pPoint->asDouble(Population));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CKernel_Density::Set_Kernel(const TSG_Point &Point, double Population)
{
	double	x	= (Point.x - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize();
	double	y	= (Point.y - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize();

	for(int iy=(int)y-m_iRadius; iy<=y+m_iRadius; iy++)
	{
		if( iy >= 0 && iy < m_pGrid->Get_NY() )
		{
			for(int ix=(int)x-m_iRadius; ix<=x+m_iRadius; ix++)
			{
				if( ix >= 0 && ix < m_pGrid->Get_NX() )
				{
					m_pGrid->Add_Value(ix, iy, Population * Get_Kernel(x - ix, y - iy));
				}
			}
		}
	}
}

//---------------------------------------------------------
inline double CKernel_Density::Get_Kernel(double dx, double dy)
{
	double	d	= SG_Get_Length(dx, dy);

	if( d >= m_dRadius )
	{
		return( 0.0 );
	}

	d	/= m_dRadius;

	switch( m_Kernel )
	{
	default:
	case 0:	// quartic kernel
		return( (3.0 / (M_PI * m_dRadius*m_dRadius)) * SG_Get_Square(1.0 - d*d) );

	case 1:	// gaussian kernel
		d	*= 2.0;
		return( exp(-0.5 * d*d) );

	case 2:	// exponential
		d	*= 2.0;
		return( exp(-d) );

	case 3:	// inverse distance
		return( pow(1.0 + d, -1.0) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
