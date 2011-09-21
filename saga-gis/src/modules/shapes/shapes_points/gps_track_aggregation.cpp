/**********************************************************
 * Version $Id: gps_track_aggregation.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               gps_track_aggregation.cpp               //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "gps_track_aggregation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	AGG_ID	= 0,
	AGG_TRACK,
	AGG_DATE,
	AGG_TIME,
	AGG_PARM,
	AGG_MIN,
	AGG_MAX,
	AGG_RANGE,
	AGG_STDDEV,
	AGG_COUNT,
	AGG_DROPPED,
	AGG_DTIME,
	AGG_X,
	AGG_Y
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGPS_Track_Aggregation::CGPS_Track_Aggregation(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Aggregate Point Observations"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Shapes(
		NULL	, "REFERENCE"		, _TL("Reference Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "REFERENCE_ID"	, _TL("ID"),
		_TL("")
	);

	pNode	= Parameters.Add_Table(
		NULL	, "OBSERVATIONS"	, _TL("Observations"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "X"				, _TL("X"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "Y"				, _TL("Y"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "TRACK"			, _TL("Track"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "DATE"			, _TL("Date"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "TIME"			, _TL("Time"),
		_TL("expected to be the second of day")
	);

	Parameters.Add_Table_Field(
		pNode	, "PARAMETER"		, _TL("Parameter"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "AGGREGATED"		, _TL("Aggregated"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "EPS_TIME"		, _TL("Maximum Time Span (Seconds)"),
		_TL("ignored if set to zero"),
		PARAMETER_TYPE_Double, 60.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "EPS_SPACE"		, _TL("Maximum Distance"),
		_TL("given as map units of reference points, ignored if set to zero"),
		PARAMETER_TYPE_Double, 0.002, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "VERBOSE"			, _TL("Verbose"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGPS_Track_Aggregation::On_Execute(void)
{
	bool				bVerbose;
	int					fRefID, fX, fY, fTrack, fDate, fTime, fParameter;
	double				eps_Time, eps_Space;
	CSG_Table			*pObservations, *pAggregated;
	CSG_Shapes_Search	Reference;

	//-----------------------------------------------------
	pObservations	= Parameters("OBSERVATIONS")	->asTable ();
	pAggregated		= Parameters("AGGREGATED")		->asTable ();
	fRefID			= Parameters("REFERENCE_ID")	->asInt   ();
	fX				= Parameters("X")				->asInt   ();
	fY				= Parameters("Y")				->asInt   ();
	fTrack			= Parameters("TRACK")			->asInt   ();
	fDate			= Parameters("DATE")			->asInt   ();
	fTime			= Parameters("TIME")			->asInt   ();
	fParameter		= Parameters("PARAMETER")		->asInt   ();
	eps_Time		= Parameters("EPS_TIME")		->asDouble();
	eps_Space		= Parameters("EPS_SPACE")		->asDouble();
	bVerbose		= Parameters("VERBOSE")			->asBool  ();

	//-----------------------------------------------------
	if( !Reference.Create(Parameters("REFERENCE")->asShapes()) )
	{
		Error_Set(_TL("could not initialize reference point search engine"));

		return( false );
	}

	//-----------------------------------------------------
	if( !pObservations->Set_Index(fTrack, TABLE_INDEX_Ascending, fDate, TABLE_INDEX_Ascending, fTime, TABLE_INDEX_Ascending) )
	{
		Error_Set(_TL("could not create index on observations"));

		return( false );
	}

	//-----------------------------------------------------
	pAggregated->Destroy();
	pAggregated->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pObservations->Get_Name(), _TL("aggregated")));

	pAggregated->Add_Field(SG_T("REFID")		, SG_DATATYPE_String);	// AGG_ID
	pAggregated->Add_Field(SG_T("TRACK")		, SG_DATATYPE_String);	// AGG_TRACK
	pAggregated->Add_Field(SG_T("DATE")			, SG_DATATYPE_String);	// AGG_DATE
	pAggregated->Add_Field(SG_T("TIME")			, SG_DATATYPE_String);	// AGG_TIME

	pAggregated->Add_Field(pObservations->Get_Field_Name(fParameter), SG_DATATYPE_Double);	// AGG_PARM

	if( bVerbose )
	{
		pAggregated->Add_Field(SG_T("MIN")			, SG_DATATYPE_Double);	// AGG_MIN,
		pAggregated->Add_Field(SG_T("MAX")			, SG_DATATYPE_Double);	// AGG_MAX
		pAggregated->Add_Field(SG_T("RANGE")		, SG_DATATYPE_Double);	// AGG_RANGE
		pAggregated->Add_Field(SG_T("STDDEV")		, SG_DATATYPE_Double);	// AGG_STDDEV,
		pAggregated->Add_Field(SG_T("COUNT")		, SG_DATATYPE_Int   );	// AGG_COUNT,
		pAggregated->Add_Field(SG_T("DROPPED")		, SG_DATATYPE_Int   );	// AGG_DROPPED,
		pAggregated->Add_Field(SG_T("DTIME")		, SG_DATATYPE_Double);	// AGG_DTIME,
		pAggregated->Add_Field(SG_T("X")			, SG_DATATYPE_Double);	// AGG_X
		pAggregated->Add_Field(SG_T("Y")			, SG_DATATYPE_Double);	// AGG_Y
	}

	//-----------------------------------------------------
	int						nDropped;
	double					iTime, dTime;
	TSG_Point				Position;
	CSG_String				iTrack, iDate;
	CSG_Table_Record		*pAggregate, *pObservation;
	CSG_Shape				*pReference, *pNearest;
	CSG_Simple_Statistics	Statistic;

	pAggregate	= NULL;
	nDropped	= 0;

	//-----------------------------------------------------
	for(int iObservation=0; iObservation<pObservations->Get_Count() && Set_Progress(iObservation, pObservations->Get_Count()); iObservation++)
	{
		pObservation	= pObservations->Get_Record(iObservation);

		if(	iTrack.Cmp(pObservation->asString(fTrack))
		||	iDate .Cmp(pObservation->asString(fDate ))
		||	(eps_Time > 0.0 && eps_Time <= pObservation->asDouble(fTime) - iTime) || !pAggregate )
		{
			pReference	= NULL;
		}

		Position.x	= pObservation->asDouble(fX);
		Position.y	= pObservation->asDouble(fY);
		pNearest	= Reference.Get_Point_Nearest(Position.x, Position.y);

		if( eps_Space > 0.0 && eps_Space <= SG_Get_Distance(Position, pNearest->Get_Point(0)) )
		{
			nDropped++;
		}
		else
		{
			if( pReference != pNearest )
			{
				if( pAggregate )
				{
					pAggregate	->Set_Value(AGG_PARM   , Statistic.Get_Mean   ());

					if( bVerbose )
					{
						pAggregate	->Set_Value(AGG_MIN    , Statistic.Get_Minimum());
						pAggregate	->Set_Value(AGG_MAX    , Statistic.Get_Maximum());
						pAggregate	->Set_Value(AGG_RANGE  , Statistic.Get_Range  ());
						pAggregate	->Set_Value(AGG_STDDEV , Statistic.Get_StdDev ());
						pAggregate	->Set_Value(AGG_COUNT  , Statistic.Get_Count  ());
						pAggregate	->Set_Value(AGG_DROPPED, nDropped);
						pAggregate	->Set_Value(AGG_DTIME  , pObservation->asDouble(fTime) - iTime);
						pAggregate	->Set_Value(AGG_TIME   , iTime + 0.5 * (pObservation->asDouble(fTime) - iTime));
					}
				}

				Statistic	.Invalidate();

				iTrack		= pObservation->asString(fTrack);
				iDate		= pObservation->asString(fDate );
				iTime		= pObservation->asDouble(fTime );

				pReference	= pNearest;

				pAggregate	= pAggregated->Add_Record();
				pAggregate	->Set_Value(AGG_ID   , pReference->asString(fRefID));
				pAggregate	->Set_Value(AGG_TRACK, iTrack);
				pAggregate	->Set_Value(AGG_DATE , iDate );

				if( bVerbose )
				{
					pAggregate	->Set_Value(AGG_X, pReference->Get_Point(0).x);
					pAggregate	->Set_Value(AGG_Y, pReference->Get_Point(0).y);
				}
			}

			Statistic.Add_Value(pObservation->asDouble(fParameter));

			dTime		= pObservation->asDouble(fTime) - iTime;
		}
	}

	if( pAggregate )
	{
		pAggregate	->Set_Value(AGG_PARM   , Statistic.Get_Mean   ());

		if( bVerbose )
		{
			pAggregate	->Set_Value(AGG_MIN    , Statistic.Get_Minimum());
			pAggregate	->Set_Value(AGG_MAX    , Statistic.Get_Maximum());
			pAggregate	->Set_Value(AGG_RANGE  , Statistic.Get_Range  ());
			pAggregate	->Set_Value(AGG_STDDEV , Statistic.Get_StdDev ());
			pAggregate	->Set_Value(AGG_COUNT  , Statistic.Get_Count  ());
			pAggregate	->Set_Value(AGG_DROPPED, nDropped);
			pAggregate	->Set_Value(AGG_DTIME  , dTime);
			pAggregate	->Set_Value(AGG_TIME   , iTime + 0.5 * dTime);
		}
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
