/**********************************************************
 * Version $Id: gcs_lon_range.cpp 1354 2012-03-15 16:51:39Z oconrad $
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
//                   gcs_lon_range.cpp                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#include "gcs_lon_range.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGCS_Grid_Longitude_Range::CGCS_Grid_Longitude_Range(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Change Longitudinal Range for Grids"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Change the longitudinal range of grids using geographic coordinates, "
		"i.e. from 0 - 360 to -180 - 180 and vice versa."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "INPUT"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "DIRECTION"	, _TL("Direction"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			SG_T("0 - 360 >> -180 - 180"),
			SG_T("-180 - 180 >> 0 - 360")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Grid_Longitude_Range::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pInput		= Parameters("INPUT" )->asGridList();
	CSG_Parameter_Grid_List	*pOutput	= Parameters("OUTPUT")->asGridList();

	if( pInput->Get_Count() <= 0 )
	{
		Message_Dlg(_TL("nothing to do: no data in selection"));

		return( false );
	}

	pOutput->Del_Items();

	//-----------------------------------------------------
	int				xZero;
	CSG_Grid_System	Target;

	//-----------------------------------------------------
	if( Parameters("DIRECTION")->asInt() == 0 )	// 0 - 360 >> -180 - 180
	{
		if( Get_XMax() <= 180.0 )
		{
			Message_Add(_TL("Nothing to do. Raster is already within target range."));

			return( true );
		}
		else if( Get_XMin() >= 180.0 )
		{
			xZero	= 0;

			Target.Assign(Get_Cellsize(), Get_XMin() - 360.0, Get_YMin(), Get_NX(), Get_NY());
		}
		else if( Get_XMax() - 360.0 < Get_XMin() - Get_Cellsize() )
		{
			Error_Set(_TL("Nothing to do be done. Raster splitting is not supported."));

			return( false );
		}
		else
		{
			xZero	= (int)(0.5 + 180.0 / Get_Cellsize());

			Target.Assign(Get_Cellsize(), Get_XMin() - 180.0, Get_YMin(), Get_NX(), Get_NY());
		}
	}

	//-----------------------------------------------------
	else										// -180 - 180 >> 0 - 360
	{
		if( Get_XMin() >= 0.0 )
		{
			Message_Add(_TL("Nothing to do. Raster is already within target range."));

			return( true );
		}
		else if( Get_XMax() <= 0.0 )
		{
			xZero	= 0;

			Target.Assign(Get_Cellsize(), Get_XMin() + 360.0, Get_YMin(), Get_NX(), Get_NY());
		}
		else if( Get_XMin() + 360.0 > Get_XMax() + Get_Cellsize() )
		{
			Error_Set(_TL("Nothing to do be done. Raster splitting is not supported."));

			return( false );
		}
		else
		{
			xZero	= (int)(0.5 + 180.0 / Get_Cellsize());

			Target.Assign(Get_Cellsize(), Get_XMin() - 180.0, Get_YMin(), Get_NX(), Get_NY());
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<pInput->Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid	*pIn	= pInput->asGrid(i);
		CSG_Grid	*pOut	= SG_Create_Grid(Target, pIn->Get_Type());

		pOut->Set_Name(pIn->Get_Name());
		pOut->Set_NoData_Value_Range(pIn->Get_NoData_Value(), pIn->Get_NoData_hiValue());
		pOut->Set_ZFactor(pIn->Get_ZFactor());

		pOutput->Add_Item(pOut);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0, xx=xZero; x<Get_NX(); x++, xx++)
			{
				if( xx >= Get_NX() )
				{
					xx	= 0;
				}

				pOut->Set_Value(xx, y, pIn->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
