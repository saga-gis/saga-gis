/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Fragmentation_Base.cpp                //
//                                                       //
//                 Copyright (C) 2008 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "fragmentation_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFragmentation_Base::CFragmentation_Base(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Fragmentation"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Fragmentation classes:\n"
		"(1) interior, if Density = 1.0\n"
		"(2) undetermined, if Density > 0.6 and Density = Connectivity\n"
		"(3) perforated, if Density > 0.6 and Density - Connectivity > 0\n"
		"(4) edge, if Density > 0.6 and Density - Connectivity < 0\n"
		"(5) transitional, if 0.4 < Density < 0.6\n"
		"(6) patch, if Density < 0.4\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "CLASSES"		, _TL("Classification"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "DENSITY"		, _TL("Density [Percent]"),
		_TL("Density Index (Pf)."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "CONNECTIVITY"	, _TL("Connectivity [Percent]"),
		_TL("Connectivity Index (Pff)."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "FRAGMENTATION"	, _TL("Fragmentation"),
		_TL("Fragmentation Index"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Table(
		"", "FRAGSTATS"		, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Int(
		"", "CLASS"			, _TL("Class Identifier"),
		_TL(""),
		1
	);

	Parameters.Add_Range(
		"", "NEIGHBORHOOD"	, _TL("Neighborhood"),
		_TL("Moving window size = 1 + 2 * Neighborhood."),
		3, 3, 0, true
	);

	Parameters.Add_Choice(
		"", "AGGREGATION"	, _TL("Level Aggregation"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("average"),
			_TL("multiplicative")
		), 0
	);

	Parameters.Add_Bool(
		"", "BORDER"		, _TL("Add Border"),
		_TL(""),
		false
	);

	Parameters.Add_Double(
		"", "WEIGHT"		, _TL("Connectivity Weighting"),
		_TL(""),
		1.1, 0.0, true
	);

	Parameters.Add_Double(
		"", "DENSITY_MIN"	, _TL("Minimum Density [Percent]"),
		_TL(""),
		10.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Double(
		"", "DENSITY_INT"	, _TL("Minimum Density for Interior Forest [Percent]"),
		_TL("if less than 100, it is distinguished between interior and core forest"),
		99.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Base::On_Execute(void)
{
	int			Class;
	CSG_Grid	*pClasses, *pDensity, *pConnectivity, *pFragmentation;

	//-----------------------------------------------------
	pClasses			= Parameters("CLASSES"      )->asGrid();
	pDensity			= Parameters("DENSITY"      )->asGrid();
	pConnectivity		= Parameters("CONNECTIVITY" )->asGrid();
	pFragmentation		= Parameters("FRAGMENTATION")->asGrid();

	Class				= Parameters("CLASS"        )->asInt();
	m_Radius_Min		= Parameters("NEIGHBORHOOD" )->asRange()->Get_Min();
	m_Radius_Max		= Parameters("NEIGHBORHOOD" )->asRange()->Get_Max();
	m_Aggregation		= Parameters("AGGREGATION"  )->asInt();
	m_Weight			= Parameters("WEIGHT"       )->asDouble();
	m_Density_Min		= Parameters("DENSITY_MIN"  )->asDouble() / 100.0;
	m_Density_Interior	= Parameters("DENSITY_INT"  )->asDouble() / 100.0;

	m_Radius_iMin		= (int)(0.5 + m_Radius_Min);
	m_Radius_iMax		= (int)(0.5 + m_Radius_Max);

	//-----------------------------------------------------
	DataObject_Set_Colors(pDensity     , 11, SG_COLORS_WHITE_GREEN);
	DataObject_Set_Colors(pConnectivity, 11, SG_COLORS_WHITE_GREEN);

	Set_Classification(pFragmentation);

	if( !Initialise(pClasses, Class) )
	{
		Finalise();

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Density, Connectivity;

			if( Get_Fragmentation(x, y, Density, Connectivity) )
			{
				pDensity      ->Set_Value (x, y, 100.0 * Density);
				pConnectivity ->Set_Value (x, y, 100.0 * Connectivity);
			//	pFragmentation->Set_Value (x, y, 100.0 * Density * Connectivity);
				pFragmentation->Set_Value (x, y, Get_Classification(Density, Connectivity));
			}
			else
			{
				pDensity      ->Set_NoData(x, y);
				pConnectivity ->Set_NoData(x, y);
				pFragmentation->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("BORDER")->asBool() )
	{
		Add_Border(pFragmentation);
	}

	Get_Statistics(pFragmentation, *Parameters("FRAGSTATS")->asTable());

	Finalise();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Base::Set_Classification(CSG_Grid *pFragmentation)
{
	DataObject_Set_Colors(pFragmentation, 11, SG_COLORS_WHITE_GREEN, true);

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pFragmentation, P) && P("COLORS_TYPE") && P("LUT") )
	{
		#define LUT_ADD(color, id, name)	{ CSG_Table_Record *pLUT = LUT.Add_Record();\
			pLUT->Set_Value(0, color);\
			pLUT->Set_Value(1, name );\
			pLUT->Set_Value(3, id   );\
			pLUT->Set_Value(4, id   );\
		}

		CSG_Table	LUT;

		LUT.Add_Field("COLOR", SG_DATATYPE_Color );
		LUT.Add_Field("NAME" , SG_DATATYPE_String);
		LUT.Add_Field("DESC" , SG_DATATYPE_String);
		LUT.Add_Field("MIN"  , SG_DATATYPE_Double);
		LUT.Add_Field("MAX"  , SG_DATATYPE_Double);

		LUT_ADD(SG_GET_RGB(  0, 127,   0), CLASS_CORE        , _TL("Core"        ));
		LUT_ADD(SG_GET_RGB( 34, 255,  34), CLASS_INTERIOR    , _TL("Interior"    ));
		LUT_ADD(SG_GET_RGB(129, 255, 129), CLASS_UNDETERMINED, _TL("Undetermined"));
		LUT_ADD(SG_GET_RGB(255, 110,   0), CLASS_PERFORATED  , _TL("Perforated"  ));
		LUT_ADD(SG_GET_RGB(255, 255,   0), CLASS_EDGE        , _TL("Edge"        ));
		LUT_ADD(SG_GET_RGB(162, 162, 255), CLASS_TRANSITIONAL, _TL("Transitional"));
		LUT_ADD(SG_GET_RGB( 56,  56, 255), CLASS_PATCH       , _TL("Patch"       ));
		LUT_ADD(SG_GET_RGB(200, 200, 200), CLASS_NONE        , _TL("None"        ));

		P("LUT")->asTable()->Assign_Values(&LUT);	// Lookup Table
		P("COLORS_TYPE")->Set_Value(1);				// Color Classification Type: Lookup Table

	//	pFragmentation->Set_NoData_Value(CLASS_NONE);

		DataObject_Set_Parameters(pFragmentation, P);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFragmentation_Base::Get_Classification(double Density, double Connectivity)
{
	if( Density >= 0.999 )					// (7) core, if Density = 1.0
	{
		if( m_Density_Interior < 0.999 )
		{
			return( CLASS_CORE );
		}
		else
		{
			return( CLASS_INTERIOR );
		}
	}
	if( Density >= m_Density_Interior )		// (4) interior, if Density = 1.0
	{
		return( CLASS_INTERIOR );
	}
	else if( Density > 0.6 )
	{
		Connectivity	*= m_Weight;

		if( Density < Connectivity )		// (1) edge, if Density > 0.6 and Density - Connectivity < 0
		{
			return( CLASS_EDGE );
		}
		else if( Density > Connectivity )	// (3) perforated, if Density > 0.6 and Density - Connectivity > 0
		{
			return( CLASS_PERFORATED );
		}
		else								// (2) undetermined, if Density > 0.6 and Density = Connectivity
		{
			return( CLASS_UNDETERMINED );
		}
	}
	else if( Density >= 0.4 )				// (6) transitional, if 0.4 < Density < 0.6
	{
		return( CLASS_TRANSITIONAL );
	}
	else if( Density >= m_Density_Min )		// (5) patch, if Density < 0.4
	{
		return( CLASS_PATCH );
	}
	else
	{
		return( CLASS_NONE );
	}
}

//---------------------------------------------------------
void CFragmentation_Base::Add_Border(CSG_Grid *pFragmentation)
{
	int			x, y;
	CSG_Grid	Tmp(pFragmentation, SG_DATATYPE_Byte);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pFragmentation->asInt(x, y) == CLASS_INTERIOR )
			{
				int		i, ix, iy;

				for(i=0; i<8; i++)
				{
					if( pFragmentation->Get_System().Get_Neighbor_Pos(i, x, y, ix, iy)
					&&	pFragmentation->asInt(ix, iy) != CLASS_INTERIOR
					&&	pFragmentation->asInt(ix, iy) != CLASS_CORE )
					{
						Tmp.Set_Value(ix, iy, 1);
					}
				}
			}
		}
	}

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( Tmp.asInt(x, y) )
			{
				pFragmentation->Set_Value(x, y, CLASS_INTERIOR);
			}
		}
	}
}

//---------------------------------------------------------
#define STATISTICS_ADD(i, id, name)	Statistics.Add_Record();\
	Statistics[i][0] = id;\
	Statistics[i][1] = name;\
	Statistics[i][2] = s[i];\
	Statistics[i][3] = s[i] * Get_Cellsize();\
	Statistics[i][4] = 100.0 * s[i] / (double)n;

//---------------------------------------------------------
void CFragmentation_Base::Get_Statistics(CSG_Grid *pFragmentation, CSG_Table &Statistics)
{
	if( pFragmentation != NULL && &Statistics != NULL )
	{
		int		i, n, s[8];

		for(i=0, n=0; i<8; i++)
		{
			s[i]	= 0;
		}

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				switch( pFragmentation->asInt(x, y) )
				{
				case CLASS_CORE:			s[0]++; n++;	break;
				case CLASS_INTERIOR:		s[1]++; n++;	break;
				case CLASS_UNDETERMINED:	s[2]++; n++;	break;
				case CLASS_PERFORATED:		s[3]++; n++;	break;
				case CLASS_EDGE:			s[4]++; n++;	break;
				case CLASS_TRANSITIONAL:	s[5]++; n++;	break;
				case CLASS_PATCH:			s[6]++; n++;	break;
				case CLASS_NONE:			s[7]++; n++;	break;
				}
			}
		}

		//-------------------------------------------------
		Statistics.Destroy();

		Statistics.Add_Field(SG_T("CLASS_ID")	, SG_DATATYPE_Int);
		Statistics.Add_Field(SG_T("CLASS_NAME")	, SG_DATATYPE_String);
		Statistics.Add_Field(SG_T("N_CELLS")	, SG_DATATYPE_Int);
		Statistics.Add_Field(SG_T("AREA_ABS")	, SG_DATATYPE_Double);
		Statistics.Add_Field(SG_T("AREA_REL")	, SG_DATATYPE_Double);

		STATISTICS_ADD(0, CLASS_CORE		, _TL("Core"))
		STATISTICS_ADD(1, CLASS_INTERIOR	, _TL("Interior"))
		STATISTICS_ADD(2, CLASS_UNDETERMINED, _TL("Undetermined"))
		STATISTICS_ADD(3, CLASS_PERFORATED	, _TL("Perforated"))
		STATISTICS_ADD(4, CLASS_EDGE		, _TL("Edge"))
		STATISTICS_ADD(5, CLASS_TRANSITIONAL, _TL("Transitional"))
		STATISTICS_ADD(6, CLASS_PATCH		, _TL("Patch"))
		STATISTICS_ADD(7, CLASS_NONE		, _TL("None"))
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
