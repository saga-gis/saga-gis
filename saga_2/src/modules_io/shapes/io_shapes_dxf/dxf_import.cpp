
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
CDXF_Import::CDXF_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import DXF Files"));

	Set_Author		(_TL("Copyrights (c) 2007 by O. Conrad"));

	Set_Description	(_TW(
		"This module imports DXF files using the free \"dxflib\" library. Get more information "
		"about this library from the RibbonSoft homepage at:\n"
		"<a href=\"http://www.ribbonsoft.com/dxflib.html\">http://www.ribbonsoft.com/dxflib.html</a>"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		_TL("DXF Files (*.dxf)|*.dxf|All Files|*.*")
	);
}

//---------------------------------------------------------
CDXF_Import::~CDXF_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDXF_Import::On_Execute(void)
{
	bool			bResult	= false;
	CSG_String		FileName;

	//-----------------------------------------------------
	Parameters("SHAPES")->asShapesList()->Del_Items();

	FileName	= Parameters("FILE")->asString();

	m_pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point		, SG_File_Get_Name(FileName, true));
	m_pPoints	->Get_Table().Add_Field("VAL"	, TABLE_FIELDTYPE_Double);
	m_pPoints	->Get_Table().Add_Field("NAM"	, TABLE_FIELDTYPE_String);

	m_pLines	= SG_Create_Shapes(SHAPE_TYPE_Line		, SG_File_Get_Name(FileName, true));
	m_pLines	->Get_Table().Add_Field("VAL"	, TABLE_FIELDTYPE_Double);
	m_pLines	->Get_Table().Add_Field("NAM"	, TABLE_FIELDTYPE_String);

	m_pPolygons	= SG_Create_Shapes(SHAPE_TYPE_Polygon	, SG_File_Get_Name(FileName, true));
	m_pPolygons	->Get_Table().Add_Field("VAL"	, TABLE_FIELDTYPE_Double);
	m_pPolygons	->Get_Table().Add_Field("NAM"	, TABLE_FIELDTYPE_String);

	m_pShape	= NULL;

	//-----------------------------------------------------
	if( SG_File_Exists(FileName) )
	{
		DL_Dxf	*pDXF	= new DL_Dxf();

		pDXF->in(FileName.b_str(), this);

		delete(pDXF);
	}

	//-----------------------------------------------------
	if( m_pPoints->is_Valid() && m_pPoints->Get_Count() > 0 )
	{
		bResult	= true;
		DataObject_Add(m_pPoints);
		Parameters("SHAPES")->asShapesList()->Add_Item(m_pPoints);
	}
	else
	{
		delete(m_pPoints);
	}

	if( m_pLines->is_Valid() && m_pLines->Get_Count() > 0 )
	{
		bResult	= true;
		DataObject_Add(m_pLines);
		Parameters("SHAPES")->asShapesList()->Add_Item(m_pLines);
	}
	else
	{
		delete(m_pLines);
	}

	if( m_pPolygons->is_Valid() && m_pPolygons->Get_Count() > 0 )
	{
		bResult	= true;
		DataObject_Add(m_pPolygons);
		Parameters("SHAPES")->asShapesList()->Add_Item(m_pPolygons);
	}
	else
	{
		delete(m_pPolygons);
	}

	return( bResult );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDXF_Import::addLayer(const DL_LayerData &data)
{
//	Message_Add(CSG_String::Format(SG_T("LAYER: %s flags: %d"), data.name.c_str(), data.flags));
	m_pShape	= NULL;
}

//---------------------------------------------------------
void CDXF_Import::addBlock(const DL_BlockData &data)
{
//	Message_Add(CSG_String::Format(SG_T("BLOCK: %s flags: %d, x%6.3f y%6.3f z%6.3f"), data.name.c_str(), data.flags, data.bpx, data.bpy, data.bpz));
	Message_Add(CSG_String::Format(SG_T("%s"), data.name.c_str()));

	m_Offset.x	= data.bpx;
	m_Offset.y	= data.bpy;
	m_Offset.z	= data.bpz;
}

//---------------------------------------------------------
void CDXF_Import::endBlock(void)
{
	m_pShape	= NULL;
}

//---------------------------------------------------------
void CDXF_Import::addPolyline(const DL_PolylineData &data)
{
	switch( data.flags )
	{
	default:
	case 0:	m_pShape	= m_pLines		->Add_Shape();	break;
	case 1:	m_pShape	= m_pPolygons	->Add_Shape();	break;
	}
}

//---------------------------------------------------------
void CDXF_Import::endSequence(void)
{
	m_pShape	= NULL;
}

//---------------------------------------------------------
void CDXF_Import::addPoint(const DL_PointData &data)
{
	CSG_Shape	*pShape	= m_pPoints->Add_Shape();

	pShape->Add_Point(m_Offset.x + data.x, m_Offset.y + data.y);
	pShape->Get_Record()->Set_Value(0, m_Offset.z + data.z);
}

//---------------------------------------------------------
void CDXF_Import::addLine(const DL_LineData &data)
{
	CSG_Shape	*pShape	= m_pShape ? m_pShape : m_pLines->Add_Shape();

	pShape->Add_Point(m_Offset.x + data.x1, m_Offset.y + data.y1);
	pShape->Add_Point(m_Offset.x + data.x2, m_Offset.y + data.y2);
//	pShape->Get_Record()->Set_Value(0, m_Offset.z + data.z1);
//	pShape->Get_Record()->Set_Value(0, m_Offset.z + data.z2);
}

//---------------------------------------------------------
void CDXF_Import::addArc(const DL_ArcData &data)
{
//	Message_Add(CSG_String::Format("ARC      (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f",
//           data.cx, data.cy, data.cz,
//           data.radius, data.angle1, data.angle2));
}

//---------------------------------------------------------
void CDXF_Import::addCircle(const DL_CircleData &data)
{
//	Message_Add(CSG_String::Format("CIRCLE   (%6.3f, %6.3f, %6.3f) %6.3f",
//           data.cx, data.cy, data.cz,
//           data.radius));
}

//---------------------------------------------------------
void CDXF_Import::addVertex(const DL_VertexData &data)
{
	if( m_pShape )
	{
		m_pShape->Add_Point(m_Offset.x + data.x, m_Offset.y + data.y);	// data.z, data.bulge
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
