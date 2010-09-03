
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
//                   crs_transform.cpp                   //
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
#include "crs_transform.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform::CCRS_Transform(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Coordinate Transformation"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Define or change the Coordinate Reference System (CRS) "
		"associated with the supplied data sets."
	));

	//-----------------------------------------------------
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
int CCRS_Transform::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CCRS_Transform::On_Execute(void)
{
	CSG_Projection	Projection;

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

	if( !Projection.is_Okay() )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool	bResult	= false;

	m_pPrjSrc	= NULL;
	m_pPrjDst	= NULL;

	m_bInverse	= false;

	//-------------------------------------------------
	if(	_Set_Projection(Projection, &m_pPrjDst, true) )
	{
		bResult	= On_Execute_Conversion();
	}

	//-------------------------------------------------
	if( m_pPrjSrc )
	{
		pj_free(m_pPrjSrc);

		m_pPrjSrc	= NULL;
	}

	if( m_pPrjDst )
	{
		pj_free(m_pPrjDst);

		m_pPrjDst	= NULL;
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform::_Set_Projection(const CSG_Projection &Projection, PJ **ppPrj, bool bInverse)
{
	if( *ppPrj )
	{
		pj_free(*ppPrj);

		*ppPrj	= NULL;
	}

	//-------------------------------------------------
	if( (*ppPrj = pj_init_plus(SG_STR_SGTOMB(Projection.Get_Proj4()))) == NULL )
	{
		Error_Set(CSG_String::Format(SG_T("%s:\n%s"), _TL("projection initialization failure"), SG_STR_MBTOSG(pj_strerrno(pj_errno))));

		return( false );
	}

	//-------------------------------------------------
	if( bInverse && (*ppPrj)->inv == NULL )
	{
		Error_Set(_TL("Inverse transformation not available for selected projection type."));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CCRS_Transform::Set_Source(const CSG_Projection &Projection)
{
	return( _Set_Projection(Projection, &m_pPrjSrc, false) );
}

//---------------------------------------------------------
bool CCRS_Transform::Set_Inverse(bool bOn)
{
	if( m_bInverse == bOn )
	{
		return( true );
	}

	if( m_pPrjDst && m_pPrjDst->inv )
	{
		m_bInverse	= bOn;

		PJ	*tmp	= m_pPrjSrc;
		m_pPrjSrc	= m_pPrjDst;
		m_pPrjDst	= tmp;

		return( true );
	}

	Error_Set(_TL("Inverse transformation not available for selected projection type."));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform::Get_Converted(double &x, double &y)
{
	if( m_pPrjSrc && m_pPrjDst )
	{
		double	z	= 0.0;

		if( pj_is_latlong(m_pPrjSrc) )
		{
			x	*= DEG_TO_RAD;
			y	*= DEG_TO_RAD;
		}

		if( pj_transform(m_pPrjSrc, m_pPrjDst, 1, 0, &x, &y, &z) == 0 )
		{
			if( pj_is_latlong(m_pPrjDst) )
			{
				x	*= RAD_TO_DEG;
				y	*= RAD_TO_DEG;
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_Shapes::CCRS_Transform_Shapes(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Coordinate Transformation (Shapes)"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"Coordinate Transformation for Shapes.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4 Homepage</a>\n"
	));


	//-----------------------------------------------------
	// 2. In-/Output...

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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Shapes::On_Execute_Conversion(void)
{
	bool	bResult	= false;

	CSG_Shapes	*pSource, *pTarget;

	//-----------------------------------------------------
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

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Shapes::_Get_Conversion(CSG_Shapes *pSource, CSG_Shapes *pTarget)
{
	if( pSource && pSource->is_Valid() && pTarget && Set_Source(pSource->Get_Projection()) )
	{
		int		nDropped	= 0;

		Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("Processing"), pSource->Get_Name()));

		pTarget->Create(pSource->Get_Type(), pSource->Get_Name(), pSource);

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
