/**********************************************************
 * Version $Id: gdal_import_netcdf.cpp 1379 2012-04-26 11:58:47Z manfred-e $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                gdal_import_netcdf.cpp                 //
//                                                       //
//            Copyright (C) 2012 O. Conrad               //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_import_netcdf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import_NetCDF::CGDAL_Import_NetCDF(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import NetCDF"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"This tool imports grids NetCDF Format using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*.nc|%s|*.*",
			_TL("NetCDF Files (*.nc)"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_Bool("",
		"SAVE_FILE"	, _TL("Save to File"),
		_TL("save output to file instead of memory"),
		false
	);

	Parameters.Add_FilePath("SAVE_FILE",
		"SAVE_PATH"	, _TL("Save to Path"),
		_TL(""),
		NULL, NULL, true, true
	);

	Parameters.Add_Bool("",
		"SILENT"	, _TL("Silent"),
		_TL("imports all layers without request"),
		false
	)->Set_UseInCMD(false);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"TRANSFORM"	, _TL("Transformation"),
		_TL("apply coordinate transformation if appropriate"),
		true
	);

	Parameters.Add_Choice("TRANSFORM",
		"RESAMPLING", _TL("Resampling"),
		_TL("interpolation method to use if grid needs to be aligned to coordinate system"),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import_NetCDF::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("SAVE_FILE") )
	{
		pParameters->Set_Enabled("SAVE_PATH" , pParameter->asBool() == true);
	}

	if(	pParameter->Cmp_Identifier("VARS_ALL" ) )
	{
		pParameters->Set_Enabled("VARS"      , pParameter->asBool() == false);
	}

	if(	pParameter->Cmp_Identifier("TIME_ALL" ) )
	{
		pParameters->Set_Enabled("TIME"      , pParameter->asBool() == false);
	}

	if(	pParameter->Cmp_Identifier("LEVEL_ALL") )
	{
		pParameters->Set_Enabled("LEVEL"     , pParameter->asBool() == false);
	}

	if(	pParameter->Cmp_Identifier("TRANSFORM") )
	{
		pParameters->Set_Enabled("RESAMPLING", pParameter->asBool() == true);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CGDAL_Import_NetCDF::Get_Variable(CSG_GDAL_DataSet &DataSet, int iBand)
{
	const char *s	= DataSet.Get_MetaData_Item(iBand, "NETCDF_VARNAME");

	return( s );
}

//---------------------------------------------------------
const char * CGDAL_Import_NetCDF::Get_Time(CSG_GDAL_DataSet &DataSet, int iBand)
{
	const char *s	= DataSet.Get_MetaData_Item(iBand, "NETCDF_DIMENSION_time");
	if( !s )    s	= DataSet.Get_MetaData_Item(iBand, "NETCDF_DIM_time");

	return( s );
}

//---------------------------------------------------------
const char * CGDAL_Import_NetCDF::Get_Level(CSG_GDAL_DataSet &DataSet, int iBand)
{
	const char *s	= DataSet.Get_MetaData_Item(iBand, "NETCDF_DIMENSION_level");
	if( !s )    s	= DataSet.Get_MetaData_Item(iBand, "NETCDF_DIM_level");
	if( !s )    s	= DataSet.Get_MetaData_Item(iBand, "NETCDF_DIM_lev");

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CGDAL_Import_NetCDF::Get_Time_String(const CSG_String &Time, int Format)
{
	CSG_DateTime	Date;

	switch( Format )
	{
	case  0:
		Date.Set_Hours_AD(Time.asInt());
		return( Date.Format_ISOCombined() );

	default:
		return( Time );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_NetCDF::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")->asGridList();
	m_pGrids	->Del_Items();

	if( (m_bSaveFile = Parameters("SAVE_FILE")->asBool()) == true )
	{
		m_SavePath	= Parameters("SAVE_PATH")->asString();

		if( !SG_Dir_Exists(m_SavePath) )
		{
			m_SavePath	= SG_File_Get_Path(Parameters("FILE")->asString());
		}
	}

	//-----------------------------------------------------
	CSG_GDAL_DataSet		DataSet;

	if( !DataSet.Open_Read(Parameters("FILE")->asString()) )
	{
		Error_Fmt("%s [%s]", _TL("could not open file"), Parameters("FILE")->asString());

		return( false );
	}

	if( DataSet.Get_DriverID().Cmp("netCDF") )
	{
		Message_Add(CSG_String::Format("\n%s: %s [%s]\n", _TL("Warning"), _TL("Driver"), DataSet.Get_DriverID().c_str()), false);
	}

	//-----------------------------------------------------
	CSG_Strings	SubDataSets	= DataSet.Get_SubDataSets(false);

	if( SubDataSets.Get_Count() > 0 )
	{
		CSG_Strings	Descriptions	= DataSet.Get_SubDataSets(true);

		for(int i=0; i<SubDataSets.Get_Count() && Process_Get_Okay(); i++)
		{
			if( DataSet.Open_Read(SubDataSets[i]) )
			{
				CSG_String	Name	= SubDataSets[i];

				if( Name.Find("://") >= 0 )
				{
					Name	= Name.Right(Name.Length() - Name.Find("://") - 3);
				}

				Load(DataSet, Name, Descriptions[i]);
			}
		}

		return( true );
	}

	return( Load(DataSet, SG_File_Get_Name(Parameters("FILE")->asString(), false), "") );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_NetCDF::Load(CSG_GDAL_DataSet &DataSet, const CSG_String &Name, const CSG_String &Description)
{
	//-----------------------------------------------------
	CSG_MetaData	MetaData, *pChild;

	DataSet.Get_MetaData(MetaData);

	Message_Add(CSG_String::Format("\n____\n%s\n%s\n%s\n",
		Name.c_str(),
		Description.c_str(),
		MetaData.asText().c_str()), false
	);

	//-----------------------------------------------------
	int	tFmt	= -1;

	if( (pChild = MetaData.Get_Child("time#units")) != NULL )
	{
		if( pChild->Get_Content().Find("hours since 1-1-1 00:00:0.0") >= 0 )
		{
			tFmt	= 0;
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P, *pVars = NULL, *pTime = NULL, *pLevel = NULL;

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() && Parameters("SILENT")->asBool() == false )
	{
		P.Create(this, CSG_String::Format("%s [%s]", _TL("Import NetCDF"), Name.c_str()), _TL(""));
		P.Set_Callback_On_Parameter_Changed(Get_Parameter_Changed());

		P.Add_Bool      (""         , "VARS_ALL" , _TL("All Variables"), _TL(""), true); pVars  =
		P.Add_Parameters("VARS_ALL" , "VARS"     , _TL("Selection"    ), _TL(""))->asParameters();

		P.Add_Bool      (""         , "TIME_ALL" , _TL("All Times"    ), _TL(""), true); pTime  =
		P.Add_Parameters("TIME_ALL" , "TIME"     , _TL("Selection"    ), _TL(""))->asParameters();

		P.Add_Bool      (""         , "LEVEL_ALL", _TL("All Levels"   ), _TL(""), true); pLevel =
		P.Add_Parameters("LEVEL_ALL", "LEVEL"    , _TL("Selection"    ), _TL(""))->asParameters();

		for(int i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
		{
			const char	*s;

			if( !!(s = Get_Variable(DataSet, i)) && !(*pVars )(s) ) pVars ->Add_Bool("", s, s, _TL(""), false);
			if( !!(s = Get_Time    (DataSet, i)) && !(*pTime )(s) ) pTime ->Add_Bool("", s, Get_Time_String(s, tFmt), _TL(""), false);
			if( !!(s = Get_Level   (DataSet, i)) && !(*pLevel)(s) ) pLevel->Add_Bool("", s, s, _TL(""), false);
		}

		P("VARS_ALL" )->Set_Enabled(pVars ->Get_Count() > 1);
		P("TIME_ALL" )->Set_Enabled(pTime ->Get_Count() > 1);
		P("LEVEL_ALL")->Set_Enabled(pLevel->Get_Count() > 1);

		//-------------------------------------------------
		if( (pVars->Get_Count() > 1 || pTime->Get_Count() > 1 || pLevel->Get_Count() > 1)
		&&  !Dlg_Parameters(&P, CSG_String::Format("%s: %s", _TL("Import NetCDF"), Name.c_str())) )
		{
			return( false );
		}

		//-------------------------------------------------
		if( !P("VARS_ALL" )->is_Enabled() || P("VARS_ALL" )->asBool() )	pVars	= NULL;
		if( !P("TIME_ALL" )->is_Enabled() || P("TIME_ALL" )->asBool() )	pTime	= NULL;
		if( !P("LEVEL_ALL")->is_Enabled() || P("LEVEL_ALL")->asBool() )	pLevel	= NULL;
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline         ;	break;
	}

	bool	bTransform	= Parameters("TRANSFORM")->asBool() && DataSet.Needs_Transformation();

	//-----------------------------------------------------
	for(int i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
	{
		CSG_Grid		*pGrid;
		CSG_Parameter	*pLoad;

		if( (!pVars  || (!!(pLoad = (*pVars )(Get_Variable(DataSet, i))) && pLoad->asBool()))
		&&	(!pTime  || (!!(pLoad = (*pTime )(Get_Time    (DataSet, i))) && pLoad->asBool()))
		&&	(!pLevel || (!!(pLoad = (*pLevel)(Get_Level   (DataSet, i))) && pLoad->asBool())) )
		{
			SG_UI_Progress_Lock(true);

			if( (pGrid = DataSet.Read(i)) != NULL )
			{
				if( bTransform )
				{
					Process_Set_Text(CSG_String::Format("%s [%d/%d]", _TL("band transformation"), i + 1, DataSet.Get_Count()));

					DataSet.Get_Transformation(&pGrid, Resampling, true);
				}

				CSG_String	_Name	= Name;

				const char *s;
				
				if( !!(s = Get_Variable(DataSet, i)) && *s ) _Name  = s;
				if( !!(s = Get_Time    (DataSet, i)) && *s ) _Name += " [" + Get_Time_String(s, tFmt) + "]";
				if( !!(s = Get_Level   (DataSet, i)) && *s ) _Name += " [" + CSG_String(s) + "]";

				pGrid->Set_Name(_Name);

				if( m_bSaveFile )
				{
					_Name.Replace(".", "_");
					_Name.Replace(":", "-");

					pGrid->Save(SG_File_Make_Path(m_SavePath, _Name));

					delete(pGrid);
				}
				else
				{
					m_pGrids->Add_Item(pGrid);
				}
			}

			SG_UI_Progress_Lock(false);
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
