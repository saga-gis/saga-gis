/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Shapes_Buffer.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes_buffer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Buffer::CShapes_Buffer(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Shapes Buffer"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"A vector based buffer construction partly based on the method supposed by Dong et al. 2003. "
		"\n\n"
		"References:\n"
		"Dong, P, Yang, C., Rui, X., Zhang, L., Cheng, Q. (2003): "
		"'An effective buffer generation method in GIS'. "
		"Geoscience and Remote Sensing Symposium, 2003. "
		"IGARSS '03. Proceedings. 2003 IEEE International, Vol.6, p.3706-3708.\n"
		"<a href=\"http://ieeexplore.ieee.org/iel5/9010/28606/01295244.pdf\">online version</a>\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "BUFFER"		, _TL("Buffer"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "DIST_TYPE"	, _TL("Buffer Distance"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("constant"),
			_TL("attribute")
		), 0
	);

	pNode	= Parameters.Add_Table_Field(
		pNode	, "DIST_FIELD"	, _TL("Buffer Distance Attribute"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "DIST_SCALE"	, _TL("Scaling Factor for Attribute Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DIST_CONST"	, _TL("Constant Buffer Distance"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DISSOLVE"	, _TL("Dissolve Buffers"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "NZONES"		, _TL("Number of Buffer Zones"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Value(
		NULL	, "POLY_INNER"	, _TL("Inner Buffer"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "DARC"		, _TL("Arc Vertex Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 5.0, 0.01, true, 45.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Buffer::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SHAPES")) )
	{
		pParameters->Get_Parameter("POLY_INNER")->Set_Enabled(pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Polygon);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NZONES")) )
	{
		pParameters->Get_Parameter("DISSOLVE"  )->Set_Enabled(pParameter->asInt() == 1);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DIST_TYPE")) )
	{
		pParameters->Get_Parameter("DIST_CONST")->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("DIST_FIELD")->Set_Enabled(pParameter->asInt() != 0);
		pParameters->Get_Parameter("DIST_SCALE")->Set_Enabled(pParameter->asInt() != 0);
	}

	//-----------------------------------------------------
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::On_Execute(void)
{
	int			Field, nZones;
	CSG_Shapes	*pShapes, *pBuffers;

	//-----------------------------------------------------
	pShapes			= Parameters("SHAPES"    )->asShapes();
	pBuffers		= Parameters("BUFFER"    )->asShapes();
	nZones			= Parameters("NZONES"    )->asInt();
	Field			= Parameters("DIST_TYPE" )->asInt() == 0 ? -1
					: Parameters("DIST_FIELD")->asInt();
	m_dArc			= Parameters("DARC"      )->asDouble() * M_DEG_TO_RAD;
	m_bPolyInner	= Parameters("POLY_INNER")->asBool() && pShapes->Get_Type() == SHAPE_TYPE_Polygon;

	//-----------------------------------------------------
	if( !pShapes->is_Valid() )
	{
		Message_Add(_TL("Invalid Shapes"));

		return( false );
	}

	if( Field < 0 && Parameters("DIST_CONST")->asDouble() <= 0.0 )
	{
		Message_Add(_TL("Invalid Buffer Distance"));

		return( false );
	}

	//-----------------------------------------------------
	if( nZones == 1 )
	{
		Get_Buffers(pShapes, Field, pBuffers, 1.0, Parameters("DISSOLVE")->asBool());
	}

	//-----------------------------------------------------
	else if( nZones > 1 )
	{
		CSG_Shape	*pBuffer;
		CSG_Shapes	Buffers;

		pBuffers->Create(SHAPE_TYPE_Polygon);
		pBuffers->Add_Field(_TL("ID")	, SG_DATATYPE_Int);
		pBuffers->Add_Field(_TL("ZONE")	, SG_DATATYPE_Double);

		double	dZone	= 1.0 / nZones;

		for(int iZone=0; iZone<nZones; iZone++)
		{
			Get_Buffers(pShapes, Field, &Buffers, (nZones - iZone) * dZone, true);

			if( iZone > 0 )
			{
				SG_Polygon_Difference(pBuffer, Buffers.Get_Shape(0));
			}

			pBuffer	= pBuffers->Add_Shape(Buffers.Get_Shape(0));
			pBuffer	->Set_Value(0, (nZones - iZone) + 1);
			pBuffer	->Set_Value(1, (nZones - iZone) * dZone * 100.0);
		}
	}

	//-----------------------------------------------------
	pBuffers->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Buffer")));

	return( pBuffers->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffers(CSG_Shapes *pShapes, int Field, CSG_Shapes *pBuffers, double Scale, bool bDissolve)
{
	//-----------------------------------------------------
	double		Distance;
	CSG_Shapes	Part(SHAPE_TYPE_Polygon);
	CSG_Shape	*pPart	= Part.Add_Shape(), *pBuffer;

	Distance	= Parameters("DIST_CONST")->asDouble() * Scale;
	Scale		= Parameters("DIST_SCALE")->asDouble() * Scale;

	if( !bDissolve )
	{
		pBuffers->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Buffer")), pShapes);
	}
	else
	{
		pBuffers->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Buffer")));
		pBuffers->Add_Field(_TL("ID"), SG_DATATYPE_Int);
		pBuffer	= pBuffers->Add_Shape();
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( Field < 0 || (Distance = Scale * pShape->asDouble(Field)) > 0.0 )
		{
			if( !bDissolve )
			{
				pBuffer	= pBuffers->Add_Shape(pShape, SHAPE_COPY_ATTR);
			}

			if( pBuffer->Get_Part_Count() == 0 )
			{
				Get_Buffer(pShape, pBuffer, Distance);
			}
			else
			{
				Get_Buffer(pShape, pPart  , Distance);

				SG_Polygon_Union(pBuffer, pPart);

				pPart->Del_Parts();
			}
		}
	}

	//-----------------------------------------------------
	return( pBuffers->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer(CSG_Shape *pShape, CSG_Shape *pBuffer, double Distance)
{
	switch( pShape->Get_Type() )
	{
	case SHAPE_TYPE_Point:		return( Get_Buffer_Point	(pShape, pBuffer, Distance) );
	case SHAPE_TYPE_Points:		return( Get_Buffer_Points	(pShape, pBuffer, Distance) );
	case SHAPE_TYPE_Line:		return( Get_Buffer_Line		(pShape, pBuffer, Distance) );
	case SHAPE_TYPE_Polygon:	return( Get_Buffer_Polygon	(pShape, pBuffer, Distance) );
	default:					return( false );
	}
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Point(CSG_Shape *pPoint, CSG_Shape *pBuffer, double Distance)
{
	Add_Arc(pBuffer, pPoint->Get_Point(0), Distance, 0.0, M_PI_360);

	return( true );
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Points(CSG_Shape *pPoints, CSG_Shape *pBuffer, double Distance)
{
	CSG_Shapes	Part(SHAPE_TYPE_Polygon);
	CSG_Shape	*pPart	= Part.Add_Shape();

	for(int iPart=0; iPart<pPoints->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pPoints->Get_Point_Count(iPart); iPoint++)
		{
			if( pBuffer->Get_Part_Count() == 0 )
			{
				Add_Arc(pBuffer, pPoints->Get_Point(iPoint), Distance, 0.0, M_PI_360);
			}
			else
			{
				Add_Arc(pPart  , pPoints->Get_Point(iPoint), Distance, 0.0, M_PI_360);

				SG_Polygon_Union(pBuffer, pPart);

				pPart->Del_Parts();
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Line(CSG_Shape *pLine, CSG_Shape *pBuffer, double Distance)
{
	CSG_Shapes	Part(SHAPE_TYPE_Polygon);
	CSG_Shape	*pPart	= Part.Add_Shape();

	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		if( pBuffer->Get_Part_Count() == 0 )
		{
			Add_Line(pBuffer, Distance, ((CSG_Shape_Line *)pLine)->Get_Part(iPart));
		}
		else
		{
			Add_Line(pPart  , Distance, ((CSG_Shape_Line *)pLine)->Get_Part(iPart));

			SG_Polygon_Union(pBuffer, pPart);

			pPart->Del_Parts();
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Polygon(CSG_Shape *pPolygon, CSG_Shape *pBuffer, double Distance)
{
	if( m_bPolyInner )
	{
		if(	SG_Polygon_Offset(pPolygon, -Distance, m_dArc, pBuffer) )
			SG_Polygon_Difference(pPolygon, pBuffer, pBuffer);
		else
			pBuffer->Assign(pPolygon, false);

		return( true );
	}

	return( SG_Polygon_Offset(pPolygon, Distance, m_dArc, pBuffer) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CShapes_Buffer::Get_Direction(const TSG_Point &From, const TSG_Point &To)
{
	double	dx	= To.x - From.x;
	double	dy	= To.y - From.y;

	return(	dx != 0.0 ?	M_PI_180 + atan2(dy, dx)	: (
			dy  > 0.0 ?	M_PI_270					: (
			dy  < 0.0 ?	M_PI_090					:
						0.0							) )
	);
}

//---------------------------------------------------------
inline bool CShapes_Buffer::Get_Parallel(const TSG_Point &A, const TSG_Point &B, TSG_Point AB[2], double Distance)
{
	double		d;
	TSG_Point	C;

	if( (d = SG_Get_Distance(A, B)) > 0.0 )
	{
		d		= Distance / d;

		C.x		= d * (A.y - B.y);
		C.y		= d * (B.x - A.x);

		AB[0].x	= A.x + C.x;
		AB[0].y	= A.y + C.y;

		AB[1].x	= B.x + C.x;
		AB[1].y	= B.y + C.y;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CShapes_Buffer::Add_Vertex(CSG_Shape *pBuffer, const TSG_Point &Center, double Distance, double theta)
{
	pBuffer->Add_Point(
		Center.x + Distance * cos(theta),
		Center.y + Distance * sin(theta)
	);
}

//---------------------------------------------------------
inline void CShapes_Buffer::Add_Arc(CSG_Shape *pBuffer, const TSG_Point &Center, double Distance, double alpha, double beta)
{
	double	theta;

	if( alpha < beta )
	{
		for(theta=alpha; theta<beta; theta+=m_dArc)
		{
			Add_Vertex(pBuffer, Center, Distance, theta);
		}
	}
	else
	{
		for(theta=alpha; theta>beta; theta-=m_dArc)
		{
			Add_Vertex(pBuffer, Center, Distance, theta);
		}
	}

	Add_Vertex(pBuffer, Center, Distance, beta);
}

//---------------------------------------------------------
inline void CShapes_Buffer::Add_Arc(CSG_Shape *pBuffer, const TSG_Point &Center, double Distance, const TSG_Point &A, const TSG_Point &B)
{
	double	alpha, beta;

	alpha	= Get_Direction(A, Center);
	beta	= Get_Direction(B, Center);

	if( alpha - beta >= M_PI_180 )
	{
		beta	+= M_PI_360;
	}

	Add_Arc(pBuffer, Center, Distance, alpha, beta);
}

//---------------------------------------------------------
void CShapes_Buffer::Add_Line(CSG_Shape *pBuffer, double Distance, CSG_Shape_Part *pLine)
{
	int		i;

	for(i=0; i<pLine->Get_Count(); i++)
	{
		pBuffer->Add_Point(pLine->Get_Point(i, true));
	}

	for(i=1; i<pLine->Get_Count(); i++)
	{
		pBuffer->Add_Point(pLine->Get_Point(i, false));
	}

	SG_Polygon_Offset(pBuffer, Distance, m_dArc, pBuffer);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
