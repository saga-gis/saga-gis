/**********************************************************
 * Version $Id: import_clip_resample.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                import_clip_resample.cpp               //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include "import_clip_resample.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CImport_Clip_Resample::CImport_Clip_Resample(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import, Clip and Resample Grids"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Image Files"),
		_TL(""),
		CSG_String::Format("%s|*.tif;*.tiff|%s|*.*",
			_TL("GeoTIFF Files"),
			_TL("All Files")
		), NULL, false, false, true
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "CLIP"		, _TL("Region of Interest"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "CELLSIZE"	, _TL("Target Cell Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CImport_Clip_Resample::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) || Files.Get_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pClip	= Parameters("CLIP")->asShapes();

	m_System.Assign(Parameters("CELLSIZE")->asDouble(), pClip->Get_Extent());

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")->asGridList();

	m_pGrids->Del_Items();

	for(int i=0; i<Files.Get_Count() && Process_Get_Okay(); i++)
	{
		Load_File(Files[i]);
	}

	//-----------------------------------------------------
	if( m_pGrids->Get_Count() == 0 )
	{
		Error_Set(_TL("no grids have been imported"));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CImport_Clip_Resample::Load_File(const CSG_String &File)
{
	CSG_Data_Manager	Grids;

	if( !Grids.Add(File) || !Grids.Get_Grid_System(0) || !Grids.Get_Grid_System(0)->Get(0) )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("could not load file"), File.c_str()));

		return( false );
	}

	int	n	= 0;

	for(size_t iSystem=0; iSystem<Grids.Grid_System_Count(); iSystem++)
	{
		for(size_t iGrid=0; iGrid<Grids.Get_Grid_System(iSystem)->Count(); iGrid++)
		{
			CSG_Grid	*pImport	= (CSG_Grid *)Grids.Get_Grid_System(iSystem)->Get(iGrid);

			if( m_System.Get_Extent().Intersects(pImport->Get_Extent()) )
			{
				CSG_Grid	*pGrid	= SG_Create_Grid(m_System, SG_DATATYPE_Float);

				pGrid->Assign  (pImport);
				pGrid->Set_Name(pImport->Get_Name());

				m_pGrids->Add_Item(pGrid);

				n++;
			}
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
