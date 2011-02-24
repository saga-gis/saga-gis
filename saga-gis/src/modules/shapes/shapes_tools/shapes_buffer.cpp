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

#include "Polygon_Clipper.h"


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
		NULL	, "BUF_TYPE"	, _TL("Buffer Distance"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("fixed value"),
			_TL("attribute field")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "BUF_DIST"	, _TL("Buffer Distance (Fixed)"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	Parameters.Add_Table_Field(
		pNode	, "BUF_FIELD"	, _TL("Buffer Distance (Attribute)"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "BUF_SCALE"	, _TL("Scaling Factor for Attribute Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "BUF_ZONES"	, _TL("Number of Buffer Zones"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Value(
		NULL	, "DCIRCLE"		, _TL("Circle Point Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 5.0, 0.01, true, 45.0, true
	);

	Parameters.Add_Value(
		NULL	, "DISSOLVE"	, _TL("Dissolve Buffers"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}

//---------------------------------------------------------
CShapes_Buffer::~CShapes_Buffer(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::On_Execute(void)
{
	int			nZones;
	CSG_Shapes	*pBuffers;

	pBuffers	= Parameters("BUFFER")		->asShapes();
	nZones		= Parameters("BUF_ZONES")	->asInt();

	if( Initialise() )
	{
		if( nZones == 1 )
		{
			Get_Buffers(pBuffers, 1.0);
		}

		//-------------------------------------------------
		else if( nZones > 1 )
		{
			double		dZone		= 1.0 / nZones;
			CSG_Shape	*pBuffer;
			CSG_Shapes	Buffers;

			pBuffers->Create(SHAPE_TYPE_Polygon);
			pBuffers->Add_Field(_TL("ID")	, SG_DATATYPE_Int);
			pBuffers->Add_Field(_TL("ZONE")	, SG_DATATYPE_Double);

			for(int iZone=0; iZone<nZones; iZone++)
			{
				Get_Buffers(&Buffers, (nZones - iZone) * dZone);

				if( iZone > 0 )
				{
					GPC_Difference(pBuffer, Buffers.Get_Shape(0));
				}

				pBuffer	= pBuffers->Add_Shape(Buffers.Get_Shape(0));
				pBuffer	->Set_Value(0, (nZones - iZone) + 1);
				pBuffer	->Set_Value(1, (nZones - iZone) * dZone * 100.0);
			}
		}

		Finalise();

		pBuffers->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pShapes->Get_Name(), _TL("Buffer")));

		return( pBuffers->is_Valid() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffers(CSG_Shapes *pBuffers, double dZone)
{
	//-----------------------------------------------------
	if( pBuffers )
	{
		bool	bDissolve	= Parameters("DISSOLVE")	->asBool();

		if( bDissolve )
		{
			pBuffers	->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), m_pShapes->Get_Name(), _TL("Buffer")));
			pBuffers	->Add_Field(_TL("ID"), SG_DATATYPE_Int);
			m_pBuffer	= pBuffers->Add_Shape();
			m_pBuffer	->Set_Value(0, 1);
		}
		else
		{
			pBuffers	->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), m_pShapes->Get_Name(), _TL("Buffer")), m_pShapes);
		}

		m_Distance	= dZone * Parameters("BUF_DIST")	->asDouble();
		m_Scale		= dZone * Parameters("BUF_SCALE")	->asDouble();
		m_ID		= 0;

		for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= m_pShapes->Get_Shape(iShape);

			if( m_Type == 0 || (m_Distance = m_Scale * pShape->asDouble(m_Field)) > 0.0 )
			{
				if( !bDissolve )
				{
					m_pBuffer	= pBuffers->Add_Shape(pShape, SHAPE_COPY_ATTR);
				}

				switch( m_pShapes->Get_Type() )
				{
				case SHAPE_TYPE_Point:		Get_Buffer_Point	(pShape);	break;
				case SHAPE_TYPE_Points:		Get_Buffer_Points	(pShape);	break;
				case SHAPE_TYPE_Line:		Get_Buffer_Line		(pShape);	break;
				case SHAPE_TYPE_Polygon:	Get_Buffer_Polygon	(pShape);	break;
				}
			}
		}

		return( pBuffers->is_Valid() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::Initialise(void)
{
	//-----------------------------------------------------
	m_pShapes	= Parameters("SHAPES")		->asShapes();
	m_Type		= Parameters("BUF_TYPE")	->asInt();
	m_Field		= Parameters("BUF_FIELD")	->asInt();
	m_dArc		= Parameters("DCIRCLE")		->asDouble() * M_DEG_TO_RAD;

	//-----------------------------------------------------
	if( m_Type == 0 && Parameters("BUF_DIST")->asDouble() <= 0.0 )
	{
		Message_Add(_TL("Invalid Buffer Distance"));
	}
	else if( !m_pShapes->is_Valid() )
	{
		Message_Add(_TL("Invalid Shapes"));
	}

	//-----------------------------------------------------
	else
	{
		m_Tmp.Create(SHAPE_TYPE_Polygon);

		m_pSegment	= m_Tmp.Add_Shape();
		m_pUnion	= m_Tmp.Add_Shape();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CShapes_Buffer::Finalise(void)
{
	m_Tmp.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Point(CSG_Shape *pPoint)
{
	m_pSegment->Del_Parts();

	Add_Arc(pPoint->Get_Point(0), 0.0, M_PI_360);

	Add_Buffer();

	return( true );
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Points(CSG_Shape *pPoints)
{
	for(int iPart=0; iPart<pPoints->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pPoints->Get_Point_Count(iPart); iPoint++)
		{
			m_pSegment->Del_Parts();

			Add_Arc(pPoints->Get_Point(iPoint), 0.0, M_PI_360);

			Add_Buffer();
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Line(CSG_Shape *pLine)
{
	Del_Duplicates(pLine);

	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		m_pSegment->Del_Parts();

		Add_Line((CSG_Shape_Line *)pLine, iPart);

		Add_Buffer();
	}

	return( true );
}

//---------------------------------------------------------
bool CShapes_Buffer::Get_Buffer_Polygon(CSG_Shape *pPolygon)
{
	int		iPart;

	Del_Duplicates(pPolygon);

	for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		if( ((CSG_Shape_Polygon *)pPolygon)->is_Lake(iPart) == false )
		{
			m_pSegment->Del_Parts();

			Add_Polygon((CSG_Shape_Polygon *)pPolygon, iPart);

			Add_Buffer();
		}
	}

	for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		if( ((CSG_Shape_Polygon *)pPolygon)->is_Lake(iPart) == true )
		{
			m_pSegment->Del_Parts();

			Add_Polygon((CSG_Shape_Polygon *)pPolygon, iPart);

			Add_Buffer(true);
		}
	}

	return( true );
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
inline void CShapes_Buffer::Add_Vertex(const TSG_Point &Center, double theta)
{
	m_pSegment->Add_Point(
		Center.x + m_Distance * cos(theta),
		Center.y + m_Distance * sin(theta)
	);
}

//---------------------------------------------------------
void CShapes_Buffer::Add_Arc(const TSG_Point &Center, double alpha, double beta)
{
	double	theta;

	if( alpha < beta )
	{
		for(theta=alpha; theta<beta; theta+=m_dArc)
		{
			Add_Vertex(Center, theta);
		}
	}
	else
	{
		for(theta=alpha; theta>beta; theta-=m_dArc)
		{
			Add_Vertex(Center, theta);
		}
	}

	Add_Vertex(Center, beta);
}

//---------------------------------------------------------
void CShapes_Buffer::Add_Arc(const TSG_Point &Center, const TSG_Point &A, const TSG_Point &B)
{
	double	alpha, beta;

	alpha	= Get_Direction(A, Center);
	beta	= Get_Direction(B, Center);

	if( alpha - beta >= M_PI_180 )
	{
		beta	+= M_PI_360;
	}

	Add_Arc(Center, alpha, beta);
}

//---------------------------------------------------------
inline bool CShapes_Buffer::Get_Parallel(const TSG_Point &A, const TSG_Point &B, TSG_Point AB[2])
{
	double		d;
	TSG_Point	C;

	if( (d = SG_Get_Distance(A, B)) > 0.0 )
	{
		d		= m_Distance / d;

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

//---------------------------------------------------------
void CShapes_Buffer::Del_Duplicates(CSG_Shape *pShape)
{
	int			iPart, iPoint;
	CSG_Point	A, B;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		B	= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);

		for(iPoint=pShape->Get_Point_Count(iPart)-2; iPoint>=0; iPoint--)
		{
			A	= pShape->Get_Point(iPoint, iPart);

			if( A == B )
			{
				pShape->Del_Point(iPoint, iPart);
			}
			else
			{
				B	= A;
			}
		}

		if( pShape->Get_Type() == SHAPE_TYPE_Polygon )
		{
			iPoint	= pShape->Get_Point_Count(iPart) - 1;

			A	= pShape->Get_Point(iPoint, iPart);
			B	= pShape->Get_Point(0     , iPart);

			if( A == B )
			{
				pShape->Del_Point(iPoint, iPart);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Buffer::Add_Line(CSG_Shape_Line *pShape, int iPart)
{
	int			iPoint, n	= pShape->Get_Point_Count(iPart);
	double		a;
	TSG_Point	A, B, C, AB[2], BC[2];

	//-----------------------------------------------------
	B	= pShape->Get_Point(0, iPart);
	A	= pShape->Get_Point(1, iPart);

	Get_Parallel(A, B, AB);

	for(iPoint=2; iPoint<n; iPoint++)
	{
		B	= A;
		A	= pShape->Get_Point(iPoint, iPart);

		BC[0]	= AB[0];
		BC[1]	= AB[1];

		Get_Parallel(A, B, AB);

		if( SG_Get_Crossing(C, AB[0], AB[1], BC[0], BC[1]) )
			m_pSegment->Add_Point(C);
		else
			Add_Arc(B, BC[0], AB[1]);
	}

	a	= Get_Direction(A, B);
	Add_Arc(A, a - M_PI_090, a + M_PI_090);

	//-----------------------------------------------------
	B	= pShape->Get_Point(n - 1, iPart);
	A	= pShape->Get_Point(n - 2, iPart);

	Get_Parallel(A, B, AB);

	for(iPoint=n-3; iPoint>=0; iPoint--)
	{
		B	= A;
		A	= pShape->Get_Point(iPoint, iPart);

		BC[0]	= AB[0];
		BC[1]	= AB[1];

		Get_Parallel(A, B, AB);

		if( SG_Get_Crossing(C, AB[0], AB[1], BC[0], BC[1]) )
			m_pSegment->Add_Point(C);
		else
			Add_Arc(B, BC[0], AB[1]);
	}

	a	= Get_Direction(A, B);
	Add_Arc(A, a - M_PI_090, a + M_PI_090);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Buffer::Add_Polygon(CSG_Shape_Polygon *pShape, int iPart)
{
	int			iPoint, n	= pShape->Get_Point_Count(iPart);
	TSG_Point	A, B, C, AB[2], BC[2];

	//-----------------------------------------------------
	bool	bClockwise	= pShape->is_Lake(iPart) ? pShape->is_Clockwise(iPart) : !pShape->is_Clockwise(iPart);

	//-----------------------------------------------------
	if( bClockwise )
	{
		B	= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);
		A	= pShape->Get_Point(0, iPart);

		Get_Parallel(A, B, AB);

		for(iPoint=1; iPoint<=n; iPoint++)
		{
			B	= A;
			A	= pShape->Get_Point(iPoint < n ? iPoint : 0, iPart);

			BC[0]	= AB[0];
			BC[1]	= AB[1];

			Get_Parallel(A, B, AB);

			if( SG_Get_Crossing(C, AB[0], AB[1], BC[0], BC[1]) )
				m_pSegment->Add_Point(C);
			else
				Add_Arc(B, BC[0], AB[1]);
		}
	}

	//-----------------------------------------------------
	else
	{
		B	= pShape->Get_Point(0    , iPart);
		A	= pShape->Get_Point(n - 1, iPart);

		Get_Parallel(A, B, AB);

		for(iPoint=n-2; iPoint>=-1; iPoint--)
		{
			B	= A;
			A	= pShape->Get_Point(iPoint >= 0 ? iPoint : n - 1, iPart);

			BC[0]	= AB[0];
			BC[1]	= AB[1];

			Get_Parallel(A, B, AB);

			if( SG_Get_Crossing(C, AB[0], AB[1], BC[0], BC[1]) )
				m_pSegment->Add_Point(C);
			else
				Add_Arc(B, BC[0], AB[1]);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Buffer::Add_Buffer(bool bLake)
{
	Get_SelfIntersection();

	if( m_pUnion->is_Valid() )
	{
		if( m_pBuffer->Get_Part_Count() )
		{
			if( bLake )
			{
				for(int iPart=m_pUnion->Get_Part_Count()-1; iPart>=0; iPart--)
				{
					if( ((CSG_Shape_Polygon *)m_pUnion)->is_Clockwise(iPart) == false )
					{
						m_pUnion->Del_Part(iPart);
					}
				}

				if( m_pUnion->Get_Part_Count() > 0 )
				{
					GPC_Difference(m_pBuffer, m_pUnion);
				}
			}
			else
			{
				GPC_Union(m_pBuffer, m_pUnion);
			}
		}
		else
		{
			m_pBuffer->Assign(m_pUnion, false);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Buffer::Get_SelfIntersection(void)
{
	int		iPart, iMax;
	double	dMax;

	m_pUnion->Del_Parts();

	Get_SelfIntersection((CSG_Shape_Polygon *)m_pSegment);

	for(iPart=0, iMax=-1; iPart<m_pUnion->Get_Part_Count(); iPart++)
	{
		if( ((CSG_Shape_Polygon *)m_pUnion)->is_Clockwise(iPart) == false )
		{
			if( iMax < 0 || dMax < ((CSG_Shape_Polygon *)m_pUnion)->Get_Area(iPart) )
			{
				iMax	= iPart;
				dMax	= ((CSG_Shape_Polygon *)m_pUnion)->Get_Area(iPart);
			}
		}
	}

	for(iPart=m_pUnion->Get_Part_Count()-1; iPart>=0; iPart--)
	{
		if(  ((CSG_Shape_Polygon *)m_pUnion)->Get_Area(iPart) == 0.0
		||	(((CSG_Shape_Polygon *)m_pUnion)->is_Clockwise(iPart) == false && iPart != iMax) )
		{
			m_pUnion->Del_Part(iPart);
		}
	}
}

//---------------------------------------------------------
void CShapes_Buffer::Get_SelfIntersection(CSG_Shape_Polygon *pSegment)
{
	if( pSegment->Get_Point_Count(0) >= 3 )
	{
		int			i, j, k;
		TSG_Point	A[2], B[2], C;
		CSG_Shape	*pPolygon;
		CSG_Shapes	Points;

		A[0]	= pSegment->Get_Point(0);

		for(i=1; i<pSegment->Get_Point_Count(0)-2; i++)
		{
			A[1]	= A[0];
			A[0]	= pSegment->Get_Point(i);

			B[0]	= pSegment->Get_Point(i + 1);

			for(j=i+2; j<pSegment->Get_Point_Count(0); j++)
			{
				B[1]	= B[0];
				B[0]	= pSegment->Get_Point(j);

				//-----------------------------------------
				if( SG_Get_Crossing(C, A[0], A[1], B[0], B[1]) )
				{
					Points.Create(SHAPE_TYPE_Polygon);
					pPolygon	= Points.Add_Shape();
					pPolygon->Add_Point(C);

					for(k=i; k<j; k++)
						pPolygon->Add_Point(pSegment->Get_Point(k));

					Get_SelfIntersection((CSG_Shape_Polygon *)pPolygon);

					//-------------------------------------
					Points.Create(SHAPE_TYPE_Polygon);
					pPolygon	= Points.Add_Shape();
					pPolygon->Add_Point(C);

					for(k=j; k<pSegment->Get_Point_Count(0); k++)
						pPolygon->Add_Point(pSegment->Get_Point(k));

					for(k=0; k<i; k++)
						pPolygon->Add_Point(pSegment->Get_Point(k));

					Get_SelfIntersection((CSG_Shape_Polygon *)pPolygon);

					return;
				}
			}
		}

		//-------------------------------------------------
		int		iPart	= m_pUnion->Get_Part_Count();

		for(i=0; i<pSegment->Get_Point_Count(0); i++)
		{
			m_pUnion->Add_Point(pSegment->Get_Point(i), iPart);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
