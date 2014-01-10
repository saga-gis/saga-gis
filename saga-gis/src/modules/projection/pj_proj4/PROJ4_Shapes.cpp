/**********************************************************
 * Version $Id: PROJ4_Shapes.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
CPROJ4_Shapes::CPROJ4_Shapes(int Interface, bool bInputList)
	: CPROJ4_Base(Interface, bInputList)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(CSG_String::Format(SG_T("%s (%s, %s)"),
		_TL("Proj.4"),
		Interface == PROJ4_INTERFACE_DIALOG ? _TL("Dialog") : _TL("Command Line Arguments"),
		m_bInputList ? _TL("List of Shapes Layers") : _TL("Shapes")
	));

	Set_Author		(SG_T("O. Conrad (c) 2004-8"));

	Set_Description	(_TW(
		"Coordinate Transformation for Shapes.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4 Homepage</a>\n"
	));


	//-----------------------------------------------------
	// 2. In-/Output...

	if( m_bInputList )
	{
		Parameters.Add_Shapes_List(
			Parameters("SOURCE_NODE")	, "SOURCE", _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Shapes_List(
			NULL						, "TARGET", _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}
	else
	{
		Parameters.Add_Shapes(
			Parameters("SOURCE_NODE")	, "SOURCE", _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Shapes(
			Parameters("TARGET_NODE")	, "TARGET", _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Shapes::On_Execute_Conversion(void)
{
	bool	bResult	= false;

	CSG_Shapes	*pSource, *pTarget;

	//-----------------------------------------------------
	if( m_bInputList )
	{
		CSG_Parameter_Shapes_List	*pSources, *pTargets;

		pSources	= Parameters("SOURCE")->asShapesList();
		pTargets	= Parameters("TARGET")->asShapesList();

		pTargets->Del_Items();

		for(int i=0; i<pSources->Get_Count() && Process_Get_Okay(false); i++)
		{
			pSource	= pSources->asShapes(i);
			pTarget	= SG_Create_Shapes();

			if( _Get_Conversion(pSource, pTarget) )
			{
				bResult	= true;
				pTargets->Add_Item(pTarget);
			}
			else
			{
				delete( pTarget );
			}
		}
	}
	else
	{
		bool	bCopy;

		pSource	= Parameters("SOURCE")->asShapes();
		pTarget	= Parameters("TARGET")->asShapes();

		if( (bCopy = pSource == pTarget) == true )
		{
			pTarget	= SG_Create_Shapes();
		}

		//-------------------------------------------------
		bResult	= _Get_Conversion(pSource, pTarget);

		//-------------------------------------------------
		if( bCopy )
		{
			pSource->Assign(pTarget);
			delete( pTarget );
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Shapes::_Get_Conversion(CSG_Shapes *pSource, CSG_Shapes *pTarget)
{
	if( pSource && pSource->is_Valid() && pTarget )
	{
		int		nDropped	= 0;

		Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("Processing"), pSource->Get_Name()));

		pTarget->Create(pSource->Get_Type(), CSG_String::Format(SG_T("%s [%s]"), pSource->Get_Name(), Get_Proj_Name().c_str()), pSource);

		for(int iShape=0; iShape<pSource->Get_Count() && Set_Progress(iShape, pSource->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape_Source	= pSource->Get_Shape(iShape);
			CSG_Shape	*pShape_Target	= pTarget->Add_Shape(pShape_Source, SHAPE_COPY_ATTR);

			bool	bDropped	= false;

			for(int iPart=0; iPart<pShape_Source->Get_Part_Count() && !bDropped; iPart++)
			{
				for(int iPoint=0; iPoint<pShape_Source->Get_Point_Count(iPart) && !bDropped; iPoint++)
				{
					TSG_Point	Point	= pShape_Source->Get_Point(iPoint, iPart);

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

		if( nDropped > 0 )
		{
			Message_Add(CSG_String::Format(SG_T("%d %s"), nDropped, _TL("shapes have been dropped")));
		}

		return( pTarget->Get_Count() > 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
