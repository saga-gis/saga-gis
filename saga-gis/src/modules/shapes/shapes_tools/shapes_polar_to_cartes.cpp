/**********************************************************
 * Version $Id: shapes_polar_to_cartes.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               shapes_polar_to_cartes.cpp              //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "shapes_polar_to_cartes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Polar_to_Cartes::CShapes_Polar_to_Cartes(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Polar to Cartesian Coordinates"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Shapes(
		NULL	, "POLAR"			, _TL("Polar Coordinates"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Table_Field(
		pNode	, "F_EXAGG"			, _TL("Exaggeration"),
		_TL(""),
		true
	);

	Parameters.Add_Value(
		pNode	, "D_EXAGG"			, _TL("Exaggeration Factor"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Shapes(
		NULL	, "CARTES"			, _TL("Cartesion Coordinates"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"			, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Double, 6371000.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DEGREE"			, _TL("Degree"),
		_TL("polar coordinates given in degree"),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Polar_to_Cartes::On_Execute(void)
{
	bool		bDegree;
	int			fExagg;
	double		Radius, dExagg, Exagg	= 0.0;
	CSG_Shapes	*pPolar, *pCartes;

	//-----------------------------------------------------
	pPolar	= Parameters("POLAR")	->asShapes();
	pCartes	= Parameters("CARTES")	->asShapes();
	Radius	= Parameters("RADIUS")	->asDouble();
	bDegree	= Parameters("DEGREE")	->asBool();
	fExagg	= Parameters("F_EXAGG")	->asInt();
	dExagg	= Parameters("D_EXAGG")	->asDouble();

	//-----------------------------------------------------
	if( !pPolar->is_Valid() )
	{
		Error_Set(_TL("invalid input"));

		return( false );
	}

	//-----------------------------------------------------
	pCartes->Create(pPolar->Get_Type(), CSG_String::Format(SG_T("%s [%s]"), pPolar->Get_Name(), _TL("cartesian")), pPolar, SG_VERTEX_TYPE_XYZ);

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPolar->Get_Count() && Set_Progress(iShape, pPolar->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape		= pPolar->Get_Shape(iShape);
		CSG_Shape	*pTarget	= pCartes->Add_Shape(pShape, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point_Z	Point	= Get_Cartes(pShape->Get_Point(iPoint, iPart), fExagg < 0 ? Radius : Radius + dExagg * pShape->asDouble(fExagg), bDegree);

				pTarget->Add_Point(Point.x, Point.y, iPart);
				pTarget->Set_Z    (Point.z, iPoint , iPart);
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
inline TSG_Point_Z CShapes_Polar_to_Cartes::Get_Cartes(TSG_Point Point, double Radius, bool bDegree)
{
	if( bDegree )
	{
		Point.x	*= M_DEG_TO_RAD;
		Point.y	*= M_DEG_TO_RAD;
	}

	TSG_Point_Z	p;

	p.z		= Radius * sin(Point.y);
	Radius	= Radius * cos(Point.y);
	p.x		= Radius * cos(Point.x);
	p.y		= Radius * sin(Point.x);

	return( p );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
