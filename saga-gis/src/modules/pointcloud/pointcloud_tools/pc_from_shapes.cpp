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
	int				zField, nFields, *Fields;
	CSG_PointCloud	*pPoints;
	CSG_Shapes		*pShapes;

	pShapes	= Parameters("SHAPES")->asShapes();
	pPoints	= Parameters("POINTS")->asPointCloud();
	zField	= Parameters("ZFIELD")->asInt();

	if( !pShapes->is_Valid() )
	{
		Error_Set(_TL("invalid input"));

		return( false );
	}

	//-----------------------------------------------------
	pPoints->Create();
	pPoints->Set_Name(pShapes->Get_Name());

	nFields	= 0;
	Fields	= new int[pShapes->Get_Field_Count()];

	if( Parameters("OUTPUT")->asInt() == 1 )	// all attributes
	{
		for(int iField=0; iField<pShapes->Get_Field_Count(); iField++)
		{
			if( iField != zField && pPoints->Add_Field(pShapes->Get_Field_Name(iField), pShapes->Get_Field_Type(iField)) )
			{
				Fields[nFields++]	= iField;
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

				for(int iField=0, jField=3; iField<nFields; iField++, jField++)
				{
					switch( pPoints->Get_Field_Type(jField) )
					{
					case SG_DATATYPE_Date:
					case SG_DATATYPE_String:
						pPoints->Set_Value(jField, pShape->asString(Fields[iField]));
						break;

					default:
						pPoints->Set_Value(jField, pShape->asDouble(Fields[iField]));
						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	delete[](Fields);

	return( pPoints->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
