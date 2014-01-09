/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Projection_GeoTRANS                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   GEOTRANS_Base.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "GEOTRANS_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEOTRANS_Base::CGEOTRANS_Base(void)
{
//	CSG_String	Path(SG_File_Get_Path(MLB_Interface.Get_File_Name()));
	CSG_Parameter	*pNode_0, *pNode_Source, *pNode_Target;

	bInitialized	= false;

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL, "DATA_ELLIPSOID"	, _TL("Ellipsoids"),
		_TL(""),
		NULL //, SG_File_Make_Path(Path, "Projection_GeoTRANS_Ellipsoid.dat")
	);

	Parameters.Add_FilePath(
		NULL, "DATA_DATUM_7"	, _TL("Datums (7 Parameters)"),
		_TL(""),
		NULL //, SG_File_Make_Path(Path, "Projection_GeoTRANS_Datum_7.dat")
	);

	Parameters.Add_FilePath(
		NULL, "DATA_DATUM_3"	, _TL("Datums (3 Parameters)"),
		_TL(""),
		NULL //, SG_File_Make_Path(Path, "Projection_GeoTRANS_Datum_3.dat")
	);

	Parameters.Add_FilePath(
		NULL, "DATA_GEOID"		, _TL("Geoid"),
		_TL(""),
		NULL //, SG_File_Make_Path(Path, "Projection_GeoTRANS_Geoid_EGM96.dat")
	);


	//-----------------------------------------------------
	Parameters.Add_Node(NULL, "SOURCE_NODE", _TL("Source Parameters"), _TL(""));
	Parameters.Add_Node(NULL, "TARGET_NODE", _TL("Target Parameters"), _TL(""));


	//-----------------------------------------------------
	pProjection		= Add_Parameters("PROJECTION", _TL("Projection Parameters"), _TL(""));

	pNode_Source	= pProjection->Add_Node(
		NULL		, "SOURCE_NODE"			, _TL("Source Parameters"),
		_TL("")
	);

	pNode_Target	= pProjection->Add_Node(
		NULL		, "TARGET_NODE"			, _TL("Target Parameters"),
		_TL("")
	);

	pNode_0			= pProjection->Add_Choice(
		pNode_Source, "SOURCE_DATUM"		, _TL("Source Datum"),
		_TL(""),
		_TL("[not set]|")
	);

	pNode_0			= pProjection->Add_Choice(
		pNode_Target, "TARGET_DATUM"		, _TL("Target Datum"),
		_TL(""),
		_TL("[not set]|")
	);

	pNode_0			= pProjection->Add_Choice(
		pNode_Source, "SOURCE_PROJECTION"	, _TL("Source Projection"),
		_TL(""),
		_TL("[not set]|")
	);

	pNode_0			= pProjection->Add_Choice(
		pNode_Target, "TARGET_PROJECTION"	, _TL("Target Projection"),
		_TL(""),
		_TL("[not set]|")
	);
}


//---------------------------------------------------------
CGEOTRANS_Base::~CGEOTRANS_Base(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//					Initialization						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Base::Initialize(void)
{
	char		datum_code[10], datum_name[40], system_name[50];
	long		error_code, i, count;
	CSG_String	sList;

	//-----------------------------------------------------
	if( bInitialized
	&&	!fn_Ellipsoid	.Cmp(Parameters("DATA_ELLIPSOID")	->asString())
	&&	!fn_Datum7		.Cmp(Parameters("DATA_DATUM_7")		->asString())
	&&	!fn_Datum3		.Cmp(Parameters("DATA_DATUM_3")		->asString())
	&&	!fn_Geoid		.Cmp(Parameters("DATA_GEOID")		->asString())	)
	{
		return( true );
	}

	bInitialized	= false;

	fn_Ellipsoid	= Parameters("DATA_ELLIPSOID")	->asString();
	fn_Datum7		= Parameters("DATA_DATUM_7")	->asString();
	fn_Datum3		= Parameters("DATA_DATUM_3")	->asString();
	fn_Geoid		= Parameters("DATA_GEOID")		->asString();

	if( Initialize_Engine_File(fn_Ellipsoid.b_str(), fn_Datum7.b_str(), fn_Datum3.b_str(), fn_Geoid.b_str()) != ENGINE_NO_ERROR )
	{
		Error_Set(_TL("GeoTRANS engine initializing error!\n\nPlease check your data path settings!"));
	}

	//-----------------------------------------------------
	else
	{
		if( (error_code = Get_Coordinate_System_Count(&count)) == 0 )
		{
			sList.Clear();

			for(i=0; i<count; i++)
			{
				error_code	= Get_Coordinate_System_Name(i + 1, system_name);
				sList	+= CSG_String(system_name) + SG_T("|");
			}

			if( sList.Length() > 0 )
			{
				((CSG_Parameter_Choice *)pProjection->Get_Parameter("SOURCE_PROJECTION")->Get_Data())->Set_Items(sList);
				((CSG_Parameter_Choice *)pProjection->Get_Parameter("TARGET_PROJECTION")->Get_Data())->Set_Items(sList);
			}
		}
		else
		{
			return( false );
		}

		//-----------------------------------------------------
		if( (error_code = Get_Datum_Count(&count)) == 0 )
		{
			sList.Clear();

			for(i=0; i<count; i++)
			{
				error_code	= Get_Datum_Code(i + 1, datum_code);
				error_code	= Get_Datum_Name(i + 1, datum_name);
				sList	+= CSG_String(datum_code) + SG_T(": ") + datum_name  + SG_T("|");
			}

			if( sList.Length() > 0 )
			{
				((CSG_Parameter_Choice *)pProjection->Get_Parameter("SOURCE_DATUM")->Get_Data())->Set_Items(sList);
				((CSG_Parameter_Choice *)pProjection->Get_Parameter("TARGET_DATUM")->Get_Data())->Set_Items(sList);
			}
		}
		else
		{
			return( false );
		}

		bInitialized	= true;
	}

	return( bInitialized );
}

//---------------------------------------------------------
CSG_Parameters * CGEOTRANS_Base::Get_Projection_Parameters(bool bSource, Coordinate_Type Type, bool bShow_Dialog)
{
	CSG_String		sName, sIdentifier;
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	pParameters	= NULL;

	sName		.Printf(bSource ? _TL("Source Parameters") : _TL("Target Parameters"));
	sIdentifier	.Printf(bSource ? SG_T("SOURCE_") : SG_T("TARGET_"));

	switch( Type )
	{
	//-----------------------------------------------------
	case Albers_Equal_Area_Conic:
		sIdentifier.Append(SG_T("ALBERS"));

		if( (pParameters = Get_Parameters(sIdentifier))					== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Albers Equal Area Conic"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 45.0);
			pParameters->Add_Value		(NULL, "PARALLEL_1"		, _TL("1st Std. Parallel")	, _TL(""), PARAMETER_TYPE_Degree, 40.0);
			pParameters->Add_Value		(NULL, "PARALLEL_2"		, _TL("2nd Std. Parallel")	, _TL(""), PARAMETER_TYPE_Degree, 50.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Azimuthal_Equidistant:
		sIdentifier.Append(SG_T("AZIMUTHAL"));

		if( (pParameters = Get_Parameters(sIdentifier))					== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Azimuthal Equidistant"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Bonne:
		sIdentifier.Append(SG_T("BONNE"));

		if( (pParameters = Get_Parameters(sIdentifier))					== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection"	)		, _TL(""), _TL("Bonne"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude")	, _TL(""), PARAMETER_TYPE_Degree, 45.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Cassini:
		sIdentifier.Append(SG_T("CASSINI"));

		if( (pParameters = Get_Parameters(sIdentifier))					== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Cassini"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Cylindrical_Equal_Area:
		sIdentifier.Append(SG_T("CYLINDRICAL"));

		if( (pParameters = Get_Parameters(sIdentifier))					== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Cylindrical Equal Area"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Eckert4:
	case Eckert6:
		sIdentifier.Append(SG_T("ECKERT"));

		if( (pParameters = Get_Parameters(sIdentifier))					== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Eckert IV/VI"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Equidistant_Cylindrical:
		sIdentifier.Append(SG_T("EQUIDSTCYL"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Equidistant Cylindrical"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "PARALLEL"		, _TL("Standard Parallel")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Geocentric:					// Layout:0*0*3
		sIdentifier.Append(SG_T("GEOCENTRIC"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{}
		break;

	//-----------------------------------------------------
	case Geodetic:	// case height type
		sIdentifier.Append(SG_T("GEODETIC"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(
				NULL, "TYPE"	, _TL("Projection"),
				_TL(""),
				_TL("Geodetic")
			);

			pParameters->Add_Choice(
				NULL, "HEIGHT"	, _TL("Height"),
				_TL(""),

				CSG_String::Format(SG_T("%s|%s|%s|"),
					_TL("No Height"),
					_TL("Ellipsoid Height [m]"),
					_TL("MSL Height [m]")
				), 0
			);
		}
		break;

	//-----------------------------------------------------
	case GEOREF:						// Layout: 0*0*1
		break;

	//-----------------------------------------------------
	case Gnomonic:
		sIdentifier.Append(SG_T("GNOMONIC"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Gnomonic"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Lambert_Conformal_Conic_1:
		sIdentifier.Append(SG_T("LAMBERT_1"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Lambert Conformal Conic (1 parallel)"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 45.0);
			pParameters->Add_Value		(NULL, "SCALE"			, _TL("Scale Factor")		, _TL(""), PARAMETER_TYPE_Degree, 1.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Lambert_Conformal_Conic:
		sIdentifier.Append(SG_T("LAMBERT"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Lambert Conformal Conic (2 parallel)"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 45.0);
			pParameters->Add_Value		(NULL, "PARALLEL_1"		, _TL("1st Std. Parallel")	, _TL(""), PARAMETER_TYPE_Degree, 40.0);
			pParameters->Add_Value		(NULL, "PARALLEL_2"		, _TL("2nd Std. Parallel")	, _TL(""), PARAMETER_TYPE_Degree, 50.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Local_Cartesian:
		sIdentifier.Append(SG_T("LOCALCARTES"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Local Cartesian"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "HEIGHT"			, _TL("Origin Height")		, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "ORIENT"			, _TL("Orientation"	)		, _TL(""), PARAMETER_TYPE_Degree, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Mercator:
		sIdentifier.Append(SG_T("MERCATOR"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Mercator"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "SCALE"			, _TL("Scale Factor")		, _TL(""), PARAMETER_TYPE_Double, 1.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Miller_Cylindrical:
		sIdentifier.Append(SG_T("MILLER"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection"	)		, _TL(""), _TL("Miller Cylindrical"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Mollweide:
		sIdentifier.Append(SG_T("MOLLWEIDE"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Mollweide"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Neys:
		sIdentifier.Append(SG_T("NEYS"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Neys (Modified Lambert Conformal Conic)"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 80.0);
			pParameters->Add_Choice		(NULL, "PARALLEL_1"		, _TL("1st Std. Parallel")	, _TL(""), SG_T(" 71 | 74 |"), 0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case NZMG:							// Layout:0*0*2
		sIdentifier.Append(SG_T("NZMG"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{}
		break;

	//-----------------------------------------------------
	case Oblique_Mercator:
		sIdentifier.Append(SG_T("OBLIQUE_MERCATOR"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Oblique Mercator"));
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude")	, _TL(""), PARAMETER_TYPE_Degree, 45.0);
			pParameters->Add_Value		(NULL, "SCALE"			, _TL("Scale Factor")		, _TL(""), PARAMETER_TYPE_Double, 1.0);
			pParameters->Add_Value		(NULL, "LONGITUDE_1"	, _TL("Longitude 1"	)		, _TL(""), PARAMETER_TYPE_Degree, -5.0);
			pParameters->Add_Value		(NULL, "LATITUDE_1"		, _TL("Latitude 1"	)		, _TL(""), PARAMETER_TYPE_Degree, 40.0);
			pParameters->Add_Value		(NULL, "LONGITUDE_2"	, _TL("Longitude 2"	)		, _TL(""), PARAMETER_TYPE_Degree, 5.0);
			pParameters->Add_Value		(NULL, "LATITUDE_2"		, _TL("Latitude 2"	)		, _TL(""), PARAMETER_TYPE_Degree, 50.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Orthographic:
		sIdentifier.Append(SG_T("ORTHOGRAPH"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection"	)		, _TL(""), _TL("Orthographic"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Polar_Stereo:
		sIdentifier.Append(SG_T("POLARSTEREO"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Polar Stereographic"));
			pParameters->Add_Value		(NULL, "LONGITUDE"		, _TL("Lon. Down")			, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Lat. of True Scale")	, _TL(""), PARAMETER_TYPE_Degree, 90.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Polyconic:
		sIdentifier.Append(SG_T("POLYCONIC"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Polyconic"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Sinusoidal:
		sIdentifier.Append(SG_T("SINUSOIDAL"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Sinusoidal"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Stereographic:
		sIdentifier.Append(SG_T("STEREOGRAPH"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Stereographic"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Transverse_Cylindrical_Equal_Area:
		sIdentifier.Append(SG_T("TRNSVCYLIND"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Transverse Cylindrical Equal Area"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "SCALE"			, _TL("Scale Factor")		, _TL(""), PARAMETER_TYPE_Double, 1.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case Transverse_Mercator:
		sIdentifier.Append(SG_T("TRNSVMERCAT"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection"	)		, _TL(""), _TL("Transverse Mercator"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "LATITUDE"		, _TL("Origin Latitude"	)	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "SCALE"			, _TL("Scale Factor")		, _TL(""), PARAMETER_TYPE_Double, 1.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;

	//-----------------------------------------------------
	case UPS:		//					case Layout:0*0*2		case Hemisphere
		sIdentifier.Append(SG_T("UPS"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Choice		(NULL, "HEMISPHERE"		, _TL("Hemisphere")			, _TL(""), _TL("North|South|"), 0);
		}
		break;

	//-----------------------------------------------------
	case UTM:	// case Hemisphere		case Zone
		sIdentifier.Append(SG_T("UTM"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection")			, _TL(""), _TL("Universal Transvers Mercator (UTM)"));
			pParameters->Add_Value		(NULL, "TOGCHECK"		, _TL("Override")			, _TL(""), PARAMETER_TYPE_Bool, true);
			pParameters->Add_Choice		(NULL, "HEMISPHERE"		, _TL("Hemisphere")			, _TL(""),_TL( "North|South|"), 0);
			pParameters->Add_Value		(NULL, "ZONE"			, _TL("Zone")				, _TL(""), PARAMETER_TYPE_Int, 1, 1, true , 60, true);
		}
		break;

	//-----------------------------------------------------
	case Van_der_Grinten:				//Layout: 1*2*2
		sIdentifier.Append(SG_T("VANGRINTEN"));

		if( (pParameters = Get_Parameters(sIdentifier))				== NULL
		&&	(pParameters = Add_Parameters(sIdentifier, sName, _TL("")))	!= NULL )
		{
			pParameters->Add_Info_String(NULL, "TYPE"			, _TL("Projection"	)		, _TL(""), _TL("Van der Grinten"));
			pParameters->Add_Value		(NULL, "MERIDIAN"		, _TL("Central Meridian")	, _TL(""), PARAMETER_TYPE_Degree, 0.0);
			pParameters->Add_Value		(NULL, "EASTING"		, _TL("False Easting [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
			pParameters->Add_Value		(NULL, "NORTHING"		, _TL("False Northing [m]")	, _TL(""), PARAMETER_TYPE_Double, 0.0);
		}
		break;
	}

	//-----------------------------------------------------
	if( pParameters )
	{
		if( !bShow_Dialog || pParameters->Get_Count() <= 0 )
		{
			return( pParameters );
		}
		else if( Dlg_Parameters(sIdentifier) )
		{
			return( pParameters );
		}
	}

	//-----------------------------------------------------
	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Base::On_Execute(void)
{
	if( !Initialize() )
	{
		Message_Add(_TL("The GeoTRANS engine could not be initialized !!\n\n"));
		Message_Add(_TL("Make sure that the files\n"));
		Message_Add(_TL("Projection_GeoTRANS_Ellipsoid.dat,\n"));
		Message_Add(_TL("Projection_GeoTRANS_Datum_3.dat,\n"));
		Message_Add(_TL("Projection_GeoTRANS_Geoid_EGM96.dat,\n"));
		Message_Add(_TL("Projection_GeoTRANS_Datum_7.dat,\n\n"));
		Message_Add(_TL("are in the specified directory.\n"));

		return( false );
	}

	if( !Dlg_Parameters("PROJECTION") )
	{
		return( false );
	}

	if( Set_Transformation(true) )
	{
		return( On_Execute_Conversion() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Base::Set_Transformation(bool bShow_Dialog)
{
	int			Type_Index, Datum_Index;
	CSG_Parameters	*pParms;

	//-----------------------------------------------------
	// 1. Input...

	Datum_Index	= pProjection->Get_Parameter("SOURCE_DATUM")		->asInt() + 1;
	Type_Index	= pProjection->Get_Parameter("SOURCE_PROJECTION")	->asInt() + 1;

	if(	Get_Coordinate_System_Type(Type_Index, &Type_Input)		!= 0 )
	{
		Message_Dlg(_TL("Source Coordinate System Type Error"));
		return( false );
	}

	if(	Set_Datum(Interactive, Input, Datum_Index)				!= 0 )
	{
		Message_Dlg(_TL("Source Datum Error"));
		return( false );
	}

	if(	Set_Coordinate_System(Interactive, Input, Type_Input)	!= 0 )
	{
		Message_Dlg(_TL("Source Coordinate System Error"));
		return( false );
	}

	if(	(pParms = Get_Projection_Parameters(true, Type_Input, bShow_Dialog))	== NULL )
	{
		Message_Dlg(_TL("Source Parameter List Initialisation Error"));
		return( false );
	}

	if(	Set_Projection_Parameters(Input, Type_Input, pParms)	== false )
	{
		Message_Dlg(_TL("Source Projection Error"));
		return( false );
	}


	//-----------------------------------------------------
	// 2. Output...

	Datum_Index	= pProjection->Get_Parameter("TARGET_DATUM")		->asInt() + 1;
	Type_Index	= pProjection->Get_Parameter("TARGET_PROJECTION")	->asInt() + 1;

	if(	Get_Coordinate_System_Type(Type_Index, &Type_Output)	!= 0 )
	{
		Message_Dlg(_TL("Target Coordinate System Type Error"));
		return( false );
	}

	if(	Set_Datum(Interactive, Output, Datum_Index)				!= 0 )
	{
		Message_Dlg(_TL("Target Datum Error"));
		return( false );
	}

	if(	Set_Coordinate_System(Interactive, Output, Type_Output)	!= 0 )
	{
		Message_Dlg(_TL("Target Coordinate System Error"));
		return( false );
	}

	if(	(pParms = Get_Projection_Parameters(false, Type_Output, bShow_Dialog))	== NULL )
	{
		Message_Dlg(_TL("Target Parameter List Initialisation Error"));
		return( false );
	}

	if(	Set_Projection_Parameters(Output, Type_Output, pParms)	== false )
	{
		Message_Dlg(_TL("Target Projection Error"));
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CGEOTRANS_Base::Set_Transformation_Inverse(void)
{
	long			Datum_Input, Datum_Output;
	Coordinate_Type	Type;
	CSG_Parameters		*pParms_Input, *pParms_Output;

//	if( Set_Transformation(false) )
	{
		pParms_Input	= Get_Projection_Parameters(true , Type_Input , false);
		pParms_Output	= Get_Projection_Parameters(false, Type_Output, false);

		Get_Datum(Interactive,  Input, &Datum_Input);
		Get_Datum(Interactive, Output, &Datum_Output);

		Type			= Type_Input;
		Type_Input		= Type_Output;
		Type_Output		= Type;

		return(	Set_Datum(Interactive,  Input,  Datum_Output)					== 0
			&&	Set_Coordinate_System(Interactive,  Input, Type_Input)			== 0
			&&	Set_Projection_Parameters( Input, Type_Input , pParms_Output)	== true

			&&	Set_Datum(Interactive, Output,  Datum_Input)					== 0
			&&	Set_Coordinate_System(Interactive, Output, Type_Output)			== 0
			&&	Set_Projection_Parameters(Output, Type_Output, pParms_Input)	== true
		);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Base::Set_Projection_Parameters(Input_Output dir, Coordinate_Type Type, CSG_Parameters *pParms)
{
	if( pParms )
	{
		switch( Type )	// Get_Parms()...
		{
		//-------------------------------------------------
		case Albers_Equal_Area_Conic:		// Layout 4*2*2
			{	Albers_Equal_Area_Conic_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.std_parallel_1	= pParms->Get_Parameter("PARALLEL_1")	->asDouble() * M_DEG_TO_RAD;
				params.std_parallel_2	= pParms->Get_Parameter("PARALLEL_2")	->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Albers_Equal_Area_Conic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Azimuthal_Equidistant:			// Layout 2*2*2
			{	Azimuthal_Equidistant_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Azimuthal_Equidistant_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Bonne:							// Layout 2*2*2
			{	Bonne_Parameters						params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Bonne_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case BNG:							// Layout:0*0*1
			break;

		//-------------------------------------------------
		case Cassini:						// Layout 2*2*2
			{	Cassini_Parameters						params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Cassini_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Cylindrical_Equal_Area:		// Layout 2*2*2
			{	Cylindrical_Equal_Area_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Cylindrical_Equal_Area_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Eckert4:						// Layout 1*2*2
			{	Eckert4_Parameters						params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Eckert4_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Eckert6:						// Layout 1*2*2
			{	Eckert6_Parameters						params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Eckert6_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Equidistant_Cylindrical:		// Layout 2*2*2
			{	Equidistant_Cylindrical_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.std_parallel		= pParms->Get_Parameter("PARALLEL")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Equidistant_Cylindrical_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Geocentric:					// Layout:0*0*3
			break;

		//-------------------------------------------------
		case Geodetic:						// Layout:0*0*3		// height type
			{	Geodetic_Parameters						params;

				switch( pParms->Get_Parameter("HEIGHT")->asInt() )
				{
				case 0:	default:
					params.height_type	= No_Height;
					break;

				case 1:
					params.height_type	= Ellipsoid_Height;
					break;

				case 2:
					params.height_type	= Geoid_or_MSL_Height;
					break;
				}

				Set_Geodetic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case GEOREF:						// Layout: 0*0*1
			break;

		//-------------------------------------------------
		case Gnomonic:						// Layout: 2*2*2
			{	Gnomonic_Parameters						params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Gnomonic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Lambert_Conformal_Conic_1:		// Layout: 4*2*2
			{	Lambert_Conformal_Conic_1_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.scale_factor		= pParms->Get_Parameter("SCALE")		->asDouble();
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Lambert_Conformal_Conic_1_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Lambert_Conformal_Conic:		// Layout: 4*2*2
			{	Lambert_Conformal_Conic_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.std_parallel_1	= pParms->Get_Parameter("PARALLEL_1")	->asDouble() * M_DEG_TO_RAD;
				params.std_parallel_2	= pParms->Get_Parameter("PARALLEL_2")	->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Lambert_Conformal_Conic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Local_Cartesian:				// Layout: 3*1*3
			{	Local_Cartesian_Parameters				params;

				params.origin_longitude	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.origin_height	= pParms->Get_Parameter("HEIGHT")		->asDouble();
				params.orientation		= pParms->Get_Parameter("ORIENT")		->asDouble() * M_DEG_TO_RAD;

				Set_Local_Cartesian_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Mercator:						// Layout: 3*2*2
			{	Mercator_Parameters					params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.scale_factor		= pParms->Get_Parameter("SCALE")		->asDouble();
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Mercator_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case MGRS:							// Layout:0*0*1
			break;

		//-------------------------------------------------
		case Miller_Cylindrical:			// Layout:1*2*2
			{	Miller_Cylindrical_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Miller_Cylindrical_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Mollweide:						// Layout:1*2*2
			{	Mollweide_Parameters				params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Mollweide_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Neys:							// Layout: 4*2*2
			{	Neys_Parameters						params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;

				switch( pParms->Get_Parameter("PARALLEL_1")->asInt() )
				{
				case 0:	default:
					params.std_parallel_1 = 71.0 * M_DEG_TO_RAD;
					break;

				case 1:
					params.std_parallel_1 = 74.0 * M_DEG_TO_RAD;
					break;
				}

				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Neys_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case NZMG:							// Layout:0*0*2
			break;

		//-------------------------------------------------
		case Oblique_Mercator:				// Layout:4*4*2
			{	Oblique_Mercator_Parameters			params;

				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.scale_factor		= pParms->Get_Parameter("SCALE")		->asDouble();
				params.longitude_1		= pParms->Get_Parameter("LONGITUDE_1")	->asDouble() * M_DEG_TO_RAD;
				params.latitude_1		= pParms->Get_Parameter("LATITUDE_1")	->asDouble() * M_DEG_TO_RAD;
				params.longitude_2		= pParms->Get_Parameter("LONGITUDE_2")	->asDouble() * M_DEG_TO_RAD;
				params.latitude_2		= pParms->Get_Parameter("LATITUDE_2")	->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Oblique_Mercator_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Orthographic:					// Layout:2*2*2
			{	Orthographic_Parameters				params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Orthographic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Polar_Stereo:					// Layout:2*2*2
			{	Polar_Stereo_Parameters				params;

				params.longitude_down_from_pole	= pParms->Get_Parameter("LONGITUDE")->asDouble() * M_DEG_TO_RAD;
				params.latitude_of_true_scale	= pParms->Get_Parameter("LATITUDE")	->asDouble() * M_DEG_TO_RAD;
				params.false_easting			= pParms->Get_Parameter("EASTING")	->asDouble();
				params.false_northing			= pParms->Get_Parameter("NORTHING")	->asDouble();

				Set_Polar_Stereo_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Polyconic:						// Layout:2*2*2
			{	Polyconic_Parameters				params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Polyconic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Sinusoidal:					// Layout:1*2*2
			{	Sinusoidal_Parameters				params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Sinusoidal_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Stereographic:					// Layout 2*2*2
			{	Stereographic_Parameters			params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Stereographic_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Transverse_Cylindrical_Equal_Area:	// Layout:3*2*2
			{	Transverse_Cylindrical_Equal_Area_Parameters	params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.scale_factor		= pParms->Get_Parameter("SCALE")		->asDouble();
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Transverse_Cylindrical_Equal_Area_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Transverse_Mercator:			// Layout:3*2*2
			{	Transverse_Mercator_Parameters	params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.origin_latitude	= pParms->Get_Parameter("LATITUDE")		->asDouble() * M_DEG_TO_RAD;
				params.scale_factor		= pParms->Get_Parameter("SCALE")		->asDouble();
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Transverse_Mercator_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case UPS:							// Layout:0*0*2		// Hemisphere
			{
				flag_Hemisphere	= pParms->Get_Parameter("HEMISPHERE")->asInt() == 1 ? 'S' : 'N';
			}
			break;

		//-------------------------------------------------
		case UTM:							// Layout:0*0*2		// Hemisphere		// Zone
			{	UTM_Parameters					params;

				flag_Hemisphere	= pParms->Get_Parameter("HEMISPHERE")	->asInt() == 1 ? 'S' : 'N';
				UTM_Zone		= pParms->Get_Parameter("ZONE")			->asInt();

				if( pParms->Get_Parameter("TOGCHECK")->asBool() && UTM_Zone >= 1 && UTM_Zone <= 60 )
				{
					params.override	= 1;
					params.zone		= UTM_Zone;
				}
				else
				{
					params.override	= 0;
					params.zone		= UTM_Zone	= 0;
				}

				Set_UTM_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		case Van_der_Grinten:				//Layout: 1*2*2
			{	Van_der_Grinten_Parameters		params;

				params.central_meridian	= pParms->Get_Parameter("MERIDIAN")		->asDouble() * M_DEG_TO_RAD;
				params.false_easting	= pParms->Get_Parameter("EASTING")		->asDouble();
				params.false_northing	= pParms->Get_Parameter("NORTHING")		->asDouble();

				Set_Van_der_Grinten_Params(Interactive, dir, params);
				break;
			}

		//-------------------------------------------------
		default:
			{
				return( false );
			}
		}	// end of switch...

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
bool CGEOTRANS_Base::Get_Converted(double &x, double &y, double &z)
{
	return( Convert_Set(x, y, z) && Convert(Interactive) == 0 && Convert_Get(x, y, z) );
}

bool CGEOTRANS_Base::Get_Converted(TSG_Point &Point)
{
	return( Get_Converted(Point.x, Point.y) );
}

bool CGEOTRANS_Base::Get_Converted(double &x, double &y)
{
	return( Convert_Set(x, y) && Convert(Interactive) == 0 && Convert_Get(x, y) );
}

//---------------------------------------------------------
bool CGEOTRANS_Base::Convert_Set(double x, double y, double z)
{
	long	error_code	= -1;

	if( bInitialized )
	{
		switch( Type_Input )
		{
		default:
			return( Convert_Set(x, y) );

		case Geocentric:
			{
				Geocentric_Tuple coord;
				coord.x				= x;
				coord.y				= y;
				coord.z				= z;
				error_code			= Set_Geocentric_Coordinates(Interactive, Input, coord);
				break;
			}

		case Geodetic:
			{
				Geodetic_Tuple coord;
				coord.longitude		= x * M_DEG_TO_RAD;
				coord.latitude		= y * M_DEG_TO_RAD;
				coord.height		= z;
				error_code			= Set_Geodetic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Local_Cartesian:
			{
				Local_Cartesian_Tuple coord;
				coord.x				= x;
				coord.y				= y;
				coord.z				= z;
				error_code			= Set_Local_Cartesian_Coordinates(Interactive, Input, coord);
				break;
			}
		}
	}

	return( error_code == 0 );
}

//---------------------------------------------------------
bool CGEOTRANS_Base::Convert_Set(double x, double y)
{
	long	error_code	= -1;

	if( bInitialized )
	{
		switch( Type_Input )
		{
		default:	// case BNG: case GEOREF: case MGRS:
			return( false );

		case Geocentric:
		case Geodetic:
		case Local_Cartesian:
			return( Convert_Set(x, y, 0.0) );

		case Albers_Equal_Area_Conic:
			{
				Albers_Equal_Area_Conic_Tuple	coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Albers_Equal_Area_Conic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Azimuthal_Equidistant:
			{
				Azimuthal_Equidistant_Tuple		coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Azimuthal_Equidistant_Coordinates(Interactive, Input, coord);
				break;
			}

		case Bonne:
			{
				Bonne_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Bonne_Coordinates(Interactive, Input, coord);
				break;
			}

		case Cassini:
			{
				Cassini_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Cassini_Coordinates(Interactive, Input, coord);
				break;
			}
	
		case Cylindrical_Equal_Area:
			{
				Cylindrical_Equal_Area_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Cylindrical_Equal_Area_Coordinates(Interactive, Input, coord);
				break;
			}

		case Eckert4:
			{
				Eckert4_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Eckert4_Coordinates(Interactive, Input, coord);
				break;
			}

		case Eckert6:
			{
				Eckert6_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Eckert6_Coordinates(Interactive, Input, coord);
				break;
			}

		case Equidistant_Cylindrical:
			{
				Equidistant_Cylindrical_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Equidistant_Cylindrical_Coordinates(Interactive, Input, coord);
				break;
			}

		case Gnomonic:
			{
				Gnomonic_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Gnomonic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Lambert_Conformal_Conic_1:
			{
				Lambert_Conformal_Conic_1_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Lambert_Conformal_Conic_1_Coordinates(Interactive, Input, coord);
				break;
			}

		case Lambert_Conformal_Conic:
			{
				Lambert_Conformal_Conic_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Lambert_Conformal_Conic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Mercator:
			{
				Mercator_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Mercator_Coordinates(Interactive, Input, coord);
				break;
			}

		case Miller_Cylindrical:
			{
				Miller_Cylindrical_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Miller_Cylindrical_Coordinates(Interactive, Input, coord);
				break;
			}

		case Mollweide:
			{
				Mollweide_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Mollweide_Coordinates(Interactive, Input, coord);
				break;
			}

		case Neys:
			{
				Neys_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Neys_Coordinates(Interactive, Input, coord);
				break;
			}

		case NZMG:
			{
				NZMG_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_NZMG_Coordinates(Interactive, Input, coord);
				break;
			}

		case Oblique_Mercator:
			{
				Oblique_Mercator_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Oblique_Mercator_Coordinates(Interactive, Input, coord);
				break;
			}

		case Orthographic:
			{
				Orthographic_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Orthographic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Polar_Stereo:
			{
				Polar_Stereo_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Polar_Stereo_Coordinates(Interactive, Input, coord);
				break;
			}

		case Polyconic:
			{
				Polyconic_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Polyconic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Sinusoidal:
			{
				Sinusoidal_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Sinusoidal_Coordinates(Interactive, Input, coord);
				break;
			}

		case Stereographic:
			{
				Stereographic_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Stereographic_Coordinates(Interactive, Input, coord);
				break;
			}

		case Transverse_Cylindrical_Equal_Area:
			{
				Transverse_Cylindrical_Equal_Area_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Transverse_Cylindrical_Equal_Area_Coordinates(Interactive, Input, coord);
				break;
			}

		case Transverse_Mercator:
			{
				Transverse_Mercator_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Transverse_Mercator_Coordinates(Interactive, Input, coord);
				break;
			}

		case UPS:
			{
				UPS_Tuple coord;
				coord.hemisphere	= flag_Hemisphere;	// Hemisphere...
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_UPS_Coordinates(Interactive, Input, coord);
				break;
			}

		case UTM:
			{
				UTM_Tuple coord;
				coord.hemisphere	= flag_Hemisphere;	// Hemisphere...
				coord.zone			= UTM_Zone;			// UTM-Zone...
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_UTM_Coordinates(Interactive, Input, coord);
				break;
			}

		case Van_der_Grinten:
			{
				Van_der_Grinten_Tuple coord;
				coord.easting		= x;
				coord.northing		= y;
				error_code			= Set_Van_der_Grinten_Coordinates(Interactive, Input, coord);
				break;
			}
		}
	}

	return( error_code == 0 );
}

//---------------------------------------------------------
bool CGEOTRANS_Base::Convert_Get(double &x, double &y, double &z)
{
	long	error_code	= -1;

	if( bInitialized )
	{
		switch( Type_Output )
		{
		default:
			return( Convert_Get(x, y) );

		case Geocentric:
			{
				Geocentric_Tuple coord;
				error_code	= Get_Geocentric_Coordinates(Interactive, Output, &coord);
				x			= coord.x;
				y			= coord.y;
				z			= coord.z;
				break;
			}

		case Geodetic:
			{
				Geodetic_Tuple coord;
				error_code	= Get_Geodetic_Coordinates(Interactive, Output, &coord);
				x			= coord.longitude	* M_RAD_TO_DEG;
				y			= coord.latitude	* M_RAD_TO_DEG;
				z			= coord.height;
				break;
			}

		case Local_Cartesian:
			{
				Local_Cartesian_Tuple coord;
				error_code	= Get_Local_Cartesian_Coordinates(Interactive, Output, &coord);
				x			= coord.x;
				y			= coord.y;
				z			= coord.z;
				break;
			}
		}
	}

	return( error_code == 0 );
}


//---------------------------------------------------------
bool CGEOTRANS_Base::Convert_Get(double &x, double &y)
{
	long	error_code	= -1;

	if( bInitialized )
	{
		switch( Type_Output )
		{
		default:	// case BNG: case GEOREF: case MGRS:
			return( false );

		case Geocentric:
		case Geodetic:
		case Local_Cartesian:
			double	z;
			return( Convert_Get(x, y, z) );

		case Albers_Equal_Area_Conic:
			{
				Albers_Equal_Area_Conic_Tuple	coord;
				error_code	= Get_Albers_Equal_Area_Conic_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Azimuthal_Equidistant:
			{
				Azimuthal_Equidistant_Tuple		coord;
				error_code	= Get_Azimuthal_Equidistant_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Bonne:
			{
				Bonne_Tuple coord;
				error_code	= Get_Bonne_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Cassini:
			{
				Cassini_Tuple coord;
				error_code	= Get_Cassini_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}
	
		case Cylindrical_Equal_Area:
			{
				Cylindrical_Equal_Area_Tuple coord;
				error_code	= Get_Cylindrical_Equal_Area_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Eckert4:
			{
				Eckert4_Tuple coord;
				error_code	= Get_Eckert4_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Eckert6:
			{
				Eckert6_Tuple coord;
				error_code	= Get_Eckert6_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Equidistant_Cylindrical:
			{
				Equidistant_Cylindrical_Tuple coord;
				error_code	= Get_Equidistant_Cylindrical_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Gnomonic:
			{
				Gnomonic_Tuple coord;
				error_code	= Get_Gnomonic_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Lambert_Conformal_Conic_1:
			{
				Lambert_Conformal_Conic_1_Tuple coord;
				error_code	= Get_Lambert_Conformal_Conic_1_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Lambert_Conformal_Conic:
			{
				Lambert_Conformal_Conic_Tuple coord;
				error_code	= Get_Lambert_Conformal_Conic_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Mercator:
			{
				Mercator_Tuple coord;
				error_code	= Get_Mercator_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Miller_Cylindrical:
			{
				Miller_Cylindrical_Tuple coord;
				error_code	= Get_Miller_Cylindrical_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Mollweide:
			{
				Mollweide_Tuple coord;
				error_code	= Get_Mollweide_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Neys:
			{
				Neys_Tuple coord;
				error_code	= Get_Neys_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case NZMG:
			{
				NZMG_Tuple coord;
				error_code	= Get_NZMG_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Oblique_Mercator:
			{
				Oblique_Mercator_Tuple coord;
				error_code	= Get_Oblique_Mercator_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Orthographic:
			{
				Orthographic_Tuple coord;
				error_code	= Get_Orthographic_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Polar_Stereo:
			{
				Polar_Stereo_Tuple coord;
				error_code	= Get_Polar_Stereo_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Polyconic:
			{
				Polyconic_Tuple coord;
				error_code	= Get_Polyconic_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Sinusoidal:
			{
				Sinusoidal_Tuple coord;
				error_code	= Get_Sinusoidal_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Stereographic:
			{
				Stereographic_Tuple coord;
				error_code	= Get_Stereographic_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Transverse_Cylindrical_Equal_Area:
			{
				Transverse_Cylindrical_Equal_Area_Tuple coord;
				error_code	= Get_Transverse_Cylindrical_Equal_Area_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Transverse_Mercator:
			{
				Transverse_Mercator_Tuple coord;
				error_code	= Get_Transverse_Mercator_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case UPS:
			{
				UPS_Tuple coord;
				error_code	= Get_UPS_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case UTM:
			{
				UTM_Tuple coord;
				error_code	= Get_UTM_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}

		case Van_der_Grinten:
			{
				Van_der_Grinten_Tuple coord;
				error_code	= Get_Van_der_Grinten_Coordinates(Interactive, Output, &coord);
				x			= coord.easting;
				y			= coord.northing;
				break;
			}
		}
	}

	return( error_code == 0 );
}
