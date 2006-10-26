
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "TIN_Gradient.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Gradient::CTIN_Gradient(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Gradient"));

	Set_Author(_TL("Copyrights (c) 2004 by Olaf Conrad"));

	Set_Description(
		_TL("Calculates the gradient based on the values of each triangle's points.\n\n")
	);


	//-----------------------------------------------------
	pNode	= Parameters.Add_TIN(
		NULL	, "TIN"			, "TIN",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"		, _TL("Z Values"),
		""
	);

	Parameters.Add_Shapes(
		NULL	, "GRADIENT"	, _TL("TIN_Gradient"),
		"",
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "DEGREE"		, _TL("Output Unit"),
		"",
		_TL("Radians|"
		"Degree|"), 1
	);
}

//---------------------------------------------------------
CTIN_Gradient::~CTIN_Gradient(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_Gradient::On_Execute(void)
{
	bool			bDegree;
	int				iTriangle, zField;
	double			a, b;
	CSG_TIN_Triangle	*pTriangle;
	CSG_TIN			*pTIN;
	CSG_Shape			*pShape;
	CSG_Shapes			*pShapes;

	//-----------------------------------------------------
	pTIN		= Parameters("TIN")			->asTIN();
	zField		= Parameters("ZFIELD")		->asInt();
	pShapes		= Parameters("GRADIENT")	->asShapes();
	bDegree		= Parameters("DEGREE")		->asInt() == 1;

	//-----------------------------------------------------
	pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format(_TL("TIN_Gradient [%s], TIN [%s]"), pTIN->Get_Table().Get_Field_Name(zField), pTIN->Get_Name()));

	pShapes->Get_Table().Add_Field(_TL("ID")		, TABLE_FIELDTYPE_Int);
	pShapes->Get_Table().Add_Field(_TL("AREA")	, TABLE_FIELDTYPE_Double);
	pShapes->Get_Table().Add_Field(_TL("DECLINE"), TABLE_FIELDTYPE_Double);
	pShapes->Get_Table().Add_Field(_TL("AZIMUTH"), TABLE_FIELDTYPE_Double);

	//-----------------------------------------------------
	for(iTriangle=0; iTriangle<pTIN->Get_Triangle_Count() && Set_Progress(iTriangle, pTIN->Get_Triangle_Count()); iTriangle++)
	{
		pTriangle	= pTIN->Get_Triangle(iTriangle);

		if( pTriangle->Get_Gradient(zField, a, b) )
		{
			if( bDegree )
			{
				a	*= M_RAD_TO_DEG;
				b	*= M_RAD_TO_DEG;
			}

			pShape		= pShapes->Add_Shape();
			pShape->Add_Point(pTriangle->Get_Point(0)->Get_Point());
			pShape->Add_Point(pTriangle->Get_Point(1)->Get_Point());
			pShape->Add_Point(pTriangle->Get_Point(2)->Get_Point());

			pShape->Get_Record()->Set_Value(0, iTriangle + 1);
			pShape->Get_Record()->Set_Value(1, pTriangle->Get_Area());
			pShape->Get_Record()->Set_Value(2, a);
			pShape->Get_Record()->Set_Value(3, b);
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
