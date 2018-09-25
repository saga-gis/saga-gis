
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             grid_cell_polygon_coverage.cpp            //
//                                                       //
//                 Copyright (C) 2016 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_cell_polygon_coverage.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cell_Polygon_Coverage::CGrid_Cell_Polygon_Coverage(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Cell Area Covered by Polygons"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool calculates for each grid cell of the selected grid system "
		"the area that is covered by the input polygons layer. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL("Choose cell wise, if you have not many polygons. Polygon wise will show much better performance, if you have many (small) polygons."),
		CSG_String::Format("%s|%s|",
			_TL("cell wise"),
			_TL("polygon wise")
		), 1
	);

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("area"),
			_TL("percentage")
		), 1
	);

	Parameters.Add_Bool("",
		"SELECTION"	, _TL("Only Selected Polygons"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("AREA", _TL("Area of Coverage"), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Cell_Polygon_Coverage::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("POLYGONS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Cell_Polygon_Coverage::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("POLYGONS") )
	{
		pParameters->Set_Enabled("SELECTION", pParameter->asShapes() && pParameter->asShapes()->Get_Selection_Count() > 0);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_NPOLYGONS	(bSelection ? pPolygons->Get_Selection_Count() : pPolygons->Get_Count())
#define GET_POLYGON(i)	((CSG_Shape_Polygon *)(bSelection ? pPolygons->Get_Selection(i) : pPolygons->Get_Shape(i)))

//---------------------------------------------------------
bool CGrid_Cell_Polygon_Coverage::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	CSG_Grid	*pArea	= m_Grid_Target.Get_Grid("AREA");

	if( pPolygons->Get_Count() <= 0 || pArea == NULL || !pPolygons->Get_Extent().Intersects(pArea->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid system and polygon layer"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bSelection	= pPolygons->Get_Selection_Count() > 0 ? Parameters("SELECTION")->asBool() : false;

	pArea->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("Coverage"));
	pArea->Set_NoData_Value(0.0);

	DataObject_Add(pArea);
	DataObject_Set_Colors(pArea, 11, SG_COLORS_RED_GREEN, true);

	//-----------------------------------------------------
	if( Parameters("METHOD")->asInt() == 0 )
	{
		CSG_Grid_System	s(pArea->Get_System());

		for(int y=0; y<s.Get_NY() && Set_Progress(y, s.Get_NY()); y++)
		{
			double	py	= s.Get_YMin() + s.Get_Cellsize() * (y - 0.5);

			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int x=0; x<s.Get_NX(); x++)
			{
				double	px	= s.Get_XMin() + s.Get_Cellsize() * (x - 0.5);

				CSG_Shapes	Cell(SHAPE_TYPE_Polygon);
				CSG_Shape_Polygon	*pCell	= (CSG_Shape_Polygon *)Cell.Add_Shape();

				pCell->Add_Point(px                   , py                   );
				pCell->Add_Point(px + s.Get_Cellsize(), py                   );
				pCell->Add_Point(px + s.Get_Cellsize(), py + s.Get_Cellsize());
				pCell->Add_Point(px                   , py + s.Get_Cellsize());

				//---------------------------------------------
				if( pPolygons->Get_Extent().Intersects(pCell->Get_Extent()) )
				{
					for(size_t i=0; pCell->Get_Area() > 0.0 && i<GET_NPOLYGONS; i++)
					{
						if( !SG_Polygon_Difference(pCell, GET_POLYGON(i)) )	// completely contained or identical > difference is empty !
						{
							pCell->Del_Parts();
						}
					}
				}

				pArea->Set_Value(x, y, s.Get_Cellarea() > pCell->Get_Area() ? s.Get_Cellarea() - pCell->Get_Area() : 0.0);
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		pArea->Assign(0.0);

		for(size_t i=0; i<GET_NPOLYGONS && Set_Progress(i, GET_NPOLYGONS); i++)
		{
			Get_Area(GET_POLYGON(i), pArea);
		}
	}

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asInt() == 1 )	// Percentage
	{
		pArea->Multiply(100.0 / pArea->Get_Cellarea());
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cell_Polygon_Coverage::Get_Area(CSG_Shape_Polygon *pPolygon, CSG_Grid *pArea)
{
	CSG_Grid_System	s(pArea->Get_System());

	int	xMin = s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin()); if( xMin <  0          ) xMin = 0;
	int	xMax = s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax()); if( xMax >= s.Get_NX() ) xMax = s.Get_NX() - 1;
	int	yMin = s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMin()); if( yMin <  0          ) yMin = 0;
	int	yMax = s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMax()); if( yMax >= s.Get_NY() ) yMax = s.Get_NY() - 1;

	double	d	= 0.5 * s.Get_Cellsize();

	#pragma omp parallel for
	for(int y=yMin; y<=yMax; y++)
	{
		CSG_Shapes Cell(SHAPE_TYPE_Polygon); CSG_Shape_Polygon *pCell = (CSG_Shape_Polygon *)Cell.Add_Shape();

		TSG_Point	p;	p.y	= s.Get_YMin() + s.Get_Cellsize() * y;

		p.x	= s.Get_XMin() + s.Get_Cellsize() * xMin;

		for(int x=xMin; x<=xMax; x++, p.x+=s.Get_Cellsize())
		{
			pCell->Add_Point(p.x - d, p.y - d);
			pCell->Add_Point(p.x - d, p.y + d);
			pCell->Add_Point(p.x + d, p.y + d);
			pCell->Add_Point(p.x + d, p.y - d);

			if( SG_Polygon_Intersection(pCell, pPolygon) && pCell->Get_Area() > 0.0 )
			{
				pArea->Add_Value(x, y, pCell->Get_Area());
			}

			pCell->Del_Parts();
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
CPolygonCategories2Grid::CPolygonCategories2Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Categories to Grid"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"This tool has been designed to rasterize polygons representing "
		"categories and selects that category, which has maximum coverage "
		"of a cell. The advantage using this tool (instead the more simple "
		"'Shapes to Grid' or 'Polygons to Grid' tools) is that it summarizes "
		"all polygon coverages belonging to the same category. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("POLYGONS",
		"FIELD"		, _TL("Category"),
		_TL("The attribute field that specifies the category a polygon belongs to.")
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL("Choose cell wise, if you have not many polygons. Polygon wise will show much better performance, if you have many (small) polygons."),
		CSG_String::Format("%s|%s|",
			_TL("cell wise"),
			_TL("polygon wise")
		), 1
	);

	Parameters.Add_Choice("",
		"MULTIPLE"	, _TL("Multiple Polygons"),
		_TL("Output value for cells that intersect wiht more than one polygon."),
		CSG_String::Format("%s|%s|",
			_TL("minimum coverage"),
			_TL("maximum coverage")
		), 1
	);

	Parameters.Add_Table("",
		"CLASSES"	, _TL("Classification"),
		_TL("Classification look-up table for interpretation of resulting grid cell values."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("CATEGORY", _TL("Category"), false);
	m_Grid_Target.Add_Grid("COVERAGE", _TL("Coverage"),  true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygonCategories2Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("POLYGONS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CPolygonCategories2Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygonCategories2Grid::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	int		Field	= Parameters("FIELD")->asInt();

	bool	bNumber	= SG_Data_Type_is_Numeric(pPolygons->Get_Field_Type(Field));

	CSG_Grid	*pCategory	= m_Grid_Target.Get_Grid("CATEGORY", bNumber ? pPolygons->Get_Field_Type(Field) : SG_DATATYPE_Int);

	if( pPolygons->Get_Count() <= 0 || pCategory == NULL || !pPolygons->Get_Extent().Intersects(pCategory->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid system and polygon layer"));

		return( false );
	}

	//-----------------------------------------------------
	pCategory->Fmt_Name("%s [%s]", pPolygons->Get_Name(), pPolygons->Get_Field_Name(Field));
	pCategory->Assign_NoData();

	//-----------------------------------------------------
	if( !pPolygons->Set_Index(Field, TABLE_INDEX_Ascending) )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	Coverage, *pCoverage	= m_Grid_Target.Get_Grid("COVERAGE");

	if( pCoverage == NULL )
	{
		Coverage.Create(pCategory->Get_System());

		pCoverage	= &Coverage;
	}

	pCoverage->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("Coverage"));
	pCoverage->Set_NoData_Value(0.);
	pCoverage->Assign(0.);

	//-----------------------------------------------------
	CSG_Table	Classes;

	Classes.Add_Field("COLOR"      , SG_DATATYPE_Color );
	Classes.Add_Field("NAME"       , SG_DATATYPE_String);
	Classes.Add_Field("DESCRIPTION", SG_DATATYPE_String);
	Classes.Add_Field("MINIMUM"    , SG_DATATYPE_Double);
	Classes.Add_Field("MAXIMUM"    , SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_String	Category;

	pPolygons->Select();	// clear selection

	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape_byIndex(i);

		if( Category.Cmp(pPolygon->asString(Field)) )	// new category
		{
			Set_Category(pPolygons, pCategory, pCoverage, Classes, Category, bNumber);	// also clears selection

			Category	= pPolygon->asString(Field);
		}

		pPolygons->Select(pPolygon, true);	// adds polygon to selection
	}

	Set_Category(pPolygons, pCategory, pCoverage, Classes, Category, bNumber);

	//-----------------------------------------------------
	DataObject_Add   (pCategory);
	DataObject_Update(pCategory);

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pCategory, "LUT");

	if( pLUT && pLUT->asTable() && pLUT->asTable()->Create(Classes) )
	{
		DataObject_Set_Parameter(pCategory, pLUT);
		DataObject_Set_Parameter(pCategory, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	if( Parameters("CLASSES")->asTable() )
	{
		Classes.Del_Field(4);	// MAXIMUM
		Classes.Del_Field(2);	// DESCRIPTION
		Classes.Del_Field(0);	// COLOR

		Classes.Set_Field_Name(0, _TL("Category"));
		Classes.Set_Field_Name(1, _TL("Value"   ));

		Parameters("CLASSES")->asTable()->Create(Classes);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygonCategories2Grid::Set_Category(CSG_Shapes *pPolygons, CSG_Grid *pCategory, CSG_Grid *pCoverage, CSG_Table &Classes, const CSG_String &Category, bool bNumber)
{
	if( pPolygons->Get_Selection_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	Coverage(pCoverage->Get_System());

	CGrid_Cell_Polygon_Coverage	Get_Coverage;

	Get_Coverage.Set_Manager(NULL);

	Get_Coverage.Set_Parameter("POLYGONS"         , pPolygons);
	Get_Coverage.Set_Parameter("METHOD"           , Parameters("METHOD"));
	Get_Coverage.Set_Parameter("OUTPUT"           , 0);	// area (not percentage)
	Get_Coverage.Set_Parameter("TARGET_DEFINITION", 1);	// grid or grid system
	Get_Coverage.Set_Parameter("AREA"             , &Coverage);

	SG_UI_ProgressAndMsg_Lock(true);

	if( !Get_Coverage.Execute() )
	{
		SG_UI_ProgressAndMsg_Lock(false);

		pPolygons->Select();	// clear selection

		return( false );
	}

	SG_UI_ProgressAndMsg_Lock(false);

	pPolygons->Select();	// clear selection

	//-----------------------------------------------------
	CSG_Table_Record	&Class	= *Classes.Add_Record();

	double	ClassID	= bNumber ? Category.asDouble() : Classes.Get_Count();

	Class.Set_Value(0, SG_Color_Get_Random());
	Class.Set_Value(1, Category);
	Class.Set_Value(3, ClassID);
	Class.Set_Value(4, ClassID);

	int	Multiple	= Parameters("MULTIPLE")->asInt();

	#pragma omp parallel for
	for(sLong i=0; i<pCategory->Get_NCells(); i++)
	{
		switch( Multiple )
		{
		case  0:	// minimum
			if( pCoverage->asDouble(i) <= 0. || Coverage.asDouble(i) < pCoverage->asDouble(i) )
			{
				pCategory->Set_Value(i, ClassID);
				pCoverage->Set_Value(i, Coverage.asDouble(i));
			}
			break;

		default:	// maximum
			if( Coverage.asDouble(i) > pCoverage->asDouble(i) )
			{
				pCategory->Set_Value(i, ClassID);
				pCoverage->Set_Value(i, Coverage.asDouble(i));
			}
			break;
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
