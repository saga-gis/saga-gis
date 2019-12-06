
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    pj_georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             georef_with_coordinate_grids.cpp          //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "georef_with_coordinate_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoRef_with_Coordinate_Grids::CGeoRef_with_Coordinate_Grids(void)
{
	Set_Name		(_TL("Georeference with Coordinate Grids"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Georeferencing grids of grids two coordinate grids (x/y) that "
		"provide for each grid cell the targeted coordinate. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID_X"	, _TL("X m_Coords"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"GRID_Y"	, _TL("Y m_Coords"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"OUTPUT"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("RESAMPLING",
		"BYTEWISE"	, _TL("Bytewise Interpolation"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL(""),
		false
	);

	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoRef_with_Coordinate_Grids::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID_X")
	||  pParameter->Cmp_Identifier("GRID_Y") )
	{
		CSG_Grid	*pX	= (*pParameters)("GRID_X")->asGrid();
		CSG_Grid	*pY	= (*pParameters)("GRID_Y")->asGrid();

		if( pX && pY )
		{
			m_Grid_Target.Set_User_Defined(pParameters, CSG_Rect(pX->Get_Min(), pY->Get_Min(), pX->Get_Max(), pY->Get_Max()), pX->Get_NY());
		}
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGeoRef_with_Coordinate_Grids::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("RESAMPLING") )
	{
		pParameters->Set_Enabled("BYTEWISE" , pParameter->asInt() != 0);
	}

	if( pParameter->Cmp_Identifier("BYTEWISE"  ) )
	{
		pParameters->Set_Enabled("KEEP_TYPE", pParameter->asBool() == false);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoRef_with_Coordinate_Grids::On_Execute(void)
{
	CSG_Grid	Coords[2];

	if( !Get_Coordinates(Coords) )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	case  0: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	default: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	bool	bByteWise	= Parameters("BYTEWISE")->asBool();

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids[2];

	pGrids[0]	= Parameters("GRIDS" )->asGridList();
	pGrids[1]	= Parameters("OUTPUT")->asGridList();

	for(int i=0; i<pGrids[0]->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		bool	bKeepType	= bByteWise || Parameters("KEEP_TYPE")->asBool();

		CSG_Data_Object	*pOutput, *pObject = pGrids[0]->Get_Item(i);

		switch( pObject->Get_ObjectType() )
		{
		default:
			{
				CSG_Grid	*pGrid	= (CSG_Grid  *)pObject;

				pOutput	= SG_Create_Grid (Coords[0].Get_System(),
					bKeepType ? pGrid->Get_Type() : SG_DATATYPE_Undefined
				);

				if( bKeepType )
				{
					((CSG_Grid  *)pOutput)->Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());
					pOutput->Set_NoData_Value_Range(pObject->Get_NoData_Value(), pObject->Get_NoData_hiValue());
				}
			}
			break;

			case SG_DATAOBJECT_TYPE_Grids:
			{
				CSG_Grids	*pGrids	= (CSG_Grids *)pObject;

				pOutput	= SG_Create_Grids(Coords[0].Get_System(), pGrids->Get_Attributes(), pGrids->Get_Z_Attribute(),
					bKeepType ? pGrids->Get_Type() : SG_DATATYPE_Undefined, true
				);

				if( bKeepType )
				{
					((CSG_Grids *)pOutput)->Set_Scaling(pGrids->Get_Scaling(), pGrids->Get_Offset());
					pOutput->Set_NoData_Value_Range(pObject->Get_NoData_Value(), pObject->Get_NoData_hiValue());
				}
			}
			break;
		}

		if( !pOutput )
		{
			Error_Set(_TL("failed to allocate memory"));

			return( false );
		}

		pOutput->Set_Name             (pObject->Get_Name       ());
		pOutput->Set_Description      (pObject->Get_Description());
		pOutput->Get_MetaData().Assign(pObject->Get_MetaData   ());

		pGrids[1]->Add_Item(pOutput);
	}

	//-----------------------------------------------------
	for(int y=0; y<Coords[0].Get_NY() && Set_Progress(y, Coords[0].Get_NY()); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Coords[0].Get_NX(); x++)
		{
			if( Coords[0].is_NoData(x, y) || Coords[0].is_NoData(x, y) )
			{
				for(int i=0; i<pGrids[0]->Get_Grid_Count(); i++)
				{
					pGrids[1]->Get_Grid(i)->Set_NoData(x, y);
				}
			}
			else
			{
				double	cx, cy, z;

				cx	= Get_XMin() + Get_Cellsize() * Coords[0].asDouble(x, y);
				cy	= Get_XMin() + Get_Cellsize() * Coords[1].asDouble(x, y);

				for(int i=0; i<pGrids[0]->Get_Grid_Count(); i++)
				{
					if( pGrids[0]->Get_Grid(i)->Get_Value(cx, cy, z, Resampling, false, bByteWise) )
					{
						pGrids[1]->Get_Grid(i)->Set_Value(x, y, z);
					}
					else
					{
						pGrids[1]->Get_Grid(i)->Set_NoData(x, y);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids[0]->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		DataObject_Add(pGrids[1]->Get_Item(i));
		DataObject_Set_Parameters(pGrids[1]->Get_Item(i), pGrids[0]->Get_Item(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoRef_with_Coordinate_Grids::Get_Coordinates(CSG_Grid Coords[2])
{
	CSG_Grid_System	System	= m_Grid_Target.Get_System();

	Coords[0].Create(System); Coords[0].Assign_NoData();
	Coords[1].Create(System); Coords[1].Assign_NoData();

	CSG_Grid	*pX	= Parameters("GRID_X")->asGrid();
	CSG_Grid	*pY	= Parameters("GRID_Y")->asGrid();

	//-----------------------------------------------------
	for(int y=1; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=1; x<Get_NX(); x++)
		{
			if( !pX->is_NoData(x, y) && !pX->is_NoData(x - 1, y - 1)
			&&  !pY->is_NoData(x, y) && !pY->is_NoData(x - 1, y - 1) )
			{
				TSG_Point_Z	p[3];

				#define SET_POINT(P, X, Y, C) {\
					P.x = (pX->asDouble(X, Y) - Coords[i].Get_XMin()) / Coords[i].Get_Cellsize();\
					P.y = (pY->asDouble(X, Y) - Coords[i].Get_YMin()) / Coords[i].Get_Cellsize();\
					P.z = C == 0 ? X : Y;\
				}

				if( !pX->is_NoData(x - 1, y    )
				&&  !pY->is_NoData(x - 1, y    ) )
				{
					for(int i=0; i<2; i++)
					{
						SET_POINT(p[0], x - 1, y - 1, i);
						SET_POINT(p[1], x - 1, y    , i);
						SET_POINT(p[2], x    , y    , i);

						Set_Triangle(Coords[i], p);
					}
				}

				if( !pX->is_NoData(x    , y - 1)
				&&  !pY->is_NoData(x    , y - 1) )
				{
					for(int i=0; i<2; i++)
					{
						SET_POINT(p[0], x - 1, y - 1, i);
						SET_POINT(p[1], x    , y - 1, i);
						SET_POINT(p[2], x    , y    , i);

						Set_Triangle(Coords[i], p);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGeoRef_with_Coordinate_Grids::Set_Triangle(CSG_Grid &Coords, TSG_Point_Z p[3])
{
	//-----------------------------------------------------
	#define SWAP_POINTS(a, b) { TSG_Point_Z pp = p[a]; p[a] = p[b]; p[b] = pp; }

	if( p[1].y < p[0].y ) SWAP_POINTS(1, 0);
	if( p[2].y < p[0].y ) SWAP_POINTS(2, 0);
	if( p[2].y < p[1].y ) SWAP_POINTS(2, 1);

	//-----------------------------------------------------
	TSG_Rect	r;

	r.yMin	= p[0].y;
	r.yMax	= p[2].y;
	r.xMin	= p[0].x < p[1].x ? (p[0].x < p[2].x ? p[0].x : p[2].x) : (p[1].x < p[2].x ? p[1].x : p[2].x);
	r.xMax	= p[0].x > p[1].x ? (p[0].x > p[2].x ? p[0].x : p[2].x) : (p[1].x > p[2].x ? p[1].x : p[2].x);

	if( r.yMin >= r.yMax || r.xMin >= r.xMax )
	{
		return;	// no area
	}

	if( (r.yMin < 0. && r.yMax < 0.) || (r.yMin >= Coords.Get_NY() && r.yMax >= Coords.Get_NY())
	||	(r.xMin < 0. && r.xMax < 0.) || (r.xMin >= Coords.Get_NX() && r.xMax >= Coords.Get_NX()) )
	{
		return;	// completely outside grid
	}

	//-----------------------------------------------------
	TSG_Point_Z	d[3];

	if( (d[0].y	= p[2].y - p[0].y) != 0. )
	{
		d[0].x	= (p[2].x - p[0].x) / d[0].y;
		d[0].z	= (p[2].z - p[0].z) / d[0].y;
	}

	if( (d[1].y	= p[1].y - p[0].y) != 0. )
	{
		d[1].x	= (p[1].x - p[0].x) / d[1].y;
		d[1].z	= (p[1].z - p[0].z) / d[1].y;
	}

	if( (d[2].y	= p[2].y - p[1].y) != 0. )
	{
		d[2].x	= (p[2].x - p[1].x) / d[2].y;
		d[2].z	= (p[2].z - p[1].z) / d[2].y;
	}

	//-----------------------------------------------------
	int	ay	= (int)r.yMin;	if( ay < 0 )	ay	= 0;	if( ay < r.yMin )	ay++;
	int	by	= (int)r.yMax;	if( by >= Coords.Get_NY() )	by	= Coords.Get_NY() - 1;

	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1].y && d[1].y > 0. )
		{
			Set_Triangle_Line(Coords, y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[0].x + (y - p[0].y) * d[1].x,
				p[0].z + (y - p[0].y) * d[1].z
			);
		}
		else if( d[2].y > 0. )
		{
			Set_Triangle_Line(Coords, y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[1].x + (y - p[1].y) * d[2].x,
				p[1].z + (y - p[1].y) * d[2].z
			);
		}
	}
}

//---------------------------------------------------------
inline void CGeoRef_with_Coordinate_Grids::Set_Triangle_Line(CSG_Grid &Coords, int y, double xa, double za, double xb, double zb)
{
	if( xb < xa )
	{
		double	d;

		d	= xa;	xa	= xb;	xb	= d;
		d	= za;	za	= zb;	zb	= d;
	}

	if( xb > xa )
	{
		double	dz	= (zb - za) / (xb - xa);
		int		ax	= (int)xa;	if( ax < 0 )	ax	= 0;	if( ax < xa )	ax++;
		int		bx	= (int)xb;	if( bx >= Coords.Get_NX() )	bx	= Coords.Get_NX() - 1;

		for(int x=ax; x<=bx; x++)
		{
			double	z	= za + dz * (x - xa);

		//	if( Coords.is_NoData(x, y) )
			{
				Coords.Set_Value(x, y, z);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
