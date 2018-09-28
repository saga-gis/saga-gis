/**********************************************************
 * Version $Id: grid_seeds.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    grid_seeds.cpp                     //
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
#include "grid_seeds.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Seeds::CGrid_Seeds(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Seed Generation"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		"", "FEATURES"		, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "VARIANCE"		, _TL("Variance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "SEED_GRID"		, _TL("Seeds Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		"", "SEED_POINTS"	, _TL("Seed Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		"", "SEED_TYPE"		, _TL("Seed Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("minima of variance"),
			_TL("maxima of variance")
		), 0
	);

	Parameters.Add_Choice(
		"", "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("band width smoothing"),
			_TL("band width search")
		), 0
	);

	Parameters.Add_Double(
		"", "BAND_WIDTH"	, _TL("Bandwidth (Cells)"),
		_TL(""),
		10.0, 1.0, true
	);

	Parameters.Add_Bool(
		"", "NORMALIZE"		, _TL("Normalize Features"),
		_TL(""),
		false
	);

	m_Cells.Get_Weighting().Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Cells.Get_Weighting().Set_BandWidth(5.0);
	m_Cells.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Seeds::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("DISTANCE_WEIGHTING", pParameter->asInt() == 1);
	}

	return( m_Cells.Get_Weighting().Enable_Parameters(pParameters) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pFeatures	= Parameters("FEATURES")->asGridList();

	if( (m_nFeatures = pFeatures->Get_Grid_Count()) <= 0 )
	{
		Error_Set(_TL("no features in input list") );

		return( false );
	}

	m_pFeatures	= (CSG_Grid **)SG_Calloc(m_nFeatures, sizeof(CSG_Grid *));

	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	if( Method == 0 )	// resampling
	{
		double	Cellsize	= Parameters("BAND_WIDTH")->asDouble() * Get_Cellsize();

		CSG_Grid	Smoothed(SG_DATATYPE_Float,
			4 + (int)(Get_System()->Get_XRange() / Cellsize),
			4 + (int)(Get_System()->Get_YRange() / Cellsize),
			Cellsize,
			Get_XMin() - Cellsize,
			Get_YMin() - Cellsize
		);

		for(int i=0; i<m_nFeatures; i++)
		{
			Process_Set_Text("%s: %s", _TL("resampling"), pFeatures->Get_Grid(i)->Get_Name());

			SG_UI_Progress_Lock(true);

			Smoothed.Assign(pFeatures->Get_Grid(i), GRID_RESAMPLING_Mean_Cells);

			m_pFeatures[i]	= new CSG_Grid(*Get_System(), SG_DATATYPE_Float);
			m_pFeatures[i]	->Assign(&Smoothed, GRID_RESAMPLING_BSpline);
			m_pFeatures[i]	->Set_Name(pFeatures->Get_Grid(i)->Get_Name());

			SG_UI_Progress_Lock(false);
		}
	}
	else	// search radius
	{
		m_Cells.Get_Weighting().Set_Parameters(&Parameters);
		m_Cells.Set_Radius(Parameters("BAND_WIDTH")->asInt());

		for(int i=0; i<m_nFeatures; i++)
		{
			m_pFeatures[i]	= pFeatures->Get_Grid(i);
		}
	}

	//-----------------------------------------------------
	if( (m_bNormalize = Parameters("NORMALIZE")->asBool()) == true )
	{
		m_Norm.Create(m_nFeatures, 2);

		for(int i=0; i<m_nFeatures; i++)
		{
			m_Norm[0][i]	= pFeatures->Get_Grid(i)->Get_Mean  ();
			m_Norm[1][i]	= pFeatures->Get_Grid(i)->Get_StdDev();	if( m_Norm[1][i] == 0.0 )	m_Norm[1][i]	= 1.0;
		}
	}

	//-----------------------------------------------------
	int	x, y;

	m_pVariance		= Parameters("VARIANCE")->asGrid();
	m_pVariance->Set_NoData_Value(-1.0);

	Process_Set_Text(_TL("masking no data"));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(int i=0; !bNoData && i<m_nFeatures; i++)
			{
				bNoData	= m_pFeatures[i]->is_NoData(x, y);
			}

			m_pVariance->Set_Value(x, y, bNoData ? -1.0 : 0.0);
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("calculating variance"));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			if( !m_pVariance->is_NoData(x, y) )
			{
				if( Method == 0 )
				{
					Get_Resampled(x, y);
				}
				else
				{
					Get_Radius(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	Get_Seeds();

	//-----------------------------------------------------
	for(int i=0; Method==0 && i<m_nFeatures; i++)
	{
		delete(m_pFeatures[i]);
	}

	SG_Free(m_pFeatures);

	m_Norm.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGrid_Seeds::Get_Feature(int i, int x, int y)
{
	double	z	= m_pFeatures[i]->asDouble(x, y);

	if( m_bNormalize )
	{
		return( (z - m_Norm[0][i]) / m_Norm[1][i] );
	}

	return( z );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::Get_Resampled(int x, int y)
{
	int			ix, iy, n, iFeature;
	CSG_Vector	Centroid(m_nFeatures);

	//-----------------------------------------------------
	for(n=0, iy=y-1; iy<=y+1; iy++)
	{
		for(ix=x-1; ix<=x+1; ix++)
		{
			if( m_pVariance->is_InGrid(ix, iy) )
			{
				for(n++, iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					Centroid[iFeature]	= Get_Feature(iFeature, ix, iy);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( n > 0 )
	{
		CSG_Simple_Statistics	s;

		Centroid	*= 1.0 / n;

		for(iy=y-1; iy<=y+1; iy++)
		{
			for(ix=x-1; ix<=x+1; ix++)
			{
				if( m_pVariance->is_InGrid(ix, iy) )
				{
					double	Distance	= 0.0;

					for(iFeature=0, Distance=0.0; iFeature<m_nFeatures; iFeature++)
					{
						Distance	+= SG_Get_Square(Centroid[iFeature] - Get_Feature(iFeature, ix, iy));
					}

					s	+= sqrt(Distance);
				}
			}
		}

		//-------------------------------------------------
		if( s.Get_Count() > 0 )
		{
			m_pVariance->Set_Value(x, y, s.Get_Variance());

			return( true );
		}
	}

	m_pVariance->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::Get_Radius(int x, int y)
{
	int			iFeature, iCell, ix, iy;
	double		iDistance, iWeight, Weights, Distance;
	CSG_Vector	Centroid(m_nFeatures);

	//-----------------------------------------------------
	for(iCell=0, Weights=0.0; iCell<m_Cells.Get_Count(); iCell++)
	{
		if( m_Cells.Get_Values(iCell, ix = x, iy = y, iDistance, iWeight, true) && m_pVariance->is_InGrid(ix, iy) )
		{
			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				Centroid[iFeature]	+= iWeight * Get_Feature(iFeature, ix, iy);
			}

			Weights			+= iWeight;
		}
	}

	//-----------------------------------------------------
	if( Weights > 0.0 )
	{
		CSG_Simple_Statistics	s;

		Centroid	*= 1.0 / Weights;

		for(iCell=0; iCell<m_Cells.Get_Count(); iCell++)
		{
			if( m_Cells.Get_Values(iCell, ix = x, iy = y, iDistance, iWeight, true) && m_pVariance->is_InGrid(ix, iy) )
			{
				for(iFeature=0, Distance=0.0; iFeature<m_nFeatures; iFeature++)
				{
					Distance	+= SG_Get_Square(Centroid[iFeature] - Get_Feature(iFeature, ix, iy));
				}

				s.Add_Value(sqrt(Distance), iWeight);
			}
		}

		m_pVariance->Set_Value(x, y, s.Get_Variance());

		return( true );
	}

	//-----------------------------------------------------
	m_pVariance->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::Get_Seeds(void)
{
	int	Type	= Parameters("SEED_TYPE")->asInt();

	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("SEED_POINTS")->asShapes();

	if( pPoints )
	{
		pPoints->Create(SHAPE_TYPE_Point, _TL("Seeds"));

		pPoints->Add_Field("ID" , SG_DATATYPE_Int   );
		pPoints->Add_Field("X"  , SG_DATATYPE_Int   );
		pPoints->Add_Field("Y"  , SG_DATATYPE_Int   );
		pPoints->Add_Field("VAR", SG_DATATYPE_Double);

		for(int iFeature=0; iFeature<m_nFeatures; iFeature++)
		{
			pPoints->Add_Field(m_pFeatures[iFeature]->Get_Name(), SG_DATATYPE_Double);
		}
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("SEED_GRID")->asGrid();

	if( (pGrid = Parameters("SEED_GRID")->asGrid()) != NULL )
	{
		pGrid->Assign_NoData();
	}

	//-----------------------------------------------------
	for(int n=0, y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pVariance->is_NoData(x, y) )
			{
				bool	bExtreme	= true;

				double	z	= m_pVariance->asDouble(x, y);

				for(int i=0; bExtreme && i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( !m_pVariance->is_InGrid(ix, iy)
					||  (Type == 0 && z > m_pVariance->asDouble(ix, iy))   // minimum
					||  (Type == 1 && z < m_pVariance->asDouble(ix, iy)) ) // maximum
					{
						bExtreme	= false;
					}
				}

				//-----------------------------------------
				if( bExtreme )
				{
					n++;

					if( pPoints )
					{
						CSG_Shape	*pPoint	= pPoints->Add_Shape();

						pPoint->Add_Point(Get_System()->Get_Grid_to_World(x, y));

						pPoint->Set_Value(0, n);
						pPoint->Set_Value(1, x);
						pPoint->Set_Value(2, y);
						pPoint->Set_Value(3, z);

						for(int iFeature=0; iFeature<m_nFeatures; iFeature++)
						{
							pPoint->Set_Value(3 + iFeature, m_pFeatures[iFeature]->asDouble(x, y));
						}
					}

					if( pGrid )
					{
						pGrid->Set_Value(x, y, n);
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
