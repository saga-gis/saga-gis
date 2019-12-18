
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        io_grid                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       gvmd.cpp                        //
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
#include "gvmd.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGVMD_Import::CGVMD_Import(void)
{
	Set_Name		(_TL("Import Regular SubsurfaceViewer Grid"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Import a SubsurfaceViewer file. Works only for regular cells (x/y).\n"
		"(SubsurfaceViewer - a GIS for the geological subsurface)"
	));

	Add_Reference("http://subsurfaceviewer.com/ssv/", SG_T("SubsurfaceViewer Homepage"));

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL("The input file."),
		CSG_String::Format(
			"%s (*.gvmd)|*.gvmd|%s|*.*",
			_TL("SubsurfaceViewer Files"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_String("",
		"FIELD"		, _TL("Layer Field Name"),
		_TL(""),
		"name"
	);

	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Cell Centers"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Grids_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGVMD_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGVMD_Import::On_Execute(void)
{
	CSG_String LayerName(Parameters("FIELD")->asString());

	CSG_Unique_String_Statistics Layers;

	CSG_Table Table, *pTable = Parameters("TABLE")->asTable(); if( !pTable ) pTable = &Table;

	if( !Get_Table(Table, Layers, LayerName) )
	{
		return( false );
	}

	Set_Points(Table);

	Set_Grids(Table, Layers, LayerName);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGVMD_Import::Set_Grids(const CSG_Table &Table, const CSG_Unique_String_Statistics &Layers, const CSG_String &LayerName)
{
	double	Cellsize	= Table[0].asDouble(m_Fields[1][0]) - Table[0].asDouble(m_Fields[0][0]);

	CSG_Grid_System	System(Cellsize,
		Table.Get_Minimum(m_Fields[0][0]) + Cellsize / 2.,
		Table.Get_Minimum(m_Fields[0][1]) + Cellsize / 2.,
		Table.Get_Maximum(m_Fields[1][0]) - Cellsize / 2.,
		Table.Get_Maximum(m_Fields[1][1]) - Cellsize / 2.
	);

	int	id	= Table.Get_Field(LayerName);

	//-----------------------------------------------------
	for(int iLayer=0; iLayer<Layers.Get_Count() && Process_Get_Okay(); iLayer++)
	{
		CSG_String	Layer(Layers.Get_Value(iLayer));

		Process_Set_Text(CSG_String::Format("%s: %s", _TL("grids"), Layer.c_str()));

		CSG_Grids	*pGrids	= SG_Create_Grids(System, 2);

		if( !pGrids )
		{
			Error_Set(_TL("failed to allocate memory for grid collection"));

			return( false );
		}

		Parameters("GRIDS")->asGridsList()->Add_Item(pGrids);

		pGrids->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false) + "." + Layer);
		pGrids->Assign_NoData();

		for(int i=0; i<Table.Get_Count() && Set_Progress(i, Table.Get_Count()); i++)
		{
			CSG_Table_Record	*pRecord	= Table.Get_Record(i);

			if( !Layer.Cmp(pRecord->asString(id)) )
			{
				TSG_Point_Z	P[2];

				P[0].x = pRecord->asDouble(m_Fields[0][0]); P[1].x = pRecord->asDouble(m_Fields[1][0]);
				P[0].y = pRecord->asDouble(m_Fields[0][1]); P[1].y = pRecord->asDouble(m_Fields[1][1]);
				P[0].z = pRecord->asDouble(m_Fields[0][2]); P[1].z = pRecord->asDouble(m_Fields[1][2]);

				int	x	= System.Get_xWorld_to_Grid((P[1].x + P[0].x) / 2.);
				int	y	= System.Get_yWorld_to_Grid((P[1].y + P[0].y) / 2.);

				if( System.is_InGrid(x, y) )
				{
					pGrids->Set_Value(x, y, 0, P[0].z);
					pGrids->Set_Value(x, y, 1, P[1].z);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGVMD_Import::Get_Table(CSG_Table &Table, CSG_Unique_String_Statistics &Layers, const CSG_String &LayerName)
{
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_R, false) )
	{
		Error_Fmt("%s\n[%s]", _TL("could not open file"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	sLine;

	while( Stream.Read_Line(sLine) && sLine.Find("XY_irregular") < 0 )	{}

	if( Stream.is_EOF() || sLine.Find("XY_irregular") < 0 || sLine.AfterFirst('=').CmpNoCase("false") )
	{
		Error_Set(_TL("file is not a regular SubsurfaceViewer file"));

		return( false );
	}

	//-----------------------------------------------------
	Stream.Read_Line(sLine); CSG_Strings Names(SG_String_Tokenize(sLine));
	Stream.Read_Line(sLine); CSG_Strings Types(SG_String_Tokenize(sLine));

	if( Stream.is_EOF() || Names.Get_Count() < 7 || Names.Get_Count() != Types.Get_Count() )
	{
		Error_Set(_TL("file is not a regular SubsurfaceViewer file"));

		return( false );
	}

	//-----------------------------------------------------
	Table.Destroy();

	m_Fields[0][0] = m_Fields[1][0] =
	m_Fields[0][1] = m_Fields[1][1] =
	m_Fields[0][2] = m_Fields[1][2] = -1;

	for(int i=0; i<Names.Get_Count(); i++)
	{
		Table.Add_Field(Names[i], !Types[i].CmpNoCase("float") ? SG_DATATYPE_Double : SG_DATATYPE_String);

		if( !Names[i].CmpNoCase("xmin") ) m_Fields[0][0] = i;
		if( !Names[i].CmpNoCase("xmax") ) m_Fields[1][0] = i;

		if( !Names[i].CmpNoCase("ymin") ) m_Fields[0][1] = i;
		if( !Names[i].CmpNoCase("ymax") ) m_Fields[1][1] = i;

		if( !Names[i].CmpNoCase("zmin") ) m_Fields[0][2] = i;
		if( !Names[i].CmpNoCase("zmax") ) m_Fields[1][2] = i;
	}

	if( m_Fields[0][0] < 0 || m_Fields[1][0] < 0
	||  m_Fields[0][1] < 0 || m_Fields[1][1] < 0
	||  m_Fields[0][2] < 0 || m_Fields[1][2] < 0 )
	{
		Error_Set(_TL("missing coordinate fields"));

		return( false );
	}

	int	id	= Table.Get_Field(LayerName);

	if( id < 0 )
	{
		Error_Set(_TL("missing layer id field"));

		return( false );
	}

	//-----------------------------------------------------
	for(sLong Length=Stream.Length(); Stream.Read_Line(sLine) && Set_Progress((double)Stream.Tell(), (double)Length); )
	{
		CSG_Strings	Values(SG_String_Tokenize(sLine));

		if( Values.Get_Count() < Table.Get_Field_Count() )
		{
			continue;
		}

		CSG_Table_Record	&Record	= *Table.Add_Record();

		for(int i=0; i<Table.Get_Field_Count(); i++)
		{
			Record.Set_Value(i, Values[i]);
		}

		Layers	+= Record.asString(id);
	}

	//-----------------------------------------------------
	return( Table.Get_Count() > 0 && Layers.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGVMD_Import::Set_Points(const CSG_Table &Table)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( !pPoints )
	{
		return( false );
	}

	pPoints->Create(SHAPE_TYPE_Point, _TL("Points"), (CSG_Table *)&Table, SG_VERTEX_TYPE_XYZ);

	for(int i=0; i<Table.Get_Count() && Set_Progress(i, Table.Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= Table.Get_Record(i);

		TSG_Point_Z	P[2];

		P[0].x = pRecord->asDouble(m_Fields[0][0]); P[1].x = pRecord->asDouble(m_Fields[1][0]);
		P[0].y = pRecord->asDouble(m_Fields[0][1]); P[1].y = pRecord->asDouble(m_Fields[1][1]);
		P[0].z = pRecord->asDouble(m_Fields[0][2]); P[1].z = pRecord->asDouble(m_Fields[1][2]);

		CSG_Shape	*pPoint	= pPoints->Add_Shape(pRecord, SHAPE_COPY_ATTR);

		pPoint->Set_Point(
			P[0].x + (P[1].x - P[0].x) / 2.,
			P[0].y + (P[1].y - P[0].y) / 2., 0
		);

		pPoint->Set_Z(
			P[0].z + (P[1].z - P[0].z) / 2., 0
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
