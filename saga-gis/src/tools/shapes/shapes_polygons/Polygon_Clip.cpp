/**********************************************************
 * Version $Id: Polygon_Clip.cpp 1230 2011-11-22 11:12:10Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Polygon_Clip.cpp                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#include "Polygon_Clip.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Clip::CPolygon_Clip(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Clipping"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Clipping of vector layers with a polygon layer.\n"
		"Uses the free and open source software library <b>Clipper</b> created by Angus Johnson.\n"
		"<a target=\"_blank\" href=\"http://www.angusj.com/delphi/clipper.php\">Clipper Homepage</a>\n"
		"<a target=\"_blank\" href=\"http://sourceforge.net/projects/polyclipping/\">Clipper at SourceForge</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "CLIP"		, _TL("Clip Features"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "S_INPUT"		, _TL("Input Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "S_OUTPUT"	, _TL("Output Features"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes_List(
		NULL	, "M_INPUT"		, _TL("Input Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes_List(
		NULL	, "M_OUTPUT"	, _TL("Output Features"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DISSOLVE"	, _TL("Dissolve Clip Features"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "MULTIPLE"	, _TL("Multiple Input Features"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Clip::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier(SG_T("MULTIPLE")) )
	{
		pParameters->Get_Parameter("S_INPUT" )->Set_Enabled(pParameter->asBool() == false);
		pParameters->Get_Parameter("S_OUTPUT")->Set_Enabled(pParameter->asBool() == false);

		pParameters->Get_Parameter("M_INPUT" )->Set_Enabled(pParameter->asBool() == true);
		pParameters->Get_Parameter("M_OUTPUT")->Set_Enabled(pParameter->asBool() == true);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Clip::On_Execute(void)
{
	CSG_Shapes	Clip, *pClip	= Parameters("CLIP")->asShapes();

	if( Parameters("DISSOLVE")->asBool() && Dissolve(pClip, &Clip) )
	{
		pClip	= &Clip;
	}

	//-----------------------------------------------------
	if( !Parameters("MULTIPLE")->asBool() )	// single input mode
	{
		return( Clip_Shapes(pClip,
			Parameters("S_INPUT" )->asShapes(),
			Parameters("S_OUTPUT")->asShapes()
		));
	}

	//-----------------------------------------------------
	else									// multiple input mode
	{
		CSG_Parameter_Shapes_List	*pInput		= Parameters("M_INPUT" )->asShapesList();
		CSG_Parameter_Shapes_List	*pOutput	= Parameters("M_OUTPUT")->asShapesList();

		pOutput->Del_Items();

		for(int i=0; i<pInput->Get_Item_Count() && Process_Get_Okay(); i++)
		{
			CSG_Shapes	*pShapes	= SG_Create_Shapes();

			if( Clip_Shapes(pClip, pInput->Get_Shapes(i), pShapes) )
			{
				pOutput->Add_Item(pShapes);
			}
			else
			{
				delete(pShapes);
			}
		}

		return( pOutput->Get_Item_Count() > 0 );
	}
}

//---------------------------------------------------------
bool CPolygon_Clip::Clip_Shapes(CSG_Shapes *pClip, CSG_Shapes *pInput, CSG_Shapes *pOutput)
{
	pOutput->Create(pInput->Get_Type(), CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("clipped")), pInput);

	switch( pInput->Get_Type() )
	{
	case SHAPE_TYPE_Point:		Clip_Points  (pClip, pInput, pOutput);	break;
	case SHAPE_TYPE_Points:		Clip_Points  (pClip, pInput, pOutput);	break;
	case SHAPE_TYPE_Line:		Clip_Lines   (pClip, pInput, pOutput);	break;
	case SHAPE_TYPE_Polygon:	Clip_Polygons(pClip, pInput, pOutput);	break;
	}

	return( pOutput->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPolygon_Clip::Clip_Points(CSG_Shapes *pClips, CSG_Shapes *pInputs, CSG_Shapes *pOutputs)
{
	for(int iClip=0; iClip<pClips->Get_Count() && Set_Progress(iClip, pClips->Get_Count()); iClip++)
	{
		CSG_Shape_Polygon	*pClip	= (CSG_Shape_Polygon *)pClips->Get_Shape(iClip);

		for(int iInput=0; iInput<pInputs->Get_Count(); iInput++)
		{
			CSG_Shape	*pInput		= pInputs->Get_Shape(iInput);
			CSG_Shape	*pOutput	= NULL;

			for(int iPoint=0; iPoint<pInput->Get_Point_Count(0); iPoint++)
			{
				if( pClip->Contains(pInput->Get_Point(iPoint, 0)) )
				{
					if( pOutput == NULL )
					{
						pOutput	= pOutputs->Add_Shape(pInput, SHAPE_COPY_ATTR);
					}

					pOutput->Add_Point(pInput->Get_Point(iPoint, 0));
				}
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
void CPolygon_Clip::Clip_Lines(CSG_Shapes *pClips, CSG_Shapes *pInputs, CSG_Shapes *pOutputs)
{
	for(int iClip=0; iClip<pClips->Get_Count() && Set_Progress(iClip, pClips->Get_Count()); iClip++)
	{
		CSG_Shape_Polygon	*pClip	= (CSG_Shape_Polygon *)pClips->Get_Shape(iClip);

		if( pInputs->Select(pClip->Get_Extent()) )
		{
			for(int iInput=0; iInput<pInputs->Get_Selection_Count(); iInput++)
			{
				CSG_Shape	*pNew_Line, *pLine	= pInputs->Get_Selection(iInput);

				for(int iPart=0, jPart=-1; iPart<pLine->Get_Part_Count(); iPart++)
				{
					TSG_Point	B, A	= pLine->Get_Point(0, iPart);
					bool		bIn		= pClip->Contains(A);

					if( bIn )
					{
						pNew_Line	= pOutputs->Add_Shape(pLine, SHAPE_COPY_ATTR);

						pNew_Line->Add_Point(A, ++jPart);
					}

					for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
					{
						B	= A;
						A	= pLine->Get_Point(iPoint, iPart);

						if( bIn )
						{
							if( pClip->Contains(A) )
							{
								pNew_Line->Add_Point(A, jPart);
							}
							else
							{
								pNew_Line->Add_Point(Get_Crossing(pClip, A, B), jPart);

								bIn	= false;
							}
						}
						else if( pClip->Contains(A) )
						{
							if( jPart < 0 )
							{
								pNew_Line	= pOutputs->Add_Shape(pLine, SHAPE_COPY_ATTR);
							}

							pNew_Line->Add_Point(Get_Crossing(pClip, A, B), ++jPart);
							pNew_Line->Add_Point(A, jPart);

							bIn	= true;
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
TSG_Point CPolygon_Clip::Get_Crossing(CSG_Shape_Polygon *pPolygon, const TSG_Point &a, const TSG_Point &b)
{
	TSG_Point	c	= a;

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point	A, B;

		B	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			A	= B;
			B	= pPolygon->Get_Point(iPoint, iPart);

			if( SG_Get_Crossing(c, A, B, a, b) )
			{
				return( c );
			}
		}
	}

	return( c );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPolygon_Clip::Clip_Polygons(CSG_Shapes *pClips, CSG_Shapes *pInputs, CSG_Shapes *pOutputs)
{
	for(int iClip=0; iClip<pClips->Get_Count() && Process_Get_Okay(); iClip++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %d/%d"), _TL("clip features"), iClip + 1, pClips->Get_Count()));

		CSG_Shape	*pClip	= pClips->Get_Shape(iClip);

		for(int iInput=0; iInput<pInputs->Get_Count() && Set_Progress(iInput, pInputs->Get_Count()); iInput++)
		{
			CSG_Shape	*pOutput	= pOutputs->Add_Shape(pInputs->Get_Shape(iInput));

			if( !SG_Polygon_Intersection(pOutput, pClip) )
			{
				pOutputs->Del_Shape(pOutputs->Get_Count() - 1);
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
bool CPolygon_Clip::Dissolve(CSG_Shapes *pPolygons, CSG_Shapes *pOutput)
{
	pOutput->Create(SHAPE_TYPE_Polygon);
	pOutput->Add_Field(_TL("ID"), SG_DATATYPE_Int);

	CSG_Shape	*pDissolved	= pOutput->Add_Shape(pPolygons->Get_Shape(0), SHAPE_COPY_GEOM);

	for(int iPolygon=1; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(iPolygon);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			CSG_Shape_Part	*pPart	= ((CSG_Shape_Polygon *)pPolygon)->Get_Part(iPart);

			for(int iPoint=0, nParts=pDissolved->Get_Part_Count(); iPoint<pPart->Get_Count(); iPoint++)
			{
				pDissolved->Add_Point(pPart->Get_Point(iPoint), nParts);
			}
		}
	}

	return( SG_Polygon_Dissolve(pDissolved) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
