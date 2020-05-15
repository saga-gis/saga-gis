
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Cross_Profiles.cpp                //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Cross_Profiles.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cross_Profiles::CGrid_Cross_Profiles(void)
{
	Set_Name		(_TL("Cross Profiles"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Create cross profiles from a grid based DEM for given lines."
	));

	Parameters.Add_Grid("",
		"DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"LINES"			, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"PROFILES"		, _TL("Cross Profiles"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double("",
		"DIST_LINE"		, _TL("Profile Distance"),
		_TL("The distance of each cross profile along the lines."),
		10., 0., true
	);

	Parameters.Add_Double("",
		"DIST_PROFILE"	, _TL("Profile Length"),
		_TL("The length of each cross profile."),
		10., 0., true
	);

	Parameters.Add_Int("",
		"NUM_PROFILE"	, _TL("Profile Samples"),
		_TL("The number of profile points per cross profile."),
		11, 3, true
	);

	Parameters.Add_Choice("",
		"INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"           ),
			_TL("Bilinear Interpolation"      ),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"      )
		), 3
	);

	Parameters.Add_Choice("",
		"OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("vertices"),
			_TL("attributes"),
			_TL("vertices and attributes")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Cross_Profiles::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DEM") )
	{
		CSG_Grid	*pDEM	= (*pParameters)("DEM")->asGrid();

		if( pDEM )
		{
			if( (*pParameters)("DIST_LINE")->asDouble() < pDEM->Get_Cellsize() )
			{
				pParameters->Set_Parameter("DIST_LINE", pDEM->Get_Cellsize());
			}

			int	nSamples	= (*pParameters)("NUM_PROFILE")->asInt() - 1;

			if( (*pParameters)("DIST_PROFILE")->asDouble() < nSamples * pDEM->Get_Cellsize() )
			{
				pParameters->Set_Parameter("DIST_PROFILE", nSamples * pDEM->Get_Cellsize());
			}
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cross_Profiles::On_Execute(void)
{
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	double	Step	= Parameters("DIST_LINE"   )->asDouble();
	double	Length	= Parameters("DIST_PROFILE")->asDouble() / 2.;

	int	nSamples	= Parameters("NUM_PROFILE")->asInt();

	m_Output	= Parameters("OUTPUT")->asInt();

	m_pDEM	= Parameters("DEM")->asGrid();

	switch( Parameters("INTERPOLATION")->asInt() )
	{
	default: m_Interpolation = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: m_Interpolation = GRID_RESAMPLING_Bilinear        ; break;
	case  2: m_Interpolation = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: m_Interpolation = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	m_pProfiles	= Parameters("PROFILES")->asShapes();

	m_pProfiles->Create(SHAPE_TYPE_Line, _TL("Profiles"), NULL, m_Output == 1 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZ);
	m_pProfiles->Add_Field("ID", SG_DATATYPE_Int);

	if( m_Output != 0 )	// NOT vertices only
	{
		int	n	= nSamples < 10 ? 1 : nSamples < 100 ? 2 : nSamples < 1000 ? 3 : 4;

		for(int iPoint=0; iPoint<nSamples; iPoint++)
		{
			m_pProfiles->Add_Field(CSG_String::Format("P%0*d", n, 1 + iPoint), SG_DATATYPE_Double);
		}
	}

	//-----------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape_Line	*pLine	= (CSG_Shape_Line *)pLines->Get_Shape(iLine);

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( pLine->Get_Length(iPart) > 0. )
			{
				double	dStep	= 0.;

				for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					CSG_Point	A	= pLine->Get_Point(iPoint - 1, iPart);
					CSG_Point	B	= pLine->Get_Point(iPoint    , iPart);	B	-= A;

					double	dSegment	= B.Get_Length();

					if( dSegment > 0. )
					{
						B	/= dSegment;

						while( dStep < dSegment )
						{
							CSG_Point	P(A.x + dStep * B.x, A.y + dStep * B.y);

							Add_Profile(P, B, Length, nSamples);

							dStep	+= Step;
						}

						dStep	-= dSegment;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_pProfiles->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cross_Profiles::Add_Profile(const TSG_Point &P, const TSG_Point &N, double Length, int nSamples)
{
	if( !m_pDEM->is_InGrid_byPos(P) )
	{
		return( false );
	}

	CSG_Shape	*pProfile	= m_pProfiles->Add_Shape();

	pProfile->Set_Value(0, m_pProfiles->Get_Count());

	CSG_Point	A(P.x + N.y * Length, P.y - N.x * Length);
	CSG_Point	B(P.x - N.y * Length, P.y + N.x * Length);

	if( m_Output == 1 )	// attributes only
	{
		pProfile->Add_Point(A);
		pProfile->Add_Point(B);
	}

	B	-= A;
	B	/= nSamples - 1.;

	//-----------------------------------------------------
	for(int i=0; i<nSamples; i++)
	{
		double	z;

		if( m_pDEM->Get_Value(A, z, m_Interpolation) )
		{
			if( m_Output != 1 )	// NOT attributes only
			{
				pProfile->Add_Point(A);
				pProfile->Set_Z(z, i);
			}

			if( m_Output != 0 )	// NOT vertices only
			{
				pProfile->Set_Value(1 + i, z);
			}
		}
		else
		{
			if( m_Output != 0 )	// NOT vertices only
			{
				pProfile->Set_NoData(1 + i);
			}
		}

		A	+= B;
	}

	//-----------------------------------------------------
	if( pProfile->Get_Point_Count() < 2 )
	{
		m_pProfiles->Del_Shape(pProfile);

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
