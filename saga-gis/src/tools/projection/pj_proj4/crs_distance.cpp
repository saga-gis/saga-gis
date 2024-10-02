
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   crs_distance.cpp                    //
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
#include "crs_distance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Distance_Calculator::CCRS_Distance_Calculator(void)
{}

//---------------------------------------------------------
CCRS_Distance_Calculator::CCRS_Distance_Calculator(const CSG_Projection &Projection, double Epsilon)
{
	Create(Projection, Epsilon);
}

//---------------------------------------------------------
CCRS_Distance_Calculator::~CCRS_Distance_Calculator(void)
{}

//---------------------------------------------------------
bool CCRS_Distance_Calculator::Create(const CSG_Projection &Projection, double Epsilon)
{
	if( !m_ProjToGCS.Set_Transformation(Projection, CSG_Projection::Get_GCS_WGS84()) || !m_Projector.Set_Target(Projection) )
	{
		return( false );
	}

	m_Epsilon = Epsilon;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCRS_Distance_Calculator::Get_Orthodrome(const TSG_Point &A, const TSG_Point &B, CSG_Shape *pLine)
{
	const char *PROJ = "+proj=aeqd +R=6371000 +lon_0=%f +lat_0=%f";
	const char *WKT2 =
		"PROJCRS[\"<custom>\","
		"    BASEGEODCRS[\"<custom>\","
		"        DATUM[\"<custom>\","
		"            ELLIPSOID[\"<custom>\",6371000,0]],"
		"        UNIT[\"degree\",0.0174532925199433,"
		"            ID[\"EPSG\",9122]]],"
		"    CONVERSION[\"<custom>\","
		"        METHOD[\"Azimuthal Equidistant\","
		"            ID[\"EPSG\",1125]],"
		"        PARAMETER[\"Latitude of natural origin\",%f,"
		"            ID[\"EPSG\",8801]],"
		"        PARAMETER[\"Longitude of natural origin\",%f,"
		"            ID[\"EPSG\",8802]],"
		"        PARAMETER[\"False easting\",0,"
		"            ID[\"EPSG\",8806]],"
		"        PARAMETER[\"False northing\",0,"
		"            ID[\"EPSG\",8807]]],"
		"    CS[Cartesian,2],"
		"        AXIS[\"(E)\",east],"
		"        AXIS[\"(N)\",north],"
		"        UNIT[\"metre\",1,"
		"            ID[\"EPSG\",9001]]]";

	static const TSG_Point P0 = { 0., 0. };

	TSG_Point P = A;

	if( m_ProjToGCS.Get_Projection(P) )
	{
		CSG_Projection Projection(CSG_String::Format(WKT2, P.x, P.y), CSG_String::Format(PROJ, P.x, P.y)); // fast construction!

		m_Projector.Set_Inverse();

		if( m_Projector.Set_Source(Projection, true) && m_Projector.Get_Projection(P = B) )
		{
			m_Projector.Set_Forward();

			Add_Segment(P0, P, pLine);

			return( SG_Get_Distance(P0, P) );
		}
	}

	return( Get_Distance(A, B) );
}

//---------------------------------------------------------
double CCRS_Distance_Calculator::Get_Loxodrome(const TSG_Point &A, const TSG_Point &B, CSG_Shape *pLine)
{
	TSG_Point AA, BB;

	m_Projector.Set_Source(CSG_Projection("+proj=merc +datum=WGS84"));

	m_Projector.Set_Inverse();

	if( m_Projector.Get_Projection(AA = A)
	&&  m_Projector.Get_Projection(BB = B) )
	{
		double Length = 0.;

		m_Projector.Set_Forward();

		Add_Segment(AA, BB, pLine, &Length);

		return( Length );
	}

	return( Get_Distance(A, B) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCRS_Distance_Calculator::Get_Distance(TSG_Point A, TSG_Point B)
{
	if( m_ProjToGCS.Get_Projection(A) && m_ProjToGCS.Get_Projection(B) )
	{
		return( SG_Get_Distance_Polar(A, B) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
void CCRS_Distance_Calculator::Add_Segment(const TSG_Point &A, const TSG_Point &B, CSG_Shape *pLine, double *Length)
{
	if( SG_Get_Distance(A, B) >= m_Epsilon )
	{
		TSG_Point C, CC;

		C.x = CC.x = A.x + 0.5 * (B.x - A.x);
		C.y = CC.y = A.y + 0.5 * (B.y - A.y);

		if( m_Projector.Get_Projection(CC) )
		{
			Add_Segment(A, C, pLine);

			pLine->Add_Point(CC);

			Add_Segment(C, B, pLine);
		}
	}
	else if( Length != NULL )
	{
		TSG_Point AA, BB;

		if( m_Projector.Get_Projection(AA = A) && m_Projector.Get_Projection(BB = B) )
		{
			*Length += Get_Distance(AA, BB);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Distance_Lines::CCRS_Distance_Lines(void)
{
	Set_Name		(_TL("Geographic Distances"));

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(_TW(
		"Calculates for all segments of the input lines the planar, great elliptic, "
		"and loxodrome distance and re-projects the latter two to the projection "
		"of the input lines. "
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"PLANAR"    , _TL("Segments"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"ORTHODROME", _TL("Great Elliptic"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"LOXODROME" , _TL("Loxodrome"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double("",
		"EPSILON"   , _TL("Epsilon"),
		_TL("defines the maximum resolution [km] for the re-projected distance segments"),
		100., 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Distance_Lines::On_Execute(void)
{
	CSG_Shapes *pPlanars     = Parameters("PLANAR"    )->asShapes();
	CSG_Shapes *pOrthodromes = Parameters("ORTHODROME")->asShapes();
	CSG_Shapes *pLoxodromes  = Parameters("LOXODROME" )->asShapes();

	//-----------------------------------------------------
	CCRS_Distance_Calculator Distance;

	if( !Distance.Create(pPlanars->Get_Projection(), Parameters("EPSILON")->asDouble() * 1000.) )
	{
		Error_Set(_TL("projection initialization failed"));

		return( false );
	}

	//-----------------------------------------------------
	pOrthodromes->Create(pPlanars->Get_Type(), CSG_String::Format("%s [%s]", pPlanars->Get_Name(), _TL("Orthodromes")), pPlanars);
	pOrthodromes->Add_Field("LENGTH_PLAN", SG_DATATYPE_Double);
	pOrthodromes->Add_Field("LENGTH"     , SG_DATATYPE_Double);

	pLoxodromes ->Create(pPlanars->Get_Type(), CSG_String::Format("%s [%s]", pPlanars->Get_Name(), _TL("Loxodromes" )), pPlanars);
	pLoxodromes ->Add_Field("LENGTH_PLAN", SG_DATATYPE_Double);
	pLoxodromes ->Add_Field("LENGTH"     , SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(sLong iShape=0; iShape<pPlanars->Get_Count() && Set_Progress(iShape, pPlanars->Get_Count()); iShape++)
	{
		CSG_Shape_Line *pProj = (CSG_Shape_Line *)pPlanars->Get_Shape(iShape);

		for(int iPart=0; iPart<pProj->Get_Part_Count(); iPart++)
		{
			if( pProj->Get_Point_Count(iPart) > 1 )
			{
				TSG_Point A, B = pProj->Get_Point(0, iPart);

				CSG_Shape_Line *pOrthodrome = (CSG_Shape_Line *)pOrthodromes->Add_Shape(pProj, SHAPE_COPY_ATTR);
				CSG_Shape_Line *pLoxodrome  = (CSG_Shape_Line *)pLoxodromes->Add_Shape(pProj, SHAPE_COPY_ATTR);

				pOrthodrome->Set_Value(pPlanars->Get_Field_Count() + 0, pProj->Get_Length(iPart));
				pLoxodrome ->Set_Value(pPlanars->Get_Field_Count() + 0, pProj->Get_Length(iPart));

				pOrthodrome->Add_Point(B);
				pLoxodrome ->Add_Point(B);

				double dOrthodrome = 0.;
				double dLoxodrome  = 0.;

				for(int iPoint=1; iPoint<pProj->Get_Point_Count(iPart); iPoint++)
				{
					A = B; B = pProj->Get_Point(iPoint, iPart);

					dOrthodrome += Distance.Get_Orthodrome(A, B, pOrthodrome);
					dLoxodrome  += Distance.Get_Loxodrome (A, B, pLoxodrome );

					pOrthodrome->Add_Point(B);
					pLoxodrome ->Add_Point(B);
				}

				pOrthodrome->Set_Value(pPlanars->Get_Field_Count() + 1, dOrthodrome);
				pLoxodrome ->Set_Value(pPlanars->Get_Field_Count() + 1, dLoxodrome );
			}
		}
	}

	//-----------------------------------------------------
	return( pOrthodromes->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Distance_Points::CCRS_Distance_Points(void)
{
	Set_Name		(_TL("Geographic Distances (Pair of Coordinates)"));

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(_TW(
		"Calculates for all segments of the input lines the planar, great elliptic, "
		"and loxodrome distance and re-projects the latter two to the projection "
		"of the input lines. "
	));

	Parameters.Add_Shapes("",
		"DISTANCES", _TL("Geographic Distances"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Node  ("", "FROM", _TL("From"), _TL(""));
	Parameters.Add_Double("FROM", "COORD_X1", _TL("X"), _TL(""),  10.0);
	Parameters.Add_Double("FROM", "COORD_Y1", _TL("Y"), _TL(""),  53.5);

	Parameters.Add_Node  ("", "TO"  , _TL("To"  ), _TL(""));
	Parameters.Add_Double("TO"  , "COORD_X2", _TL("X"), _TL(""), 116.5);
	Parameters.Add_Double("TO"  , "COORD_Y2", _TL("Y"), _TL(""),   6.4);

	Parameters.Add_Double("",
		"EPSILON", _TL("Epsilon"),
		_TL("defines the maximum resolution [km] for the re-projected distance segments"),
		100., 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Distance_Points::On_Execute(void)
{
	CSG_Projection Projection;

	if( !Get_Projection(Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CCRS_Distance_Calculator Distance;

	if( !Distance.Create(Projection, Parameters("EPSILON")->asDouble() * 1000.) )
	{
		Error_Set(_TL("projection initialization failed"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Point A(
		Parameters("COORD_X1")->asDouble(),
		Parameters("COORD_Y1")->asDouble()
	);

	CSG_Point B(
		Parameters("COORD_X2")->asDouble(),
		Parameters("COORD_Y2")->asDouble()
	);

	//-----------------------------------------------------
	CSG_Shapes *pDistances = Parameters("DISTANCES")->asShapes();

	pDistances->Create(SHAPE_TYPE_Line, CSG_String::Format("%s", _TL("Geographic Distances")));

	pDistances->Add_Field("TYPE"  , SG_DATATYPE_String);
	pDistances->Add_Field("LENGTH", SG_DATATYPE_Double);

	pDistances->Get_Projection().Create(Projection);

	//-----------------------------------------------------
	CSG_Shape *pPlanar     = pDistances->Add_Shape(); pPlanar    ->Set_Value(0, "Planar"    );
	CSG_Shape *pOrthodrome = pDistances->Add_Shape(); pOrthodrome->Set_Value(0, "Orthodrome");
	CSG_Shape *pLoxodrome  = pDistances->Add_Shape(); pLoxodrome ->Set_Value(0, "Loxodrome" );

	pPlanar    ->Add_Point(A);
	pOrthodrome->Add_Point(A);
	pLoxodrome ->Add_Point(A);

	pPlanar    ->Set_Value(1, SG_Get_Distance        (A, B             ));
	pOrthodrome->Set_Value(1, Distance.Get_Orthodrome(A, B, pOrthodrome));
	pLoxodrome ->Set_Value(1, Distance.Get_Loxodrome (A, B, pLoxodrome ));

	pPlanar    ->Add_Point(B);
	pOrthodrome->Add_Point(B);
	pLoxodrome ->Add_Point(B);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Distance_Interactive::CCRS_Distance_Interactive(void)
{
	Set_Name		(_TL("Geographic Distances"));

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(_TW(
		"Calculates for all segments of the input lines the planar, great elliptic, "
		"and loxodrome distance and re-projects the latter two to the projection "
		"of the input lines. "
	));

	Parameters.Add_Shapes("",
		"DISTANCES", _TL("Geographic Distances"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double("",
		"EPSILON"  , _TL("Epsilon"),
		_TL("defines the maximum resolution [km] for the re-projected distance segments"),
		100., 1., true
	);

	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_LINE);
}

//---------------------------------------------------------
bool CCRS_Distance_Interactive::On_Execute(void)
{
	CCRS_Picker CRS;

	if( !Dlg_Parameters(CRS.Get_Parameters(), CRS.Get_Name()) )
	{
		m_Projection.Destroy();

		return( false );
	}

	CSG_Parameters &P = *CRS.Get_Parameters();

	return( m_Projection.Create(P["CRS_WKT"].asString(), P["CRS_PROJ"].asString()) );
}

//---------------------------------------------------------
bool CCRS_Distance_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode == TOOL_INTERACTIVE_LDOWN )
	{
		m_Down = ptWorld;
	}
	else if( Mode == TOOL_INTERACTIVE_LUP )
	{
		if( m_Down != ptWorld )
		{
			CCRS_Distance_Points Distance;

			Distance.Set_Parameter("DISTANCES", Parameters("DISTANCES")->asShapes());
			Distance.Set_Parameter("EPSILON"  , Parameters("EPSILON"  )->asDouble());
			Distance.Set_Parameter("CRS_WKT"  , m_Projection.Get_WKT ());
			Distance.Set_Parameter("CRS_PROJ" , m_Projection.Get_PROJ());
			Distance.Set_Parameter("COORD_X1" , m_Down .x);
			Distance.Set_Parameter("COORD_Y1" , m_Down .y);
			Distance.Set_Parameter("COORD_X2" , ptWorld.x);
			Distance.Set_Parameter("COORD_Y2" , ptWorld.y);

			Distance.Execute();

			DataObject_Update(Parameters("DISTANCES")->asShapes());

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
