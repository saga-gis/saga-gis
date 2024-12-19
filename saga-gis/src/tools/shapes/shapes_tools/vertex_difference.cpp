
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 vertex_difference.cpp                 //
//                                                       //
//                 Copyright (C) 2024 by                 //
//                  Justus Spitzmueller                  //
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
//    e-mail:     jspitzmueller@scilands.de              //
//                                                       //
//    contact:    Justus Spitzmueller                    //
//                scilands GmbH                          //
//                Goethe-Allee 11                        //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "vertex_difference.h"
#include <vector>
#include <algorithm>
#include <set>
#include <functional>

///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVertexDifference::CVertexDifference(void)
{
	Set_Name 	(_TL("Vertex Difference"));

	Set_Author 	("J. Spitzm\u00FCller \u00A9 scilands GmbH 2024");

	Set_Version ("0.9");

	Set_Description(_TW(
		"This tool identifies differences between two vector datasets and outputs the vertices that are "
		"present in either of the datasets but not in both." ));

	Parameters.Add_Shapes(NULL, "IN_A", _TL("Layer A"), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Shapes(NULL, "IN_B", _TL("Layer B"), _TL(""), PARAMETER_INPUT );
	
	Parameters.Add_Shapes(NULL, "OUT", _TL("Difference"), 	_TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Point );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVertexDifference::On_Execute(void)
{
	CSG_Shapes *pA 		= Parameters("IN_A")->asShapes();
	CSG_Shapes *pB 		= Parameters("IN_B")->asShapes();
	CSG_Shapes *pOut 	= Parameters("OUT" )->asShapes();
	
	// TSG_Point Compare Lambda for std::set and std::set_symmetric_difference
	auto ComparePoints = [](const TSG_Point& A, const TSG_Point& B) 
		{if( A.x == B.x ) { return A.y < B.y; } return A.x < B.x; };

	// std::function is used to also use the Lambda above in std::set and std::set_symmetric_difference
	std::set<TSG_Point, std::function<bool(const TSG_Point&, const TSG_Point&)>> Points_A(ComparePoints);
	std::set<TSG_Point, std::function<bool(const TSG_Point&, const TSG_Point&)>> Points_B(ComparePoints);
	
	// Add all points of layer A and B into a std::set (sorted and unique points)
	Process_Set_Text(_TL("Preparing Layer A"));
	for( sLong i=0; i<pA->Get_Count() && Set_Progress(i,pA->Get_Count()); i++ )
	{
	 	CSG_Shape *pShape = pA->Get_Shape(i);
		
		for( int j=0; j<pShape->Get_Point_Count(); j++ )
		{
			Points_A.insert(pShape->Get_Point(j));
		}
	}

	Process_Set_Text(_TL("Preparing Layer B"));
	for( sLong i=0; i<pB->Get_Count() && Set_Progress(i,pB->Get_Count()); i++ )
	{
	 	CSG_Shape *pShape = pB->Get_Shape(i);
		
		for( int j=0; j<pShape->Get_Point_Count(); j++ )
		{
			Points_B.insert(pShape->Get_Point(j));
		}
	}

	// Use std::set_symmetric_difference to find points present in either but not both shapes.
	// Use the ComparePoints-Lambda for point comparison.
	Process_Set_Text(_TL("Calculate Difference"));
	std::vector<TSG_Point> Result;
   	std::set_symmetric_difference(	Points_A.begin(), Points_A.end(),
                                	Points_B.begin(), Points_B.end(),
                                  	std::back_inserter(Result), ComparePoints);

	// Copy the all points in the result vector into the output shapefile
	Process_Set_Text(_TL("Create Output"));
	for( TSG_Point Point : Result )
	{
		CSG_Shape *pNew = pOut->Add_Shape();
		if( pNew ) pNew->Add_Point(Point);
	}

	return true;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////
