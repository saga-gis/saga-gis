
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Polygon_Union.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "Polygon_Union.h"

#include "Polygon_Clipper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Union::CPolygon_Union(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Union"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"The union of polygons, which have the same attribute value."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "UNION"		, _TL("Union"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "ALL"			, _TL("Union of"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("polygons with same attribute value"),
			_TL("all polygons")
		), 0
	);
}

//---------------------------------------------------------
CPolygon_Union::~CPolygon_Union(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Union::On_Execute(void)
{
	bool		bAll;
	int			iField, iPolygon;
	CSG_String	Value;
	CSG_Table	*pTable;
	CSG_Shape	*pPolygon, *pUnion;
	CSG_Shapes	*pPolygons, *pUnions;

	//-----------------------------------------------------
	pPolygons	= Parameters("POLYGONS")	->asShapes();
	pUnions		= Parameters("UNION")		->asShapes();
	iField		= Parameters("ATTRIBUTE")	->asInt();
	bAll		= Parameters("ALL")			->asInt() == 1;

	//-----------------------------------------------------
	if(	pPolygons->is_Valid() && iField >= 0 && iField < pPolygons->Get_Table().Get_Field_Count() )
	{
		pTable	= &pPolygons->Get_Table();

		pUnions	->Create(SHAPE_TYPE_Polygon);
		pUnions	->Get_Table().Add_Field(pTable->Get_Field_Name(iField), pTable->Get_Field_Type(iField));

		//-------------------------------------------------
		if( bAll )
		{
			pUnion	= pUnions->Add_Shape(pPolygons->Get_Shape(0), false);

			for(iPolygon=1; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
			{
				GPC_Union(pUnion, pPolygons->Get_Shape(iPolygon));
			}
		}

		//-------------------------------------------------
		else
		{
			pTable	->Set_Index(iField, TABLE_INDEX_Ascending);

			pUnions	->Set_Name(CSG_String::Format(SG_T("%s [%s]-[%s]"), _TL("Union"), pPolygons->Get_Name(), pTable->Get_Field_Name(iField)));

			for(iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
			{
				pPolygon	= pPolygons->Get_Shape(pTable->Get_Record_byIndex(iPolygon)->Get_Index());

				if( iPolygon == 0 || Value.Cmp(pPolygon->Get_Record()->asString(iField)) )
				{
					Value	= pPolygon->Get_Record()->asString(iField);
					pUnion	= pUnions->Add_Shape(pPolygon, false);
					pUnion	->Get_Record()->Set_Value(0, Value);
				}
				else
				{
					GPC_Union(pUnion, pPolygon);
				}
			}
		}

		//-------------------------------------------------
		return( pUnions->is_Valid() );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
