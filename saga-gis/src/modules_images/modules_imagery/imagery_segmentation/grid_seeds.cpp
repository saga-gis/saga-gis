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

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SURFACE"			, _TL("Surface"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SEEDS_GRID"		, _TL("Seeds Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "SEEDS"			, _TL("Seeds"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "FACTOR"			, _TL("Bandwidth (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Double, 2.0, 1.0, true
	);

	Parameters.Add_Choice(
		NULL	, "TYPE_SURFACE"	, _TL("Type of Surface"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("smoothed surface"),
			_TL("variance (a)"),
			_TL("variance (b)")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "TYPE_SEEDS"		, _TL("Extraction of..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("minima"),
			_TL("maxima"),
			_TL("minima and maxima")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "TYPE_MERGE"		, _TL("Feature Aggregation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("additive"),
			_TL("multiplicative")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "NORMALIZE"		, _TL("Normalized"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	//-----------------------------------------------------
	m_Direction.Set_Count(8);

	for(int i=0; i<8; i++)
	{
		m_Direction[i].z	= (M_PI_360 * i) / 8.0;
		m_Direction[i].x	= sin(m_Direction[i].z);
		m_Direction[i].y	= cos(m_Direction[i].z);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::On_Execute(void)
{
	bool		bNormalize;
	int			Merge;
	double		Cellsize;
	CSG_Grid	*pSurface, Surface, *pSeeds_Grid;
	CSG_Shapes	*pSeeds;

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")		->asGridList();
	pSurface	= Parameters("SURFACE")		->asGrid();
	pSeeds_Grid	= Parameters("SEEDS_GRID")	->asGrid();
	pSeeds		= Parameters("SEEDS")		->asShapes();

	m_Method	= Parameters("TYPE_SURFACE")->asInt();
	Cellsize	= Parameters("FACTOR")		->asDouble() * Get_Cellsize();
	Merge		= Parameters("TYPE_MERGE")	->asInt();
	bNormalize	= Parameters("NORMALIZE")	->asBool();

	//-----------------------------------------------------
	m_Smooth.Create(SG_DATATYPE_Float,
		4 + (int)(Get_System()->Get_XRange() / Cellsize),
		4 + (int)(Get_System()->Get_YRange() / Cellsize),
		Cellsize,
		Get_XMin() - Cellsize,
		Get_YMin() - Cellsize
	);

	if( !m_Smooth.is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( m_pGrids->Get_Count() > 1 )
	{
		Surface.Create(*Get_System(), pSurface->Get_Type());
	}

	for(int i=0; i<m_pGrids->Get_Count(); i++)
	{
		m_Smooth.Assign(m_pGrids->asGrid(i), GRID_INTERPOLATION_Mean_Cells);

		if( i == 0 )
		{
			Get_Surface(m_pGrids->asGrid(i), pSurface, bNormalize);
		}
		else
		{
			Get_Surface(m_pGrids->asGrid(i), &Surface, bNormalize);

			Add_Surface(pSurface, &Surface, Merge);
		}
	}

	Get_Seeds(pSurface, pSeeds, pSeeds_Grid, Parameters("TYPE_SEEDS")->asInt());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::Get_Surface(CSG_Grid *pFeature, CSG_Grid *pSurface, bool bNormalize)
{
	int			x, y;
	double		z, Radius;
	TSG_Point	p;

	Radius	= m_Method == 2 ? Get_Cellsize() : m_Smooth.Get_Cellsize();

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( pFeature->is_InGrid(x, y) && m_Smooth.Get_Value(p, z) )
			{
				switch( m_Method )
				{
				case 0:
					{
						pSurface->Set_Value(x, y, z);
					}
					break;

				case 1: case 2:
					{
						CSG_Simple_Statistics	s;

						s.Add_Value(z);

						for(int i=0; i<8; i++)
						{
							TSG_Point	q;

							q.x	= p.x + Radius * m_Direction[i].x;
							q.y	= p.y + Radius * m_Direction[i].y;

							if( m_Smooth.Get_Value(q, z) )
							{
								s.Add_Value(z);
							}
						}

						if( s.Get_Count() > 0 )
						{
							pSurface->Set_Value(x, y, s.Get_StdDev());
						}
						else
						{
							pSurface->Set_NoData(x, y);
						}
					}
					break;
				}
			}
			else
			{
				pSurface->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( bNormalize && pSurface->Get_StdDev() > 0.0 )
	{
		pSurface->Multiply(1.0 / pSurface->Get_StdDev());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::Add_Surface(CSG_Grid *pSurface, CSG_Grid *pAdd, int Method)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pSurface->is_NoData(x, y) )
			{
				if( pAdd->is_NoData(x, y) )
				{
					pSurface->Set_NoData(x, y);
				}
				else switch( Method )
				{
				case 0:
					pSurface->Add_Value(x, y, pAdd->asDouble(x, y));
					break;

				case 1:
					pSurface->Mul_Value(x, y, pAdd->asDouble(x, y));
					break;
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Seeds::Get_Seeds(CSG_Grid *pSurface, CSG_Shapes *pSeeds, CSG_Grid *pGrid, int Method)
{
	bool		bMinimum, bMaximum;
	int			x, y, i, ix, iy, id;
	double		z;
	TSG_Point	p;

	//-----------------------------------------------------
	pSeeds->Create(SHAPE_TYPE_Point, _TL("Seeds"));
	pSeeds->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
	pSeeds->Add_Field(SG_T("X") , SG_DATATYPE_Int);
	pSeeds->Add_Field(SG_T("Y") , SG_DATATYPE_Int);
	pSeeds->Add_Field(SG_T("S") , SG_DATATYPE_Double);

	for(i=0; i<m_pGrids->Get_Count(); i++)
	{
		pSeeds->Add_Field(m_pGrids->asGrid(i)->Get_Name(), SG_DATATYPE_Double);
	}

	if( pGrid )
	{
		pGrid->Assign_NoData();
	}

	//-----------------------------------------------------
	for(id=0, y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( !pSurface->is_NoData(x, y) )
			{
				for(i=0, z=pSurface->asDouble(x, y), bMinimum=true, bMaximum=true; (bMinimum || bMaximum) && i<8; i++)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if( !pSurface->is_InGrid(ix, iy) )
					{
						bMinimum	= bMaximum	= false;
					}
					else if( z > pSurface->asDouble(ix, iy) )
					{
						bMinimum	= false;
					}
					else if( z < pSurface->asDouble(ix, iy) )
					{
						bMaximum	= false;
					}
				}

				//-----------------------------------------
				if( (Method == 0 && bMinimum)
				||	(Method == 1 && bMaximum)
				||	(Method == 2 && (bMinimum || bMaximum)) )
				{
					CSG_Shape	*pSeed	= pSeeds->Add_Shape();

					id++;

					pSeed->Add_Point(p);
					pSeed->Set_Value(0, id);
					pSeed->Set_Value(1, x);
					pSeed->Set_Value(2, y);
					pSeed->Set_Value(3, z);

					for(i=0; i<m_pGrids->Get_Count(); i++)
					{
						pSeed->Set_Value(3 + i, m_pGrids->asGrid(i)->asDouble(x, y));
					}

					if( pGrid )
					{
						pGrid->Set_Value(x, y, id);
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
