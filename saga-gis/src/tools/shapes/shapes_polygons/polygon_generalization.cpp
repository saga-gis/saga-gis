
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
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Generalization"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"A simple generalization tool for polygons. "
		"The tool joins polygons with an area size smaller than "
		"the specified threshold to their largest neighbouring polygon. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS", _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"GENERALIZED"	, _TL("Shape Indices"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double("",
		"THRESHOLD"	, _TL("Threshold"),
		_TL(""),
		100., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Generalization::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Generalization::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons = Parameters("POLYGONS")->asShapes();

	if( !pPolygons->is_Valid() )
	{
		Error_Set(_TL("invalid polygons layer"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("GENERALIZED")->asShapes() && Parameters("GENERALIZED")->asShapes() != pPolygons )
	{
		CSG_Shapes	*pTarget   = Parameters("GENERALIZED")->asShapes();

		pTarget->Create(*pPolygons);

		pTarget->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("generalized"));

		pPolygons	= pTarget;
	}

	//-----------------------------------------------------
	int	i = 0, n = pPolygons->Get_Count();

	do
	{
		Process_Set_Text(CSG_String::Format("%s %d", _TL("pass"), ++i));
	}
	while( Set_JoinTos(pPolygons) && Process_Get_Okay() );

	//-----------------------------------------------------
	if( pPolygons == Parameters("POLYGONS")->asShapes() )
	{	// output is always updated automatically - but if input has been modified, this needs a manual update!
		DataObject_Update(pPolygons);
	}

	n	-= pPolygons->Get_Count();

	Message_Fmt("\n%s: %d", _TL("total number of removed polygons"), n);

	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Generalization::Set_JoinTos(CSG_Shapes *pPolygons)
{
	CSG_Array_Int	JoinTo;

	if( !Get_JoinTos(pPolygons, JoinTo) )
	{
		return( false );
	}

	int	i, nDropped = 0, nRemoved = 0;

	for(i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		if( JoinTo[i] < 0 )
		{
			nDropped++;
		}
		else if( JoinTo[i] != i )
		{
			nRemoved++;

			CSG_Shape_Polygon	*pJoinTo	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(JoinTo[i]);
			CSG_Shape_Polygon	*pRemove	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(       i );

			for(int iPart=0; iPart<pRemove->Get_Part_Count(); iPart++)
			{
				pJoinTo->Add_Part(pRemove->Get_Part(iPart));
			}

			SG_Polygon_Dissolve(pJoinTo);
		}
	}

	for(i=pPolygons->Get_Count()-1; i>=0; i--)
	{
		if( JoinTo[i] >= 0 && JoinTo[i] != i )
		{
			pPolygons->Del_Shape(i);
		}
	}

	Message_Fmt("\n%s: %d; %s: %d", _TL("candidates"), nRemoved + nDropped, _TL("eliminated"), nRemoved);

	return( nDropped > 0 && nRemoved > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Generalization::Get_JoinTos(CSG_Shapes *pPolygons, CSG_Array_Int &JoinTo)
{
	double	Threshold	= Parameters("THRESHOLD")->asDouble();

	if( Threshold <= 0.0 || !JoinTo.Create(pPolygons->Get_Count()) )
	{
		return( false );
	}

	int	n	= 0;

	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(i);

		if( Threshold <= pPolygon->Get_Area() )
		{
			JoinTo[i]	= i;
		}
		else // if( Threshold > pPolygon->Get_Area() )
		{
			JoinTo[i]	= -1;

			double	maxArea	= Threshold;

			for(int j=0; j<pPolygons->Get_Count(); j++)
			{
				if( j != i )
				{
					CSG_Shape_Polygon	*pNeighbour	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(j);

					if( maxArea <= pNeighbour->Get_Area() && pPolygon->is_Neighbour(pNeighbour) )
					{
						maxArea		= pNeighbour->Get_Area();

						JoinTo[i]	= j;
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
