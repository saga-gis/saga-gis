
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    crs_assign.cpp                     //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "crs_assign.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Assign::CCRS_Assign(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Set Coordinate Reference System"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Define or change the Coordinate Reference System (CRS) "
		"associated with the supplied data sets."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("EPSG Code"),
			_TL("Well Known Text from File"),
			_TL("User Defined"),
			_TL("Loaded Data Set")
		), 0
	);

	pNode	= Parameters.Add_Value(
		NULL	, "EPSG_CODE"	, _TL("EPSG Code"),
		_TL(""),
		PARAMETER_TYPE_Int, 4326, 2000, true, 32766, true
	);

	Parameters.Add_Choice(
		pNode	, "LIST_GEOGCS"	, _TL("Geographic Coordinate Systems"),
		_TL(""),
		SG_Get_Projections().Get_Names_List(SG_PROJ_TYPE_CS_Geographic)
	);

	Parameters.Add_Choice(
		pNode	, "LIST_PROJCS"	, _TL("Projected Coordinate Systems"),
		_TL(""),
		SG_Get_Projections().Get_Names_List(SG_PROJ_TYPE_CS_Projected)
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Well Known Text from File"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.prj|%s|*.wkt|%s|*.txt|%s|*.prj;*.wkt;*.txt|%s|*.*"),
			_TL("ESRI WKT Files (*.prj)"),
			_TL("WKT Files (*.wkt)"),
			_TL("Text Files (*.txt)"),
			_TL("All Recognized Files"),
			_TL("All Files")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Assign::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LIST_GEOGCS"))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LIST_PROJCS")) )
	{
		int		EPSG;

		if( pParameter->asChoice()->Get_Data(EPSG) )
		{
			pParameters->Get_Parameter("EPSG_CODE")->Set_Value(EPSG);
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Assign::On_Execute(void)
{
	CSG_Projection	Projection;

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	default:	// EPSG Code
		Projection.Create(Parameters("EPSG_CODE")->asInt());
		break;

	case 1:				// Well Known Text from File"),
		Projection.Create(Parameters("FILE")->asString());
		break;

	case 2:				// user defined"),
		break;

	case 3:				// loaded data set")
		break;
	}

	//-----------------------------------------------------
	if( !Projection.is_Okay() )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		nProjected	= 0;
	int		nFailed		= 0;

	Set_Projections(Projection, Parameters("GRIDS" )->asList(), nProjected, nFailed);
	Set_Projections(Projection, Parameters("SHAPES")->asList(), nProjected, nFailed);

	//-----------------------------------------------------
	return( nProjected > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Assign::Set_Projections(const CSG_Projection &Projection, CSG_Parameter_List *pList, int &nProjected, int &nFailed)
{
	for(int i=0; i<pList->Get_Count(); i++)
	{
		if( pList->asDataObject(i)->Get_Projection().Create(Projection) )
		{
			nProjected	++;
		}
		else
		{
			nFailed		++;
		}
	}

	return( nProjected );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
