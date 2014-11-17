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
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  line_smoothing.cpp                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "line_smoothing.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Smoothing::CLine_Smoothing(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Smoothing"));

	Set_Author		(SG_T("V. Wichmann (c) 2014"));

	Set_Description	(_TW(
		"The module performs line smoothing by iterative averaging as described by "
		"Mansouryar & Hedayati (2012). A higher smoothing sensitivity results in a "
		"stronger smoothing in less iterations and vice versa. The 'improved SIA "
		"model' simply applies a preservation factor in the first iteration and "
		"then runs the 'basic SIA model' for the following iterations.\n\n"
		"In case the density of line vertices is too high, the 'Line Simplification' "
		"tool can be applied first. If the density of line vertices is too low, "
		"additional vertices can be inserted by applying the 'Convert Lines to Points' "
		"and the 'Convert Points to Line(s)' tools prior to smoothing.\n\n"
		"Reference:\n"
		"Mansouryar, M. & Hedayati, A. (2012): Smoothing Via Iterative Averaging (SIA) "
		"- A Basic Technique for Line Smoothing. International Journal of Computer and "
		"Electrical Engineering Vol. 4, No. 3: 307-311. "
		"(<a target=\"_blank\" href=\"http://www.ijcee.org/papers/501-P063.pdf\">pdf</a>)\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES_IN"		, _TL("Lines"),
		_TL("The input line shapefile to smooth."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "LINES_OUT"	, _TL("Smoothed Lines"),
		_TL("The smoothed output line shapefile."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Choose the method to apply."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("basic SIA model"),
			_TL("improved SIA model")
		), 1
	);
	
	Parameters.Add_Value(
		NULL	, "SENSITIVITY"	, _TL("Sensitivity"),
		_TL("Half the size of the moving window [vertex count], controls smoothing sensitivity."),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Value(
		NULL	, "ITERATIONS"	, _TL("Iterations"),
		_TL("The number of smoothing iterations [-]."),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	Parameters.Add_Value(
		NULL	, "PRESERVATION"	, _TL("Preservation"),
		_TL("The smoothing preservation factor [-]."),
		PARAMETER_TYPE_Double, 10.0, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Smoothing::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("PRESERVATION", pParameter->asInt() == 1);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Smoothing::On_Execute(void)
{
	CSG_Shapes	*pLines, *pSmooth;
	int			iMethod, iSS, iSI;
	double		dSP;
	int			iOffset;


	pLines		= Parameters("LINES_IN")->asShapes();
	pSmooth		= Parameters("LINES_OUT")->asShapes();
	iMethod		= Parameters("METHOD")->asInt();
	iSS			= Parameters("SENSITIVITY")->asInt();
	iSI			= Parameters("ITERATIONS")->asInt();
	dSP			= Parameters("PRESERVATION")->asDouble();


	//--------------------------------------------------------
	pSmooth->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s_smoothed"), pLines->Get_Name()), pLines, pLines->Get_Vertex_Type());

	if(	iSS > iSI )
	{
		SG_UI_Msg_Add(_TL("WARNING: smoothing sensitivity is greater than the number of iterations!"), true);
	}

	if(	iMethod == 1 && iSI < 2 )
	{
		SG_UI_Msg_Add_Error(_TL("The improved SIA model requires more than one smoothing iteration!"));
		return( false );
	}


	//--------------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape *pLine = pLines->Get_Shape(iLine);
		
		CSG_Shape *pSmoothed = pSmooth->Add_Shape(pLine, SHAPE_COPY_ATTR);


		//--------------------------------------------------------
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			std::vector<TSG_Point>	verticesA(pLine->Get_Point_Count(iPart));
			std::vector<TSG_Point>	verticesB(pLine->Get_Point_Count(iPart));
			TSG_Point				p;

			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				p.x	= pLine->Get_Point(iPoint, iPart).x;
				p.y	= pLine->Get_Point(iPoint, iPart).y;

				verticesB.at(iPoint) = p;
			}


			//--------------------------------------------------------
			for(int i=0; i<iSI; i++)
			{
				verticesA = verticesB;

				for(int iPoint=0; iPoint<(int)verticesA.size(); iPoint++)
				{
					if( iPoint < iSS )
					{
						iOffset = iPoint;
					}
					else if( iSS <= iPoint && iPoint < (int)verticesA.size() - iSS )
					{
						iOffset = iSS;
					}
					else // if( iPoint >= (int)verticesA.size() - iSS )
					{
						iOffset = (int)verticesA.size() - 1 - iPoint;
					}

					if( iPoint - iOffset < 0 )	// validate lower ...
					{
						iOffset = 0;
					}
					if( iPoint + iOffset >= verticesA.size() )	// ... and upper limit
					{
						iOffset = (int)verticesA.size() - 1 - iPoint;
					}


					if( iMethod == 1 && i == 0 )	// improved SIA, apply preservation in first iteration
					{
						p.x = (verticesA.at(iPoint - iOffset).x + verticesA.at(iPoint).x + verticesA.at(iPoint + iOffset).x) / 3.0;
						p.y = (verticesA.at(iPoint - iOffset).y + verticesA.at(iPoint).y + verticesA.at(iPoint + iOffset).y) / 3.0;

						TSG_Point	p_GA;	// vector GA (triangle centroid - point)

						p_GA.x = verticesA.at(iPoint).x - p.x;
						p_GA.y = verticesA.at(iPoint).y - p.y;

						double dLength = sqrt(p_GA.x * p_GA.x + p_GA.y * p_GA.y);

						if( dLength > 0.0 )
						{
							double dScaled = dLength * dSP;	// scale by preservation

							p.x = verticesA.at(iPoint).x + (p_GA.x / dLength) * dScaled;	// shift point
							p.y = verticesA.at(iPoint).y + (p_GA.y / dLength) * dScaled;
						}
						else
						{
							p = verticesA.at(iPoint);
						}
					}
					else	// basic SIA
					{
						p.x = (verticesA.at(iPoint - iOffset).x + verticesA.at(iPoint).x + verticesA.at(iPoint + iOffset).x) / 3.0;
						p.y = (verticesA.at(iPoint - iOffset).y + verticesA.at(iPoint).y + verticesA.at(iPoint + iOffset).y) / 3.0;
					}

					verticesB.at(iPoint) = p;
				} // iPoint
			} // iIteration


			//--------------------------------------------------------
			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				pSmoothed->Add_Point(verticesB.at(iPoint).x, verticesB.at(iPoint).y, iPart);
				
				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pSmoothed->Set_Z(pLine->Get_Z(iPoint, iPart), iPoint, iPart);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pSmoothed->Set_M(pLine->Get_M(iPoint, iPart), iPoint, iPart);
					}
				}
			}	
		} // iPart
	} // iLine


	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
