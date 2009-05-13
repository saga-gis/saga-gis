
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     io_shapes_dxf                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     dxf_import.cpp                    //
//                                                       //
//                 Copyright (C) 2007 by                 //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "dxf_import.h"

#include "./dxflib/dl_dxf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_RESULT(list, result)	if( result->is_Valid() && result->Get_Count() > 0 ) Parameters(list)->asList()->Add_Item(result); else delete(result);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	TBL_LAYERS_NAME		= 0,
	TBL_LAYERS_FLAGS
};

enum
{
	TBL_BLOCKS_NAME		= 0,
	TBL_BLOCKS_FLAGS,
	TBL_BLOCKS_X,
	TBL_BLOCKS_Y,
	TBL_BLOCKS_Z
};

enum
{
	TBL_POINTS_LAYER	= 0,
	TBL_POINTS_Z
};

enum
{
	TBL_LINES_LAYER		= 0,
	TBL_LINES_Z1,
	TBL_LINES_Z2
};

enum
{
	TBL_POLYOBJ_LAYER	= 0,
	TBL_POLYOBJ_FLAGS
};

enum
{
	TBL_TRIANGLE_LAYER	= 0,
	TBL_TRIANGLE_THICK,
	TBL_TRIANGLE_Z1,
	TBL_TRIANGLE_Z2,
	TBL_TRIANGLE_Z3
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDXF_Import::CDXF_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import DXF Files"));

	Set_Author		(SG_T("(c) 2007 by O.Conrad"));

	Set_Description	(_TW(
		"This module imports DXF files using the free \"dxflib\" library. Get more information "
		"about this library from the RibbonSoft homepage at:\n"
		"<a href=\"http://www.ribbonsoft.com/dxflib.html\">http://www.ribbonsoft.com/dxflib.html</a>"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes_List(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL(""),
		_TL("DXF Files (*.dxf)|*.dxf|All Files|*.*")
	);

	Parameters.Add_Choice(
		NULL	, "FILTER"		, _TL("Import Filter"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("all entities"),
			_TL("only entities with layer definition"),
			_TL("only entities without layer definition")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "DCIRCLE"		, _TL("Circle Point Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 5.0, 0.01, true, 45.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDXF_Import::On_Execute(void)
{
	CSG_String	fName	= Parameters("FILE")->asString();

	Parameters("TABLES")->asTableList() ->Del_Items();
	Parameters("SHAPES")->asShapesList()->Del_Items();

	m_Filter	= Parameters("FILTER")	->asInt();
	m_dArc		= Parameters("DCIRCLE")	->asDouble() * M_DEG_TO_RAD;

	//-----------------------------------------------------
	if( SG_File_Exists(fName) )
	{
		m_pLayers		= SG_Create_Table();
		m_pLayers		->Set_Name(CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Layers")));
		m_pLayers		->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pLayers		->Add_Field("FLAGS"	, TABLE_FIELDTYPE_Int);

		m_pBlocks		= SG_Create_Table();
		m_pBlocks		->Set_Name(CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Blocks")));
		m_pBlocks		->Add_Field("BLOCK"	, TABLE_FIELDTYPE_String);
		m_pBlocks		->Add_Field("FLAGS"	, TABLE_FIELDTYPE_Int);
		m_pBlocks		->Add_Field("X"		, TABLE_FIELDTYPE_Double);
		m_pBlocks		->Add_Field("Y"		, TABLE_FIELDTYPE_Double);
		m_pBlocks		->Add_Field("Z"		, TABLE_FIELDTYPE_Double);

		m_pPoints		= SG_Create_Shapes(SHAPE_TYPE_Point		, CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Points")));
		m_pPoints		->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pPoints		->Add_Field("Z"		, TABLE_FIELDTYPE_Double);

		m_pLines		= SG_Create_Shapes(SHAPE_TYPE_Line		, CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Lines")));
		m_pLines		->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pLines		->Add_Field("Z1"	, TABLE_FIELDTYPE_Double);
		m_pLines		->Add_Field("Z2"	, TABLE_FIELDTYPE_Double);

		m_pPolyLines	= SG_Create_Shapes(SHAPE_TYPE_Line		, CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Polylines")));
		m_pPolyLines	->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pPolyLines	->Add_Field("FLAGS"	, TABLE_FIELDTYPE_Int);

		m_pPolygons		= SG_Create_Shapes(SHAPE_TYPE_Polygon	, CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Polygons")));
		m_pPolygons		->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pPolygons		->Add_Field("FLAGS"	, TABLE_FIELDTYPE_Int);

		m_pCircles		= SG_Create_Shapes(SHAPE_TYPE_Line		, CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Circles")));
		m_pCircles		->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pCircles		->Add_Field("FLAGS"	, TABLE_FIELDTYPE_Int);

		m_pTriangles	= SG_Create_Shapes(SHAPE_TYPE_Polygon	, CSG_String::Format(SG_T("%s [%s]"), SG_File_Get_Name(fName, false).c_str(), _TL("Triangles")));
		m_pTriangles	->Add_Field("LAYER"	, TABLE_FIELDTYPE_String);
		m_pTriangles	->Add_Field("THICK"	, TABLE_FIELDTYPE_Int);
		m_pTriangles	->Add_Field("Z1"	, TABLE_FIELDTYPE_Double);
		m_pTriangles	->Add_Field("Z2"	, TABLE_FIELDTYPE_Double);
		m_pTriangles	->Add_Field("Z3"	, TABLE_FIELDTYPE_Double);

		//-------------------------------------------------
		m_Offset.x		= 0.0;
		m_Offset.y		= 0.0;
		m_Offset.z		= 0.0;

		m_pPolyLine		= NULL;

		DL_Dxf	*pDXF	= new DL_Dxf();

		pDXF->in(fName.b_str(), this);

		delete(pDXF);

		//-------------------------------------------------
		ADD_RESULT("TABLES", m_pLayers);
		ADD_RESULT("TABLES", m_pBlocks);
		ADD_RESULT("SHAPES", m_pPoints);
		ADD_RESULT("SHAPES", m_pLines);
		ADD_RESULT("SHAPES", m_pPolyLines);
		ADD_RESULT("SHAPES", m_pPolygons);
		ADD_RESULT("SHAPES", m_pCircles);
		ADD_RESULT("SHAPES", m_pTriangles);
	}

	//-----------------------------------------------------
	return( Parameters("SHAPES")->asShapesList()->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CDXF_Import::Check_Process(void)
{
	static int	iProcess	= 0;

	if( (iProcess++) % 100 == 0 )
	{
		Process_Get_Okay();
	}
}

//---------------------------------------------------------
inline bool CDXF_Import::Check_Layer(const CSG_String &Name)
{
	Check_Process();

	switch( m_Filter )
	{
	case 1:	return( Name.Cmp(SG_T("0")) != 0 );
	case 2:	return( Name.Cmp(SG_T("0")) == 0 );
	}

	return( true );
}

//---------------------------------------------------------
inline void CDXF_Import::Add_Arc_Point(CSG_Shape *pShape, double cx, double cy, double d, double theta)
{
	pShape->Add_Point(
		cx + d * cos(theta),
		cy + d * sin(theta)
	);
}

//---------------------------------------------------------
void CDXF_Import::Add_Arc(CSG_Shape *pShape, double cx, double cy, double d, double alpha, double beta)
{
	double	theta;

	alpha	*= M_DEG_TO_RAD;
	beta	*= M_DEG_TO_RAD;

	if( alpha > beta )
	{
		beta	+= M_PI_360;
	}

	for(theta=alpha; theta<beta; theta+=m_dArc)
	{
		Add_Arc_Point(pShape, cx, cy, d, theta);
	}

	Add_Arc_Point(pShape, cx, cy, d, beta);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDXF_Import::addLayer(const DL_LayerData &data)
{
	CSG_Table_Record	*pRecord	= m_pLayers->Add_Record();

	pRecord->Set_Value(TBL_LAYERS_NAME	, CSG_String(data.name.c_str()));
	pRecord->Set_Value(TBL_LAYERS_FLAGS	, data.flags);
}

//---------------------------------------------------------
void CDXF_Import::addBlock(const DL_BlockData &data)
{
	CSG_Table_Record	*pRecord	= m_pBlocks->Add_Record();

	pRecord->Set_Value(TBL_BLOCKS_NAME	, CSG_String(data.name.c_str()));
	pRecord->Set_Value(TBL_BLOCKS_FLAGS	, data.flags);
	pRecord->Set_Value(TBL_BLOCKS_X		, data.bpx);
	pRecord->Set_Value(TBL_BLOCKS_Y		, data.bpy);
	pRecord->Set_Value(TBL_BLOCKS_Z		, data.bpz);
}

//---------------------------------------------------------
void CDXF_Import::endBlock(void)
{
}

//---------------------------------------------------------
void CDXF_Import::addPoint(const DL_PointData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

	pPoint->Add_Point(m_Offset.x + data.x, m_Offset.y + data.y);

	pPoint->Set_Value(TBL_POINTS_LAYER	, CSG_String(attributes.getLayer().c_str()));
	pPoint->Set_Value(TBL_POINTS_Z		, m_Offset.z + data.z);
}

//---------------------------------------------------------
void CDXF_Import::addLine(const DL_LineData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pLine	= m_pLines->Add_Shape();

	pLine->Add_Point(m_Offset.x + data.x1, m_Offset.y + data.y1);
	pLine->Add_Point(m_Offset.x + data.x2, m_Offset.y + data.y2);

	pLine->Set_Value(TBL_LINES_LAYER	, CSG_String(attributes.getLayer().c_str()));
	pLine->Set_Value(TBL_LINES_Z1		, m_Offset.z + data.z1);
	pLine->Set_Value(TBL_LINES_Z2		, m_Offset.z + data.z2);
}

//---------------------------------------------------------
void CDXF_Import::addPolyline(const DL_PolylineData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	switch( data.flags )
	{
	default:
	case 0:	m_pPolyLine	= m_pPolyLines	->Add_Shape();	break;
	case 1:	m_pPolyLine	= m_pPolygons	->Add_Shape();	break;
	}

	m_pPolyLine->Set_Value(TBL_POLYOBJ_LAYER, CSG_String(attributes.getLayer().c_str()));
}

//---------------------------------------------------------
void CDXF_Import::addVertex(const DL_VertexData &data)
{
	if( m_pPolyLine )
	{
		m_pPolyLine->Add_Point(m_Offset.x + data.x, m_Offset.y + data.y);	// data.z, data.bulge
	}
}

//---------------------------------------------------------
void CDXF_Import::endSequence(void)
{
	if( m_pPolyLine )
	{
		if( !m_pPolyLine->is_Valid() )
		{
			((CSG_Shapes *)m_pPolyLine->Get_Table())->Del_Shape(m_pPolyLine);
		}

		m_pPolyLine	= NULL;
	}
}

//---------------------------------------------------------
void CDXF_Import::addCircle(const DL_CircleData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pCircle	= m_pCircles->Add_Shape();

	Add_Arc(pCircle, data.cx, data.cy, data.radius, 0.0, M_PI_360);
}

//---------------------------------------------------------
void CDXF_Import::addArc(const DL_ArcData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pArc	= m_pPolyLine ? m_pPolyLine : m_pPolyLines->Add_Shape();

	Add_Arc(pArc, data.cx, data.cy, data.radius, data.angle1, data.angle2);

	if( pArc != m_pPolyLine )
	{
		pArc->Set_Value(TBL_POLYOBJ_LAYER, CSG_String(attributes.getLayer().c_str()));
	}
}

//---------------------------------------------------------
void CDXF_Import::add3dFace(const DL_3dFaceData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pTriangle	= m_pTriangles->Add_Shape();

	for(int i=0; i<3; i++)
	{
		pTriangle->Add_Point(m_Offset.x + data.x[i], m_Offset.y + data.y[i]);
	}

	pTriangle->Set_Value(TBL_TRIANGLE_LAYER	, CSG_String(attributes.getLayer().c_str()));
	pTriangle->Set_Value(TBL_TRIANGLE_THICK	, data.thickness);
	pTriangle->Set_Value(TBL_TRIANGLE_Z1	, m_Offset.z + data.z[0]);
	pTriangle->Set_Value(TBL_TRIANGLE_Z2	, m_Offset.z + data.z[1]);
	pTriangle->Set_Value(TBL_TRIANGLE_Z3	, m_Offset.z + data.z[2]);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
