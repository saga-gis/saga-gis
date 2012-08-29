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

#include <gdal_priv.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <time.h>

//---------------------------------------------------------
enum
{
	SG_TIME_UNIT_Seconds_Unix	= 0,
	SG_TIME_UNIT_Hours_AD
};

//---------------------------------------------------------
CSG_String	SG_Get_Time_Str	(int Time, int Unit)
{
	CSG_String	s;

	switch( Unit )
	{
	case SG_TIME_UNIT_Seconds_Unix:
		{
			struct tm*	t;
			time_t		tUnix	= Time;
			
			#ifdef _SAGA_LINUX
			t = gmtime(&tUnix);
			#else
			gmtime_s(t, &tUnix);
			#endif
			
			s.Printf(SG_T("%04d.%02d.%02d %02d:%02d:%02d"), t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

		}
		break;

	case SG_TIME_UNIT_Hours_AD:
		{
			struct tm	t;
			double	d	= 1721424.0 + (Time - 12.0) / 24.0;
			long	n, l, jd	= long(d);		// Truncate to integral day
			double	frac	= double(d) - jd + 0.5;	// Fractional part of calendar day

			if( frac >= 1.0 )	// Is it really the next calendar day?
			{
				frac	--;
				jd		++;
			}

			frac		= 24.0 * (frac);
			t.tm_hour	= (int)frac;
			frac		= 60.0 * (frac - t.tm_hour);
			t.tm_min	= (int)frac;
			frac		= 60.0 * (frac - t.tm_min);
			t.tm_sec	= (int)frac;

			l			= jd + 68569;
			n			= 4 * l / 146097l;
			l			= l - (146097 * n + 3l) / 4;
			t.tm_year	= 4000 * (l + 1) / 1461001;
			l			= l - 1461 * t.tm_year / 4 + 31;	// 1461 = 365.25 * 4
			t.tm_mon	= 80 * l / 2447;
			t.tm_mday	= l - 2447 * t.tm_mon / 80;
			l			= t.tm_mon / 11;
			t.tm_mon	= t.tm_mon + 2 - 12 * l;
			t.tm_year	= 100 * (n - 49) + t.tm_year + l;

			s.Printf(SG_T("%04d.%02d.%02d %02d"), t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour);
		}
		break;
	}

	//-----------------------------------------------------
	return( s );
}


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
bool CGDAL_Import_NetCDF::On_Execute(void)
{
	const char				*s;
	int						i, tType	= SG_TIME_UNIT_Hours_AD;
	CSG_Parameter			*pNode;
	CSG_Parameters			&P = *Get_Parameters("BANDS"), *pVars, *pTime, *pLevel;
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

	if( SG_STR_CMP(DataSet.Get_Driver()->GetDescription(), "netCDF") )
	{
		Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("invalid NetCDF file"), Parameters("FILE")->asString()));

		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	MetaData;
	CSG_String		sMetaData;

	if( DataSet.Get_MetaData(MetaData) )
	{
		for(int i=0; i<MetaData.Get_Children_Count(); i++)
		{
			sMetaData	+= MetaData.Get_Child(i)->Get_Name() + ":\t" + MetaData.Get_Child(i)->Get_Content() + "\n";
		}
	}

	P("METADATA")->Set_Value(sMetaData);

	//-----------------------------------------------------
	(pVars	= P("VARS" )->asParameters())->Del_Parameters();
	(pTime	= P("TIME" )->asParameters())->Del_Parameters();
	(pLevel	= P("LEVEL")->asParameters())->Del_Parameters();

	for(i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
	{
		if( !pVars ->Get_Parameter(s = DataSet.Get_MetaData_Item(i, "NETCDF_VARNAME")) )
			pVars  ->Add_Value(NULL, s, s, _TL(""), PARAMETER_TYPE_Bool, false);

		if( !pTime ->Get_Parameter(s = DataSet.Get_MetaData_Item(i, "NETCDF_DIMENSION_time")) )
			pTime  ->Add_Value(NULL, s, SG_Get_Time_Str(atoi(s), tType), _TL(""), PARAMETER_TYPE_Bool, false);

		if( !pLevel->Get_Parameter(s = DataSet.Get_MetaData_Item(i, "NETCDF_DIMENSION_level")) )
			pLevel ->Add_Value(NULL, s, s, _TL(""), PARAMETER_TYPE_Bool, false);
	}

	//-----------------------------------------------------
	if( pVars->Get_Count() <= 0 || pTime->Get_Count() <= 0 || pLevel->Get_Count() <= 0 )
	{
		Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("could not find any band data"), Parameters("FILE")->asString()));

		return( false );
	}
	
	if( !Dlg_Parameters(&P, _TL("Import NetCDF")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool	bAll_Vars	= P("VARS_ALL" )->asBool();
	bool	bAll_Time	= P("TIME_ALL" )->asBool();
	bool	bAll_Level	= P("LEVEL_ALL")->asBool();

	for(i=0; i<DataSet.Get_Count() && Set_Progress(i, DataSet.Get_Count()); i++)
	{
		CSG_Parameter	*pParm;

		if( (bAll_Vars  || (!!(pParm = (*pVars )(DataSet.Get_MetaData_Item(i, "NETCDF_VARNAME"        ))) && pParm->asBool()))
		&&	(bAll_Time  || (!!(pParm = (*pTime )(DataSet.Get_MetaData_Item(i, "NETCDF_DIMENSION_time" ))) && pParm->asBool()))
		&&	(bAll_Level || (!!(pParm = (*pLevel)(DataSet.Get_MetaData_Item(i, "NETCDF_DIMENSION_level"))) && pParm->asBool())) )
		{
			SG_UI_Progress_Lock(true);

			CSG_Grid	*pGrid	= DataSet.Read(i);

			if( pGrid )
			{
				pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s] [%s]"),
					CSG_String(          DataSet.Get_MetaData_Item(i, "NETCDF_VARNAME"        )        ).c_str(),
					SG_Get_Time_Str(atoi(DataSet.Get_MetaData_Item(i, "NETCDF_DIMENSION_time" )), tType).c_str(),
					CSG_String(          DataSet.Get_MetaData_Item(i, "NETCDF_DIMENSION_level")        ).c_str()
				));

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
