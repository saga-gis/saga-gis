
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
//                 pretty_printer_tool.h                 //
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
#pragma once

//---------------------------------------------------------
#include "pretty_printer_tool.h"
#include <vector>
#include <array>
#include <string>
#include <tuple>


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPrettyPrinter::CPrettyPrinter(void)
{
	Set_Name 	(_TL("Pretty-Printer Tool"));

	Set_Author 	("J. Spitzm\u00FCller \u00A9 scilands GmbH 2024");

	Set_Description(_TW(
		"This tool is intended for test and debug purposes of the GDB pretty printer."
		"It created a bunch of C++ SAGA API Object one can inspect in GDB."
	));

	Parameters.Add_String(NULL, "STRING", _TL("String"), _TL(""), "This is a String Parameters");

	Parameters.Add_Color(NULL, "COLOR", _TL("Color"), _TL("") );

	Parameters.Add_Colors(NULL, "COLORS", _TL("Colors"), _TL("") );

	Parameters.Add_Date(NULL, "DATE", _TL("Date"), _TL("") );

	Parameters.Add_Bool(NULL, "BOOL", _TL("Bool"), _TL(""), true );

	Parameters.Add_Int(NULL, "INT", _TL("Int"), _TL(""), 23 );

	Parameters.Add_Int(NULL, "INT_MIN_MAX", _TL("Int Min Max"), _TL(""), 23, 0, true, 42, true );

	Parameters.Add_Double(NULL, "DOUBLE", _TL("Double"), _TL(""), 42.8 );

	Parameters.Add_Double(NULL, "DOUBLE_MIN_MAX", _TL("Double Min Max"), _TL(""), 42.8 );

	Parameters.Add_Range(NULL, "RANGE", _TL("Range"), _TL(""), 13.0, 874.123, -100., true, 1000., true);

	Parameters.Add_Choice(NULL, "CHOICE", _TL("Choice"), _TL(""),
		CSG_String::Format("%s|%s|%s|", _TL("Option 1"), _TL("Option 2"), _TL("Option 3")),
		0 
	);

	Parameters.Add_Choices("", "CHOICES", _TL("Choices"), _TL(""),
		CSG_String::Format("%s|%s|%s|", _TL("Option 1"), _TL("Option 2"), _TL("Option 3"))
	
	);

	Parameters.Add_Grid(NULL, "GRID", _TL("Grid"), _TL(""), PARAMETER_OPTIONAL );

	//Parameters.Add_Grids("", "GRIDS", _TL("Grids"), _TL(""), PARAMETER_OPTIONAL );

	Parameters.Add_Table(NULL, "TABLE", _TL("Table"), _TL(""), PARAMETER_OPTIONAL );

	Parameters.Add_Shapes(NULL, "SHAPES", _TL("Shapes"), _TL(""), PARAMETER_OPTIONAL );

	Parameters.Add_TIN(NULL, "TIN", _TL("TIN"), _TL(""), PARAMETER_OPTIONAL );



	Parameters.Add_PointCloud(NULL, "POINTCLOUD", _TL("Pointcloud"), _TL(""), PARAMETER_OPTIONAL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

bool CPrettyPrinter::On_Execute(void)
{
	std::vector<double> stl_vector;

	stl_vector.push_back(123.12);
	stl_vector.push_back(321.21);
	stl_vector.push_back(8341.21);
	stl_vector.push_back(3.1);
	stl_vector.push_back(1000000);
	stl_vector.push_back(8127912);

	std::array<double, 6> stl_array;

	stl_array[0] = 123.12;
	stl_array[1] = 321.21;
	stl_array[2] = 8341.21;
	stl_array[3] = 3.1;
	stl_array[4] = 1000000;
	stl_array[5] = 8127912;

	std::string stl_string = "test";

	std::vector<std::string> stl_strings;

	stl_strings.push_back("This");
	stl_strings.push_back("is");
	stl_strings.push_back("a");
	stl_strings.push_back("test");
	stl_strings.push_back("for");
	stl_strings.push_back("strings");


	std::tuple<std::string,std::string> stl_tuple("name", "Content");


	CSG_DateTime DateTime = Parameters("DATE")->asDate()->Get_Date();




	CSG_Array Array;

	Array.Set_Growth(TSG_Array_Growth::SG_ARRAY_GROWTH_1);
	Array.Inc_Array(2);
	void* value = Array[0];
	value = (void*) 1;
	value = Array[1];
	value = (void*) 2;

	CSG_Array_Int Array_Int;

	Array_Int.Set_Growth(TSG_Array_Growth::SG_ARRAY_GROWTH_1);
	Array_Int.Inc_Array(2);
	Array_Int[0] = 2;
	Array_Int[1] = 2241248;
	Array_Int.Add(1);
	Array_Int.Add(625189474);

	CSG_Strings Strings;

	Strings += "The First";
	Strings += "Second";
	Strings += "Third string";


	Message_Fmt("1:%s 2:%s 3:%s", Strings[0].c_str(), Strings.Get_String(1).c_str(), Strings[2].c_str());

	CSG_Rect Rect;

	CSG_Data_Object *pData_Object;

	CSG_String String = Parameters("STRING")->asString();

	CSG_Colors *pColors = Parameters("COLORS")->asColors();

	if( pColors ) Message_Add(_TL("Colors are set"));

	CSG_Grid_System *pGrid_System = Parameters("GRID")->asGrid_System();
	
	if( pGrid_System ) Message_Add(_TL("Grid system is set"));


	CSG_Grid *pGrid = Parameters("GRID")->asGrid();
	
	if( pGrid ) 
	{
		Message_Add(_TL("Grid is set"));

		CSG_Projection Projection = pGrid->Get_Projection();
	
		Message_Fmt("%s", Projection.Get_Authority().c_str() );

		CSG_MetaData MetaData = pGrid->Get_MetaData();

		CSG_String Name = MetaData.Get_Name();
		CSG_String Content = MetaData.Get_Content();

		CSG_Grid_System System = pGrid->Get_System();

		Message_Fmt("%d", System.Get_Cellsize() );

		CSG_String s = "";
		for( int i=0; i<MetaData.Get_Property_Count(); i++ )
		{
			s += MetaData.Get_Property_Name(i);
			s += ", ";
			s += MetaData.Get_Property(i);
		}

		CSG_Array_Pointer Array_Pointer;

		Array_Pointer.Set_Growth(TSG_Array_Growth::SG_ARRAY_GROWTH_3);
		Array_Pointer += NULL;
		Array_Pointer += pGrid;
		
		Message_Fmt("%s %s %s", Name.c_str(), Content.c_str(), s.c_str() );
	}

	//CSG_Grids *pGrids = Parameters("GRIDS")->asGrids();

	CSG_Table *pTable = Parameters("TABLE")->asTable();

	if( pTable )
	{
		Message_Add("Found Table");
	}

	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes();

	if( pShapes )
	{
		Message_Add("Found Shapes");

		Rect = pShapes->Get_Extent();
		pData_Object = Parameters("SHAPES")->asDataObject();

		Message_Fmt("%s", pData_Object->Get_Name() );

		if( pShapes->Get_Type() == SHAPE_TYPE_Point )
		{
			CSG_Shape_Point *pPoint= pShapes->Get_Shape(0)->asPoint();
			if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XY )
			{
				CSG_Shape_Point *pPoint= pShapes->Get_Shape(0)->asPoint();
				TSG_Point A = pPoint->Get_Point(0);

				double x = A.x;
				double y = A.y;

				Message_Fmt("%d", x + y );
			}

			if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZ )
			{
				CSG_Shape_Point *pPoint= pShapes->Get_Shape(0)->asPoint();
				TSG_Point_3D B = pPoint->Get_Point_Z(0);

				CSG_Shape_Point_Z *pPoint_Z= (CSG_Shape_Point_Z*) pShapes->Get_Shape(0)->asPoint();

				double x = B.x;
				double y = B.y;
				double z = pPoint_Z->Get_Point_Z(0).z;

				Message_Fmt("%d", x + y + z );
			}

			if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			{
				CSG_Shape_Point *pPoint= pShapes->Get_Shape(0)->asPoint();
				TSG_Point_4D C = pPoint->Get_Point_ZM(0);

				CSG_Shape_Point_ZM *pPoint_ZM = (CSG_Shape_Point_ZM*) pShapes->Get_Shape(0)->asPoint();

				double x = C.x;
				double y = C.y;
				double z = pPoint_ZM->Get_Point_ZM(0).z;
				double m = pPoint_ZM->Get_Point_ZM(0).m;

				Message_Fmt("%d", x + y + z + m );
			}

		}
		if( pShapes->Get_Type() == SHAPE_TYPE_Points )
		{
			//if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XY )
			//{
			//	CSG_Shape_Points *pPoints = pShapes->Get_Shape(0)->asPoints();
			//	TSG_Point A = pPoints->Get_Point(0);
			//}

			//if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZ )
			//{
			//	//CSG_Shape_Points_Z *pPoints = pShapes->Get_Shape(0)->asPoints();
			//	CSG_Shape_Points *pPoints = pShapes->Get_Shape(0)->asPoints();
			//	TSG_Point_3D B = (TSG_Point_3D) pPoints->Get_Point(0);

			//if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			//	TSG_Point_4D C = pPoints->Get_Point_ZM(0);
		}
		//if( pShapes->Get_Type() == SHAPE_TYPE_Line )
		//if( pShapes->Get_Type() == SHAPE_TYPE_Polygon )

	}

	CSG_TIN *pTIN = Parameters("TIN")->asTIN();

	CSG_PointCloud pPointCloud = Parameters("POINTCLOUD")->asPointCloud();

	

	return true;
}

