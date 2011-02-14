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
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Fragmentation_Classify.cpp              //
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
#include "fragmentation_classify.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFragmentation_Classify::CFragmentation_Classify(void)
{
	Parameters.Create(this, SG_T(""), SG_T(""), SG_T(""), true);

	//-----------------------------------------------------
	Set_Name		(_TL("Fragmentation Classes from Density and Connectivity"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"\n"
		"(1) interior, if Density = 1.0\n"
		"(2) undetermined, if Density > 0.6 and Density = Connectivity\n"
		"(3) perforated, if Density > 0.6 and Density - Connectivity > 0\n"
		"(4) edge, if Density > 0.6 and Density - Connectivity < 0\n"
		"(5) transitional, if 0.4 < Density < 0.6\n"
		"(6) patch, if Density < 0.4\n"
		"\n"
		"\n"
		"References:\n"
		"Riitters, K., Wickham, J., O'Neill, R., Jones, B., Smith, E. (2000): \n"
		"Global-scale patterns of forest fragmentation. Conservation Ecology 4(2): 3\n"
		"<a href=\"http://www.ecologyandsociety.org/vol4/iss2/art3/\">http://www.ecologyandsociety.org/vol4/iss2/art3/</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DENSITY"			, _TL("Density [Percent]"),
		_TL("Density Index (Pf)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONNECTIVITY"	, _TL("Connectivity [Percent]"),
		_TL("Connectivity Index (Pff)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "FRAGMENTATION"	, _TL("Fragmentation"),
		_TL("Fragmentation Index"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Value(
		NULL	, "BORDER"			, _TL("Add Border"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);

	Parameters.Add_Value(
		NULL	, "WEIGHT"			, _TL("Connectivity Weighting"),
		_TL(""),
		PARAMETER_TYPE_Double		, 1.1, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DENSITY_MIN"		, _TL("Minimum Density [Percent]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 10.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "DENSITY_INT"		, _TL("Minimum Density for Interior Forest [Percent]"),
		_TL("if less than 100, it is distinguished between interior and core forest"),
		PARAMETER_TYPE_Double		, 99.0, 0.0, true, 100.0, true
	);
}

//---------------------------------------------------------
CFragmentation_Classify::~CFragmentation_Classify(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Classify::On_Execute(void)
{
	CSG_Grid	*pDensity, *pConnectivity, *pFragmentation;

	pDensity			= Parameters("DENSITY")			->asGrid();
	pConnectivity		= Parameters("CONNECTIVITY")	->asGrid();
	pFragmentation		= Parameters("FRAGMENTATION")	->asGrid();

	m_Weight			= Parameters("WEIGHT")			->asDouble();
	m_Density_Min		= Parameters("DENSITY_MIN")		->asDouble() / 100.0;
	m_Density_Interior	= Parameters("DENSITY_INT")		->asDouble() / 100.0;

	//-----------------------------------------------------
	CSG_Parameters	Parms;

	DataObject_Set_Colors(pFragmentation, 100, SG_COLORS_WHITE_GREEN, true);

	if( DataObject_Get_Parameters(pFragmentation, Parms) && Parms("COLORS_TYPE") && Parms("LUT") )
	{
		Parms("LUT")->asTable()->Assign_Values(&m_LUT);	// Lookup Table
		Parms("COLORS_TYPE")->Set_Value(1);				// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pFragmentation, Parms);
	}

//	pFragmentation->Set_NoData_Value(CLASS_NONE);

	//-----------------------------------------------------
	if( 1 )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pDensity->is_NoData(x, y) && !pConnectivity->is_NoData(x, y) )
				{
					double	Density			= pDensity		->asDouble(x, y) / 100.0;
					double	Connectivity	= pConnectivity	->asDouble(x, y) / 100.0;

				//	pFragmentation	->Set_Value (x, y, 100.0 * Density * Connectivity);
					pFragmentation	->Set_Value (x, y, Get_Classification(Density, Connectivity));
				}
				else
				{
					pFragmentation	->Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
		if( Parameters("BORDER")->asBool() )
		{
			Add_Border(pFragmentation);
		}

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
