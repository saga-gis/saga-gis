/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            Grid_Statistics_AddTo_Polygon.cpp          //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//                 quantile calculation:                 //
//                 Copyright (C) 2007 by                 //
//                   Johan Van de Wauw                   //
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
#include "Grid_Statistics_AddTo_Polygon.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_AddTo_Polygon::CGrid_Statistics_AddTo_Polygon(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Statistics for Polygons"));

	Set_Author		("O.Conrad (c) 2003, Quantile Calculation (c) 2007 by Johan Van de Wauw");

	Set_Description	(_TW(
		"Zonal grid statistics. For each polygon statistics based on all covered grid cells will be calculated."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "NAMING"		, _TL("Field Naming"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("grid number"),
			_TL("grid name")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("simple and fast"),
			_TL("polygon wise (cell centers)"),
			_TL("polygon wise (cell area)"),
			_TL("polygon wise (cell area weighted)")
		), 0
	);

	Parameters.Add_Bool(
		NULL	, "PARALLELIZED"	, _TL("Use Multiple Cores"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(pNode, "COUNT"   , _TL("Number of Cells"   ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MIN"     , _TL("Minimum"           ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MAX"     , _TL("Maximum"           ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "RANGE"   , _TL("Range"             ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "SUM"     , _TL("Sum"               ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MEAN"    , _TL("Mean"              ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "VAR"     , _TL("Variance"          ), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "STDDEV"  , _TL("Standard Deviation"), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "QUANTILE", _TL("Quantile"          ), 
		_TL("Calculate distribution quantiles. Value specifies interval (median=50, quartiles=25, deciles=10, ...). Set to zero to omit quantile calculation."),
		PARAMETER_TYPE_Int, 0, 0, true, 50, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Statistics_AddTo_Polygon::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("PARALLELIZED", pParameter->asInt() != 0 && SG_OMP_Get_Max_Num_Threads() > 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_FIELD_NAME(VAR)	Naming == 0 ? CSG_String::Format("G%02d_%s", iGrid + 1, VAR) : CSG_String::Format("%s [%s]", pGrids->asGrid(iGrid)->Get_Name(), VAR)

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( pPolygons->Get_Count() <= 0 )
	{
		Error_Set(_TL("no polygons in input layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Count() < 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	if( !Get_System()->Get_Extent().Intersects(pPolygons->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid(s) and polygon layer"));

		return( false );
	}

	//-----------------------------------------------------
	int	Quantile	= Parameters("QUANTILE")->asInt();
	int	nFields		= 0;

	int	fCOUNT		= Parameters("COUNT"   )->asBool() ? nFields++ : -1;
	int	fMIN		= Parameters("MIN"     )->asBool() ? nFields++ : -1;
	int	fMAX		= Parameters("MAX"     )->asBool() ? nFields++ : -1;
	int	fRANGE		= Parameters("RANGE"   )->asBool() ? nFields++ : -1;
	int	fSUM		= Parameters("SUM"     )->asBool() ? nFields++ : -1;
	int	fMEAN		= Parameters("MEAN"    )->asBool() ? nFields++ : -1;
	int	fVAR		= Parameters("VAR"     )->asBool() ? nFields++ : -1;
	int	fSTDDEV		= Parameters("STDDEV"  )->asBool() ? nFields++ : -1;
	int	fQUANTILE	= Quantile > 0                     ? nFields++ : -1;

	if( nFields == 0 )
	{
		Error_Set(_TL("no output parameter in selection"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bParallelized	= Parameters("PARALLELIZED")->is_Enabled() && Parameters("PARALLELIZED")->asBool();

	int		Naming			= Parameters("NAMING")->asInt();

	int		Method			= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	CSG_Grid	Index;

	if( Method == 0 && !Get_Simple_Index(pPolygons, Index) )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asShapes() != NULL && Parameters("RESULT")->asShapes() != pPolygons )
	{
		pPolygons	= Parameters("RESULT")->asShapes();
		pPolygons	->Assign(Parameters("POLYGONS")->asShapes());
		pPolygons	->Set_Name(CSG_String::Format("%s [%s]", Parameters("POLYGONS")->asShapes()->Get_Name(), _TL("Grid Statistics")));
	}

	CSG_Simple_Statistics	*Statistics	= new CSG_Simple_Statistics[pPolygons->Get_Count()];

	//-----------------------------------------------------
	for(int iGrid=0; iGrid<pGrids->Get_Count() && Process_Get_Okay(); iGrid++)
	{
		Process_Set_Text(CSG_String::Format("[%d/%d] %s", 1 + iGrid, pGrids->Get_Count(), pGrids->asGrid(iGrid)->Get_Name()));

		if( (Method == 0 && Get_Simple (pGrids->asGrid(iGrid), pPolygons, Statistics, Quantile > 0, Index))
		||  (Method != 0 && Get_Precise(pGrids->asGrid(iGrid), pPolygons, Statistics, Quantile > 0, bParallelized)) )
		{
			nFields	= pPolygons->Get_Field_Count();

			if( fCOUNT    >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("CELLS"   )), SG_DATATYPE_Int   );
			if( fMIN      >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("MIN"     )), SG_DATATYPE_Double);
			if( fMAX      >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("MAX"     )), SG_DATATYPE_Double);
			if( fRANGE    >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("RANGE"   )), SG_DATATYPE_Double);
			if( fSUM      >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("SUM"     )), SG_DATATYPE_Double);
			if( fMEAN     >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("MEAN"    )), SG_DATATYPE_Double);
			if( fVAR      >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("VARIANCE")), SG_DATATYPE_Double);
			if( fSTDDEV   >= 0 )	pPolygons->Add_Field(GET_FIELD_NAME(_TL("STDDEV"  )), SG_DATATYPE_Double);
			if( fQUANTILE >= 0 )
			{
				for(int iQuantile=Quantile; iQuantile<100; iQuantile+=Quantile)
				{
					pPolygons->Add_Field(GET_FIELD_NAME(CSG_String::Format("Q%02d", iQuantile).c_str()), SG_DATATYPE_Double);
				}
			}

			//---------------------------------------------
			for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
			{
				CSG_Shape	*pPolygon	= pPolygons->Get_Shape(i);

				if( Statistics[i].Get_Count() == 0 )
				{
					if( fCOUNT    >= 0 )	pPolygon->Set_NoData(nFields + fCOUNT );
					if( fMIN      >= 0 )	pPolygon->Set_NoData(nFields + fMIN   );
					if( fMAX      >= 0 )	pPolygon->Set_NoData(nFields + fMAX   );
					if( fRANGE    >= 0 )	pPolygon->Set_NoData(nFields + fRANGE );
					if( fSUM      >= 0 )	pPolygon->Set_NoData(nFields + fSUM   );
					if( fMEAN     >= 0 )	pPolygon->Set_NoData(nFields + fMEAN  );
					if( fVAR      >= 0 )	pPolygon->Set_NoData(nFields + fVAR   );
					if( fSTDDEV   >= 0 )	pPolygon->Set_NoData(nFields + fSTDDEV);
					if( fQUANTILE >= 0 )
					{
						for(int iQuantile=Quantile, iField=nFields + fQUANTILE; iQuantile<100; iQuantile+=Quantile, iField++)
						{
							pPolygon->Set_NoData(iField);
						}
					}
				}
				else
				{
					if( fCOUNT    >= 0 )	pPolygon->Set_Value(nFields + fCOUNT , Statistics[i].Get_Count   ());
					if( fMIN      >= 0 )	pPolygon->Set_Value(nFields + fMIN   , Statistics[i].Get_Minimum ());
					if( fMAX      >= 0 )	pPolygon->Set_Value(nFields + fMAX   , Statistics[i].Get_Maximum ());
					if( fRANGE    >= 0 )	pPolygon->Set_Value(nFields + fRANGE , Statistics[i].Get_Range   ());
					if( fSUM      >= 0 )	pPolygon->Set_Value(nFields + fSUM   , Statistics[i].Get_Sum     ());
					if( fMEAN     >= 0 )	pPolygon->Set_Value(nFields + fMEAN  , Statistics[i].Get_Mean    ());
					if( fVAR      >= 0 )	pPolygon->Set_Value(nFields + fVAR   , Statistics[i].Get_Variance());
					if( fSTDDEV   >= 0 )	pPolygon->Set_Value(nFields + fSTDDEV, Statistics[i].Get_StdDev  ());
					if( fQUANTILE >= 0 )
					{
						for(int iQuantile=Quantile, iField=nFields + fQUANTILE; iQuantile<100; iQuantile+=Quantile, iField++)
						{
							pPolygon->Set_Value(iField, Statistics[i].Get_Quantile(iQuantile));
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	delete[](Statistics);

	DataObject_Update(pPolygons);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::Get_Precise(CSG_Grid *pGrid, CSG_Shapes *pPolygons, CSG_Simple_Statistics *Statistics, bool bQuantiles, bool bParallelized)
{
	int	Method	= Parameters("METHOD")->asInt();

	if( bParallelized )
	{
		#pragma omp parallel for
		for(int i=0; i<pPolygons->Get_Count(); i++)
		{
			Get_Precise(pGrid, (CSG_Shape_Polygon *)pPolygons->Get_Shape(i), Statistics[i], bQuantiles, Method);
		}
	}
	else
	{
		for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
		{
			Get_Precise(pGrid, (CSG_Shape_Polygon *)pPolygons->Get_Shape(i), Statistics[i], bQuantiles, Method);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::Get_Precise(CSG_Grid *pGrid, CSG_Shape_Polygon *pPolygon, CSG_Simple_Statistics &Statistics, bool bQuantiles, int Method)
{
	//-----------------------------------------------------
	CSG_Shapes	Intersect(SHAPE_TYPE_Polygon);

	CSG_Shape_Polygon	*pCell, *pArea;

	if( Method == 3 )	// polygon wise (cell area weighted)
	{
		pCell	= (CSG_Shape_Polygon *)Intersect.Add_Shape();
		pArea	= (CSG_Shape_Polygon *)Intersect.Add_Shape();
	}

	//-----------------------------------------------------
	Statistics.Create(bQuantiles);

	int	ax	= Get_System()->Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin()) - 1;	if( ax < 0         )	ax	= 0;
	int	bx	= Get_System()->Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax()) + 1;	if( bx >= Get_NX() )	bx	= Get_NX() - 1;
	int	ay	= Get_System()->Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMin()) - 1;	if( ay < 0         )	ay	= 0;
	int	by	= Get_System()->Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMax()) + 1;	if( by >= Get_NY() )	by	= Get_NY() - 1;

	TSG_Point	Center;
	TSG_Rect	Cell;

	//-----------------------------------------------------
	Center.y	= Get_System()->Get_yGrid_to_World(ay);
	Cell.yMin	= Center.y - 0.5 * Get_Cellsize();
	Cell.yMax	= Cell.yMin + Get_Cellsize();
		
	for(int y=ay; y<=by; y++, Center.y+=Get_Cellsize(), Cell.yMin+=Get_Cellsize(), Cell.yMax+=Get_Cellsize())
	{
		Center.x	= Get_System()->Get_xGrid_to_World(ax);
		Cell.xMin	= Center.x - 0.5 * Get_Cellsize();
		Cell.xMax	= Cell.xMin + Get_Cellsize();

		for(int x=ax; x<=bx; x++, Center.x+=Get_Cellsize(), Cell.xMin+=Get_Cellsize(), Cell.xMax+=Get_Cellsize())
		{
			if( !pGrid->is_NoData(x, y) )
			{
				switch( Method )
				{
				//-----------------------------------------
				default:	// polygon wise (cell centers)
					if( pPolygon->Contains(Center) )
					{
						Statistics	+= pGrid->asDouble(x, y);
					}
					break;

				//-----------------------------------------
				case  2:	// polygon wise (cell area)
					if( pPolygon->Intersects(Cell) )
					{
						Statistics	+= pGrid->asDouble(x, y);
					}
					break;

				//-----------------------------------------
				case  3:	// polygon wise (cell area weighted)
					switch( pPolygon->Intersects(Cell) )
					{
					case INTERSECTION_None     :	break;
					case INTERSECTION_Identical:
					case INTERSECTION_Contains :	Statistics.Add_Value(pGrid->asDouble(x, y),       Get_Cellarea());	break;
					case INTERSECTION_Contained:	Statistics.Add_Value(pGrid->asDouble(x, y), pPolygon->Get_Area());	break;
					case INTERSECTION_Overlaps :
						pCell->Del_Parts();

						pCell->Add_Point(Cell.xMin, Cell.yMin);	pCell->Add_Point(Cell.xMin, Cell.yMax);
						pCell->Add_Point(Cell.xMax, Cell.yMax);	pCell->Add_Point(Cell.xMax, Cell.yMin);

						if( SG_Polygon_Intersection(pPolygon, pCell, pArea) )
						{
							Statistics.Add_Value(pGrid->asDouble(x, y), pArea->Get_Area());
						}
						break;
					}
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
bool CGrid_Statistics_AddTo_Polygon::Get_Simple(CSG_Grid *pGrid, CSG_Shapes *pPolygons, CSG_Simple_Statistics *Statistics, bool bQuantiles, CSG_Grid &Index)
{
	int		i;

	for(i=0; i<pPolygons->Get_Count(); i++)
	{
		Statistics[i].Create(bQuantiles);
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) && (i = Index.asInt(x, y)) >= 0 && i < pPolygons->Get_Count() )
			{
				Statistics[i]	+= pGrid->asDouble(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::Get_Simple_Index(CSG_Shapes *pPolygons, CSG_Grid &Index)
{
	bool		bFill, *bCrossing;
	int			x, y, ix, xStart, xStop, iShape, iPart, iPoint;
	double		yPos;
	TSG_Point	pLeft, pRight, pa, pb, p;
	TSG_Rect	Extent;
	CSG_Shape	*pPolygon;

	//-----------------------------------------------------
	Index.Create(*Get_System(), pPolygons->Get_Count() < 32767 ? SG_DATATYPE_Short : SG_DATATYPE_Int);
	Index.Assign(-1.0);

	bCrossing	= (bool *)SG_Malloc(Get_NX() * sizeof(bool));

	//-----------------------------------------------------
	for(iShape=0; iShape<pPolygons->Get_Count() && Set_Progress(iShape, pPolygons->Get_Count()); iShape++)
	{
		pPolygon	= pPolygons->Get_Shape(iShape);
		Extent		= pPolygon->Get_Extent().m_rect;

		xStart		= Get_System()->Get_xWorld_to_Grid(Extent.xMin) - 1;	if( xStart < 0 )		xStart	= 0;
		xStop		= Get_System()->Get_xWorld_to_Grid(Extent.xMax) + 1;	if( xStop >= Get_NX() )	xStop	= Get_NX() - 1;

		pLeft.x		= Get_XMin() - 1.0;
		pRight.x	= Get_XMax() + 1.0;

		//-------------------------------------------------
		for(y=0, yPos=Get_YMin(); y<Get_NY(); y++, yPos+=Get_Cellsize())
		{
			if( yPos >= Extent.yMin && yPos <= Extent.yMax )
			{
				memset(bCrossing, 0, Get_NX() * sizeof(bool));

				pLeft.y	= pRight.y	= yPos;

				//-----------------------------------------
				for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
				{
					pb		= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

					for(iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
					{
						pa	= pb;
						pb	= pPolygon->Get_Point(iPoint, iPart);

						if(	(	(pa.y <= yPos && yPos < pb.y)
							||	(pa.y > yPos && yPos >= pb.y)	)	)
						{
							SG_Get_Crossing(p, pa, pb, pLeft, pRight, false);

							ix	= (int)((p.x - Get_XMin()) / Get_Cellsize() + 1.0);

							if( ix < 0)
							{
								ix	= 0;
							}
							else if( ix >= Get_NX() )
							{
								continue;
							}

							bCrossing[ix]	= !bCrossing[ix];
						}
					}
				}

				//-----------------------------------------
				for(x=xStart, bFill=false; x<=xStop; x++)
				{
					if( bCrossing[x] )
					{
						bFill	= !bFill;
					}

					if( bFill )
					{
						Index.Set_Value(x, y, iShape);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(bCrossing);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
