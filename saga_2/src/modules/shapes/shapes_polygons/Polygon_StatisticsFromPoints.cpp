/*******************************************************************************
    CPolygonStatisticsFromPoints.cpp
    Copyright (C) 2005 Victor Olaya
	Reworked (C) 2010 Olaf Conrad

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Polygon_StatisticsFromPoints.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	STAT_Sum	= 0,
	STAT_Avg,
	STAT_Var,
	STAT_Dev,
	STAT_Min,
	STAT_Max,
	STAT_Num,
	STAT_Count
};

//---------------------------------------------------------
int	STAT_Flag[STAT_Count]	=
{
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40
};

//---------------------------------------------------------
CSG_String	STAT_Name[STAT_Count]	=
{
	SG_T("Sum"),
	SG_T("Mean"),
	SG_T("Variance"),
	SG_T("Deviation"),
	SG_T("Minimum"),
	SG_T("Maximum"),
	SG_T("Count")
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygonStatisticsFromPoints::CPolygonStatisticsFromPoints(void)
{
	Set_Name		(_TL("Point Statistics for Polygons"));

	Set_Author		(SG_T("V. Olaya, O. Conrad (c) 2005, 2010"));

	Set_Description	(_TW(
		"Calculates statistics over all points falling in a polygon."
	));

	Parameters.Add_Shapes(
		NULL, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "POLYGONS"	, _TL("Polygons"),
		_TL(""), 
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL, "STATISTICS"	, _TL("Statistics"),
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Add_Parameters("ATTRIBUTES", _TL("Attributes"), _TL(""));
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygonStatisticsFromPoints::On_Execute(void)
{
	int						i, j, n, Offset, *bAttribute;
	CSG_Simple_Statistics	*Statistics;
	CSG_Parameters			*pParameters;
	CSG_Shapes				*pPolygons, *pPoints;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	pPolygons	= Parameters("POLYGONS")	->asShapes();

	if( pPolygons->Get_Count() <= 0 || pPoints->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	pParameters	= Get_Parameters("ATTRIBUTES");

	pParameters->Del_Parameters();

	for(i=0; i<pPoints->Get_Field_Count(); i++)
	{
		if( SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(i)) )
		{
			CSG_Parameter	*pNode	= pParameters->Add_Node(NULL, CSG_String::Format(SG_T("%d"), i), pPoints->Get_Field_Name(i), _TL(""));

			for(j=0; j<STAT_Count; j++)
			{
				pParameters->Add_Value(pNode,
					CSG_String::Format(SG_T("%d|%d"), i, j),
					CSG_String::Format(SG_T("[%s]"), STAT_Name[j].c_str()),
					_TL(""), PARAMETER_TYPE_Bool, false
				);
			}
		}
	}

	if( !Dlg_Parameters("ATTRIBUTES") )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("STATISTICS")->asShapes() == NULL )
	{
		Parameters("STATISTICS")->Set_Value(pPolygons);
	}
	else if( pPolygons != Parameters("STATISTICS")->asShapes() )
	{
		Parameters("STATISTICS")->asShapes()->Assign(pPolygons);

		pPolygons	= Parameters("STATISTICS")->asShapes();
	}

	//-----------------------------------------------------
	bAttribute	= new int[pPoints->Get_Field_Count()];
	Offset		= pPolygons->Get_Field_Count();

	for(i=0, n=0; i<pPoints->Get_Field_Count(); i++)
	{
		bAttribute[i]	= 0;

		if( SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(i)) )
		{
			for(j=0; j<STAT_Count; j++)
			{
				CSG_Parameter	*pParameter	= pParameters->Get_Parameter(CSG_String::Format(SG_T("%d|%d"), i, j));

				if( pParameter && pParameter->asBool() )
				{
					bAttribute[i]	|= STAT_Flag[j];

					pPolygons->Add_Field(CSG_String::Format(SG_T("%s_%s"), pPoints->Get_Field_Name(i), STAT_Name[j].c_str()), SG_DATATYPE_Double);

					n++;
				}
			}
		}
	}

	if( n == 0 )
	{
		delete[](bAttribute);

		return( false );
	}

	//-----------------------------------------------------
	Statistics	= new CSG_Simple_Statistics[pPoints->Get_Field_Count()];

	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		//-------------------------------------------------
		for(i=0; i<pPoints->Get_Field_Count(); i++)
		{
			Statistics[i].Invalidate();
		}

		//-------------------------------------------------
		for(int iPoint=0; iPoint<pPoints->Get_Count() && Process_Get_Okay(); iPoint++)
		{
			CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

			if( pPolygon->is_Containing(pPoint->Get_Point(0)) )
			{
				for(i=0; i<pPoints->Get_Field_Count(); i++)
				{
					if( bAttribute[i] )
					{
						Statistics[i].Add_Value(pPoint->asDouble(i));
					}
				}
			}
		}

		//-------------------------------------------------
		for(i=0, n=Offset; i<pPoints->Get_Field_Count(); i++)
		{
			if( bAttribute[i] )
			{
				if( Statistics[i].Get_Count() > 0 )
				{
					if( bAttribute[i] & STAT_Flag[STAT_Sum] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_Sum());		}
					if( bAttribute[i] & STAT_Flag[STAT_Avg] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_Mean());		}
					if( bAttribute[i] & STAT_Flag[STAT_Var] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_Variance());	}
					if( bAttribute[i] & STAT_Flag[STAT_Dev] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_StdDev());	}
					if( bAttribute[i] & STAT_Flag[STAT_Min] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_Minimum());	}
					if( bAttribute[i] & STAT_Flag[STAT_Max] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_Maximum());	}
					if( bAttribute[i] & STAT_Flag[STAT_Num] )	{	pPolygon->Set_Value(n++, Statistics[i].Get_Count());	}
				}
				else
				{
					if( bAttribute[i] & STAT_Flag[STAT_Sum] )	{	pPolygon->Set_NoData(n++);	}
					if( bAttribute[i] & STAT_Flag[STAT_Avg] )	{	pPolygon->Set_NoData(n++);	}
					if( bAttribute[i] & STAT_Flag[STAT_Var] )	{	pPolygon->Set_NoData(n++);	}
					if( bAttribute[i] & STAT_Flag[STAT_Dev] )	{	pPolygon->Set_NoData(n++);	}
					if( bAttribute[i] & STAT_Flag[STAT_Min] )	{	pPolygon->Set_NoData(n++);	}
					if( bAttribute[i] & STAT_Flag[STAT_Max] )	{	pPolygon->Set_NoData(n++);	}
					if( bAttribute[i] & STAT_Flag[STAT_Num] )	{	pPolygon->Set_NoData(n++);	}
				}
			}
		}
	}

	//-----------------------------------------------------
	delete[](Statistics);
	delete[](bAttribute);

	DataObject_Update(pPolygons);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
