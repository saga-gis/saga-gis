/**********************************************************
 * Version $Id: gdal_import_netcdf.cpp 1379 2012-04-26 11:58:47Z manfred-e $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("GDAL: Import NetCDF"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"This module imports grids NetCDF Format using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.nc|%s|*.*"),
			_TL("NetCDF Files (*.nc)"),
			_TL("All Files")
		), NULL, false
	);

	pNode	= Parameters.Add_Value(
		NULL	, "SAVE_FILE"	, _TL("Save to File"),
		_TL("save output to file instead of memory"),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_FilePath(
		pNode	, "SAVE_PATH"	, _TL("Save to Path"),
		_TL(""),
		NULL, NULL, true, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Value(
		NULL	, "TRANSFORM"	, _TL("Transformation"),
		_TL("apply coordinate transformation if appropriate"),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		pNode	, "INTERPOL"	, _TL("Interpolation"),
		_TL("interpolation method to use if grid needs to be aligned to coordinate system"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import_NetCDF::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SAVE_FILE")) )
	{
		pParameters->Get_Parameter("SAVE_PATH")->Set_Enabled(pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("VARS_ALL" )) && pParameters->Get_Parameter("VARS") )
	{
		pParameters->Get_Parameter("VARS" )->Set_Enabled(!pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TIME_ALL" )) && pParameters->Get_Parameter("TIME") )
	{
		pParameters->Get_Parameter("TIME" )->Set_Enabled(!pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LEVEL_ALL")) && pParameters->Get_Parameter("LEVEL") )
	{
		pParameters->Get_Parameter("LEVEL")->Set_Enabled(!pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TRANSFORM")) )
	{
		pParameters->Get_Parameter("INTERPOL")->Set_Enabled(pParameter->asBool());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CGDAL_Import_NetCDF::Get_Time_String(const CSG_String &Time, int Format)
{
	switch( Format )
	{
	case  0:	return( CSG_Time_Converter::Get_String(Time.asInt(), SG_TIME_FMT_Hours_AD) );

	default:	return( Time );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
		Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("could not open file"), Parameters("FILE")->asString()));

		return( false );
	}

	if( DataSet.Get_DriverID().Cmp("netCDF") )
	{
		Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("invalid NetCDF file"), Parameters("FILE")->asString()));

		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	MetaData;

	if( DataSet.Get_Count() <= 0 && DataSet.Get_MetaData(MetaData, "SUBDATASETS") )
	{
		int	i, n;

		for(i=0, n=0; i==n; i++)
		{
			CSG_MetaData	*pEntry	= MetaData.Get_Child(CSG_String::Format(SG_T("SUBDATASET_%d_NAME"), i + 1));

			if( pEntry && DataSet.Open_Read(pEntry->Get_Content()) )
			{
				CSG_String	Desc	= _TL("unknown");

				if( (pEntry = MetaData.Get_Child(CSG_String::Format(SG_T("SUBDATASET_%d_DESC"), i + 1))) != NULL )
				{
					Desc	= pEntry->Get_Content();
				}

				if( Load(DataSet, Desc) )
				{
					n++;
				}
			}
		}

		return( n > 0 );
	}

	return( Load(DataSet, SG_File_Get_Name(Parameters("FILE")->asString(), false)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_NetCDF::Load(CSG_GDAL_DataSet &DataSet, const CSG_String &Description)
{
	const char	*s;
	int			i;

	//-----------------------------------------------------
	CSG_MetaData	MetaData, *pChild;

	DataSet.Get_MetaData(MetaData);

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
	if( SG_UI_Get_Window_Main() )
	{
		CSG_Parameter	*pNode;

		P.Create(this, _TL("Import NetCDF"), _TL(""));
		P.Set_Callback_On_Parameter_Changed(Get_Parameter_Changed());

		P.Add_Info_String(NULL , "METADATA", _TL("Metadata"), _TL(""), MetaData.asText(), true);

		pNode	= P.Add_Value     (NULL , "VARS_ALL"  , _TL("All Variables"), _TL(""), PARAMETER_TYPE_Bool, true);
		pVars	= P.Add_Parameters(pNode, "VARS"      , _TL("Selection")    , _TL(""))->asParameters();

		pNode	= P.Add_Value     (NULL , "TIME_ALL"  , _TL("All Times")    , _TL(""), PARAMETER_TYPE_Bool, true);
		pTime	= P.Add_Parameters(pNode, "TIME"      , _TL("Selection")    , _TL(""))->asParameters();

		pNode	= P.Add_Value     (NULL , "LEVEL_ALL" , _TL("All Levels")   , _TL(""), PARAMETER_TYPE_Bool, true);
		pLevel	= P.Add_Parameters(pNode, "LEVEL"     , _TL("Selection")    , _TL(""))->asParameters();

		for(i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
		{
			if( (s = Get_Variable(DataSet, i)) != NULL && !pVars ->Get_Parameter(s) )
				pVars  ->Add_Value(NULL, s, s, _TL(""), PARAMETER_TYPE_Bool, false);

			if( (s = Get_Time    (DataSet, i)) != NULL && !pTime ->Get_Parameter(s) )
				pTime  ->Add_Value(NULL, s, Get_Time_String(s, tFmt), _TL(""), PARAMETER_TYPE_Bool, false);

			if( (s = Get_Level   (DataSet, i)) != NULL && !pLevel->Get_Parameter(s) )
				pLevel ->Add_Value(NULL, s, s, _TL(""), PARAMETER_TYPE_Bool, false);
		}

		P("VARS_ALL" )->Set_Enabled(pVars ->Get_Count() > 1);
		P("TIME_ALL" )->Set_Enabled(pTime ->Get_Count() > 1);
		P("LEVEL_ALL")->Set_Enabled(pLevel->Get_Count() > 1);

		//-------------------------------------------------
		if( !Dlg_Parameters(&P, CSG_String::Format(SG_T("%s: %s"), _TL("Import NetCDF"), Description.c_str())) )
		{
			return( false );
		}

		//-------------------------------------------------
		if( !P("VARS_ALL" )->is_Enabled() || P("VARS_ALL" )->asBool() )	pVars	= NULL;
		if( !P("TIME_ALL" )->is_Enabled() || P("TIME_ALL" )->asBool() )	pTime	= NULL;
		if( !P("LEVEL_ALL")->is_Enabled() || P("LEVEL_ALL")->asBool() )	pLevel	= NULL;
	}

	//-----------------------------------------------------
	TSG_Grid_Interpolation	Interpolation;

	switch( Parameters("INTERPOL")->asInt() )
	{
	default:
	case 0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
	case 1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
	case 2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
	case 3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
	case 4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
	}

	bool	bTransform	= Parameters("TRANSFORM")->asBool() && DataSet.Needs_Transformation();

	//-----------------------------------------------------
	for(i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
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
					Process_Set_Text(CSG_String::Format(SG_T("%s [%d/%d]"), _TL("band transformation"), i + 1, DataSet.Get_Count()));

					DataSet.Get_Transformation(&pGrid, Interpolation, true);
				}

				CSG_String	Name(_TL("unknown"));
				
				if( (s = Get_Variable(DataSet, i)) != NULL && *s )	Name	= s;
				if( (s = Get_Time    (DataSet, i)) != NULL && *s )	Name	+= " [" + Get_Time_String(s, tFmt) + "]";
				if( (s = Get_Level   (DataSet, i)) != NULL && *s )	Name	+= " [" + CSG_String(s) + "]";

				pGrid->Set_Name(Name);

				if( m_bSaveFile )
				{
					Name.Replace(".", "_");
					Name.Replace(":", "-");

					pGrid->Save(SG_File_Make_Path(m_SavePath, Name));

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
