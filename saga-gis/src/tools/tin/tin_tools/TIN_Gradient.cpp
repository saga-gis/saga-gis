
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    TIN_Gradient.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "TIN_Gradient.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Gradient::CTIN_Gradient(void)
{
	Set_Name		(_TL("Gradient"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description(
		_TL("Calculates the gradient based on the values of each triangle's points.\n\n")
	);

	//-----------------------------------------------------
	Parameters.Add_TIN("",
		"TIN"		, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TIN",
		"ZFIELD"	, _TL("Z Values"),
		_TL("")
	);

	Parameters.Add_Shapes("",
		"GRADIENT"	, _TL("TIN_Gradient"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"DEGREE"	, _TL("Output Unit"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Radians"),
			_TL("Degree")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_Gradient::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_TIN *pTIN = Parameters("TIN")->asTIN();

	int  zField  = Parameters("ZFIELD")->asInt();
	bool bDegree = Parameters("DEGREE")->asInt() == 1;

	//-----------------------------------------------------
	CSG_Shapes *pShapes = Parameters("GRADIENT")->asShapes();
	pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s], %s [%s]", _TL("TIN_Gradient"), pTIN->Get_Field_Name(zField), _TL("TIN"), pTIN->Get_Name()));
	pShapes->Add_Field("ID"     , SG_DATATYPE_Int   );
	pShapes->Add_Field("AREA"   , SG_DATATYPE_Double);
	pShapes->Add_Field("DECLINE", SG_DATATYPE_Double);
	pShapes->Add_Field("AZIMUTH", SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(sLong iTriangle=0; iTriangle<pTIN->Get_Triangle_Count() && Set_Progress(iTriangle, pTIN->Get_Triangle_Count()); iTriangle++)
	{
		CSG_TIN_Triangle *pTriangle = pTIN->Get_Triangle(iTriangle); double a, b;

		if( pTriangle->Get_Gradient(zField, a, b) )
		{
			if( bDegree )
			{
				a *= M_RAD_TO_DEG;
				b *= M_RAD_TO_DEG;
			}

			CSG_Shape *pShape = pShapes->Add_Shape();

			pShape->Add_Point(pTriangle->Get_Node(0)->Get_Point());
			pShape->Add_Point(pTriangle->Get_Node(1)->Get_Point());
			pShape->Add_Point(pTriangle->Get_Node(2)->Get_Point());

			pShape->Set_Value(0, iTriangle + 1);
			pShape->Set_Value(1, pTriangle->Get_Area());
			pShape->Set_Value(2, a);
			pShape->Set_Value(3, b);
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
