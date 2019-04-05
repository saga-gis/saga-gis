
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

		Parameters.Add_PointCloud("",
			"TARGET_PC"	, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT
		);
	}

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"TRANSFORM_Z"	, _TL("Z Transformation"),
		_TL("Transform elevation (z) values, if appropriate."),
		true
	);

	Parameters.Add_Bool("",
		"COPY"			, _TL("Copy"),
		_TL("If set the projected data will be created as a copy of the orignal, if not vertices will be projected in place thus reducing memory requirements."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Shapes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !m_bList )
	{
		if( pParameter->Cmp_Identifier("SOURCE")
		||  pParameter->Cmp_Identifier("COPY"  ) )
		{
			CSG_Parameter	*pSource	= (*pParameters)("SOURCE");

			bool	bCreate	= (*pParameters)("COPY")->asBool() && pSource->asDataObject();

			pParameters->Set_Enabled("TARGET"   , bCreate && pSource->asPointCloud() == NULL);
			pParameters->Set_Enabled("TARGET_PC", bCreate && pSource->asPointCloud() != NULL);

			pParameters->Set_Enabled("TRANSFORM_Z", pSource->asDataObject() &&
				((CSG_Shapes *)pSource->asDataObject())->Get_Vertex_Type() >= SG_VERTEX_TYPE_XYZ
			);
		}
	}

	return( CCRS_Transform::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Shapes::On_Execute_Transformation(void)
{
	if( m_bList )
	{
		CSG_Parameter_Shapes_List	*pSources	= Parameters("SOURCE")->asShapesList();
		CSG_Parameter_Shapes_List	*pTargets	= Parameters("TARGET")->asShapesList();

		pTargets->Del_Items();

		bool	bResult	= true;

		for(int i=0; i<pSources->Get_Item_Count() && Process_Get_Okay(false); i++)
		{
			CSG_Shapes	*pTarget	= pSources->Get_Shapes(i);

			if( Parameters("COPY")->asBool() )
			{
				pTarget	= SG_Create_Shapes(*pTarget);
			}

			pTargets->Add_Item(pTarget);

			if( !Transform(pTarget) )
			{
				bResult	= false;
			}
		}

		return( bResult );
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shapes	*pSource	= Parameters("SOURCE"   )->asShapes();
		CSG_Shapes	*pTarget	= Parameters("COPY"     )->asBool() == false ? NULL : pSource->asPointCloud()
								? Parameters("TARGET_PC")->asShapes()
								: Parameters("TARGET"   )->asShapes();

		if( pTarget && pTarget != pSource )
		{
			pTarget->Create(*pSource);
		}
		else
		{
			pTarget	= pSource;
		}

		bool	bResult	= Transform(pTarget);

		if( pTarget == pSource )
		{
			DataObject_Update(pSource);
		}

		return( bResult );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Shapes::Transform(CSG_Shapes *pShapes)
{
	if( !pShapes || !pShapes->is_Valid() )
	{
		return( false );
	}

	if( !m_Projector.Set_Source(pShapes->Get_Projection()) )
	{
		return( false );
	}

	bool	bTransform_Z	= Parameters("TRANSFORM_Z")->asBool() && pShapes->Get_Vertex_Type() >= SG_VERTEX_TYPE_XYZ;

	Process_Set_Text("%s: %s", _TL("Processing"), pShapes->Get_Name());

	int	nDropped = 0, nShapes = pShapes->Get_Count();

	for(int i=0, j=nShapes-1; i<nShapes && Set_Progress(i, nShapes); i++, j--)
	{
		if( pShapes->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud )
		{
			TSG_Point_Z	p	= pShapes->asPointCloud()->Get_Point(j);

			if( bTransform_Z ? m_Projector.Get_Projection(p.x, p.y, p.z) : m_Projector.Get_Projection(p.x, p.y) )
			{
				pShapes->asPointCloud()->Set_Point(j, p);
			}
			else
			{
				pShapes->asPointCloud()->Del_Point(j);

				nDropped++;
			}
		}
		else
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(j);

			bool	bOkay	= true;

			for(int iPart=0; bOkay && iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; bOkay && iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

					if( bTransform_Z )
					{
						double	z	= pShape->Get_Z(iPoint, iPart);

						if( (bOkay = m_Projector.Get_Projection(p.x, p.y, z)) == true )
						{
							pShape->Set_Z(z, iPoint, iPart);

							pShape->Set_Point(p.x, p.y, iPoint, iPart);
						}
					}
					else if( (bOkay = m_Projector.Get_Projection(p.x, p.y)) == true )
					{
						pShape->Set_Point(p.x, p.y, iPoint, iPart);
					}
				}
			}

			if( !bOkay )
			{
				pShapes->Del_Shape(j);

				nDropped++;
			}
		}
	}

	if( nDropped > 0 )
	{
		Message_Fmt("\n%s: %s [%d]", pShapes->Get_Name(), _TL("not all features have been projected"), nDropped, nShapes);
	}

	pShapes->Get_Projection() = m_Projector.Get_Target();

	return( pShapes->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
