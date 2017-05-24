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
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                crs_transform_shapes.cpp               //
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
#include "crs_transform_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_Shapes::CCRS_Transform_Shapes(bool bList)
{
	m_bList	= bList;

	//-----------------------------------------------------
	Set_Name		(CSG_String::Format("%s (%s)", _TL("Coordinate Transformation"),
		bList ? _TL("Shapes List") : _TL("Shapes")
	));

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		"Coordinate transformation for shapes."
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	if( m_bList )
	{
		Parameters.Add_Shapes_List("",
			"SOURCE"	, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Shapes_List("",
			"TARGET"	, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}

	//-----------------------------------------------------
	else
	{
		Parameters.Add_Shapes("",
			"SOURCE"	, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Shapes("",
			"TARGET"	, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Shapes::On_Execute_Transformation(void)
{
	if( m_bList )
	{
		CSG_Parameter_Shapes_List	*pSources, *pTargets;

		pSources	= Parameters("SOURCE")->asShapesList();
		pTargets	= Parameters("TARGET")->asShapesList();

		pTargets->Del_Items();

		for(int i=0; i<pSources->Get_Item_Count() && Process_Get_Okay(false); i++)
		{
			CSG_Shapes	*pSource	= pSources->Get_Shapes(i);
			CSG_Shapes	*pTarget	= SG_Create_Shapes(pSource->Get_Type(), pSource->Get_Name(), pSource, pSource->Get_Vertex_Type());

			if( Transform(pSource, pTarget) )
			{
				pTargets->Add_Item(pTarget);
			}
			else
			{
				delete(pTarget);
			}
		}

		return( pTargets->Get_Item_Count() > 0 );
	}
	else
	{
		CSG_Shapes	*pSource	= Parameters("SOURCE")->asShapes();
		CSG_Shapes	*pTarget	= Parameters("TARGET")->asShapes();

		if( pSource == pTarget )
		{
			pTarget	= SG_Create_Shapes(pSource->Get_Type(), pSource->Get_Name(), pSource, pSource->Get_Vertex_Type());

			if( Transform(pSource, pTarget) )
			{
				pSource->Assign(pTarget);

				return( true );
			}
			else
			{
				delete(pTarget);

				return( false );
			}
		}
		else
		{
			pTarget->Create(pSource->Get_Type(), pSource->Get_Name(), pSource, pSource->Get_Vertex_Type());
		}

		return( Transform(pSource, pTarget) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Shapes::Transform(CSG_Shapes *pSource, CSG_Shapes *pTarget)
{
	if( !pTarget || !pSource || !pSource->is_Valid() )
	{
		return( false );
	}

	if( !m_Projector.Set_Source(pSource->Get_Projection()) )
	{
		return( false );
	}

	int		nDropped	= 0;

	Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("Processing"), pSource->Get_Name()));

	for(int iShape=0; iShape<pSource->Get_Count() && Set_Progress(iShape, pSource->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape_Source	= pSource->Get_Shape(iShape);
		CSG_Shape	*pShape_Target	= pTarget->Add_Shape(pShape_Source, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pShape_Source->Get_Part_Count() && pShape_Target; iPart++)
		{
			for(int iPoint=0; iPoint<pShape_Source->Get_Point_Count(iPart) && pShape_Target; iPoint++)
			{
				TSG_Point	Point	= pShape_Source->Get_Point(iPoint, iPart);

				bool bSuccess = false;

				if( pShape_Source->Get_Vertex_Type() == SG_VERTEX_TYPE_XY )
				{
					if( m_Projector.Get_Projection(Point.x, Point.y) )
					{
						pShape_Target->Add_Point(Point.x, Point.y, iPart);
						bSuccess = true;
					}
				}
				else
				{
					double z = pShape_Source->Get_Z(iPoint, iPart);

					if( m_Projector.Get_Projection(Point.x, Point.y, z) )
					{
						pShape_Target->Add_Point(Point.x, Point.y, iPart);
						pShape_Target->Set_Z(z, iPoint, iPart);
						bSuccess = true;

						if( pShape_Source->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							pShape_Target->Set_M(pShape_Source->Get_M(iPoint, iPart), iPoint, iPart);
						}
					}
				}

				if (!bSuccess)
				{
					nDropped++;

					pTarget->Del_Shape(pShape_Target);

					pShape_Target	= NULL;
				}
			}
		}
	}

	if( nDropped > 0 )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %d %s"), pTarget->Get_Name(), nDropped, _TL("shapes have been dropped")));
	}

	pTarget->Get_Projection() = m_Projector.Get_Target();

	return( pTarget->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
