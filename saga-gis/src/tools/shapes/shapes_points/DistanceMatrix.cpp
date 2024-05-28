
/*******************************************************************************
    DistanceMatrix.cpp
    Copyright (C) Victor Olaya
    
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "DistanceMatrix.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDistanceMatrix::CDistanceMatrix(void)
{	
	Set_Name		(_TL("Point to Point Distances"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Computes distances between pairs of points."
	));

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"), 
		_TL(""), 
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"ID_POINTS"	, _TL("Identifier"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes("",
		"NEAR"		, _TL("Near Points"), 
		_TL(""), 
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("NEAR",
		"ID_NEAR"	, _TL("Identifier"),
		_TL(""),
		true
	);

	Parameters.Add_Table("",
		"DISTANCES"	, _TL("Distances"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"LINES"		, _TL("Distances as Lines"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice("",
		"FORMAT"	, _TL("Output Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("complete input times near points matrix"),
			_TL("each pair with a single record"),
			_TL("find only the nearest point for each input point")
		), 1
	);

	Parameters.Add_Double("",
		"MIN_DIST"	, _TL("Minimum Distance"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Double("",
		"MAX_DIST"	, _TL("Maximum Distance"),
		_TL("ignored if set to zero (consider all pairs)"),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDistanceMatrix::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FORMAT") )
	{
		pParameters->Set_Enabled("MIN_DIST", pParameter->asInt() == 1);
		pParameters->Set_Enabled("MAX_DIST", pParameter->asInt() >= 1);
		pParameters->Set_Enabled("LINES"   , pParameter->asInt() >= 1);
	}

	if( pParameter->Cmp_Identifier("NEAR") )
	{
		pParameters->Set_Enabled("ID_NEAR" , pParameter->asShapes() != NULL);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_ID(p, id, i)	(id < 0 ? CSG_String::Format("%d", i + 1) : CSG_String(p->Get_Shape(i)->asString(id)))

//---------------------------------------------------------
bool CDistanceMatrix::On_Execute(void)
{
	CSG_Shapes   *pPoints = Parameters("POINTS"   )->asShapes();
	int         id_Points = Parameters("ID_POINTS")->asInt   ();
	CSG_Shapes   *pTarget = Parameters("NEAR"     )->asShapes();
	int         id_Target = Parameters("ID_NEAR"  )->asInt   ();
	CSG_Table *pDistances = Parameters("DISTANCES")->asTable ();

	//-----------------------------------------------------
	if( pTarget == NULL )
	{
		pTarget = pPoints; id_Target = id_Points;
	}

	pDistances->Destroy();

	if( pPoints != pTarget )
	{
		pDistances->Fmt_Name("%s [%s / %s]", _TL("Distances"), pPoints->Get_Name(), pTarget->Get_Name());
	}
	else
	{
		pDistances->Fmt_Name("%s [%s]", _TL("Distances"), pPoints->Get_Name());
	}

	switch( Parameters("FORMAT")->asInt() )
	{
	//-----------------------------------------------------
	case  0: { // complete input times near points matrix
		pDistances->Add_Field(id_Points < 0 ? SG_T("ID") : pPoints->Get_Field_Name(id_Points), SG_DATATYPE_String);

		for(sLong iTarget=0; iTarget<pTarget->Get_Count(); iTarget++)
		{
			pDistances->Add_Field(GET_ID(pTarget, id_Target, iTarget), SG_DATATYPE_Double);
		}

		for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
		{
			CSG_Point Point = pPoints->Get_Shape(iPoint)->Get_Point();

			CSG_Table_Record *pRecord = pDistances->Add_Record();

			pRecord->Set_Value(0, GET_ID(pPoints, id_Points, iPoint));

			for(sLong iTarget=0; iTarget<pTarget->Get_Count(); iTarget++)
			{
				pRecord->Set_Value(1 + (int)iTarget, SG_Get_Distance(Point, pTarget->Get_Shape(iTarget)->Get_Point()));
			}
		}
		break; }

	//-----------------------------------------------------
	default: { // each pair with a single record
		pDistances->Add_Field("FROM"    , SG_DATATYPE_String);
		pDistances->Add_Field("TO"      , SG_DATATYPE_String);
		pDistances->Add_Field("DISTANCE", SG_DATATYPE_Double);

		CSG_Shapes *pLines = Parameters("LINES")->asShapes();

		if( pLines )
		{
			pLines->Create(SHAPE_TYPE_Line, pDistances->Get_Name(), pDistances);
		}

		double min_Distance = Parameters("MIN_DIST")->asDouble();
		double max_Distance = Parameters("MAX_DIST")->asDouble();

		if( min_Distance > max_Distance )
		{
			min_Distance = 0.;

			Message_Fmt("%s: %s", _TL("Warning"), _TL("Minimum distance has been set greater than maximum distance and will be ignored!"));
		}

		for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
		{
			CSG_Point Point = pPoints->Get_Shape(iPoint)->Get_Point();

			for(sLong iTarget=pPoints!=pTarget?0:iPoint+1; iTarget<pTarget->Get_Count(); iTarget++)
			{
				double Distance = SG_Get_Distance(Point, pTarget->Get_Shape(iTarget)->Get_Point());

				if( Distance >= min_Distance && (Distance <= max_Distance || max_Distance <= 0.) )
				{
					CSG_Table_Record *pDistance = pDistances->Add_Record();

					pDistance->Set_Value(0, GET_ID(pPoints, id_Points, iPoint ));
					pDistance->Set_Value(1, GET_ID(pTarget, id_Target, iTarget));
					pDistance->Set_Value(2, Distance);

					if( pLines )
					{
						CSG_Shape *pLine = pLines->Add_Shape(pDistance, SHAPE_COPY_ATTR);

						pLine->Add_Point(Point);
						pLine->Add_Point(pTarget->Get_Shape(iTarget)->Get_Point());
					}
				}
			}
		}
		break; }

	//-----------------------------------------------------
	case  2: { // find only the nearest point for each input point
		pDistances->Add_Field("FROM"    , SG_DATATYPE_String);
		pDistances->Add_Field("TO"      , SG_DATATYPE_String);
		pDistances->Add_Field("DISTANCE", SG_DATATYPE_Double);

		CSG_Shapes *pLines = Parameters("LINES")->asShapes();

		if( pLines )
		{
			pLines->Create(SHAPE_TYPE_Line, pDistances->Get_Name(), pDistances);
		}

		for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
		{
			CSG_Point Point = pPoints->Get_Shape(iPoint)->Get_Point(), to_Point;

			CSG_Table_Record *pDistance = NULL; double max_Distance = -1.;

			for(sLong iTarget=0; iTarget<pTarget->Get_Count(); iTarget++)
			{
				if( pPoints != pTarget || iPoint != iTarget )
				{
					double Distance = SG_Get_Distance(Point, pTarget->Get_Shape(iTarget)->Get_Point());

					if( !pDistance || Distance < max_Distance )
					{
						if( !pDistance )
						{
							pDistance = pDistances->Add_Record();

							pDistance->Set_Value(0, GET_ID(pPoints, id_Points, iPoint));
						}

						pDistance->Set_Value(1, GET_ID(pTarget, id_Target, iTarget));
						pDistance->Set_Value(2, Distance);

						max_Distance = Distance; to_Point = pTarget->Get_Shape(iTarget)->Get_Point();
					}
				}
			}

			if( pLines && pDistance )
			{
				CSG_Shape *pLine = pLines->Add_Shape(pDistance, SHAPE_COPY_ATTR);

				pLine->Add_Point(Point   );
				pLine->Add_Point(to_Point);
			}
		}
		break; }
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
