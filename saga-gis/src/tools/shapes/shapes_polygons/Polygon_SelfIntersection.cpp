/**********************************************************
 * Version $Id: Polygon_SelfIntersection.cpp 1230 2011-11-22 11:12:10Z oconrad $
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Self-Intersection"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Self-intersection of one layer's polygons.\n"
		"Uses the free and open source software library <b>Clipper</b> created by Angus Johnson.\n"
		"<a target=\"_blank\" href=\"http://www.angusj.com/delphi/clipper.php\">Clipper Homepage</a>\n"
		"<a target=\"_blank\" href=\"http://sourceforge.net/projects/polyclipping/\">Clipper at SourceForge</a>\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "ID"			, _TL("Identifier"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes(
		NULL	, "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_SelfIntersection::On_Execute(void)
{
	int			ID;
	CSG_Shapes	*pPolygons, Intersect;

	pPolygons		= Parameters("POLYGONS" )->asShapes();
	m_pIntersect	= Parameters("INTERSECT")->asShapes() ? Parameters("INTERSECT")->asShapes() : &Intersect;
	ID				= Parameters("ID")->asInt();	if( ID >= pPolygons->Get_Field_Count() )	{	ID	= -1;	}

	m_pIntersect->Create(SHAPE_TYPE_Polygon, pPolygons->Get_Name(), pPolygons);
	m_pIntersect->Add_Field("ID", SG_DATATYPE_String);

	//-----------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		Add_Polygon(pPolygons->Get_Shape(i), ID);
	}

	//-----------------------------------------------------
	if( m_pIntersect->Get_Count() != pPolygons->Get_Count() )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("number of added polygons"), m_pIntersect->Get_Count() - pPolygons->Get_Count()));

		if( m_pIntersect == &Intersect )
		{
			pPolygons->Create(Intersect);
			DataObject_Update(pPolygons);
		}
		else
		{
			m_pIntersect->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPolygons->Get_Name(), _TL("self-intersection")));
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPolygon_SelfIntersection::Add_Polygon(CSG_Shape *pPolygon, int ID)
{
	CSG_String	sID;

	if( ID >= 0 )
	{	sID	= pPolygon->asString(ID);	}
	else
	{	sID	= SG_Get_String(pPolygon->Get_Index() + 1);	}

	ID	= m_pIntersect->Get_Field_Count() - 1;

	//-----------------------------------------------------
	if( !m_pIntersect->Select(pPolygon->Get_Extent()) )
	{
		pPolygon	= m_pIntersect->Add_Shape(pPolygon);
		pPolygon	->Set_Value(ID, sID);

		return;
	}

	//-----------------------------------------------------
	CSG_Shapes	Intersect(m_pIntersect->Get_Type(), SG_T(""), m_pIntersect);

	int	nIntersects	= m_pIntersect->Get_Count();

	pPolygon	= m_pIntersect->Add_Shape(pPolygon);
	pPolygon	->Set_Value(ID, sID);

	for(int i=0; i<nIntersects && pPolygon->is_Valid(); i++)
	{
		if( pPolygon != m_pIntersect->Get_Shape(i) && pPolygon->Intersects(m_pIntersect->Get_Shape(i)) )
		{
			CSG_Shape	*pOriginal	= Intersect.Add_Shape(m_pIntersect->Get_Shape(i));
			CSG_Shape	*pIntersect	= Intersect.Add_Shape();

			if( SG_Polygon_Intersection(pOriginal, pPolygon, pIntersect) )
			{
				pIntersect	= m_pIntersect->Add_Shape(pIntersect);
				pIntersect	->Set_Value(ID, CSG_String::Format(SG_T("%s|%s"), pPolygon->asString(ID), pOriginal->asString(ID)));

				SG_Polygon_Difference(m_pIntersect->Get_Shape(i), pPolygon);
				SG_Polygon_Difference(pPolygon, pOriginal);
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
