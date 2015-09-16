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
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "EDGE_POINTS"	, _TL("Edge Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "EDGE_LINES"	, _TL("Edge Segments"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Value(
		NULL	, "TMAGNITUDE"	, _TL("Minimum Magnitude"),
		_TL("Minimum magnitude as percentile."),
		PARAMETER_TYPE_Double, 90.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "TDIRECTION"	, _TL("Maximum Angle"),
		_TL("Maximum angular difference as degree between adjacent segment points."),
		PARAMETER_TYPE_Double, 10.0, 0.0, true, 180.0, true
	);

//	Parameters.Add_Choice(
//		NULL	, "NEIGHBOUR"	, _TL("Segment Connection"),
//		_TL(""),
//		CSG_String::Format(SG_T("%s|%s|"),
//			_TL("Rooke"),
//			_TL("Queen")
//		), 1
//	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWombling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid_System	System(Get_Cellsize(),
		Get_XMin() + 0.5 * Get_Cellsize(),
		Get_YMin() + 0.5 * Get_Cellsize(),
		Get_NX() - 1, Get_NY() - 1
	);

	CSG_Grid	Slope(System), Aspect(System);

	Aspect.Set_NoData_Value(-1.0);

	//-----------------------------------------------------
	Get_Gradient   (&Slope, &Aspect);

	Get_Edge_Points(&Slope, &Aspect);

	Get_Edge_Lines (&Slope, &Aspect);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling::Get_Edge_Lines(CSG_Grid *pSlope, CSG_Grid *pAspect)
{
//	int		Neighbour	= Parameters("NEIGHBOUR" )->asInt();
	double	Threshold	= Parameters("TDIRECTION")->asDouble() * M_DEG_TO_RAD;

	CSG_Shapes	*pLines	= Parameters("EDGE_LINES")->asShapes();

	pLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s %s", Parameters("GRID")->asGrid()->Get_Name(), _TL("Edges")));
	pLines->Add_Field("ID"   , SG_DATATYPE_Int);
	pLines->Add_Field("ANGLE", SG_DATATYPE_Double);

	for(int y=0; y<Get_NY()-2 && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX()-2; x++)
		{
			Get_Edge_Lines(x, y, x + 1, y    , pSlope, pAspect, pLines, Threshold);
			Get_Edge_Lines(x, y, x    , y + 1, pSlope, pAspect, pLines, Threshold);
		}
	}

	return( true );
}

//---------------------------------------------------------
inline void CWombling::Get_Edge_Lines(int ix, int iy, int jx, int jy, CSG_Grid *pSlope, CSG_Grid *pAspect, CSG_Shapes *pLines, double Threshold)
{
	if( !pSlope->is_NoData(ix, iy) && !pAspect->is_NoData(ix, iy)
	&&  !pSlope->is_NoData(jx, jy) && !pAspect->is_NoData(jx, jy) )
	{
		double	i	= pAspect->asDouble(ix, iy);	if( i < 0.0 )	i	+= M_PI_360;	i	= fmod(i, M_PI_360);
		double	j	= pAspect->asDouble(jx, jy);	if( j < 0.0 )	j	+= M_PI_360;	j	= fmod(j, M_PI_360);

		double	diff	= fmod(fabs(i - j), M_PI_180);

		if( Threshold == 0.0 || diff <= Threshold )
		{
			CSG_Shape	*pLine	= pLines->Add_Shape();
			pLine->Add_Point(pSlope->Get_System().Get_Grid_to_World(ix, iy));
			pLine->Add_Point(pSlope->Get_System().Get_Grid_to_World(jx, jy));
			pLine->Set_Value(0, pLines->Get_Count());
			pLine->Set_Value(1, diff * M_RAD_TO_DEG);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling::Get_Edge_Points(CSG_Grid *pSlope, CSG_Grid *pAspect)
{
	double	Threshold	= pSlope->Get_Percentile(Parameters("TMAGNITUDE")->asDouble());

	CSG_Shapes	*pPoints	= Parameters("EDGE_POINTS")->asShapes();

	pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s %s", Parameters("GRID")->asGrid()->Get_Name(), _TL("Edges")));
	pPoints->Add_Field("ID"       , SG_DATATYPE_Int);
	pPoints->Add_Field("MAGNITUDE", SG_DATATYPE_Double);
	pPoints->Add_Field("DIRECTION", SG_DATATYPE_Double);

	for(int y=0; y<Get_NY()-1 && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX()-1; x++)
		{
			if( !pSlope->is_NoData(x, y) && pSlope->asDouble(x, y) >= Threshold )
			{
				CSG_Shape	*pPoint	= pPoints->Add_Shape();
				pPoint->Set_Point(pSlope->Get_System().Get_Grid_to_World(x, y), 0);
				pPoint->Set_Value(0, pPoints->Get_Count());
				pPoint->Set_Value(1, pSlope ->asDouble(x, y));
				pPoint->Set_Value(2, pAspect->asDouble(x, y) * M_RAD_TO_DEG);
			}
			else
			{
				pSlope->Set_NoData(x, y);
			}
		}
	}

	return( pPoints->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWombling::Get_Gradient(CSG_Grid *pSlope, CSG_Grid *pAspect)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	for(int y=0; y<Get_NY()-1 && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX()-1; x++)
		{
			if( pGrid->is_NoData(x    , y    )
			||  pGrid->is_NoData(x + 1, y    )
			||  pGrid->is_NoData(x    , y + 1)
			||  pGrid->is_NoData(x + 1, y + 1) )
			{
				pSlope ->Set_NoData(x, y);
				pAspect->Set_NoData(x, y);
			}
			else
			{
				double	Slope, Aspect, z[4];

				z[0]	= pGrid->asDouble(x    , y    );
				z[1]	= pGrid->asDouble(x + 1, y    );
				z[2]	= pGrid->asDouble(x    , y + 1);
				z[3]	= pGrid->asDouble(x + 1, y + 1);

				Get_Gradient(z, Slope, Aspect);

				pSlope ->Set_Value(x, y, Slope );
				pAspect->Set_Value(x, y, Aspect);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
inline void CWombling::Get_Gradient(double z[4], double &Slope, double &Aspect)
{
	double	a	= 0.5 * Get_Cellsize() * (-z[2] + z[3] + z[0] - z[1]);
	double	b	= 0.5 * Get_Cellsize() * ( z[2] + z[3] - z[0] - z[1]);

	Slope	= sqrt(a*a + b*b);
	Aspect	= a != 0.0 ? M_PI_180 + atan2(b, a)
		: b >  0.0 ? M_PI_270
		: b <  0.0 ? M_PI_090 : -1;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
