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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pc_from_shapes.cpp                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "pc_from_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_From_Shapes::CPC_From_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud from Shapes"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"		, _TL("Z Value"),
		_TL(""),
		true
	);

	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("only z"),
			_TL("all attributes")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_From_Shapes::On_Execute(void)
{
	int				zField, iField, nFields, *Fields;
	CSG_PointCloud	*pPoints;
	CSG_Shapes		*pShapes;

	pShapes	= Parameters("SHAPES")	->asShapes();
	pPoints	= Parameters("POINTS")	->asPointCloud();
	zField	= Parameters("ZFIELD")	->asInt();

	if( !pShapes->is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	pPoints->Create();
	pPoints->Set_Name(pShapes->Get_Name());

	nFields	= 0;
	Fields	= new int[pShapes->Get_Field_Count()];

	if( Parameters("OUTPUT")->asInt() == 1 )
	{
		for(iField=0, nFields=0; iField<pShapes->Get_Field_Count(); iField++)
		{
			if( iField != zField && SG_Data_Type_Get_Size(pShapes->Get_Field_Type(iField)) > 0 )
			{
				Fields[nFields++]	= iField;
				pPoints->Add_Field(pShapes->Get_Field_Name(iField), pShapes->Get_Field_Type(iField));
			}
		}
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

				pPoints->Add_Point(p.x, p.y, zField < 0 ? pShape->Get_Z(iPoint, iPart) : pShape->asDouble(zField));

				for(iField=0; iField<nFields; iField++)
				{
					pPoints->Set_Value(3 + iField, pShape->asDouble(Fields[iField]));
				}
			}
		}
	}

	//-----------------------------------------------------
	delete [] Fields;

	return( pPoints->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
