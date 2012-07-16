/**********************************************************
 * Version $Id$
 *********************************************************/
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

	Parameters.Add_Value(NULL, "SUM", _TL("Sum")		, _TL("")	, PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "AVG", _TL("Mean")		, _TL("")	, PARAMETER_TYPE_Bool, true );
	Parameters.Add_Value(NULL, "VAR", _TL("Variance")	, _TL("")	, PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "DEV", _TL("Deviation")	, _TL("")	, PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "MIN", _TL("Minimum")	, _TL("")	, PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "MAX", _TL("Maximum")	, _TL("")	, PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "NUM", _TL("Count")		, _TL("")	, PARAMETER_TYPE_Bool, false);

	Add_Parameters("ATTRIBUTES", _TL("Attributes"), _TL(""));

	Parameters.Add_Choice(
		NULL, "FIELD_NAME"	, _TL("Field Name"),
		_TL(""), 
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("type + original"),
			_TL("original + type"),
			_TL("original"),
			_TL("type")
		), 0
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygonStatisticsFromPoints::On_Execute(void)
{
	bool					bSum, bAvg, bVar, bDev, bMin, bMax, bNum, *bAttribute;
	int						i, n, Offset;
	CSG_Simple_Statistics	*Statistics;
	CSG_Parameters			*pParameters;
	CSG_Shapes				*pPolygons, *pPoints;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	pPolygons	= Parameters("POLYGONS")	->asShapes();

	bSum		= Parameters("SUM")->asBool();
	bAvg		= Parameters("AVG")->asBool();
	bVar		= Parameters("VAR")->asBool();
	bDev		= Parameters("DEV")->asBool();
	bMin		= Parameters("MIN")->asBool();
	bMax		= Parameters("MAX")->asBool();
	bNum		= Parameters("NUM")->asBool();

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
			pParameters->Add_Value(NULL, CSG_String::Format(SG_T("%d"), i), pPoints->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, true);
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
	Offset		= pPolygons->Get_Field_Count();
	bAttribute	= new bool[pPoints->Get_Field_Count()];

	for(i=0, n=0; i<pPoints->Get_Field_Count(); i++)
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(CSG_String::Format(SG_T("%d"), i));

		if( (bAttribute[i] = (pParameter && pParameter->asBool() && SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(i)))) == true )
		{
			CSG_String	sName	= pPoints->Get_Field_Name(i);

			if( bSum )	{	pPolygons->Add_Field(Get_Field_Name("SUM", sName), SG_DATATYPE_Double);	n++;	}
			if( bAvg )	{	pPolygons->Add_Field(Get_Field_Name("AVG", sName), SG_DATATYPE_Double);	n++;	}
			if( bVar )	{	pPolygons->Add_Field(Get_Field_Name("VAR", sName), SG_DATATYPE_Double);	n++;	}
			if( bDev )	{	pPolygons->Add_Field(Get_Field_Name("DEV", sName), SG_DATATYPE_Double);	n++;	}
			if( bMin )	{	pPolygons->Add_Field(Get_Field_Name("MIN", sName), SG_DATATYPE_Double);	n++;	}
			if( bMax )	{	pPolygons->Add_Field(Get_Field_Name("MAX", sName), SG_DATATYPE_Double);	n++;	}
			if( bNum )	{	pPolygons->Add_Field(Get_Field_Name("NUM", sName), SG_DATATYPE_Int   );	n++;	}
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

			if( pPolygon->Contains(pPoint->Get_Point(0)) )
			{
				for(i=0; i<pPoints->Get_Field_Count(); i++)
				{
					if( bAttribute[i] && !pPoint->is_NoData(i))
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
					if( bSum )	{	pPolygon->Set_Value (n++, Statistics[i].Get_Sum());			}
					if( bAvg )	{	pPolygon->Set_Value (n++, Statistics[i].Get_Mean());		}
					if( bVar )	{	pPolygon->Set_Value (n++, Statistics[i].Get_Variance());	}
					if( bDev )	{	pPolygon->Set_Value (n++, Statistics[i].Get_StdDev());		}
					if( bMin )	{	pPolygon->Set_Value (n++, Statistics[i].Get_Minimum());		}
					if( bMax )	{	pPolygon->Set_Value (n++, Statistics[i].Get_Maximum());		}
					if( bNum )	{	pPolygon->Set_Value (n++, Statistics[i].Get_Count());		}
				}
				else
				{
					if( bSum )	{	pPolygon->Set_NoData(n++);		}
					if( bAvg )	{	pPolygon->Set_NoData(n++);		}
					if( bVar )	{	pPolygon->Set_NoData(n++);		}
					if( bDev )	{	pPolygon->Set_NoData(n++);		}
					if( bMin )	{	pPolygon->Set_NoData(n++);		}
					if( bMax )	{	pPolygon->Set_NoData(n++);		}
					if( bNum )	{	pPolygon->Set_Value (n++, 0.0);	}
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
CSG_String CPolygonStatisticsFromPoints::Get_Field_Name(const CSG_String &Type, const CSG_String &Name)
{
	CSG_String	s;
	
	switch( Parameters("FIELD_NAME")->asInt() )
	{
	default:
	case 0:	s.Printf(SG_T("%s_%s"), Type.c_str(), Name.c_str());	break;
	case 1:	s.Printf(SG_T("%s_%s"), Name.c_str(), Type.c_str());	break;
	case 2:	s.Printf(SG_T("%s"   ), Name.c_str()              );	break;
	case 3:	s.Printf(SG_T("%s"   ), Type.c_str()              );	break;
	}

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
