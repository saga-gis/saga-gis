
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygon                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               polygon_generalization.cpp              //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "polygon_generalization.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Generalization::CPolygon_Generalization(void)
{
	Set_Name		(_TL("Polygon Generalization"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"A simple generalization tool for polygons. "
		"The tool joins polygons with an area size smaller than "
		"the specified threshold to a neighbouring polygon. "
		"Either the neighbouring polygon with the largest area or "
		"the one with which the largest edge length is shared wins."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS", _TL("Shapes"),
		_TL("The input polygons."),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"GENERALIZED"	, _TL("Generalized Shapes"),
		_TL("The generalized output polygons."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double("",
		"THRESHOLD"	, _TL("Area Threshold"),
		_TL("The maximum area of a polygon to get joined [map units squared]."),
		100., 0., true
	);

	Parameters.Add_Choice("",
		"JOIN_TO"	, _TL("Join to Neighbour with ..."),
		_TL("Choose the method to determine the winner polygon."),
		CSG_String::Format("%s|%s",
			_TL("largest area"),
			_TL("largest shared edge length")
		), 0
	);

	Parameters.Add_Bool("",
		"VERTICES"	, _TL("Check Vertices"),
		_TL(""),
		false
	);

	Parameters.Add_Double("VERTICES",
		"EPSILON"	, _TL("Tolerance"),
		_TL(""),
		0.00001, 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Generalization::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("JOIN_TO") )
	{
		pParameters->Set_Enabled("VERTICES", pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("VERTICES") )
	{
		pParameters->Set_Enabled("EPSILON", pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Generalization::On_Execute(void)
{
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	if( !pPolygons->is_Valid() )
	{
		Error_Set(_TL("invalid polygons layer"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("GENERALIZED")->asShapes() && Parameters("GENERALIZED")->asShapes() != pPolygons )
	{
		CSG_Shapes *pTarget = Parameters("GENERALIZED")->asShapes();

		pTarget->Create(*pPolygons);

		pTarget->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("generalized"));

		pPolygons = pTarget;
	}

	//-----------------------------------------------------
	sLong i = 0, n = pPolygons->Get_Count();

	do
	{
		Process_Set_Text(CSG_String::Format("%s %lld", _TL("pass"), ++i));
	}
	while( Set_JoinTos(pPolygons) && Process_Get_Okay() );

	//-----------------------------------------------------
	if( pPolygons == Parameters("POLYGONS")->asShapes() )
	{	// output is always updated automatically - but if input has been modified, this needs a manual update!
		DataObject_Update(pPolygons);
	}

	n -= pPolygons->Get_Count();

	Message_Fmt("\n%s: %lld", _TL("total number of removed polygons"), n);

	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Generalization::Set_JoinTos(CSG_Shapes *pPolygons)
{
	CSG_Array_sLong JoinTo;

	if( !Get_JoinTos(pPolygons, JoinTo) )
	{
		return( false );
	}

	sLong nDropped = 0, nRemoved = 0;

	for(sLong i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		if( JoinTo[i] < 0 )
		{
			nDropped++;
		}
		else if( JoinTo[i] != i )
		{
			nRemoved++;

			CSG_Shape_Polygon *pJoinTo = (CSG_Shape_Polygon *)pPolygons->Get_Shape(JoinTo[i]);
			CSG_Shape_Polygon *pRemove = (CSG_Shape_Polygon *)pPolygons->Get_Shape(       i );

			for(int iPart=0; iPart<pRemove->Get_Part_Count(); iPart++)
			{
				pJoinTo->Add_Part(pRemove->Get_Part(iPart));
			}

			SG_Shape_Get_Dissolve(pJoinTo);
		}
	}

	for(sLong i=pPolygons->Get_Count()-1; i>=0; i--)
	{
		if( JoinTo[i] >= 0 && JoinTo[i] != i )
		{
			pPolygons->Del_Shape(i);
		}
	}

	Message_Fmt("\n%s: %lld; %s: %lld", _TL("candidates"), nRemoved + nDropped, _TL("eliminated"), nRemoved);

	return( nDropped > 0 && nRemoved > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Generalization::Get_JoinTos(CSG_Shapes *pPolygons, CSG_Array_sLong &JoinTo)
{
	double Threshold = Parameters("THRESHOLD")->asDouble();
	int       Method = Parameters("JOIN_TO")->asInt();
	bool   bVertices = Parameters("VERTICES")->asBool();
	double   Epsilon = Parameters("EPSILON")->asDouble();

	if( Threshold <= 0. || !JoinTo.Create(pPolygons->Get_Count()) )
	{
		return( false );
	}

	sLong n = 0;

	for(sLong i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon *pPolygon = (CSG_Shape_Polygon *)pPolygons->Get_Shape(i);

		if( Threshold <= pPolygon->Get_Area() )
		{
			JoinTo[i] = i;
		}
		else // if( Threshold > pPolygon->Get_Area() )
		{
			JoinTo[i] = -1;

			double maxValue = Method == 0 ? Threshold : 0.;

			for(sLong j=0; j<pPolygons->Get_Count(); j++)
			{
				if( j != i )
				{
					CSG_Shape_Polygon *pNeighbour = (CSG_Shape_Polygon *)pPolygons->Get_Shape(j);

					if( Method == 0 ) // largest area
					{
						if( maxValue <= pNeighbour->Get_Area() && pPolygon->is_Neighbour(pNeighbour) )
						{
							maxValue   = pNeighbour->Get_Area();

							JoinTo[i] = j;
						}
					}
					else if( pPolygon->is_Neighbour(pNeighbour) ) // largest shared edge length
					{
						double sharedLength = pPolygon->Get_Shared_Length(pNeighbour, bVertices, Epsilon);
						
						if( sharedLength > maxValue )
						{
							maxValue  = sharedLength;

							JoinTo[i] = j;
						}
					}
				}
			}

			n++;
		}
	}

	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
