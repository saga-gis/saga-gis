
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
//                  vertex_inspector.cpp                 //
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
#include "vertex_inspector.h"


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVertexInspector::CVertexInspector(void)
{
	Set_Name 	(_TL("Vertex Inspector"));

	Set_Author 	(SG_T("J. Spitzm\u00FCller \u00A9 scilands GmbH 2024"));

	Set_Version ("1.3");

	Set_Description(_TW(
		"This interactive tool is designed to inspect and manipulate individual vertices of geometries. "
 		"It allows users to select vertices on the map using a drag box. All vertices within the "
		"drag box are then sorted hierarchically by dataset, shape, and part, and the X, Y, Z and M "
		"(if available) values are displayed. The tool offers two "
		"access modes: read only and mutable. Read only offers a higher precision as the coordinates "
		"are rounded differently. Mutable changes the vertices in the input and creates no output. "
		"While the tool is running, the selected point is indicated by a rhombus on the current map"));


	Parameters.Add_Shapes_List(NULL, "SHAPES", _TL("Shapes"), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Choice("", "READ_ONLY", _TL("Access Mode"), _TL(""), 
		CSG_String::Format("%s|%s|", _TL("Read Only"), _TL("Mutable")),0 );

	Parameters.Add_Bool("READ_ONLY", "CONFIRM", _TL("Confirm Changes"), _TL(""), true );

	CSG_Parameters *pParameters = Add_Parameters("RESULT", _TL(""), _TL(""));
	pParameters->Add_Choice("DATA", "DATASET", _TL("Dataset"), _TL(""), ""); 
	pParameters->Add_Choice( "DATASET", "SHAPE", 	_TL("Shapes"), 	_TL(""), ""); 
	pParameters->Add_Choice( "SHAPE", 	"PART", 	_TL("Parts"), 	_TL(""), ""); 
	pParameters->Add_Choice( "PART", 	"POINTS", 	_TL("Points"), 	_TL(""), ""); 

	pParameters->Add_Node("", "POINT_RO", _TL("Vertex (Read Only)"), _TL("") ); 
	pParameters->Add_Info_String("POINT_RO", "XI", _TL("X"), _TL(""), "");
	pParameters->Add_Info_String("POINT_RO", "YI", _TL("Y"), _TL(""), "");
	pParameters->Add_Info_String("POINT_RO", "ZI", _TL("Z"), _TL(""), "");
	pParameters->Add_Info_String("POINT_RO", "MI", _TL("M"), _TL(""), "");

	pParameters->Add_Node("", "POINT_W", _TL("Vertex (Mutable)"), _TL("") ); 
	pParameters->Add_Double("POINT_W", "XD", _TL("X"), _TL(""), 0.0);
	pParameters->Add_Double("POINT_W", "YD", _TL("Y"), _TL(""), 0.0);
	pParameters->Add_Double("POINT_W", "ZD", _TL("Z"), _TL(""), 0.0);
	pParameters->Add_Double("POINT_W", "MD", _TL("M"), _TL(""), 0.0);

	pParameters->Add_Node("", "TABLE_NODE", _TL("All Selected Vertices as Table"), "" );
	m_pTable = pParameters->Add_FixedTable("TABLE_NODE", "TABLE", _TL("Table"), "")->asTable();
	m_pTable->Add_Field("Dataset",	SG_DATATYPE_Int);
	m_pTable->Add_Field("Shape", 	SG_DATATYPE_Int);
	m_pTable->Add_Field("Part", 	SG_DATATYPE_Int);
	m_pTable->Add_Field("Point", 	SG_DATATYPE_Int);
	m_pTable->Add_Field("X", 		SG_DATATYPE_String);
	m_pTable->Add_Field("Y", 		SG_DATATYPE_String);
	m_pTable->Add_Field("Z", 		SG_DATATYPE_String);
	m_pTable->Add_Field("M", 		SG_DATATYPE_String);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

bool CVertexInspector::On_Execute_Finish(void)
{
	if( SG_UI_DataObject_Del(m_pIndicator, false) )
	{
		m_pIndicator = NULL;
	}

	return true;
}

bool CVertexInspector::On_Execute(void)
{
	m_bDown	= false;
	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_BOX);

	m_pList 	= Parameters("SHAPES")->asShapesList();
	m_bConfirm 	= Parameters("CONFIRM")->asBool();

	Get_Parameters("RESULT")->Set_Enabled("POINT_RO", Parameters("READ_ONLY")->asInt() == 0 );
	Get_Parameters("RESULT")->Set_Enabled("POINT_W",  Parameters("READ_ONLY")->asInt() == 1 );

	m_pIndicator = SG_Create_Shapes(SHAPE_TYPE_Point, NULL, NULL, SG_VERTEX_TYPE_XY);
	m_pIndicator->Add_Shape()->Add_Point(0.0,0.0);
	DataObject_Add(m_pIndicator);

	CSG_Parameters DataObjectUIParameter;
	DataObject_Get_Parameters(m_pIndicator, DataObjectUIParameter );
	DataObjectUIParameter.Set_Parameter("DISPLAY_BRUSH", "Transparent");
	DataObjectUIParameter.Set_Parameter("DISPLAY_SYMBOL_TYPE", "rhombus");
	DataObjectUIParameter.Set_Parameter("SIZE_DEFAULT", 7.5);
	DataObject_Set_Parameters(m_pIndicator, DataObjectUIParameter );

	DataObject_Update(m_pIndicator, SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);

	return true;
}

bool CVertexInspector::Handle_Mutable_Point()
{
	if( m_ptOld.x == m_ptNew.x && m_ptOld.y == m_ptNew.y
	&&	m_ptOld.z == m_ptNew.z && m_ptOld.m == m_ptNew.m )
	{
		return true;
	}

	bool Set = true;
	if( m_bConfirm )
	{
		Set = Message_Dlg_Confirm(_TL("Save changes?"), _TL("Please Confirm"));
	}

	if( Set )
	{
		CSG_Shapes 	*pShapes 	= m_LastPoint.shapes;
		CSG_Shape 	*pShape 	= m_LastPoint.shape;
		sLong 		Part 		= m_LastPoint.part;
		sLong 		Point 		= m_LastPoint.point;

		double x = m_ptNew.x;
		double y = m_ptNew.y;
		pShape->Set_Point( x, y, Point, Part);

		if( pShapes->Get_Vertex_Type() > SG_VERTEX_TYPE_XY )
		{
			pShape->Set_Z(m_ptNew.z, Point, Part);
		}
		
		if( pShapes->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
		{
			pShape->Set_M(m_ptNew.m, Point, Part);
		}
	}

	m_ptOld = m_ptNew;

	return true;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

int CVertexInspector::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameters->Get_Parameter("CONFIRM") && pParameters->Get_Parameter("READ_ONLY") )
	{
		pParameters->Set_Enabled("CONFIRM", pParameters->Get_Parameter("READ_ONLY")->asInt() == 1 );
	}
	
	if( pParameters->Get_Parameter("TABLE") && pParameters->Get_Parameter("READ_ONLY") )
	{
		pParameters->Set_Enabled("TABLE", pParameters->Get_Parameter("READ_ONLY")->asInt() == 0 );
	}

	if( pParameters->Cmp_Identifier("RESULT") )
	{
		if( pParameter->Cmp_Identifier("XD") )
		{
			m_ptNew.x = pParameter->asDouble();
		}
		if( pParameter->Cmp_Identifier("YD") )
		{
			m_ptNew.y = pParameter->asDouble();
		}
		if( pParameter->Cmp_Identifier("ZD") )
		{
			m_ptNew.z = pParameter->asDouble();
		}
		if( pParameter->Cmp_Identifier("MD") )
		{
			m_ptNew.m = pParameter->asDouble();
		}

		if( pParameter->Cmp_Identifier("POINTS") )
		{
			int Dataset = -1, Shape = -1, Part = -1;
			
			pParameters->Get_Parameter("DATASET")->asChoice()->Get_Data(Dataset);
			pParameters->Get_Parameter("SHAPE")->asChoice()->Get_Data(Shape);
			pParameters->Get_Parameter("PART")->asChoice()->Get_Data(Part);

			Index Obj = m_Map.at(Dataset).at(Shape).at(Part).at(pParameter->asInt());

			CSG_Shape *pShape = Obj.shape;
			sLong part = Obj.part;
			sLong point = Obj.point;

			TSG_Vertex_Type Dims = pShape->Get_Vertex_Type();

			// Move and update the indicator
			m_pIndicator->Get_Shape(0)->Set_Point(pShape->Get_Point(point,part));
			DataObject_Update(m_pIndicator, SG_UI_DATAOBJECT_UPDATE);

			if( Parameters("READ_ONLY")->asInt() == 0)
			{
				pParameters->Get_Parameter("XI")->Set_Value( CSG_String::Format( "%.16f", pShape->Get_Point(point,part).x) );
				pParameters->Get_Parameter("YI")->Set_Value( CSG_String::Format( "%.16f", pShape->Get_Point(point,part).y) );
				
				if( Dims == SG_VERTEX_TYPE_XYZ || Dims == SG_VERTEX_TYPE_XYZM )
				{
					pParameters->Get_Parameter("ZI")->Set_Value( CSG_String::Format( "%.16f", pShape->Get_Point_Z(point,part).z) );
				}

				if( Dims == SG_VERTEX_TYPE_XYZM )
				{
					pParameters->Get_Parameter("MI")->Set_Value( CSG_String::Format( "%.16f", pShape->Get_Point_ZM(point,part).m) );
				}

				pParameters->Set_Enabled("ZI", Dims >= SG_VERTEX_TYPE_XYZ  );
				pParameters->Set_Enabled("MI", Dims == SG_VERTEX_TYPE_XYZM );
			}
			if( Parameters("READ_ONLY")->asInt() == 1)
			{
				Handle_Mutable_Point();

				m_LastPoint = { m_pList->Get_Item(Dataset)->asShapes(), pShape, part, point };

				double x = pShape->Get_Point(point, part).x;
				double y = pShape->Get_Point(point, part).y;
				m_ptOld = m_ptNew = { x, y, 0.0, 0.0};

				pParameters->Get_Parameter("XD")->Set_Value( x );
				pParameters->Get_Parameter("YD")->Set_Value( y );

				if( Dims == SG_VERTEX_TYPE_XYZ || Dims == SG_VERTEX_TYPE_XYZM )
				{
					double z = pShape->Get_Point_Z(point,part).z;
					pParameters->Get_Parameter("ZD")->Set_Value( z );
					m_ptOld.z = m_ptNew.z = z;
				}

				if( Dims == SG_VERTEX_TYPE_XYZM )
				{
					double m = pShape->Get_Point_ZM(point,part).m;
					pParameters->Get_Parameter("MD")->Set_Value( m );
					m_ptOld.m = m_ptNew.m = m;
				}

				pParameters->Set_Enabled("ZD", Dims >= SG_VERTEX_TYPE_XYZ  );
				pParameters->Set_Enabled("MD", Dims == SG_VERTEX_TYPE_XYZM );
			}
		}

		if( pParameter->Cmp_Identifier("DATASET") )
		{
			int Dataset = -1;	
			pParameters->Get_Parameter("DATASET")->asChoice()->Get_Data(Dataset);
			
			std::map<sLong, std::map<sLong, std::vector<Index>>> Map = m_Map.at(Dataset);

			CSG_Parameter_Choice *pChoice = pParameters->Get_Parameter("SHAPE")->asChoice();
			pChoice->Del_Items();
			int count = 1;
			int max = Map.size();
			for( auto& Pair : Map )
			{
				pChoice->Add_Item(CSG_String::Format("%d/%d %s %d", count, max, _TL("Shape"), Pair.first), CSG_String::Format("%d", Pair.first));
				count++;
			}

			pChoice->Set_Value(0);
			On_Parameter_Changed(pParameters, pParameters->Get_Parameter("SHAPE"));
		}

		if( pParameter->Cmp_Identifier("SHAPE") )
		{
			int Dataset = -1, Shape = -1;
			pParameters->Get_Parameter("DATASET")->asChoice()->Get_Data(Dataset);
			pParameters->Get_Parameter("SHAPE")->asChoice()->Get_Data(Shape);

			std::map<sLong, std::vector<Index>> Map = m_Map.at(Dataset).at(Shape);

			CSG_Shapes *pShapes = m_pList->Get_Item(Dataset)->asShapes();
			TSG_Shape_Type Type = pShapes->Get_Type();

			CSG_Parameter_Choice *pChoice = pParameters->Get_Parameter("PART")->asChoice();
			pChoice->Del_Items();
			int count = 1;
			int max = Map.size();
			for( auto& pair : Map )
			{
				CSG_String Clockwise_Lake_Info = "";

				if( Type == SHAPE_TYPE_Polygon )
				{
					if( pShapes->Get_Shape(Shape)->asPolygon()->is_Clockwise(pair.first) )
					{
						Clockwise_Lake_Info += "Clockwise";
					}

					if( pShapes->Get_Shape(Shape)->asPolygon()->is_Lake(pair.first) )
					{
						Clockwise_Lake_Info += "Lake";
					}
				}

				if( !Clockwise_Lake_Info.is_Empty() )
				{
					Clockwise_Lake_Info.Prepend(" (");
					Clockwise_Lake_Info.Append(")");
				}

				pChoice->Add_Item(CSG_String::Format("%d/%d %s %d%s", count, max, _TL("Part"), pair.first, Clockwise_Lake_Info.c_str()), CSG_String::Format("%d", pair.first ));
				count++;
			}

			pChoice->Set_Value(0);
			On_Parameter_Changed(pParameters, pParameters->Get_Parameter("PART"));
		}

		if( pParameter->Cmp_Identifier("PART") )
		{
			int Dataset = -1, Shape = -1, Part = -1;
			
			pParameters->Get_Parameter("DATASET")->asChoice()->Get_Data(Dataset);
			pParameters->Get_Parameter("SHAPE")->asChoice()->Get_Data(Shape);
			pParameters->Get_Parameter("PART")->asChoice()->Get_Data(Part);
			std::vector<Index> Vec = m_Map.at(Dataset).at(Shape).at(Part);

			int count = 1;
			int max = Vec.size();

			CSG_Parameter_Choice *pChoice = pParameters->Get_Parameter("POINTS")->asChoice();
			pChoice->Del_Items();
			for( auto& tuple : Vec )
			{
				CSG_String End = "";
				CSG_Shape *pShape = tuple.shape;
				if( pShape->Get_Type() == SHAPE_TYPE_Polygon )
				{
					if( tuple.point == 0 )
					{
						End = " (Start)";
					}

					if( tuple.point == pShape->Get_Point_Count(Part)-1 )
					{
						End = " (End)";
					}

				}
				pChoice->Add_Item(CSG_String::Format("%d/%d %s %d%s", count, max, _TL("Point"), tuple.point, End.c_str()), CSG_String::Format("%d", tuple.point ));
				count++;
			}

			pParameters->Get_Parameter("POINTS")->Set_Value(0);
			On_Parameter_Changed(pParameters, pParameters->Get_Parameter("POINTS"));
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

bool CVertexInspector::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
		default: break;

		case TOOL_INTERACTIVE_LDOWN:
			if( !m_bDown )
			{
				m_bDown		= true;
				m_ptDown	= ptWorld;
			}
		break;

		case TOOL_INTERACTIVE_LUP:
			if( m_bDown )
			{
				m_bDown		= false;
				CSG_Rect Drag_Box= CSG_Rect(m_ptDown, ptWorld);

				Select_from_Drag_Box( Drag_Box );
			}
		break;
	}
	return true;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

bool CVertexInspector::Select_from_Drag_Box( CSG_Rect Drag_Box )
{
	m_Map.clear();
	
	for( sLong list=0; list<m_pList->Get_Item_Count(); list++ )
	{
	 	CSG_Shapes *pShapes = m_pList->Get_Item(list)->asShapes();
		for( sLong shape=0; shape<pShapes->Get_Count(); shape++ )
		{
	 		CSG_Shape *pShape = pShapes->Get_Shape(shape);
			for( sLong part=0; part<pShape->Get_Part_Count(); part++ )
			{
				for( sLong point=0; point<pShape->Get_Point_Count(part); point++ )
				{
					if( Drag_Box.Contains(pShape->Get_Point(point,part)) )
					{
						// Insert elements witch the operator[] 
						m_Map[list][shape][part].push_back({NULL, pShape, part, point});

					}
				}
			}
		}
	}

	if( m_Map.size() == 0 )
	{
		return false;
	}

	CSG_Parameters 	*pParameters = Get_Parameters("RESULT");
	CSG_Parameter 	*pParameter  = pParameters->Get_Parameter("DATASET");
	CSG_Parameter_Choice *pChoice = pParameter->asChoice();
	pChoice->Del_Items();
	int Count = 1;
	int Max = m_Map.size();

	// std::map uses a std::pair to store keys and values
	// auto& pair is a std::pair
	for( auto& pair : m_Map )
	{
		CSG_Shapes *pShapes = m_pList->Get_Item(pair.first)->asShapes();
		CSG_String Type = "";
		switch(pShapes->Get_Type())
	 	{
			default: 					Type = "Undefined"; break;
	 		case SHAPE_TYPE_Point: 		Type = "Point"; 	break;
	 		case SHAPE_TYPE_Points: 	Type = "Points"; 	break;
	 		case SHAPE_TYPE_Line: 		Type = "Line"; 		break;
	 		case SHAPE_TYPE_Polygon:	Type = "Polygon"; 	break;
		}

		// "1/2 Dataset 0 "Perimeter_Buffer" (Polygon)"
		pChoice->Add_Item(CSG_String::Format("%d/%d %s %d: \"%s\" (%s)", Count, Max, _TL("Dataset"), pair.first, pShapes->Get_Name(), Type.c_str()), CSG_String::Format("%d", pair.first));
		Count++;
	}

	m_pTable->Del_Records();
	for( auto& dataset : m_Map )
	{
		std::map<sLong, std::map<sLong, std::vector<Index>>> shapes = dataset.second;
		for( auto& shape : shapes )
		{
			std::map<sLong, std::vector<Index>> parts = shape.second;
			for( auto& part : parts )
			{
				std::vector<Index> points = part.second;
				for( Index point : points )
				{
					TSG_Point Point = point.shape->Get_Point(point.point, point.part);
					TSG_Vertex_Type Dims = point.shape->Get_Vertex_Type();

					CSG_Table_Record *pRecord = m_pTable->Add_Record();
					pRecord->Set_Value( 0, dataset.first );
					pRecord->Set_Value( 1, shape.first );
					pRecord->Set_Value( 2, part.first );
					pRecord->Set_Value( 3, point.point );
					pRecord->Set_Value( 4, CSG_String::Format( "%.16f", Point.x ));
					pRecord->Set_Value( 5, CSG_String::Format( "%.16f", Point.y ));
				
					if( Dims == SG_VERTEX_TYPE_XYZ || Dims == SG_VERTEX_TYPE_XYZM )
					{
						pRecord->Set_Value( 6 , CSG_String::Format( "%.16f", point.shape->Get_Point_Z(point.point, point.part).z));
					}
					else
					{
						pRecord->Set_Value( 6 , "-" );
					}

					if( Dims == SG_VERTEX_TYPE_XYZM )
					{
						pRecord->Set_Value( 7 , CSG_String::Format( "%.16f", point.shape->Get_Point_ZM(point.point, point.part).m));
					}
					else
					{
						pRecord->Set_Value( 7 , "-" );
					}
				}
			}
		}
	}
	
	// The Set_Value will not always trigger a callback to the On_Parameter_Changed 
	// if the content is not changed. This happens in a reselection on the same datasets
	// So triggering the callback manually
	pParameter->Set_Value(0);
	On_Parameter_Changed( pParameters, pParameter );
	
	if( Dlg_Parameters(pParameters) )
	{
		Handle_Mutable_Point();
	}

	return true;
}

///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

