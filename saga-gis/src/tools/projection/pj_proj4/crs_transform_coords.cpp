
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       pj_proj4                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                crs_transform_coords.cpp               //
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
#include "crs_transform_coords.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_Coords_Grid::CCRS_Transform_Coords_Grid(void)
{
	Set_Name		(_TL("Coordinate Conversion (Grids)"));

	Set_Author		("O. Conrad (c) 2019");

	Set_Description	(_TW(
		"This tool projects coordinate tuples. Coordinate tuples "
		"have to be provided by the two source coordinate grids. "
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SOURCE_X"	, _TL("X Coordinate Source"),
		_TL("Grid that provides the source X coordinates."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SOURCE_Y"	, _TL("Y Coordinate Source"),
		_TL("Grid that provides the source Y coordinates."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"TARGET_X"	, _TL("Projected X Coordinates"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"TARGET_Y"	, _TL("Projected Y Coordinates"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	CCRS_Picker	Picker;

	Parameters.Add_Parameters("",
		"SOURCE_CRS", _TL("Source Coordinate System"),
		_TL("")
	)->asParameters()->Assign_Parameters(Picker.Get_Parameters());

	Parameters.Add_Parameters("",
		"TARGET_CRS", _TL("Target Coordinate System"),
		_TL("")
	)->asParameters()->Assign_Parameters(Picker.Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Coords_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Base::Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CCRS_Transform_Coords_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Base::Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Coords_Grid::On_Execute(void)
{
	CSG_CRSProjector	Projector;

	CSG_Projection	Source(Parameters("SOURCE_CRS")->asParameters()->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);

	if( !Projector.Set_Source(Source) )
	{
		Error_Set(_TL("failed to initialize source projection"));

		return( false );
	}

	CSG_Projection	Target(Parameters("TARGET_CRS")->asParameters()->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);

	if( !Projector.Set_Source(Source) || !Projector.Set_Target(Target) )
	{
		Error_Set(_TL("failed to initialize target projection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pSource_X	= Parameters("SOURCE_X")->asGrid();
	CSG_Grid	*pSource_Y	= Parameters("SOURCE_Y")->asGrid();

	CSG_Grid	*pTarget_X	= Parameters("TARGET_X")->asGrid();
	CSG_Grid	*pTarget_Y	= Parameters("TARGET_Y")->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pSource_X->is_NoData(x, y) && !pSource_Y->is_NoData(x, y) )
			{
				CSG_Point	Point(pSource_X->asDouble(x, y), pSource_Y->asDouble(x, y));

				if( Projector.Get_Projection(Point) )
				{
					pTarget_X->Set_Value(x, y, Point.Get_X());
					pTarget_Y->Set_Value(x, y, Point.Get_Y());

					continue;
				}
			}

			pTarget_X->Set_NoData(x, y);
			pTarget_Y->Set_NoData(x, y);
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
CCRS_Transform_Coords_Table::CCRS_Transform_Coords_Table(void)
{
	Set_Name		(_TL("Coordinate Conversion (Table)"));

	Set_Author		("O. Conrad (c) 2019");

	Set_Description	(_TW(
		"This tool projects coordinate tuples. Coordinate tuples "
		"have to be provided by the two source coordinate fields. "
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"SOURCE_X"	, _TL("X Coordinate Source"),
		_TL("Table field that provides the source X coordinates.")
	);

	Parameters.Add_Table_Field("TABLE",
		"SOURCE_Y"	, _TL("Y Coordinate Source"),
		_TL("Table field that provides the source Y coordinates.")
	);

	Parameters.Add_Table_Field("TABLE",
		"TARGET_X"	, _TL("Projected X Coordinates"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field("TABLE",
		"TARGET_Y"	, _TL("Projected Y Coordinates"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	CCRS_Picker	Picker;

	Parameters.Add_Parameters("",
		"SOURCE_CRS", _TL("Source Coordinate System"),
		_TL("")
	)->asParameters()->Assign_Parameters(Picker.Get_Parameters());

	Parameters.Add_Parameters("",
		"TARGET_CRS", _TL("Target Coordinate System"),
		_TL("")
	)->asParameters()->Assign_Parameters(Picker.Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Coords_Table::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Base::Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CCRS_Transform_Coords_Table::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Base::Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Coords_Table::On_Execute(void)
{
	CSG_CRSProjector	Projector;

	CSG_Projection	Source(Parameters("SOURCE_CRS")->asParameters()->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);

	if( !Projector.Set_Source(Source) )
	{
		Error_Set(_TL("failed to initialize source projection"));

		return( false );
	}

	CSG_Projection	Target(Parameters("TARGET_CRS")->asParameters()->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);

	if( !Projector.Set_Source(Source) || !Projector.Set_Target(Target) )
	{
		Error_Set(_TL("failed to initialize target projection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	int	Source_X	= Parameters("SOURCE_X")->asInt();
	int	Source_Y	= Parameters("SOURCE_Y")->asInt();

	int	Target_X	= Parameters("TARGET_X")->asInt();
	int	Target_Y	= Parameters("TARGET_Y")->asInt();

	if( Target_X < 0 )
	{
		Target_X	= pTable->Get_Field_Count();

		pTable->Add_Field("X_PROJECTED", SG_DATATYPE_Double);
	}

	if( Target_Y < 0 )
	{
		Target_Y	= pTable->Get_Field_Count();

		pTable->Add_Field("Y_PROJECTED", SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	//	#pragma omp parallel for
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		if( !pRecord->is_NoData(Source_X) && !pRecord->is_NoData(Source_Y) )
		{
			CSG_Point	Point(pRecord->asDouble(Source_X), pRecord->asDouble(Source_Y));

			if( Projector.Get_Projection(Point) )
			{
				pRecord->Set_Value(Target_X, Point.Get_X());
				pRecord->Set_Value(Target_Y, Point.Get_Y());

				continue;
			}
		}

		pRecord->Set_NoData(Target_X);
		pRecord->Set_NoData(Target_Y);
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
