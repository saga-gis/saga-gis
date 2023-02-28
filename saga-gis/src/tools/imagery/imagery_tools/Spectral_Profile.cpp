
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Tutorial                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Spectral_Profile.cpp                 //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
#include "Spectral_Profile.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FIELD_ID = 0,
	FIELD_NAME,
	FIELD_LENGTH,
	FIELD_VALUES
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSpectral_Profile::CSpectral_Profile(void)
{
	Set_Name		(_TL("Spectral Profile"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	m_pBands = Parameters.Add_Grid_List("",
		"BANDS"     , _TL("Spectral Bands"),
		_TL(""),
		PARAMETER_INPUT, false
	)->asGridList();

	Parameters.Add_Shapes("",
		"LOCATION"  , _TL("Profile Location"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table("",
		"PROFILE"   , _TL("Spectral Profile"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_String("",
		"LENGTHS"   , _TL("Wave Lengths"),
		_TL(""),
		"0.485 0.56 0.66 0.83 1.65 2.215 11.45"
	);

	Parameters.Add_Choice("LENGTHS",
		"PREDEFS"   , _TL("Predefined"),
		_TL(""),
		CSG_String::Format("<%s>|%s|%s|%s|%s|%s",
			_TL("select from predefined wave lengths"),
			SG_T("Landsat 1-5 MSS (Bands 1, 2, 3, 4)"),
			SG_T("Landsat 4-5 TM (Bands 1, 2, 3, 4, 5, 7, 6)"),
			SG_T("Landsat 7 ETM+ (Bands 1, 2, 3, 4, 5, 7, 6)"),
			SG_T("Landsat 8-9 OLI/TIRS (Bands 2, 3, 4, 5, 7, 10, 11)"),
			SG_T("Sentinel-2 (Bands 2, 3, 4, 5, 6, 7, 8, 11, 12)")
		), 0
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSpectral_Profile::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PREDEFS") )
	{
		switch( pParameter->asInt() )
		{
		case  1: (*pParameters)["LENGTHS"].Set_Value("0.55 0.65 0.75 0.95"                                           ); break; // Landsat 1-5 MSS
		case  2: (*pParameters)["LENGTHS"].Set_Value("0.485 0.56 0.66 0.83 1.65 2.215 11.45"                         ); break; // Landsat 4-5 TM
		case  3: (*pParameters)["LENGTHS"].Set_Value("0.485 0.57 0.66 0.84 1.65 2.220 11.45"                         ); break; // Landsat 7 ETM+
		case  4: (*pParameters)["LENGTHS"].Set_Value("0.4825 0.5625 0.655 0.865 1.61 2.2 10.8 12"                    ); break; // Landsat 8-9 OLI/TIRS
		case  5: (*pParameters)["LENGTHS"].Set_Value("0.4927 0.5598 0.6646 0.7041 0.4705 0.7828 0.8328 1.6137 2.2024"); break; // Sentinel-2
		}

		pParameter->Set_Value(0);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Profile::On_Execute(void)
{
	if( !Initialize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pLocations = Parameters("LOCATION")->asShapes();

	for(int i=0; i<pLocations->Get_Count() && Set_Progress(i, pLocations->Get_Count()); i++)
	{
		Add_Profile(pLocations->Get_Shape(i)->Get_Point(0), true);
	}

	//-----------------------------------------------------
	Update_Profile();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Profile::Initialize(void)
{
	if( m_pBands->Get_Grid_Count() < 1 )
	{
		Error_Fmt(_TL("There is no band in input list."));

		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: m_Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: m_Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: m_Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: m_Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	CSG_Vector Length(m_pBands->Get_Grid_Count());

	{
		CSG_Strings Values = SG_String_Tokenize(Parameters("LENGTHS")->asString(), " ;");

		if( Length.Get_Size() < Values.Get_Size() )
		{
			SG_UI_Msg_Add_Error(CSG_String::Format("[%s] %s", _TL("Warning"), _TL("There are more bands provided than wave lengths!")));
		}

		for(size_t i=0; i<Length.Get_Size(); i++)
		{
			double Value;

			Length[i] = Values[i].asDouble(Value) ? Value : i > 0 ? Length[i - 1] + 1. : 0.;
		}
	}

	//-----------------------------------------------------
	m_pProfile = Parameters("PROFILE")->asTable();
	m_pProfile->Destroy();
	m_pProfile->Set_Name(_TL("Spectral Profile"));
	m_pProfile->Add_Field("ID"    , SG_DATATYPE_Int   ); // FIELD_ID
	m_pProfile->Add_Field("NAME"  , SG_DATATYPE_String); // FIELD_NAME
	m_pProfile->Add_Field("LENGTH", SG_DATATYPE_Double); // FIELD_LENGTH

	for(int i=0; i<m_pBands->Get_Grid_Count(); i++)
	{
		CSG_Table_Record &Band = *m_pProfile->Add_Record();

		Band.Set_Value(FIELD_ID    , i + 1);
		Band.Set_Value(FIELD_NAME  , m_pBands->Get_Grid(i)->Get_Name());
		Band.Set_Value(FIELD_LENGTH, Length[i]);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Profile::Add_Profile(const CSG_Point &Point, bool bMultiple)
{
	int Field = bMultiple ? m_pProfile->Get_Field_Count() : FIELD_VALUES;

	if( bMultiple )
	{
		CSG_String Name(CSG_String::Format("PROFILE_%02d", 1 + m_pProfile->Get_Field_Count() - FIELD_VALUES));

		m_pProfile->Add_Field(Name     , SG_DATATYPE_Double);
	}
	else if( Field >= m_pProfile->Get_Field_Count() )
	{
		m_pProfile->Add_Field("PROFILE", SG_DATATYPE_Double);
	}

	for(int i=0; i<m_pBands->Get_Grid_Count(); i++)
	{
		double Value; CSG_Table_Record &Band = *m_pProfile->Get_Record(i);

		if( m_pBands->Get_Grid(i)->Get_Value(Point, Value, m_Resampling) )
		{
			Band.Set_Value (Field, Value);
		}
		else
		{
			Band.Set_NoData(Field);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Profile::Update_Profile(bool bUpdate)
{
	CSG_Parameters P; CSG_String Fields(CSG_Parameter_Table_Field::Get_Choices(*m_pProfile, true));

	P.Add_Int   ("", "WINDOW_ARRANGE", "", "", SG_UI_WINDOW_ARRANGE_MDI_TILE_HOR|SG_UI_WINDOW_ARRANGE_TDI_SPLIT_BOTTOM);
	P.Add_Bool  ("", "UPDATE"        , "", "", bUpdate); // force parameters update if diagram is already open

	P.Add_Bool  ("", "LEGEND"        , "", "", false);
	P.Add_Choice("", "X_FIELD"       , "", "", Fields, FIELD_LENGTH); // wave length
	P.Add_Int   ("", "LINES_SIZE"    , "", "", 2);
	P.Add_Bool  ("", "AXES_ORIGINS"  , "", "", true);
	P.Add_Bool  ("", "Y_MIN_FIX"     , "", "", true);
	P.Add_Double("", "Y_MIN_VAL"     , "", "", 0.);

	double max = m_pBands->Get_Grid(0)->Get_Max();
	if( max <= 255. )
	{
		P.Add_Bool  ("", "Y_MAX_FIX" , "", "", true);
		P.Add_Double("", "Y_MAX_VAL" , "", "", max <= 1. ? 1. : 255.);
	}

	for(int i=FIELD_VALUES; i<m_pProfile->Get_Field_Count(); i++)
	{
		P.Add_Bool("", CSG_String::Format("FIELD_%d", i), "", "", true);
	}

	SG_UI_Diagram_Show(m_pProfile, &P);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSpectral_Profile_Interactive::CSpectral_Profile_Interactive(void)
{
	m_Profile.Set_Manager(NULL);

	Parameters.Assign_Parameters(&m_Profile.Parameters);

	//-----------------------------------------------------
	Set_Name		(m_Profile.Get_Name());

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Del_Parameter("LOCATION");

	Parameters.Add_Table("",
		"LOCATION"  , _TL("Profile Location"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"COLLECT"   , _TL("Collect"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("single profile"),
			_TL("multiple profiles")
		), 0
	);

	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSpectral_Profile_Interactive::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Profile.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool_Interactive::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Profile_Interactive::On_Execute(void)
{
	m_Profile.Parameters.Assign_Values(&Parameters);

	if( !m_Profile.Initialize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_bMultiple = Parameters("COLLECT")->asInt() == 1;

	//-----------------------------------------------------
	m_pLocation = Parameters("LOCATION")->asTable();
	m_pLocation->Destroy();
	m_pLocation->Set_Name(_TL("Profile Location"));
	m_pLocation->Add_Field("ID"  , SG_DATATYPE_Int   );
	m_pLocation->Add_Field("NAME", SG_DATATYPE_String);
	m_pLocation->Add_Field("X"   , SG_DATATYPE_Double);
	m_pLocation->Add_Field("Y"   , SG_DATATYPE_Double);

	//-----------------------------------------------------
	if( m_bMultiple == false )
	{
		m_pLocation->Add_Record();
		m_pLocation->Get_Record(0)->Set_Value(0, 1);
		m_pLocation->Get_Record(0)->Set_Value(1, "PROFILE");

		m_Profile.Add_Profile(CSG_Point(0., 0.), false);
	}

	//-----------------------------------------------------
	m_Profile.Update_Profile();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Profile_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( m_bMultiple )
	{
		if( Mode != TOOL_INTERACTIVE_LDOWN )
		{
			return( true );
		}

		CSG_Table_Record &Location = *m_pLocation->Add_Record();

		Location.Set_Value("ID"  , m_pLocation->Get_Count());
		Location.Set_Value("NAME", CSG_String::Format("PROFILE_%02d", 1 + m_pLocation->Get_Count()));
		Location.Set_Value("X"   , ptWorld.x);
		Location.Set_Value("Y"   , ptWorld.y);
	}
	else
	{
		if( Mode != TOOL_INTERACTIVE_LDOWN && Mode != TOOL_INTERACTIVE_MOVE_LDOWN )
		{
			return( true );
		}

		m_pLocation->Get_Record(0)->Set_Value("X", ptWorld.x);
		m_pLocation->Get_Record(0)->Set_Value("Y", ptWorld.y);
	}

	return( m_Profile.Add_Profile(ptWorld, m_bMultiple) && m_Profile.Update_Profile(m_bMultiple) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
