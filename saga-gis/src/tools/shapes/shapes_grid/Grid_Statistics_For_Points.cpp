/**********************************************************
 * Version $Id: Grid_Statistics_For_Points.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//             Grid_Statistics_For_Points.cpp            //
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
#include "Grid_Statistics_For_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_For_Points::CGrid_Statistics_For_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Statistics for Points"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"For each given point statistics based on all grid cells in the defined neighbourhood will be calculated."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "KERNEL_TYPE"	, _TL("Kernel Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("square"),
			_TL("circle")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "KERNEL_SIZE"	, _TL("Kernel Size"),
		_TL("kernel size defined as radius number of cells"),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "NAMING"		, _TL("Field Naming"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("grid number"),
			_TL("grid name")
		), 1
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
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
bool CGrid_Statistics_For_Points::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( pPoints->Get_Count() <= 0 )
	{
		Error_Set(_TL("no points in input layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	if( !Get_System().Get_Extent().Intersects(pPoints->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid(s) and points layer"));

		return( false );
	}

	//-----------------------------------------------------
	if( !m_Kernel.Set_Radius(Parameters("KERNEL_SIZE")->asInt(), Parameters("KERNEL_TYPE")->asInt() == 0) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	//-----------------------------------------------------
	int	Quantile	= Parameters("QUANTILE")->asInt();
	int	nFields		= 0, offField;

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
	if( Parameters("RESULT")->asShapes() != NULL && Parameters("RESULT")->asShapes() != pPoints )
	{
		pPoints	= Parameters("RESULT")->asShapes();
		pPoints	->Assign(Parameters("POINTS")->asShapes());
		pPoints	->Set_Name("%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), _TL("Grid Statistics"));
	}

	//-----------------------------------------------------
	{
		#define GET_FIELD_NAME(VAR)	Naming == 0 ? CSG_String::Format(SG_T("G%02d_%s"), iGrid + 1, VAR) : CSG_String::Format(SG_T("%s (%s)"), pGrids->Get_Grid(iGrid)->Get_Name(), VAR)

		int	Naming	= Parameters("NAMING")->asInt();

		offField	= pPoints->Get_Field_Count();

		for(int iGrid=0; iGrid<pGrids->Get_Grid_Count(); iGrid++)
		{
			int	iField	= pPoints->Get_Field_Count();

			if( fCOUNT    >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("CELLS"   )), SG_DATATYPE_Int   );
			if( fMIN      >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("MIN"     )), SG_DATATYPE_Double);
			if( fMAX      >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("MAX"     )), SG_DATATYPE_Double);
			if( fRANGE    >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("RANGE"   )), SG_DATATYPE_Double);
			if( fSUM      >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("SUM"     )), SG_DATATYPE_Double);
			if( fMEAN     >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("MEAN"    )), SG_DATATYPE_Double);
			if( fVAR      >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("VARIANCE")), SG_DATATYPE_Double);
			if( fSTDDEV   >= 0 )	pPoints->Add_Field(GET_FIELD_NAME(_TL("STDDEV"  )), SG_DATATYPE_Double);
			if( fQUANTILE >= 0 )
			{
				for(int iQuantile=Quantile; iQuantile<100; iQuantile+=Quantile)
				{
					pPoints->Add_Field(GET_FIELD_NAME(CSG_String::Format(SG_T("Q%02d"), iQuantile).c_str()), SG_DATATYPE_Double);

					if( iGrid == 0 && iQuantile > Quantile )
					{
						nFields++;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

		for(int iGrid=0; iGrid<pGrids->Get_Grid_Count(); iGrid++)
		{
			CSG_Simple_Statistics	Statistics(Quantile >= 0);

			int	iField	= offField + iGrid * nFields;

			if( !Get_Statistics(pPoint->Get_Point(0), pGrids->Get_Grid(iGrid), Statistics) )
			{
				if( fCOUNT    >= 0 )	pPoint->Set_NoData(iField + fCOUNT );
				if( fMIN      >= 0 )	pPoint->Set_NoData(iField + fMIN   );
				if( fMAX      >= 0 )	pPoint->Set_NoData(iField + fMAX   );
				if( fRANGE    >= 0 )	pPoint->Set_NoData(iField + fRANGE );
				if( fSUM      >= 0 )	pPoint->Set_NoData(iField + fSUM   );
				if( fMEAN     >= 0 )	pPoint->Set_NoData(iField + fMEAN  );
				if( fVAR      >= 0 )	pPoint->Set_NoData(iField + fVAR   );
				if( fSTDDEV   >= 0 )	pPoint->Set_NoData(iField + fSTDDEV);
				if( fQUANTILE >= 0 )
				{
					for(int iQuantile=Quantile; iQuantile<100; iQuantile+=Quantile, iField++)
					{
						pPoint->Set_NoData(iField + fQUANTILE);
					}
				}
			}
			else
			{
				if( fCOUNT    >= 0 )	pPoint->Set_Value(iField + fCOUNT , Statistics.Get_Count   ());
				if( fMIN      >= 0 )	pPoint->Set_Value(iField + fMIN   , Statistics.Get_Minimum ());
				if( fMAX      >= 0 )	pPoint->Set_Value(iField + fMAX   , Statistics.Get_Maximum ());
				if( fRANGE    >= 0 )	pPoint->Set_Value(iField + fRANGE , Statistics.Get_Range   ());
				if( fSUM      >= 0 )	pPoint->Set_Value(iField + fSUM   , Statistics.Get_Sum     ());
				if( fMEAN     >= 0 )	pPoint->Set_Value(iField + fMEAN  , Statistics.Get_Mean    ());
				if( fVAR      >= 0 )	pPoint->Set_Value(iField + fVAR   , Statistics.Get_Variance());
				if( fSTDDEV   >= 0 )	pPoint->Set_Value(iField + fSTDDEV, Statistics.Get_StdDev  ());
				if( fQUANTILE >= 0 )
				{
					for(int iQuantile=Quantile; iQuantile<100; iQuantile+=Quantile, iField++)
					{
						pPoint->Set_Value(iField + fQUANTILE, Statistics.Get_Quantile(iQuantile));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pPoints);

	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_For_Points::Get_Statistics(const TSG_Point &Point, CSG_Grid *pGrid, CSG_Simple_Statistics &Statistics)
{
	int	x	= Get_System().Get_xWorld_to_Grid(Point.x);
	int	y	= Get_System().Get_yWorld_to_Grid(Point.y);

	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i, x);
		int	iy	= m_Kernel.Get_Y(i, y);

		if( pGrid->is_InGrid(ix, iy) )
		{
			Statistics	+= pGrid->asDouble(ix, iy);
		}
	}

	return( Statistics.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
