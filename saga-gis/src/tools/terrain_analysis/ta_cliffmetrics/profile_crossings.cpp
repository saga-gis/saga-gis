
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     CliffMetrics                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  profile_crossings.cpp                //
//                                                       //
//                 Copyright (C) 2020 by                 //
//               Andres Payo & Olaf Conrad               //
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
#include "profile_crossings.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CProfile_Crossings::CProfile_Crossings(void)
{
	Set_Name		(_TL("Coastal Profile Crossings"));

	Set_Author		("Andres Payo & Olaf Conrad (c) 2020");

	Set_Description	(_TW(
		"The Coastal Profile Crossings tool identifies the crossing points between coastal profiles "
		"along a reference coastline (from CliffMetrics Normal outputs) and any other coast lines "
		"and calculates the distance and coastline differences metrics. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES_SeaSide"		, _TL("SeaSide Profile Lines Layer"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"LINES_LandSide"	, _TL("LandSide Profile Lines Layer"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"LINES_Coast"		, _TL("Coast Lines Layer"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"CROSSINGS_SEASIDE"	, _TL("Crossings at Sea Side"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"CROSSINGS_LANDSIDE", _TL("Crossings at Land Side"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
	
	Parameters.Add_Choice("",
		"ATTRIBUTES"		, _TL("Parent Attributes"),
		_TL("attributes inherited by parent lines layers"),
		CSG_String::Format("%s|%s|%s",
			_TL("index"),
			_TL("attributes"),
			_TL("index and attributes")
		), 0
	);

	Parameters.Add_Shapes("",
		"DISTANCES"			, _TL("Distances to profile start point"),
		_TL("Distances > 0 if towards sea side (acretion) and < 0 if towards landside (erosion)"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProfile_Crossings::On_Execute(void)
{
	CSG_Shapes	*pLines_Sea			  = Parameters("LINES_SeaSide"     )->asShapes();
	CSG_Shapes  *pLines_Land		  = Parameters("LINES_LandSide"    )->asShapes();
	CSG_Shapes	*pLines_Coast		  = Parameters("LINES_Coast"       )->asShapes();
	CSG_Shapes	*pCrossings_SeaSide	  = Parameters("CROSSINGS_SEASIDE" )->asShapes();
	CSG_Shapes  *pCrossings_LandSide  = Parameters("CROSSINGS_LANDSIDE")->asShapes();
	CSG_Shapes  *pDistances           = Parameters("DISTANCES"         )->asShapes();

	if(	!pLines_Sea->is_Valid() || !pLines_Coast->is_Valid() || pLines_Sea->Get_Extent().Intersects(pLines_Coast->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no intersection at Sea Side"));

		return( false );
	}

	if( !pLines_Land->is_Valid() || !pLines_Coast->is_Valid() || pLines_Land->Get_Extent().Intersects(pLines_Coast->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no intersection at Land Side"));

		return(false);
	}

	//--------------------------------------------------------
	int	Attributes	= Parameters("ATTRIBUTES")->asInt();

	pCrossings_SeaSide ->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s - %s]", _TL("Crossings at Sea Side" ), pLines_Sea->Get_Name(), pLines_Coast->Get_Name()));
	pCrossings_LandSide->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s - %s]", _TL("Crossings at Land Side"), pLines_Land->Get_Name(), pLines_Coast->Get_Name()));
	pDistances         ->Create(SHAPE_TYPE_Point, CSG_String::Format("%s", _TL("Distances from profile start point")));

	if( Attributes == 0 || Attributes == 2 )
	{
		pCrossings_SeaSide ->Add_Field("ID_Profile", SG_DATATYPE_Int);
		pCrossings_SeaSide ->Add_Field("ID_Coast"  , SG_DATATYPE_Int);

		pCrossings_LandSide->Add_Field("ID_Profile", SG_DATATYPE_Int);
		pCrossings_LandSide->Add_Field("ID_Coast"  , SG_DATATYPE_Int);

		pDistances         ->Add_Field("ID_Profile", SG_DATATYPE_Int);
		pDistances         ->Add_Field("ID_Coast"  , SG_DATATYPE_Int);
		pDistances         ->Add_Field("Distance"  , SG_DATATYPE_Double);
	}

	if( Attributes == 1 || Attributes == 2 )
	{
		Add_Attributes(pCrossings_SeaSide , pLines_Sea  );
		Add_Attributes(pCrossings_SeaSide , pLines_Coast);

		Add_Attributes(pCrossings_LandSide, pLines_Land );
		Add_Attributes(pCrossings_LandSide, pLines_Coast);
	}

	//--------------------------------------------------------
	Get_Crossing(pLines_Sea , pLines_Coast, pCrossings_SeaSide , Attributes, pDistances,  1);
	Get_Crossing(pLines_Land, pLines_Coast, pCrossings_LandSide, Attributes, pDistances, -1);
	//--------------------------------------------------------

	return( pCrossings_SeaSide->Get_Count() > 0 || pCrossings_LandSide->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProfile_Crossings::Add_Attributes(CSG_Shapes *pCrossings, CSG_Shapes *pLines)
{
	for(int i=0; i<pLines->Get_Field_Count(); i++)
	{
		pCrossings->Add_Field(pLines->Get_Field_Name(i), pLines->Get_Field_Type(i));
	}

	return( true );
}

//---------------------------------------------------------
bool CProfile_Crossings::Set_Attributes(CSG_Shape *pCrossing, CSG_Shape *pLine, int &Offset)
{
	for(int i=0; i<pLine->Get_Table()->Get_Field_Count(); i++)
	{
		if( SG_Data_Type_is_Numeric(pLine->Get_Table()->Get_Field_Type(i)) )
		{
			pCrossing->Set_Value(Offset++, pLine->asDouble(i));
		}
		else
		{
			pCrossing->Set_Value(Offset++, pLine->asString(i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CProfile_Crossings::Set_Crossing(const TSG_Point &Crossing, CSG_Shape *pA, CSG_Shape *pB, CSG_Shape *pCrossing, int Attributes)
{
	int	Offset	= 0;

	if( Attributes == 0 || Attributes == 2 )
	{
		pCrossing->Set_Value(Offset++, pA->Get_Index());
		pCrossing->Set_Value(Offset++, pB->Get_Index());
	}

	if( Attributes == 1 || Attributes == 2 )
	{
		Set_Attributes(pCrossing, pA, Offset);
		Set_Attributes(pCrossing, pB, Offset);
	}

	pCrossing->Set_Point(Crossing, 0);

	return( true );
}

//---------------------------------------------------------
bool CProfile_Crossings::Set_Distance(const TSG_Point StartPoint, const TSG_Point Crossing, CSG_Shape* pA, CSG_Shape* pB, CSG_Shape *pDistances, double disSeaward)
{
	int	Offset = 0;

	pDistances->Set_Value(Offset++, pA->Get_Index());
	pDistances->Set_Value(Offset++, pB->Get_Index());

	double dDistance = SG_Get_Distance(StartPoint, Crossing);

	dDistance *= disSeaward;

	Set_Attributes(pDistances, dDistance, Offset);

	pDistances->Set_Point(Crossing, 0);

	return( true );
}

//---------------------------------------------------------
bool CProfile_Crossings::Set_Attributes(CSG_Shape *pDistances, double dDistance, int &Offset)
{
	pDistances->Set_Value(Offset++, dDistance);

	return( true );
}

//---------------------------------------------------------
bool CProfile_Crossings::Get_Crossing(CSG_Shapes *pLines_A, CSG_Shapes *pLines_B, CSG_Shapes *pCrossings_AB, int Attributes, CSG_Shapes *pDistances_AB, double disSeaward)
{
	for(int aLine=0, iPair=0, nPairs=pLines_A->Get_Count()*pLines_B->Get_Count() && Process_Get_Okay(); aLine<pLines_A->Get_Count(); aLine++)
	{
		CSG_Shape_Line	*pA	= (CSG_Shape_Line *)pLines_A->Get_Shape(aLine);
		
		TSG_Point	ProfileStartPoint = pA->Get_Point(0);

		for(int bLine=0; bLine<pLines_B->Get_Count() && Set_Progress(iPair++, nPairs); bLine++)
		{
			CSG_Shape_Line	*pB	= (CSG_Shape_Line *)pLines_B->Get_Shape(bLine);
			
			if( pA->Intersects(pB) )
			{
				for(int aPart=0; aPart<pA->Get_Part_Count(); aPart++)
				{
					TSG_Point	A[2];	A[1]	= pA->Get_Point(0, aPart);

					for(int aPoint=1; aPoint<pA->Get_Point_Count(aPart); aPoint++)
					{
						A[0]	= A[1];	A[1]	= pA->Get_Point(aPoint, aPart);

						for(int bPart=0; bPart<pB->Get_Part_Count(); bPart++)
						{
							TSG_Point	B[2], C;	B[1]	= pB->Get_Point(0, bPart);

							for(int bPoint=1; bPoint<pB->Get_Point_Count(bPart); bPoint++)
							{
								B[0]	= B[1];	B[1]	= pB->Get_Point(bPoint, bPart);

								if( SG_Get_Crossing(C, A[0], A[1], B[0], B[1]) )
								{
									Set_Crossing(C, pA, pB, pCrossings_AB->Add_Shape(), Attributes);
									Set_Distance(ProfileStartPoint, C, pA, pB, pDistances_AB->Add_Shape(), disSeaward);
								}
							}
						}
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
