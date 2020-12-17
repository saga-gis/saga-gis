
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                visualize_gradients.cpp                //
//                                                       //
//                  Olaf Conrad (C) 2020                 //
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
#include "visualize_gradients.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVisualize_Gradients::CVisualize_Gradients(void)
{
	Set_Name		(_TL("Gradient Lines from Points"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"Create lines representing gradients from point data attributes. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"DIRECTION"	, _TL("Direction"),
		_TL("The direction measured in degrees with the North direction as origin and increasing clockwise.")
	);

	Parameters.Add_Table_Field("POINTS",
		"LENGTH"	, _TL("Length"),
		_TL("")
	);

	Parameters.Add_Table_Field("POINTS",
		"X_COMP"	, _TL("X-Component"),
		_TL("")
	);

	Parameters.Add_Table_Field("POINTS",
		"Y_COMP"	, _TL("Y-Component"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"DEFINITION", _TL("Gradient Definition"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("direction and length"),
			_TL("directional components")
		), 0
	);

	Parameters.Add_Double("",
		"SCALING"	, _TL("Length Scaling Factor"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Shapes("",
		"GRADIENTS"	, _TL("Gradient"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"TARGETS"	, _TL("Target Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVisualize_Gradients::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DEFINITION") )
	{
		pParameters->Set_Enabled("DIRECTION", pParameter->asInt() == 0);
		pParameters->Set_Enabled("LENGTH"   , pParameter->asInt() == 0);

		pParameters->Set_Enabled("X_COMP"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("Y_COMP"   , pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVisualize_Gradients::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( !pPoints->is_Valid() || pPoints->Get_Count() < 1 )
	{
		Error_Set(_TL("Invalid input points."));

		return( false );
	}

	int	Definition	= Parameters("DEFINITION")->asInt(), Field[2];

	switch( Definition )
	{
	default: // direction and length
		Field[0] = Parameters("DIRECTION")->asInt();
		Field[1] = Parameters("LENGTH"   )->asInt();
		break;

	case  1: // directional components
		Field[0] = Parameters("X_COMP"   )->asInt();
		Field[1] = Parameters("Y_COMP"   )->asInt();
		break;
	}

	double	Scale	= Parameters("SCALING")->asDouble();

	//-----------------------------------------------------
	CSG_Shapes	*pGradients	= Parameters("GRADIENTS")->asShapes();

	pGradients->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("Gradients")), pPoints);

	CSG_Shapes	*pTargets	= Parameters("TARGETS")->asShapes();

	if( pTargets )
	{
		pTargets->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("Target Points")), pPoints);
	}

	//-----------------------------------------------------
	for(int i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Field[0])
		&&  !pPoint->is_NoData(Field[1]) )
		{
			TSG_Point	Point	= pPoint->Get_Point(0);

			switch( Definition )
			{
			default: // direction and length
				Point.x += Scale * pPoint->asDouble(Field[1]) * sin(M_DEG_TO_RAD * pPoint->asDouble(Field[0]));
				Point.y += Scale * pPoint->asDouble(Field[1]) * cos(M_DEG_TO_RAD * pPoint->asDouble(Field[0]));
				break;

			case  1: // directional components
				Point.x += Scale * pPoint->asDouble(Field[0]);
				Point.y += Scale * pPoint->asDouble(Field[1]);
				break;
			}

			CSG_Shape	*pGradient	= pGradients->Add_Shape(pPoint, SHAPE_COPY_ATTR);
			
			pGradient->Add_Point(pPoint->Get_Point(0)); pGradient->Add_Point(Point);

			if( pTargets )
			{
				pTargets->Add_Shape(pPoint)->Set_Point(Point, 0);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
