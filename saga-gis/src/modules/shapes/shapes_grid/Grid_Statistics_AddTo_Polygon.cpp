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
#include <vector>
#include <list>

#include "Grid_Statistics_AddTo_Polygon.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_AddTo_Polygon::CGrid_Statistics_AddTo_Polygon(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Grid Statistics for Polygons"));

	Set_Author		(SG_T("(c) 2003 by Olaf Conrad, Quantile Calculation (c) 2007 by Johan Van de Wauw"));

	Set_Description	(_TW(
		"For each polygon statistics about the values of all contained grid nodes will be generated."
	));


	//-----------------------------------------------------
	// 2. Parameters...

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
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("standard"),
			_TL("shape wise, supports overlapping polygons")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "NAMING"		, _TL("Field Naming"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("grid number"),
			_TL("grid name")
		), 1
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(pNode, "COUNT"   , _TL("Number of Cells")   , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MIN"     , _TL("Minimum")           , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MAX"     , _TL("Maximum")           , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "RANGE"   , _TL("Range")             , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "SUM"     , _TL("Sum")               , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MEAN"    , _TL("Mean")              , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "VAR"     , _TL("Variance")          , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "STDDEV"  , _TL("Standard Deviation"), _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "QUANTILE", _TL("Quantile")          , 
		_TL("Calculate distribution quantiles. Value specifies interval (median=50, quartiles=25, deciles=10, ...). Set to zero to omit quantile calculation."),
		PARAMETER_TYPE_Int, 0, 0, true, 50, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_FIELD_NAME(VAR)	Naming == 0 ? CSG_String::Format(SG_T("G%02d_%s"), iGrid + 1, VAR) : CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), VAR)

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::On_Execute(void)
{
	int						Method, Naming, Quantile;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Shapes				*pShapes;

	//-----------------------------------------------------
	pGrids			= Parameters("GRIDS"   )->asGridList();
	pShapes			= Parameters("POLYGONS")->asShapes();
	Method			= Parameters("METHOD"  )->asInt();
	Naming			= Parameters("NAMING"  )->asInt();
	Quantile		= Parameters("QUANTILE")->asInt();

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

	if( pShapes->Get_Count() <= 0 )
	{
		Error_Set(_TL("no polygons in polygon layer"));

		return( false );
	}

	if( !Get_System()->Get_Extent().Intersects(pShapes->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid(s) and polygon layer"));

		return( false );
	}

	if( Method == 0 && !Get_ShapeIDs(pShapes) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asShapes() != NULL && Parameters("RESULT")->asShapes() != pShapes )
	{
		pShapes		= Parameters("RESULT")->asShapes();
		pShapes		->Assign(Parameters("POLYGONS")->asShapes());
	}

	//-----------------------------------------------------
	for(int iGrid=0; iGrid<pGrids->Get_Count() && Process_Get_Okay(); iGrid++)
	{
		CSG_Simple_Statistics	*Statistics	= new CSG_Simple_Statistics[pShapes->Get_Count()];
		CSG_Table				*Values		= fQUANTILE >= 0 ? new CSG_Table[pShapes->Get_Count()] : NULL;
		CSG_Grid				*pGrid		= pGrids->asGrid(iGrid);

		Process_Set_Text(CSG_String::Format(SG_T("[%d/%d] %s"), 1 + iGrid, pGrids->Get_Count(), pGrid->Get_Name()));

		if( (Method == 0 && Get_Statistics    (pGrid, pShapes, Statistics, Values))
		||  (Method == 1 && Get_Statistics_Alt(pGrid, pShapes, Statistics, Values)) )
		{
			nFields		= pShapes->Get_Field_Count();

			if( fCOUNT    >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("CELLS")   ), SG_DATATYPE_Int   );
			if( fMIN      >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("MIN")     ), SG_DATATYPE_Double);
			if( fMAX      >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("MAX")     ), SG_DATATYPE_Double);
			if( fRANGE    >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("RANGE")   ), SG_DATATYPE_Double);
			if( fSUM      >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("SUM")     ), SG_DATATYPE_Double);
			if( fMEAN     >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("MEAN")    ), SG_DATATYPE_Double);
			if( fVAR      >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("VARIANCE")), SG_DATATYPE_Double);
			if( fSTDDEV   >= 0 )	pShapes->Add_Field(GET_FIELD_NAME(_TL("STDDEV")  ), SG_DATATYPE_Double);
			if( fQUANTILE >= 0 )
			{
				for(int iQuantile=Quantile; iQuantile<100; iQuantile+=Quantile)
				{
					pShapes->Add_Field(GET_FIELD_NAME(CSG_String::Format(SG_T("Q%02d"), iQuantile).c_str()), SG_DATATYPE_Double);
				}
			}

			//---------------------------------------------
			for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
			{
				CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

				if( Statistics[iShape].Get_Count() == 0 )
				{
					if( fCOUNT    >= 0 )	pShape->Set_NoData(nFields + fCOUNT);
					if( fMIN      >= 0 )	pShape->Set_NoData(nFields + fMIN);
					if( fMAX      >= 0 )	pShape->Set_NoData(nFields + fMAX);
					if( fRANGE    >= 0 )	pShape->Set_NoData(nFields + fRANGE);
					if( fSUM      >= 0 )	pShape->Set_NoData(nFields + fSUM);
					if( fMEAN     >= 0 )	pShape->Set_NoData(nFields + fMEAN);
					if( fVAR      >= 0 )	pShape->Set_NoData(nFields + fVAR);
					if( fSTDDEV   >= 0 )	pShape->Set_NoData(nFields + fSTDDEV);
					if( fQUANTILE >= 0 )
					{
						for(int iQuantile=Quantile, iField=nFields + fQUANTILE; iQuantile<100; iQuantile+=Quantile, iField++)
						{
							pShape->Set_NoData(iField);
						}
					}
				}
				else
				{
					if( fCOUNT    >= 0 )	pShape->Set_Value(nFields + fCOUNT	, Statistics[iShape].Get_Count   ());
					if( fMIN      >= 0 )	pShape->Set_Value(nFields + fMIN	, Statistics[iShape].Get_Minimum ());
					if( fMAX      >= 0 )	pShape->Set_Value(nFields + fMAX	, Statistics[iShape].Get_Maximum ());
					if( fRANGE    >= 0 )	pShape->Set_Value(nFields + fRANGE	, Statistics[iShape].Get_Range   ());
					if( fSUM      >= 0 )	pShape->Set_Value(nFields + fSUM	, Statistics[iShape].Get_Sum     ());
					if( fMEAN     >= 0 )	pShape->Set_Value(nFields + fMEAN	, Statistics[iShape].Get_Mean    ());
					if( fVAR      >= 0 )	pShape->Set_Value(nFields + fVAR	, Statistics[iShape].Get_Variance());
					if( fSTDDEV   >= 0 )	pShape->Set_Value(nFields + fSTDDEV	, Statistics[iShape].Get_StdDev  ());
					if( fQUANTILE >= 0 )
					{
						Values[iShape].Set_Index(0, TABLE_INDEX_Ascending);

						double	dQuantile	= Values[iShape].Get_Count() / 100.0;

						for(int iQuantile=Quantile, iField=nFields + fQUANTILE; iQuantile<100; iQuantile+=Quantile, iField++)
						{
							pShape->Set_Value(iField, Values[iShape].Get_Record_byIndex((int)(iQuantile * dQuantile))->asDouble(0));
						}
					}
				}
			}
		}

		//-------------------------------------------------
		delete[](Statistics);

		if( Values )
		{
			delete[](Values);
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pShapes);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::Get_Statistics(CSG_Grid *pGrid, CSG_Shapes *pShapes, CSG_Simple_Statistics *Statistics, CSG_Table *Values)
{
	int		x, y, iShape;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) && (iShape = m_ShapeIDs.asInt(x, y)) >= 0 && iShape < pShapes->Get_Count() )
			{
				Statistics[iShape].Add_Value(pGrid->asDouble(x, y));

				if( Values )
				{
					if( Values[iShape].Get_Field_Count() == 0 )
					{
						Values[iShape].Add_Field("Z", SG_DATATYPE_Double);
					}

					Values[iShape].Add_Record()->Set_Value(0, pGrid->asDouble(x, y));
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::Get_Statistics_Alt(CSG_Grid *pGrid, CSG_Shapes *pShapes, CSG_Simple_Statistics *Statistics, CSG_Table *Values)
{
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape_Polygon	*pShape	= (CSG_Shape_Polygon *)pShapes->Get_Shape(iShape);

		int	ax	= Get_System()->Get_xWorld_to_Grid(pShapes->Get_Extent().Get_XMin()) - 1;	if( ax < 0 )			ax	= 0;
		int	bx	= Get_System()->Get_xWorld_to_Grid(pShapes->Get_Extent().Get_XMax()) + 1;	if( bx >= Get_NX() )	bx	= Get_NX() - 1;
		int	ay	= Get_System()->Get_yWorld_to_Grid(pShapes->Get_Extent().Get_YMin()) - 1;	if( ay < 0 )			ay	= 0;
		int	by	= Get_System()->Get_yWorld_to_Grid(pShapes->Get_Extent().Get_YMax()) + 1;	if( by >= Get_NY() )	by	= Get_NY() - 1;

		double	py	= Get_System()->Get_yGrid_to_World(ay);

		for(int y=ay; y<=by; y++, py+=Get_Cellsize())
		{
			double	px	= Get_System()->Get_xGrid_to_World(ax);

			for(int x=ax; x<=bx; x++, px+=Get_Cellsize())
			{
				if( !pGrid->is_NoData(x, y) && pShape->Contains(px, py) )
				{
					Statistics[iShape].Add_Value(pGrid->asDouble(x, y));

					if( Values )
					{
						if( Values[iShape].Get_Field_Count() == 0 )
						{
							Values[iShape].Add_Field("Z", SG_DATATYPE_Double);
						}

						Values[iShape].Add_Record()->Set_Value(0, pGrid->asDouble(x, y));
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
bool CGrid_Statistics_AddTo_Polygon::Get_ShapeIDs(CSG_Shapes *pShapes)
{
	bool		bFill, *bCrossing;
	int			x, y, ix, xStart, xStop, iShape, iPart, iPoint;
	double		yPos;
	TSG_Point	pLeft, pRight, pa, pb, p;
	TSG_Rect	Extent;
	CSG_Shape	*pShape;

	//-----------------------------------------------------
	m_ShapeIDs.Create(*Get_System(), pShapes->Get_Count() < 32767 ? SG_DATATYPE_Short : SG_DATATYPE_Int);
	m_ShapeIDs.Assign(-1.0);

	bCrossing	= (bool *)SG_Malloc(Get_NX() * sizeof(bool));

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape		= pShapes->Get_Shape(iShape);
		Extent		= pShape->Get_Extent().m_rect;

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
				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					pb		= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);

					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						pa	= pb;
						pb	= pShape->Get_Point(iPoint, iPart);

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
						m_ShapeIDs.Set_Value(x, y, iShape);
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
