
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
#include <memory.h>
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
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Grid Statistics for Polygons"));

	Set_Author		(SG_T("(c) 2003 by Olaf Conrad, Quantile Calculation (c) 2007 by Johan Van de Wauw"));

	Set_Description	(_TW(
		"For each polygon statistics of the contained grid values will be generated."
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "POLY"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL, "RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL, "QUANTILES", _TL("Quantiles"), 
		_TL("Calculate distribution Quantiles"),
		PARAMETER_TYPE_Bool, 0.0
	);

	Parameters.Add_Choice(
		NULL, "QUANTILE_STEP", _TL("Quantile Step"),
		_TL("Choose which quantiles you would like to calculate"),
		_TL("median|quartiles|deciles|5% interval|"),
		1
	);
}
//---------------------------------------------------------
CGrid_Statistics_AddTo_Polygon::~CGrid_Statistics_AddTo_Polygon(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_AddTo_Polygon::On_Execute(void)
{
	bool				bQuantiles;
	int					x, y, iShape, nShapes, *Num,
						field_CELLS, field_MEAN, field_VARI, field_QUANTILES, quantile_step;
	double				*Sum, *Dif, d;
	CSG_Table_Record	*pRecord;
	CSG_Grid			*pGrid, ShapeIDs;
	CSG_Shapes			*pShapes;

	//-----------------------------------------------------
	pShapes		= Parameters("POLY")		->asShapes();
	pGrid		= Parameters("GRID")		->asGrid();
	bQuantiles	= Parameters("QUANTILES")	->asBool();

	switch(Parameters("QUANTILE_STEP")->asInt())
	{
	case 0:				quantile_step	= 50;	break;
	case 1:				quantile_step	= 25;	break;
	case 2:				quantile_step	= 10;	break;
	case 3: default:	quantile_step	= 5;	break;
	}

	//-----------------------------------------------------
	if(	pShapes->Get_Type() == SHAPE_TYPE_Polygon && pShapes->Get_Count() > 0
	&&	pGrid->is_Intersecting(pShapes->Get_Extent()) )
	{
		ShapeIDs.Create(pGrid, GRID_TYPE_Int);
		ShapeIDs.Assign(-1.0);

		//-------------------------------------------------
		if( Get_ShapeIDs(pShapes, &ShapeIDs) )
		{
			nShapes	= pShapes->Get_Count();

			Num		= (int    *)calloc(nShapes, sizeof(int   ));
			Sum		= (double *)calloc(nShapes, sizeof(double));
			Dif		= (double *)calloc(nShapes, sizeof(double));

			std::vector<std::list<double> >ShapePixels(nShapes);

			//---------------------------------------------
			for(y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					if( !pGrid->is_NoData(x, y) && (iShape = ShapeIDs.asInt(x, y)) >= 0 && iShape < nShapes )
					{
						d			= pGrid->asDouble(x, y);
						Sum[iShape]	+= d;
						Dif[iShape]	+= d * d;
						Num[iShape]	++;

						if( bQuantiles )
							ShapePixels[iShape].push_back(d);
					}
				}
			}
			
			//---------------------------------------------
			pShapes	= Parameters("RESULT")->asShapes();
			pShapes->Assign(Parameters("POLY")->asShapes());

			field_CELLS	= pShapes->Get_Field_Count();
			pShapes->Add_Field(_TL("CELLS")		, TABLE_FIELDTYPE_Int);

			field_MEAN	= pShapes->Get_Field_Count();
			pShapes->Add_Field(_TL("MEAN")		, TABLE_FIELDTYPE_Double);

			field_VARI	= pShapes->Get_Field_Count();
			pShapes->Add_Field(_TL("VARIANCE")	, TABLE_FIELDTYPE_Double);

			if( bQuantiles )
			{
				field_QUANTILES = pShapes->Get_Field_Count();

				for(int i=quantile_step; i<100; i+=quantile_step)
				{
					pShapes->Add_Field(CSG_String::Format(SG_T("Q%d"), i).c_str(), TABLE_FIELDTYPE_Double);
				}
			}

			for(iShape=0; iShape<nShapes; iShape++)
			{
				pRecord	= pShapes->Get_Shape(iShape);

				if( Num[iShape] > 0 )
				{
					d		= Sum[iShape] / (double)Num[iShape];

					pRecord->Set_Value(field_CELLS	, Num[iShape]);
					pRecord->Set_Value(field_MEAN	, d);
					pRecord->Set_Value(field_VARI	, Dif[iShape] / (double)Num[iShape] - d * d);

					if( bQuantiles )
					{
						Set_Progress(iShape,nShapes);
						ShapePixels[iShape].sort();	

						int		i	= 0;	// Used to iterate over the different quantiles
						int		k	= 0;	// Used to count at which position in the list we are j: iterates over the list

						for(std::list<double>::iterator j=ShapePixels[iShape].begin(); j!=ShapePixels[iShape].end(); j++)
						{
							k++;

							while( ((double)k / (Num[iShape])) > ((double)i * quantile_step / 100.0) )
							{
								pRecord->Set_Value(field_QUANTILES + i, *j);
								i++;
							}
						}
					}	
				}
				else
				{
					pRecord->Set_Value(field_CELLS	, 0.0);
					pRecord->Set_Value(field_MEAN	, 0.0);
					pRecord->Set_Value(field_VARI	, 0.0);
				}
			}

			//---------------------------------------------
			free(Num);
			free(Sum);
			free(Dif);

			return( true );
		}
	}

	return( false );
}

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
								ix	= pShapeIDs->Get_NX() - 1;
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


