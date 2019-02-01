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
//                       pj_proj4                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 crs_transform_utm.cpp                 //
//                                                       //
//                 Copyright (C) 2017 by                 //
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
#include "crs_transform_utm.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		CRS_Get_UTM_Zone	(const CSG_Rect &Extent, const CSG_Projection &Source, int &Zone, bool &bSouth)
{
	CSG_CRSProjector	Projector;

	Projector.Set_Target(CSG_Projection("+proj=longlat +datum=WGS84", SG_PROJ_FMT_Proj4));

	TSG_Point	Point	= Extent.Get_Center();

	if( Projector.Set_Source(Source) && Projector.Get_Projection(Point) )
	{
		if( Point.x < -180. )
		{
			Point.x	= 360. + fmod(Point.x, 360.);
		}

		Zone	= 1 + (int)fmod(floor((Point.x + 180.) / 6.), 60.);

		bSouth	= Point.y < 0.0;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Projection	CRS_Get_UTM_Projection	(int Zone, bool bSouth)
{
	CSG_Projection	UTM;

	int	EPSG_ID	= (bSouth ? 32700 : 32600) + Zone;

	if( UTM.Create(EPSG_ID) )
	{
		return( UTM );
	}

	//-----------------------------------------------------
	CSG_String	Proj4;

	Proj4.Printf("+proj=utm +zone=%d%s +datum=WGS84 +units=m +no_defs", Zone, bSouth ? SG_T(" +south") : SG_T(""));

	UTM.Create(Proj4, SG_PROJ_FMT_Proj4);

	return( UTM );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_UTM_Grids::CCRS_Transform_UTM_Grids(bool bList)
	: CCRS_Transform_Grid(bList)
{
	//-----------------------------------------------------
	Set_Name		(CSG_String::Format("%s (%s)", _TL("UTM Projection"),
		bList ? _TL("Grid List") : _TL("Grid")
	));

	Set_Author		("O. Conrad (c) 2017");

	Set_Description	(_TW(
		"Project grids into UTM coordinates."
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"UTM_ZONE"	, _TL("Zone"),
		_TL(""),
		1, 1, true, 60, true
	);

	Parameters.Add_Bool("",
		"UTM_SOUTH"	, _TL("South"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Set_Enabled("CRS_PROJ4", false);
	Parameters.Set_Enabled("PRECISE"  , false);
}

//---------------------------------------------------------
int CCRS_Transform_UTM_Grids::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("SOURCE") )
	{
		int Zone; bool bSouth; CSG_Data_Object *pObject = pParameter->is_DataObject() ? pParameter->asDataObject() : pParameter->asList()->Get_Item(0);

		if( pObject )
		{
			CSG_Grid	*pGrid	= pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid ? pObject->asGrid() : pObject->asGrids()->Get_Grid_Ptr(0);

			if( CRS_Get_UTM_Zone(pGrid->Get_Extent(), pGrid->Get_Projection(), Zone, bSouth) )
			{
				CSG_Projection	UTM	= CRS_Get_UTM_Projection(Zone, bSouth);

				pParameters->Set_Parameter("UTM_ZONE"     , Zone  );
				pParameters->Set_Parameter("UTM_SOUTH"    , bSouth);
				pParameters->Set_Parameter("CRS_PROJ4"    , UTM.Get_Proj4       ());
				pParameters->Set_Parameter("CRS_EPSG"     , UTM.Get_Authority_ID());
				pParameters->Set_Parameter("CRS_EPSG_AUTH", UTM.Get_Authority   ());

				return( CCRS_Transform_Grid::On_Parameter_Changed(pParameters, (*pParameters)
					(UTM.Get_EPSG() > 0 ? "CRS_EPSG" : "CRS_PROJ4")
				));
			}
		}
	}

	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("UTM_ZONE" )
	||  pParameter->Cmp_Identifier("UTM_SOUTH") )
	{
		CSG_Projection	UTM	= CRS_Get_UTM_Projection(
			(*pParameters)("UTM_ZONE" )->asInt (),
			(*pParameters)("UTM_SOUTH")->asBool()
		);

		pParameters->Set_Parameter("CRS_PROJ4"    , UTM.Get_Proj4       ());
		pParameters->Set_Parameter("CRS_EPSG"     , UTM.Get_Authority_ID());
		pParameters->Set_Parameter("CRS_EPSG_AUTH", UTM.Get_Authority   ());

		return( CCRS_Transform_Grid::On_Parameter_Changed(pParameters, (*pParameters)
			(UTM.Get_EPSG() > 0 ? "CRS_EPSG" : "CRS_PROJ4")
		));
	}

	//-----------------------------------------------------
	return( CCRS_Transform_Grid::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_UTM_Shapes::CCRS_Transform_UTM_Shapes(bool bList)
	: CCRS_Transform_Shapes(bList)
{
	//-----------------------------------------------------
	Set_Name		(CSG_String::Format("%s (%s)", _TL("UTM Projection"),
		bList ? _TL("Shapes List") : _TL("Shapes")
	));

	Set_Author		("O. Conrad (c) 2017");

	Set_Description	(_TW(
		"Project shapes into UTM coordinates."
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"UTM_ZONE"	, _TL("Zone"),
		_TL(""),
		1, 1, true, 60, true
	);

	Parameters.Add_Bool("",
		"UTM_SOUTH"	, _TL("South"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Set_Enabled("CRS_PROJ4", false);
	Parameters.Set_Enabled("PRECISE"  , false);
}

//---------------------------------------------------------
int CCRS_Transform_UTM_Shapes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("SOURCE") )
	{
		int Zone; bool bSouth; CSG_Shapes *pObject = (CSG_Shapes *)(pParameter->is_DataObject() ? pParameter->asDataObject() : pParameter->asShapesList()->Get_Item(0));

		if( pObject && CRS_Get_UTM_Zone(pObject->Get_Extent(), pObject->Get_Projection(), Zone, bSouth) )
		{
			CSG_Projection	UTM	= CRS_Get_UTM_Projection(Zone, bSouth);

			pParameters->Set_Parameter("UTM_ZONE"     , Zone  );
			pParameters->Set_Parameter("UTM_SOUTH"    , bSouth);
			pParameters->Set_Parameter("CRS_PROJ4"    , UTM.Get_Proj4       ());
			pParameters->Set_Parameter("CRS_EPSG"     , UTM.Get_Authority_ID());
			pParameters->Set_Parameter("CRS_EPSG_AUTH", UTM.Get_Authority   ());

			return( CCRS_Transform_Shapes::On_Parameter_Changed(pParameters, (*pParameters)
				(UTM.Get_EPSG() > 0 ? "CRS_EPSG" : "CRS_PROJ4")
			));
		}
	}

	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("UTM_ZONE" )
	||  pParameter->Cmp_Identifier("UTM_SOUTH") )
	{
		CSG_Projection	UTM	= CRS_Get_UTM_Projection(
			(*pParameters)("UTM_ZONE" )->asInt (),
			(*pParameters)("UTM_SOUTH")->asBool()
		);

		pParameters->Set_Parameter("CRS_PROJ4"    , UTM.Get_Proj4       ());
		pParameters->Set_Parameter("CRS_EPSG"     , UTM.Get_Authority_ID());
		pParameters->Set_Parameter("CRS_EPSG_AUTH", UTM.Get_Authority   ());

		return( CCRS_Transform_Shapes::On_Parameter_Changed(pParameters, (*pParameters)
			(UTM.Get_EPSG() > 0 ? "CRS_EPSG" : "CRS_PROJ4")
		));
	}

	//-----------------------------------------------------
	return( CCRS_Transform_Shapes::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_UTM_PointCloud::CCRS_Transform_UTM_PointCloud(bool bList)
	: CCRS_Transform_PointCloud(bList)
{
	//-----------------------------------------------------
	Set_Name		(CSG_String::Format("%s (%s)", _TL("UTM Projection"),
		bList ? _TL("Point Cloud List") : _TL("Point Cloud")
	));

	Set_Author		("O. Conrad (c) 2017");

	Set_Description	(_TW(
		"Project point clouds into UTM coordinates."
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"UTM_ZONE"	, _TL("Zone"),
		_TL(""),
		1, 1, true, 60, true
	);

	Parameters.Add_Bool("",
		"UTM_SOUTH"	, _TL("South"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Set_Enabled("CRS_PROJ4", false);
	Parameters.Set_Enabled("PRECISE"  , false);
}

//---------------------------------------------------------
int CCRS_Transform_UTM_PointCloud::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("SOURCE") )
	{
		int Zone; bool bSouth; CSG_Shapes *pObject = (CSG_Shapes *)(pParameter->is_DataObject() ? pParameter->asDataObject() : pParameter->asShapesList()->Get_Item(0));

		if( pObject && CRS_Get_UTM_Zone(pObject->Get_Extent(), pObject->Get_Projection(), Zone, bSouth) )
		{
			CSG_Projection	UTM	= CRS_Get_UTM_Projection(Zone, bSouth);

			pParameters->Set_Parameter("UTM_ZONE"     , Zone  );
			pParameters->Set_Parameter("UTM_SOUTH"    , bSouth);
			pParameters->Set_Parameter("CRS_PROJ4"    , UTM.Get_Proj4       ());
			pParameters->Set_Parameter("CRS_EPSG"     , UTM.Get_Authority_ID());
			pParameters->Set_Parameter("CRS_EPSG_AUTH", UTM.Get_Authority   ());

			return( CCRS_Transform_PointCloud::On_Parameter_Changed(pParameters, (*pParameters)
				(UTM.Get_EPSG() > 0 ? "CRS_EPSG" : "CRS_PROJ4")
			));
		}
	}

	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("UTM_ZONE" )
	||  pParameter->Cmp_Identifier("UTM_SOUTH") )
	{
		CSG_Projection	UTM	= CRS_Get_UTM_Projection(
			(*pParameters)("UTM_ZONE" )->asInt (),
			(*pParameters)("UTM_SOUTH")->asBool()
		);

		pParameters->Set_Parameter("CRS_PROJ4"    , UTM.Get_Proj4       ());
		pParameters->Set_Parameter("CRS_EPSG"     , UTM.Get_Authority_ID());
		pParameters->Set_Parameter("CRS_EPSG_AUTH", UTM.Get_Authority   ());

		return( CCRS_Transform_PointCloud::On_Parameter_Changed(pParameters, (*pParameters)
			(UTM.Get_EPSG() > 0 ? "CRS_EPSG" : "CRS_PROJ4")
		));
	}

	//-----------------------------------------------------
	return( CCRS_Transform_PointCloud::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
