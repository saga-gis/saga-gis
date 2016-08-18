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
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                line_simplification.cpp                //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "line_simplification.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CStack : public CSG_Stack
{
public:
	CStack(void) : CSG_Stack(2 * sizeof(int))	{}

	bool		Push	(int iAnchor, int iFloater)
	{
		int		*Record	= (int *)Get_Record_Push();

		if( Record )
		{
			Record[0]	= iAnchor;
			Record[1]	= iFloater;

			return( true );
		}

		return( false );
	}

	bool		Pop		(int &iAnchor, int &iFloater)
	{
		int		*Record	= (int *)Get_Record_Pop();

		if( Record )
		{
			iAnchor		= Record[0];
			iFloater	= Record[1];

			return( true );
		}

		return( false );
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Simplification::CLine_Simplification(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Simplification"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"Line simplification implementing the Ramer-Douglas-Peucker algorithm.\n"
		"\n"
		"Refererences:\n"
		"- Ramer, U. (1972): An iterative procedure for the polygonal approximation of plane curves. Computer Graphics and Image Processing, 1(3), 244-256\n"
		"- Douglas, D., Peucker, T. (1973): Algorithms for the reduction of the number of points required to represent a digitized line or its caricature. The Canadian Cartographer 10(2), 112-122\n"
		"\n"
		"- Polyline Reduction source code at <a target=\"_blank\" href=\"http://mappinghacks.com/code/PolyLineReduction/\">mappinghacks.com</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL("Line or polygon shapefile to simplify."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Simplified Lines"),
		_TL("If not set points will be removed from the input data set."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "TOLERANCE"	, _TL("Tolerance"),
		_TL("Maximum deviation allowed between original and simplified curve [map units]."),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Simplification::On_Execute(void)
{
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid or empty shapes layer"));

		return( false );
	}

	if(	pLines->Get_Type() != SHAPE_TYPE_Line && pLines->Get_Type() != SHAPE_TYPE_Polygon )
	{
		Error_Set(_TL("line simplification can only be applied to lines and polygons"));

		return( false );
	}

	if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pLines )
	{
		pLines	= Parameters("OUTPUT")->asShapes();
		pLines->Create(*Parameters("LINES")->asShapes());
	}

	//-----------------------------------------------------
	CSG_Array	Keep(sizeof(bool));

	m_Tolerance		= Parameters("TOLERANCE")->asDouble();

	int	nTotal		= 0;
	int	nRemoved	= 0;

	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape	*pLine	= pLines->Get_Shape(iLine);

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			nTotal	+= pLine->Get_Point_Count(iPart);

			Keep.Set_Array(pLine->Get_Point_Count(iPart), false);

			if( Simplify(pLine, iPart, (bool *)Keep.Get_Array()) )
			{
				for(int iPoint=pLine->Get_Point_Count(iPart)-1; iPoint>=0; iPoint--)
				{
					if( !((bool *)Keep.Get_Array())[iPoint] )
					{
						pLine->Del_Point(iPoint, iPart);

						nRemoved	++;
					}
				}
			}
		}
	}

	Message_Add(CSG_String::Format(SG_T("\n%s: %0.2f%% (%d / %d)"), _TL("Reduction"), 100.0 * nRemoved / (double)nTotal, nRemoved, nTotal), false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Simplification::Simplify(CSG_Shape *pLine, int iPart, bool *Keep)
{
	int			iVertexMax, iVertex, iAnchor, iFloater;
	double		dVertexMax, dVertex, dLength, dProjScalar;
	TSG_Point	Anchor, Floater, Vertex, vAnchor, vVertex;
	CStack		Stack;
 
	memset(Keep, 0, sizeof(bool) * pLine->Get_Point_Count(iPart));

	Stack.Push(0, pLine->Get_Point_Count(iPart) - 1);

	while( Stack.Pop(iAnchor, iFloater) && Process_Get_Okay() )
	{
		Anchor		= pLine->Get_Point(iAnchor , iPart);
		Floater		= pLine->Get_Point(iFloater, iPart);

		while( Anchor.x == Floater.x && Anchor.y == Floater.y )
		{
			if( iAnchor >= iFloater - 1 )
			{
				return( false );
			}

			Keep[iFloater--]	= iAnchor == 0 && iFloater == pLine->Get_Point_Count(iPart) - 1;
			Floater				= pLine->Get_Point(iFloater, iPart);
		}

		vAnchor.x	= Floater.x - Anchor.x;
		vAnchor.y	= Floater.y - Anchor.y;
		dLength		= SG_Get_Length(vAnchor.x, vAnchor.y);
		vAnchor.x	= vAnchor.x / dLength;
		vAnchor.y	= vAnchor.y / dLength;

		for(iVertex=iVertexMax=iAnchor+1, dVertexMax=0.0; iVertex<iFloater; iVertex++)
		{
			Vertex		= pLine->Get_Point(iVertex, iPart);

			vVertex.x	= Vertex.x - Anchor.x;
			vVertex.y	= Vertex.y - Anchor.y;
			dLength		= SG_Get_Length(Vertex.x, Vertex.y);
			dProjScalar	= vVertex.x * vAnchor.x + vVertex.y * vAnchor.y;	//dot product:

			if( dProjScalar < 0.0 )
			{
				dVertex = dLength;
			}
			else
			{
				vVertex.x		= Vertex.x - Floater.x;
				vVertex.y		= Vertex.y - Floater.y;
				dLength			= SG_Get_Length(vVertex.x, vVertex.y);
				dProjScalar		= vVertex.x * (-vAnchor.x) + vVertex.y * (-vAnchor.y);	//dot product:

				if( dProjScalar < 0.0 )
				{
					dVertex		= dLength;
				}
				else //calculate perpendicular distance to line (pythagorean theorem):
				{
					dVertex		= sqrt(fabs(dLength*dLength - dProjScalar*dProjScalar));
				}
			}

			if( dVertexMax < dVertex )
			{
				dVertexMax		= dVertex;
				iVertexMax		= iVertex;
			}
		}

		if( dVertexMax <= m_Tolerance )
		{
			Keep[iAnchor]	= true;
			Keep[iFloater]	= true;
		}
		else
		{
			Stack.Push(iAnchor   , iVertexMax);
			Stack.Push(iVertexMax, iFloater  );
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
