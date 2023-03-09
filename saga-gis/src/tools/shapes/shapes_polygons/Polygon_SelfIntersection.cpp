
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
//              Polygon_SelfIntersection.cpp             //
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
#include "Polygon_SelfIntersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_SelfIntersection::CPolygon_SelfIntersection(void)
{
	Set_Name		(_TL("Polygon Self-Intersection"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"This tool identifies self-intersection in polygons. "
		"The Intersecting areas are added as new polygons to the dataset, "
		"leaving the input areas with the geometric difference. "
		"The new polygons are labeled with the identifier of the intersecting polygons separated by a \'|\' character. " 
		"The identifier can be set with the \"Identifier\"-field option, otherwise the identifier is just the polygon index.\n"
		"Uses the free and open source software library <b>Clipper</b> created by Angus Johnson.\n"
		"<a target=\"_blank\" href=\"http://www.angusj.com/delphi/clipper.php\">Clipper Homepage</a>\n"
		"<a target=\"_blank\" href=\"http://sourceforge.net/projects/polyclipping/\">Clipper at SourceForge</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("POLYGONS",
		"ID"		, _TL("Identifier"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes("",
		"INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_SelfIntersection::On_Execute(void)
{
	CSG_Shapes Intersect, *pPolygons = Parameters("POLYGONS" )->asShapes();

	m_pIntersect = Parameters("INTERSECT")->asShapes() ? Parameters("INTERSECT")->asShapes() : &Intersect;
	m_pIntersect->Create(SHAPE_TYPE_Polygon, pPolygons->Get_Name(), pPolygons);
	m_pIntersect->Add_Field("ID", SG_DATATYPE_String);

	//-----------------------------------------------------
	int ID = Parameters("ID")->asInt(); if( ID >= pPolygons->Get_Field_Count() ) { ID = -1; }
	int ID_Field = m_pIntersect->Get_Field_Count()-1;

	if( ID >= 0 )
	{
		m_pIntersect->Set_Field_Name( ID_Field, CSG_String::Format("%s Intersection" , m_pIntersect->Get_Field_Name(ID)));
	}

	for(sLong i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		Add_Polygon(pPolygons->Get_Shape(i)->asPolygon(), ID, ID_Field );
	}

	//-----------------------------------------------------
	if( m_pIntersect->Get_Count() != pPolygons->Get_Count() )
	{
		Message_Fmt("\n%s: %lld", _TL("number of added polygons"), m_pIntersect->Get_Count() - pPolygons->Get_Count());

		if( m_pIntersect == &Intersect )
		{
			pPolygons->Create(Intersect);
			DataObject_Update(pPolygons);
		}
		else
		{
			Message_Fmt("\n%s [%s]", pPolygons->Get_Name(), _TL("self-intersection"));
		}
	}
	else
	{
		Message_Add(_TL("no self-intersecting polygons detected"));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPolygon_SelfIntersection::Add_Polygon(CSG_Shape_Polygon *pPolygon, int ID, int ID_Field)
{
	CSG_String	sID;

	if( ID >= 0 )
	{	sID	= pPolygon->asString(ID);	}
	else
	{	sID	= SG_Get_String((int)pPolygon->Get_Index() + 1);	}


	//-----------------------------------------------------
	if( !m_pIntersect->Select(pPolygon->Get_Extent()) )
	{
		pPolygon	= m_pIntersect->Add_Shape(pPolygon)->asPolygon();
		pPolygon	->Set_Value(ID_Field, sID);

		return;
	}

	//-----------------------------------------------------
	CSG_Shapes	Intersect(m_pIntersect->Get_Type(), SG_T(""), m_pIntersect);

	int	nIntersects	= m_pIntersect->Get_Count();

	pPolygon	= m_pIntersect->Add_Shape(pPolygon)->asPolygon();
	pPolygon	->Set_Value(ID_Field, sID);

	for(int i=0; i<nIntersects && pPolygon->is_Valid(); i++)
	{
		if( pPolygon != m_pIntersect->Get_Shape(i) && pPolygon->Intersects(m_pIntersect->Get_Shape(i)) )
		{
			CSG_Shape_Polygon *pOriginal  = Intersect.Add_Shape(m_pIntersect->Get_Shape(i))->asPolygon();
			CSG_Shape_Polygon *pIntersect = Intersect.Add_Shape()->asPolygon();

			if( SG_Shape_Get_Intersection(pOriginal, pPolygon, pIntersect) )
			{
				pIntersect = m_pIntersect->Add_Shape(pIntersect)->asPolygon();
				pIntersect->Set_Value(ID_Field, CSG_String::Format("%s|%s", pPolygon->asString(ID_Field), pOriginal->asString(ID_Field)));

				SG_Shape_Get_Difference(m_pIntersect->Get_Shape(i), pPolygon);
				SG_Shape_Get_Difference(pPolygon, pOriginal);
			}

			Intersect.Del_Shapes();
		}
	}

	m_pIntersect->Select();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
