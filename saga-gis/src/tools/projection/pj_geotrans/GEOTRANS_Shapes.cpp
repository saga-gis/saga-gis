
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Projection_GeoTRANS                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  GEOTRANS_Shapes.cpp                  //
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
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GEOTRANS_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEOTRANS_Shapes::CGEOTRANS_Shapes(void)
{
	Set_Name		(_TL("GeoTrans (Shapes)"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Coordinate Transformation for Shapes. "
		"This library makes use of the Geographic Translator (GeoTrans) library.\n\n"
		"GeoTrans is maintained by the National Geospatial Agency (NGA)."
	));

	Add_Reference("https://earth-info.nga.mil/GandG/geotrans/", SG_T("GeoTrans, National Geospatial Agency (NGA)"));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		Parameters("SOURCE_NODE"), "SOURCE", _TL("Source"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		Parameters("TARGET_NODE"), "TARGET", _TL("Target"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Shapes::On_Execute_Conversion(void)
{
	CSG_Shapes *pSource = Parameters("SOURCE")->asShapes();
	CSG_Shapes *pTarget = Parameters("TARGET")->asShapes();

	bool bCopy = pSource == pTarget;
	
	if( bCopy )
	{
		pTarget = SG_Create_Shapes();
	}

	pTarget->Create(pSource->Get_Type(), pSource->Get_Name(), pSource);

	//-------------------------------------------------
	sLong nDropped = 0;

	for(sLong iShape=0; iShape<pSource->Get_Count() && Set_Progress(iShape, pSource->Get_Count()); iShape++)
	{
		CSG_Shape *pShape_Source = pSource->Get_Shape(iShape);
		CSG_Shape *pShape_Target = pTarget->Add_Shape(pShape_Source, SHAPE_COPY_ATTR);

		bool bDropped = false;

		for(int iPart=0; !bDropped && iPart<pShape_Source->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape_Source->Get_Point_Count(iPart) && !bDropped; iPoint++)
			{
				TSG_Point Point = pShape_Source->Get_Point(iPoint, iPart);

				if( Get_Converted(Point.x, Point.y) )
				{
					pShape_Target->Add_Point(Point.x, Point.y, iPart);
				}
				else
				{
					bDropped = true;
				}
			}
		}

		if( bDropped )
		{
			nDropped++; pTarget->Del_Shape(pShape_Target);
		}
	}

	//-------------------------------------------------
	if( nDropped > 0 )
	{
		Message_Fmt("\n%s: %d", _TL("number of dropped shapes"), nDropped);
	}

	if( bCopy )
	{
		pSource->Assign(pTarget);

		delete(pTarget);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
