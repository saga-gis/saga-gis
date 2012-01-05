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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
		"For each polygon statistics of the contained grid values will be generated."
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

	pNode	= Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(pNode, "COUNT" , _TL("Number of Cells")	, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MIN"   , _TL("Minimum")			, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MAX"   , _TL("Maximum")			, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "RANGE" , _TL("Range")				, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "SUM"   , _TL("Sum")				, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "MEAN"  , _TL("Mean")				, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "VAR"   , _TL("Variance")			, _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(pNode, "STDDEV", _TL("Standard Deviation")	, _TL(""), PARAMETER_TYPE_Bool, true);

	Parameters.Add_Value(
		NULL	, "QUANTILE"	, _TL("Quantiles"), 
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
bool CGrid_Statistics_AddTo_Polygon::On_Execute(void)
{
	int						iShape, Quantile, nFields, fCOUNT, fMIN, fMAX, fRANGE, fSUM, fMEAN, fVAR, fSTDDEV, fQUANTILE;
	CSG_Grid				ShapeIDs;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Shapes				*pShapes;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")		->asGridList();
	pShapes		= Parameters("POLYGONS")	->asShapes();
	Quantile	= Parameters("QUANTILE")	->asInt();

	nFields		= 0;

	fCOUNT		= Parameters("COUNT")		->asBool() ? nFields++ : -1;
	fMIN		= Parameters("MIN")			->asBool() ? nFields++ : -1;
	fMAX		= Parameters("MAX")			->asBool() ? nFields++ : -1;
	fRANGE		= Parameters("RANGE")		->asBool() ? nFields++ : -1;
	fSUM		= Parameters("SUM")			->asBool() ? nFields++ : -1;
	fMEAN		= Parameters("MEAN")		->asBool() ? nFields++ : -1;
	fVAR		= Parameters("VAR")			->asBool() ? nFields++ : -1;
	fSTDDEV		= Parameters("STDDEV")		->asBool() ? nFields++ : -1;
	fQUANTILE	= Quantile > 0                         ? nFields++ : -1;

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

	if( !Get_ShapeIDs(pShapes, &ShapeIDs) )
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
		std::vector<std::list<double> >	Values(pShapes->Get_Count());

		CSG_Simple_Statistics	*Statistics	= new CSG_Simple_Statistics[pShapes->Get_Count()];

		CSG_Grid				*pGrid		= pGrids->asGrid(iGrid);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pGrid->is_NoData(x, y) && (iShape = ShapeIDs.asInt(x, y)) >= 0 && iShape < pShapes->Get_Count() )
				{
					Statistics[iShape].Add_Value(pGrid->asDouble(x, y));

					if( fQUANTILE >= 0 )
					{
						Values[iShape].push_back(pGrid->asDouble(x, y));
					}
				}
			}
		}
			
		//-------------------------------------------------
		nFields		= pShapes->Get_Field_Count();

		if( fCOUNT    >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("CELLS")   ), SG_DATATYPE_Int   );
		if( fMIN      >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("MIN")     ), SG_DATATYPE_Double);
		if( fMAX      >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("MAX")     ), SG_DATATYPE_Double);
		if( fRANGE    >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("RANGE")   ), SG_DATATYPE_Double);
		if( fSUM      >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("SUM")     ), SG_DATATYPE_Double);
		if( fMEAN     >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("MEAN")    ), SG_DATATYPE_Double);
		if( fVAR      >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("VARIANCE")), SG_DATATYPE_Double);
		if( fSTDDEV   >= 0 )	pShapes->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("STDDEV")  ), SG_DATATYPE_Double);
		if( fQUANTILE >= 0 )
		{
			for(int iQuantile=Quantile; iQuantile<100; iQuantile+=Quantile)
			{
				pShapes->Add_Field(CSG_String::Format(SG_T("%s [Q%d]"), pGrid->Get_Name(), iQuantile), SG_DATATYPE_Double);
			}
		}

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
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
				if( fQUANTILE >= 0 )	pShape->Set_NoData(nFields + fQUANTILE);
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
					int		iQuantile, iValue;

					std::list<double>::iterator	Value;

					Values[iShape].sort();	

					for(iQuantile=0, iValue=0, Value=Values[iShape].begin(); Value!=Values[iShape].end(); Value++, iValue++)
					{
						while( ((double)iValue / (Statistics[iShape].Get_Count())) > ((double)(iQuantile + 1) * Quantile / 100.0) )
						{
							pShape->Set_Value(nFields + fQUANTILE + (iQuantile++), *Value);
						}
					}

					Values[iShape].clear();
				}	
			}
		}

		delete[](Statistics);
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
bool CGrid_Statistics_AddTo_Polygon::Get_ShapeIDs(CSG_Shapes *pShapes, CSG_Grid *pShapeIDs)
{
	bool		bFill, *bCrossing;
	int			x, y, ix, xStart, xStop, iShape, iPart, iPoint;
	double		yPos;
	TSG_Point	pLeft, pRight, pa, pb, p;
	TSG_Rect	Extent;
	CSG_Shape	*pShape;

	//-----------------------------------------------------
	pShapeIDs->Create(*Get_System(), SG_DATATYPE_Int);
	pShapeIDs->Assign(-1.0);

	bCrossing	= (bool *)SG_Malloc(pShapeIDs->Get_NX() * sizeof(bool));

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape		= pShapes->Get_Shape(iShape);
		Extent		= pShape->Get_Extent().m_rect;

		xStart		= Get_System()->Get_xWorld_to_Grid(Extent.xMin) - 1;	if( xStart < 0 )		xStart	= 0;
		xStop		= Get_System()->Get_xWorld_to_Grid(Extent.xMax) + 1;	if( xStop >= Get_NX() )	xStop	= Get_NX() - 1;

		pLeft.x		= pShapeIDs->Get_XMin() - 1.0;
		pRight.x	= pShapeIDs->Get_XMax() + 1.0;

		//-------------------------------------------------
		for(y=0, yPos=pShapeIDs->Get_YMin(); y<pShapeIDs->Get_NY(); y++, yPos+=pShapeIDs->Get_Cellsize())
		{
			if( yPos >= Extent.yMin && yPos <= Extent.yMax )
			{
				memset(bCrossing, 0, pShapeIDs->Get_NX() * sizeof(bool));

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

							ix	= (int)((p.x - pShapeIDs->Get_XMin()) / pShapeIDs->Get_Cellsize() + 1.0);

							if( ix < 0)
							{
								ix	= 0;
							}
							else if( ix >= pShapeIDs->Get_NX() )
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
						pShapeIDs->Set_Value(x, y, iShape);
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
