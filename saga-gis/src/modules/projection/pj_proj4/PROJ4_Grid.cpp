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
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    PROJ4_Grid.cpp                     //
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
#include "PROJ4_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPROJ4_Grid::CPROJ4_Grid(int Interface, bool bInputList)
	: CPROJ4_Base(Interface, bInputList)
{
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Set_Name		(CSG_String::Format(SG_T("[deprecated] %s (%s, %s)"),
		_TL("Proj.4"),
		Interface == PROJ4_INTERFACE_DIALOG ? _TL("Dialog") : _TL("Command Line Arguments"),
		m_bInputList ? _TL("List of Grids") : _TL("Grid")
	));

	Set_Author		("O. Conrad (c) 2004-8");

	Set_Description	(_TW(
		"Coordinate Transformation for Grids.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4 Homepage</a>\n"
	));


	//-----------------------------------------------------
	m_Grid_Target.Create(Add_Parameters("TARGET", _TL("Target Grid System"), _TL("")), false);

	if( m_bInputList )
	{
		Parameters.Add_Grid_List(
			Parameters("SOURCE_NODE"),
			"SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT, false
		);

		Parameters.Add_Grid_List(
			NULL,
			"TARGET"		, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}
	else
	{
		Parameters.Add_Grid(
			Parameters("SOURCE_NODE"),
			"SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		m_Grid_Target.Add_Grid("TARGET", _TL("Target"), false);
	}

	m_Grid_Target.Add_Grid("OUT_X", _TL("X Coordinates"), true);
	m_Grid_Target.Add_Grid("OUT_Y", _TL("Y Coordinates"), true);


	//-----------------------------------------------------
	Parameters.Add_Choice(
		Parameters("TARGET_NODE")	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neigbhor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPROJ4_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CPROJ4_Base::On_Parameter_Changed(pParameters, pParameter);

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CPROJ4_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CPROJ4_Base::On_Parameters_Enable(pParameters, pParameter);

	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::On_Execute_Conversion(void)
{
	TSG_Rect	Extent;

	m_Interpolation	= Parameters("INTERPOLATION")->asInt();

	//-----------------------------------------------------
	if( m_bInputList )
	{
		CSG_Parameter_Grid_List	*pSources	= Parameters("SOURCE")->asGridList();
		CSG_Parameter_Grid_List	*pTargets	= Parameters("TARGET")->asGridList();

		if( pSources->Get_Count() < 1 )
		{
			return( false );
		}

		if( Get_Target_Extent(pSources->asGrid(0), Extent) )
		{
			m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), Extent, pSources->asGrid(0)->Get_NY());
		}

		if( !Dlg_Parameters("TARGET") )
		{
			return( false );
		}

		CSG_Grid_System	System(m_Grid_Target.Get_System());

		//-------------------------------------------------
		if( System.is_Valid() )
		{
			pTargets->Del_Items();

			for(int i=0; i<pSources->Get_Count(); i++)
			{
				pTargets->Add_Item(SG_Create_Grid(System, m_Interpolation == 0 ? pSources->asGrid(i)->Get_Type() : SG_DATATYPE_Float));

				Init_Target(pSources->asGrid(i), pTargets->asGrid(i));
			}

			return( Set_Grids(pSources, pTargets) );
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Grid	*pSource	= Parameters("SOURCE")->asGrid();

		if( Get_Target_Extent(pSource, Extent) )
		{
			m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), Extent, pSource->Get_NY());
		}

		if( !Dlg_Parameters("TARGET") )
		{
			return( false );
		}

		CSG_Grid	*pTarget	= m_Grid_Target.Get_Grid(m_Interpolation == 0 ? pSource->Get_Type() : SG_DATATYPE_Float);

		//-------------------------------------------------
		if( pTarget )
		{
			return( Set_Grid(pSource, pTarget) );
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Grids(CSG_Parameter_Grid_List *pSources, CSG_Parameter_Grid_List *pTargets)
{
	if( !pSources || pSources->Get_Count() < 1 || !pTargets || pTargets->Get_Count() != pSources->Get_Count() || !Set_Inverse() )
	{
		return( false );
	}

	//-------------------------------------------------
	int				x, y, i;
	double			z;
	TSG_Point		Pt_Source, Pt_Target;
	CSG_Grid_System	System;
	CSG_Grid		*pX, *pY;

	System	= pTargets->asGrid(0)->Get_System();

	Init_XY(System, &pX, &pY);

	//-------------------------------------------------
	for(y=0, Pt_Target.y=System.Get_YMin(); y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++, Pt_Target.y+=System.Get_Cellsize())
	{
		for(x=0, Pt_Target.x=System.Get_XMin(); x<System.Get_NX(); x++, Pt_Target.x+=System.Get_Cellsize())
		{
			Pt_Source	= Pt_Target;

			if( Get_Converted(Pt_Source) )
			{
				if( pX )	pX->Set_Value(x, y, Pt_Source.x);
				if( pY )	pY->Set_Value(x, y, Pt_Source.y);

				for(i=0; i<pSources->Get_Count(); i++)
				{
					if( pSources->asGrid(i)->Get_Value(Pt_Source, z, m_Interpolation) )
					{
						pTargets->asGrid(i)->Set_Value(x, y, z);
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Shapes(CSG_Parameter_Grid_List *pSources, CSG_Shapes *pTarget)
{
	int			x, y, i;
	double		z;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Grid	*pSource;
	CSG_Shape	*pShape;

	if( pSources && pSources->Get_Count() > 0 && pTarget )
	{
		pSource	= pSources->asGrid(0);

		pTarget->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pSource->Get_Name(), Get_Proj_Name().c_str()));

		for(i=0; i<pSources->Get_Count(); i++)
		{
			pTarget->Add_Field(pSources->asGrid(i)->Get_Name(), pSources->asGrid(i)->Get_Type());
		}

		for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=pSource->Get_Cellsize())
		{
			for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
			{
				if( !pSource->is_NoData(x, y) )
				{
					Pt_Target	= Pt_Source;

					if( Get_Converted(Pt_Target) )
					{
						pShape	= pTarget->Add_Shape();
						pShape->Add_Point(Pt_Target);

						for(i=0; i<pSources->Get_Count(); i++)
						{
							if( pSources->asGrid(i)->Get_Value(Pt_Source, z, m_Interpolation) )
							{
								pShape->Set_Value(i, z);
							}
							else
							{
								pShape->Set_NoData(i);
							}
						}
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Grid(CSG_Grid *pSource, CSG_Grid *pTarget)
{
	if( pSource && pTarget && Set_Inverse() )
	{
		int			x, y;
		double		z;
		TSG_Point	Pt_Source, Pt_Target;
		CSG_Grid	*pX, *pY;

		Init_XY(pTarget->Get_System(), &pX, &pY);

		Init_Target(pSource, pTarget);

		//-------------------------------------------------
		for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
		{
			for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( Get_Converted(Pt_Source) )
				{
					if( pX )	pX->Set_Value(x, y, Pt_Source.x);
					if( pY )	pY->Set_Value(x, y, Pt_Source.y);

					if( pSource->Get_Value(Pt_Source, z, m_Interpolation) )
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
bool CPROJ4_Grid::Set_Shapes(CSG_Grid *pSource, CSG_Shapes *pTarget)
{
	int			x, y;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Shape	*pShape;

	if( pSource && pTarget )
	{
		pTarget->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pSource->Get_Name(), Get_Proj_Name().c_str()));
		pTarget->Add_Field("Z", SG_DATATYPE_Double);

		for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=pSource->Get_Cellsize())
		{
			for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
			{
				if( !pSource->is_NoData(x, y) )
				{
					Pt_Target	= Pt_Source;

					if( Get_Converted(Pt_Target) )
					{
						pShape	= pTarget->Add_Shape();
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
bool CPROJ4_Grid::Init_XY(const CSG_Grid_System &System, CSG_Grid **ppX, CSG_Grid **ppY)
{
	*ppX	= *ppY	= NULL;

	if( (*ppX	= m_Grid_Target.Get_Grid("OUT_X", SG_DATATYPE_Float)) != NULL )
	{
		(*ppX)->Assign_NoData();
		(*ppX)->Set_Name(_TL("X Coordinates"));
	}

	if( (*ppY	= m_Grid_Target.Get_Grid("OUT_Y", SG_DATATYPE_Float)) != NULL )
	{
		(*ppY)->Assign_NoData();
		(*ppY)->Set_Name(_TL("Y Coordinates"));
	}

	return( true );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Init_Target(CSG_Grid *pSource, CSG_Grid *pTarget)
{
	if( pSource && pTarget )
	{
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_Scaling(pSource->Get_Scaling(), pSource->Get_Offset());
		pTarget->Set_Name	(CSG_String::Format(SG_T("%s [%s]"), pSource->Get_Name(), Get_Proj_Name(false).c_str()));
		pTarget->Set_Unit	(pSource->Get_Unit());
		pTarget->Assign_NoData();

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
inline void CPROJ4_Grid::Get_MinMax(TSG_Rect &r, double x, double y)
{
	if( Get_Converted(x, y) )
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
bool CPROJ4_Grid::Get_Target_Extent(CSG_Grid *pSource, TSG_Rect &Extent)
{
	if( !pSource )
	{
		return( false );
	}

	int			x, y, Resolution;

	Resolution	= 256;

	Extent.xMin	= Extent.yMin	= 1.0;
	Extent.xMax	= Extent.yMax	= 0.0;

	switch( 1 )
	{
	case 0:	// corners
		{
			Get_MinMax(Extent, pSource->Get_XMin(), pSource->Get_YMin());
			Get_MinMax(Extent, pSource->Get_XMax(), pSource->Get_YMin());
			Get_MinMax(Extent, pSource->Get_XMin(), pSource->Get_YMax());
			Get_MinMax(Extent, pSource->Get_XMax(), pSource->Get_YMax());
		}
		break;

	case 1:	// edges
		{
			double	d;

			int	yStep	= 1 + pSource->Get_NY() / Resolution;

			for(y=0, d=pSource->Get_YMin(); y<pSource->Get_NY(); y+=yStep, d+=yStep*pSource->Get_Cellsize())
			{
				Get_MinMax(Extent, pSource->Get_XMin(), d);
				Get_MinMax(Extent, pSource->Get_XMax(), d);
			}

			int	xStep	= 1 + pSource->Get_NX() / Resolution;

			for(x=0, d=pSource->Get_XMin(); x<pSource->Get_NX(); x+=xStep, d+=xStep*pSource->Get_Cellsize())
			{
				Get_MinMax(Extent, d, pSource->Get_YMin());
				Get_MinMax(Extent, d, pSource->Get_YMax());
			}
		}
		break;

	case 2:	// all cells
		{
			TSG_Point	p;

			int	xStep	= 1 + pSource->Get_NX() / Resolution;
			int	yStep	= 1 + pSource->Get_NY() / Resolution;

			for(y=0, p.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y+=yStep, p.y+=yStep*pSource->Get_Cellsize())
			{
				for(x=0, p.x=pSource->Get_XMin(); x<pSource->Get_NX(); x+=xStep, p.x+=xStep*pSource->Get_Cellsize())
				{
					if( !pSource->is_NoData(x, y) )
					{
						Get_MinMax(Extent, p.x, p.y);
					}
				}
			}
		}
		break;
	}

	return( is_Progress() && Extent.xMin < Extent.xMax && Extent.yMin < Extent.yMax );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
