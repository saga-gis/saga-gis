/**********************************************************
 * Version $Id: wombling.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     wombling.cpp                      //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "wombling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double	SG_Get_Angle_Diff(double a, double b)
{
	double	d	= fmod(b - a, M_PI_360);

	if( d < 0.0 )	d	+= M_PI_360;

	return( d > M_PI_180 ? d - M_PI_180 : d );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWombling_Base::CWombling_Base(void)
{
	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "TMAGNITUDE"		, _TL("Minimum Magnitude"),
		_TL("Minimum magnitude as percentile."),
		PARAMETER_TYPE_Double, 90.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "TDIRECTION"		, _TL("Maximum Angle"),
		_TL("Maximum angular difference as degree between adjacent segment points."),
		PARAMETER_TYPE_Double, 30.0, 0.0, true, 180.0, true
	);

	Parameters.Add_Choice(
		NULL	, "ALIGNMENT"		, _TL("Alignment"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("between cells"),
			_TL("on cell")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "NEIGHBOUR"		, _TL("Edge Connectivity"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Rooke's case"),
			_TL("Queen's case")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWombling_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling_Base::Initialize(CSG_Grid Gradient[2], CSG_Grid *pEdges)
{
	m_Neighbour		= Parameters("NEIGHBOUR")->asInt() == 0 ? 2 : 1;
	m_minNeighbours	= 1;
	m_maxAngle		= Parameters("TDIRECTION")->asDouble() * M_DEG_TO_RAD;

	//-----------------------------------------------------
	CSG_Grid_System	System;

	if( Parameters("ALIGNMENT")->asInt() == 1 )
	{
		System	= *Get_System();
	}
	else
	{
		System.Assign(Get_Cellsize(),
			Get_XMin() + 0.5 * Get_Cellsize(),
			Get_YMin() + 0.5 * Get_Cellsize(),
			Get_NX() - 1, Get_NY() - 1
		);
	}

	//-----------------------------------------------------
	Gradient[0].Create(System);	Gradient[0].Set_NoData_Value(-1.0);
	Gradient[1].Create(System);	Gradient[1].Set_NoData_Value(-1.0);

	pEdges->Create(System, SG_DATATYPE_Char); pEdges->Set_NoData(0.0);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling_Base::Get_Gradient(CSG_Grid Gradient[2], CSG_Grid *pFeature, bool bOrientation)
{
	int	Alignment	= Parameters("ALIGNMENT")->asInt() == 1;

	Gradient[0].Set_Name(CSG_String::Format("%s [%s]", pFeature->Get_Name(), _TL("Magnitude")));
	Gradient[1].Set_Name(CSG_String::Format("%s [%s]", pFeature->Get_Name(), _TL("Direction")));

	//-----------------------------------------------------
	for(int y=0; y<Gradient[0].Get_NY() && Set_Progress(y, Gradient[0].Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Gradient[0].Get_NX(); x++)
		{
			double	Slope, Aspect, z[4];

			if( Alignment == 1 )
			{
				if( !pFeature->Get_Gradient(x, y, Slope, Aspect) )
				{
					Slope	= -1.0;
				}
			}
			else if( pFeature->is_NoData(x    , y    )
				||   pFeature->is_NoData(x + 1, y    )
				||   pFeature->is_NoData(x    , y + 1)
				||   pFeature->is_NoData(x + 1, y + 1) )
			{
				Slope	= -1.0;
			}
			else
			{
				z[0]	= pFeature->asDouble(x    , y    );
				z[1]	= pFeature->asDouble(x + 1, y    );
				z[2]	= pFeature->asDouble(x    , y + 1);
				z[3]	= pFeature->asDouble(x + 1, y + 1);

				_Get_Gradient_2x2(z, Slope, Aspect);
			}

			//---------------------------------------------
			if( Slope < 0.0 )
			{
				Gradient[0].Set_NoData(x, y);
			}
			else
			{
				Gradient[0].Set_Value(x, y, Slope);
			}

			if( Aspect < 0.0 )
			{
				Gradient[1].Set_NoData(x, y);
			}
			else
			{
				Gradient[1].Set_Value(x, y, bOrientation && Aspect > M_PI_180 ? Aspect - M_PI_180 : Aspect);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
inline void CWombling_Base::_Get_Gradient_2x2(double z[4], double &Slope, double &Aspect)
{
	double	a	= (-z[2] + z[3] + z[0] - z[1]) * 0.5 / Get_Cellsize();
	double	b	= ( z[2] + z[3] - z[0] - z[1]) * 0.5 / Get_Cellsize();

	Slope	= sqrt(a*a + b*b);
	Aspect	= a != 0.0 ? M_PI_180 + atan2(b, a)
		: b >  0.0 ? M_PI_270
		: b <  0.0 ? M_PI_090 : -1;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling_Base::Get_Edge_Cells(CSG_Grid *Gradient, CSG_Grid *pEdges)
{
	CSG_Shapes	*pPoints	= Parameters("EDGE_POINTS") ? Parameters("EDGE_POINTS")->asShapes() : NULL;

	if( pPoints )
	{
		pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s.%s", Parameters("FEATURE")->asGrid()->Get_Name(), _TL("Edges")));

		pPoints->Add_Field("ID"       , SG_DATATYPE_Int   );
		pPoints->Add_Field("MAGNITUDE", SG_DATATYPE_Double);
		pPoints->Add_Field("DIRECTION", SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	int		y;

	Lock_Create();

	//-----------------------------------------------------
	// 1. magnitude

	double	Threshold	= Gradient[0].Get_Percentile(Parameters("TMAGNITUDE")->asDouble());

	for(y=0; y<Gradient[0].Get_NY() && Set_Progress(y, Gradient[0].Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Gradient[0].Get_NX(); x++)
		{
			if( !Gradient[1].is_NoData(x, y) && !Gradient[0].is_NoData(x, y) && Gradient[0].asDouble(x, y) >= Threshold )
			{
				Lock_Set(x, y);
			}
		}
	}

	//-----------------------------------------------------
	// 2. neighbours and direction

	for(y=0; y<Gradient[0].Get_NY() && Set_Progress(y, Gradient[0].Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Gradient[0].Get_NX(); x++)
		{
			if( _is_Edge_Cell(Gradient, x, y) )
			{
				pEdges->Set_Value(x, y, 1.0);

				if( pPoints )
				{
					CSG_Shape	*pPoint	= pPoints->Add_Shape();

					pPoint->Set_Point(Gradient[0].Get_System().Get_Grid_to_World(x, y), 0);
					pPoint->Set_Value(0, pPoints->Get_Count());
					pPoint->Set_Value(1, Gradient[0].asDouble(x, y));
					pPoint->Set_Value(2, Gradient[1].asDouble(x, y) * M_RAD_TO_DEG);
				}
			}
			else
			{
				pEdges->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	Lock_Destroy();

	return( true );
}

//---------------------------------------------------------
bool CWombling_Base::_is_Edge_Cell(CSG_Grid Gradient[2], int x, int y)
{
	int		n	= 0;

	if( Lock_Get(x, y) )
	{
		for(int i=0; i<8; i+=m_Neighbour)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( Gradient[0].is_InGrid(ix, iy) && Lock_Get(ix, iy) && SG_Get_Angle_Diff(Gradient[1].asDouble(x, y), Gradient[1].asDouble(ix, iy)) <= m_maxAngle )
			{
				n++;
			}
		}
	}

	return( n >= m_minNeighbours );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling_Base::Get_Edge_Lines(CSG_Grid Gradient[2], CSG_Grid *pEdge)
{
	CSG_Shapes	*pLines	= Parameters("EDGE_LINES") ? Parameters("EDGE_LINES")->asShapes() : NULL;

	if( !pLines )
	{
		return( false );
	}

	pLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s %s", Parameters("FEATURE")->asGrid()->Get_Name(), _TL("Edges")));

	pLines->Add_Field("ID"   , SG_DATATYPE_Int);
	pLines->Add_Field("ANGLE", SG_DATATYPE_Double);

	for(int y=0; y<Gradient[0].Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Gradient[0].Get_NX(); x++)
		{
			if( !pEdge->is_NoData(x, y) )
			{
				for(int i=0; i<4; i+=2)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( pEdge->is_InGrid(ix, iy) )
					{
						double	diff	= SG_Get_Angle_Diff(Gradient[1].asDouble(x, y), Gradient[1].asDouble(ix, iy));

						if( diff <= m_maxAngle )
						{
							CSG_Shape	*pLine	= pLines->Add_Shape();

							pLine->Add_Point(Gradient[0].Get_System().Get_Grid_to_World( x,  y));
							pLine->Add_Point(Gradient[0].Get_System().Get_Grid_to_World(ix, iy));
							pLine->Set_Value(0, pLines->Get_Count());
							pLine->Set_Value(1, diff * M_RAD_TO_DEG);
						}
					}
				}
			}
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
CWombling::CWombling(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Wombling (Edge Detection)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Continuous Wombling for edge detection. Uses magnitude of gradient "
		"to detect edges between adjacent cells. Edge segments connect such "
		"edges, when the difference of their gradient directions is below given threshold.\n"
		"\n"
		"References:\n"
		"- Fitzpatrick, M.C., Preisser, E.L., Porter, A., Elkinton, J., Waller, L.A., Carlin, B.P., Ellison, A.M. (2010):"
		" Ecological boundary detection using Bayesian areal wombling. Ecology 91(12): 3448-3455. doi:10.1890/10-0807.1\n"
		"- Fortin, M.-J. and Dale, M.R.T (2005):"
		" Spatial Analysis - A Guide for Ecologists. Cambridge University Press.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "FEATURE"			, _TL("Feature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "EDGE_POINTS"		, _TL("Edge Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "EDGE_LINES"		, _TL("Edge Segments"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Value(
		NULL	, "GRADIENTS_OUT"	, _TL("Output of Gradients"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid_List(
		NULL	, "GRADIENTS"		, _TL("Gradients"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	Gradient[2], Edges;

	if( !Initialize(Gradient, &Edges) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pFeature	= Parameters("FEATURE")->asGrid();

	Edges.Set_Name(CSG_String::Format("%s [%s]", pFeature->Get_Name(), _TL("Edges")));

	Get_Gradient   (Gradient, pFeature, false);

	Get_Edge_Cells (Gradient, &Edges);
	Get_Edge_Lines (Gradient, &Edges);

	//-----------------------------------------------------
	if( Parameters("GRADIENTS_OUT")->asBool() )
	{
		CSG_Parameter_Grid_List	*pGrids	= Parameters("GRADIENTS")->asGridList();

		if( pGrids->asGrid(0) && pGrids->asGrid(0)->Get_System().is_Equal(Gradient[0].Get_System())
		&&  pGrids->asGrid(1) && pGrids->asGrid(1)->Get_System().is_Equal(Gradient[1].Get_System()) )
		{
			pGrids->asGrid(0)->Assign(&Gradient[0]);
			pGrids->asGrid(1)->Assign(&Gradient[1]);
		}
		else
		{
			pGrids->Del_Items();

			pGrids->Add_Item(SG_Create_Grid(Gradient[0]));
			pGrids->Add_Item(SG_Create_Grid(Gradient[1]));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWombling_MultiFeature::CWombling_MultiFeature(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Wombling for Multiple Features (Edge Detection)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Continuous Wombling for edge detection. Uses magnitude of gradient "
		"to detect edges between adjacent cells. Edge segments connect such "
		"edges, when the difference of their gradient directions is below given threshold.\n"
		"\n"
		"References:\n"
		"- Fitzpatrick, M.C., Preisser, E.L., Porter, A., Elkinton, J., Waller, L.A., Carlin, B.P., Ellison, A.M. (2010):"
		" Ecological boundary detection using Bayesian areal wombling. Ecology 91(12): 3448-3455. doi:10.1890/10-0807.1\n"
		"- Fortin, M.-J. and Dale, M.R.T (2005):"
		" Spatial Analysis - A Guide for Ecologists. Cambridge University Press.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "FEATURES"		, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "EDGE_CELLS"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//Parameters.Add_Choice(
	//	NULL	, "ASPECT_CMP"		, _TL("Direction Difference"),
	//	_TL(""),
	//	CSG_String::Format("%s|%s|",
	//		_TL("direction"),
	//		_TL("orientation")
	//	), 0
	//);

	Parameters.Add_Choice(
		NULL	, "OUTPUT_ADD"		, _TL("Additional Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("none"),
			_TL("gradients"),
			_TL("edge cells")
		), 0
	);

	Parameters.Add_Grid_List(
		NULL	, "OUTPUT"			, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_Value(
		NULL	, "ZERO_AS_NODATA"	, _TL("Zero as No-Data"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling_MultiFeature::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pFeatures	= Parameters("FEATURES")->asGridList(), *pOutput = NULL;

	//-----------------------------------------------------
	CSG_Grid	Gradient[2], Edges;

	if( !Initialize(Gradient, &Edges) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("OUTPUT_ADD")->asInt() )
	{
		pOutput	= Parameters("OUTPUT")->asGridList();

		for(int i=pOutput->Get_Count()-1; i>=0; i--)
		{
			if( !pOutput->asGrid(i)->Get_System().is_Equal(Gradient[0].Get_System()) )
			{
				pOutput->Del_Item(i);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Grid	Count, *pCount	= Parameters("EDGE_CELLS")->asGridList()->asGrid(0);

	if( !pCount || !pCount->Get_System().is_Equal(Gradient[0].Get_System()) )
	{
		Parameters("EDGE_CELLS")->asGridList()->Del_Items();
		Parameters("EDGE_CELLS")->asGridList()->Add_Item(pCount = SG_Create_Grid(Gradient[0].Get_System(), SG_DATATYPE_Char));
	}

	pCount->Set_Name(_TL("Edge Cells"));
	pCount->Assign(0.0);
	pCount->Set_NoData_Value(-1.0);

	//-----------------------------------------------------
	for(int i=0; i<pFeatures->Get_Count() && Process_Get_Okay(); i++)
	{
		Edges.Set_Name(CSG_String::Format("%s [%s]", pFeatures->asGrid(i)->Get_Name(), _TL("Edges")));

		Get_Gradient(Gradient, pFeatures->asGrid(i), false);

		Get_Edge_Cells(Gradient, &Edges);

		pCount->Add(Edges);

		if( pOutput )
		{
			if( !pOutput->asGrid(i) )
			{
				pOutput->Add_Item(SG_Create_Grid());
			}

			pOutput->asGrid(i)->Create(Parameters("OUTPUT_ADD")->asInt() == 1 ? Gradient[0] : Edges);
		}
	}

	if( Parameters("ZERO_AS_NODATA")->asBool() )
	{
		pCount->Set_NoData_Value(0.0);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
