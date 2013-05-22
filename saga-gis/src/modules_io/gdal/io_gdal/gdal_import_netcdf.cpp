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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

	//-----------------------------------------------------
	CSG_Parameter	*pNode;
	CSG_Parameters	*pParameters	= Add_Parameters("BANDS", _TL("Import NetCDF"), _TL(""));

	pParameters->Add_Info_String(
		NULL	, "METADATA"	, _TL("Metadata"),
		_TL(""),
		"", true
	);

	pNode	= pParameters->Add_Node(
		NULL	, "VARS_NODE"	, _TL("Variables"),
		_TL("")
	);

	pParameters->Add_Value(
		pNode	, "VARS_ALL"	, _TL("All"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	pParameters->Add_Parameters(
		pNode	, "VARS"		, _TL("Selection"),
		_TL("")
	);

	pNode	= pParameters->Add_Node(
		NULL	, "TIME_NODE"	, _TL("Times"),
		_TL("")
	);

	pParameters->Add_Value(
		pNode	, "TIME_ALL"	, _TL("All"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	pParameters->Add_Parameters(
		pNode	, "TIME"		, _TL("Selection"),
		_TL("")
	);

	pNode	= pParameters->Add_Node(
		NULL	, "LEVEL_NODE"	, _TL("Levels"),
		_TL("")
	);

	pParameters->Add_Value(
		pNode	, "LEVEL_ALL"	, _TL("All"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	pParameters->Add_Parameters(
		pNode	, "LEVEL"		, _TL("Selection"),
		_TL("")
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
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("VARS_ALL" )) )
	{
		pParameters->Get_Parameter("VARS" )->Set_Enabled(!pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TIME_ALL" )) )
	{
		pParameters->Get_Parameter("TIME" )->Set_Enabled(!pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LEVEL_ALL")) )
	{
		pParameters->Get_Parameter("LEVEL")->Set_Enabled(!pParameter->asBool());
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
bool CGDAL_Import_NetCDF::On_Execute(void)
{
	const char				*s;
	int						i;
	TSG_Time_Format			tFmt	= SG_TIME_FMT_Hours_AD;
	CSG_Parameters			&P		= *Get_Parameters("BANDS"), *pVars, *pTime, *pLevel;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_GDAL_DataSet		DataSet;

	//-----------------------------------------------------
	pGrids	= Parameters("GRIDS")->asGridList();
	pGrids	->Del_Items();

	//-----------------------------------------------------
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

	DataSet.Get_MetaData(MetaData);

	P("METADATA")->Set_Value(MetaData.asText());

	//-----------------------------------------------------
	if( DataSet.Get_Count() <= 0 && DataSet.Get_MetaData(MetaData, "SUBDATASETS") )
	{
//		DataSet.Get_MetaData("SUBDATASETS")
	}

	//-----------------------------------------------------
	(pVars	= P("VARS" )->asParameters())->Del_Parameters();
	(pTime	= P("TIME" )->asParameters())->Del_Parameters();
	(pLevel	= P("LEVEL")->asParameters())->Del_Parameters();

	for(i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
	{
		if( (s = Get_Variable(DataSet, i)) != NULL && !pVars ->Get_Parameter(s) )
			pVars  ->Add_Value(NULL, s, s, _TL(""), PARAMETER_TYPE_Bool, false);

		if( (s = Get_Time    (DataSet, i)) != NULL && !pTime ->Get_Parameter(s) )
			pTime  ->Add_Value(NULL, s, CSG_Time_Converter::Get_String(atoi(s), tFmt), _TL(""), PARAMETER_TYPE_Bool, false);

		if( (s = Get_Level   (DataSet, i)) != NULL && !pLevel->Get_Parameter(s) )
			pLevel ->Add_Value(NULL, s, s, _TL(""), PARAMETER_TYPE_Bool, false);
	}

	//-----------------------------------------------------
	if( pVars ->Get_Count() <= 0 )
	{
		if( !Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("no variable information"), Parameters("FILE")->asString())) )
		{
			return( false );
		}

		pVars	= NULL;
	}

	if( pTime ->Get_Count() <= 0 )
	{
		if( !Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("no time stamp information"), Parameters("FILE")->asString())) )
		{
			return( false );
		}

		pTime	= NULL;
	}

	if( pLevel->Get_Count() <= 0 )
	{
		if( !Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("no level information"), Parameters("FILE")->asString())) )
		{
			return( false );
		}

		pLevel	= NULL;
	}

	//-----------------------------------------------------
	if( !Dlg_Parameters(&P, _TL("Import NetCDF")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( P("VARS_ALL" )->asBool() )	pVars	= NULL;
	if( P("TIME_ALL" )->asBool() )	pTime	= NULL;
	if( P("LEVEL_ALL")->asBool() )	pLevel	= NULL;

	//-----------------------------------------------------
	for(i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
	{
		CSG_Parameter	*pParm;

		if( (!pVars  || (!!(pParm = (*pVars )(Get_Variable(DataSet, i))) && pParm->asBool()))
		&&	(!pTime  || (!!(pParm = (*pTime )(Get_Time    (DataSet, i))) && pParm->asBool()))
		&&	(!pLevel || (!!(pParm = (*pLevel)(Get_Level   (DataSet, i))) && pParm->asBool())) )
		{
			SG_UI_Progress_Lock(true);

			CSG_Grid	*pGrid	= DataSet.Read(i);

			if( pGrid )
			{
				CSG_String	Name(_TL("unknown"));	if( (s = Get_Variable(DataSet, i)) != NULL )	Name	= s;

				if( (s = Get_Time (DataSet, i)) != NULL && *s )
				{
					Name	+= " [" + CSG_Time_Converter::Get_String(atoi(s), tFmt) + "]";
				}

				if( (s = Get_Level(DataSet, i)) != NULL && *s )
				{
					Name	+= " [" + CSG_String(s) + "]";
				}

				pGrid->Set_Name(Name);

				pGrids->Add_Item(pGrid);
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
