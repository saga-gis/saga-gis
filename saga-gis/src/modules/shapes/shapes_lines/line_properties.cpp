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
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  line_properties.cpp                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
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
#include "line_properties.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Properties::CLine_Properties(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Properties"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Line properties: length, number of vertices."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Lines with Property Attributes"),
		_TL("If not set property attributes will be added to the orignal layer."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Value(
		NULL	, "BPARTS"		, _TL("Number of Parts"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "BPOINTS"		, _TL("Number of Vertices"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "BLENGTH"		, _TL("Length"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Properties::On_Execute(void)
{
	//-------------------------------------------------
	int	bParts	= Parameters("BPARTS")	->asBool() ? 0 : -1;
	int	bPoints	= Parameters("BPOINTS")	->asBool() ? 0 : -1;
	int	bLength	= Parameters("BLENGTH")	->asBool() ? 0 : -1;

	if( bParts && bPoints && bLength )
	{
		Error_Set(_TL("no properties selected"));

		return( false );
	}

	//-------------------------------------------------
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid lines layer"));

		return( false );
	}

	if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pLines )
	{
		pLines	= Parameters("OUTPUT")->asShapes();
		pLines->Create(*Parameters("LINES")->asShapes());
	}

	//-------------------------------------------------
	if( !bParts )	{	bParts	= pLines->Get_Field_Count();	pLines->Add_Field(SG_T("NPARTS") , SG_DATATYPE_Int   );	}
	if( !bPoints )	{	bPoints	= pLines->Get_Field_Count();	pLines->Add_Field(SG_T("NPOINTS"), SG_DATATYPE_Int   );	}
	if( !bLength )	{	bLength	= pLines->Get_Field_Count();	pLines->Add_Field(SG_T("LENGTH") , SG_DATATYPE_Double);	}

	//-------------------------------------------------
	for(int i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++)
	{
		CSG_Shape	*pLine	= pLines->Get_Shape(i);

		if( bParts  >= 0 )	pLine->Set_Value(bParts , pLine->Get_Part_Count());
		if( bPoints >= 0 )	pLine->Set_Value(bPoints, pLine->Get_Point_Count());
		if( bLength >= 0 )	pLine->Set_Value(bLength, ((CSG_Shape_Line *)pLine)->Get_Length());
	}

	//-------------------------------------------------
	if( pLines == Parameters("LINES")->asShapes() )
	{
		DataObject_Update(pLines);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
