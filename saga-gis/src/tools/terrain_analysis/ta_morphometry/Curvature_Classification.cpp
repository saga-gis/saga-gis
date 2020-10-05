
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             Curvature_Classification.cpp              //
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
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Curvature_Classification.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCurvature_Classification::CCurvature_Classification(void)
{
	Set_Name		(_TL("Curvature Classification"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Landform classification based on the profile and tangential (across slope) curvatures. "
	));

	Add_Reference("Dikau, R.", "1988",
		"Entwurf einer geomorphographisch-analytischen Systematik von Reliefeinheiten",
		"Heidelberger Geographische Bausteine, Heft 5."
	);

	Add_Reference("R.A. MacMillan & P.A. Shary", "2009",
		"Landforms and Landform Elements in Geomorphometry",
		"In: Hengl, T. & Reuter, H.I. [Eds.]: Geomorphometry - Concepts, Software, Applications. Developments in Soil Science, Vol.33:227-254.",
		SG_T("https://doi.org/10.1016/S0166-2481(08)00009-3"), SG_T("doi:10.1016/S0166-2481(08)00009-3")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CLASSES"	, _TL("Curvature Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Double("",
		"STRAIGHT"	, _TL("Threshold Radius"),
		_TL("Curvature radius threshold [map units] to distinct between straight and curved surfaces."),
		2000., 0.00001, true
	);

	Parameters.Add_Choice("",
		"VERTICAL"	, _TL("Vertical Curvature"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("longitudinal curvature"),
			_TL("profile curvature"),
			_TL("")
		), 1
	);

	Parameters.Add_Choice("",
		"HORIZONTAL", _TL("Horizontal Curvature"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("cross-sectional curvature"),
			_TL("tangential curvature"),
			_TL("plan curvature")
		), 1
	);

	Parameters.Add_Int("",
		"SMOOTH"	, _TL("Smoothing"),
		_TL("Smoothing kernel radius. No smoothing will be done, if set to zero."),
		0, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCurvature_Classification::On_Execute(void)
{
	m_pDEM       = Parameters("DEM")->asGrid();

	m_Vertical   = Parameters("VERTICAL"  )->asInt();
	m_Horizontal = Parameters("HORIZONTAL")->asInt();

	double	Threshold	= 1. / Parameters("STRAIGHT")->asDouble();

	CSG_Grid	DEM, *pClassification	= Parameters("CLASSES")->asGrid();

	//-----------------------------------------------------
	if( Parameters("SMOOTH")->asInt() > 0 && DEM.Create(Get_System()) )
	{
		CSG_Grid_Cell_Addressor	Kernel; Kernel.Set_Circle(Parameters("SMOOTH")->asDouble());

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				CSG_Simple_Statistics	s;

				if( !m_pDEM->is_NoData(x, y) )
				{
					for(int i=0; i<Kernel.Get_Count(); i++)
					{
						int ix = Kernel.Get_X(i, x), iy = Kernel.Get_Y(i, y);

						if( m_pDEM->is_InGrid(ix, iy) )
						{
							s	+= m_pDEM->asDouble(ix, iy);
						}
					}
				}

				if( s.Get_Count() > 0 )
				{
					DEM.Set_Value(x, y, s.Get_Mean());
				}
				else
				{
					DEM.Set_NoData(x, y);
				}
			}
		}

		m_pDEM	= &DEM;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Plan, Prof;

			if( Get_Curvature(x, y, Plan, Prof) )
			{
				pClassification->Set_Value(x, y,
						(fabs(Plan) < Threshold ? 3 : Plan < 0. ? 0 : 6)
					+	(fabs(Prof) < Threshold ? 1 : Prof < 0. ? 0 : 2)
				);
			}
			else
			{
				pClassification->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameter	*pClasses	= DataObject_Get_Parameter(pClassification, "LUT");

	if( pClasses && pClasses->asTable() )
	{
		const char	*Name[9]	=
		{
			 "V / V" ,
			"GE / V" ,
			 "X / V" ,
			 "V / GR",
			"GE / GR",
			 "X / GR",
			 "V / X" ,
			"GE / X" ,
			 "X / X"
		};

		//-------------------------------------------------
		const int Color[9]	=
		{
			SG_GET_RGB(  0,   0, 127),	// V / V
			SG_GET_RGB(  0,  63, 200),	// G / V
			SG_GET_RGB(  0, 127, 255),	// X / V
			SG_GET_RGB(127, 200, 255),	// V / G
			SG_GET_RGB(245, 245, 245),	// G / G
			SG_GET_RGB(255, 200, 127),	// X / G
			SG_GET_RGB(255, 127,   0),	// V / X
			SG_GET_RGB(200,  63,   0),	// G / X
			SG_GET_RGB(127,   0,   0),	// X / X
		};

		//-------------------------------------------------
		pClasses->asTable()->Del_Records();

		for(int i=0; i<9; i++)
		{
			CSG_Table_Record	&Class	= *pClasses->asTable()->Add_Record();

			Class.Set_Value(0, Color[i]);
			Class.Set_Value(1, Name [i]);
			Class.Set_Value(2, Name [i]);
			Class.Set_Value(3,       i );
			Class.Set_Value(4,       i );
		}

		DataObject_Set_Parameter(pClassification, pClasses);
		DataObject_Set_Parameter(pClassification, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCurvature_Classification::Get_Curvature(int x, int y, double &Plan, double &Profile)
{
	static const int	Index[8]	=	{ 5, 8, 7, 6, 3, 0, 1, 2 };

	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	double	z	= m_pDEM->asDouble(x, y), Z[9];	Z[4]	= 0.;

	for(int i=0, ix, iy; i<8; i++)
	{
		if( m_pDEM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) )
		{
			Z[Index[i]]	= m_pDEM->asDouble(ix, iy) - z;
		}
		else if( m_pDEM->is_InGrid(ix = Get_xFrom(i, x), iy = Get_yFrom(i, y)) )
		{
			Z[Index[i]]	= z - m_pDEM->asDouble(ix, iy);
		}
		else
		{
			Z[Index[i]]	= 0.;
		}
	}

	double	r	= ((Z[3] + Z[5]) / 2.  - Z[4]) * 2.00 / Get_Cellarea();
	double	t	= ((Z[1] + Z[7]) / 2.  - Z[4]) * 2.00 / Get_Cellarea();
	double	s	=  (Z[0] - Z[2] - Z[6] + Z[8]) * 0.25 / Get_Cellarea();
	double	p	=  (Z[5] - Z[3])               * 0.50 / Get_Cellsize();
    double	q	=  (Z[7] - Z[1])               * 0.50 / Get_Cellsize();

	switch( m_Vertical )
	{
	case  0: Profile = -2. * (r * p*p + t * q*q + s*p*q) / ((p*p + q*q)                           ); break; // longitudinal
	default: Profile = -(r * p*p + t * q*q + 2. * s*p*q) / ((p*p + q*q) * pow(1. + p*p + q*q, 1.5)); break; // profile
	}

	switch( m_Horizontal )
	{
	case  0: Plan    = -2. * (t * p*p + r * q*q + s*p*q) / ((p*p + q*q)                           ); break; // cross-sectional
	default: Plan    = -(t * p*p + r * q*q - 2. * s*p*q) / ((p*p + q*q) * pow(1. + p*p + q*q, 0.5)); break; // tangential
	case  2: Plan    = -(t * p*p + r * q*q - 2. * s*p*q) / (              pow(1. + p*p + q*q, 1.5)); break; // plan
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
