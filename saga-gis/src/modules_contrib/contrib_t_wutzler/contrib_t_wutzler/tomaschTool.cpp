/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                   contrib_t_wutzler                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    tomaschTool.cpp                    //
//                                                       //
//            Copyright (C) 2003 Your Name               //
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
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tomaschTool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CtomaschTool::CtomaschTool(void)
{
	//-----------------------------------------------------
	// Place information about your module here...

	Set_Name		(_TL("tomaschTool"));

	Set_Author		(_TL("Copyrights (c) 2004 by Thomas Wutzler"));

	Set_Description	(_TW(
		"tomaschTool\n"
	));


	//-----------------------------------------------------
	// This module example is based on CSG_Module_Grid, which already
	// comes with a whole set of grid specific methods. It assumes
	// also that all grid objects ordered through the parameters
	// object have the same dimensions. Let's now order two grids,
	// one for input and one for output...

	Parameters.Add_Grid(	NULL, "INPUT"	, _TL("Input")	, _TL("Input for module calculations.")	, PARAMETER_INPUT);
	Parameters.Add_Grid(	NULL, "RESULT"	, _TL("Output")	, _TL("Output of module calculations.")	, PARAMETER_OUTPUT);


	//-----------------------------------------------------
	// And go for some other setting types...

	//Parameters.Add_Value(	NULL, "BOOLEAN"	, "Boolean"	, "A value of type boolean."		, PARAMETER_TYPE_Bool	, true);
	//Parameters.Add_Value(	NULL, "INTEGER"	, "Integer"	, "A value of type integer."		, PARAMETER_TYPE_Int	, 200);
	//Parameters.Add_Value(	NULL, "DOUBLE"	, "Double"	, "A floating point value."			, PARAMETER_TYPE_Double	, 3.145);
	Parameters.Add_Value(	NULL, "NoDataValue"	, _TL("NoDataValue"), _TL("Value for 'no data'")		, PARAMETER_TYPE_Double	, -9999);
	Parameters.Add_Value(	NULL, "LowerLimit"	, _TL("LowerLimit")	, _TL("Lower Limit of real data")	, PARAMETER_TYPE_Double	, 0);

	/*
	Parameters.Add_Select(	NULL, "METHOD"	, "Method"	, "Choose a method from this select option.",
		"First Method\0"
		"Second Method\0",
		0
	);
	*/
}

//---------------------------------------------------------
CtomaschTool::~CtomaschTool(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// The only thing left to do is to realize your ideas whithin
// the On_Execute() function (which will be called automatically
// by the framework). But that's really your job :-)

bool CtomaschTool::On_Execute(void)
{
	int		x, y;

	double	a, z;
	double  noDataValue, lowerLimit;

	CSG_Grid	*pInput, *pResult;

	//-----------------------------------------------------
	// Get user inputs from the 'Parameters' object...

	pInput		= Parameters("INPUT")->asGrid();
	pResult		= Parameters("RESULT")->asGrid();

	noDataValue	= Parameters("NoDataValue")->asDouble();
	lowerLimit = Parameters("LowerLimit")->asDouble();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			
			a = pInput->asDouble(x,y);
			if( a < lowerLimit ) z = noDataValue; else z = a;
			pResult->Set_Value(x, y, z );
		}
	}

	//-----------------------------------------------------
	return( true );
}
