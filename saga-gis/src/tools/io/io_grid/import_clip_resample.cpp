/**********************************************************
 * Version $Id: import_clip_resample.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
	CSG_Parameter	*pNode;

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Image Files"),
		_TL(""),
		CSG_String::Format("%s|*.tif;*.tiff|%s|*.*",
			_TL("GeoTIFF Files"),
			_TL("All Files")
		), NULL, false, false, true
	);

	//-----------------------------------------------------
	Parameters.Add_Bool(
		NULL	, "KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL(""),
		false
	);

	pNode	= Parameters.Add_Bool(
		NULL	, "NODATA"		, _TL("User Defined No-Data Value"),
		_TL(""),
		false
	);

	Parameters.Add_Double(
		pNode	, "NODATA_VAL"	, _TL("No-Data Value"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "CLIP"		, _TL("Region of Interest"),
		_TL("bounding box for clipping"),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Bool(
		NULL	, "RESAMPLE"	, _TL("Resample"),
		_TL(""),
		true
	);

	Parameters.Add_Double(
		pNode	, "CELLSIZE"	, _TL("Cell Size"),
		_TL(""),
		100.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CImport_Clip_Resample::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CImport_Clip_Resample::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NODATA") )
	{
		pParameters->Set_Enabled("NODATA_VAL", pParameter->asBool());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "RESAMPLE") )
	{
		pParameters->Set_Enabled("CELLSIZE"  , pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
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
	m_pGrids	= Parameters("GRIDS")->asGridList();

	m_pGrids->Del_Items();

	//-----------------------------------------------------
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
		Error_Set(CSG_String::Format("%s: %s", _TL("could not load file"), File.c_str()));

		return( false );
	}

	int	n	= 0;

	for(size_t iSystem=0; iSystem<Grids.Grid_System_Count(); iSystem++)
	{
		for(size_t iGrid=0; iGrid<Grids.Get_Grid_System(iSystem)->Count(); iGrid++)
		{
			if( Load_Grid((CSG_Grid *)Grids.Get_Grid_System(iSystem)->Get(iGrid)) )
			{
				n++;
			}
		}
	}

	return( n > 0 );
}

//---------------------------------------------------------
bool CImport_Clip_Resample::Load_Grid(CSG_Grid *pImport)
{
	CSG_Grid_System	System	= pImport->Get_System();

	//-----------------------------------------------------
	const CSG_Rect	*pClip	= Parameters("CLIP")->asShapes() ? &Parameters("CLIP")->asShapes()->Get_Extent() : NULL;

	if( pClip )
	{
		if( !pClip->Intersects(System.Get_Extent()) )
		{
			return( false );
		}

		TSG_Rect	Extent	= System.Get_Extent();

		if( pClip->Get_XMin() > System.Get_XMin() )	Extent.xMin	= System.Fit_xto_Grid_System(pClip->Get_XMin());
		if( pClip->Get_XMax() < System.Get_XMax() )	Extent.xMax	= System.Fit_xto_Grid_System(pClip->Get_XMax());
		if( pClip->Get_YMin() > System.Get_YMin() )	Extent.yMin	= System.Fit_yto_Grid_System(pClip->Get_YMin());
		if( pClip->Get_YMax() < System.Get_YMax() )	Extent.yMax	= System.Fit_yto_Grid_System(pClip->Get_YMax());

		System.Assign(System.Get_Cellsize(), Extent);
	}

	//-----------------------------------------------------
	if( Parameters("RESAMPLE")->asBool() )
	{
		double	Cellsize	= Parameters("CELLSIZE")->asDouble();

		if( Cellsize > 0.0 && Cellsize != System.Get_Cellsize() )
		{
			System.Assign(Cellsize, System.Get_Extent());
		}
	}

	//-----------------------------------------------------
	if( Parameters("NODATA")->asBool() )
	{
		pImport->Set_NoData_Value(Parameters("NODATA_VAL")->asDouble());
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= SG_Create_Grid(System, Parameters("KEEP_TYPE")->asBool() ? pImport->Get_Type() : SG_DATATYPE_Float);

	if( pGrid )
	{
		pGrid->Assign  (pImport);
		pGrid->Set_Name(pImport->Get_Name());

		m_pGrids->Add_Item(pGrid);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
