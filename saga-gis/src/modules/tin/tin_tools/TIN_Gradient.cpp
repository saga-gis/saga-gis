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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description(
		_TL("Calculates the gradient based on the values of each triangle's points.\n\n")
	);


	//-----------------------------------------------------
	pNode	= Parameters.Add_TIN(
		NULL	, "TIN"			, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"		, _TL("Z Values"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "GRADIENT"	, _TL("TIN_Gradient"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "DEGREE"		, _TL("Output Unit"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Radians"),
			_TL("Degree")
		), 1
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
	pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s], %s [%s]"), _TL("TIN_Gradient"), pTIN->Get_Field_Name(zField), _TL("TIN"), pTIN->Get_Name()));

	pShapes->Add_Field(_TL("ID")		, SG_DATATYPE_Int);
	pShapes->Add_Field(_TL("AREA")	, SG_DATATYPE_Double);
	pShapes->Add_Field(_TL("DECLINE"), SG_DATATYPE_Double);
	pShapes->Add_Field(_TL("AZIMUTH"), SG_DATATYPE_Double);

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
