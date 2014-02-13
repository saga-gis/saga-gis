/**********************************************************
 * Version $Id: crs_assign.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
#include "crs_assign.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Assign::CCRS_Assign(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Set Coordinate Reference System"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"The module allows to define the Coordinate Reference System (CRS) "
		"of the supplied data sets. The module applies no transformation to "
		"the data sets, it just updates their CRS metadata.\n"
		"A complete and correct description of the CRS of a dataset is necessary "
		"in order to be able to actually apply a projection with one of the "
		"'Coordinate Transformation' modules later.\n\n"
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

//	Parameters.Add_TIN_List(
//		NULL	, "TINS"	, _TL("TINs"),
//		_TL(""),
//		PARAMETER_INPUT_OPTIONAL
//	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Assign::On_Execute(void)
{
	int	nTotal	= Parameters("GRIDS" )->asList()->Get_Count()
				+ Parameters("SHAPES")->asList()->Get_Count();
	//			+ Parameters("TINS"  )->asList()->Get_Count();

	if( nTotal <= 0 )
	{
		Message_Dlg(_TL("nothing to do: no data in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Projection	Projection;

	if( !Get_Projection(Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	nProjected	= 0;

	nProjected	+= Set_Projections(Projection, Parameters("GRIDS" )->asList());
	nProjected	+= Set_Projections(Projection, Parameters("SHAPES")->asList());
//	nProjected	+= Set_Projections(Projection, Parameters("TINS")  ->asList());

	//-----------------------------------------------------
	return( nProjected > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Assign::Set_Projections(const CSG_Projection &Projection, CSG_Parameter_List *pList)
{
	int	nProjected	= 0;

	for(int i=0; i<pList->Get_Count(); i++)
	{
		if( pList->asDataObject(i)->Get_Projection().Create(Projection) )
		{
			pList->asDataObject(i)->Set_Modified();

			DataObject_Update(pList->asDataObject(i));

			nProjected++;
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
