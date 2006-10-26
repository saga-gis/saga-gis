
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
//                   PROJ4_Shapes.cpp                    //
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
#include "PROJ4_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPROJ4_Shapes::CPROJ4_Shapes(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name	(_TL("Proj4 (Shapes)"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Coordinate Transformation for Shapes.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://remotesensing.org/proj/\">Homepage</a>\n"
	));


	//-----------------------------------------------------
	// 2. In-/Output...

	Parameters.Add_Shapes(
		Parameters("SOURCE_NODE"), "SOURCE", _TL("Source"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		Parameters("TARGET_NODE"), "TARGET", _TL("Target"),
		"",
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CPROJ4_Shapes::~CPROJ4_Shapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Shapes::On_Execute_Conversion(void)
{
	bool		bCopy, bDropped;
	char		sMessage[256];
	int			iShape, iPart, iPoint, nDropped;
	TSG_Point	Point;
	CSG_Shape		*pShape_Source, *pShape_Target;
	CSG_Shapes		*pSource, *pTarget;

	//-----------------------------------------------------
	if( 1 )
	{
		pSource		= Parameters("SOURCE")->asShapes();
		pTarget		= Parameters("TARGET")->asShapes();

		if( pSource == pTarget )
		{
			bCopy		= true;

			pTarget		= SG_Create_Shapes();
		}
		else
		{
			bCopy		= false;
		}

		pTarget->Create(pSource->Get_Type(), pSource->Get_Name(), &pSource->Get_Table());

		//-------------------------------------------------
		for(iShape=0, nDropped=0; iShape<pSource->Get_Count() && Set_Progress(iShape, pSource->Get_Count()); iShape++)
		{
			pShape_Source	= pSource->Get_Shape(iShape);
			pShape_Target	= pTarget->Add_Shape(pShape_Source->Get_Record());

			for(iPart=0, bDropped=false; iPart<pShape_Source->Get_Part_Count() && !bDropped; iPart++)
			{
				for(iPoint=0; iPoint<pShape_Source->Get_Point_Count(iPart) && !bDropped; iPoint++)
				{
					Point	= pShape_Source->Get_Point(iPoint, iPart);

					if( Get_Converted(Point.x, Point.y) )
					{
						pShape_Target->Add_Point(Point.x, Point.y, iPart);
					}
					else
					{
						bDropped	= true;
					}
				}
			}

			if( bDropped )
			{
				nDropped++;
				pTarget->Del_Shape(pShape_Target);
			}
		}

		//-------------------------------------------------
		if( nDropped > 0 )
		{
			sprintf(sMessage, _TL("%d have been dropped"), nDropped);
			Message_Add(sMessage);
		}

		if( bCopy )
		{
			pSource->Assign(pTarget);

			delete( pTarget );
		}

		return( true );
	}

	return( false );
}
