
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
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

	Set_Author		("V. Olaya, O. Conrad (c) 2005, 2010");

	Set_Description	(_TW(
		"Calculates statistics over all points falling in a polygon."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"    , _TL("Points"), _TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Fields("POINTS",
		"FIELDS"    , _TL("Attributes"), _TL("")
	);

	Parameters.Add_Shapes("",
		"POLYGONS"  , _TL("Polygons"  ), _TL(""), 
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"STATISTICS", _TL("Statistics"), _TL(""), 
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Bool("", "SUM", _TL("Sum"      ), _TL(""), false);
	Parameters.Add_Bool("", "AVG", _TL("Mean"     ), _TL(""), true );
	Parameters.Add_Bool("", "VAR", _TL("Variance" ), _TL(""), false);
	Parameters.Add_Bool("", "DEV", _TL("Deviation"), _TL(""), false);
	Parameters.Add_Bool("", "MIN", _TL("Minimum"  ), _TL(""), false);
	Parameters.Add_Bool("", "MAX", _TL("Maximum"  ), _TL(""), false);
	Parameters.Add_Bool("", "NUM", _TL("Count"    ), _TL(""), false);

	Parameters.Add_Choice("",
		"FIELD_NAME", _TL("Field Naming"), _TL(""), CSG_String::Format("%s|%s|%s|%s",
			_TL("variable type + original name"),
			_TL("original name + variable type"),
			_TL("original name"),
			_TL("variable type")
		), 0
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygonStatisticsFromPoints::On_Execute(void)
{
	bool bSum = Parameters("SUM")->asBool();
	bool bAvg = Parameters("AVG")->asBool();
	bool bVar = Parameters("VAR")->asBool();
	bool bDev = Parameters("DEV")->asBool();
	bool bMin = Parameters("MIN")->asBool();
	bool bMax = Parameters("MAX")->asBool();
	bool bNum = Parameters("NUM")->asBool();

	if( !bSum && !bAvg && !bVar && !bDev && !bMin && !bMax && !bNum )
	{
		Error_Set(_TL("no target variable in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Table_Fields *pFields = Parameters("FIELDS")->asTableFields();

	if( pFields->Get_Count() <= 0 )
	{
		Error_Set(_TL("no attributes in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pPoints   = Parameters("POINTS"  )->asShapes();
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	if( pPolygons->Get_Count() <= 0 || pPoints->Get_Count() <= 0 )
	{
		Error_Set(_TL("no records in input data"));

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
	int Offset = pPolygons->Get_Field_Count();

	for(int i=0; i<pFields->Get_Count(); i++)
	{
		CSG_String sName = pPoints->Get_Field_Name(pFields->Get_Index(i));

		if( bSum ) { pPolygons->Add_Field(Get_Field_Name("SUM", sName), SG_DATATYPE_Double); }
		if( bAvg ) { pPolygons->Add_Field(Get_Field_Name("AVG", sName), SG_DATATYPE_Double); }
		if( bVar ) { pPolygons->Add_Field(Get_Field_Name("VAR", sName), SG_DATATYPE_Double); }
		if( bDev ) { pPolygons->Add_Field(Get_Field_Name("DEV", sName), SG_DATATYPE_Double); }
		if( bMin ) { pPolygons->Add_Field(Get_Field_Name("MIN", sName), SG_DATATYPE_Double); }
		if( bMax ) { pPolygons->Add_Field(Get_Field_Name("MAX", sName), SG_DATATYPE_Double); }
		if( bNum ) { pPolygons->Add_Field(Get_Field_Name("NUM", sName), SG_DATATYPE_Long  ); }
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics *Statistics = new CSG_Simple_Statistics[pFields->Get_Count()];

	for(sLong iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon *pPolygon = pPolygons->Get_Shape(iPolygon)->asPolygon();

		//-------------------------------------------------
		for(int i=0; i<pFields->Get_Count(); i++)
		{
			Statistics[i].Invalidate();
		}

		//-------------------------------------------------
		for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Process_Get_Okay(); iPoint++)
		{
			CSG_Shape *pPoint = pPoints->Get_Shape(iPoint);

			if( pPolygon->Contains(pPoint->Get_Point()) )
			{
				for(int i=0; i<pFields->Get_Count(); i++)
				{
					if( !pPoint->is_NoData(pFields->Get_Index(i)))
					{
						Statistics[i].Add_Value(pPoint->asDouble(pFields->Get_Index(i)));
					}
				}
			}
		}

		//-------------------------------------------------
		for(int i=0, n=Offset; i<pFields->Get_Count(); i++)
		{
			if( Statistics[i].Get_Count() > 0l )
			{
				if( bSum ) { pPolygon->Set_Value (n++, Statistics[i].Get_Sum          ()); }
				if( bAvg ) { pPolygon->Set_Value (n++, Statistics[i].Get_Mean         ()); }
				if( bVar ) { pPolygon->Set_Value (n++, Statistics[i].Get_Variance     ()); }
				if( bDev ) { pPolygon->Set_Value (n++, Statistics[i].Get_StdDev       ()); }
				if( bMin ) { pPolygon->Set_Value (n++, Statistics[i].Get_Minimum      ()); }
				if( bMax ) { pPolygon->Set_Value (n++, Statistics[i].Get_Maximum      ()); }
				if( bNum ) { pPolygon->Set_Value (n++, (double)Statistics[i].Get_Count()); }
			}
			else
			{
				if( bSum ) { pPolygon->Set_NoData(n++);     }
				if( bAvg ) { pPolygon->Set_NoData(n++);     }
				if( bVar ) { pPolygon->Set_NoData(n++);     }
				if( bDev ) { pPolygon->Set_NoData(n++);     }
				if( bMin ) { pPolygon->Set_NoData(n++);     }
				if( bMax ) { pPolygon->Set_NoData(n++);     }
				if( bNum ) { pPolygon->Set_Value (n++, 0.); }
			}
		}
	}

	//-----------------------------------------------------
	delete[](Statistics);

	DataObject_Update(pPolygons);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CPolygonStatisticsFromPoints::Get_Field_Name(const CSG_String &Type, const CSG_String &Name)
{
	CSG_String s;
	
	switch( Parameters("FIELD_NAME")->asInt() )
	{
	default: s.Printf("%s_%s", Type.c_str(), Name.c_str());	break;
	case  1: s.Printf("%s_%s", Name.c_str(), Type.c_str());	break;
	case  2: s.Printf("%s"   , Name.c_str()              );	break;
	case  3: s.Printf("%s"   , Type.c_str()              );	break;
	}

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
